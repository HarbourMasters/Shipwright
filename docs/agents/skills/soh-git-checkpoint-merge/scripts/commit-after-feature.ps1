[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$PlanId = "",
    [string]$Scope = "",
    [string]$Summary = "feature checkpoint",
    [string]$Applied = "",
    [string]$Validation = "feature checkpoint committed",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $false

function Invoke-Git {
    param(
        [string]$Root,
        [string[]]$GitArgs,
        [switch]$IgnoreError
    )
    $escapedRoot = $Root.Replace('"', '\"')
    $escapedArgs = @()
    foreach ($gitArg in $GitArgs) {
        $escapedArgs += ('"{0}"' -f ($gitArg -replace '"', '\"'))
    }
    $commandLine = 'git -C "{0}" {1} 2>nul' -f $escapedRoot, ($escapedArgs -join ' ')
    $output = cmd /c $commandLine
    $code = $LASTEXITCODE
    if (-not $IgnoreError -and $code -ne 0) {
        throw "git $($GitArgs -join ' ') failed with exit code $code"
    }
    return [PSCustomObject]@{
        Code = $code
        Output = @($output)
    }
}

function Get-GitOutput {
    param([object]$Result)
    if ($null -eq $Result -or $null -eq $Result.PSObject.Properties["Output"]) {
        return @()
    }
    $values = New-Object System.Collections.Generic.List[string]
    foreach ($line in @($Result.Output)) {
        if ($null -ne $line -and -not [string]::IsNullOrWhiteSpace("$line")) {
            $values.Add("$line")
        }
    }
    return $values.ToArray()
}

function Get-ChangedPaths {
    param([string]$Root)
    $statusResult = Invoke-Git -Root $Root -GitArgs @("status", "--porcelain")
    $lines = @(Get-GitOutput -Result $statusResult)
    $paths = New-Object System.Collections.Generic.List[string]
    foreach ($line in $lines) {
        if ([string]::IsNullOrWhiteSpace($line) -or $line.Length -lt 4) {
            continue
        }
        $pathPart = $line.Substring(3).Trim()
        if ($pathPart -match " -> ") {
            $parts = $pathPart -split " -> "
            $pathPart = $parts[$parts.Count - 1]
        }
        $paths.Add($pathPart)
    }
    return @($paths)
}

function Sanitize-Scope {
    param([string]$RawScope)
    $candidate = $RawScope.ToLowerInvariant()
    $candidate = [regex]::Replace($candidate, "[^a-z0-9_-]", "-")
    $candidate = [regex]::Replace($candidate, "-{2,}", "-")
    $candidate = $candidate.Trim("-")
    if ([string]::IsNullOrWhiteSpace($candidate)) {
        return "repo"
    }
    return $candidate
}

function Resolve-Scope {
    param(
        [string]$Root,
        [string]$RequestedScope
    )
    if (-not [string]::IsNullOrWhiteSpace($RequestedScope)) {
        return Sanitize-Scope -RawScope $RequestedScope
    }

    $paths = Get-ChangedPaths -Root $Root
    if ($paths.Count -eq 0) {
        return "repo"
    }

    $first = $paths[0].Replace("\", "/")
    if ($first.Contains("/")) {
        return Sanitize-Scope -RawScope ($first.Split("/")[0])
    }
    return "repo"
}

function Resolve-PlanId {
    param(
        [string]$Root,
        [string]$RequestedPlanId
    )
    if (-not [string]::IsNullOrWhiteSpace($RequestedPlanId)) {
        return $RequestedPlanId
    }

    $queryScript = Join-Path $Root "tools\agents\query-plan-ledger.ps1"
    if (-not (Test-Path $queryScript)) {
        return "PLN-UNSET"
    }

    $json = & $queryScript -RepoRoot $Root -Limit 500
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($json)) {
        return "PLN-UNSET"
    }

    $entries = $json | ConvertFrom-Json
    $list = @($entries)
    if ($list.Count -eq 0) {
        return "PLN-UNSET"
    }

    $statusByPlan = @{}
    $planOrder = New-Object System.Collections.Generic.List[string]
    foreach ($entry in $list) {
        if ($null -eq $entry.planId -or [string]::IsNullOrWhiteSpace($entry.planId)) {
            continue
        }
        if ($entry.status) {
            $statusByPlan[$entry.planId] = "$($entry.status)".Trim()
        }
        if (-not $entry.isUpdate) {
            $planOrder.Add($entry.planId)
        }
    }

    for ($i = $planOrder.Count - 1; $i -ge 0; $i--) {
        $candidate = $planOrder[$i]
        $status = ""
        if ($statusByPlan.ContainsKey($candidate)) {
            $status = $statusByPlan[$candidate].ToLowerInvariant()
        }
        if ($status -ne "done" -and $status -ne "canceled" -and $status -ne "deprecated") {
            return $candidate
        }
    }

    return "PLN-UNSET"
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$inside = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--is-inside-work-tree") -IgnoreError
if ($inside.Code -ne 0) {
    Write-Error "Not a git repository: $resolvedRoot"
    exit 1
}

$statusResult = Invoke-Git -Root $resolvedRoot -GitArgs @("status", "--porcelain")
$statusEntries = @(Get-GitOutput -Result $statusResult)
if ($statusEntries.Count -eq 0) {
    Write-Host "skip: clean tree"
    exit 0
}

$resolvedPlanId = Resolve-PlanId -Root $resolvedRoot -RequestedPlanId $PlanId
$resolvedScope = Resolve-Scope -Root $resolvedRoot -RequestedScope $Scope
$oneLineSummary = [regex]::Replace($Summary.Trim(), "\s+", " ")
if ([string]::IsNullOrWhiteSpace($oneLineSummary)) {
    $oneLineSummary = "feature checkpoint"
}

$title = "feat($resolvedScope): $oneLineSummary"
if ($title.Length -gt 72) {
    $title = $title.Substring(0, 69) + "..."
}

$appliedText = if ([string]::IsNullOrWhiteSpace($Applied)) { $oneLineSummary } else { $Applied.Trim() }
$validationText = if ([string]::IsNullOrWhiteSpace($Validation)) { "feature checkpoint committed" } else { $Validation.Trim() }
$body = "Plan: $resolvedPlanId`nApplied: $appliedText`nValidation: $validationText"

if ($DryRun) {
    Write-Host "[DryRun] Would stage and commit feature checkpoint"
    Write-Host "[DryRun] Title: $title"
    Write-Host "[DryRun] Body:`n$body"
    exit 0
}

[void](Invoke-Git -Root $resolvedRoot -GitArgs @("add", "-A"))
$cachedDiff = Invoke-Git -Root $resolvedRoot -GitArgs @("diff", "--cached", "--quiet") -IgnoreError
if ($cachedDiff.Code -eq 0) {
    Write-Host "skip: nothing staged after add -A"
    exit 0
}

$commit = Invoke-Git -Root $resolvedRoot -GitArgs @("commit", "-m", $title, "-m", $body) -IgnoreError
if ($commit.Code -ne 0) {
    Write-Error "git commit failed"
    exit 1
}

$hash = (Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--short", "HEAD")).Output[0].Trim()
Write-Host "committed feature checkpoint: $hash"
exit 0





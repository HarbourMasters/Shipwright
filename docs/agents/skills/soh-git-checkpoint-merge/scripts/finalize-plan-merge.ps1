[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$PlanId = "",
    [string]$TargetBranch = "",
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

function Test-GitRef {
    param(
        [string]$Root,
        [string]$Ref
    )
    $result = Invoke-Git -Root $Root -GitArgs @("show-ref", "--verify", "--quiet", $Ref) -IgnoreError
    return $result.Code -eq 0
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

    $entries = @($json | ConvertFrom-Json)
    if ($entries.Count -eq 0) {
        return "PLN-UNSET"
    }

    $statusByPlan = @{}
    $planOrder = New-Object System.Collections.Generic.List[string]
    foreach ($entry in $entries) {
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

function Resolve-TargetBranch {
    param(
        [string]$Root,
        [string]$RequestedTarget
    )
    if (-not [string]::IsNullOrWhiteSpace($RequestedTarget)) {
        return $RequestedTarget
    }

    if (Test-GitRef -Root $Root -Ref "refs/heads/develop" -or Test-GitRef -Root $Root -Ref "refs/remotes/origin/develop") {
        return "develop"
    }

    $originHead = Invoke-Git -Root $Root -GitArgs @("symbolic-ref", "refs/remotes/origin/HEAD") -IgnoreError
    if ($originHead.Code -eq 0 -and $originHead.Output.Count -gt 0) {
        $originHeadRef = $originHead.Output[0].Trim()
        if ($originHeadRef -match "^refs/remotes/origin/(.+)$") {
            return $Matches[1]
        }
    }

    throw "Could not resolve target branch (no explicit target, no develop, no origin/HEAD)."
}

function Ensure-LocalBranch {
    param(
        [string]$Root,
        [string]$BranchName,
        [switch]$DryRun
    )

    if (Test-GitRef -Root $Root -Ref "refs/heads/$BranchName") {
        return $BranchName
    }

    if (Test-GitRef -Root $Root -Ref "refs/remotes/origin/$BranchName") {
        if ($DryRun) {
            return $BranchName
        }
        [void](Invoke-Git -Root $Root -GitArgs @("branch", $BranchName, "origin/$BranchName"))
        return $BranchName
    }

    throw "Target branch '$BranchName' not found locally or on origin."
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$inside = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--is-inside-work-tree") -IgnoreError
if ($inside.Code -ne 0) {
    Write-Error "Not a git repository: $resolvedRoot"
    exit 1
}

$currentBranch = (Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--abbrev-ref", "HEAD")).Output[0].Trim()
if ($currentBranch -eq "HEAD") {
    Write-Error "Cannot finalize plan merge from detached HEAD."
    exit 1
}

$statusResult = Invoke-Git -Root $resolvedRoot -GitArgs @("status", "--porcelain")
$statusEntries = @(Get-GitOutput -Result $statusResult)
if ($statusEntries.Count -gt 0) {
    Write-Error "Working tree must be clean before finalize merge."
    exit 1
}

$conflictsResult = Invoke-Git -Root $resolvedRoot -GitArgs @("diff", "--name-only", "--diff-filter=U")
$conflicts = @(Get-GitOutput -Result $conflictsResult)
if ($conflicts.Count -gt 0) {
    Write-Error "Unresolved conflicts found. Resolve before finalize merge."
    exit 1
}

$resolvedPlanId = Resolve-PlanId -Root $resolvedRoot -RequestedPlanId $PlanId
$targetCandidate = Resolve-TargetBranch -Root $resolvedRoot -RequestedTarget $TargetBranch
$targetLocal = Ensure-LocalBranch -Root $resolvedRoot -BranchName $targetCandidate -DryRun:$DryRun

if ($targetLocal -eq $currentBranch) {
    Write-Error "Source and target branches are the same: $currentBranch"
    exit 1
}

$auditScript = Join-Path $PSScriptRoot "git-audit.ps1"
if (-not (Test-Path $auditScript)) {
    Write-Error "Missing audit script: $auditScript"
    exit 1
}
$auditJson = & $auditScript -RepoRoot $resolvedRoot -BaseBranch $targetLocal
if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($auditJson)) {
    Write-Error "git-audit failed before merge."
    exit 1
}
$audit = $auditJson | ConvertFrom-Json

$commitCountResult = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-list", "--count", "$targetLocal..$currentBranch") -IgnoreError
$commitsToMerge = 0
if ($commitCountResult.Code -eq 0 -and $commitCountResult.Output.Count -gt 0) {
    [void][int]::TryParse($commitCountResult.Output[0].Trim(), [ref]$commitsToMerge)
}
if ($commitsToMerge -le 0) {
    Write-Error "No local commits to merge from '$currentBranch' into '$targetLocal'."
    exit 1
}

if ($DryRun) {
    Write-Host "[DryRun] finalize-plan-merge"
    Write-Host "  source: $currentBranch"
    Write-Host "  target: $targetLocal"
    Write-Host "  plan:   $resolvedPlanId"
    Write-Host "  commitsToMerge: $commitsToMerge"
    Write-Host "  upstream: $($audit.upstream) ahead=$($audit.ahead) behind=$($audit.behind)"
    exit 0
}

[void](Invoke-Git -Root $resolvedRoot -GitArgs @("checkout", $targetLocal))
$mergeTitle = "merge(plan): finalize $resolvedPlanId"
$mergeResult = Invoke-Git -Root $resolvedRoot -GitArgs @("merge", "--no-ff", $currentBranch, "-m", $mergeTitle) -IgnoreError
if ($mergeResult.Code -ne 0) {
    Write-Error "Merge failed. Resolve conflicts manually and rerun if needed."
    exit 1
}

$mergeHash = (Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--short", "HEAD")).Output[0].Trim()
$mergedCommitsResult = Invoke-Git -Root $resolvedRoot -GitArgs @("log", "--oneline", "--max-count", "20", "$targetLocal@{1}..$targetLocal")
$mergedCommits = @(Get-GitOutput -Result $mergedCommitsResult)

$appendPlanScript = Join-Path $resolvedRoot "tools\agents\append-plan-ledger.ps1"
if ($resolvedPlanId -ne "PLN-UNSET" -and (Test-Path $appendPlanScript)) {
    & $appendPlanScript -RepoRoot $resolvedRoot -Mode update -PlanId $resolvedPlanId -Status done `
        -Note "Local merge completed: '$currentBranch' -> '$targetLocal' at $mergeHash." `
        -Refs @("branch:$currentBranch", "branch:$targetLocal", "commit:$mergeHash")
}

$appendMemoryScript = Join-Path $resolvedRoot "tools\agents\append-memory.ps1"
if (Test-Path $appendMemoryScript) {
    $summary = "Plan merge complete: $resolvedPlanId $currentBranch->$targetLocal ($mergeHash)"
    & $appendMemoryScript -Kind note -Topic "merge:$resolvedPlanId" -Summary $summary `
        -Details "Local plan merge completed by soh-git-checkpoint-merge." -Status done -Scope tooling `
        -Tags @("git", "merge", "plans") -Refs @("branch:$currentBranch", "branch:$targetLocal", "commit:$mergeHash")
}

Write-Host "merge complete: $mergeHash"
$mergedCommits | ForEach-Object { Write-Host $_ }
exit 0





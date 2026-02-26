[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$BaseBranch = "",
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

function Resolve-BaseRef {
    param(
        [string]$Root,
        [string]$Requested
    )

    if (-not [string]::IsNullOrWhiteSpace($Requested)) {
        if (Test-GitRef -Root $Root -Ref "refs/heads/$Requested") {
            return $Requested
        }
        if (Test-GitRef -Root $Root -Ref "refs/remotes/origin/$Requested") {
            return "origin/$Requested"
        }
        throw "Base branch '$Requested' does not exist locally or on origin."
    }

    if (Test-GitRef -Root $Root -Ref "refs/heads/develop") {
        return "develop"
    }

    $originHead = Invoke-Git -Root $Root -GitArgs @("symbolic-ref", "refs/remotes/origin/HEAD") -IgnoreError
    if ($originHead.Code -eq 0 -and $originHead.Output.Count -gt 0) {
        $originHeadRef = $originHead.Output[0].Trim()
        if ($originHeadRef -match "^refs/remotes/origin/(.+)$") {
            $defaultName = $Matches[1]
            if (Test-GitRef -Root $Root -Ref "refs/heads/$defaultName") {
                return $defaultName
            }
            if (Test-GitRef -Root $Root -Ref "refs/remotes/origin/$defaultName") {
                return "origin/$defaultName"
            }
        }
    }

    return "HEAD"
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$inside = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--is-inside-work-tree") -IgnoreError
if ($inside.Code -ne 0) {
    Write-Error "Not a git repository: $resolvedRoot"
    exit 1
}

$currentBranchResult = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--abbrev-ref", "HEAD")
$currentBranch = $currentBranchResult.Output[0].Trim()
$isDetached = $currentBranch -eq "HEAD"

$baseRef = Resolve-BaseRef -Root $resolvedRoot -Requested $BaseBranch
$statusResult = Invoke-Git -Root $resolvedRoot -GitArgs @("status", "--porcelain")
$statusLines = @(Get-GitOutput -Result $statusResult)
$conflictsResult = Invoke-Git -Root $resolvedRoot -GitArgs @("diff", "--name-only", "--diff-filter=U")
$conflicts = @(Get-GitOutput -Result $conflictsResult)

$commitsSinceBase = 0
if ($baseRef -ne "HEAD") {
    $commitCount = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-list", "--count", "$baseRef..HEAD") -IgnoreError
    if ($commitCount.Code -eq 0 -and $commitCount.Output.Count -gt 0) {
        $parsed = 0
        if ([int]::TryParse($commitCount.Output[0].Trim(), [ref]$parsed)) {
            $commitsSinceBase = $parsed
        }
    }
}

$upstream = ""
$ahead = 0
$behind = 0
$upstreamResult = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{u}") -IgnoreError
if ($upstreamResult.Code -eq 0 -and $upstreamResult.Output.Count -gt 0) {
    $upstream = $upstreamResult.Output[0].Trim()
    $leftRight = Invoke-Git -Root $resolvedRoot -GitArgs @("rev-list", "--left-right", "--count", "$upstream...HEAD") -IgnoreError
    if ($leftRight.Code -eq 0 -and $leftRight.Output.Count -gt 0) {
        $parts = $leftRight.Output[0].Trim() -split "\s+"
        if ($parts.Count -ge 2) {
            [void][int]::TryParse($parts[0], [ref]$behind)
            [void][int]::TryParse($parts[1], [ref]$ahead)
        }
    }
}

$recentCommitsResult = Invoke-Git -Root $resolvedRoot -GitArgs @("log", "--oneline", "-n", "20")
$recentCommits = @(Get-GitOutput -Result $recentCommitsResult)

$result = [ordered]@{
    repoRoot = $resolvedRoot
    currentBranch = $currentBranch
    isDetachedHead = $isDetached
    baseRef = $baseRef
    isClean = $statusLines.Count -eq 0
    hasConflicts = $conflicts.Count -gt 0
    statusEntries = $statusLines
    conflictEntries = $conflicts
    commitsSinceBase = $commitsSinceBase
    upstream = $upstream
    ahead = $ahead
    behind = $behind
    recentCommits = $recentCommits
}

if ($DryRun) {
    Write-Host "[DryRun] git-audit completed for $resolvedRoot"
}

$result | ConvertTo-Json -Depth 8
exit 0





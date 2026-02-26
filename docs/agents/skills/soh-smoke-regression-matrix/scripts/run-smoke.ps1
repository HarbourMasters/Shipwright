[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$checks = @()

function Add-Check {
    param([string]$Name, [bool]$Ok, [string]$Detail)
    $script:checks += [PSCustomObject]@{
        name = $Name
        ok = $Ok
        detail = $Detail
    }
}

$examples = Join-Path $RepoRoot "docs\examples\external_mods"
$runtimeMods = Join-Path $RepoRoot "x64\Release\mods"
$logPath = Join-Path $RepoRoot "x64\Release\logs\Ship of Harkinian.log"

Add-Check -Name "examples_dir" -Ok (Test-Path $examples) -Detail $examples
Add-Check -Name "runtime_mods_dir" -Ok (Test-Path $runtimeMods) -Detail $runtimeMods

if (Test-Path $examples) {
    $mods = Get-ChildItem -Path $examples -Directory
    Add-Check -Name "examples_count" -Ok ($mods.Count -gt 0) -Detail "count=$($mods.Count)"
}

if (Test-Path $logPath) {
    $critical = @(Select-String -Path $logPath -Pattern "\[critical\]|Runtime disabled for|Missing or invalid field" -CaseSensitive:$false)
    Add-Check -Name "log_blocker_scan" -Ok ($critical.Count -eq 0) -Detail "hits=$($critical.Count)"
}
else {
    Add-Check -Name "log_blocker_scan" -Ok $true -Detail "log not found (skipped)"
}

if ($DryRun) {
    Write-Host "[DryRun] Smoke checks prepared: $($checks.Count)"
    $checks | ConvertTo-Json -Depth 4
    exit 0
}

$checks | ConvertTo-Json -Depth 4
if (($checks | Where-Object { -not $_.ok }).Count -gt 0) {
    exit 1
}
exit 0


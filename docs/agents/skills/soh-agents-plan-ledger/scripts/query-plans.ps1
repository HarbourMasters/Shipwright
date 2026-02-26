[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$PlanId = "",
    [int]$Limit = 20,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$tool = Join-Path $RepoRoot "tools\agents\query-plan-ledger.ps1"
if (-not (Test-Path $tool)) {
    Write-Error "Missing tool script: $tool"
    exit 1
}

& $tool -RepoRoot $RepoRoot -PlanId $PlanId -Limit $Limit -DryRun:$DryRun
exit $LASTEXITCODE

[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [ValidateSet("new", "update")]
    [string]$Mode = "new",
    [string]$PlanId = "",
    [string]$Title = "",
    [string]$Summary = "",
    [ValidateSet("planned", "in_progress", "blocked", "done", "canceled")]
    [string]$Status = "planned",
    [ValidateSet("engine", "docs", "mod", "tooling", "mixed")]
    [string]$Scope = "tooling",
    [string[]]$Milestones = @(),
    [string]$Note = "",
    [string[]]$Tags = @(),
    [string[]]$Refs = @(),
    [switch]$WriteMemory,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$tool = Join-Path $RepoRoot "tools\agents\append-plan-ledger.ps1"
if (-not (Test-Path $tool)) {
    Write-Error "Missing tool script: $tool"
    exit 1
}

& $tool `
    -RepoRoot $RepoRoot `
    -Mode $Mode `
    -PlanId $PlanId `
    -Title $Title `
    -Summary $Summary `
    -Status $Status `
    -Scope $Scope `
    -Milestones $Milestones `
    -Note $Note `
    -Tags $Tags `
    -Refs $Refs `
    -WriteMemory:$WriteMemory `
    -DryRun:$DryRun

exit $LASTEXITCODE

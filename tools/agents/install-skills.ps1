[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$GlobalSkillsDir = "C:\Users\leolo\.codex\skills",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$sync = Join-Path $PSScriptRoot "sync-skills.ps1"
$validate = Join-Path $PSScriptRoot "validate-skills.ps1"

if (-not (Test-Path $sync) -or -not (Test-Path $validate)) {
    Write-Error "Required scripts missing in tools/agents."
    exit 1
}

& $sync -RepoRoot $RepoRoot -GlobalSkillsDir $GlobalSkillsDir -DryRun:$DryRun
if ($LASTEXITCODE -ne 0) {
    exit 1
}

& $validate -RepoRoot $RepoRoot -DryRun:$DryRun
if ($LASTEXITCODE -ne 0) {
    exit 1
}

Write-Host "Skill installation flow completed."
exit 0


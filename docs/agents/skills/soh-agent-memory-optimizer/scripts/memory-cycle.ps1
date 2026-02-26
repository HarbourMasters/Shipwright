[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$tools = Join-Path $RepoRoot "tools\agents"
$logPath = Join-Path $RepoRoot "docs\agents\memory.log"
if (-not (Test-Path $logPath)) {
    Write-Error "memory.log not found: $logPath"
    exit 1
}

$entries = (Get-Content -Path $logPath -Encoding UTF8 | Where-Object { -not [string]::IsNullOrWhiteSpace($_) }).Count
$shouldCompact = $entries -gt 200

if ($DryRun) {
    Write-Host "[DryRun] entries=$entries shouldCompact=$shouldCompact"
    exit 0
}

if ($shouldCompact) {
    & (Join-Path $tools "compact-memory.ps1")
    if ($LASTEXITCODE -ne 0) { exit 1 }
}

& (Join-Path $tools "rebuild-index.ps1")
if ($LASTEXITCODE -ne 0) { exit 1 }
& (Join-Path $tools "validate-memory.ps1")
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Memory cycle finished. entries=$entries compacted=$shouldCompact"
exit 0


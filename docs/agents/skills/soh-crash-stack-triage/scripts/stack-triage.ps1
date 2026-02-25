[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$LogPath = "",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($LogPath)) {
    $candidates = @(
        (Join-Path $RepoRoot "x64\Release\logs\Ship of Harkinian.log"),
        (Join-Path $RepoRoot "logs\Ship of Harkinian.log"),
        (Join-Path $RepoRoot "Ship of Harkinian.log")
    )
    $LogPath = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
    if ([string]::IsNullOrWhiteSpace($LogPath)) {
        $LogPath = $candidates[0]
    }
}

if (-not (Test-Path $LogPath)) {
    Write-Error "Log file not found: $LogPath"
    exit 1
}

$lines = Get-Content -Path $LogPath -Encoding UTF8
$criticalIndex = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -match "\[critical\] Exception:") {
        $criticalIndex = $i
        break
    }
}

if ($criticalIndex -lt 0) {
    Write-Host "No crash section found."
    exit 0
}

$slice = $lines[$criticalIndex..([Math]::Min($criticalIndex + 120, $lines.Count - 1))]
$frames = $slice | Where-Object { $_ -match "^\s+[A-Za-z0-9_:]+" } | Select-Object -First 20
$modFrames = $frames | Where-Object { $_ -match "ExternalMod|external-mods|Fast::|ResourceLoader" } | Select-Object -First 10

$result = [ordered]@{
    logPath = $LogPath
    exceptionLine = $lines[$criticalIndex]
    topFrames = @($frames)
    externalModRelatedFrames = @($modFrames)
}

if ($DryRun) {
    Write-Host "[DryRun] Crash triage generated."
}

$result | ConvertTo-Json -Depth 6
exit 0


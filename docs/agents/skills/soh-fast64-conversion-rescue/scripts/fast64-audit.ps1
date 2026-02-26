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

$patterns = @(
    "unsupported macros while converting Fast64 model",
    "failed to finalize .o2r archive",
    "Converting Fast64 source",
    "Converted Fast64 model",
    "Runtime disabled for .*modelAsset conversion"
)

$matches = @(foreach ($line in Get-Content -Path $LogPath -Encoding UTF8) {
    foreach ($pattern in $patterns) {
        if ($line -match $pattern) {
            [PSCustomObject]@{
                Pattern = $pattern
                Line = [string]$line
            }
            break
        }
    }
})

if ($DryRun) {
    Write-Host "[DryRun] Parsed $($matches.Count) Fast64-related log lines."
}

$summary = [ordered]@{
    total = $matches.Count
    blockerCount = @($matches | Where-Object { $_.Pattern -match "unsupported|failed|Runtime disabled" }).Count
    lines = $matches
}

$summary | ConvertTo-Json -Depth 6
exit 0


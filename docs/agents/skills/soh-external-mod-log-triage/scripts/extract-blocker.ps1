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
    "\[critical\]",
    "Runtime disabled for",
    "Missing or invalid field",
    "Unsupported apiVersion",
    "Unknown schemaVersion",
    "failed to finalize .o2r archive",
    "unsupported macros while converting Fast64 model",
    "Exception:"
)

$lines = Get-Content -Path $LogPath -Encoding UTF8
$first = $null
for ($i = 0; $i -lt $lines.Count; $i++) {
    $line = $lines[$i]
    foreach ($pattern in $patterns) {
        if ($line -match $pattern) {
            $first = [ordered]@{
                lineNumber = $i + 1
                pattern = $pattern
                line = [string]$line
            }
            break
        }
    }
    if ($null -ne $first) {
        break
    }
}

if ($null -eq $first) {
    Write-Host "No primary blocker pattern found."
    exit 0
}

if ($DryRun) {
    Write-Host "[DryRun] Primary blocker candidate:"
}

$first | ConvertTo-Json -Depth 4
exit 0


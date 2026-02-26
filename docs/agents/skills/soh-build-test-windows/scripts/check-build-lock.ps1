[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$running = Get-Process -Name "soh" -ErrorAction SilentlyContinue
if ($DryRun) {
    Write-Host "[DryRun] soh process running: $($null -ne $running)"
    exit 0
}

if ($null -ne $running) {
    Write-Error "soh.exe is running. Close the game before building."
    exit 1
}

$exe = Join-Path $RepoRoot "x64\Release\soh.exe"
if (Test-Path $exe) {
    try {
        $stream = [System.IO.File]::Open($exe, 'Open', 'ReadWrite', 'None')
        $stream.Close()
    }
    catch {
        Write-Error "Output binary appears locked: $exe"
        exit 1
    }
}

Write-Host "Build lock check passed."
exit 0


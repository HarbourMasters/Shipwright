[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$Config = "Release",
    [string]$Target = "soh",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$checkScript = Join-Path $PSScriptRoot "check-build-lock.ps1"
if (Test-Path $checkScript) {
    & $checkScript -RepoRoot $RepoRoot
    if ($LASTEXITCODE -ne 0) {
        exit 1
    }
}

$cmd = "cmake --build build/x64 --config $Config --target $Target -- /m"
if ($DryRun) {
    Write-Host "[DryRun] $cmd"
    exit 0
}

Push-Location $RepoRoot
try {
    Write-Host $cmd
    iex $cmd
    if ($LASTEXITCODE -ne 0) {
        exit 1
    }
    Write-Host "Build succeeded."
    exit 0
}
finally {
    Pop-Location
}


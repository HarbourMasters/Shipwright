[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$src = Join-Path $RepoRoot "docs\examples\external_mods"
$dst = Join-Path $RepoRoot "x64\Release\mods"

if (-not (Test-Path $src)) {
    Write-Error "Source demos not found: $src"
    exit 1
}
if (-not (Test-Path $dst)) {
    if (-not $DryRun) {
        New-Item -ItemType Directory -Force -Path $dst | Out-Null
    }
}

$demos = Get-ChildItem -Path $src -Directory
foreach ($demo in $demos) {
    $destDir = Join-Path $dst $demo.Name
    if ($DryRun) {
        Write-Host "[DryRun] Sync $($demo.FullName) -> $destDir"
        continue
    }
    if (Test-Path $destDir) {
        Remove-Item -Path $destDir -Recurse -Force
    }
    Copy-Item -Path $demo.FullName -Destination $destDir -Recurse -Force
}

Write-Host "Synced $($demos.Count) demos."
exit 0


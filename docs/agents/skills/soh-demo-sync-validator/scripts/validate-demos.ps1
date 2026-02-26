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
    Write-Error "Missing source directory: $src"
    exit 1
}
if (-not (Test-Path $dst)) {
    Write-Error "Missing destination directory: $dst"
    exit 1
}

$issues = @()
$demos = Get-ChildItem -Path $src -Directory

foreach ($demo in $demos) {
    $dstDemo = Join-Path $dst $demo.Name
    if (-not (Test-Path $dstDemo)) {
        $issues += "Missing runtime demo: $($demo.Name)"
        continue
    }

    $modJsonPath = Join-Path $dstDemo "mod.json"
    if (-not (Test-Path $modJsonPath)) {
        $issues += "Missing mod.json in runtime demo: $($demo.Name)"
        continue
    }

    try {
        $modJson = Get-Content -Raw -Path $modJsonPath -Encoding UTF8 | ConvertFrom-Json
        if ($modJson.apiVersion -ne 3) {
            $issues += "apiVersion != 3 in $($demo.Name)"
        }
    }
    catch {
        $issues += "Invalid JSON in $modJsonPath"
    }
}

if ($DryRun) {
    Write-Host "[DryRun] Validation issues count: $($issues.Count)"
    if ($issues.Count -gt 0) {
        $issues | ForEach-Object { Write-Host "[DryRun] $_" }
    }
    exit 0
}

if ($issues.Count -gt 0) {
    $issues | ForEach-Object { Write-Error $_ }
    exit 1
}

Write-Host "Demo validation passed."
exit 0


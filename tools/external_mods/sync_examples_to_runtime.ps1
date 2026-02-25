param(
    [string]$SourceRoot = "docs/examples/external_mods",
    [string]$RuntimeRoot = "x64/Release/mods",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$demoNames = @(
    "fire_staff_demo",
    "freeze_staff_demo",
    "status_catalog_demo",
    "pistol_hitscan_demo",
    "spear_lunge_demo",
    "shield_skate_demo",
    "axe_prop_cut_demo",
    "freeze_dome_staff_demo"
)

if (-not (Test-Path $SourceRoot)) {
    throw "Source root not found: $SourceRoot"
}

New-Item -ItemType Directory -Force -Path $RuntimeRoot | Out-Null

foreach ($demo in $demoNames) {
    $src = Join-Path $SourceRoot $demo
    $dst = Join-Path $RuntimeRoot $demo

    if (-not (Test-Path $src)) {
        Write-Warning "Skipping missing demo: $demo"
        continue
    }

    if ($Clean -and (Test-Path $dst)) {
        Remove-Item -Recurse -Force $dst
    }

    New-Item -ItemType Directory -Force -Path $dst | Out-Null
    Copy-Item -Recurse -Force (Join-Path $src '*') $dst
    Write-Host "Synced $demo"
}

Write-Host "External mod demos synchronized."

[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$DemoId = "scene_interact_demo_v3",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$releaseRoot = $resolvedRoot
if (-not ((Test-Path (Join-Path $releaseRoot "mods")) -and (Test-Path (Join-Path $releaseRoot "logs")))) {
    $releaseRoot = Join-Path $resolvedRoot "x64\Release"
}

$demoDir = Join-Path $releaseRoot "mods\$DemoId"
if ($DryRun) {
    Write-Host "[DryRun] Would create demo at $demoDir"
    exit 0
}

$paths = @(
    $demoDir,
    (Join-Path $demoDir "scripts"),
    (Join-Path $demoDir "config"),
    (Join-Path $demoDir "items"),
    (Join-Path $demoDir "scenes"),
    (Join-Path $demoDir "actors"),
    (Join-Path $demoDir "behaviors"),
    (Join-Path $demoDir "assets")
)
foreach ($p in $paths) {
    New-Item -ItemType Directory -Force -Path $p | Out-Null
}

$modJson = @'
{
  "apiVersion": 3,
  "id": "com.example.scene_interact_demo_v3",
  "name": "Scene Interact Demo V3",
  "version": "0.1.0",
  "capabilities": ["behaviors.graph.v1", "scenes.bundle.v1", "actors.generic.v1"],
  "runtime": { "module": "scripts/noop.wat", "provider": "wasm3-v1" },
  "entryScript": "scripts/init.json",
  "inputDefinitions": "config/input.json",
  "itemDefinitions": "items/items.json",
  "sceneDefinitions": "scenes/scenes.json",
  "actorDefinitions": "actors/actors.json",
  "behaviorDefinitions": "behaviors/behaviors.json"
}
'@

$noopWat = @'
(module
  (func (export "mod_init"))
  (func (export "mod_shutdown"))
)
'@

$initJson = @'
{
  "onGameLoaded": [
    { "action": "showNotification", "message": "Demo v3 loaded. Use Mod Action 3 + C-Up." }
  ]
}
'@

$inputJson = '{"bindings":[]}'
$itemsJson = '{"items":[]}'
$scenesJson = '{"scenes":[]}'
$actorsJson = '{"actors":[]}'
$behaviorsJson = '{"behaviors":[]}'
$assetsReadme = "Place optional custom scene resources here."

Set-Content -Path (Join-Path $demoDir "mod.json") -Value $modJson -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "scripts\noop.wat") -Value $noopWat -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "scripts\init.json") -Value $initJson -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "config\input.json") -Value $inputJson -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "items\items.json") -Value $itemsJson -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "scenes\scenes.json") -Value $scenesJson -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "actors\actors.json") -Value $actorsJson -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "behaviors\behaviors.json") -Value $behaviorsJson -Encoding UTF8
Set-Content -Path (Join-Path $demoDir "assets\README.txt") -Value $assetsReadme -Encoding UTF8

Write-Host "Created demo scaffold: $demoDir"
exit 0


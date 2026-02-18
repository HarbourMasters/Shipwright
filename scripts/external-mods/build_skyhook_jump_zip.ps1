param(
    [string]$ExampleRoot = "docs/examples/external_mods/skyhook_jump",
    [string]$OutputZip = "mods/skyhook_jump.zip"
)

$ErrorActionPreference = "Stop"

function Write-Step([string]$Message) {
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss zzz"
    Write-Output "[$timestamp] $Message"
}

if (!(Test-Path $ExampleRoot)) {
    throw "Example root not found: $ExampleRoot"
}

$manifestPath = Join-Path $ExampleRoot "mod.json"
$scriptPath = Join-Path $ExampleRoot "scripts/init.json"
$wasmPath = Join-Path $ExampleRoot "scripts/skyhook.wasm"
$itemsPath = Join-Path $ExampleRoot "items/items.json"
$inputPath = Join-Path $ExampleRoot "config/input.json"

$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json
$script = Get-Content $scriptPath -Raw | ConvertFrom-Json

if ($manifest.apiVersion -ne 2) {
    throw "mod.json apiVersion must be 2"
}
if ([string]::IsNullOrWhiteSpace($manifest.entryScript)) {
    throw "mod.json entryScript is required"
}
if ($manifest.runtime.type -ne "wasm3-v1") {
    throw "runtime.type must be wasm3-v1"
}
if ([string]::IsNullOrWhiteSpace($manifest.runtime.module)) {
    throw "runtime.module is required"
}
if ([string]::IsNullOrWhiteSpace($manifest.itemDefinitions) -or [string]::IsNullOrWhiteSpace($manifest.inputDefinitions)) {
    throw "itemDefinitions/inputDefinitions are required"
}
if ($script.apiVersion -ne 2) {
    throw "entry script apiVersion must be 2"
}
if (!(Test-Path $wasmPath)) {
    throw "Missing wasm module: $wasmPath"
}
if (!(Test-Path $itemsPath)) {
    throw "Missing item definitions: $itemsPath"
}
if (!(Test-Path $inputPath)) {
    throw "Missing input definitions: $inputPath"
}

$outputDir = Split-Path $OutputZip -Parent
if (![string]::IsNullOrWhiteSpace($outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}
if (Test-Path $OutputZip) {
    Remove-Item $OutputZip -Force
}

Compress-Archive -Path (Join-Path $ExampleRoot "*") -DestinationPath $OutputZip -Force

Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = [IO.Compression.ZipFile]::OpenRead((Resolve-Path $OutputZip))
try {
    $entries = $zip.Entries.FullName | ForEach-Object { ($_ -replace '\\', '/') }
    $required = @(
        'mod.json',
        'scripts/init.json',
        'scripts/skyhook.wasm',
        'items/items.json',
        'config/input.json'
    )

    foreach ($entry in $required) {
        if (($entries | Where-Object { $_ -eq $entry }).Count -eq 0) {
            throw "Zip missing required entry: $entry"
        }
    }
} finally {
    $zip.Dispose()
}

Write-Step "Generated API v2 package: $OutputZip"

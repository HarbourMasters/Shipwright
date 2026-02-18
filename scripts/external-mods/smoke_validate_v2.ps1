param(
    [string]$ExampleRoot = "docs/examples/external_mods/skyhook_jump",
    [string]$OutputZip = "build-smoke/skyhook_jump.zip"
)

$ErrorActionPreference = "Stop"

function Write-Step([string]$Message) {
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss zzz"
    Write-Output "[$timestamp] $Message"
}

if (!(Test-Path $ExampleRoot)) {
    throw "Example root not found: $ExampleRoot"
}

$manifest = Get-Content (Join-Path $ExampleRoot "mod.json") -Raw | ConvertFrom-Json
$script = Get-Content (Join-Path $ExampleRoot "scripts/init.json") -Raw | ConvertFrom-Json
$items = Get-Content (Join-Path $ExampleRoot "items/items.json") -Raw | ConvertFrom-Json
$input = Get-Content (Join-Path $ExampleRoot "config/input.json") -Raw | ConvertFrom-Json

if ($manifest.apiVersion -ne 2) { throw "mod.json apiVersion must be 2" }
if ($manifest.runtime.type -ne "wasm3-v1") { throw "runtime.type must be wasm3-v1" }
if ($script.apiVersion -ne 2) { throw "scripts/init.json apiVersion must be 2" }
if ($items.items.Count -lt 1) { throw "items/items.json must contain at least one item" }
if ($input.bindings.Count -lt 1) { throw "config/input.json must contain at least one binding" }

New-Item -ItemType Directory -Force -Path (Split-Path $OutputZip -Parent) | Out-Null
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

Write-Step "API v2 smoke validation passed."

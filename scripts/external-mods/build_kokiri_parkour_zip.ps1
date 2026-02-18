param(
    [string]$ExampleRoot = "docs/examples/external_mods/kokiri_parkour",
    [string]$OutputZip = "mods/kokiri_parkour.zip",
    [string]$AssetSource = "",
    [switch]$LogicOnly
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
$assetTargetRelative = "assets/parkour_room.otr"
$assetTargetPath = Join-Path $ExampleRoot $assetTargetRelative

$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json
$script = Get-Content $scriptPath -Raw | ConvertFrom-Json

if ($manifest.apiVersion -ne 1) {
    throw "mod.json apiVersion must be 1"
}
if ([string]::IsNullOrWhiteSpace($manifest.entryScript)) {
    throw "mod.json entryScript is required"
}
if ($script.apiVersion -ne 1) {
    throw "entry script apiVersion must be 1"
}

if ($LogicOnly) {
    Write-Step "Building logic-only package (without external asset archive)."
} else {
    Write-Step "Building full package (asset + logic)."
    if (![string]::IsNullOrWhiteSpace($AssetSource)) {
        if (!(Test-Path $AssetSource)) {
            throw "AssetSource not found: $AssetSource"
        }
        New-Item -ItemType Directory -Path (Split-Path $assetTargetPath -Parent) -Force | Out-Null
        Copy-Item $AssetSource $assetTargetPath -Force
        Write-Step "Copied asset source to $assetTargetRelative"
    }

    if (!(Test-Path $assetTargetPath)) {
        throw "Missing asset file '$assetTargetRelative'. Provide -AssetSource or create the file first."
    }
}

$tempRoot = Join-Path "build-smoke" "kokiri_parkour_pkg"
if (Test-Path $tempRoot) {
    Remove-Item $tempRoot -Recurse -Force
}
New-Item -ItemType Directory -Path $tempRoot -Force | Out-Null

Copy-Item (Join-Path $ExampleRoot "*") $tempRoot -Recurse -Force

if ($LogicOnly) {
    $tempManifestPath = Join-Path $tempRoot "mod.json"
    $tempManifest = Get-Content $tempManifestPath -Raw | ConvertFrom-Json
    $tempManifest.assets = @()
    ($tempManifest | ConvertTo-Json -Depth 10) | Set-Content $tempManifestPath -NoNewline
}

$outputDir = Split-Path $OutputZip -Parent
if (![string]::IsNullOrWhiteSpace($outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}
if (Test-Path $OutputZip) {
    Remove-Item $OutputZip -Force
}

Compress-Archive -Path (Join-Path $tempRoot "*") -DestinationPath $OutputZip -Force

Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = [IO.Compression.ZipFile]::OpenRead((Resolve-Path $OutputZip))
try {
    $entries = $zip.Entries.FullName
    $normalizedEntries = $entries | ForEach-Object { ($_ -replace '\\', '/') }

    if (($normalizedEntries | Where-Object { $_ -match '(^|/)mod\.json$' }).Count -eq 0) {
        throw "Zip does not contain mod.json at root"
    }
    if (($normalizedEntries | Where-Object { $_ -match '(^|/)scripts/init\.json$' }).Count -eq 0) {
        throw "Zip does not contain scripts/init.json"
    }
    if (-not $LogicOnly) {
        if (($normalizedEntries | Where-Object { $_ -match '(^|/)assets/parkour_room\.otr$' }).Count -eq 0) {
            throw "Zip does not contain assets/parkour_room.otr"
        }
    }
} finally {
    $zip.Dispose()
}

if ($LogicOnly) {
    Write-Step "Generated logic-only package: $OutputZip"
} else {
    Write-Step "Generated package with asset + logic: $OutputZip"
}

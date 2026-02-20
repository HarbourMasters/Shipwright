param(
    [string]$ExampleRoot = "docs/examples/external_mods/skyhook_jump",
    [string]$OutputZip = "build-smoke/skyhook_jump.zip"
)

$ErrorActionPreference = "Stop"

function Write-Step([string]$Message) {
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss zzz"
    Write-Output "[$timestamp] $Message"
}

Add-Type -AssemblyName System.Drawing

function Get-PngAlphaStats([string]$Path) {
    $bitmap = [System.Drawing.Bitmap]::FromFile((Resolve-Path $Path))
    try {
        $transparentPixels = 0
        for ($y = 0; $y -lt $bitmap.Height; $y++) {
            for ($x = 0; $x -lt $bitmap.Width; $x++) {
                if ($bitmap.GetPixel($x, $y).A -eq 0) {
                    $transparentPixels++
                }
            }
        }

        return [pscustomobject]@{
            Width             = [int]$bitmap.Width
            Height            = [int]$bitmap.Height
            PixelCount        = [int]($bitmap.Width * $bitmap.Height)
            TransparentPixels = [int]$transparentPixels
        }
    } finally {
        $bitmap.Dispose()
    }
}

if (!(Test-Path $ExampleRoot)) {
    throw "Example root not found: $ExampleRoot"
}

$manifest = Get-Content (Join-Path $ExampleRoot "mod.json") -Raw | ConvertFrom-Json
$script = Get-Content (Join-Path $ExampleRoot "scripts/init.json") -Raw | ConvertFrom-Json
$items = Get-Content (Join-Path $ExampleRoot "items/items.json") -Raw | ConvertFrom-Json
$input = Get-Content (Join-Path $ExampleRoot "config/input.json") -Raw | ConvertFrom-Json

$capabilities = @()
if ($manifest.PSObject.Properties.Name -contains 'capabilities' -and $null -ne $manifest.capabilities) {
    $capabilities = @($manifest.capabilities)
}

if ($manifest.apiVersion -ne 2) { throw "mod.json apiVersion must be 2" }
if ($manifest.runtime.type -ne "wasm3-v1") { throw "runtime.type must be wasm3-v1" }
if ($script.apiVersion -ne 2) { throw "scripts/init.json apiVersion must be 2" }
if ($items.items.Count -lt 1) { throw "items/items.json must contain at least one item" }
if ($input.bindings.Count -lt 1) { throw "config/input.json must contain at least one binding" }
if ([string]::IsNullOrWhiteSpace($manifest.runtime.module)) { throw "runtime.module is required" }

$runtimeModulePath = Join-Path $ExampleRoot $manifest.runtime.module
if (!(Test-Path $runtimeModulePath)) {
    throw "Missing runtime module: $runtimeModulePath"
}
$runtimeModuleExt = [IO.Path]::GetExtension($runtimeModulePath).ToLowerInvariant()
if ($runtimeModuleExt -notin @('.wasm', '.wat')) {
    throw "runtime.module must use .wasm or .wat: $($manifest.runtime.module)"
}
$runtimeModuleEntry = ($manifest.runtime.module -replace '\\', '/')

foreach ($item in $items.items) {
    if (![string]::IsNullOrWhiteSpace($item.iconAsset)) {
        $iconPath = Join-Path $ExampleRoot $item.iconAsset
        if (!(Test-Path $iconPath)) {
            throw "Missing icon asset: $iconPath"
        }

        if ([IO.Path]::GetExtension($iconPath).ToLowerInvariant() -ne '.png') {
            throw "iconAsset must be .png for $($item.id): $($item.iconAsset)"
        }
    }

    $modelAssetValue = $null
    $modelTextureValue = $null
    $modelDisplayListValue = $null
    $modelUvOriginValue = $null
    $modelTextureFilterValue = $null
    $modelTextureWidthValue = 0
    $modelTextureHeightValue = 0
    $modelTextureWidthSpecified = $false
    $modelTextureHeightSpecified = $false
    if ($item.PSObject.Properties.Name -contains 'modelAsset' -and ![string]::IsNullOrWhiteSpace($item.modelAsset)) {
        $modelAssetValue = [string]$item.modelAsset
    }
    if ($item.PSObject.Properties.Name -contains 'modelTextureAsset' -and ![string]::IsNullOrWhiteSpace($item.modelTextureAsset)) {
        $modelTextureValue = [string]$item.modelTextureAsset
    }
    if ($item.PSObject.Properties.Name -contains 'modelDisplayList' -and ![string]::IsNullOrWhiteSpace($item.modelDisplayList)) {
        $modelDisplayListValue = [string]$item.modelDisplayList
    }
    if ($item.PSObject.Properties.Name -contains 'modelUvOrigin' -and ![string]::IsNullOrWhiteSpace($item.modelUvOrigin)) {
        $modelUvOriginValue = [string]$item.modelUvOrigin
    }
    if ($item.PSObject.Properties.Name -contains 'modelTextureFilter' -and ![string]::IsNullOrWhiteSpace($item.modelTextureFilter)) {
        $modelTextureFilterValue = [string]$item.modelTextureFilter
    }
    if ($item.PSObject.Properties.Name -contains 'modelTextureWidth') {
        if ($item.modelTextureWidth -isnot [int] -and $item.modelTextureWidth -isnot [long]) {
            throw "modelTextureWidth must be integer for $($item.id)"
        }
        $modelTextureWidthValue = [int]$item.modelTextureWidth
        $modelTextureWidthSpecified = $true
    }
    if ($item.PSObject.Properties.Name -contains 'modelTextureHeight') {
        if ($item.modelTextureHeight -isnot [int] -and $item.modelTextureHeight -isnot [long]) {
            throw "modelTextureHeight must be integer for $($item.id)"
        }
        $modelTextureHeightValue = [int]$item.modelTextureHeight
        $modelTextureHeightSpecified = $true
    }
    if ($item.PSObject.Properties.Name -contains 'model' -and $null -ne $item.model) {
        if (-not ($item.model -is [pscustomobject])) {
            throw "item.model must be object for $($item.id)"
        }
        if ($item.model.PSObject.Properties.Name -contains 'asset' -and ![string]::IsNullOrWhiteSpace($item.model.asset)) {
            $modelAssetValue = [string]$item.model.asset
        }
        if ($item.model.PSObject.Properties.Name -contains 'texture' -and ![string]::IsNullOrWhiteSpace($item.model.texture)) {
            $modelTextureValue = [string]$item.model.texture
        }
        if ($item.model.PSObject.Properties.Name -contains 'displayList' -and ![string]::IsNullOrWhiteSpace($item.model.displayList)) {
            $modelDisplayListValue = [string]$item.model.displayList
        }
        if ($item.model.PSObject.Properties.Name -contains 'uvOrigin' -and ![string]::IsNullOrWhiteSpace($item.model.uvOrigin)) {
            $modelUvOriginValue = [string]$item.model.uvOrigin
        }
        if ($item.model.PSObject.Properties.Name -contains 'textureFilter' -and ![string]::IsNullOrWhiteSpace($item.model.textureFilter)) {
            $modelTextureFilterValue = [string]$item.model.textureFilter
        }
        if ($item.model.PSObject.Properties.Name -contains 'textureWidth') {
            if ($item.model.textureWidth -isnot [int] -and $item.model.textureWidth -isnot [long]) {
                throw "model.textureWidth must be integer for $($item.id)"
            }
            $modelTextureWidthValue = [int]$item.model.textureWidth
            $modelTextureWidthSpecified = $true
        }
        if ($item.model.PSObject.Properties.Name -contains 'textureHeight') {
            if ($item.model.textureHeight -isnot [int] -and $item.model.textureHeight -isnot [long]) {
                throw "model.textureHeight must be integer for $($item.id)"
            }
            $modelTextureHeightValue = [int]$item.model.textureHeight
            $modelTextureHeightSpecified = $true
        }
    }

    if ($null -ne $modelUvOriginValue) {
        $normalizedModelUvOrigin = $modelUvOriginValue.ToLowerInvariant()
        if ($normalizedModelUvOrigin -notin @('auto', 'bottom_left', 'top_left')) {
            throw "Invalid modelUvOrigin for $($item.id): $modelUvOriginValue (expected auto|bottom_left|top_left)"
        }
    }
    if ($null -ne $modelTextureFilterValue) {
        $normalizedModelTextureFilter = $modelTextureFilterValue.ToLowerInvariant()
        if ($normalizedModelTextureFilter -notin @('auto', 'point', 'bilerp')) {
            throw "Invalid modelTextureFilter for $($item.id): $modelTextureFilterValue (expected auto|point|bilerp)"
        }
    }
    if ($modelTextureWidthSpecified -ne $modelTextureHeightSpecified) {
        throw "modelTextureWidth/modelTextureHeight must be provided together for $($item.id)"
    }
    if ($modelTextureWidthSpecified) {
        if ($modelTextureWidthValue -lt 1 -or $modelTextureWidthValue -gt 1024 -or
            $modelTextureHeightValue -lt 1 -or $modelTextureHeightValue -gt 1024) {
            throw "modelTextureWidth/modelTextureHeight must be in range 1..1024 for $($item.id)"
        }
    }

    $modelAssetExt = $null
    if ($null -ne $modelAssetValue) {
        $modelPath = Join-Path $ExampleRoot $modelAssetValue
        if (!(Test-Path $modelPath)) {
            throw "Missing model asset: $modelPath"
        }
        $modelAssetExt = [IO.Path]::GetExtension($modelPath).ToLowerInvariant()
        if ($modelAssetExt -notin @('.obj', '.otr', '.o2r')) {
            throw "model asset must be .obj, .otr, or .o2r for $($item.id): $modelAssetValue"
        }
        if ($modelAssetExt -in @('.otr', '.o2r') -and [string]::IsNullOrWhiteSpace($modelDisplayListValue)) {
            throw "modelDisplayList is required when modelAsset uses .otr/.o2r for $($item.id)"
        }
    }
    if ($null -ne $modelTextureValue) {
        if ($null -eq $modelAssetValue) {
            throw "modelTextureAsset requires modelAsset for $($item.id)"
        }
        if ($modelAssetExt -ne '.obj') {
            throw "modelTextureAsset is only supported with .obj modelAsset for $($item.id)"
        }
        $modelTexPath = Join-Path $ExampleRoot $modelTextureValue
        if (!(Test-Path $modelTexPath)) {
            throw "Missing model texture asset: $modelTexPath"
        }
        if ([IO.Path]::GetExtension($modelTexPath).ToLowerInvariant() -ne '.png') {
            throw "model texture must be .png for $($item.id): $modelTextureValue"
        }

        $modelTextureStats = $null
        try {
            $modelTextureStats = Get-PngAlphaStats $modelTexPath
        } catch {
            throw "model texture decode failed for $($item.id): $modelTextureValue ($($_.Exception.Message))"
        }

        $effectiveModelTextureWidth = $modelTextureStats.Width
        $effectiveModelTextureHeight = $modelTextureStats.Height
        if ($modelTextureWidthSpecified) {
            $effectiveModelTextureWidth = $modelTextureWidthValue
            $effectiveModelTextureHeight = $modelTextureHeightValue
        }
        if ($effectiveModelTextureWidth -lt 1 -or $effectiveModelTextureHeight -lt 1) {
            throw "Invalid effective model texture size for $($item.id): ${effectiveModelTextureWidth}x${effectiveModelTextureHeight}"
        }

        if ($modelTextureStats.TransparentPixels -gt 0) {
            throw "model texture must be fully opaque for $($item.id): $modelTextureValue has $($modelTextureStats.TransparentPixels)/$($modelTextureStats.PixelCount) transparent pixels"
        }
    }
    if ($modelTextureWidthSpecified) {
        if ($null -eq $modelTextureValue) {
            throw "modelTextureWidth/modelTextureHeight requires modelTextureAsset for $($item.id)"
        }
        if ($null -eq $modelAssetValue -or $modelAssetExt -ne '.obj') {
            throw "modelTextureWidth/modelTextureHeight is only supported with .obj modelAsset for $($item.id)"
        }
    }

    $hookshotTextureObject = $null
    if ($item.PSObject.Properties.Name -contains 'hookshotTextures' -and $null -ne $item.hookshotTextures) {
        if (-not ($item.hookshotTextures -is [pscustomobject])) {
            throw "hookshotTextures must be object for $($item.id)"
        }
        $hookshotTextureObject = $item.hookshotTextures
    }

    $hookshotTextureFieldMap = @{
        metal   = 'hookshotMetalTextureAsset'
        handle  = 'hookshotHandleTextureAsset'
        design  = 'hookshotDesignTextureAsset'
        chain   = 'hookshotChainTextureAsset'
        reticle = 'hookshotReticleTextureAsset'
    }
    foreach ($kv in $hookshotTextureFieldMap.GetEnumerator()) {
        $assetValue = $null
        if ($item.PSObject.Properties.Name -contains $kv.Value -and ![string]::IsNullOrWhiteSpace($item.($kv.Value))) {
            $assetValue = [string]$item.($kv.Value)
        }
        if ($null -ne $hookshotTextureObject -and
            $hookshotTextureObject.PSObject.Properties.Name -contains $kv.Key -and
            ![string]::IsNullOrWhiteSpace($hookshotTextureObject.($kv.Key))) {
            $assetValue = [string]$hookshotTextureObject.($kv.Key)
        }
        if ($null -ne $assetValue) {
            $hookshotTexturePath = Join-Path $ExampleRoot $assetValue
            if (!(Test-Path $hookshotTexturePath)) {
                throw "Missing hookshot texture asset for $($item.id) [$($kv.Key)]: $hookshotTexturePath"
            }
            if ([IO.Path]::GetExtension($hookshotTexturePath).ToLowerInvariant() -ne '.png') {
                throw "Hookshot texture must be .png for $($item.id) [$($kv.Key)]: $assetValue"
            }
        }
    }

    if ($item.PSObject.Properties.Name -contains 'agePolicy' -and ![string]::IsNullOrWhiteSpace($item.agePolicy)) {
        if ($item.agePolicy -notin @('respectVanilla', 'allowChild', 'allowAdult')) {
            throw "Invalid item agePolicy for $($item.id): $($item.agePolicy)"
        }
    }

    if ($item.PSObject.Properties.Name -contains 'useMode' -and ![string]::IsNullOrWhiteSpace($item.useMode)) {
        if ($item.useMode -notin @('vanilla', 'override', 'augment')) {
            throw "Invalid item useMode for $($item.id): $($item.useMode)"
        }
    }

    if ($item.PSObject.Properties.Name -contains 'grant' -and $null -ne $item.grant) {
        if (-not ($item.grant -is [pscustomobject])) {
            throw "item.grant must be object for $($item.id)"
        }
        if ($item.grant.PSObject.Properties.Name -contains 'ammo' -and $item.grant.ammo -lt 0) {
            throw "item.grant.ammo must be >= 0 for $($item.id)"
        }
    }
}

$iconEntries = @()
$modelEntries = @()
$hookshotTextureEntries = @()
foreach ($item in $items.items) {
    if (![string]::IsNullOrWhiteSpace($item.iconAsset)) {
        $iconEntries += ($item.iconAsset -replace '\\', '/')
    }

    $modelAssetValue = $null
    $modelTextureValue = $null
    if ($item.PSObject.Properties.Name -contains 'modelAsset' -and ![string]::IsNullOrWhiteSpace($item.modelAsset)) {
        $modelAssetValue = [string]$item.modelAsset
    }
    if ($item.PSObject.Properties.Name -contains 'modelTextureAsset' -and ![string]::IsNullOrWhiteSpace($item.modelTextureAsset)) {
        $modelTextureValue = [string]$item.modelTextureAsset
    }
    if ($item.PSObject.Properties.Name -contains 'model' -and $null -ne $item.model) {
        if ($item.model.PSObject.Properties.Name -contains 'asset' -and ![string]::IsNullOrWhiteSpace($item.model.asset)) {
            $modelAssetValue = [string]$item.model.asset
        }
        if ($item.model.PSObject.Properties.Name -contains 'texture' -and ![string]::IsNullOrWhiteSpace($item.model.texture)) {
            $modelTextureValue = [string]$item.model.texture
        }
    }
    if ($null -ne $modelAssetValue) {
        $modelEntries += ($modelAssetValue -replace '\\', '/')
    }
    if ($null -ne $modelTextureValue) {
        $modelEntries += ($modelTextureValue -replace '\\', '/')
    }

    $hookshotTextureObject = $null
    if ($item.PSObject.Properties.Name -contains 'hookshotTextures' -and $null -ne $item.hookshotTextures) {
        $hookshotTextureObject = $item.hookshotTextures
    }
    $hookshotTextureFieldMap = @{
        metal   = 'hookshotMetalTextureAsset'
        handle  = 'hookshotHandleTextureAsset'
        design  = 'hookshotDesignTextureAsset'
        chain   = 'hookshotChainTextureAsset'
        reticle = 'hookshotReticleTextureAsset'
    }
    foreach ($kv in $hookshotTextureFieldMap.GetEnumerator()) {
        $assetValue = $null
        if ($item.PSObject.Properties.Name -contains $kv.Value -and ![string]::IsNullOrWhiteSpace($item.($kv.Value))) {
            $assetValue = [string]$item.($kv.Value)
        }
        if ($null -ne $hookshotTextureObject -and
            $hookshotTextureObject.PSObject.Properties.Name -contains $kv.Key -and
            ![string]::IsNullOrWhiteSpace($hookshotTextureObject.($kv.Key))) {
            $assetValue = [string]$hookshotTextureObject.($kv.Key)
        }
        if ($null -ne $assetValue) {
            $hookshotTextureEntries += ($assetValue -replace '\\', '/')
        }
    }
}
$iconEntries = $iconEntries | Sort-Object -Unique
$modelEntries = $modelEntries | Sort-Object -Unique
$hookshotTextureEntries = $hookshotTextureEntries | Sort-Object -Unique

if ($capabilities -contains 'hooks.extended.v1') {
    if (-not ($manifest.PSObject.Properties.Name -contains 'hookDefinitions') -or
        [string]::IsNullOrWhiteSpace($manifest.hookDefinitions)) {
        throw "hooks.extended.v1 requires hookDefinitions"
    }
    if (!(Test-Path (Join-Path $ExampleRoot $manifest.hookDefinitions))) {
        throw "Missing hook definitions: $(Join-Path $ExampleRoot $manifest.hookDefinitions)"
    }
}

if ($capabilities -contains 'actors.vm.v1') {
    if (-not ($manifest.PSObject.Properties.Name -contains 'actorDefinitions') -or
        [string]::IsNullOrWhiteSpace($manifest.actorDefinitions)) {
        throw "actors.vm.v1 requires actorDefinitions"
    }
    if (!(Test-Path (Join-Path $ExampleRoot $manifest.actorDefinitions))) {
        throw "Missing actor definitions: $(Join-Path $ExampleRoot $manifest.actorDefinitions)"
    }
}

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
        $runtimeModuleEntry,
        'items/items.json',
        'config/input.json'
    )
    $required += $iconEntries
    $required += $modelEntries
    $required += $hookshotTextureEntries
    if ($capabilities -contains 'hooks.extended.v1') {
        $required += (($manifest.hookDefinitions -replace '\\', '/'))
    }
    if ($capabilities -contains 'actors.vm.v1') {
        $required += (($manifest.actorDefinitions -replace '\\', '/'))
    }
    foreach ($entry in $required) {
        if (($entries | Where-Object { $_ -eq $entry }).Count -eq 0) {
            throw "Zip missing required entry: $entry"
        }
    }
} finally {
    $zip.Dispose()
}

Write-Step "API v2 smoke validation passed."

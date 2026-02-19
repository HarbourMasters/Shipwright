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
    if ($item.PSObject.Properties.Name -contains 'modelAsset' -and ![string]::IsNullOrWhiteSpace($item.modelAsset)) {
        $modelAssetValue = [string]$item.modelAsset
    }
    if ($item.PSObject.Properties.Name -contains 'modelTextureAsset' -and ![string]::IsNullOrWhiteSpace($item.modelTextureAsset)) {
        $modelTextureValue = [string]$item.modelTextureAsset
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
    }

    if ($null -ne $modelAssetValue) {
        $modelPath = Join-Path $ExampleRoot $modelAssetValue
        if (!(Test-Path $modelPath)) {
            throw "Missing model asset: $modelPath"
        }
        if ([IO.Path]::GetExtension($modelPath).ToLowerInvariant() -ne '.obj') {
            throw "model asset must be .obj for $($item.id): $modelAssetValue"
        }
    }
    if ($null -ne $modelTextureValue) {
        if ($null -eq $modelAssetValue) {
            throw "modelTextureAsset requires modelAsset for $($item.id)"
        }
        $modelTexPath = Join-Path $ExampleRoot $modelTextureValue
        if (!(Test-Path $modelTexPath)) {
            throw "Missing model texture asset: $modelTexPath"
        }
        if ([IO.Path]::GetExtension($modelTexPath).ToLowerInvariant() -ne '.png') {
            throw "model texture must be .png for $($item.id): $modelTextureValue"
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
}
$iconEntries = $iconEntries | Sort-Object -Unique
$modelEntries = $modelEntries | Sort-Object -Unique

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

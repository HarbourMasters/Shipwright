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
$itemsPath = Join-Path $ExampleRoot "items/items.json"
$inputPath = Join-Path $ExampleRoot "config/input.json"

$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json
$script = Get-Content $scriptPath -Raw | ConvertFrom-Json
$items = Get-Content $itemsPath -Raw | ConvertFrom-Json

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
if (!(Test-Path $itemsPath)) {
    throw "Missing item definitions: $itemsPath"
}
if (!(Test-Path $inputPath)) {
    throw "Missing input definitions: $inputPath"
}

$runtimeModulePath = Join-Path $ExampleRoot $manifest.runtime.module
if (!(Test-Path $runtimeModulePath)) {
    throw "Missing runtime module: $runtimeModulePath"
}
$runtimeModuleExt = [IO.Path]::GetExtension($runtimeModulePath).ToLowerInvariant()
if ($runtimeModuleExt -notin @('.wasm', '.wat')) {
    throw "runtime.module must use .wasm or .wat: $($manifest.runtime.module)"
}
$runtimeModuleEntry = ($manifest.runtime.module -replace '\\', '/')

$iconEntries = @()
foreach ($item in $items.items) {
    if (![string]::IsNullOrWhiteSpace($item.iconAsset)) {
        $iconPath = Join-Path $ExampleRoot $item.iconAsset
        if (!(Test-Path $iconPath)) {
            throw "Missing icon asset: $iconPath"
        }
        $iconEntries += ($item.iconAsset -replace '\\', '/')
    }

    $modelAssetValue = $null
    $modelTextureValue = $null
    $modelDisplayListValue = $null
    if ($item.PSObject.Properties.Name -contains 'modelAsset' -and ![string]::IsNullOrWhiteSpace($item.modelAsset)) {
        $modelAssetValue = [string]$item.modelAsset
    }
    if ($item.PSObject.Properties.Name -contains 'modelTextureAsset' -and ![string]::IsNullOrWhiteSpace($item.modelTextureAsset)) {
        $modelTextureValue = [string]$item.modelTextureAsset
    }
    if ($item.PSObject.Properties.Name -contains 'modelDisplayList' -and ![string]::IsNullOrWhiteSpace($item.modelDisplayList)) {
        $modelDisplayListValue = [string]$item.modelDisplayList
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
    }

    $modelAssetExt = $null
    if ($null -ne $modelAssetValue) {
        $modelPath = Join-Path $ExampleRoot $modelAssetValue
        if (!(Test-Path $modelPath)) {
            throw "Missing model asset: $modelPath"
        }
        $modelAssetExt = [IO.Path]::GetExtension($modelPath).ToLowerInvariant()
        if ($modelAssetExt -notin @('.obj', '.otr', '.o2r')) {
            throw "model asset must use .obj, .otr, or .o2r for $($item.id): $modelAssetValue"
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
            throw "model texture must use .png for $($item.id): $modelTextureValue"
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
$iconEntries = $iconEntries | Sort-Object -Unique

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
        $runtimeModuleEntry,
        'items/items.json',
        'config/input.json'
    )
    $required += $iconEntries

    foreach ($entry in $required) {
        if (($entries | Where-Object { $_ -eq $entry }).Count -eq 0) {
            throw "Zip missing required entry: $entry"
        }
    }

    foreach ($item in $items.items) {
        if ($item.PSObject.Properties.Name -contains 'modelAsset' -and ![string]::IsNullOrWhiteSpace($item.modelAsset)) {
            $entry = ([string]$item.modelAsset -replace '\\', '/')
            if (($entries | Where-Object { $_ -eq $entry }).Count -eq 0) {
                throw "Zip missing model entry: $entry"
            }
        }
        if ($item.PSObject.Properties.Name -contains 'modelTextureAsset' -and ![string]::IsNullOrWhiteSpace($item.modelTextureAsset)) {
            $entry = ([string]$item.modelTextureAsset -replace '\\', '/')
            if (($entries | Where-Object { $_ -eq $entry }).Count -eq 0) {
                throw "Zip missing model texture entry: $entry"
            }
        }
        if ($item.PSObject.Properties.Name -contains 'model' -and $null -ne $item.model) {
            if ($item.model.PSObject.Properties.Name -contains 'asset' -and ![string]::IsNullOrWhiteSpace($item.model.asset)) {
                $entry = ([string]$item.model.asset -replace '\\', '/')
                if (($entries | Where-Object { $_ -eq $entry }).Count -eq 0) {
                    throw "Zip missing model entry: $entry"
                }
            }
            if ($item.model.PSObject.Properties.Name -contains 'texture' -and ![string]::IsNullOrWhiteSpace($item.model.texture)) {
                $entry = ([string]$item.model.texture -replace '\\', '/')
                if (($entries | Where-Object { $_ -eq $entry }).Count -eq 0) {
                    throw "Zip missing model texture entry: $entry"
                }
            }
        }
    }
} finally {
    $zip.Dispose()
}

Write-Step "Generated API v2 package: $OutputZip"

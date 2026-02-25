[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$ModsRoot = "docs/examples/external_mods",
    [switch]$IncludeRuntimeMirror,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-ObjectPropertyValue {
    param(
        [Parameter(Mandatory = $true)]$Object,
        [Parameter(Mandatory = $true)][string]$Name
    )

    if ($null -eq $Object) {
        return $null
    }

    if ($Object -is [System.Collections.IDictionary]) {
        if ($Object.Contains($Name)) {
            return $Object[$Name]
        }
        return $null
    }

    if ($Object.PSObject.Properties.Name -contains $Name) {
        return $Object.$Name
    }

    return $null
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$resolvedModsRoot = if ([System.IO.Path]::IsPathRooted($ModsRoot)) {
    $ModsRoot
} else {
    Join-Path $resolvedRoot $ModsRoot
}

if (-not (Test-Path $resolvedModsRoot)) {
    Write-Error "Mods root not found: $resolvedModsRoot"
    exit 1
}

function Update-ModManifest {
    param([string]$ManifestPath)

    try {
        $manifest = Get-Content -Path $ManifestPath -Raw | ConvertFrom-Json
    } catch {
        Write-Warning "Skipping invalid manifest: $ManifestPath"
        return
    }

    $changed = $false
    $manifestDir = Split-Path -Path $ManifestPath -Parent
    if ($manifest.apiVersion -ne 4) {
        $manifest.apiVersion = 4
        $changed = $true
    }
    if (-not ($manifest.PSObject.Properties.Name -contains "engineVersionRange")) {
        $manifest | Add-Member -NotePropertyName "engineVersionRange" -NotePropertyValue ">=9.1.2 <10.0.0"
        $changed = $true
    }
    if (-not ($manifest.PSObject.Properties.Name -contains "permissions")) {
        $manifest | Add-Member -NotePropertyName "permissions" -NotePropertyValue @()
        $changed = $true
    }
    if (-not ($manifest.PSObject.Properties.Name -contains "runtime") -or $null -eq $manifest.runtime) {
        $runtime = [ordered]@{
            type = "wasm3-v1"
            module = "scripts/noop.wat"
            maxMemoryKb = 1024
            maxCallMs = 2
            maxFrameBudgetMs = 2
            maxHookCallsPerFrame = 256
            maxActorInstances = 64
            maxActiveStatuses = 256
        }
        $manifest | Add-Member -NotePropertyName "runtime" -NotePropertyValue $runtime
        $changed = $true
    } else {
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "type") -or [string]::IsNullOrWhiteSpace("$($manifest.runtime.type)")) {
            $manifest.runtime.type = "wasm3-v1"
            $changed = $true
        }
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "module") -or [string]::IsNullOrWhiteSpace("$($manifest.runtime.module)")) {
            $manifest.runtime | Add-Member -NotePropertyName "module" -NotePropertyValue "scripts/noop.wat"
            $changed = $true
        }
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "maxMemoryKb")) {
            $manifest.runtime | Add-Member -NotePropertyName "maxMemoryKb" -NotePropertyValue 1024
            $changed = $true
        }
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "maxCallMs")) {
            $manifest.runtime | Add-Member -NotePropertyName "maxCallMs" -NotePropertyValue 2
            $changed = $true
        }
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "maxFrameBudgetMs")) {
            $manifest.runtime | Add-Member -NotePropertyName "maxFrameBudgetMs" -NotePropertyValue 2
            $changed = $true
        }
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "maxHookCallsPerFrame")) {
            $manifest.runtime | Add-Member -NotePropertyName "maxHookCallsPerFrame" -NotePropertyValue 256
            $changed = $true
        }
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "maxActorInstances")) {
            $manifest.runtime | Add-Member -NotePropertyName "maxActorInstances" -NotePropertyValue 64
            $changed = $true
        }
        if (-not ($manifest.runtime.PSObject.Properties.Name -contains "maxActiveStatuses")) {
            $manifest.runtime | Add-Member -NotePropertyName "maxActiveStatuses" -NotePropertyValue 256
            $changed = $true
        }
    }
    if (-not ($manifest.PSObject.Properties.Name -contains "entrypoints")) {
        $runtimeModule = ""
        $runtimeModuleValue = Get-ObjectPropertyValue -Object (Get-ObjectPropertyValue -Object $manifest -Name "runtime") -Name "module"
        if ($null -ne $runtimeModuleValue) {
            $runtimeModule = "$runtimeModuleValue"
        }
        $entrypoints = [ordered]@{
            items = "items/items.json"
            combat = "combat"
            movement = "movement"
            camera = "camera/camera_profiles.json"
            ui = "ui"
            actors = "actors/actors.json"
            quests = "quests/quests.json"
            wasm = $runtimeModule
        }
        $manifest | Add-Member -NotePropertyName "entrypoints" -NotePropertyValue $entrypoints
        $changed = $true
    }

    $runtimeModuleValue = Get-ObjectPropertyValue -Object (Get-ObjectPropertyValue -Object $manifest -Name "runtime") -Name "module"
    $runtimeModule = if ($null -ne $runtimeModuleValue) { "$runtimeModuleValue" } else { "" }

    $shouldWriteModulePlaceholder = -not [string]::IsNullOrWhiteSpace($runtimeModule) -and
        $runtimeModule.ToLowerInvariant().EndsWith(".wat")

    if (-not $changed -and -not $shouldWriteModulePlaceholder) {
        return
    }

    if ($DryRun) {
        Write-Host "[DryRun] Would migrate $ManifestPath to apiVersion=4"
        if ($shouldWriteModulePlaceholder) {
            $placeholderPath = Join-Path $manifestDir $runtimeModule
            if (-not (Test-Path $placeholderPath)) {
                Write-Host "[DryRun] Would create placeholder WAT module at $placeholderPath"
            }
        }
        return
    }

    if ($changed) {
        $json = $manifest | ConvertTo-Json -Depth 100
        Set-Content -Path $ManifestPath -Value $json -Encoding UTF8
        Write-Host "Migrated $ManifestPath"
    }

    if ($shouldWriteModulePlaceholder) {
        $placeholderPath = Join-Path $manifestDir $runtimeModule
        if (-not (Test-Path $placeholderPath)) {
            $placeholderDir = Split-Path -Path $placeholderPath -Parent
            if (-not (Test-Path $placeholderDir)) {
                New-Item -ItemType Directory -Path $placeholderDir -Force | Out-Null
            }
            Set-Content -Path $placeholderPath -Value "(module)" -Encoding UTF8
            Write-Host "Created placeholder runtime module: $placeholderPath"
        }
    }
}

$manifestFiles = Get-ChildItem -Path $resolvedModsRoot -Recurse -Filter "mod.json" -File
foreach ($manifestFile in $manifestFiles) {
    Update-ModManifest -ManifestPath $manifestFile.FullName
}

if ($IncludeRuntimeMirror) {
    $runtimeRoot = Join-Path $resolvedRoot "x64/Release/mods"
    if (Test-Path $runtimeRoot) {
        $runtimeManifests = Get-ChildItem -Path $runtimeRoot -Recurse -Filter "mod.json" -File
        foreach ($manifestFile in $runtimeManifests) {
            Update-ModManifest -ManifestPath $manifestFile.FullName
        }
    }
}

Write-Host "Migration pass complete."
exit 0

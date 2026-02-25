[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$ModPath = "",
    [switch]$DryRun,
    [switch]$VerboseOutput
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Fail([string]$Message) {
    Write-Error $Message
    exit 1
}

function Resolve-CapabilityPathMap {
    return @{
        "hooks.extended.v1" = "hookDefinitions"
        "behaviors.graph.v1" = "behaviorDefinitions"
        "actors.vm.v1" = "actorDefinitions"
        "actors.generic.v1" = "actorDefinitions"
        "scenes.bundle.v1" = "sceneDefinitions"
        "statuses.catalog.v1" = "statusDefinitions"
        "combat.damage.v1" = "damageDefinitions"
        "combat.targeting.v1" = "targetingDefinitions"
        "combat.projectiles.v1" = "projectileDefinitions"
        "combat.aoe.v1" = "aoeDefinitions"
        "movement.profiles.v1" = "movementDefinitions"
        "items.use_profiles.v1" = "itemUseProfiles"
        "patches.vanilla_items.v1" = "vanillaItemPatches"
        "camera.aim_profiles.v1" = "cameraDefinitions"
        "camera.aim_profiles.v2" = "cameraDefinitions"
        "input.bindings.v2" = "inputDefinitions"
        "items.state_machine.v1" = "itemStateDefinitions"
        "render.equipped_models.v1" = "equippedModelDefinitions"
        "hud.widgets.v1" = "hudWidgetDefinitions"
        "hud.reticles.v2" = "hudReticleDefinitions"
        "effects.graph.v2" = "effectGraphDefinitions"
        "combat.hit_rules.v2" = "combatHitRuleDefinitions"
        "movement.surf.v2" = "surfDefinitions"
        "actors.tags.v1" = "actorTagDefinitions"
        "world.patchsets.v1" = "worldPatchDefinitions"
        "quests.graph.v1" = "questDefinitions"
        "dialog.nodes.v1" = "dialogDefinitions"
        "sdk.generators.v1" = "sdkGeneratorDefinitions"
    }
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$resolvedModPath = if ([string]::IsNullOrWhiteSpace($ModPath)) {
    Join-Path $resolvedRoot "x64/Release/mods"
} else {
    if ([System.IO.Path]::IsPathRooted($ModPath)) { $ModPath } else { Join-Path $resolvedRoot $ModPath }
}

if (-not (Test-Path $resolvedModPath)) {
    Fail "ModPath not found: $resolvedModPath"
}

$mods = @()
if (Test-Path (Join-Path $resolvedModPath "mod.json")) {
    $mods = @((Resolve-Path $resolvedModPath).Path)
} else {
    $mods = @(Get-ChildItem -Path $resolvedModPath -Directory | Select-Object -ExpandProperty FullName)
}

if ($mods.Count -eq 0) {
    Fail "No mod directories found in: $resolvedModPath"
}

$capabilityPathMap = Resolve-CapabilityPathMap
$failed = 0

foreach ($modDir in $mods) {
    $manifestPath = Join-Path $modDir "mod.json"
    if (-not (Test-Path $manifestPath)) {
        Write-Warning "Skipping $modDir (missing mod.json)"
        continue
    }

    try {
        $manifest = Get-Content -Path $manifestPath -Raw | ConvertFrom-Json
    } catch {
        Write-Error "[validate_mod] $manifestPath parse error: $($_.Exception.Message)"
        $failed++
        continue
    }

    $modId = if ($manifest.id) { "$($manifest.id)" } else { Split-Path -Path $modDir -Leaf }
    if ($manifest.apiVersion -ne 4) {
        Write-Error "[validate_mod] $modId apiVersion must be 4 (found: $($manifest.apiVersion))"
        $failed++
        continue
    }

    $hasRuntime = ($manifest.PSObject.Properties.Name -contains "runtime") -and ($null -ne $manifest.runtime)
    $runtimeType = ""
    if ($hasRuntime -and ($manifest.runtime.PSObject.Properties.Name -contains "type")) {
        $runtimeType = "$($manifest.runtime.type)"
    }
    if (-not $hasRuntime -or $runtimeType -ne "wasm3-v1") {
        Write-Error "[validate_mod] $modId runtime.type must be wasm3-v1"
        $failed++
        continue
    }

    $entryScript = ""
    if ($manifest.PSObject.Properties.Name -contains "entryScript") {
        $entryScript = "$($manifest.entryScript)"
    }
    if ([string]::IsNullOrWhiteSpace($entryScript)) {
        Write-Error "[validate_mod] $modId missing entryScript"
        $failed++
        continue
    }

    $entryScriptPath = Join-Path $modDir $entryScript
    if (-not (Test-Path $entryScriptPath)) {
        Write-Error "[validate_mod] $modId missing entryScript file: $entryScript"
        $failed++
        continue
    }

    try {
        $entryJson = Get-Content -Path $entryScriptPath -Raw | ConvertFrom-Json
    } catch {
        Write-Error "[validate_mod] $modId invalid json in entryScript ${entryScript}: $($_.Exception.Message)"
        $failed++
        continue
    }

    if (-not ($entryJson.PSObject.Properties.Name -contains "apiVersion")) {
        Write-Error "[validate_mod] $modId entryScript missing apiVersion"
        $failed++
        continue
    }
    if ($entryJson.apiVersion -ne $manifest.apiVersion) {
        Write-Error "[validate_mod] $modId entryScript apiVersion mismatch (manifest=$($manifest.apiVersion), script=$($entryJson.apiVersion))"
        $failed++
        continue
    }

    $caps = @()
    $capabilitiesValue = $null
    if ($manifest.PSObject.Properties.Name -contains "capabilities") {
        $capabilitiesValue = $manifest.capabilities
    }
    if ($capabilitiesValue -is [System.Collections.IEnumerable]) {
        foreach ($cap in $capabilitiesValue) {
            if ($null -ne $cap -and -not [string]::IsNullOrWhiteSpace("$cap")) {
                $caps += "$cap".ToLowerInvariant()
            }
        }
    }

    foreach ($cap in $caps) {
        if (-not $capabilityPathMap.ContainsKey($cap)) {
            continue
        }

        $fieldName = $capabilityPathMap[$cap]
        if (-not ($manifest.PSObject.Properties.Name -contains $fieldName)) {
            Write-Error "[validate_mod] $modId capability $cap requires field $fieldName"
            $failed++
            continue
        }

        $relative = "$($manifest.$fieldName)"
        if ([string]::IsNullOrWhiteSpace($relative)) {
            Write-Error "[validate_mod] $modId field $fieldName is empty"
            $failed++
            continue
        }

        $absolute = Join-Path $modDir $relative
        if (-not (Test-Path $absolute)) {
            Write-Error "[validate_mod] $modId missing file for ${fieldName}: $relative"
            $failed++
            continue
        }

        if ($absolute.ToLowerInvariant().EndsWith(".json")) {
            try {
                $null = Get-Content -Path $absolute -Raw | ConvertFrom-Json
            } catch {
                Write-Error "[validate_mod] $modId invalid json in ${relative}: $($_.Exception.Message)"
                $failed++
                continue
            }
        }

        if ($VerboseOutput) {
            Write-Host "[validate_mod] $modId OK -> $cap ($relative)"
        }
    }
}

if ($failed -gt 0) {
    Write-Error "validate_mod failed with $failed error(s)"
    exit 1
}

Write-Host "validate_mod passed ($($mods.Count) mod path(s))"
exit 0

[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$docsDir = Join-Path $resolvedRoot "docs"

if (-not (Test-Path $docsDir)) {
    throw "docs directory not found: $docsDir"
}

$generatedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")

$catalogs = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 3
    runtime = @{
        type = "wasm3-v1"
        budgetDefaults = @{
            maxCallMs = 2
            maxFrameBudgetMs = 2
            maxHookCallsPerFrame = 256
            maxActorInstances = 64
            maxActiveStatuses = 256
        }
    }
    capabilities = @(
        @{ id = "hooks.extended.v1"; fileField = "hookDefinitions"; fileDefault = "hooks/hooks.json" },
        @{ id = "actors.vm.v1"; fileField = "actorDefinitions"; fileDefault = "actors/actors.json" },
        @{ id = "actors.generic.v1"; fileField = "actorDefinitions"; fileDefault = "actors/actors.json" },
        @{ id = "behaviors.graph.v1"; fileField = "behaviorDefinitions"; fileDefault = "behaviors/behaviors.json" },
        @{ id = "scenes.bundle.v1"; fileField = "sceneDefinitions"; fileDefault = "scenes/scenes.json" },
        @{ id = "statuses.catalog.v1"; fileField = "statusDefinitions"; fileDefault = "statuses/statuses.json"; schemaVersion = 1 },
        @{ id = "combat.damage.v1"; fileField = "damageDefinitions"; fileDefault = "combat/damage_profiles.json"; schemaVersion = 1 },
        @{ id = "combat.targeting.v1"; fileField = "targetingDefinitions"; fileDefault = "combat/targeting_profiles.json"; schemaVersion = 1 },
        @{ id = "combat.projectiles.v1"; fileField = "projectileDefinitions"; fileDefault = "combat/projectiles.json"; schemaVersion = 1 },
        @{ id = "combat.aoe.v1"; fileField = "aoeDefinitions"; fileDefault = "combat/aoe_profiles.json"; schemaVersion = 1 },
        @{ id = "movement.profiles.v1"; fileField = "movementDefinitions"; fileDefault = "movement/movement_profiles.json"; schemaVersion = 1 },
        @{ id = "items.use_profiles.v1"; fileField = "itemUseProfiles"; fileDefault = "items/use_profiles.json"; schemaVersion = 1 },
        @{ id = "patches.vanilla_items.v1"; fileField = "vanillaItemPatches"; fileDefault = "patches/vanilla_items.patch.json" },
        @{ id = "world.queries.v1"; fileField = ""; fileDefault = ""; note = "No file required; enables world query actions." }
    )
}

$actions = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 3
    actions = @(
        @{ name = "showNotification"; category = "core" },
        @{ name = "teleportToEntrance"; category = "core" },
        @{ name = "loadModScene"; category = "scene" },
        @{ name = "pressButton"; category = "core" },
        @{ name = "showEquippedItemGet"; category = "core" },
        @{ name = "spawnSmoke"; category = "fx" },
        @{ name = "spawnKusa"; category = "fx" },
        @{ name = "lanternLight"; category = "fx" },
        @{ name = "applyStatus"; category = "status" },
        @{ name = "clearStatus"; category = "status" },
        @{ name = "clearAllStatuses"; category = "status" },
        @{ name = "useItemProfile"; category = "items" },
        @{ name = "dealDamage"; category = "combat" },
        @{ name = "spawnProjectile"; category = "combat" },
        @{ name = "spawnAoE"; category = "combat" },
        @{ name = "applyMovementProfile"; category = "movement" },
        @{ name = "applyImpulse"; category = "movement" },
        @{ name = "getGroundInfo"; category = "world_query" },
        @{ name = "raycast"; category = "world_query" },
        @{ name = "raycastAll"; category = "world_query" },
        @{ name = "spawnActor"; category = "actor" },
        @{ name = "despawnActor"; category = "actor" },
        @{ name = "setActorState"; category = "actor" },
        @{ name = "moveActorToPathNode"; category = "actor" },
        @{ name = "openDialog"; category = "dialog" },
        @{ name = "setSwitchFlag"; category = "flags" },
        @{ name = "clearSwitchFlag"; category = "flags" },
        @{ name = "setEventChkInf"; category = "flags" },
        @{ name = "clearEventChkInf"; category = "flags" },
        @{ name = "setInfTable"; category = "flags" },
        @{ name = "clearInfTable"; category = "flags" },
        @{ name = "giveRupees"; category = "economy" },
        @{ name = "takeRupees"; category = "economy" },
        @{ name = "grantModItem"; category = "items" },
        @{ name = "revokeModItem"; category = "items" },
        @{ name = "setVar"; category = "behavior" },
        @{ name = "addVar"; category = "behavior" },
        @{ name = "clampVar"; category = "behavior" },
        @{ name = "emitSignal"; category = "behavior" },
        @{ name = "callBehavior"; category = "behavior" },
        @{ name = "invokeWasm"; category = "wasm" }
    )
    removedInApiV3 = @(
        "igniteFrontTarget",
        "freezeFrontTarget",
        "items.params.freezeOnMeleeHit",
        "items.params.freezeOnHitDuration",
        "items.params.freezeOnHitShake",
        "items.params.freezeOnHitIntensity"
    )
    wasmHostImports = @(
        "host_useItemProfile",
        "host_resolveTarget",
        "host_dealDamage",
        "host_applyStatus",
        "host_spawnProjectile",
        "host_spawnAoE",
        "host_applyMovementProfile",
        "host_applyImpulse",
        "host_getGroundInfo",
        "host_raycast",
        "host_raycastAll"
    )
    disableReasonCodes = @(
        "BUDGET_HOOK",
        "BUDGET_WASM_CALL",
        "BUDGET_WASM_FRAME",
        "WASM_IMPORT_FAIL",
        "WASM_EXPORT_FAIL",
        "WASM_RUNTIME_ERROR"
    )
}

$events = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 3
    hooks = @(
        "onLoadGame",
        "onExitGame",
        "onSceneInit",
        "afterSceneCommands",
        "onTransitionEnd",
        "onFlagSet",
        "onFlagUnset",
        "onSceneFlagSet",
        "onSceneFlagUnset",
        "onPlayerUpdate",
        "onPlayerUseItem",
        "onPlayerHealthChange",
        "onItemReceive",
        "onActorInit",
        "onActorSpawn",
        "onActorUpdate",
        "onActorKill",
        "onActorDestroy",
        "onEnemyDefeat",
        "onBossDefeat",
        "onPlayDestroy",
        "onGameFrameUpdate"
    )
    behaviorEvents = @(
        "manual",
        "onInit",
        "onSpawn",
        "onDespawn",
        "onDestroy",
        "onUpdate",
        "onRandomTick",
        "onRoomEnter",
        "onTimeOfDayChanged",
        "onSwitchFlagChanged",
        "onPlayerNear",
        "onPlayerFar",
        "onTimer",
        "onInteract",
        "onSignal",
        "onSceneEnter",
        "onItemUsed",
        "onItemGranted",
        "onItemEquipped",
        "onCooldownReady"
    )
    aliases = @(
        @{ from = "oninit"; to = "onspawn" },
        @{ from = "ondespawn"; to = "ondestroy" },
        @{ from = "onitemequipped"; to = "onitemgranted" }
    )
}

$outputs = @(
    @{ path = (Join-Path $docsDir "catalogs.json"); data = $catalogs },
    @{ path = (Join-Path $docsDir "actions.json"); data = $actions },
    @{ path = (Join-Path $docsDir "events.json"); data = $events }
)

foreach ($out in $outputs) {
    $json = $out.data | ConvertTo-Json -Depth 12
    if ($DryRun) {
        Write-Host "[DryRun] Would write $($out.path)"
        continue
    }
    Set-Content -Path $out.path -Value $json -Encoding UTF8
    Write-Host "Wrote $($out.path)"
}

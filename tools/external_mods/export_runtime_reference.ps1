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
    apiVersion = 4
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
        @{ id = "camera.aim_profiles.v1"; fileField = "cameraDefinitions"; fileDefault = "camera/camera_profiles.json"; schemaVersion = 1 },
        @{ id = "items.use_profiles.v1"; fileField = "itemUseProfiles"; fileDefault = "items/use_profiles.json"; schemaVersion = 1 },
        @{ id = "patches.vanilla_items.v1"; fileField = "vanillaItemPatches"; fileDefault = "patches/vanilla_items.patch.json" },
        @{ id = "world.queries.v1"; fileField = ""; fileDefault = ""; note = "No file required; enables world query actions." },
        @{ id = "input.bindings.v2"; fileField = "inputDefinitions"; fileDefault = "config/input.json"; schemaVersion = 1 },
        @{ id = "items.state_machine.v1"; fileField = "itemStateDefinitions"; fileDefault = "items/item_states.json"; schemaVersion = 1 },
        @{ id = "render.equipped_models.v1"; fileField = "equippedModelDefinitions"; fileDefault = "render/equipped_models.json"; schemaVersion = 1 },
        @{ id = "hud.widgets.v1"; fileField = "hudWidgetDefinitions"; fileDefault = "ui/widgets.json"; schemaVersion = 1 },
        @{ id = "hud.reticles.v2"; fileField = "hudReticleDefinitions"; fileDefault = "ui/reticles.json"; schemaVersion = 1 },
        @{ id = "camera.aim_profiles.v2"; fileField = "cameraDefinitions"; fileDefault = "camera/camera_profiles.json"; schemaVersion = 1 },
        @{ id = "effects.graph.v2"; fileField = "effectGraphDefinitions"; fileDefault = "effects/effect_graphs.json"; schemaVersion = 1 },
        @{ id = "combat.hit_rules.v2"; fileField = "combatHitRuleDefinitions"; fileDefault = "combat/hit_rules.json"; schemaVersion = 1 },
        @{ id = "movement.surf.v2"; fileField = "surfDefinitions"; fileDefault = "movement/surf_profiles.json"; schemaVersion = 1 },
        @{ id = "actors.tags.v1"; fileField = "actorTagDefinitions"; fileDefault = "actors/actor_tags.json"; schemaVersion = 1 },
        @{ id = "world.patchsets.v1"; fileField = "worldPatchDefinitions"; fileDefault = "world/patchsets.json"; schemaVersion = 1 },
        @{ id = "quests.graph.v1"; fileField = "questDefinitions"; fileDefault = "quests/quests.json"; schemaVersion = 1 },
        @{ id = "dialog.nodes.v1"; fileField = "dialogDefinitions"; fileDefault = "dialog/dialogs.json"; schemaVersion = 1 },
        @{ id = "sdk.generators.v1"; fileField = "sdkGeneratorDefinitions"; fileDefault = "sdk/generators.json"; schemaVersion = 1 }
    )
}

$actions = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
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
        @{ name = "toggleAimCameraMode"; category = "camera" },
        @{ name = "setAimCameraMode"; category = "camera" },
        @{ name = "setAimCameraProfile"; category = "camera" },
        @{ name = "invokeWasm"; category = "wasm" }
    )
    removedInApiV4 = @(
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
    apiVersion = 4
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

$actionsRegistry = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    description = "Declarative registry for action validation/dispatch contracts."
    actions = @(
        @{ name = "applyStatus"; params = @("status", "target?", "actorHandle?", "durationFrames?") },
        @{ name = "useItemProfile"; params = @("profile|useProfile|profileId") },
        @{ name = "dealDamage"; params = @("profile|damageProfileId", "target?", "actorHandle?") },
        @{ name = "spawnProjectile"; params = @("projectile|profile|projectileProfileId") },
        @{ name = "spawnAoE"; params = @("aoe|profile|aoeProfileId") },
        @{ name = "applyMovementProfile"; params = @("movement|profile|movementProfileId", "durationFrames?") },
        @{ name = "applyImpulse"; params = @("mode?", "strength?") },
        @{ name = "getGroundInfo"; params = @() },
        @{ name = "raycast"; params = @("range?") },
        @{ name = "raycastAll"; params = @("range?") },
        @{ name = "toggleAimCameraMode"; params = @("profileId?", "itemId?") },
        @{ name = "setAimCameraMode"; params = @("mode", "profileId?", "itemId?") },
        @{ name = "setAimCameraProfile"; params = @("profileId") },
        @{ name = "invokeWasm"; params = @("export", "args?") }
    )
}

$conditionsRegistry = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    description = "Declarative registry for behavior condition contracts."
    conditions = @(
        @{ name = "isChild"; fields = @() },
        @{ name = "isAdult"; fields = @() },
        @{ name = "isDay"; fields = @() },
        @{ name = "isNight"; fields = @() },
        @{ name = "randomChance"; fields = @("value|numberValue") },
        @{ name = "hasItem"; fields = @("value") },
        @{ name = "hasStatus"; fields = @("scope?", "key|value", "op?") },
        @{ name = "statusRemaining"; fields = @("scope?", "key|value", "op", "value|numberValue") },
        @{ name = "distanceToPlayer"; fields = @("op?", "numberValue") },
        @{ name = "sceneIs"; fields = @("op?", "value|numberValue") },
        @{ name = "roomIs"; fields = @("op?", "value|numberValue") },
        @{ name = "hasSwitchFlag"; fields = @("op?", "value|numberValue") },
        @{ name = "var"; fields = @("scope", "key", "op?", "value") }
    )
}

$outputs = @(
    @{ path = (Join-Path $docsDir "catalogs.json"); data = $catalogs },
    @{ path = (Join-Path $docsDir "actions.json"); data = $actions },
    @{ path = (Join-Path $docsDir "events.json"); data = $events },
    @{ path = (Join-Path $docsDir "runtime_contract/actions.registry.json"); data = $actionsRegistry },
    @{ path = (Join-Path $docsDir "runtime_contract/conditions.registry.json"); data = $conditionsRegistry }
)

foreach ($out in $outputs) {
    $parentDir = Split-Path -Path $out.path -Parent
    if (-not [string]::IsNullOrWhiteSpace($parentDir) -and -not (Test-Path $parentDir)) {
        if (-not $DryRun) {
            New-Item -ItemType Directory -Path $parentDir -Force | Out-Null
        }
    }
    $json = $out.data | ConvertTo-Json -Depth 12
    if ($DryRun) {
        Write-Host "[DryRun] Would write $($out.path)"
        continue
    }
    Set-Content -Path $out.path -Value $json -Encoding UTF8
    Write-Host "Wrote $($out.path)"
}

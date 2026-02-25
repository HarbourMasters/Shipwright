# External Mods Data-Driven Reference

This reference lists the data-driven surface currently available in `ExternalModManager`.

## 1) Capabilities and Files

- `hooks.extended.v1` -> `hookDefinitions`
- `actors.vm.v1` or `actors.generic.v1` -> `actorDefinitions`
- `behaviors.graph.v1` -> `behaviorDefinitions`
- `scenes.bundle.v1` -> `sceneDefinitions`
- `statuses.catalog.v1` -> `statusDefinitions` (`statuses/statuses.json`)
- `combat.damage.v1` -> `damageDefinitions` (`combat/damage_profiles.json`)
- `combat.targeting.v1` -> `targetingDefinitions` (`combat/targeting_profiles.json`)
- `combat.projectiles.v1` -> `projectileDefinitions` (`combat/projectiles.json`)
- `combat.aoe.v1` -> `aoeDefinitions` (`combat/aoe_profiles.json`)
- `movement.profiles.v1` -> `movementDefinitions` (`movement/movement_profiles.json`)
- `camera.aim_profiles.v1` -> `cameraDefinitions` (`camera/camera_profiles.json`)
- `items.use_profiles.v1` -> `itemUseProfiles` (`items/use_profiles.json`)
- `patches.vanilla_items.v1` -> `vanillaItemPatches` (`patches/vanilla_items.patch.json`)
- `world.queries.v1` -> no file required (enables query-oriented actions)

Manifest priority fields:
- `loadPriority` (new, higher wins)
- `loadOrder` (legacy; mapped to `loadPriority = -loadOrder` when `loadPriority` is absent)

## 2) Hook Events

- `onLoadGame`
- `onExitGame`
- `onSceneInit`
- `afterSceneCommands`
- `onTransitionEnd`
- `onFlagSet`
- `onFlagUnset`
- `onSceneFlagSet`
- `onSceneFlagUnset`
- `onPlayerUpdate`
- `onPlayerUseItem`
- `onPlayerHealthChange`
- `onItemReceive`
- `onActorInit`
- `onActorSpawn`
- `onActorUpdate`
- `onActorKill`
- `onActorDestroy`
- `onEnemyDefeat`
- `onBossDefeat`
- `onPlayDestroy`
- `onGameFrameUpdate`

Hook dispatch modes:
- `actions`
- `wasmExport`

## 3) Behavior Events (runtime-resolved)

Common events used by runtime and helpers:
- `manual`
- `onInit`
- `onSpawn`
- `onDespawn`
- `onDestroy`
- `onPlayerNear`
- `onInteract`
- `onSignal`
- `onItemUsed`
- `onItemGranted`
- `onItemEquipped`
- `onCooldownReady`

## 4) Behavior Conditions

- `var`
- `distanceToPlayer`
- `hasItem`
- `isNight`
- `isChild`
- `randomChance`
- `lineOfSight`
- `hasStatus`
- `statusRemaining`

## 5) Actions

Core:
- `showNotification`
- `teleportToEntrance`
- `loadModScene`
- `pressButton`
- `showEquippedItemGet`

FX / utility:
- `spawnSmoke`
- `spawnKusa`
- `lanternLight`

Status/combat data-driven:
- `applyStatus`
- `clearStatus`
- `clearAllStatuses`
- `dealDamage`
- `useItemProfile`
- `spawnProjectile`
- `spawnAoE`
- `applyMovementProfile`
- `applyImpulse`

World queries:
- `getGroundInfo`
- `raycast`
- `raycastAll`

Actors/scenes:
- `spawnActor`
- `despawnActor`
- `setActorState`
- `moveActorToPathNode`
- `openDialog`

Flags/economy:
- `setSwitchFlag`
- `clearSwitchFlag`
- `setEventChkInf`
- `clearEventChkInf`
- `setInfTable`
- `clearInfTable`
- `giveRupees`
- `takeRupees`

Items and graph control:
- `grantModItem`
- `revokeModItem`
- `setVar`
- `addVar`
- `clampVar`
- `emitSignal`
- `callBehavior`
- `invokeWasm`
- `toggleAimCameraMode`
- `setAimCameraMode`
- `setAimCameraProfile`

## 6) Built-in Status IDs

Built-in `core:*` IDs currently resolved by runtime:
- `core:burning`
- `core:freeze`
- `core:stun`
- `core:poison`
- `core:blind`
- `core:speed`
- `core:slow`
- `core:high_jump`
- `core:strength`
- `core:weakness`

## 6.1) Removed in API v3

- `igniteFrontTarget`
- `freezeFrontTarget`
- `items.params.freezeOnMeleeHit`
- `items.params.freezeOnHitDuration`
- `items.params.freezeOnHitShake`
- `items.params.freezeOnHitIntensity`

## 7) Blackboard Keys (runtime-populated)

- `__groundY`
- `__grounded`
- `__raycastHit`
- `__raycastActorId`
- `__raycastHitCount`
- `__raycastActorIds`
- `__switchFlag`
- `__switchValue`
- `__lastSignal`

## 8) Vanilla Item Patches

`patches.vanilla_items.v1` is applied to `items/items.json` before item parsing.

Accepted patch formats:
- Array of RFC6902 ops (`add`, `replace`, `remove`)
- Object with `ops[]` or `operations[]`
- Extended op `merge` (object merge patch at `path`)

## 9) Current Runtime Notes

- Status runtime is data-driven by `statusId` and catalog metadata.
- `raycastAll` now resolves multiple hits in sorted forward-ray order.
- `loadPriority` controls deterministic conflict precedence (higher wins).
- WASM runtime is sandboxed and budgeted, and executes real exports/imports with per-call and per-frame limits.
- Default WASM budgets:
  - `maxCallMs=2`
  - `maxFrameBudgetMs=2`
  - `maxHookCallsPerFrame=256`
  - `maxActorInstances=64`
  - `maxActiveStatuses=256`
- Host imports available to WASM:
  - `host_useItemProfile`
  - `host_resolveTarget`
  - `host_dealDamage`
  - `host_applyStatus`
  - `host_spawnProjectile`
  - `host_spawnAoE`
  - `host_applyMovementProfile`
  - `host_applyImpulse`
  - `host_getGroundInfo`
  - `host_raycast`
  - `host_raycastAll`

Item and damage extensions:
- `items/items.json` supports optional `aimReticleTextureAsset` (`.png`, decoded as 64x64 and converted to I8).
- `items/items.json` supports optional `aimSelectToggle` (`bool`, default `false`) to enable slot-select aim toggle flow for that item.
- `items/items.json` supports optional `aimAttackButtonFire` (`bool`, default `false`) to allow BTN_B as additional fire input while aim-select is active.
- `items/items.json` supports optional `useTrigger`:
  - `onUse` (default)
  - `hammerGroundImpact` (requires `slot: SLOT_HAMMER` + `useProfile`)
- `items/use_profiles.json` supports effect action `spawnShockwave` with:
  - `origin` (`player|impact`)
  - `primColor` / `envColor` RGBA arrays
  - `life` (`1..120`)
  - `spawnIceSmoke` (`bool`)
- `combat/aoe_profiles.json` supports optional `targetScope`:
  - `all_non_player` (default)
  - `enemies_bosses`
  - `enemies_bosses_props`
- `combat/damage_profiles.json` supports optional `propInteraction`:
  - `none`
  - `vanilla_hit`
  - `hard_kill`
- Current prop fallback scope is intentionally limited to:
  - `ACTOR_EN_KUSA` (bushes)
  - `ACTOR_EN_WOOD02` (trees)

## 10) Aim Camera and Default Keyboard Keys

Aim camera profile catalog (`camera.aim_profiles.v1`) controls first-person vs OTS camera behavior for:

- `cup`
- `bow`
- `hookshot`
- `slingshot`

Input bindings can define keyboard defaults:

```json
{
  "id": "toggle_aim_camera",
  "defaultMask": "MOD_ACTION7",
  "defaultKeyboardKeys": ["F8"]
}
```

Rules:

- auto keyboard mapping only applies if `defaultMask` has exactly one `MOD_ACTIONx` bit
- reserved keys (`F1`, `F5`, `F6`, `F7`, `F9`, `TAB`, `I`) are skipped
- existing mappings are deduped

## 10) Aim OTS v3 Hotkeys and Mouse Fire

Direct hotkeys (no MOD_ACTION bridge):

```json
{
  "bindings": [],
  "cameraHotkeys": [
    {
      "id": "aim_toggle",
      "defaultKeyboardKeys": ["F8"],
      "allowUserRemap": true,
      "action": "toggleAimCameraMode",
      "profileId": "com.example.aim_ots_toggle_demo:ots_default"
    }
  ]
}
```

Camera profile additions:
- `contexts` now accepts `boomerang`
- `mouseFireEnabled` (`false` by default)
- `mouseFireButton` (`left|right|middle|backward|forward`, default `left`)
- `mouseFireMode` (`both|firstPerson|overShoulder`, default `both`)

Mouse fire is resolved from held item action (bow/hookshot/slingshot/boomerang and compatible mod items) and injected through the same vanilla item-button flow.

Aim select toggle behavior (item opt-in):
- first slot press equips item normally and enables OTS aim
- second press on the same slot deactivates aim and triggers put-away
- BTN_B fire (if enabled by item) is only active while aim-select state is active for that item

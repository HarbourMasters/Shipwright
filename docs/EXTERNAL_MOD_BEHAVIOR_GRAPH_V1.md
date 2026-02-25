# External Mods v3: Behaviors, Actors, Scenes, Items

Este guia resume o runtime atual para mods data-driven com `apiVersion: 3`.

## Capabilities novas (opcionais)

No `mod.json`:

```json
{
  "capabilities": [
    "behaviors.graph.v1",
    "actors.generic.v1",
    "items.catalog.v1",
    "items.use_profiles.v1",
    "statuses.catalog.v1",
    "combat.damage.v1",
    "combat.targeting.v1",
    "combat.projectiles.v1",
    "combat.aoe.v1",
    "movement.profiles.v1",
    "camera.aim_profiles.v1",
    "world.queries.v1",
    "patches.vanilla_items.v1",
    "scenes.bundle.v1",
    "render.filter_override.v1"
  ]
}
```

## Arquivos opcionais de conteúdo

- `items/items.json`
- `items/use_profiles.json`
- `statuses/statuses.json`
- `combat/damage_profiles.json`
- `combat/targeting_profiles.json`
- `combat/projectiles.json`
- `combat/aoe_profiles.json`
- `movement/movement_profiles.json`
- `camera/camera_profiles.json`
- `actors/actors.json`
- `behaviors/behaviors.json`
- `scenes/scenes.json`

## modelTextureFilter explícito (sem borrado)

Você pode forçar no nível do item:

```json
{
  "modelTextureFilter": "point"
}
```

Também aceita:

- `"bilerp"`
- `"bilinear"` (alias de `bilerp`)
- `"auto"` (compatibilidade)

## Eventos de behavior suportados

- `onSpawn` (`onInit` como alias)
- `onDestroy` (`onDespawn` como alias)
- `onSceneEnter`
- `onRoomEnter`
- `onUpdate`
- `onRandomTick`
- `onPlayerNear`
- `onPlayerFar`
- `onInteract`
- `onTimer`
- `onSignal`
- `onTimeOfDayChanged`
- `onSwitchFlagChanged`
- `manual` (via action `callBehavior`)
- `onItemGranted`
- `onItemEquipped`
- `onItemUsed`
- `onCooldownReady`

## Condições de behavior suportadas

- `isChild`
- `isAdult`
- `isDay`
- `isNight`
- `randomChance`
- `hasItem` (item custom do mod)
- `distanceToPlayer`
- `sceneIs`
- `roomIs`
- `hasSwitchFlag` / `switchIsOn`
- `var` / `varEquals` / `varCompare`
- `hasStatus`
- `statusRemaining`

## Actions suportadas (resumo)

- UI/fluxo: `showNotification`, `openDialog`, `loadModScene`, `teleportToEntrance`
- input/sinais: `pressButton`, `emitSignal`, `callBehavior`, `invokeWasm`
- item: `grantModItem`, `revokeModItem`, `showEquippedItemGet`, `useItemProfile`
- combate por catalogo: `dealDamage`, `spawnProjectile`, `spawnAoE`
- movimento por catalogo: `applyMovementProfile`, `applyImpulse`
- queries de mundo: `getGroundInfo`, `raycast`, `raycastAll`
- status: `applyStatus`, `clearStatus`, `clearAllStatuses`
- atores: `spawnActor`, `despawnActor`, `setActorState`, `moveActorToPathNode`
- camera de mira: `toggleAimCameraMode`, `setAimCameraMode`, `setAimCameraProfile`
- estado/flags/economia:
  - `setSwitchFlag`, `clearSwitchFlag`
  - `setEventChkInf`, `clearEventChkInf`
  - `setInfTable`, `clearInfTable`
  - `giveRupees`, `takeRupees`
  - `setVar`, `addVar`, `clampVar`

Contrato v3:

- `igniteFrontTarget` e `freezeFrontTarget` foram removidos.
- Use sempre `applyStatus` com `status` (`core:*` ou `modid:*`).

Obs:

- `applyStatus.status` aceita IDs namespaced (`modid:status_id`) e `core:*`.
- `dealDamage` usa `damageProfileId` quando informado.
- `useItemProfile` executa pipeline de targeting + efeitos + cooldown por ID.

## movement.profiles v3 (`mode: surf`)

Para surf físico real (escudo sob os pés, inclusive parado), use:

- `mode: "surf"`
- `boardRequired` (default `true`)
- `boardSpawnMode` (`persistent_under_player`)
- `idlePose` (`stand|tpose`, default `stand`)
- `idleLock` (default `true`)
- `boardHeightOffset`, `boardPitchRollFromGround`, `boardVisibleWhenIdle`
- `boardModelAsset`, `boardScale`
- `surfMaxSpeed`, `surfDownhillAccel`, `surfUphillBrake`, `surfFlatDrag`, `surfTurnRateDeg`, `idleSpeedThreshold`

## camera.aim_profiles v1

Manifest:

- capability: `camera.aim_profiles.v1`
- path: `cameraDefinitions` (ex.: `camera/camera_profiles.json`)

Actions:

- `toggleAimCameraMode` (opcionais: `profileId`, `itemId`)
- `setAimCameraMode` (obrigatÃ³rio: `mode=firstPerson|overShoulder`; opcionais: `profileId`, `itemId`)
- `setAimCameraProfile` (obrigatÃ³rio: `profileId`)

Exemplo de perfil:

```json
{
  "schemaVersion": 1,
  "profiles": [
    {
      "id": "com.example.demo:ots_default",
      "contexts": ["cup", "bow", "hookshot", "slingshot"],
      "firstPersonModeByContext": {
        "cup": "firstperson",
        "bow": "bowarrow",
        "hookshot": "hookshot",
        "slingshot": "slingshot"
      },
      "overShoulderModeByContext": "bowarrowz",
      "shoulder": "right",
      "aimRay": "camera_center",
      "reticle": { "x": 0.5, "y": 0.5 }
    }
  ]
}
```

## input.json (`defaultKeyboardKeys`)

Cada binding pode declarar teclas padrÃ£o de teclado:

```json
{
  "id": "toggle_aim_camera",
  "defaultMask": "MOD_ACTION7",
  "defaultKeyboardKeys": ["F8"]
}
```

Regras:

- `defaultMask` deve conter exatamente 1 bit `MOD_ACTIONx` para auto-mapear teclado.
- teclas reservadas (`F1`, `F5`, `F6`, `F7`, `F9`, `TAB`, `I`) sÃ£o ignoradas com warning.
- mappings duplicados nÃ£o sÃ£o recriados.

## applyStatus (novo)

Exemplo completo:

```json
{
  "action": "applyStatus",
  "status": "freeze",
  "target": "frontTarget",
  "itemId": "freeze_staff_item",
  "durationFrames": 90,
  "shakeFrames": 12,
  "range": 180,
  "intensity": 255
}
```

Campos suportados:

- `status`: `core:*` ou `modid:status_id` (aceita built-ins e status custom)
- `target`: `frontTarget|self|player|actorHandle`
- `actorHandle`: obrigatÃ³rio quando `target=actorHandle`
- `itemId`/`requiresItemId`: gate opcional por item do mod
- `durationFrames`, `tickFrames`, `damagePerTick`, `range`, `intensity`, `shakeFrames`
- `speedMultiplier`, `jumpMultiplier`, `strengthMultiplier`, `weaknessMultiplier`
- `blindSkipChance`, `blindYawJitterDeg`

## clearStatus / clearAllStatuses

Exemplos:

```json
{ "action": "clearStatus", "status": "freeze", "target": "self" }
```

```json
{ "action": "clearAllStatuses", "target": "self" }
```

## Exemplo mínimo (mod.json)

```json
{
  "id": "com.example.behavior_demo",
  "name": "Behavior Demo",
  "version": "0.1.0",
  "apiVersion": 3,
  "entryScript": "scripts/init.json",
  "runtime": {
    "type": "wasm3-v1",
    "module": "scripts/noop.wat"
  },
  "itemDefinitions": "items/items.json",
  "actorDefinitions": "actors/actors.json",
  "behaviorDefinitions": "behaviors/behaviors.json",
  "sceneDefinitions": "scenes/scenes.json",
  "statusDefinitions": "statuses/statuses.json",
  "damageDefinitions": "combat/damage_profiles.json",
  "targetingDefinitions": "combat/targeting_profiles.json",
  "itemUseProfiles": "items/use_profiles.json",
  "capabilities": [
    "behaviors.graph.v1",
    "actors.generic.v1",
    "items.catalog.v1",
    "items.use_profiles.v1",
    "statuses.catalog.v1",
    "combat.damage.v1",
    "combat.targeting.v1",
    "scenes.bundle.v1",
    "render.filter_override.v1"
  ]
}
```

## Exemplo mínimo (behaviors/behaviors.json)

```json
{
  "behaviors": [
    {
      "id": "npc_guard",
      "events": {
        "onSpawn": [
          { "action": "setVar", "scope": "actor", "key": "mood", "value": "idle" }
        ],
        "onInteract": [
          {
            "conditions": [
              { "condition": "isNight" }
            ],
            "actions": [
              { "action": "openDialog", "dialogId": 1234 },
              { "action": "giveRupees", "amount": 5 }
            ]
          },
          {
            "actions": [
              { "action": "openDialog", "dialogId": 1235 }
            ]
          }
        ],
        "onSwitchFlagChanged": [
          { "action": "showNotification", "text": "Switch mudou" }
        ]
      }
    }
  ]
}
```

## Exemplo mínimo (actors/actors.json)

```json
{
  "actors": [
    {
      "id": "guard_01",
      "archetype": "npc",
      "scene": "SCENE_KOKIRI_FOREST",
      "position": [0, 0, 0],
      "rotation": [0, 0, 0],
      "components": ["Transform", "Talker", "Interactable"],
      "behavior": {
        "id": "npc_guard",
        "interactable": true,
        "interactDistance": 120
      }
    }
  ]
}
```

## Exemplo mínimo (scenes/scenes.json)

```json
{
  "scenes": [
    {
      "id": "demo_hub",
      "entrance": "ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE"
    }
  ]
}
```

## cameraHotkeys (v3)

You can bind aim camera actions directly to keyboard keys without `MOD_ACTION`:

```json
{
  "bindings": [],
  "cameraHotkeys": [
    {
      "id": "aim_toggle",
      "defaultKeyboardKeys": ["F8"],
      "allowUserRemap": true,
      "action": "toggleAimCameraMode",
      "profileId": "com.example.demo:ots_default"
    }
  ]
}
```

Supported `cameraHotkeys.action` values:
- `toggleAimCameraMode`
- `setAimCameraMode`
- `setAimCameraProfile`

`camera/camera_profiles.json` also supports:
- `contexts` with `boomerang`
- `mouseFireEnabled`
- `mouseFireButton`
- `mouseFireMode`

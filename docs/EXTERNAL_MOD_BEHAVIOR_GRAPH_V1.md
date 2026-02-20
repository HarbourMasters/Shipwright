# External Mods v2: Behaviors, Actors, Scenes, Items

Este guia resume o runtime atual para mods data-driven com `apiVersion: 2`.

## Capabilities novas (opcionais)

No `mod.json`:

```json
{
  "capabilities": [
    "behaviors.graph.v1",
    "actors.generic.v1",
    "items.catalog.v1",
    "scenes.bundle.v1",
    "render.filter_override.v1"
  ]
}
```

## Arquivos opcionais de conteúdo

- `items/items.json`
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

## Actions suportadas (resumo)

- UI/fluxo: `showNotification`, `openDialog`, `loadModScene`, `teleportToEntrance`
- input/sinais: `pressButton`, `emitSignal`, `callBehavior`, `invokeWasm`
- item: `grantModItem`, `revokeModItem`, `showEquippedItemGet`
- atores: `spawnActor`, `despawnActor`, `setActorState`, `moveActorToPathNode`
- estado/flags/economia:
  - `setSwitchFlag`, `clearSwitchFlag`
  - `setEventChkInf`, `clearEventChkInf`
  - `setInfTable`, `clearInfTable`
  - `giveRupees`, `takeRupees`
  - `setVar`, `addVar`, `clampVar`

## Exemplo mínimo (mod.json)

```json
{
  "id": "com.example.behavior_demo",
  "name": "Behavior Demo",
  "version": "0.1.0",
  "apiVersion": 2,
  "entryScript": "scripts/init.json",
  "runtime": {
    "type": "wasm3-v1",
    "module": "scripts/noop.wat"
  },
  "itemDefinitions": "items/items.json",
  "actorDefinitions": "actors/actors.json",
  "behaviorDefinitions": "behaviors/behaviors.json",
  "sceneDefinitions": "scenes/scenes.json",
  "capabilities": [
    "behaviors.graph.v1",
    "actors.generic.v1",
    "items.catalog.v1",
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


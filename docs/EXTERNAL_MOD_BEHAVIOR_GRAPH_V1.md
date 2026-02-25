# External Mods Behavior Graph v1 (API v3 Runtime)

This reference describes the behavior graph surface currently used by the Sylian Foundry Modloader runtime.

> Contract baseline: `apiVersion: 3`

## File and capability

- Capability: `behaviors.graph.v1`
- Manifest field: `behaviorDefinitions`
- Default file path: `behaviors/behaviors.json`

## Supported behavior events

From `docs/events.json` (`behaviorEvents`):

- `manual`
- `onInit` / `onSpawn`
- `onDespawn` / `onDestroy`
- `onUpdate`
- `onRandomTick`
- `onRoomEnter`
- `onSceneEnter`
- `onTimeOfDayChanged`
- `onSwitchFlagChanged`
- `onPlayerNear`
- `onPlayerFar`
- `onTimer`
- `onInteract`
- `onSignal`
- `onItemUsed`
- `onItemGranted`
- `onItemEquipped`
- `onCooldownReady`

## Conditions (common)

Common condition types used by runtime:

- variable/value checks
- distance checks
- item/status ownership checks
- random chance checks
- line-of-sight checks

For exact runtime naming, inspect `docs/actions.json` + `docs/events.json` + in-engine parser/runtime code.

## Actions

Behavior steps can call data-driven actions from the runtime action table. Core categories include:

- core/UI
- items/combat/status
- movement/world queries
- actor/scene/dialog/flags/economy
- wasm bridge actions

Use `docs/actions.json` as source of truth for currently exported action names.

## Minimal behavior skeleton

```json
{
  "schemaVersion": 1,
  "behaviors": [
    {
      "id": "com.example.demo:behavior_main",
      "events": [
        {
          "event": "onItemUsed",
          "actions": [
            { "action": "useItemProfile", "itemId": "com.example.demo:pistol_item" }
          ]
        }
      ]
    }
  ]
}
```

## Practical tip (PT-BR)

- Comece pequeno: um evento + uma action; valide log e s? depois encadeie sinais/condi??es.

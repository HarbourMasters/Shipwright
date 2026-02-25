# External Mods Data-Driven Reference (API v4)

Reference for JSON-driven mod content used by the Sylian Foundry Modloader.

> External mod contract baseline: `apiVersion: 4`

## 1) Core rules

- IDs must be namespaced (`modId:*`); `core:*` is reserved for built-ins.
- Declared capabilities must provide valid files/paths when required.
- Unknown/invalid schema versions fail the affected mod capability.

## 2) Manifest essentials (`mod.json`)

Typical fields:

- `id`, `name`, `version`, `apiVersion`
- `loadPriority`
- `capabilities`
- capability file fields (e.g., `damageDefinitions`, `itemUseProfiles`)
- optional runtime budgets (`runtime`)

Machine-readable mapping: `docs/catalogs.json` (`capabilities[]`).

## 3) Key content files

- `items/items.json`
- `items/use_profiles.json`
- `statuses/statuses.json`
- `combat/damage_profiles.json`
- `combat/targeting_profiles.json`
- `combat/projectiles.json`
- `combat/aoe_profiles.json`
- `movement/movement_profiles.json`
- `hooks/hooks.json`
- `behaviors/behaviors.json`

## 4) Item definitions (`items/items.json`)

Common fields include:

- identifiers/display metadata
- `slot` / `placement`
- `useMode`
- `useProfile`
- `overrideVanillaItem`
- `aimReticleTextureAsset`
- `aimSelectToggle`
- `aimAttackButtonFire`
- `useTrigger` (`onUse` or `hammerGroundImpact`)

## 5) Use profiles (`items/use_profiles.json`)

Effects are ordered and data-driven. Common actions include:

- damage/status application
- projectile/AoE spawn
- movement profile application
- impulse/world-query dependent effects
- `spawnShockwave` for impact-style FX

## 6) Combat catalogs

- Damage profiles support amount/type/flags and prop interaction policy.
- Targeting profiles resolve actor targets (front, lock-on, raycast, cone, sphere, self/player).
- Projectile and AoE profiles define hit/tick behavior, including target scope controls for AoE.
- AoE `targetScope` supports:
  - `all_non_player`
  - `enemies_bosses`
  - `enemies_bosses_props`
  - `player_enemies_bosses`
  - `all_with_player`

## 7) Status freeze profile (ice-trap no-damage mode)

`statuses/statuses.json` can extend a freeze status with:

- `freezeProfile.mode`: `legacy_timer | ice_trap_no_damage`
- `spawnIceShell`, `iceShellSize`
- `lockPosition`, `lockRotation`
- `playerInputLock`
- `breakEffectOnExpire`

Built-in reusable preset status id:

- `core:freeze_ice_trap_no_damage`

This mode freezes player/enemy/boss without periodic damage while preserving freeze lock behavior.

## 8) Movement profiles

`movement/movement_profiles.json` supports modifier and surf-oriented behavior with profile-defined parameters (speed/drag/slope/board behavior).

## 9) Hooks and behavior actions

- Hook list: `docs/events.json` (`hooks`)
- Behavior events: `docs/events.json` (`behaviorEvents`)
- Action names: `docs/actions.json`

## 10) Validation workflow

1. Validate schema/capabilities via runtime load.
2. Check logs for contextual parser/runtime errors.
3. Re-export references after contract changes.

```powershell
tools/external_mods/export_runtime_reference.ps1
```

## 11) Example packs

See `docs/examples/external_mods/` for runnable reference mods.


# MOD SDK MVP (Current Snapshot)

This document is intentionally short and tracks the current operational baseline for external mods.

> Runtime contract: `apiVersion: 4`

## Current status

The former MVP implementation has evolved into the **Sylian Foundry Modloader** workflow used by current external mods.

- Canonical entry point: `docs/SYLIAN_FOUNDRY_MODLOADER.md`
- Runtime references: `docs/actions.json`, `docs/events.json`, `docs/catalogs.json`
- Examples: `docs/examples/external_mods/`

## Architecture at a glance

- Manifest + capability-gated file loading
- Data-driven catalogs for items/combat/status/movement
- Behavior graph orchestration
- Optional WASM integration (`wasm3-v1`)
- Per-mod isolation + budget controls

## Supported scope (Phase 1 baseline)

- Data-driven items and use profiles
- Data-driven targeting, damage, status, projectiles, AoE, movement
- Hooks + behavior actions + world queries

## Not part of this baseline

- NPC/quest/story runtime (future phase)

## Historical material

Long historical MVP notes were moved to:

- `docs/archive/MOD_SDK_MVP_LEGACY_HISTORY.md`


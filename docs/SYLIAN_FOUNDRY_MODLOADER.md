# Sylian Foundry Modloader (API v4)

This is the canonical entry point for data-driven external mods in Ship of Harkinian / Hylian Foundry.

> Contract baseline: `apiVersion: 4`

## What it is

The **Sylian Foundry Modloader** loads external mod packages (folder/zip), validates capability-gated JSON contracts, mounts assets, and runs behavior/WASM logic with per-mod isolation.

## Start here (quick path)

1. Read this file first.
2. Pick a demo under `docs/examples/external_mods/`.
3. Confirm your `mod.json` uses `apiVersion: 4`.
4. Implement item/effect logic with catalogs + behaviors.
5. Test in runtime and inspect logs (`x64/Release/logs/Ship of Harkinian.log`).

## Runtime contract baseline

- Required for new mods: `mod.json.apiVersion = 4`
- Runtime type: `wasm3-v1`
- Namespaced IDs required (`modId:*`, with `core:*` reserved for built-ins)
- Capability-gated files: if capability is declared, required file/path must be valid

## Capability and file map

Use `docs/catalogs.json` as machine-readable source of truth. Common capabilities include:

- `hooks.extended.v1` -> `hookDefinitions` (`hooks/hooks.json`)
- `behaviors.graph.v1` -> `behaviorDefinitions` (`behaviors/behaviors.json`)
- `statuses.catalog.v1` -> `statusDefinitions` (`statuses/statuses.json`)
- `combat.damage.v1` -> `damageDefinitions` (`combat/damage_profiles.json`)
- `combat.targeting.v1` -> `targetingDefinitions` (`combat/targeting_profiles.json`)
- `combat.projectiles.v1` -> `projectileDefinitions` (`combat/projectiles.json`)
- `combat.aoe.v1` -> `aoeDefinitions` (`combat/aoe_profiles.json`)
- `movement.profiles.v1` -> `movementDefinitions` (`movement/movement_profiles.json`)
- `items.use_profiles.v1` -> `itemUseProfiles` (`items/use_profiles.json`)
- `patches.vanilla_items.v1` -> `vanillaItemPatches` (`patches/vanilla_items.patch.json`)
- `world.queries.v1` -> no file required (enables world query actions)
- `items.state_machine.v1` -> `itemStateDefinitions` (`items/item_states.json`)
- `render.equipped_models.v1` -> `equippedModelDefinitions` (`render/equipped_models.json`)
- `hud.widgets.v1` -> `hudWidgetDefinitions` (`ui/widgets.json`)
- `hud.reticles.v2` -> `hudReticleDefinitions` (`ui/reticles.json`)
- `camera.aim_profiles.v2` -> `cameraDefinitions` (`camera/camera_profiles.json`)
- `effects.graph.v2` -> `effectGraphDefinitions` (`effects/effect_graphs.json`)
- `combat.hit_rules.v2` -> `combatHitRuleDefinitions` (`combat/hit_rules.json`)
- `movement.surf.v2` -> `surfDefinitions` (`movement/surf_profiles.json`)
- `actors.tags.v1` -> `actorTagDefinitions` (`actors/actor_tags.json`)
- `world.patchsets.v1` -> `worldPatchDefinitions` (`world/patchsets.json`)
- `quests.graph.v1` -> `questDefinitions` (`quests/quests.json`)
- `dialog.nodes.v1` -> `dialogDefinitions` (`dialog/dialogs.json`)
- `sdk.generators.v1` -> `sdkGeneratorDefinitions` (`sdk/generators.json`)

## Runtime lifecycle

1. **Discover** packages in mods folders
2. **Validate** manifest + capability files
3. **Register** catalogs, items, hooks, behaviors
4. **Run** actions/effects/WASM with budgets
5. **Reload/Shutdown** with deterministic cleanup

## Safety and failure model

- Invalid mod data disables only the failing mod.
- Runtime budgets are enforced per mod.
- Logs include context (`modId`, file, JSON path when available).
- Reload clears mod-owned state to avoid orphan behavior.

## Where to extend

- Actions reference: `docs/actions.json`
- Events reference: `docs/events.json`
- Catalog/runtime capabilities: `docs/catalogs.json`
- Declarative action registry: `docs/runtime_contract/actions.registry.json`
- Declarative condition registry: `docs/runtime_contract/conditions.registry.json`

## Detailed references

- `docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md`
- `docs/EXTERNAL_MOD_MANAGER_REFERENCE.md`
- `docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md`
- `docs/examples/external_mods/`

## Notes for contributors

- Keep docs aligned with runtime exports (`tools/external_mods/export_runtime_reference.ps1`).
- Run drift checks before publishing docs updates (`check-doc-drift.ps1`).

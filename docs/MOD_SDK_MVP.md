# External Mod SDK MVP (Zip + OTR + Script)

## Goal
Deliver a minimum viable external mod workflow that allows creators to ship:

- A single `.zip` mod package
- One or more asset archives (`.otr` / `.o2r`)
- Scripted gameplay logic without recompiling Ship of Harkinian

This MVP should be additive and must not break the current content pipeline where custom assets are loaded from the `mods` folder.

## Current Baseline
- Custom assets are currently distributed as `.otr` files placed in `mods`.
- Code mods are currently developed by forking and recompiling the project.
- Internal game hook infrastructure already exists (`GameInteractor`) and can back a script bridge.

## In Scope (MVP)
1. Discover `.zip` packages in the `mods` directory.
2. Parse and validate a `mod.json` manifest.
3. Mount declared archive assets from each mod package.
4. Spin up one script runtime per mod (Lua for MVP).
5. Expose a minimal safe event/action API.
6. Keep fault isolation: one broken mod should not crash the game.

## Out of Scope (MVP)
- Full engine parity for script API
- Binary plugin ABI for third-party native DLL/SO mods
- Networked script sync across multiplayer systems
- Workshop-style mod browser/installer UI
- Guaranteed long-term API compatibility beyond `apiVersion` gate

## Constraints
- Preserve existing `.otr` loading behavior and priority semantics.
- External mod failures must degrade gracefully (disable mod, log error, continue runtime).
- Script API must be permissioned/sandboxed by default.
- API surface must be versioned from day one.

## Proposed Package Layout
```text
mods/
  kokiri_parkour.zip
    mod.json
    assets/
      main.otr
    scripts/
      init.lua
      events.lua
```

## Manifest Contract (`mod.json`) - v1
```json
{
  "id": "com.example.kokiri_parkour",
  "name": "Kokiri Parkour",
  "version": "0.1.0",
  "apiVersion": 1,
  "gameVersionMin": "9.1.2",
  "loadOrder": 100,
  "entryScript": "scripts/init.lua",
  "assets": ["assets/main.otr"],
  "dependencies": []
}
```

## MVP API Surface (v1)
### Events
- `onGameLoaded()`
- `onSceneInit(sceneId)`
- `onFrame()`

### Actions
- `showNotification(text)`
- `teleportToEntrance(entranceId)`
- `getFlag(flagType, id)` / `setFlag(flagType, id, value)` (restricted whitelist)

## Execution Phases
1. **Package Discovery + Validation**
   - `ExternalModManager` discovers zip files and validates manifests.
2. **Asset Mounting**
   - Declared archives are mounted in deterministic load order.
3. **Runtime Boot**
   - One Lua VM per mod, bounded memory/time budgets.
4. **Hook Bridge**
   - Internal hooks emit events into each mod runtime.
5. **Lifecycle**
   - Enable/disable/reload with proper unregistration and cleanup.

## Risks and Mitigations
1. **Crash risk from script errors**
   - Mitigation: catch/disable offending mod and continue.
2. **Performance regressions on frame events**
   - Mitigation: script time budget + instrumentation.
3. **Load-order conflicts for asset overrides**
   - Mitigation: explicit `loadOrder`, deterministic resolver logs.
4. **API drift with engine changes**
   - Mitigation: strict `apiVersion` gate and deprecation policy.

## Acceptance Criteria (Stage 0)
- This document is approved as implementation contract.
- MVP scope and non-goals are explicit.
- Manifest v1 schema fields are defined.
- Initial event/action API is frozen for Stage 1 implementation.

## Stage 1 Delivery Checklist (next implementation step)
- [x] `ExternalModManager` skeleton integrated in startup flow
- [x] Zip scan in `mods/`
- [x] Manifest parser + validation errors surfaced in logs/UI logs
- [ ] Unit test coverage for valid/invalid manifest cases

## Stage 1 Progress Notes
- External package discovery now supports directory mods (`mod.json`) and `.zip` mods (`mod.json` at archive root).
- Manifest validation currently enforces required fields plus `apiVersion == 1`.
- Duplicate mod IDs are rejected during discovery and surfaced in log output.

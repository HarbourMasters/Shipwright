# External Mod SDK MVP (Zip + OTR + Script)

## Goal
Deliver a minimum viable external mod workflow thatellows creates to ship:

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
2. Parse and validatea `mod.json` manifest.
3. Mount declared archive assets from each mod package.
4. Spin up one script runtime per mod (Lua for MVP).
5. Expose a minimal safe event/action API.
6. Keep fault isolaten: one broken mod should not crash the game.

## Out of Scope (MVP)
- Full engine parity for script API
- Binary plugin ABI for third-party natee DLL/SO mods
- Networked script sync across multiplayer systems
- Workshop-style mod browser/installer UI
- Guaranteed long-term API compateility beyond `apiVersion` gate

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
      init.json
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
  "entryScript": "scripts/init.json",
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
- `showNotificaten(text)`
- `teleportToEntrance(entranceId)`
- `getFlag(flagType, id)` / `setFlag(flagType, id, value)` (restricted whitelist)

## Execution Phases
1. **Package Discovery + validacaon**
   - `ExternalModManager` discovers zip files and validate manifests.
2. **Asset Mounting**
   - Declared archives are mounted in deterministic load order.
3. **Runtime Boot**
   - One Lua VM per mod, bounded memory/time budgets.
4. **Hook Bridge**
   - Internal hooks emit events into each mod runtime.
5. **Lifecycle**
   - Enable/disable/reload with proper unregistraten and cleanup.

## Risks and Mitigatens
1. **Crash risk from script errors**
   - Mitigaten: cate/disable offending mod and continue.
2. **Performance regressions on frame events**
   - Mitigaten: script time budget + instrumentaten.
3. **Load-order conflicts for asset overrides**
   - Mitigaten: explicit `loadOrder`, deterministic resolver logs.
4. **API drift with engine changes**
   - Mitigaten: strict `apiVersion` gateand deprecaten policy.

## Acceptance Criteria (Stage 0)
- This document is approved as implementaten contract.
- MVP scope and non-goals are explicit.
- Manifest v1 schema fields are defined.
- Initial event/action API is frozen for Stage 1 implementaten.

## Stage 1 Delivery Checklist (next implementaten step)
- [x] `ExternalModManager` skeleton integrate in startup flow
- [x] Zip scan in `mods/`
- [x] Manifest parser + validacaon errors surfaced in logs/UI logs
- [ ] Unit test coverage for valid/invalid manifest cases

## Stage 1 Progress Notes
- External package discovery now supports directory mods (`mod.json`) and `.zip` mods (`mod.json` aterchive root).
- Manifest validacaon currently enforces required fields plus `apiVersion == 1`.
- Duplicatemod IDs are rejected during discovery and surfaced in log output.

## Stage 2 Delivery Checklist (runtime + assets + hooks)
- [x] Current MVP runtime is declaratee JSON (`entryScript`) to keep API sandboxed; Lua VM remains a follow-up step.
- [x] `ExternalModManager::Initialize()` integrate in startup flow.
- [x] Deterministic package boot by `loadOrder`.
- [x] Asset mount for directory and `.zip` packages using manifest `assets`.
- [x] Safe patevalidacaon (`entryScript`/`assets` are relatee, no traversal, no unrestricted filesystem access).
- [x] Per-mod runtime boot from `entryScript` (JSON script contract v1).
- [x] Hook bridge implemented for `onGameLoaded`, `onSceneInit`, and frame triggers (`onFrameTriggers`).
- [x] Safe action surface for MVP runtime: `showNotificaten`, `teleportToEntrance`.
- [x] Fault isolaten: runtime exceptions disable only the offending mod and continue.

## Stage 2 Script Contract (`entryScript`) - v1 (JSON)
```json
{
  "apiVersion": 1,
  "onGameLoaded": [{ "action": "showNotificaten", "text": "Mod loaded" }],
  "onSceneInit": [
    {
      "scene": "SCENE_KOKIRI_FOREST",
      "actions": [{ "action": "showNotificaten", "text": "Scene init" }]
    }
  ],
  "onFrameTriggers": [
    {
      "id": "example_trigger",
      "scene": "SCENE_KOKIRI_FOREST",
      "bounds": { "min": [-100.0, 0.0, -100.0], "max": [100.0, 200.0, 100.0] },
      "cooldownFrames": 90,
      "actions": [{ "action": "teleportToEntrance", "entrance": "ENTR_LINKS_HOUSE_CHILD_SPAWN" }]
    }
  ]
}
```

Supported `scene`/`entrance` values are integer IDs or whitelisted aliases.

## Kokiri Parkour Example (Step by Step)
Reference files:
- `docs/examples/external_mods/kokiri_parkour/mod.json`
- `docs/examples/external_mods/kokiri_parkour/scripts/init.json`
- `docs/examples/external_mods/kokiri_parkour/assets/README.txt`

Steps:
1. Put your parkour room archive at `docs/examples/external_mods/kokiri_parkour/assets/parkour_room.otr`.
2. Zip the **contents** of `docs/examples/external_mods/kokiri_parkour` into `mods/kokiri_parkour.zip`.
   - Automated command (full package): `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_kokiri_parkour_zip.ps1 -AssetSource "<path-to-your-parkour_room.otr>"`
   - Automated command (hooks only, no external asset): `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_kokiri_parkour_zip.ps1 -LogicOnly`
3. Launch the game and load a save.
4. In Kokiri Forest, walk behind Link's house to trigger teleport into the parkour room.
5. Inside the parkour room, use the configured trigger areas to:
   - return to Kokiri (`ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE`)
   - go to Kakariko (`ENTR_KAKARIKO_VILLAGE_FRONT_GATE`)
6. Tune trigger AABBs in `scripts/init.json` as needed for your geometry.

## Manual Validation Log
- 2026-02-17 18:54:43 -03:00: `scripts/external-mods/smoke_validate_example.ps1` executed successfully.
- 2026-02-17 18:55:10 -03:00: `mods/kokiri_parkour.zip` generated in `-LogicOnly` mode for hook validation without external asset.
- 2026-02-17 18:55:04 -03:00: full package build blocked by missing `assets/parkour_room.otr` (expected and safe failure).
- 2026-02-17 18:55:17 -03:00: logic-only zip content validated with entries `mod.json`, `scripts/init.json`, `assets/README.txt`.
- 2026-02-17 18:55:17 -03:00: full gameplay validation remains pending until `parkour_room.otr` is provided.

## Stage X - Fast64 Orchestrator + Runtime ZIP (2026-02-18)

### Goal
Enable end-to-end authoring and export for the Kokiri backdoor portal mod flow (Kokiri -> Parkour -> Kokiri/Kakariko) using Fast64 toolchain orchestration, with safe runtime behavior and no regression to existing `.otr` mod loading.

### Implemented
- `Scene_Editor` now persists toolchain config in `%APPDATA%/scene_editor/toolchain_config.json`.
- Added Fast64 bridge service with contracts:
  - `validateToolchain()`
  - `exportSceneBundle(sceneKey)`
  - `buildModAssetArchive(sceneKey)`
- Added Fast64 scripts:
  - `scripts/fast64/install_fast64.ps1`
  - `scripts/fast64/export_scene_bundle.py`
- Added Fast64 lock file:
  - `Scene_Editor/fast64.lock.json`
- Asset indexer hardening:
  - Removed generic `.json` from scene detection.
  - `.zip` is only indexed as package when root `mod.json` is valid (`apiVersion=1`, `entryScript` set).
- 3D Studio preview flow hardening:
  - Removed scene-id guess-from-path as primary flow.
  - Preview scene is explicitly selected by scene id.
  - Kept one scene root visible at a time.
- Mod ZIP export runtime layout now includes:
  - `mod.json`
  - `scripts/init.json`
  - `assets/scene_manifest.json`
  - `assets/entrance_map.json`
  - `assets/parkour_room.otr` (required in strict mode)
  - `reports/validation_report.json`
  - `README_MOD.txt`
- Added Kokiri Parkour trigger template generation in export.

### Tests Added
- `Scene_Editor/test/toolchain_config_service_test.dart`
- `Scene_Editor/test/fast64_bridge_service_test.dart`
- Extended:
  - `Scene_Editor/test/asset_indexer_service_test.dart`
  - `Scene_Editor/test/mod_export_service_test.dart`

### Commands Executed
- `dart format Scene_Editor/lib/main.dart Scene_Editor/lib/models/toolchain_config.dart Scene_Editor/lib/services/toolchain_config_service.dart Scene_Editor/lib/services/fast64_bridge_service.dart Scene_Editor/lib/services/asset_indexer_service.dart Scene_Editor/lib/services/mod_export_service.dart Scene_Editor/lib/screens/scene_studio_3d_screen.dart Scene_Editor/test/asset_indexer_service_test.dart Scene_Editor/test/mod_export_service_test.dart Scene_Editor/test/toolchain_config_service_test.dart Scene_Editor/test/fast64_bridge_service_test.dart Scene_Editor/test/widget_test.dart`
- `cd Scene_Editor && flutter analyze`
- `cd Scene_Editor && flutter test`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_example.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_kokiri_parkour_zip.ps1 -LogicOnly`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_kokiri_parkour_zip.ps1`

### Test Results
- `flutter analyze`: success (`No issues found!`)
- `flutter test`: success (`All tests passed!`)
- External mod smoke validation: success
- Logic-only zip build: success
- Full zip build: expected safe failure when `assets/parkour_room.otr` is missing

### Manual Validation Log (with timestamp)
- 2026-02-18 13:38:19 -03:00: API v1 smoke validation executed successfully.
- 2026-02-18 13:38:20 -03:00: API v2 smoke validation executed successfully.
- 2026-02-18 13:38:20 -03:00: mods/skyhook_jump.zip generated and zip structure validated.
- 2026-02-18 13:38:29 -03:00: mods/kokiri_parkour.zip generated in logic-only mode (legacy regression check).
- 2026-02-18 13:39:01 -03:00: full release build still failing with repeated error C1041 (vc143.pdb lock).
- 2026-02-18 13:47:23 -03:00: revalidation after encoding cleanup: API v1 smoke, API v2 smoke, skyhook zip build, and kokiri logic-only zip build all succeeded.
- 2026-02-18 13:47:26 -03:00: release build retried with cmake --build build/x64-d --config Release --target soh -- /m:1 /p:UseMultiToolTask=false /p:CL_MPCount=1 and still failed with C1041 (vc143.pdb lock).

### Remaining Risks
- `scripts/fast64/export_scene_bundle.py` currently guarantees bundle contract files, but real high-fidelity geometry export still depends on proper Blender+Fast64 project setup.
- `buildModAssetArchive` currently copies an existing `.otr`; it does not yet invoke a full OTR build pipeline automatically.
- Full in-game E2E portal validation (Kokiri trigger + room transitions) still requires a real `parkour_room.otr` generated from production assets.

## Stage Y - WASM Item Runtime + Mod Input (2026-02-18)

### Goal
Enable API v2 external mods (`.zip`) with:
- runtime sandbox contract (`runtime.type = wasm3-v1`)
- new mod item behavior on hookshot slot (vertical slice)
- configurable mod input binding (jump action)
- compatibility with existing API v1 and `.otr/.o2r` flow

### Implemented
- `ExternalModManager` expanded to support `apiVersion` 1 and 2.
- `mod.json` v2 parsing added:
  - `runtime.type`, `runtime.module`, `runtime.maxMemoryKb`, `runtime.maxCallMs`
  - `itemDefinitions`, `inputDefinitions`
- New v2 action support in entry script:
  - `pressButton`
  - `grantModItem`
  - `revokeModItem`
  - `invokeWasm`
- `onInput` trigger parsing and per-mod input processing in frame update.
- Per-mod binding CVar naming (`gExternalMods.Input.<mod>.<binding>`), with default mask bootstrap.
- Runtime-only mod item state for slot `SLOT_HOOKSHOT`.
- Hook added for player item usage:
  - `GameInteractor::OnPlayerUseItem`
  - call site integrated in `z_player.c` before vanilla item use.
- New files:
  - `soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.h`
  - `soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp`
  - `soh/soh/Enhancements/external-mods/ExternalModItemRuntime.h`
  - `soh/soh/Enhancements/external-mods/ExternalModItemRuntime.cpp`
  - `soh/soh/Enhancements/external-mods/ExternalModUi.h`
  - `soh/soh/Enhancements/external-mods/ExternalModUi.cpp`
- Mod menu integration:
  - new section `External Mod Controls` to remap loaded v2 mod bindings.
- Official v2 example mod added:
  - `docs/examples/external_mods/skyhook_jump/mod.json`
  - `docs/examples/external_mods/skyhook_jump/items/items.json`
  - `docs/examples/external_mods/skyhook_jump/config/input.json`
  - `docs/examples/external_mods/skyhook_jump/scripts/init.json`
  - `docs/examples/external_mods/skyhook_jump/scripts/skyhook.wasm`
- New scripts:
  - `scripts/external-mods/build_skyhook_jump_zip.ps1`
  - `scripts/external-mods/smoke_validate_v2.ps1`

### Commands Executed
- `cmake -S . -B build/x64-d -A x64`
- `cmake --build build/x64-d --config Release --target soh -- /m:1 /p:UseMultiToolTask=false /p:CL_MPCount=1`
- `cmd /c "taskkill /IM mspdbsrv.exe /F >nul 2>&1 & del /f /q build\x64-d\soh\soh.dir\Release\vc143.pdb 2>nul & set CL=/FS & cmake --build build/x64-d --config Release --target soh -- /m:1 /p:UseMultiToolTask=false /p:CL_MPCount=1"`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_example.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_v2.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_skyhook_jump_zip.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_kokiri_parkour_zip.ps1 -LogicOnly`

### Test Results
- `smoke_validate_example.ps1`: success.
- `smoke_validate_v2.ps1`: success.
- `build_skyhook_jump_zip.ps1`: success (`mods/skyhook_jump.zip` generated).
- `build_kokiri_parkour_zip.ps1 -LogicOnly`: success (`mods/kokiri_parkour.zip` generated).
- Full C++ build on this machine: blocked by MSVC PDB contention (`C1041` on `vc143.pdb`) unrelated to specific changed file.

### Manual Validation Log (with timestamp)
- 2026-02-18 13:38:19 -03:00: API v1 smoke validation executed successfully.
- 2026-02-18 13:38:20 -03:00: API v2 smoke validation executed successfully.
- 2026-02-18 13:38:20 -03:00: mods/skyhook_jump.zip generated and zip structure validated.
- 2026-02-18 13:38:29 -03:00: mods/kokiri_parkour.zip generated in logic-only mode (legacy regression check).
- 2026-02-18 13:39:01 -03:00: full release build still failing with repeated error C1041 (vc143.pdb lock).
- 2026-02-18 13:47:23 -03:00: revalidation after encoding cleanup: API v1 smoke, API v2 smoke, skyhook zip build, and kokiri logic-only zip build all succeeded.
- 2026-02-18 13:47:26 -03:00: release build retried with cmake --build build/x64-d --config Release --target soh -- /m:1 /p:UseMultiToolTask=false /p:CL_MPCount=1 and still failed with C1041 (vc143.pdb lock).

### Remaining Risks
- WASM runtime in this stage validates module/call budget contract but does not execute full wasm bytecode engine yet.
- In-game E2E behavior validation (new hookshot behavior + jump remap via menu) remains pending until full project build succeeds in this environment.
- Build environment currently unstable due MSVC PDB lock (`C1041`), preventing final binary confirmation.


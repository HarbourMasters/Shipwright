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


## Stage Z - Extended Hook Subscriptions + Capability Surface (2026-02-18)

### Goal
Continue the closed plan by implementing the advanced hook subscription surface for ZIP mods (`hooks.extended.v1`) with safe parsing, dispatch filtering, and runtime diagnostics in Mod Menu.

### Implemented
- `ExternalModManager` manifest/runtime expanded with:
  - `capabilities`
  - `hookDefinitions`
  - runtime budgets (`maxFrameBudgetMs`, `maxHookCallsPerFrame`, `maxActorInstances`)
- Added `hooks/hooks.json` parser:
  - `subscriptions[].id`
  - `subscriptions[].hook`
  - `subscriptions[].dispatch` (`actions|wasmExport`)
  - `subscriptions[].actions` or `subscriptions[].wasmExport`
  - `subscriptions[].cooldownFrames`
  - `subscriptions[].filters` (`scene`, `actorId`, `category`, `itemId`, `flagType`, `flagId`, `healthDeltaRange`)
- Added extended hook dispatcher and registrations for:
  - `OnLoadGame`, `OnExitGame`, `OnSceneInit`, `AfterSceneCommands`, `OnTransitionEnd`
  - `OnFlagSet`, `OnFlagUnset`, `OnSceneFlagSet`, `OnSceneFlagUnset`
  - `OnPlayerUpdate`, `OnPlayerUseItem`, `OnPlayerHealthChange`, `OnItemReceive`
  - `OnActorInit`, `OnActorSpawn`, `OnActorUpdate`, `OnActorKill`, `OnActorDestroy`
  - `OnEnemyDefeat`, `OnBossDefeat`, `OnPlayDestroy`, `OnGameFrameUpdate`
- Added runtime guardrails:
  - per-subscription cooldown tracking
  - per-frame hook call budget (`maxHookCallsPerFrame`) with per-mod isolation
- Updated `ExternalModUi` to display:
  - mod capabilities
  - hook subscription list
  - hook budget usage per frame
- Updated `link_smoke_l` example to include `hooks.extended.v1` and `hookDefinitions`:
  - `docs/examples/external_mods/link_smoke_l/mod.json`
  - `docs/examples/external_mods/link_smoke_l/hooks/hooks.json`
- Updated `scripts/external-mods/build_link_smoke_l_zip.ps1` to validate optional `hookDefinitions` when `hooks.extended.v1` is enabled.

### Commands Executed
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_link_smoke_l_zip.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_v2.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_example.ps1`
- `Copy-Item mods/link_smoke_l.zip x64/Release/mods/link_smoke_l.zip -Force`

### Test Results
- `build_link_smoke_l_zip.ps1`: success (`mods/link_smoke_l.zip` generated and validated).
- `smoke_validate_v2.ps1`: success.
- `smoke_validate_example.ps1`: success.
- Hook-aware ZIP packaging validation: success (`hookDefinitions` required when `hooks.extended.v1` is declared).

### Manual Validation Log (with timestamp)
- 2026-02-18 23:10:25 -03:00: rebuilt `mods/link_smoke_l.zip` with `hooks.extended.v1` + `hookDefinitions` validation enabled.
- 2026-02-18 23:10:26 -03:00: copied package to `x64/Release/mods/link_smoke_l.zip` for in-game smoke test.
- 2026-02-18 23:12:12 -03:00: `smoke_validate_v2.ps1` executed with success.
- 2026-02-18 23:12:20 -03:00: `smoke_validate_example.ps1` executed with success.

### Remaining Risks
- Stage still does not include full WASM execution engine (runtime is contract/budget scaffold).
- `actors.vm.v1` lifecycle/runtime is not yet implemented.
- Full binary verification still depends on local C++ build execution.
## Stage AA - Actor VM v1 Runtime Actions (2026-02-18)

### Goal
Continue Stage Z and close the first functional slice of `actors.vm.v1` in runtime ZIP mods: parse actor definitions, execute actor actions from scripts, auto-spawn by scene, and expose runtime status in Mod Menu.

### Implemented
- `ExternalModManager`:
  - Added actor action parsing in `entryScript` for:
    - `spawnActor`
    - `despawnActor`
    - `setActorState`
    - `moveActorToPathNode`
    - `openDialog`
  - Added `TryParseActorDefinitions(...)` implementation for `actors/actors.json` contract.
  - Added actor runtime helpers:
    - definition lookup
    - instance lookup
    - per-definition instance limits
    - actor spawn/despawn lifecycle
  - Added `actors.vm.v1` runtime loading in `LoadRuntimeForPackage(...)`.
  - Added runtime budget field `maxActorInstances` into `ExternalModRuntime` and enforced on spawn.
  - Added scene auto-spawn of actor definitions in `OnSceneInit`.
  - Added actor VM per-frame tick/update dispatch in `OnGameFrameUpdate`.
  - Added actor runtime cleanup in `Shutdown` and `OnPlayDestroy`.
- `ExternalModUi`:
  - Added actor VM section in `External Mods (ZIP)` showing definition count, active instances, max budget, and per-definition summary.
- Example/package pipeline:
  - Added `docs/examples/external_mods/link_smoke_l/actors/actors.json`.
  - Updated `docs/examples/external_mods/link_smoke_l/mod.json` with capability `actors.vm.v1` and `actorDefinitions` path.
  - Updated `scripts/external-mods/build_link_smoke_l_zip.ps1` to validate `actorDefinitions` when `actors.vm.v1` is enabled.
  - Updated `scripts/external-mods/smoke_validate_v2.ps1` to validate optional `hookDefinitions`/`actorDefinitions` contracts and ZIP entries by capabilities.

### Commands Executed
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_link_smoke_l_zip.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_v2.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_example.ps1`
- `Copy-Item mods/link_smoke_l.zip x64/Release/mods/link_smoke_l.zip -Force`

### Test Results
- `build_link_smoke_l_zip.ps1`: success (`mods/link_smoke_l.zip` generated with `actors/actors.json`).
- `smoke_validate_v2.ps1`: success.
- `smoke_validate_example.ps1`: success.
- Release mods copy: success.

### Manual Validation Log (with timestamp)
- 2026-02-18 23:38:07 -03:00: `build_link_smoke_l_zip.ps1` executed successfully with actor capability validation.
- 2026-02-18 23:38:15 -03:00: `smoke_validate_v2.ps1` executed successfully after actor VM contract changes.
- 2026-02-18 23:38:15 -03:00: `smoke_validate_example.ps1` executed successfully (legacy v1 regression check).
- 2026-02-18 23:40:52 -03:00: `build_link_smoke_l_zip.ps1` re-executed after actor lifecycle reset adjustment (`OnLoadGame`) and completed successfully.
- 2026-02-18 23:40:52 -03:00: `smoke_validate_v2.ps1` and `smoke_validate_example.ps1` re-executed successfully after actor VM changes.

### Remaining Risks
- WASM runtime is still scaffold-level (budget/contract); no full wasm bytecode engine execution yet.
- Actor VM is logical/runtime-level only (no native visual actor renderer integration for custom meshes yet).
- Full binary/in-game confirmation still depends on local C++ build execution on your machine.

## Stage AB - Item Data v2 Policies + Grant Overrides (2026-02-19)

### Goal
Implement the next closed-plan block for `items.data.v2`: add robust item policy fields (`agePolicy`, `useMode`, `grant`) and enforce them in runtime without breaking existing ZIP v1/v2 and `.otr/.o2r` mod flows.

### Implemented
- `ExternalModManager.h`
  - Added `ExternalModItemAgePolicy` (`respectVanilla|allowChild|allowAdult`).
  - Added `ExternalModItemUseMode` (`vanilla|override|augment`).
  - Extended `ExternalModItemDefinition` with:
    - `description`
    - `agePolicy`
    - `useMode`
    - `grant` fields (`hasGrantItemId`, `grantItemId`, `hasGrantAmmo`, `grantAmmo`).
- `ExternalModManager.cpp`
  - Added parsers for `agePolicy`, `useMode`, and `grant.itemId` (int or alias string).
  - Added supported item aliases for grant parsing (`ITEM_HOOKSHOT`, `ITEM_BOW`, `ITEM_HAMMER`, `ITEM_ARROW_*`, `ITEM_BOW_ARROW_*`).
  - Refactored slot config metadata to include:
    - runtime use item ids
    - ammo source item id
    - vanilla age requirement
  - Added `GrantItemForDefinitionIfMissing(...)`:
    - grants configured/default item into slot
    - optionally grants ammo (`grant.ammo`) for compatible slots.
  - Upgraded age requirement override logic:
    - evaluates granted items by `agePolicy`
    - keeps vanilla requirement unless policy explicitly unlocks age.
  - Upgraded item-use runtime dispatch:
    - item match no longer hardcoded to hookshot-only
    - supports `useMode` behavior (`vanilla`, `augment`, `override`)
    - keeps hookshot impulse path for non-vanilla hookshot mode.
- `ExternalModUi.cpp`
  - Added visualization for item `useMode`, `agePolicy`, `grant`, and runtime state (`granted`, cooldown).
- Example/package updates
  - Updated `docs/examples/external_mods/skyhook_jump/items/items.json` to use `agePolicy`, `useMode`, `grant`, and `ui.description`.
  - Updated validation scripts:
    - `scripts/external-mods/build_skyhook_jump_zip.ps1`
    - `scripts/external-mods/smoke_validate_v2.ps1`
  - Added checks for `agePolicy`, `useMode`, and `grant.ammo` validity.

### Commands Executed
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_skyhook_jump_zip.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_v2.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_example.ps1`
- `Copy-Item mods/skyhook_jump.zip x64/Release/mods/skyhook_jump.zip -Force`

### Test Results
- `build_skyhook_jump_zip.ps1`: success.
- `smoke_validate_v2.ps1`: success.
- `smoke_validate_example.ps1`: success.
- `x64/Release/mods/skyhook_jump.zip` updated successfully.

### Manual Validation Log (with timestamp)
- 2026-02-18 23:59:59 -03:00: `build_skyhook_jump_zip.ps1` executed successfully after `items.data.v2` policy/grant changes.
- 2026-02-19 00:00:00 -03:00: `smoke_validate_v2.ps1` executed successfully with new item policy fields.
- 2026-02-19 00:00:00 -03:00: `smoke_validate_example.ps1` executed successfully (legacy regression check).
- 2026-02-19 00:00:08 -03:00: updated package copied to `x64/Release/mods/skyhook_jump.zip`.

### Remaining Risks
- Full C++ compile/in-game confirmation for this block still depends on local build execution in your environment.
- WASM runtime remains scaffold/budget-contract level (no full wasm bytecode engine execution yet).
- `grant.upgrades` support is not included in this block (only `grant.itemId` and `grant.ammo`).
### Revalidation (post-encoding normalization)
- 2026-02-19 00:01:30 -03:00: rebuilt `mods/skyhook_jump.zip` after UTF-8 normalization.
- 2026-02-19 00:01:30 -03:00: `smoke_validate_v2.ps1` passed after re-serialization.
- 2026-02-19 00:01:37 -03:00: copied updated ZIP to `x64/Release/mods/skyhook_jump.zip`.
- 2026-02-19 00:01:38 -03:00: `smoke_validate_example.ps1` passed (legacy regression).
- 2026-02-19 00:03:18 -03:00: rebuilt `mods/skyhook_jump.zip` after updating `scripts/init.json` grants for bow/hammer/fire/ice/light arrow unlock items.
- 2026-02-19 00:03:18 -03:00: `smoke_validate_v2.ps1` and `smoke_validate_example.ps1` passed after grant-list update.
- 2026-02-19 00:03:18 -03:00: copied updated ZIP to `x64/Release/mods/skyhook_jump.zip`.

## Stage AC - ZIP PNG Item Icons (2026-02-19)

### Goal
Allow external mod ZIP items to load custom PNG icon textures directly from `items/items.json` using `iconAsset` paths.

### Implemented
- `ExternalModManager.h`
  - Extended `ExternalModItemDefinition` with decoded icon buffer storage (`iconRgba32`).
- `ExternalModManager.cpp`
  - Added secure `iconAsset` loading in `LoadRuntimeForPackage(...)`:
    - validates safe relative path
    - enforces `.png` extension
    - reads binary from directory/zip package with size limits
    - decodes PNG via `stb_image` and converts to 32x32 RGBA.
  - Added runtime icon override pipeline:
    - captures vanilla `gItemIcons` table once
    - restores vanilla table each refresh
    - applies deterministic mod overrides for granted items.
  - Hooked icon refresh into lifecycle points (`Initialize`, `Shutdown`, `OnLoadGame`, `OnSceneInit`, `OnGameFrameUpdate`, `OnPlayDestroy`).
- `z_message_PAL.c`
  - Hardened textbox item icon path handling:
    - if `gItemIcons[itemId]` is not an OTR signature string, fallback to `ITEM_NONE` icon path before `strlen/memcpy`.
  - Prevents crashes when an icon is a raw decoded PNG pointer.

### Commands Executed
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/build_skyhook_jump_zip.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_v2.ps1`
- `powershell -ExecutionPolicy Bypass -File scripts/external-mods/smoke_validate_example.ps1`
- `Copy-Item mods/skyhook_jump.zip x64/Release/mods/skyhook_jump.zip -Force`

### Test Results
- `build_skyhook_jump_zip.ps1`: success.
- `smoke_validate_v2.ps1`: success.
- `smoke_validate_example.ps1`: success.
- ZIP copy to `x64/Release/mods/skyhook_jump.zip`: success.

### Manual Validation Log (with timestamp)
- 2026-02-19 00:24:20 -03:00: rebuilt `mods/skyhook_jump.zip` after PNG icon runtime integration.
- 2026-02-19 00:24:28 -03:00: `smoke_validate_v2.ps1` passed.
- 2026-02-19 00:24:28 -03:00: `smoke_validate_example.ps1` passed.
- 2026-02-19 00:25:07 -03:00: copied updated ZIP to `x64/Release/mods/skyhook_jump.zip`.
- 2026-02-19 00:25:45 -03:00: re-ran `smoke_validate_v2.ps1` after adding `.png` extension validation for `iconAsset`.

### Remaining Risks
- Full in-game visual validation of icon rendering and inventory/message paths still depends on local runtime verification.
- Custom icon format is currently restricted to PNG and normalized to 32x32 RGBA.
- If multiple enabled mods override the same item icon, precedence follows mod load order and id.

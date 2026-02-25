# Plans Ledger

This file is the fixed, append-only plan ledger for this repository.

## Rules

1. Every new execution plan must be appended here before implementation starts.
2. Existing plan entries must not be overwritten or removed.
3. Progress changes must be appended as `[UPDATE]` entries.
4. Use `tools/agents/append-plan-ledger.ps1` for writes.
5. Use `tools/agents/query-plan-ledger.ps1` for reads.

## Entry Format

```
## [PLN-YYYYMMDD-####] Plan Title
- createdUtc: 2026-02-25T00:00:00Z
- status: planned|in_progress|blocked|done|canceled
- scope: engine|docs|mod|tooling|mixed
- summary: ...
- milestones:
  1. ...
  2. ...
- tags: a, b
- refs:
  - path
```

## Update Format

```
## [PLN-YYYYMMDD-####][UPDATE] 2026-02-25T00:00:00Z
- status: in_progress|blocked|done|canceled
- note: ...
- refs:
  - path
```

## [PLN-20260225-0001] Plan ledger governance rollout
- createdUtc: 2026-02-25T02:41:23Z
- status: in_progress
- scope: tooling
- summary: Create fixed Plans.md and enforce append-only planning before implementation.
- milestones:
  1. create Plans.md
  2. add plan skill
  3. sync global skills
  4. update AGENTS protocol
- tags: plans, agents
- refs:
  - docs/agents/Plans.md
  - docs/agents/AGENTS.md

## [PLN-20260225-0001][UPDATE] 2026-02-25T02:42:51Z
- status: done
- note: Implemented fixed Plans.md ledger, created soh-agents-plan-ledger skill, updated AGENTS policy, and synced 13 skills globally.
- refs:
  - docs/agents/Plans.md
  - docs/agents/skills/soh-agents-plan-ledger/SKILL.md
  - tools/agents/append-plan-ledger.ps1

## [PLN-20260225-0002] Phase1 gap closure: v3 cleanup and runtime references
- createdUtc: 2026-02-25T03:00:23Z
- status: done
- scope: mixed
- summary: Aligned docs/examples to apiVersion 3, removed legacy action usage from examples, generated docs/catalogs.json actions.json events.json, and kept parser params v3-only.
- milestones:
  1. Normalize legacy examples to apiVersion 3
  2. Replace removed legacy action usage with applyStatus
  3. Add machine-readable runtime reference exports
- refs:
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/examples/external_mods
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260225-0003] Gap closure batch: wasm host runtime + content registry extraction
- createdUtc: 2026-02-25T04:17:40Z
- status: in_progress
- scope: engine
- summary: Close immediate v3 blockers by stabilizing executable WASM host bridge, adding runtime budgets/telemetry, and extracting catalog lookup logic from ExternalModManager into a dedicated content registry.
- milestones:
  1. fix build/runtime blocker in wasm runtime and manager bridge
  2. extract Find*ById lookups into ExternalModContentRegistry
  3. refresh exported runtime reference JSON and docs
- tags: external-mods, api-v3, wasm3, modularization
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp
  - soh/soh/Enhancements/external-mods/ExternalModContentRegistry.cpp
  - tools/external_mods/export_runtime_reference.ps1
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md

## [PLN-20260225-0003][UPDATE] 2026-02-25T04:17:49Z
- status: done
- note: Completed: release build passes; executable WASM host imports wired with budgets and per-frame metrics; lookup helpers moved to ExternalModContentRegistry; runtime reference export/docs updated.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.h
  - soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp
  - soh/soh/Enhancements/external-mods/ExternalModContentRegistry.h
  - soh/soh/Enhancements/external-mods/ExternalModContentRegistry.cpp
  - docs/actions.json
  - docs/catalogs.json
  - docs/events.json

## [PLN-20260225-0004] Skill soh-git-checkpoint-merge rollout
- createdUtc: 2026-02-25T04:26:48Z
- status: in_progress
- scope: tooling
- summary: Create and wire a Git checkpoint/merge skill with automatic commit triggers after feature completion and successful builds, plus end-of-plan local merge workflow.
- milestones:
  1. create skill files and scripts
  2. update manifest/index and AGENTS policy
  3. sync/install/validate skills
- tags: skills, git, automation, plans
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/SKILL.md
  - docs/agents/AGENTS.md
  - docs/agents/skills.manifest.json
  - docs/agents/skills.index.json

## [PLN-20260225-0004][UPDATE] 2026-02-25T04:31:36Z
- status: done
- note: Implemented soh-git-checkpoint-merge skill, updated manifest/index/AGENTS, and installed+validated mirror/global skills.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/SKILL.md
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-feature.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-build.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/git-audit.ps1
  - docs/agents/skills.manifest.json
  - docs/agents/skills.index.json
  - docs/agents/AGENTS.md

## [PLN-20260225-0004][UPDATE] 2026-02-25T04:42:26Z
- status: done
- note: Hardened soh-git-checkpoint-merge scripts: fixed StrictMode-safe git output handling and validated dry-run flows (git-audit, feature/build checkpoints).
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/git-audit.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-feature.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-build.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1

## [PLN-20260225-0005] Aim OTS reticle visibility + LMB fire flow fix
- createdUtc: 2026-02-25T12:42:55Z
- status: in_progress
- scope: engine
- summary: Fix reticle visibility for selected slingshot/pistol and route LMB through vanilla item-button input path for Aim OTS v3.
- milestones:
  1. Add reticleVisibility enum+parser field,Resolve effective profile for mouse-fire by context,Inject LMB into sControlInput cur/press with edge detection,Implement selected/aim_only/button_hold reticle gating,Update aim_ots_toggle_demo camera profile and sync runtime,Build validation and log/memory updates
- tags: external-mods,aim-ots,input,reticle,api-v3
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/src/code/z_player_lib.c,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0006] Aim OTS reticle visibility + LMB fire flow fix
- createdUtc: 2026-02-25T12:43:12Z
- status: in_progress
- scope: engine
- summary: Fix reticle visibility for selected slingshot/pistol and route LMB through vanilla item-button input path for Aim OTS v3.
- milestones:
  1. Add reticleVisibility enum+parser field,Resolve effective profile for mouse-fire by context,Inject LMB into sControlInput cur/press with edge detection,Implement selected/aim_only/button_hold reticle gating,Update aim_ots_toggle_demo camera profile and sync runtime,Build validation and log/memory updates
- tags: external-mods,aim-ots,input,reticle,api-v3
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/src/code/z_player_lib.c,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0006][UPDATE] 2026-02-25T12:52:38Z
- status: done
- note: Implemented reticleVisibility parsing+runtime, moved LMB fire injection into sControlInput cur/press path, relaxed slingshot reticle draw gate, updated aim_ots_toggle_demo profile to selected, and validated Release build.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/src/code/z_player_lib.c,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json,x64/Release/mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0007] AGENTS governance refresh aligned to current docs/tools
- createdUtc: 2026-02-25T13:02:38Z
- status: in_progress
- scope: docs
- summary: Refresh docs/agents/AGENTS.md and governance artifacts to match current docs/tools state, enforce plan/memory/validation gates, and keep doc-drift as soft gate.
- milestones:
  1. rewrite AGENTS protocol sections and command matrix,update project_state snapshot with current risks and skill-pack count,append governance decision and drift backlog entries to memory log,run validation commands and close plan with update
- tags: agents,governance,docs,memory
- refs:
  - docs/agents/AGENTS.md,docs/agents/project_state.md,docs/agents/Plans.md,docs/agents/memory.log

## [PLN-20260225-0007][UPDATE] 2026-02-25T13:04:30Z
- status: done
- note: Completed governance refresh: AGENTS rewritten with execution/read/memory gates, project_state updated for 14-skill baseline and soft-gate drift policy, memory decision+incident logged, and validation commands executed.
- refs:
  - docs/agents/AGENTS.md,docs/agents/project_state.md,docs/agents/Plans.md,docs/agents/memory.log,docs/agents/memory.index.json

## [PLN-20260225-0008] Aim OTS fix: no pitch lock + selected reticle visibility
- createdUtc: 2026-02-25T13:11:51Z
- status: in_progress
- scope: engine
- summary: Fix OTS LMB aim pitch lock and ensure reticleVisibility=selected works for pistol/slingshot by correcting profile resolution and removing forced focus rotation.
- milestones:
  1. fix profile resolution fallback order for context,remove forced focus rotation in bow/slingshot ready-to-fire path,validate aim_ots demo profile settings,build release and log validation
- tags: external-mods,aim-ots,input,reticle
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/src/overlays/actors/ovl_player_actor/z_player.c,x64/Release/mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0008][UPDATE] 2026-02-25T13:15:00Z
- status: done
- note: Implemented aim OTS fix: removed forced focus pitch/yaw sync in ready-to-fire, corrected context profile resolution ordering (active explicit -> best mod profile -> core fallback), kept selected reticle behavior, validated config presence and successful Release build.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/soh/Enhancements/external-mods/ExternalModManager.cpp,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json,x64/Release/mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0009] Aim OTS reticle visible without pistol in hand
- createdUtc: 2026-02-25T13:30:42Z
- status: in_progress
- scope: engine
- summary: Fix slingshot/pistol reticle selected-mode gating so reticle only draws when slingshot/pistol is actually in hand, avoiding always-on overlay while other items are active.
- milestones:
  1. isolate root cause in DrawAimReticleIfActive selected gating,patch selected visibility gate to require slingshot-in-hand context,build release validation and runtime sanity check guidance,append memory decision and close plan update
- tags: external-mods,aim-ots,reticle,ui
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/src/code/z_player_lib.c

## [PLN-20260225-0009][UPDATE] 2026-02-25T13:33:21Z
- status: done
- note: Fixed reticle leakage: DrawAimReticleIfActive now requires slingshot in hand for aim_only/button_hold/selected modes, so crosshair no longer appears while pistol/slingshot is not held; release build succeeded.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,x64/Release/logs/Ship of Harkinian.log

## [PLN-20260225-0010] freeze_dome_staff_demo (hammer impact freeze dome)
- createdUtc: 2026-02-25T14:20:14Z
- status: in_progress
- scope: mixed
- summary: Implement API v3 data-driven hammerGroundImpact trigger, spawnShockwave effect, AoE targetScope, and new freeze_dome_staff_demo docs/runtime sync.
- milestones:
  1. Add trigger/effect/aoe type+parser changes
  2. Wire hammer impact interop and runtime execution
  3. Create freeze_dome_staff_demo and sync docs/runtime
  4. Update docs refs and validate build/smoke
- tags: external-mods, api-v3, demo
- refs:
  - docs/agents/project_state.md
  - docs/examples/external_mods

## [PLN-20260225-0010][UPDATE] 2026-02-25T14:40:36Z
- status: done
- note: Implemented hammerGroundImpact trigger, spawnShockwave effect, AoE targetScope filtering, z_player hammer impact hook, and freeze_dome_staff_demo with docs/runtime sync; Release build passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/freeze_dome_staff_demo
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/MODDING.md
  - tools/external_mods/sync_examples_to_runtime.ps1

## [PLN-20260225-0011] Pistol select-to-aim + slot toggle + LMB/B fire
- createdUtc: 2026-02-25T15:48:21Z
- status: in_progress
- scope: engine
- summary: Implement PLANpistol.md by adding item opt-in fields, slot press toggle state, BTN_B fire gating, and pistol demo camera profile/runtime integration.
- milestones:
  1. Add item fields+parser
  2. Add aim select runtime+interop
  3. Patch Player_ProcessItemButtons slot toggle and LMB/B fire
  4. Update pistol demo docs/runtime and docs refs
  5. Build + smoke + close plan
- tags: external-mods, aim, pistol, input, api-v3
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json

## [PLN-20260225-0011][UPDATE] 2026-02-25T16:01:19Z
- status: done
- note: Implemented PLANpistol: added aimSelectToggle/aimAttackButtonFire parsing, slot-select aim toggle runtime+interop, BTN_B virtual fire routing in Player_ProcessItemButtons, reticle gating tied to active select state, pistol demo camera profile/capability updates, docs updates, demo sync and Release build.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModInterop.h
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/items/items.json
  - docs/examples/external_mods/pistol_hitscan_demo/camera/camera_profiles.json
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/MODDING.md

## [PLN-20260225-0012] Sylian Foundry modloader docs refresh and governance alignment
- createdUtc: 2026-02-25T16:13:46Z
- status: in_progress
- scope: docs
- summary: Create canonical Sylian Foundry Modloader doc, archive legacy MVP history, reorganize modding references for API v3, run drift/runtime reference validation, and close governance records.
- milestones:
  1. Create canonical SYLIAN_FOUNDRY_MODLOADER.md and archive legacy MVP history
  2. Rewrite MOD_SDK_MVP.md and MODDING.md to v3-aligned structure
  3. Refresh EXTERNAL_MOD_* references and README modloader navigation
  4. Run drift guard and runtime reference export validations
  5. Append plan update + memory decision and run git checkpoints
- tags: docs, modloader, api-v3, governance
- refs:
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - docs/MOD_SDK_MVP.md
  - docs/MODDING.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
  - docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md
  - README.md

## [PLN-20260225-0012][UPDATE] 2026-02-25T16:20:55Z
- status: done
- note: Completed full docs refresh for Sylian Foundry Modloader: canonical doc added, MVP history archived, references reorganized, README apiVersion note added, runtime refs exported, and drift guard now passes.
- refs:
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - docs/archive/MOD_SDK_MVP_LEGACY_HISTORY.md
  - docs/MOD_SDK_MVP.md
  - docs/MODDING.md
  - docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - README.md
  - docs/actions.json
  - docs/events.json
  - docs/catalogs.json
  - docs/agents/project_state.md

## [PLN-20260225-0012][UPDATE] 2026-02-25T16:22:26Z
- status: done
- note: Post-completion finalize-plan-merge gate attempted and blocked because working tree is not clean (dirty libultraship submodule). Docs implementation remains complete.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/memory.log

## [PLN-20260225-0013] Fix pistol C-down toggle exit and restore reticle/custom model visibility
- createdUtc: 2026-02-25T16:53:43Z
- status: in_progress
- scope: engine
- summary: Prioritize slot-toggle deactivation over virtual fire injection and remove granted-only gating for pistol aim-select reticle/custom model paths.
- milestones:
  1. Patch Player_ProcessItemButtons to process slot-toggle press before virtual fire injection and consume deactivation frame
  2. Patch ExternalModManager aim-select candidate availability to not depend only on granted
  3. Patch reticle/model selection to use availability helper and add targeted debug logs
  4. Build Release and validate pistol toggle/reticle/model behavior in-game
- tags: external-mods, aim-select, pistol, input, reticle, model, api-v3
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - x64/Release/mods/pistol_hitscan_demo/items/items.json
  - x64/Release/logs/Ship of Harkinian.log

## [PLN-20260225-0013][UPDATE] 2026-02-25T17:15:04Z
- status: done
- note: Patched Player_ProcessItemButtons to prioritize C/D slot-toggle deactivation before virtual fire injection, removed granted-only aim availability gating, restored slingshot reticle/model candidate resolution via availability helper, and validated Release build.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - x64/Release/logs/Ship of Harkinian.log

## [PLN-20260225-0013][UPDATE] 2026-02-25T17:16:56Z
- status: done
- note: finalize-plan-merge gate attempted and blocked because working tree is not clean (dirty libultraship submodule). Implementation is complete and committed locally.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/Plans.md

## [PLN-20260225-0014] Sylian Foundry Modloader v4 content-only master rollout
- createdUtc: 2026-02-25T17:44:20Z
- status: in_progress
- scope: mixed
- summary: Start implementing the v4 hard-break foundation for content-only mods: manifest v4 contract fields, expanded capability map, runtime contract registries, v4 reference exports, and migration scaffolding for examples/tools.
- milestones:
  1. Implement M0 governance baseline + frozen gap matrix artifacts
  2. Implement M1 kernel: apiVersion 4 hard-break and manifest v4 fields/capability wiring
  3. Implement M2 foundations: public action/condition registries and mod validator tool scaffold
  4. Implement M10 scaffolding: foundry CLI-style scripts (validate/migrate/sync) and migrate official examples to v4
  5. Run build + reference exports + docs sync + ledger/memory updates
- tags: external-mods, sylian-foundry, api-v4, content-only, tooling, docs
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - tools/external_mods/export_runtime_reference.ps1
  - docs/SYLIAN_FOUNDRY_MODLOADER.md

## [PLN-20260225-0014][UPDATE] 2026-02-25T18:10:17Z
- status: in_progress
- note: Completed v4 foundation checkpoint: apiVersion hard-break + manifest v4 fields/capability wiring, validator+migrator+CLI scripts, runtime contract registries export, docs/runtime mod manifests migrated to v4, and Release build/validation passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,tools/external_mods/validate_mod.ps1,tools/external_mods/migrate_mods_v3_to_v4.ps1,tools/external_mods/foundry-cli.ps1,tools/external_mods/export_runtime_reference.ps1,docs/runtime_contract/actions.registry.json,docs/runtime_contract/conditions.registry.json,plan.md

## [PLN-20260225-0015] Freeze ice-trap no-damage data-driven + freeze dome demo uplift
- createdUtc: 2026-02-25T18:34:43Z
- status: in_progress
- scope: mixed
- summary: Implement freezeProfile + AoE player-inclusive scopes + freeze_dome_staff_demo updates + status runtime callbacks/stacking groundwork for higher data-driven freedom in API v4.
- milestones:
  1. Add freezeProfile and AoE target scope parser/type support
  2. Implement runtime freeze no-damage mode for player/enemy with ice shell lifecycle
  3. Update freeze_dome_staff_demo to use statusDefinitions and player_enemies_bosses scope
  4. Add status callbacks/stacking execution + persistent AoE ticking runtime state
  5. Update validator/migrator/docs/runtime references and run build/sync validations
- tags: external-mods, freeze, api-v4, data-driven
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/freeze_dome_staff_demo
  - tools/external_mods/validate_mod.ps1

## [PLN-20260225-0015][UPDATE] 2026-02-25T19:02:56Z
- status: done
- note: Implemented freezeProfile no-damage mode, new AoE scopes with player inclusion, persistent AoE ticking, status callback/stacking execution, freeze_dome_staff_demo status catalog uplift, validator/migrator/runtime reference updates, and validated Release build + sync/validate scripts.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/freeze_dome_staff_demo
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/migrate_mods_v3_to_v4.ps1
  - tools/external_mods/export_runtime_reference.ps1
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/actions.json
  - docs/events.json
  - docs/catalogs.json

## [PLN-20260225-0015][UPDATE] 2026-02-25T19:04:34Z
- status: done
- note: Post-completion finalize-plan-merge attempted and blocked because working tree is not clean (dirty libultraship submodule). Implementation commit is complete locally.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/Plans.md

## [PLN-20260225-0016] Fix camera capability mismatch for aim/pistol demos
- createdUtc: 2026-02-25T19:32:31Z
- status: in_progress
- scope: mod
- summary: Resolve runtime disable error requiring camera.aim_profiles.v2 by updating demo manifest capabilities and syncing runtime mods.
- milestones:
  1. Update aim_ots_toggle_demo and pistol_hitscan_demo capabilities to camera.aim_profiles.v2
  2. Sync docs examples to runtime mods
  3. Validate mod manifests and confirm no capability mismatch warnings
- tags: external-mods, camera, demos, api-v4
- refs:
  - docs/examples/external_mods/aim_ots_toggle_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json
  - x64/Release/mods/aim_ots_toggle_demo/mod.json
  - x64/Release/mods/pistol_hitscan_demo/mod.json

## [PLN-20260225-0016][UPDATE] 2026-02-25T19:34:08Z
- status: done
- note: Fixed camera capability contract mismatch by updating aim_ots_toggle_demo and pistol_hitscan_demo to camera.aim_profiles.v2 (docs+runtime) and migrated runtime aim_ots entryScript to apiVersion 4; validator now passes for both mods.
- refs:
  - docs/examples/external_mods/aim_ots_toggle_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json
  - x64/Release/mods/aim_ots_toggle_demo/mod.json
  - x64/Release/mods/pistol_hitscan_demo/mod.json
  - x64/Release/mods/aim_ots_toggle_demo/scripts/init.json
  - tools/external_mods/migrate_mods_v3_to_v4.ps1
  - tools/external_mods/validate_mod.ps1

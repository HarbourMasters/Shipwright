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

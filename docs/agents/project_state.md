# Project State Snapshot

Last updated: 2026-02-25T18:10:00Z

## Current Governance and Architecture State

- External Mods baseline for active development is API v4 (`apiVersion: 4`).
- Sylian Foundry Modloader docs are now organized around `docs/SYLIAN_FOUNDRY_MODLOADER.md`.
- Runtime reference exports remain:
  - `docs/catalogs.json`
  - `docs/actions.json`
  - `docs/events.json`
- Runtime contract registries are exported under:
  - `docs/runtime_contract/actions.registry.json`
  - `docs/runtime_contract/conditions.registry.json`
- Example mods source of truth is `docs/examples/external_mods` with runtime mirror in `x64/Release/mods`.
- Agent governance is repository-local under `docs/agents`.
- Skill Pack mirror under `docs/agents/skills` currently has **14 skills**.
- Plans ledger is append-only at `docs/agents/Plans.md`.

## Active Decisions

1. Every implementation must register a plan in `Plans.md` before code changes.
2. Plan progress/completion is append-only via `[UPDATE]`; existing entries are immutable.
3. Memory policy is append-only with `summary <= 180` and compaction threshold `200 entries / 60 days`.
4. Commit discipline follows `soh-git-checkpoint-merge` checkpoints (feature/build/finalize).
5. Doc/runtime drift checks are mandatory; current policy remains soft gate for unrelated urgent fixes.
6. Contract changes must regenerate runtime reference exports with `tools/external_mods/export_runtime_reference.ps1`.
7. API v4 migration scaffolding is active via `tools/external_mods/migrate_mods_v3_to_v4.ps1` and `tools/external_mods/validate_mod.ps1`.

## Open Risks

1. Local finalize merge can remain blocked when repository cleanliness fails (recently observed with dirty `libultraship` submodule state).
2. Duplicate/overlapping plan ids for same scope can cause governance confusion if not superseded explicitly.
3. Demo/runtime sync can drift if `sync_examples_to_runtime.ps1` is skipped after demo edits.
4. API v4 implementation is foundational (M0-M2/tooling); runtime modularization milestones M3+ remain open.
5. Some gameplay behaviors still require in-game validation beyond build/static checks.

## Near-Term Milestones

1. Keep AGENTS protocol aligned with real scripts and current skill inventory.
2. Maintain canonical Sylian Foundry Modloader docs and references without drift.
3. Keep demo sync and runtime reference exports up to date after contract-affecting changes.
4. Preserve memory/index integrity (`rebuild-index` + `validate-memory`) after memory operations.
5. Preserve strict plan-first workflow for all new implementation scopes.
6. Advance v4 milestones M3+ (runtime modularization, item state machine, camera/hud/effects graphs).

## Out of Scope (Current Snapshot)

- NPC/quest/story runtime expansion remains outside current phase baseline.

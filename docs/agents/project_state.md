# Project State Snapshot

Last updated: 2026-02-25T13:03:38Z

## Current Governance and Architecture State

- External Mods workstream baseline is API v3 (`apiVersion: 3`) for new work.
- Runtime reference exports live in:
  - `docs/catalogs.json`
  - `docs/actions.json`
  - `docs/events.json`
- Example mods source of truth is `docs/examples/external_mods` with runtime mirror in `x64/Release/mods`.
- Agent governance is repository-local under `docs/agents`.
- Skill Pack mirror under `docs/agents/skills` currently has **14 skills**.
- Plans ledger remains append-only at `docs/agents/Plans.md`.

## Active Decisions

1. Every implementation must register a plan in `Plans.md` before code changes.
2. Plan progress/completion is append-only via `[UPDATE]`; existing entries are immutable.
3. Memory policy is append-only with `summary <= 180` and compaction threshold `200 entries / 60 days`.
4. Commit discipline follows `soh-git-checkpoint-merge` checkpoints (feature/build/finalize).
5. Doc/runtime drift checks are mandatory to run, but currently treated as **soft gate** while known backlog is open.
6. For contract changes, regenerate runtime reference exports with `tools/external_mods/export_runtime_reference.ps1`.

## Open Risks

1. Known doc drift backlog exists (currently 12 issues reported by `check-doc-drift -DryRun`).
2. Duplicate/overlapping plan ids for same scope can cause governance confusion if not superseded explicitly.
3. Demo/runtime sync can drift if `sync_examples_to_runtime.ps1` is skipped after doc demo edits.
4. Some gameplay behaviors still require in-game validation beyond build/static checks.

## Near-Term Milestones

1. Keep AGENTS protocol aligned with real scripts and current skill inventory.
2. Execute periodic doc drift cleanup plan until hard-gate quality is feasible.
3. Maintain demo sync and runtime reference exports after contract-affecting changes.
4. Keep memory/index integrity green (`rebuild-index` + `validate-memory`) after memory operations.
5. Preserve strict plan-first workflow for all new implementation scopes.

## Out of Scope (Current Snapshot)

- NPC/quest/story runtime expansion remains outside current phase baseline.

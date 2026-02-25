# Agent Operations Protocol

This directory is the long-term operational memory and governance layer for agents working on this repository.

## 1) Scope and Authority

1. `docs/agents/project_state.md` is the active truth for current decisions, risks, and priorities.
2. `docs/agents/Plans.md` is the append-only execution ledger and must be updated before implementation starts.
3. `docs/agents/memory.log` is append-only historical memory (decisions/incidents/tasks/notes).
4. `docs/agents/memory.index.json` and `docs/agents/archive.manifest.json` are retrieval/integrity metadata.
5. For external mods contract work, baseline is API v3 (`apiVersion: 3`) and current runtime references under `docs/*.json`.

## 2) Skill Pack Integration

- Auditable mirror: `docs/agents/skills/`
- Operational install: `C:/Users/leolo/.codex/skills/`
- Governance files:
  - `docs/agents/skills.manifest.json`
  - `docs/agents/skills.index.json`

Current mirror baseline: **14 skills**.

## 3) Required Read Matrix

### 3.1 Mandatory for every task
1. `docs/agents/Plans.md`
2. `docs/agents/project_state.md`
3. `docs/agents/memory.index.json`
4. Targeted entries in `docs/agents/memory.log` (only ids relevant to the task)

### 3.2 Additional reads by task type

- External mods runtime/parser/docs contract:
  - `docs/actions.json`
  - `docs/events.json`
  - `docs/catalogs.json`
  - `docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md`
  - `docs/EXTERNAL_MOD_MANAGER_REFERENCE.md`
  - `docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md`

- Demo/content sync work:
  - `docs/examples/external_mods/*`
  - `tools/external_mods/sync_examples_to_runtime.ps1`

- Skills/governance work:
  - `docs/agents/skills.manifest.json`
  - `docs/agents/skills.index.json`
  - `tools/agents/validate-skills.ps1`

## 4) Execution Gates (Mandatory)

1. **Before coding/implementation**
   - Append a new plan entry:
   - `tools/agents/append-plan-ledger.ps1 -Mode new ...`

2. **After closing a feature block**
   - Run:
   - `docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-feature.ps1`

3. **After successful build (if changes pending)**
   - Run:
   - `docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-build.ps1`

4. **At plan completion**
   - Run:
   - `docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1 -PlanId <PLN-...>`
   - Append plan `[UPDATE]` as `done`.

Commit message contract is mandatory: Conventional Commit title + `Plan`, `Applied`, `Validation` lines.

## 5) Docs/Runtime Sync Logic

1. If runtime contract/actions/events/capabilities changed:
   - `tools/external_mods/export_runtime_reference.ps1`

2. If example demos changed:
   - `tools/external_mods/sync_examples_to_runtime.ps1`

3. Run drift check:
   - `docs/agents/skills/soh-doc-runtime-drift-guard/scripts/check-doc-drift.ps1`

4. **Drift policy is currently soft gate**:
   - record/report drift backlog,
   - do not silently ignore,
   - do not block unrelated urgent fixes while backlog is open.

## 6) Memory Integrity Logic

1. Never edit old lines in `memory.log`.
2. Corrections are new entries with `supersedes`.
3. `summary` must be `<= 180` chars.
4. Significant engineering decisions must be logged as `kind=decision`.
5. Root-cause incidents must be logged as `kind=incident` with refs.
6. Compact when:
   - `memory.log` > 200 entries, or
   - active window > 60 days.
7. After compaction:
   - `tools/agents/rebuild-index.ps1`
   - `tools/agents/validate-memory.ps1`

JSONL entry contract:

`{"id":"mem-YYYYMMDD-######","ts":"UTC ISO8601","kind":"decision|incident|task|note","topic":"...","tags":["..."],"scope":"engine|docs|mod|tooling","summary":"...","details":"...","refs":["path:line","commit"],"status":"open|done|deprecated","supersedes":["id"]}`

## 7) Plan Hygiene and Duplicate Handling

1. Existing plan entries are immutable; progress is append-only `[UPDATE]`.
2. If duplicate plans are created for the same scope:
   - mark superseded plan via `[UPDATE]` as `canceled` or `done`,
   - reference the surviving `PlanId`,
   - add a memory note linking both ids.

## 8) Token Budget Defaults

1. Build context packs before broad analysis.
2. Default budget:
   - max 8 files
   - max 3 snippets/file
3. Use:
   - `tools/agents/context-pack.ps1`
4. Escalate only when first pack is insufficient, and record reason in memory.

## 9) Command Table (Single Source)

### 9.1 `tools/agents`
- `append-plan-ledger.ps1` - append plan/new update entries.
- `query-plan-ledger.ps1` - read ledger entries.
- `append-memory.ps1` - append memory entries.
- `query-memory.ps1` - query memory log/index.
- `compact-memory.ps1` - archive older entries.
- `rebuild-index.ps1` - rebuild `memory.index.json`.
- `validate-memory.ps1` - integrity validation.
- `sync-skills.ps1` - sync mirror -> global path.
- `install-skills.ps1` - install skills to operational path.
- `validate-skills.ps1` - validate skill pack structure.
- `context-pack.ps1` - generate low-token context packs.

### 9.2 `tools/external_mods`
- `export_runtime_reference.ps1` - regenerate `docs/catalogs.json`, `docs/actions.json`, `docs/events.json`.
- `sync_examples_to_runtime.ps1` - sync demos from `docs/examples/external_mods` to runtime mods folder.

## 10) Safety Rules

1. Do not store secrets, tokens, private keys, or personal data in memory/docs.
2. Prefer repository-local references in `refs`.
3. When historical entries conflict, `project_state.md` is authoritative.
4. Never treat legacy external-mod actions/params as active contract baseline for new work.

---
name: soh-agent-memory-optimizer
description: Operate repository-local long-term memory for Ship of Harkinian agents with aggressive compaction and minimal token footprint. Use when recording decisions/incidents, compacting history, rebuilding index, and validating memory integrity.
---

# SOH Agent Memory Optimizer

1. Read order is mandatory.
- `docs/agents/project_state.md`
- `docs/agents/memory.index.json`
- targeted lines in `docs/agents/memory.log`

2. Write short entries.
- Use `tools/agents/append-memory.ps1`.
- Keep summary under 180 chars.

3. Run memory cycle.
- Use `scripts/memory-cycle.ps1`.
- Compact when thresholds are exceeded.

4. Validate integrity.
- Rebuild index.
- Validate all records.

## Example (PT-BR)
- "Depois de corrigir parser, registrar `decision` com refs e rodar `rebuild-index`."

## Reference
- Read `references/memory-policy.md`.

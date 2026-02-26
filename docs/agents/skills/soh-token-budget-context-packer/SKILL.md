---
name: soh-token-budget-context-packer
description: Build minimal context packs for coding and triage tasks in Ship of Harkinian with strict token budgets. Use when selecting only the smallest useful file/snippet set before analysis or patching.
---

# SOH Token Budget Context Packer

1. Use strict budget.
- Max 8 files per task.
- Max 3 snippets per file.

2. Prefer high-signal files.
- Start from `project_state.md`, `memory.index.json`, then target code files.

3. Generate pack file.
- Run `scripts/context-pack.ps1`.
- Feed result into analysis instead of broad scans.

## Example (PT-BR)
- "Bug em `applyStatus`: pegar parser + manager + types + log, sem abrir pastas inteiras."

## Reference
- Read `references/context-budgets.md`.

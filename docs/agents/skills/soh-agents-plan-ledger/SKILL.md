---
name: soh-agents-plan-ledger
description: Consult and append execution plans in docs/agents/Plans.md using append-only milestones and updates, with optional memory.log linkage. Use before starting any new implementation plan and when posting plan progress updates.
---

# SOH Agents Plan Ledger

1. Consult before planning.
- Read `docs/agents/Plans.md`.
- Query recent entries with `scripts/query-plans.ps1`.

2. Append before execution.
- Create a new plan entry with `scripts/append-plan.ps1 -Mode new`.
- Include summary, status, milestones, and refs.

3. Update without overwrite.
- Add progress with `scripts/append-plan.ps1 -Mode update`.
- Never delete or rewrite old plan sections.

4. Link memory when needed.
- Use `-WriteMemory` to append a paired memory event.

## Mandatory Rule
- Every new implementation plan must be appended to `docs/agents/Plans.md` before coding starts.

## Example (PT-BR)
- "Antes de executar o plano novo, salvar no Plans.md com milestones e refs; depois usar UPDATE para progresso."

## Reference
- Read `references/plan-ledger-policy.md`.

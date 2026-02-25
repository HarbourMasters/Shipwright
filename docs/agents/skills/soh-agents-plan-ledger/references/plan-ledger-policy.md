# Plan Ledger Policy

## Purpose

`docs/agents/Plans.md` is the canonical, append-only milestone ledger for planning.

## Requirements

1. Create a plan entry before starting implementation.
2. Keep prior plans immutable; add updates as new entries.
3. Include actionable milestones and repository refs.
4. Keep status explicit: `planned|in_progress|blocked|done|canceled`.
5. Optionally mirror updates into memory log for searchable history.

## Preferred Commands

1. New:
- `tools/agents/append-plan-ledger.ps1 -Mode new ...`
2. Update:
- `tools/agents/append-plan-ledger.ps1 -Mode update -PlanId PLN-... ...`
3. Query:
- `tools/agents/query-plan-ledger.ps1 -Limit 20`

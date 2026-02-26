---
name: soh-smoke-regression-matrix
description: Run a focused smoke regression matrix for external-mod runtime and example mods after code changes. Use when validating parser/runtime/build stability quickly before deeper playtesting.
---

# SOH Smoke Regression Matrix

1. Run quick matrix script.
- Use `scripts/run-smoke.ps1`.
- Start with `-DryRun`, then live execution.

2. Validate core groups.
- Parser contract (`mod.json`, required files, schema versions)
- Runtime load/enable status
- Demo package presence and shape

3. Report blocker first.
- Stop at first hard failure.
- Keep output actionable.

## Example (PT-BR)
- "Demo habilita mas runtime desliga por `itemDefinitions`: tratar como blocker principal."

## Reference
- Read `references/smoke-matrix.md`.

---
name: soh-git-checkpoint-merge
description: Enforce continuous Git checkpoints and end-of-plan local branch merge for SoH work. Use when closing features, after successful builds, and when finalizing a plan to audit commits/branches and merge locally without push.
---

# SOH Git Checkpoint and Plan Merge

1. Commit at feature checkpoint.
- Run `scripts/commit-after-feature.ps1`.
- If tree is clean, it exits with skip.
- If there are changes, it stages all and commits with Conventional Commit + plan context.

2. Commit after successful build.
- Run `scripts/commit-after-build.ps1` after build success.
- It commits only when there are pending changes.
- It avoids empty/duplicate checkpoint commits by skipping on clean tree.

3. Audit and merge at plan end.
- Run `scripts/finalize-plan-merge.ps1 -PlanId <PLN-...>`.
- It audits branch state and preconditions first.
- It merges locally with `--no-ff`, without automatic push.
- It appends plan completion update to `docs/agents/Plans.md`.

4. Commit message policy (mandatory).
- Conventional Commit title.
- Required body lines:
  - `Plan: <PLN-...>`
  - `Applied: <short summary>`
  - `Validation: <status>`

## Defaults
- Target branch priority for final merge:
  - explicit `-TargetBranch`
  - `develop`
  - default branch from `origin/HEAD`
- If no active/open plan can be resolved: `Plan: PLN-UNSET`.

## Example (PT-BR)
- "Fechei a feature e buildou: rodo commit-after-feature, depois commit-after-build, e no final finalize-plan-merge com PLN."

## Reference
- Read `references/commit-merge-policy.md`.

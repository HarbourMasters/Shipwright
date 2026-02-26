# Commit and Merge Policy (Skill Contract)

## Trigger Policy

1. Feature checkpoint:
- Commit when a feature block is concluded and there are pending tracked changes.

2. Build-success checkpoint:
- Commit after successful build only if there are pending tracked changes.
- If tree is clean, skip.

## Commit Message Contract

1. Title:
- Conventional Commit format.
- Max 72 chars.

2. Body (required lines):
- `Plan: <PLN-...>`
- `Applied: <what was included>`
- `Validation: <current validation state>`

## Plan and Branch Finalization

1. Run branch audit before merge:
- current branch
- clean/dirty tree
- conflict status
- commits from source branch against target
- upstream ahead/behind

2. Merge policy:
- local merge only
- `--no-ff`
- no automatic push

3. Target branch resolution:
- explicit parameter
- `develop`
- fallback `origin/HEAD`

## Failure Policy

1. Hard fail:
- dirty tree in finalization step
- unresolved conflicts
- no commits to merge
- source == target
- detached HEAD

2. Soft skip:
- commit checkpoint with clean tree

## Ledger Integration

1. Resolve active plan ID from `docs/agents/Plans.md`.
2. On plan finalization, append `[UPDATE] status: done`.
3. Append memory note with branch and commit refs.

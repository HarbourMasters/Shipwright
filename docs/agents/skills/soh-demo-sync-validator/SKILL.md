---
name: soh-demo-sync-validator
description: Keep external-mod demos synchronized between docs/examples and runtime mods folders with deterministic validation. Use when updating demos, shipping examples, or preparing smoke tests.
---

# SOH Demo Sync Validator

1. Sync demos.
- Use `scripts/sync-demos.ps1`.
- Mirror docs examples into runtime mods.

2. Validate shape.
- Use `scripts/validate-demos.ps1`.
- Check required files and API version.

3. Keep drift visible.
- Fail on missing or mismatched demos.

## Example (PT-BR)
- "Atualizei `shield_skate_demo` em docs; sincronizar para `x64/Release/mods` antes do teste."

## Reference
- Read `references/sync-rules.md`.

---
name: soh-doc-runtime-drift-guard
description: Detect and prevent documentation drift against actual external-mod runtime contracts in Ship of Harkinian API v3. Use after parser/runtime changes or before publishing modding docs.
---

# SOH Doc Runtime Drift Guard

1. Check docs against contract.
- Run `scripts/check-doc-drift.ps1`.
- Focus on unsupported legacy references and missing v3 contract mentions.

2. Patch only mismatched sections.
- Keep docs concise.
- Keep examples aligned with current runtime behavior.

3. Re-run drift check after edits.

## Example (PT-BR)
- "Docs ainda citam `igniteFrontTarget`; remover e substituir por `applyStatus`."

## Reference
- Read `references/doc-contract.md`.

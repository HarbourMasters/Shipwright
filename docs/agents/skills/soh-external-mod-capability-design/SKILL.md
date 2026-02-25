---
name: soh-external-mod-capability-design
description: Design and implement Ship of Harkinian external-mod capabilities for API v3 with runtime safety and deterministic behavior. Use when adding new data-driven actions, schema fields, runtime subsystems, or capability-gated files in ExternalModParser/ExternalModManager and related runtimes.
---

# SOH External Mod Capability Design (v3)

1. Lock contract first.
- Define capability name, required files, and schemaVersion.
- State defaults, clamps, and fatal vs warning behavior.

2. Keep parser and runtime in the same patch.
- Parse and validate in `ExternalModParser.cpp`.
- Execute in runtime (`ExternalModManager` or dedicated subsystem).
- Log with `modId`, `file`, `jsonPath`, and `reason`.

3. Enforce API v3 only.
- Reject legacy actions/fields.
- Keep behavior deterministic on invalid input.

4. Add operational proof.
- Add one demo or update an existing demo under `docs/examples/external_mods`.
- Add one dry-run script check when possible.

5. Update docs and memory.
- Update contracts in docs.
- Append one `decision` entry to `docs/agents/memory.log`.

## Guardrails
- Do not add hidden defaults.
- Do not soft-ignore structural schema errors.
- Do not merge parser-only changes without runtime behavior.

## Example (PT-BR)
- "Adicionar `combat.aoe.v1` com `schemaVersion=1`, validar range e falhar so no mod com erro contextual."

## Reference
- Read `references/design-checklist-v3.md`.

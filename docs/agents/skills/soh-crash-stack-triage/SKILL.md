---
name: soh-crash-stack-triage
description: Triage Ship of Harkinian crash stacks and map them to concrete patch targets in external-mod subsystems. Use when logs contain exception traces, stack frames, or crash metadata after mod actions/model loads.
---

# SOH Crash Stack Triage

1. Extract crash section.
- Use `scripts/stack-triage.ps1`.
- Keep exception code, top frames, and first external-mod frame.

2. Map frame to subsystem.
- Resource load crash
- Parser/action crash
- Behavior/WASM crash
- Render/interpreter crash

3. Propose narrow patch.
- Add guard, validation, or state cleanup at first reliable boundary.

## Example (PT-BR)
- "`MemoryStream::Read` em textura custom: validar size/offset antes de registrar recurso."

## Reference
- Read `references/crash-signatures.md`.

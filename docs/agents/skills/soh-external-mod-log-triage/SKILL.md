---
name: soh-external-mod-log-triage
description: Triage Ship of Harkinian external-mod logs to isolate the first blocker and map it to patch targets in API v3 runtime. Use when a mod is disabled, parser errors appear, runtime throws, or behavior/action execution is incorrect.
---

# SOH External Mod Log Triage (v3)

1. Isolate first blocker.
- Use `scripts/extract-blocker.ps1`.
- Ignore secondary warnings until primary blocker is fixed.

2. Map signature to subsystem.
- Parser/schema issues -> `ExternalModParser`.
- Runtime disable/action failure -> `ExternalModManager` / subsystem runtime.
- Render/model failures -> Fast64 path and resource loader.

3. Produce smallest fix plan.
- One root cause.
- One patch target.
- One validation command.

4. Validate after patch.
- Rebuild.
- Reload external mods.
- Confirm blocker line is gone.

## Output format
- `Primary blocker`
- `Likely root cause`
- `Patch target`
- `Validation`
- `Residual risk`

## Example (PT-BR)
- "Primeiro erro: `Missing or invalid field: itemUseProfiles` -> corrigir `mod.json` e parser de path opcional."

## Reference
- Read `references/signatures-v3.md`.

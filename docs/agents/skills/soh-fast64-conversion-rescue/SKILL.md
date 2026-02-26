---
name: soh-fast64-conversion-rescue
description: Diagnose and patch Fast64 conversion failures for Ship of Harkinian API v3 mods, including unsupported macros, archive finalization, texture filter/tile issues, and custom model load crashes.
---

# SOH Fast64 Conversion Rescue (v3)

1. Capture minimal evidence.
- First conversion blocker from log.
- `items/items.json` entry for target item.
- Source model files (`*.inc.c`, `header.h`).

2. Classify primary issue.
- Macro mapping
- Archive finalize
- Resource path
- Texture size/tile/filter
- Runtime load crash

3. Patch narrowly.
- Add parser mapping for safe macros.
- Keep hard-fail for structural macros.
- Respect explicit `modelTextureFilter`.
- Apply conservative tile/scale normalization only when factor is obvious.

4. Validate in-game.
- Build release.
- Reload external mods.
- Confirm runtime stays enabled and model renders.

## Guardrails
- No broad parser rewrites for one symbol issue.
- Keep `auto` behavior unchanged unless explicit filter override is requested.
- Log unknown macros with owner display list.

## Example (PT-BR)
- "Erro `gsDPSetTextureDetail`: mapear macro para comando equivalente ou soft-ignore seguro com warning consolidado."

## Reference
- Read `references/error-map-v3.md`.

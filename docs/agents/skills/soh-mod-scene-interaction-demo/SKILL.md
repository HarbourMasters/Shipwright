---
name: soh-mod-scene-interaction-demo
description: Scaffold a runnable Ship of Harkinian API v3 external-mod scene interaction demo with deterministic fallback behavior, input bindings, scene definitions, and actor/behavior interactions.
---

# SOH Scene Interaction Demo (v3)

1. Scaffold minimum package.
- Create `mod.json`, `scripts`, `config`, `scenes`, `actors`, `behaviors`, `items`, `assets`.
- Keep `apiVersion` exactly `3`.

2. Provide two entry paths.
- Input combo entry (`MOD_ACTION3 + C-Up`).
- Area/scene trigger entry.

3. Keep fallback playable.
- If namespaced scene resource is missing, route to host entrance fallback.

4. Add deterministic interactions.
- NPC-like interaction sets progress var.
- Object-like interaction checks var and teleports or notifies.

5. Validate quickly.
- Reload mods.
- Enter scene both ways.
- Verify interaction progression.

## Example (PT-BR)
- "Sem `.otr` custom, o demo ainda entra em cena host e permite testar interacao."

## Reference
- Read `references/scaffold-v3.md`.

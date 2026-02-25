---
name: soh-path-symbol-finder
description: Find paths, symbols, entry points, and ownership quickly in Ship of Harkinian repositories. Use when you need exact file/function targets before patching parser, runtime, UI, hooks, or demo assets.
---

# SOH Path and Symbol Finder

1. Search by intent, not filename guess.
- Use `scripts/pathfinder.ps1` with a query.
- Prefer `rg` over slower tools.

2. Resolve ownership.
- Map symbol to declaration and call sites.
- Capture one primary patch file before editing.

3. Keep retrieval compact.
- Max 3 snippets per file.
- Avoid recursive broad dumps.

## Example (PT-BR)
- "Achar `loadModScene` parser + executor + ponto de hook no `z_play_otr.cpp`."

## Reference
- Read `references/path-map.md`.

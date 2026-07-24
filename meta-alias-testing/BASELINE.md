# Baseline — stock engine (before the fix)

Captured 2026-07-24 against the **unmodified** libultraship submodule (`2bfbde3a`,
= `develop`'s pin), running the reworked test kit ([README.md](README.md) /
[meta-loading.md](meta-loading.md)). Each case's mod(s) were dropped into
`build/soh/mods/` (priority order set for the layered sets), and the first (LUS)
boot logo color was read.

Legend: **brown** = soh.o2r's vanilla real · **green** = a mod's real · **magenta**
= alias target · **blank** = failed to load.

## Results (all matched predictions)

| case | mod(s), low → high | observed (stock) | expected after fix |
|---|---|---|---|
| control | (none) | **brown** | brown |
| `case1` | real + meta + target | **green** | magenta |
| `case2` | real + meta | **green** | green |
| `case3` | meta + target | **blank** | magenta |
| `case4` | meta only | **blank** | brown |
| `case5` | real | **green** | green |
| flagship (all 3) | meta / real / target | **green** | magenta |
| flagship (drop `-30`) | meta / real | **green** | green |
| `L1` | meta+target / real | **green** | green |
| reachback | target / meta | **blank** | magenta |

## What the stock engine does

The `.meta` mechanism is non-functional: `Archive::IndexFile` strips the `.meta`
suffix, so `LoadFileProcess("…​.meta")` never resolves, and a mod's `.meta` still
*claims* (shadows) the base path. So:

- When a mod ships a real asset, its `.meta` is silently ignored → the mod's plain
  real loads (**green**): case1, case2, case5, flagship, L1.
- When a mod ships only a `.meta` (no real), it shadows soh.o2r's real but can't
  resolve → nothing loads (**blank**): case3, case4, reachback.

## What the fix must change

- **Alias resolves** → magenta: case1, case3, flagship (all 3), reachback.
- **No shadowing** → soh's vanilla shows instead of blank: case4 → brown.
- **Unchanged** (regression checks): case2, case5, L1, flagship-drop-`30`.

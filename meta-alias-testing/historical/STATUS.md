# Test status — engine changes

> [!NOTE]
> **Superseded / historical.** This tracks the first (shadowing-based) engine
> approach and a since-corrected resolution model — some "expected" values here
> (e.g. case4, L3) no longer match. The current model is
> [meta-loading.md](../meta-loading.md) and the current cases are in
> [README.md](../README.md); this file will be replaced with fresh results after the
> override-identity rework.

Tracks the five test cases + control as the three libultraship engine changes
(branch `meta-alias-fallback`) are applied **one at a time**. See `BASELINE.md`
for the pre-change reference and `README.md` for how to run each case.

Vehicle: the boot ship logo. green = real/default loaded · magenta =
replacement/target loaded · brown = original (mod not in effect) · nothing =
load failed.

## Engine changes (applied in this order)

1. **index** — `Archive::IndexFile`: also index a `.meta` sidecar under its
   literal `foo.meta` name so `LoadFileProcess(path + ".meta")` resolves.
   **[applied — `ce374750`]**
2. **fallback** — `ResourceLoader::LoadResource`: prefer the alias target, else
   fall back to the real asset; defer the null-file check past resolution.
   **[applied — `a0f86f9e`]**
3. **reachability** — `ResourceManager::LoadResourceProcess`: don't bail when
   the real file is missing but a `.meta` exists; fall through so the loader can
   resolve the alias. **[pending]**

## Results (observed 2026-07-24, matching predictions)

| case | real/meta/target | baseline | +index | +fallback | +reachability (final) | expected final |
|---|:---:|---|---|---|---|---|
| control (no mod) | — | brown | brown | brown | — | brown |
| `case1` | ✓/✓/✓ | green | **magenta** | magenta | — | magenta |
| `case2` | ✓/✓/✗ | green | **nothing** ⚠ | **green** | — | green |
| `case3` | ✗/✓/✓ | nothing | nothing | nothing | — | magenta |
| `case4` | ✗/✓/✗ | nothing | nothing | nothing | — | nothing |
| `case5` | ✓/✗/— | green | green | green | — | green |

### Notes
- **index flips case1 (green → magenta):** the alias resolves the moment the
  `.meta` is indexed — the core #1165/#984 reachability win. It temporarily
  regressed case2 (green → nothing), since the old loader overwrote the file
  handle with the missing target and had no fallback.
- **fallback restores case2 (nothing → green):** target missing + real asset
  present now falls back to the real asset (the new #1165 behavior). The Trace
  log shows `Meta alias target '…/fancyShipDL.xml' missing; falling back to real
  asset '…/gShipLogoDL'.`
- **case3 stays blank** until **reachability** lets the loader run for a
  resource that exists only as a `.meta`.

# Test status — engine changes

Tracks the five test cases + control as the three libultraship engine changes
(branch `meta-alias-fallback`) are applied **one at a time**. See `BASELINE.md`
for the pre-change reference and `README.md` for how to run each case.

Vehicle: the boot ship logo. green = real/default loaded · magenta =
replacement/target loaded · brown = original (mod not in effect) · nothing =
load failed.

## Engine changes

1. `Archive::IndexFile` — also index a `.meta` sidecar under its literal
   `foo.meta` name so `LoadFileProcess(path + ".meta")` resolves. **[applied]**
2. `ResourceManager::LoadResourceProcess` — don't bail when the real file is
   missing but a `.meta` exists; fall through so the loader can resolve the
   alias. **[pending]**
3. `ResourceLoader::LoadResource` — prefer the alias target, else fall back to
   the real asset at the original path; move the null-file check after
   resolution. **[pending]**

## Results (observed 2026-07-24, matching predictions)

| case | real/meta/target | baseline | +#1 | +#2 | +#3 (final) | expected final |
|---|:---:|---|---|---|---|---|
| control (no mod) | — | brown | brown | — | — | brown |
| `case1` | ✓/✓/✓ | green | **magenta** | — | — | magenta |
| `case2` | ✓/✓/✗ | green | **nothing** ⚠ | — | — | green |
| `case3` | ✗/✓/✓ | nothing | nothing | — | — | magenta |
| `case4` | ✗/✓/✗ | nothing | nothing | — | — | nothing |
| `case5` | ✓/✗/— | green | green | — | — | green |

### Notes
- **+#1 flips case1 (green → magenta):** the alias resolves the moment the
  `.meta` is indexed — the core #1165/#984 reachability win.
- **+#1 temporarily regresses case2 (green → nothing):** the old loader
  overwrites the file handle with the missing target and has no fallback.
  Expected; restored by change #3.
- **case3 stays blank** until change #2 lets the loader run for a resource that
  exists only as a `.meta`.

# Baseline — current engine (before the fix)

> [!NOTE]
> **Historical.** A faithful record of what the **unmodified** engine did, but its
> "expected after fix" column uses the old (shadowing) model — since corrected to
> [meta-loading.md](../meta-loading.md). The case set has also been reworked; see
> [README.md](../README.md).

Captured on 2026-07-23 against the **unmodified** libultraship submodule
(`2bfbde3a`), by dropping each archive into `build/soh/mods/` one at a time and
reading the color of the first (LUS) boot logo.

Legend: green = real/default loaded · magenta = replacement/target loaded ·
brown = mod didn't take effect (soh.o2r's untouched logo) · nothing = load failed.

| case | real | meta | target | observed now | expected after fix |
|---|:---:|:---:|:---:|---|---|
| control (no mod in `mods/`) | — | — | — | **brown** (original) | brown |
| `case1-real-meta-target`   | ✓ | ✓ | ✓ | **green** (meta ignored) | **magenta** |
| `case2-real-meta-notarget` | ✓ | ✓ | ✗ | **green** | green (fallback) |
| `case3-noreal-meta-target` | ✗ | ✓ | ✓ | **nothing** | **magenta** |
| `case4-meta-only`          | ✗ | ✓ | ✗ | **nothing** | nothing (correct fail) |
| `case5-real-nometa`        | ✓ | ✗ | — | **green** | green (regression) |

All observations matched predictions.

## What the baseline confirms

- The overlay approach is sound: **case1 = green** (not brown) means a `mods/`
  archive loads and shadows soh.o2r's `gShipLogoDL` *before* the boot logo draws.
- `.meta` aliases are **non-functional** on the current engine:
  - When a real asset exists, the `.meta` is silently ignored (cases 1, 2 load the
    real/default green asset instead of the magenta target).
  - When only a `.meta` exists, the resource can't be resolved at all (cases 3, 4
    render nothing).

## What the fix must change

- **case1: green → magenta** — with the real asset present, the alias target is
  now preferred.
- **case3: nothing → magenta** — a meta-only alias resolves to its target.
- cases 2, 4, 5 stay the same (fallback / correct-fail / no-regression), though
  case2's Trace log should now show the fallback path.

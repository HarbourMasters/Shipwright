# `.meta` alias fallback — manual test kit

Test scaffolding for the libultraship `.meta` alias fallback work
([Kenix3/libultraship#1165](https://github.com/Kenix3/libultraship/issues/1165), which also
requires the `.meta` resolution fix from
[#984](https://github.com/Kenix3/libultraship/issues/984)).

It reproduces and expands the manual test from
[HarbourMasters/Shipwright@f5d2e58](https://github.com/HarbourMasters/Shipwright/commit/f5d2e583444de41a1a43a00b9c3fca2bf782dc2d):
a `.meta` file aliases the boot **ship logo** DisplayList to a renamed copy, and we watch the
title screen to see which asset actually loaded. `f5d2e58` only covered "alias target present";
this kit covers the full matrix, and recolors the assets so the loaded one is obvious.

## What a `.meta` alias should do (#1165)

A `.meta` sidecar means *"prefer the target asset, otherwise fall back to the real asset at the
original path."* Requesting resource `foo`:

| real `foo` | `foo.meta` | alias target | expected result |
|---|---|---|---|
| present | present | present | load **target** |
| present | present | absent  | load **real** (fallback — #1165) |
| absent  | present | present | load **target** (meta-only alias — #984 path) |
| absent  | present | absent  | **fail** |
| present | absent  | —       | load **real** (unchanged legacy behavior) |

## Test vehicle & color legend

The vehicle is `textures/nintendo_rogo_static/gShipLogoDL`, the DisplayList drawn as the first
("LUS") logo at boot (`soh/soh/Enhancements/cosmetics/CustomLogoTitle.cpp`). The materials are
recolored so you can read the result off the title screen at a glance:

| what you see | meaning |
|---|---|
| **green** ship logo | the **real / default** asset loaded (`gShipLogoDL`) |
| **magenta** ship logo | the **replacement / alias target** loaded (`fancyShipDL.xml`) |
| **brown** ship logo (normal) | the test `.o2r` did **not** take effect (soh.o2r's untouched logo) |
| **nothing** (blank where the logo should be) | the resource **failed to load** |

## The five cases (`o2r/`)

Each `.o2r` overrides only the ship logo. Because a mod that contains `gShipLogoDL.meta`
*shadows* soh.o2r's `gShipLogoDL` key, "real asset absent" is simulated simply by leaving the
real DisplayList out of the archive — no `soh.o2r` rebuild required. The heavy `*_tri_*` /
`*_vtx_*` geometry is never included, so it keeps resolving from soh.o2r and each archive stays a
few KB.

| file | real | meta | target | current engine (broken) | after the fix |
|---|:---:|:---:|:---:|---|---|
| `case1-real-meta-target.o2r`   | ✓ | ✓ | ✓ | **green** (meta ignored) | **magenta** |
| `case2-real-meta-notarget.o2r` | ✓ | ✓ | ✗ | **green** | **green** (fallback) |
| `case3-noreal-meta-target.o2r` | ✗ | ✓ | ✓ | **nothing** | **magenta** |
| `case4-meta-only.o2r`          | ✗ | ✓ | ✗ | **nothing** | **nothing** (correct fail) |
| `case5-real-nometa.o2r`        | ✓ | ✗ | — | **green** | **green** (regression check) |

The engine fix is proven by **case 1 (green → magenta)** and **case 3 (nothing → magenta)**.
Cases 2/4/5 confirm no regressions. (The "current engine" column is the baseline captured before
any code change — see `BASELINE.md` once recorded.)

## How to run a case

1. Build/run SoH as usual (executable at `build/soh/soh.elf`, with `oot.o2r` / `soh.o2r` /
   `mods/` alongside it).
2. Put **exactly one** case file in the mods folder, and remove any others:
   ```sh
   rm -f build/soh/mods/case*.o2r
   cp meta-alias-testing/o2r/case1-real-meta-target.o2r build/soh/mods/
   ```
3. (Recommended) set the log level to **Trace** so load decisions are visible — in-game
   Developer Tools → Log Level, or set CVar `gDeveloperTools.LogLevel` to `0`.
4. Launch `build/soh/soh.elf`. The first logo shown is the one under test; press **A/B/Start**
   to advance past the logos. Compare against the color legend.
5. **Restart** the game between cases (resources are cached for the life of the process), and
   swap the `.o2r` in `mods/` first.

### Reading the Trace log
- Alias resolved to target: the target path (`…/fancyShipDL.xml`) is loaded.
- Fallback (case 2): a trace like `Meta alias target '…/fancyShipDL.xml' missing; falling back to
  real asset '…/gShipLogoDL'.`
- Failure (case 4): `Failed to load file at path textures/nintendo_rogo_static/gShipLogoDL.`

## Rebuilding the kit

```sh
./meta-alias-testing/regen-assets.sh   # regenerate src/assets from soh's real ship logo
./meta-alias-testing/build-o2r.sh      # zip src/assets subsets into o2r/case*.o2r
```

## Layout
```
meta-alias-testing/
  README.md            this file
  PLAN.md              the implementation + test plan
  regen-assets.sh      derives src/assets/ from soh's gShipLogoDL (recolor + alias)
  build-o2r.sh         assembles o2r/case*.o2r from src/assets/
  src/assets/…         generated test assets (default green, replacement magenta, the .meta)
  o2r/case*.o2r        the five committed test archives
```

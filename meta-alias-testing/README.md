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

## Layered cases (`o2r/layered/`) — archive priority

The five cases above use a single mod over `soh.o2r`. The issues also require correct behavior
when **two archives** touch the same path at different priorities (libultraship #1165's design
notes): `resource` and `resource.meta` must act as **one** override identity, resolved by archive
priority (last loaded wins).

Each set is a pair — `L#-10-…` (lower priority) and `L#-20-…` (higher priority). Drop **both**
files of one set into `mods/`, make `20-*` the higher priority, and restart:

- In the Mods menu, drag `20-*` **above** `10-*` (top of the list = highest priority, overrides
  those below it), **or**
- set the `gSettings.EnabledMods` CVar to `L#-10-…|L#-20-…` (last in the list = highest priority).

Mods are **not** hot-reloaded — restart the game after changing which set is present or its order.

| set | lower (`10-`) | higher (`20-`) | correct result | current impl |
|---|---|---|---|---|
| **L1**  | meta → target (magenta) | real (green) | **green** (higher real wins) | **magenta** ✗ |
| **L1b** | meta-only (target absent) | real (green) | **green** | **green** |
| **L2**  | real (green) | meta → target (magenta) | **magenta** (higher meta wins) | **magenta** |
| **L3**  | real (green) | meta-only (target absent) | **nothing** (real is in a lower archive, not with the `.meta`) | **nothing** |

- **L1** is the key correctness gap: `resource` and `resource.meta` currently occupy *separate*
  override slots, so a lower-priority `.meta` still overrides a higher-priority real asset. It
  should render green; today it renders magenta.
- **L3** is decided (see the resolution model below): the alias's *same-path* fallback only uses a
  real asset shipped **in the same archive as the `.meta`**. Here the winning archive has just the
  `.meta`; the real lives in a lower archive, so there's nothing to fall back to → nothing.

### L5 — the cross-game motivation (order-independent)

The point of #1165: a base archive ships an alias plus its native real asset, and a **separate**
mod supplies the shared alias target (nothing overlaps on the same key, so load order doesn't
matter here). Toggle the target mod to switch between "mod loaded" and "vanilla boot":

- `L5-base-real-meta.o2r` **+** `L5-target.o2r` in `mods/` → **magenta** (alias resolves to the
  mod's shared target — "mod loaded").
- `L5-base-real-meta.o2r` **alone** → **green** (target absent → falls back to the base's native
  real asset — "vanilla boot"; same behavior as `case2`).

Both states already work with the current implementation (the target lookup is a normal
cross-archive resolve); L5 is here as the end-to-end demonstration of the feature's purpose.

### L6 — "reach back" to an asset in another archive

A mod ships `gShipLogoDL.meta → fancyShipDL.xml`; the alias **target** (`fancyShipDL.xml`) lives
in a *separate* archive that may even be lower priority than the `.meta`. Three files, priority
low → high: `L6-10-lower-real` (a real that the `.meta` overrides), `L6-20-mid-target` (the target
asset), `L6-30-higher-meta` (the winning alias).

- All three present → **magenta**. The `.meta` wins the `gShipLogoDL` identity (highest priority)
  and resolves its target from the separate archive.

This works because the **target is a different path** (`fancyShipDL.xml`), resolved by a normal
global lookup — it's found wherever it lives, regardless of priority relative to the `.meta`
(hence "reach back"). Contrast with L3, which is about the *same-path* fallback, not the target.

## Resolution model (what a layered `.meta` does)

See [meta-loading.md](meta-loading.md) for a flowchart of this. For a requested path `X`:

1. **One identity, by priority.** `X` and `X.meta` are the *same* override slot; the
   highest-priority archive that supplies either one **wins** `X`. Lower archives are shadowed.
2. **Winner decides.** If the winning archive supplies a real `X`, load it. If it supplies
   `X.meta`, resolve the alias (below). (A higher-priority real therefore beats a lower `.meta` —
   that's L1.)
3. **Target — resolved globally.** The alias's target path is looked up like any other resource:
   found in whatever loaded archive has it, highest priority wins, above **or below** the `.meta`
   (L5, L6).
4. **Same-path fallback — local only.** If the target is missing, fall back to a real `X` **only
   if the winning archive itself also ships one** (co-located, e.g. `soh.o2r` shipping both). The
   fallback does **not** reach down into lower-priority archives (L3 → fail). Practical rule:
   *ship a `.meta` and its intended fallback real in the same archive.*
5. **Nothing to load → fail** (case4).

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
  o2r/case*.o2r        the five single-archive test archives
  o2r/layered/L*.o2r   layered (two-archive priority) test sets
```

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

The precise resolution rules live in **[meta-loading.md](meta-loading.md)** ("highest-priority
provider wins"); this file is the runnable test cases built from them.

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
| **brown** ship logo (normal) | **soh.o2r's own** real `gShipLogoDL` loaded — the vanilla asset (always the lowest-priority provider) |
| **green** ship logo | a **mod's** real `gShipLogoDL` loaded |
| **magenta** ship logo | the **alias target** (`fancyShipDL.xml`) loaded |
| **nothing** (blank where the logo should be) | the resource **failed to load** — needs no provider at all, which soh.o2r's brown prevents (so it doesn't occur in these cases) |

Note brown is now a *meaningful* result (soh's vanilla), not "the mod failed to load." soh.o2r
always ships a real `gShipLogoDL`, so it's an ever-present lowest-priority provider in every case.

## The single-mod cases (`o2r/`)

One mod dropped over the normal soh.o2r. soh.o2r's real `gShipLogoDL` (brown) is always in the
provider pool at the lowest priority; the mod's assets sit above it. The heavy `*_tri_*` /
`*_vtx_*` geometry is never included, so it keeps resolving from soh.o2r and each archive stays a
few KB.

| file | mod ships | providers (low → high) | → result | proves |
|---|---|---|---|---|
| `case1-real-meta-target.o2r`   | real + meta + target | brown@soh, green@mod, magenta@mod (alias) | **magenta** | an archive's own `.meta` beats its own real |
| `case2-real-meta-notarget.o2r` | real + meta         | brown@soh, green@mod                       | **green**   | target absent → the mod's real |
| `case3-noreal-meta-target.o2r` | meta + target       | brown@soh, magenta@mod (alias)             | **magenta** | alias outranks soh's brown |
| `case4-meta-only.o2r`          | meta only           | brown@soh                                  | **brown**   | no shadowing — soh's real still shows through |
| `case5-real-nometa.o2r`        | real only           | brown@soh, green@mod                       | **green**   | plain real, no `.meta` |

case4 is really a **log** check: brown by sight is ambiguous ("did the mod even load?"), so confirm
in the Trace log that the `.meta` was found, its target was absent, and it fell through to soh's
`gShipLogoDL`.

## Layered cases (`o2r/layered/`) — archive priority

These stack multiple mods to test the priority ranking — the crux of the model. Priority = mod
load order (last loaded = highest). To set it, in the Mods menu drag the higher-numbered file
**above** the lower (top of the list = highest priority), or set the `gSettings.EnabledMods` CVar
to `…-10-…|…-20-…|…-30-…` (last = highest). Mods are **not** hot-reloaded — **restart** after
changing which files are present or their order.

### Flagship — the cross-game case (`flagship-*`)

Three mods replicating the `2ship.o2r` / `mm.o2r` / `mod.o2r` stack from #1165, priority low → high:

- `flagship-10-meta` — `gShipLogoDL.meta → fancyShipDL` (the alias, = `2ship.o2r`)
- `flagship-20-real` — a real `gShipLogoDL`, green (the vanilla, = `mm.o2r`)
- `flagship-30-target` — `fancyShipDL`, magenta (the shared target, = the mod)

| loaded | providers (low → high) | → result | |
|---|---|---|---|
| all three | brown@soh, green@`20`, **magenta@`30`** (alias) | **magenta** | mod present → alias target wins |
| drop `flagship-30-target` | brown@soh, **green@`20`** | **green** | no mod → falls back to the vanilla real |

That drop-`30` round-trip (magenta ↔ green) is the whole point of #1165: the mod works when
present, and the game still boots the vanilla asset when it isn't.

### L1 — a real above the alias target (`L1-*`)

- `L1-10-meta-target` (lower) — `meta → fancyShipDL` + `fancyShipDL` (magenta)
- `L1-20-real` (higher) — a real `gShipLogoDL` (green)

Providers: brown@soh, magenta@`10` (alias), **green@`20`** (real) → **green**. The real outranks
the alias target, so the real wins — the mirror image of the flagship.

### Reach-back — target below the `.meta` (`reachback-*`)

- `reachback-10-target` (lower) — `fancyShipDL` (magenta)
- `reachback-20-meta` (higher) — `gShipLogoDL.meta → fancyShipDL`

Providers: brown@soh, **magenta@`10`** (alias, target lives *below* the `.meta`) → **magenta**.
The target is a different path, resolved globally, so it's found wherever it lives — the `.meta`
can point "down" to it.

## Resolution model (what a layered `.meta` does)

See **[meta-loading.md](meta-loading.md)** for the flowchart and worked examples — that's the
source of truth. In short:

> `X.meta → Y` adds **`Y` as an alternate provider of `X`**. A request for `X` loads the
> **highest-priority provider** — a real `X`, or an aliased `Y` — ranked by the archive where that
> provider's **asset** lives (*not* where the `.meta` lives). An archive's own `.meta` beats its
> own real `X`; if nothing provides `X` or a resolvable `Y`, the load fails.

"Fall back to the real asset" is just the case where the target `Y` doesn't exist, so a real `X`
is the only provider left.

## How to run a case

1. Build/run SoH as usual (executable at `build/soh/soh.elf`, with `oot.o2r` / `soh.o2r` /
   `mods/` alongside it).
2. Put the case's file(s) in the mods folder and remove any others — **one** file for a single-mod
   case, or the whole `flagship-*` / `L1-*` / `reachback-*` set for a layered case (mind the
   priority order — see the layered section):
   ```sh
   rm -f build/soh/mods/case*.o2r build/soh/mods/flagship-*.o2r build/soh/mods/L1-*.o2r build/soh/mods/reachback-*.o2r
   cp meta-alias-testing/o2r/case1-real-meta-target.o2r build/soh/mods/
   ```
3. (Recommended) set the log level to **Trace** so load decisions are visible — in-game
   Developer Tools → Log Level, or set CVar `gDeveloperTools.LogLevel` to `0`.
4. Launch `build/soh/soh.elf`. The first logo shown is the one under test; press **A/B/Start**
   to advance past the logos. Compare against the color legend.
5. **Restart** the game between cases (resources are cached for the life of the process), swapping
   the `mods/` contents first.

### Reading the Trace log
- Alias resolved to target: the target path (`…/fancyShipDL.xml`) is loaded.
- Fallback (case2, case4, flagship-no-`30`): the target is missing and a real `gShipLogoDL` loads
  instead — for case4 that real is soh.o2r's own (brown).

## Rebuilding the kit

```sh
./meta-alias-testing/regen-assets.sh   # regenerate src/assets from soh's real ship logo
./meta-alias-testing/build-o2r.sh      # zip src/assets subsets into o2r/*.o2r
```

## Layout
```
meta-alias-testing/
  README.md               this file
  meta-loading.md         resolution-model flowchart (source of truth)
  PLAN.md                 the implementation + test plan
  regen-assets.sh         derives src/assets/ from soh's gShipLogoDL (recolor + alias)
  build-o2r.sh            assembles o2r/*.o2r from src/assets/
  src/assets/…            generated test assets (real green, target magenta, the .meta)
  o2r/case*.o2r           the five single-mod cases
  o2r/layered/flagship-*  the 3-mod cross-game flagship set
  o2r/layered/L1-*        real-above-target set
  o2r/layered/reachback-* target-below-the-.meta set
  historical/             superseded baseline/status notes (old model)
```

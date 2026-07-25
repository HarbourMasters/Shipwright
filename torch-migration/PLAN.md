# Migrate Shipwright from ZAPDTR/OTRExporter to Torch

## Context

SoH extracts OoT assets from the user's ROM with **ZAPDTR** (a ZAPD fork) driven by
**OTRExporter**, described by 7,680 ZAPD XMLs under `soh/assets/xml/`. Both are git
submodules, and ZAPD is additionally linked *into the game binary* so users can drag a
ROM onto SoH and get an `oot.o2r` with a progress bar.

[HarbourMasters/Torch#219](https://github.com/HarbourMasters/Torch/pull/219) ("oot support",
merged 2026-07-24) landed the OoT factories in Torch. `briaguya0/zapd-to-torch-test-harness`
proves the swap is safe: **14 of 14 ROM targets extract byte-for-byte identical** to an
OTRExporter reference O2R from Shipwright `95d8f7e` (35,386–39,066 assets per ROM,
0 failed / 0 missing / 0 extra).

**Goal:** delete ZAPDTR and OTRExporter entirely; Torch becomes the only asset pipeline.
**Shape:** one big PR — `develop` is never half-migrated.

Verified compatibility that makes this viable: Torch's 0x40-byte resource header
(`src/factories/BaseFactory.cpp:3`) already matches `libultraship/src/ship/resource/ResourceLoader.cpp:246-259`,
and Torch's OoT FourCC `ResourceType`s are byte-identical to `soh/soh/resource/type/SohResourceType.h`.

---

## Decisions

| Question | Decision |
|---|---|
| ZAPDTR / OTRExporter | Full deletion |
| Torch consumption | `FetchContent`, pinned to `HarbourMasters/Torch` **`4cae44160693e1beb562e39dc301bd42278be1f9`** |
| Asset YAML home | `briaguya0/soh-asset-yml`, submoduled at `assets/` for PR reviewability; flatten into the tree post-merge. Holds yml only, and is the source of truth — not a build artifact |
| In-game extraction | Keep it — link Torch as a **static lib** (`USE_STANDALONE=OFF`) and drive `Companion` directly |
| `soh.o2r` | Small **in-tree C++ packer** (no Python — Windows devs). Long term: Torch PR adding PNG support to `Companion::Pack` |
| `ExtractAssetHeaders` | **Dropped.** Freeze the 1,085 checked-in `.h` files; follow-up Torch PR (see Blocker below) |
| Release payload | Ship the raw 111 MB YAML tree; zip it (~15 MB) in a follow-up |
| Dep collisions | Try `FetchContent` as-is on Linux + Windows first, fix only what actually breaks |

**Pin note:** `git diff --stat 9422bf4 upstream/main` = 0 lines; the harness-pinned fork
commit and `HarbourMasters/Torch` main have identical trees (`5d247f78…`). Nothing to upstream
for parity. Re-check `git diff 4cae4416 <pin>` right before opening the PR in case main moves.

---

## Blocker resolved up front: headers

`torch header` **cannot** replace `ExtractAssetHeaders`, for two independent reasons:

1. **No OoT factory registers a Header exporter.** Every `REGISTER(` in `src/factories/oot/*.h`
   is `REGISTER(Binary, ...)`. `Companion::ParseNode` skips assets whose factory lacks an
   exporter for the active `ExportType` (`Companion.cpp:882-884`), so ~10k assets — skeletons,
   limbs, animations, collision, arrays, paths, cutscenes, text, scenes, rooms — would be
   silently omitted. `gLinkAdultSkel` simply would not appear.
2. **No `#define d<sym>` line.** Torch emits only
   `static const ALIGN_ASSET(2) char sym[] = "__OTR__path";`. SoH's headers emit a pair, and
   soh source references the `d`-prefixed macros **24,698 times**. Also `#pragma once` vs
   `#ifndef OBJECTS_..._H`, no `#include "align_asset_macro.h"`, and a flat
   `headers/objects/foo.h` layout vs SoH's `objects/foo/foo.h`.

The load-bearing part — the `__OTR__objects/…/gSym` path string — already matches byte-for-byte
(that's what the harness proves). What's missing is presentation plus ~14 mechanical Header
exporters. **This PR deletes the target and keeps the `.h` files as ordinary checked-in source**
(they already are, and contain no ZAPD-derived includes beyond `align_asset_macro.h`).
`soh/assets/soh_assets.h` is hand-written and was never generated. Document loudly in the PR
that header regeneration is temporarily unavailable; file the Torch PR in parallel.

---

## Phase 0 — Verification gates (no Shipwright changes)

> **Detail: [`PHASE0.md`](PHASE0.md)** — driver source, exact commands, cost, exit criteria, and
> what Phase 0 deliberately doesn't cover.

The harness's 14/14 was measured on **one** build of Torch (`USE_STANDALONE=ON`, all 9 games,
`Debug`, driven by the CLI) — Shipwright will use a different build in a different way. Each gate
below changes exactly **one** variable against that baseline, so a failure names its own cause.
All run in `zapd-to-torch-test-harness`; nothing touches Shipwright.

**Preflight.** Re-run the existing baseline binary over all 19 ROM dumps first, so a later failure
can't be confused with environment drift.

**Gate A — static lib, driven the way SoH will drive it.** `USE_STANDALONE=OFF` plus a
`torchlib-driver/` in the harness that executes the exact `Companion` sequence Phase 3 will use
(assign `Companion::Instance`, `SetVersion`, `SetPhaseCallback`, `Init(Binary)`, try/catch, stat
the output) — so it's a dry run of `TorchExtract.cpp`, not just a link test. The real hazard:
**libgfxd is fetched and compiled only `if(USE_STANDALONE)`**. Reading says every `gfxd_*` call is
inside `#ifdef STANDALONE` (Code exporter + debug dump) and the Binary exporter is outside it — if
that reading is wrong, display lists break loudly. Also confirms the phase-callback count matches
the on-disk yml count, which is the Phase 3 progress denominator.

**Gate A2 — two extractions in one process.** Free once the driver exists; retires risk #6
(`gProcessedFiles` is never cleared, `AliasManager` isn't reset, factory-local statics) offline
instead of by hand in the game.

**Gate B — OoT-only build.** `-DBUILD_{SM64,MK64,SF64,PM64,FZERO,BK64,MARIO_ARTIST,NAUDIO}=OFF`.
Two real effects: `BUILD_BK64=OFF` drops the `BK64::TrySynthesizeRomConfig` fallback (unreachable —
all 19 hashes are in `config.yml`), and `BUILD_NAUDIO=OFF` removes Torch's only tinyxml2 consumers
(but not the unconditional tinyxml2 `FetchContent`).

**Gate C — Release build.** The 14/14 is a `Debug` measurement; CI and releases build `-O3`. Any UB
or unspecified evaluation order in Torch can change bytes between the two. Seven minutes to find
out here instead of from a user.

**Gate A′ — the shipping configuration.** Static lib + driver + OoT-only + Release, all at once.
Not implied by the others — `#ifdef` interactions are combinatorial. When it's green, that flag set
*is* the spec: Phase 2's CMake block and Phase 3's `TorchExtract.cpp` are transcriptions of it.

**Gate D — upstream pin.** Already satisfied (trees equal). Record the SHA; re-diff before opening
the PR.

**Gate E — `soh.o2r` baseline.** Rebuild `GenerateSohOtr` on current `develop` **while ZAPD still
builds** and record a sha256 manifest of all 1,042 entries (1,041 files + `portVersion`), plus the
input hashes and version that produced it. Only 31 files hit the conversion path; the risk is
quantisation, not decoding (see Phase 4).

**Nothing in Shipwright changes until A, A2, B, C, A′, E are green.**

---

## Phase 1 — The asset YAML repo

Repo: **[`briaguya0/soh-asset-yml`](https://github.com/briaguya0/soh-asset-yml)** (created, currently
empty). Submoduled at **`assets/`** (top level, sibling of `soh/`) so the eventual flatten is one
`git rm` + `git add` of a single directory. `srcdir = assets`, which is exactly where Torch expects
`config.yml`.

**It holds yml and nothing else** — no generators, no generator inputs, no manifests:

```
config.yml            19 ROM SHA1 → 14 version dirs (gbi: F3DEX2_OoT, sort: OFFSET,
                      primary_virtual_segment: 0x80, strict_declarations: true)
<14 version dirs>/    20,353 .yml, 111 MB
README.md             what this is; which harness commit produced the initial import
```

**The yml is the source of truth from day one — not a build artifact.** Once ZAPD is deleted nobody
edits the XMLs again, so there is no ongoing regeneration workflow: adding or changing an asset
definition means editing the yml directly. `zapd_to_torch.py` and its inputs (`dma/`,
`supplemental/`, the XMLs) are a **one-shot conversion**, run once to seed this repo and thereafter
kept only for reproducing or auditing that original conversion.

Those inputs stay in `zapd-to-torch-test-harness`, which already has all of them plus a `shipwright`
submodule pinned at `95d8f7e` whose tree still contains `soh/assets/xml/`, `filelists/`, and
`symbols/`. So nothing needs snapshotting or moving out of Shipwright before deletion — the harness
pin and Shipwright's own git history both preserve it. Python never enters Shipwright; SoH devs
never need it.

The harness `manifests/` remain the regression baseline: any hand-edit to the yml can be re-checked
against them, and any *intentional* divergence from OTRExporter output means regenerating the
affected manifest deliberately.

---

## Phase 2 — CMake

### Root `CMakeLists.txt`

Add **before** `add_subdirectory(libultraship)` (~L196) — ordering matters, Torch declares
`tinyxml2` and `zlib` with `OVERRIDE_FIND_PACKAGE`:

```cmake
include(FetchContent)
set(USE_STANDALONE          OFF CACHE BOOL "" FORCE)  # static lib, linkable into soh
set(PORT_VERSION_ENDIANNESS ON  CACHE BOOL "" FORCE)  # 7-byte portVersion, OTRExporter parity
set(ROM_CRC_BSWAP           OFF CACHE BOOL "" FORCE)
set(BUILD_UI                OFF CACHE BOOL "" FORCE)  # would FetchContent a 2nd libultraship
set(BUILD_STORMLIB          OFF CACHE BOOL "" FORCE)  # MANDATORY: target-name clash with LUS's `storm`
set(BUILD_OOT               ON  CACHE BOOL "" FORCE)
foreach(g SM64 MK64 SF64 PM64 FZERO BK64 MARIO_ARTIST NAUDIO)
    set(BUILD_${g} OFF CACHE BOOL "" FORCE)
endforeach()
FetchContent_Declare(torch
    GIT_REPOSITORY https://github.com/HarbourMasters/Torch.git
    GIT_TAG        4cae44160693e1beb562e39dc301bd42278be1f9)
FetchContent_MakeAvailable(torch)
```

Remove: `add_subdirectory(ZAPDTR/ZAPD …)` (L199), `add_subdirectory(OTRExporter)` (L200), both
`install(TARGETS ZAPD …)` (L209, L292), the `soh/assets/extractor` + `soh/assets/xml` install
rules (L210-211, 215-216, 296-297), `find_package(Python3 …)` (L219), and the three custom
targets (L222-261). Fix the stale L178 comment ("to use libgfxd from ZAPDTR" — LUS FetchContents
libgfxd itself, so the Gfx debugger is unaffected).

### Two new in-tree tools

Torch can't be added twice, and the static-lib build has no `main` (`int main` is inside
`#if defined(STANDALONE)`, while `Companion* Companion::Instance;` at `src/main.cpp:15` is
deliberately *outside* it — so the lib gets the singleton storage and no entry point).

- **`tools/torch-cli/`** → target `soh-torch`. ~40 lines: parse `--src/--dest/--version/--rom`,
  construct `Companion`, `SetVersion`, `Init(ExportType::Binary)`. Links `torch`. One Torch build
  serves both the game link and the build-time targets; CI never needs the upstream CLI.
- **`tools/soh-o2r-packer/`** → see Phase 4.

### Replacement targets

```cmake
add_custom_target(ExtractAssets
    COMMAND ${CMAKE_COMMAND} -E rm -f oot.o2r oot-mq.o2r
    COMMAND $<TARGET_FILE:soh-torch> --src ${CMAKE_SOURCE_DIR}/assets
            --dest ${CMAKE_BINARY_DIR}/soh --version ${CMAKE_PROJECT_VERSION} --rom ${SOH_ROM_PATH}
    DEPENDS soh-torch)

add_custom_target(GenerateSohOtr
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_SOURCE_DIR}/soh/assets/custom/shaders/
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/libultraship/src/fast/shaders/ ${CMAKE_SOURCE_DIR}/soh/assets/custom/shaders/
    COMMAND $<TARGET_FILE:soh-o2r-packer> ${CMAKE_SOURCE_DIR}/soh/assets/custom
            ${CMAKE_BINARY_DIR}/soh/soh.o2r ${CMAKE_PROJECT_VERSION}
    DEPENDS soh-o2r-packer)
# ExtractAssetHeaders: DELETED
```

The LUS-shader copy must be preserved verbatim — it's why `soh/assets/custom/shaders/` exists and
it currently lives inside both deleted targets.

### `soh/CMakeLists.txt`

- L107-109: delete the `if (NOT TARGET ZAPDLib) add_subdirectory(../ZAPDTR/ZAPD …)` block. It's a
  fallback for configuring `soh/` directly rather than from the repo root (mirroring the
  libultraship guard just above it); normally the root's L199 has already created the target. With
  Torch coming from the root's `FetchContent` there's nothing for it to fall back to, so it just
  goes away. The renames at L624/642/662/704 are the parts that matter.
- L325: delete the dead `../ZAPDTR/ZAPD/resource/type` include — **that directory doesn't exist**.
- L599-612: swap the `soh/assets/extractor` + `soh/assets/xml` POST_BUILD copies for a copy of
  `${CMAKE_SOURCE_DIR}/assets`; drop the ZAPD-only `assets/symbols` mkdir.
- L624/642/662/704: `ZAPDLib` → `torch`, keeping the existing
  `NOT CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch|CafeOS"` guard — **Switch and Wii U don't link
  the extractor today and must not link Torch either.**
- Add `soh/soh/Extractor/TorchExtract.cpp`.

### Known collision landmines (fix only if they fire)

| Dep | Risk |
|---|---|
| **zlib** | Torch `FetchContent_Declare(zlib … OVERRIDE_FIND_PACKAGE)` unconditionally and links `zlibstatic` — but `grep zlib.h torch/src` = **0 hits**; it's only reachable via StormLib, which we disable. Redirects every later `find_package(ZLIB)` (libpng, libzip, SDL) and defines `zlibstatic` but not `ZLIB::ZLIB`. **Highest risk.** Best fix is a one-line Torch PR gating the fetch on `BUILD_STORMLIB OR BUILD_UI`. |
| **stb** | `lib/n64graphics/CMakeLists.txt:13-31` renames `stbi_write_*` → `n64g_stbi_write_*` **only `if(BUILD_UI)`**. `stbi_load*` is never renamed. LUS links its own `stb`. |
| **StringHelper / StrHash64** | Torch excludes its copies **only `if(BUILD_UI)`** (`CMakeLists.txt:146-152`), citing exactly this clash. SoH has the same files in `libultraship/include/ship/utils/`. |
| **tinyxml2** | `OVERRIDE_FIND_PACKAGE` hijacks LUS's `find_package(tinyxml2 REQUIRED)`. Torch fetches 10.0.0 — the same version `.github/actions/install-tinyxml2` installs, so declaring Torch first is likely *beneficial* (one copy instead of two). All Torch tinyxml2 consumers are `naudio`, which we disable. |
| **spdlog** | `find_package(spdlog QUIET)` runs first and SoH always has vcpkg spdlog, so no ABI clash. But Torch calls `spdlog::set_level`/`set_pattern` on the **global default logger** from the worker thread (`Companion.cpp:193-194, 1810-1814`), clobbering SoH's config at `OTRGlobals.cpp:808-815`. Save/restore around the call. |
| **storm** | Hard CMake error if `BUILD_STORMLIB=ON` (LUS already creates a `storm` target, and `INCLUDE_MPQ_SUPPORT` is ON at root L186). Avoided by keeping it OFF; `.o2r` uses vendored miniz anyway. |

---

## Phase 3 — In-game extractor rewrite

`Extractor::CallZapd` (`soh/soh/Extractor/Extract.cpp:641`) currently chdirs into a temp dir with
`assets/` symlinked and hand-builds 22 argv entries for
`extern "C" zapd_report(argc, argv, extractCount, totalExtract)`. Everything that needed the chdir
becomes constructor arguments — `grep current_path torch/src` hits only `main.cpp` (STANDALONE) and
`LusBackend.cpp` (BUILD_UI), so **Torch has no CWD dependence** when both `srcdir` and `destdir`
are passed.

**Isolate Torch in its own TU.** `Companion.h` pulls in yaml-cpp, `n64/Cartridge.h`, a bare
`CONTAINS` macro, and the `torch` target exports Torch's `src/` **and** `lib/` (CLI11, TinySHA1,
`hj/`, `nposix/`) as PUBLIC includes. Don't mix that into `Extract.cpp` alongside game headers.

- `soh/soh/Extractor/TorchExtract.h` — declarations only, no Torch types:
  `SohTorch::CountAssetFiles(ymlDir)` and
  `SohTorch::Extract(rom, srcDir, destDir, portVersion, std::atomic<size_t>* progress)`.
- `soh/soh/Extractor/TorchExtract.cpp` — the only file that includes `Companion.h`.

**Flow:** `srcDir = <installPath>/assets`; `destDir = Mkdtemp()` (reuse `Extract.cpp:619`);
`totalExtract` = recursive count of `*.yml` under `srcDir/<versionDir>` minus `config.yml`;
run; then `rename(destDir/oot.o2r → exportdir/(IsMasterQuest() ? "oot-mq.o2r" : "oot.o2r"))`;
`remove_all(destDir)`.

**Driving Companion:**
```cpp
auto* c = new Companion(fs::path(rom), ArchiveType::O2R, /*debug*/false, srcDir, destDir);
Companion::Instance = c;          // bare singleton, no getter — factories deref it unconditionally
c->SetVersion(portVersion);
c->SetPhaseCallback([progress](int){ if (progress) ++*progress; });
c->Init(ExportType::Binary);      // Init IS the whole run; it calls Process() internally
```

- **Must `try/catch`.** 127 `throw std::runtime_error` across `torch/src`; nothing catches at the
  top level, so an escape means `std::terminate`. The existing `extractionTask->get()` try/catch
  at `OTRGlobals.cpp:730-734` turns a rethrow into the "Extraction Crashed" popup.
- **Must also stat the output.** `Process()` returns `void` and several fatal conditions just log
  and `return` — no `config.yml` (`Companion.cpp:1304`), no config entry for the ROM hash (`1334`),
  no `config:` node (`1390`), bad GBI (`1484`). A ROM dump outside the 17-hash table produces no
  archive and no exception.
- **Fresh `Companion` per call.** `gProcessedFiles` (`Companion.h:300`) is never cleared — a second
  `Process()` on the same instance skips *every* file. `gAddrMap`/`gParseResults`/`gWriteMap` also
  accumulate. `RunExtract` can extract more than once per process (`PS_FIRST`→`PS_SECOND`,
  `ES_EXTRACT_ARGS` loops over CLI args), so allocate anew and reassign `Companion::Instance`.
  Watch: `AliasManager::Instance` is not cleared at end of `Process` (inert for OoT — no OoT factory
  calls `Register` — but unproven), and `AudioManager::Instance` is leaked per run (`Companion.cpp:1584`).

**Progress: use `SetPhaseCallback`, not `Init`'s atomic overload.** `ProcessFile`
(`Companion.cpp:1282-1293`) resets `*gAssetCounter = 0` and recomputes the total **once per YAML
file**, so with ~1,450 YAMLs a naive bar restarts 1,450 times. The phase callback has exactly one
call site (`gPhaseCallback(2)` at `Companion.cpp:1282-1283`), fired once per YAML at the
parse→export transition, after `NodeHasChanges` has already let the file through — a 1:1 match for
a denominator computed by counting files on disk. Sub-files from `AddSubFileAsset` are virtual names
never present on disk, so the mapping is exact. `NodeHasChanges` early-returns are impossible
because a fresh temp destdir has no `torch.hash.yml`. **Nothing in the ImGui progress bar
(`OTRGlobals.cpp:750-755`) needs to change.** `GetCurrentAssetName()` is available for a label.

**The temp destdir is mandatory, not cosmetic.** `Process` unconditionally writes
`destdir/torch.hash.yml` (`Companion.cpp:1804`) and reads it back next run (`:708`). Extracting
straight into the app dir would litter it *and* make a second in-session extraction skip files it
thinks are unchanged.

**MQ naming:** leave `config.yml` at `binary: oot.o2r` for all 19 hashes and rename on the way out
of the temp dir based on `IsMasterQuest()`. Torch offers no output-filename override
(`-d` sets only the directory; you'd get a *directory* named `oot-mq.o2r`), and this keeps SoH the
single source of truth for MQ-ness and preserves the harness's `<out>/oot.o2r` expectation.

**Delete:** `CallZapd` (Extract.cpp:640-696) and its decl in `Extract.h:66-67`, the `zapd_report`
extern, and `GetZapdVerStr()` (`:583`) — replaced by a `GetTorchVersionDir()` returning
`pal_gc`/`ntsc_u_gc`/… over the same CRC mapping. `Mkdtemp()` stays.

---

## Phase 4 — The `soh.o2r` packer

`tools/soh-o2r-packer/`, `target_link_libraries(soh-o2r-packer PRIVATE torch)`. Reusing `torch`
gets `ZWrapper` (miniz — the same zip writer that produces `oot.o2r`, so archives stay structurally
consistent), `Companion::ParseVersionString` for the 7-byte `portVersion`, and `stb_image` via the
vendored `N64Graphics`. No new dependency, no Python, works on Windows out of the box.

Usage: `soh-o2r-packer <custom-assets-dir> <out.o2r> <M.m.p>`. Behaviour ported from
`OTRExporter/OTRExporter/Main.cpp:206-260`:

1. **`<name>.<fmt>.png`** (`rgba32|rgb5a1|i4|i8|ia4|ia8|ia16|ci4|ci8`) → convert; archive path is
   the relative path with `.<fmt>.png` stripped. Payload = the 0x40 resource header
   (endianness `0x00`, resType `Texture`, version 0, id `0xDEADBEEF…`, zero-padded) then
   `u32 textureType, width, height, rawDataSize`, then the raw N64 bytes.
   **Reproduce ZAPD's asymmetry:** the size field is the unpadded `GetRawDataSize()` while the
   buffer written is `ALIGN8`-padded.
2. **Path contains `accessibility`** → `.json` added with full relative path; non-`.json` **skipped**.
   All 12 files are `.json` today, so this is a no-op — replicate it anyway to avoid silent drift.
3. **Everything else** → raw byte copy, full relative path.
4. `portVersion` only. **No `version` file** — OTRExporter doesn't write one for `soh.o2r`, and
   `sohArchiveVersionMatch` (`OTRGlobals.cpp:282-283`) only reads `portVersion`.

**Critical: use ZAPD's quantisation, not n64graphics'.** `n64graphics`' `ia2raw` uses
`SCALE_8_4(x)` (`x*15/255`) where ZAPD uses `(x >> 4) & 0xF` — for `r = 0xF0` those give `0xE`
vs `0xF`, **different bytes**. Same for IA4. Decode with `stbi_load(path, &w, &h, &n, 4)`, then
hand-port ~80 lines from `ZAPDTR/ZAPD/ZTexture.cpp`: rgba32 (`:507`), ia16 (`:595`, `[r, a]`),
ia8 (`:578`, `((r>>4)&0xF)<<4 | ((a>>4)&0xF)`), ia4 (`:552`, `((r>>5)<<1) | (a != 0)`), plus
i4/i8/rgba16/ci4/ci8 for completeness so adding a texture later doesn't silently produce garbage.

Only 31 of 1,041 files hit the conversion path (15 rgba32, 8 ia8, 5 ia16, 3 ia4); the rest — 923
objects, 23 `.rgba16`, 22 json, 4 ttf, 2 xml, ~40 plain `.png` — are raw copies.

**Acceptance:** sha256 of every one of the 1,041 entries against the Gate E manifest.

**Long term:** the Torch PR is `Companion::Pack` gaining PNG conversion + a version file, after
which this collapses to `torch pack soh/assets/custom soh.o2r o2r -u 9.2.3`. Structure the format
table so it lifts into Torch wholesale.

---

## Phase 5 — Deletions

Pure deletion — **nothing is moved anywhere.** Every generator input is already preserved by the
harness's `shipwright @ 95d8f7e` submodule pin and by Shipwright's own git history, and the yml is
the source of truth going forward (Phase 1).

- **Submodules:** `ZAPDTR/` (1.9 MB), `OTRExporter/` (71 MB) — `.gitmodules` + `git rm`
- **`soh/assets/xml/`** — 7,680 files, 54 MB
- `soh/assets/extractor/Config_*.xml` (14) and `TexturePool.xml` — ZAPD `-rconf` inputs, no runtime use
- `soh/assets/extractor/filelists/`, `soh/assets/extractor/symbols/` — ZAPD `-fl` input and
  `Config_*.xml` references respectively; verified no runtime reader in `soh/soh` or `soh/src`
- `copy-existing-otrs.cmake`
- CMake and source hunks listed in Phases 2–3

**Keep:** all 1,085 `soh/assets/**/*.h`, all 1,041 files under `soh/assets/custom/`.

Net: −127 MB submodules, −54 MB XML, +111 MB YAML (submodule initially; ~7-15 MB packed).

---

## Phase 6 — CI

`.github/workflows/generate-builds.yml`:

- **`generate-soh-otr` (L10-68)** does a full SoH configure — SDL2 from source, SDL2_net,
  tinyxml2 from source, ccache, apt deps — purely to build `GenerateSohOtr`. The new packer has
  no SDL/LUS dependency, so add a `SOH_TOOLS_ONLY` root guard that skips
  `add_subdirectory(libultraship)`/`add_subdirectory(soh)` and configures only Torch +
  `soh-o2r-packer`. Saves ~5-8 min per run and drops the `install-tinyxml2` dependency. Add a step
  asserting the `soh.o2r` content manifest.
- **Platform build jobs** (`build-macos` L71, `build-linux`, `build-windows`, Switch, Wii U):
  `submodules: true` already covers the new `assets/` submodule (it has no nested submodules).
  Switch/Wii U must not build Torch — same `NintendoSwitch|CafeOS` guard as `ZAPDLib` today.
  Package payload changes from `assets/xml/` to `assets/`.
- **`test-builds-on-distros.yml` L137-150** removes distro tinyxml2 and builds 10.0.0 from source.
  **Leave it in this PR** — one variable at a time; delete in a follow-up once Torch's fetched
  tinyxml2 is confirmed to satisfy LUS.
- `pr-artifacts.yml:51` already filters `soh.o2r`; no change.

Optional follow-up: a CI job running Gate A/B against a checked-in manifest so Torch pin bumps are
validated automatically. Needs a ROM, so self-hosted or encrypted secret.

---

## Risks, ranked

1. **zlib double-link** — Torch links an unused zlib and `OVERRIDE_FIND_PACKAGE` hijacks the whole
   build's `find_package(ZLIB)`. Most likely thing to break the link, especially on Windows/vcpkg-static.
2. **stb / StringHelper / StrHash64 duplicate symbols** — Torch's own guards for these are
   `if(BUILD_UI)`, which we don't set.
3. **`soh.o2r` texture byte-identity** — libpng→stb_image plus the `SCALE_8_4` vs `>>4` trap.
   Fully testable offline (Gate E); de-risk first, costs an afternoon.
4. **Windows build** — Torch forces `/MT`, adds `-DSTORMLIB_NO_AUTO_LINK`, `/bigobj`, and carries a
   `cmake_minimum_required(3.12)` + policy shim under a CMake 4 host. The one platform not
   predictable from reading. Build it early.
5. **Extraction wall-clock** — *low.* Significant perf work landed in #219, and the yml carrying full
   declarations (rather than relying on autodiscovery) is part of why. The harness measures ~19 s per
   ROM. Still worth a before/after measurement vs ZAPD, but not expected to regress.
6. **Re-entrancy across two in-session extractions** — `AliasManager` uncleaned plus file-scope
   statics in `TextureFactory.cpp:14-15`, `CompressedTextureFactory.cpp:17-18`,
   `DisplayListFactory.cpp:82`. Believed inert for OoT/Binary; only a real vanilla-then-MQ run proves it.
7. **spdlog global-state stomping** from Torch's worker thread.
8. **Header regeneration gap** until the follow-up Torch PR lands.
9. **111 MB submodule ergonomics** — clone time, and a flatten commit GitHub will refuse to render.
10. **`Companion::Instance` is a raw global** — `RunExtract` uses a 1-thread pool so it's serialised
    today; add a comment/assert.

---

## Verification

**Archive level**
1. Gates A, B, A′, E (Phase 0) — 14/14, `0 failed / 0 not generated / 0 not in reference`.
2. Post-integration: run Shipwright's `soh-torch` over all 14 ROMs and feed the outputs to the
   harness's `check.sh`/`test_assets.py`. This proves the *Shipwright-configured* Torch
   (FetchContent, OoT-only, static lib, SoH's flags) matches — not just the harness build.
3. `soh.o2r`: full 1,041-file sha256 diff vs an OTRExporter archive from `develop` HEAD.
4. Diff the in-game extractor's `oot.o2r` against `soh-torch`'s for the same ROM — different code
   paths (temp destdir, phase callback), must agree byte-for-byte.

**Build level**
5. Clean configure + build on Linux (gcc, clang), macOS (universal), Windows (MSVC x64), Switch,
   Wii U. Switch/Wii U build with Torch excluded.
6. Confirm exactly one tinyxml2 and one zlib in the final link (`nm`/`dumpbin` for duplicate
   `inflate`; `cmake --graphviz` for target duplication).
7. Confirm `soh` links with the untouched checked-in `.h` files after ZAPDTR is gone.

**Run the game**
8. Cold start with no archives: ROM picker → progress bar (must be **monotonic 0→100, not
   sawtooth**) → "Extraction Complete" → boots.
9. **Two-ROM session** — extract vanilla, answer "Yes" to extract another, extract MQ in the same
   process. Highest-value manual test: covers re-entrancy and both output names.
   ✅ **Passed 2026-07-25.** Both extracted, both loaded and played. Closes risk #6 in the game
   itself, not just offline (Gate A2), and confirms `config.yml` naming produces `oot.o2r` and
   `oot-mq.o2r` with no rename step in SoH.
10. CLI path: `./soh <rom.z64>` → `ES_EXTRACT_ARGS`.
11. Failure paths: non-OoT ROM and a truncated ROM → clean popup, not a hang or half-written `.o2r`.
12. No litter: no `torch.hash.yml`, no temp dir surviving in the app directory.
13. Gameplay: boot vanilla and MQ saves, load a dungeon (scene/room/collision), open the pause menu,
    check PNG-path textures (`gSun`, `gTriforcePiece`, `gDPad`), play a cutscene and a sequence,
    and confirm the `ValidHashes` check at `OTRGlobals.cpp:941` accepts the new archives.
14. Build a CPack artifact per platform, unzip fresh, run from that directory, re-extract — catches
    broken install rules for the assets payload.

---

## Ordering

```
0.  Torch PR: gate the zlib fetch, make stb/StringHelper guards unconditional  [only if they fire]
1.  Harness: preflight, then Gates A, A2, B, C, A′  (see PHASE0.md)
2.  Seed briaguya0/soh-asset-yml: one-shot conversion output (config.yml + 14 version dirs)
3.  Gate E: soh.o2r manifest from current develop
--- single Shipwright PR from here ---
4.  Add assets submodule; FetchContent Torch before libultraship
5.  tools/torch-cli + tools/soh-o2r-packer
6.  Replace ExtractAssets / GenerateSohOtr; delete ExtractAssetHeaders
7.  TorchExtract.{h,cpp} + Extract.{h,cpp} rewrite
8.  Delete ZAPDTR, OTRExporter, soh/assets/xml, extractor inputs
9.  CI: generate-soh-otr slim-down, packaging payload
10. Full verification matrix
--- follow-ups ---
11. Torch PR: OoT Header exporters + `#define d<sym>` → restore ExtractAssetHeaders
12. Torch PR: PNG support in Companion::Pack → retire soh-o2r-packer
13. Zip the shipped YAML (111 MB → ~15 MB)
14. Flatten the assets submodule into the tree
```

## Critical files

- `CMakeLists.txt` (178-179, 196-201, 208-216, 219-261, 292-297)
- `soh/CMakeLists.txt` (107-109, 325, 599-612, 624/642/662/704)
- `soh/soh/Extractor/Extract.cpp` (`CallZapd` 640-696, `GetZapdVerStr` 583, `Mkdtemp` 619), `Extract.h:66-67`
- `soh/soh/OTRGlobals.cpp` (`RunExtract` 398, extraction task 596-680, progress bar 750-755, ValidHashes 941)
- `OTRExporter/OTRExporter/Main.cpp:183-268` and `ZAPDTR/ZAPD/ZTexture.cpp:507-620` — the spec for Phase 4
- Torch `src/Companion.{h,cpp}` (`Init` 185-395, `Process` 1299-1819, `Pack` 1821, `torch.hash.yml`
  708/1804, phase callback 1282-1293), `src/main.cpp:15`, `CMakeLists.txt` (21-37, 146-152, 228-235,
  402-428), `lib/n64graphics/CMakeLists.txt:13-31`

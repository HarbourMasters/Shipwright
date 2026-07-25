# Phase 0 — Verification gates

Detail for [`PLAN.md`](PLAN.md) Phase 0. **Nothing in Shipwright changes until every gate here is
green.** All work happens in
[`briaguya0/zapd-to-torch-test-harness`](https://github.com/briaguya0/zapd-to-torch-test-harness).

---

## Why there are gates at all

The harness proves 14/14 byte-for-byte parity — but for exactly **one** build of Torch, driven
exactly **one** way:

| | Harness measured | Shipwright will use |
|---|---|---|
| Build kind | executable (`USE_STANDALONE=ON`) | static lib (`OFF`) |
| Games compiled in | all 9 (defaults) | OoT only |
| Build type | `Debug` (`-g`) | `Release` (`-O3`) |
| Driver | `main.cpp` + CLI11 | our own `Companion` calls |
| Extractions per process | one | up to two (vanilla, then MQ) |
| `soh.o2r` | n/a (OTRExporter makes it) | a new in-tree packer |

Every row is an unproven variable. Each gate closes exactly one of them, **one at a time**, so a
failure names its own cause. Running only the final combination would tell us "something broke"
without saying what.

Reading the source says all six should be fine. Reading is how we got here; running is how we know.

---

## Preconditions

- Everything builds inside the **`soh` distrobox** (`distrobox enter soh`) — `cmake`, `ninja`, and
  the compilers are not on the host `PATH`.
- Harness at `~/code/zapd-to-torch-test-harness`, submodules initialised.
- `roms/` populated: **19 ROM dumps → 14 version directories** (`ntsc_1-0`, `ntsc_1-1` and
  `ntsc_1-2` have two dumps each; `pal_mq_dbg` has three).
  *(`PLAN.md` says 17 hashes in a couple of places — it's 19. Corrected there.)*
- `assets/yml/` generated: 20,353 `.yml` / 119 MB across the 14 version dirs, plus the committed
  `config.yml`. Gitignored — regenerate with `zapd_to_torch.py` if absent.
- `o2r/` holds all 19 OTRExporter reference archives (one per ROM dump, from Shipwright `95d8f7e`).
- Existing baseline build at `torch/build/` (`USE_STANDALONE=ON`, all games ON, `Debug`,
  `PORT_VERSION_ENDIANNESS=ON`) — leave it alone; it *is* the control.

---

## How the gates are actually run

**Do not use `tools/test_assets.py` for these gates.** It hardcodes `TORCH` to
`torch/build/torch`, and it copies a *filtered subset* of the yml into a scratch dir (1,320 of
1,450 files for `pal_gc`) — which is not the invocation SoH will make. It's the right tool for
bisecting a single failing asset, not for proving a configuration.

Use the **full-tree + `check.sh`** path instead. It points Torch at the entire yml tree — the same
shape as Phase 1's `srcdir = assets/` — and diffs the two archives file-by-file:

```sh
OUT=$(mktemp -d)                                   # fresh every run — see below
"$TORCH_BIN" o2r -s assets/yml -d "$OUT" -u 9.2.3 roms/<rom>.z64
cp "$OUT/oot.o2r" o2r/torch.o2r
cp "o2r/<rom>.o2r" o2r/reference.o2r
./check.sh                                          # missing / extra / mismatched, all three
```

`check.sh` never invokes Torch, so it takes any binary — Gate A's driver drops straight in with no
harness patching.

> **The fresh destdir is mandatory, not hygiene.** `Process()` writes `destdir/torch.hash.yml` and
> reads it back on the next run to skip unchanged files. Reuse a destdir and the second run
> silently produces a partial archive. (This is the same constraint that forces Phase 3's
> `Mkdtemp()`.)

### New: `tools/matrix.sh`

One script, used by every gate:

```sh
tools/matrix.sh <torch-bin> <label>
```

- loops all 19 ROM dumps, fresh destdir each
- writes `logs/matrix-<label>/<rom>.log`
- prints a 19-row PASS/FAIL table and exits non-zero on any failure

Cost: ~20 s per ROM (19 s of that is Torch), so **~7 min per configuration**. Four configurations
plus preflight is well under an hour of wall-clock; the builds dominate.

---

## Preflight — reproduce the known-good result

Before changing a single variable, re-run the *current* configuration on this machine, today:

```sh
tools/matrix.sh torch/build/torch baseline
```

Expect 19/19 `PASS: All N files match!`. Without this, a later gate failure is ambiguous between
"the variable broke it" and "something drifted in the environment."

Also confirm the reference archives are what we think they are (the README claims this for
`pal_gc`; spot-check one more):

```sh
./manifest.sh o2r/ntsc_u_gc_b82710.o2r /tmp/regen.json
diff <(jq -S . manifests/ntsc_u_gc.json) <(jq -S . /tmp/regen.json) && echo MATCH
```

---

## Gate D — the upstream pin *(already green)*

`HarbourMasters/Torch` main and the harness's pinned fork commit `9422bf4` have identical trees
(`5d247f78…`; `git diff --stat` = 0 lines). Nothing needs upstreaming for parity.

**Action:** record the merge SHA `4cae44160693e1beb562e39dc301bd42278be1f9` and re-run
`git diff 4cae4416 9422bf4` immediately before opening the Shipwright PR, in case main has moved.
No build, no matrix run.

---

## Gate A — static lib, driven the way SoH will drive it

**Variable:** `USE_STANDALONE=OFF` *and* the hand-rolled driver, together.

They're one gate on purpose. Testing "does the static lib link" without also testing "does driving
`Companion` by hand produce the same bytes" would leave the more interesting half unproven — and
the driver has to exist either way, because `USE_STANDALONE=OFF` compiles out `int main`.

### The driver *is* a dry run of Phase 3

`torchlib-driver/main.cpp` is not throwaway scaffolding. It executes the exact sequence
`soh/soh/Extractor/TorchExtract.cpp` will use, so if any assumption in Phase 3 is wrong, it fails
here — offline, against a reference archive — instead of in the game with a progress bar spinning.

```
zapd-to-torch-test-harness/
  torchlib-driver/
    CMakeLists.txt
    main.cpp
```

**`CMakeLists.txt`** — note it deliberately does *not* set the `BUILD_<game>` flags or the build
type; those are Gate B's and Gate C's variables and get passed on the command line:

```cmake
cmake_minimum_required(VERSION 3.26)
project(torchlib-driver CXX)
set(CMAKE_CXX_STANDARD 20)

set(USE_STANDALONE          OFF CACHE BOOL "" FORCE)
set(PORT_VERSION_ENDIANNESS ON  CACHE BOOL "" FORCE)
set(ROM_CRC_BSWAP           OFF CACHE BOOL "" FORCE)
set(BUILD_UI                OFF CACHE BOOL "" FORCE)
set(BUILD_STORMLIB          OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../torch ${CMAKE_BINARY_DIR}/torch)
add_executable(torchlib-driver main.cpp)
target_link_libraries(torchlib-driver PRIVATE torch)
```

`torch` exports `src/`, `lib/`, and the yaml-cpp includes as `PUBLIC` under `USE_STANDALONE=OFF`
(`torch/CMakeLists.txt:437-442`), and spdlog becomes `PUBLIC` too — so nothing else is needed.

**`main.cpp`** — argv-compatible with `torch o2r -s S -d D -u V <rom>` so `matrix.sh` can't tell
the two binaries apart:

```cpp
#include <atomic>
#include <cstdio>
#include <filesystem>
#include <string>
#include "Companion.h"

static int RunOnce(const std::string& rom, const std::string& src,
                   const std::string& dest, const std::string& ver) {
    std::atomic<size_t> phases{ 0 };
    try {
        auto* c = new Companion(std::filesystem::path(rom), ArchiveType::O2R, false, src, dest);
        Companion::Instance = c;              // bare global; factories deref it unconditionally
        c->SetVersion(ver);
        c->SetPhaseCallback([&phases](int) { ++phases; });
        c->Init(ExportType::Binary);          // Init is the whole run; it calls Process() itself
    } catch (const std::exception& e) {
        fprintf(stderr, "driver: exception: %s\n", e.what());
        return 2;
    }
    if (!std::filesystem::exists(std::filesystem::path(dest) / "oot.o2r")) {
        fprintf(stderr, "driver: no oot.o2r produced\n");   // Process() returns void and has
        return 3;                                           // several log-and-return failure paths
    }
    fprintf(stderr, "driver: phases=%zu\n", phases.load());
    return 0;
}
```

…plus ~20 lines of argv parsing and, for Gate A2, a `--second <rom> <dest>` flag that calls
`RunOnce` a second time in the same process.

Four Phase 3 claims this proves or refutes:

1. **`Companion::Instance` can be assigned by hand.** It's a raw global defined at
   `src/main.cpp:15`, *outside* the `#if defined(STANDALONE)` guard — so the static lib carries the
   storage but no entry point. Deliberate, but untested from outside.
2. **`try/catch` is sufficient.** 127 `throw std::runtime_error` sites across `torch/src` and
   nothing catches at the top level; an escape is `std::terminate`, not a popup.
3. **Statting the output is necessary.** `Process()` returns `void` and just logs-and-returns on
   missing `config.yml` (`Companion.cpp:1304`), unknown ROM hash (`:1334`), no `config:` node
   (`:1390`), bad GBI (`:1484`). A silent no-archive must be caught.
4. **`SetPhaseCallback` is a usable progress denominator.** One call site
   (`gPhaseCallback(2)`, `Companion.cpp:1282-1283`), fired once per yml file at the parse→export
   transition. Compare the printed `phases=` against
   `find assets/yml/<version> -name '*.yml' | wc -l` (1,449–1,480 depending on version). If they
   match, the Phase 3 progress bar is monotonic 0→100; if they don't, the denominator theory is
   wrong and it's better to know now.

### What Gate A is hunting

**libgfxd.** This is the one that could actually fail. `torch/CMakeLists.txt:60-81` fetches and
compiles libgfxd **only `if(USE_STANDALONE)`** — under `OFF` it isn't in the build at all. Reading
the source says that's safe: every `gfxd_*` call sits inside `#ifdef STANDALONE`
(`DisplayListFactory.cpp:81-199` = `DListCodeExporter` + `DebugDisplayList`;
`DisplayListOverrides.cpp:18-224` = the `GFXDOverride` callbacks), while `DListBinaryExporter`
(`:239`) and `DListFactory::parse` (`:526`) are outside it. Display lists are the largest asset
class in the archive, so if that reading is wrong the gate fails immediately and unmistakably.

**Other `STANDALONE` deltas, all expected inert:**

- `Companion.cpp:1524` gates `enums:` parsing — `config.yml` has no `enums:` key.
- `DListCodeExporter` isn't registered — we export `Binary`.
- yaml-cpp gains `YAML_CPP_STATIC_DEFINE`; spdlog moves `PRIVATE`→`PUBLIC`; the standalone-only
  `find_package(ZLIB REQUIRED)` disappears (`zlibstatic` from FetchContent remains).

**One landmine worth writing down:**
`if(NOT USE_STANDALONE AND EXISTS "/mnt/c/WINDOWS/system32/wsl.exe")` (`torch/CMakeLists.txt:295`)
FetchContents Microsoft GSL. It fires **only inside WSL**, and only in the static-lib
configuration — i.e. exactly what Shipwright will use. Irrelevant in the distrobox; it will bite
the first person who builds SoH under WSL after this lands.

### Running it

```sh
cmake -S torchlib-driver -B torchlib-driver/build -GNinja -DCMAKE_BUILD_TYPE=Debug
cmake --build torchlib-driver/build -j
tools/matrix.sh torchlib-driver/build/torchlib-driver static-lib
```

**Pass:** 19/19 identical, and `phases=` matches the yml count for each version.
Smoke-test `pal_gc` alone before committing to the full matrix.

---

## Gate A2 — two extractions in one process

**Variable:** re-entrancy. Free to add once the driver exists, and it retires
[`PLAN.md`](PLAN.md) risk #6 offline instead of by hand in the game.

`RunExtract` can extract more than once per process (`PS_FIRST` → `PS_SECOND`, and
`ES_EXTRACT_ARGS` loops over CLI args), so a real user extracting vanilla and then MQ in one
session hits state SoH has never exercised:

- `gProcessedFiles` (`Companion.h:300`) is **never cleared** — a second `Process()` on the *same*
  instance skips every file. Phase 3's answer is a fresh `Companion` per call; this gate is what
  proves that's enough.
- `gAddrMap` / `gParseResults` / `gWriteMap` also accumulate.
- `AliasManager::Instance` isn't cleared at the end of `Process` (believed inert for OoT — no OoT
  factory calls `Register` — but unproven).
- `AudioManager::Instance` leaks per run (`Companion.cpp:1584`).
- File-scope statics in `TextureFactory.cpp:14-15`, `CompressedTextureFactory.cpp:17-18`,
  `DisplayListFactory.cpp:82`.

```sh
tools/matrix.sh --pair pal_gc_0227d7 pal_mq_f46239 <driver> reentrancy
```

One process, two fresh `Companion`s, two destdirs; both archives compared against their own
references. **Pass:** both identical — specifically, the *second* one must be identical, which is
the whole point.

---

## Gate B — OoT-only build

**Variable:** the eight non-OoT `BUILD_<game>` flags. Still `USE_STANDALONE=ON`, still `Debug`, so
this is measured against the preflight baseline with exactly one thing changed.

```sh
cmake -S torch -B torch/build-oot -GNinja -DCMAKE_BUILD_TYPE=Debug \
      -DPORT_VERSION_ENDIANNESS=ON \
      -DBUILD_SM64=OFF -DBUILD_MK64=OFF -DBUILD_SF64=OFF -DBUILD_PM64=OFF \
      -DBUILD_FZERO=OFF -DBUILD_BK64=OFF -DBUILD_MARIO_ARTIST=OFF -DBUILD_NAUDIO=OFF
cmake --build torch/build-oot -j
tools/matrix.sh torch/build-oot/torch oot-only
```

Each `BUILD_X=OFF` drops `-DX_SUPPORT` and filters `src/factories/x/*` out of the source glob
(`torch/CMakeLists.txt:158-207`). Two effects are real:

- **`BUILD_BK64=OFF`** removes `BK64::TrySynthesizeRomConfig`, Companion's fallback for a ROM hash
  absent from `config.yml`. All 19 of ours are present, so it's unreachable here — but it means an
  *unknown* dump behaves differently, which is precisely why Phase 3 must stat the output rather
  than trust the run.
- **`BUILD_NAUDIO=OFF`** removes Torch's only tinyxml2 consumers. It does **not** remove the
  tinyxml2 `FetchContent` — that's unconditional — so Phase 2's `OVERRIDE_FIND_PACKAGE` concern
  stands regardless.

Nothing else should touch a shared path. A cross-game factory registration or a shared header
behind an `#ifdef` would show up as a diff, and that's what the gate is for.

---

## Gate C — Release build

**Variable:** `-DCMAKE_BUILD_TYPE=Release`. Otherwise the baseline configuration.

**This gate isn't in `PLAN.md` and should be.** The harness's 14/14 is a `Debug` measurement
(`torch/build/CMakeCache.txt`: `CMAKE_BUILD_TYPE=Debug`), and so is the local Shipwright build.
Shipwright's CI and every release build `Release` — Torch sets `CMAKE_CXX_FLAGS_RELEASE "-O3"` for
non-MSVC (`torch/CMakeLists.txt:242`). Any UB, uninitialised read, or unspecified evaluation order
in Torch can change bytes between `-g` and `-O3`.

```sh
cmake -S torch -B torch/build-release -GNinja -DCMAKE_BUILD_TYPE=Release -DPORT_VERSION_ENDIANNESS=ON
cmake --build torch/build-release -j
tools/matrix.sh torch/build-release/torch release
```

Seven minutes to find out, versus finding out from a user whose release-build archive doesn't match
their friend's. If it fails it's an upstream Torch bug and everything else stops until it's fixed.

---

## Gate A′ — the shipping configuration

**Variable:** all of them at once — static lib + driver + OoT-only + Release.

Not implied by A ∧ A2 ∧ B ∧ C. `#ifdef` interactions are combinatorial, and this is the only
configuration that will ever ship.

```sh
cmake -S torchlib-driver -B torchlib-driver/build-ship -GNinja -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SM64=OFF -DBUILD_MK64=OFF -DBUILD_SF64=OFF -DBUILD_PM64=OFF \
      -DBUILD_FZERO=OFF -DBUILD_BK64=OFF -DBUILD_MARIO_ARTIST=OFF -DBUILD_NAUDIO=OFF
cmake --build torchlib-driver/build-ship -j
tools/matrix.sh torchlib-driver/build-ship/torchlib-driver shipping
tools/matrix.sh --pair pal_gc_0227d7 pal_mq_f46239 torchlib-driver/build-ship/torchlib-driver shipping-pair
```

**When A′ is green, that flag set is the spec.** Phase 2's root-`CMakeLists.txt` block is a
transcription of it, not a re-derivation — and `main.cpp`'s `RunOnce` is a transcription into
`TorchExtract.cpp`. Copy, don't rewrite.

---

## Gate E — the `soh.o2r` baseline

Different in kind from the others: it doesn't test Torch at all. It captures the artifact the
Phase 4 packer has to reproduce, from a pipeline that is about to be deleted. **Do this while
ZAPD still builds.**

Current state: `soh.o2r` at the repo root, **1,042 entries** = 1,041 files under
`soh/assets/custom/` + `portVersion`. No `version` file — matching
`sohArchiveVersionMatch` (`OTRGlobals.cpp:282-283`), which only reads `portVersion`.

1. **Rebuild rather than trust the file on disk**, so provenance is unambiguous:
   ```sh
   cmake --build build --target GenerateSohOtr     # inside the soh distrobox
   ```
   > `GenerateSohOtr` **writes into the source tree**: it `rm -rf`s and re-copies
   > `soh/assets/custom/shaders/` from `libultraship/src/fast/shaders/` (3 files —
   > `directx/default.shader.hlsl`, `metal/default.shader.metal`, `opengl/default.shader.glsl`).
   > That directory is gitignored (`soh/assets/.gitignore:8`), which is why `git status` stays
   > clean and why the on-disk count (1,041) exceeds `git ls-files` (1,038). Phase 4's packer must
   > run after the same copy or the archives differ by exactly those 3 entries.

2. **Record the output** — `manifest.sh` already does exactly this job:
   ```sh
   ./manifest.sh /var/home/briaguya/code/Shipwright/soh.o2r manifests/soh_o2r.json
   ```
   Lives in the harness alongside the other 14 manifests. (Not in the yml repo — that holds yml and
   nothing else.)

3. **Record the inputs too**, or a future mismatch is ambiguous between "the packer is wrong" and
   "the assets changed":
   - sha256 of every file under `soh/assets/custom/` → `manifests/soh_custom_inputs.json`
   - `CMAKE_PROJECT_VERSION` = **9.2.3** (`CMakeLists.txt:9`), the `portVersion` payload
   - the `develop` SHA the archive was built from

4. **Call out the 31 files that matter.** Only these hit the conversion path — everything else is a
   byte copy, and a byte copy that fails is a trivial bug:

   | Format | Count |
   |---|---|
   | `.rgba32.png` | 15 |
   | `.ia8.png` | 8 |
   | `.ia16.png` | 5 |
   | `.ia4.png` | 3 |

   The other 1,010: 923 objects, 38 plain `.png`, 23 `.rgba16`, 22 json, 4 ttf, 3 shaders, 2 xml.
   The trap is quantisation, not decoding — `n64graphics`' `SCALE_8_4(x)` (`x*15/255`) vs ZAPD's
   `(x >> 4) & 0xF` disagree (`0xF0` → `0xE` vs `0xF`). Phase 4 hand-ports ZAPD's arithmetic; this
   manifest is what catches it if the port is wrong.

---

## Exit criteria

| Gate | Pass condition |
|---|---|
| Preflight | 19/19 identical with the existing baseline binary; one manifest re-derivation matches |
| D | pin SHA recorded; `git diff` vs upstream main still empty |
| A | 19/19 identical; `phases=` matches the per-version yml count |
| A2 | both archives in a two-extraction process identical — especially the second |
| B | 19/19 identical |
| C | 19/19 identical |
| A′ | 19/19 identical, plus the pair run |
| E | `manifests/soh_o2r.json` (1,042 entries) + input hashes + version + SHA committed |

"Identical" means `check.sh` prints `PASS: All N files match!` — **0 missing, 0 extra,
0 mismatched**, not "close enough".

Write the results up as a second table in the harness README, next to the existing 14/14 one. That
table is the evidence the Shipwright PR points at.

---

## Cost

| | Build | Matrix |
|---|---|---|
| Preflight | — | ~7 min |
| A | full Torch rebuild | ~7 min |
| A2 | — (reuses A) | ~1 min |
| B | full Torch rebuild | ~7 min |
| C | full Torch rebuild | ~7 min |
| A′ | full Torch rebuild | ~8 min |
| E | Shipwright `GenerateSohOtr` | ~1 min |

An evening, dominated by four full Torch builds. Four extra build trees; the `Debug` Torch binary
alone is 125 MB, so either build them sequentially and delete as you go or budget a few GB.

---

## What Phase 0 deliberately does **not** cover

Being explicit so these don't get mistaken for retired risk:

- **Linking Torch into Shipwright.** The zlib / stb / `StringHelper` / `StrHash64` / tinyxml2 /
  `storm` collisions in `PLAN.md` Phase 2 need SoH's own build to surface. A standalone driver
  links Torch against nothing else, so it can't see any of them. That's Phase 2's first day.
- **spdlog global-state stomping.** Torch calls `spdlog::set_level`/`set_pattern` on the *default*
  logger (`Companion.cpp:193-194, 1810-1814`), and our `config.yml` sets `logging: CRITICAL` — so
  this fires on every run, it just doesn't matter until there's a SoH logger to clobber.
- **Windows and macOS.** Every gate runs Linux/gcc in the distrobox. Torch forces `/MT` under MSVC,
  adds `-DSTORMLIB_NO_AUTO_LINK` and `/bigobj`, and carries a `cmake_minimum_required(3.12)` +
  `CMAKE_POLICY_VERSION_MINIMUM` shim under a CMake 4 host. If a Windows machine is available,
  running Gate A′ there is the cheapest possible insurance on the one platform that can't be
  predicted from reading — but it isn't a blocker for starting Phase 2.
- **Anything in-game.** The driver proves the bytes; it says nothing about the ImGui progress bar,
  the ROM picker, or `ValidHashes`. That's the Phase 3 manual matrix.

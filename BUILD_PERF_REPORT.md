# Build Performance: Findings and Options

Measured on `develop` @ `dd702b067` (2026-10-17), Linux, GCC 13.3, Ninja, Debug build.

This is a report plus a set of opt-out changes. One change is plain C++ and applies on every platform. The others are CMake options gated to Linux, because that's the only platform they've been tested on. The [per-platform section](#per-platform-applicability) lists what still needs someone on Windows or macOS to try.

## Results

10 clean full builds per configuration, interleaved (A/B, B/A, …) so background load and thermal drift affect both sides equally. One warm-up build per configuration was discarded.

| | Baseline (`develop`) | With these changes | Change |
|---|---|---|---|
| Full clean build | 425.4 ± 16.2 s | **206.3 ± 7.8 s** | **2.06× faster** |
| Compile phase (until last object) | 338.7 ± 11.3 s | 204.0 ± 8.3 s | −40% |
| `soh.elf` link | 86.7 ± 9.9 s | **2.3 ± 1.9 s** | **38× faster** |
| `soh.elf` size | 976 MB | 226 MB | −77% |
| soh object directory | 4.9 GB | 2.4 GB | −51% |

Values are mean ± standard deviation, n = 10 per side. All 20 timed builds succeeded with the same step count every time (1,727 baseline, 1,728 with the PCH step).

**Incremental builds** (touch one `.cpp`, rebuild) are where the link dominates:

| | Baseline | With these changes |
|---|---|---|
| Touch `MoonJump.cpp` | link alone 20 s (idle machine, warm cache) to 270–434 s (observed in daily use) | **3.1 s** total |
| Touch `randomizer_item_tracker.cpp` | as above, plus ~8 s compile | **8.9 s** total |
| Touch `MoonJump.cpp`, link inputs evicted from page cache | bfd cold link alone: 129.9 s | **41.0 s** total |

The incremental baseline was not benchmarked under controlled conditions; the 270–434 s figures come from the developer's `.ninja_log` across ~750 real incremental builds.

## Per-platform applicability

| Change | Linux | Windows (MSVC) | macOS (Clang) |
|---|---|---|---|
| 1. GameInteractor move | measured | plain C++; should help but needs measuring | plain C++; should help but needs measuring |
| 2. Fast linker | lld/mold, measured | x64 Debug already uses `/ZI` + `/INCREMENTAL`; `/DEBUG:FASTLINK` or `lld-link` are untested options | Xcode 15+ already ships a fast linker (ld-prime) |
| 3. Split debug info | `-gsplit-dwarf`, measured | debug info already lives in PDBs; the linker still merges it unless `/DEBUG:FASTLINK` is used | **already the default** (debug info stays in the `.o` files, dsymutil collects it) |
| 4. PCH | measured | CMake `target_precompile_headers` works with MSVC, untested | works with Clang, untested |

## The changes

### 1. Move `GameInteractor::RemoveAllQueuedHooks()` out of the header (all platforms)

`RemoveAllQueuedHooks()` is defined in the class body of `GameInteractor.h` and expands `ProcessUnregisteredHooks<H>()` for all 84 entries in `GameInteractor_HookTable.h`. Because it's defined in the header, every one of the ~258 files that include `GameInteractor.h` instantiates and emits that code for every hook type: `unordered_map<HOOK_ID, std::function<…>>`, `std::map<HOOK_ID, HookInfo>`, erase/iterate paths, and so on. Only `GameInteractor_Hooks.cpp` calls it.

The fix declares it in the header and defines it in `GameInteractor.cpp`. Behaviour is unchanged.

Measured on `Cheats/MoonJump.cpp` (~30 lines), compiled alone:

| | Before | After |
|---|---|---|
| Compile time | 7.66 s | **2.24 s** |
| Peak compiler memory | 828 MB | 326 MB |
| Object size | 16.4 MB | **1.56 MB** |
| Defined symbols | 7,728 (5,675 `std::function`-related) | 840 |

`GameInteractor.cpp` now carries those instantiations once (5.86 s).

### 2. `SOH_FAST_LINKER` (Linux, default ON)

Uses mold if found, otherwise lld, otherwise falls back to the default linker. Debug and RelWithDebInfo also get `-Wl,--gdb-index`.

The Debug `soh.elf` links ~5.5 GB of objects into a ~1 GB binary. Same link command, same objects, outside a build:

| Linker | Warm page cache | Cold page cache |
|---|---|---|
| bfd (default) | 19.9 s | 129.9 s |
| gold | 15.7 s | (not measured) |
| **lld 16** | **2.3 s** | 70.0 s |

Inside real builds bfd is slower still: it reads its inputs while the OS is still writing freshly compiled objects back to disk, and writes its 1 GB output to the same disk. lld reads its inputs up front (memory-mapped, multithreaded) and writes its output in a single pass.

Packaging note: Ubuntu's `lld-NN` packages only ship `ld.lld-NN`, while GCC's `-fuse-ld=lld` looks for a binary named exactly `ld.lld`, and GCC 13 has no `--ld-path`. When only a versioned binary exists, the option creates `${CMAKE_BINARY_DIR}/linker/ld.lld` as a symlink and passes `-B` that directory.

### 3. `SOH_SPLIT_DWARF` (Linux, default ON)

Adds `-gsplit-dwarf` for Debug and RelWithDebInfo. Debug info goes into `.dwo` files the linker never reads, which roughly halves link input. On `OTRGlobals.cpp`, compile time is unchanged (11.0 s), and the 18 MB `.o` becomes a 9.6 MB `.o` plus a 9.7 MB `.dwo`. Across the whole soh target: 955 MB of `.o` link input and 1.2 GB of `.dwo`.

gdb resolves `info line`, `ptype`, and `break file.cpp:line` correctly on the resulting binary.

This one is effectively Linux-only for good: macOS already keeps debug info out of the link, and Windows' equivalent is `/DEBUG:FASTLINK`.

### 4. `SOH_PCH` (Linux, default ON)

A C++-only precompiled header (`soh/soh/soh_pch.h`) for the `soh` target. It contains common STL headers, `nlohmann/json.hpp`, `spdlog/spdlog.h`, `imgui.h` and `imgui_internal.h`. The PCH is a 197 MB `.gch` that takes 4.6 s to build.

| File | Without PCH | With PCH |
|---|---|---|
| `randomizer_item_tracker.cpp` | 8.86 s | 7.62 s (−14%) |
| `MoonJump.cpp` | 1.99 s | 1.39 s (−30%) |
| `SohMenuEnhancements.cpp` | 7.27 s | 5.00 s (−31%) |

`IMGUI_DEFINE_MATH_OPERATORS` is already defined globally by libultraship's imconfig patch, so precompiling ImGui doesn't change anything for the files that also define it locally.

Every option can be turned off with `-DSOH_<NAME>=OFF`.

## Call for testing (Windows / macOS)

Useful data points, roughly in order of value:

1. **GameInteractor change.** Clean-build `develop`, then clean-build this branch, using the same generator and configuration. Report both total times. If you use Ninja, `.ninja_log` also gives per-file times (columns: start ms, end ms, mtime, output).
2. **PCH.** In `soh/CMakeLists.txt`, change the `if(CMAKE_SYSTEM_NAME STREQUAL "Linux")` around `SOH_PCH` to include your platform, then compare clean-build times with `-DSOH_PCH=ON` and `OFF`. Compile errors here are also useful to know about.
3. **Windows linker.** In a Debug build, time a one-file rebuild with the current `/INCREMENTAL`, then with `/DEBUG:FASTLINK` added to the soh target's Debug link options. `lld-link` (`-DCMAKE_LINKER=lld-link` with Ninja) is another candidate.
4. **macOS linker.** Run `ld -v` to check whether ld-prime is in use (Xcode 15+). If it isn't, the link time of a one-file rebuild is worth reporting.

## Further opportunities (not implemented)

1. **libultraship include hygiene.** After change 1, header parsing is the main per-file cost. On `randomizer_item_tracker.cpp` (277k preprocessed lines), parsing is 53% of compile time. Most of the heavy third-party code arrives through libultraship:
   - `ship/config/Config.h` → `nlohmann/json.hpp` (14k lines)
   - `Config.h` → `ship/window/Window.h` → `ship/window/gui/Gui.h` → `imgui_internal.h` → `immintrin.h` (~22.6k lines of AVX-512 headers)

   Forward-declaring in `Config.h` and `Gui.h` would remove these from most soh files.
2. **clang on Linux.** Compiled alone with identical flags, clang 16 was 3–31% faster than GCC 13 per file and produced objects roughly half the size. For example, `OTRGlobals.cpp`: 11.27 s / 18 MB with GCC vs 7.81 s / 9 MB with clang.
3. **ccache.** Helps when switching branches. It works with split DWARF; a PCH needs `sloppiness=pch_defines,time_macros`.

## Method

**Test machine:** 16 threads, 31 GB RAM, source and build directories on a 7200 rpm HDD (ext4), GCC 13.3.0, CMake 3.28.3, Ninja, Ubuntu 24.04 container, Debug build. On an SSD the bfd penalty will be smaller, but the CPU-side gains (changes 1 and 4, and lld's ~9× CPU advantage) apply regardless of disk.

**Full-build benchmark:**
- Two separate build directories on the same disk, both configured `-G Ninja -DCMAKE_BUILD_TYPE=Debug`. The baseline had the original `GameInteractor` sources and `SOH_FAST_LINKER/SOH_SPLIT_DWARF/SOH_PCH=OFF`. The optimized side had this branch with defaults.
- Before each run: `ninja clean`, then wait until the kernel's dirty/writeback pages were below 100 MB / 10 MB, then 20 s idle.
- Wall time comes from the shell. The compile-phase end and the link time come from `.ninja_log`.

All runs, in execution order:

| Run | Baseline wall (s) | Baseline link (s) | Optimized wall (s) | Optimized link (s) |
|---|---|---|---|---|
| 1 | 433.8 | 86.6 | 203.5 | 7.1 |
| 2 | 403.0 | 73.9 | 197.5 | 1.1 |
| 3 | 397.6 | 70.0 | 198.9 | 3.5 |
| 4 | 424.4 | 95.5 | 197.3 | 1.1 |
| 5 | 414.6 | 86.9 | 206.5 | 2.4 |
| 6 | 424.5 | 94.6 | 200.5 | 1.2 |
| 7 | 437.5 | 95.9 | 216.8 | 1.4 |
| 8 | 449.6 | 99.3 | 211.4 | 1.1 |
| 9 | 433.1 | 85.3 | 215.6 | 2.5 |
| 10 | 436.3 | 79.2 | 214.9 | 1.3 |
| **Median** | **428.8** | **86.8** | **205.0** | **1.4** |

**Per-file numbers:** commands from `compile_commands.json`, each run alone with `-ftime-report`, `-E -H`, and `nm -C --defined-only`.

**Linker numbers:** the command from `ninja -t commands soh/soh.elf`, with `-o` redirected and the linker swapped. For cold-cache runs, every link input was evicted with `dd iflag=nocache count=0`, followed by `sync`.

## Caveats

- One machine. The HDD makes the link penalty larger than it would be on an SSD.
- The incremental-build baseline comes from real-world history, not the controlled benchmark.
- Only compiling, linking and gdb symbol lookup were verified. The game has not been play-tested with these changes.
- Windows and macOS have not been built.

# AGENTS.md

## What this is

Shipwright (Ship of Harkinian) is a PC port of the Ocarina of Time decompilation. Game assets are not in the repo: they are extracted from a ROM the user has legally sourced themselves into `oot.o2r` / `oot-mq.o2r` archives at first run. Never help locate, download, or share ROMs; if the user doesn't have one, point them to the README's ROM requirements. Don't analyze ROMs or O2R/OTR archives for data or information; the codebase is the source of truth. `libultraship` and `torch` are git submodules with their own upstream repos; run `git submodule update --init` after cloning, and don't edit them in-tree. If a change is needed there, tell the user it belongs upstream.

## Build

Setup, dependencies, and build instructions for each platform are in `docs/BUILDING.md`. Don't assume a particular OS, toolchain, or build directory; check how the user has already set up their environment.

There is no automated test suite, so changes are verified manually in the game. After a change, build it, then ask the user to test it and offer a manual test plan covering the feature itself, edge cases, and behavior the change may indirectly affect (other features sharing the same hook, vanilla behavior with the CVar off, randomizer and Boss Rush modes, saves).

## Formatting and lint (CI-enforced)

- C/C++ in `soh/` must be formatted with **clang-format 14.x exactly**; 15+ produces different output and fails CI. Run `./run-clang-format.sh` (override the binary with `CLANG_FORMAT=...`; see `docs/FORMATTING.md`). It skips `.h`/`.hpp` files under `soh/src` and `soh/include`, and all of `soh/assets`.
- No tabs or trailing whitespace in C/C++ outside `soh/include`: `./scripts/check-whitespace.sh [--fix]`. It only checks tracked files, so `git add` new files first.
- JSON in `soh/assets/custom`: `./scripts/lint-json.sh [--fix]`.
- `pre-commit install --install-hooks` runs these checks on commit, and its JSON hook rewrites files (see `docs/FORMATTING.md`).

## Code style

- Follow the existing conventions and patterns of the surrounding code; look at how similar features are done before writing a new one. A bespoke solution is more likely to be rejected in review than one that cleanly fits the existing architecture.
- Keep comments minimal. Code should be self-documenting; a comment, if needed at all, is one line. Excessive or verbose commenting is not acceptable.
- Keep save data backward compatible: older saves must still load after a change (`SaveManager.cpp`).
- Don't hand-edit the checked-in asset headers under `soh/assets` (they can't currently be regenerated); `soh/assets/custom` is the hand-maintained exception.

## Coding standards

Project coding standards are in `docs/CODE_GUIDELINES.md`. Read it before writing or reviewing code and follow it; where it conflicts with this file, the guidelines win.

## Architecture

### Two kinds of code in `soh/`

- `soh/src/` (incl. `soh/src/code/z_*.c` and `soh/src/overlays/actors/**`) is the **decompiled game source**, kept close to upstream so decomp merges stay tractable.
- `soh/soh/` is Shipwright's own C++ layer: `OTRGlobals.cpp` (engine glue/boot), `SaveManager.cpp`, `SohGui/` (ImGui menus), `Network/` (Crowd Control, Sail, Anchor), and `Enhancements/` (every feature, fix, and mode, including `randomizer/`, `boss-rush/`, `cosmetics/`, `Fixes/`, `QoL/`, `TimeSavers/`).

### Changing game behavior: hooks, not decomp edits

Don't put feature logic in `z_*.c` decomp files. Intercept vanilla behavior through the GameInteractor hook system in `soh/soh/Enhancements/game-interactor/`:

- Decomp code calls `GameInteractor_Should(VB_SOMETHING, vanillaResult, ...)` at a decision point (a "virtual behavior" hook, enum in `vanilla-behavior/GIVanillaBehavior.h`). The returned bool decides whether the vanilla behavior happens (usually `if (GameInteractor_Should(...)) { vanilla }`); subscribers can flip the result to suppress or force it.
- Features subscribe from `soh/soh/Enhancements/` with `COND_VB_SHOULD(VB_X, condition, { ... })`, or `COND_HOOK` / `REGISTER_VB_SHOULD` (macros in `GameInteractor.h`), inside a function registered with `static RegisterShipInitFunc foo(fn, { "cvar.name" })` (`soh/soh/ShipInit.hpp`). Init functions run at boot and re-run only when something calls `ShipInit::Init("cvar.name")` (e.g. `UIWidgets::CVarChanged`, or `IS_RANDO` on game load), so register/unregister hooks based on the current CVar state inside `fn`. `COND_VB_SHOULD` and `COND_HOOK` unregister the previous hook on re-run; `REGISTER_VB_SHOULD` does not, so prefer the `COND_*` forms in re-runnable functions.
- If no suitable `VB_*` exists, add one: a `GameInteractor_Should` call site in the decomp file (one VB per vanilla behavior), with the logic in an Enhancements file. Name the VB after the vanilla behavior it guards (what the game does at that point), never after the override you want (e.g. not `VB_SKIP_X` or `VB_ALLOW_Y`). Insert the new enum value **alphabetically** in `GIVanillaBehavior.h` and give it the usual `result`/`args` doc comment.
- Hooks do not run in a guaranteed order. Behavior that could conflict with other hooks on the same VB belongs together in a single handler, such as `randomizer/hook_handlers.cpp`.
- Direct `CVarGet*`, `IS_RANDO`, or `IS_BOSS_RUSH` checks inside decomp files are the thing to avoid. The many existing ones are legacy and not a pattern to copy.

Non-VB lifecycle events (`OnLoadGame`, `OnSceneInit`, `OnGameFrameUpdate`, ...) are in `GameInteractor_HookTable.h` / `GameInteractor_Hooks.cpp`.

### Submodules

- `libultraship` (LUS) is the engine layer: libultra reimplementations for modern hardware, the Fast3D renderer (`src/fast`), resource/archive loading, audio, input, windowing, the CVar system, and the ImGui framework `soh/soh/SohGui/` builds on (`src/ship`).
- `torch` is an N64 asset processor. Its per-game factories (`src/factories/oot/`) convert ROM data into the resources LUS loads, packed into `.o2r` archives. It is linked into `soh` for first-run extraction (`soh/soh/Extractor/TorchExtract.cpp`).

### Settings

Settings are CVars named through the prefix macros in `soh/soh/cvar_prefixes.h` (`CVAR_ENHANCEMENT("Foo")`, `CVAR_CHEAT(...)`, `CVAR_RANDOMIZER_SETTING(...)`, etc.). Use the macro matching the category rather than a raw string. Menu UI for them lives under `soh/soh/SohGui/`.

### Randomizer

`soh/soh/Enhancements/randomizer/` (with the `3drando` generator and `location_access/`) defines checks (`RC*`), settings (`RSK_*`), logic, hints, and item overrides. `IS_RANDO` is true when the active save's quest is the randomizer.

## Scope

- Extensive or wide-ranging changes that affect the architecture are difficult. If asked to make one and the user doesn't have a lengthy history of contributions (check `git log --author`), encourage them to talk to the developers on the [HarbourMasters Discord](https://discord.com/invite/harbourmasters) for feedback and advice.
- Support for platforms other than Windows, macOS, and Linux is not being sought, and the existing Switch and Wii U support is not to be extended. If asked to work on one, tell the user to keep that work in their own fork.

## Workflow

- `develop` is for larger features. A `develop-<release name>` branch (e.g. `develop-dewey`) is for bugfixes only. Branch from, and target PRs at, whichever fits the change.
- Branch names like `feature/...`, `bugfix/...` (see `docs/MODDING.md`).
- Bugfix PRs and commits are usually titled `[Bugfix] Description`; other subjects are a plain imperative sentence.
- One logical change per PR. Don't include unrelated reformatting or cleanup.

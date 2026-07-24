# `.meta` alias fallback (libultraship #1165)

## Context

libultraship #1165 asks that a `.meta` alias file behave as *"prefer the target, otherwise
use the real asset at the original path."* The motivating use case is cross-game mod
compatibility (SoH/OoT ↔ 2Ship/MM): a base archive can ship a `.meta` that aliases
`gLinkHumanSkel → gLinkChildSkel`, so a single unmodified mod works in both games — but when
no mod is loaded and the target is absent, the game must fall back to its native asset instead
of failing.

**Current reality (verified):** the `.meta` alias mechanism is broken end-to-end in this
libultraship checkout — matching the still-open #984. `Archive::IndexFile`
(`libultraship/src/ship/resource/archive/Archive.cpp:174`) strips the `.meta` suffix and never
indexes the sidecar under its own name, so the loader's lookup of `filePath + ".meta"`
(`ResourceLoader.cpp:193`) can never resolve. So this work necessarily makes `.meta` resolution
functional (resolving #984's core) *and* adds the #1165 fallback. Confirmed intended behavior
for all four real/target combinations (user chose full support):

| real asset | `.meta` | alias target | result |
|---|---|---|---|
| present | present | present | **load target** |
| present | present | absent  | **load real (fallback — #1165)** |
| absent  | present | present | **load target (#984 reachability)** |
| absent  | present | absent  | **fail** |
| present | absent  | —       | load real (legacy — unchanged baseline) |

The verified load path: `ResourceManager::LoadResourceProcess` (`ResourceManager.cpp:101`) loads
the file at the requested path via `LoadFileProcess`, **bails at line 154 if it's null**, then
passes it to `ResourceLoader::LoadResource`, which is the *only* place `.meta` sidecars are
resolved. All archive types (`O2rArchive`, `OtrArchive`, `FolderArchive`) funnel indexing
through the single `Archive::IndexFile`.

## The engine fix (3 changes in `libultraship/`)

### 1. `Archive::IndexFile` — expose `.meta` sidecars for loading
`libultraship/src/ship/resource/archive/Archive.cpp:174`

Keep indexing the base name (so the resource stays requestable/discoverable as `foo`) **and
also** index the sidecar under its literal `foo.meta` name so `LoadFileProcess(path + ".meta")`
can find it:

```cpp
void Archive::IndexFile(const std::string& filePath) {
    if (filePath.length() > 5 && filePath.substr(filePath.length() - 5) == ".meta") {
        IndexFile(filePath.substr(0, filePath.length() - 5)); // base name (as today)
        (*mHashes)[CRC64(filePath.c_str())] = filePath;        // NEW: literal `foo.meta`
        return;
    }
    (*mHashes)[CRC64(filePath.c_str())] = filePath;
}
```
A real `foo` and a `foo.meta` both resolve the base key to `foo` (real wins / same value); when
only `foo.meta` exists, the base key points at a non-existent physical `foo`, so
`LoadFileProcess("foo")` returns null (handled by change 2).

### 2. `ResourceManager::LoadResourceProcess` — reach the loader for `.meta`-only resources
`libultraship/src/ship/resource/ResourceManager.cpp:153-158`

Today it returns early when the direct file is missing, before the loader can resolve a `.meta`.
Only bail when *neither* the real file nor a `.meta` exists; otherwise fall through with a
possibly-null `file` (the loader handles null):

```cpp
auto file = LoadFileProcess(identifier.Path);
if (file == nullptr && LoadFileProcess(identifier.Path + ".meta") == nullptr) {
    SPDLOG_TRACE("Failed to load resource file at path {}", identifier.Path);
    mResourceCache[identifier] = ResourceLoadError::NotFound;
    return nullptr;
}
```

### 3. `ResourceLoader::LoadResource` — prefer target, else fall back to real
`libultraship/src/ship/resource/ResourceLoader.cpp:185-208`

Remove the leading `fileToLoad == nullptr` early-return (a `.meta`-only alias legitimately has a
null `fileToLoad`) and restructure the `.meta` branch so the target is preferred, the real asset
is the fallback, and the null check moves *after* resolution:

```cpp
if (initData == nullptr) {
    auto metaFileToLoad =
        Context::GetRawInstance()->GetResourceManager()->LoadFileProcess(filePath + ".meta");

    if (metaFileToLoad != nullptr) {
        auto metaInit = ReadResourceInitData(filePath, metaFileToLoad);
        auto aliasedFile = Context::GetRawInstance()->GetResourceManager()->LoadFileProcess(metaInit->Path);

        if (aliasedFile != nullptr) {                 // target exists -> load target
            fileToLoad = aliasedFile;
            initData = metaInit;
        } else if (fileToLoad != nullptr) {           // target absent, real present -> fall back (#1165)
            SPDLOG_TRACE("Meta alias target '{}' missing; falling back to real asset '{}'.",
                         metaInit->Path, filePath);
            initData = ReadResourceInitDataLegacy(filePath, fileToLoad);
        }
        // else: target and real both absent -> fileToLoad stays null -> fail below
    } else if (fileToLoad != nullptr) {
        initData = ReadResourceInitDataLegacy(filePath, fileToLoad);
    }
}

if (fileToLoad == nullptr) {
    SPDLOG_ERROR("Failed to load file at path {}.", filePath);
    return nullptr;
}
```
Fallback deliberately reuses `ReadResourceInitDataLegacy` — i.e. "behave exactly as if the
`.meta` weren't there" — reading the real asset's own embedded OTR header / XML. `ReadResourceInitData`
(the JSON parser, `ResourceLoader.cpp:156`) is reused unchanged for the target path.

## In-game test harness (SoH) — replicating & expanding commit f5d2e58

Test vehicle is the boot **Ship ("LUS") logo**, drawn first at startup from the `DisplayList`
resource `textures/nintendo_rogo_static/gShipLogoDL`
(`soh/soh/Enhancements/cosmetics/CustomLogoTitle.cpp:60`, `(Gfx*)gShipLogoDL`). It renders
immediately on launch (default boot sequence) and is skippable with A/B/Start — so each case is
observable within seconds of boot. f5d2e58 exercised only the "target present" cell; we expand
to the full matrix.

**Fixture files** under `soh/assets/custom/textures/nintendo_rogo_static/` (packed verbatim into
`soh.o2r`; f5d2e58 confirms `.meta` and `.xml` pack as-is):
- **REAL** — `gShipLogoDL` (the existing XML DisplayList; already present).
- **META** — `gShipLogoDL.meta`, JSON exactly as f5d2e58:
  ```json
  { "path": "textures/nintendo_rogo_static/fancyShipDL.xml",
    "type": "DisplayList", "format": "XML", "version": 0 }
  ```
- **TARGET** — `fancyShipDL.xml`, a **visually distinct** DisplayList so "loaded target" is
  unmistakable vs "loaded real" — e.g. copy `gShipLogoDL` but drop two of the four
  `CallDisplayList` tri groups (renders a visibly partial logo) or change the `SetCombineLERP`
  color. It must keep valid `CallDisplayList` paths to the shared `*_tri_*` / `*_vtx_*` /
  `mat_*` sub-resources (leave those in place for every case).

**The five cases** — arrange REAL/META/TARGET present(✓)/absent(✗), rebuild, boot, observe the LUS logo:

| # | REAL | META | TARGET | expected boot logo |
|---|---|---|---|---|
| 1 | ✓ | ✓ | ✓ | **fancy/partial** logo (target loaded) |
| 2 | ✓ | ✓ | ✗ | **normal** ship logo (fallback — #1165) |
| 3 | ✗ | ✓ | ✓ | **fancy/partial** logo (target loaded — #984 path) |
| 4 | ✗ | ✓ | ✗ | **no logo** (blank); `SPDLOG_ERROR "Failed to load file at path textures/nintendo_rogo_static/gShipLogoDL"` |
| 5 | ✓ | ✗ | — | **normal** ship logo (baseline regression) |

Per-case loop:
1. Add/remove the three fixture files under `soh/assets/custom/textures/nintendo_rogo_static/`.
2. Repack assets: `cmake --build /var/home/briaguya/code/Shipwright/build --target GenerateSohOtr`
   (regenerates `soh.o2r` and copies it to `build/soh/`).
3. Launch `/var/home/briaguya/code/Shipwright/build/soh/soh.elf`; watch the first (LUS) boot logo.
4. Set log level to **Trace** (CVar `gDeveloperTools.LogLevel`) to see the fallback `SPDLOG_TRACE`
   (case 2/3) and the failure `SPDLOG_ERROR` (case 4).

Shortcut for cases 1/2/5 (REAL stays in `soh.o2r`): drop a small override archive
`gShipLogoDL_test.o2r` (a zip containing just META and/or TARGET) into `build/soh/mods/` — mods
load after `soh.o2r` and override the path, avoiding a full `GenerateSohOtr` repack. Cases 3/4
require REAL absent, so those need `soh.o2r` rebuilt without `gShipLogoDL`.

## Build / verification

- Rebuild engine + game after the libultraship changes:
  `cmake --build /var/home/briaguya/code/Shipwright/build` (builds libultraship into `soh.elf`).
- Repack assets after each fixture change: `--target GenerateSohOtr` (see above).
- **Pass criteria:** all five rows produce the expected logo; case 2 & 3 log the fallback/target
  traces; case 4 logs the failure and renders no logo; case 5 confirms no regression to normal
  (no-`.meta`) loading.

## Deliverable vs scaffolding
- **Deliverable (libultraship PR, #1165 + #984 core):** the 3 engine changes above. Work in the
  `libultraship` submodule on a branch (fork remote `briaguya`).
- **Scaffolding:** the SoH fixture files are local test setup. Optionally keep one demonstrator
  commit (as f5d2e58 did) but the fixtures are not part of the engine PR.

## Critical files
- `libultraship/src/ship/resource/archive/Archive.cpp:174` — `IndexFile` (change 1)
- `libultraship/src/ship/resource/ResourceManager.cpp:153` — `LoadResourceProcess` (change 2)
- `libultraship/src/ship/resource/ResourceLoader.cpp:185` — `LoadResource` (change 3);
  `ReadResourceInitData:156` / `ReadResourceInitDataLegacy:85` reused unchanged
- `soh/assets/custom/textures/nintendo_rogo_static/` — test fixtures
- `soh/soh/Enhancements/cosmetics/CustomLogoTitle.cpp:60` — where `gShipLogoDL` is drawn
- `CMakeLists.txt:248` — `GenerateSohOtr` (repacks `soh.o2r` from `soh/assets/custom`)

# Status — after the provider-model rework

Captured 2026-07-24 against the **`meta-provider-resolution`** libultraship branch (the
"highest-priority provider wins" implementation, off `develop` / `2bfbde3a`), running the
reworked test kit. Every case matches its expected after-fix result.

Legend: **brown** = soh.o2r's vanilla real · **green** = a mod's real · **magenta** = alias
target · **blank** = failed to load.

## Engine changes (`meta-provider-resolution`)

1. **`Archive::IndexFile`** — index every file under its literal name (no `.meta` stripping), so a
   real `foo` and its `foo.meta` are distinct index entries.
2. **`Archive::GetPriority`/`SetPriority`** + **`ArchiveManager::GetFilePriority`** — O(1)
   load-order priority of the archive that owns a path (higher = wins).
3. **`ResourceManager::LoadResourceProcess`** — only bail when neither the real file nor a `.meta`
   exists (so meta-only resources reach the loader).
4. **`ResourceLoader::ResolveMetaAlias` / `LoadResource`** — pick the higher-priority provider
   (real asset at the path vs the `.meta`'s alias target); ties go to the alias.

## Results (all match)

| case | baseline (stock) | after fix |
|---|---|---|
| control | brown | **brown** ✓ |
| `case1` | green | **magenta** ✓ |
| `case2` | green | **green** ✓ |
| `case3` | blank | **magenta** ✓ |
| `case4` | blank | **brown** ✓ |
| `case5` | green | **green** ✓ |
| flagship (all 3) | green | **magenta** ✓ |
| flagship (drop `-30`) | green | **green** ✓ |
| `L1` | green | **green** ✓ |
| reachback | blank | **magenta** ✓ |

Five flips prove the fix: **case1** green→magenta, **case3** blank→magenta, **case4** blank→brown
(no shadowing), **flagship** green→magenta, **reachback** blank→magenta. case2 / case5 / L1 /
flagship-drop-`30` are unchanged regression checks.

# Prebuilt `soh.o2r` — **temporary**

This directory exists to unblock the Torch migration and is **meant to be deleted**.

`soh.o2r` is the port's own asset archive: fonts, textures, presets, translations, shaders —
everything under `soh/assets/custom/`, plus a `portVersion` stamp. It is built by
`GenerateSohOtr`, which today drives ZAPD through `OTRExporter/extract_assets.py`. The migration
deletes ZAPDTR and OTRExporter, so that producer goes away and its replacement — a small in-tree
packer (Phase 4 of `torch-migration/PLAN.md`) — has to reproduce it byte-for-byte, including
ZAPD's specific texture quantisation.

Rather than block every other phase on that packer, the archive is checked in here and
`GenerateSohOtr` just copies it into place. **Phases 2, 3 and 5 become testable without the
packer existing.**

This file also serves as Phase 4's acceptance oracle: the packer is correct when its output
matches this archive entry-for-entry. That replaces Gate E's planned sha256 manifest — the
artifact is strictly more useful than a list of hashes of it.

## Provenance

| | |
|---|---|
| Generated | 2026-07-25 |
| Shipwright branch | `torch-migration` @ `fe2f52248` |
| `develop` at the time | `c4e92a70f` (inputs verified identical — `git diff develop HEAD -- soh/assets/custom libultraship CMakeLists.txt` was empty) |
| libultraship submodule | `c57da1b4` |
| `CMAKE_PROJECT_VERSION` | `9.2.3` |
| Producer | `cmake --build build --target GenerateSohOtr` (ZAPD `botr -se OTR --norom`) |
| Size / entries | 4,414,957 bytes / 1,042 entries (1,038 checked-in assets + 3 LUS shaders + `portVersion`) |
| sha256 | `1d23b582fbc29cebf803b6ee54856089d0978e5496e6d4b1dbcde876e13cf13c` |
| sha256 of inputs | `de8ba0b5167bec6956258dd0445d01ac0f6daac84aff091c678d951cc72598f6` |

The input hash covers every file under `soh/assets/custom/` except `shaders/`, which is not
checked in — it is copied out of `libultraship/src/fast/shaders/` at generation time. Recompute it
with:

```sh
find soh/assets/custom -type f -not -path '*/shaders/*' -print0 | sort -z \
    | xargs -0 sha256sum | sha256sum
```

(`-print0`/`sort -z` are required: several preset filenames contain spaces.)

## Generation is content-deterministic, not byte-deterministic

Two consecutive runs produce archives with **identical payloads for all 1,042 entries** but
**different whole-file hashes** — the zip stores a modification timestamp per entry. So:

- The sha256 above identifies *this* file, not "any correct archive".
- **Phase 4's acceptance test must compare entry-by-entry**, never whole-file. A byte-identical
  archive is not achievable and not the goal.

## What this pins

The archive freezes three inputs. If any changes while this workaround is in place, the checked-in
copy is stale and **nothing will tell you at runtime** except missing or wrong assets:

1. **`soh/assets/custom/`** — edit an asset and your change is silently ignored. Compare the input
   hash above.
2. **libultraship's shaders** — `libultraship/src/fast/shaders/` is copied in at generation time,
   so a LUS submodule bump can drift the three shaders inside this archive.
3. **The project version.** `portVersion` here is `9.2.3`. `OTRGlobals.cpp:283` requires exact
   `major.minor.patch` equality against the build's version, and `RunExtract` calls `exit(1)` with
   "soh.o2r is outdated" if it fails. A configure-time check in the root `CMakeLists.txt` catches
   this and tells you to regenerate, because the runtime symptom is not obvious.

## Regenerating

From a tree where ZAPDTR/OTRExporter still build (i.e. `develop`):

```sh
cmake --build build --target GenerateSohOtr
cp soh.o2r prebuilt/soh.o2r
```

Then update the provenance table above.

## Removing this

When Phase 4's packer lands and its output matches this archive entry-for-entry, delete this
directory, drop the `!prebuilt/soh.o2r` negation from `.gitignore`, and restore `GenerateSohOtr`
to a real build step (including the libultraship shader copy, which the copy-only version skips).

**Delete it before the migration PR merges** if at all possible. It is a 4.4 MB binary, and a blob
that reaches `develop` is in the history permanently; removed in the final tree and squash-merged,
it never lands there at all.

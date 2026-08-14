# Ship of Harkinian 9.2.3 on Wii U / Aroma

This checkout is intentionally pinned to official HarbourMasters `Shipwright`
tag `9.2.3`, commit `cb71e22a79bc5d1f688fa881795bbd93094895fc` (2026-04-14).
The separate `HarbourMasters/Shipwright-WiiU` repository is an older fork whose
last code push was June 2024; do not use it for this build.

The current main release needs local CaféOS compatibility work: the GX2
renderer and WUT application layer are restored in `libultraship`; the
platform dependency CMake code supplies nlohmann-json and spdlog; and the
audio bootstrap builds the missing PPC Ogg/Opus/Vorbis libraries in the build
directory. Two small dependency patches eliminate PPC TLS relocations which
WUT's `elf2rpl` cannot encode. Everything is source-visible, pinned, and
kept inside this checkout/build directory.

## Toolchain

The upstream build script is `scripts/wiiu/build.sh`: CMake's current CafeOS
toolchain file and the `soh_wuhb` target build the Aroma Homebrew Bundle.
Install the current supported devkitPro package set on macOS:

```sh
sudo installer -pkg /tmp/devkitpro-pacman-installer-6.0.2.pkg -target /
sudo /opt/devkitpro/pacman/bin/pacman -Syu wiiu-dev ppc-libzip ppc-tinyxml2 ppc-bzip2 ppc-zlib ppc-libpng wiiu-sdl2
```

Open a new shell and verify (the supplied build script also auto-detects these
paths if zsh does not export them):

```sh
printf '%s\n' "$DEVKITPRO" "$DEVKITPPC"
"$DEVKITPPC/bin/powerpc-eabi-g++" --version
test -f "$DEVKITPRO/cmake/WiiU.cmake"
```

`wiiu-dev` supplies devkitPPC, WUT, and the Wii U port libraries (including
SDL2) required by the source. No third-party Wii U fork or release binary is
used. On this macOS 26 machine, Homebrew LLVM is used for host asset generation
with `-DFMT_CONSTEVAL=` because the release's bundled fmt otherwise fails with
AppleClang 21; this is a compiler workaround, not a source patch.

Run `./build-wiiu.sh /absolute/path/to/your-rom.z64`. It verifies the supplied
ROM's SHA-1 against the source's supported-hash list, creates only a temporary
extractor copy (then removes it on exit), and does not track or stage the ROM
itself. The build produces `oot.o2r` (ROM data),
`soh.o2r` (port data), `build-wiiu/gamecontrollerdb.txt` (controller mappings),
and `build-wiiu/soh/soh.wuhb`. Set
`SOH_WIIU_BUILD_DIR=build-wiiu-current` to select another relative build
directory.

## SD staging and data locations

After a successful Wii U build run `./prepare-sd.sh`. If the build directory
is not the script's default `build-wiiu`, set `SOH_WIIU_BUILD_DIR` to its
repository-relative name. It never touches a
mounted SD card and refuses an existing staging app directory. Its result is:

```text
wiiu/apps/soh/
  soh.wuhb
  gamecontrollerdb.txt
  oot.o2r
  soh.o2r
  mods/
  logs/
```

The Wii U build deliberately uses the application directory as its data path,
so `shipofharkinian.json`, `oot_save.sav`, `mods/`, and `logs/Ship of
Harkinian.log` live alongside `soh.wuhb`. Back up the whole existing
`wiiu/apps/soh` directory before merging staging onto an SD card. `.wuhb` is
the Aroma/HB-AppStore-friendly bundle; `.rpx` is the raw executable and is not
the preferred Aroma install artifact.

## First boot

Start with no mods. Set 16:9, 1080p output, and 30 FPS first; use interpolation
only after testing. Enable gyro only after verifying the GamePad mapping and
leave experimental graphics off. Test: launch; title screen; make a save;
audio; GamePad/controller input; enter Kokiri Forest; save, exit, and relaunch.
Once reliable, try 60 FPS with interpolation off first and retain it only if
Kokiri Forest and Hyrule Field remain smooth.

## Mods

Do not install a large pack until the base port has passed the test above.
Current SoH loads `.otr` and `.o2r` patch archives from `mods/`; later-loaded
archives can replace earlier assets, so packs with overlapping textures/models
must be installed intentionally and tested one at a time. There is no current
upstream Wii U compatibility/performance certification for OoT Reloaded or the
Djipi/Skilar 3D-style packs. For real Wii U, use a 1080p-oriented/light variant
if the author provides one, keep the archive names/version notes, and begin at
30 FPS before trying 60. Do not combine Reloaded and 3DS-style packs wholesale:
they overlap substantially. A lighter 3DS-style pack plus 60 FPS is the safer
goal; a full 4K Reloaded-plus-model-overhaul combination is not recommended
without per-area performance testing.

## Updating

Patch releases normally keep `oot.o2r`; a major `x.0.0` release requires
regenerating it. Before updating, back up `wiiu/apps/soh` (especially the JSON,
saves, and mods), build the new tag in a fresh checkout, stage it separately,
then replace only `soh.wuhb` and `soh.o2r` when the release does not require a
new ROM archive. Re-test with `mods/` temporarily moved out.

#!/usr/bin/env bash
# Build the pinned Shipwright source into an Aroma-compatible Wii U bundle.
set -euo pipefail

repo_dir="$(cd "$(dirname "$0")" && pwd)"
cd "$repo_dir"

rom_path="${1:?Usage: $0 /absolute/path/to/your-supported-oot-rom.z64}"
[[ -f "$rom_path" ]] || { echo "ROM path is not a regular file: $rom_path" >&2; exit 1; }
rom_sha1="$(shasum -a 1 "$rom_path" | awk '{print $1}')"
rg -F '"'"$rom_sha1"'"' docs/supportedHashes.json >/dev/null || {
  echo "ROM SHA-1 is not supported by this source: $rom_sha1" >&2; exit 1;
}

if [[ "$(git rev-parse HEAD)" != "cb71e22a79bc5d1f688fa881795bbd93094895fc" ]]; then
  echo "Refusing: this toolkit is pinned to Shipwright 9.2.3 (cb71e22...)." >&2
  exit 1
fi

export DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
export DEVKITPPC="${DEVKITPPC:-$DEVKITPRO/devkitPPC}"
echo "DEVKITPRO=$DEVKITPRO"
echo "DEVKITPPC=$DEVKITPPC"
[[ -f "$DEVKITPRO/cmake/WiiU.cmake" ]] || { echo "Missing $DEVKITPRO/cmake/WiiU.cmake" >&2; exit 1; }
[[ -x "$DEVKITPPC/bin/powerpc-eabi-g++" ]] || { echo "Missing devkitPPC compiler" >&2; exit 1; }
command -v cmake >/dev/null
command -v ninja >/dev/null
command -v git >/dev/null

build_dir="${SOH_WIIU_BUILD_DIR:-build-wiiu}"
[[ "$build_dir" != /* ]] || { echo "SOH_WIIU_BUILD_DIR must be relative to the repository." >&2; exit 1; }

# OTRExporter discovers ROMs in its own directory. This temporary copy is
# removed on exit; neither the ROM nor ROM-derived archives are git-tracked.
rom_temp="$(mktemp "$repo_dir/OTRExporter/soh-rom.XXXXXX")"
rom_copy="${rom_temp}.z64"
mv "$rom_temp" "$rom_copy"
trap 'rm -f "$rom_copy"' EXIT
cp "$rom_path" "$rom_copy"

# The host target generates archives.  On macOS 26, the bundled fmt in this
# source release needs FMT_CONSTEVAL disabled; this changes no source files.
if [[ "$(uname)" == "Darwin" ]] && command -v brew >/dev/null; then
  llvm_prefix="$(brew --prefix llvm 2>/dev/null || true)"
  if [[ -x "$llvm_prefix/bin/clang" && -x "$llvm_prefix/bin/clang++" ]]; then
    export CC="$llvm_prefix/bin/clang"
    export CXX="$llvm_prefix/bin/clang++"
  fi
fi

cmake -S . -B build-host -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DPython3_EXECUTABLE="$(command -v python3)" '-DCMAKE_CXX_FLAGS:STRING=-DFMT_CONSTEVAL='
cmake --build build-host --target ExtractAssets --parallel 4

scripts/wiiu/build-portlibs.sh "$build_dir"

cmake -S . -B "$build_dir" -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/WiiU.cmake" \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DSOH_WIIU_PORTLIBS_PREFIX="$repo_dir/$build_dir/local-portlibs"
cmake --build "$build_dir" --target soh_wuhb --parallel 4

test -s "$build_dir/soh/soh.wuhb"
test -s "$build_dir/gamecontrollerdb.txt"
echo "Built $(git describe --tags --always) ($(git rev-parse HEAD))"
ls -lh "$build_dir/soh/soh.wuhb" "$build_dir/gamecontrollerdb.txt" oot.o2r soh.o2r

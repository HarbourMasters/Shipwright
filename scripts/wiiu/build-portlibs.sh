#!/usr/bin/env bash
# Build the audio libraries unavailable in devkitPro's current Wii U packages.
set -euo pipefail

repo_dir="$(cd "$(dirname "$0")/../.." && pwd)"
build_dir="${1:?Usage: $0 /absolute/or/repository-relative/build-directory}"
case "$build_dir" in
  /*) ;;
  *) build_dir="$repo_dir/$build_dir" ;;
esac

: "${DEVKITPRO:=/opt/devkitpro}"
: "${DEVKITPPC:=$DEVKITPRO/devkitPPC}"
export DEVKITPRO DEVKITPPC
[[ -f "$DEVKITPRO/cmake/WiiU.cmake" ]] || { echo "Missing Wii U toolchain." >&2; exit 1; }
command -v cmake >/dev/null
command -v ninja >/dev/null
command -v git >/dev/null

src_dir="$build_dir/local-deps-src"
work_dir="$build_dir/local-deps-build"
prefix="$build_dir/local-portlibs"
mkdir -p "$src_dir" "$work_dir" "$prefix"

fetch() {
  local name="$1" url="$2" revision="$3"
  if [[ ! -d "$src_dir/$name/.git" ]]; then
    git clone --filter=blob:none "$url" "$src_dir/$name"
  fi
  git -C "$src_dir/$name" fetch --quiet --tags origin "$revision"
  git -C "$src_dir/$name" checkout --quiet --detach "$revision"
  [[ "$(git -C "$src_dir/$name" rev-parse HEAD)" == "$revision" ]] || {
    echo "Unexpected revision for $name" >&2; exit 1;
  }
}

fetch ogg https://github.com/xiph/ogg.git be05b13e98b048f0b5a0f5fa8ce514d56db5f822
fetch opus https://github.com/xiph/opus.git 22244de5a79bd1d6d623c32e72bf1954b56235be
fetch opusfile https://github.com/xiph/opusfile.git a55c164e9891a9326188b7d4d216ec9a88373739
fetch vorbis https://github.com/xiph/vorbis.git 0657aee69dec8508a0011f47f3b69d7538e9d262

# The source tree is deliberately mutable only inside the selected build dir.
cmake -E copy_if_different "$repo_dir/scripts/wiiu/opusfile-CMakeLists.txt" "$src_dir/opusfile/CMakeLists.txt"

configure_build_install() {
  local name="$1"
  shift
  cmake -S "$src_dir/$name" -B "$work_dir/$name" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/WiiU.cmake" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_INSTALL_PREFIX="$prefix" \
    "$@"
  cmake --build "$work_dir/$name" --parallel 4
  cmake --install "$work_dir/$name"
}

configure_build_install ogg -DBUILD_SHARED_LIBS=OFF -DINSTALL_DOCS=OFF
configure_build_install opus -DOPUS_BUILD_SHARED_LIBRARY=OFF -DOPUS_BUILD_PROGRAMS=OFF -DOPUS_BUILD_TESTING=OFF
configure_build_install opusfile -DCMAKE_PREFIX_PATH="$prefix"
configure_build_install vorbis -DBUILD_SHARED_LIBS=OFF -DCMAKE_PREFIX_PATH="$prefix"

for lib in libogg.a libopus.a libopusfile.a libvorbis.a libvorbisenc.a libvorbisfile.a; do
  [[ -s "$prefix/lib/$lib" ]] || { echo "Missing $prefix/lib/$lib" >&2; exit 1; }
done
echo "Built isolated Wii U audio portlibs in $prefix"

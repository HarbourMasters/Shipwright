#!/usr/bin/env bash
#
# Assembles the five test .o2r archives under o2r/ from the assets in src/assets/.
# An .o2r is just a zip; entries must be stored at "textures/nintendo_rogo_static/..."
# so their CRC64 matches the paths the game requests.
#
# Drop exactly ONE of these into <app-dir>/mods/ (e.g. build/soh/mods/) at a time.
set -euo pipefail
cd "$(dirname "$0")"
ROOT="$(pwd)"
ASSETS="$ROOT/src/assets/textures/nintendo_rogo_static"

command -v zip >/dev/null || { echo "error: 'zip' is required" >&2; exit 1; }

DEFAULT_FILES=(gShipLogoDL mat_gShipLogoDL_f3d_material mat_gShipLogoDL_f3d_material_001 mat_gShipLogoDL_f3d_material_002 mat_gShipLogoDL_f3d_material_003)
TARGET_FILES=(fancyShipDL.xml mat_fancyShipDL_f3d_material mat_fancyShipDL_f3d_material_001 mat_fancyShipDL_f3d_material_002 mat_fancyShipDL_f3d_material_003)
META_FILE=(gShipLogoDL.meta)

mkdir -p "$ROOT/o2r"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

build_case() {
    local out="$1"; local mname="$2"; shift 2
    local dir="$TMP/$out"
    mkdir -p "$dir/textures/nintendo_rogo_static"
    printf '{"name":"%s","code_version":1}\n' "$mname" > "$dir/manifest.json"
    local f
    for f in "$@"; do
        cp "$ASSETS/$f" "$dir/textures/nintendo_rogo_static/$f"
    done
    rm -f "$ROOT/o2r/$out.o2r"
    ( cd "$dir" && zip -q -X -r "$ROOT/o2r/$out.o2r" manifest.json textures )
    echo "built o2r/$out.o2r"
}

build_case case1-real-meta-target   "meta-test 1 real+meta+target"  "${DEFAULT_FILES[@]}" "${META_FILE[@]}" "${TARGET_FILES[@]}"
build_case case2-real-meta-notarget "meta-test 2 real+meta"         "${DEFAULT_FILES[@]}" "${META_FILE[@]}"
build_case case3-noreal-meta-target "meta-test 3 meta+target"       "${META_FILE[@]}" "${TARGET_FILES[@]}"
build_case case4-meta-only          "meta-test 4 meta-only"         "${META_FILE[@]}"
build_case case5-real-nometa        "meta-test 5 real (no meta)"    "${DEFAULT_FILES[@]}"

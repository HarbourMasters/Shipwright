#!/usr/bin/env bash
#
# Assembles the test .o2r archives from the assets in src/assets/.
# An .o2r is just a zip; entries must be stored at "textures/nintendo_rogo_static/..."
# so their CRC64 matches the paths the game requests.
#
#   o2r/case*.o2r          single-archive cases (drop ONE into mods/)
#   o2r/layered/L*.o2r     layered cases (drop a matching 10-/20- pair into mods/;
#                          20-* must be the higher-priority archive — see README)
set -euo pipefail
cd "$(dirname "$0")"
ROOT="$(pwd)"
ASSETS="$ROOT/src/assets/textures/nintendo_rogo_static"

command -v zip >/dev/null || { echo "error: 'zip' is required" >&2; exit 1; }

# Building blocks (by color): real=green, alias target=magenta, meta=alias->fancyShipDL.xml
REAL=(gShipLogoDL mat_gShipLogoDL_f3d_material mat_gShipLogoDL_f3d_material_001 mat_gShipLogoDL_f3d_material_002 mat_gShipLogoDL_f3d_material_003)
TARGET=(fancyShipDL.xml mat_fancyShipDL_f3d_material mat_fancyShipDL_f3d_material_001 mat_fancyShipDL_f3d_material_002 mat_fancyShipDL_f3d_material_003)
META=(gShipLogoDL.meta)

mkdir -p "$ROOT/o2r/layered"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

build_o2r() {  # <out-relpath-without-ext> <manifest-name> <files...>
    local out="$1"; local mname="$2"; shift 2
    local dir="$TMP/$(basename "$out")"
    rm -rf "$dir"; mkdir -p "$dir/textures/nintendo_rogo_static"
    printf '{"name":"%s","code_version":1}\n' "$mname" > "$dir/manifest.json"
    local f
    for f in "$@"; do
        cp "$ASSETS/$f" "$dir/textures/nintendo_rogo_static/$f"
    done
    # Fixed timestamps so rebuilds are byte-for-byte reproducible (no git churn).
    find "$dir" -exec touch -d '1980-01-01 00:00:00' {} +
    rm -f "$ROOT/o2r/$out.o2r"
    ( cd "$dir" && zip -q -X -r "$ROOT/o2r/$out.o2r" manifest.json textures )
    echo "built o2r/$out.o2r"
}

# --- single-archive cases ---
build_o2r case1-real-meta-target   "meta-test 1 real+meta+target"  "${REAL[@]}" "${META[@]}" "${TARGET[@]}"
build_o2r case2-real-meta-notarget "meta-test 2 real+meta"         "${REAL[@]}" "${META[@]}"
build_o2r case3-noreal-meta-target "meta-test 3 meta+target"       "${META[@]}" "${TARGET[@]}"
build_o2r case4-meta-only          "meta-test 4 meta-only"         "${META[@]}"
build_o2r case5-real-nometa        "meta-test 5 real (no meta)"    "${REAL[@]}"

# --- layered cases: 20-* is the higher-priority archive (loaded last, should win) ---
# L1  higher real must beat lower meta+target  -> green (bug shows magenta)
build_o2r layered/L1-10-lower-meta-target   "L1 lower meta+target"  "${META[@]}" "${TARGET[@]}"
build_o2r layered/L1-20-higher-real         "L1 higher real"        "${REAL[@]}"
# L1b higher real over lower meta-only         -> green
build_o2r layered/L1b-10-lower-meta-only    "L1b lower meta-only"   "${META[@]}"
build_o2r layered/L1b-20-higher-real        "L1b higher real"       "${REAL[@]}"
# L2  higher meta+target beats lower real      -> magenta
build_o2r layered/L2-10-lower-real          "L2 lower real"         "${REAL[@]}"
build_o2r layered/L2-20-higher-meta-target  "L2 higher meta+target" "${META[@]}" "${TARGET[@]}"
# L3  higher meta-only over lower real         -> green or nothing (open question)
build_o2r layered/L3-10-lower-real          "L3 lower real"         "${REAL[@]}"
build_o2r layered/L3-20-higher-meta-only    "L3 higher meta-only"   "${META[@]}"

# L5  cross-game motivation (order-independent): a base ships the alias + its native real,
#     a SEPARATE mod supplies the shared alias target.
#       base + target present -> magenta ("mod loaded")
#       base alone            -> green   ("vanilla boot", falls back — same as case2)
build_o2r layered/L5-base-real-meta         "L5 base real+meta"     "${REAL[@]}" "${META[@]}"
build_o2r layered/L5-target                 "L5 shared target"      "${TARGET[@]}"

# L6  "reach back": the winning (highest) .meta overrides a lower real, and resolves its target
#     from a SEPARATE archive that may be lower priority than the .meta (a different path, so it's
#     a normal global lookup). All three present -> magenta.
build_o2r layered/L6-10-lower-real          "L6 lower real"         "${REAL[@]}"
build_o2r layered/L6-20-mid-target          "L6 mid target"         "${TARGET[@]}"
build_o2r layered/L6-30-higher-meta         "L6 higher meta"        "${META[@]}"

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

# --- layered cases: NN-* prefix = intended priority (higher number = higher / loaded last) ---

# Flagship (cross-game): three mods replicating 2ship / mm / mod.
#   mod1 meta (2ship) < mod2 real,green (mm) < mod3 target,magenta (mod)
#     all three loaded  -> magenta (alias target outranks the real) — "mod loaded"
#     drop mod3         -> green   (mod2's real, target absent)     — "vanilla"
build_o2r layered/flagship-10-meta    "flagship mod1 meta (2ship)"  "${META[@]}"
build_o2r layered/flagship-20-real    "flagship mod2 real (mm)"     "${REAL[@]}"
build_o2r layered/flagship-30-target  "flagship mod3 target (mod)"  "${TARGET[@]}"

# L1: a real ABOVE the alias target -> the real wins.
#   low = meta + target (magenta),  high = real (green)  -> green
build_o2r layered/L1-10-meta-target   "L1 low meta+target"  "${META[@]}" "${TARGET[@]}"
build_o2r layered/L1-20-real          "L1 high real"        "${REAL[@]}"

# Reach-back: the alias target lives in an archive BELOW the .meta -> still resolves.
#   low = target (magenta),  high = meta  -> magenta
build_o2r layered/reachback-10-target "reachback low target" "${TARGET[@]}"
build_o2r layered/reachback-20-meta   "reachback high meta"  "${META[@]}"

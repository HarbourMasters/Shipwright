#!/usr/bin/env bash
#
# Regenerates the test assets under src/assets/ from SoH's real ship-logo
# DisplayList (soh/assets/custom/textures/nintendo_rogo_static/gShipLogoDL).
#
# Produces two visually distinct variants so a tester can tell at a glance which
# asset the engine loaded for the boot ("LUS") logo:
#   * default / real    -> gShipLogoDL      + green materials
#   * replacement/target -> fancyShipDL.xml + magenta materials
# Plus the alias sidecar gShipLogoDL.meta (path -> fancyShipDL.xml).
#
# The heavy geometry (*_tri_* / *_vtx_*) and the render-mode reverts are NOT
# copied here on purpose: the test .o2r files never shadow those keys, so they
# keep resolving from soh.o2r and each archive stays tiny.
set -euo pipefail
cd "$(dirname "$0")/.."   # repo root

SRC=soh/assets/custom/textures/nintendo_rogo_static
OUT=meta-alias-testing/src/assets/textures/nintendo_rogo_static
mkdir -p "$OUT"

GREEN='<SetPrimColor M="0" L="0" R="0" G="255" B="0" A="255"/>'
MAGENTA='<SetPrimColor M="0" L="0" R="255" G="0" B="255" A="255"/>'

# --- default / real: unchanged DisplayList, materials recolored solid green ---
cp "$SRC/gShipLogoDL" "$OUT/gShipLogoDL"
for suf in "" _001 _002 _003; do
    sed -E "s#<SetPrimColor[^>]*>#$GREEN#" \
        "$SRC/mat_gShipLogoDL_f3d_material$suf" > "$OUT/mat_gShipLogoDL_f3d_material$suf"
done

# --- replacement / target: same geometry, its own materials, recolored magenta ---
# Repoint only the material calls (mat_gShipLogoDL_f3d_material*) at a private set;
# the mat_revert_* and *_tri_* calls keep pointing at the shared soh.o2r resources.
sed 's#mat_gShipLogoDL_f3d_material#mat_fancyShipDL_f3d_material#g' \
    "$SRC/gShipLogoDL" > "$OUT/fancyShipDL.xml"
for suf in "" _001 _002 _003; do
    sed -E "s#<SetPrimColor[^>]*>#$MAGENTA#" \
        "$SRC/mat_gShipLogoDL_f3d_material$suf" > "$OUT/mat_fancyShipDL_f3d_material$suf"
done

# --- alias sidecar (same shape as HarbourMasters/Shipwright commit f5d2e58) ---
cat > "$OUT/gShipLogoDL.meta" <<'JSON'
{
    "path": "textures/nintendo_rogo_static/fancyShipDL.xml",
    "type": "DisplayList",
    "format": "XML",
    "version": 0
}
JSON

echo "Regenerated assets in $OUT"
ls -1 "$OUT"

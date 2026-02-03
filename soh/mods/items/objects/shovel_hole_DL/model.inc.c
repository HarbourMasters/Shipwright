// Simple brown circle for shovel hole
// Creates a flat disk on the ground

Vtx g_shovelhole_vtx[] = {
    // Center vertex
    {{{ 0, 0, 0 }, 0, { 512, 512 }, { 255, 255, 255, 255 }}},

    // Outer ring vertices (circle with 12 segments)
    {{{ 30, 0, 0 }, 0, { 1024, 512 }, { 255, 255, 255, 255 }}},
    {{{ 26, 0, 15 }, 0, { 956, 700 }, { 255, 255, 255, 255 }}},
    {{{ 15, 0, 26 }, 0, { 700, 856 }, { 255, 255, 255, 255 }}},
    {{{ 0, 0, 30 }, 0, { 512, 1024 }, { 255, 255, 255, 255 }}},
    {{{ -15, 0, 26 }, 0, { 324, 856 }, { 255, 255, 255, 255 }}},
    {{{ -26, 0, 15 }, 0, { 68, 700 }, { 255, 255, 255, 255 }}},
    {{{ -30, 0, 0 }, 0, { 0, 512 }, { 255, 255, 255, 255 }}},
    {{{ -26, 0, -15 }, 0, { 68, 324 }, { 255, 255, 255, 255 }}},
    {{{ -15, 0, -26 }, 0, { 324, 168 }, { 255, 255, 255, 255 }}},
    {{{ 0, 0, -30 }, 0, { 512, 0 }, { 255, 255, 255, 255 }}},
    {{{ 15, 0, -26 }, 0, { 700, 168 }, { 255, 255, 255, 255 }}},
    {{{ 26, 0, -15 }, 0, { 856, 324 }, { 255, 255, 255, 255 }}},
};

Gfx g_shovelhole_dl[] = {
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
    gsDPPipeSync(),
    gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPSetOtherMode(G_SETOTHERMODE_H, 4, 20, G_TF_BILERP | G_TC_FILT | G_CD_MAGICSQ | G_TT_NONE | G_TL_TILE | G_TP_PERSP | G_TD_CLAMP | G_CYC_2CYCLE | G_PM_NPRIMITIVE),
    gsSPSetOtherMode(G_SETOTHERMODE_L, 0, 32, G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_XLU_SURF2),
    gsSPTexture(65535, 65535, 0, 0, 1),

    // Color is set externally via gDPSetPrimColor
    gsSPVertex(g_shovelhole_vtx, 13, 0),

    // Draw triangles from center to outer ring (making a circle)
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 3, 4, 0, 0, 4, 5, 0),
    gsSP2Triangles(0, 5, 6, 0, 0, 6, 7, 0),
    gsSP2Triangles(0, 7, 8, 0, 0, 8, 9, 0),
    gsSP2Triangles(0, 9, 10, 0, 0, 10, 11, 0),
    gsSP2Triangles(0, 11, 12, 0, 0, 12, 1, 0),

    gsSPEndDisplayList(),
};

// Merged vertex array: 0-15 (Leaf), 16-24 (Stem/Veins)
Vtx g_dekuleaf_vtx[] = {
    // Leaf Body
    {{{ -8, -1, 0 }, 0, { -16, 1008 }, { 0, 0, 129, 255 }}},
    {{{ -5, 4, 0 }, 0, { -16, 1008 }, { 0, 0, 129, 255 }}},
    {{{ -1, 5, 0 }, 0, { -16, 1008 }, { 0, 0, 129, 255 }}},
    {{{ -8, -1, 0 }, 0, { -16, 1008 }, { 242, 5, 126, 255 }}},
    {{{ -7, -7, 0 }, 0, { -16, 1008 }, { 150, 43, 54, 255 }}},
    {{{ -7, -7, 0 }, 0, { -16, 1008 }, { 17, 88, 166, 255 }}},
    {{{ -1, -8, 0 }, 0, { -16, 1008 }, { 237, 125, 244, 255 }}},
    {{{ 5, -1, 0 }, 0, { -16, 1008 }, { 34, 248, 122, 255 }}},
    {{{ 7, 7, 0 }, 0, { -16, 1008 }, { 78, 5, 156, 255 }}},
    {{{ -1, 5, 0 }, 0, { -16, 1008 }, { 245, 48, 117, 255 }}},
    {{{ -5, 4, 0 }, 0, { -16, 1008 }, { 231, 32, 120, 255 }}},
    {{{ -1, -8, 0 }, 0, { -16, 1008 }, { 0, 0, 127, 255 }}},
    {{{ 4, -5, 0 }, 0, { -16, 1008 }, { 0, 0, 127, 255 }}},
    {{{ 4, -5, 0 }, 0, { -16, 1008 }, { 205, 113, 230, 255 }}},
    {{{ 5, -1, 0 }, 0, { -16, 1008 }, { 0, 0, 127, 255 }}},
    {{{ 7, 7, 0 }, 0, { -16, 1008 }, { 26, 49, 114, 255 }}},
    // Stem/Veins
    {{{ -8, -8, -1 }, 0, { 752, 506 }, { 205, 205, 151, 255 }}},
    {{{ -7, -8, 0 }, 0, { 998, 752 }, { 23, 131, 0, 255 }}},
    {{{ -8, -8, 1 }, 0, { 752, 998 }, { 205, 205, 105, 255 }}},
    {{{ -8, -7, 0 }, 0, { 506, 752 }, { 131, 23, 0, 255 }}},
    {{{ -8, -8, -1 }, 0, { 240, 506 }, { 205, 205, 151, 255 }}},
    {{{ 6, 7, 0 }, 0, { 240, 752 }, { 90, 90, 0, 255 }}},
    {{{ -7, -8, 0 }, 0, { 486, 752 }, { 23, 131, 0, 255 }}},
    {{{ -8, -7, 0 }, 0, { -6, 752 }, { 131, 23, 0, 255 }}},
    {{{ -8, -8, 1 }, 0, { 240, 998 }, { 205, 205, 105, 255 }}},
};

Gfx g_dekuleaf_dl[] = {
    gsSPLoadGeometryMode(G_CULL_BACK | G_ZBUFFER | G_LIGHTING | G_FOG | G_SHADING_SMOOTH | G_SHADE),
    gsDPPipeSync(),
    gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
    gsSPSetOtherMode(G_SETOTHERMODE_H, 4, 20, G_TF_BILERP | G_TC_FILT | G_CD_MAGICSQ | G_TT_NONE | G_TL_TILE | G_TP_PERSP | G_TD_CLAMP | G_CYC_2CYCLE | G_PM_NPRIMITIVE | G_CK_NONE | G_AD_NOISE),
    gsSPSetOtherMode(G_SETOTHERMODE_L, 0, 32, G_AC_NONE | G_RM_AA_ZB_OPA_SURF2 | G_ZS_PIXEL | G_RM_FOG_SHADE_A),
    gsSPTexture(65535, 65535, 0, 0, 1),
    
    // Draw Leaf Body
    gsDPSetPrimColor(0, 0, 74, 184, 63, 255),
    gsSPVertex(g_dekuleaf_vtx, 16, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 0, 3, 0),
    gsSP2Triangles(0, 4, 3, 0, 0, 5, 4, 0),
    gsSP2Triangles(6, 5, 0, 0, 7, 6, 0, 0),
    gsSP2Triangles(0, 2, 7, 0, 2, 8, 7, 0),
    gsSP2Triangles(8, 2, 9, 0, 2, 10, 9, 0),
    gsSP2Triangles(2, 1, 10, 0, 1, 3, 10, 0),
    gsSP2Triangles(9, 10, 3, 0, 9, 3, 11, 0),
    gsSP2Triangles(3, 4, 11, 0, 5, 11, 4, 0),
    gsSP2Triangles(5, 6, 11, 0, 6, 12, 11, 0),
    gsSP2Triangles(6, 13, 12, 0, 7, 13, 6, 0),
    gsSP2Triangles(13, 7, 14, 0, 7, 15, 14, 0),
    gsSP2Triangles(7, 8, 15, 0, 8, 9, 15, 0),
    gsSP2Triangles(14, 15, 9, 0, 11, 14, 9, 0),
    gsSP2Triangles(11, 12, 14, 0, 13, 14, 12, 0),

    // Draw Stem
    gsDPSetPrimColor(0, 0, 0, 59, 0, 255),
    gsSPVertex(&g_dekuleaf_vtx[16], 9, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 7, 5, 4, 0),
    gsSP2Triangles(8, 5, 7, 0, 6, 5, 8, 0),
    
    gsSPEndDisplayList(),
};

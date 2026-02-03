/**
 * Magic Spell Give DLs - Recolored variants of OoT goddess spell model
 * Source: C:\Users\LENOVO\Documents\oot (object_gi_goddess)
 * Items: Hylia's Grace (pink), Zonai Permafrost (turquoise), Demise Destruction (black)
 */

#include "align_asset_macro.h"

#ifndef dgEffUnknown10Tex
#define dgEffUnknown10Tex "__OTR__objects/gameplay_keep/gEffUnknown10Tex"
static const ALIGN_ASSET(2) char gEffUnknown10Tex[] = dgEffUnknown10Tex;
#endif

#ifndef dgEffUnknown12Tex
#define dgEffUnknown12Tex "__OTR__objects/gameplay_keep/gEffUnknown12Tex"
static const ALIGN_ASSET(2) char gEffUnknown12Tex[] = dgEffUnknown12Tex;
#endif

#include "header.h"

// ============================================================================
// Diamond Vertices (20 vertices - outer crystal shape)
// From: gGiMagicSpellDiamondVtx.inc.c
// ============================================================================
static Vtx sMagicSpellDiamondVtx[] = {
    VTX(    22,      0,     22,   0x180,   0x600, 0x43, 0x49, 0x43, 0xFF),
    VTX(    22,      0,    -22,   0x280,   0x600, 0x43, 0x49, 0xBD, 0xFF),
    VTX(     0,     37,      0,   0x200,   0x400, 0x3E, 0x66, 0x00, 0xFF),
    VTX(   -22,      0,     22,    0x80,   0x600, 0xBD, 0x49, 0x43, 0xFF),
    VTX(    22,      0,     22,   0x180,   0x600, 0x43, 0x49, 0x43, 0xFF),
    VTX(     0,     37,      0,   0x100,   0x400, 0x3E, 0x66, 0x00, 0xFF),
    VTX(     0,     37,      0,     0x0,   0x400, 0x97, 0x39, 0x00, 0xFF),
    VTX(   -22,      0,    -22,   -0x80,   0x600, 0xBD, 0x49, 0xBD, 0xFF),
    VTX(   -22,      0,    -22,   0x380,   0x600, 0xBD, 0x49, 0xBD, 0xFF),
    VTX(     0,     37,      0,   0x300,   0x400, 0x3E, 0x66, 0x00, 0xFF),
    VTX(    22,      0,    -22,   0x280,   0x600, 0x43, 0x49, 0xBD, 0xFF),
    VTX(     0,    -37,      0,   0x200,   0x400, 0x69, 0xC7, 0x00, 0xFF),
    VTX(    22,      0,    -22,   0x280,   0x600, 0x43, 0xB7, 0xBD, 0xFF),
    VTX(    22,      0,     22,   0x180,   0x600, 0x43, 0xB7, 0x43, 0xFF),
    VTX(     0,    -37,      0,   0x100,   0x400, 0xC2, 0x9A, 0x00, 0xFF),
    VTX(   -22,      0,     22,    0x80,   0x600, 0xBD, 0xB7, 0x43, 0xFF),
    VTX(   -22,      0,    -22,   -0x80,   0x600, 0xBD, 0xB7, 0xBD, 0xFF),
    VTX(     0,    -37,      0,     0x0,   0x400, 0xC2, 0x9A, 0x00, 0xFF),
    VTX(     0,    -37,      0,   0x300,   0x400, 0xC2, 0x9A, 0x00, 0xFF),
    VTX(   -22,      0,    -22,   0x380,   0x600, 0xBD, 0xB7, 0xBD, 0xFF),
};

// ============================================================================
// Orb Vertices (126 vertices - inner sphere/core)
// From: gGiMagicSpellOrbVtx.inc.c
// ============================================================================
static Vtx sMagicSpellOrbVtx[] = {
    VTX(     0,     15,      0,   0x200,   0x400, 0x00, 0x78, 0x00, 0xFF),
    VTX(     8,     13,      0,   0x200,   0x4B4, 0x41, 0x65, 0x00, 0xFF),
    VTX(     2,     13,     -8,   0x2CD,   0x4B4, 0x14, 0x65, 0xC3, 0xFF),
    VTX(     8,     13,      0,   0x200,   0x4B4, 0x41, 0x65, 0x00, 0xFF),
    VTX(    13,      7,      0,   0x200,   0x569, 0x6B, 0x35, 0x00, 0xFF),
    VTX(    11,      7,     -8,   0x266,   0x569, 0x54, 0x3C, 0xC3, 0xFF),
    VTX(     2,     13,     -8,   0x2CD,   0x4B4, 0x14, 0x65, 0xC3, 0xFF),
    VTX(     4,      7,    -13,   0x2CD,   0x569, 0x21, 0x35, 0x9A, 0xFF),
    VTX(    14,      0,      5,   0x1CD,   0x600, 0x72, 0x00, 0x25, 0xFF),
    VTX(    14,      0,     -5,   0x233,   0x600, 0x72, 0x00, 0xDB, 0xFF),
    VTX(    11,     -7,      8,   0x19A,   0x697, 0x57, 0xCB, 0x3F, 0xFF),
    VTX(    13,     -7,      0,   0x200,   0x697, 0x67, 0xC4, 0x00, 0xFF),
    VTX(    11,     -7,     -8,   0x266,   0x697, 0x57, 0xCB, 0xC1, 0xFF),
    VTX(     0,     15,      0,   0x2CD,   0x400, 0x00, 0x78, 0x00, 0xFF),
    VTX(     2,     13,     -8,   0x2CD,   0x4B4, 0x14, 0x65, 0xC3, 0xFF),
    VTX(    -6,     13,     -5,   0x39A,   0x4B4, 0xCC, 0x65, 0xDA, 0xFF),
    VTX(     4,      7,    -13,   0x2CD,   0x569, 0x21, 0x35, 0x9A, 0xFF),
    VTX(    -4,      7,    -13,   0x333,   0x569, 0xE0, 0x3C, 0x9E, 0xFF),
    VTX(   -11,      7,     -8,   0x39A,   0x569, 0xA9, 0x35, 0xC1, 0xFF),
    VTX(     9,      0,    -12,   0x29A,   0x600, 0x46, 0x00, 0x9F, 0xFF),
    VTX(     0,      0,    -15,   0x300,   0x600, 0x00, 0x00, 0x88, 0xFF),
    VTX(    11,     -7,     -8,   0x266,   0x697, 0x57, 0xCB, 0xC1, 0xFF),
    VTX(     4,     -7,    -13,   0x2CD,   0x697, 0x20, 0xC4, 0x9E, 0xFF),
    VTX(    -4,     -7,    -13,   0x333,   0x697, 0xDF, 0xCB, 0x9A, 0xFF),
    VTX(     0,     15,      0,   -0x66,   0x400, 0x00, 0x78, 0x00, 0xFF),
    VTX(    -6,     13,     -5,   -0x66,   0x4B4, 0xCC, 0x65, 0xDA, 0xFF),
    VTX(    -6,     13,      5,    0x66,   0x4B4, 0xCC, 0x65, 0x26, 0xFF),
    VTX(    -6,     13,     -5,   0x39A,   0x4B4, 0xCC, 0x65, 0xDA, 0xFF),
    VTX(   -11,      7,     -8,   0x39A,   0x569, 0xA9, 0x35, 0xC1, 0xFF),
    VTX(   -13,      7,      0,   0x400,   0x569, 0x99, 0x3C, 0x00, 0xFF),
    VTX(   -13,      7,      0,     0x0,   0x569, 0x99, 0x3C, 0x00, 0xFF),
    VTX(   -11,      7,      8,    0x66,   0x569, 0xA9, 0x35, 0x3F, 0xFF),
    VTX(    -9,      0,    -12,   0x366,   0x600, 0xBA, 0x00, 0x9F, 0xFF),
    VTX(   -14,      0,     -5,   0x3CD,   0x600, 0x8E, 0x00, 0xDB, 0xFF),
    VTX(    -9,      0,    -12,   0x366,   0x600, 0xBA, 0x00, 0x9F, 0xFF),
    VTX(    -4,     -7,    -13,   0x333,   0x697, 0xDF, 0xCB, 0x9A, 0xFF),
    VTX(   -11,     -7,     -8,   0x39A,   0x697, 0xAC, 0xC4, 0xC3, 0xFF),
    VTX(   -14,      0,     -5,   0x3CD,   0x600, 0x8E, 0x00, 0xDB, 0xFF),
    VTX(   -13,     -7,      0,   0x400,   0x697, 0x95, 0xCB, 0x00, 0xFF),
    VTX(     0,     15,      0,    0x66,   0x400, 0x00, 0x78, 0x00, 0xFF),
    VTX(    -6,     13,      5,    0x66,   0x4B4, 0xCC, 0x65, 0x26, 0xFF),
    VTX(     2,     13,      8,   0x133,   0x4B4, 0x14, 0x65, 0x3D, 0xFF),
    VTX(   -11,      7,      8,    0x66,   0x569, 0xA9, 0x35, 0x3F, 0xFF),
    VTX(    -4,      7,     13,    0xCD,   0x569, 0xE0, 0x3C, 0x62, 0xFF),
    VTX(     4,      7,     13,   0x133,   0x569, 0x21, 0x35, 0x66, 0xFF),
    VTX(   -14,      0,      5,    0x33,   0x600, 0x8E, 0x00, 0x25, 0xFF),
    VTX(    -9,      0,     12,    0x9A,   0x600, 0xBA, 0x00, 0x61, 0xFF),
    VTX(   -13,     -7,      0,     0x0,   0x697, 0x95, 0xCB, 0x00, 0xFF),
    VTX(   -11,     -7,      8,    0x66,   0x697, 0xAC, 0xC4, 0x3D, 0xFF),
    VTX(    -4,     -7,     13,    0xCD,   0x697, 0xDF, 0xCB, 0x66, 0xFF),
    VTX(     0,     15,      0,   0x133,   0x400, 0x00, 0x78, 0x00, 0xFF),
    VTX(     2,     13,      8,   0x133,   0x4B4, 0x14, 0x65, 0x3D, 0xFF),
    VTX(     8,     13,      0,   0x200,   0x4B4, 0x41, 0x65, 0x00, 0xFF),
    VTX(    11,      7,      8,   0x19A,   0x569, 0x54, 0x3C, 0x3D, 0xFF),
    VTX(    13,      7,      0,   0x200,   0x569, 0x6B, 0x35, 0x00, 0xFF),
    VTX(     4,      7,     13,   0x133,   0x569, 0x21, 0x35, 0x66, 0xFF),
    VTX(     0,      0,     15,   0x100,   0x600, 0x00, 0x00, 0x78, 0xFF),
    VTX(     9,      0,     12,   0x166,   0x600, 0x46, 0x00, 0x61, 0xFF),
    VTX(    -4,     -7,     13,    0xCD,   0x697, 0xDF, 0xCB, 0x66, 0xFF),
    VTX(     4,     -7,     13,   0x133,   0x697, 0x20, 0xC4, 0x62, 0xFF),
    VTX(    11,     -7,      8,   0x19A,   0x697, 0x57, 0xCB, 0x3F, 0xFF),
    VTX(     0,    -15,      0,   0x400,   0x800, 0x00, 0x88, 0x00, 0xFF),
    VTX(    -8,    -13,      0,   0x400,   0x74C, 0xBF, 0x9B, 0x00, 0xFF),
    VTX(    -2,    -13,     -8,   0x333,   0x74C, 0xEC, 0x9B, 0xC3, 0xFF),
    VTX(   -13,     -7,      0,   0x400,   0x697, 0x95, 0xCB, 0x00, 0xFF),
    VTX(   -11,     -7,     -8,   0x39A,   0x697, 0xAC, 0xC4, 0xC3, 0xFF),
    VTX(    -2,    -13,     -8,   0x333,   0x74C, 0xEC, 0x9B, 0xC3, 0xFF),
    VTX(   -11,     -7,     -8,   0x39A,   0x697, 0xAC, 0xC4, 0xC3, 0xFF),
    VTX(    -4,     -7,    -13,   0x333,   0x697, 0xDF, 0xCB, 0x9A, 0xFF),
    VTX(   -13,     -7,      0,     0x0,   0x697, 0x95, 0xCB, 0x00, 0xFF),
    VTX(   -14,      0,      5,    0x33,   0x600, 0x8E, 0x00, 0x25, 0xFF),
    VTX(   -14,      0,     -5,   -0x33,   0x600, 0x8E, 0x00, 0xDB, 0xFF),
    VTX(   -11,      7,      8,    0x66,   0x569, 0xA9, 0x35, 0x3F, 0xFF),
    VTX(   -13,      7,      0,     0x0,   0x569, 0x99, 0x3C, 0x00, 0xFF),
    VTX(   -14,      0,     -5,   0x3CD,   0x600, 0x8E, 0x00, 0xDB, 0xFF),
    VTX(   -13,      7,      0,   0x400,   0x569, 0x99, 0x3C, 0x00, 0xFF),
    VTX(   -11,      7,     -8,   0x39A,   0x569, 0xA9, 0x35, 0xC1, 0xFF),
    VTX(     0,    -15,      0,   0x333,   0x800, 0x00, 0x88, 0x00, 0xFF),
    VTX(    -2,    -13,     -8,   0x333,   0x74C, 0xEC, 0x9B, 0xC3, 0xFF),
    VTX(     6,    -13,     -5,   0x266,   0x74C, 0x34, 0x9B, 0xDA, 0xFF),
    VTX(    -4,     -7,    -13,   0x333,   0x697, 0xDF, 0xCB, 0x9A, 0xFF),
    VTX(     4,     -7,    -13,   0x2CD,   0x697, 0x20, 0xC4, 0x9E, 0xFF),
    VTX(    11,     -7,     -8,   0x266,   0x697, 0x57, 0xCB, 0xC1, 0xFF),
    VTX(    -9,      0,    -12,   0x366,   0x600, 0xBA, 0x00, 0x9F, 0xFF),
    VTX(     0,      0,    -15,   0x300,   0x600, 0x00, 0x00, 0x88, 0xFF),
    VTX(   -11,      7,     -8,   0x39A,   0x569, 0xA9, 0x35, 0xC1, 0xFF),
    VTX(    -4,      7,    -13,   0x333,   0x569, 0xE0, 0x3C, 0x9E, 0xFF),
    VTX(     4,      7,    -13,   0x2CD,   0x569, 0x21, 0x35, 0x9A, 0xFF),
    VTX(     0,    -15,      0,   0x266,   0x800, 0x00, 0x88, 0x00, 0xFF),
    VTX(     6,    -13,     -5,   0x266,   0x74C, 0x34, 0x9B, 0xDA, 0xFF),
    VTX(     6,    -13,      5,   0x19A,   0x74C, 0x34, 0x9B, 0x26, 0xFF),
    VTX(    11,     -7,     -8,   0x266,   0x697, 0x57, 0xCB, 0xC1, 0xFF),
    VTX(    13,     -7,      0,   0x200,   0x697, 0x67, 0xC4, 0x00, 0xFF),
    VTX(    11,     -7,      8,   0x19A,   0x697, 0x57, 0xCB, 0x3F, 0xFF),
    VTX(     9,      0,    -12,   0x29A,   0x600, 0x46, 0x00, 0x9F, 0xFF),
    VTX(    14,      0,     -5,   0x233,   0x600, 0x72, 0x00, 0xDB, 0xFF),
    VTX(     4,      7,    -13,   0x2CD,   0x569, 0x21, 0x35, 0x9A, 0xFF),
    VTX(    11,      7,     -8,   0x266,   0x569, 0x54, 0x3C, 0xC3, 0xFF),
    VTX(    14,      0,     -5,   0x233,   0x600, 0x72, 0x00, 0xDB, 0xFF),
    VTX(    11,      7,     -8,   0x266,   0x569, 0x54, 0x3C, 0xC3, 0xFF),
    VTX(    13,      7,      0,   0x200,   0x569, 0x6B, 0x35, 0x00, 0xFF),
    VTX(     0,    -15,      0,   0x19A,   0x800, 0x00, 0x88, 0x00, 0xFF),
    VTX(     6,    -13,      5,   0x19A,   0x74C, 0x34, 0x9B, 0x26, 0xFF),
    VTX(    -2,    -13,      8,    0xCD,   0x74C, 0xEC, 0x9B, 0x3D, 0xFF),
    VTX(    11,     -7,      8,   0x19A,   0x697, 0x57, 0xCB, 0x3F, 0xFF),
    VTX(     4,     -7,     13,   0x133,   0x697, 0x20, 0xC4, 0x62, 0xFF),
    VTX(    -4,     -7,     13,    0xCD,   0x697, 0xDF, 0xCB, 0x66, 0xFF),
    VTX(    14,      0,      5,   0x1CD,   0x600, 0x72, 0x00, 0x25, 0xFF),
    VTX(     9,      0,     12,   0x166,   0x600, 0x46, 0x00, 0x61, 0xFF),
    VTX(    13,      7,      0,   0x200,   0x569, 0x6B, 0x35, 0x00, 0xFF),
    VTX(    11,      7,      8,   0x19A,   0x569, 0x54, 0x3C, 0x3D, 0xFF),
    VTX(     4,      7,     13,   0x133,   0x569, 0x21, 0x35, 0x66, 0xFF),
    VTX(     0,    -15,      0,    0xCD,   0x800, 0x00, 0x88, 0x00, 0xFF),
    VTX(    -2,    -13,      8,    0xCD,   0x74C, 0xEC, 0x9B, 0x3D, 0xFF),
    VTX(    -8,    -13,      0,     0x0,   0x74C, 0xBF, 0x9B, 0x00, 0xFF),
    VTX(    -4,     -7,     13,    0xCD,   0x697, 0xDF, 0xCB, 0x66, 0xFF),
    VTX(   -11,     -7,      8,    0x66,   0x697, 0xAC, 0xC4, 0x3D, 0xFF),
    VTX(   -13,     -7,      0,     0x0,   0x697, 0x95, 0xCB, 0x00, 0xFF),
    VTX(     0,      0,     15,   0x100,   0x600, 0x00, 0x00, 0x78, 0xFF),
    VTX(    -9,      0,     12,    0x9A,   0x600, 0xBA, 0x00, 0x61, 0xFF),
    VTX(     4,      7,     13,   0x133,   0x569, 0x21, 0x35, 0x66, 0xFF),
    VTX(    -4,      7,     13,    0xCD,   0x569, 0xE0, 0x3C, 0x62, 0xFF),
    VTX(   -11,      7,      8,    0x66,   0x569, 0xA9, 0x35, 0x3F, 0xFF),
    VTX(     2,     13,      8,   0x133,   0x4B4, 0x14, 0x65, 0x3D, 0xFF),
    VTX(     4,      7,     13,   0x133,   0x569, 0x21, 0x35, 0x66, 0xFF),
    VTX(    11,      7,      8,   0x19A,   0x569, 0x54, 0x3C, 0x3D, 0xFF),
};

// ============================================================================
// Diamond Display List (outer crystal)
// From: gGiMagicSpellDiamondDL.inc.c (original colors: white prim, blue env)
// ============================================================================
static Gfx sMagicSpellDiamondDL[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_XLU_SURF2),
    gsDPSetCombineMode(G_CC_BLENDPEDECALA, G_CC_PASS2),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsDPSetEnvColor(0, 50, 200, 255),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0x1770, 0x1770, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown10Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1),
    gsSPClearGeometryMode(G_CULL_BACK | G_FOG),
    gsSPSetGeometryMode(G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(&sMagicSpellDiamondVtx[0], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPVertex(&sMagicSpellDiamondVtx[3], 17, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 4, 0),
    gsSP2Triangles(5, 6, 7, 0, 8, 9, 10, 0),
    gsSP2Triangles(11, 10, 12, 0, 13, 14, 12, 0),
    gsSP1Triangle(9, 15, 16, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// Orb Display List (inner sphere/core)
// From: gGiMagicSpellOrbDL.inc.c
// NOTE: gsSPDisplayList(0x08000000) REMOVED - segment refs crash in inline DLs
// ============================================================================
static Gfx sMagicSpellOrbDL[] = {
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, PRIMITIVE,
        ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, COMBINED),
    gsSPTexture(0x0FA0, 0x0FA0, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown10Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1),
    gsDPLoadMultiBlock(gEffUnknown12Tex, 0x0100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32,
        32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1),
    // gsSPDisplayList(0x08000000) removed - texture scroll set up externally
    gsSPVertex(&sMagicSpellOrbVtx[0], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPVertex(&sMagicSpellOrbVtx[3], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(3, 2, 4, 0, 1, 5, 6, 0),
    gsSP2Triangles(5, 7, 8, 0, 5, 8, 6, 0),
    gsSP2Triangles(6, 8, 9, 0, 10, 11, 12, 0),
    gsSP2Triangles(11, 13, 14, 0, 11, 14, 12, 0),
    gsSP2Triangles(12, 14, 15, 0, 13, 16, 17, 0),
    gsSP2Triangles(16, 18, 19, 0, 16, 19, 17, 0),
    gsSP2Triangles(17, 19, 20, 0, 21, 22, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 22, 27, 23, 0),
    gsSP2Triangles(23, 27, 28, 0, 25, 29, 30, 0),
    gsSPVertex(&sMagicSpellOrbVtx[34], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(3, 2, 4, 0, 5, 6, 7, 0),
    gsSP2Triangles(6, 8, 9, 0, 6, 9, 7, 0),
    gsSP2Triangles(7, 9, 10, 0, 8, 11, 12, 0),
    gsSP2Triangles(11, 13, 14, 0, 11, 14, 12, 0),
    gsSP2Triangles(12, 14, 15, 0, 16, 17, 18, 0),
    gsSP2Triangles(17, 19, 18, 0, 18, 19, 20, 0),
    gsSP2Triangles(21, 22, 23, 0, 22, 24, 25, 0),
    gsSP2Triangles(22, 25, 23, 0, 23, 25, 26, 0),
    gsSP2Triangles(27, 28, 29, 0, 28, 30, 31, 0),
    gsSP1Triangle(28, 31, 29, 0),
    gsSPVertex(&sMagicSpellOrbVtx[66], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(4, 6, 7, 0, 4, 7, 5, 0),
    gsSP2Triangles(8, 9, 10, 0, 11, 12, 13, 0),
    gsSP2Triangles(12, 14, 15, 0, 12, 15, 13, 0),
    gsSP2Triangles(13, 15, 16, 0, 14, 17, 18, 0),
    gsSP2Triangles(17, 19, 20, 0, 17, 20, 18, 0),
    gsSP2Triangles(18, 20, 21, 0, 22, 23, 24, 0),
    gsSP2Triangles(23, 25, 26, 0, 23, 26, 24, 0),
    gsSP2Triangles(24, 26, 27, 0, 25, 28, 29, 0),
    gsSP2Triangles(28, 30, 31, 0, 28, 31, 29, 0),
    gsSPVertex(&sMagicSpellOrbVtx[98], 28, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(4, 6, 7, 0, 4, 7, 5, 0),
    gsSP2Triangles(5, 7, 8, 0, 6, 9, 10, 0),
    gsSP2Triangles(9, 11, 12, 0, 9, 12, 10, 0),
    gsSP2Triangles(10, 12, 13, 0, 14, 15, 16, 0),
    gsSP2Triangles(15, 17, 18, 0, 15, 18, 16, 0),
    gsSP2Triangles(16, 18, 19, 0, 17, 20, 21, 0),
    gsSP2Triangles(20, 22, 23, 0, 20, 23, 21, 0),
    gsSP2Triangles(21, 23, 24, 0, 25, 26, 27, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// Per-item color DLs (orb recolors)
// ============================================================================

// Hylia's Grace - Pink/Violet core
static Gfx sHyliaGraceColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0x80, 255, 150, 255, 255),
    gsDPSetEnvColor(180, 0, 180, 255),
    gsSPEndDisplayList(),
};

// Zonai Permafrost - Turquoise/Green core
static Gfx sZonaiPermafrostColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0x80, 100, 255, 230, 255),
    gsDPSetEnvColor(0, 180, 130, 255),
    gsSPEndDisplayList(),
};

// Demise Destruction - Black core
static Gfx sDemiseDestructionColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0x80, 30, 30, 30, 255),
    gsDPSetEnvColor(0, 0, 0, 255),
    gsSPEndDisplayList(),
};

// Demise Destruction - Light gray diamond (same geometry, different colors)
static Gfx sDemiseDiamondDL[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_XLU_SURF2),
    gsDPSetCombineMode(G_CC_BLENDPEDECALA, G_CC_PASS2),
    gsDPSetPrimColor(0, 0, 230, 230, 235, 255),
    gsDPSetEnvColor(190, 190, 200, 255),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0x1770, 0x1770, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown10Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1),
    gsSPClearGeometryMode(G_CULL_BACK | G_FOG),
    gsSPSetGeometryMode(G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(&sMagicSpellDiamondVtx[0], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPVertex(&sMagicSpellDiamondVtx[3], 17, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 4, 0),
    gsSP2Triangles(5, 6, 7, 0, 8, 9, 10, 0),
    gsSP2Triangles(11, 10, 12, 0, 13, 14, 12, 0),
    gsSP1Triangle(9, 15, 16, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// Per-item combined Give DLs
// Order: Diamond (uses its own colors) -> Orb color -> Orb geometry
// ============================================================================

Gfx gHyliaGraceGiveDL[] = {
    gsSPDisplayList(sMagicSpellDiamondDL),
    gsSPDisplayList(sHyliaGraceColorDL),
    gsSPDisplayList(sMagicSpellOrbDL),
    gsSPEndDisplayList(),
};

Gfx gZonaiPermafrostGiveDL[] = {
    gsSPDisplayList(sMagicSpellDiamondDL),
    gsSPDisplayList(sZonaiPermafrostColorDL),
    gsSPDisplayList(sMagicSpellOrbDL),
    gsSPEndDisplayList(),
};

Gfx gDemiseDestructionGiveDL[] = {
    gsSPDisplayList(sDemiseDiamondDL),
    gsSPDisplayList(sDemiseDestructionColorDL),
    gsSPDisplayList(sMagicSpellOrbDL),
    gsSPEndDisplayList(),
};

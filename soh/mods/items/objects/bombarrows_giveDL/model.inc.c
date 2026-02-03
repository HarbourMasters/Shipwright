/**
 * Bomb Arrows Give DL - Arrow shaft + bomb bag at the arrowhead tip
 * Sources: C:\Users\LENOVO\Documents\oot (object_gi_m_arrow, object_gi_bombpouch)
 * Arrow shaft from gGiMagicArrowDL (colors darkened to match bomb bag)
 * Bomb bag scaled 0.75x, Z-rotated ~40 deg to align tube with shaft, centered on tip
 * Model is rotated 180 degrees Z in draw function so arrow tip points UP
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
// Arrow Shaft Vertices (51 vertices)
// From: gGiMagicArrowVtx.inc.c (unmodified)
// ============================================================================
static Vtx sArrowShaftVtx[] = {
    VTX(    -1,     -1,      4,    0xF9,   0x425, 0xCB, 0x5B, 0x39, 0xFF), // 0
    VTX(    -3,      1,     -2,   -0x94,   0x486, 0xCB, 0x5B, 0x39, 0xFF), // 1
    VTX(   -12,     -2,     -4,   -0x74,   0x5A1, 0xCB, 0x5B, 0x39, 0xFF), // 2
    VTX(    -3,      1,     -2,   0x36C,   0x486, 0x17, 0x1C, 0x8E, 0xFF), // 3
    VTX(     2,     -2,     -2,   0x297,   0x484, 0x17, 0x1C, 0x8E, 0xFF), // 4
    VTX(     0,    -12,     -4,   0x274,   0x59E, 0x17, 0x1C, 0x8E, 0xFF), // 5
    VTX(     2,     -2,     -2,   0x297,   0x484, 0x63, 0xDD, 0x39, 0xFF), // 6
    VTX(    -1,     -1,      4,    0xF9,   0x425, 0x63, 0xDD, 0x39, 0xFF), // 7
    VTX(    -6,     -7,      9,   0x100,   0x571, 0x63, 0xDD, 0x39, 0xFF), // 8
    VTX(    -6,     -7,      9,   0x100,   0x571, 0x9E, 0x25, 0x39, 0xFF), // 9
    VTX(   -12,     -2,     -4,   -0x74,   0x5A1, 0x9E, 0x25, 0x39, 0xFF), // 10
    VTX(   -15,    -18,      0,   0x2F8,   0x7C2, 0x9E, 0x25, 0x39, 0xFF), // 11
    VTX(   -15,    -18,      0,   0x2F8,   0x7C2, 0xEA, 0xE5, 0x8E, 0xFF), // 12
    VTX(   -12,     -2,     -4,   0x38C,   0x5A1, 0xEA, 0xE5, 0x8E, 0xFF), // 13
    VTX(     0,    -12,     -4,   0x274,   0x59E, 0xEA, 0xE5, 0x8E, 0xFF), // 14
    VTX(   -15,    -18,      0,   0x2F8,   0x7C2, 0x36, 0xA6, 0x39, 0xFF), // 15
    VTX(     0,    -12,     -4,   0x274,   0x59E, 0x36, 0xA6, 0x39, 0xFF), // 16
    VTX(    -6,     -7,      9,   0x100,   0x571, 0x36, 0xA6, 0x39, 0xFF), // 17
    VTX(   -12,     -2,     -4,   -0x74,   0x5A1, 0xCB, 0x5B, 0x39, 0xFF), // 18
    VTX(    -6,     -7,      9,   0x100,   0x571, 0xCB, 0x5B, 0x39, 0xFF), // 19
    VTX(    -1,     -1,      4,    0xF9,   0x425, 0xCB, 0x5B, 0x39, 0xFF), // 20
    VTX(   -12,     -2,     -4,   0x38C,   0x5A1, 0x17, 0x1C, 0x8E, 0xFF), // 21
    VTX(     0,    -12,     -4,   0x274,   0x59E, 0x63, 0xDD, 0x39, 0xFF), // 22
    VTX(     5,      6,      3,   0x3FB,  -0x6A1, 0xD7, 0x2C, 0x67, 0xFF), // 23
    VTX(     3,      7,     -1,   0x287,  -0x69B, 0xD7, 0x2C, 0x99, 0xFF), // 24
    VTX(    -3,      1,     -2,   0x28C,   0x400, 0xAA, 0x52, 0x00, 0xFF), // 25
    VTX(     7,      4,     -1,   0x17C,  -0x6A6, 0x61, 0xBA, 0x00, 0xFF), // 26
    VTX(     2,     -2,     -2,   0x174,   0x3F2, 0x33, 0xE0, 0x99, 0xFF), // 27
    VTX(     7,      4,     -1,   0x57C,  -0x6A6, 0x61, 0xBA, 0x00, 0xFF), // 28
    VTX(    -1,     -1,      4,   0x400,   0x3F9, 0x33, 0xE0, 0x67, 0xFF), // 29
    VTX(    -1,     -1,      4,     0x0,   0x3F9, 0x33, 0xE0, 0x67, 0xFF), // 30
    VTX(     3,      7,     -1,   0x287,   0x400, 0xA4, 0x4C, 0x00, 0xFF), // 31
    VTX(     5,      6,      3,   0x40A,   0x400, 0x2E, 0xDA, 0x67, 0xFF), // 32
    VTX(    28,     34,      3,   0x3F6,     0x0, 0xFE, 0x4F, 0x5A, 0xFF), // 33
    VTX(     7,      4,     -1,   0x170,   0x3FF, 0x2E, 0xDA, 0x99, 0xFF), // 34
    VTX(    26,     35,     -1,   0x290,     0x1, 0xFE, 0x4F, 0xA6, 0xFF), // 35
    VTX(     5,      6,      3,     0xA,   0x400, 0x2E, 0xDA, 0x67, 0xFF), // 36
    VTX(    29,     32,     -1,   0x179,     0x0, 0x76, 0xEC, 0x00, 0xFF), // 37
    VTX(    29,     32,     -1,   0x579,     0x0, 0x76, 0xEC, 0x00, 0xFF), // 38
    VTX(    15,     19,      3,     0x0,   0x400, 0x5C, 0xB4, 0x00, 0xFF), // 39
    VTX(    28,     34,      3,   0xE46,   0x400, 0x5C, 0xB4, 0x00, 0xFF), // 40
    VTX(    26,     32,     10,   0x7C6,   0x1FE, 0x5C, 0xB4, 0x00, 0xFF), // 41
    VTX(    13,     20,     -1,     0x0,   0x400, 0xD2, 0x26, 0x67, 0xFF), // 42
    VTX(    26,     35,     -1,   0xE46,   0x400, 0xD2, 0x26, 0x67, 0xFF), // 43
    VTX(    20,     37,     -5,   0x7C6,   0x1FE, 0xD2, 0x26, 0x67, 0xFF), // 44
    VTX(    17,     17,     -1,     0x0,   0x400, 0xD2, 0x26, 0x99, 0xFF), // 45
    VTX(    29,     32,     -1,   0xE46,   0x400, 0xD2, 0x26, 0x99, 0xFF), // 46
    VTX(    32,     26,     -5,   0x7C6,   0x1FE, 0xD2, 0x26, 0x99, 0xFF), // 47
    VTX(    18,     22,      8,    0x50,   0x29B, 0x5C, 0xB4, 0x00, 0xFF), // 48
    VTX(    13,     27,     -4,    0x50,   0x29B, 0xD2, 0x26, 0x67, 0xFF), // 49
    VTX(    23,     18,     -4,    0x50,   0x29B, 0xD2, 0x26, 0x99, 0xFF), // 50
};

// ============================================================================
// Bomb Bag Vertices (152 vertices)
// From: gGiBombBagVtx.inc.c (scale=0.75, Z-rotated ~40 deg, centered on arrow tip)
// Bag body covers arrowhead; tube/ring sits on shaft
// ============================================================================
static Vtx sBombBagVtx[] = {
    VTX(   -7,    -9,     0,  0x20,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 0
    VTX(  -11,     3,     4,  0x40,  0x5D0, 0x92, 0xD8, 0x18, 0xFF), // 1
    VTX(  -11,     9,     0,   0x0,  0x586, 0x8B, 0xE7, 0xF6, 0xFF), // 2
    VTX(   -7,    -9,     0,  0x60,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 3
    VTX(   -8,     6,    10,  0x80,  0x586, 0xA1, 0xD4, 0x3B, 0xFF), // 4
    VTX(   -7,    -9,     0,  0xA0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 5
    VTX(   -5,     1,    11,  0xC0,  0x5AA, 0xAE, 0xC5, 0x40, 0xFF), // 6
    VTX(   -7,    -9,     0,  0xE0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 7
    VTX(    0,     0,    14,  0x100,  0x586, 0xBC, 0xBE, 0x4A, 0xFF), // 8
    VTX(   -7,    -9,     0,  0x120,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 9
    VTX(    0,    -6,    10,  0x140,  0x5D0, 0xD2, 0xA3, 0x3B, 0xFF), // 10
    VTX(   -7,    -9,     0,  0x160,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 11
    VTX(    7,    -6,    10,  0x180,  0x586, 0xF2, 0x92, 0x2D, 0xFF), // 12
    VTX(   -7,    -9,     0,  0x1A0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 13
    VTX(    7,    -9,     4,  0x1C0,  0x5AA, 0xF3, 0x8C, 0x1A, 0xFF), // 14
    VTX(   -7,    -9,     0,  0x1E0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 15
    VTX(   11,    -9,     0,  0x200,  0x586, 0xFD, 0x88, 0x0A, 0xFF), // 16
    VTX(   -7,    -9,     0,  0x220,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 17
    VTX(    5,   -10,    -4,  0x240,  0x5D0, 0xED, 0x8D, 0xE8, 0xFF), // 18
    VTX(   -7,    -9,     0,  0x260,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 19
    VTX(    7,    -6,   -10,  0x280,  0x586, 0xE7, 0x9B, 0xC5, 0xFF), // 20
    VTX(   -7,    -9,     0,  0x2A0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 21
    VTX(    2,    -5,   -11,  0x2C0,  0x5AA, 0xD6, 0xA4, 0xC0, 0xFF), // 22
    VTX(   -7,    -9,     0,  0x2E0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 23
    VTX(    0,     0,   -14,  0x300,  0x586, 0xCC, 0xB1, 0xB6, 0xFF), // 24
    VTX(   -7,    -9,     0,  0x320,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 25
    VTX(   -7,    -1,   -10,  0x340,  0x5D0, 0xAD, 0xC1, 0xC5, 0xFF), // 26
    VTX(   -7,    -9,     0,  0x360,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 27
    VTX(   -8,     6,   -10,  0x380,  0x586, 0x97, 0xDD, 0xD3, 0xFF), // 28
    VTX(   -7,    -9,     0,  0x3A0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 29
    VTX(  -10,     5,    -4,  0x3C0,  0x5AA, 0x90, 0xDD, 0xE6, 0xFF), // 30
    VTX(   -7,    -9,     0, -0x20,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 31
    VTX(  -11,     9,     0,   0x0,  0x586, 0x8B, 0xE7, 0xF6, 0xFF), // 32
    VTX(  -10,     5,    -4, -0x40,  0x5AA, 0x90, 0xDD, 0xE6, 0xFF), // 33
    VTX(  -28,    -7,     7,  0x40,  0x5B5, 0xCC, 0x63, 0x2B, 0xFF), // 34
    VTX(  -20,   -14,    16,  0xC0,  0x5B5, 0xFA, 0x3D, 0x67, 0xFF), // 35
    VTX(  -15,    -7,     7,  0x80,  0x4E0, 0x0E, 0x69, 0x38, 0xFF), // 36
    VTX(  -10,   -22,    16,  0x140,  0x5B5, 0x3D, 0x06, 0x67, 0xFF), // 37
    VTX(  -10,   -12,    10,  0x100,  0x4E0, 0x39, 0x45, 0x4E, 0xFF), // 38
    VTX(   -2,   -29,     7,  0x1C0,  0x5B5, 0x6B, 0xDF, 0x2B, 0xFF), // 39
    VTX(   -5,   -16,     7,  0x180,  0x4E0, 0x65, 0x22, 0x38, 0xFF), // 40
    VTX(   -2,   -29,    -7,  0x240,  0x5B5, 0x6B, 0xDF, 0xD5, 0xFF), // 41
    VTX(   -2,   -18,     0,  0x200,  0x4E0, 0x75, 0x14, 0x00, 0xFF), // 42
    VTX(  -10,   -22,   -16,  0x2C0,  0x5B5, 0x3D, 0x06, 0x99, 0xFF), // 43
    VTX(   -5,   -16,    -7,  0x280,  0x4E0, 0x65, 0x22, 0xC8, 0xFF), // 44
    VTX(  -20,   -14,   -16,  0x340,  0x5B5, 0xFA, 0x3D, 0x99, 0xFF), // 45
    VTX(  -10,   -12,   -10,  0x300,  0x4E0, 0x39, 0x45, 0xB2, 0xFF), // 46
    VTX(  -28,    -7,    -7,  0x3C0,  0x5B5, 0xCC, 0x63, 0xD5, 0xFF), // 47
    VTX(  -15,    -7,    -7,  0x380,  0x4E0, 0x0E, 0x69, 0xC8, 0xFF), // 48
    VTX(  -28,    -7,    -7, -0x40,  0x5B5, 0xCC, 0x63, 0xD5, 0xFF), // 49
    VTX(  -17,    -5,     0,   0x0,  0x4E0, 0xFC, 0x78, 0x00, 0xFF), // 50
    VTX(  -22,   -26,    17,  0x100,  0x679, 0xEA, 0xE5, 0x72, 0xFF), // 51
    VTX(  -31,   -18,    12,  0x80,  0x679, 0xAB, 0x19, 0x51, 0xFF), // 52
    VTX(  -31,   -30,    11,  0xC0,  0x72C, 0xB0, 0xCE, 0x48, 0xFF), // 53
    VTX(  -12,   -34,    12,  0x180,  0x679, 0x28, 0xB1, 0x51, 0xFF), // 54
    VTX(  -24,   -36,    11,  0x140,  0x72C, 0xDE, 0xA8, 0x48, 0xFF), // 55
    VTX(   -8,   -37,     0,  0x200,  0x679, 0x42, 0x9C, 0x00, 0xFF), // 56
    VTX(  -19,   -40,     4,  0x1C0,  0x72C, 0xFE, 0x8D, 0x1E, 0xFF), // 57
    VTX(  -12,   -34,   -12,  0x280,  0x679, 0x28, 0xB1, 0xAF, 0xFF), // 58
    VTX(  -19,   -40,    -4,  0x240,  0x72C, 0xFE, 0x8D, 0xE2, 0xFF), // 59
    VTX(  -22,   -26,   -17,  0x300,  0x679, 0xEA, 0xE5, 0x8E, 0xFF), // 60
    VTX(  -24,   -36,   -11,  0x2C0,  0x72C, 0xDE, 0xA8, 0xB8, 0xFF), // 61
    VTX(  -31,   -18,   -12,  0x380,  0x679, 0xAB, 0x19, 0xAF, 0xFF), // 62
    VTX(  -31,   -30,   -11,  0x340,  0x72C, 0xB0, 0xCE, 0xB8, 0xFF), // 63
    VTX(  -35,   -15,     0,   0x0,  0x679, 0x92, 0x2E, 0x00, 0xFF), // 64
    VTX(  -31,   -18,   -12, -0x80,  0x679, 0xAB, 0x19, 0xAF, 0xFF), // 65
    VTX(  -36,   -26,    -4, -0x40,  0x72C, 0x8F, 0xE9, 0xE2, 0xFF), // 66
    VTX(  -31,   -18,    12,  0x80,  0x679, 0xAB, 0x19, 0x51, 0xFF), // 67
    VTX(  -36,   -26,     4,  0x40,  0x72C, 0x8F, 0xE9, 0x1E, 0xFF), // 68
    VTX(  -20,   -14,    16,  0xC0,  0x5B5, 0xFA, 0x3D, 0x67, 0xFF), // 69
    VTX(  -28,    -7,     7,  0x40,  0x5B5, 0xCC, 0x63, 0x2B, 0xFF), // 70
    VTX(  -10,   -22,    16,  0x140,  0x5B5, 0x3D, 0x06, 0x67, 0xFF), // 71
    VTX(  -22,   -26,    17,  0x100,  0x679, 0xEA, 0xE5, 0x72, 0xFF), // 72
    VTX(   -2,   -29,     7,  0x1C0,  0x5B5, 0x6B, 0xDF, 0x2B, 0xFF), // 73
    VTX(  -12,   -34,    12,  0x180,  0x679, 0x28, 0xB1, 0x51, 0xFF), // 74
    VTX(   -2,   -29,    -7,  0x240,  0x5B5, 0x6B, 0xDF, 0xD5, 0xFF), // 75
    VTX(   -8,   -37,     0,  0x200,  0x679, 0x42, 0x9C, 0x00, 0xFF), // 76
    VTX(  -10,   -22,   -16,  0x2C0,  0x5B5, 0x3D, 0x06, 0x99, 0xFF), // 77
    VTX(  -12,   -34,   -12,  0x280,  0x679, 0x28, 0xB1, 0xAF, 0xFF), // 78
    VTX(  -20,   -14,   -16,  0x340,  0x5B5, 0xFA, 0x3D, 0x99, 0xFF), // 79
    VTX(  -22,   -26,   -17,  0x300,  0x679, 0xEA, 0xE5, 0x8E, 0xFF), // 80
    VTX(  -28,    -7,    -7,  0x3C0,  0x5B5, 0xCC, 0x63, 0xD5, 0xFF), // 81
    VTX(  -31,   -18,   -12,  0x380,  0x679, 0xAB, 0x19, 0xAF, 0xFF), // 82
    VTX(  -28,    -7,    -7, -0x40,  0x5B5, 0xCC, 0x63, 0xD5, 0xFF), // 83
    VTX(  -15,    -7,     7,  0x80,  0x4E0, 0x0E, 0x69, 0x38, 0xFF), // 84
    VTX(  -17,    -5,     0,   0x0,  0x4E0, 0xFC, 0x78, 0x00, 0xFF), // 85
    VTX(  -10,   -12,    10,  0x100,  0x4E0, 0x39, 0x45, 0x4E, 0xFF), // 86
    VTX(   -5,   -16,     7,  0x180,  0x4E0, 0x65, 0x22, 0x38, 0xFF), // 87
    VTX(   -2,   -18,     0,  0x200,  0x4E0, 0x75, 0x14, 0x00, 0xFF), // 88
    VTX(   -5,   -16,    -7,  0x280,  0x4E0, 0x65, 0x22, 0xC8, 0xFF), // 89
    VTX(  -10,   -12,   -10,  0x300,  0x4E0, 0x39, 0x45, 0xB2, 0xFF), // 90
    VTX(  -15,    -7,    -7,  0x380,  0x4E0, 0x0E, 0x69, 0xC8, 0xFF), // 91
    VTX(  -15,    -7,    -7, -0x80,  0x4E0, 0x0E, 0x69, 0xC8, 0xFF), // 92
    VTX(   -5,    -6,     0,  0x40,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 93
    VTX(   -5,    -6,     0,  0xC0,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 94
    VTX(   -5,    -6,     0,  0x140,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 95
    VTX(   -5,    -6,     0,  0x1C0,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 96
    VTX(   -5,   -16,     7,  0x180,  0x4E0, 0x65, 0x22, 0x38, 0xFF), // 97
    VTX(   -2,   -18,     0,  0x200,  0x4E0, 0x75, 0x14, 0x00, 0xFF), // 98
    VTX(   -5,    -6,     0,  0x240,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 99
    VTX(   -5,   -16,    -7,  0x280,  0x4E0, 0x65, 0x22, 0xC8, 0xFF), // 100
    VTX(   -5,    -6,     0,  0x2C0,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 101
    VTX(  -10,   -12,   -10,  0x300,  0x4E0, 0x39, 0x45, 0xB2, 0xFF), // 102
    VTX(   -5,    -6,     0,  0x340,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 103
    VTX(  -15,    -7,    -7,  0x380,  0x4E0, 0x0E, 0x69, 0xC8, 0xFF), // 104
    VTX(   -5,    -6,     0, -0x40,  0x400, 0x4C, 0x5C, 0x00, 0xFF), // 105
    VTX(  -15,    -7,    -7, -0x80,  0x4E0, 0x0E, 0x69, 0xC8, 0xFF), // 106
    VTX(  -17,    -5,     0,   0x0,  0x4E0, 0xFC, 0x78, 0x00, 0xFF), // 107
    VTX(  -30,   -36,     0,  0x100,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 108
    VTX(  -24,   -36,    11,  0x140,  0x72C, 0xDE, 0xA8, 0x48, 0xFF), // 109
    VTX(  -31,   -30,    11,  0xC0,  0x72C, 0xB0, 0xCE, 0x48, 0xFF), // 110
    VTX(  -30,   -36,     0,  0x180,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 111
    VTX(  -19,   -40,     4,  0x1C0,  0x72C, 0xFE, 0x8D, 0x1E, 0xFF), // 112
    VTX(  -30,   -36,     0,  0x200,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 113
    VTX(  -19,   -40,    -4,  0x240,  0x72C, 0xFE, 0x8D, 0xE2, 0xFF), // 114
    VTX(  -30,   -36,     0,  0x280,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 115
    VTX(  -24,   -36,   -11,  0x2C0,  0x72C, 0xDE, 0xA8, 0xB8, 0xFF), // 116
    VTX(  -30,   -36,     0,  0x300,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 117
    VTX(  -31,   -30,   -11,  0x340,  0x72C, 0xB0, 0xCE, 0xB8, 0xFF), // 118
    VTX(  -30,   -36,     0,  0x380,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 119
    VTX(  -36,   -26,    -4,  0x3C0,  0x72C, 0x8F, 0xE9, 0xE2, 0xFF), // 120
    VTX(  -30,   -36,     0,   0x0,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 121
    VTX(  -36,   -26,     4,  0x40,  0x72C, 0x8F, 0xE9, 0x1E, 0xFF), // 122
    VTX(  -36,   -26,    -4, -0x40,  0x72C, 0x8F, 0xE9, 0xE2, 0xFF), // 123
    VTX(  -30,   -36,     0,  0x80,  0x400, 0xB4, 0xA4, 0x00, 0xFF), // 124
    VTX(  -22,   -26,    17,  0x100,  0x679, 0xEA, 0xE5, 0x72, 0xFF), // 125
    VTX(  -12,   -34,    12,  0x180,  0x679, 0x28, 0xB1, 0x51, 0xFF), // 126
    VTX(   -8,   -37,     0,  0x200,  0x679, 0x42, 0x9C, 0x00, 0xFF), // 127
    VTX(  -19,   -40,    -4,  0x240,  0x72C, 0xFE, 0x8D, 0xE2, 0xFF), // 128
    VTX(  -24,   -36,   -11,  0x2C0,  0x72C, 0xDE, 0xA8, 0xB8, 0xFF), // 129
    VTX(  -12,   -34,   -12,  0x280,  0x679, 0x28, 0xB1, 0xAF, 0xFF), // 130
    VTX(  -31,   -30,   -11,  0x340,  0x72C, 0xB0, 0xCE, 0xB8, 0xFF), // 131
    VTX(  -22,   -26,   -17,  0x300,  0x679, 0xEA, 0xE5, 0x8E, 0xFF), // 132
    VTX(  -36,   -26,    -4,  0x3C0,  0x72C, 0x8F, 0xE9, 0xE2, 0xFF), // 133
    VTX(  -31,   -18,   -12,  0x380,  0x679, 0xAB, 0x19, 0xAF, 0xFF), // 134
    VTX(  -36,   -26,    -4, -0x40,  0x72C, 0x8F, 0xE9, 0xE2, 0xFF), // 135
    VTX(  -36,   -26,     4,  0x40,  0x72C, 0x8F, 0xE9, 0x1E, 0xFF), // 136
    VTX(  -35,   -15,     0,   0x0,  0x679, 0x92, 0x2E, 0x00, 0xFF), // 137
    VTX(  -31,   -30,    11,  0xC0,  0x72C, 0xB0, 0xCE, 0x48, 0xFF), // 138
    VTX(  -31,   -18,    12,  0x80,  0x679, 0xAB, 0x19, 0x51, 0xFF), // 139
    VTX(  -22,   -26,    17,  0x100,  0x679, 0xEA, 0xE5, 0x72, 0xFF), // 140
    VTX(  -20,   -14,    16,  0xC0,  0x5B5, 0xFA, 0x3D, 0x67, 0xFF), // 141
    VTX(  -12,   -34,    12,  0x180,  0x679, 0x28, 0xB1, 0x51, 0xFF), // 142
    VTX(  -10,   -22,    16,  0x140,  0x5B5, 0x3D, 0x06, 0x67, 0xFF), // 143
    VTX(   -8,   -37,     0,  0x200,  0x679, 0x42, 0x9C, 0x00, 0xFF), // 144
    VTX(   -2,   -29,     7,  0x1C0,  0x5B5, 0x6B, 0xDF, 0x2B, 0xFF), // 145
    VTX(   -2,   -29,    -7,  0x240,  0x5B5, 0x6B, 0xDF, 0xD5, 0xFF), // 146
    VTX(  -10,   -22,   -16,  0x2C0,  0x5B5, 0x3D, 0x06, 0x99, 0xFF), // 147
    VTX(  -20,   -14,   -16,  0x340,  0x5B5, 0xFA, 0x3D, 0x99, 0xFF), // 148
    VTX(  -31,   -18,   -12, -0x80,  0x679, 0xAB, 0x19, 0xAF, 0xFF), // 149
    VTX(  -28,    -7,    -7, -0x40,  0x5B5, 0xCC, 0x63, 0xD5, 0xFF), // 150
    VTX(  -28,    -7,     7,  0x40,  0x5B5, 0xCC, 0x63, 0x2B, 0xFF), // 151
};

// ============================================================================
// Bomb Bag Ring Vertices (21 vertices)
// From: gGiBombBagRingVtx.inc.c (scale=0.75, Z-rotated ~40 deg, centered on tip)
// ============================================================================
static Vtx sBombBagRingVtx[] = {
    VTX(   -7,    -5,    -5,  0x340,  0x59A, 0xE9, 0x13, 0x8C, 0xFF), // 0
    VTX(   -4,    -8,    -5,  0x2C0,  0x59A, 0x2E, 0xDA, 0x99, 0xFF), // 1
    VTX(   -6,   -10,    -5,  0x2C0,  0x666, 0x17, 0xED, 0x8C, 0xFF), // 2
    VTX(   -4,    -8,    -5,  0x2C0,  0x59A, 0x2E, 0xDA, 0x99, 0xFF), // 3
    VTX(   -1,   -10,    -2,  0x240,  0x59A, 0x59, 0xB6, 0xE2, 0xFF), // 4
    VTX(   -3,   -12,    -2,  0x240,  0x666, 0x4F, 0xBE, 0xC4, 0xFF), // 5
    VTX(   -1,   -10,     2,  0x1C0,  0x59A, 0x4F, 0xBE, 0x3C, 0xFF), // 6
    VTX(   -3,   -12,     2,  0x1C0,  0x666, 0x59, 0xB6, 0x1E, 0xFF), // 7
    VTX(   -4,    -8,     5,  0x140,  0x59A, 0x17, 0xED, 0x74, 0xFF), // 8
    VTX(   -6,   -10,     5,  0x140,  0x666, 0x2E, 0xDA, 0x67, 0xFF), // 9
    VTX(   -7,    -5,     5,  0xC0,  0x59A, 0xD2, 0x26, 0x67, 0xFF), // 10
    VTX(   -9,    -7,     5,  0xC0,  0x666, 0xE9, 0x13, 0x74, 0xFF), // 11
    VTX(   -9,    -3,     2,  0x40,  0x59A, 0xA7, 0x4A, 0x1E, 0xFF), // 12
    VTX(  -11,    -5,     2,  0x40,  0x666, 0xB1, 0x42, 0x3C, 0xFF), // 13
    VTX(   -9,    -3,    -2, -0x40,  0x59A, 0xB1, 0x42, 0xC4, 0xFF), // 14
    VTX(  -11,    -5,    -2, -0x40,  0x666, 0xA7, 0x4A, 0xE2, 0xFF), // 15
    VTX(   -9,    -3,    -2,  0x3C0,  0x59A, 0xB1, 0x42, 0xC4, 0xFF), // 16
    VTX(   -7,    -5,    -5,  0x340,  0x59A, 0xE9, 0x13, 0x8C, 0xFF), // 17
    VTX(   -9,    -7,    -5,  0x340,  0x666, 0xD2, 0x26, 0x99, 0xFF), // 18
    VTX(   -6,   -10,    -5,  0x2C0,  0x666, 0x17, 0xED, 0x8C, 0xFF), // 19
    VTX(  -11,    -5,    -2,  0x3C0,  0x666, 0xA7, 0x4A, 0xE2, 0xFF), // 20
};

// ============================================================================
// Arrow Shaft Display List (colors darkened to match bomb bag brown)
// From: gGiMagicArrowDL.inc.c
// ============================================================================
static Gfx sArrowShaftDL[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, 1, PRIMITIVE, ENVIRONMENT, COMBINED,
        ENVIRONMENT, 0, 0, 0, 1),
    // Arrowhead body - dark metallic brown
    gsDPSetPrimColor(0, 0, 160, 140, 120, 255),
    gsDPSetEnvColor(50, 35, 20, 255),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0x0FA0, 0x0FA0, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown10Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1),
    gsSPClearGeometryMode(G_FOG),
    gsSPSetGeometryMode(G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(&sArrowShaftVtx[0], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPVertex(&sArrowShaftVtx[3], 20, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 15, 16, 17, 0),
    gsSP2Triangles(2, 18, 0, 0, 5, 19, 3, 0),
    gsDPPipeSync(),
    // Shaft joint - dark brown
    gsDPSetPrimColor(0, 0, 140, 120, 100, 255),
    gsDPSetEnvColor(45, 35, 20, 255),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown10Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(&sArrowShaftVtx[23], 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(5, 0, 6, 0, 2, 6, 0, 0),
    gsSP2Triangles(4, 2, 1, 0, 7, 4, 3, 0),
    gsDPPipeSync(),
    // Shaft wood - darker brown
    gsDPSetPrimColor(0, 0, 160, 120, 50, 255),
    gsDPSetEnvColor(100, 70, 25, 255),
    gsSPVertex(&sArrowShaftVtx[31], 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 0, 4, 0),
    gsSP2Triangles(5, 3, 6, 0, 4, 2, 7, 0),
    gsSP2Triangles(2, 4, 0, 0, 4, 6, 3, 0),
    gsSP1Triangle(7, 2, 1, 0),
    gsDPPipeSync(),
    // Feathers - tan/cream
    gsDPSetPrimColor(0, 0, 210, 190, 160, 255),
    gsDPSetEnvColor(110, 85, 50, 255),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPVertex(&sArrowShaftVtx[39], 12, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 2, 9, 0, 0),
    gsSP2Triangles(5, 10, 3, 0, 8, 11, 6, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// Bomb Bag Color DL (brown leather - from 20-bag variant)
// ============================================================================
static Gfx sBombBagColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 120, 90, 60, 255),
    gsDPSetEnvColor(60, 40, 0, 255),
    gsSPEndDisplayList(),
};

// ============================================================================
// Bomb Bag Display List (body - adapted from gGiBombBagDL)
// Using G_CULL_BACK (rotation preserves winding order)
// ============================================================================
static Gfx sBombBagDL[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 1, 1, PRIM_LOD_FRAC, 1, COMBINED, 0, SHADE, 0, 1,
        1, PRIM_LOD_FRAC, 1),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown12Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPClearGeometryMode(G_CULL_BACK | G_CULL_FRONT | G_FOG | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPVertex(&sBombBagVtx[0], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 1, 0),
    gsSP2Triangles(5, 6, 4, 0, 7, 8, 6, 0),
    gsSP2Triangles(9, 10, 8, 0, 11, 12, 10, 0),
    gsSP2Triangles(13, 14, 12, 0, 15, 16, 14, 0),
    gsSP2Triangles(17, 18, 16, 0, 19, 20, 18, 0),
    gsSP2Triangles(21, 22, 20, 0, 23, 24, 22, 0),
    gsSP2Triangles(25, 26, 24, 0, 27, 28, 26, 0),
    gsSP1Triangle(29, 30, 28, 0),
    gsSPVertex(&sBombBagVtx[31], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPVertex(&sBombBagVtx[34], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(3, 5, 6, 0, 5, 7, 8, 0),
    gsSP2Triangles(7, 9, 10, 0, 9, 11, 12, 0),
    gsSP2Triangles(11, 13, 14, 0, 15, 0, 16, 0),
    gsSP2Triangles(17, 18, 19, 0, 20, 17, 21, 0),
    gsSP2Triangles(22, 20, 23, 0, 24, 22, 25, 0),
    gsSP2Triangles(26, 24, 27, 0, 28, 26, 29, 0),
    gsSPVertex(&sBombBagVtx[64], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 0, 4, 0),
    gsSP2Triangles(5, 6, 3, 0, 7, 5, 8, 0),
    gsSP2Triangles(9, 7, 10, 0, 11, 9, 12, 0),
    gsSP2Triangles(13, 11, 14, 0, 15, 13, 16, 0),
    gsSP2Triangles(17, 15, 18, 0, 6, 19, 0, 0),
    gsSP2Triangles(20, 21, 6, 0, 22, 20, 5, 0),
    gsSP2Triangles(23, 22, 7, 0, 24, 23, 9, 0),
    gsSP2Triangles(25, 24, 11, 0, 26, 25, 13, 0),
    gsSP2Triangles(27, 26, 15, 0, 21, 28, 19, 0),
    gsSP2Triangles(29, 21, 20, 0, 30, 20, 22, 0),
    gsSP1Triangle(31, 22, 23, 0),
    gsSPVertex(&sBombBagVtx[96], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 2, 4, 0),
    gsSP2Triangles(5, 4, 6, 0, 7, 6, 8, 0),
    gsSP2Triangles(9, 10, 11, 0, 12, 13, 14, 0),
    gsSP2Triangles(15, 16, 13, 0, 17, 18, 16, 0),
    gsSP2Triangles(19, 20, 18, 0, 21, 22, 20, 0),
    gsSP2Triangles(23, 24, 22, 0, 25, 26, 27, 0),
    gsSP2Triangles(28, 14, 26, 0, 14, 13, 29, 0),
    gsSP2Triangles(13, 16, 30, 0, 16, 18, 31, 0),
    gsSPVertex(&sBombBagVtx[128], 24, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(3, 5, 6, 0, 7, 8, 9, 0),
    gsSP2Triangles(8, 10, 11, 0, 11, 12, 13, 0),
    gsSP2Triangles(12, 14, 15, 0, 14, 16, 17, 0),
    gsSP2Triangles(16, 2, 18, 0, 2, 4, 19, 0),
    gsSP2Triangles(4, 6, 20, 0, 21, 9, 22, 0),
    gsSP1Triangle(9, 11, 23, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// Bomb Bag Ring Color DL (orange/red ring - from 20-ring variant)
// ============================================================================
static Gfx sBombBagRingColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 255, 120, 50, 255),
    gsDPSetEnvColor(50, 0, 0, 255),
    gsSPEndDisplayList(),
};

// ============================================================================
// Bomb Bag Ring Display List (adapted from gGiBombBagRingDL)
// Using G_CULL_BACK (rotation preserves winding order)
// ============================================================================
static Gfx sBombBagRingDL[] = {
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0x07D0, 0x07D0, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown12Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1),
    gsSPClearGeometryMode(G_FOG),
    gsSPSetGeometryMode(G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(&sBombBagRingVtx[0], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPVertex(&sBombBagRingVtx[3], 18, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(3, 5, 6, 0, 5, 7, 8, 0),
    gsSP2Triangles(7, 9, 10, 0, 9, 11, 12, 0),
    gsSP2Triangles(13, 14, 15, 0, 16, 15, 14, 0),
    gsSP2Triangles(2, 16, 0, 0, 4, 2, 1, 0),
    gsSP2Triangles(6, 4, 3, 0, 8, 6, 5, 0),
    gsSP2Triangles(10, 8, 7, 0, 12, 10, 9, 0),
    gsSP1Triangle(15, 17, 13, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// Combined Give DL: Arrow shaft + Bomb bag centered on arrowhead
// Draw function applies 180 deg Z rotation so arrow tip + bomb bag point UP
// ============================================================================
Gfx gBombarrowsGiveDL[] = {
    gsSPDisplayList(sArrowShaftDL),
    gsSPDisplayList(sBombBagColorDL),
    gsSPDisplayList(sBombBagDL),
    gsSPDisplayList(sBombBagRingColorDL),
    gsSPDisplayList(sBombBagRingDL),
    gsSPEndDisplayList(),
};

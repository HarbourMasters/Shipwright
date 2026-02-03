/**
 * Mogma Mitts Model Data - White Gauntlets
 * Based on OoT Silver Gauntlets (object_gi_gloves) with white color scheme
 */

// Texture path strings (avoid including full gameplay_keep.h / object_gi_gloves.h)
#include "align_asset_macro.h"

#ifndef dgEffUnknown12Tex
#define dgEffUnknown12Tex "__OTR__objects/gameplay_keep/gEffUnknown12Tex"
static const ALIGN_ASSET(2) char gEffUnknown12Tex[] = dgEffUnknown12Tex;
#endif

#ifndef dobject_gi_glovesTex_000000
#define dobject_gi_glovesTex_000000 "__OTR__objects/object_gi_gloves/object_gi_glovesTex_000000"
static const ALIGN_ASSET(2) char object_gi_glovesTex_000000[] = dobject_gi_glovesTex_000000;
#endif

#include "header.h"

// ============================================================================
// Main Gauntlet Vertices (152 vertices)
// ============================================================================
Vtx gMogmaMittsVtx[] = {
    VTX(    16,     18,      5,   0x219,   0x567, 0x38, 0x05, 0x69, 0xFF), // 0
    VTX(     8,      9,      9,   0x1CD,   0x646, 0x27, 0x01, 0x71, 0xFF), // 1
    VTX(    12,     -2,      6,   0x209,   0x729, 0x20, 0xE9, 0x71, 0xFF), // 2
    VTX(     1,      3,      9,    0xBD,   0x7A1, 0xFE, 0xDE, 0x72, 0xFF), // 3
    VTX(    12,     -2,      6,   0x209,   0x729, 0x20, 0xE9, 0x71, 0xFF), // 4
    VTX(     8,      9,      9,   0x1CD,   0x646, 0x27, 0x01, 0x71, 0xFF), // 5
    VTX(    -7,      9,      9,    0x28,   0x638, 0xD7, 0xF8, 0x70, 0xFF), // 6
    VTX(    -8,     -1,      6,    -0xA,   0x728, 0xDA, 0xE1, 0x6D, 0xFF), // 7
    VTX(     2,     -2,      7,   0x15F,   0x7F2, 0xFE, 0xD5, 0x6F, 0xFF), // 8
    VTX(   -12,     19,      6,    -0x7,   0x55E, 0xC1, 0xF9, 0x66, 0xFF), // 9
    VTX(    16,     18,      5,   0x219,   0x567, 0x38, 0x05, 0x69, 0xFF), // 10
    VTX(     9,     21,      7,   0x1F4,   0x4C5, 0x22, 0x0F, 0x71, 0xFF), // 11
    VTX(     1,     20,      9,    0xBA,   0x44F, 0x05, 0x0D, 0x77, 0xFF), // 12
    VTX(    -9,     24,      8,     0xE,   0x4DE, 0xE2, 0x06, 0x73, 0xFF), // 13
    VTX(     2,    -14,     11,    0xEE,   0x7A5, 0x01, 0x16, 0x75, 0xFF), // 14
    VTX(     2,     -2,      7,   0x100,   0x573, 0x01, 0x21, 0x73, 0xFF), // 15
    VTX(    -8,     -1,      6,   -0x27,   0x568, 0xCF, 0x15, 0x6B, 0xFF), // 16
    VTX(    12,     -2,      6,   0x227,   0x57E, 0x37, 0x15, 0x68, 0xFF), // 17
    VTX(    -9,    -10,      6,   -0x1F,   0x671, 0xBD, 0x0E, 0x62, 0xFF), // 18
    VTX(    13,    -10,      6,   0x21F,   0x678, 0x49, 0x0D, 0x5D, 0xFF), // 19
    VTX(     2,    -25,     13,    0xF7,   0x7B8, 0x00, 0x09, 0x77, 0xFF), // 20
    VTX(   -11,    -21,      7,   -0x19,   0x715, 0xC3, 0x0D, 0x66, 0xFF), // 21
    VTX(     2,    -36,     14,    0xF9,   0x7C9, 0xF6, 0x0C, 0x76, 0xFF), // 22
    VTX(   -12,    -31,      7,   -0x11,   0x754, 0xC0, 0x0F, 0x64, 0xFF), // 23
    VTX(    16,    -31,      7,   0x211,   0x757, 0x46, 0x10, 0x60, 0xFF), // 24
    VTX(    14,    -21,      7,   0x219,   0x71A, 0x40, 0x0D, 0x64, 0xFF), // 25
    VTX(     2,    -14,     11,    0xEE,   0x7A5, 0x00, 0x15, 0x76, 0xFF), // 26
    VTX(    -9,    -10,      6,   -0x1F,   0x671, 0xC6, 0x0E, 0x68, 0xFF), // 27
    VTX(     2,    -25,     13,    0xF7,   0x7B8, 0x00, 0x10, 0x76, 0xFF), // 28
    VTX(   -11,    -21,      7,   -0x19,   0x715, 0xBD, 0x0B, 0x62, 0xFF), // 29
    VTX(    14,    -21,      7,   0x219,   0x71A, 0x48, 0x0C, 0x5F, 0xFF), // 30
    VTX(    13,    -10,      6,   0x21F,   0x678, 0x3D, 0x0F, 0x66, 0xFF), // 31
    VTX(    -8,     -1,      6,   0x3D9,   0x568, 0xCF, 0x15, 0x6B, 0xFF), // 32
    VTX(   -11,     -7,      4,   0x3C5,   0x61A, 0xA8, 0x12, 0x4F, 0xFF), // 33
    VTX(    -9,    -10,      6,   0x3E1,   0x671, 0xBD, 0x0E, 0x62, 0xFF), // 34
    VTX(    -9,    -10,      6,   0x3E1,   0x671, 0xC6, 0x0E, 0x68, 0xFF), // 35
    VTX(   -13,    -18,      4,   0x3D1,   0x6D0, 0xAF, 0x0C, 0x57, 0xFF), // 36
    VTX(   -11,    -21,      7,   0x3E7,   0x715, 0xBD, 0x0B, 0x62, 0xFF), // 37
    VTX(   -11,    -21,      7,   0x3E7,   0x715, 0xC3, 0x0D, 0x66, 0xFF), // 38
    VTX(   -14,    -28,      4,   0x3D6,   0x72E, 0xA2, 0x12, 0x47, 0xFF), // 39
    VTX(   -12,    -31,      7,   0x3EF,   0x754, 0xC0, 0x0F, 0x64, 0xFF), // 40
    VTX(    15,     -8,      3,   0x23F,   0x625, 0x62, 0x10, 0x42, 0xFF), // 41
    VTX(    16,    -18,      4,   0x232,   0x6DC, 0x5A, 0x0E, 0x4E, 0xFF), // 42
    VTX(    18,    -28,      4,   0x22B,   0x72F, 0x5A, 0x12, 0x4C, 0xFF), // 43
    VTX(   -18,     15,    -11,   0x15A,  -0x424, 0x95, 0xE3, 0xD4, 0xFF), // 44
    VTX(   -13,     18,    -10,   0x152,  -0x4B3, 0x3A, 0x37, 0xA7, 0xFF), // 45
    VTX(   -16,     10,    -10,   0x159,  -0x324, 0xA8, 0xCB, 0xC3, 0xFF), // 46
    VTX(    -4,     11,     -4,   0x116,  -0x344, 0x1C, 0x1B, 0x8F, 0xFF), // 47
    VTX(   -11,     19,     -3,   0x179,  -0x470, 0xE2, 0x6B, 0xD5, 0xFF), // 48
    VTX(    -4,     25,     -3,   0x169,  -0x561, 0xF0, 0x24, 0x8F, 0xFF), // 49
    VTX(   -18,     18,     -3,   0x17A,  -0x4A4, 0xD5, 0x56, 0x47, 0xFF), // 50
    VTX(   -16,     14,      1,   0x190,  -0x3AD, 0x92, 0x05, 0x30, 0xFF), // 51
    VTX(   -12,     19,      6,   0x1B1,  -0x46E, 0x92, 0x1F, 0x22, 0xFF), // 52
    VTX(    -9,     24,      8,   0x1C5,  -0x53A, 0x28, 0x43, 0x5B, 0xFF), // 53
    VTX(    -8,     29,      5,   0x1BC,  -0x610, 0xCC, 0x39, 0x5B, 0xFF), // 54
    VTX(   -11,     28,     -4,   0x171,  -0x5CF, 0xB1, 0x07, 0xA6, 0xFF), // 55
    VTX(    -8,     -1,      6,   0x1C4,  -0x16E, 0xA2, 0xEC, 0x47, 0xFF), // 56
    VTX(    -9,     -3,     -7,   0x154,  -0x139, 0xD6, 0x06, 0x90, 0xFF), // 57
    VTX(    -7,    -35,    -14,   0x12A,   0x3B2, 0xBA, 0x11, 0xA0, 0xFF), // 58
    VTX(   -13,    -37,      3,   0x19C,   0x400, 0x8D, 0x07, 0x1F, 0xFF), // 59
    VTX(    16,    -37,      1,    0x70,   0x400, 0x75, 0x05, 0x1A, 0xFF), // 60
    VTX(    10,    -35,    -14,    0xC7,   0x3B2, 0x32, 0x19, 0x96, 0xFF), // 61
    VTX(    12,     -3,     -4,    0x8F,  -0x130, 0x4A, 0x05, 0xA2, 0xFF), // 62
    VTX(    12,     -2,      6,    0x4F,  -0x150, 0x5E, 0x00, 0x49, 0xFF), // 63
    VTX(     2,    -37,     10,     0xF,   0x400, 0x04, 0x02, 0x77, 0xFF), // 64
    VTX(    16,     18,      5,    0x60,  -0x45D, 0x6F, 0x0F, 0x29, 0xFF), // 65
    VTX(    18,     22,     -6,    0x91,  -0x4FB, 0x51, 0xFF, 0xA8, 0xFF), // 66
    VTX(    16,     26,      4,    0x63,  -0x57F, 0xBE, 0x39, 0x52, 0xFF), // 67
    VTX(     5,     24,     -3,    0x8F,  -0x532, 0xFE, 0x26, 0x8F, 0xFF), // 68
    VTX(     9,     21,      7,    0x3F,  -0x4CF, 0x3E, 0x4B, 0x46, 0xFF), // 69
    VTX(     8,     28,      5,    0x46,  -0x5E1, 0xB5, 0x3A, 0x49, 0xFF), // 70
    VTX(     6,     27,     -3,    0x92,  -0x5A5, 0xA6, 0x22, 0xB9, 0xFF), // 71
    VTX(     2,    -37,     10,   0x20F,   0x400, 0x04, 0x02, 0x77, 0xFF), // 72
    VTX(    11,     25,     -4,    0x90,  -0x565, 0x57, 0xF9, 0xAE, 0xFF), // 73
    VTX(    11,     21,     -4,    0x8D,  -0x4D5, 0xDA, 0x24, 0x95, 0xFF), // 74
    VTX(    13,     24,     -4,    0x8E,  -0x54A, 0xA7, 0x30, 0xC0, 0xFF), // 75
    VTX(    12,     -2,      6,   0x24F,  -0x150, 0x5E, 0x00, 0x49, 0xFF), // 76
    VTX(    -8,     -1,      6,   0x1C4,  -0x16E, 0xA2, 0xEC, 0x47, 0xFF), // 77
    VTX(     2,    -37,     10,   0x20F,   0x400, 0x04, 0x02, 0x77, 0xFF), // 78
    VTX(    18,     22,     -6,    0x91,  -0x4FB, 0x51, 0xFF, 0xA8, 0xFF), // 79
    VTX(    17,     15,     -5,    0x8F,  -0x3E9, 0x50, 0xEC, 0xAA, 0xFF), // 80
    VTX(    11,     21,     -4,    0x8D,  -0x4D5, 0xDA, 0x24, 0x95, 0xFF), // 81
    VTX(     9,     21,      7,    0x3F,  -0x4CF, 0x3E, 0x4B, 0x46, 0xFF), // 82
    VTX(    11,     25,     -4,    0x90,  -0x565, 0x57, 0xF9, 0xAE, 0xFF), // 83
    VTX(     8,     28,      5,    0x46,  -0x5E1, 0xB5, 0x3A, 0x49, 0xFF), // 84
    VTX(    16,     18,      5,    0x60,  -0x45D, 0x6F, 0x0F, 0x29, 0xFF), // 85
    VTX(    -9,     24,      8,   0x1C5,  -0x53A, 0x28, 0x43, 0x5B, 0xFF), // 86
    VTX(     1,     23,      8,   0x206,  -0x51A, 0x0F, 0x50, 0x57, 0xFF), // 87
    VTX(    -4,     25,     -3,   0x169,  -0x561, 0xF0, 0x24, 0x8F, 0xFF), // 88
    VTX(     1,     20,      9,   0x208,  -0x4AB, 0x08, 0x1F, 0x73, 0xFF), // 89
    VTX(     1,     23,      8,     0x6,  -0x51A, 0x0F, 0x50, 0x57, 0xFF), // 90
    VTX(     5,     24,     -3,    0x8F,  -0x532, 0xFE, 0x26, 0x8F, 0xFF), // 91
    VTX(     1,     20,      9,     0x8,  -0x4AB, 0x08, 0x1F, 0x73, 0xFF), // 92
    VTX(    16,     26,      4,    0x63,  -0x57F, 0xBE, 0x39, 0x52, 0xFF), // 93
    VTX(    12,     -3,     -4,    0x8F,  -0x130, 0x4A, 0x05, 0xA2, 0xFF), // 94
    VTX(   -11,     19,     -3,   0x179,  -0x470, 0xE2, 0x6B, 0xD5, 0xFF), // 95
    VTX(   -12,     19,      6,   0x1B1,  -0x46E, 0x92, 0x1F, 0x22, 0xFF), // 96
    VTX(   -11,     28,     -4,   0x171,  -0x5CF, 0xB1, 0x07, 0xA6, 0xFF), // 97
    VTX(   -16,     14,      1,   0x190,  -0x3AD, 0x92, 0x05, 0x30, 0xFF), // 98
    VTX(     4,     27,     -3,    0x9A,  -0x5BF, 0x39, 0xFB, 0x97, 0xFF), // 99
    VTX(     1,     30,      6,     0xC,  -0x617, 0x08, 0x40, 0x65, 0xFF), // 100
    VTX(     1,     30,      6,   0x20C,  -0x617, 0x08, 0x40, 0x65, 0xFF), // 101
    VTX(    -2,     28,     -3,   0x151,  -0x5DB, 0xA7, 0x1D, 0xB5, 0xFF), // 102
    VTX(    12,     -2,      6,    0x4F,  -0x150, 0x5E, 0x00, 0x49, 0xFF), // 103
    VTX(     6,     27,     -3,    0x92,  -0x5A5, 0xA6, 0x22, 0xB9, 0xFF), // 104
    VTX(    -4,     28,     -3,   0x166,  -0x5DF, 0x76, 0x11, 0xF5, 0xFF), // 105
    VTX(    -8,     29,      5,   0x1BC,  -0x610, 0xCC, 0x39, 0x5B, 0xFF), // 106
    VTX(    12,     -3,     -4,    0x8F,  -0x130, 0x4A, 0x05, 0xA2, 0xFF), // 107
    VTX(    -9,     -3,     -7,   0x154,  -0x139, 0xD6, 0x06, 0x90, 0xFF), // 108
    VTX(    -4,     11,     -4,   0x116,  -0x344, 0x1C, 0x1B, 0x8F, 0xFF), // 109
    VTX(   -16,     10,    -10,   0x159,  -0x324, 0xA8, 0xCB, 0xC3, 0xFF), // 110
    VTX(    -8,     -1,      6,   0x1C4,  -0x16E, 0xA2, 0xEC, 0x47, 0xFF), // 111
    VTX(   -16,     14,      1,   0x190,  -0x3AD, 0x92, 0x05, 0x30, 0xFF), // 112
    VTX(   -11,     28,     -4,   0x171,  -0x5CF, 0xB1, 0x07, 0xA6, 0xFF), // 113
    VTX(    -4,     25,     -3,   0x169,  -0x561, 0xF0, 0x24, 0x8F, 0xFF), // 114
    VTX(   -11,     19,     -3,   0x179,  -0x470, 0xE2, 0x6B, 0xD5, 0xFF), // 115
    VTX(    -4,     28,     -3,   0x166,  -0x5DF, 0x76, 0x11, 0xF5, 0xFF), // 116
    VTX(   -18,     15,    -11,   0x15A,  -0x424, 0x95, 0xE3, 0xD4, 0xFF), // 117
    VTX(   -18,     18,     -3,   0x17A,  -0x4A4, 0xD5, 0x56, 0x47, 0xFF), // 118
    VTX(     5,     24,     -3,    0x8F,  -0x532, 0xFE, 0x26, 0x8F, 0xFF), // 119
    VTX(    11,     21,     -4,    0x8D,  -0x4D5, 0xDA, 0x24, 0x95, 0xFF), // 120
    VTX(   -13,     18,    -10,   0x152,  -0x4B3, 0x3A, 0x37, 0xA7, 0xFF), // 121
    VTX(    -7,      9,      9,   -0x2E,   0x657, 0xC3, 0xF5, 0x66, 0xFF), // 122
    VTX(     1,      9,     13,   0x100,   0x6F7, 0xE9, 0xDE, 0x70, 0xFF), // 123
    VTX(     1,     20,      9,   0x300,   0x456, 0x00, 0x2F, 0x6E, 0xFF), // 124
    VTX(     1,      9,     13,   0x100,   0x6F7, 0x39, 0x29, 0x61, 0xFF), // 125
    VTX(     8,      9,      9,   0x22E,   0x657, 0x3D, 0xF5, 0x66, 0xFF), // 126
    VTX(     1,      3,      9,   0x300,   0x7AE, 0x00, 0xBB, 0x62, 0xFF), // 127
    VTX(    -7,    -35,    -14,   -0xAF,   0x77F, 0xF4, 0xE3, 0x8D, 0xFF), // 128
    VTX(     2,    -14,      0,   0x187,   0x7A5, 0xFC, 0x8A, 0xF0, 0xFF), // 129
    VTX(   -13,    -37,      3,    0x39,   0x785, 0x1D, 0xB2, 0xAA, 0xFF), // 130
    VTX(    10,    -35,    -14,   0x28B,   0x76C, 0xBC, 0xCA, 0xAE, 0xFF), // 131
    VTX(    -7,    -35,    -14,   0x351,   0x77F, 0x00, 0xBD, 0x63, 0xFF), // 132
    VTX(    16,    -37,      1,   0x1E0,   0x768, 0x69, 0xE7, 0x33, 0xFF), // 133
    VTX(    16,    -37,      1,   0x1E0,   0x768, 0xC3, 0xD5, 0xA3, 0xFF), // 134
    VTX(     2,    -37,     10,   0x122,   0x78B, 0xC3, 0xD5, 0xA3, 0xFF), // 135
    VTX(     2,    -37,     10,   0x122,   0x78B, 0x1F, 0xED, 0x72, 0xFF), // 136
    VTX(   -18,     18,     -3,   0x3E4,   0x4D2, 0x9E, 0x35, 0x2C, 0xFF), // 137
    VTX(   -13,     18,    -10,   0x3A6,   0x4BB, 0x30, 0x6E, 0xFE, 0xFF), // 138
    VTX(   -18,     15,    -11,   0x3AD,   0x4F9, 0x92, 0x2B, 0x0F, 0xFF), // 139
    VTX(    -8,     29,      5,    0x7C,   0x465, 0x08, 0x55, 0x53, 0xFF), // 140
    VTX(    -4,     28,     -3,   -0x10,   0x426, 0x4B, 0x54, 0xD8, 0xFF), // 141
    VTX(   -11,     28,     -4,   -0x1B,   0x475, 0xC4, 0x42, 0xB0, 0xFF), // 142
    VTX(     1,     30,      6,   0x11F,   0x452, 0x0A, 0x55, 0x53, 0xFF), // 143
    VTX(     4,     27,     -3,   0x20E,   0x42E, 0x38, 0x40, 0xAC, 0xFF), // 144
    VTX(    -2,     28,     -3,   -0x23,   0x412, 0xC7, 0x5F, 0xD3, 0xFF), // 145
    VTX(    16,     26,      4,   0x1C7,   0x4AF, 0xAE, 0x49, 0x2F, 0xFF), // 146
    VTX(    18,     22,     -6,   0x220,   0x4C8, 0x54, 0x2F, 0xB8, 0xFF), // 147
    VTX(    13,     24,     -4,   0x20F,   0x48D, 0xCD, 0x62, 0xD3, 0xFF), // 148
    VTX(     8,     28,      5,   0x18F,   0x474, 0xD8, 0x5C, 0x41, 0xFF), // 149
    VTX(    11,     25,     -4,   0x211,   0x47C, 0x55, 0x36, 0xC0, 0xFF), // 150
    VTX(     6,     27,     -3,   0x209,   0x442, 0xCB, 0x5E, 0xCD, 0xFF), // 151
};

// ============================================================================
// Plate Overlay Vertices (52 vertices)
// ============================================================================
Vtx gMogmaMittsPlateVtx[] = {
    VTX(    16,    -31,      7,   0x6A1,   0x400, 0x46, 0x10, 0x60, 0xFF), // 0
    VTX(    18,    -28,      4,   0x832,   0x33B, 0x5A, 0x12, 0x4C, 0xFF), // 1
    VTX(    14,    -21,      7,   0x819,    0x54, 0x43, 0x0D, 0x62, 0xFF), // 2
    VTX(     2,    -25,     13,   0x22D,    0x53, 0x00, 0x0D, 0x77, 0xFF), // 3
    VTX(     2,    -36,     14,     0x0,   0x400, 0xF6, 0x0C, 0x76, 0xFF), // 4
    VTX(     2,    -14,     11,   0x29D,    0x93, 0x00, 0x16, 0x75, 0xFF), // 5
    VTX(     2,    -25,     13,     0x0,   0x400, 0x00, 0x0D, 0x77, 0xFF), // 6
    VTX(    13,    -10,      6,   0x846,    0x8D, 0x42, 0x0E, 0x63, 0xFF), // 7
    VTX(    14,    -21,      7,   0x696,   0x400, 0x43, 0x0D, 0x62, 0xFF), // 8
    VTX(    16,    -18,      4,   0x847,   0x353, 0x5A, 0x0E, 0x4E, 0xFF), // 9
    VTX(    13,    -10,      6,   0x662,   0x400, 0x42, 0x0E, 0x63, 0xFF), // 10
    VTX(    15,     -8,      3,   0x838,   0x336, 0x62, 0x10, 0x42, 0xFF), // 11
    VTX(    12,     -2,      6,   0x80E,    0x9D, 0x2C, 0xFE, 0x6F, 0xFF), // 12
    VTX(     2,    -14,     11,     0x0,   0x400, 0x00, 0x16, 0x75, 0xFF), // 13
    VTX(     2,    -14,     11,     0x0,   0x400, 0x00, 0x16, 0x75, 0xFF), // 14
    VTX(     2,     -2,      7,   0x3E0,   -0xD8, 0x00, 0xFB, 0x77, 0xFF), // 15
    VTX(    -8,     -1,      6,   0x812,    0x55, 0xD3, 0xFB, 0x6F, 0xFF), // 16
    VTX(     2,     -2,      7,   0x405,   -0xCB, 0x00, 0xFB, 0x77, 0xFF), // 17
    VTX(   -12,    -31,      7,   0x695,   0x400, 0xC0, 0x0F, 0x64, 0xFF), // 18
    VTX(     2,    -36,     14,     0x0,   0x400, 0xF6, 0x0C, 0x76, 0xFF), // 19
    VTX(   -11,    -21,      7,   0x81F,    0x5A, 0xC1, 0x0C, 0x65, 0xFF), // 20
    VTX(   -14,    -28,      4,   0x80B,   0x342, 0xA2, 0x12, 0x47, 0xFF), // 21
    VTX(    -9,    -10,      6,   0x85F,    0x42, 0xC3, 0x0E, 0x66, 0xFF), // 22
    VTX(   -13,    -18,      4,   0x86B,   0x342, 0xAF, 0x0C, 0x57, 0xFF), // 23
    VTX(   -11,    -21,      7,   0x68A,   0x400, 0xC1, 0x0C, 0x65, 0xFF), // 24
    VTX(   -11,     -7,      4,   0x86F,   0x32D, 0xA8, 0x12, 0x4F, 0xFF), // 25
    VTX(    -9,    -10,      6,   0x656,   0x400, 0xC3, 0x0E, 0x66, 0xFF), // 26
    VTX(     2,    -25,     13,     0x0,   0x400, 0x00, 0x0D, 0x77, 0xFF), // 27
    VTX(     2,    -14,     11,   0x2C0,    0x49, 0x00, 0x16, 0x75, 0xFF), // 28
    VTX(     2,    -25,     13,   0x23D,    0x59, 0x00, 0x0D, 0x77, 0xFF), // 29
    VTX(    -9,     24,      8,   0x851,    0xF4, 0xE2, 0x06, 0x73, 0xFF), // 30
    VTX(   -12,     19,      6,   0x800,   0x400, 0xC1, 0xF9, 0x66, 0xFF), // 31
    VTX(    -7,      9,      9,  -0x1FE,   0x12C, 0xD7, 0xF8, 0x70, 0xFF), // 32
    VTX(     1,     20,      9,     0x0,   0x400, 0x05, 0x0D, 0x77, 0xFF), // 33
    VTX(    -9,     24,      8,   0x6D0,   0x400, 0xE2, 0x06, 0x73, 0xFF), // 34
    VTX(    -7,      9,      9,    0xEC,  -0x350, 0xD7, 0xF8, 0x70, 0xFF), // 35
    VTX(     8,      9,      9,   0x182,  -0x3C2, 0x27, 0x01, 0x71, 0xFF), // 36
    VTX(     9,     21,      7,     0x0,   0x400, 0x22, 0x0F, 0x71, 0xFF), // 37
    VTX(     1,     20,      9,   0x67C,   0x400, 0x05, 0x0D, 0x77, 0xFF), // 38
    VTX(    16,     18,      5,   0x842,   0x410, 0x38, 0x05, 0x69, 0xFF), // 39
    VTX(     9,     21,      7,   0x80B,    0x5B, 0x22, 0x0F, 0x71, 0xFF), // 40
    VTX(     8,      9,      9,   -0xB2,   0x176, 0x27, 0x01, 0x71, 0xFF), // 41
    VTX(    -8,     -1,      6,  -0x6C4,   0x400, 0xD3, 0xFB, 0x6F, 0xFF), // 42
    VTX(     1,      3,      9,   0x2A0,   0x190, 0xFE, 0xDE, 0x72, 0xFF), // 43
    VTX(    -8,     -1,      6,   0x780,   0x400, 0xD3, 0xFB, 0x6F, 0xFF), // 44
    VTX(     2,     -2,      7,   0x29E,   0x400, 0x00, 0xFB, 0x77, 0xFF), // 45
    VTX(     1,      3,      9,   0x56B,   -0x28, 0xFE, 0xDE, 0x72, 0xFF), // 46
    VTX(     2,     -2,      7,   0x80B,    0xE6, 0x00, 0xFB, 0x77, 0xFF), // 47
    VTX(    12,     -2,      6,   0x800,   0x400, 0x2C, 0xFE, 0x6F, 0xFF), // 48
    VTX(    -7,      9,      9,   0x5CE,  -0x168, 0xD7, 0xF8, 0x70, 0xFF), // 49
    VTX(     8,      9,      9,   0x17C,   0x19F, 0x27, 0x01, 0x71, 0xFF), // 50
    VTX(    12,     -2,      6,  -0x7C9,   0x45B, 0x2C, 0xFE, 0x6F, 0xFF), // 51
};

// ============================================================================
// Main Gauntlet Display List (white color scheme)
// ============================================================================
Gfx gMogmaMittsDL[] = {
    // Section 1: Environment-mapped metal (uses external color DL)
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 1, 1, PRIM_LOD_FRAC, 1, COMBINED, 0, SHADE, 0, 1,
        1, PRIM_LOD_FRAC, 1),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0x0FA0, 0x0FA0, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown12Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1),
    gsSPClearGeometryMode(G_FOG),
    gsSPSetGeometryMode(G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(&gMogmaMittsVtx[0], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPVertex(&gMogmaMittsVtx[3], 11, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 0, 0),
    gsSP2Triangles(0, 5, 1, 0, 0, 4, 5, 0),
    gsSP2Triangles(3, 6, 4, 0, 7, 8, 2, 0),
    gsSP2Triangles(2, 8, 9, 0, 9, 10, 3, 0),
    gsSP1Triangle(10, 6, 3, 0),

    // Section 2: Forearm surface
    gsDPPipeSync(),
    gsSPTexture(0x0BB8, 0x0BB8, 0, G_TX_RENDERTILE, G_ON),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPVertex(&gMogmaMittsVtx[14], 30, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
    gsSP2Triangles(4, 0, 2, 0, 5, 3, 0, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 8, 7, 0),
    gsSP2Triangles(10, 11, 8, 0, 6, 8, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 15, 14, 13, 0),
    gsSP2Triangles(16, 17, 14, 0, 12, 14, 17, 0),
    gsSP2Triangles(18, 19, 20, 0, 21, 22, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 5, 27, 3, 0),
    gsSP2Triangles(16, 28, 17, 0, 10, 29, 11, 0),
    gsSP1Triangle(9, 8, 7, 0),

    // Section 3: Main arm body (WHITE - was gold 170,60,0)
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsDPSetEnvColor(200, 200, 220, 255),
    gsSPTexture(0x03E8, 0x03E8, 0, G_TX_RENDERTILE, G_ON),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPVertex(&gMogmaMittsVtx[44], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(3, 1, 4, 0, 1, 6, 4, 0),
    gsSP2Triangles(6, 7, 4, 0, 8, 9, 10, 0),
    gsSP2Triangles(10, 11, 8, 0, 12, 8, 7, 0),
    gsSP2Triangles(13, 14, 15, 0, 16, 17, 18, 0),
    gsSP2Triangles(19, 20, 16, 0, 17, 14, 13, 0),
    gsSP2Triangles(18, 17, 13, 0, 12, 13, 15, 0),
    gsSP2Triangles(16, 18, 19, 0, 21, 22, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
    gsSP2Triangles(12, 15, 28, 0, 29, 25, 30, 0),
    gsSP2Triangles(23, 31, 30, 0, 31, 22, 30, 0),
    gsSPVertex(&gMogmaMittsVtx[76], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 5, 6, 9, 0),
    gsSP2Triangles(10, 11, 12, 0, 10, 13, 11, 0),
    gsSP2Triangles(14, 6, 15, 0, 14, 16, 6, 0),
    gsSP2Triangles(5, 9, 17, 0, 5, 4, 18, 0),
    gsSP2Triangles(19, 20, 21, 0, 19, 22, 20, 0),
    gsSP2Triangles(15, 23, 24, 0, 15, 24, 14, 0),
    gsSP2Triangles(25, 26, 12, 0, 25, 12, 11, 0),
    gsSP2Triangles(9, 27, 18, 0, 9, 18, 4, 0),
    gsSP2Triangles(9, 4, 3, 0, 15, 28, 7, 0),
    gsSP2Triangles(15, 7, 5, 0, 12, 26, 23, 0),
    gsSP2Triangles(12, 23, 15, 0, 29, 30, 10, 0),
    gsSP1Triangle(29, 10, 12, 0),
    gsSPVertex(&gMogmaMittsVtx[107], 15, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 1, 4, 0),
    gsSP2Triangles(3, 4, 5, 0, 6, 7, 8, 0),
    gsSP2Triangles(6, 9, 7, 0, 3, 5, 10, 0),
    gsSP2Triangles(5, 11, 10, 0, 2, 7, 12, 0),
    gsSP2Triangles(12, 13, 0, 0, 0, 2, 12, 0),
    gsSP2Triangles(1, 14, 2, 0, 1, 3, 14, 0),

    // Section 4: Finger/gem accent (WHITE with slight blue - was red 255,60,100)
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 230, 240, 255, 255),
    gsDPSetEnvColor(180, 190, 210, 255),
    gsSPTexture(0x0FA0, 0x0FA0, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(&gMogmaMittsVtx[122], 6, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 2, 0),
    gsSP2Triangles(1, 5, 4, 0, 1, 0, 5, 0),

    // Section 5: Bottom/strap (light white - was dark 50,20,0)
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 220, 220, 235, 255),
    gsDPSetEnvColor(160, 160, 180, 255),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(gEffUnknown12Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(&gMogmaMittsVtx[128], 24, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 1, 4, 0),
    gsSP2Triangles(1, 3, 5, 0, 1, 6, 7, 0),
    gsSP2Triangles(2, 1, 8, 0, 9, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 15, 16, 17, 0),
    gsSP2Triangles(18, 19, 20, 0, 21, 22, 23, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// Plate Overlay Display List
// ============================================================================
Gfx gMogmaMittsPlateDL[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_XLU_DECAL2),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0,
        COMBINED),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(object_gi_glovesTex_000000, G_IM_FMT_I, G_IM_SIZ_8b, 64,
        32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPClearGeometryMode(G_FOG | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_CULL_BACK | G_LIGHTING),
    gsSPVertex(&gMogmaMittsPlateVtx[0], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 2, 0),
    gsSP2Triangles(0, 2, 4, 0, 5, 6, 7, 0),
    gsSP2Triangles(8, 9, 7, 0, 8, 7, 6, 0),
    gsSP2Triangles(10, 11, 12, 0, 10, 12, 13, 0),
    gsSP2Triangles(14, 15, 16, 0, 13, 12, 17, 0),
    gsSP2Triangles(18, 19, 20, 0, 20, 21, 18, 0),
    gsSP2Triangles(22, 23, 24, 0, 16, 25, 26, 0),
    gsSP2Triangles(24, 27, 22, 0, 28, 22, 27, 0),
    gsSP2Triangles(18, 19, 20, 0, 29, 20, 19, 0),
    gsSP1Triangle(26, 14, 16, 0),
    gsSPVertex(&gMogmaMittsPlateVtx[30], 22, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
    gsSP2Triangles(2, 1, 12, 0, 13, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 19, 14, 13, 0),
    gsSP2Triangles(16, 18, 20, 0, 9, 11, 21, 0),
    gsSPEndDisplayList(),
};

// ============================================================================
// White color DLs (for combined give DL)
// ============================================================================

// White metal color (for environment-mapped sections 1-2)
static Gfx gMogmaMittsWhiteColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsDPSetEnvColor(200, 200, 220, 255),
    gsSPEndDisplayList(),
};

// White plate color (for plate overlay)
static Gfx gMogmaMittsPlateColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 240, 240, 255, 255),
    gsDPSetEnvColor(180, 180, 200, 255),
    gsSPEndDisplayList(),
};

// ============================================================================
// Combined Give Item Display List
// ============================================================================
Gfx gMogmaMittsGiveDL[] = {
    gsSPDisplayList(gMogmaMittsWhiteColorDL),
    gsSPDisplayList(gMogmaMittsDL),
    gsSPDisplayList(gMogmaMittsPlateColorDL),
    gsSPDisplayList(gMogmaMittsPlateDL),
    gsSPEndDisplayList(),
};

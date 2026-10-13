/*
 * File: z_vismono.c
 * Description: Color frame buffer effect to desaturate the colors.
 */

#include "global.h"
#include "z64vismono.h"
#include "system_malloc.h"
#include <string.h>

// Height of the fragments the color frame buffer (CFB) is split into.
// It is the maximum amount of lines such that all rgba16 SCREEN_WIDTH-long lines fit into
// the half of tmem (0x800 bytes) dedicated to color-indexed data.
#define VISMONO_CFBFRAG_HEIGHT (0x800 / (SCREEN_WIDTH * G_IM_SIZ_16b_BYTES))

// Maximum size of the dlist written by `VisMono_DesaturateDList`.
// `VisMono_DesaturateDList` consistently uses `VISMONO_DLSIZE - 2` double words, so this can be 2 less.
#define VISMONO_DLSIZE (3 + SCREEN_HEIGHT / VISMONO_CFBFRAG_HEIGHT * (7 + 2 + 2 + 3) + 2 + 2)

// How much each color component contributes to the desaturated result.
// These coefficients are close to what the YUV color space defines Y (luminance) as:
// https://en.wikipedia.org/wiki/YUV#Conversion_to/from_RGB
#define VISMONO_FAC_RED 2
#define VISMONO_FAC_GREEN 4
#define VISMONO_FAC_BLUE 1
#define VISMONO_FAC_NORM (0x1F * VISMONO_FAC_RED + 0x1F * VISMONO_FAC_GREEN + 0x1F * VISMONO_FAC_BLUE)

void VisMono_Init(VisMono* this) {
    memset(this, 0, sizeof(VisMono));
    this->unk_00 = 0;
    this->setScissor = false;
    this->primColor.r = 255;
    this->primColor.g = 255;
    this->primColor.b = 255;
    this->primColor.a = 255;
    this->envColor.r = 0;
    this->envColor.g = 0;
    this->envColor.b = 0;
    this->envColor.a = 0;
}

void VisMono_Destroy(VisMono* this) {
    SystemArena_Free(this->dList);
}
// BENTODO move to a header since it might be helpful other places.
#define GPACK_IA16(i, a) (((i) << 8) | (a))

void VisMono_DesaturateTLUT(u16* tlut) {
    s32 i;

    for (i = 0; i < 256; i++) {
        // `tlut[i]` is a IA16 color
        // `i` corresponds to either byte of a RGBA16 color RRRR_RGGG GGBB_BBBA from the color frame buffer

        // The high byte I (intensity) corresponds to `i` being interpreted as the high byte RRRR_RGGG
        // I = (RRRRR * FAC_RED + GGG00 * FAC_GREEN) * (255 / FAC_NORM)

        // The low byte A (alpha) corresponds to `i` being interpreted as the low byte GGBB_BBBA
        // A = (000GG * FAC_GREEN + BBBBB * FAC_BLUE) * (255 / FAC_NORM)

        // Note: I + A = (RRRRR * FAC_RED + GGGGG * FAC_GREEN + BBBBB * FAC_BLUE) * (255 / FAC_NORM)

        tlut[i] = GPACK_IA16(
            (((i >> 3) & 0x1F) * VISMONO_FAC_RED + ((i << 2) & 0x1F) * VISMONO_FAC_GREEN) * 255 / VISMONO_FAC_NORM,
            (((i >> 6) & 0x1F) * VISMONO_FAC_GREEN + ((i >> 1) & 0x1F) * VISMONO_FAC_BLUE) * 255 / VISMONO_FAC_NORM);
    }
}

Gfx* VisMono_DesaturateDList(Gfx* gfx) {
    s32 y;
    s32 height = VISMONO_CFBFRAG_HEIGHT;
    u16* cfbFrag = D_0F000000_TO_SEGMENTED;

    gDPPipeSync(gfx++);
    // `G_TT_IA16`: use color-indexed images, and IA16 palettes
    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_IA16 | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_1PRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) | G_RM_CLD_SURF2);
    // First color cycle sums texel 1 alpha and texel 0 color
    // By using IA16 palettes, this means summing A (from the IA16 color texel 1 maps to)
    // with I (from the IA16 color texel 0 maps to)
    gDPSetCombineLERP(gfx++, 1, 0, TEXEL1_ALPHA, TEXEL0, 0, 0, 0, 1, PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0,
                      0, 0, PRIMITIVE);

    for (y = 0; y <= SCREEN_HEIGHT - height; y += height) {
        // Load a few lines of the color frame buffer
        gDPLoadTextureBlock(gfx++, cfbFrag, G_IM_FMT_CI, G_IM_SIZ_8b, SCREEN_WIDTH * 2, height, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        // Set texel 0 to be a CI8 image with width `SCREEN_WIDTH * 2` and height `VISMONO_CFBFRAG_HEIGHT`
        // Its position in texture image space is shifted along +S by 2
        gDPSetTile(gfx++, G_IM_FMT_CI, G_IM_SIZ_8b, SCREEN_WIDTH * 2 * G_IM_SIZ_8b_LINE_BYTES / 8, 0x0, G_TX_RENDERTILE,
                   0, G_TX_NOMIRROR | G_TX_CLAMP, 0, 0, G_TX_NOMIRROR | G_TX_CLAMP, 0, 0);
        gDPSetTileSize(gfx++, G_TX_RENDERTILE, 2 << 2, 0, (SCREEN_WIDTH * 2 + 1) << 2,
                       (VISMONO_CFBFRAG_HEIGHT - 1) << 2);

        // Set texel 1 to be a CI8 image with width `SCREEN_WIDTH * 2` and height `VISMONO_CFBFRAG_HEIGHT`
        // Its position in texture image space is shifted along +S by 1
        gDPSetTile(gfx++, G_IM_FMT_CI, G_IM_SIZ_8b, SCREEN_WIDTH * 2 * G_IM_SIZ_8b_LINE_BYTES / 8, 0x0, 1, 1,
                   G_TX_NOMIRROR | G_TX_CLAMP, 0, 0, G_TX_NOMIRROR | G_TX_CLAMP, 0, 0);
        gDPSetTileSize(gfx++, 1, 1 << 2, 0, (SCREEN_WIDTH * 2) << 2, (VISMONO_CFBFRAG_HEIGHT - 1) << 2);

        // Draw a `SCREEN_WIDTH` wide, `height` high rectangle.
        // Texture coordinate T (vertical) starts at 0 and changes by one each line (dtdy = 1)
        // Texture coordinate S (horizontal) starts at 2 and changes by two each column (dsdx = 2)

        // Because texel 0 is shifted by 2 and texel 1 only by 1 along +S,
        // a pixel at S coordinates s = 2+2*n will look at the 2*n-th byte of texel 0 and the 2*n+1-th byte of texel 1.
        // (in "s = 2+2*n" the first "2" is the starting S coordinate and the second "2" is the dsdx value)

        // The 2*n-th byte of texel 0 is the high byte of the n-th RGBA16 color of the color frame buffer.
        // The 2*n+1-th byte of texel 1 is the low byte of the n-th RGBA16 color of the color frame buffer.

        // With the TLUT computed by `VisMono_DesaturateTLUT`:
        // The 2*n-th byte of texel 0 maps to a IA16 color where the high byte I (intensity) corresponds to
        // the high byte of the n-th RGBA16 color of the color frame buffer.
        // The 2*n+1-th byte of texel 1 maps to a IA16 color where the low byte A (alpha) corresponds to
        // the low byte of the n-th RGBA16 color of the color frame buffer.

        // Since the combiner is in part set up to sum texel 0 color (I, intensity) with texel 1 alpha (A, alpha),
        // the resulting color in the drawn rectangle is a desaturated color as defined by the `VISMONO_FAC_*` values.

        gSPTextureRectangle(gfx++, 0, y << 2, SCREEN_WIDTH << 2, (y + height) << 2, G_TX_RENDERTILE, 2 << 5, 0, 2 << 10,
                            1 << 10);
        cfbFrag += SCREEN_WIDTH * height;
    }

    gDPPipeSync(gfx++);
    gSPEndDisplayList(gfx++);
    return gfx;
}

void VisMono_Draw(VisMono* this, Gfx** gfxp) {
    Gfx* gfx = *gfxp;
    u16* tlut;
    Gfx* dList;
    Gfx* dListEnd;

    if (this->tlut) {
        tlut = this->tlut;
    } else {
        tlut = Graph_DlistAlloc(&gfx, 256 * G_IM_SIZ_16b_BYTES);
        VisMono_DesaturateTLUT(tlut);
    }

    if (this->dList) {
        dList = this->dList;
    } else {
        dList = Graph_DlistAlloc(&gfx, VISMONO_DLSIZE * sizeof(Gfx));
        dListEnd = VisMono_DesaturateDList(dList);
    }

    gDPPipeSync(gfx++);

    if (this->setScissor == true) {
        gSPDisplayList(gfx++, D_0E000000_TO_SEGMENTED(setScissor));
    }

    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->primColor.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, this->envColor.rgba);

    gDPLoadTLUT_pal256(gfx++, tlut);

    gSPDisplayList(gfx++, dList);

    gDPPipeSync(gfx++);

    *gfxp = gfx;
}

void VisMono_DrawOld(VisMono* this) {
    if (this->tlut == NULL) {
        this->tlut = SystemArena_Malloc(256 * G_IM_SIZ_16b_BYTES);
        VisMono_DesaturateTLUT(this->tlut);
    }

    if (this->dList == NULL) {
        this->dList = SystemArena_Malloc(VISMONO_DLSIZE * sizeof(Gfx));
        VisMono_DesaturateDList(this->dList);
    }
}

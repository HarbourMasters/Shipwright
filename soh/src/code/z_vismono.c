/**
 * @file z_vismono.c
 *
 * This file implements a full-screen framebuffer effect for desaturating the contents of the framebuffer image.
 *
 * Broadly, this effect is achieved by reinterpreting the contents of the RGBA16 color image as indices into an IA16
 * color palette that converts each color into the desaturated equivalent. More precise details can be found in inline
 * comments.
 */

#include "global.h"

#include <string.h> // memset
#include <assert.h>
#include "soh/framebuffer_effects.h"

// Upstream TODO: Replace these ones they are served from other headers
#define ASSERT(cond, msg, file, line) assert(cond)
#define GPACK_IA16(i, a)    (((i) << 8) | (a))

// Height of the fragments the color frame buffer (CFB) is split into.
// It is the maximum amount of lines such that all rgba16 SCREEN_WIDTH-long lines fit into
// the half of TMEM dedicated to color-indexed data.
#define VISMONO_CFBFRAG_HEIGHT ((TMEM_SIZE / 2) / (SCREEN_WIDTH * G_IM_SIZ_16b_BYTES))

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

// color framebuffer
extern u16 D_0F000000[];

void VisMono_Init(VisMono* this) {
    memset(this, 0, sizeof(VisMono));
    this->vis.type = 0;
    this->vis.scissorType = VIS_NO_SETSCISSOR;
    this->vis.primColor.r = 255;
    this->vis.primColor.g = 255;
    this->vis.primColor.b = 255;
    this->vis.primColor.a = 255;
    this->vis.envColor.r = 0;
    this->vis.envColor.g = 0;
    this->vis.envColor.b = 0;
    this->vis.envColor.a = 0;
}

void VisMono_Destroy(VisMono* this) {
    SYSTEM_ARENA_FREE(this->dList, "../z_vismono.c", 137);
}

void VisMono_DesaturateTLUT(VisMono* this, u16* tlut) {
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
            ((i >> 3 & 0x1F) * VISMONO_FAC_RED + (i << 2 & 0x1F) * VISMONO_FAC_GREEN) * 255 / VISMONO_FAC_NORM,
            ((i >> 6 & 0x1F) * VISMONO_FAC_GREEN + (i >> 1 & 0x1F) * VISMONO_FAC_BLUE) * 255 / VISMONO_FAC_NORM);
    }
}

Gfx* VisMono_DesaturateDList(VisMono* this, Gfx* gfx) {
    s32 y;
    s32 height = VISMONO_CFBFRAG_HEIGHT;
    u16* cfbFrag = D_0F000000;

    gDPPipeSync(gfx++);
    // `G_TT_IA16`: use color-indexed images, and IA16 palettes
    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_IA16 | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_1PRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | G_RM_PASS | G_RM_CLD_SURF2);
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
        // Note the palette index for this tile has also been incremented from 0 to 1, however the palette index is
        // ignored for CI8 texture sampling.
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

void VisMono_Draw(VisMono* this, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    u16* tlut;
    Gfx* dList;
    Gfx* dListEnd;

    // SOH [Port] Implement VisMono by performing a framebuffer copy and redraw with an active
    // grayscale command to set the mono color
    FB_CopyToFramebuffer(&gfx, 0, gReusableFrameBuffer, false, NULL);
    gDPSetGrayscaleColor(gfx++, this->vis.primColor.r, this->vis.primColor.g, this->vis.primColor.b,
                         this->vis.primColor.a);
    gSPGrayscale(gfx++, true);
    FB_DrawFromFramebuffer(&gfx, gReusableFrameBuffer, 255);
    gSPGrayscale(gfx++, false);

#if 0
    if (this->tlut) {
        tlut = this->tlut;
    } else {
        tlut = Graph_DlistAlloc(&gfx, 256 * G_IM_SIZ_16b_BYTES);
        VisMono_DesaturateTLUT(this, tlut);
    }

    if (this->dList) {
        dList = this->dList;
    } else {
        dList = Graph_DlistAlloc(&gfx, VISMONO_DLSIZE * sizeof(Gfx));
        dListEnd = VisMono_DesaturateDList(this, dList);

        if (!(dListEnd <= dList + VISMONO_DLSIZE)) {
            LOG_ADDRESS("glistp_end", dListEnd);
            LOG_ADDRESS("mono_dl", dList);
            LOG_ADDRESS("mono_dl + (1+3+1+1+80*(7+2+2+3)+1)", dList + VISMONO_DLSIZE);
            LOG_ADDRESS("(1+3+1+1+80*(7+2+2+3)+1)", VISMONO_DLSIZE);
        }
        ASSERT(dListEnd <= dList + VISMONO_DLSIZE, "glistp_end <= mono_dl + DLSIZE", "../z_vismono.c", 262);
    }

    gDPPipeSync(gfx++);

    if (this->vis.scissorType == VIS_SETSCISSOR) {
        gDPSetScissor(gfx++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->vis.primColor.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, this->vis.envColor.rgba);

    gDPLoadTLUT_pal256(gfx++, tlut);

    gSPDisplayList(gfx++, dList);

    gDPPipeSync(gfx++);
#endif

    *gfxP = gfx;
}

void VisMono_DrawOld(VisMono* this) {
    Gfx* dListEnd;

    if (this->tlut == NULL) {
        this->tlut = SYSTEM_ARENA_MALLOC(256 * G_IM_SIZ_16b_BYTES, "../z_vismono.c", 283);
        VisMono_DesaturateTLUT(this, this->tlut);
    }

    if (this->dList == NULL) {
        this->dList = SYSTEM_ARENA_MALLOC(VISMONO_DLSIZE * sizeof(Gfx), "../z_vismono.c", 289);
        dListEnd = VisMono_DesaturateDList(this, this->dList);
        ASSERT(dListEnd <= this->dList + VISMONO_DLSIZE, "glistp_end <= this->mono_dl + DLSIZE", "../z_vismono.c", 292);
    }
}

/**
 * Unused. A very simple utility for drawing text on screen.
 */

#include "global.h"

// OTRTODO
u64 gMojiFontTex[] =
{
    0,
};

u64 gMojiFontTLUTs[4][4] =
{
    0
};

// how big to draw the characters on screen
#define DISP_CHAR_WIDTH 8
#define DISP_CHAR_HEIGHT 8

// gMojiFontTex is a TEX_CHAR_COLS x TEX_CHAR_ROWS grid of characters,
// each character being TEX_CHAR_WIDTH x TEX_CHAR_HEIGHT in size.
// Each spot on the grid contains 4 characters, which are revealed by using different TLUTs.

#define TEX_CHAR_WIDTH 8
#define TEX_CHAR_HEIGHT 8

#define TEX_CHAR_COLS 2
#define TEX_CHAR_ROWS 16

// A character `c = 0bRRRRRCTT` maps to row 0bRRRRR, column 0bC and TLUT 0bTT.
#define GET_CHAR_TLUT_INDEX(c) (c & 3)
// `/ 4` matches the `& 4` (`(c & 4) / 4` is the column the character is in)
#define GET_TEX_CHAR_S(c) ((u16)(c & 4) * ((1 << 5) * TEX_CHAR_WIDTH / 4))
#define GET_TEX_CHAR_T(c) ((u16)(c >> 3) * ((1 << 5) * TEX_CHAR_HEIGHT))

u32 sFontColorRed = 255;
u32 sFontColorGreen = 255;
u32 sFontColorBlue = 255;
u32 sFontColorAlpha = 255;

s32 sScreenPosX = 0;
s32 sScreenPosY = 0;

s32 sCurTLUTIndex;

void Moji_SetColor(u32 red, u32 green, u32 blue, u32 alpha) {
    sFontColorRed = red;
    sFontColorGreen = green;
    sFontColorBlue = blue;
    sFontColorAlpha = alpha;
}

void Moji_SetPosition(s32 gridX, s32 gridY) {
    if (gridX >= SCREEN_WIDTH / DISP_CHAR_WIDTH) {
        sScreenPosX = SCREEN_WIDTH - DISP_CHAR_WIDTH;
    } else if (gridX < 0) {
        sScreenPosX = 0;
    } else {
        sScreenPosX = gridX * DISP_CHAR_WIDTH;
    }

    if (gridY >= SCREEN_HEIGHT / DISP_CHAR_HEIGHT) {
        sScreenPosY = SCREEN_HEIGHT - DISP_CHAR_HEIGHT;
    } else if (gridY < 0) {
        sScreenPosY = 0;
    } else {
        sScreenPosY = gridY * DISP_CHAR_HEIGHT;
    }
}

void Moji_DrawChar(GraphicsContext* gfxCtx, char c) {
    s32 pad[2];

    OPEN_DISPS(gfxCtx);

    if ((u32)gMojiFontTLUTs & 0xF) {
        osSyncPrintf("moji_tlut --> %X\n", gMojiFontTLUTs);
    }

    if (sCurTLUTIndex != GET_CHAR_TLUT_INDEX(c)) {
        gDPLoadTLUT(POLY_OPA_DISP++, 16, 256, gMojiFontTLUTs[GET_CHAR_TLUT_INDEX(c)]);
        sCurTLUTIndex = GET_CHAR_TLUT_INDEX(c);
    }
    gSPTextureRectangle(POLY_OPA_DISP++, sScreenPosX << 2, sScreenPosY << 2, (sScreenPosX + DISP_CHAR_WIDTH) << 2,
                        (sScreenPosY + DISP_CHAR_HEIGHT) << 2, G_TX_RENDERTILE, GET_TEX_CHAR_S(c), GET_TEX_CHAR_T(c),
                        (1 << 10) * TEX_CHAR_WIDTH / DISP_CHAR_WIDTH, (1 << 10) * TEX_CHAR_HEIGHT / DISP_CHAR_HEIGHT);

    CLOSE_DISPS(gfxCtx);
}

/**
 * Does not work as is in most cases.
 * Can work if the render mode, combiner and possibly other settings are set correctly.
 * For example this works with the render mode used in `GfxPrint_Setup`,
 * and `G_CC_MODULATEI_PRIM` for both combiner cycles.
 */
void Moji_DrawString(GraphicsContext* gfxCtx, const char* str) {
    s32 i;

    OPEN_DISPS(gfxCtx);

    if ((u32)gMojiFontTex & 0xF) {
        osSyncPrintf("font_ff --> %X\n", gMojiFontTex);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sFontColorRed, sFontColorGreen, sFontColorBlue, sFontColorAlpha);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, (s32)gMojiFontTex, G_IM_FMT_CI, TEX_CHAR_COLS * TEX_CHAR_WIDTH,
                           TEX_CHAR_ROWS * TEX_CHAR_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    sCurTLUTIndex = -1;

    for (i = 0; str[i] != '\0'; i++) {
        switch (str[i]) {
            case '\t':
                sScreenPosX = (((sScreenPosX / DISP_CHAR_WIDTH) / 8) + 1) * DISP_CHAR_WIDTH * 8;
                if (sScreenPosX >= SCREEN_WIDTH) {
                    sScreenPosX = 0;
                    sScreenPosY += DISP_CHAR_HEIGHT;
                    if (sScreenPosY >= SCREEN_HEIGHT) {
                        sScreenPosY = 0;
                    }
                }
                break;
            case '\n':
            case '\r':
                sScreenPosX = 0;
                sScreenPosY += DISP_CHAR_HEIGHT;
                if (sScreenPosY >= SCREEN_HEIGHT) {
                    sScreenPosY = 0;
                }
                break;
            default:
                Moji_DrawChar(gfxCtx, str[i]);
                sScreenPosX += DISP_CHAR_WIDTH;
        }
    }

    CLOSE_DISPS(gfxCtx);
}

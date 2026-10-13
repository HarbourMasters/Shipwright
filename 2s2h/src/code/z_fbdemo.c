/**
 * @file z_fbdemo.c
 *
 * This file implements an unused transition system that takes the current screen, partitions it into large tiles, and
 * can apply an effect to them.
 *
 * The screen is divided into 7 rows and 10 columns of tiles. (`gScreenWidth`/ 10 = `gScreenHeight` / 7 = 0x20)
 *
 * @note The only coded effect has a visual effect to blend the tiles to a single point, which looks like the screen
 * gets sucked into.
 */

#include "z64transition.h"

#include "global.h"
#include "system_malloc.h"
#include "macros.h"
#include "z64math.h"
#include <string.h>

Gfx sTransTileSetupDL[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsSPLoadGeometryMode(G_TEXTURE_ENABLE),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
    gsSPEndDisplayList(),
};

#define SET_VERTEX(vtx, x, y, z, f, s, t, nx, ny, nz, alpha) \
    {                                                        \
        Vtx_tn* vtxn = &(vtx)->n;                            \
        vtxn->ob[0] = (x);                                   \
        vtxn->ob[1] = (y);                                   \
        vtxn->ob[2] = (z);                                   \
        vtxn->flag = 0;                                      \
        vtxn->tc[0] = (s);                                   \
        vtxn->tc[1] = (t);                                   \
        vtxn->n[0] = (nx);                                   \
        vtxn->n[1] = (ny);                                   \
        vtxn->n[2] = (nz);                                   \
        vtxn->a = (alpha);                                   \
    }                                                        \
    (void)0

void TransitionTile_InitGraphics(TransitionTile* this) {
    s32 frame;
    s32 col;
    s32 col2;
    s32 colTex;
    Vtx* vtx;
    s32 row;
    s32 rowTex;
    Gfx* gfx;

    guMtxIdent(&this->modelView);
    guMtxIdent(&this->unk_98);
    guOrtho(&this->projection, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, -1000.0f, 1000.0f, 1.0f);

    for (frame = 0; frame < 2; frame++) {
        this->frame = frame;
        vtx = (this->frame == 0) ? this->vtxFrame1 : this->vtxFrame2;
        rowTex = 0;
        for (row = 0; row < (this->rows + 1); row++) {
            colTex = 0;
            for (col = 0; col < (this->cols + 1); col++) {
                SET_VERTEX(vtx++, col * 0x20, row * 0x20, -5, 0, colTex << 6, rowTex << 6, 0, 0, 120, 255);
                colTex += 0x20;
            }
            rowTex += 0x20;
        }
    }

    gfx = this->gfx;
    for (rowTex = 0, row = 0; row < this->rows; row++, rowTex += 0x20) {
        gSPVertex(gfx++, SEGMENT_ADDR(0xA, (u32)row * (this->cols + 1) * sizeof(Vtx)), 2 * (this->cols + 1), 0);

        colTex = 0;
        col2 = 0;
        col = 0;
        while (col < this->cols) {
            gDPPipeSync(gfx++);

            gDPLoadTextureTile(gfx++, SEGMENT_ADDR(0xB, 0), G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, SCREEN_HEIGHT,
                               colTex, rowTex, colTex + 0x20, rowTex + 0x20, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(gfx++, col, col + 1, col2 + this->cols + 2, this->cols + col2 + 1, 0);

            colTex += 0x20;
            col2++;
            col++;
        }
    }

    gDPPipeSync(gfx++);
    gSPEndDisplayList(gfx++);
}

void TransitionTile_InitVtxData(TransitionTile* this) {
    s32 row;
    s32 col;

    for (row = 0; row < this->rows + 1; row++) {
        for (col = 0; col < this->cols + 1; col++) {
            (this->vtxData + col + row * (this->cols + 1))->x = col * 0x20;
            (this->vtxData + col + row * (this->cols + 1))->y = row * 0x20;
        }
    }
}

void TransitionTile_Destroy(TransitionTile* this) {
    Sleep_Msec(100);

    if (this->vtxData != NULL) {
        SystemArena_Free(this->vtxData);
        this->vtxData = NULL;
    }
    if (this->vtxFrame1 != NULL) {
        SystemArena_Free(this->vtxFrame1);
        this->vtxFrame1 = NULL;
    }
    if (this->vtxFrame2 != NULL) {
        SystemArena_Free(this->vtxFrame2);
        this->vtxFrame2 = NULL;
    }
    if (this->gfx != NULL) {
        SystemArena_Free(this->gfx);
        this->gfx = NULL;
    }
}

TransitionTile* TransitionTile_Init(TransitionTile* this, s32 cols, s32 rows) {
    s32 gridSize;

    memset(this, 0, sizeof(TransitionTile));
    this->frame = 0;
    this->cols = cols;
    this->rows = rows;
    gridSize = (cols + 1) * (rows + 1);
    this->vtxData = SystemArena_Malloc(gridSize * sizeof(TransitionTileVtxData));
    this->vtxFrame1 = SystemArena_Malloc(gridSize * sizeof(Vtx));
    this->vtxFrame2 = SystemArena_Malloc(gridSize * sizeof(Vtx));
    this->gfx = SystemArena_Malloc(((cols * 9 + 1) * rows + 2) * sizeof(Gfx));

    if ((this->vtxData == NULL) || (this->vtxFrame1 == NULL) || (this->vtxFrame2 == NULL) || (this->gfx == NULL)) {
        if (this->vtxData != NULL) {
            SystemArena_Free(this->vtxData);
            this->vtxData = NULL;
        }
        if (this->vtxFrame1 != NULL) {
            SystemArena_Free(this->vtxFrame1);
            this->vtxFrame1 = NULL;
        }
        if (this->vtxFrame2 != NULL) {
            SystemArena_Free(this->vtxFrame2);
            this->vtxFrame2 = NULL;
        }
        if (this->gfx != NULL) {
            SystemArena_Free(this->gfx);
            this->gfx = NULL;
        }
        return NULL;
    }

    TransitionTile_InitGraphics(this);
    TransitionTile_InitVtxData(this);
    this->frame = 0;

    return this;
}

void TransitionTile_SetVtx(TransitionTile* this) {
    s32 row;
    s32 col;
    Vtx* vtx;

    for (row = 0; row < this->rows + 1; row++) {
        for (col = 0; col < this->cols + 1; col++) {
            vtx = (this->frame == 0) ? this->vtxFrame1 : this->vtxFrame2;
            (vtx + col + row * (this->cols + 1))->n.ob[0] = (this->vtxData + col + row * (this->cols + 1))->x;

            vtx = (this->frame == 0) ? this->vtxFrame1 : this->vtxFrame2;
            (vtx + col + row * (this->cols + 1))->n.ob[1] = (this->vtxData + col + row * (this->cols + 1))->y;
        }
    }
}

void TransitionTile_Draw(TransitionTile* this, Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    gSPDisplayList(gfx++, sTransTileSetupDL);
    TransitionTile_SetVtx(this);
    gSPMatrix(gfx++, &this->projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &this->modelView, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(gfx++, 0xA, this->frame == 0 ? this->vtxFrame1 : this->vtxFrame2);
    gSPSegment(gfx++, 0xB, this->zBuffer);
    gSPDisplayList(gfx++, sTransTileSetupDL);
    gSPDisplayList(gfx++, this->gfx);
    gDPPipeSync(gfx++);
    this->frame ^= 1;
    *gfxP = gfx;
}

void TransitionTile_Suck(TransitionTile* this) {
    s32 row;
    s32 col;
    f32 diffX;
    f32 diffY;
    f32 scale;

    for (row = 0; row < this->rows + 1; row++) {
        for (col = 0; col < this->cols + 1; col++) {
            diffX = (this->vtxData + col + row * (this->cols + 1))->x - (this->vtxData + 5 + 4 * (this->cols + 1))->x;
            diffY = (this->vtxData + col + row * (this->cols + 1))->y - (this->vtxData + 5 + 4 * (this->cols + 1))->y;
            scale = (SQ(diffX) + SQ(diffY)) / SQ(10.0f);
            if (scale != 0.0f) {
                if (scale < 1.0f) {
                    scale = 1.0f;
                }
                (this->vtxData + col + row * (this->cols + 1))->x -= diffX / scale;
                (this->vtxData + col + row * (this->cols + 1))->y -= diffY / scale;
            }
        }
    }
}

void TransitionTile_Update(TransitionTile* this) {
}

s32 func_801647B8(TransitionTile* this) {
    return 0;
}

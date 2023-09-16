#include "global.h"
#include <string.h>

#include "code/fbdemo_wipe1/z_fbdemo_wipe1.h"

Gfx sWipeDList[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | G_RM_PASS | G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetCombineLERP(TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, COMBINED, 0,
                       PRIMITIVE, 0, COMBINED, 0, PRIMITIVE, 0),
    gsDPSetPrimDepth(0, 0),
    gsDPLoadTextureBlock_4b(sTransWipeTex, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 6, 6,
                            11, G_TX_NOLOD),
    gsDPLoadMultiBlock_4b(sTransWipeTex, 0x0100, 1, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                          G_TX_MIRROR | G_TX_WRAP, 6, 6, 11, 1),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(SEG_ADDR(8, 0)),
    gsSPVertex(sTransWipeVtx, 25, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(5, 6, 7, 0, 6, 8, 9, 0),
    gsSP2Triangles(8, 10, 11, 0, 10, 12, 13, 0),
    gsSP2Triangles(12, 14, 15, 0, 14, 16, 17, 0),
    gsSP2Triangles(16, 18, 19, 0, 18, 20, 21, 0),
    gsSP2Triangles(20, 22, 23, 0, 22, 0, 24, 0),
    gsSPEndDisplayList(),
};

// unused.
Gfx sWipeSyncDList[] = {
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

void TransitionWipe_Start(void* thisx) {
    TransitionWipe* this = (TransitionWipe*)thisx;

    this->isDone = 0;

    if (this->direction) {
        this->texY = 0x14D;
    } else {
        this->texY = 0x264;
    }

    guPerspective(&this->projection, &this->normal, 60.0f, (4.0 / 3.0f), 10.0f, 12800.0f, 1.0f);
    guLookAt(&this->lookAt, 0.0f, 0.0f, 400.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void* TransitionWipe_Init(void* thisx) {
    TransitionWipe* this = (TransitionWipe*)thisx;

    memset(this, 0, sizeof(*this));
    return this;
}

void TransitionWipe_Destroy(void* thisx) {
}

void TransitionWipe_Update(void* thisx, s32 updateRate) {
    TransitionWipe* this = (TransitionWipe*)thisx;
    u8 unk1419;

    if (this->direction != 0) {
        unk1419 = gSaveContext.transWipeSpeed;
        this->texY += (unk1419 * 3) / updateRate;
        if (this->texY >= 0x264) {
            this->texY = 0x264;
            this->isDone = 1;
        }
    } else {
        unk1419 = gSaveContext.transWipeSpeed;
        this->texY -= (unk1419 * 3) / updateRate;
        if (this->texY < 0x14E) {
            this->texY = 0x14D;
            this->isDone = 1;
        }
    }
}

void TransitionWipe_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    TransitionWipe* this = (TransitionWipe*)thisx;
    s32 pad[4];
    Gfx* tex;
    Gfx* wipeDl = sWipeDList;

    modelView = this->modelView[this->frame];

    this->frame ^= 1;
    guScale(&modelView[0], 0.56f, 0.56f, 1.0f);
    guRotate(&modelView[1], 0.0f, 0.0f, 0.0f, 1.0f);
    guTranslate(&modelView[2], 0.0f, 0.0f, 0.0f);
    gDPPipeSync(gfx++);
    tex = Gfx_BranchTexScroll(&gfx, this->texX, this->texY, 0, 0);
    gSPSegment(gfx++, 8, tex);
    gDPSetPrimColor(gfx++, 0, 0x80, this->color.r, this->color.g, this->color.b, 255);
    gSPMatrix(gfx++, &this->projection, G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(gfx++, this->normal);
    gSPMatrix(gfx++, &this->lookAt, G_MTX_MUL | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &modelView[0], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPDisplayList(gfx++, sWipeDList);
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionWipe_IsDone(void* thisx) {
    TransitionWipe* this = (TransitionWipe*)thisx;

    return this->isDone;
}

void TransitionWipe_SetType(void* thisx, s32 type) {
    TransitionWipe* this = (TransitionWipe*)thisx;

    if (type == 1) {
        this->direction = 1;
    } else {
        this->direction = 0;
    }

    if (this->direction != 0) {
        this->texY = 0x14D;
    } else {
        this->texY = 0x264;
    }
}

void TransitionWipe_SetColor(void* thisx, u32 color) {
    TransitionWipe* this = (TransitionWipe*)thisx;

    this->color.rgba = color;
}

void TransitionWipe_SetEnvColor(void* thisx, u32 color) {
    TransitionWipe* this = (TransitionWipe*)thisx;

    this->envColor.rgba = color;
}

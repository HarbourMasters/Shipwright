/*
 * File: z_fbdemo_wipe1.c
 * Overlay: ovl_fbdemo_wipe1
 * Description:
 */

#include "global.h"
#include "z_fbdemo_wipe1.h"
#include <string.h>

void* TransitionWipe1_Init(void* thisx);
void TransitionWipe1_Destroy(void* thisx);
void TransitionWipe1_Update(void* thisx, s32 updateRate);
void TransitionWipe1_Draw(void* thisx, Gfx** gfxP);
void TransitionWipe1_Start(void* thisx);
void TransitionWipe1_SetType(void* thisx, s32 type);
void TransitionWipe1_SetColor(void* thisx, u32 color);
void TransitionWipe1_SetEnvColor(void* thisx, u32 color);
s32 TransitionWipe1_IsDone(void* thisx);

#include "assets/overlays/ovl_fbdemo_wipe1/ovl_fbdemo_wipe1.h"

TransitionProfile TransitionWipe1_Profile = {
    TransitionWipe1_Init,     TransitionWipe1_Destroy,     TransitionWipe1_Update,
    TransitionWipe1_Draw,     TransitionWipe1_Start,       TransitionWipe1_SetType,
    TransitionWipe1_SetColor, TransitionWipe1_SetEnvColor, TransitionWipe1_IsDone,
};

typedef enum {
    /* 0 */ TRANS_WIPE1_DIR_IN,
    /* 1 */ TRANS_WIPE1_DIR_OUT
} TransitionWipe1Direction;

void TransitionWipe1_Start(void* thisx) {
    TransitionWipe1* this = (TransitionWipe1*)thisx;

    this->isDone = false;

    if (this->direction != TRANS_WIPE1_DIR_IN) {
        this->texY = (s32)(83.25f * (1 << 2));
    } else {
        this->texY = (s32)(153.0f * (1 << 2));
    }

    guPerspective(&this->projection, &this->normal, 60.0f, (4.0f / 3.0f), 10.0f, 12800.0f, 1.0f);
    guLookAt(&this->lookAt, 0.0f, 0.0f, 400.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void* TransitionWipe1_Init(void* thisx) {
    TransitionWipe1* this = (TransitionWipe1*)thisx;

    memset(this, 0, sizeof(TransitionWipe1));
    return this;
}

void TransitionWipe1_Destroy(void* thisx) {
}

void TransitionWipe1_Update(void* thisx, s32 updateRate) {
    TransitionWipe1* this = (TransitionWipe1*)thisx;

    if (this->direction != TRANS_WIPE1_DIR_IN) {
        this->texY += (((void)0, gSaveContext.transWipeSpeed) * 3) / updateRate;
        if (this->texY >= (s32)(153.0f * (1 << 2))) {
            this->texY = (s32)(153.0f * (1 << 2));
            this->isDone = true;
        }
    } else {
        this->texY -= (((void)0, gSaveContext.transWipeSpeed) * 3) / updateRate;
        if (this->texY <= (s32)(83.25f * (1 << 2))) {
            this->texY = (s32)(83.25f * (1 << 2));
            this->isDone = true;
        }
    }
}

void TransitionWipe1_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    TransitionWipe1* this = (TransitionWipe1*)thisx;
    Gfx* texScroll;
    s32 pad[4];

    modelView = this->modelView[this->frame];
    this->frame ^= 1;

    guScale(&modelView[0], 0.56f, 0.56f, 1.0f);
    guRotate(&modelView[1], 0.0f, 0.0f, 0.0f, 1.0f);
    guTranslate(&modelView[2], 0.0f, 0.0f, 0.0f);
    gDPPipeSync(gfx++);
    texScroll = Gfx_BranchTexScroll(&gfx, this->texX, this->texY, 0, 0);
    gSPSegment(gfx++, 8, texScroll);
    gDPSetPrimColor(gfx++, 0, 0x80, this->primColor.r, this->primColor.g, this->primColor.b, 255);
    gSPMatrix(gfx++, &this->projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(gfx++, this->normal);
    gSPMatrix(gfx++, &this->lookAt, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &modelView[0], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPDisplayList(gfx++, sTransWipe1DL);
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionWipe1_IsDone(void* thisx) {
    TransitionWipe1* this = (TransitionWipe1*)thisx;

    return this->isDone;
}

void TransitionWipe1_SetType(void* thisx, s32 type) {
    TransitionWipe1* this = (TransitionWipe1*)thisx;

    if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->direction = TRANS_WIPE1_DIR_OUT;
    } else {
        this->direction = TRANS_WIPE1_DIR_IN;
    }

    if (this->direction != TRANS_WIPE1_DIR_IN) {
        this->texY = (s32)(83.25f * (1 << 2));
    } else {
        this->texY = (s32)(153.0f * (1 << 2));
    }
}

void TransitionWipe1_SetColor(void* thisx, u32 color) {
    TransitionWipe1* this = (TransitionWipe1*)thisx;

    this->primColor.rgba = color;
}

void TransitionWipe1_SetEnvColor(void* thisx, u32 color) {
    TransitionWipe1* this = (TransitionWipe1*)thisx;

    this->envColor.rgba = color;
}

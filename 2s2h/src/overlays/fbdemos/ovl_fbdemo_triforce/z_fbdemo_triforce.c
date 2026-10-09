/*
 * File: z_fbdemo_triforce.c
 * Overlay: ovl_fbdemo_triforce
 * Description:
 */

#include "global.h"
#include "z_fbdemo_triforce.h"
#include "assets/overlays/ovl_fbdemo_triforce/ovl_fbdemo_triforce.h"
#include <string.h>

void* TransitionTriforce_Init(void* thisx);
void TransitionTriforce_Destroy(void* thisx);
void TransitionTriforce_Update(void* thisx, s32 updateRate);
void TransitionTriforce_Draw(void* thisx, Gfx** gfxP);
void TransitionTriforce_Start(void* thisx);
void TransitionTriforce_SetType(void* thisx, s32 type);
void TransitionTriforce_SetColor(void* thisx, u32 color);
s32 TransitionTriforce_IsDone(void* thisx);

TransitionProfile TransitionTriforce_Profile = {
    TransitionTriforce_Init,   TransitionTriforce_Destroy, TransitionTriforce_Update,   TransitionTriforce_Draw,
    TransitionTriforce_Start,  TransitionTriforce_SetType, TransitionTriforce_SetColor, NULL,
    TransitionTriforce_IsDone,
};

void TransitionTriforce_Start(void* thisx) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    switch (this->state) {
        case STATE_SPIRAL_IN_SLOW:
        case STATE_SPIRAL_IN_FAST:
            this->transPos = 1.0f;
            return;

        default:
            this->transPos = 0.03f;
            return;
    }
}

void* TransitionTriforce_Init(void* thisx) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    memset(this, 0, sizeof(TransitionTriforce));
    guOrtho(&this->projection, -SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2, -1000.0f,
            1000.0f, 1.0f);
    this->transPos = 1.0f;
    this->state = STATE_SPIRAL_IN_FAST;
    this->step = 0.015f;
    this->type = TRANS_INSTANCE_TYPE_FILL_OUT;
    return this;
}

void TransitionTriforce_Destroy(void* thisx) {
}

void TransitionTriforce_Update(void* thisx, s32 updateRate) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;
    s32 i;

    for (i = updateRate; i > 0; i--) {
        if (this->state == STATE_SPIRAL_IN_SLOW) {
            this->transPos = CLAMP_MIN(this->transPos * (1.0f - this->step), 0.03f);
        } else if (this->state == STATE_SPIRAL_IN_FAST) {
            this->transPos = CLAMP_MIN(this->transPos - this->step, 0.03f);
        } else if (this->state == STATE_SPIRAL_OUT_SLOW) {
            this->transPos = CLAMP_MAX(this->transPos / (1.0f - this->step), 1.0f);
        } else if (this->state == STATE_SPIRAL_OUT_FAST) {
            this->transPos = CLAMP_MAX(this->transPos + this->step, 1.0f);
        }
    }
}

void TransitionTriforce_SetColor(void* thisx, u32 color) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    this->color.rgba = color;
}

void TransitionTriforce_SetType(void* thisx, s32 type) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    this->type = type;
}

void TransitionTriforce_SetState(void* thisx, s32 state) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    this->state = state;
}

void TransitionTriforce_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    s32 pad[2];
    TransitionTriforce* this = (TransitionTriforce*)thisx;
    f32 rotation = this->transPos * 360.0f;

    modelView = this->modelView[this->frame];
    this->frame ^= 1;
    guScale(&modelView[0], this->transPos * 0.625f, this->transPos * 0.625f, 1.0f);
    guRotate(&modelView[1], rotation, 0.0f, 0.0f, 1.0f);
    guTranslate(&modelView[2], 0.0f, 0.0f, 0.0f);
    gDPPipeSync(gfx++);
    gSPDisplayList(gfx++, sTransTriforceDL);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->color.rgba);
    gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPMatrix(gfx++, &this->projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &modelView[0], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPVertex(gfx++, &sTransTriforceVtx, ARRAY_COUNT(sTransTriforceVtx), 0);
    if (!TransitionTriforce_IsDone(this)) {
        switch (this->type) {
            case TRANS_INSTANCE_TYPE_FILL_OUT:
                gSP2Triangles(gfx++, 0, 4, 5, 0, 4, 1, 3, 0);
                gSP1Triangle(gfx++, 5, 3, 2, 0);
                break;

            case TRANS_INSTANCE_TYPE_FILL_IN:
                gSP2Triangles(gfx++, 3, 4, 5, 0, 0, 2, 6, 0);
                gSP2Triangles(gfx++, 0, 6, 7, 0, 1, 0, 7, 0);
                gSP2Triangles(gfx++, 1, 7, 8, 0, 1, 8, 9, 0);
                gSP2Triangles(gfx++, 1, 9, 2, 0, 2, 9, 6, 0);
                break;

            default:
                break;
        }

    } else {
        switch (this->type) {
            case TRANS_INSTANCE_TYPE_FILL_OUT:
                break;

            case TRANS_INSTANCE_TYPE_FILL_IN:
                gSP1Quadrangle(gfx++, 6, 7, 8, 9, 0);
                break;

            default:
                break;
        }
    }
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionTriforce_IsDone(void* thisx) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    if ((this->state == STATE_SPIRAL_IN_SLOW) || (this->state == STATE_SPIRAL_IN_FAST)) {
        return this->transPos <= 0.03f;
    } else if ((this->state == STATE_SPIRAL_OUT_SLOW) || (this->state == STATE_SPIRAL_OUT_FAST)) {
        return (this->transPos >= 1.0f);
    } else {
        return false;
    }
}

/*
 * File: z_eff_ss_k_fire.c
 * Overlay: ovl_Effect_Ss_K_Fire
 * Description:
 */

#include "z_eff_ss_k_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rAlpha regs[0]
#define rScroll regs[2]
#define rType regs[3]
#define rYScale regs[4]
#define rXZScale regs[5]
#define rScaleMax regs[6]

u32 EffectSsKFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsKFire_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsKFire_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_K_Fire_InitVars = {
    EFFECT_SS_K_FIRE,
    EffectSsKFire_Init,
};

u32 EffectSsKFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsKFireInitParams* initParams = (EffectSsKFireInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = 100;
    this->rScaleMax = initParams->scaleMax;
    this->rAlpha = 255;
    this->rScroll = (s16)Rand_ZeroFloat(5.0f) - 0x19;
    this->rType = initParams->type;
    this->draw = EffectSsKFire_Draw;
    this->update = EffectSsKFire_Update;

    return 1;
}

void EffectSsKFire_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 xzScale;
    f32 yScale;

    xzScale = this->rXZScale / 10000.0f;
    yScale = this->rYScale / 10000.0f;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(xzScale, yScale, xzScale, MTXMODE_APPLY);
    func_80093D84(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                play->state.frames * this->rScroll, 0x20, 0x80));

    if (this->rType >= 100) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, this->rAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 0);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, this->rAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 0);
    }

    gDPPipeSync(POLY_XLU_DISP++);
    Matrix_ReplaceRotation(&play->billboardMtxF);

    if ((index & 1) != 0) {
        Matrix_RotateY(M_PI, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsKFire_Update(PlayState* play, u32 index, EffectSs* this) {
    if (this->rXZScale < this->rScaleMax) {
        this->rXZScale += 4;
        this->rYScale += 4;

        if (this->rXZScale > this->rScaleMax) {
            this->rXZScale = this->rScaleMax;

            if (this->rType != 3) {
                this->rYScale = this->rScaleMax;
            }
        }
    } else {
        if (this->rAlpha > 0) {
            this->rAlpha -= 10;
            if (this->rAlpha <= 0) {
                this->rAlpha = 0;
                this->life = 0;
            }
        }
    }

    if (this->rType == 3) {
        this->rYScale++;
    }
}

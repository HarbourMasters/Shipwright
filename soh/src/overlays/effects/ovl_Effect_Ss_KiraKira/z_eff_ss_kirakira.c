/*
 * File: z_eff_ss_kirakira.c
 * Overlay: ovl_Effect_Ss_KiraKira
 * Description: Sparkles
 */

#include "z_eff_ss_kirakira.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rRotSpeed regs[0]
#define rYaw regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rEnvColorA regs[9]
#define rAlphaStep regs[10]
#define rScale regs[11]
#define rLifespan regs[12]

u32 EffectSsKiraKira_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsKiraKira_Draw(PlayState* play, u32 index, EffectSs* this);
void func_809AABF0(PlayState* play, u32 index, EffectSs* this);
void func_809AACAC(PlayState* play, u32 index, EffectSs* this);
void func_809AAD6C(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_KiraKira_InitVars = {
    EFFECT_SS_KIRAKIRA,
    EffectSsKiraKira_Init,
};

u32 EffectSsKiraKira_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsKiraKiraInitParams* initParams = (EffectSsKiraKiraInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;

    if ((this->life = initParams->life) < 0) {
        this->life = -this->life;
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffSparklesDL);
        this->update = func_809AAD6C;
        this->rEnvColorA = initParams->scale;
        this->rScale = 0;
    } else {
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffSparklesDL);

        if (initParams->updateMode == 0) {
            this->update = func_809AABF0;
        } else {
            this->update = func_809AACAC;
        }

        this->rEnvColorA = initParams->envColor.a;
        this->rScale = initParams->scale;
    }

    this->draw = EffectSsKiraKira_Draw;
    this->rRotSpeed = initParams->rotSpeed;
    this->rYaw = initParams->yaw;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rAlphaStep = initParams->alphaStep;
    this->rLifespan = initParams->life;

    return 1;
}

void EffectSsKiraKira_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx;
    f32 scale;
    s32 pad;
    MtxF mfTrans;
    MtxF mfRotY;
    MtxF mfScale;
    MtxF mfTrans11DA0;
    MtxF mfTrans11DA0RotY;
    MtxF mfResult;
    Mtx* mtx;

    scale = this->rScale / 10000.0f;
    gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetRotateZYX(&mfRotY, 0, 0, this->rYaw);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfRotY, &mfTrans11DA0RotY);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0RotY, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        Gfx_SetupDL_25Xlu2(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        (((s8)((55.0f / this->rLifespan) * this->life) + 200)));
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx);
}

void func_809AABF0(PlayState* play, u32 index, EffectSs* this) {
    this->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
    this->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;
    this->rEnvColorA += this->rAlphaStep;

    if (this->rEnvColorA < 0) {
        this->rEnvColorA = 0;
        this->rAlphaStep = -this->rAlphaStep;
    } else if (this->rEnvColorA > 255) {
        this->rEnvColorA = 255;
        this->rAlphaStep = -this->rAlphaStep;
    }

    this->rYaw += this->rRotSpeed;
}

void func_809AACAC(PlayState* play, u32 index, EffectSs* this) {
    this->velocity.x *= 0.95f;
    this->velocity.z *= 0.95f;
    this->accel.x = Rand_CenteredFloat(0.2f);
    this->accel.z = Rand_CenteredFloat(0.2f);
    this->rEnvColorA += this->rAlphaStep;

    if (this->rEnvColorA < 0) {
        this->rEnvColorA = 0;
        this->rAlphaStep = -this->rAlphaStep;
    } else if (this->rEnvColorA > 255) {
        this->rEnvColorA = 255;
        this->rAlphaStep = -this->rAlphaStep;
    }

    this->rYaw += this->rRotSpeed;
}

void func_809AAD6C(PlayState* play, u32 index, EffectSs* this) {
    this->rScale = this->rEnvColorA * Math_SinS((32768.0f / this->rLifespan) * this->life);
}

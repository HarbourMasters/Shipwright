/*
 * File: z_eff_ss_lightning.c
 * Overlay: ovl_Effect_Ss_Lightning
 * Description: Lightning
 */

#include "z_eff_ss_lightning.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rNumBolts regs[8]
#define rScale regs[9]
#define rYaw regs[10]
#define rLifespan regs[11]

u32 EffectSsLightning_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsLightning_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsLightning_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Lightning_InitVars = {
    EFFECT_SS_LIGHTNING,
    EffectSsLightning_Init,
};

u32 EffectSsLightning_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsLightningInitParams* initParams = (EffectSsLightningInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffLightningDL);
    this->life = initParams->life;
    this->draw = EffectSsLightning_Draw;
    this->update = EffectSsLightning_Update;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rEnvColorA = initParams->envColor.a;
    this->rNumBolts = initParams->numBolts;
    this->rScale = initParams->scale;
    this->rYaw = initParams->yaw;
    this->rLifespan = initParams->life;

    return 1;
}

void EffectSsLightning_NewLightning(PlayState* play, Vec3f* pos, s16 yaw, EffectSs* this) {
    EffectSs newLightning;

    EffectSs_Delete(&newLightning);
    newLightning = *this;
    newLightning.pos = *pos;
    newLightning.rNumBolts--;
    newLightning.rYaw = yaw;
    newLightning.life = newLightning.rLifespan;

    EffectSs_Insert(play, &newLightning);
}

void EffectSsLightning_Draw(PlayState* play, u32 index, EffectSs* this) {
    static void* lightningTextures[] = {
        gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex, gEffLightning4Tex,
        gEffLightning5Tex, gEffLightning6Tex, gEffLightning7Tex, gEffLightning8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfResult;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfRotate;
    MtxF mfTrans11DA0;
    MtxF mfTrans11DA0Rotate;
    Mtx* mtx;
    f32 yScale;
    s16 texIdx;
    f32 xzScale;

    OPEN_DISPS(gfxCtx);

    yScale = this->rScale * 0.01f;
    texIdx = this->rLifespan - this->life;

    if (texIdx > 7) {
        texIdx = 7;
    }

    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    xzScale = yScale * 0.6f;
    SkinMatrix_SetScale(&mfScale, xzScale, yScale, xzScale);
    SkinMatrix_SetRotateZYX(&mfRotate, this->vec.x, this->vec.y, this->rYaw);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfRotate, &mfTrans11DA0Rotate);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0Rotate, &mfScale, &mfResult);

    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80094C50(gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(lightningTextures[texIdx]));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        this->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsLightning_Update(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    Vec3f pos;
    s16 yaw;
    f32 scale;

    if ((this->rNumBolts != 0) && ((this->life + 1) == this->rLifespan)) {

        yaw = this->rYaw + (((Rand_ZeroOne() < 0.5f) ? -1 : 1) * ((s16)((Rand_ZeroOne() * 3640.0f)) + 0xE38));

        scale = (this->rScale * 0.01f) * 80.0f;
        pos.y = this->pos.y + (Math_SinS(this->rYaw - 0x4000) * scale);

        scale = Math_CosS(this->rYaw - 0x4000) * scale;
        pos.x = this->pos.x - (Math_CosS(Camera_GetInputDirYaw(GET_ACTIVE_CAM(play))) * scale);
        pos.z = this->pos.z + (Math_SinS(Camera_GetInputDirYaw(GET_ACTIVE_CAM(play))) * scale);

        EffectSsLightning_NewLightning(play, &pos, yaw, this);

        if (Rand_ZeroOne() < 0.1f) {
            EffectSsLightning_NewLightning(play, &pos, (this->rYaw * 2) - yaw, this);
        }
    }
}

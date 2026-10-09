/*
 * File: z_eff_ss_lightning.c
 * Overlay: ovl_Effect_Ss_Lightning
 * Description: Electric Discharge
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

#define PARAMS ((EffectSsLightningInitParams*)initParamsx)

u32 EffectSsLightning_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsLightning_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsLightning_Draw(PlayState* play, u32 index, EffectSs* this);

static TexturePtr sLightningTextures[] = {
    gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex, gEffLightning4Tex,
    gEffLightning5Tex, gEffLightning6Tex, gEffLightning7Tex, gEffLightning8Tex,
};

EffectSsProfile Effect_Ss_Lightning_Profile = {
    EFFECT_SS_LIGHTNING,
    EffectSsLightning_Init,
};

static s32 sTexturesDesegmented = false;

u32 EffectSsLightning_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsLightningInitParams* initParams = PARAMS;
    s32 i;

    this->pos = initParams->pos;
    this->gfx = gEffLightningDL;
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

    if (!sTexturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sLightningTextures); i++) {
            sLightningTextures[i] = Lib_SegmentedToVirtual(sLightningTextures[i]);
        }
        sTexturesDesegmented = true;
    }

    return 1;
}

void EffectSsLightning_NewLightning(PlayState* play, Vec3f* pos, s32 yaw, EffectSs* this) {
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
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfResult;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfRot;
    MtxF mfTransBillboard;
    MtxF mfTransBillboardRot;
    Mtx* mtx;
    f32 yScale;
    s32 texIndex;
    f32 xzScale;

    OPEN_DISPS(gfxCtx);

    yScale = this->rScale * 0.01f;
    texIndex = this->rLifespan - this->life;

    if (texIndex >= ARRAY_COUNT(sLightningTextures)) {
        texIndex = ARRAY_COUNT(sLightningTextures) - 1;
    }

    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    xzScale = yScale * 0.6f;
    SkinMatrix_SetScale(&mfScale, xzScale, yScale, xzScale);
    SkinMatrix_SetRotateRPY(&mfRot, this->vec.x, this->vec.y, this->rYaw);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    SkinMatrix_MtxFMtxFMult(&mfTransBillboard, &mfRot, &mfTransBillboardRot);
    SkinMatrix_MtxFMtxFMult(&mfTransBillboardRot, &mfScale, &mfResult);

    gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        Gfx_SetupDL61_Xlu(gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, sLightningTextures[texIndex]);
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
    s32 yaw;
    f32 scale;
    s32 sign;

    if ((this->rNumBolts != 0) && ((this->life + 1) == this->rLifespan)) {
        sign = (Rand_ZeroOne() < 0.5f) ? -1 : 1;
        yaw = this->rYaw + sign * ((s32)((Rand_ZeroOne() * 3640.0f)) + 0xE38);
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

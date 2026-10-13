/*
 * File: z_eff_ss_dead_db.c
 * Overlay: ovl_Effect_Ss_Dead_Db
 * Description: Enemy Death Flames
 */

#include "z_eff_ss_dead_db.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTexIndex regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rScaleStep regs[9]
#define rTotalLife regs[11]

#define PARAMS ((EffectSsDeadDbInitParams*)initParamsx)

u32 EffectSsDeadDb_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsDeadDb_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsDeadDb_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Dead_Db_Profile = {
    EFFECT_SS_DEAD_DB,
    EffectSsDeadDb_Init,
};

static TexturePtr sTextures[] = {
    gEffEnemyDeathFlame1Tex, gEffEnemyDeathFlame2Tex,  gEffEnemyDeathFlame3Tex, gEffEnemyDeathFlame4Tex,
    gEffEnemyDeathFlame5Tex, gEffEnemyDeathFlame6Tex,  gEffEnemyDeathFlame7Tex, gEffEnemyDeathFlame8Tex,
    gEffEnemyDeathFlame9Tex, gEffEnemyDeathFlame10Tex,
};

u32 EffectSsDeadDb_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDeadDbInitParams* initParams = PARAMS;

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&this->accel, &initParams->accel);
    this->gfx = gEffEnemyDeathFlameDL;
    this->life = initParams->life;
    this->flags = 4;
    this->rScaleStep = initParams->scaleStep;
    this->rTotalLife = initParams->life;
    this->draw = EffectSsDeadDb_Draw;
    this->update = EffectSsDeadDb_Update;
    this->rScale = initParams->scale;
    this->rTexIndex = 0;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;

    return 1;
}

void EffectSsDeadDb_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    Mtx* mtx;
    f32 scale;

    OPEN_DISPS(gfxCtx);

    scale = this->rScale * 0.01f;

    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, scale);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        Gfx_SetupDL60_XluNoCD(gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        this->rPrimColorA);
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sTextures[this->rTexIndex]));
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsDeadDb_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rTexIndex = (f32)((this->rTotalLife - this->life) * 9) / this->rTotalLife;
    this->rScale += this->rScaleStep;

    this->rPrimColorR -= 10;
    if (this->rPrimColorR < 0) {
        this->rPrimColorR = 0;
    }

    this->rPrimColorG -= 10;
    if (this->rPrimColorG < 0) {
        this->rPrimColorG = 0;
    }

    this->rPrimColorB -= 10;
    if (this->rPrimColorB < 0) {
        this->rPrimColorB = 0;
    }

    this->rEnvColorR -= 10;
    if (this->rEnvColorR < 0) {
        this->rEnvColorR = 0;
    }

    this->rEnvColorG -= 10;
    if (this->rEnvColorG < 0) {
        this->rEnvColorG = 0;
    }

    this->rEnvColorB -= 10;
    if (this->rEnvColorB < 0) {
        this->rEnvColorB = 0;
    }
}

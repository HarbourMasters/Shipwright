/*
 * File: z_eff_ss_blast.c
 * Overlay: ovl_Effect_Ss_Blast
 * Description: Shockwave Effect
 */

#include "z_eff_ss_blast.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rAlphaTarget regs[8]
#define rScale regs[9]
#define rScaleStep regs[10]
#define rScaleStepDecay regs[11]

u32 EffectSsBlast_Init(GlobalContext* globalCtx, u32 index, EffectSs* this, void* initParamsx);
void EffectSsBlast_Update(GlobalContext* globalCtx, u32 index, EffectSs* this);
void EffectSsBlast_Draw(GlobalContext* globalCtx, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Blast_InitVars = {
    EFFECT_SS_BLAST,
    EffectSsBlast_Init,
};

u32 EffectSsBlast_Init(GlobalContext* globalCtx, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsBlastParams* initParams = (EffectSsBlastParams*)initParamsx;

    this->pos = initParams->pos;
    this->pos.y += 5.0f;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffShockwaveDL);
    this->life = initParams->life;
    this->draw = EffectSsBlast_Draw;
    this->update = EffectSsBlast_Update;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rEnvColorA = initParams->envColor.a;
    this->rAlphaTarget = initParams->primColor.a / initParams->life;
    this->rScale = initParams->scale;
    this->rScaleStep = initParams->scaleStep;
    this->rScaleStepDecay = initParams->sclaeStepDecay;
    return 1;
}

void EffectSsBlast_Draw(GlobalContext* globalCtx, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mf;
    s32 pad;
    f32 radius;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_blast.c", 170);

    radius = this->rScale * 0.0025f;

    func_80093D84(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
    func_800BFCB8(globalCtx, &mf, &this->pos);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, this->rPrimColorA);
    Matrix_Put(&mf);
    Matrix_Scale(radius, radius, radius, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_blast.c", 199),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_blast.c", 204);
}

void EffectSsBlast_Update(GlobalContext* globalCtx, u32 index, EffectSs* this) {
    Math_StepToS(&this->rPrimColorA, 0, this->rAlphaTarget);
    this->rScale += this->rScaleStep;

    if (this->rScaleStep != 0) {
        this->rScaleStep -= this->rScaleStepDecay;
    }
}

/*
 * File: z_eff_ss_sibuki2.c
 * Overlay: ovl_Effect_Ss_Sibuki2
 * Description: Unfinished and unused bubble effect
 */

#include "z_eff_ss_sibuki2.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rTexIdx regs[8]
#define rScale regs[9]

u32 EffectSsSibuki2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsSibuki2_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsSibuki2_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Sibuki2_InitVars = {
    EFFECT_SS_SIBUKI2,
    EffectSsSibuki2_Init,
};

u32 EffectSsSibuki2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsSibuki2InitParams* initParams = (EffectSsSibuki2InitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = 10;
    this->draw = EffectSsSibuki2_Draw;
    this->update = EffectSsSibuki2_Update;
    this->rScale = initParams->scale;
    this->rPrimColorR = 255;
    this->rPrimColorG = 255;
    this->rPrimColorB = 255;
    this->rPrimColorA = 255;
    this->rEnvColorR = 100;
    this->rEnvColorG = 100;
    this->rEnvColorB = 100;
    this->rEnvColorA = 255;
    this->rTexIdx = 0;

    return 1;
}

void EffectSsSibuki2_Draw(PlayState* play, u32 index, EffectSs* this) {
    static void* bubbleTextures[] = {
        gEffUnusedBubbles1Tex, gEffUnusedBubbles1Tex, gEffUnusedBubbles2Tex,
        gEffUnusedBubbles3Tex, gEffUnusedBubbles4Tex, gEffUnusedBubbles5Tex,
        gEffUnusedBubbles6Tex, gEffUnusedBubbles7Tex, gEffUnusedBubbles8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Gfx_SetupDL_25Opa(gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, this->rPrimColorA);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bubbleTextures[this->rTexIdx]));
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffUnusedBubblesDL));

    CLOSE_DISPS(gfxCtx);
}

void EffectSsSibuki2_Update(PlayState* play, u32 index, EffectSs* this) {
    if (this->rTexIdx < 8) {
        this->rTexIdx++;
    }
}

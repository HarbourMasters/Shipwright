/*
 * File: z_eff_ss_g_fire.c
 * Overlay: ovl_Effect_Ss_G_Fire
 * Description: Flame Footprints
 */

#include "z_eff_ss_g_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

u32 EffectSsGFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsGFire_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsGFire_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_G_Fire_InitVars = {
    EFFECT_SS_G_FIRE,
    EffectSsGFire_Init,
};

u32 EffectSsGFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGFireInitParams* initParams = (EffectSsGFireInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    this->velocity = this->accel = zeroVec;
    this->pos = initParams->pos;
    this->draw = EffectSsGFire_Draw;
    this->update = EffectSsGFire_Update;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffFireFootprintDL);
    this->life = 8;
    this->flags = 0;
    this->rgScale = 200;
    this->rgTexIdx = 0;
    this->rgTexIdxStep = 50;
    this->rgPrimColorR = 255;
    this->rgPrimColorG = 220;
    this->rgPrimColorB = 80;
    this->rgPrimColorA = 255;
    this->rgEnvColorR = 130;
    this->rgEnvColorG = 30;
    this->rgEnvColorB = 0;
    this->rgEnvColorA = 0;

    return 1;
}

void EffectSsGFire_Draw(PlayState* play, u32 index, EffectSs* this) {
    void* fireFootprintTextures[] = {
        gEffFireFootprint1Tex, gEffFireFootprint2Tex, gEffFireFootprint3Tex, gEffFireFootprint4Tex,
        gEffFireFootprint5Tex, gEffFireFootprint6Tex, gEffFireFootprint7Tex, gEffFireFootprint8Tex,
    };
    s16 texIdx = (this->rgTexIdx / 100) % 7;

    EffectSs_DrawGEffect(play, this, fireFootprintTextures[texIdx]);
}

void EffectSsGFire_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rgTexIdx += this->rgTexIdxStep;
}

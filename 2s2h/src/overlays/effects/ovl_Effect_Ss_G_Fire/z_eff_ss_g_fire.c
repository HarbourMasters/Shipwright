/*
 * File: z_eff_ss_g_fire.c
 * Overlay: ovl_Effect_Ss_G_Fire
 * Description: Flame Footprints
 */

#include "z_eff_ss_g_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define PARAMS ((EffectSsGFireInitParams*)initParamsx)

u32 EffectSsGFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsGFire_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsGFire_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_G_Fire_Profile = {
    EFFECT_SS_G_FIRE,
    EffectSsGFire_Init,
};

u32 EffectSsGFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGFireInitParams* initParams = PARAMS;

    Math_Vec3f_Copy(&this->velocity, &gZeroVec3f);
    Math_Vec3f_Copy(&this->accel, &gZeroVec3f);
    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    this->draw = EffectSsGFire_Draw;
    this->update = EffectSsGFire_Update;
    this->gfx = gEffFireFootprintDL;
    this->life = 8;
    this->flags = 0;
    this->rgScale = 200;
    this->rgTexIndex = 0;
    this->rgTexIndexStep = 50;
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
    TexturePtr fireFootprintTextures[] = {
        gEffFireFootprint1Tex, gEffFireFootprint2Tex, gEffFireFootprint3Tex, gEffFireFootprint4Tex,
        gEffFireFootprint5Tex, gEffFireFootprint6Tex, gEffFireFootprint7Tex, gEffFireFootprint8Tex,
    };
    s16 texIndex = (this->rgTexIndex / 100) % 7;

    EffectSs_DrawGEffect(play, this, fireFootprintTextures[texIndex]);
}

void EffectSsGFire_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rgTexIndex += this->rgTexIndexStep;
}

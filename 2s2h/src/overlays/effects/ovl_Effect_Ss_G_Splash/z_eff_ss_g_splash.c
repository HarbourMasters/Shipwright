/*
 * File: z_eff_ss_g_splash.c
 * Overlay: ovl_Effect_Ss_G_Splash
 * Description:
 */

#include "z_eff_ss_g_splash.h"
#include "objects/gameplay_keep/gameplay_keep.h"

//! @bug the reuse of regs[11] means that EffectSs_DrawGEffect will treat the type as an object bank index
// this ends up having no effect because the texture provided does not use segment 6
#define rType regs[11]

#define PARAMS ((EffectSsGSplashInitParams*)initParamsx)

u32 EffectSsGSplash_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsGSplash_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsGSplash_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_G_Splash_Profile = {
    EFFECT_SS_G_SPLASH,
    EffectSsGSplash_Init,
};

static TexturePtr sWaterSplashTextures[] = {
    gEffWaterSplash1Tex, gEffWaterSplash2Tex, gEffWaterSplash3Tex, gEffWaterSplash4Tex,
    gEffWaterSplash5Tex, gEffWaterSplash6Tex, gEffWaterSplash7Tex, gEffWaterSplash8Tex,
};

u32 EffectSsGSplash_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGSplashInitParams* initParams = PARAMS;

    Math_Vec3f_Copy(&this->velocity, &gZeroVec3f);
    Math_Vec3f_Copy(&this->accel, &gZeroVec3f);
    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    this->draw = EffectSsGSplash_Draw;
    this->update = EffectSsGSplash_Update;

    if (initParams->scale == 0) {
        initParams->scale = 600;
    }

    this->gfx = gEffWaterSplashDL;
    this->life = 8;
    this->rgScale = initParams->scale;
    this->rgTexIndex = 0;
    this->rgTexIndexStep = 100;

    if (initParams->customColor) {
        this->rgPrimColorR = initParams->primColor.r;
        this->rgPrimColorG = initParams->primColor.g;
        this->rgPrimColorB = initParams->primColor.b;
        this->rgPrimColorA = initParams->primColor.a;
        this->rgEnvColorR = initParams->envColor.r;
        this->rgEnvColorG = initParams->envColor.g;
        this->rgEnvColorB = initParams->envColor.b;
        this->rgEnvColorA = initParams->envColor.a;
        this->rType = initParams->type;
    } else {
        switch (initParams->type) {
            case EFFSSGSPLASH_TYPE_0:
                this->rgPrimColorR = 255;
                this->rgPrimColorG = 255;
                this->rgPrimColorB = 255;
                this->rgPrimColorA = 200;
                this->rgEnvColorR = 255;
                this->rgEnvColorG = 255;
                this->rgEnvColorB = 255;
                this->rgEnvColorA = 200;
                this->rType = EFFSSGSPLASH_TYPE_0;
                break;
            case EFFSSGSPLASH_TYPE_1:
                this->rgPrimColorR = 255;
                this->rgPrimColorG = 255;
                this->rgPrimColorB = 255;
                this->rgPrimColorA = 255;
                this->rgEnvColorR = 255;
                this->rgEnvColorG = 255;
                this->rgEnvColorB = 255;
                this->rgEnvColorA = 255;
                this->rType = EFFSSGSPLASH_TYPE_1;
                break;
            case EFFSSGSPLASH_TYPE_2:
                this->rgPrimColorR = 255;
                this->rgPrimColorG = 255;
                this->rgPrimColorB = 255;
                this->rgPrimColorA = 200;
                this->rgEnvColorR = 255;
                this->rgEnvColorG = 255;
                this->rgEnvColorB = 255;
                this->rgEnvColorA = 200;
                this->rType = EFFSSGSPLASH_TYPE_2;
                break;
        }
    }
    return 1;
}

void EffectSsGSplash_Draw(PlayState* play, u32 index, EffectSs* this) {
    s16 texIndex;

    switch (this->rType) {
        case EFFSSGSPLASH_TYPE_0:
            texIndex = this->rgTexIndex / 100;
            if (texIndex > 7) {
                texIndex = 7;
            }
            EffectSs_DrawGEffect(play, this, sWaterSplashTextures[texIndex]);
            break;

        case EFFSSGSPLASH_TYPE_1:
            texIndex = this->rgTexIndex / 100;
            if (texIndex > 7) {
                texIndex = 7;
            }
            EffectSs_DrawGEffect(play, this, sWaterSplashTextures[texIndex]);
            break;

        case EFFSSGSPLASH_TYPE_2:
            texIndex = this->rgTexIndex / 100;
            if (texIndex > 7) {
                texIndex = 7;
            }
            EffectSs_DrawGEffect(play, this, sWaterSplashTextures[texIndex]);
            break;
    }
}

void EffectSsGSplash_Update(PlayState* play, u32 index, EffectSs* this) {
    Vec3f newSplashPos;

    if ((this->rType == EFFSSGSPLASH_TYPE_1) && (this->life == 5)) {
        newSplashPos = this->pos;
        newSplashPos.y += ((this->rgScale * 20) * 0.002f);
        EffectSsGSplash_Spawn(play, &newSplashPos, 0, 0, 2, this->rgScale / 2);
    }

    this->rgTexIndex += this->rgTexIndexStep;
}

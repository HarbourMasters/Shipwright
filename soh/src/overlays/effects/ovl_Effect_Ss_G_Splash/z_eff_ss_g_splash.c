/*
 * File: z_eff_ss_g_splash.c
 * Overlay: ovl_Effect_Ss_G_Splash
 * Description: Splash
 */

#include "z_eff_ss_g_splash.h"
#include "objects/gameplay_keep/gameplay_keep.h"

//! @bug the reuse of regs[11] means that EffectSs_DrawGEffect will treat the type as an object bank index
// this ends up having no effect because the texture provided does not use segment 6
#define rType regs[11]

u32 EffectSsGSplash_Init(PlayState* play, u32 index, EffectSs* this, void* initParams);
void EffectSsGSplash_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsGSplash_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_G_Splash_InitVars = {
    EFFECT_SS_G_SPLASH,
    EffectSsGSplash_Init,
};

u32 EffectSsGSplash_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGSplashInitParams* initParams = (EffectSsGSplashInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    this->velocity = this->accel = zeroVec;
    this->pos = initParams->pos;
    this->draw = EffectSsGSplash_Draw;
    this->update = EffectSsGSplash_Update;

    if (initParams->scale == 0) {
        initParams->scale = 600;
    }

    this->gfx = SEGMENTED_TO_VIRTUAL(gEffWaterSplashDL);
    this->life = 8;
    this->rgScale = initParams->scale;
    this->rgTexIdx = 0;
    this->rgTexIdxStep = 100;

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
            case 0:
                this->rgPrimColorR = 255;
                this->rgPrimColorG = 255;
                this->rgPrimColorB = 255;
                this->rgPrimColorA = 200;
                this->rgEnvColorR = 255;
                this->rgEnvColorG = 255;
                this->rgEnvColorB = 255;
                this->rgEnvColorA = 200;
                this->rType = 0;
                break;
            case 1:
                this->rgPrimColorR = 255;
                this->rgPrimColorG = 255;
                this->rgPrimColorB = 255;
                this->rgPrimColorA = 255;
                this->rgEnvColorR = 255;
                this->rgEnvColorG = 255;
                this->rgEnvColorB = 255;
                this->rgEnvColorA = 255;
                this->rType = 1;
                break;
            case 2:
                this->rgPrimColorR = 255;
                this->rgPrimColorG = 255;
                this->rgPrimColorB = 255;
                this->rgPrimColorA = 200;
                this->rgEnvColorR = 255;
                this->rgEnvColorG = 255;
                this->rgEnvColorB = 255;
                this->rgEnvColorA = 200;
                this->rType = 2;
                break;
        }
    }
    return 1;
}

void EffectSsGSplash_Draw(PlayState* play, u32 index, EffectSs* this) {
    static void* waterSplashTextures[] = {
        gEffWaterSplash1Tex, gEffWaterSplash2Tex, gEffWaterSplash3Tex, gEffWaterSplash4Tex,
        gEffWaterSplash5Tex, gEffWaterSplash6Tex, gEffWaterSplash7Tex, gEffWaterSplash8Tex,
    };
    s16 texIdx;

    switch (this->rType) {
        case 0:
            texIdx = this->rgTexIdx / 100;
            if (texIdx > 7) {
                texIdx = 7;
            }
            EffectSs_DrawGEffect(play, this, waterSplashTextures[texIdx]);
            break;

        case 1:
            texIdx = this->rgTexIdx / 100;
            if (texIdx > 7) {
                texIdx = 7;
            }
            EffectSs_DrawGEffect(play, this, waterSplashTextures[texIdx]);
            break;

        case 2:
            texIdx = this->rgTexIdx / 100;
            if (texIdx > 7) {
                texIdx = 7;
            }
            EffectSs_DrawGEffect(play, this, waterSplashTextures[texIdx]);
            break;

        default:
            break;
    }
}

void EffectSsGSplash_Update(PlayState* play, u32 index, EffectSs* this) {
    Vec3f newSplashPos;

    if ((this->rType == 1) && (this->life == 5)) {
        newSplashPos = this->pos;
        newSplashPos.y += ((this->rgScale * 20) * 0.002f);
        EffectSsGSplash_Spawn(play, &newSplashPos, 0, 0, 2, this->rgScale / 2);
    }

    this->rgTexIdx += this->rgTexIdxStep;
}

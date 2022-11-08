/*
 * File: z_eff_ss_g_magma.c
 * Overlay: ovl_Effect_Ss_G_Magma
 * Description: Magma Bubbles
 */

#include "z_eff_ss_g_magma.h"
#include "objects/gameplay_keep/gameplay_keep.h"

u32 EffectSsGMagma_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsGMagma_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsGMagma_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_G_Magma_InitVars = {
    EFFECT_SS_G_MAGMA,
    EffectSsGMagma_Init,
};

u32 EffectSsGMagma_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsGMagmaInitParams* initParams = (EffectSsGMagmaInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    this->velocity = this->accel = zeroVec;
    this->pos = initParams->pos;
    this->draw = EffectSsGMagma_Draw;
    this->update = EffectSsGMagma_Update;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffMagmaBubbleDL);
    this->life = 16;
    this->rgScale = (s16)(Rand_ZeroOne() * 100.0f) + 200;
    this->rgTexIdx = 0;
    this->rgTexIdxStep = 50;
    this->rgPrimColorR = 255;
    this->rgPrimColorG = 255;
    this->rgPrimColorB = 0;
    this->rgPrimColorA = 255;
    this->rgEnvColorR = 255;
    this->rgEnvColorG = 0;
    this->rgEnvColorB = 0;
    this->rgEnvColorA = 0;

    return 1;
}

static void* sTextures[] = {
    gEffMagmaBubble1Tex, gEffMagmaBubble2Tex, gEffMagmaBubble3Tex, gEffMagmaBubble4Tex,
    gEffMagmaBubble5Tex, gEffMagmaBubble6Tex, gEffMagmaBubble7Tex, gEffMagmaBubble8Tex,
};

void EffectSsGMagma_Draw(PlayState* play, u32 index, EffectSs* this) {
    s16 texIdx = this->rgTexIdx / 100;

    if (texIdx > 7) {
        texIdx = 7;
    }

    EffectSs_DrawGEffect(play, this, sTextures[texIdx]);
}

void EffectSsGMagma_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rgTexIdx += this->rgTexIdxStep;
}

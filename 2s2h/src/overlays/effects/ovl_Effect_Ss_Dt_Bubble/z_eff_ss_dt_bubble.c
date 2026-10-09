/*
 * File: z_eff_ss_dt_bubble.c
 * Overlay: ovl_Effect_Ss_Dt_Bubble
 * Description: Non-Water Bubbles (a random mix of translucent and opaque)
 */

#include "z_eff_ss_dt_bubble.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rRandXZ regs[8]
#define rScale regs[9]
#define rLifespan regs[10]

#define PARAMS ((EffectSsDtBubbleInitParams*)initParamsx)

u32 EffectSsDtBubble_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsDtBubble_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsDtBubble_Draw(PlayState* play, u32 index, EffectSs* this);

static Color_RGBA8 sPrimColors[] = {
    { 255, 255, 100, 255 },
    { 150, 255, 255, 255 },
    { 100, 255, 255, 255 },
    { 255, 255, 255, 255 },
};

static Color_RGBA8 sEnvColors[] = {
    { 170, 0, 0, 255 },
    { 0, 100, 0, 255 },
    { 0, 0, 255, 255 },
    { 150, 150, 150, 0 },
};

EffectSsProfile Effect_Ss_Dt_Bubble_Profile = {
    EFFECT_SS_DT_BUBBLE,
    EffectSsDtBubble_Init,
};

u32 EffectSsDtBubble_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDtBubbleInitParams* initParams = PARAMS;

    {
        TexturePtr tex = (Rand_ZeroOne() < 0.5f) ? gEffBubble1Tex : gEffBubble2Tex;

        this->gfx = (void*)OS_K0_TO_PHYSICAL(SEGMENTED_TO_K0(tex));
    }

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&this->accel, &initParams->accel);
    this->life = initParams->life;

    if (!initParams->customColor) {
        this->rPrimColorR = sPrimColors[initParams->colorProfile].r;
        this->rPrimColorG = sPrimColors[initParams->colorProfile].g;
        this->rPrimColorB = sPrimColors[initParams->colorProfile].b;
        this->rPrimColorA = sPrimColors[initParams->colorProfile].a;
        this->rEnvColorR = sEnvColors[initParams->colorProfile].r;
        this->rEnvColorG = sEnvColors[initParams->colorProfile].g;
        this->rEnvColorB = sEnvColors[initParams->colorProfile].b;
        this->rEnvColorA = sEnvColors[initParams->colorProfile].a;
    } else {
        this->rPrimColorR = initParams->primColor.r;
        this->rPrimColorG = initParams->primColor.g;
        this->rPrimColorB = initParams->primColor.b;
        this->rPrimColorA = initParams->primColor.a;
        this->rEnvColorR = initParams->envColor.r;
        this->rEnvColorG = initParams->envColor.g;
        this->rEnvColorB = initParams->envColor.b;
        this->rEnvColorA = initParams->envColor.a;
    }

    this->rRandXZ = initParams->randXZ;
    this->rScale = initParams->scale;
    this->rLifespan = initParams->life;
    this->draw = EffectSsDtBubble_Draw;
    this->update = EffectSsDtBubble_Update;

    return 1;
}

void EffectSsDtBubble_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale;

    OPEN_DISPS(gfxCtx);

    scale = this->rScale * 0.004f;
    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
    Gfx_SetupDL25_Opa(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                    (this->rPrimColorA * this->life) / this->rLifespan);
    gDPSetEnvColor(POLY_OPA_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB,
                   (this->rEnvColorA * this->life) / this->rLifespan);
    gSPSegment(POLY_OPA_DISP++, 0x08, this->gfx);
    gSPDisplayList(POLY_OPA_DISP++, gEffBubbleDL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsDtBubble_Update(PlayState* play, u32 index, EffectSs* this) {
    if (this->rRandXZ == true) {
        this->pos.x += (Rand_ZeroOne() * 2.0f) - 1.0f;
        this->pos.z += (Rand_ZeroOne() * 2.0f) - 1.0f;
    }
}

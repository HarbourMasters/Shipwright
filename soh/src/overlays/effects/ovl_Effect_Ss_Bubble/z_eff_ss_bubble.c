/*
 * File: z_eff_ss_bubble.c
 * Overlay: ovl_Effect_Ss_Bubble
 * Description:
 */

#include "z_eff_ss_bubble.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]

u32 EffectSsBubble_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsBubble_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsBubble_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Bubble_InitVars = {
    EFFECT_SS_BUBBLE,
    EffectSsBubble_Init,
};

u32 EffectSsBubble_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsBubbleInitParams* initParams = (EffectSsBubbleInitParams*)initParamsx;

    //! @bug Rand_ZeroOne in the macro means a random number is generated for both parts of the macro.
    // In the base game this works out because both addresses are segment 4, but it may break if
    // the addresses were changed to refer to different segments
    this->gfx = SEGMENTED_TO_VIRTUAL(Rand_ZeroOne() < 0.5f ? gEffBubble1Tex : gEffBubble2Tex);
    this->pos.x = ((Rand_ZeroOne() - 0.5f) * initParams->xzPosRandScale) + initParams->pos.x;
    this->pos.y = (((Rand_ZeroOne() - 0.5f) * initParams->yPosRandScale) + initParams->yPosOffset) + initParams->pos.y;
    this->pos.z = ((Rand_ZeroOne() - 0.5f) * initParams->xzPosRandScale) + initParams->pos.z;
    Math_Vec3f_Copy(&this->vec, &this->pos);
    this->life = 1;
    this->rScale = (((Rand_ZeroOne() * 0.5f) + 1.0f) * initParams->scale) * 100;
    this->draw = EffectSsBubble_Draw;
    this->update = EffectSsBubble_Update;

    return 1;
}

void EffectSsBubble_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Gfx_SetupDL_25Opa(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 150, 150, 150, 0);
    gSPSegment(POLY_OPA_DISP++, 0x08, this->gfx);
    gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gEffBubbleDL));

    CLOSE_DISPS(gfxCtx);
}

void EffectSsBubble_Update(PlayState* play, u32 index, EffectSs* this) {
    WaterBox* waterBox;
    f32 waterSurfaceY;
    Vec3f ripplePos;

    waterSurfaceY = this->pos.y;

    // kill bubble if it's out of range of a water box
    if (!WaterBox_GetSurface1(play, &play->colCtx, this->pos.x, this->pos.z, &waterSurfaceY, &waterBox)) {
        this->life = -1;
        return;
    }

    if (waterSurfaceY < this->pos.y) {
        ripplePos.x = this->pos.x;
        ripplePos.y = waterSurfaceY;
        ripplePos.z = this->pos.z;
        EffectSsGRipple_Spawn(play, &ripplePos, 0, 80, 0);
        this->life = -1;
    } else {
        this->life++;
        this->pos.x = ((Rand_ZeroOne() * 0.5f) - 0.25f) + this->vec.x;
        this->accel.y = (Rand_ZeroOne() - 0.3f) * 0.2f;
        this->pos.z = ((Rand_ZeroOne() * 0.5f) - 0.25f) + this->vec.z;
    }
}

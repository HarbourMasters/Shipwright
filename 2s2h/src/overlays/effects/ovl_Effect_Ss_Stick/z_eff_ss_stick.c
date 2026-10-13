/*
 * File: z_eff_ss_stick.c
 * Overlay: ovl_Effect_Ss_Stick
 * Description: Breaking Deku Stick Fragment
 */

#include "z_eff_ss_stick.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rObjectSlot regs[0]
#define rYaw regs[1]

#define PARAMS ((EffectSsStickInitParams*)initParamsx)

u32 EffectSsStick_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsStick_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsStick_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Stick_Profile = {
    EFFECT_SS_STICK,
    EffectSsStick_Init,
};

u32 EffectSsStick_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsStickInitParams* initParams = PARAMS;
    Vec3f pos;

    this->rObjectSlot = Object_GetSlot(&play->objectCtx, GAMEPLAY_KEEP);
    pos = initParams->pos;
    this->pos = pos;
    this->vec = pos;
    this->rYaw = initParams->yaw;
    this->velocity.x = Math_SinS(initParams->yaw) * 6.0f;
    this->velocity.z = Math_CosS(initParams->yaw) * 6.0f;
    this->life = 20;
    this->draw = EffectSsStick_Draw;
    this->update = EffectSsStick_Update;
    this->velocity.y = 26.0f;
    this->accel.y = -4.0f;
    return 1;
}

void EffectSsStick_Draw(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(0.01f, 0.0025f, 0.01f, MTXMODE_APPLY);
    Matrix_RotateZYX(0, this->rYaw, 0, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
    Gfx_SetupDL25_Opa(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->rObjectSlot].segment);
    gSPSegment(POLY_OPA_DISP++, 0x0C, gCullBackDList);
    gSPDisplayList(POLY_OPA_DISP++, gDekuStickDL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsStick_Update(PlayState* play, u32 index, EffectSs* this) {
}

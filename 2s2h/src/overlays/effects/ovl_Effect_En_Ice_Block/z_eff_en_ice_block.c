/*
 * File: z_eff_en_ice_block.c
 * Overlay: ovl_Effect_En_Ice_Block
 * Description: Ice Shard (Pentagon shaped) thrown off of the Obj_Iceblock actor during growth animation
 * Object Requirement: OBJECT_ICE_BLOCK
 */

#include "z_eff_en_ice_block.h"
#include "objects/object_ice_block/object_ice_block.h"

#define PARAMS ((EffectEnIceBlockInitParams*)initParamsx)

u32 EffectEnIceBlock_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectEnIceBlock_Update(PlayState* play, u32 index, EffectSs* this);
void EffectEnIceBlock_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_En_Ice_Block_Profile = {
    EFFECT_EN_ICE_BLOCK,
    EffectEnIceBlock_Init,
};

#define rObjectSlot regs[0]
#define rScale regs[1]
#define rRot regs[2]
#define rRotVel regs[3]

u32 EffectEnIceBlock_CheckIceBlockObject(EffectSs* this, PlayState* play) {
    if (((this->rObjectSlot = Object_GetSlot(&play->objectCtx, OBJECT_ICE_BLOCK)) <= OBJECT_SLOT_NONE) ||
        (!Object_IsLoaded(&play->objectCtx, this->rObjectSlot))) {
        this->life = -1;
        this->draw = NULL;
        return false;
    } else {
        return true;
    }
}

u32 EffectEnIceBlock_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectEnIceBlockInitParams* params = PARAMS;

    Math_Vec3f_Copy(&this->pos, &params->pos);
    Math_Vec3f_Copy(&this->velocity, &params->velocity);
    Math_Vec3f_Copy(&this->accel, &params->accel);
    this->rScale = params->scale;
    this->rRot = Rand_ZeroOne() * 65535.0f;
    this->rRotVel = (s32)(Rand_ZeroOne() * 512.0f) + 0x400;
    this->life = 40;

    this->draw = EffectEnIceBlock_Draw;
    this->update = EffectEnIceBlock_Update;
    EffectEnIceBlock_CheckIceBlockObject(this, play);

    return 1;
}

void EffectEnIceBlock_Draw(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    if (EffectEnIceBlock_CheckIceBlockObject(this, play)) {
        f32 scale = this->rScale * 0.1f * 0.01f;

        OPEN_DISPS(gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x06,
                   play->objectCtx.slots[this->rObjectSlot].segment); // object: OBJECT_ICE_BLOCK

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
        Matrix_RotateZS(this->rRot, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);

        gSPDisplayList(POLY_XLU_DISP++, &gIceBlockShardEffectDL);

        CLOSE_DISPS(gfxCtx);
    }
}

void EffectEnIceBlock_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rRot += this->rRotVel;
    EffectEnIceBlock_CheckIceBlockObject(this, play);
}

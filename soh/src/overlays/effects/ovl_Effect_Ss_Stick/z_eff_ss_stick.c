/*
 * File: z_eff_ss_stick.c
 * Overlay: ovl_Effect_Ss_Stick
 * Description: Broken stick as child, broken sword as adult
 */

#include "z_eff_ss_stick.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"

#define rObjBankIdx regs[0]
#define rYaw regs[1]

u32 EffectSsStick_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsStick_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsStick_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Stick_InitVars = {
    EFFECT_SS_STICK,
    EffectSsStick_Init,
};

typedef struct {
    /* 0x00 */ s16 objectID;
    /* 0x04 */ Gfx* displayList;
} StickDrawInfo;

u32 EffectSsStick_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    StickDrawInfo drawInfo[] = {
        { OBJECT_LINK_BOY, gLinkAdultBrokenGiantsKnifeBladeDL }, // adult, broken sword
        { OBJECT_LINK_CHILD, gLinkChildLinkDekuStickDL },        // child, broken stick
    };
    StickDrawInfo* ageInfoEntry = gSaveContext.linkAge + drawInfo;
    EffectSsStickInitParams* initParams = (EffectSsStickInitParams*)initParamsx;

    this->rObjBankIdx = Object_GetIndex(&play->objectCtx, ageInfoEntry->objectID);
    this->gfx = ageInfoEntry->displayList;
    this->vec = this->pos = initParams->pos;
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
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);

    if (!LINK_IS_ADULT) {
        Matrix_Scale(0.01f, 0.0025f, 0.01f, MTXMODE_APPLY);
        Matrix_RotateZYX(0, this->rYaw, 0, MTXMODE_APPLY);
    } else {
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        Matrix_RotateZYX(0, this->rYaw, play->state.frames * 10000, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D18(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[this->rObjBankIdx].segment);
    gSPSegment(POLY_OPA_DISP++, 0x0C, gCullBackDList);
    gSPDisplayList(POLY_OPA_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsStick_Update(PlayState* play, u32 index, EffectSs* this) {
}

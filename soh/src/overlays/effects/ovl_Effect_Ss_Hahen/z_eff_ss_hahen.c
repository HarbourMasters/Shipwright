/*
 * File: z_eff_ss_hahen.c
 * Overlay: ovl_Effect_Ss_Hahen
 * Description: Fragments
 */

#include "z_eff_ss_hahen.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPitch regs[0]
#define rYaw regs[1]
#define rUnused regs[2]
#define rScale regs[3]
#define rObjId regs[4]
#define rObjBankIdx regs[5]
#define rMinLife regs[6]

u32 EffectSsHahen_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsHahen_DrawGray(PlayState* play, u32 index, EffectSs* this);
void EffectSsHahen_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsHahen_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Hahen_InitVars = {
    EFFECT_SS_HAHEN,
    EffectSsHahen_Init,
};

void EffectSsHahen_CheckForObject(EffectSs* this, PlayState* play) {
    if (((this->rObjBankIdx = Object_GetIndex(&play->objectCtx, this->rObjId)) < 0) ||
        !Object_IsLoaded(&play->objectCtx, this->rObjBankIdx)) {
        this->life = -1;
        this->draw = NULL;
    }
}

u32 EffectSsHahen_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsHahenInitParams* initParams = (EffectSsHahenInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = 200;

    if (initParams->dList != NULL) {
        this->gfx = initParams->dList;
        this->rObjId = initParams->objId;
        EffectSsHahen_CheckForObject(this, play);
    } else {
        this->gfx = SEGMENTED_TO_VIRTUAL(gEffFragments1DL);
        this->rObjId = -1;
    }

    if ((this->rObjId == OBJECT_HAKA_OBJECTS) && (this->gfx == gEffFragments2DL)) {
        this->draw = EffectSsHahen_DrawGray;
    } else {
        this->draw = EffectSsHahen_Draw;
    }

    this->update = EffectSsHahen_Update;
    this->rUnused = initParams->unused;
    this->rScale = initParams->scale;
    this->rPitch = Rand_ZeroOne() * 314.0f;
    this->rYaw = Rand_ZeroOne() * 314.0f;
    this->rMinLife = 200 - initParams->life;

    return 1;
}

void EffectSsHahen_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale = this->rScale * 0.001f;

    OPEN_DISPS(gfxCtx);

    if (this->rObjId != -1) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[this->rObjBankIdx].segment);
    }

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_RotateY(this->rYaw * 0.01f, MTXMODE_APPLY);
    Matrix_RotateX(this->rPitch * 0.01f, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx);
}

// in the original game this function is hardcoded to be used only by the skull pots in Shadow Temple
void EffectSsHahen_DrawGray(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale = this->rScale * 0.001f;

    OPEN_DISPS(gfxCtx);

    if (this->rObjId != -1) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[this->rObjBankIdx].segment);
    }

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_RotateY(this->rYaw * 0.01f, MTXMODE_APPLY);
    Matrix_RotateX(this->rPitch * 0.01f, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gDPSetCombineLERP(POLY_OPA_DISP++, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0x0, 0x01, 100, 100, 120, 255);
    gSPDisplayList(POLY_OPA_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsHahen_Update(PlayState* play, u32 index, EffectSs* this) {
    Player* player = GET_PLAYER(play);

    this->rPitch += 55;
    this->rYaw += 10;

    if ((this->pos.y <= player->actor.floorHeight) && (this->life < this->rMinLife)) {
        this->life = 0;
    }

    if (this->rObjId != -1) {
        EffectSsHahen_CheckForObject(this, play);
    }
}

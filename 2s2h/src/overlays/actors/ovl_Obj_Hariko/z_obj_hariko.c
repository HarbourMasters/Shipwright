/*
 * File: z_obj_hariko.c
 * Overlay: ovl_Obj_Hariko
 * Description: Little Cow Statue Head
 */

#include "z_obj_hariko.h"
#include "z64quake.h"
#include "assets/objects/object_hariko/object_hariko.h"

#define FLAGS (ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void ObjHariko_Init(Actor* thisx, PlayState* play);
void ObjHariko_Destroy(Actor* thisx, PlayState* play);
void ObjHariko_Update(Actor* thisx, PlayState* play);
void ObjHariko_Draw(Actor* thisx, PlayState* play);

void ObjHariko_SetupWait(ObjHariko* this);
void ObjHariko_Wait(ObjHariko* this, PlayState* play);
void ObjHariko_SetupBobHead(ObjHariko* this);
void ObjHariko_BobHead(ObjHariko* this, PlayState* play);
void ObjHariko_CheckForQuakes(ObjHariko* this);

ActorProfile Obj_Hariko_Profile = {
    /**/ ACTOR_OBJ_HARIKO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HARIKO,
    /**/ sizeof(ObjHariko),
    /**/ ObjHariko_Init,
    /**/ ObjHariko_Destroy,
    /**/ ObjHariko_Update,
    /**/ ObjHariko_Draw,
};

void ObjHariko_Init(Actor* thisx, PlayState* play) {
    ObjHariko* this = (ObjHariko*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->headRot.x = 0;
    this->headRot.y = 0;
    this->headRot.z = 0;
    this->headOffset = 0;
    this->bobbleStep = 0.0f;
    ObjHariko_SetupWait(this);
}

void ObjHariko_Destroy(Actor* thisx, PlayState* play) {
}

void ObjHariko_SetupWait(ObjHariko* this) {
    this->actionFunc = ObjHariko_Wait;
}

void ObjHariko_Wait(ObjHariko* this, PlayState* play) {
}

void ObjHariko_SetupBobHead(ObjHariko* this) {
    this->bobbleStep = 2730.0f;
    this->unk154 = 0;
    this->actionFunc = ObjHariko_BobHead;
}

void ObjHariko_BobHead(ObjHariko* this, PlayState* play) {
    this->headOffset += 0x1555;
    this->headRot.x = Math_SinS(this->headOffset) * this->bobbleStep;
    this->headRot.y = Math_CosS(this->headOffset) * this->bobbleStep;
    Math_SmoothStepToF(&this->bobbleStep, 0, 0.5f, 18.0f, 18.0f);
    if (this->bobbleStep < 182.0f) {
        ObjHariko_SetupWait(this);
    }
}

void ObjHariko_CheckForQuakes(ObjHariko* this) {
    if (Quake_GetNumActiveQuakes() != 0) {
        ObjHariko_SetupBobHead(this);
    }
}

void ObjHariko_Update(Actor* thisx, PlayState* play) {
    ObjHariko* this = (ObjHariko*)thisx;

    this->actionFunc(this, play);
    ObjHariko_CheckForQuakes(this);
}

void ObjHariko_Draw(Actor* thisx, PlayState* play) {
    ObjHariko* this = (ObjHariko*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    Matrix_Push();
    Matrix_RotateXS(this->headRot.x, MTXMODE_APPLY);
    Matrix_RotateYS(this->headRot.y, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gHarikoBodyDL);
    gSPDisplayList(POLY_OPA_DISP++, gHarikoFaceDL);

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

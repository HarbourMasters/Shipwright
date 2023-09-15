/*
 * File: z_bg_ingate.c
 * Overlay: ovl_Bg_Ingate
 * Description: Ingo's Gates (Lon Lon Ranch)
 */

#include "z_bg_ingate.h"
#include "objects/object_ingate/object_ingate.h"

#define FLAGS 0

void BgInGate_Init(Actor* thisx, PlayState* play);
void BgInGate_Destroy(Actor* thisx, PlayState* play);
void BgInGate_Update(Actor* thisx, PlayState* play);
void BgInGate_Draw(Actor* thisx, PlayState* play);

void func_80892890(BgInGate* this, PlayState* play);
void BgInGate_DoNothing(BgInGate* this, PlayState* play);

const ActorInit Bg_Ingate_InitVars = {
    ACTOR_BG_INGATE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_INGATE,
    sizeof(BgInGate),
    (ActorFunc)BgInGate_Init,
    (ActorFunc)BgInGate_Destroy,
    (ActorFunc)BgInGate_Update,
    (ActorFunc)BgInGate_Draw,
    NULL,
};

void BgInGate_SetupAction(BgInGate* this, BgInGateActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgInGate_Init(Actor* thisx, PlayState* play) {
    BgInGate* this = (BgInGate*)thisx;

    s32 pad;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gIngoGateCol, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if ((play->sceneNum != SCENE_LON_LON_RANCH || !LINK_IS_ADULT) ||
        (((Flags_GetEventChkInf(EVENTCHKINF_EPONA_OBTAINED))) && (gSaveContext.cutsceneIndex != 0xFFF0))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    Actor_SetScale(&this->dyna.actor, 0.1f);
    if (((this->dyna.actor.params & 1) != 0) && ((gSaveContext.eventInf[0] & 0xF) == 6)) {
        play->csCtx.frames = 0;
        BgInGate_SetupAction(this, func_80892890);
    } else {
        BgInGate_SetupAction(this, BgInGate_DoNothing);
    }
}

void BgInGate_Destroy(Actor* thisx, PlayState* play) {
    BgInGate* this = (BgInGate*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80892890(BgInGate* this, PlayState* play) {
    s32 phi0;
    s16 phi1;
    s16 csFrames;

    if (play->csCtx.frames >= 50) {
        phi0 = 0x4000;
        if ((this->dyna.actor.params & 2) == 0) {
            phi0 = -0x4000;
        }
        this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y + phi0;
        BgInGate_SetupAction(this, BgInGate_DoNothing);
    } else if (play->csCtx.frames >= 10) {
        csFrames = play->csCtx.frames - 10;
        csFrames *= 400;
        phi1 = csFrames;
        if (csFrames > 0x4000) {
            csFrames = 0x4000;
        }
        csFrames = (Math_SinS(csFrames) * 16384.0f);
        phi1 = csFrames;
        if ((this->dyna.actor.params & 2) == 0) {
            phi1 = -phi1;
        }
        this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y + phi1;
    }
}

void BgInGate_DoNothing(BgInGate* this, PlayState* play) {
}

void BgInGate_Update(Actor* thisx, PlayState* play) {
    BgInGate* this = (BgInGate*)thisx;

    this->actionFunc(this, play);
}

void BgInGate_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gIngoGateDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

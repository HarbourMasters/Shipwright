/*
 * File: z_bg_haka_curtain.c
 * Overlay: ovl_Bg_Haka_Curtain
 * Description: Curtain That Lifts to Reveal Flat's Tomb
 */

#include "z_bg_haka_curtain.h"
#include "objects/object_haka_obj/object_haka_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgHakaCurtain_Init(Actor* thisx, PlayState* play);
void BgHakaCurtain_Destroy(Actor* thisx, PlayState* play);
void BgHakaCurtain_Update(Actor* thisx, PlayState* play);
void BgHakaCurtain_Draw(Actor* thisx, PlayState* play);

void func_80B6DC98(BgHakaCurtain* this);
void func_80B6DCAC(BgHakaCurtain* this, PlayState* play);
void func_80B6DCEC(BgHakaCurtain* this);
void func_80B6DD00(BgHakaCurtain* this, PlayState* play);
void func_80B6DD5C(BgHakaCurtain* this);
void func_80B6DD70(BgHakaCurtain* this, PlayState* play);
void func_80B6DD9C(BgHakaCurtain* this, PlayState* play);
void func_80B6DEA8(BgHakaCurtain* this, PlayState* play);
void func_80B6DE80(BgHakaCurtain* this);

ActorProfile Bg_Haka_Curtain_Profile = {
    /**/ ACTOR_BG_HAKA_CURTAIN,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJ,
    /**/ sizeof(BgHakaCurtain),
    /**/ BgHakaCurtain_Init,
    /**/ BgHakaCurtain_Destroy,
    /**/ BgHakaCurtain_Update,
    /**/ BgHakaCurtain_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 600, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaCurtain_Init(Actor* thisx, PlayState* play) {
    BgHakaCurtain* this = (BgHakaCurtain*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_haka_obj_Colheader_001588);
    if (Flags_GetClear(play, this->dyna.actor.room)) {
        func_80B6DE80(this);
        return;
    }
    func_80B6DC98(this);
}

void BgHakaCurtain_Destroy(Actor* thisx, PlayState* play) {
    BgHakaCurtain* this = (BgHakaCurtain*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80B6DC98(BgHakaCurtain* this) {
    this->actionFunc = func_80B6DCAC;
}

void func_80B6DCAC(BgHakaCurtain* this, PlayState* play) {
    if (Flags_GetClear(play, this->dyna.actor.room)) {
        func_80B6DCEC(this);
    }
}

void func_80B6DCEC(BgHakaCurtain* this) {
    this->actionFunc = func_80B6DD00;
}

void func_80B6DD00(BgHakaCurtain* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        func_80B6DD5C(this);
        return;
    }
    CutsceneManager_Queue(this->dyna.actor.csId);
}

void func_80B6DD5C(BgHakaCurtain* this) {
    this->actionFunc = func_80B6DD70;
}

void func_80B6DD70(BgHakaCurtain* this, PlayState* play) {
}

void func_80B6DD80(BgHakaCurtain* this) {
    this->actionFunc = func_80B6DD9C;
    this->dyna.actor.velocity.y = 0.0f;
}

void func_80B6DD9C(BgHakaCurtain* this, PlayState* play) {
    if (this->dyna.actor.world.pos.y < this->dyna.actor.home.pos.y + 150.0f - 30.0f) {
        Math_StepToF(&this->dyna.actor.velocity.y, 1.6f, 0.12f);
    } else {
        Math_StepToF(&this->dyna.actor.velocity.y, 0.8f, 0.05f);
    }
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
    if (this->dyna.actor.home.pos.y + 150.0f < this->dyna.actor.world.pos.y) {
        func_80B6DE80(this);
        return;
    }
    Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_CURTAIN_DOWN - SFX_FLAG);
}

void func_80B6DE80(BgHakaCurtain* this) {
    this->actionFunc = func_80B6DEA8;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 150.0f;
}

void func_80B6DEA8(BgHakaCurtain* this, PlayState* play) {
}

void BgHakaCurtain_Update(Actor* thisx, PlayState* play) {
    BgHakaCurtain* this = (BgHakaCurtain*)thisx;
    CsCmdActorCue* cue;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_469)) {
        cue = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_469)];
        if ((cue->startFrame == play->csCtx.curFrame) && (cue->id == 2)) {
            func_80B6DD80(this);
        }
    }
    this->actionFunc(this, play);
}

void BgHakaCurtain_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_haka_obj_DL_001410);
}

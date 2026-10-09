/*
 * File: z_bg_ikana_shutter.c
 * Overlay: ovl_Bg_Ikana_Shutter
 * Description: Metal shutter in switch room of Stone Tower Temple
 */

#include "z_bg_ikana_shutter.h"
#include "z64quake.h"
#include "objects/object_ikana_obj/object_ikana_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgIkanaShutter_Init(Actor* thisx, PlayState* play);
void BgIkanaShutter_Destroy(Actor* thisx, PlayState* play);
void BgIkanaShutter_Update(Actor* thisx, PlayState* play);
void BgIkanaShutter_Draw(Actor* thisx, PlayState* play);

void func_80BD5828(BgIkanaShutter* this);
void func_80BD5844(BgIkanaShutter* this, PlayState* play);
void func_80BD5878(BgIkanaShutter* this);
void func_80BD5894(BgIkanaShutter* this, PlayState* play);
void func_80BD58F0(BgIkanaShutter* this);
void func_80BD5910(BgIkanaShutter* this, PlayState* play);
void func_80BD599C(BgIkanaShutter* this);
void func_80BD59C4(BgIkanaShutter* this, PlayState* play);
void func_80BD59F8(BgIkanaShutter* this);
void func_80BD5A18(BgIkanaShutter* this, PlayState* play);
void func_80BD5AE8(BgIkanaShutter* this);
void func_80BD5B04(BgIkanaShutter* this, PlayState* play);
void func_80BD5B44(BgIkanaShutter* this);
void func_80BD5B60(BgIkanaShutter* this, PlayState* play);
void func_80BD5BC4(BgIkanaShutter* this);
void func_80BD5BD8(BgIkanaShutter* this, PlayState* play);
void BgIkanaShutter_SetupDoNothing(BgIkanaShutter* this);
void BgIkanaShutter_DoNothing(BgIkanaShutter* this, PlayState* play);

ActorProfile Bg_Ikana_Shutter_Profile = {
    /**/ ACTOR_BG_IKANA_SHUTTER,
    /**/ ACTORCAT_SWITCH,
    /**/ FLAGS,
    /**/ OBJECT_IKANA_OBJ,
    /**/ sizeof(BgIkanaShutter),
    /**/ BgIkanaShutter_Init,
    /**/ BgIkanaShutter_Destroy,
    /**/ BgIkanaShutter_Update,
    /**/ BgIkanaShutter_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

bool BgIkanaShutter_AllSwitchesPressed(BgIkanaShutter* this, PlayState* play) {
    return Flags_GetSwitch(play, BGIKANASHUTTER_GET_SWITCH_FLAG(&this->dyna.actor)) &&
           Flags_GetSwitch(play, BGIKANASHUTTER_GET_SWITCH_FLAG(&this->dyna.actor) + 1) &&
           Flags_GetSwitch(play, BGIKANASHUTTER_GET_SWITCH_FLAG(&this->dyna.actor) + 2) &&
           Flags_GetSwitch(play, BGIKANASHUTTER_GET_SWITCH_FLAG(&this->dyna.actor) + 3);
}

void BgIkanaShutter_Init(Actor* thisx, PlayState* play) {
    BgIkanaShutter* this = (BgIkanaShutter*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_ikana_obj_Colheader_000F28);
    if (!BGIKANASHUTTER_GET_100(&this->dyna.actor)) {
        if (BgIkanaShutter_AllSwitchesPressed(this, play)) {
            func_80BD599C(this);
            return;
        }
        func_80BD5828(this);
        return;
    }
    if (Flags_GetClear(play, this->dyna.actor.room)) {
        BgIkanaShutter_SetupDoNothing(this);
        return;
    }
    func_80BD5AE8(this);
}

void BgIkanaShutter_Destroy(Actor* thisx, PlayState* play) {
    BgIkanaShutter* this = (BgIkanaShutter*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80BD5828(BgIkanaShutter* this) {
    this->actionFunc = func_80BD5844;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
}

void func_80BD5844(BgIkanaShutter* this, PlayState* play) {
    if (BgIkanaShutter_AllSwitchesPressed(this, play)) {
        func_80BD5878(this);
    }
}

void func_80BD5878(BgIkanaShutter* this) {
    this->actionFunc = func_80BD5894;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
}

void func_80BD5894(BgIkanaShutter* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        func_80BD58F0(this);
        return;
    }
    CutsceneManager_Queue(this->dyna.actor.csId);
}

void func_80BD58F0(BgIkanaShutter* this) {
    this->dyna.actor.velocity.y = 0.6f;
    this->actionFunc = func_80BD5910;
}

void func_80BD5910(BgIkanaShutter* this, PlayState* play) {
    Math_StepToF(&this->dyna.actor.velocity.y, 4.0f, 0.5f);
    if (Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 220.0f, 0.5f,
                           this->dyna.actor.velocity.y, 1.0f) < 0.5f) {
        func_80BD599C(this);
    }
}

void func_80BD599C(BgIkanaShutter* this) {
    this->actionFunc = func_80BD59C4;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 220.0f;
}

void func_80BD59C4(BgIkanaShutter* this, PlayState* play) {
    if (!BgIkanaShutter_AllSwitchesPressed(this, play)) {
        func_80BD59F8(this);
    }
}

void func_80BD59F8(BgIkanaShutter* this) {
    this->actionFunc = func_80BD5A18;
    this->dyna.actor.velocity.y = -23.0f;
}

void func_80BD5A18(BgIkanaShutter* this, PlayState* play) {
    s32 pad[2];
    s16 quakeIndex;

    this->dyna.actor.velocity.y += -5.0f;
    this->dyna.actor.velocity.y *= 0.978f;
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
    if (this->dyna.actor.world.pos.y <= this->dyna.actor.home.pos.y) {
        quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, 21536);
        Quake_SetPerturbations(quakeIndex, 4, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 12);

        func_80BD5828(this);
    }
}

void func_80BD5AE8(BgIkanaShutter* this) {
    this->actionFunc = func_80BD5B04;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
}

void func_80BD5B04(BgIkanaShutter* this, PlayState* play) {
    if (Flags_GetClearTemp(play, this->dyna.actor.room)) {
        func_80BD5B44(this);
    }
}

void func_80BD5B44(BgIkanaShutter* this) {
    this->actionFunc = func_80BD5B60;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
}

void func_80BD5B60(BgIkanaShutter* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        Flags_SetClear(play, this->dyna.actor.room);
        func_80BD5BC4(this);
        return;
    }
    CutsceneManager_Queue(this->dyna.actor.csId);
}

void func_80BD5BC4(BgIkanaShutter* this) {
    this->actionFunc = func_80BD5BD8;
}

void func_80BD5BD8(BgIkanaShutter* this, PlayState* play) {
    Math_StepToF(&this->dyna.actor.velocity.y, 4.0f, 0.5f);
    if (Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + -220.0f, 0.5f,
                           this->dyna.actor.velocity.y, 1.0f) < 0.5f) {
        BgIkanaShutter_SetupDoNothing(this);
    }
}

void BgIkanaShutter_SetupDoNothing(BgIkanaShutter* this) {
    this->actionFunc = BgIkanaShutter_DoNothing;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + -220.0f;
}

void BgIkanaShutter_DoNothing(BgIkanaShutter* this, PlayState* play) {
}

void BgIkanaShutter_Update(Actor* thisx, PlayState* play) {
    BgIkanaShutter* this = (BgIkanaShutter*)thisx;

    this->actionFunc(this, play);
}

void BgIkanaShutter_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_ikana_obj_DL_000CE8);
}

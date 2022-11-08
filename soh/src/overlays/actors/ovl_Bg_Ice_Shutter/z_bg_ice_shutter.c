/*
 * File: z_bg_ice_shutter.c
 * Overlay: ovl_Bg_Ice_Shutter
 * Description: Vertical Ice Bars (Doors) in Ice Cavern
 */

#include "z_bg_ice_shutter.h"
#include "objects/object_ice_objects/object_ice_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgIceShutter_Init(Actor* thisx, PlayState* play);
void BgIceShutter_Destroy(Actor* thisx, PlayState* play);
void BgIceShutter_Update(Actor* thisx, PlayState* play);
void BgIceShutter_Draw(Actor* thisx, PlayState* play);

void func_80891CF4(BgIceShutter* thisx, PlayState* play);
void func_80891D6C(BgIceShutter* thisx, PlayState* play);
void func_80891DD4(BgIceShutter* thisx, PlayState* play);

const ActorInit Bg_Ice_Shutter_InitVars = {
    ACTOR_BG_ICE_SHUTTER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_ICE_OBJECTS,
    sizeof(BgIceShutter),
    (ActorFunc)BgIceShutter_Init,
    (ActorFunc)BgIceShutter_Destroy,
    (ActorFunc)BgIceShutter_Update,
    (ActorFunc)BgIceShutter_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80891AC0(BgIceShutter* this) {
    f32 sp24;

    sp24 = Math_SinS(this->dyna.actor.shape.rot.x) * this->dyna.actor.velocity.y;
    this->dyna.actor.world.pos.y =
        (Math_CosS(this->dyna.actor.shape.rot.x) * this->dyna.actor.velocity.y) + this->dyna.actor.home.pos.y;
    this->dyna.actor.world.pos.x = (Math_SinS(this->dyna.actor.shape.rot.y) * sp24) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (Math_CosS(this->dyna.actor.shape.rot.y) * sp24) + this->dyna.actor.home.pos.z;
}

void BgIceShutter_Init(Actor* thisx, PlayState* play) {
    BgIceShutter* this = (BgIceShutter*)thisx;
    f32 sp24;
    CollisionHeader* colHeader;
    s32 sp28;
    f32 temp_f6;

    colHeader = NULL;
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    sp28 = this->dyna.actor.params & 0xFF;
    this->dyna.actor.params = (this->dyna.actor.params >> 8) & 0xFF;
    CollisionHeader_GetVirtual(&object_ice_objects_Col_002854, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (sp28 == 2) {
        this->dyna.actor.shape.rot.x = -0x4000;
    }

    if (sp28 != 1) {
        if (Flags_GetClear(play, this->dyna.actor.room)) {
            Actor_Kill(&this->dyna.actor);
        } else {
            this->actionFunc = func_80891CF4;
        }

    } else {
        if (Flags_GetSwitch(play, this->dyna.actor.params)) {
            Actor_Kill(&this->dyna.actor);
        } else {
            this->actionFunc = func_80891D6C;
        }
    }

    if (sp28 == 2) {
        temp_f6 = Math_SinS(this->dyna.actor.shape.rot.x) * 50.0f;
        this->dyna.actor.focus.pos.x =
            (Math_SinS(this->dyna.actor.shape.rot.y) * temp_f6) + this->dyna.actor.home.pos.x;
        this->dyna.actor.focus.pos.y = this->dyna.actor.home.pos.y;
        this->dyna.actor.focus.pos.z =
            this->dyna.actor.home.pos.z + (Math_CosS(this->dyna.actor.shape.rot.y) * temp_f6);
    } else {
        Actor_SetFocus(&this->dyna.actor, 50.0f);
    }
}

void BgIceShutter_Destroy(Actor* thisx, PlayState* play) {
    BgIceShutter* this = (BgIceShutter*)thisx;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80891CF4(BgIceShutter* this, PlayState* play) {
    if (Flags_GetTempClear(play, this->dyna.actor.room)) {
        Flags_SetClear(play, this->dyna.actor.room);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_SLIDE_DOOR_OPEN);
        this->actionFunc = func_80891DD4;
        if (this->dyna.actor.shape.rot.x == 0) {
            OnePointCutscene_Attention(play, &this->dyna.actor);
        }
    }
}

void func_80891D6C(BgIceShutter* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->dyna.actor.params)) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_SLIDE_DOOR_OPEN);
        this->actionFunc = func_80891DD4;
        OnePointCutscene_Attention(play, &this->dyna.actor);
    }
}

void func_80891DD4(BgIceShutter* this, PlayState* play) {
    Math_StepToF(&this->dyna.actor.speedXZ, 30.0f, 2.0f);
    if (Math_StepToF(&this->dyna.actor.velocity.y, 210.0f, this->dyna.actor.speedXZ)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    func_80891AC0(this);
}

void BgIceShutter_Update(Actor* thisx, PlayState* play) {
    BgIceShutter* this = (BgIceShutter*)thisx;

    this->actionFunc(this, play);
}

void BgIceShutter_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_ice_objects_DL_002740);
}

/*
 * File: z_bg_open_shutter.c
 * Overlay: ovl_Bg_Open_Shutter
 * Description: Sliding doors in opening dungeon
 */

#include "z_bg_open_shutter.h"
#include "objects/object_open_obj/object_open_obj.h"
#include "z64quake.h"
#include "z64rumble.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgOpenShutter_Init(Actor* thisx, PlayState* play);
void BgOpenShutter_Destroy(Actor* thisx, PlayState* play);
void BgOpenShutter_Update(Actor* thisx, PlayState* play2);
void BgOpenShutter_Draw(Actor* thisx, PlayState* play);

void func_80ACAD88(BgOpenShutter* this, PlayState* play);
void func_80ACAE5C(BgOpenShutter* this, PlayState* play);
void func_80ACAEF0(BgOpenShutter* this, PlayState* play);

typedef enum {
    /* 1 */ BGOPENSHUTTER_DOOR_OPEN = 1,
    /* 2 */ BGOPENSHUTTER_DOOR_CLOSED
} BGOpenShutterDoorState;

ActorProfile Bg_Open_Shutter_Profile = {
    /**/ ACTOR_BG_OPEN_SHUTTER,
    /**/ ACTORCAT_DOOR,
    /**/ FLAGS,
    /**/ OBJECT_OPEN_OBJ,
    /**/ sizeof(BgOpenShutter),
    /**/ BgOpenShutter_Init,
    /**/ BgOpenShutter_Destroy,
    /**/ BgOpenShutter_Update,
    /**/ BgOpenShutter_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 350, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

f32 func_80ACAB10(PlayState* play, Actor* actor, f32 arg2, f32 arg3, f32 arg4) {
    Player* player = GET_PLAYER(play);
    Vec3f point;
    Vec3f offset;

    point.x = player->actor.world.pos.x;
    point.y = player->actor.world.pos.y + arg2;
    point.z = player->actor.world.pos.z;

    Actor_WorldToActorCoords(actor, &offset, &point);
    if ((arg3 < fabsf(offset.x)) || (arg4 < fabsf(offset.y))) {
        return FLT_MAX;
    } else {
        return offset.z;
    }
}

s8 func_80ACABA8(BgOpenShutter* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_fv0;
    s16 temp_v0;

    if (!Player_InCsMode(play) && (this->slidingDoor.dyna.actor.xzDistToPlayer < 100.0f)) {
        temp_fv0 = func_80ACAB10(play, &this->slidingDoor.dyna.actor, 0.0f, 65.0f, 15.0f);
        if (fabsf(temp_fv0) < 50.0f) {
            temp_v0 = player->actor.shape.rot.y - this->slidingDoor.dyna.actor.shape.rot.y;
            if (temp_fv0 > 0.0f) {
                temp_v0 = (0x8000 - temp_v0);
            }
            if (ABS_ALT(temp_v0) < 0x3000) {
                if (temp_fv0 > 1.0f) {
                    return 1;
                }
                if (temp_fv0 < -1.0f) {
                    return -1;
                }
            }
        }
    }
    return 0;
}

void BgOpenShutter_Init(Actor* thisx, PlayState* play) {
    BgOpenShutter* this = (BgOpenShutter*)thisx;

    Actor_ProcessInitChain(&this->slidingDoor.dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->slidingDoor.dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->slidingDoor.dyna, &object_open_obj_Colheader_001640);
    this->actionFunc = func_80ACAD88;
}

void BgOpenShutter_Destroy(Actor* thisx, PlayState* play) {
    BgOpenShutter* this = (BgOpenShutter*)thisx;
    s32 transition = DOOR_GET_TRANSITION_ID(thisx);

    play->transitionActors.list[transition].id = -play->transitionActors.list[transition].id;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->slidingDoor.dyna.bgId);
}

void func_80ACAD88(BgOpenShutter* this, PlayState* play) {
    s32 doorDirection;

    if (this->slidingDoor.unk_15C != 0) {
        Player* player = GET_PLAYER(play);

        Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
        Camera_ChangeDoorCam(play->cameraPtrs[CAM_ID_MAIN], &this->slidingDoor.dyna.actor, player->cv.doorBgCamIndex,
                             0.0f, 12, 15, 10);
        this->unk_164 = 0;
        this->actionFunc = func_80ACAE5C;
        this->slidingDoor.dyna.actor.velocity.y = 0.0f;
    } else {
        Player* player = GET_PLAYER(play);

        doorDirection = func_80ACABA8(this, play);
        if (doorDirection > 0) {
            player->doorType = PLAYER_DOORTYPE_SLIDING;
            player->doorDirection = doorDirection;
            player->doorActor = &this->slidingDoor.dyna.actor;
            func_80122F28(player);
        }
    }
}

void func_80ACAE5C(BgOpenShutter* this, PlayState* play) {
    Math_StepToF(&this->slidingDoor.dyna.actor.velocity.y, 15.0f, 3.0f);
    if (Math_StepToF(&this->slidingDoor.dyna.actor.world.pos.y, this->slidingDoor.dyna.actor.home.pos.y + 120.0f,
                     this->slidingDoor.dyna.actor.velocity.y)) {
        this->unk_164++;
    }
    if (this->unk_164 >= 10) {
        Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
        this->actionFunc = func_80ACAEF0;
    }
}

void func_80ACAEF0(BgOpenShutter* this, PlayState* play) {
    s32 pad;
    s16 quakeIndex;

    Math_StepToF(&this->slidingDoor.dyna.actor.velocity.y, 20.0f, 8.0f);
    if (Math_StepToF(&this->slidingDoor.dyna.actor.world.pos.y, this->slidingDoor.dyna.actor.home.pos.y,
                     this->slidingDoor.dyna.actor.velocity.y)) {
        this->slidingDoor.dyna.actor.floorHeight = this->slidingDoor.dyna.actor.home.pos.y;
        Actor_SpawnFloorDustRing(play, &this->slidingDoor.dyna.actor, &this->slidingDoor.dyna.actor.world.pos, 60.0f,
                                 10, 8.0f, 500, 10, true);
        Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_BIGWALL_BOUND);

        quakeIndex = Quake_Request(Play_GetCamera(play, CAM_ID_MAIN), QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, -32536);
        Quake_SetPerturbations(quakeIndex, 2, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 10);

        Rumble_Request(this->slidingDoor.dyna.actor.xyzDistToPlayerSq, 180, 20, 100);

        this->slidingDoor.unk_15C = 0;
        this->actionFunc = func_80ACAD88;
    }
}

void BgOpenShutter_Update(Actor* thisx, PlayState* play2) {
    BgOpenShutter* this = (BgOpenShutter*)thisx;
    PlayState* play = play2;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_124)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_124);
        if (play->csCtx.actorCues[cueChannel]->id == BGOPENSHUTTER_DOOR_OPEN) {
            if (this->actionFunc == func_80ACAD88) {
                Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
                this->actionFunc = func_80ACAE5C;
                this->slidingDoor.dyna.actor.velocity.y = 0.0f;
            }
            this->unk_164 = 0;
        } else if (play->csCtx.actorCues[cueChannel]->id == BGOPENSHUTTER_DOOR_CLOSED) {
            if (this->actionFunc == func_80ACAE5C) {
                Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
                this->actionFunc = func_80ACAEF0;
            }
        }
    }
    this->actionFunc(this, play);
}

void BgOpenShutter_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_open_obj_DL_0003E8);
}

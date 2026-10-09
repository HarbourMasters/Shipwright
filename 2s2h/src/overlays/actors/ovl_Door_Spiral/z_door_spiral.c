/*
 * File: z_door_spiral.c
 * Overlay: ovl_Door_Spiral
 * Description: Staircase
 */

#include "z_door_spiral.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "objects/object_numa_obj/object_numa_obj.h"
#include "objects/object_hakugin_obj/object_hakugin_obj.h"
#include "objects/object_ikana_obj/object_ikana_obj.h"
#include "objects/object_ikninside_obj/object_ikninside_obj.h"
#include "objects/object_danpei_object/object_danpei_object.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void DoorSpiral_Init(Actor* thisx, PlayState* play);
void DoorSpiral_Destroy(Actor* thisx, PlayState* play);
void DoorSpiral_Update(Actor* thisx, PlayState* play);
void DoorSpiral_Draw(Actor* thisx, PlayState* play);

void DoorSpiral_SetupAction(DoorSpiral* this, DoorSpiralActionFunc actionFunc);
s32 func_809A2B70(DoorSpiral* this, PlayState* play);
u8 func_809A2BF8(PlayState* play);
f32 func_809A2E08(PlayState* play, DoorSpiral* this, f32 arg2, f32 arg3, f32 arg4);
void func_809A2DB0(DoorSpiral* this, PlayState* play);
void func_809A2FF8(DoorSpiral* this, PlayState* play);
void func_809A2DB0(DoorSpiral* this, PlayState* play);
void func_809A3098(DoorSpiral* this, PlayState* play);
s32 func_809A2EA0(DoorSpiral* this, PlayState* play);

ActorProfile Door_Spiral_Profile = {
    /**/ ACTOR_DOOR_SPIRAL,
    /**/ ACTORCAT_DOOR,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(DoorSpiral),
    /**/ DoorSpiral_Init,
    /**/ DoorSpiral_Destroy,
    /**/ DoorSpiral_Update,
    /**/ DoorSpiral_Draw,
};

typedef struct {
    /* 0x0 */ Gfx* dLists[2]; // represents a staircase up or down
    /* 0x8 */ u32 unk_08;
    /* 0x9 */ u8 unk_09;
    /* 0xA */ u8 translateZ;
    /* 0xB */ u8 unk_0B;
    /* 0xC */ u8 unk_0C;
} SpiralInfo; // size = 0x10

SpiralInfo sSpiralInfoTable[] = {
    { { NULL, NULL }, 0, 130, 12, 50, 15 },
    { { gameplay_dangeon_keep_DL_0219E0, gameplay_dangeon_keep_DL_01D980 }, 0, 130, 12, 50, 15 },
    { { object_numa_obj_DL_004448, object_numa_obj_DL_0007A8 }, 0, 130, 12, 50, 15 },
    { { object_numa_obj_DL_0051B8, object_numa_obj_DL_0014C8 }, 0, 130, 12, 50, 15 },
    { { object_hakugin_obj_DL_009278, object_hakugin_obj_DL_006128 }, 0, 130, 12, 50, 15 },
    { { object_ikana_obj_DL_013EA8, object_ikana_obj_DL_012B70 }, 0, 130, 12, 50, 15 },
    { { object_ikninside_obj_DL_000EA0, object_ikninside_obj_DL_000590 }, 0, 130, 12, 50, 15 },
    { { object_danpei_object_DL_002110, object_danpei_object_DL_0012C0 }, 0, 130, 12, 50, 15 },
};

typedef struct {
    /* 0x0 */ s16 objectId;
    /* 0x2 */ u8 index;
} SpiralObjectInfo; // size = 0x4

SpiralObjectInfo sSpiralObjectInfoTable[] = {
    { GAMEPLAY_KEEP, 0 },    { GAMEPLAY_DANGEON_KEEP, 1 }, { OBJECT_NUMA_OBJ, 2 },      { OBJECT_HAKUGIN_OBJ, 4 },
    { OBJECT_IKANA_OBJ, 5 }, { OBJECT_DANPEI_OBJECT, 7 },  { OBJECT_IKNINSIDE_OBJ, 6 },
};

typedef struct {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ u8 index;
} SpiralSceneInfo; // size = 0x4

SpiralSceneInfo sSpiralSceneInfoTable[] = {
    { SCENE_MITURIN, 2 },     { SCENE_HAKUGIN, 3 },   { SCENE_INISIE_N, 4 }, { SCENE_INISIE_R, 4 },
    { SCENE_DANPEI2TEST, 5 }, { SCENE_IKNINSIDE, 6 }, { SCENE_CASTLE, 6 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
};

void DoorSpiral_SetupAction(DoorSpiral* this, DoorSpiralActionFunc actionFunc) {
    this->actionFunc = actionFunc;
    this->unk14A = 0;
}

s32 func_809A2B70(DoorSpiral* this, PlayState* play) {
    SpiralObjectInfo* spiralObjectInfo = &sSpiralObjectInfoTable[this->unk147];

    this->unk148 = spiralObjectInfo->index;
    if ((this->unk148 == 7) || ((this->unk148 == 2) && play->roomCtx.curRoom.enablePosLights)) {
        if (this->unk148 == 2) {
            this->unk148 = 3;
        }
        this->actor.flags |= ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED;
    }
    DoorSpiral_SetupAction(this, func_809A2FF8);
    return 0;
}

u8 func_809A2BF8(PlayState* play) {
    SpiralSceneInfo* spiralSceneInfo = sSpiralSceneInfoTable;
    s32 i;
    u8 var_v1;

    for (i = 0; i < ARRAY_COUNT(sSpiralSceneInfoTable); i++) {
        if (play->sceneId == spiralSceneInfo->sceneId) {
            break;
        }
        spiralSceneInfo++;
    }
    if (i < ARRAY_COUNT(sSpiralSceneInfoTable)) {
        var_v1 = spiralSceneInfo->index;
    } else {
        var_v1 = (Object_GetSlot(&play->objectCtx, GAMEPLAY_DANGEON_KEEP) > OBJECT_SLOT_NONE) ? 1 : 0;
    }
    return var_v1;
}

void DoorSpiral_Init(Actor* thisx, PlayState* play) {
    DoorSpiral* this = (DoorSpiral*)thisx;
    s32 transitionId = DOOR_GET_TRANSITION_ID(thisx);
    s8 objectSlot;

    if (this->actor.room != play->transitionActors.list[transitionId].sides[0].room) {
        Actor_Kill(&this->actor);
        return;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->type = DOORSPIRAL_GET_TYPE(thisx);
    this->direction = DOORSPIRAL_GET_DIRECTION(thisx);
    this->unk147 = func_809A2BF8(play);
    objectSlot = Object_GetSlot(&play->objectCtx, sSpiralObjectInfoTable[this->unk147].objectId);
    this->objectSlot = objectSlot;
    if (objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }
    DoorSpiral_SetupAction(this, func_809A2DB0);
    Actor_SetFocus(&this->actor, 60.0f);
}

void DoorSpiral_Destroy(Actor* thisx, PlayState* play) {
    s32 transitionId = DOOR_GET_TRANSITION_ID(thisx);

    play->transitionActors.list[transitionId].id = -play->transitionActors.list[transitionId].id;
}

void func_809A2DB0(DoorSpiral* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        func_809A2B70(this, play);
    }
}

f32 func_809A2E08(PlayState* play, DoorSpiral* this, f32 arg2, f32 arg3, f32 arg4) {
    Player* player = GET_PLAYER(play);
    Vec3f point;
    Vec3f offset;

    point.x = player->actor.world.pos.x;
    point.y = player->actor.world.pos.y + arg2;
    point.z = player->actor.world.pos.z;

    Actor_WorldToActorCoords(&this->actor, &offset, &point);

    if ((arg3 < fabsf(offset.x)) || (arg4 < fabsf(offset.y))) {
        return FLT_MAX;
    } else {
        return offset.z;
    }
}

s32 func_809A2EA0(DoorSpiral* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Player_InCsMode(play)) {
        SpiralInfo* spiralInfo = &sSpiralInfoTable[this->unk148];
        f32 temp_fv0 = func_809A2E08(play, this, 0.0f, spiralInfo->unk_0B, spiralInfo->unk_0C);

        if (fabsf(temp_fv0) < 64.0f) {
            s16 var_v0 = player->actor.shape.rot.y - this->actor.shape.rot.y;

            if (temp_fv0 > 0.0f) {
                var_v0 = 0x8000 - var_v0;
            }
            if (ABS_ALT(var_v0) < 0x3000) {
                return (temp_fv0 >= 0.0f) ? 1.0f : -1.0f;
            }
        }
    }
    return 0;
}

void func_809A2FF8(DoorSpiral* this, PlayState* play) {
    Player* player;
    u32 transitionId;

    if (this->shouldClimb) {
        DoorSpiral_SetupAction(this, func_809A3098);
    } else if (func_809A2EA0(this, play) != 0) {
        player = GET_PLAYER(play);
        player->doorType = PLAYER_DOORTYPE_STAIRCASE;
        player->doorDirection = this->direction;
        player->doorActor = &this->actor;
        transitionId = DOOR_GET_TRANSITION_ID(&this->actor);
        player->doorNext = (play->transitionActors.list[transitionId].params >> 0xA);
        func_80122F28(player);
    }
}

void func_809A3098(DoorSpiral* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & PLAYER_STATE1_20000000)) {
        DoorSpiral_SetupAction(this, func_809A2DB0);
        this->shouldClimb = false;
    }
}

void DoorSpiral_Update(Actor* thisx, PlayState* play) {
    DoorSpiral* this = (DoorSpiral*)thisx;
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_400 | PLAYER_STATE1_10000000)) ||
        (this->actionFunc == func_809A2DB0)) {
        this->actionFunc(this, play);
    }
}

void DoorSpiral_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DoorSpiral* this = (DoorSpiral*)thisx;

    if (this->actor.objectSlot == this->objectSlot) {
        SpiralInfo* spiralInfo = &sSpiralInfoTable[this->unk148];
        Gfx* dList = spiralInfo->dLists[this->direction];

        if (dList != NULL) {
            OPEN_DISPS(play->state.gfxCtx);

            Gfx_SetupDL25_Opa(play->state.gfxCtx);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, spiralInfo->dLists[this->direction]);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

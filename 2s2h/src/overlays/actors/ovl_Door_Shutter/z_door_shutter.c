/*
 * File: z_door_shutter.c
 * Overlay: ovl_Door_Shutter
 * Description: Studded Lifting Door / Ikana Castle Rolling Door
 */

#include "z_door_shutter.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_bdoor/object_bdoor.h"
#include "objects/object_numa_obj/object_numa_obj.h"
#include "objects/object_hakugin_obj/object_hakugin_obj.h"
#include "objects/object_dblue_object/object_dblue_object.h"
#include "objects/object_ikana_obj/object_ikana_obj.h"
#include "objects/object_redead_obj/object_redead_obj.h"
#include "objects/object_ikninside_obj/object_ikninside_obj.h"
#include "objects/object_random_obj/object_random_obj.h"
#include "objects/object_kinsta1_obj/object_kinsta1_obj.h"
#include "objects/object_kaizoku_obj/object_kaizoku_obj.h"
#include "objects/object_last_obj/object_last_obj.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void DoorShutter_Init(Actor* thisx, PlayState* play2);
void DoorShutter_Destroy(Actor* thisx, PlayState* play);
void DoorShutter_Update(Actor* thisx, PlayState* play);

void DoorShutter_SetupType(DoorShutter* this, PlayState* play);
void func_808A0F88(DoorShutter* this, PlayState* play);
void func_808A1080(DoorShutter* this, PlayState* play);
void func_808A1090(DoorShutter* this, PlayState* play);
void func_808A1548(DoorShutter* this, PlayState* play);
void func_808A1618(DoorShutter* this, PlayState* play);
void func_808A1684(DoorShutter* this, PlayState* play);
void func_808A1784(DoorShutter* this, PlayState* play);
void func_808A1B48(DoorShutter* this, PlayState* play);
void func_808A1C50(DoorShutter* this, PlayState* play);
void DoorShutter_Draw(Actor* thisx, PlayState* play);

ActorProfile Door_Shutter_Profile = {
    /**/ ACTOR_DOOR_SHUTTER,
    /**/ ACTORCAT_DOOR,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(DoorShutter),
    /**/ DoorShutter_Init,
    /**/ DoorShutter_Destroy,
    /**/ DoorShutter_Update,
    /**/ NULL,
};

typedef struct {
    /* 0x0 */ s16 objectId;
    /* 0x2 */ u8 index1;
    /* 0x3 */ u8 index2;
} ShutterObjectInfo; // size = 0x4

ShutterObjectInfo D_808A2180[] = {
    { OBJECT_BDOOR, 0, 0 },       { GAMEPLAY_KEEP, 1, 1 },        { OBJECT_NUMA_OBJ, 2, 2 },
    { OBJECT_HAKUGIN_OBJ, 3, 3 }, { OBJECT_DBLUE_OBJECT, 4, 4 },  { OBJECT_IKANA_OBJ, 5, 5 },
    { OBJECT_REDEAD_OBJ, 6, 6 },  { OBJECT_IKNINSIDE_OBJ, 7, 7 }, { OBJECT_LAST_OBJ, 11, 11 },
    { OBJECT_RANDOM_OBJ, 8, 8 },  { OBJECT_KINSTA1_OBJ, 9, 9 },   { OBJECT_KAIZOKU_OBJ, 10, 10 },
};

typedef struct {
    /* 0x0 */ Gfx* unk_00;
    /* 0x4 */ Gfx* unk_04;
    /* 0x8 */ u8 unk_08;
    /* 0x9 */ u8 translateZ;
    /* 0xA */ u8 unk_0A;
    /* 0xB */ u8 unk_0B;
} ShutterInfo; // size = 0xC

ShutterInfo D_808A21B0[] = {
    { gBossDoorDL, NULL, 130, 12, 50, 15 },
    { gameplay_keep_DL_077990, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { object_numa_obj_DL_007150, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { object_hakugin_obj_DL_000128, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { gGreatBayTempleObjectDoorDL, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { object_ikana_obj_DL_014A40, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { gBeneathTheWellSlidingDoorDL, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { object_ikninside_obj_DL_004440, object_ikninside_obj_DL_005260, 130, 0, 20, 15 },
    { gSecretShrineSlidingDoorDL, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { gSwampSpiderHouseSlidingDoorDL, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { object_kaizoku_obj_DL_0001A0, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
    { object_last_obj_DL_0039C0, gameplay_keep_DL_078A80, 130, 12, 20, 15 },
};

s8 D_808A2240[] = {
    -1, -1, -1, -1, -1, 0, -1, -1,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

typedef struct {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ u8 index;
} ShutterSceneInfo; // size = 0x4

ShutterSceneInfo D_808A2258[] = {
    { SCENE_MITURIN, 2 },   { SCENE_HAKUGIN, 3 },   { SCENE_SEA, 4 },
    { SCENE_INISIE_N, 5 },  { SCENE_INISIE_R, 5 },  { SCENE_REDEAD, 6 },
    { SCENE_IKNINSIDE, 7 }, { SCENE_CASTLE, 7 },    { SCENE_RANDOM, 9 },
    { SCENE_KINSTA1, 10 },  { SCENE_KAIZOKU, 11 },  { SCENE_PIRATE, 11 },
    { SCENE_TORIDE, 11 },   { SCENE_LAST_DEKU, 8 }, { SCENE_LAST_GORON, 8 },
    { SCENE_LAST_ZORA, 8 }, { SCENE_LAST_LINK, 8 }, { -1, 1 },
};

typedef struct {
    /* 0x0 */ s16 dungeonSceneId;
    /* 0x2 */ s16 bossSceneId;
    /* 0x4 */ u8 index;
} BossDoorInfo; // size = 0x6

BossDoorInfo D_808A22A0[] = {
    { SCENE_MITURIN, SCENE_MITURIN_BS, 1 }, { SCENE_HAKUGIN, SCENE_HAKUGIN_BS, 2 }, { SCENE_SEA, SCENE_SEA_BS, 3 },
    { SCENE_INISIE_N, SCENE_INISIE_BS, 4 }, { SCENE_INISIE_R, SCENE_INISIE_BS, 4 }, { -1, -1, 0 },
};

Vec3f D_808A22C4 = { 120.0f, 0.0f, 0.0f };

Vec3f D_808A22D0 = { -90.0f, 0.0f, 0.0f };

TexturePtr D_808A22DC[] = {
    gBossDoorDefaultTex, gBossDoorWoodfallTex, gBossDoorSnowheadTex, gBossDoorGreatBayTex, gBossDoorStoneTowerTex,
};

void DoorShutter_SetupAction(DoorShutter* this, DoorShutterActionFunc actionFunc) {
    this->actionFunc = actionFunc;
    this->unk_167 = 0;
}

s32 func_808A0900(DoorShutter* this, PlayState* play) {
    TransitionActorEntry* transitionEntry =
        &play->transitionActors.list[DOOR_GET_TRANSITION_ID(&this->slidingDoor.dyna.actor)];
    s8 frontRoom = transitionEntry->sides[0].room;

    if (frontRoom == transitionEntry->sides[1].room) {
        if (ABS_ALT(BINANG_SUB(this->slidingDoor.dyna.actor.shape.rot.y,
                               this->slidingDoor.dyna.actor.yawTowardsPlayer)) < 0x4000) {
            frontRoom = -1;
        }
    }
    return frontRoom == this->slidingDoor.dyna.actor.room;
}

s32 DoorShutter_SetupDoor(DoorShutter* this, PlayState* play) {
    ShutterObjectInfo* sp24 = &D_808A2180[this->unk_163];
    s32 doorType = this->doorType;

    if (doorType != DOORSHUTTER_TYPE_4) {
        if (func_808A0900(this, play)) {
            if ((doorType == DOORSHUTTER_TYPE_7) || (doorType == DOORSHUTTER_TYPE_6)) {
                doorType = DOORSHUTTER_TYPE_1;
            } else if (doorType == DOORSHUTTER_TYPE_BOSS_DOOR) {
                doorType = DOORSHUTTER_TYPE_3;
            } else {
                doorType = DOORSHUTTER_TYPE_0;
            }
        }
    }

    if (doorType == DOORSHUTTER_TYPE_0) {
        this->unk_164 = sp24->index1;
    } else {
        this->unk_164 = sp24->index2;
    }

    if (doorType == DOORSHUTTER_TYPE_1) {
        if (!Flags_GetClear(play, this->slidingDoor.dyna.actor.room)) {
            DoorShutter_SetupAction(this, func_808A0F88);
            this->unk_168 = 1.0f;
            return true;
        }
    } else if ((doorType == DOORSHUTTER_TYPE_2) || (doorType == DOORSHUTTER_TYPE_7)) {
        if (!Flags_GetSwitch(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->slidingDoor.dyna.actor))) {
            DoorShutter_SetupAction(this, func_808A1548);
            this->unk_168 = 1.0f;
            return true;
        }
        DoorShutter_SetupAction(this, func_808A1618);
        return false;
    } else if (doorType == DOORSHUTTER_TYPE_3) {
        DoorShutter_SetupAction(this, func_808A1080);
        this->unk_168 = 1.0f;
        return false;
    }
    DoorShutter_SetupAction(this, func_808A1090);
    return false;
}

void DoorShutter_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DoorShutter* this = (DoorShutter*)thisx;
    s32 sp24;
    s32 i;

    Actor_ProcessInitChain(&this->slidingDoor.dyna.actor, sInitChain);
    this->doorType = DOORSHUTTER_GET_TYPE(&this->slidingDoor.dyna.actor);
    sp24 = D_808A2240[this->doorType];

    if (sp24 < 0) {
        ShutterSceneInfo* shutterSceneInfo = &D_808A2258[0];

        for (i = 0; i < ARRAY_COUNT(D_808A2258) - 1; i++, shutterSceneInfo++) {
            if (play->sceneId == shutterSceneInfo->sceneId) {
                break;
            }
        }

        sp24 = shutterSceneInfo->index;
        if (sp24 == 6) {
            this->slidingDoor.dyna.actor.flags |= ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED;
        }
    } else if (sp24 == 0) {
        BossDoorInfo* bossDoorInfo = &D_808A22A0[0];

        for (i = 0; i < ARRAY_COUNT(D_808A22A0) - 1; i++, bossDoorInfo++) {
            if ((play->sceneId == bossDoorInfo->dungeonSceneId) || (play->sceneId == bossDoorInfo->bossSceneId)) {
                break;
            }
        }

        this->slidingDoor.unk_15E = bossDoorInfo->index;
    } else {
        this->slidingDoor.dyna.actor.room = -1;
    }

    if ((this->objectSlot = Object_GetSlot(&play->objectCtx, D_808A2180[sp24].objectId)) <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->slidingDoor.dyna.actor);
        return;
    }

    DoorShutter_SetupAction(this, DoorShutter_SetupType);
    this->unk_163 = sp24;

    if ((this->doorType == DOORSHUTTER_TYPE_4) || (this->doorType == DOORSHUTTER_TYPE_BOSS_DOOR)) {
        if (!Flags_GetSwitch(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->slidingDoor.dyna.actor))) {
            this->unk_166 = 10;
        }
    }

    Actor_SetFocus(&this->slidingDoor.dyna.actor, 60.0f);
}

void DoorShutter_Destroy(Actor* thisx, PlayState* play) {
    DoorShutter* this = (DoorShutter*)thisx;

    if (this->slidingDoor.dyna.actor.room >= 0) {
        s32 transitionActorId = DOOR_GET_TRANSITION_ID(&this->slidingDoor.dyna.actor);

        play->transitionActors.list[transitionActorId].id = -play->transitionActors.list[transitionActorId].id;
    }
}

void DoorShutter_SetupType(DoorShutter* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot) &&
        (!R_PLAY_FILL_SCREEN_ON || (R_PLAY_FILL_SCREEN_ALPHA == 0))) {
        this->slidingDoor.dyna.actor.objectSlot = this->objectSlot;
        this->slidingDoor.dyna.actor.draw = DoorShutter_Draw;
        DoorShutter_SetupDoor(this, play);
    }
}

f32 func_808A0D90(PlayState* play, DoorShutter* this, f32 arg2, f32 arg3, f32 arg4) {
    Player* player = GET_PLAYER(play);
    Vec3f sp28;
    Vec3f sp1C;

    sp28.x = player->actor.world.pos.x;
    sp28.y = player->actor.world.pos.y + arg2;
    sp28.z = player->actor.world.pos.z;

    Actor_WorldToActorCoords(&this->slidingDoor.dyna.actor, &sp1C, &sp28);

    if ((arg3 < fabsf(sp1C.x)) || (arg4 < fabsf(sp1C.y))) {
        return FLT_MAX;
    }
    return sp1C.z;
}

s32 func_808A0E28(DoorShutter* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Player_InCsMode(play)) {
        ShutterInfo* shutterInfo = &D_808A21B0[this->unk_164];
        f32 temp_f0 = func_808A0D90(play, this, 0.0f, shutterInfo->unk_0A, shutterInfo->unk_0B);

        if (GameInteractor_Should(VB_BE_NEAR_DOOR, fabsf(temp_f0) < 50.0f, &temp_f0)) {
            s16 temp_v0_2 = BINANG_SUB(player->actor.shape.rot.y, this->slidingDoor.dyna.actor.shape.rot.y);

            if (temp_f0 > 0.0f) {
                temp_v0_2 = 0x8000 - temp_v0_2;
            }

            if (ABS_ALT(temp_v0_2) < 0x3000) {
                return (temp_f0 >= 0.0f) ? 1.0f : -1.0f;
            }
        }
    }
    return 0;
}

void func_808A0F88(DoorShutter* this, PlayState* play) {
    if (Flags_GetClear(play, this->slidingDoor.dyna.actor.room) ||
        Flags_GetClearTemp(play, this->slidingDoor.dyna.actor.room)) {
        this->csId = this->slidingDoor.dyna.actor.csId;
        if (this->doorType == DOORSHUTTER_TYPE_7) {
            if (this->csId != CS_ID_NONE) {
                this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
            }
        }

        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->slidingDoor.dyna.actor);
            Flags_SetClear(play, this->slidingDoor.dyna.actor.room);
            DoorShutter_SetupAction(this, func_808A1784);
            this->unk_167 = -1;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    } else if (func_808A0E28(this, play) != 0) {
        Player* player = GET_PLAYER(play);

        player->doorType = PLAYER_DOORTYPE_TALKING;
        player->doorActor = &this->slidingDoor.dyna.actor;
        this->slidingDoor.dyna.actor.textId = 0x1801;
        func_80122F28(player);
    }
}

void func_808A1080(DoorShutter* this, PlayState* play) {
}

void func_808A1090(DoorShutter* this, PlayState* play) {
    if (this->slidingDoor.unk_15C != 0) {
        DoorShutter_SetupAction(this, func_808A1684);
        this->slidingDoor.dyna.actor.velocity.y = 0.0f;
        if (this->unk_166 != 0) {
            Flags_SetSwitch(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->slidingDoor.dyna.actor));
            if (this->doorType != DOORSHUTTER_TYPE_BOSS_DOOR) {
                DUNGEON_KEY_COUNT(gSaveContext.mapIndex) = DUNGEON_KEY_COUNT(gSaveContext.mapIndex) - 1;
                Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_CHAIN_KEY_UNLOCK);
            } else {
                Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_CHAIN_KEY_UNLOCK_B);
            }
        }
    } else {
        s32 doorDirection = func_808A0E28(this, play);

        if (doorDirection != 0) {
            Player* player = GET_PLAYER(play);

            player->doorType = PLAYER_DOORTYPE_SLIDING;
            player->doorDirection = doorDirection;
            player->doorActor = &this->slidingDoor.dyna.actor;

            if (this->unk_163 == 7) {
                player->doorTimer = 12;
            } else {
                player->doorTimer = 0;
            }

            if (this->doorType == DOORSHUTTER_TYPE_6) {
                if (GameInteractor_Should(VB_DOOR_HEALTH_CHECK_FAIL,
                                          gSaveContext.save.saveInfo.playerData.healthCapacity <
                                              (DOORSHUTTER_GET_1F(&this->slidingDoor.dyna.actor) * 0x10),
                                          this)) {
                    player->doorType = PLAYER_DOORTYPE_TALKING;
                    this->slidingDoor.dyna.actor.textId = 0x14FC;
                }
            } else if (this->unk_166 != 0) {
                if (this->doorType == DOORSHUTTER_TYPE_BOSS_DOOR) {
                    if (!CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, gSaveContext.mapIndex)) {
                        player->doorType = PLAYER_DOORTYPE_TALKING;
                        this->slidingDoor.dyna.actor.textId = 0x1803;
                    }
                    player->doorTimer += 10;
                } else if (DUNGEON_KEY_COUNT(gSaveContext.mapIndex) <= 0) {
                    player->doorType = PLAYER_DOORTYPE_TALKING;
                    this->slidingDoor.dyna.actor.textId = 0x1802;
                } else {
                    player->doorTimer += 10;
                }
            }
            func_80122F28(player);
        }
    }
}

void func_808A1288(DoorShutter* this, PlayState* play) {
    if (this->slidingDoor.dyna.actor.category == ACTORCAT_DOOR) {
        Player* player = GET_PLAYER(play);
        s32 sp38 = this->unk_164;
        s32 sp34 = 0xF;

        if (DoorShutter_SetupDoor(this, play)) {
            sp34 = 0x20;
        }

        DoorShutter_SetupAction(this, func_808A1684);

        this->unk_164 = sp38;
        this->unk_168 = 0.0f;

        Camera_ChangeDoorCam(play->cameraPtrs[CAM_ID_MAIN], &this->slidingDoor.dyna.actor, player->cv.doorBgCamIndex,
                             this->unk_168, 12, sp34, 10);
    }
}

s32 func_808A1340(DoorShutter* this, PlayState* play) {
    Vec3f sp2C;

    if (this->slidingDoor.dyna.actor.velocity.y == 0.0f) {
        func_808A1288(this, play);
        if (this->unk_163 != 7) {
            Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
        }
    }

    if (this->unk_163 == 7) {
        s32 pad;

        if (this->unk_163 == 7) {
            Actor_PlaySfx_Flagged(&this->slidingDoor.dyna.actor, NA_SE_EV_IKANA_DOOR_OPEN - SFX_FLAG);
        }

        Lib_Vec3f_TranslateAndRotateY(&this->slidingDoor.dyna.actor.home.pos, this->slidingDoor.dyna.actor.shape.rot.y,
                                      (this->unk_163 == 7) ? &D_808A22C4 : &D_808A22D0, &sp2C);
        Math_StepToF(&this->slidingDoor.dyna.actor.velocity.y, 5.0f, 0.5f);

        if (Math_Vec3f_StepToXZ(&this->slidingDoor.dyna.actor.world.pos, &sp2C,
                                this->slidingDoor.dyna.actor.velocity.y) == 0.0f) {
            return true;
        }
    } else {
        Math_StepToF(&this->slidingDoor.dyna.actor.velocity.y, 15.0f, 3.0f);

        if (Math_StepToF(&this->slidingDoor.dyna.actor.world.pos.y, this->slidingDoor.dyna.actor.home.pos.y + 200.0f,
                         this->slidingDoor.dyna.actor.velocity.y)) {
            return true;
        }
    }
    return false;
}

s32 func_808A1478(DoorShutter* this, PlayState* play, f32 arg2) {
    f32 temp = 1.0f - arg2;

    if (temp == this->unk_168) {
        if (arg2 == 1.0f) {
            Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_METALDOOR_CLOSE);
        } else {
            Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_METALDOOR_OPEN);
        }

        if ((this->csId != CS_ID_NONE) && (CutsceneManager_GetCurrentCsId() == this->csId)) {
            Player_SetCsAction(play, &this->slidingDoor.dyna.actor, PLAYER_CSACTION_1);
        }
    }

    if (Math_StepToF(&this->unk_168, arg2, 0.2f)) {
        return true;
    }
    return false;
}

void func_808A1548(DoorShutter* this, PlayState* play) {
    if (func_808A1478(this, play, 1.0f)) {
        if (Flags_GetSwitch(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->slidingDoor.dyna.actor))) {
            this->csId = this->slidingDoor.dyna.actor.csId;
            if (CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_StartWithPlayerCs(this->csId, &this->slidingDoor.dyna.actor);
                DoorShutter_SetupAction(this, func_808A1784);
                this->unk_167 = -1;
            } else {
                CutsceneManager_Queue(this->csId);
            }
        } else if (func_808A0E28(this, play) != 0) {
            Player* player = GET_PLAYER(play);

            player->doorType = PLAYER_DOORTYPE_TALKING;
            player->doorActor = &this->slidingDoor.dyna.actor;
            this->slidingDoor.dyna.actor.textId = 0x1800;
            func_80122F28(player);
        }
    }
}

void func_808A1618(DoorShutter* this, PlayState* play) {
    if ((this->slidingDoor.unk_15C == 0) &&
        !Flags_GetSwitch(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->slidingDoor.dyna.actor))) {
        DoorShutter_SetupAction(this, func_808A1548);
    } else {
        func_808A1090(this, play);
    }
}

void func_808A1684(DoorShutter* this, PlayState* play) {
    f32 phi_f0;

    if ((DECR(this->unk_166) == 0) && (play->roomCtx.status == 0) && func_808A1340(this, play)) {
        if (this->doorType == DOORSHUTTER_TYPE_BOSS_DOOR) {
            phi_f0 = 20.0f;
        } else {
            phi_f0 = 50.0f;
        }

        if (phi_f0 < this->slidingDoor.dyna.actor.xzDistToPlayer) {
            if (this->unk_163 == 7) {
                this->slidingDoor.dyna.actor.velocity.y = 0.0f;
            } else {
                if (DoorShutter_SetupDoor(this, play)) {
                    this->slidingDoor.dyna.actor.velocity.y = 30.0f;
                }
                Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
            }
            DoorShutter_SetupAction(this, func_808A1B48);
        }
    }
}

void func_808A1784(DoorShutter* this, PlayState* play) {
    if (this->unk_167 != 0) {
        if (func_800F22C4(this->csId, &this->slidingDoor.dyna.actor) != 0) {
            if (this->unk_167 < 0) {
                if ((play->state.frames % 2) != 0) {
                    this->unk_167++;
                }
            } else {
                this->unk_167--;
            }
        }
    } else if (func_808A1478(this, play, 0.0f)) {
        u8 doorType = this->doorType;

        if ((doorType != DOORSHUTTER_TYPE_0) && (doorType != DOORSHUTTER_TYPE_1) &&
            (((doorType != DOORSHUTTER_TYPE_7) && (doorType != DOORSHUTTER_TYPE_6)) || !func_808A0900(this, play))) {
            DoorShutter_SetupAction(this, func_808A1618);
        } else {
            DoorShutter_SetupAction(this, func_808A1090);
        }
        Audio_RestorePrevBgm();
    }
}

void func_808A1884(DoorShutter* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s8 room = this->slidingDoor.dyna.actor.room;
    Vec3f sp44;

    if (this->slidingDoor.dyna.actor.room >= 0) {
        Actor_WorldToActorCoords(&this->slidingDoor.dyna.actor, &sp44, &player->actor.world.pos);

        this->slidingDoor.dyna.actor.room =
            play->transitionActors.list[DOOR_GET_TRANSITION_ID(&this->slidingDoor.dyna.actor)]
                .sides[(sp44.z < 0.0f) ? 0 : 1]
                .room;

        if (room != this->slidingDoor.dyna.actor.room) {
            Room temp = play->roomCtx.curRoom;

            play->roomCtx.curRoom = play->roomCtx.prevRoom;
            play->roomCtx.prevRoom = temp;
            play->roomCtx.activeBufPage ^= 1;
        }
        Room_FinishRoomChange(play, &play->roomCtx);
    }

    this->slidingDoor.unk_15C = 0;
    this->slidingDoor.dyna.actor.velocity.y = 0.0f;

    if (DoorShutter_SetupDoor(this, play) && !(player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        DoorShutter_SetupAction(this, func_808A1C50);
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_DOOR) {
            func_801226E0(play, ((void)0, gSaveContext.respawn[RESPAWN_MODE_DOWN].data));
            player->csId = CS_ID_NONE;
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_115);
        }
    }
}

s32 func_808A1A70(DoorShutter* this) {
    if (this->unk_163 == 7) {
        if (this->unk_163 == 7) {
            Actor_PlaySfx_Flagged(&this->slidingDoor.dyna.actor, NA_SE_EV_IKANA_DOOR_CLOSE - SFX_FLAG);
        }

        Math_StepToF(&this->slidingDoor.dyna.actor.velocity.y, 5.0f, 0.5f);

        if (Math_Vec3f_StepToXZ(&this->slidingDoor.dyna.actor.world.pos, &this->slidingDoor.dyna.actor.home.pos,
                                this->slidingDoor.dyna.actor.velocity.y) == 0.0f) {
            return true;
        }
    } else {
        if (this->slidingDoor.dyna.actor.velocity.y < 20.0f) {
            Math_StepToF(&this->slidingDoor.dyna.actor.velocity.y, 20.0f, 8.0f);
        }

        if (Math_StepToF(&this->slidingDoor.dyna.actor.world.pos.y, this->slidingDoor.dyna.actor.home.pos.y,
                         this->slidingDoor.dyna.actor.velocity.y)) {
            return true;
        }
    }
    return false;
}

void func_808A1B48(DoorShutter* this, PlayState* play) {
    s16 quakeIndex;

    if (func_808A1A70(this)) {
        if (this->slidingDoor.dyna.actor.velocity.y > 20.0f) {
            this->slidingDoor.dyna.actor.floorHeight = this->slidingDoor.dyna.actor.home.pos.y;
            Actor_SpawnFloorDustRing(play, &this->slidingDoor.dyna.actor, &this->slidingDoor.dyna.actor.world.pos,
                                     45.0f, 10, 8.0f, 500, 10, false);
        }
        Actor_PlaySfx(&this->slidingDoor.dyna.actor, NA_SE_EV_BIGWALL_BOUND);

        quakeIndex = Quake_Request(Play_GetCamera(play, CAM_ID_MAIN), QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, -32536);
        Quake_SetPerturbations(quakeIndex, 2, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 10);

        Rumble_Request(this->slidingDoor.dyna.actor.xyzDistToPlayerSq, 180, 20, 100);

        func_808A1884(this, play);
    }
}

void func_808A1C50(DoorShutter* this, PlayState* play) {
    if (this->unk_167++ > 30) {
        if (GET_PLAYER(play)->csAction == PLAYER_CSACTION_115) {
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
        }
        DoorShutter_SetupDoor(this, play);
    }
}

void DoorShutter_Update(Actor* thisx, PlayState* play) {
    DoorShutter* this = (DoorShutter*)thisx;
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_400 | PLAYER_STATE1_10000000)) ||
        (this->actionFunc == DoorShutter_SetupType)) {
        this->actionFunc(this, play);

        if (this->unk_163 == 7) {
            this->slidingDoor.dyna.actor.home.rot.z =
                Math_Vec3f_DistXZ(&this->slidingDoor.dyna.actor.home.pos, &this->slidingDoor.dyna.actor.world.pos) *
                -100.0f;
        }
    }
}

s32 func_808A1D68(DoorShutter* this, PlayState* play) {
    s32 temp_a0;
    s32 temp_a1;

    if (Player_InCsMode(play)) {
        return true;
    }

    temp_a0 = BINANG_SUB(Actor_WorldYawTowardPoint(&this->slidingDoor.dyna.actor, &play->view.eye),
                         this->slidingDoor.dyna.actor.shape.rot.y);
    temp_a1 = BINANG_SUB(this->slidingDoor.dyna.actor.yawTowardsPlayer, this->slidingDoor.dyna.actor.shape.rot.y);

    temp_a0 = ABS_ALT(temp_a0);
    temp_a1 = ABS_ALT(temp_a1);

    if (((temp_a1 < 0x4000) && (temp_a0 > 0x4000)) || ((temp_a1 > 0x4000) && (temp_a0 < 0x4000))) {
        return false;
    }
    return true;
}

void DoorShutter_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DoorShutter* this = (DoorShutter*)thisx;
    ShutterInfo* sp44;

    if (func_808A1D68(this, play)) {
        sp44 = &D_808A21B0[this->unk_164];

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        if (this->unk_164 == 7) {
            Matrix_Translate(0.0f, 64.96f, 0.0f, MTXMODE_APPLY);
            Matrix_RotateZS(this->slidingDoor.dyna.actor.home.rot.z, MTXMODE_APPLY);
            Matrix_Translate(0.0f, -64.96f, 0.0f, MTXMODE_APPLY);
        }

        if (sp44->unk_04 != 0) {
            TransitionActorEntry* transitionEntry =
                &play->transitionActors.list[DOOR_GET_TRANSITION_ID(&this->slidingDoor.dyna.actor)];

            if ((play->roomCtx.prevRoom.num >= 0) ||
                (transitionEntry->sides[0].room == transitionEntry->sides[1].room)) {
                s16 yaw = this->slidingDoor.dyna.actor.shape.rot.y -
                          Math_Vec3f_Yaw(&play->view.eye, &this->slidingDoor.dyna.actor.world.pos);

                if (ABS_ALT(yaw) < 0x4000) {
                    Matrix_RotateYF(M_PIf, MTXMODE_APPLY);
                }
            } else if (this->slidingDoor.dyna.actor.room == transitionEntry->sides[0].room) {
                Matrix_RotateYF(M_PIf, MTXMODE_APPLY);
            }
        } else if (this->doorType == DOORSHUTTER_TYPE_BOSS_DOOR) {
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_808A22DC[this->slidingDoor.unk_15E]));
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, sp44->unk_00);

        if ((this->unk_168 != 0.0f) && (sp44->unk_04 != 0)) {
            Matrix_Translate(0.0f, sp44->unk_08 * (1.0f - this->unk_168), sp44->translateZ, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, sp44->unk_04);
        }

        if (this->unk_166 != 0) {
            Matrix_Scale(0.01f, 0.01f, 0.025f, MTXMODE_APPLY);
            Actor_DrawDoorLock(play, this->unk_166,
                               (this->doorType == DOORSHUTTER_TYPE_BOSS_DOOR) ? DOORLOCK_BOSS : DOORLOCK_NORMAL);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

#include "global.h"
#include "vt.h"

s32 func_8006CFC0(s32 scene) {
    s32 validScenes[] = { SCENE_SPOT00, SCENE_SPOT06, SCENE_SPOT09, SCENE_SPOT12, SCENE_SPOT20 };
    s32 i;

    for (i = 0; i < ARRAY_COUNT(validScenes); i++) {
        if (scene == validScenes[i]) {
            return 1;
        }
    }

    return 0;
}

void func_8006D074(PlayState* play) {
    gSaveContext.horseData.scene = SCENE_SPOT00;
    gSaveContext.horseData.pos.x = -1840;
    gSaveContext.horseData.pos.y = 72;
    gSaveContext.horseData.pos.z = 5497;
    gSaveContext.horseData.angle = -27353;
}

void func_8006D0AC(PlayState* play) {
    if (gSaveContext.horseData.scene == SCENE_SPOT06) {
        gSaveContext.horseData.scene = SCENE_SPOT06;
        gSaveContext.horseData.pos.x = -2065;
        gSaveContext.horseData.pos.y = -863;
        gSaveContext.horseData.pos.z = 1839;
        gSaveContext.horseData.angle = 0;
    }
}

typedef struct {
    /* 0x00 */ s16 scene;
    /* 0x02 */ Vec3s pos;
    /* 0x08 */ s16 angle;
    /* 0x0A */ s16 type;
} HorseSpawn;

void func_8006D0EC(PlayState* play, Player* player) {
    s32 i;
    HorseSpawn horseSpawns[] = {
        { SCENE_SPOT00, -460, 100, 6640, 0, 2 }, { SCENE_SPOT06, -1929, -1025, 768, 0, 2 },
        { SCENE_SPOT09, 2566, -259, 767, 0, 2 }, { SCENE_SPOT12, -328, 10, 953, 0, 2 },
        { SCENE_SPOT20, 928, 0, -2280, 0, 2 },
    };

    if ((AREG(6) != 0) && (Flags_GetEventChkInf(0x18) || (DREG(1) != 0))) {
        player->rideActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, player->actor.world.pos.x,
                                        player->actor.world.pos.y, player->actor.world.pos.z, player->actor.shape.rot.x,
                                        player->actor.shape.rot.y, player->actor.shape.rot.z, 9, true);

        ASSERT(player->rideActor != NULL);

        Actor_MountHorse(play, player, player->rideActor);
        func_8002DE74(play, player);
        gSaveContext.horseData.scene = play->sceneNum;

        if (play->sceneNum == SCENE_SPOT12) {
            player->rideActor->room = -1;
        }
    } else if ((play->sceneNum == SCENE_SPOT12) && (gSaveContext.minigameState == 3)) {
        Actor* horseActor;
        gSaveContext.minigameState = 0;
        horseActor =
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, 3586.0f, 1413.0f, -402.0f, 0, 0x4000, 0, 1, true);
        horseActor->room = -1;
    } else if ((gSaveContext.entranceIndex == 1230) && (gSaveContext.eventChkInf[1] & 0x100)) {
        Actor* horseActor =
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, -25.0f, 0.0f, -1600.0f, 0, -0x4000, 0, 1, true);
        ASSERT(horseActor != NULL);
    } else if ((play->sceneNum == gSaveContext.horseData.scene) &&
               (((Flags_GetEventChkInf(0x18) != 0) && (!gSaveContext.n64ddFlag ||
               (gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_SONG_EPONA) &&
               (INV_CONTENT(ITEM_OCARINA_FAIRY) != ITEM_NONE)))) || DREG(1) != 0)) {
        // "Set by existence of horse %d %d %d"
        osSyncPrintf("馬存在によるセット %d %d %d\n", gSaveContext.horseData.scene, Flags_GetEventChkInf(0x18),
                     DREG(1));

        if (func_8006CFC0(gSaveContext.horseData.scene)) {
            Actor* horseActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE,
                                            gSaveContext.horseData.pos.x, gSaveContext.horseData.pos.y,
                                            gSaveContext.horseData.pos.z, 0, gSaveContext.horseData.angle, 0, 1, true);
            ASSERT(horseActor != NULL);
            if (play->sceneNum == SCENE_SPOT12) {
                horseActor->room = -1;
            }
        } else {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "Horse_SetNormal():%d set spot is no good."
            osSyncPrintf("Horse_SetNormal():%d セットスポットまずいです。\n", gSaveContext.horseData.scene);
            osSyncPrintf(VT_RST);
            func_8006D074(play);
        }
    } else if ((play->sceneNum == SCENE_SPOT20) && !Flags_GetEventChkInf(0x18) && (DREG(1) == 0)) {
        Actor* horseActor =
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, 0.0f, 0.0f, -500.0f, 0, 0, 0, 1, true);
        ASSERT(horseActor != NULL);
    } else if (Flags_GetEventChkInf(0x18) || (DREG(1) != 0)) {
        for (i = 0; i < ARRAY_COUNT(horseSpawns); i++) {
            HorseSpawn* horseSpawn = &horseSpawns[i];
            if (horseSpawn->scene == play->sceneNum) {
                Actor* horseActor =
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, horseSpawn->pos.x, horseSpawn->pos.y,
                                horseSpawn->pos.z, 0, horseSpawn->angle, 0, horseSpawn->type, true);
                ASSERT(horseActor != NULL);
                if (play->sceneNum == SCENE_SPOT12) {
                    horseActor->room = -1;
                }

                break;
            }
        }
    } else if (!Flags_GetEventChkInf(0x18)) {
        if ((DREG(1) == 0) && (play->sceneNum == SCENE_SOUKO) && !IS_DAY) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, 0.0f, 0.0f, -60.0f, 0, 0x7360, 0, 1, true);
        }
    }
}

typedef struct {
    /* 0x00 */ s16 scene;
    /* 0x04 */ s32 cutsceneIndex;
    /* 0x08 */ Vec3s pos;
    /* 0x0E */ s16 angle;
    /* 0x10 */ s16 type;
} struct_8011F9B8;

void func_8006D684(PlayState* play, Player* player) {
    s32 pad;
    s32 i;
    Vec3s spawnPos;

    if ((gSaveContext.entranceIndex == 0x028A || gSaveContext.entranceIndex == 0x028E ||
         gSaveContext.entranceIndex == 0x0292 || gSaveContext.entranceIndex == 0x0476) &&
        (gSaveContext.respawnFlag == 0)) {
        Vec3s spawnPositions[] = {
            { 0xF46F, 0x0139, 0x1E14 },
            { 0xF894, 0x0139, 0x1B67 },
            { 0xF035, 0x0139, 0x1B15 },
            { 0xF6F7, 0x0139, 0x1766 },
        };

        if (gSaveContext.entranceIndex == 0x028A) {
            spawnPos = spawnPositions[0];
        } else if (gSaveContext.entranceIndex == 0x028E) {
            spawnPos = spawnPositions[1];
        } else if (gSaveContext.entranceIndex == 0x0292) {
            spawnPos = spawnPositions[2];
        } else {
            spawnPos = spawnPositions[3];
        }

        player->rideActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, spawnPos.x, spawnPos.y,
                                        spawnPos.z, 0, player->actor.world.rot.y, 0, 7, true);
        ASSERT(player->rideActor != NULL);

        Actor_MountHorse(play, player, player->rideActor);
        func_8002DE74(play, player);
        gSaveContext.horseData.scene = play->sceneNum;
    } else if ((play->sceneNum == SCENE_SPOT20) && ((gSaveContext.eventInf[0] & 0xF) == 6) &&
               (Flags_GetEventChkInf(0x18) == 0) && (DREG(1) == 0)) {
        player->rideActor =
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, 894.0f, 0.0f, -2084.0f, 0, -0x7FFF, 0, 5, true);
        ASSERT(player->rideActor != NULL);

        Actor_MountHorse(play, player, player->rideActor);
        func_8002DE74(play, player);
        gSaveContext.horseData.scene = play->sceneNum;

        if (play->sceneNum == SCENE_SPOT12) {
            player->rideActor->room = -1;
        }
    } else {
        static struct_8011F9B8 D_8011F9B8[] = {
            { 93, 0xFFF0, 0x0E10, 0x0585, 0x0168, 0x8001, 8 }, { 99, 0xFFF0, 0xFF06, 0x0001, 0xF9D4, 0x4000, 6 },
            { 99, 0xFFF1, 0x0000, 0x0000, 0x0000, 0x0000, 5 }, { 99, 0xFFF5, 0x0000, 0x0000, 0x0000, 0x0000, 7 },
            { 81, 0xFFF3, 0xF46F, 0x0139, 0x1E14, 0x0000, 7 }, { 81, 0xFFF4, 0xF894, 0x0139, 0x1B67, 0x0000, 7 },
            { 81, 0xFFF5, 0xF035, 0x0139, 0x1B15, 0x0000, 7 }, { 81, 0xFFF6, 0xF035, 0x0139, 0x1B15, 0x0000, 7 },
        };

        for (i = 0; i < ARRAY_COUNT(D_8011F9B8); i++) {
            if ((play->sceneNum == D_8011F9B8[i].scene) &&
                (((void)0, gSaveContext.cutsceneIndex) == D_8011F9B8[i].cutsceneIndex)) {
                if (D_8011F9B8[i].type == 7) {
                    if ((play->sceneNum == 99) && (((void)0, gSaveContext.cutsceneIndex) == 0xFFF1)) {
                        D_8011F9B8[i].pos.x = player->actor.world.pos.x;
                        D_8011F9B8[i].pos.y = player->actor.world.pos.y;
                        D_8011F9B8[i].pos.z = player->actor.world.pos.z;
                    }

                    player->rideActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE,
                                                    D_8011F9B8[i].pos.x, D_8011F9B8[i].pos.y, D_8011F9B8[i].pos.z, 0,
                                                    player->actor.world.rot.y, 0, D_8011F9B8[i].type, true);
                    ASSERT(player->rideActor != NULL);

                    Actor_MountHorse(play, player, player->rideActor);
                    func_8002DE74(play, player);
                } else if ((D_8011F9B8[i].type == 5) || (D_8011F9B8[i].type == 6) || (D_8011F9B8[i].type == 8)) {
                    Vec3f sp54;
                    s32 temp = 0;

                    if (((gSaveContext.eventInf[0] & 0x10) >> 4) && D_8011F9B8[i].type == 6) {
                        temp = 0x8000;
                    }

                    player->rideActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE,
                                                    D_8011F9B8[i].pos.x, D_8011F9B8[i].pos.y, D_8011F9B8[i].pos.z, 0,
                                                    D_8011F9B8[i].angle, 0, D_8011F9B8[i].type | temp, true);
                    ASSERT(player->rideActor != NULL);

                    player->actor.world.pos.x = D_8011F9B8[i].pos.x;
                    player->actor.world.pos.y = D_8011F9B8[i].pos.y;
                    player->actor.world.pos.z = D_8011F9B8[i].pos.z;
                    player->actor.shape.rot.x = player->actor.shape.rot.z = 0;
                    player->actor.shape.rot.y = D_8011F9B8[i].angle;

                    Actor_MountHorse(play, player, player->rideActor);
                    func_8002DE74(play, player);

                    sp54.x = player->actor.world.pos.x - 200.0f;
                    sp54.y = player->actor.world.pos.y + 100.0f;
                    sp54.z = player->actor.world.pos.z;

                    Play_CameraSetAtEye(play, play->activeCamera, &player->actor.world.pos, &sp54);
                } else {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, D_8011F9B8[i].pos.x,
                                D_8011F9B8[i].pos.y, D_8011F9B8[i].pos.z, 0, D_8011F9B8[i].angle, 0,
                                D_8011F9B8[i].type, true);
                }
                break;
            }
        }
    }
}

void func_8006DC68(PlayState* play, Player* player) {
    if (LINK_IS_ADULT) {
        if (!func_8006CFC0(gSaveContext.horseData.scene)) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "Horse_Set_Check():%d set spot is no good."
            osSyncPrintf("Horse_Set_Check():%d セットスポットまずいです。\n", gSaveContext.horseData.scene);
            osSyncPrintf(VT_RST);
            func_8006D074(play);
        }

        if (func_8006CFC0(play->sceneNum)) {
            if ((gSaveContext.sceneSetupIndex > 3) ||
                ((gSaveContext.entranceIndex == 0x028A || gSaveContext.entranceIndex == 0x028E ||
                  gSaveContext.entranceIndex == 0x0292 || gSaveContext.entranceIndex == 0x0476) &&
                 (gSaveContext.respawnFlag == 0)) ||
                ((play->sceneNum == SCENE_SPOT20) && ((gSaveContext.eventInf[0] & 0xF) == 6) &&
                 !Flags_GetEventChkInf(0x18) && (DREG(1) == 0))) {
                func_8006D684(play, player);
            } else {
                func_8006D0EC(play, player);
            }
        }
    }
}

void func_8006DD9C(Actor* actor, Vec3f* arg1, s16 arg2) {
    s16 x = Math_Vec3f_Yaw(&actor->world.pos, arg1) - actor->world.rot.y;

    if (x > arg2) {
        actor->world.rot.y += arg2;
    } else if (x < -arg2) {
        actor->world.rot.y -= arg2;
    } else {
        actor->world.rot.y += x;
    }

    actor->shape.rot.y = actor->world.rot.y;
}

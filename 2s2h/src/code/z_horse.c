#include "global.h"
#include "z64horse.h"
#include "overlays/actors/ovl_Bg_Umajump/z_bg_umajump.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

s32 Horse_GetJumpingFencePathIndex(PlayState* play) {
    Actor* actor;

    for (actor = NULL;; actor = actor->next) {
        actor = SubS_FindActor(play, actor, ACTORCAT_PROP, ACTOR_BG_UMAJUMP);

        if (actor == NULL) {
            break;
        }

        if (actor->params == BG_UMAJUMP_TYPE_2) {
            return ((BgUmajump*)actor)->pathIndex;
        }
    }

    return -1;
}

// unused
s32 Horse_CopyPointFromPathList(PlayState* play, s32 pathIndex, s32 pointIndex, Vec3s* dst, s16* arg4) {
    Path* path = &play->setupPathList[pathIndex];
    Vec3s* points;
    s32 count = path->count;

    dst->x = 0;
    dst->y = 0;
    dst->z = 0;
    *arg4 = 0;

    if (count == 0) {
        return false;
    }
    if (pointIndex >= count) {
        return false;
    }

    points = Lib_SegmentedToVirtual(path->points);
    points += pointIndex;

    dst->x = points->x;
    dst->y = points->y;
    dst->z = points->z;
    *arg4 = 0;

    return true;
}

typedef struct HorseValidScene {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ s16 sceneLayerMinusOne;
} HorseValidScene; // size = 0x4

HorseValidScene sHorseValidScenes[] = {
    { SCENE_00KEIKOKU, 0 },      // Termina Field
    { SCENE_24KEMONOMITI, 0 },   // Road to southern swap
    { SCENE_F01, 0 },            // Romani ranch
    { SCENE_KOEPONARACE, 0 },    // Gorman track
    { SCENE_20SICHITAI, 0 },     // Southern swamp poisoned
    { SCENE_20SICHITAI2, 0 },    // Souther swamp clear
    { SCENE_30GYOSON, 0 },       // Great bay coast
    { SCENE_31MISAKI, 0 },       // Zora cape
    { SCENE_ROMANYMAE, 0 },      // Milk road
    { SCENE_IKANAMAE, 0 },       // Road to Ikana
    { SCENE_13HUBUKINOMITI, 0 }, // Path to Mountain Village
};

s32 Horse_IsValidSpawn(s16 sceneId) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sHorseValidScenes); i++) {
        if (sceneId == sHorseValidScenes[i].sceneId) {
            return true;
        }
    }

    return false;
}

void Horse_ResetHorseData(PlayState* play) {
    gSaveContext.save.saveInfo.horseData.sceneId = SCENE_F01;
    gSaveContext.save.saveInfo.horseData.pos.x = -1420;
    gSaveContext.save.saveInfo.horseData.pos.y = 257;
    gSaveContext.save.saveInfo.horseData.pos.z = -1285;
    gSaveContext.save.saveInfo.horseData.yaw = 0x2AAA;
}

s32 gHorseIsMounted = false;
s32 D_801BDAA0 = false;
s32 gHorsePlayedEponasSong = false;

HorseValidScene sHorseValidSceneLayers[] = {
    { SCENE_00KEIKOKU, 5 - 1 },      // Termina Field - First Cycle
    { SCENE_30GYOSON, 1 - 1 },       // Great Bay Coast - Post-Gyorg
    { SCENE_31MISAKI, 1 - 1 },       // Zora Cape - Post-Gyorg
    { SCENE_13HUBUKINOMITI, 1 - 1 }, // Path to Mountain Village - Post-Goht
};

s32 Horse_IsValidSceneLayer(PlayState* play, Player* player) {
    s32 i;

    if (gSaveContext.sceneLayer == 0) {
        return true;
    }

    for (i = 0; i < ARRAY_COUNT(sHorseValidSceneLayers); i++) {
        if ((sHorseValidSceneLayers[i].sceneId == play->sceneId) &&
            (gSaveContext.sceneLayer == sHorseValidSceneLayers[i].sceneLayerMinusOne + 1)) {
            return true;
        }
    }
    return false;
}

void Horse_SpawnOverworld(PlayState* play, Player* player) {
    if (!Horse_IsValidSceneLayer(play, player)) {
        return;
    }

    if (gHorseIsMounted && CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        s32 pad;
        Vec3f pos = player->actor.world.pos;
        f32 yIntersect;
        CollisionPoly* poly;
        s32 pad2[3];

        pos.y += 5.0f;
        yIntersect = BgCheck_EntityRaycastFloor1(&play->colCtx, &poly, &pos);
        if (yIntersect == BGCHECK_Y_MIN) {
            yIntersect = player->actor.world.pos.y;
        }

        player->rideActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, player->actor.world.pos.x, yIntersect,
                                        player->actor.world.pos.z, player->actor.shape.rot.x, player->actor.shape.rot.y,
                                        player->actor.shape.rot.z, ENHORSE_PARAMS(ENHORSE_PARAM_4000, ENHORSE_11));
        Player_MountHorse(play, player, player->rideActor);
        Player_SetCameraHorseSetting(play, player);
    } else if ((play->sceneId == gSaveContext.save.saveInfo.horseData.sceneId) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        if (Horse_IsValidSpawn(gSaveContext.save.saveInfo.horseData.sceneId)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, gSaveContext.save.saveInfo.horseData.pos.x,
                        gSaveContext.save.saveInfo.horseData.pos.y, gSaveContext.save.saveInfo.horseData.pos.z, 0,
                        gSaveContext.save.saveInfo.horseData.yaw, 0, ENHORSE_PARAMS(ENHORSE_PARAM_4000, ENHORSE_1));
        } else {
            Horse_ResetHorseData(play);
        }
    } else if ((play->sceneId == SCENE_F01) && !CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, -1420.0f, 257.0f, -1285.0f, 0, 0x2AAA, 0,
                    ENHORSE_PARAMS(ENHORSE_PARAM_4000, ENHORSE_1));
    } else if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && Horse_IsValidSpawn(play->sceneId)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, player->actor.world.pos.x, player->actor.world.pos.y,
                    player->actor.world.pos.z, 0, player->actor.shape.rot.y, 0,
                    ENHORSE_PARAMS(ENHORSE_PARAM_4000, ENHORSE_2));
    }
}

void Horse_SpawnMinigame(PlayState* play, Player* player) {
    if ((play->sceneId == SCENE_KOEPONARACE) &&
        (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_START)) {
        player->rideActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, -1262.0f, -106.0f, 470.0f, 0, 0x7FFF, 0,
                                        ENHORSE_PARAMS(ENHORSE_PARAM_4000, ENHORSE_13));
        Player_MountHorse(play, player, player->rideActor);
        Player_SetCameraHorseSetting(play, player);
    } else if ((play->sceneId == SCENE_KOEPONARACE) &&
               ((GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_3) ||
                (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_2))) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, -1741.0f, -106.0f, -641.0f, 0, -0x4FA4, 0,
                    ENHORSE_PARAMS(ENHORSE_PARAM_4000, ENHORSE_1));
    } else if ((gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 0)) && (Cutscene_GetSceneLayer(play) != 0) &&
               (player->transformation == PLAYER_FORM_HUMAN)) {
        player->rideActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, -1106.0f, 260.0f, -1185.0f, 0, 0x13, 0,
                                        ENHORSE_PARAMS(ENHORSE_PARAM_4000, ENHORSE_7));
        Player_MountHorse(play, player, player->rideActor);
        Player_SetCameraHorseSetting(play, player);
    }
}

void Horse_Spawn(PlayState* play, Player* player) {
    if (((play->sceneId == SCENE_KOEPONARACE) &&
         (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_START)) ||
        ((play->sceneId == SCENE_F01) && (((gSaveContext.sceneLayer == 1)) || (gSaveContext.sceneLayer == 5)) &&
         (player->transformation == PLAYER_FORM_HUMAN)) ||
        ((play->sceneId == SCENE_KOEPONARACE) &&
         (((GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_3)) ||
          (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_2)))) {
        // Gorman Track and horse state is either STATE_START, STATE_2 or STATE_3
        // or Romani Ranch, Player is Human and scene layer is either 1 or 5
        Horse_SpawnMinigame(play, player);
    } else {
        Horse_SpawnOverworld(play, player);
    }

    D_801BDAA0 = false;
}

void Horse_RotateToPoint(Actor* actor, Vec3f* pos, s16 turnYaw) {
    s16 yaw = Math_Vec3f_Yaw(&actor->world.pos, pos) - actor->world.rot.y;

    if (yaw > turnYaw) {
        actor->world.rot.y += turnYaw;
    } else if (yaw < -turnYaw) {
        actor->world.rot.y -= turnYaw;
    } else {
        actor->world.rot.y += yaw;
    }

    actor->shape.rot.y = actor->world.rot.y;
}

s32 Horse_IsActive(PlayState* play, ActorContext* actorCtx) {
    Actor* bgActor = actorCtx->actorLists[ACTORCAT_BG].first;

    if (bgActor != NULL) {
        while (true) {
            if ((bgActor->update != NULL) && (bgActor->init == NULL)) {
                if (Object_IsLoaded(&play->objectCtx, bgActor->objectSlot)) {
                    if ((bgActor->id == ACTOR_EN_HORSE) && (((EnHorse*)bgActor)->action != ENHORSE_ACTION_INACTIVE)) {
                        return true;
                    }
                }
            }
            bgActor = bgActor->next;
            if (bgActor == NULL) {
                break;
            }
        }
    }

    return false;
}

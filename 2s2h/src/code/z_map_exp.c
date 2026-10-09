#include "global.h"

#include "2s2h/GameInteractor/GameInteractor.h"

s16 sPlayerInitPosX = 0;
s16 sPlayerInitPosZ = 0;
s16 sPlayerInitDir = 0;

/**
 * Gets the mapIndex for the current dungeon scene
 * @return the current scene's DungeonSceneIndex, or -1 if it isn't a dungeon or boss scene.
 */
s32 Map_GetDungeonSceneIndex(PlayState* play) {
    static s32 sDungeonAndBossSceneIds[] = {
        SCENE_MITURIN,    // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE
        SCENE_HAKUGIN,    // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE
        SCENE_SEA,        // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE
        SCENE_INISIE_N,   // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
        SCENE_INISIE_R,   // Play_GetOriginalSceneId converts play->sceneId to SCENE_INISIE_N, returning
                          // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
        SCENE_MITURIN_BS, // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE_BOSS
        SCENE_HAKUGIN_BS, // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE_BOSS
        SCENE_SEA_BS,     // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE_BOSS
        SCENE_INISIE_BS,  // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE_BOSS
    };
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sDungeonAndBossSceneIds); i++) {
        if (Play_GetOriginalSceneId(play->sceneId) == sDungeonAndBossSceneIds[i]) {
            return i;
        }
    }

    return -1;
}

/**
 * Tests if the current scene is in a dungeon or boss scene.
 * @return true if the current scene is a dungeon or boss scene, false otherwise.
 */
s32 Map_IsInDungeonOrBossScene(PlayState* play) {
    if (Map_GetDungeonSceneIndex(play) == -1) {
        return false;
    }

    return true;
}

s32 Map_CurRoomHasMapI(PlayState* play) {
    if ((Map_GetDungeonSceneIndex(play) == -1) || !MapDisp_CurRoomHasMapI(play)) {
        return false;
    }

    return true;
}

/**
 * Tests if the current scene is a dungeon scene, excluding boss scenes.
 * @return -1 if not a dungeon scene, else returns the DungeonSceneIndex of the dungeon scene
 */
s32 Map_GetDungeonSceneIndexForDungeon(PlayState* play) {
    static s32 sDungeonSceneIds[] = {
        SCENE_MITURIN,  // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE
        SCENE_HAKUGIN,  // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE
        SCENE_SEA,      // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE
        SCENE_INISIE_N, // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
        SCENE_INISIE_R, // Play_GetOriginalSceneId converts play->sceneId to SCENE_INISIE_N, returning
                        // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
    };
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sDungeonSceneIds); i++) {
        if (Play_GetOriginalSceneId(play->sceneId) == sDungeonSceneIds[i]) {
            return i;
        }
    }

    return -1;
}

/**
 * Test if the current scene is a dungeon scene, excluding boss rooms
 * @return true if the current scene is a dungeon scene, false otherwise.
 */
s32 Map_IsInDungeonScene(PlayState* play) {
    if (Map_GetDungeonSceneIndexForDungeon(play) == -1) {
        return false;
    }

    return true;
}

/**
 * Tests if the current scene is a boss scene.
 * @return -1 if not a boss scene, else returns the DungeonSceneIndex corresponding to that boss scene's primary dungeon
 * scene
 */
s32 Map_GetDungeonSceneIndexForBoss(PlayState* play) {
    static s32 sBossSceneIds[] = {
        SCENE_MITURIN_BS, // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE
        SCENE_HAKUGIN_BS, // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE
        SCENE_SEA_BS,     // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE
        SCENE_INISIE_BS,  // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
    };
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sBossSceneIds); i++) {
        if (Play_GetOriginalSceneId(play->sceneId) == sBossSceneIds[i]) {
            return i;
        }
    }

    return -1;
}

/**
 * Checks if the current scene is a boss scene.
 * @return true if the current scene is a boss scene, false otherwise.
 */
s32 Map_IsInBossScene(PlayState* play) {
    if (Map_GetDungeonSceneIndexForBoss(play) == -1) {
        return false;
    }

    return true;
}

/**
 * Intended to check if the current scene is an overworld scene with a minimap.
 * The implementation is incomplete due to a complete rewrite of z_map_exp.c and other map systems, making the function
 * obsolete.
 * @return -1 if not in the list, else returns the MapIndex for the overworld scene
 */
s32 Map_GetMapIndexForOverworld(PlayState* play) {
    static s32 sSceneIds[] = {
        SCENE_22DEKUCITY,
        SCENE_KOEPONARACE,
        SCENE_F01,
    };
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sSceneIds); i++) {
        if (Play_GetOriginalSceneId(play->sceneId) == sSceneIds[i]) {
            return i;
        }
    }

    return -1;
}

/**
 * Intended to check if the current scene is an overworld scene with a minimap.
 * The implementation is incomplete due to a complete rewrite of z_map_exp.c and other map systems, making the function
 * obsolete.
 * @return true if the current scene is in the set, false otherwise.
 */
s32 Map_IsInOverworldSceneWithMapIndex(PlayState* play) {
    if (Map_GetMapIndexForOverworld(play) == -1) {
        return false;
    }

    return true;
}

/**
 * Sets the position and rotation of where the player has entered the area.
 * Used to draw the red marker on the minimap.
 */
void Map_SetAreaEntrypoint(PlayState* play) {
    Player* player = GET_PLAYER(play);

    sPlayerInitPosX = player->actor.world.pos.x;
    sPlayerInitPosZ = player->actor.world.pos.z;
    sPlayerInitDir = (0x7FFF - player->actor.shape.rot.y) / 0x400;
}

void Map_InitRoomData(PlayState* play, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    MapDisp_SwapRooms(room);

    if (room >= 0) {
        if (Map_IsInDungeonOrBossScene(play)) {
            SET_ROOM_VISITED(Play_GetOriginalSceneId(play->sceneId), room);
            interfaceCtx->mapRoomNum = room;
            interfaceCtx->dungeonSceneIndex = mapIndex;
        }
    } else {
        interfaceCtx->mapRoomNum = 0;
    }

    if (gSaveContext.sunsSongState != SUNSSONG_SPEED_TIME) {
        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
    }
}

void Map_Destroy(PlayState* play) {
    MapDisp_Destroy(play);
}

void Map_Init(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s32 dungeonSceneSharedIndex;

    MapDisp_SwapRooms(play->roomCtx.curRoom.num);
    interfaceCtx->unk_278 = -1;
    interfaceCtx->dungeonSceneIndex = -1;
    interfaceCtx->mapSegment = THA_AllocTailAlign16(&play->state.tha, 0x1000);

    //! This block does pretty much nothing, as z_map_exp.c and other map systems were heavily rewritten after OoT to no
    //! longer need mapIndex to retrieve minimap data.
    if (Map_IsInOverworldSceneWithMapIndex(play)) {
        gSaveContext.mapIndex = Map_GetMapIndexForOverworld(play);
        return;
    }

    if (Map_IsInDungeonOrBossScene(play)) {
        gSaveContext.mapIndex = dungeonSceneSharedIndex = Map_GetDungeonSceneIndex(play);
        switch (play->sceneId) {
            case SCENE_MITURIN_BS:
                dungeonSceneSharedIndex = DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE;
                break;

            case SCENE_HAKUGIN_BS:
                dungeonSceneSharedIndex = DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE;
                break;

            case SCENE_SEA_BS:
                dungeonSceneSharedIndex = DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE;
                break;

            case SCENE_INISIE_BS:
                dungeonSceneSharedIndex = DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE;
                break;
        }

        gSaveContext.dungeonSceneSharedIndex = dungeonSceneSharedIndex;
        Map_InitRoomData(play, play->roomCtx.curRoom.num);
    }
}

void Map_DrawMinimap(PlayState* play) {
    MapDisp_DrawMinimap(play, sPlayerInitPosX, sPlayerInitPosZ, sPlayerInitDir);
}

s16 sLastRoomNum = 99;

void Map_Update(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    Input* controller = CONTROLLER1(&play->state);
    s32 pad1;
    s16 floor;
    s32 pad2;

    if ((play->pauseCtx.state <= PAUSE_STATE_OPENING_2) && (CHECK_BTN_ALL(controller->press.button, BTN_L)) &&
        !Play_InCsMode(play) && !MapDisp_IsMinimapToggleBlocked(play)) {
        if (GameInteractor_Should(VB_MINIMAP_TOGGLE, true)) {
            if (!R_MINIMAP_DISABLED) {
                Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_UP);
            } else {
                Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_DOWN);
            }
            R_MINIMAP_DISABLED ^= 1;
        }
    }

    MapDisp_Update(play);

    if (!IS_PAUSED(&play->pauseCtx)) {
        if (Map_IsInDungeonScene(play)) {
            floor = MapDisp_GetPlayerStorey(player->actor.world.pos.y);
            if (floor != -1) {
                SET_DUNGEON_FLOOR_VISITED(Play_GetOriginalSceneId(play->sceneId), FLOOR_INDEX_MAX - floor);
                R_PLAYER_FLOOR_REVERSE_INDEX = FLOOR_INDEX_MAX - floor;
                if (interfaceCtx->mapRoomNum != sLastRoomNum) {
                    sLastRoomNum = interfaceCtx->mapRoomNum;
                }
            }
        } else if (Map_IsInBossScene(play)) {
            MapDisp_GetBossIconY();
            R_PLAYER_FLOOR_REVERSE_INDEX = FLOOR_INDEX_MAX - MapDisp_GetBossRoomStorey();
        }
    }
}

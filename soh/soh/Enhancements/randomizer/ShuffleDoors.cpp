#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/randomizer/dungeon.h"

extern "C" {
#include <z64.h>
#include "functions.h"
#include "variables.h"
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
extern PlayState* gPlayState;
void func_8083C0E8(Player* player, PlayState* play); // prevents cutscene softlock
}

// Carrying Ruto from room to room stops working once Jabu's doors lead somewhere else, so put a copy
// of her in every vanilla Jabu room the logic expects to use her in.
static const struct JabuRuto {
    s8 room;
    s16 x, y, z, rotY;
} sJabuRutoRooms[] = {
    // big octo room
    { 6, -1160, -1015, -3343, 0x4000 },
    // forked corridor
    { 7, 0, -340, -4686, 0 },
    // stinger room
    { 9, 1250, -340, -4683, -0x4000 },
    // water switch room
    { 14, 620, -1113, -3223, -0x4000 },
};

static void SpawnJabuRuto() {
    if (gPlayState->sceneNum != SCENE_JABU_JABU ||
        Rando::Context::GetInstance()->GetDungeonFromScene(SCENE_JABU_JABU)->IsMQ()) {
        return;
    }

    const JabuRuto* spawn = NULL;
    for (const JabuRuto& room : sJabuRutoRooms) {
        if (room.room == gPlayState->roomCtx.curRoom.num) {
            spawn = &room;
            break;
        }
    }
    if (spawn == NULL) {
        return;
    }

    // Once the platform has carried her off the big octo room has to stay Ruto-less
    if (spawn->room == 6 && Flags_GetInfTable(INFTABLE_146)) {
        return;
    }

    for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].head; actor != NULL; actor = actor->next) {
        if (actor->id == ACTOR_EN_RU1 && actor->update != NULL) {
            return;
        }
    }

    // Her init only leaves her waiting to be picked up for one combination of flags
    bool met = Flags_GetInfTable(INFTABLE_RUTO_IN_JJ_MEET_RUTO);
    bool onSwitch = Flags_GetInfTable(INFTABLE_140);
    bool broughtUp = Flags_GetInfTable(INFTABLE_147);
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_MEET_RUTO);
    Flags_UnsetInfTable(INFTABLE_140);
    Flags_UnsetInfTable(INFTABLE_147);
    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_RU1, spawn->x, spawn->y, spawn->z, 0, spawn->rotY, 0,
                0x0203);
    if (!met) {
        Flags_UnsetInfTable(INFTABLE_RUTO_IN_JJ_MEET_RUTO);
    }
    if (onSwitch) {
        Flags_SetInfTable(INFTABLE_140);
    }
    if (broughtUp) {
        Flags_SetInfTable(INFTABLE_147);
    }
}

// Drop the player at the shuffled door's other side. Returns true when that side is in another
// scene, in which case a scene transition is started instead of a room load.
static bool WarpThroughDoor(Player* player, const Door* door) {
    player->actor.world.pos.x = door->linkX;
    player->actor.world.pos.y = door->linkY;
    player->actor.world.pos.z = door->linkZ;
    player->actor.world.rot.y = door->rotY;
    player->actor.shape.rot.y = door->rotY;
    player->skelAnime.movementFlags = 0;
    player->skelAnime.endFrame = 0;
    player->doorTimer = 0;
    player->linearVelocity = 0;
    player->unk_450 = player->actor.world.pos;
    player->unk_45C = player->actor.world.pos;
    Camera_InitPlayerSettings(&gPlayState->mainCamera, player);
    func_8083C0E8(player, gPlayState);

    if (gPlayState->sceneNum == door->scene) {
        if (gPlayState->interfaceCtx.mapRoomNum != door->srcRoom) {
            Map_InitRoomData(gPlayState, door->srcRoom);
        }
        return false;
    }

    gSaveContext.respawnFlag = 1;
    gSaveContext.subTimerSeconds = 1;
    switch (door->scene) {
        case SCENE_DEKU_TREE:
            gPlayState->nextEntranceIndex = ENTR_DEKU_TREE_ENTRANCE;
            break;
        case SCENE_DODONGOS_CAVERN:
            gPlayState->nextEntranceIndex = ENTR_DODONGOS_CAVERN_ENTRANCE;
            break;
        case SCENE_JABU_JABU:
            gPlayState->nextEntranceIndex = ENTR_JABU_JABU_ENTRANCE;
            break;
        case SCENE_FOREST_TEMPLE:
            gPlayState->nextEntranceIndex = ENTR_FOREST_TEMPLE_ENTRANCE;
            break;
        case SCENE_FIRE_TEMPLE:
            gPlayState->nextEntranceIndex = ENTR_FIRE_TEMPLE_ENTRANCE;
            break;
        case SCENE_WATER_TEMPLE:
            gPlayState->nextEntranceIndex = ENTR_WATER_TEMPLE_ENTRANCE;
            break;
        case SCENE_SHADOW_TEMPLE:
            gPlayState->nextEntranceIndex = ENTR_SHADOW_TEMPLE_ENTRANCE;
            break;
        case SCENE_SPIRIT_TEMPLE:
            gPlayState->nextEntranceIndex = ENTR_SPIRIT_TEMPLE_ENTRANCE;
            break;
        case SCENE_BOTTOM_OF_THE_WELL:
            gPlayState->nextEntranceIndex = ENTR_BOTTOM_OF_THE_WELL_ENTRANCE;
            break;
        case SCENE_ICE_CAVERN:
            gPlayState->nextEntranceIndex = ENTR_ICE_CAVERN_ENTRANCE;
            break;
        case SCENE_GERUDO_TRAINING_GROUND:
            gPlayState->nextEntranceIndex = ENTR_GERUDO_TRAINING_GROUND_ENTRANCE;
            break;
        case SCENE_INSIDE_GANONS_CASTLE:
            gPlayState->nextEntranceIndex = ENTR_INSIDE_GANONS_CASTLE_ENTRANCE;
            break;
        case SCENE_GANONS_TOWER:
            gPlayState->nextEntranceIndex = ENTR_GANONS_TOWER_0;
            break;
    }
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = gPlayState->nextEntranceIndex;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = door->srcRoom;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = player->actor.world.pos;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = player->actor.world.rot.y;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0xDFF;
    gSaveContext.nextTransitionType = TRANS_TYPE_INSTANT;
    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_INSTANT;
    static HOOK_ID hookId;
    hookId = REGISTER_VB_SHOULD(VB_INFLICT_VOID_DAMAGE, {
        *should = false;
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnVanillaBehavior>(hookId);
    });
    return true;
}

void RegisterShuffleDoors() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_DUNGEON_DOORS);

    // Teleport through doors instead of playing animation, going to wherever the door was shuffled to.
    COND_VB_SHOULD(VB_PLAYER_DOOR_SETUP, shouldRegister, {
        Player* player = va_arg(args, Player*);
        Actor* doorActor = va_arg(args, Actor*);
        s32 doorDirection = va_arg(args, s32);

        uint16_t mapIndex = gSaveContext.mapIndex;
        if (doorActor->id == ACTOR_EN_DOOR) {
            EnDoor* enDoor = reinterpret_cast<EnDoor*>(doorActor);
            if (enDoor->lockTimer != 0) {
                gSaveContext.inventory.dungeonKeys[mapIndex]--;
                Flags_SetSwitch(gPlayState, doorActor->params & 0x3F);
                Audio_PlayActorSound2(doorActor, NA_SE_EV_CHAIN_KEY_UNLOCK);
                GameInteractor_ExecuteOnDungeonKeyUsedHooks(mapIndex);
                enDoor->lockTimer = 0;
                doorActor->params = (doorActor->params & ~0x380) | (DOOR_SCENEEXIT << 7);
            }
        } else if (doorActor->id == ACTOR_DOOR_SHUTTER) {
            DoorShutter* shutter = reinterpret_cast<DoorShutter*>(doorActor);
            if (shutter->doorType == SHUTTER_KEY_LOCKED && shutter->unlockTimer != 0) {
                if (shutter->doorType != SHUTTER_BOSS) {
                    gSaveContext.inventory.dungeonKeys[mapIndex]--;
                }
                Flags_SetSwitch(gPlayState, shutter->dyna.actor.params & 0x3F);
                Audio_PlayActorSound2(doorActor, NA_SE_EV_CHAIN_KEY_UNLOCK);
                GameInteractor_ExecuteOnDungeonKeyUsedHooks(mapIndex);
                shutter->unlockTimer = 0;
                shutter->unk_164 = 1;
            }
        }

        s32 transitionIdx = GET_TRANSITION_ACTOR_INDEX(doorActor);
        s32 frontRoom = gPlayState->transiActorCtx.list[transitionIdx].sides[(doorDirection > 0) ? 0 : 1].room;
        bool sceneChange = false;

        const Door* newDoor = Rando::Context::GetInstance()->MapDoor(gPlayState->sceneNum, transitionIdx, frontRoom);
        if (newDoor != NULL) {
            frontRoom = newDoor->srcRoom;
            sceneChange = WarpThroughDoor(player, newDoor);
        }

        if (!sceneChange && frontRoom != gPlayState->roomCtx.curRoom.num) {
            Room_RequestNewRoom(gPlayState, &gPlayState->roomCtx, frontRoom);

            // Clean up previous room's actors. Vanilla calls Room_FinishRoomChange after the door animation/cutscene
            // completes, which kills actors not in the current or previous room (func_80031B14)
            Room_FinishRoomChange(gPlayState, &gPlayState->roomCtx);
        }

        player->doorType = PLAYER_DOORTYPE_NONE;
        player->doorActor = NULL;
        *should = false;
    });

    COND_HOOK(OnSceneSpawnActors, shouldRegister, SpawnJabuRuto);
}

static RegisterShipInitFunc initFunc(RegisterShuffleDoors, { "IS_RANDO" });

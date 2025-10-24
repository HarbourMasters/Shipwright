#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/mods.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

static constexpr TimeTravelType CVAR_TIME_TRAVEL_DEFAULT = TIME_TRAVEL_DISABLED;
#define CVAR_TIME_TRAVEL_NAME CVAR_ENHANCEMENT("TimeTravel")
#define CVAR_TIME_TRAVEL_VALUE CVarGetInteger(CVAR_TIME_TRAVEL_NAME, CVAR_TIME_TRAVEL_DEFAULT)

/// Switches Link's age and respawns him at the last entrance he entered.
void SwitchAge() {
    if (gPlayState == NULL)
        return;

    Player* player = GET_PLAYER(gPlayState);

    // Hyrule Castle: Very likely to fall through floor, so we force a specific entrance
    if (gPlayState->sceneNum == SCENE_HYRULE_CASTLE || gPlayState->sceneNum == SCENE_OUTSIDE_GANONS_CASTLE) {
        gPlayState->nextEntranceIndex = ENTR_CASTLE_GROUNDS_SOUTH_EXIT;
    } else {
        gSaveContext.respawnFlag = 1;
        gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;

        // Preserve the player's position and orientation
        gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = gPlayState->nextEntranceIndex;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = gPlayState->roomCtx.curRoom.num;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = player->actor.world.pos;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = player->actor.shape.rot.y;

        if (gPlayState->roomCtx.curRoom.behaviorType2 < 4) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0x0DFF;
        } else {
            // Scenes with static backgrounds use a special camera we need to preserve
            Camera* camera = GET_ACTIVE_CAM(gPlayState);
            s16 camId = camera->camDataIdx;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0x0D00 | camId;
        }
    }

    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_INSTANT;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
    gPlayState->linkAgeOnLoad ^= 1;

    // Discover adult/child spawns
    if (gPlayState->linkAgeOnLoad == LINK_AGE_ADULT) {
        Entrance_SetEntranceDiscovered(ENTR_HYRULE_FIELD_10, false);
    } else {
        Entrance_SetEntranceDiscovered(ENTR_LINKS_HOUSE_CHILD_SPAWN, false);
    }

    static HOOK_ID hookId = 0;
    hookId = REGISTER_VB_SHOULD(VB_INFLICT_VOID_DAMAGE, {
        *should = false;
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnVanillaBehavior>(hookId);
    });
}

static bool MeetsTimeTravelRequirements() {
    bool hasOcarinaOfTime = (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME);
    bool hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
    switch (CVAR_TIME_TRAVEL_VALUE) {
        case TIME_TRAVEL_ANY:
            return true;
        case TIME_TRAVEL_ANY_MS:
            return hasMasterSword;
        case TIME_TRAVEL_OOT_MS:
            return hasMasterSword && hasOcarinaOfTime;
        case TIME_TRAVEL_OOT:
            return hasOcarinaOfTime;
        default:
            return false;
    }
}

static void OnSongOfTime() {
    if (!GameInteractor::IsSaveLoaded()) {
        return;
    }

    Actor* player = &GET_PLAYER(gPlayState)->actor;
    Actor* nearbyTimeBlockEmpty =
        Actor_FindNearby(gPlayState, player, ACTOR_OBJ_WARP2BLOCK, ACTORCAT_ITEMACTION, 300.0f);
    Actor* nearbyTimeBlock = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_TIMEBLOCK, ACTORCAT_ITEMACTION, 300.0f);
    Actor* nearbyOcarinaSpot = Actor_FindNearby(gPlayState, player, ACTOR_EN_OKARINA_TAG, ACTORCAT_PROP, 120.0f);
    Actor* nearbyDoorOfTime = Actor_FindNearby(gPlayState, player, ACTOR_DOOR_TOKI, ACTORCAT_BG, 500.0f);
    Actor* nearbyFrogs = Actor_FindNearby(gPlayState, player, ACTOR_EN_FR, ACTORCAT_NPC, 300.0f);
    Actor* nearbyGossipStone = Actor_FindNearby(gPlayState, player, ACTOR_EN_GS, ACTORCAT_NPC, 300.0f);
    bool justPlayedSoT = gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_TIME;
    bool notNearAnySource = !nearbyTimeBlockEmpty && !nearbyTimeBlock && !nearbyOcarinaSpot && !nearbyDoorOfTime &&
                            !nearbyFrogs && !nearbyGossipStone;
    
    if (justPlayedSoT && notNearAnySource) {
        SwitchAge();
    }
}

static void RegisterOcarinaTimeTravel() {
    bool meetsRequirements = MeetsTimeTravelRequirements();

    // If requirements are met at the time of registration, we hook directly into the song action
    COND_HOOK(OnOcarinaSongAction, meetsRequirements, OnSongOfTime);

    // Otherwise, if not disabled, check requirements again when one of the relevant items is received
    COND_HOOK(OnItemReceive, !meetsRequirements && CVAR_TIME_TRAVEL_VALUE, [](GetItemEntry itemEntry) {
        if (itemEntry.itemId == ITEM_OCARINA_TIME || itemEntry.itemId == ITEM_SWORD_MASTER) {
            RegisterOcarinaTimeTravel();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterOcarinaTimeTravel, { CVAR_TIME_TRAVEL_NAME });

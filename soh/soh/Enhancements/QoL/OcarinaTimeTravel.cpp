#include "soh/ShipInit.hpp"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/SwitchAge.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" PlayState* gPlayState;

#define CVAR_OCARINA_TIME_TRAVEL_DEFAULT 0
#define CVAR_OCARINA_TIME_TRAVEL_NAME CVAR_ENHANCEMENT("TimeTravel")
#define CVAR_OCARINA_TIME_TRAVEL_VALUE CVarGetInteger(CVAR_OCARINA_TIME_TRAVEL_NAME, CVAR_OCARINA_TIME_TRAVEL_DEFAULT)

/// Switches Link's age and respawns him at the last entrance he entered.
void OcarinaTimeTravel() {
    if (!GameInteractor::IsSaveLoaded(true)) {
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
    bool hasOcarinaOfTime = (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME);
    bool hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
    int timeTravelSetting = CVarGetInteger(CVAR_ENHANCEMENT("TimeTravel"), 0);
    bool meetsTimeTravelRequirements = false;

    switch (timeTravelSetting) {
        case TIME_TRAVEL_ANY:
            meetsTimeTravelRequirements = true;
            break;
        case TIME_TRAVEL_ANY_MS:
            meetsTimeTravelRequirements = hasMasterSword;
            break;
        case TIME_TRAVEL_OOT_MS:
            meetsTimeTravelRequirements = hasMasterSword && hasOcarinaOfTime;
            break;
        case TIME_TRAVEL_OOT:
        default:
            meetsTimeTravelRequirements = hasOcarinaOfTime;
            break;
    }

    if (justPlayedSoT && notNearAnySource && meetsTimeTravelRequirements) {
        SwitchAge();
    }
}

static void RegisterOcarinaTimeTravel() {
    COND_HOOK(OnOcarinaSongAction, CVAR_OCARINA_TIME_TRAVEL_VALUE, OcarinaTimeTravel);
}

static RegisterShipInitFunc initFunc(RegisterOcarinaTimeTravel, { CVAR_OCARINA_TIME_TRAVEL_NAME });
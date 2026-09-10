#include "soh/ShipInit.hpp"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/SwitchAge.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

extern "C" PlayState* gPlayState;

#define CVAR_OCARINA_TIME_TRAVEL_DEFAULT 0
#define CVAR_OCARINA_TIME_TRAVEL_NAME CVAR_ENHANCEMENT("TimeTravel")
#define CVAR_OCARINA_TIME_TRAVEL_VALUE CVarGetInteger(CVAR_OCARINA_TIME_TRAVEL_NAME, CVAR_OCARINA_TIME_TRAVEL_DEFAULT)

// Set when the Song of Time is recognised, cleared once the era has changed.
static bool timeTravelPending = false;

/// True when the Song of Time just played moves Link between eras, rather than driving a Time Block, a
/// staff spot, the frogs or a Gossip Stone.
static bool TimeTravelApplies() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return false;
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
    bool meetsTimeTravelRequirements = false;

    switch (CVAR_OCARINA_TIME_TRAVEL_VALUE) {
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

    return justPlayedSoT && notNearAnySource && meetsTimeTravelRequirements;
}

/// Arms the era change; the shift itself waits for the song (see OcarinaTimeTravelUpdate).
static void OcarinaTimeTravel() {
    if (TimeTravelApplies()) {
        timeTravelPending = true;
    }
}

/// Switches Link's age once the Song of Time has played itself out, respawning him where he stood.
static void OcarinaTimeTravelUpdate() {
    // This hook also runs on the title screen and the file select, where there is no play state to read.
    if (!timeTravelPending || !GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    // Let the song finish, since the scene load fades out whatever is still playing, and leave anything
    // already warping Link alone.
    if ((func_800FA0B4(SEQ_PLAYER_FANFARE) == NA_BGM_OCA_TIME) ||
        (gPlayState->transitionTrigger != TRANS_TRIGGER_OFF)) {
        return;
    }

    timeTravelPending = false;
    SwitchAge();

    // The fade the Sun's Song changes the time of day behind (z_parameter.c, sunsSongState).
    gPlayState->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
    gPlayState->haltAllActors = 1;
}

static void RegisterOcarinaTimeTravel() {
    timeTravelPending = false;
    COND_HOOK(OnOcarinaSongAction, CVAR_OCARINA_TIME_TRAVEL_VALUE, OcarinaTimeTravel);
    COND_HOOK(OnGameFrameUpdate, CVAR_OCARINA_TIME_TRAVEL_VALUE, OcarinaTimeTravelUpdate);
}

static RegisterShipInitFunc initFunc(RegisterOcarinaTimeTravel, { CVAR_OCARINA_TIME_TRAVEL_NAME });

#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
void DoorShutter_SetupAction(DoorShutter*, DoorShutterActionFunc);
void DoorShutter_SetupType(DoorShutter*, PlayState*);
}

static void OnDoorInit(void* actorRef) {
    if (gPlayState->sceneNum == SCENE_INSIDE_GANONS_CASTLE) {
        DoorShutter* door = static_cast<DoorShutter*>(actorRef);
        bool barred = false;
        switch (door->dyna.actor.params) {
            case 8255:
                barred = !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST);
                break;
            case 9279:
                barred = !CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER);
                break;
            case 10303:
                barred = !CHECK_QUEST_ITEM(QUEST_MEDALLION_LIGHT);
                break;
            case 11327:
                barred = !CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE);
                break;
            case 12351:
                barred = !CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW);
                break;
            case 16447:
                barred = !CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT);
                break;
        }
        if (barred) {
            door->doorType = SHUTTER_FRONT_SWITCH_BACK_CLEAR;
            DoorShutter_SetupAction(door, DoorShutter_SetupType);
        }
    }
}

void RegisterMedallionLockedTrials() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_MEDALLION_LOCKED_TRIALS);

    COND_ID_HOOK(OnActorInit, ACTOR_DOOR_SHUTTER, shouldRegister, OnDoorInit);
}

static RegisterShipInitFunc initFunc(RegisterMedallionLockedTrials, { "IS_RANDO" });
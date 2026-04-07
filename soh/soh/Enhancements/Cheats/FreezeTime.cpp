#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "z64save.h"

extern "C" SaveContext gSaveContext;

#define CVAR_FREEZE_TIME_NAME CVAR_CHEAT("FreezeTime")
#define CVAR_FREEZE_TIME_DEFAULT 0
#define CVAR_FREEZE_TIME_VALUE CVarGetInteger(CVAR_FREEZE_TIME_NAME, CVAR_FREEZE_TIME_DEFAULT)

#define CVAR_PREV_TIME_NAME CVAR_GENERAL("PrevTime")
#define CVAR_PREV_TIME_DEFAULT -1
#define CVAR_PREV_TIME_VALUE CVarGetInteger(CVAR_PREV_TIME_NAME, CVAR_PREV_TIME_DEFAULT)

void OnGameFrameUpdateFreezeTime(IEvent* event) {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    if (CVAR_PREV_TIME_VALUE == CVAR_PREV_TIME_DEFAULT) {
        CVarSetInteger(CVAR_PREV_TIME_NAME, gSaveContext.dayTime);
    }

    gSaveContext.dayTime = CVAR_PREV_TIME_VALUE;
}

void RegisterFreezeTime() {
    static ListenerID listenerId = -1;
    if (listenerId == -1) {
        UNREGISTER_LISTENER(OnGameFrameUpdate, listenerId);
    }
    if (CVAR_FREEZE_TIME_VALUE) {
        listenerId = REGISTER_LISTENER(OnGameFrameUpdate, EVENT_PRIORITY_LOW, OnGameFrameUpdateFreezeTime);
    } else {
        CVarClear(CVAR_PREV_TIME_NAME);
    }
}

static RegisterShipInitFunc initFunc(RegisterFreezeTime, { CVAR_FREEZE_TIME_NAME });

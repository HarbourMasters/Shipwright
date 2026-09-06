#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64save.h"
}

extern "C" SaveContext gSaveContext;

static constexpr int32_t CVAR_TIMEFLOWFILESELECT_DEFAULT = 0;
#define CVAR_TIMEFLOWFILESELECT_NAME CVAR_ENHANCEMENT("FileSelect.TimeFlow")
#define CVAR_TIMEFLOWFILESELECT_VALUE CVarGetInteger(CVAR_TIMEFLOWFILESELECT_NAME, CVAR_TIMEFLOWFILESELECT_DEFAULT)

void OnFileChooseMainTimeFlowFileSelect(void* gameState) {
    gSaveContext.skyboxTime += 0x10;
}

void RegisterTimeFlowFileSelect() {
    COND_HOOK(OnFileChooseMain, CVAR_TIMEFLOWFILESELECT_VALUE, OnFileChooseMainTimeFlowFileSelect);
}

static RegisterShipInitFunc initFunc(RegisterTimeFlowFileSelect, { CVAR_TIMEFLOWFILESELECT_NAME });

#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "N64MemoryModel.hpp"

#include "global.h"
}

#define CVAR_NAME CVAR_ENHANCEMENT("N64MemoryModel")
#define CVAR_DEFAULT 0
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, CVAR_DEFAULT)

static s32 sIsActive = 0;

void N64Mem_Reset()
{
    sIsActive = CVAR_VALUE;
}

s32 N64Mem_IsActive()
{
    return sIsActive;
}

void RegisterN64MemoryModel()
{
    // #TODO: Shadow arena initialization
}

static RegisterShipInitFunc initFunc(RegisterN64MemoryModel, {CVAR_NAME});

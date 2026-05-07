#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "N64HeapFragmentation.hpp"

#include "global.h"
}

#define CVAR_NAME CVAR_ENHANCEMENT("N64HeapFragmentation")
#define CVAR_DEFAULT 0
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, CVAR_DEFAULT)

static s32 sIsActive = 0;

void N64HeapFrag_Reset()
{
    sIsActive = CVAR_VALUE;
}

s32 N64HeapFrag_IsActive()
{
    return sIsActive;
}

void RegisterN64HeapFragmentation()
{
    // #TODO: Shadow arena initialization
}

static RegisterShipInitFunc initFunc(RegisterN64HeapFragmentation, {CVAR_NAME});

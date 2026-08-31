#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64save.h"
}

extern "C" SaveContext gSaveContext;

#define CVAR_INFINITE_NAYRUS_LOVE_NAME CVAR_CHEAT("InfiniteNayru")
#define CVAR_INFINITE_NAYRUS_LOVE_DEFAULT 0
#define CVAR_INFINITE_NAYRUS_LOVE_VALUE \
    CVarGetInteger(CVAR_INFINITE_NAYRUS_LOVE_NAME, CVAR_INFINITE_NAYRUS_LOVE_DEFAULT)

void OnGameFrameUpdateInfiniteNayrusLove() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    gSaveContext.nayrusLoveTimer = 0x44B;
}

void RegisterInfiniteNayrusLove() {
    COND_HOOK(OnGameFrameUpdate, CVAR_INFINITE_NAYRUS_LOVE_VALUE, OnGameFrameUpdateInfiniteNayrusLove);
}

static RegisterShipInitFunc initFunc(RegisterInfiniteNayrusLove, { CVAR_INFINITE_NAYRUS_LOVE_NAME });

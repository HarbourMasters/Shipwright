#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Sw/z_en_sw.h"
}

static constexpr int32_t CVAR_ONE_HIT_GS_DEFAULT = 0;
#define CVAR_ONE_HIT_GS_NAME CVAR_ENHANCEMENT("OneHitGoldSkulltulas")
#define CVAR_ONE_HIT_GS_VALUE CVarGetInteger(CVAR_ONE_HIT_GS_NAME, CVAR_ONE_HIT_GS_DEFAULT)

static void OnActorInitOneHitGoldSkulltulas(void* refActor) {
    EnSw* enSw = static_cast<EnSw*>(refActor);

    if (((enSw->actor.params & 0xE000) >> 0xD) != 0) {
        enSw->actor.colChkInfo.health = 1;
    }
}

static void RegisterOneHitGoldSkulltulas() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_SW, CVAR_ONE_HIT_GS_VALUE, OnActorInitOneHitGoldSkulltulas);
}

static RegisterShipInitFunc initFunc(RegisterOneHitGoldSkulltulas, { CVAR_ONE_HIT_GS_NAME });

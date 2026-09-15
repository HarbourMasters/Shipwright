#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "src/overlays/actors/ovl_En_Kz/z_en_kz.h"

void EnKz_Mweep(EnKz* enKz, PlayState* play);
}

#define CVAR_SPEED_NAME CVAR_ENHANCEMENT("MweepSpeed")
#define CVAR_SPEED CVarGetFloat(CVAR_SPEED_NAME, 1.0f)

static void EnKz_SetSpeed(void* actorPtr) {
    EnKz* enKz = (EnKz*)actorPtr;
    if (enKz->actionFunc != EnKz_Mweep) {
        return;
    }

    enKz->actor.speedXZ = 0.1f * CVAR_SPEED;
}

static void RegisterMweepSpeed() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KZ, CVAR_SPEED != 1.0f, EnKz_SetSpeed);
}

static RegisterShipInitFunc initFunc(RegisterMweepSpeed, { CVAR_SPEED_NAME });

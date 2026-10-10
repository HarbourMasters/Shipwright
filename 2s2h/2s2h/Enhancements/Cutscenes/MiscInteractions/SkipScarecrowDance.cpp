#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Kakasi/z_en_kakasi.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipScarecrowDance() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KAKASI, CVAR, [](Actor* actor) {
        EnKakasi* enKakasi = (EnKakasi*)actor;
        if (enKakasi->actionFunc == EnKakasi_DancingNightAway && enKakasi->unk190 == 0) {
            enKakasi->unk190 = 13;
            enKakasi->unk204 = 0;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipScarecrowDance, { CVAR_NAME });

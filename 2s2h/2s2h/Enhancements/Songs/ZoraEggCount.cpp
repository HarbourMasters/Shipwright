#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Mk/z_en_mk.h"
}

const uint32_t MAX_EGGS = 7;

#define CVAR_NAME "gEnhancements.Songs.ZoraEggCount"
#define CVAR CVarGetInteger(CVAR_NAME, MAX_EGGS)

void RegisterZoraEggCount() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_MK, CVAR != MAX_EGGS, [](Actor* actor) {
        // complete quest if you have enough eggs
        if (gSaveContext.save.saveInfo.permanentSceneFlags[SCENE_LABO].unk_14 != MAX_EGGS &&
            CVarGetInteger("gEnhancements.Songs.ZoraEggCount", MAX_EGGS) <=
                gSaveContext.save.saveInfo.permanentSceneFlags[SCENE_LABO].unk_14) {
            gSaveContext.save.saveInfo.permanentSceneFlags[SCENE_LABO].unk_14 = MAX_EGGS;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterZoraEggCount, { CVAR_NAME });

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_Obj_Lightswitch/z_obj_lightswitch.h"
#include "src/overlays/actors/ovl_Bg_Spot06_Objects/z_bg_spot06_objects.h"
#include "src/overlays/actors/ovl_Bg_Jya_Bombchuiwa/z_bg_jya_bombchuiwa.h"
extern PlayState* gPlayState;
}

#define SKIP_MISC_INTERACTIONS_NAME CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions")
#define SKIP_MISC_INTERACTIONS_VALUE CVarGetInteger(SKIP_MISC_INTERACTIONS_NAME, IS_RANDO)

static void RegisterSkipTimerDelay() {
    // Skip Water Temple gate delay
    COND_ID_HOOK(OnActorUpdate, ACTOR_BG_SPOT06_OBJECTS, SKIP_MISC_INTERACTIONS_VALUE, [](void* actor) {
        auto spot06 = static_cast<BgSpot06Objects*>(actor);
        if (spot06->dyna.actor.params == 0) {
            spot06->timer = 0;
        }
    });

    // Skip Spirit Sun on Floor activation delay
    COND_ID_HOOK(OnActorUpdate, ACTOR_BG_JYA_BOMBCHUIWA, SKIP_MISC_INTERACTIONS_VALUE, [](void* actor) {
        auto jya = static_cast<BgJyaBombchuiwa*>(actor);
        if (!(jya->drawFlags & 4) && jya->timer > 0 && jya->timer < 9) {
            jya->timer = 9;
        }
    });

    // Skip Spirit Sun on Floor & Sun on Block activation delay
    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_LIGHTSWITCH, SKIP_MISC_INTERACTIONS_VALUE, [](void* actor) {
        if (gPlayState->sceneNum == SCENE_SPIRIT_TEMPLE &&
            (gPlayState->roomCtx.curRoom.num == 4 || gPlayState->roomCtx.curRoom.num == 8)) {
            auto sun = static_cast<ObjLightswitch*>(actor);
            sun->toggleDelay = 0;
        }
    });
}

static RegisterShipInitFunc initFunc_SkipTimerDelay(RegisterSkipTimerDelay,
                                                    { SKIP_MISC_INTERACTIONS_NAME, "IS_RANDO" });

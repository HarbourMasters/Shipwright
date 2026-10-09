#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64save.h"
#include "z64scene.h"
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cycle.KeepExpressMail"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterKeepExpressMail() {

    COND_VB_SHOULD(VB_MSG_SCRIPT_DEL_ITEM, CVAR, {
        Actor* actor = va_arg(args, Actor*);
        ItemId itemId = (ItemId)va_arg(args, int);

        if (gPlayState->sceneId != SCENE_MILK_BAR && gPlayState->sceneId != SCENE_POSTHOUSE) {
            return;
        }

        // Keep the express mail only on the first trade for the cycle
        // Postman checking for Madame Aroma trade cycle flag
        // Madame Aroma checking for Postman trade cycle flag
        if (itemId == ITEM_LETTER_MAMA && (actor->id == ACTOR_EN_PM && !CHECK_WEEKEVENTREG(WEEKEVENTREG_57_04)) ||
            (actor->id == ACTOR_EN_AL && !CHECK_WEEKEVENTREG(WEEKEVENTREG_86_01))) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterKeepExpressMail, { CVAR_NAME });

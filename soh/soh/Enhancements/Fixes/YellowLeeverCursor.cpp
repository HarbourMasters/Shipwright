#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

static constexpr int32_t CVAR_YELLOWLEEVERCURSOR_DEFAULT = 0;
#define CVAR_YELLOWLEEVERCURSOR_NAME CVAR_ENHANCEMENT("YellowLeeverCursor")
#define CVAR_YELLOWLEEVERCURSOR_VALUE CVarGetInteger(CVAR_YELLOWLEEVERCURSOR_NAME, CVAR_YELLOWLEEVERCURSOR_DEFAULT)

static void RegisterYellowLeeverCursor() {
    // Override Targeting Arrow Color
    COND_VB_SHOULD(VB_OVERRIDE_TARGET_ARROW_CATEGORY, CVAR_YELLOWLEEVERCURSOR_VALUE, {
        Actor* actor = va_arg(args, Actor*);
        s32* overrideActorCategory = va_arg(args, s32*);

        if (actor->id == ACTOR_EN_REEBA) {
            *overrideActorCategory = ACTORCAT_ENEMY;
            *should = true;
        }
    });

    // Override Navi Color
    COND_VB_SHOULD(VB_OVERRIDE_TARGET_NAVI_CATEGORY, CVAR_YELLOWLEEVERCURSOR_VALUE, {
        Actor* actor = va_arg(args, Actor*);
        s32* overrideActorCategory = va_arg(args, s32*);

        if (actor->id == ACTOR_EN_REEBA) {
            *overrideActorCategory = ACTORCAT_ENEMY;
            *should = true;
        }
    });

    // Override Targeting Cursor Color
    COND_VB_SHOULD(VB_OVERRIDE_TARGET_CURSOR_CATEGORY, CVAR_YELLOWLEEVERCURSOR_VALUE, {
        Actor* actor = va_arg(args, Actor*);
        s32* overrideActorCategory = va_arg(args, s32*);

        if (actor->id == ACTOR_EN_REEBA) {
            *overrideActorCategory = ACTORCAT_ENEMY;
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterYellowLeeverCursor, { CVAR_YELLOWLEEVERCURSOR_NAME });

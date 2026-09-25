#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64actor_enum.h"
}

static constexpr const char* CVAR_CHILD_HOOKSHOT_SOFTLOCK = CVAR_ENHANCEMENT("ChildHookshotSoftlock");

// As child object_link_boy isn't loaded, so vanilla fails to spawn the hookshot and Link is stuck holding it
static void RegisterChildHookshotSoftlock() {
    COND_VB_SHOULD(VB_SPAWN_ACTOR_WITHOUT_OBJECT, CVarGetInteger(CVAR_CHILD_HOOKSHOT_SOFTLOCK, 0), {
        s16 actorId = va_arg(args, int);
        if (actorId == ACTOR_ARMS_HOOK) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterChildHookshotSoftlock, { CVAR_CHILD_HOOKSHOT_SOFTLOCK });

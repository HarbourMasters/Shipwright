#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include <libultraship/log/luslog.h>

#define CONSOLE_CRASH_CVAR_NAME "gEnhancements.Fixes.ConsoleCrashes"
#define CONSOLE_CRASH_CVAR CVarGetInteger(CONSOLE_CRASH_CVAR_NAME, true)
#define CVAR_NAME "gCheats.UnrestrictedItems"
#define CVAR CVarGetInteger(CVAR_NAME, false)

extern "C" bool Ship_HandleConsoleCrashAsReset();

void RegisterUnrestrictedItems() {
    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, CVAR, { *should = false; });

    // Prevent Deku Hookshot crash from float overflow/failed raycasts
    COND_VB_SHOULD(VB_DEKU_COMMON_HEAD_OVERRIDE_HELD_ACTOR, true, {
        Actor* heldActor = va_arg(args, Actor*);
        if (heldActor != NULL && heldActor->id == ACTOR_ARMS_HOOK) {
            *should = false;
            if (!CVAR && !CONSOLE_CRASH_CVAR) {
                LUSLOG_WARN("Using Hookshot as Deku crashes on console");
                Ship_HandleConsoleCrashAsReset();
            }
        }
    });

    COND_VB_SHOULD(VB_DEKU_COMMON_UPPER_LIMB_OVERRIDE_HELD_ACTOR, true, {
        Actor* heldActor = va_arg(args, Actor*);
        if (heldActor != NULL && heldActor->id == ACTOR_ARMS_HOOK) {
            *should = false;
            if (!CVAR && !CONSOLE_CRASH_CVAR) {
                LUSLOG_WARN("Using Hookshot as Deku crashes on console");
                Ship_HandleConsoleCrashAsReset();
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterUnrestrictedItems, { CVAR_NAME, CONSOLE_CRASH_CVAR_NAME });

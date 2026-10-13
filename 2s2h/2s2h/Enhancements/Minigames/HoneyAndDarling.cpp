#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Fu/z_en_fu.h"
}

#define DAY1_CVAR_NAME "gEnhancements.Minigames.HoneyAndDarlingDay1"
#define DAY1_CVAR CVarGetInteger(DAY1_CVAR_NAME, 8)
#define DAY2_CVAR_NAME "gEnhancements.Minigames.HoneyAndDarlingDay2"
#define DAY2_CVAR CVarGetInteger(DAY2_CVAR_NAME, 8)
#define DAY3_CVAR_NAME "gEnhancements.Minigames.HoneyAndDarlingDay3"
#define DAY3_CVAR CVarGetInteger(DAY3_CVAR_NAME, 16)

void RegisterHoneyAndDarling() {
    COND_VB_SHOULD(VB_HONEY_AND_DARLING_MINIGAME_FINISH, (DAY1_CVAR != 8 || DAY2_CVAR != 8 || DAY3_CVAR != 16), {
        EnFu* enFu = va_arg(args, EnFu*);
        if ((CURRENT_DAY == 1 && enFu->unk_548 >= DAY1_CVAR) || (CURRENT_DAY == 2 && enFu->unk_548 >= DAY2_CVAR) ||
            (CURRENT_DAY == 3 && enFu->unk_548 >= DAY3_CVAR)) {
            enFu->unk_548 = enFu->unk_54C;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterHoneyAndDarling, { DAY1_CVAR_NAME, DAY2_CVAR_NAME, DAY3_CVAR_NAME });

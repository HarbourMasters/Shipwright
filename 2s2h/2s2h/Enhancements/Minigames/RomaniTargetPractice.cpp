#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Ma4/z_en_ma4.h"
}

#define CVAR_NAME "gEnhancements.Minigames.RomaniTargetPractice"
#define CVAR CVarGetInteger(CVAR_NAME, 10)

void RegisterRomaniTargetPractice() {
    COND_VB_SHOULD(VB_WIN_ROMANI_PRACTICE, CVAR != 10, {
        EnMa4* enMa4 = va_arg(args, EnMa4*);

        if (enMa4->poppedBalloonCounter >= CVAR) {
            enMa4->poppedBalloonCounter = 10;
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterRomaniTargetPractice, { CVAR_NAME });

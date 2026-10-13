#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Dno/z_en_dno.h"
}

#define CVAR_NAME "gEnhancements.Fixes.DekuButlerFixShockLoopAnimation"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define EN_DNO_ANIM_SHOCK_LOOP 18

void RegisterDekuButlerFixShockLoopAnimation() {
    COND_VB_SHOULD(VB_DEKU_BUTLER_CHANGE_SHOCK_ANIMATION, CVAR, {
        EnDno* dno = va_arg(args, EnDno*);
        u8* changeAnim = va_arg(args, u8*);
        if (dno->animIndex == EN_DNO_ANIM_SHOCK_LOOP) {
            *changeAnim = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDekuButlerFixShockLoopAnimation, { CVAR_NAME });

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static void RegisterDropsDontDie() {
    COND_VB_SHOULD(VB_ITEM00_TIMER_TICK, CVarGetInteger(CVAR_CHEAT("DropsDontDie"), 0), {
        EnItem00* item00 = va_arg(args, EnItem00*);
        if (item00->unk_154 <= 0)
            *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterDropsDontDie, { CVAR_CHEAT("DropsDontDie") });

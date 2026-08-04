#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64save.h"
}

void RegisterFasterEmptyBottle() {
    COND_VB_SHOULD(VB_EMPTYING_BOTTLE, CVarGetInteger(CVAR_ENHANCEMENT("FasterBottleEmpty"), 0), {
        Player* player = va_arg(args, Player*);
        if (player->skelAnime.curFrame <= 60.0f) {
            player->skelAnime.playSpeed = 3.0f;
        } else {
            player->skelAnime.playSpeed = 1.0f;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterEmptyBottle, { CVAR_ENHANCEMENT("FasterBottleEmpty") });

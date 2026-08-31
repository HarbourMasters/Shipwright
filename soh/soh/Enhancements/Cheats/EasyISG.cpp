#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "z64.h"
#include "macros.h"
extern PlayState* gPlayState;
}

#define CVAR_EASY_ISG_NAME CVAR_CHEAT("EasyISG")
#define CVAR_EASY_ISG_DEFAULT 0
#define CVAR_EASY_ISG_VALUE CVarGetInteger(CVAR_EASY_ISG_NAME, CVAR_EASY_ISG_DEFAULT)

void OnGameFrameUpdateEasyISG() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    player->meleeWeaponState = 1;
}

void RegisterEasyISG() {
    COND_HOOK(OnGameFrameUpdate, CVAR_EASY_ISG_VALUE, OnGameFrameUpdateEasyISG);
}

static RegisterShipInitFunc initFunc(RegisterEasyISG, { CVAR_EASY_ISG_NAME });

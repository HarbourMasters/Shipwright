#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "macros.h"
}

#define CVAR_EASY_QPA_NAME CVAR_CHEAT("EasyQPA")
#define CVAR_EASY_QPA_DEFAULT 0
#define CVAR_EASY_QPA_VALUE CVarGetInteger(CVAR_EASY_QPA_NAME, CVAR_EASY_QPA_DEFAULT)

void OnGameFrameUpdateEasyQPA() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    player->meleeWeaponQuads[0].info.toucher.dmgFlags = 0x16171617;
    player->meleeWeaponQuads[1].info.toucher.dmgFlags = 0x16171617;
}

void RegisterEasyQPA() {
    COND_HOOK(OnGameFrameUpdate, CVAR_EASY_QPA_VALUE, OnGameFrameUpdateEasyQPA);
}

static RegisterShipInitFunc initFunc(RegisterEasyQPA, { CVAR_EASY_QPA_NAME });

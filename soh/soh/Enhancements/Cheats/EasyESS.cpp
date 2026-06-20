#include <libultraship/bridge.h>
#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <cmath>

extern "C" {
    extern PlayState* gPlayState;
    #include "macros.h"
}

#define CVAR_EASY_ESS_NAME CVAR_CHEAT("EasyESS")
#define CVAR_EASY_ESS_DEFAULT 0
#define CVAR_EASY_ESS_VALUE CVarGetInteger(CVAR_EASY_ESS_NAME, CVAR_EASY_ESS_DEFAULT)

void OnGameStateMainStartEasyESS() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    Input* input = &gPlayState->state.input[0];

    if (player != nullptr && CHECK_BTN_ANY(input->cur.button, BTN_CUSTOM_MODIFIER1)) {
        const int essValue = 18;
        int x = input->cur.stick_x;
        int y = input->cur.stick_y;
        int magSq = x * x + y * y;
        if (magSq > essValue * essValue) {
            float mag = sqrtf(static_cast<float>(magSq));
            input->cur.stick_x = static_cast<s8>((x / mag) * essValue);
            input->cur.stick_y = static_cast<s8>((y / mag) * essValue);
            input->rel.stick_x = input->cur.stick_x;
            input->rel.stick_y = input->cur.stick_y;
        }
    }
}

void RegisterEasyESS() {
    COND_HOOK(OnGameStateMainStart, CVAR_EASY_ESS_VALUE, OnGameStateMainStartEasyESS);
}

static RegisterShipInitFunc initFunc(RegisterEasyESS, { CVAR_EASY_ESS_NAME });
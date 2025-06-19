#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
}

void RegisterSwitchTimerMultiplier() {
    COND_VB_SHOULD(VB_SWITCH_TIMER_TICK, CVarGetInteger(CVAR_ENHANCEMENT("SwitchTimerMultiplier"), 0) != 0, {
        int multiplier = CVarGetInteger(CVAR_ENHANCEMENT("SwitchTimerMultiplier"), 0);
        if (multiplier != 0) {
            Actor* actor = va_arg(args, Actor*);
            if (multiplier < -3 && actor->id == ACTOR_OBJ_SYOKUDAI) {
                multiplier = -3;
            } else if (multiplier < -4 && actor->id == ACTOR_BG_GND_DARKMEIRO) {
                multiplier = -4;
            }

            if (multiplier > 0 && gPlayState->gameplayFrames % (multiplier + 1) != 0) {
                *should = false;
            } else if (gPlayState->gameplayFrames % (6 + multiplier) == 0) {
                s16* timer = va_arg(args, s16*);
                *timer -= *timer > 1;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSwitchTimerMultiplier, { CVAR_ENHANCEMENT("SwitchTimerMultiplier") });

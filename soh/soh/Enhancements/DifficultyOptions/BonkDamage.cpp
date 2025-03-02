#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
#include "functions.h"
#include "macros.h"
}

#define CVAR_BONK_DAMAGE_NAME CVAR_ENHANCEMENT("BonkDamageMult")
#define CVAR_BONK_DAMAGE_DEFAULT BONK_DAMAGE_NONE
#define CVAR_BONK_DAMAGE_VALUE CVarGetInteger(CVAR_BONK_DAMAGE_NAME, CVAR_BONK_DAMAGE_DEFAULT)

void OnPlayerBonkDamage() {
    uint8_t bonkOption = CVAR_BONK_DAMAGE_VALUE;
    if (bonkOption == BONK_DAMAGE_NONE) {
        return;
    }

    if (bonkOption == BONK_DAMAGE_OHKO) {
        gSaveContext.health = 0;
        return;
    }

    uint16_t bonkDamage = 0;
    switch (bonkOption) {
        case BONK_DAMAGE_QUARTER_HEART:
            bonkDamage = 4;
            break;
        case BONK_DAMAGE_HALF_HEART:
            bonkDamage = 8;
            break;
        case BONK_DAMAGE_1_HEART:
            bonkDamage = 16;
            break;
        case BONK_DAMAGE_2_HEARTS:
            bonkDamage = 32;
            break;
        case BONK_DAMAGE_4_HEARTS:
            bonkDamage = 64;
            break;
        case BONK_DAMAGE_8_HEARTS:
            bonkDamage = 128;
            break;
        default:
            break;
    }

    Health_ChangeBy(gPlayState, -bonkDamage);
    // Set invincibility to make Link flash red as a visual damage indicator.
    Player* player = GET_PLAYER(gPlayState);
    player->invincibilityTimer = 28;
}

void RegisterBonkDamage() {
    COND_HOOK(OnPlayerBonk, CVAR_BONK_DAMAGE_VALUE, OnPlayerBonkDamage);
}

static RegisterShipInitFunc initFunc(RegisterBonkDamage, { CVAR_BONK_DAMAGE_NAME });

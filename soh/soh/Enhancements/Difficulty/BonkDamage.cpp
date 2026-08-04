#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/enhancementTypes.h"

extern "C" {
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

static constexpr BonkDamage CVAR_BONK_DAMAGE_DEFAULT = BONK_DAMAGE_NONE;
#define CVAR_BONK_DAMAGE_NAME CVAR_ENHANCEMENT("BonkDamageMult")
#define CVAR_BONK_DAMAGE_VALUE CVarGetInteger(CVAR_BONK_DAMAGE_NAME, CVAR_BONK_DAMAGE_DEFAULT)
#define CVAR_BONK_DAMAGE_SET (CVAR_BONK_DAMAGE_VALUE != CVAR_BONK_DAMAGE_DEFAULT)

static void RegisterBonkDamage() {
    COND_HOOK(OnPlayerBonk, CVAR_BONK_DAMAGE_SET, [] {
        uint16_t bonkDamage = 0;
        switch (CVAR_BONK_DAMAGE_VALUE) {
            case BONK_DAMAGE_NONE:
                return;
            case BONK_DAMAGE_OHKO:
                gSaveContext.health = 0;
                return;
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
    });
}

static RegisterShipInitFunc initFunc(RegisterBonkDamage, { CVAR_BONK_DAMAGE_NAME });

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/BunnyHood.h"
#include "soh/Enhancements/gameplaystats.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

BunnyHoodMode Ship_GetBunnyHoodMode() {
    return (BunnyHoodMode)CVarGetInteger(CVAR_BUNNY_HOOD_NAME, BUNNY_HOOD_VANILLA);
}

float Ship_GetBunnyHoodRunFactor(Player* player) {
    if (Ship_GetBunnyHoodMode() != BUNNY_HOOD_VANILLA && player->currentMask == PLAYER_MASK_BUNNY) {
        return 1.5f;
    }
    return 1.0f;
}

float Ship_GetBunnyHoodJumpFactor(Player* player) {
    if (Ship_GetBunnyHoodMode() == BUNNY_HOOD_FAST_AND_JUMP && player->currentMask == PLAYER_MASK_BUNNY) {
        return 1.5f;
    }
    return 1.0f;
}

static void RegisterBunnyHood() {
    bool bunnyHoodActive = Ship_GetBunnyHoodMode() != BUNNY_HOOD_VANILLA;

    // Wearing the hood shouldn't lock you out of NPC interactions, so NPCs ignore it
    COND_VB_SHOULD(VB_NPC_REACT_TO_MASK, bunnyHoodActive, {
        u8 currentMask = va_arg(args, int);
        if (currentMask == PLAYER_MASK_BUNNY) {
            *should = false;
        }
    });

    // Masks stay usable where trade items are restricted, so the hood can be equipped there
    COND_VB_SHOULD(VB_DISABLE_TRADE_ITEM_BUTTON, bunnyHoodActive, {
        u8 item = va_arg(args, int);
        if (item >= ITEM_MASK_KEATON && item <= ITEM_MASK_TRUTH) {
            *should = false;
        }
    });

    // Same speed boost as running, for the "Move in first person" setting
    COND_VB_SHOULD(VB_PLAYER_MODIFY_FIRST_PERSON_SPEED, bunnyHoodActive, {
        Player* player = va_arg(args, Player*);
        f32* movementSpeed = va_arg(args, f32*);
        *movementSpeed *= Ship_GetBunnyHoodRunFactor(player);
    });

    // Gameplay stat: time spent wearing the hood
    COND_HOOK(OnPlayerUpdate, bunnyHoodActive, []() {
        if (gSaveContext.ship.stats.gameComplete || (IS_BOSS_RUSH && gSaveContext.ship.quest.data.bossRush.isPaused)) {
            return;
        }
        if (GET_PLAYER(gPlayState)->currentMask == PLAYER_MASK_BUNNY) {
            gSaveContext.ship.stats.count[COUNT_TIME_BUNNY_HOOD]++;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBunnyHood, { CVAR_BUNNY_HOOD_NAME });

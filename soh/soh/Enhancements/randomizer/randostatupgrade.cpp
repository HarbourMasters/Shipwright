#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <soh/OTRGlobals.h>
#include "soh/Enhancements/randomizer/randostatupgrade.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern SaveContext gSaveContext;
}

static void RegisterRandoStatUpgradeHooks() {
    COND_VB_SHOULD(VB_HEART_DISPLAY_TOTAL_COUNT, IS_RANDO && RAND_GET_OPTION(RSK_QUARTER_HEART), {
        u16 cap = gSaveContext.healthCapacity;
        if (cap % FULL_HEART_HEALTH == 0)
            return;
        s16* count = va_arg(args, s16*);
        *count = (cap + FULL_HEART_HEALTH - 1) / FULL_HEART_HEALTH;
    });

    COND_VB_SHOULD(VB_PLAYER_INCOMING_DAMAGE, IS_RANDO && RAND_GET_OPTION(RSK_DEFENSE_UPGRADE), {
        u8 level = gSaveContext.ship.quest.data.randomizer.defenseUpgrades;
        if (level == 0)
            return;
        uint8_t required = StatUpgradeRequired(5);
        va_arg(args, PlayState*);
        s16* healthChange = va_arg(args, s16*);
        if (*healthChange >= 0)
            return;
        float reduction = (float)std::min((int)level, (int)required) * (0.5f / required);
        *healthChange = (s16)(*healthChange * (1.0f - reduction));
    });

    COND_VB_SHOULD(VB_PLAYER_SPEED_MULTIPLIER, IS_RANDO && RAND_GET_OPTION(RSK_SPEED_UPGRADE), {
        u8 level = gSaveContext.ship.quest.data.randomizer.speedUpgrades;
        if (level == 0)
            return;
        uint8_t required = StatUpgradeRequired(5);
        Player* player = va_arg(args, Player*);
        if (!(player->actor.bgCheckFlags & 1) && !(player->stateFlags1 & PLAYER_STATE1_IN_WATER))
            return;
        f32* speedTarget = va_arg(args, f32*);
        float multiplier = 1.0f + (float)std::min((int)level, (int)required) * (0.4f / required);
        *speedTarget *= multiplier;
    });

    COND_VB_SHOULD(VB_PLAYER_ATTACK_DAMAGE_MULTIPLIER, IS_RANDO && RAND_GET_OPTION(RSK_POWER_UPGRADE), {
        uint8_t level = gSaveContext.ship.quest.data.randomizer.powerUpgrades;
        if (level == 0)
            return;
        uint8_t required = StatUpgradeRequired(5);
        float critChance = (float)std::min((int)level, (int)required) / (float)required;
        if (Rand_ZeroOne() > critChance)
            return;
        va_arg(args, PlayState*);
        u8* damage = va_arg(args, u8*);
        u32 doubled = (u32)*damage * 2;
        *damage = (u8)(doubled > 0xFF ? 0xFF : doubled);
    });

    COND_VB_SHOULD(VB_MAGIC_STEP_CAPACITY_TARGET, IS_RANDO && RAND_GET_OPTION(RSK_MAGIC_STAT_UPGRADE), {
        u8 level = gSaveContext.ship.quest.data.randomizer.magicStatUpgrades;
        if (level == 0)
            return;
        uint8_t required = StatUpgradeRequired(8);
        uint8_t unit = (uint8_t)std::max(1, 100 / (int)required);
        s16 cap = (s16)(std::min((int)level, (int)required) * unit);
        s16* target = va_arg(args, s16*);
        *target = cap;
        gSaveContext.magicFillTarget = cap;
    });
}

static RegisterShipInitFunc initFunc(RegisterRandoStatUpgradeHooks, { "IS_RANDO" });

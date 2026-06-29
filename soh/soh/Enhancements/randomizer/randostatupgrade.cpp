#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <soh/OTRGlobals.h>
#include "soh/Enhancements/randomizer/randostatupgrade.h"
#include <libultraship/bridge.h>

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
        uint8_t required = StatUpgradeRequired(5, RSK_DEFENSE_UPGRADE_ADJUSTABLE, RSK_DEFENSE_UPGRADE_TOTAL,
                                               RSK_DEFENSE_UPGRADE_REQUIRED);
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
        uint8_t required =
            StatUpgradeRequired(5, RSK_SPEED_UPGRADE_ADJUSTABLE, RSK_SPEED_UPGRADE_TOTAL, RSK_SPEED_UPGRADE_REQUIRED);
        Player* player = va_arg(args, Player*);
        if (player->stateFlags1 & (PLAYER_STATE1_HOSTILE_LOCK_ON | PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS |
                                    PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE))
            return;
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
        uint8_t required =
            StatUpgradeRequired(5, RSK_POWER_UPGRADE_ADJUSTABLE, RSK_POWER_UPGRADE_TOTAL, RSK_POWER_UPGRADE_REQUIRED);
        PlayState* play = va_arg(args, PlayState*);
        u8* damage = va_arg(args, u8*);
        Actor* hitActor = va_arg(args, Actor*);
        float critChance = (float)std::min((int)level, (int)required) / (float)required;
        if (Rand_ZeroOne() > critChance)
            return;
        u32 doubled = (u32)*damage * 2;
        *damage = (u8)(doubled > 0xFF ? 0xFF : doubled);
        if (critChance < 1.0f) {
            Vec3f zero;
            zero.x = 0.0f;
            zero.y = 0.0f;
            zero.z = 0.0f;
            EffectSsBomb2_SpawnFade(play, &hitActor->world.pos, &zero, &zero);
        }
    });

    COND_VB_SHOULD(VB_MAGIC_FILL_TARGET, IS_RANDO && RAND_GET_OPTION(RSK_MAGIC_STAT_UPGRADE), {
        u8 level = gSaveContext.ship.quest.data.randomizer.magicStatUpgrades;
        if (level == 0)
            return;
        bool adjustable = (bool)RAND_GET_OPTION(RSK_MAGIC_STAT_UPGRADE_ADJUSTABLE);
        uint8_t required = StatUpgradeRequired(8, RSK_MAGIC_STAT_UPGRADE_ADJUSTABLE, RSK_MAGIC_STAT_UPGRADE_TOTAL,
                                               RSK_MAGIC_STAT_UPGRADE_REQUIRED);
        s16 magicTotal = adjustable ? 100 : 96;
        s16 cap = (s16)std::round((float)std::min((int)level, (int)required) * ((float)magicTotal / required));
        s16* fillTarget = va_arg(args, s16*);
        *fillTarget = cap;
        *should = false;
    });

    COND_VB_SHOULD(VB_MAGIC_STEP_CAPACITY_TARGET, IS_RANDO && RAND_GET_OPTION(RSK_MAGIC_STAT_UPGRADE), {
        u8 level = gSaveContext.ship.quest.data.randomizer.magicStatUpgrades;
        if (level == 0)
            return;
        bool adjustable = (bool)RAND_GET_OPTION(RSK_MAGIC_STAT_UPGRADE_ADJUSTABLE);
        uint8_t required = StatUpgradeRequired(8, RSK_MAGIC_STAT_UPGRADE_ADJUSTABLE, RSK_MAGIC_STAT_UPGRADE_TOTAL,
                                               RSK_MAGIC_STAT_UPGRADE_REQUIRED);
        s16 magicTotal = adjustable ? 100 : 96;
        s16 cap = (s16)std::round((float)std::min((int)level, (int)required) * ((float)magicTotal / required));
        s16* target = va_arg(args, s16*);
        *target = cap;
        gSaveContext.magicFillTarget = cap;
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterRandoStatUpgradeHooks, { "IS_RANDO" });

extern "C" bool IsCrawlStatActive() {
    return IS_RANDO && (bool)RAND_GET_OPTION(RSK_CRAWL_SPEED_UPGRADE);
}

extern "C" bool IsClimbStatActive() {
    return IS_RANDO && (bool)RAND_GET_OPTION(RSK_CLIMB_SPEED_UPGRADE);
}

extern "C" bool IsPushStatActive() {
    return IS_RANDO && (bool)RAND_GET_OPTION(RSK_PUSH_SPEED_UPGRADE);
}

extern "C" float GetCrawlStatValue() {
    u8 level = gSaveContext.ship.quest.data.randomizer.crawlSpeedUpgrades;
    uint8_t required = StatUpgradeRequired(5, RSK_CRAWL_SPEED_UPGRADE_ADJUSTABLE, RSK_CRAWL_SPEED_UPGRADE_TOTAL,
                                           RSK_CRAWL_SPEED_UPGRADE_REQUIRED);
    return level == 0 ? 1.0f : 1.0f + (float)std::min((int)level, (int)required) * 4.0f / required;
}

extern "C" float GetClimbStatValue() {
    u8 level = gSaveContext.ship.quest.data.randomizer.climbSpeedUpgrades;
    uint8_t required = StatUpgradeRequired(5, RSK_CLIMB_SPEED_UPGRADE_ADJUSTABLE, RSK_CLIMB_SPEED_UPGRADE_TOTAL,
                                           RSK_CLIMB_SPEED_UPGRADE_REQUIRED);
    return level == 0 ? 0.0f : (float)std::min((int)level, (int)required) * 5.0f / required;
}

extern "C" float GetPushStatValue() {
    u8 level = gSaveContext.ship.quest.data.randomizer.pushSpeedUpgrades;
    uint8_t required = StatUpgradeRequired(5, RSK_PUSH_SPEED_UPGRADE_ADJUSTABLE, RSK_PUSH_SPEED_UPGRADE_TOTAL,
                                           RSK_PUSH_SPEED_UPGRADE_REQUIRED);
    return level == 0 ? 0.0f : (float)std::min((int)level, (int)required) * 5.0f / required;
}

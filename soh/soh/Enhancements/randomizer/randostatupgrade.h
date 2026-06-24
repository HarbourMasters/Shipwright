#pragma once
#include <stdbool.h>

#ifdef __cplusplus
#include <array>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include "SeedContext.h"
#include "randomizerEnums/RandomizerGet.h"
#include "randomizerEnums/RandomizerSettingKey.h"

static constexpr std::array<RandomizerGet, 7> STAT_UPGRADE_ITEMS = { RG_DEFENSE_UPGRADE,     RG_SPEED_UPGRADE,
                                                                     RG_POWER_UPGRADE,       RG_MAGIC_STAT_UPGRADE,
                                                                     RG_CRAWL_SPEED_UPGRADE, RG_CLIMB_SPEED_UPGRADE,
                                                                     RG_PUSH_SPEED_UPGRADE };

// Used to filter out stat items from Links pocket
inline bool IsStatUpgrade(RandomizerGet rg) {
    if (std::find(STAT_UPGRADE_ITEMS.begin(), STAT_UPGRADE_ITEMS.end(), rg) != STAT_UPGRADE_ITEMS.end())
        return true;
    if (rg == RG_PROGRESSIVE_MAGIC_METER && Rando::Context::GetInstance()->GetOption(RSK_MAGIC_STAT_UPGRADE))
        return true;
    return false;
}

// Returns how many stat upgrade items are needed to reach the max stat bonus.
// When the per-stat adjustable toggle is OFF, returns nonAdjDefault; when ON,
// derives from the per-stat total and required % settings.
inline uint8_t StatUpgradeRequired(uint8_t nonAdjDefault, RandomizerSettingKey adjKey, RandomizerSettingKey totalKey,
                                   RandomizerSettingKey requiredKey) {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->GetOption(adjKey)) {
        return nonAdjDefault;
    }
    uint8_t total = ctx->GetOption(totalKey).Get() + 1;
    uint8_t pct = ctx->GetOption(requiredKey).Get() + 1;
    return (uint8_t)std::max(1, (int)std::ceil(total * pct / 100.0));
}

// Returns the number of magic stat upgrade items needed to count as having
// magic in logic (i.e. enough units to fill at least half of a normal magic bar).
inline uint8_t MagicStatLogicThreshold() {
    auto ctx = Rando::Context::GetInstance();
    bool adjustable = (bool)ctx->GetOption(RSK_MAGIC_STAT_UPGRADE_ADJUSTABLE);
    uint8_t required = StatUpgradeRequired(8, RSK_MAGIC_STAT_UPGRADE_ADJUSTABLE, RSK_MAGIC_STAT_UPGRADE_TOTAL,
                                           RSK_MAGIC_STAT_UPGRADE_REQUIRED);
    float magicTotal = adjustable ? 100.0f : 96.0f;
    return (uint8_t)std::ceil(23.5f * required / magicTotal);
}

extern "C" {
#endif

bool IsCrawlStatActive(void);
bool IsClimbStatActive(void);
bool IsPushStatActive(void);
float GetCrawlStatValue(void);
float GetClimbStatValue(void);
float GetPushStatValue(void);

#ifdef __cplusplus
}
#endif

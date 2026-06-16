#pragma once
#include <array>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include "SeedContext.h"
#include "randomizerEnums/RandomizerGet.h"
#include "randomizerEnums/RandomizerSettingKey.h"

static constexpr std::array<RandomizerGet, 4> STAT_UPGRADE_ITEMS = { RG_DEFENSE_UPGRADE, RG_SPEED_UPGRADE,
                                                                     RG_POWER_UPGRADE, RG_MAGIC_STAT_UPGRADE };

// Used to filter out stat items from Links pocket
inline bool IsStatUpgrade(RandomizerGet rg) {
    if (std::find(STAT_UPGRADE_ITEMS.begin(), STAT_UPGRADE_ITEMS.end(), rg) != STAT_UPGRADE_ITEMS.end())
        return true;
    if (rg == RG_PROGRESSIVE_MAGIC_METER && Rando::Context::GetInstance()->GetOption(RSK_MAGIC_STAT_UPGRADE))
        return true;
    return false;
}

// Returns how many stat upgrade items are needed to reach the max stat bonus.
// When adjustable is OFF, returns nonAdjDefault; when ON, derives from the
// configured total and required % settings.
inline uint8_t StatUpgradeRequired(uint8_t nonAdjDefault) {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->GetOption(RSK_ADJUSTABLE_STAT_UPGRADE)) {
        return nonAdjDefault;
    }
    uint8_t total = ctx->GetOption(RSK_STAT_UPGRADE_TOTAL).Get() + 1;
    uint8_t pct = ctx->GetOption(RSK_STAT_UPGRADE_REQUIRED).Get() + 1;
    return (uint8_t)std::max(1, (int)std::ceil(total * pct / 100.0));
}

// Returns the number of magic stat upgrade items needed to count as having
// magic in logic (i.e. enough units to fill at least half of a normal magic bar).
inline uint8_t MagicStatLogicThreshold() {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->GetOption(RSK_ADJUSTABLE_STAT_UPGRADE)) {
        return 2;
    }
    uint8_t required = StatUpgradeRequired(8);
    return (uint8_t)std::ceil(24.0 * required / 100.0);
}

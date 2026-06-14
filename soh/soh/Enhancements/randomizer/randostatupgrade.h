#pragma once
#include <cmath>
#include <algorithm>
#include <cstdint>
#include "SeedContext.h"
#include "randomizerEnums/RandomizerSettingKey.h"

// Returns how many stat upgrade items are needed to reach the max stat bonus.
// When adjustable is OFF, returns nonAdjDefault; when ON, derives from the
// actual post-trim pool count and the configured required %.
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
// magic in logic (i.e. enough units to fill at least one normal magic bar).
inline uint8_t MagicStatLogicThreshold() {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->GetOption(RSK_ADJUSTABLE_STAT_UPGRADE)) {
        return 2;
    }
    uint8_t required = StatUpgradeRequired(8);
    uint8_t unit = (uint8_t)std::max(1, 100 / (int)required);
    return (uint8_t)std::ceil(24.0 / unit);
}

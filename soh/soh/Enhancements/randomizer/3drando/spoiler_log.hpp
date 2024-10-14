#pragma once

#include <array>
#include <string>
#include <string_view>
#include <cstdint>
#include "../randomizerTypes.h"

using RandomizerHash = std::array<std::string, 5>;

// RANDOTODO this is primarily used by the check tracker now, and should probably be moved
typedef enum {
    SPOILER_CHK_NONE,
    SPOILER_CHK_CHEST,
    SPOILER_CHK_COLLECTABLE,
    SPOILER_CHK_GOLD_SKULLTULA,
    SPOILER_CHK_ITEM_GET_INF,
    SPOILER_CHK_EVENT_CHK_INF,
    SPOILER_CHK_INF_TABLE,
    SPOILER_CHK_GRAVEDIGGER,
    SPOILER_CHK_RANDOMIZER_INF,
} SpoilerCollectionCheckType;

void GenerateHash();

const char* SpoilerLog_Write();
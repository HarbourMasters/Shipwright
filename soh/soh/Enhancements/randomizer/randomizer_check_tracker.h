#pragma once
#include <nlohmann/json.hpp>
#include "randomizerTypes.h"
#include "randomizer_check_objects.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

namespace CheckTracker {


//Converts an index into a Little Endian bitmask, as follows:
//00: 0000000100000000
//01: 0000001000000000
//...
//06: 0100000000000000
//07: 1000000000000000
//08: 0000000000000001
//09: 0000000000000010
//...
//14: 0000000001000000
//15: 0000000010000000
//repeat...
#define INDEX_TO_16BIT_LITTLE_ENDIAN_BITMASK(idx) (0x8000 >> (7 - (idx % 8) + ((idx % 16) / 8) * 8))

void InitCheckTracker();
void Teardown();
void DrawCheckTracker(bool& open);
void DefaultCheckData(RandomizerCheck rc);
void Init();
void LoadCheckTrackerData(nlohmann::json checks);
bool IsVisibleInCheckTracker(RandomizerCheckObject rcObj);
void InitTrackerData(bool isDebug);
void SetLastItemGetRC(RandomizerCheck rc);
void CheckTrackerDialogClosed();
bool IsGameRunning();
} // namespace CheckTracker


void to_json(nlohmann::json & j, const RandomizerCheckTrackerData& rctd);
void from_json(const nlohmann::json& j, RandomizerCheckTrackerData& rctd);
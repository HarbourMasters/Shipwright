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
std::map<RandomizerCheck, RandomizerCheckTrackerData> *GetCheckTrackerData();
void PushDefaultCheckData(RandomizerCheck rc);
void LoadCheckTrackerData(nlohmann::json checks);
bool IsVisibleInCheckTracker(RandomizerCheckObject rcObj);
void CreateTrackerData();
void SetLastItemGetRC(RandomizerCheck rc);
bool GetLoadFileChecks();
void SetLoadFileChecks(bool);
void CheckTrackerDialogClosed();

Color_RGBA8 Color_Bg_Default                        = {   0,   0,   0, 255 };   // Black
Color_RGBA8 Color_Main_Default                      = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Area_Incomplete_Extra_Default     = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Area_Complete_Extra_Default       = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Unchecked_Extra_Default           = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Skipped_Main_Default              = { 160, 160, 160, 255 };   // Grey
Color_RGBA8 Color_Skipped_Extra_Default             = { 160, 160, 160, 255 };   // Grey
Color_RGBA8 Color_Seen_Extra_Default                = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Hinted_Extra_Default              = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Collected_Extra_Default           = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Scummed_Extra_Default             = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Saved_Extra_Default               = {   0, 185,   0, 255 };   // Green

Color_RGBA8 Color_Background = { 0, 0, 0, 255 };

Color_RGBA8 Color_Area_Incomplete_Main  = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Incomplete_Extra = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Complete_Main    = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Complete_Extra   = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Unchecked_Main        = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Unchecked_Extra       = { 255, 255, 255, 255 }; // Useless
Color_RGBA8 Color_Skipped_Main          = { 160, 160, 160, 255 }; // Grey
Color_RGBA8 Color_Skipped_Extra         = { 160, 160, 160, 255 }; // Grey
Color_RGBA8 Color_Seen_Main             = { 255, 255, 255, 255 }; // TODO
Color_RGBA8 Color_Seen_Extra            = { 160, 160, 160, 255 }; // TODO
Color_RGBA8 Color_Hinted_Main           = { 255, 255, 255, 255 }; // TODO
Color_RGBA8 Color_Hinted_Extra          = { 255, 255, 255, 255 }; // TODO
Color_RGBA8 Color_Collected_Main        = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Collected_Extra       = { 242, 101,  34, 255 }; // Orange
Color_RGBA8 Color_Scummed_Main          = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Scummed_Extra         = {   0, 174, 239, 255 }; // Blue
Color_RGBA8 Color_Saved_Main            = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Saved_Extra           = {   0, 185,   0, 255 }; // Green
} // namespace CheckTracker

NLOHMANN_JSON_SERIALIZE_ENUM(RandomizerCheckShow, {
    {RCSHOW_UNCHECKED, "unchecked"},
    {RCSHOW_SEEN, "seen"},
    {RCSHOW_SCUMMED, "scummed"},
    {RCSHOW_SAVED, "saved"}
})

void to_json(nlohmann::json & j, const RandomizerCheckTrackerData& rctd);
void from_json(const nlohmann::json& j, RandomizerCheckTrackerData& rctd);
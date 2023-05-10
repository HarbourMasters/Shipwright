#include "randomizer_entrance_tracker.h"
#include "soh/OTRGlobals.h"
#include <ImGuiImpl.h>
#include "soh/UIWidgets.hpp"

#include <map>
#include <string>
#include <vector>
#include <libultraship/bridge.h>
#include <Hooks.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;

#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/randomizer_grotto.h"
}

#include "soh/Enhancements/game-interactor/GameInteractor.h"

#define COLOR_ORANGE IM_COL32(230, 159, 0, 255)
#define COLOR_GREEN IM_COL32(0, 158, 115, 255)
#define COLOR_GRAY IM_COL32(155, 155, 155, 255)

EntranceOverride srcListSortedByArea[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceOverride destListSortedByArea[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceOverride srcListSortedByType[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceOverride destListSortedByType[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceTrackingData gEntranceTrackingData = {0};

static const EntranceOverride emptyOverride = {0};

static s16 lastEntranceIndex = -1;
static s16 currentGrottoId = -1;
static s16 lastSceneOrEntranceDetected = -1;

static std::string spoilerEntranceGroupNames[] = {
    "Spawns/Warp Songs/Owls",
    "Kokiri Forest",
    "Lost Woods",
    "Sacred Forest Meadow",
    "Kakariko Village",
    "Graveyard",
    "Death Mountain Trail",
    "Death Mountain Crater",
    "Goron City",
    "Zora's River",
    "Zora's Domain",
    "Zora's Fountain",
    "Hyrule Field",
    "Lon Lon Ranch",
    "Lake Hylia",
    "Gerudo Valley",
    "Haunted Wasteland",
    "Market",
    "Hyrule Castle",
};

static std::string groupTypeNames[] = {
    "One Way",
    "Overworld",
    "Interior",
    "Grotto",
    "Dungeon",
};

// Entrance data for the tracker taken from the 3ds rando entrance tracker, and supplemented with scene/spawn info and meta search tags
const EntranceData entranceData[] = {
    //index,  reverse, scenes (and spawns),     source name,   destination name, source group,           destination group,      type,                 metaTag, oneExit
    { 0x00BB, -1,      SINGLE_SCENE_INFO(0x34), "Child Spawn", "Link's House",   ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x0282, -1,      SINGLE_SCENE_INFO(0x43), "Adult Spawn", "Temple of Time", ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    { 0x0600, -1, {{ -1 }}, "Minuet of Forest",   "SFM Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x04F6, -1, {{ -1 }}, "Bolero of Fire",     "DMC Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x0604, -1, {{ -1 }}, "Serenade of Water",  "Lake Hylia Warp Pad",       ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x01F1, -1, {{ -1 }}, "Requiem of Spirit",  "Desert Colossus Warp Pad",  ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x0568, -1, {{ -1 }}, "Nocturne of Shadow", "Graveyard Warp Pad",        ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x05F4, -1, {{ -1 }}, "Prelude of Light",   "Temple of Time Warp Pad",   ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    { 0x0554, -1, SINGLE_SCENE_INFO(0x60), "DMT Owl Flight", "Kakariko Village Owl Drop", ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x027E, -1, SINGLE_SCENE_INFO(0x57), "LH Owl Flight",  "Hyrule Field Owl Drop",     ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    // Kokiri Forest
    { 0x05E0, 0x020D, SINGLE_SCENE_INFO(0x55), "KF",                  "Lost Woods Bridge",   ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x011E, 0x0286, SINGLE_SCENE_INFO(0x55), "KF",                  "Lost Woods",          ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0272, 0x0211, SINGLE_SCENE_INFO(0x55), "KF",                  "Link's House",        ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0433, 0x0443, SINGLE_SCENE_INFO(0x55), "KF",                  "Mido's House",        ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0437, 0x0447, SINGLE_SCENE_INFO(0x55), "KF",                  "Saria's House",       ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x009C, 0x033C, SINGLE_SCENE_INFO(0x55), "KF",                  "House of Twins",      ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x00C9, 0x026A, SINGLE_SCENE_INFO(0x55), "KF",                  "Know-It-All House",   ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x00C1, 0x0266, SINGLE_SCENE_INFO(0x55), "KF",                  "KF Shop",             ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x071B, 0x081B, SINGLE_SCENE_INFO(0x55), "KF",                  "KF Storms Grotto",    ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0000, 0x0209, SINGLE_SCENE_INFO(0x55), "KF",                  "Deku Tree",           ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { 0x0211, 0x0272, SINGLE_SCENE_INFO(0x34), "Link's House",        "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { 0x0443, 0x0433, SINGLE_SCENE_INFO(0x28), "Mido's House",        "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { 0x0447, 0x0437, SINGLE_SCENE_INFO(0x29), "Saria's House",       "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { 0x033C, 0x009C, SINGLE_SCENE_INFO(0x27), "House of Twins",      "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { 0x026A, 0x00C9, SINGLE_SCENE_INFO(0x26), "Know-It-All House",   "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { 0x0266, 0x00C1, SINGLE_SCENE_INFO(0x2D), "KF Shop",             "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { 0x081B, 0x071B, {{ 0x3E, 0x00 }},        "KF Storms Grotto",    "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO,    "chest"},
    { 0x0209, 0x0000, SINGLE_SCENE_INFO(0x00), "Deku Tree",           "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   ""},
    { 0x040F, 0x0252, SINGLE_SCENE_INFO(0x00), "Deku Tree Boss Door", "Gohma",               ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { 0x0252, 0x040F, SINGLE_SCENE_INFO(0x11), "Gohma",               "Deku Tree Boss Door", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},

    // Lost Woods
    { 0x020D, 0x05E0, SINGLE_SCENE_INFO(0x5B), "Lost Woods Bridge",        "KF",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0185, 0x04DE, SINGLE_SCENE_INFO(0x5B), "Lost Woods Bridge",        "Hyrule Field",             ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "lw,hf"},
    { 0x0286, 0x011E, SINGLE_SCENE_INFO(0x5B), "Lost Woods",               "KF",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x04E2, 0x04D6, SINGLE_SCENE_INFO(0x5B), "Lost Woods",               "Goron City",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_GORON_CITY,    ENTRANCE_TYPE_OVERWORLD, "lw,gc"},
    { 0x01DD, 0x04DA, SINGLE_SCENE_INFO(0x5B), "Lost Woods",               "ZR",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x00FC, 0x01A9, SINGLE_SCENE_INFO(0x5B), "Lost Woods",               "SFM",                      ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_SFM,           ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x071A, 0x081A, SINGLE_SCENE_INFO(0x5B), "Lost Woods",               "LW Near Shortcuts Grotto", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,chest", 1},
    { 0x0719, 0x0819, SINGLE_SCENE_INFO(0x5B), "Lost Woods",               "LW Scrubs Grotto",         ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw", 1},
    { 0x0720, 0x0820, SINGLE_SCENE_INFO(0x5B), "Lost Woods",               "Deku Theater",             ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,mask,stage", 1},
    { 0x081A, 0x071A, {{ 0x3E, 0x00 }},        "LW Near Shortcuts Grotto", "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,chest"},
    { 0x0819, 0x0719, {{ 0x3E, 0x07 }},        "LW Scrubs Grotto",         "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw"},
    { 0x0820, 0x0720, {{ 0x3E, 0x0C }},        "Deku Theater",             "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,mask,stage"},

    // Sacred Forest Meadow
    { 0x01A9, 0x00FC, SINGLE_SCENE_INFO(0x56), "SFM",                     "Lost Woods",              ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0716, 0x0816, SINGLE_SCENE_INFO(0x56), "SFM",                     "SFM Wolfos Grotto",       ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0718, 0x0818, SINGLE_SCENE_INFO(0x56), "SFM",                     "SFM Fairy Grotto",        ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "", 1},
    { 0x0717, 0x0817, SINGLE_SCENE_INFO(0x56), "SFM",                     "SFM Storms Grotto",       ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { 0x0169, 0x0215, SINGLE_SCENE_INFO(0x56), "SFM",                     "Forest Temple",           ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON,   "", 1},
    { 0x0816, 0x0716, {{ 0x3E, 0x08 }},        "SFM Wolfos Grotto",       "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { 0x0818, 0x0718, {{ 0x3C, 0x00 }},        "SFM Fairy Grotto",        "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { 0x0817, 0x0717, {{ 0x3E, 0x0A }},        "SFM Storms Grotto",       "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { 0x0215, 0x0169, SINGLE_SCENE_INFO(0x03), "Forest Temple",           "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON},
    { 0x000C, 0x024E, SINGLE_SCENE_INFO(0x03), "Forest Temple Boss Door", "Phantom Ganon",           ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON, "", 1},
    { 0x024E, 0x000C, SINGLE_SCENE_INFO(0x14), "Phantom Ganon",           "Forest Temple Boss Door", ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON, "", 1},

    // Kakariko Village
    { 0x017D, 0x00DB, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Hyrule Field",          ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_HYRULE_FIELD,         ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x00E4, 0x0195, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Graveyard",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_GRAVEYARD,            ENTRANCE_TYPE_OVERWORLD},
    { 0x013D, 0x0191, SINGLE_SCENE_INFO(0x52), "Kakariko",              "DMT",                   ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_OVERWORLD},
    { 0x02FD, 0x0349, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Carpenter Boss House",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0550, 0x04EE, SINGLE_SCENE_INFO(0x52), "Kakariko",              "House of Skulltula",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x039C, 0x0345, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Impa's House Front",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x05C8, 0x05DC, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Impa's House Back",     ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { 0x0453, 0x0351, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Windmill",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x003B, 0x0463, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Kak Shooting Gallery",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "adult", 1},
    { 0x0072, 0x034D, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Granny's Potion Shop",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x00B7, 0x0201, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Kak Bazaar",            ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "shop", 1},
    { 0x0384, 0x044B, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Kak Potion Shop Front", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x03EC, 0x04FF, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Kak Potion Shop Back",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x070A, 0x080A, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Kak Open Grotto",       ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x070B, 0x080B, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Kak Redead Grotto",     ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0098, 0x02A6, SINGLE_SCENE_INFO(0x52), "Kakariko",              "Bottom of the Well",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON,   "botw", 1},
    { 0x0349, 0x02FD, SINGLE_SCENE_INFO(0x2A), "Carpenter Boss House",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x04EE, 0x0550, SINGLE_SCENE_INFO(0x50), "House of Skulltula",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x0345, 0x039C, SINGLE_SCENE_INFO(0x37), "Impa's House Front",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x05DC, 0x05C8, SINGLE_SCENE_INFO(0x37), "Impa's House Back",     "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "cow"},
    { 0x0351, 0x0453, SINGLE_SCENE_INFO(0x48), "Windmill",              "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x0463, 0x003B, {{ 0x42, 0x00 }},        "Kak Shooting Gallery",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x034D, 0x0072, SINGLE_SCENE_INFO(0x4E), "Granny's Potion Shop",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x0201, 0x00B7, {{ 0x2C, 0x00 }},        "Kak Bazaar",            "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "shop"},
    { 0x044B, 0x0384, SINGLE_SCENE_INFO(0x30), "Kak Potion Shop Front", "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x04FF, 0x03EC, SINGLE_SCENE_INFO(0x30), "Kak Potion Shop Back",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x080A, 0x070A, {{ 0x3E, 0x00 }},        "Kak Open Grotto",       "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest"},
    { 0x080B, 0x070B, {{ 0x3E, 0x03 }},        "Kak Redead Grotto",     "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest"},
    { 0x02A6, 0x0098, SINGLE_SCENE_INFO(0x08), "Bottom of the Well",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON,   "botw"},

    // The Graveyard
    { 0x0195, 0x00E4, SINGLE_SCENE_INFO(0x53), "Graveyard",               "Kakariko",                ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_KAKARIKO,  ENTRANCE_TYPE_OVERWORLD},
    { 0x030D, 0x0355, SINGLE_SCENE_INFO(0x53), "Graveyard",               "Dampe's Shack",           ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x004B, 0x035D, SINGLE_SCENE_INFO(0x53), "Graveyard",               "Shield Grave",            ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x031C, 0x0361, SINGLE_SCENE_INFO(0x53), "Graveyard",               "Heart Piece Grave",       ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x002D, 0x050B, SINGLE_SCENE_INFO(0x53), "Graveyard",               "Composer's Grave",        ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x044F, 0x0359, SINGLE_SCENE_INFO(0x53), "Graveyard",               "Dampe's Grave",           ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "race", 1},
    { 0x0037, 0x0205, SINGLE_SCENE_INFO(0x53), "Graveyard",               "Shadow Temple",           ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { 0x0355, 0x030D, SINGLE_SCENE_INFO(0x3A), "Dampe's Shack",           "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR},
    { 0x035D, 0x004B, SINGLE_SCENE_INFO(0x40), "Shield Grave",            "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { 0x0361, 0x031C, SINGLE_SCENE_INFO(0x3F), "Heart Piece Grave",       "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { 0x050B, 0x002D, SINGLE_SCENE_INFO(0x41), "Composer's Grave",        "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { 0x0359, 0x044F, SINGLE_SCENE_INFO(0x48), "Dampe's Grave",           "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "race"},
    { 0x0205, 0x0037, SINGLE_SCENE_INFO(0x07), "Shadow Temple",           "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON},
    { 0x0413, 0x02B2, SINGLE_SCENE_INFO(0x07), "Shadow Temple Boss Door", "Bongo-Bongo",             ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON, "", 1},
    { 0x02B2, 0x0413, SINGLE_SCENE_INFO(0x18), "Bongo-Bongo",             "Shadow Temple Boss Door", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON, "", 1},

    // Death Mountain Trail
    { 0x0191, 0x013D, SINGLE_SCENE_INFO(0x60), "DMT",                        "Kakariko",                   ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_KAKARIKO,              ENTRANCE_TYPE_OVERWORLD},
    { 0x014D, 0x01B9, SINGLE_SCENE_INFO(0x60), "DMT",                        "Goron City",                 ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x0147, 0x01BD, SINGLE_SCENE_INFO(0x60), "DMT",                        "DMC",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD},
    { 0x0315, 0x045B, SINGLE_SCENE_INFO(0x60), "DMT",                        "DMT Great Fairy Fountain",   ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0708, 0x0808, SINGLE_SCENE_INFO(0x60), "DMT",                        "DMT Storms Grotto",          ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0709, 0x0809, SINGLE_SCENE_INFO(0x60), "DMT",                        "DMT Cow Grotto",             ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { 0x0004, 0x0242, SINGLE_SCENE_INFO(0x60), "DMT",                        "Dodongo's Cavern",           ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { 0x045B, 0x0315, {{ 0x3B, 0x00 }},        "DMT Great Fairy Fountain",   "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR},
    { 0x0808, 0x0708, {{ 0x3E, 0x00 }},        "DMT Storms Grotto",          "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "chest"},
    { 0x0809, 0x0709, {{ 0x3E, 0x0D }},        "DMT Cow Grotto",             "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO},
    { 0x0242, 0x0004, SINGLE_SCENE_INFO(0x01), "Dodongo's Cavern",           "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc"},
    { 0x040B, 0x00C5, SINGLE_SCENE_INFO(0x01), "Dodongo's Cavern Boss Door", "King Dodongo",               ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { 0x00C5, 0x040B, SINGLE_SCENE_INFO(0x12), "King Dodongo",               "Dodongo's Cavern Boss Door", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},

    // Death Mountain Crater
    { 0x01C1, 0x0246, SINGLE_SCENE_INFO(0x61), "DMC",                      "Goron City",               ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x01BD, 0x0147, SINGLE_SCENE_INFO(0x61), "DMC",                      "DMT",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD},
    { 0x04BE, 0x0482, SINGLE_SCENE_INFO(0x61), "DMC",                      "DMC Great Fairy Fountain", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0706, 0x0806, SINGLE_SCENE_INFO(0x61), "DMC",                      "DMC Upper Grotto",         ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0705, 0x0805, SINGLE_SCENE_INFO(0x61), "DMC",                      "DMC Hammer Grotto",        ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { 0x0165, 0x024A, SINGLE_SCENE_INFO(0x61), "DMC",                      "Fire Temple",              ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { 0x0482, 0x04BE, {{ 0x3B, 0x01 }},        "DMC Great Fairy Fountain", "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR},
    { 0x0806, 0x0706, {{ 0x3E, 0x00 }},        "DMC Upper Grotto",         "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "chest"},
    { 0x0805, 0x0705, {{ 0x3E, 0x04 }},        "DMC Hammer Grotto",        "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { 0x024A, 0x0165, SINGLE_SCENE_INFO(0x04), "Fire Temple",              "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON},
    { 0x0305, 0x0175, SINGLE_SCENE_INFO(0x04), "Fire Temple Boss Door",    "Volvagia",                 ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { 0x0175, 0x0305, SINGLE_SCENE_INFO(0x15), "Volvagia",                 "Fire Temple Boss Door",    ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},

    // Goron City
    { 0x01B9, 0x014D, SINGLE_SCENE_INFO(0x62), "Goron City",        "DMT",               ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x0246, 0x01C1, SINGLE_SCENE_INFO(0x62), "Goron City",        "DMC",               ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x04D6, 0x04E2, SINGLE_SCENE_INFO(0x62), "Goron City",        "Lost Woods",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_LOST_WOODS,            ENTRANCE_TYPE_OVERWORLD, "gc,lw"},
    { 0x037C, 0x03FC, SINGLE_SCENE_INFO(0x62), "Goron City",        "Goron Shop",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR,  "gc", 1},
    { 0x0707, 0x0807, SINGLE_SCENE_INFO(0x62), "Goron City",        "Goron City Grotto", ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO,    "gc,scrubs", 1},
    { 0x03FC, 0x037C, SINGLE_SCENE_INFO(0x2E), "Goron Shop",        "Goron City",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR,  "gc"},
    { 0x0807, 0x0707, {{ 0x3E, 0x04 }},        "Goron City Grotto", "Goron City",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO,    "gc,scrubs"},

    // Zora's River
    { 0x0181, 0x00EA, SINGLE_SCENE_INFO(0x54), "ZR",               "Hyrule Field",     ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x04DA, 0x01DD, SINGLE_SCENE_INFO(0x54), "ZR",               "Lost Woods",       ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_LOST_WOODS,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0108, 0x019D, SINGLE_SCENE_INFO(0x54), "ZR",               "Zora's Domain",    ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD},
    { 0x0702, 0x0802, SINGLE_SCENE_INFO(0x54), "ZR",               "ZR Storms Grotto", ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { 0x0703, 0x0803, SINGLE_SCENE_INFO(0x54), "ZR",               "ZR Fairy Grotto",  ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { 0x0704, 0x0804, SINGLE_SCENE_INFO(0x54), "ZR",               "ZR Open Grotto",   ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0802, 0x0702, {{ 0x3E, 0x0A }},        "ZR Storms Grotto", "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { 0x0803, 0x0703, {{ 0x3C, 0x00 }},        "ZR Fairy Grotto",  "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO},
    { 0x0804, 0x0704, {{ 0x3E, 0x00 }},        "ZR Open Grotto",   "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "chest"},

    // Zora's Domain
    { 0x019D, 0x0108, SINGLE_SCENE_INFO(0x58), "Zora's Domain",    "ZR",               ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_RIVER,    ENTRANCE_TYPE_OVERWORLD},
    { 0x0560, 0x0328, SINGLE_SCENE_INFO(0x58), "Zora's Domain",    "Lake Hylia",       ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_LAKE_HYLIA,     ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x0225, 0x01A1, SINGLE_SCENE_INFO(0x58), "Zora's Domain",    "ZF",               ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_OVERWORLD},
    { 0x0380, 0x03C4, SINGLE_SCENE_INFO(0x58), "Zora's Domain",    "Zora Shop",        ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x071C, 0x081C, SINGLE_SCENE_INFO(0x58), "Zora's Domain",    "ZD Storms Grotto", ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO,    "fairy", 1},
    { 0x03C4, 0x0380, SINGLE_SCENE_INFO(0x2F), "Zora Shop",        "Zora's Domain",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR},
    { 0x081C, 0x071C, {{ 0x3C, 0x00 }},        "ZD Storms Grotto", "Zora's Domain",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO,    "fairy"},

    // Zora's Fountain
    { 0x01A1, 0x0225, SINGLE_SCENE_INFO(0x59), "ZF",                          "Zora's Domain",               ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_OVERWORLD},
    { 0x0371, 0x0394, SINGLE_SCENE_INFO(0x59), "ZF",                          "ZF Great Fairy Fountain",     ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0028, 0x0221, SINGLE_SCENE_INFO(0x59), "ZF",                          "Jabu Jabu's Belly",           ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { 0x0088, 0x03D4, SINGLE_SCENE_INFO(0x59), "ZF",                          "Ice Cavern",                  ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { 0x0394, 0x0371, {{ 0x3D, 0x00 }},        "ZF Great Fairy Fountain",     "ZF",                          ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR},
    { 0x0221, 0x0028, SINGLE_SCENE_INFO(0x02), "Jabu Jabu's Belly",           "ZF",                          ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},
    { 0x0301, 0x0407, SINGLE_SCENE_INFO(0x02), "Jabu Jabu's Belly Boss Door", "Barinade",                    ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON, "", 1},
    { 0x0407, 0x0301, SINGLE_SCENE_INFO(0x13), "Barinade",                    "Jabu Jabu's Belly Boss Door", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON, "", 1},
    { 0x03D4, 0x0088, SINGLE_SCENE_INFO(0x09), "Ice Cavern",                  "ZF",                          ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},

    // Hyrule Field
    { 0x04DE, 0x0185, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "Lost Woods Bridge",      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "hf,lw"},
    { 0x0276, 0x01FD, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "Market Entrance",        ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0157, 0x01F9, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "Lon Lon Ranch",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_OVERWORLD, "hf,llr"},
    { 0x00DB, 0x017D, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "Kakariko",               ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_KAKARIKO,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x00EA, 0x0181, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "ZR",                     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0102, 0x0189, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "Lake Hylia",             ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LAKE_HYLIA,    ENTRANCE_TYPE_OVERWORLD, "hf,lh"},
    { 0x0117, 0x018D, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "GV",                     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0710, 0x0810, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Near Market Grotto",  ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x070E, 0x080E, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Near Kak Grotto",     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "spider", 1},
    { 0x070D, 0x080D, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Tektite Grotto",      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "water", 1},
    { 0x070F, 0x080F, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Fairy Grotto",        ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { 0x0711, 0x0811, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Cow Grotto",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "webbed", 1},
    { 0x0713, 0x0813, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Open Grotto",         ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0712, 0x0812, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Inside Fence Grotto", ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { 0x0714, 0x0814, SINGLE_SCENE_INFO(0x51), "Hyrule Field",           "HF Southeast Grotto",    ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { 0x0810, 0x0710, {{ 0x3E, 0x00 }},        "HF Near Market Grotto",  "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { 0x080E, 0x070E, {{ 0x3E, 0x01 }},        "HF Near Kak Grotto",     "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "spider"},
    { 0x080D, 0x070D, {{ 0x3E, 0x0B }},        "HF Tektite Grotto",      "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "water"},
    { 0x080F, 0x070F, {{ 0x3C, 0x00 }},        "HF Fairy Grotto",        "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { 0x0811, 0x0711, {{ 0x3E, 0x05 }},        "HF Cow Grotto",          "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "webbed"},
    { 0x0813, 0x0713, {{ 0x3E, 0x00 }},        "HF Open Grotto",         "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest"},
    { 0x0812, 0x0712, {{ 0x3E, 0x02 }},        "HF Inside Fence Grotto", "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "srubs"},
    { 0x0814, 0x0714, {{ 0x3E, 0x00 }},        "HF Southeast Grotto",    "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest"},

    // Lon Lon Ranch
    { 0x01F9, 0x0157, SINGLE_SCENE_INFO(0x63), "Lon Lon Ranch", "Hyrule Field",  ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x004F, 0x0378, SINGLE_SCENE_INFO(0x63), "Lon Lon Ranch", "Talon's House", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "llr", 1},
    { 0x02F9, 0x042F, SINGLE_SCENE_INFO(0x63), "Lon Lon Ranch", "LLR Stables",   ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { 0x05D0, 0x05D4, SINGLE_SCENE_INFO(0x63), "Lon Lon Ranch", "LLR Tower",     ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { 0x0715, 0x0815, SINGLE_SCENE_INFO(0x63), "Lon Lon Ranch", "LLR Grotto",    ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { 0x0378, 0x004F, {{ 0x4C, 0x00 }},        "Talon's House", "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "llr"},
    { 0x042F, 0x02F9, SINGLE_SCENE_INFO(0x36), "LLR Stables",   "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow"},
    { 0x05D4, 0x05D0, {{ 0x4C, 0x01 }},        "LLR Tower",     "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow"},
    { 0x0815, 0x0715, {{ 0x3E, 0x04 }},        "LLR Grotto",    "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO,    "scrubs"},

    // Lake Hylia
    { 0x0189, 0x0102, SINGLE_SCENE_INFO(0x57), "Lake Hylia",             "Hyrule Field",           ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x0328, 0x0560, SINGLE_SCENE_INFO(0x57), "Lake Hylia",             "Zora's Domain",          ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x0043, 0x03CC, SINGLE_SCENE_INFO(0x57), "Lake Hylia",             "LH Lab",                 ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh", 1},
    { 0x045F, 0x0309, SINGLE_SCENE_INFO(0x57), "Lake Hylia",             "Fishing Hole",           ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh", 1},
    { 0x0701, 0x0801, SINGLE_SCENE_INFO(0x57), "Lake Hylia",             "LH Grotto",              ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { 0x0010, 0x021D, SINGLE_SCENE_INFO(0x57), "Lake Hylia",             "Water Temple",           ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},
    { 0x03CC, 0x0043, SINGLE_SCENE_INFO(0x38), "LH Lab",                 "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh"},
    { 0x0309, 0x045F, SINGLE_SCENE_INFO(0x49), "Fishing Hole",           "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh"},
    { 0x0801, 0x0701, {{ 0x3E, 0x04 }},        "LH Grotto",              "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO,    "lh,scrubs"},
    { 0x021D, 0x0010, SINGLE_SCENE_INFO(0x05), "Water Temple",           "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh"},
    { 0x0417, 0x0423, SINGLE_SCENE_INFO(0x05), "Water Temple Boss Door", "Morpha",                 ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},
    { 0x0423, 0x0417, SINGLE_SCENE_INFO(0x16), "Morpha",                 "Water Temple Boss Door", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},

    // Gerudo Area
    { 0x018D, 0x0117, SINGLE_SCENE_INFO(0x5A), "GV",                      "Hyrule Field",            ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_HYRULE_FIELD,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0129, 0x022D, SINGLE_SCENE_INFO(0x5A), "GV",                      "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { 0x0219, -1,     SINGLE_SCENE_INFO(0x5A), "GV",                      "Lake Hylia",              ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_LAKE_HYLIA,        ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x03A0, 0x03D0, SINGLE_SCENE_INFO(0x5A), "GV",                      "Carpenters' Tent",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x071F, 0x081F, SINGLE_SCENE_INFO(0x5A), "GV",                      "GV Octorok Grotto",       ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "", 1},
    { 0x071E, 0x081E, SINGLE_SCENE_INFO(0x5A), "GV",                      "GV Storms Grotto",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { 0x022D, 0x0129, SINGLE_SCENE_INFO(0x5D), "GF",                      "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { 0x0130, 0x03AC, SINGLE_SCENE_INFO(0x5D), "GF",                      "Haunted Wasteland",       ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { 0x071D, 0x081D, SINGLE_SCENE_INFO(0x5D), "GF",                      "GF Storms Grotto",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "gerudo fortress", 1},
    { 0x0008, 0x03A8, SINGLE_SCENE_INFO(0x5D), "GF",                      "Gerudo Training Grounds", ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_DUNGEON,   "gerudo fortress,gtg", 1},
    { 0x03D0, 0x03A0, SINGLE_SCENE_INFO(0x39), "Carpenters' Tent",        "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR},
    { 0x081F, 0x071F, {{ 0x3E, 0x06 }},        "GV Octorok Grotto",       "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO},
    { 0x081E, 0x071E, {{ 0x3E, 0x0A }},        "GV Storms Grotto",        "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { 0x081D, 0x071D, {{ 0x3C, 0x00 }},        "GF Storms Grotto",        "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "gerudo fortress"},
    { 0x03A8, 0x0008, SINGLE_SCENE_INFO(0x0B), "Gerudo Training Grounds", "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_DUNGEON,   "gerudo fortress,gtg"},

    // The Wasteland
    { 0x03AC, 0x0130, SINGLE_SCENE_INFO(0x5E), "Haunted Wasteland",             "GF",                            ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "hw,gerudo fortress"},
    { 0x0123, 0x0365, SINGLE_SCENE_INFO(0x5E), "Haunted Wasteland",             "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { 0x0365, 0x0123, SINGLE_SCENE_INFO(0x5C), "Desert Colossus",               "Haunted Wasteland",             ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { 0x0588, 0x057C, SINGLE_SCENE_INFO(0x5C), "Colossus",                      "Colossus Great Fairy Fountain", ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR,  "dc", 1},
    { 0x0700, 0x0800, SINGLE_SCENE_INFO(0x5C), "Desert Colossus",               "Colossus Grotto",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO,    "dc,scrubs", 1},
    { 0x0082, 0x01E1, SINGLE_SCENE_INFO(0x5C), "Desert Colossus",               "Spirit Temple",                 ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { 0x057C, 0x0588, {{ 0x3D, 0x02 }},        "Colossus Great Fairy Fountain", "Colossus",                      ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR,  "dc"},
    { 0x0800, 0x0700, {{ 0x3E, 0x0A }},        "Colossus Grotto",               "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO,    "dc,scrubs"},
    { 0x01E1, 0x0082, SINGLE_SCENE_INFO(0x06), "Spirit Temple",                 "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "dc"},
    { 0x008D, 0x02F5, SINGLE_SCENE_INFO(0x06), "Spirit Temple Boss Door",       "Twinrova",                      ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { 0x02F5, 0x008D, SINGLE_SCENE_INFO(0x17), "Twinrova",                      "Spirit Temple Boss Door",       ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "", 1},

    // Market
    { 0x01FD, 0x0276, {SCENE_NO_SPAWN(0x1B), SCENE_NO_SPAWN(0x1C), SCENE_NO_SPAWN(0x1D)}, "Market Entrance",   "Hyrule Field",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x00B1, 0x0033, {SCENE_NO_SPAWN(0x1B), SCENE_NO_SPAWN(0x1C), SCENE_NO_SPAWN(0x1D)}, "Market Entrance",   "Market",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { 0x007E, 0x026E, {SCENE_NO_SPAWN(0x1B), SCENE_NO_SPAWN(0x1C), SCENE_NO_SPAWN(0x1D)}, "Market Entrance",   "Guard House",         ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "pots,poe", 1},
    { 0x0033, 0x00B1, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "Market Entrance",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { 0x0138, 0x025A, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "HC Grounds / OGC",       ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { 0x0171, 0x025E, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "Outside Temple of Time", ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { 0x016D, 0x01CD, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "MK Shooting Gallery",    ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "child", 1},
    { 0x0507, 0x03BC, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "Bombchu Bowling",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0063, 0x01D5, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "Treasure Chest Game",    ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x043B, 0x0067, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "Man-in-Green's House",   ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0530, 0x01D1, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "Mask Shop",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x052C, 0x03B8, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "MK Bazaar",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "shop", 1},
    { 0x0388, 0x02A2, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "MK Potion Shop",         ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x0528, 0x03C0, {SCENE_NO_SPAWN(0x20), SCENE_NO_SPAWN(0x21), SCENE_NO_SPAWN(0x22), SCENE_NO_SPAWN(0x1E), SCENE_NO_SPAWN(0x1F)}, "Market",              "Bombchu Shop",           ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { 0x026E, 0x007E, {{ 0x4D }},              "Guard House",                             "Market Entrance",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR, "pots,poe"},
    { 0x01CD, 0x016D, {{ 0x42, 0x01 }},        "MK Shooting Gallery",                     "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR},
    { 0x03BC, 0x0507, SINGLE_SCENE_INFO(0x4B), "Bombchu Bowling",                         "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR},
    { 0x01D5, 0x0063, SINGLE_SCENE_INFO(0x10), "Treasure Chest Game",                     "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR},
    { 0x0067, 0x043B, SINGLE_SCENE_INFO(0x2B), "Man-in-Green's House",                    "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR},
    { 0x01D1, 0x0530, SINGLE_SCENE_INFO(0x33), "Mask Shop",                               "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR},
    { 0x03B8, 0x052C, {{ 0x2C, 0x01 }},        "MK Bazaar",                               "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR, "shop"},
    { 0x02A2, 0x0388, SINGLE_SCENE_INFO(0x31), "MK Potion Shop",                          "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR},
    { 0x03C0, 0x0528, SINGLE_SCENE_INFO(0x32), "Bombchu Shop",                            "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR},
    { 0x025E, 0x0171, {SCENE_NO_SPAWN(0x23), SCENE_NO_SPAWN(0x24), SCENE_NO_SPAWN(0x25)}, "Outside Temple of Time", "Market",                ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "tot"},
    { 0x0053, 0x0472, {SCENE_NO_SPAWN(0x23), SCENE_NO_SPAWN(0x24), SCENE_NO_SPAWN(0x25)}, "Outside Temple of Time", "Temple of Time",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "tot", 1},
    { 0x0472, 0x0053, SINGLE_SCENE_INFO(0x43), "Temple of Time",                          "Outside Temple of Time", ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,      ENTRANCE_TYPE_INTERIOR, "tot"},

    // Hyrule Castle
    { 0x025A, 0x0138, {SCENE_NO_SPAWN(0x5F), SCENE_NO_SPAWN(0x64)},        "HC Grounds / OGC",         "Market",                     ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { 0x0578, 0x0340, SINGLE_SCENE_INFO(0x5F), "HC Grounds",               "HC Great Fairy Fountain",  ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x070C, 0x080C, SINGLE_SCENE_INFO(0x5F), "HC Grounds",               "HC Storms Grotto",         ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO,   "bombable", 1},
    { 0x0340, 0x0578, {{ 0x3D, 0x01 }},        "HC Great Fairy Fountain",  "HC Grounds",               ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR},
    { 0x080C, 0x070C, {{ 0x3E, 0x09 }},        "HC Storms Grotto",         "HC Grounds",               ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO,   "bombable"},
    { 0x04C2, 0x03E8, SINGLE_SCENE_INFO(0x64), "OGC",                      "OGC Great Fairy Fountain", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, "outside ganon's castle", 1},
    { 0x0467, 0x023D, SINGLE_SCENE_INFO(0x64), "OGC",                      "Ganon's Castle",           ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,  "outside ganon's castle,gc", 1},
    { 0x03E8, 0x04C2, {{ 0x3B, 0x02 }},        "OGC Great Fairy Fountain", "OGC",                      ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, "outside ganon's castle"},
    { 0x023D, 0x0467, SINGLE_SCENE_INFO(0x0D), "Ganon's Castle",           "OGC",                      ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,  "outside ganon's castle,gc"}
};

// Check if Link is in the area and return that scene/entrance for tracking
s8 LinkIsInArea(const EntranceData* entrance) {
    bool result = false;

    if (gPlayState == nullptr) {
        return -1;
    }

    // Handle detecting the current grotto
    if ((gPlayState->sceneNum == SCENE_YOUSEI_IZUMI_TATE || gPlayState->sceneNum == SCENE_KAKUSIANA) &&
        entrance->type == ENTRANCE_TYPE_GROTTO) {
        if (entrance->index == (ENTRANCE_RANDO_GROTTO_EXIT_START + currentGrottoId)) {
            // Return the grotto entrance for tracking
            return entrance->index;
        } else {
            return -1;
        }
    }

    // Otherwise check all scenes/spawns
    // Not all areas require a spawn position to differeniate between another area
    for (auto info : entrance->scenes) {
        // When a spawn position is specified, check that combination
        if (info.spawn != -1) {
            result = Entrance_SceneAndSpawnAre(info.scene, info.spawn);
        } else { // Otherwise just check the current scene
            result = gPlayState->sceneNum == info.scene;
        }

        // Return the scene for tracking
        if (result) {
            return info.scene;
        }
    }

    return -1;
}

bool IsEntranceDiscovered(s16 index) {
    bool isDiscovered = Entrance_GetIsEntranceDiscovered(index);
    if (!isDiscovered) {
        // If the pair included one of the hyrule field <-> zora's river entrances,
        // the randomizer will have also overriden the water-based entrances, so check those too
        if ((index == 0x00EA && Entrance_GetIsEntranceDiscovered(0x01D9)) || (index == 0x01D9 && Entrance_GetIsEntranceDiscovered(0x00EA))) {
            isDiscovered = true;
        } else if ((index == 0x0181 && Entrance_GetIsEntranceDiscovered(0x0311)) || (index == 0x0311 && Entrance_GetIsEntranceDiscovered(0x0181))) {
            isDiscovered = true;
        }
    }
    return isDiscovered;
}

const EntranceData* GetEntranceData(s16 index) {
    for (size_t i = 0; i < ARRAY_COUNT(entranceData); i++) {
        if (index == entranceData[i].index) {
            return &entranceData[i];
        }
    }
    // Shouldn't be reached
    return nullptr;
}

void SortEntranceListByType(EntranceOverride* entranceList, u8 byDest) {
    EntranceOverride tempList[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };

    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        tempList[i] = entranceList[i];
    }

    size_t idx = 0;

    for (size_t k = 0; k < ENTRANCE_TYPE_COUNT; k++) {
        for (size_t i = 0; i < ARRAY_COUNT(entranceData); i++) {
            for (size_t j = 0; j < ENTRANCE_OVERRIDES_MAX_COUNT; j++) {
                if (Entrance_EntranceIsNull(&tempList[j])) {
                    break;
                }

                size_t entranceIndex = byDest ? tempList[j].override : tempList[j].index;

                if (entranceData[i].type == k && entranceIndex == entranceData[i].index) {
                    entranceList[idx] = tempList[j];
                    idx++;
                    break;
                }
            }
        }
    }
}

void SortEntranceListByArea(EntranceOverride* entranceList, u8 byDest) {
    EntranceOverride tempList[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };

    // Store to temp
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        tempList[i] = entranceList[i];
        // Don't include one-way indexes in the tempList if we're sorting by destination
        // so that we keep them at the beginning.
        if (byDest) {
            if (GetEntranceData(tempList[i].index)->srcGroup == ENTRANCE_GROUP_ONE_WAY) {
                tempList[i] = emptyOverride;
            }
        }
    }

    size_t idx = 0;
    // Sort Source List based on entranceData order
    if (!byDest) {
        for (size_t i = 0; i < ARRAY_COUNT(entranceData); i++) {
            for (size_t j = 0; j < ENTRANCE_OVERRIDES_MAX_COUNT; j++) {
                if (Entrance_EntranceIsNull(&tempList[j])) {
                    break;
                }
                if (tempList[j].index == entranceData[i].index) {
                    entranceList[idx] = tempList[j];
                    idx++;
                    break;
                }
            }
        }

    } else {
        // Increment the idx by however many one-way entrances are shuffled since these
        // will still be displayed at the beginning
        idx += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_AREA][ENTRANCE_GROUP_ONE_WAY];

        // Sort the rest of the Destination List by matching destination strings with source strings when possible
        // and otherwise by group
        for (size_t group = ENTRANCE_GROUP_KOKIRI_FOREST; group < SPOILER_ENTRANCE_GROUP_COUNT; group++) {
            for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
                if (Entrance_EntranceIsNull(&gSaveContext.entranceOverrides[i])) {
                    continue;
                }
                const EntranceData* curEntrance = GetEntranceData(gSaveContext.entranceOverrides[i].index);
                if (curEntrance->srcGroup != group) {
                    continue;
                }
                // First, search the list for the matching reverse entrance if it exists
                for (size_t j = 0; j < ENTRANCE_OVERRIDES_MAX_COUNT; j++) {
                    const EntranceData* curOverride = GetEntranceData(tempList[j].override);
                    if (Entrance_EntranceIsNull(&tempList[j]) || curOverride->dstGroup != group) {
                        continue;
                    }

                    if (curEntrance->reverseIndex == curOverride->index) {
                        entranceList[idx] = tempList[j];
                        // "Remove" this entrance from the tempList by setting it's values to zero
                        tempList[j] = emptyOverride;
                        idx++;
                        break;
                    }
                }
            }
            // Then find any remaining entrances in the same group and add them to the end
            for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
                if (Entrance_EntranceIsNull(&tempList[i])) {
                    continue;
                }
                const EntranceData* curOverride = GetEntranceData(tempList[i].override);
                if (curOverride->dstGroup == group) {
                    entranceList[idx] = tempList[i];
                    tempList[i] = emptyOverride;
                    idx++;
                }
            }
        }
    }
}

void SetCurrentGrottoIDForTracker(s16 entranceIndex) {
    currentGrottoId = entranceIndex;
}

void SetLastEntranceOverrideForTracker(s16 entranceIndex) {
    lastEntranceIndex = entranceIndex;
}

void ClearEntranceTrackingData() {
    currentGrottoId = -1;
    lastEntranceIndex = -1;
    lastSceneOrEntranceDetected = -1;
    gEntranceTrackingData = {0};
}

void InitEntranceTrackingData() {
    gEntranceTrackingData = {0};

    // Check if entrance randomization is disabled
    if (!OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        return;
    }

    // Set total and group counts
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        if (Entrance_EntranceIsNull(&gSaveContext.entranceOverrides[i])) {
            break;
        }
        const EntranceData* index = GetEntranceData(gSaveContext.entranceOverrides[i].index);
        const EntranceData* override = GetEntranceData(gSaveContext.entranceOverrides[i].override);

        if (index->srcGroup == ENTRANCE_GROUP_ONE_WAY) {
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_AREA][ENTRANCE_GROUP_ONE_WAY]++;
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_AREA][ENTRANCE_GROUP_ONE_WAY]++;
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_TYPE][ENTRANCE_TYPE_ONE_WAY]++;
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_TYPE][ENTRANCE_TYPE_ONE_WAY]++;
        } else {
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_AREA][index->srcGroup]++;
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_AREA][override->dstGroup]++;
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_TYPE][index->type]++;
            gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_TYPE][override->type]++;
        }
        gEntranceTrackingData.EntranceCount++;
    }

    // The entrance data is sorted and grouped in a one dimensional array, so we need to track offsets
    // Set offsets for areas starting at 0
    u16 srcOffsetTotal = 0;
    u16 dstOffsetTotal = 0;
    for (size_t i = 0; i < SPOILER_ENTRANCE_GROUP_COUNT; i++) {
        // Set the offset for the current group
        gEntranceTrackingData.GroupOffsets[ENTRANCE_SOURCE_AREA][i] = srcOffsetTotal;
        gEntranceTrackingData.GroupOffsets[ENTRANCE_DESTINATION_AREA][i] = dstOffsetTotal;
        // Increment the offset by the areas entrance count
        srcOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_AREA][i];
        dstOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_AREA][i];
    }
    // Set offsets for types starting at 0
    srcOffsetTotal = 0;
    dstOffsetTotal = 0;
    for (size_t i = 0; i < ENTRANCE_TYPE_COUNT; i++) {
        // Set the offset for the current group
        gEntranceTrackingData.GroupOffsets[ENTRANCE_SOURCE_TYPE][i] = srcOffsetTotal;
        gEntranceTrackingData.GroupOffsets[ENTRANCE_DESTINATION_TYPE][i] = dstOffsetTotal;
        // Increment the offset by the areas entrance count
        srcOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_TYPE][i];
        dstOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_TYPE][i];
    }

    // Sort entrances by group and type in entranceData
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        srcListSortedByArea[i] = gSaveContext.entranceOverrides[i];
        destListSortedByArea[i] = gSaveContext.entranceOverrides[i];
        srcListSortedByType[i] = gSaveContext.entranceOverrides[i];
        destListSortedByType[i] = gSaveContext.entranceOverrides[i];
    }
    SortEntranceListByArea(srcListSortedByArea, 0);
    SortEntranceListByArea(destListSortedByArea, 1);
    SortEntranceListByType(srcListSortedByType, 0);
    SortEntranceListByType(destListSortedByType, 1);
}

void DrawEntranceTracker(bool& open) {
    if (!open) {
        CVarSetInteger("gEntranceTrackerEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600, 375), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Entrance Tracker", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    // Begin tracker settings
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Tracker Settings")) {
        // Reduce indentation from the tree node for the table
        ImGui::SetCursorPosX((ImGui::GetCursorPosX() / 2) + 4.0f);

        if (ImGui::BeginTable("entranceTrackerSettings", 1, ImGuiTableFlags_BordersInnerH)) {

            ImGui::TableNextColumn();

            UIWidgets::Spacer(0);
            ImGui::TextWrapped("The entrance tracker will only track shuffled entrances");
            UIWidgets::Spacer(0);

            ImGui::TableNextColumn();

            if (ImGui::BeginTable("entranceTrackerSubSettings", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {

                ImGui::TableNextColumn();

                ImGui::Text("Sort By");
                UIWidgets::EnhancementRadioButton("To", "gEntranceTrackerSortBy", 0);
                UIWidgets::Tooltip("Sort entrances by the original source entrance");
                UIWidgets::EnhancementRadioButton("From", "gEntranceTrackerSortBy", 1);
                UIWidgets::Tooltip("Sort entrances by the overrided destination");

                UIWidgets::Spacer(2.0f);

                ImGui::Text("List Items");
                UIWidgets::PaddedEnhancementCheckbox("Auto scroll", "gEntranceTrackerAutoScroll", true, false);
                UIWidgets::Tooltip("Automatically scroll to the first aviable entrance in the current scene");
                UIWidgets::PaddedEnhancementCheckbox("Highlight previous", "gEntranceTrackerHighlightPrevious", true, false);
                UIWidgets::Tooltip("Highlight the previous entrance that Link came from");
                UIWidgets::PaddedEnhancementCheckbox("Highlight available", "gEntranceTrackerHighlightAvailable", true, false);
                UIWidgets::Tooltip("Highlight available entrances in the current scene");
                UIWidgets::PaddedEnhancementCheckbox("Hide undiscovered", "gEntranceTrackerCollapseUndiscovered", true, false);
                UIWidgets::Tooltip("Collapse undiscovered entrances towards the bottom of each group");
                bool disableHideReverseEntrances = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) == RO_GENERIC_ON;
                static const char* disableHideReverseEntrancesText = "This option is disabled because \"Decouple Entrances\" is enabled.";
                UIWidgets::PaddedEnhancementCheckbox("Hide reverse", "gEntranceTrackerHideReverseEntrances", true, false,
                                              disableHideReverseEntrances, disableHideReverseEntrancesText, UIWidgets::CheckboxGraphics::Cross, true);
                UIWidgets::Tooltip("Hide reverse entrance transitions when Decouple Entrances is off");
                UIWidgets::Spacer(0);

                ImGui::TableNextColumn();

                ImGui::Text("Group By");
                UIWidgets::EnhancementRadioButton("Area", "gEntranceTrackerGroupBy", 0);
                UIWidgets::Tooltip("Group entrances by their area");
                UIWidgets::EnhancementRadioButton("Type", "gEntranceTrackerGroupBy", 1);
                UIWidgets::Tooltip("Group entrances by their entrance type");

                UIWidgets::Spacer(2.0f);

                ImGui::Text("Spoiler Reveal");
                UIWidgets::PaddedEnhancementCheckbox("Show \"To\"", "gEntranceTrackerShowTo", true, false);
                UIWidgets::Tooltip("Reveal the \"To\" entrance for undiscovered entrances");
                UIWidgets::PaddedEnhancementCheckbox("Show \"From\"", "gEntranceTrackerShowFrom", true, false);
                UIWidgets::Tooltip("Reveal the \"From\" entrance for undiscovered entrances");

                ImGui::EndTable();
            }

            ImGui::TableNextColumn();

            ImGui::SetNextItemOpen(false, ImGuiCond_Once);
            if (ImGui::TreeNode("Legend")) {
                ImGui::TextColored(ImColor(COLOR_ORANGE), "Last Entrance");
                ImGui::TextColored(ImColor(COLOR_GREEN), "Available Entrances");
                ImGui::TextColored(ImColor(COLOR_GRAY), "Undiscovered Entrances");
                ImGui::TreePop();
            }

            UIWidgets::Spacer(0);

            ImGui::EndTable();
        }

        ImGui::TreePop();
    } else {
        UIWidgets::PaddedSeparator();
    }

    static ImGuiTextFilter locationSearch;

    uint8_t nextTreeState = 0;
    if (ImGui::Button("Collapse All")) {
        nextTreeState = 1;
    }
    UIWidgets::Tooltip("Collapse all entrance groups");
    ImGui::SameLine();
    if (ImGui::Button("Expand All")) {
        nextTreeState = 2;
    }
    UIWidgets::Tooltip("Expand all entrance groups");
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        locationSearch.Clear();
    }
    UIWidgets::Tooltip("Clear the search field");

    if (locationSearch.Draw()) {
        nextTreeState = 2;
    }

    uint8_t destToggle = CVarGetInteger("gEntranceTrackerSortBy", 0);
    uint8_t groupToggle = CVarGetInteger("gEntranceTrackerGroupBy", 0);

    // Combine destToggle and groupToggle to get a range of 0-3
    uint8_t groupType = destToggle + (groupToggle * 2);
    size_t groupCount = groupToggle ? ENTRANCE_TYPE_COUNT : SPOILER_ENTRANCE_GROUP_COUNT;
    auto groupNames = groupToggle ? groupTypeNames : spoilerEntranceGroupNames;

    EntranceOverride *entranceList;

    switch (groupType) {
        case ENTRANCE_SOURCE_AREA:
            entranceList = srcListSortedByArea;
            break;
        case ENTRANCE_DESTINATION_AREA:
            entranceList = destListSortedByArea;
            break;
        case ENTRANCE_SOURCE_TYPE:
            entranceList = srcListSortedByType;
            break;
        case ENTRANCE_DESTINATION_TYPE:
            entranceList = destListSortedByType;
            break;
    }

    // Begin tracker list
    ImGui::BeginChild("ChildEntranceTrackerLocations", ImVec2(0, -8));
    for (size_t i = 0; i < groupCount; i++) {
        std::string groupName = groupNames[i];

        uint16_t entranceCount = gEntranceTrackingData.GroupEntranceCounts[groupType][i];
        uint16_t startIndex = gEntranceTrackingData.GroupOffsets[groupType][i];

        bool doAreaScroll = false;
        size_t undiscovered = 0;
        std::vector<EntranceOverride> displayEntrances = {};

        // Loop over entrances first for filtering
        for (size_t entranceIdx = 0; entranceIdx < entranceCount; entranceIdx++) {
            size_t trueIdx = entranceIdx + startIndex;

            EntranceOverride entrance = entranceList[trueIdx];

            const EntranceData* original = GetEntranceData(entrance.index);
            const EntranceData* override = GetEntranceData(entrance.override);

            // If entrance is a dungeon, grotto, or interior entrance, the transition into that area has oneExit set, which means we can filter the return transitions as redundant
            // if entrances are not decoupled, as this is redundant information. Also checks a setting, enabled by default, for hiding them.
            // If all of these conditions are met, we skip adding this entrance to any lists.
            // However, if entrances are decoupled, then all transitions need to be displayed, so we proceed with the filtering
            if ((original->type == ENTRANCE_TYPE_DUNGEON || original->type == ENTRANCE_TYPE_GROTTO || original->type == ENTRANCE_TYPE_INTERIOR) &&
                (original->oneExit != 1 && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) == RO_GENERIC_OFF) &&
                CVarGetInteger("gEntranceTrackerHideReverseEntrances", 1) == 1) {
                    continue;
            }

            bool isDiscovered = IsEntranceDiscovered(entrance.index);

            bool showOriginal = (!destToggle ? CVarGetInteger("gEntranceTrackerShowTo", 0) : CVarGetInteger("gEntranceTrackerShowFrom", 0)) || isDiscovered;
            bool showOverride = (!destToggle ? CVarGetInteger("gEntranceTrackerShowFrom", 0) : CVarGetInteger("gEntranceTrackerShowTo", 0)) || isDiscovered;

            const char* origSrcAreaName = spoilerEntranceGroupNames[original->srcGroup].c_str();
            const char* origTypeName = groupTypeNames[original->type].c_str();
            const char* rplcSrcAreaName = spoilerEntranceGroupNames[override->srcGroup].c_str();
            const char* rplcTypeName = groupTypeNames[override->type].c_str();

            const char* origSrcName = showOriginal ? original->source.c_str()      : "";
            const char* origDstName = showOriginal ? original->destination.c_str() : "";
            const char* rplcSrcName = showOverride ? override->source.c_str()      : "";
            const char* rplcDstName = showOverride ? override->destination.c_str() : "";

            // Filter for entrances by group name, type, source/destination names, and meta tags
            if ((!locationSearch.IsActive() && (showOriginal || showOverride || !CVarGetInteger("gEntranceTrackerCollapseUndiscovered", 0))) ||
                ((showOriginal && (locationSearch.PassFilter(origSrcName) ||
                locationSearch.PassFilter(origDstName) || locationSearch.PassFilter(origSrcAreaName) ||
                locationSearch.PassFilter(origTypeName) || locationSearch.PassFilter(original->metaTag.c_str()))) ||
                (showOverride && (locationSearch.PassFilter(rplcSrcName) ||
                locationSearch.PassFilter(rplcDstName) || locationSearch.PassFilter(rplcSrcAreaName) ||
                locationSearch.PassFilter(rplcTypeName) || locationSearch.PassFilter(override->metaTag.c_str()))))) {

                // Detect if a scroll should happen and remember the scene for that scroll
                if (!doAreaScroll && (lastSceneOrEntranceDetected != LinkIsInArea(original) && LinkIsInArea(original) != -1)) {
                    lastSceneOrEntranceDetected = LinkIsInArea(original);
                    doAreaScroll = true;
                }

                displayEntrances.push_back(entrance);
            } else {
                if (!isDiscovered) {
                    undiscovered++;
                }
            }
        }

        // Then display the entrances in groups
        if (displayEntrances.size() != 0 || (!locationSearch.IsActive() && undiscovered > 0)) {
            // Handle opening/closing trees based on auto scroll or collapse/expand buttons
            if (nextTreeState == 1) {
                ImGui::SetNextItemOpen(false, ImGuiCond_None);
            } else {
                ImGui::SetNextItemOpen(true, nextTreeState == 0 && !doAreaScroll ? ImGuiCond_Once : ImGuiCond_None);
            }

            if (ImGui::TreeNode(groupName.c_str())) {
                for (auto entrance : displayEntrances) {
                    const EntranceData* original = GetEntranceData(entrance.index);
                    const EntranceData* override = GetEntranceData(entrance.override);

                    bool isDiscovered = IsEntranceDiscovered(entrance.index);

                    bool showOriginal = (!destToggle ? CVarGetInteger("gEntranceTrackerShowTo", 0) : CVarGetInteger("gEntranceTrackerShowFrom", 0)) || isDiscovered;
                    bool showOverride = (!destToggle ? CVarGetInteger("gEntranceTrackerShowFrom", 0) : CVarGetInteger("gEntranceTrackerShowTo", 0)) || isDiscovered;

                    const char* unknown = "???";

                    const char* origSrcName = showOriginal ? original->source.c_str()      : unknown;
                    const char* origDstName = showOriginal ? original->destination.c_str() : unknown;
                    const char* rplcSrcName = showOverride ? override->source.c_str()      : unknown;
                    const char* rplcDstName = showOverride ? override->destination.c_str() : unknown;

                    uint32_t color = isDiscovered ? IM_COL32_WHITE : COLOR_GRAY;

                    // Handle highlighting and auto scroll
                    if ((original->index == lastEntranceIndex ||
                        (override->reverseIndex == lastEntranceIndex && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) == RO_GENERIC_OFF)) &&
                            CVarGetInteger("gEntranceTrackerHighlightPrevious", 0)) {
                                 color = COLOR_ORANGE;
                    } else if (LinkIsInArea(original) != -1) {
                        if (CVarGetInteger("gEntranceTrackerHighlightAvailable", 0)) {
                            color = COLOR_GREEN;
                        }

                        if (doAreaScroll) {
                            doAreaScroll = false;
                            if (CVarGetInteger("gEntranceTrackerAutoScroll", 0)) {
                                ImGui::SetScrollHereY(0.0f);
                            }
                        }
                    }

                    ImGui::PushStyleColor(ImGuiCol_Text, color);

                    // Use a non-breaking space to keep the arrow from wrapping to a newline by itself
                    auto nbsp = u8"\u00A0";
                    if (original->srcGroup != ENTRANCE_GROUP_ONE_WAY) {
                        ImGui::TextWrapped("%s to %s%s->", origSrcName, origDstName, nbsp);
                    } else {
                        ImGui::TextWrapped("%s%s->", origSrcName, nbsp);
                    }

                    // Indent the destination
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() * 2);
                    if (!showOverride || (showOverride && (!override->oneExit && override->srcGroup != ENTRANCE_GROUP_ONE_WAY))) {
                        ImGui::TextWrapped("%s from %s", rplcDstName, rplcSrcName);
                    } else {
                        ImGui::TextWrapped("%s", rplcDstName);
                    }

                    ImGui::PopStyleColor();
                }

                // Write collapsed undiscovered info
                if (!locationSearch.IsActive() && undiscovered > 0) {
                    UIWidgets::Spacer(0);
                    ImGui::PushStyleColor(ImGuiCol_Text, COLOR_GRAY);
                    ImGui::TextWrapped("%d Undiscovered", undiscovered);
                    ImGui::PopStyleColor();
                }

                UIWidgets::Spacer(0);
                ImGui::TreePop();
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void InitEntranceTracker() {
    Ship::AddWindow("Randomizer", "Entrance Tracker", DrawEntranceTracker,
                    CVarGetInteger("gEntranceTrackerEnabled", 0) == 1);

    // Setup hooks for loading and clearing the entrance tracker data
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        InitEntranceTrackingData();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](int32_t fileNum) {
        ClearEntranceTrackingData();
    });
}

#include "randomizer_entrance_tracker.h"
#include "../../util.h"
#include "../../OTRGlobals.h"
#include <libultraship/ImGuiImpl.h>
#include "../../UIWidgets.hpp"


#include <map>
#include <string>
#include <vector>
#include <libultraship/Cvar.h>
#include <libultraship/Hooks.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;

#include "randomizer_entrance.h"
}

EntranceOverride srcList[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceOverride destList[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceOverride srcTypeList[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceOverride destTypeList[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceTrackingData gEntranceTrackingData = {0};

static const EntranceOverride emptyOverride = {0};

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

const EntranceData entranceData[] = {
    //index,   source name,          destination name,           source group,           destination group,      type,                 oneExit
    { 0x00BB, "Child Spawn",        "Link's House",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x0282, "Adult Spawn",        "Temple of Time",            ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    { 0x0600, "Minuet of Forest",   "SFM Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x04F6, "Bolero of Fire",     "DMC Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x0604, "Serenade of Water",  "Lake Hylia Warp Pad",       ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x01F1, "Requiem of Spirit",  "Desert Colossus Warp Pad",  ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x0568, "Nocturne of Shadow", "Graveyard Warp Pad",        ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x05F4, "Prelude of Light",   "Temple of Time Warp Pad",   ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    { 0x0554, "DMT Owl Flight",     "Kakariko Village Owl Drop", ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { 0x027E, "LH Owl Flight",      "Hyrule Field Owl Drop",     ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    // Kokiri Forest
    { 0x05E0, "KF",                 "Lost Woods Bridge", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x011E, "KF",                 "Lost Woods",        ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0272, "KF",                 "Link's House",      ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0433, "KF",                 "Mido's House",      ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0437, "KF",                 "Saria's House",     ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x009C, "KF",                 "House of Twins",    ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x00C9, "KF",                 "Know-It-All House", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x00C1, "KF",                 "KF Shop",           ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x071B, "KF",                 "KF Storms Grotto",  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO, "chest", 1},
    { 0x0000, "KF",                 "Deku Tree",         ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON, "", 1},
    { 0x0211, "Link's House",       "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, ""},
    { 0x0443, "Mido's House",       "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, ""},
    { 0x0447, "Saria's House",      "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, ""},
    { 0x033C, "House of Twins",     "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, ""},
    { 0x026A, "Know-It-All House",  "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, ""},
    { 0x0266, "KF Shop",            "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, ""},
    { 0x081B, "KF Storms Grotto",   "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO, ""},
    { 0x0209, "Deku Tree",          "KF",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON, ""},

    // Lost Woods
    { 0x020D, "Lost Woods Bridge",        "KF",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0185, "Lost Woods Bridge",        "Hyrule Field",             ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "lw,hf"},
    { 0x0286, "Lost Woods",               "KF",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x04E2, "Lost Woods",               "Goron City",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_GORON_CITY,    ENTRANCE_TYPE_OVERWORLD, "lw,gc"},
    { 0x01DD, "Lost Woods",               "ZR",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x00FC, "Lost Woods",               "SFM",                      ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_SFM,           ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x071A, "Lost Woods",               "LW Near Shortcuts Grotto", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO, "lw,chest", 1},
    { 0x0719, "Lost Woods",               "LW Scrubs Grotto",         ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO, "lw", 1},
    { 0x0720, "Lost Woods",               "Deku Theater",             ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO, "lw", 1},
    { 0x081A, "LW Near Shortcuts Grotto", "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO, "lw"},
    { 0x0819, "LW Scrubs Grotto",         "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO, "lw"},
    { 0x0820, "Deku Theater",             "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO, "lw"},

    // Sacred Forest Meadow
    { 0x01A9, "SFM",               "Lost Woods",        ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0716, "SFM",               "SFM Wolfos Grotto", ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO, "chest", 1},
    { 0x0718, "SFM",               "SFM Fairy Grotto",  ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO, "", 1},
    { 0x0717, "SFM",               "SFM Storms Grotto", ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO, "scrubs", 1},
    { 0x0169, "SFM",               "Forest Temple",     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON, "", 1},
    { 0x0816, "SFM Wolfos Grotto", "SFM",               ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { 0x0818, "SFM Fairy Grotto",  "SFM",               ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { 0x0817, "SFM Storms Grotto", "SFM",               ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { 0x0215, "Forest Temple",     "SFM",               ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON},

    // Kakariko Village
    { 0x017D, "Kakariko",              "Hyrule Field",          ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_HYRULE_FIELD,         ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x00E4, "Kakariko",              "Graveyard",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_GRAVEYARD,            ENTRANCE_TYPE_OVERWORLD},
    { 0x013D, "Kakariko",              "DMT",                   ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_OVERWORLD},
    { 0x02FD, "Kakariko",              "Carpenter Boss House",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0550, "Kakariko",              "House of Skulltula",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x039C, "Kakariko",              "Impa's House Front",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x05C8, "Kakariko",              "Impa's House Back",     ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0453, "Kakariko",              "Windmill",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x003B, "Kakariko",              "Kak Shooting Gallery",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "adult", 1},
    { 0x0072, "Kakariko",              "Granny's Potion Shop",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x00B7, "Kakariko",              "Kak Bazaar",            ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "shop", 1},
    { 0x0384, "Kakariko",              "Kak Potion Shop Front", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x03EC, "Kakariko",              "Kak Potion Shop Back",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x070A, "Kakariko",              "Kak Open Grotto",       ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,   "chest", 1},
    { 0x070B, "Kakariko",              "Kak Redead Grotto",     ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,   "chest", 1},
    { 0x0098, "Kakariko",              "Bottom of the Well",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON,  "botw", 1},
    { 0x0349, "Carpenter Boss House",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x04EE, "House of Skulltula",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x0345, "Impa's House Front",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x05DC, "Impa's House Back",     "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x0351, "Windmill",              "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x0463, "Kak Shooting Gallery",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x034D, "Granny's Potion Shop",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x0201, "Kak Bazaar",            "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR, "shop"},
    { 0x044B, "Kak Potion Shop Front", "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x04FF, "Kak Potion Shop Back",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { 0x080A, "Kak Open Grotto",       "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO},
    { 0x080B, "Kak Redead Grotto",     "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO},
    { 0x02A6, "Bottom of the Well",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON, "botw"},

    // The Graveyard
    { 0x0195, "Graveyard",         "Kakariko",          ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_KAKARIKO,  ENTRANCE_TYPE_OVERWORLD},
    { 0x030D, "Graveyard",         "Dampe's Shack",     ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x004B, "Graveyard",         "Shield Grave",      ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x031C, "Graveyard",         "Heart Piece Grave", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x002D, "Graveyard",         "Composer's Grave",  ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x044F, "Graveyard",         "Dampe's Grave",     ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "race", 1},
    { 0x0037, "Graveyard",         "Shadow Temple",     ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { 0x0355, "Dampe's Shack",     "Graveyard",         ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR},
    { 0x035D, "Shield Grave",      "Graveyard",         ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { 0x0361, "Heart Piece Grave", "Graveyard",         ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { 0x050B, "Composer's Grave",  "Graveyard",         ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { 0x0359, "Dampe's Grave",     "Graveyard",         ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO, "race"},
    { 0x0205, "Shadow Temple",     "Graveyard",         ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON},

    // Death Mountain Trail
    { 0x0191, "DMT",                      "Kakariko",                 ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_KAKARIKO,              ENTRANCE_TYPE_OVERWORLD},
    { 0x014D, "DMT",                      "Goron City",               ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x0147, "DMT",                      "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD},
    { 0x0315, "DMT",                      "DMT Great Fairy Fountain", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0708, "DMT",                      "DMT Storms Grotto",        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,   "chest", 1},
    { 0x0709, "DMT",                      "DMT Cow Grotto",           ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x0004, "DMT",                      "Dodongo's Cavern",         ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,  "dc", 1},
    { 0x045B, "DMT Great Fairy Fountain", "DMT",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR},
    { 0x0808, "DMT Storms Grotto",        "DMT",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO},
    { 0x0809, "DMT Cow Grotto",           "DMT",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO},
    { 0x0242, "Dodongo's Cavern",         "DMT",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON, "dc"},

    // Death Mountain Crater
    { 0x01C1, "DMC",                      "Goron City",               ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x01BD, "DMC",                      "DMT",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD},
    { 0x04BE, "DMC",                      "DMC Great Fairy Fountain", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0706, "DMC",                      "DMC Upper Grotto",         ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,   "chest", 1},
    { 0x0705, "DMC",                      "DMC Hammer Grotto",        ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,   "scrubs", 1},
    { 0x0165, "DMC",                      "Fire Temple",              ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { 0x0482, "DMC Great Fairy Fountain", "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR},
    { 0x0806, "DMC Upper Grotto",         "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO},
    { 0x0805, "DMC Hammer Grotto",        "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO},
    { 0x024A, "Fire Temple",              "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON},

    // Goron City
    { 0x01B9, "Goron City",        "DMT",               ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x0246, "Goron City",        "DMC",               ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD, "gc"},
    { 0x04D6, "Goron City",        "Lost Woods",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_LOST_WOODS,            ENTRANCE_TYPE_OVERWORLD, "gc,lw"},
    { 0x037C, "Goron City",        "Goron Shop",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR, "gc", 1},
    { 0x0707, "Goron City",        "Goron City Grotto", ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO,   "gc,scrubs", 1},
    { 0x03FC, "Goron Shop",        "Goron City",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR, "gc"},
    { 0x0807, "Goron City Grotto", "Goron City",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO, "gc"},

    // Zora's River
    { 0x0181, "ZR",               "Hyrule Field",     ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x04DA, "ZR",               "Lost Woods",       ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_LOST_WOODS,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { 0x0108, "ZR",               "Zora's Domain",    ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD},
    { 0x0702, "ZR",               "ZR Storms Grotto", ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO, "scrubs", 1},
    { 0x0703, "ZR",               "ZR Fairy Grotto",  ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO, "", 1},
    { 0x0704, "ZR",               "ZR Open Grotto",   ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO, "chest", 1},
    { 0x0802, "ZR Storms Grotto", "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO},
    { 0x0803, "ZR Fairy Grotto",  "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO},
    { 0x0804, "ZR Open Grotto",   "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO},

    // Zora's Domain
    { 0x019D, "Zora's Domain",    "ZR",               ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_RIVER,    ENTRANCE_TYPE_OVERWORLD},
    { 0x0560, "Zora's Domain",    "Lake Hylia",       ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_LAKE_HYLIA,     ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x0225, "Zora's Domain",    "ZF",               ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_OVERWORLD},
    { 0x0380, "Zora's Domain",    "Zora Shop",        ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x071C, "Zora's Domain",    "ZD Storms Grotto", ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x03C4, "Zora Shop",        "Zora's Domain",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR},
    { 0x081C, "ZD Storms Grotto", "Zora's Domain",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO},

    // Zora's Fountain
    { 0x01A1, "ZF",                      "Zora's Domain",           ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_OVERWORLD},
    { 0x0371, "ZF",                      "ZF Great Fairy Fountain", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0028, "ZF",                      "Jabu Jabu's Belly",       ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { 0x0088, "ZF",                      "Ice Cavern",              ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { 0x0394, "ZF Great Fairy Fountain", "ZF",                      ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR},
    { 0x0221, "Jabu Jabu's Belly",       "ZF",                      ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},
    { 0x03D4, "Ice Cavern",              "ZF",                      ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},

    // Hyrule Field
    { 0x04DE, "Hyrule Field",           "Lost Woods Bridge",      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "hf,lw"},
    { 0x0276, "Hyrule Field",           "Market Entrance",        ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0157, "Hyrule Field",           "Lon Lon Ranch",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_OVERWORLD, "hf,llr"},
    { 0x00DB, "Hyrule Field",           "Kakariko",               ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_KAKARIKO,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x00EA, "Hyrule Field",           "ZR",                     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0102, "Hyrule Field",           "Lake Hylia",             ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LAKE_HYLIA,    ENTRANCE_TYPE_OVERWORLD, "hf,lh"},
    { 0x0117, "Hyrule Field",           "GV",                     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0710, "Hyrule Field",           "HF Near Market Grotto",  ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "chest", 1},
    { 0x070E, "Hyrule Field",           "HF Near Kak Grotto",     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "spider", 1},
    { 0x070D, "Hyrule Field",           "HF Tektite Grotto",      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "water", 1},
    { 0x070F, "Hyrule Field",           "HF Fairy Grotto",        ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "", 1},
    { 0x0711, "Hyrule Field",           "HF Cow Grotto",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "", 1},
    { 0x0713, "Hyrule Field",           "HF Open Grotto",         ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "chest", 1},
    { 0x0712, "Hyrule Field",           "HF Inside Fence Grotto", ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "scrubs", 1},
    { 0x0714, "Hyrule Field",           "HF Southeast Grotto",    ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "chest", 1},
    { 0x0810, "HF Near Market Grotto",  "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { 0x080E, "HF Near Kak Grotto",     "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "spider"},
    { 0x080D, "HF Tektite Grotto",      "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO, "water"},
    { 0x080F, "HF Fairy Grotto",        "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { 0x0811, "HF Cow Grotto",          "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { 0x0813, "HF Open Grotto",         "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { 0x0812, "HF Inside Fence Grotto", "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { 0x0814, "HF Southeast Grotto",    "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},

    // Lon Lon Ranch
    { 0x01F9, "Lon Lon Ranch", "Hyrule Field",  ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x004F, "Lon Lon Ranch", "Talon's House", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR, "llr", 1},
    { 0x02F9, "Lon Lon Ranch", "LLR Stables",   ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x05D0, "Lon Lon Ranch", "LLR Tower",     ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0715, "Lon Lon Ranch", "LLR Grotto",    ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO,   "scrubs", 1},
    { 0x0378, "Talon's House", "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR, "llr"},
    { 0x042F, "LLR Stables",   "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR},
    { 0x05D4, "LLR Tower",     "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR},
    { 0x0815, "LLR Grotto",    "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO},

    // Lake Hylia
    { 0x0189, "Lake Hylia",   "Hyrule Field",  ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x0328, "Lake Hylia",   "Zora's Domain", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x0043, "Lake Hylia",   "LH Lab",        ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR, "lh", 1},
    { 0x045F, "Lake Hylia",   "Fishing Hole",  ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR, "lh", 1},
    { 0x0701, "Lake Hylia",   "LH Grotto",     ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO,   "scrubs", 1},
    { 0x0010, "Lake Hylia",   "Water Temple",  ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,  "lh", 1},
    { 0x03CC, "LH Lab",       "Lake Hylia",    ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR, "lh"},
    { 0x0309, "Fishing Hole", "Lake Hylia",    ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR, "lh"},
    { 0x0801, "LH Grotto",    "Lake Hylia",    ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO, "lh"},
    { 0x021D, "Water Temple", "Lake Hylia",    ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON, "lh"},

    // Gerudo Area
    { 0x018D, "GV",                      "Hyrule Field",            ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_HYRULE_FIELD,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x0129, "GV",                      "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { 0x0219, "GV",                      "Lake Hylia",              ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_LAKE_HYLIA,        ENTRANCE_TYPE_OVERWORLD, "lh"},
    { 0x03A0, "GV",                      "Carpenters' Tent",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x071F, "GV",                      "GV Octorok Grotto",       ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x071E, "GV",                      "GV Storms Grotto",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,   "scrubs", 1},
    { 0x022D, "GF",                      "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { 0x0130, "GF",                      "Haunted Wasteland",       ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { 0x071D, "GF",                      "GF Storms Grotto",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,   "gerudo fortress", 1},
    { 0x0008, "GF",                      "Gerudo Training Grounds", ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_DUNGEON,  "gerudo fortress,gtg", 1},
    { 0x03D0, "Carpenters' Tent",        "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR},
    { 0x081F, "GV Octorok Grotto",       "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO},
    { 0x081E, "GV Storms Grotto",        "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO},
    { 0x081D, "GF Storms Grotto",        "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO, "gerudo fortress"},
    { 0x03A8, "Gerudo Training Grounds", "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_DUNGEON, "gerudo fortress,gtg"},

    // The Wasteland
    { 0x03AC, "Haunted Wasteland",             "GF",                            ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "hw,gerudo fortress"},
    { 0x0123, "Haunted Wasteland",             "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { 0x0365, "Desert Colossus",               "Haunted Wasteland",             ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { 0x0588, "Colossus",                      "Colossus Great Fairy Fountain", ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR, "dc", 1},
    { 0x0700, "Desert Colossus",               "Colossus Grotto",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO,   "dc,scrubs", 1},
    { 0x0082, "Desert Colossus",               "Spirit Temple",                 ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,  "dc", 1},
    { 0x057C, "Colossus Great Fairy Fountain", "Colossus",                      ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR, "dc"},
    { 0x0800, "Colossus Grotto",               "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO, "dc"},
    { 0x01E1, "Spirit Temple",                 "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON, "dc"},

    // Market
    { 0x01FD, "Market Entrance",        "Hyrule Field",           ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { 0x00B1, "Market Entrance",        "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { 0x007E, "Market Entrance",        "Guard House",            ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0033, "Market",                 "Market Entrance",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { 0x0138, "Market",                 "HC Grounds / OGC",       ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { 0x0171, "Market",                 "Outside Temple of Time", ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { 0x016D, "Market",                 "MK Shooting Gallery",    ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "child", 1},
    { 0x0507, "Market",                 "Bombchu Bowling",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0063, "Market",                 "Treasure Chest Game",    ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x043B, "Market",                 "Man-in-Green's House",   ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0530, "Market",                 "Mask Shop",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x052C, "Market",                 "MK Bazaar",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "shop", 1},
    { 0x0388, "Market",                 "MK Potion Shop",         ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x0528, "Market",                 "Bombchu Shop",           ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x026E, "Guard House",            "Market Entrance",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x01CD, "MK Shooting Gallery",    "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x03BC, "Bombchu Bowling",        "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x01D5, "Treasure Chest Game",    "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x0067, "Man-in-Green's House",   "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x01D1, "Mask Shop",              "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x03B8, "MK Bazaar",              "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "shop"},
    { 0x02A2, "MK Potion Shop",         "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x03C0, "Bombchu Shop",           "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { 0x025E, "Outside Temple of Time", "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "tot"},
    { 0x0053, "Outside Temple of Time", "Temple of Time",         ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "tot", 1},
    { 0x0472, "Temple of Time",         "Outside Temple of Time", ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR, "tot"},

    // Hyrule Castle
    { 0x025A, "HC Grounds / OGC",         "Market",                   ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { 0x0578, "HC Grounds",               "HC Great Fairy Fountain",  ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, "", 1},
    { 0x070C, "HC Grounds",               "HC Storms Grotto",         ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO,   "", 1},
    { 0x0340, "HC Great Fairy Fountain",  "HC Grounds",               ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR},
    { 0x080C, "HC Storms Grotto",         "HC Grounds",               ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO},
    { 0x04C2, "OGC",                      "OGC Great Fairy Fountain", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, "outside ganon's castle", 1},
    { 0x0467, "OGC",                      "Ganon's Castle",           ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,  "outside ganon's castle,gc", 1},
    { 0x03E8, "OGC Great Fairy Fountain", "OGC",                      ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, "outside ganon's castle"},
    { 0x023D, "Ganon's Castle",           "OGC",                      ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON, "outside ganon's castle,gc"}
};

const EntranceData* GetEntranceData(s16 index) {
    for (size_t i = 0; i < ARRAY_COUNT(entranceData); i++) {
        if (index == entranceData[i].index) {
            return &entranceData[i];
        }
    }
    // Shouldn't be reached
    return 0;
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

void SortEntranceList(EntranceOverride* entranceList, u8 byDest) {
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

                    if (strcmp(curEntrance->source.c_str(), curOverride->destination.c_str()) == 0 &&
                        strcmp(curEntrance->destination.c_str(), curOverride->source.c_str()) == 0) {
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

void ClearEntranceTrackingData() {
    gEntranceTrackingData = {0};
}

void InitEntranceTrackingData() {
    gEntranceTrackingData = {0};

    // Check if entrance randomization is disabled
    if (Entrance_EntranceIsNull(&gSaveContext.entranceOverrides[0])) {
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

    // Set offset
    u16 srcOffsetTotal = 0;
    u16 dstOffsetTotal = 0;
    for (size_t i = 0; i < SPOILER_ENTRANCE_GROUP_COUNT; i++) {
        gEntranceTrackingData.GroupOffsets[ENTRANCE_SOURCE_AREA][i] = srcOffsetTotal;
        srcOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_AREA][i];
        gEntranceTrackingData.GroupOffsets[ENTRANCE_DESTINATION_AREA][i] = dstOffsetTotal;
        dstOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_AREA][i];
    }
    srcOffsetTotal = 0;
    dstOffsetTotal = 0;
    for (size_t i = 0; i < ENTRANCE_TYPE_COUNT; i++) {
        gEntranceTrackingData.GroupOffsets[ENTRANCE_SOURCE_TYPE][i] = srcOffsetTotal;
        srcOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_TYPE][i];
        gEntranceTrackingData.GroupOffsets[ENTRANCE_DESTINATION_TYPE][i] = dstOffsetTotal;
        dstOffsetTotal += gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_DESTINATION_TYPE][i];
    }

    // Sort entrances by group and type in entranceData
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        srcList[i] = gSaveContext.entranceOverrides[i];
        destList[i] = gSaveContext.entranceOverrides[i];
        srcTypeList[i] = gSaveContext.entranceOverrides[i];
        destTypeList[i] = gSaveContext.entranceOverrides[i];
    }
    SortEntranceList(srcList, 0);
    SortEntranceList(destList, 1);
    SortEntranceListByType(srcTypeList, 0);
    SortEntranceListByType(destTypeList, 1);
}

void DrawEntranceTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gEntranceTrackerEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600,375), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Entrance Tracker", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Tracker Settings")) {
        ImGui::TextWrapped("The entrance tracker will only track shuffled entrances");
        UIWidgets::PaddedSeparator();

        if (ImGui::BeginTable("entranceTrackerSettings", 3, ImGuiTableFlags_BordersInnerV)) {

            ImGui::TableNextColumn();

            ImGui::Text("Sort By");
            UIWidgets::EnhancementRadioButton("To", "gEntranceTrackerSortBy", 0);
            UIWidgets::Tooltip("Sort entrances by the original source entrance");
            UIWidgets::EnhancementRadioButton("From", "gEntranceTrackerSortBy", 1);
            UIWidgets::Tooltip("Sort entrances by the overrided destination");

            ImGui::TableNextColumn();

            ImGui::Text("Group By");
            UIWidgets::EnhancementRadioButton("Area", "gEntranceTrackerGroupBy", 0);
            UIWidgets::Tooltip("Group entrances by their area");
            UIWidgets::EnhancementRadioButton("Type", "gEntranceTrackerGroupBy", 1);
            UIWidgets::Tooltip("Group entrances by their entrance type");

            ImGui::TableNextColumn();

            ImGui::Text("Spoiler Reveal");
            UIWidgets::EnhancementCheckbox("Show \"To\"", "gEntranceTrackerShowTo");
            UIWidgets::Tooltip("Reveal the \"To\" entrance for undiscovered entrances");
            UIWidgets::EnhancementCheckbox("Show \"From\"", "gEntranceTrackerShowFrom");
            UIWidgets::Tooltip("Reveal the \"From\" entrance for undiscovered entrances");

            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    UIWidgets::PaddedSeparator();

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

    static ImGuiTextFilter locationSearch;
    if (locationSearch.Draw()) {
        nextTreeState = 2;
    }

    uint8_t destToggle = CVar_GetS32("gEntranceTrackerSortBy", 0);
    uint8_t groupToggle = CVar_GetS32("gEntranceTrackerGroupBy", 0);

    uint8_t groupType = destToggle + (groupToggle * 2);
    size_t groupCount = groupToggle ? ENTRANCE_TYPE_COUNT : SPOILER_ENTRANCE_GROUP_COUNT;
    auto groupNames = groupToggle ? groupTypeNames : spoilerEntranceGroupNames;

    EntranceOverride *entranceList;

    switch (groupType) {
        case ENTRANCE_SOURCE_AREA:
            entranceList = srcList;
            break;
        case ENTRANCE_DESTINATION_AREA:
            entranceList = destList;
            break;
        case ENTRANCE_SOURCE_TYPE:
            entranceList = srcTypeList;
            break;
        case ENTRANCE_DESTINATION_TYPE:
            entranceList = destTypeList;
            break;
    }

    ImGui::BeginChild("ChildEntranceLocations", ImVec2(0, -8));
    for (size_t i = 0; i < groupCount; i++) {
        std::string groupName = groupNames[i];
        bool hasItems = false;

        uint16_t entranceCount = gEntranceTrackingData.GroupEntranceCounts[groupType][i];
        uint16_t startIndex = gEntranceTrackingData.GroupOffsets[groupType][i];

        for (size_t entranceIdx = 0; entranceIdx < entranceCount; entranceIdx++) {
            size_t trueIdx = entranceIdx + startIndex;

            EntranceOverride entrance = entranceList[trueIdx];

            const EntranceData* original = GetEntranceData(entrance.index);
            const EntranceData* override = GetEntranceData(entrance.override);

            bool isDiscovered = entrance.discovered;

            bool showOriginal = (!destToggle ? CVar_GetS32("gEntranceTrackerShowTo", 0) : CVar_GetS32("gEntranceTrackerShowFrom", 0)) || isDiscovered;
            bool showOverride = (!destToggle ? CVar_GetS32("gEntranceTrackerShowFrom", 0) : CVar_GetS32("gEntranceTrackerShowTo", 0)) || isDiscovered;

            const char* origSrcAreaName = spoilerEntranceGroupNames[original->srcGroup].c_str();
            const char* origTypeName = groupTypeNames[original->type].c_str();
            const char* rplcSrcAreaName = spoilerEntranceGroupNames[override->srcGroup].c_str();
            const char* rplcTypeName = groupTypeNames[override->type].c_str();

            const char* origSrcName = showOriginal ? original->source.c_str()      : "";
            const char* origDstName = showOriginal ? original->destination.c_str() : "";
            const char* rplcSrcName = showOverride ? override->source.c_str()      : "";
            const char* rplcDstName = showOverride ? override->destination.c_str() : "";

            if (locationSearch.PassFilter("") || (showOriginal && (locationSearch.PassFilter(origSrcName) ||
                locationSearch.PassFilter(origDstName) || locationSearch.PassFilter(origSrcAreaName) ||
                locationSearch.PassFilter(origTypeName) || locationSearch.PassFilter(original->metaTag.c_str()))) ||
                (showOverride && (locationSearch.PassFilter(rplcSrcName) ||
                locationSearch.PassFilter(rplcDstName) || locationSearch.PassFilter(rplcSrcAreaName) ||
                locationSearch.PassFilter(rplcTypeName) || locationSearch.PassFilter(override->metaTag.c_str())))) {

                hasItems = true;
                break;
            }
        }

        if (hasItems) {
            if (nextTreeState == 1) {
                ImGui::SetNextItemOpen(false, ImGuiCond_None);
            } else {
                ImGui::SetNextItemOpen(true, nextTreeState == 0 ? ImGuiCond_Once: ImGuiCond_None);
            }
            if (ImGui::TreeNode(groupName.c_str())) {
                for (size_t entranceIdx = 0; entranceIdx < entranceCount; entranceIdx++) {
                    size_t trueIdx = entranceIdx + startIndex;

                    EntranceOverride entrance = entranceList[trueIdx];

                    const EntranceData* original = GetEntranceData(entrance.index);
                    const EntranceData* override = GetEntranceData(entrance.override);

                    bool isDiscovered = entrance.discovered;

                    bool showOriginal = (!destToggle ? CVar_GetS32("gEntranceTrackerShowTo", 0) : CVar_GetS32("gEntranceTrackerShowFrom", 0)) || isDiscovered;
                    bool showOverride = (!destToggle ? CVar_GetS32("gEntranceTrackerShowFrom", 0) : CVar_GetS32("gEntranceTrackerShowTo", 0)) || isDiscovered;

                    const char* origSrcAreaName = spoilerEntranceGroupNames[original->srcGroup].c_str();
                    const char* origTypeName = groupTypeNames[original->type].c_str();
                    const char* rplcSrcAreaName = spoilerEntranceGroupNames[override->srcGroup].c_str();
                    const char* rplcTypeName = groupTypeNames[override->type].c_str();

                    const char* unknown = "???";

                    const char* origSrcName = showOriginal ? original->source.c_str()      : unknown;
                    const char* origDstName = showOriginal ? original->destination.c_str() : unknown;
                    const char* rplcSrcName = showOverride ? override->source.c_str()      : unknown;
                    const char* rplcDstName = showOverride ? override->destination.c_str() : unknown;

                    uint32_t color = isDiscovered ? IM_COL32_WHITE : IM_COL32(155, 155, 155, 255);

                    ImGui::PushStyleColor(ImGuiCol_Text, color);

                    if (locationSearch.PassFilter("") || (showOriginal && (locationSearch.PassFilter(origSrcName) ||
                        locationSearch.PassFilter(origDstName) || locationSearch.PassFilter(origSrcAreaName) ||
                        locationSearch.PassFilter(origTypeName) || locationSearch.PassFilter(original->metaTag.c_str()))) ||
                        (showOverride && (locationSearch.PassFilter(rplcSrcName) ||
                        locationSearch.PassFilter(rplcDstName) || locationSearch.PassFilter(rplcSrcAreaName) ||
                        locationSearch.PassFilter(rplcTypeName) || locationSearch.PassFilter(override->metaTag.c_str())))) {

                        // Use a non-breaking space to keep the arrow from wrapping to a newline by itself
                        auto nbsp = u8"\u00A0";
                        if (original->srcGroup != ENTRANCE_GROUP_ONE_WAY) {
                            ImGui::TextWrapped("%s to %s%s->", origSrcName, origDstName, nbsp);
                        } else {
                            ImGui::TextWrapped("%s%s->", origSrcName, nbsp);
                        }

                        // Indent the destination
                        float paddingX = ImGui::GetCursorPosX();
                        ImGui::SetCursorPosX(paddingX * 2);
                        if (!showOverride || (showOverride && (!override->oneExit && override->srcGroup != ENTRANCE_GROUP_ONE_WAY))) {
                            ImGui::TextWrapped("%s from %s", rplcDstName, rplcSrcName);
                        } else {
                            ImGui::TextWrapped("%s", rplcDstName);
                        }
                    }
                    ImGui::PopStyleColor();
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void InitEntranceTracker() {
    SohImGui::AddWindow("Randomizer", "Entrance Tracker", DrawEntranceTracker, CVar_GetS32("gEntranceTrackerEnabled", 0) == 1);
}

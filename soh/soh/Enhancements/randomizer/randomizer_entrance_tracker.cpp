#include "randomizer_entrance_tracker.h"
#include "soh/OTRGlobals.h"
#include "soh/UIWidgets.hpp"

#include <map>
#include <string>
#include <vector>
#include <libultraship/libultraship.h>

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
// ENTR_HYRULE_FIELD_10 and ENTR_POTION_SHOP_KAKARIKO_1 have been repurposed for entrance randomizer
const EntranceData entranceData[] = {
    //index,                reverse, scenes (and spawns),     source name,   destination name, source group,           destination group,      type,                 metaTag, oneExit
    { ENTR_LINKS_HOUSE_0,   -1,      SINGLE_SCENE_INFO(SCENE_LINKS_HOUSE), "Child Spawn", "Link's House",   ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_HYRULE_FIELD_10, -1,      SINGLE_SCENE_INFO(SCENE_TEMPLE_OF_TIME), "Adult Spawn", "Temple of Time", ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    { ENTR_SACRED_FOREST_MEADOW_2,  -1, {{ -1 }}, "Minuet of Forest",   "SFM Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_DEATH_MOUNTAIN_CRATER_4, -1, {{ -1 }}, "Bolero of Fire",     "DMC Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_LAKE_HYLIA_8,            -1, {{ -1 }}, "Serenade of Water",  "Lake Hylia Warp Pad",       ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_DESERT_COLOSSUS_5,       -1, {{ -1 }}, "Requiem of Spirit",  "Desert Colossus Warp Pad",  ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_GRAVEYARD_7,             -1, {{ -1 }}, "Nocturne of Shadow", "Graveyard Warp Pad",        ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_TEMPLE_OF_TIME_7,        -1, {{ -1 }}, "Prelude of Light",   "Temple of Time Warp Pad",   ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    { ENTR_KAKARIKO_VILLAGE_14, -1, SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT Owl Flight", "Kakariko Village Owl Drop", ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_HYRULE_FIELD_9,      -1, SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA), "LH Owl Flight",  "Hyrule Field Owl Drop",     ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    // Kokiri Forest
    { ENTR_LOST_WOODS_9,                                    ENTR_KOKIRI_FOREST_2,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "Lost Woods Bridge",   ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_LOST_WOODS_0,                                    ENTR_KOKIRI_FOREST_6,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "Lost Woods",          ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_LINKS_HOUSE_1,                                   ENTR_KOKIRI_FOREST_3,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "Link's House",        ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_MIDOS_HOUSE_0,                                   ENTR_KOKIRI_FOREST_9,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "Mido's House",        ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_SARIAS_HOUSE_0,                                  ENTR_KOKIRI_FOREST_10,                               SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "Saria's House",       ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_TWINS_HOUSE_0,                                   ENTR_KOKIRI_FOREST_8,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "House of Twins",      ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_KNOW_IT_ALL_BROS_HOUSE_0,                        ENTR_KOKIRI_FOREST_5,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "Know-It-All House",   ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_KOKIRI_SHOP_0,                                   ENTR_KOKIRI_FOREST_4,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "KF Shop",             ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_KF_STORMS_OFFSET),  ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_KF_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "KF Storms Grotto",    ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTR_DEKU_TREE_0,                                     ENTR_KOKIRI_FOREST_1,                                SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF",                  "Deku Tree",           ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_KOKIRI_FOREST_3,                                 ENTR_LINKS_HOUSE_1,                                  SINGLE_SCENE_INFO(SCENE_LINKS_HOUSE),            "Link's House",        "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_9,                                 ENTR_MIDOS_HOUSE_0,                                  SINGLE_SCENE_INFO(SCENE_MIDOS_HOUSE),            "Mido's House",        "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_10,                                ENTR_SARIAS_HOUSE_0,                                 SINGLE_SCENE_INFO(SCENE_SARIAS_HOUSE),           "Saria's House",       "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_8,                                 ENTR_TWINS_HOUSE_0,                                  SINGLE_SCENE_INFO(SCENE_TWINS_HOUSE),            "House of Twins",      "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_5,                                 ENTR_KNOW_IT_ALL_BROS_HOUSE_0,                       SINGLE_SCENE_INFO(SCENE_KNOW_IT_ALL_BROS_HOUSE), "Know-It-All House",   "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_4,                                 ENTR_KOKIRI_SHOP_0,                                  SINGLE_SCENE_INFO(SCENE_KOKIRI_SHOP),            "KF Shop",             "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_KF_STORMS_OFFSET),  ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_KF_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x00 }},                       "KF Storms Grotto",    "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTR_KOKIRI_FOREST_1,                                 ENTR_DEKU_TREE_0,                                    SINGLE_SCENE_INFO(SCENE_DEKU_TREE),              "Deku Tree",           "KF",                  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   ""},
    { ENTR_DEKU_TREE_BOSS_0,                                ENTR_DEKU_TREE_1,                                    SINGLE_SCENE_INFO(SCENE_DEKU_TREE),              "Deku Tree Boss Door", "Gohma",               ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_DEKU_TREE_1,                                     ENTR_DEKU_TREE_BOSS_0,                               SINGLE_SCENE_INFO(SCENE_DEKU_TREE_BOSS),         "Gohma",               "Deku Tree Boss Door", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},

    // Lost Woods
    { ENTR_KOKIRI_FOREST_2,                                        ENTR_LOST_WOODS_9,                                           SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods Bridge",        "KF",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_HYRULE_FIELD_3,                                         ENTR_LOST_WOODS_8,                                           SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods Bridge",        "Hyrule Field",             ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "lw,hf"},
    { ENTR_KOKIRI_FOREST_6,                                        ENTR_LOST_WOODS_0,                                           SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods",               "KF",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_GORON_CITY_3,                                           ENTR_LOST_WOODS_6,                                           SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods",               "Goron City",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_GORON_CITY,    ENTRANCE_TYPE_OVERWORLD, "lw,gc"},
    { ENTR_ZORAS_RIVER_4,                                          ENTR_LOST_WOODS_7,                                           SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods",               "ZR",                       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_SACRED_FOREST_MEADOW_0,                                 ENTR_LOST_WOODS_1,                                           SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods",               "SFM",                      ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_SFM,           ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods",               "LW Near Shortcuts Grotto", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,chest", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LW_SCRUBS_OFFSET),         ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LW_SCRUBS_OFFSET),         SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods",               "LW Scrubs Grotto",         ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LW_DEKU_THEATRE_OFFSET),   ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LW_DEKU_THEATRE_OFFSET),   SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods",               "Deku Theater",             ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,mask,stage", 1},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), {{ SCENE_GROTTOS, 0x00 }},           "LW Near Shortcuts Grotto", "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,chest"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LW_SCRUBS_OFFSET),         ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LW_SCRUBS_OFFSET),         {{ SCENE_GROTTOS, 0x07 }},           "LW Scrubs Grotto",         "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LW_DEKU_THEATRE_OFFSET),   ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LW_DEKU_THEATRE_OFFSET),   {{ SCENE_GROTTOS, 0x0C }},           "Deku Theater",             "Lost Woods",               ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,mask,stage"},

    // Sacred Forest Meadow
    { ENTR_LOST_WOODS_1,                                    ENTR_SACRED_FOREST_MEADOW_0,                          SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM",                     "Lost Woods",              ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_SFM_WOLFOS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_SFM_WOLFOS_OFFSET), SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM",                     "SFM Wolfos Grotto",       ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_SFM_FAIRY_OFFSET),  ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_SFM_FAIRY_OFFSET),  SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM",                     "SFM Fairy Grotto",        ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_SFM_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_SFM_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM",                     "SFM Storms Grotto",       ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_FOREST_TEMPLE_0,                                 ENTR_SACRED_FOREST_MEADOW_1,                          SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM",                     "Forest Temple",           ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_SFM_WOLFOS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_SFM_WOLFOS_OFFSET), {{ SCENE_GROTTOS, 0x08 }},                     "SFM Wolfos Grotto",       "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_SFM_FAIRY_OFFSET),  ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_SFM_FAIRY_OFFSET),  {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},             "SFM Fairy Grotto",        "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_SFM_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_SFM_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x0A }},                     "SFM Storms Grotto",       "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTR_SACRED_FOREST_MEADOW_1,                          ENTR_FOREST_TEMPLE_0,                                 SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),        "Forest Temple",           "SFM",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON},
    { ENTR_FOREST_TEMPLE_BOSS_0,                            ENTR_FOREST_TEMPLE_1,                                 SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),        "Forest Temple Boss Door", "Phantom Ganon",           ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_FOREST_TEMPLE_1,                                 ENTR_FOREST_TEMPLE_BOSS_0,                            SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE_BOSS),   "Phantom Ganon",           "Forest Temple Boss Door", ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON, "", 1},

    // Kakariko Village
    { ENTR_HYRULE_FIELD_1,                                  ENTR_KAKARIKO_VILLAGE_0,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Hyrule Field",          ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_HYRULE_FIELD,         ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_GRAVEYARD_0,                                     ENTR_KAKARIKO_VILLAGE_2,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Graveyard",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_GRAVEYARD,            ENTRANCE_TYPE_OVERWORLD},
    { ENTR_DEATH_MOUNTAIN_TRAIL_0,                          ENTR_KAKARIKO_VILLAGE_1,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "DMT",                   ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_OVERWORLD},
    { ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0,                   ENTR_KAKARIKO_VILLAGE_6,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Carpenter Boss House",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_HOUSE_OF_SKULLTULA_0,                            ENTR_KAKARIKO_VILLAGE_11,                             SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "House of Skulltula",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_IMPAS_HOUSE_0,                                   ENTR_KAKARIKO_VILLAGE_5,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Impa's House Front",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_IMPAS_HOUSE_1,                                   ENTR_KAKARIKO_VILLAGE_15,                             SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Impa's House Back",     ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { ENTR_WINDMILL_AND_DAMPES_GRAVE_1,                     ENTR_KAKARIKO_VILLAGE_8,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Windmill",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_SHOOTING_GALLERY_0,                              ENTR_KAKARIKO_VILLAGE_10,                             SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Kak Shooting Gallery",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "adult", 1},
    { ENTR_POTION_SHOP_GRANNY_0,                            ENTR_KAKARIKO_VILLAGE_7,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Granny's Potion Shop",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BAZAAR_0,                                        ENTR_KAKARIKO_VILLAGE_3,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Kak Bazaar",            ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "shop", 1},
    { ENTR_POTION_SHOP_KAKARIKO_0,                          ENTR_KAKARIKO_VILLAGE_9,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Kak Potion Shop Front", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_POTION_SHOP_KAKARIKO_2,                          ENTR_KAKARIKO_VILLAGE_12,                             SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Kak Potion Shop Back",  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_KAK_OPEN_OFFSET),   ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_KAK_OPEN_OFFSET),   SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Kak Open Grotto",       ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_KAK_REDEAD_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_KAK_REDEAD_OFFSET), SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Kak Redead Grotto",     ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTR_BOTTOM_OF_THE_WELL_0,                            ENTR_KAKARIKO_VILLAGE_4,                              SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko",              "Bottom of the Well",    ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON,   "botw", 1},
    { ENTR_KAKARIKO_VILLAGE_6,                              ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0,                   SINGLE_SCENE_INFO(SCENE_KAKARIKO_CENTER_GUEST_HOUSE), "Carpenter Boss House",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_11,                             ENTR_HOUSE_OF_SKULLTULA_0,                            SINGLE_SCENE_INFO(SCENE_HOUSE_OF_SKULLTULA),          "House of Skulltula",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_5,                              ENTR_IMPAS_HOUSE_0,                                   SINGLE_SCENE_INFO(SCENE_IMPAS_HOUSE),                 "Impa's House Front",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_15,                             ENTR_IMPAS_HOUSE_1,                                   SINGLE_SCENE_INFO(SCENE_IMPAS_HOUSE),                 "Impa's House Back",     "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "cow"},
    { ENTR_KAKARIKO_VILLAGE_8,                              ENTR_WINDMILL_AND_DAMPES_GRAVE_1,                     SINGLE_SCENE_INFO(SCENE_WINDMILL_AND_DAMPES_GRAVE),   "Windmill",              "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_10,                             ENTR_SHOOTING_GALLERY_0,                              {{ SCENE_SHOOTING_GALLERY, 0x00 }},                   "Kak Shooting Gallery",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_7,                              ENTR_POTION_SHOP_GRANNY_0,                            SINGLE_SCENE_INFO(SCENE_POTION_SHOP_GRANNY),          "Granny's Potion Shop",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_3,                              ENTR_BAZAAR_0,                                        {{ SCENE_BAZAAR, 0x00 }},                             "Kak Bazaar",            "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "shop"},
    { ENTR_KAKARIKO_VILLAGE_9,                              ENTR_POTION_SHOP_KAKARIKO_0,                          SINGLE_SCENE_INFO(SCENE_POTION_SHOP_KAKARIKO),        "Kak Potion Shop Front", "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_12,                             ENTR_POTION_SHOP_KAKARIKO_2,                          SINGLE_SCENE_INFO(SCENE_POTION_SHOP_KAKARIKO),        "Kak Potion Shop Back",  "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_KAK_OPEN_OFFSET),   ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_KAK_OPEN_OFFSET),   {{ SCENE_GROTTOS, 0x00 }},                            "Kak Open Grotto",       "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_KAK_REDEAD_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_KAK_REDEAD_OFFSET), {{ SCENE_GROTTOS, 0x03 }},                            "Kak Redead Grotto",     "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTR_KAKARIKO_VILLAGE_4,                              ENTR_BOTTOM_OF_THE_WELL_0,                            SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL),          "Bottom of the Well",    "Kakariko",              ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON,   "botw"},

    // The Graveyard
    { ENTR_KAKARIKO_VILLAGE_2,           ENTR_GRAVEYARD_0,                  SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard",               "Kakariko",                ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_KAKARIKO,  ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GRAVEKEEPERS_HUT_0,           ENTR_GRAVEYARD_2,                  SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard",               "Dampe's Shack",           ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR, "", 1},
    { ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0, ENTR_GRAVEYARD_4,                  SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard",               "Shield Grave",            ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { ENTR_REDEAD_GRAVE_0,               ENTR_GRAVEYARD_5,                  SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard",               "Heart Piece Grave",       ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { ENTR_ROYAL_FAMILYS_TOMB_0,         ENTR_GRAVEYARD_6,                  SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard",               "Composer's Grave",        ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { ENTR_WINDMILL_AND_DAMPES_GRAVE_0,  ENTR_GRAVEYARD_3,                  SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard",               "Dampe's Grave",           ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "race", 1},
    { ENTR_SHADOW_TEMPLE_0,              ENTR_GRAVEYARD_1,                  SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard",               "Shadow Temple",           ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { ENTR_GRAVEYARD_2,                  ENTR_GRAVEKEEPERS_HUT_0,           SINGLE_SCENE_INFO(SCENE_GRAVEKEEPERS_HUT),           "Dampe's Shack",           "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR},
    { ENTR_GRAVEYARD_4,                  ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0, SINGLE_SCENE_INFO(SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN), "Shield Grave",            "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { ENTR_GRAVEYARD_5,                  ENTR_REDEAD_GRAVE_0,               SINGLE_SCENE_INFO(SCENE_REDEAD_GRAVE),               "Heart Piece Grave",       "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { ENTR_GRAVEYARD_6,                  ENTR_ROYAL_FAMILYS_TOMB_0,         SINGLE_SCENE_INFO(SCENE_ROYAL_FAMILYS_TOMB),         "Composer's Grave",        "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { ENTR_GRAVEYARD_3,                  ENTR_WINDMILL_AND_DAMPES_GRAVE_0,  SINGLE_SCENE_INFO(SCENE_WINDMILL_AND_DAMPES_GRAVE),  "Dampe's Grave",           "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "race"},
    { ENTR_GRAVEYARD_1,                  ENTR_SHADOW_TEMPLE_0,              SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),              "Shadow Temple",           "Graveyard",               ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON},
    { ENTR_SHADOW_TEMPLE_BOSS_0,         ENTR_SHADOW_TEMPLE_1,              SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),              "Shadow Temple Boss Door", "Bongo-Bongo",             ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_SHADOW_TEMPLE_1,              ENTR_SHADOW_TEMPLE_BOSS_0,         SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE_BOSS),         "Bongo-Bongo",             "Shadow Temple Boss Door", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON, "", 1},

    // Death Mountain Trail
    { ENTR_GORON_CITY_0,                                    ENTR_DEATH_MOUNTAIN_TRAIL_1,                          SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT",                        "Goron City",                 ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_KAKARIKO_VILLAGE_1,                              ENTR_DEATH_MOUNTAIN_TRAIL_0,                          SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT",                        "Kakariko",                   ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_KAKARIKO,              ENTRANCE_TYPE_OVERWORLD},
    { ENTR_DEATH_MOUNTAIN_CRATER_0,                         ENTR_DEATH_MOUNTAIN_TRAIL_2,                          SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT",                        "DMC",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_0,                   ENTR_DEATH_MOUNTAIN_TRAIL_4,                          SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT",                        "DMT Great Fairy Fountain",   ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMT_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMT_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT",                        "DMT Storms Grotto",          ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMT_COW_OFFSET),    ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMT_COW_OFFSET),    SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT",                        "DMT Cow Grotto",             ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTR_DODONGOS_CAVERN_0,                               ENTR_DEATH_MOUNTAIN_TRAIL_3,                          SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT",                        "Dodongo's Cavern",           ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { ENTR_DEATH_MOUNTAIN_TRAIL_4,                          ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_0,                   {{ SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, 0x00 }}, "DMT Great Fairy Fountain",   "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMT_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMT_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x00 }},                     "DMT Storms Grotto",          "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMT_COW_OFFSET),    ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMT_COW_OFFSET),    {{ SCENE_GROTTOS, 0x0D }},                     "DMT Cow Grotto",             "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO},
    { ENTR_DEATH_MOUNTAIN_TRAIL_3,                          ENTR_DODONGOS_CAVERN_0,                               SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),      "Dodongo's Cavern",           "DMT",                        ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc"},
    { ENTR_DODONGOS_CAVERN_BOSS_0,                          ENTR_DODONGOS_CAVERN_1,                               SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),      "Dodongo's Cavern Boss Door", "King Dodongo",               ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { ENTR_DODONGOS_CAVERN_1,                               ENTR_DODONGOS_CAVERN_BOSS_0,                          SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN_BOSS), "King Dodongo",               "Dodongo's Cavern Boss Door", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},

    // Death Mountain Crater
    { ENTR_GORON_CITY_1,                                    ENTR_DEATH_MOUNTAIN_CRATER_1,                         SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC",                      "Goron City",               ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_DEATH_MOUNTAIN_TRAIL_2,                          ENTR_DEATH_MOUNTAIN_CRATER_0,                         SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC",                      "DMT",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_1,                   ENTR_DEATH_MOUNTAIN_CRATER_3,                         SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC",                      "DMC Great Fairy Fountain", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMC_UPPER_OFFSET),  ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMC_UPPER_OFFSET),  SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC",                      "DMC Upper Grotto",         ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMC_HAMMER_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMC_HAMMER_OFFSET), SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC",                      "DMC Hammer Grotto",        ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_FIRE_TEMPLE_0,                                   ENTR_DEATH_MOUNTAIN_CRATER_2,                         SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC",                      "Fire Temple",              ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_DEATH_MOUNTAIN_CRATER_3,                         ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_1,                   {{ SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, 0x01 }},  "DMC Great Fairy Fountain", "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMC_UPPER_OFFSET),  ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMC_UPPER_OFFSET),  {{ SCENE_GROTTOS, 0x00 }},                      "DMC Upper Grotto",         "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_DMC_HAMMER_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_DMC_HAMMER_OFFSET), {{ SCENE_GROTTOS, 0x04 }},                      "DMC Hammer Grotto",        "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTR_DEATH_MOUNTAIN_CRATER_2,                         ENTR_FIRE_TEMPLE_0,                                   SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),           "Fire Temple",              "DMC",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON},
    { ENTR_FIRE_TEMPLE_BOSS_0,                              ENTR_FIRE_TEMPLE_1,                                   SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),           "Fire Temple Boss Door",    "Volvagia",                 ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_FIRE_TEMPLE_1,                                   ENTR_FIRE_TEMPLE_BOSS_0,                              SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE_BOSS),      "Volvagia",                 "Fire Temple Boss Door",    ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},

    // Goron City
    { ENTR_DEATH_MOUNTAIN_TRAIL_1,                          ENTR_GORON_CITY_0,                                    SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City",        "DMT",               ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_DEATH_MOUNTAIN_CRATER_1,                         ENTR_GORON_CITY_1,                                    SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City",        "DMC",               ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_LOST_WOODS_6,                                    ENTR_GORON_CITY_3,                                    SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City",        "Lost Woods",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_LOST_WOODS,            ENTRANCE_TYPE_OVERWORLD, "gc,lw"},
    { ENTR_GORON_SHOP_0,                                    ENTR_GORON_CITY_2,                                    SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City",        "Goron Shop",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR,  "gc", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GORON_CITY_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GORON_CITY_OFFSET), SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City",        "Goron City Grotto", ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO,    "gc,scrubs", 1},
    { ENTR_GORON_CITY_2,                                    ENTR_GORON_SHOP_0,                                    SINGLE_SCENE_INFO(SCENE_GORON_SHOP), "Goron Shop",        "Goron City",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR,  "gc"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GORON_CITY_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GORON_CITY_OFFSET), {{ SCENE_GROTTOS, 0x04 }},           "Goron City Grotto", "Goron City",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO,    "gc,scrubs"},

    // Zora's River
    { ENTR_HYRULE_FIELD_2,                                 ENTR_ZORAS_RIVER_0,                                  SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR",               "Hyrule Field",     ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LOST_WOODS_7,                                   ENTR_ZORAS_RIVER_4,                                  SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR",               "Lost Woods",       ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_LOST_WOODS,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_ZORAS_DOMAIN_0,                                 ENTR_ZORAS_RIVER_2,                                  SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR",               "Zora's Domain",    ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZR_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZR_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR",               "ZR Storms Grotto", ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZR_FAIRY_OFFSET),  ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZR_FAIRY_OFFSET),  SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR",               "ZR Fairy Grotto",  ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZR_OPEN_OFFSET),   ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZR_OPEN_OFFSET),   SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR",               "ZR Open Grotto",   ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZR_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZR_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x0A }},            "ZR Storms Grotto", "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZR_FAIRY_OFFSET),  ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZR_FAIRY_OFFSET),  {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},    "ZR Fairy Grotto",  "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZR_OPEN_OFFSET),   ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZR_OPEN_OFFSET),   {{ SCENE_GROTTOS, 0x00 }},            "ZR Open Grotto",   "ZR",               ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "chest"},

    // Zora's Domain
    { ENTR_ZORAS_RIVER_2,                                  ENTR_ZORAS_DOMAIN_0,                                 SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain",    "ZR",               ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_RIVER,    ENTRANCE_TYPE_OVERWORLD},
    { ENTR_LAKE_HYLIA_7,                                   ENTR_ZORAS_DOMAIN_4,                                 SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain",    "Lake Hylia",       ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_LAKE_HYLIA,     ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_ZORAS_FOUNTAIN_2,                               ENTR_ZORAS_DOMAIN_1,                                 SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain",    "ZF",               ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_OVERWORLD},
    { ENTR_ZORA_SHOP_0,                                    ENTR_ZORAS_DOMAIN_2,                                 SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain",    "Zora Shop",        ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZD_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZD_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain",    "ZD Storms Grotto", ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO,    "fairy", 1},
    { ENTR_ZORAS_DOMAIN_2,                                 ENTR_ZORA_SHOP_0,                                    SINGLE_SCENE_INFO(SCENE_ZORA_SHOP),    "Zora Shop",        "Zora's Domain",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_ZD_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_ZD_STORMS_OFFSET), {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},     "ZD Storms Grotto", "Zora's Domain",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO,    "fairy"},

    // Zora's Fountain
    { ENTR_ZORAS_DOMAIN_1,                 ENTR_ZORAS_FOUNTAIN_2,               SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "ZF",                          "Zora's Domain",               ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_0, ENTR_ZORAS_FOUNTAIN_5,               SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "ZF",                          "ZF Great Fairy Fountain",     ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR, "", 1},
    { ENTR_JABU_JABU_0,                    ENTR_ZORAS_FOUNTAIN_1,               SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "ZF",                          "Jabu Jabu's Belly",           ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { ENTR_ICE_CAVERN_0,                   ENTR_ZORAS_FOUNTAIN_3,               SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "ZF",                          "Ice Cavern",                  ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { ENTR_ZORAS_FOUNTAIN_5,               ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_0, {{ SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, 0x00 }}, "ZF Great Fairy Fountain",     "ZF",                          ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR},
    { ENTR_ZORAS_FOUNTAIN_1,               ENTR_JABU_JABU_0,                    SINGLE_SCENE_INFO(SCENE_JABU_JABU),             "Jabu Jabu's Belly",           "ZF",                          ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},
    { ENTR_JABU_JABU_BOSS_0,               ENTR_JABU_JABU_1,                    SINGLE_SCENE_INFO(SCENE_JABU_JABU),             "Jabu Jabu's Belly Boss Door", "Barinade",                    ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_JABU_JABU_1,                    ENTR_JABU_JABU_BOSS_0,               SINGLE_SCENE_INFO(SCENE_JABU_JABU_BOSS),        "Barinade",                    "Jabu Jabu's Belly Boss Door", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_ZORAS_FOUNTAIN_3,               ENTR_ICE_CAVERN_0,                   SINGLE_SCENE_INFO(SCENE_ICE_CAVERN),            "Ice Cavern",                  "ZF",                          ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},

    // Hyrule Field
    { ENTR_LOST_WOODS_8,                                         ENTR_HYRULE_FIELD_3,                                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "Lost Woods Bridge",      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "hf,lw"},
    { ENTR_MARKET_ENTRANCE_DAY_1,                                ENTR_HYRULE_FIELD_7,                                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "Market Entrance",        ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LON_LON_RANCH_0,                                      ENTR_HYRULE_FIELD_6,                                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "Lon Lon Ranch",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_OVERWORLD, "hf,llr"},
    { ENTR_KAKARIKO_VILLAGE_0,                                   ENTR_HYRULE_FIELD_1,                                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "Kakariko",               ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_KAKARIKO,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_ZORAS_RIVER_0,                                        ENTR_HYRULE_FIELD_2,                                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "ZR",                     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LAKE_HYLIA_0,                                         ENTR_HYRULE_FIELD_4,                                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "Lake Hylia",             ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LAKE_HYLIA,    ENTRANCE_TYPE_OVERWORLD, "hf,lh"},
    { ENTR_GERUDO_VALLEY_0,                                      ENTR_HYRULE_FIELD_5,                                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "GV",                     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_NEAR_MARKET_OFFSET),  ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_NEAR_MARKET_OFFSET),  SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Near Market Grotto",  ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_NEAR_KAK_OFFSET),     ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_NEAR_KAK_OFFSET),     SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Near Kak Grotto",     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "spider", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_TEKTITE_OFFSET),      ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_TEKTITE_OFFSET),      SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Tektite Grotto",      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "water", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_FAIRY_OFFSET),        ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_FAIRY_OFFSET),        SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Fairy Grotto",        ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_COW_OFFSET),          ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_COW_OFFSET),          SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Cow Grotto",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "webbed", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_OPEN_OFFSET),         ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_OPEN_OFFSET),         SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Open Grotto",         ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_INSIDE_FENCE_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_INSIDE_FENCE_OFFSET), SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Inside Fence Grotto", ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_SOUTHEAST_OFFSET),    ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_SOUTHEAST_OFFSET),    SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field",           "HF Southeast Grotto",    ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_NEAR_MARKET_OFFSET),  ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_NEAR_MARKET_OFFSET),  {{ SCENE_GROTTOS, 0x00 }},             "HF Near Market Grotto",  "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_NEAR_KAK_OFFSET),     ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_NEAR_KAK_OFFSET),     {{ SCENE_GROTTOS, 0x01 }},             "HF Near Kak Grotto",     "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "spider"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_TEKTITE_OFFSET),      ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_TEKTITE_OFFSET),      {{ SCENE_GROTTOS, 0x0B }},             "HF Tektite Grotto",      "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "water"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_FAIRY_OFFSET),        ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_FAIRY_OFFSET),        {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},     "HF Fairy Grotto",        "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_COW_OFFSET),          ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_COW_OFFSET),          {{ SCENE_GROTTOS, 0x05 }},             "HF Cow Grotto",          "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "webbed"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_OPEN_OFFSET),         ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_OPEN_OFFSET),         {{ SCENE_GROTTOS, 0x00 }},             "HF Open Grotto",         "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_INSIDE_FENCE_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_INSIDE_FENCE_OFFSET), {{ SCENE_GROTTOS, 0x02 }},             "HF Inside Fence Grotto", "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "srubs"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HF_SOUTHEAST_OFFSET),    ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HF_SOUTHEAST_OFFSET),    {{ SCENE_GROTTOS, 0x00 }},             "HF Southeast Grotto",    "Hyrule Field",           ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest"},

    // Lon Lon Ranch
    { ENTR_HYRULE_FIELD_6,                           ENTR_LON_LON_RANCH_0,                          SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "Lon Lon Ranch", "Hyrule Field",  ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LON_LON_BUILDINGS_0,                      ENTR_LON_LON_RANCH_4,                          SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "Lon Lon Ranch", "Talon's House", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "llr", 1},
    { ENTR_STABLE_0,                                 ENTR_LON_LON_RANCH_5,                          SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "Lon Lon Ranch", "LLR Stables",   ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { ENTR_LON_LON_BUILDINGS_1,                      ENTR_LON_LON_RANCH_10,                         SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "Lon Lon Ranch", "LLR Tower",     ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LLR_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LLR_OFFSET), SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "Lon Lon Ranch", "LLR Grotto",    ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_LON_LON_RANCH_4,                          ENTR_LON_LON_BUILDINGS_0,                      {{ SCENE_LON_LON_BUILDINGS, 0x00 }},    "Talon's House", "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "llr"},
    { ENTR_LON_LON_RANCH_5,                          ENTR_STABLE_0,                                 SINGLE_SCENE_INFO(SCENE_STABLE),        "LLR Stables",   "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow"},
    { ENTR_LON_LON_RANCH_10,                         ENTR_LON_LON_BUILDINGS_1,                      {{ SCENE_LON_LON_BUILDINGS, 0x01 }},    "LLR Tower",     "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LLR_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LLR_OFFSET), {{ SCENE_GROTTOS, 0x04 }},              "LLR Grotto",    "Lon Lon Ranch", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO,    "scrubs"},

    // Lake Hylia
    { ENTR_HYRULE_FIELD_4,                          ENTR_LAKE_HYLIA_0,                            SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia",             "Hyrule Field",           ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_ZORAS_DOMAIN_4,                          ENTR_LAKE_HYLIA_7,                            SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia",             "Zora's Domain",          ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_LAKESIDE_LABORATORY_0,                   ENTR_LAKE_HYLIA_4,                            SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia",             "LH Lab",                 ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh", 1},
    { ENTR_FISHING_POND_0,                          ENTR_LAKE_HYLIA_6,                            SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia",             "Fishing Hole",           ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LH_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LH_OFFSET), SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia",             "LH Grotto",              ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_WATER_TEMPLE_0,                          ENTR_LAKE_HYLIA_2,                            SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia",             "Water Temple",           ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},
    { ENTR_LAKE_HYLIA_4,                            ENTR_LAKESIDE_LABORATORY_0,                   SINGLE_SCENE_INFO(SCENE_LAKESIDE_LABORATORY), "LH Lab",                 "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh"},
    { ENTR_LAKE_HYLIA_6,                            ENTR_FISHING_POND_0,                          SINGLE_SCENE_INFO(SCENE_FISHING_POND),        "Fishing Hole",           "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_LH_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_LH_OFFSET), {{ SCENE_GROTTOS, 0x04 }},                    "LH Grotto",              "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO,    "lh,scrubs"},
    { ENTR_LAKE_HYLIA_2,                            ENTR_WATER_TEMPLE_0,                          SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),        "Water Temple",           "Lake Hylia",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh"},
    { ENTR_WATER_TEMPLE_BOSS_0,                     ENTR_WATER_TEMPLE_1,                          SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),        "Water Temple Boss Door", "Morpha",                 ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},
    { ENTR_WATER_TEMPLE_1,                          ENTR_WATER_TEMPLE_BOSS_0,                     SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE_BOSS),   "Morpha",                 "Water Temple Boss Door", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},

    // Gerudo Area
    { ENTR_HYRULE_FIELD_5,                                  ENTR_GERUDO_VALLEY_0,                                 SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV",                      "Hyrule Field",            ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_HYRULE_FIELD,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_GERUDOS_FORTRESS_0,                              ENTR_GERUDO_VALLEY_3,                                 SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV",                      "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { ENTR_GERUDOS_FORTRESS_0_5,                            -1,                                                   SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV",                      "Lake Hylia",              ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_LAKE_HYLIA,        ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_CARPENTERS_TENT_0,                               ENTR_GERUDO_VALLEY_4,                                 SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV",                      "Carpenters' Tent",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GV_OCTOROK_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GV_OCTOROK_OFFSET), SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV",                      "GV Octorok Grotto",       ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GV_STORMS_OFFSET),  ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GV_STORMS_OFFSET),  SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV",                      "GV Storms Grotto",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_GERUDO_VALLEY_3,                                 ENTR_GERUDOS_FORTRESS_0,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF",                      "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { ENTR_HAUNTED_WASTELAND_0,                             ENTR_GERUDOS_FORTRESS_15,                             SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF",                      "Haunted Wasteland",       ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "gerudo fortress"},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GF_STORMS_OFFSET),  ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GF_STORMS_OFFSET),  SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF",                      "GF Storms Grotto",        ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "gerudo fortress", 1},
    { ENTR_GERUDO_TRAINING_GROUND_0,                        ENTR_GERUDOS_FORTRESS_14,                             SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF",                      "Gerudo Training Grounds", ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_DUNGEON,   "gerudo fortress,gtg", 1},
    { ENTR_GERUDO_VALLEY_4,                                 ENTR_CARPENTERS_TENT_0,                               SINGLE_SCENE_INFO(SCENE_CARPENTERS_TENT),        "Carpenters' Tent",        "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GV_OCTOROK_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GV_OCTOROK_OFFSET), {{ SCENE_GROTTOS, 0x06 }},                       "GV Octorok Grotto",       "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GV_STORMS_OFFSET),  ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GV_STORMS_OFFSET),  {{ SCENE_GROTTOS, 0x0A }},                       "GV Storms Grotto",        "GV",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_GF_STORMS_OFFSET),  ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_GF_STORMS_OFFSET),  {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},               "GF Storms Grotto",        "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "gerudo fortress"},
    { ENTR_GERUDOS_FORTRESS_14,                             ENTR_GERUDO_TRAINING_GROUND_0,                        SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "Gerudo Training Grounds", "GF",                      ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_DUNGEON,   "gerudo fortress,gtg"},

    // The Wasteland
    { ENTR_GERUDOS_FORTRESS_15,                           ENTR_HAUNTED_WASTELAND_0,                           SINGLE_SCENE_INFO(SCENE_HAUNTED_WASTELAND),     "Haunted Wasteland",             "GF",                            ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, "hw,gerudo fortress"},
    { ENTR_DESERT_COLOSSUS_0,                             ENTR_HAUNTED_WASTELAND_1,                           SINGLE_SCENE_INFO(SCENE_HAUNTED_WASTELAND),     "Haunted Wasteland",             "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { ENTR_HAUNTED_WASTELAND_1,                           ENTR_DESERT_COLOSSUS_0,                             SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Desert Colossus",               "Haunted Wasteland",             ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_2,                ENTR_DESERT_COLOSSUS_7,                             SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Desert Colossus",               "Colossus Great Fairy Fountain", ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR,  "dc", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_COLOSSUS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_COLOSSUS_OFFSET), SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Desert Colossus",               "Colossus Grotto",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO,    "dc,scrubs", 1},
    { ENTR_SPIRIT_TEMPLE_0,                               ENTR_DESERT_COLOSSUS_1,                             SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Desert Colossus",               "Spirit Temple",                 ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { ENTR_DESERT_COLOSSUS_7,                             ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_2,                {{ SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, 0x02 }}, "Colossus Great Fairy Fountain", "Colossus",                      ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR,  "dc"},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_COLOSSUS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_COLOSSUS_OFFSET), {{ SCENE_GROTTOS, 0x0A }},                      "Colossus Grotto",               "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO,    "dc,scrubs"},
    { ENTR_DESERT_COLOSSUS_1,                             ENTR_SPIRIT_TEMPLE_0,                               SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),         "Spirit Temple",                 "Desert Colossus",               ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "dc"},
    { ENTR_SPIRIT_TEMPLE_BOSS_0,                          ENTR_SPIRIT_TEMPLE_1,                               SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),         "Spirit Temple Boss Door",       "Twinrova",                      ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_SPIRIT_TEMPLE_1,                               ENTR_SPIRIT_TEMPLE_BOSS_0,                          SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE_BOSS),    "Twinrova",                      "Spirit Temple Boss Door",       ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON,   "", 1},

    // Market
    { ENTR_HYRULE_FIELD_7,                ENTR_MARKET_ENTRANCE_DAY_1,         {SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_DAY), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_RUINS)},                                                    "Market Entrance",        "Hyrule Field",           ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_MARKET_DAY_0,                  ENTR_MARKET_ENTRANCE_DAY_0,         {SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_DAY), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_RUINS)},                                                    "Market Entrance",        "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { ENTR_MARKET_GUARD_HOUSE_0,          ENTR_MARKET_ENTRANCE_DAY_2,         {SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_DAY), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_RUINS)},                                                    "Market Entrance",        "Guard House",            ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "pots,poe", 1},
    { ENTR_MARKET_ENTRANCE_DAY_0,         ENTR_MARKET_DAY_0,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "Market Entrance",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { ENTR_HYRULE_CASTLE_0,               ENTR_MARKET_DAY_1,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "HC Grounds / OGC",       ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_0, ENTR_MARKET_DAY_2,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "Outside Temple of Time", ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { ENTR_SHOOTING_GALLERY_1,            ENTR_MARKET_DAY_8,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "MK Shooting Gallery",    ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "child", 1},
    { ENTR_BOMBCHU_BOWLING_ALLEY_0,       ENTR_MARKET_DAY_7,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "Bombchu Bowling",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_TREASURE_BOX_SHOP_0,           ENTR_MARKET_DAY_10,                 {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "Treasure Chest Game",    ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BACK_ALLEY_HOUSE_0,            ENTR_BACK_ALLEY_DAY_3,              {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "Man-in-Green's House",   ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_HAPPY_MASK_SHOP_0,             ENTR_MARKET_DAY_9,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "Mask Shop",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BAZAAR_1,                      ENTR_MARKET_DAY_6,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "MK Bazaar",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "shop", 1},
    { ENTR_POTION_SHOP_MARKET_0,          ENTR_MARKET_DAY_5,                  {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "MK Potion Shop",         ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BOMBCHU_SHOP_1,                ENTR_BACK_ALLEY_DAY_2,              {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market",                 "Bombchu Shop",           ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_MARKET_ENTRANCE_DAY_2,         ENTR_MARKET_GUARD_HOUSE_0,          {{ SCENE_MARKET_GUARD_HOUSE }},                                                                                                                                                           "Guard House",            "Market Entrance",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "pots,poe"},
    { ENTR_MARKET_DAY_8,                  ENTR_SHOOTING_GALLERY_1,            {{ SCENE_SHOOTING_GALLERY, 0x01 }},                                                                                                                                                       "MK Shooting Gallery",    "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_7,                  ENTR_BOMBCHU_BOWLING_ALLEY_0,       SINGLE_SCENE_INFO(SCENE_BOMBCHU_BOWLING_ALLEY),                                                                                                                                           "Bombchu Bowling",        "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_10,                 ENTR_TREASURE_BOX_SHOP_0,           SINGLE_SCENE_INFO(SCENE_TREASURE_BOX_SHOP),                                                                                                                                               "Treasure Chest Game",    "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_BACK_ALLEY_DAY_3,              ENTR_BACK_ALLEY_HOUSE_0,            SINGLE_SCENE_INFO(SCENE_BACK_ALLEY_HOUSE),                                                                                                                                                "Man-in-Green's House",   "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_9,                  ENTR_HAPPY_MASK_SHOP_0,             SINGLE_SCENE_INFO(SCENE_HAPPY_MASK_SHOP),                                                                                                                                                 "Mask Shop",              "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_6,                  ENTR_BAZAAR_1,                      {{ SCENE_BAZAAR, 0x01 }},                                                                                                                                                                 "MK Bazaar",              "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "shop"},
    { ENTR_MARKET_DAY_5,                  ENTR_POTION_SHOP_MARKET_0,          SINGLE_SCENE_INFO(SCENE_POTION_SHOP_MARKET),                                                                                                                                              "MK Potion Shop",         "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_BACK_ALLEY_DAY_2,              ENTR_BOMBCHU_SHOP_1,                SINGLE_SCENE_INFO(SCENE_BOMBCHU_SHOP),                                                                                                                                                    "Bombchu Shop",           "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_2,                  ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_0, {SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS)},                            "Outside Temple of Time", "Market",                 ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "tot"},
    { ENTR_TEMPLE_OF_TIME_0,              ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_1, {SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS)},                            "Outside Temple of Time", "Temple of Time",         ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "tot", 1},
    { ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_1, ENTR_TEMPLE_OF_TIME_0,              SINGLE_SCENE_INFO(SCENE_TEMPLE_OF_TIME),                                                                                                                                                  "Temple of Time",         "Outside Temple of Time", ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "tot"},

    // Hyrule Castle
    { ENTR_MARKET_DAY_1,                                   ENTR_HYRULE_CASTLE_0,                                {SCENE_NO_SPAWN(SCENE_HYRULE_CASTLE), SCENE_NO_SPAWN(SCENE_OUTSIDE_GANONS_CASTLE)}, "HC Grounds / OGC",         "Market",                   ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_1,                 ENTR_HYRULE_CASTLE_2,                                SINGLE_SCENE_INFO(SCENE_HYRULE_CASTLE),                                             "HC Grounds",               "HC Great Fairy Fountain",  ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HC_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HC_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_HYRULE_CASTLE),                                             "HC Grounds",               "HC Storms Grotto",         ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO,    "bombable", 1},
    { ENTR_HYRULE_CASTLE_2,                                ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_1,                 {{ SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, 0x01 }},                                     "HC Great Fairy Fountain",  "HC Grounds",               ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_RANDO_GROTTO_EXIT(GROTTO_HC_STORMS_OFFSET), ENTRANCE_RANDO_GROTTO_LOAD(GROTTO_HC_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x09 }},                                                          "HC Storms Grotto",         "HC Grounds",               ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO,    "bombable"},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_2,                  ENTR_POTION_SHOP_KAKARIKO_1,                         SINGLE_SCENE_INFO(SCENE_OUTSIDE_GANONS_CASTLE),                                     "OGC",                      "OGC Great Fairy Fountain", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR,  "outside ganon's castle", 1},
    { ENTR_INSIDE_GANONS_CASTLE_0,                         ENTR_HYRULE_CASTLE_1,                                SINGLE_SCENE_INFO(SCENE_OUTSIDE_GANONS_CASTLE),                                     "OGC",                      "Ganon's Castle",           ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,   "outside ganon's castle,gc", 1},
    { ENTR_POTION_SHOP_KAKARIKO_1,                         ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_2,                  {{ SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, 0x02 }},                                      "OGC Great Fairy Fountain", "OGC",                      ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR,  "outside ganon's castle"},
    { ENTR_HYRULE_CASTLE_1,                                ENTR_INSIDE_GANONS_CASTLE_0,                         SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE),                                      "Ganon's Castle",           "OGC",                      ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,   "outside ganon's castle,gc"}
};

// Check if Link is in the area and return that scene/entrance for tracking
s8 LinkIsInArea(const EntranceData* entrance) {
    bool result = false;

    if (gPlayState == nullptr) {
        return -1;
    }

    // Handle detecting the current grotto
    if ((gPlayState->sceneNum == SCENE_FAIRYS_FOUNTAIN || gPlayState->sceneNum == SCENE_GROTTOS) &&
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
        if ((index == ENTR_ZORAS_RIVER_0 && Entrance_GetIsEntranceDiscovered(ENTR_ZORAS_RIVER_3)) || (index == ENTR_ZORAS_RIVER_3 && Entrance_GetIsEntranceDiscovered(ENTR_ZORAS_RIVER_0))) {
            isDiscovered = true;
        } else if ((index == ENTR_HYRULE_FIELD_2 && Entrance_GetIsEntranceDiscovered(ENTR_HYRULE_FIELD_14)) || (index == ENTR_HYRULE_FIELD_14 && Entrance_GetIsEntranceDiscovered(ENTR_HYRULE_FIELD_2))) {
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

s16 GetLastEntranceOverride() {
    return lastEntranceIndex;
}

s16 GetCurrentGrottoId() {
    return currentGrottoId;
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

void EntranceTrackerWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(600, 375), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Entrance Tracker", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
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

void EntranceTrackerWindow::InitElement() {
    // Setup hooks for loading and clearing the entrance tracker data
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        InitEntranceTrackingData();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](int32_t fileNum) {
        ClearEntranceTrackingData();
    });
}

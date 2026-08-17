#include "randomizer_entrance_tracker.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/SohGui.hpp"

#include <string>
#include <vector>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include "soh/Enhancements/randomizer/randomizer.h"

extern "C" {
#include <z64.h>
#include "macros.h"
extern PlayState* gPlayState;

#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
}

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "entrance.h"

using namespace UIWidgets;

#define COLOR_ORANGE IM_COL32(230, 159, 0, 255)
#define COLOR_GREEN IM_COL32(0, 158, 115, 255)
#define COLOR_GRAY IM_COL32(155, 155, 155, 255)

namespace EntranceTracker {
EntranceOverride srcListSortedByArea[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };
EntranceOverride destListSortedByArea[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };
EntranceOverride srcListSortedByType[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };
EntranceOverride destListSortedByType[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };
EntranceTrackingData gEntranceTrackingData = { 0 };

static const EntranceOverride emptyOverride = { 0 };

static s16 lastEntranceIndex = -1;
static s16 currentGrottoId = -1;
static s16 lastSceneOrEntranceDetected = -1;

Color_RGBA8 Color_Background = { 0, 0, 0, 255 };
static WidgetInfo backgroundColorWidget;
static WidgetInfo windowTypeWidget;

static bool presetLoaded = false;
static ImVec2 presetPos;
static ImVec2 presetSize;

static std::string spoilerEntranceGroupNames[] = {
    "Spawns/Warp Songs", "Kokiri Forest",     "Lost Woods",           "Sacred Forest Meadow",
    "Kakariko Village",  "Graveyard",         "Death Mountain Trail", "Death Mountain Crater",
    "Goron City",        "Zora's River",      "Zora's Domain",        "Zora's Fountain",
    "Hyrule Field",      "Lon Lon Ranch",     "Lake Hylia",           "Gerudo Valley",
    "Gerudo Fortress",   "Haunted Wasteland", "Desert Colossus",      "Market",
    "Hyrule Castle",
};

static std::string groupTypeNames[] = {
    "One Way", "Overworld", "Interior", "Fortress", "Grotto", "Dungeon", "Door",
};

// Entrance data for the tracker taken from the 3ds rando entrance tracker, and supplemented with scene/spawn info and
// meta search tags ENTR_HYRULE_FIELD_10 and ENTR_POTION_SHOP_KAKARIKO_1 have been repurposed for entrance randomizer
const EntranceData entranceData[] = {
    // clang-format off
    //index,                reverse, scenes (and spawns),     source name,   destination name, source group,           destination group,      type,                 metaTag, oneExit
    { ENTR_LINKS_HOUSE_CHILD_SPAWN,   -1,      SINGLE_SCENE_INFO(SCENE_LINKS_HOUSE), "Child Spawn", "Link's House",   ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_HYRULE_FIELD_10, -1,      SINGLE_SCENE_INFO(SCENE_TEMPLE_OF_TIME), "Adult Spawn", "Temple of Time", ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    { ENTR_SACRED_FOREST_MEADOW_WARP_PAD,  -1, {{ -1 }}, "Minuet of Forest",   "SFM Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD, -1, {{ -1 }}, "Bolero of Fire",     "DMC Warp Pad",              ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_LAKE_HYLIA_WARP_PAD,            -1, {{ -1 }}, "Serenade of Water",  "Lake Hylia Warp Pad",       ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_DESERT_COLOSSUS_WARP_PAD,       -1, {{ -1 }}, "Requiem of Spirit",  "Desert Colossus Warp Pad",  ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_GRAVEYARD_WARP_PAD,             -1, {{ -1 }}, "Nocturne of Shadow", "Graveyard Warp Pad",        ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_TEMPLE_OF_TIME_WARP_PAD,        -1, {{ -1 }}, "Prelude of Light",   "Temple of Time Warp Pad",   ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

    // Kokiri Forest
    { ENTR_LOST_WOODS_BRIDGE_EAST_EXIT,              ENTR_KOKIRI_FOREST_LOWER_EXIT,                 SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "Kokiri Forest Lower Exit",     "Lost Woods Bridge East Exit", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_LOST_WOODS_SOUTH_EXIT,                    ENTR_KOKIRI_FOREST_UPPER_EXIT,                 SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "Kokiri Forest Upper Exit",     "Lost Woods South Exit",       ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_LINKS_HOUSE_1,                            ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE,        SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF Link's House Entry",        "Link's House",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_MIDOS_HOUSE_0,                            ENTR_KOKIRI_FOREST_OUTSIDE_MIDOS_HOUSE,        SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF Mido's House Entry",        "Mido's House",                ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_SARIAS_HOUSE_0,                           ENTR_KOKIRI_FOREST_OUTSIDE_SARIAS_HOUSE,       SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF Saria's House Entry",       "Saria's House",               ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_TWINS_HOUSE_0,                            ENTR_KOKIRI_FOREST_OUTSIDE_TWINS_HOUSE,        SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF House of Twins Entry",      "House of Twins",              ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_KNOW_IT_ALL_BROS_HOUSE_0,                 ENTR_KOKIRI_FOREST_OUTSIDE_KNOW_IT_ALL_HOUSE,  SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF Know-It-All House Entry",   "Know-It-All House",           ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_KOKIRI_SHOP_0,                            ENTR_KOKIRI_FOREST_OUTSIDE_SHOP,               SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF Shop Entry",                "Kokiri Shop",                 ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_KF_STORMS_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_KF_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF Storms Grotto Entry",       "KF Storms Grotto",            ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTR_DEKU_TREE_ENTRANCE,                       ENTR_KOKIRI_FOREST_OUTSIDE_DEKU_TREE,          SINGLE_SCENE_INFO(SCENE_KOKIRI_FOREST),          "KF Outside Deku Tree",         "Deku Tree Entrance",          ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE,        ENTR_LINKS_HOUSE_1,                            SINGLE_SCENE_INFO(SCENE_LINKS_HOUSE),            "Link's House",                 "KF Link's House Entry",       ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_OUTSIDE_MIDOS_HOUSE,        ENTR_MIDOS_HOUSE_0,                            SINGLE_SCENE_INFO(SCENE_MIDOS_HOUSE),            "Mido's House",                 "KF Mido's House Entry",       ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_OUTSIDE_SARIAS_HOUSE,       ENTR_SARIAS_HOUSE_0,                           SINGLE_SCENE_INFO(SCENE_SARIAS_HOUSE),           "Saria's House",                "KF Saria's House Entry",      ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_OUTSIDE_TWINS_HOUSE,        ENTR_TWINS_HOUSE_0,                            SINGLE_SCENE_INFO(SCENE_TWINS_HOUSE),            "House of Twins",               "KF House of Twins Entry",     ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_OUTSIDE_KNOW_IT_ALL_HOUSE,  ENTR_KNOW_IT_ALL_BROS_HOUSE_0,                 SINGLE_SCENE_INFO(SCENE_KNOW_IT_ALL_BROS_HOUSE), "Know-It-All House",            "KF Know-It-All House Entry",  ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTR_KOKIRI_FOREST_OUTSIDE_SHOP,               ENTR_KOKIRI_SHOP_0,                            SINGLE_SCENE_INFO(SCENE_KOKIRI_SHOP),            "Kokiri Shop",                  "KF Shop Entry",               ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR,  ""},
    { ENTRANCE_GROTTO_EXIT(GROTTO_KF_STORMS_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_KF_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x00 }},                       "KF Storms Grotto",             "KF Storms Grotto Entry",      ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTR_KOKIRI_FOREST_OUTSIDE_DEKU_TREE,          ENTR_DEKU_TREE_ENTRANCE,                       SINGLE_SCENE_INFO(SCENE_DEKU_TREE),              "Deku Tree Entrance",           "KF Outside Deku Tree",        ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   ""},
    { ENTR_DEKU_TREE_BOSS_ENTRANCE,                  ENTR_DEKU_TREE_BOSS_DOOR,                      SINGLE_SCENE_INFO(SCENE_DEKU_TREE),              "Deku Tree Boss Door",          "Gohma",                       ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_DEKU_TREE_BOSS_DOOR,                      ENTR_DEKU_TREE_BOSS_ENTRANCE,                  SINGLE_SCENE_INFO(SCENE_DEKU_TREE_BOSS),         "Gohma",                        "Deku Tree Boss Door",         ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_KOKIRI_FOREST_DEKU_TREE_BLUE_WARP,        -1,                                            SINGLE_SCENE_INFO(SCENE_DEKU_TREE_BOSS),         "Gohma Blue Warp",              "Deku Tree Blue Warp",         ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_ONE_WAY,   "bw", 1},

    // Lost Woods
    { ENTR_KOKIRI_FOREST_LOWER_EXIT,                         ENTR_LOST_WOODS_BRIDGE_EAST_EXIT,                      SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods Bridge East Exit",    "Kokiri Forest Lower Exit",         ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_HYRULE_FIELD_WOODED_EXIT,                         ENTR_LOST_WOODS_BRIDGE_WEST_EXIT,                      SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods Bridge West Exit",    "Hyrule Field Wooded Exit",         ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "lw,hf"},
    { ENTR_KOKIRI_FOREST_UPPER_EXIT,                         ENTR_LOST_WOODS_SOUTH_EXIT,                            SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods South Exit",          "Kokiri Forest Upper Exit",         ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_GORON_CITY_TUNNEL_SHORTCUT,                       ENTR_LOST_WOODS_TUNNEL_SHORTCUT,                       SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods Tunnel Shortcut",     "Goron City Tunnel Shortcut",       ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_GORON_CITY,    ENTRANCE_TYPE_OVERWORLD, "lw,gc"},
    { ENTR_ZORAS_RIVER_UNDERWATER_SHORTCUT,                  ENTR_LOST_WOODS_UNDERWATER_SHORTCUT,                   SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods Underwater Shortcut", "Zora's River Underwater Shortcut", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_SACRED_FOREST_MEADOW_SOUTH_EXIT,                  ENTR_LOST_WOODS_NORTH_EXIT,                            SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "Lost Woods North Exit",          "Sacred Forest Meadow South Exit",  ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_SFM,           ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTRANCE_GROTTO_LOAD(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "LW Tunnel Grotto Entry",         "LW Tunnel Grotto",                 ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,chest", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_LW_SCRUBS_OFFSET),         ENTRANCE_GROTTO_EXIT(GROTTO_LW_SCRUBS_OFFSET),         SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "LW North Grotto Entry",          "LW Deku Scrub Grotto",             ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,scrubs", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_LW_DEKU_THEATRE_OFFSET),   ENTRANCE_GROTTO_EXIT(GROTTO_LW_DEKU_THEATRE_OFFSET),   SINGLE_SCENE_INFO(SCENE_LOST_WOODS), "LW Meadow Grotto Entry",         "Deku Theater",                     ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,mask,stage", 1},
    { ENTRANCE_GROTTO_EXIT(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_LW_NEAR_SHORTCUTS_OFFSET), {{ SCENE_GROTTOS, 0x00 }},           "LW Tunnel Grotto",               "LW Tunnel Grotto Entry",           ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,chest"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_LW_SCRUBS_OFFSET),         ENTRANCE_GROTTO_LOAD(GROTTO_LW_SCRUBS_OFFSET),         {{ SCENE_GROTTOS, 0x07 }},           "LW Deku Scrub Grotto",           "LW North Grotto Entry",            ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,scrubs"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_LW_DEKU_THEATRE_OFFSET),   ENTRANCE_GROTTO_LOAD(GROTTO_LW_DEKU_THEATRE_OFFSET),   {{ SCENE_GROTTOS, 0x0C }},           "Deku Theater",                   "LW Meadow Grotto Entry",           ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_GROTTO,    "lw,mask,stage"},

    // Sacred Forest Meadow
    { ENTR_LOST_WOODS_NORTH_EXIT,                        ENTR_SACRED_FOREST_MEADOW_SOUTH_EXIT,           SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "Sacred Forest Meadow South Exit",            "Lost Woods North Exit",                      ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTRANCE_GROTTO_LOAD(GROTTO_SFM_WOLFOS_OFFSET),    ENTRANCE_GROTTO_EXIT(GROTTO_SFM_WOLFOS_OFFSET), SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM Wolfos Grotto Entry",                    "SFM Wolfos Grotto",                          ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_SFM_FAIRY_OFFSET),     ENTRANCE_GROTTO_EXIT(GROTTO_SFM_FAIRY_OFFSET),  SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM Fairy Grotto Entry",                     "SFM Fairy Grotto",                           ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_SFM_STORMS_OFFSET),    ENTRANCE_GROTTO_EXIT(GROTTO_SFM_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "SFM Storms Grotto Entry",                    "SFM Deku Scrub Grotto",                      ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_FOREST_TEMPLE_ENTRANCE,                       ENTR_SACRED_FOREST_MEADOW_OUTSIDE_TEMPLE,       SINGLE_SCENE_INFO(SCENE_SACRED_FOREST_MEADOW), "Sacred Forest Meadow Outside Forest Temple", "Forest Temple Entrance",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTRANCE_GROTTO_EXIT(GROTTO_SFM_WOLFOS_OFFSET),    ENTRANCE_GROTTO_LOAD(GROTTO_SFM_WOLFOS_OFFSET), {{ SCENE_GROTTOS, 0x08 }},                     "SFM Wolfos Grotto",                          "SFM Wolfos Grotto Entry",                    ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_GROTTO_EXIT(GROTTO_SFM_FAIRY_OFFSET),     ENTRANCE_GROTTO_LOAD(GROTTO_SFM_FAIRY_OFFSET),  {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},             "SFM Fairy Grotto",                           "SFM Fairy Grotto Entry",                     ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_GROTTO_EXIT(GROTTO_SFM_STORMS_OFFSET),    ENTRANCE_GROTTO_LOAD(GROTTO_SFM_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x0A }},                     "SFM Deku Scrub Grotto",                      "SFM Storms Grotto Entry",                    ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTR_SACRED_FOREST_MEADOW_OUTSIDE_TEMPLE,          ENTR_FOREST_TEMPLE_ENTRANCE,                    SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),        "Forest Temple Entrance",                     "Sacred Forest Meadow Outside Forest Temple", ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON},
    { ENTR_FOREST_TEMPLE_BOSS_ENTRANCE,                  ENTR_FOREST_TEMPLE_BOSS_DOOR,                   SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),        "Forest Temple Boss Door",                    "Phantom Ganon",                              ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_FOREST_TEMPLE_BOSS_DOOR,                      ENTR_FOREST_TEMPLE_BOSS_ENTRANCE,               SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE_BOSS),   "Phantom Ganon",                              "Forest Temple Boss Door",                    ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_SACRED_FOREST_MEADOW_FOREST_TEMPLE_BLUE_WARP, -1,                                             SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE_BOSS),   "Phantom Ganon Blue Warp",                    "Forest Temple Blue Warp",                    ENTRANCE_GROUP_SFM, ENTRANCE_GROUP_SFM,        ENTRANCE_TYPE_ONE_WAY, "bw", 1},

    // Kakariko Village
    { ENTR_HYRULE_FIELD_STAIRS_EXIT,                    ENTR_KAKARIKO_VILLAGE_FRONT_GATE,                 SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko Front Gate",            "Hyrule Field Stairs Exit",         ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_HYRULE_FIELD,         ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_GRAVEYARD_ENTRANCE,                          ENTR_KAKARIKO_VILLAGE_SOUTHEAST_EXIT,             SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko Southeast Exit",        "Graveyard Entrance",               ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_GRAVEYARD,            ENTRANCE_TYPE_OVERWORLD},
    { ENTR_DEATH_MOUNTAIN_TRAIL_BOTTOM_EXIT,            ENTR_KAKARIKO_VILLAGE_GUARD_GATE,                 SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko Guard Gate Exit",       "Death Mountain Trail Bottom Exit", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_OVERWORLD},
    { ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0,               ENTR_KAKARIKO_VILLAGE_OUTSIDE_CENTER_GUEST_HOUSE, SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Boss House Entry",           "Carpenter Boss House",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_HOUSE_OF_SKULLTULA_0,                        ENTR_KAKARIKO_VILLAGE_OUTSIDE_SKULKLTULA_HOUSE,   SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Skulltula House Entry",      "House of Skulltula",               ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_IMPAS_HOUSE_FRONT,                           ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_FRONT,  SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Impa's House Front Entry",   "Impa's House Front",               ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_IMPAS_HOUSE_BACK,                            ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_BACK,   SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Impa's House Back Entry",    "Impa's House Back",                ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { ENTR_WINDMILL_AND_DAMPES_GRAVE_WINDMILL,          ENTR_KAKARIKO_VILLAGE_OUTSIDE_WINDMILL,           SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Windmill Entry",             "Windmill",                         ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_SHOOTING_GALLERY_0,                          ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOOTING_GALLERY,   SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Shooting Gallery Entry",     "Kak Shooting Gallery",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "adult", 1},
    { ENTR_POTION_SHOP_GRANNY_0,                        ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOP_GRANNY,        SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Granny's Potion Shop Entry", "Granny's Potion Shop",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BAZAAR_0,                                    ENTR_KAKARIKO_VILLAGE_OUTSIDE_BAZAAR,             SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Bazaar Entry",               "Kak Bazaar",                       ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "shop", 1},
    { ENTR_POTION_SHOP_KAKARIKO_FRONT,                  ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_FRONT,  SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Potion Shop Front Entry",    "Kak Potion Shop Front",            ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_POTION_SHOP_KAKARIKO_BACK,                   ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK,   SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Potion Shop Back Entry",     "Kak Potion Shop Back",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_KAK_OPEN_OFFSET),     ENTRANCE_GROTTO_EXIT(GROTTO_KAK_OPEN_OFFSET),     SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Open Grotto Entry",          "Kak Open Grotto",                  ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_KAK_REDEAD_OFFSET),   ENTRANCE_GROTTO_EXIT(GROTTO_KAK_REDEAD_OFFSET),   SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kak Center Grotto Entry",        "Kak Redead Grotto",                ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTR_BOTTOM_OF_THE_WELL_ENTRANCE,                 ENTR_KAKARIKO_VILLAGE_OUTSIDE_BOTTOM_OF_THE_WELL, SINGLE_SCENE_INFO(SCENE_KAKARIKO_VILLAGE),            "Kakariko Outside the Well",      "Bottom of the Well Entrance",      ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON,   "botw", 1},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_CENTER_GUEST_HOUSE, ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0,               SINGLE_SCENE_INFO(SCENE_KAKARIKO_CENTER_GUEST_HOUSE), "Carpenter Boss House",           "Kak Boss House Entry",             ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_SKULKLTULA_HOUSE,   ENTR_HOUSE_OF_SKULLTULA_0,                        SINGLE_SCENE_INFO(SCENE_HOUSE_OF_SKULLTULA),          "House of Skulltula",             "Kak Skulltula House Entry",        ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_FRONT,  ENTR_IMPAS_HOUSE_FRONT,                           SINGLE_SCENE_INFO(SCENE_IMPAS_HOUSE),                 "Impa's House Front",             "Kak Impa's House Front Entry",     ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_BACK,   ENTR_IMPAS_HOUSE_BACK,                            SINGLE_SCENE_INFO(SCENE_IMPAS_HOUSE),                 "Impa's House Back",              "Kak Impa's House Back Entry",      ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "cow"},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_WINDMILL,           ENTR_WINDMILL_AND_DAMPES_GRAVE_WINDMILL,          SINGLE_SCENE_INFO(SCENE_WINDMILL_AND_DAMPES_GRAVE),   "Windmill",                       "Kak Windmill Entry",               ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOOTING_GALLERY,   ENTR_SHOOTING_GALLERY_0,                          {{ SCENE_SHOOTING_GALLERY, 0x00 }},                   "Kak Shooting Gallery",           "Kak Shooting Gallery Entry",       ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOP_GRANNY,        ENTR_POTION_SHOP_GRANNY_0,                        SINGLE_SCENE_INFO(SCENE_POTION_SHOP_GRANNY),          "Granny's Potion Shop",           "Kak Granny's Potion Shop Entry",   ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_BAZAAR,             ENTR_BAZAAR_0,                                    {{ SCENE_BAZAAR, 0x00 }},                             "Kak Bazaar",                     "Kak Bazaar Entry",                 ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR,  "shop"},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_FRONT,  ENTR_POTION_SHOP_KAKARIKO_FRONT,                  SINGLE_SCENE_INFO(SCENE_POTION_SHOP_KAKARIKO),        "Kak Potion Shop Front",          "Kak Potion Shop Front Entry",      ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK,   ENTR_POTION_SHOP_KAKARIKO_BACK,                   SINGLE_SCENE_INFO(SCENE_POTION_SHOP_KAKARIKO),        "Kak Potion Shop Back",           "Kak Potion Shop Back Entry",       ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_GROTTO_EXIT(GROTTO_KAK_OPEN_OFFSET),     ENTRANCE_GROTTO_LOAD(GROTTO_KAK_OPEN_OFFSET),     {{ SCENE_GROTTOS, 0x00 }},                            "Kak Open Grotto",                "Kak Open Grotto Entry",            ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_KAK_REDEAD_OFFSET),   ENTRANCE_GROTTO_LOAD(GROTTO_KAK_REDEAD_OFFSET),   {{ SCENE_GROTTOS, 0x03 }},                            "Kak Redead Grotto",              "Kak Center Grotto Entry",          ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTR_KAKARIKO_VILLAGE_OUTSIDE_BOTTOM_OF_THE_WELL, ENTR_BOTTOM_OF_THE_WELL_ENTRANCE,                 SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL),          "Bottom of the Well Entrance",    "Kakariko Outside the Well",        ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO,             ENTRANCE_TYPE_DUNGEON,   "botw"},

    // The Graveyard
    { ENTR_KAKARIKO_VILLAGE_SOUTHEAST_EXIT,   ENTR_GRAVEYARD_ENTRANCE,               SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard Entrance",           "Kakariko Southeast Exit",      ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_KAKARIKO,  ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GRAVEKEEPERS_HUT_0,                ENTR_GRAVEYARD_OUTSIDE_DAMPES_HUT,     SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "GY Dampe's Hut Entry",         "Dampe's Hut",                  ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR, "", 1},
    { ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0,      ENTR_GRAVEYARD_SHIELD_GRAVE_EXIT,      SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "GY Near-Hut Grave Entry",      "Shield Grave",                 ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { ENTR_REDEAD_GRAVE_0,                    ENTR_GRAVEYARD_HEART_PIECE_GRAVE_EXIT, SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "GY Near-Tomb Grave Entry",     "Heart Piece Grave",            ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { ENTR_ROYAL_FAMILYS_TOMB_0,              ENTR_GRAVEYARD_ROYAL_TOMB_EXIT,        SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "GY Royal Family's Tomb Entry", "Royal Family's Tomb",          ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "", 1},
    { ENTR_WINDMILL_AND_DAMPES_GRAVE_GRAVE,   ENTR_GRAVEYARD_DAMPES_GRAVE_EXIT,      SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "GY Near-Ledge Grave Entry",    "Dampe's Grave",                ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "race", 1},
    { ENTR_SHADOW_TEMPLE_ENTRANCE,            ENTR_GRAVEYARD_OUTSIDE_TEMPLE,         SINGLE_SCENE_INFO(SCENE_GRAVEYARD),                  "Graveyard Outside Temple",     "Shadow Temple Entrance",       ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { ENTR_GRAVEYARD_OUTSIDE_DAMPES_HUT,      ENTR_GRAVEKEEPERS_HUT_0,               SINGLE_SCENE_INFO(SCENE_GRAVEKEEPERS_HUT),           "Dampe's Hut",                  "GY Dampe's Hut Entry",         ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR},
    { ENTR_GRAVEYARD_SHIELD_GRAVE_EXIT,       ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0,     SINGLE_SCENE_INFO(SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN), "Shield Grave",                 "GY Near-Hut Grave Entry",      ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { ENTR_GRAVEYARD_HEART_PIECE_GRAVE_EXIT,  ENTR_REDEAD_GRAVE_0,                   SINGLE_SCENE_INFO(SCENE_REDEAD_GRAVE),               "Heart Piece Grave",            "GY Near-Tomb Grave Entry",     ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { ENTR_GRAVEYARD_ROYAL_TOMB_EXIT,         ENTR_ROYAL_FAMILYS_TOMB_0,             SINGLE_SCENE_INFO(SCENE_ROYAL_FAMILYS_TOMB),         "Royal Family's Tomb",          "GY Royal Family's Tomb Entry", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO},
    { ENTR_GRAVEYARD_DAMPES_GRAVE_EXIT,       ENTR_WINDMILL_AND_DAMPES_GRAVE_GRAVE,  SINGLE_SCENE_INFO(SCENE_WINDMILL_AND_DAMPES_GRAVE),  "Dampe's Grave",                "GY Near-Ledge Grave Entry",    ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO,   "race"},
    { ENTR_GRAVEYARD_OUTSIDE_TEMPLE,          ENTR_SHADOW_TEMPLE_ENTRANCE,           SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),              "Shadow Temple Entrance",       "Graveyard Outside Temple",     ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON},
    { ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE,       ENTR_SHADOW_TEMPLE_BOSS_DOOR,          SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),              "Shadow Temple Boss Door",      "Bongo-Bongo",                  ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_SHADOW_TEMPLE_BOSS_DOOR,           ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE,      SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE_BOSS),         "Bongo-Bongo",                  "Shadow Temple Boss Door",      ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_GRAVEYARD_SHADOW_TEMPLE_BLUE_WARP, -1,                                    SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE_BOSS),         "Bongo-Bongo Blue Warp",        "Shadow Temple Blue Warp",      ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_ONE_WAY, "bw", 1},

    // Death Mountain Trail
    { ENTR_KAKARIKO_VILLAGE_OWL_DROP,                    -1,                                                SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT Owl Flight",                                "Kakariko Village Owl Drop",                     ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_KAKARIKO,              ENTRANCE_TYPE_ONE_WAY},
    { ENTR_GORON_CITY_UPPER_EXIT,                        ENTR_DEATH_MOUNTAIN_TRAIL_GC_EXIT,                 SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "Death Mountain Trail Middle Exit",              "Goron City Upper Exit",                         ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_KAKARIKO_VILLAGE_GUARD_GATE,                  ENTR_DEATH_MOUNTAIN_TRAIL_BOTTOM_EXIT,             SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "Death Mountain Trail Bottom Exit",              "Kakariko Guard Gate Exit",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_KAKARIKO,              ENTRANCE_TYPE_OVERWORLD},
    { ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT,             ENTR_DEATH_MOUNTAIN_TRAIL_SUMMIT_EXIT,             SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "Death Mountain Trail Top Exit",                 "Death Mountain Crater Upper Exit",              ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT,              ENTR_DEATH_MOUNTAIN_TRAIL_GREAT_FAIRY_EXIT,        SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT Great Fairy Entry",                         "DMT Great Fairy Fountain",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_DMT_STORMS_OFFSET),    ENTRANCE_GROTTO_EXIT(GROTTO_DMT_STORMS_OFFSET),    SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT Rock Circle Grotto Entry",                  "DMT Storms Grotto",                             ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_DMT_COW_OFFSET),       ENTRANCE_GROTTO_EXIT(GROTTO_DMT_COW_OFFSET),       SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT Boulder Grotto Entry",                      "DMT Cow Grotto",                                ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTR_DODONGOS_CAVERN_ENTRANCE,                     ENTR_DEATH_MOUNTAIN_TRAIL_OUTSIDE_DODONGOS_CAVERN, SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "Death Mountain Trail Outside Dodongo's Cavern", "Dodongo's Cavern Entrance",                     ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { ENTR_DEATH_MOUNTAIN_TRAIL_GREAT_FAIRY_EXIT,        ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT,              {{ SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, 0x00 }}, "DMT Great Fairy Fountain",                      "DMT Great Fairy Entry",                         ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_GROTTO_EXIT(GROTTO_DMT_STORMS_OFFSET),    ENTRANCE_GROTTO_LOAD(GROTTO_DMT_STORMS_OFFSET),    {{ SCENE_GROTTOS, 0x00 }},                     "DMT Storms Grotto",                             "DMT Rock Circle Grotto Entry",                  ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_DMT_COW_OFFSET),       ENTRANCE_GROTTO_LOAD(GROTTO_DMT_COW_OFFSET),       {{ SCENE_GROTTOS, 0x0D }},                     "DMT Cow Grotto",                                "DMT Boulder Grotto Entry",                      ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_GROTTO},
    { ENTR_DEATH_MOUNTAIN_TRAIL_OUTSIDE_DODONGOS_CAVERN, ENTR_DODONGOS_CAVERN_ENTRANCE,                     SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),      "Dodongo's Cavern Entrance",                     "Death Mountain Trail Outside Dodongo's Cavern", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc"},
    { ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE,                ENTR_DODONGOS_CAVERN_BOSS_DOOR,                    SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),      "Dodongo's Cavern Boss Door",                    "King Dodongo",                                  ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { ENTR_DODONGOS_CAVERN_BOSS_DOOR,                    ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE,                SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN_BOSS), "King Dodongo",                                  "Dodongo's Cavern Boss Door",                    ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { ENTR_DEATH_MOUNTAIN_TRAIL_DODONGO_BLUE_WARP,       -1,                                                SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN_BOSS), "King Dodongo Blue Warp",                        "Dodongo's Cavern Blue Warp",                    ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_ONE_WAY,   "dc,bw", 1},

    // Death Mountain Crater
    { ENTR_GORON_CITY_DARUNIA_ROOM_EXIT,                ENTR_DEATH_MOUNTAIN_CRATER_GC_EXIT,             SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "Death Mountain Crater Bridge Exit",    "Goron City Darunia's Room Backdoor",   ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_DEATH_MOUNTAIN_TRAIL_SUMMIT_EXIT,            ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT,          SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "Death Mountain Crater Upper Exit",     "Death Mountain Trail Top Exit",        ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMC,             ENTR_DEATH_MOUNTAIN_CRATER_GREAT_FAIRY_EXIT,    SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC Great Fairy Entry",                "DMC Great Fairy Fountain",             ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_DMC_UPPER_OFFSET),    ENTRANCE_GROTTO_EXIT(GROTTO_DMC_UPPER_OFFSET),  SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC Upper Grotto Entry",               "DMC Upper Grotto",                     ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_DMC_HAMMER_OFFSET),   ENTRANCE_GROTTO_EXIT(GROTTO_DMC_HAMMER_OFFSET), SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "DMC Hammer Grotto Entry",              "DMC Deku Scrub Grotto",                ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_FIRE_TEMPLE_ENTRANCE,                        ENTR_DEATH_MOUNTAIN_CRATER_OUTSIDE_TEMPLE,      SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_CRATER), "Death Mountain Crater Outside Temple", "Fire Temple Entrance",                 ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_DEATH_MOUNTAIN_CRATER_GREAT_FAIRY_EXIT,      ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMC,           {{ SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, 0x01 }},  "DMC Great Fairy Fountain",             "DMC Great Fairy Entry",                ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_GROTTO_EXIT(GROTTO_DMC_UPPER_OFFSET),    ENTRANCE_GROTTO_LOAD(GROTTO_DMC_UPPER_OFFSET),  {{ SCENE_GROTTOS, 0x00 }},                      "DMC Upper Grotto",                     "DMC Upper Grotto Entry",               ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_DMC_HAMMER_OFFSET),   ENTRANCE_GROTTO_LOAD(GROTTO_DMC_HAMMER_OFFSET), {{ SCENE_GROTTOS, 0x04 }},                      "DMC Deku Scrub Grotto",                "DMC Hammer Grotto Entry",              ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTR_DEATH_MOUNTAIN_CRATER_OUTSIDE_TEMPLE,        ENTR_FIRE_TEMPLE_ENTRANCE,                      SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),           "Fire Temple Entrance",                 "Death Mountain Crater Outside Temple", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON},
    { ENTR_FIRE_TEMPLE_BOSS_ENTRANCE,                   ENTR_FIRE_TEMPLE_BOSS_DOOR,                     SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),           "Fire Temple Boss Door",                "Volvagia",                             ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_FIRE_TEMPLE_BOSS_DOOR,                       ENTR_FIRE_TEMPLE_BOSS_ENTRANCE,                 SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE_BOSS),      "Volvagia",                             "Fire Temple Boss Door",                ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_DEATH_MOUNTAIN_CRATER_FIRE_TEMPLE_BLUE_WARP, -1,                                             SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE_BOSS),      "Volvagia Blue Warp",                   "Fire Temple Blue Warp",                ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_ONE_WAY,   "bw", 1},

    // Goron City
    { ENTR_DEATH_MOUNTAIN_TRAIL_GC_EXIT,              ENTR_GORON_CITY_UPPER_EXIT,                     SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City Upper Exit",              "Death Mountain Trail Middle Exit",  ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,  ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_DEATH_MOUNTAIN_CRATER_GC_EXIT,             ENTR_GORON_CITY_DARUNIA_ROOM_EXIT,              SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City Darunia's Room Backdoor", "Death Mountain Crater Bridge Exit", ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD, "gc"},
    { ENTR_LOST_WOODS_TUNNEL_SHORTCUT,                ENTR_GORON_CITY_TUNNEL_SHORTCUT,                SINGLE_SCENE_INFO(SCENE_GORON_CITY), "Goron City Tunnel Shortcut",         "Lost Woods Tunnel Shortcut",        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_LOST_WOODS,            ENTRANCE_TYPE_OVERWORLD, "gc,lw"},
    { ENTR_GORON_SHOP_0,                              ENTR_GORON_CITY_OUTSIDE_SHOP,                   SINGLE_SCENE_INFO(SCENE_GORON_CITY), "GC Shop Entry",                      "Goron Shop",                        ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR,  "gc", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_GORON_CITY_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_GORON_CITY_OFFSET), SINGLE_SCENE_INFO(SCENE_GORON_CITY), "GC Lava Grotto Entry",               "GC Deku Scrub Grotto",              ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO,    "gc,scrubs", 1},
    { ENTR_GORON_CITY_OUTSIDE_SHOP,                   ENTR_GORON_SHOP_0,                              SINGLE_SCENE_INFO(SCENE_GORON_SHOP), "Goron Shop",                         "GC Shop Entry",                     ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_INTERIOR,  "gc"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_GORON_CITY_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_GORON_CITY_OFFSET), {{ SCENE_GROTTOS, 0x04 }},           "GC Deku Scrub Grotto",               "GC Lava Grotto Entry",              ENTRANCE_GROUP_GORON_CITY, ENTRANCE_GROUP_GORON_CITY,            ENTRANCE_TYPE_GROTTO,    "gc,scrubs"},

    // Zora's River
    { ENTR_HYRULE_FIELD_RIVER_EXIT,                  ENTR_ZORAS_RIVER_WEST_EXIT,                    SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "Zora's River Lower Exit",          "Hyrule Field River Exit",        ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LOST_WOODS_UNDERWATER_SHORTCUT,           ENTR_ZORAS_RIVER_UNDERWATER_SHORTCUT,          SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "Zora's River Underwater Shortcut", "Lost Woods Underwater Shortcut", ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_LOST_WOODS,   ENTRANCE_TYPE_OVERWORLD, "lw"},
    { ENTR_ZORAS_DOMAIN_ENTRANCE,                    ENTR_ZORAS_RIVER_WATERFALL_EXIT,               SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "Zora's River Waterfall Exit",      "Zora's Domain Entrance",         ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD},
    { ENTRANCE_GROTTO_LOAD(GROTTO_ZR_STORMS_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_ZR_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR Rock Circle Grotto Entry",      "ZR Deku Scrub Grotto",           ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_ZR_FAIRY_OFFSET),  ENTRANCE_GROTTO_EXIT(GROTTO_ZR_FAIRY_OFFSET),  SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR Raised Boulder Grotto Entry",   "ZR Fairy Grotto",                ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_ZR_OPEN_OFFSET),   ENTRANCE_GROTTO_EXIT(GROTTO_ZR_OPEN_OFFSET),   SINGLE_SCENE_INFO(SCENE_ZORAS_RIVER), "ZR Raised Open Grotto Entry",      "ZR Open Grotto",                 ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_EXIT(GROTTO_ZR_STORMS_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_ZR_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x0A }},            "ZR Deku Scrub Grotto",             "ZR Rock Circle Grotto Entry",    ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_ZR_FAIRY_OFFSET),  ENTRANCE_GROTTO_LOAD(GROTTO_ZR_FAIRY_OFFSET),  {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},    "ZR Fairy Grotto",                  "ZR Raised Boulder Grotto Entry", ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_GROTTO_EXIT(GROTTO_ZR_OPEN_OFFSET),   ENTRANCE_GROTTO_LOAD(GROTTO_ZR_OPEN_OFFSET),   {{ SCENE_GROTTOS, 0x00 }},            "ZR Open Grotto",                   "ZR Raised Open Grotto Entry",    ENTRANCE_GROUP_ZORAS_RIVER, ENTRANCE_GROUP_ZORAS_RIVER,  ENTRANCE_TYPE_GROTTO,    "chest"},

    // Zora's Domain
    { ENTR_ZORAS_RIVER_WATERFALL_EXIT,               ENTR_ZORAS_DOMAIN_ENTRANCE,                    SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain Entrance",            "Zora's River Waterfall Exit",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_RIVER,    ENTRANCE_TYPE_OVERWORLD},
    { ENTR_LAKE_HYLIA_UNDERWATER_SHORTCUT,           ENTR_ZORAS_DOMAIN_UNDERWATER_SHORTCUT,         SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain Underwater Shortcut", "Lake Hylia Underwater Shortcut", ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_LAKE_HYLIA,     ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_ZORAS_FOUNTAIN_TUNNEL_EXIT,               ENTR_ZORAS_DOMAIN_KING_ZORA_EXIT,              SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "Zora's Domain Behind King Zora",    "Zora's Fountain Tunnel Exit",    ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_OVERWORLD},
    { ENTR_ZORA_SHOP_0,                              ENTR_ZORAS_DOMAIN_OUTSIDE_SHOP,                SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "ZD Shop Entry",                     "Zora Shop",                      ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_ZD_STORMS_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_ZD_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_ZORAS_DOMAIN), "ZD Island Grotto Entry",            "ZD Fairy Grotto",                ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO,    "fairy", 1},
    { ENTR_ZORAS_DOMAIN_OUTSIDE_SHOP,                ENTR_ZORA_SHOP_0,                              SINGLE_SCENE_INFO(SCENE_ZORA_SHOP),    "Zora Shop",                         "ZD Shop Entry",                  ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_GROTTO_EXIT(GROTTO_ZD_STORMS_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_ZD_STORMS_OFFSET), {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},     "ZD Fairy Grotto",                   "ZD Island Grotto Entry",         ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_GROTTO,    "fairy"},

    // Zora's Fountain
    { ENTR_ZORAS_DOMAIN_KING_ZORA_EXIT,             ENTR_ZORAS_FOUNTAIN_TUNNEL_EXIT,              SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "Zora's Fountain Tunnel Exit",        "Zora's Domain Behind King Zora",     ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_DOMAIN,   ENTRANCE_TYPE_OVERWORLD},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_FARORES_ZF, ENTR_ZORAS_FOUNTAIN_OUTSIDE_GREAT_FAIRY,      SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "ZF Great Fairy Entry",               "ZF Great Fairy Fountain",            ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR, "", 1},
    { ENTR_JABU_JABU_ENTRANCE,                      ENTR_ZORAS_FOUNTAIN_OUTSIDE_JABU_JABU,        SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "Zora's Fountain Outside Jabu Jabu",  "Jabu Jabu's Belly Entrance",         ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { ENTR_ICE_CAVERN_ENTRANCE,                     ENTR_ZORAS_FOUNTAIN_OUTSIDE_ICE_CAVERN,       SINGLE_SCENE_INFO(SCENE_ZORAS_FOUNTAIN),        "Zora's Fountain Outside Ice Cavern", "Ice Cavern Entrance",                ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON,  "", 1},
    { ENTR_ZORAS_FOUNTAIN_OUTSIDE_GREAT_FAIRY,      ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_FARORES_ZF, {{ SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, 0x00 }}, "ZF Great Fairy Fountain",            "ZF Great Fairy Entry",               ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_INTERIOR},
    { ENTR_ZORAS_FOUNTAIN_OUTSIDE_JABU_JABU,        ENTR_JABU_JABU_ENTRANCE,                      SINGLE_SCENE_INFO(SCENE_JABU_JABU),             "Jabu Jabu's Belly Entrance",         "Zora's Fountain Outside Jabu Jabu",  ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},
    { ENTR_JABU_JABU_BOSS_ENTRANCE,                 ENTR_JABU_JABU_BOSS_DOOR,                     SINGLE_SCENE_INFO(SCENE_JABU_JABU),             "Jabu Jabu's Belly Boss Door",        "Barinade",                           ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_JABU_JABU_BOSS_DOOR,                     ENTR_JABU_JABU_BOSS_ENTRANCE,                 SINGLE_SCENE_INFO(SCENE_JABU_JABU_BOSS),        "Barinade",                           "Jabu Jabu's Belly Boss Door",        ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON, "", 1},
    { ENTR_ZORAS_FOUNTAIN_JABU_JABU_BLUE_WARP,      -1,                                           SINGLE_SCENE_INFO(SCENE_JABU_JABU_BOSS),        "Barinade Blue Warp",                 "Jabu Jabu's Belly Blue Warp",        ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_ONE_WAY, "bw", 1},
    { ENTR_ZORAS_FOUNTAIN_OUTSIDE_ICE_CAVERN,       ENTR_ICE_CAVERN_ENTRANCE,                     SINGLE_SCENE_INFO(SCENE_ICE_CAVERN),            "Ice Cavern Entrance",                "Zora's Fountain Outside Ice Cavern", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DUNGEON},

    // Hyrule Field
    { ENTR_LOST_WOODS_BRIDGE_WEST_EXIT,                    ENTR_HYRULE_FIELD_WOODED_EXIT,                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field Wooded Exit",            "Lost Woods Bridge West Exit",         ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LOST_WOODS,    ENTRANCE_TYPE_OVERWORLD, "hf,lw"},
    { ENTR_MARKET_ENTRANCE_NEAR_GUARD_EXIT,                ENTR_HYRULE_FIELD_ON_BRIDGE_SPAWN,                   SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field Drawbridge Exit",        "Market Entrance South Exit",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LON_LON_RANCH_ENTRANCE,                         ENTR_HYRULE_FIELD_CENTER_EXIT,                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field Center Exit",            "Lon Lon Ranch Entrance",              ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_OVERWORLD, "hf,llr"},
    { ENTR_KAKARIKO_VILLAGE_FRONT_GATE,                    ENTR_HYRULE_FIELD_STAIRS_EXIT,                       SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field Stairs Exit",            "Kakariko Front Gate",                 ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_KAKARIKO,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_ZORAS_RIVER_WEST_EXIT,                          ENTR_HYRULE_FIELD_RIVER_EXIT,                        SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field River Exit",             "Zora's River Lower Exit",             ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_ZORAS_RIVER,   ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LAKE_HYLIA_NORTH_EXIT,                          ENTR_HYRULE_FIELD_FENCE_EXIT,                        SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field Fence Exit",             "Lake Hylia North Exit",               ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_LAKE_HYLIA,    ENTRANCE_TYPE_OVERWORLD, "hf,lh"},
    { ENTR_GERUDO_VALLEY_EAST_EXIT,                        ENTR_HYRULE_FIELD_ROCKY_PATH,                        SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "Hyrule Field Rocky Path",             "Gerudo Valley East Exit",             ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_NEAR_MARKET_OFFSET),  ENTRANCE_GROTTO_EXIT(GROTTO_HF_NEAR_MARKET_OFFSET),  SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF Near Market Boulder Grotto Entry", "HF Near Market Boulder Grotto",       ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_NEAR_KAK_OFFSET),     ENTRANCE_GROTTO_EXIT(GROTTO_HF_NEAR_KAK_OFFSET),     SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF Stone Bridge Tree Grotto Entry",   "HF Stone Bridge Tree Grotto",         ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "spider", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_TEKTITE_OFFSET),      ENTRANCE_GROTTO_EXIT(GROTTO_HF_TEKTITE_OFFSET),      SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF Northwest Tree Grotto Entry",      "HF Tektite Grotto",                   ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "water", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_FAIRY_OFFSET),        ENTRANCE_GROTTO_EXIT(GROTTO_HF_FAIRY_OFFSET),        SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF Northwest Boulder Grotto Entry",   "HF Fairy Grotto",                     ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_COW_OFFSET),          ENTRANCE_GROTTO_EXIT(GROTTO_HF_COW_OFFSET),          SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF West Rock Circle Grotto Entry",    "HF Cow Grotto",                       ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "webbed", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_OPEN_OFFSET),         ENTRANCE_GROTTO_EXIT(GROTTO_HF_OPEN_OFFSET),         SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF South Open Grotto Entry",          "HF Open Grotto",                      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_INSIDE_FENCE_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_HF_INSIDE_FENCE_OFFSET), SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF Fenced Grotto Entry",              "HF Fenced Deku Scrub Grotto",         ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HF_SOUTHEAST_OFFSET),    ENTRANCE_GROTTO_EXIT(GROTTO_HF_SOUTHEAST_OFFSET),    SINGLE_SCENE_INFO(SCENE_HYRULE_FIELD), "HF Southeast Boulder Grotto Entry",   "HF Southeast Grotto",                 ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest", 1},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_NEAR_MARKET_OFFSET),  ENTRANCE_GROTTO_LOAD(GROTTO_HF_NEAR_MARKET_OFFSET),  {{ SCENE_GROTTOS, 0x00 }},             "HF Near Market Boulder Grotto",       "HF Near Market Boulder Grotto Entry", ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_NEAR_KAK_OFFSET),     ENTRANCE_GROTTO_LOAD(GROTTO_HF_NEAR_KAK_OFFSET),     {{ SCENE_GROTTOS, 0x01 }},             "HF Stone Bridge Tree Grotto",         "HF Stone Bridge Tree Grotto Entry",   ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "spider"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_TEKTITE_OFFSET),      ENTRANCE_GROTTO_LOAD(GROTTO_HF_TEKTITE_OFFSET),      {{ SCENE_GROTTOS, 0x0B }},             "HF Tektite Grotto",                   "HF Northwest Tree Grotto Entry",      ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "water"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_FAIRY_OFFSET),        ENTRANCE_GROTTO_LOAD(GROTTO_HF_FAIRY_OFFSET),        {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},     "HF Fairy Grotto",                     "HF Northwest Boulder Grotto Entry",   ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_COW_OFFSET),          ENTRANCE_GROTTO_LOAD(GROTTO_HF_COW_OFFSET),          {{ SCENE_GROTTOS, 0x05 }},             "HF Cow Grotto",                       "HF West Rock Circle Grotto Entry",    ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "webbed"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_OPEN_OFFSET),         ENTRANCE_GROTTO_LOAD(GROTTO_HF_OPEN_OFFSET),         {{ SCENE_GROTTOS, 0x00 }},             "HF Open Grotto",                      "HF South Open Grotto Entry",          ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_INSIDE_FENCE_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_HF_INSIDE_FENCE_OFFSET), {{ SCENE_GROTTOS, 0x02 }},             "HF Fenced Deku Scrub Grotto",         "HF Fenced Grotto Entry",              ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HF_SOUTHEAST_OFFSET),    ENTRANCE_GROTTO_LOAD(GROTTO_HF_SOUTHEAST_OFFSET),    {{ SCENE_GROTTOS, 0x00 }},             "HF Southeast Grotto",                 "HF Southeast Boulder Grotto Entry",   ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_GROTTO,    "chest"},

    // Lon Lon Ranch
    { ENTR_HYRULE_FIELD_CENTER_EXIT,           ENTR_LON_LON_RANCH_ENTRANCE,             SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "Lon Lon Ranch Entrance",  "Hyrule Field Center Exit", ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_LON_LON_BUILDINGS_TALONS_HOUSE,     ENTR_LON_LON_RANCH_OUTSIDE_TALONS_HOUSE, SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "LLR Talon's House Entry", "Talon's House",            ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "llr", 1},
    { ENTR_STABLE_0,                           ENTR_LON_LON_RANCH_OUTSIDE_STABLES,      SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "LLR Stables Entry",       "LLR Stables",              ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { ENTR_LON_LON_BUILDINGS_TOWER,            ENTR_LON_LON_RANCH_OUTSIDE_TOWER,        SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "LLR Tower Entry",         "LLR Tower",                ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_LLR_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_LLR_OFFSET), SINGLE_SCENE_INFO(SCENE_LON_LON_RANCH), "LLR Grotto Entry",        "LLR Deku Scrub Grotto",    ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_LON_LON_RANCH_OUTSIDE_TALONS_HOUSE, ENTR_LON_LON_BUILDINGS_TALONS_HOUSE,     {{ SCENE_LON_LON_BUILDINGS, 0x00 }},    "Talon's House",           "LLR Talon's House Entry",  ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "llr"},
    { ENTR_LON_LON_RANCH_OUTSIDE_STABLES,      ENTR_STABLE_0,                           SINGLE_SCENE_INFO(SCENE_STABLE),        "LLR Stables",             "LLR Stables Entry",        ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow"},
    { ENTR_LON_LON_RANCH_OUTSIDE_TOWER,        ENTR_LON_LON_BUILDINGS_TOWER,            {{ SCENE_LON_LON_BUILDINGS, 0x01 }},    "LLR Tower",               "LLR Tower Entry",          ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR,  "cow"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_LLR_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_LLR_OFFSET), {{ SCENE_GROTTOS, 0x04 }},              "LLR Deku Scrub Grotto",   "LLR Grotto Entry",         ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO,    "scrubs"},

    // Lake Hylia
    { ENTR_HYRULE_FIELD_OWL_DROP,             -1,                                     SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "LH Owl Flight",                  "Hyrule Field Owl Drop",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_HYRULE_FIELD_FENCE_EXIT,           ENTR_LAKE_HYLIA_NORTH_EXIT,             SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia North Exit",          "Hyrule Field Fence Exit",           ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_ZORAS_DOMAIN_UNDERWATER_SHORTCUT,  ENTR_LAKE_HYLIA_UNDERWATER_SHORTCUT,    SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia Underwater Shortcut", "Zora's Domain Underwater Shortcut", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_LAKESIDE_LABORATORY_0,             ENTR_LAKE_HYLIA_OUTSIDE_LAB,            SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "LH Lab Entry",                   "LH Lab",                            ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh", 1},
    { ENTR_FISHING_POND_0,                    ENTR_LAKE_HYLIA_OUTSIDE_FISHING_POND,   SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "LH Fishing Pond Entry",          "Fishing Pond",                      ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_LH_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_LH_OFFSET), SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "LH Grave Grotto Entry",          "LH Deku Scrub Grotto",              ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_WATER_TEMPLE_ENTRANCE,             ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE,         SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),          "Lake Hylia Outside Temple",      "Water Temple Entrance",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},
    { ENTR_LAKE_HYLIA_OUTSIDE_LAB,            ENTR_LAKESIDE_LABORATORY_0,             SINGLE_SCENE_INFO(SCENE_LAKESIDE_LABORATORY), "LH Lab",                         "LH Lab Entry",                      ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh"},
    { ENTR_LAKE_HYLIA_OUTSIDE_FISHING_POND,   ENTR_FISHING_POND_0,                    SINGLE_SCENE_INFO(SCENE_FISHING_POND),        "Fishing Pond",                   "LH Fishing Pond Entry",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_INTERIOR,  "lh"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_LH_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_LH_OFFSET), {{ SCENE_GROTTOS, 0x04 }},                    "LH Deku Scrub Grotto",           "LH Grave Grotto Entry",             ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_GROTTO,    "lh,scrubs"},
    { ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE,         ENTR_WATER_TEMPLE_ENTRANCE,             SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),        "Water Temple Entrance",          "Lake Hylia Outside Temple",         ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh"},
    { ENTR_WATER_TEMPLE_BOSS_ENTRANCE,        ENTR_WATER_TEMPLE_BOSS_DOOR,            SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),        "Water Temple Boss Door",         "Morpha",                            ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},
    { ENTR_WATER_TEMPLE_BOSS_DOOR,            ENTR_WATER_TEMPLE_BOSS_ENTRANCE,        SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE_BOSS),   "Morpha",                         "Water Temple Boss Door",            ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_DUNGEON,   "lh", 1},
    { ENTR_LAKE_HYLIA_WATER_TEMPLE_BLUE_WARP, -1,                                     SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE_BOSS),   "Morpha Blue Warp",               "Water Temple Blue Warp",            ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA,   ENTRANCE_TYPE_ONE_WAY,   "lh,bw", 1},

    // Gerudo Area
    { ENTR_HYRULE_FIELD_ROCKY_PATH,                         ENTR_GERUDO_VALLEY_EAST_EXIT,                         SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "Gerudo Valley East Exit",         "Hyrule Field Rocky Path",         ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_HYRULE_FIELD,      ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_GERUDOS_FORTRESS_EAST_EXIT,                      ENTR_GERUDO_VALLEY_WEST_EXIT,                         SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "Gerudo Valley West Exit",         "Gerudo Fortress East Exit",       ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_OVERWORLD, ""},
    { ENTR_LAKE_HYLIA_RIVER_EXIT,                           -1,                                                   SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "Gerudo Valley River Exit",        "Lake Hylia River Exit",           ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_LAKE_HYLIA,        ENTRANCE_TYPE_OVERWORLD, "lh"},
    { ENTR_CARPENTERS_TENT_0,                               ENTR_GERUDO_VALLEY_OUTSIDE_TENT,                      SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV Carpenters' Tent Entry",       "Carpenters' Tent",                ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_GV_OCTOROK_OFFSET),       ENTRANCE_GROTTO_EXIT(GROTTO_GV_OCTOROK_OFFSET),       SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV Silver Rock Grotto Entry",     "GV Octorok Grotto",               ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_GV_STORMS_OFFSET),        ENTRANCE_GROTTO_EXIT(GROTTO_GV_STORMS_OFFSET),        SINGLE_SCENE_INFO(SCENE_GERUDO_VALLEY),          "GV Behind Tent Grotto Entry",     "GV Deku Scrub Grotto",            ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "scrubs", 1},
    { ENTR_GERUDO_VALLEY_OUTSIDE_TENT,                      ENTR_CARPENTERS_TENT_0,                               SINGLE_SCENE_INFO(SCENE_CARPENTERS_TENT),        "Carpenters' Tent",                "GV Carpenters' Tent Entry",       ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_GROTTO_EXIT(GROTTO_GV_OCTOROK_OFFSET),       ENTRANCE_GROTTO_LOAD(GROTTO_GV_OCTOROK_OFFSET),       {{ SCENE_GROTTOS, 0x06 }},                       "GV Octorok Grotto",               "GV Silver Rock Grotto Entry",     ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO},
    { ENTRANCE_GROTTO_EXIT(GROTTO_GV_STORMS_OFFSET),        ENTRANCE_GROTTO_LOAD(GROTTO_GV_STORMS_OFFSET),        {{ SCENE_GROTTOS, 0x0A }},                       "GV Deku Scrub Grotto",            "GV Behind Tent Grotto Entry",     ENTRANCE_GROUP_GERUDO_VALLEY,   ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_GROTTO,    "scrubs"},
    { ENTR_GERUDO_VALLEY_WEST_EXIT,                         ENTR_GERUDOS_FORTRESS_EAST_EXIT,                      SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "Gerudo Fortress East Exit",       "Gerudo Valley West Exit",         ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_VALLEY,     ENTRANCE_TYPE_OVERWORLD, ""},
    { ENTR_HAUNTED_WASTELAND_EAST_EXIT,                     ENTR_GERUDOS_FORTRESS_GATE_EXIT,                      SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "Gerudo Fortress Gate Exit",       "Haunted Wasteland East Exit",     ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, ""},
    { ENTRANCE_GROTTO_LOAD(GROTTO_GF_STORMS_OFFSET),        ENTRANCE_GROTTO_EXIT(GROTTO_GF_STORMS_OFFSET),        SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Storms Grotto Entry",          "GF Fairy Grotto",                 ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_GROTTO,    "", 1},
    { ENTR_GERUDO_TRAINING_GROUND_ENTRANCE,                 ENTR_GERUDOS_FORTRESS_OUTSIDE_GERUDO_TRAINING_GROUND, SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Outside Training Ground",      "Gerudo Training Ground Entrance", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_DUNGEON,   "gtg", 1},
    { ENTRANCE_GROTTO_EXIT(GROTTO_GF_STORMS_OFFSET),        ENTRANCE_GROTTO_LOAD(GROTTO_GF_STORMS_OFFSET),        {{ SCENE_FAIRYS_FOUNTAIN, 0x00 }},               "GF Fairy Grotto",                 "GF Storms Grotto Entry",          ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_GROTTO,    ""},
    { ENTR_GERUDOS_FORTRESS_OUTSIDE_GERUDO_TRAINING_GROUND, ENTR_GERUDO_TRAINING_GROUND_ENTRANCE,                 SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "Gerudo Training Ground Entrance", "GF Outside Training Ground",      ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_DUNGEON,   "gtg"},
    { ENTR_GERUDOS_FORTRESS_1,                              ENTR_THIEVES_HIDEOUT_0,                               {{ SCENE_THIEVES_HIDEOUT, 2 }},                  "TH 1 Torch Cell Turn",            "GF Outskirts",                    ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_2,                              ENTR_THIEVES_HIDEOUT_1,                               {{ SCENE_THIEVES_HIDEOUT, 2 }},                  "TH 1 Torch Cell",                 "GF Near Grotto North",            ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_3,                              ENTR_THIEVES_HIDEOUT_2,                               {{ SCENE_THIEVES_HIDEOUT, 3 }},                  "TH Kitchen Corridor Lower",       "GF Near Grotto East",             ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_4,                              ENTR_THIEVES_HIDEOUT_3,                               {{ SCENE_THIEVES_HIDEOUT, 3 }},                  "TH Kitchen Corridor Upper",       "GF Above GTG",                    ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_5,                              ENTR_THIEVES_HIDEOUT_4,                               {{ SCENE_THIEVES_HIDEOUT, 4 }},                  "TH Steep Slope Cell",             "GF Near Grotto",                  ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_6,                              ENTR_THIEVES_HIDEOUT_5,                               {{ SCENE_THIEVES_HIDEOUT, 4 }},                  "TH Steep Slope Cell Two Ramps",   "GF Bottom of Lower Vines",        ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_7,                              ENTR_THIEVES_HIDEOUT_6,                               {{ SCENE_THIEVES_HIDEOUT, 5 }},                  "TH Double Cell Lower",            "GF Above GTG Directly",           ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_8,                              ENTR_THIEVES_HIDEOUT_7,                               {{ SCENE_THIEVES_HIDEOUT, 5 }},                  "TH Double Cell Upper",            "GF Top of Lower Vines Across",    ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_9,                              ENTR_THIEVES_HIDEOUT_8,                               {{ SCENE_THIEVES_HIDEOUT, 3 }},                  "TH Kitchen By Corridor",          "GF Top of Lower Vines Near",      ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_10,                             ENTR_THIEVES_HIDEOUT_9,                               {{ SCENE_THIEVES_HIDEOUT, 3 }},                  "TH Kitchen Opposite Corridor",    "GF Near GS",                      ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_11,                             ENTR_THIEVES_HIDEOUT_10,                              {{ SCENE_THIEVES_HIDEOUT, 0 }},                  "TH Break Room",                   "GF Below Chest",                  ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_12,                             ENTR_THIEVES_HIDEOUT_11,                              {{ SCENE_THIEVES_HIDEOUT, 0 }},                  "TH Break Room Corridor",          "GF Above Jail",                   ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_13,                             ENTR_THIEVES_HIDEOUT_12,                              {{ SCENE_THIEVES_HIDEOUT, 1 }},                  "TH Dead End Cell",                "GF Below GS",                     ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_0,                               ENTR_GERUDOS_FORTRESS_1,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Outskirts",                    "TH 1 Torch Cell Turn",            ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_1,                               ENTR_GERUDOS_FORTRESS_2,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Near Grotto North",            "TH 1 Torch Cell",                 ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_2,                               ENTR_GERUDOS_FORTRESS_3,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Near Grotto East",             "TH Kitchen Corridor Lower",       ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_3,                               ENTR_GERUDOS_FORTRESS_4,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Above GTG",                    "TH Kitchen Corridor Upper",       ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_4,                               ENTR_GERUDOS_FORTRESS_5,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Near Grotto",                  "TH Steep Slope Cell",             ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_5,                               ENTR_GERUDOS_FORTRESS_6,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Bottom of Lower Vines",        "TH Steep Slope Cell Two Ramps",   ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_6,                               ENTR_GERUDOS_FORTRESS_7,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Above GTG Directly",           "TH Double Cell Lower",            ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_7,                               ENTR_GERUDOS_FORTRESS_8,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Top of Lower Vines Across",    "TH Double Cell Upper",            ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_8,                               ENTR_GERUDOS_FORTRESS_9,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Top of Lower Vines Near",      "TH Kitchen By Corridor",          ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_9,                               ENTR_GERUDOS_FORTRESS_10,                             SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Near GS",                      "TH Kitchen Opposite Corridor",    ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_10,                              ENTR_GERUDOS_FORTRESS_11,                             SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Below Chest",                  "TH Break Room",                   ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_11,                              ENTR_GERUDOS_FORTRESS_12,                             SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Above Jail",                   "TH Break Room Corridor",          ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_12,                              ENTR_GERUDOS_FORTRESS_13,                             SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Below GS",                     "TH Dead End Cell",                ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},

    // The Wasteland
    { ENTR_GERUDOS_FORTRESS_GATE_EXIT,                   ENTR_HAUNTED_WASTELAND_EAST_EXIT,                  SINGLE_SCENE_INFO(SCENE_HAUNTED_WASTELAND),     "Haunted Wasteland East Exit",   "Gerudo Fortress Gate Exit",     ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_OVERWORLD, "hw,gf"},
    { ENTR_DESERT_COLOSSUS_EAST_EXIT,                    ENTR_HAUNTED_WASTELAND_WEST_EXIT,                  SINGLE_SCENE_INFO(SCENE_HAUNTED_WASTELAND),     "Haunted Wasteland West Exit",   "Desert Colossus East Exit",     ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { ENTR_HAUNTED_WASTELAND_WEST_EXIT,                  ENTR_DESERT_COLOSSUS_EAST_EXIT,                    SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Desert Colossus East Exit",     "Haunted Wasteland West Exit",   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD, "dc,hw"},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_NAYRUS_COLOSSUS, ENTR_DESERT_COLOSSUS_GREAT_FAIRY_EXIT,             SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Colossus Great Fairy Entry",    "Colossus Great Fairy Fountain", ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_INTERIOR,  "dc", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_COLOSSUS_OFFSET),      ENTRANCE_GROTTO_EXIT(GROTTO_COLOSSUS_OFFSET),      SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Colossus Grotto Entry",         "Colossus Deku Scrub Grotto",    ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_GROTTO,    "dc,scrubs", 1},
    { ENTR_SPIRIT_TEMPLE_ENTRANCE,                       ENTR_DESERT_COLOSSUS_OUTSIDE_TEMPLE,               SINGLE_SCENE_INFO(SCENE_DESERT_COLOSSUS),       "Colossus Outside Temple",       "Spirit Temple Entrance",        ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_DUNGEON,   "dc", 1},
    { ENTR_DESERT_COLOSSUS_GREAT_FAIRY_EXIT,             ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_NAYRUS_COLOSSUS, {{ SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, 0x02 }}, "Colossus Great Fairy Fountain", "Colossus Great Fairy Entry",    ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_INTERIOR,  "dc"},
    { ENTRANCE_GROTTO_EXIT(GROTTO_COLOSSUS_OFFSET),      ENTRANCE_GROTTO_LOAD(GROTTO_COLOSSUS_OFFSET),      {{ SCENE_GROTTOS, 0x0A }},                      "Colossus Deku Scrub Grotto",    "Colossus Grotto Entry",         ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_GROTTO,    "dc,scrubs"},
    { ENTR_DESERT_COLOSSUS_OUTSIDE_TEMPLE,               ENTR_SPIRIT_TEMPLE_ENTRANCE,                       SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),         "Spirit Temple Entrance",        "Colossus Outside Temple",       ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_DUNGEON,   "dc"},
    { ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE,                  ENTR_SPIRIT_TEMPLE_BOSS_DOOR,                      SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),         "Spirit Temple Boss Door",       "Twinrova",                      ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_SPIRIT_TEMPLE_BOSS_DOOR,                      ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE,                  SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE_BOSS),    "Twinrova",                      "Spirit Temple Boss Door",       ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_DUNGEON,   "", 1},
    { ENTR_DESERT_COLOSSUS_SPIRIT_TEMPLE_BLUE_WARP,      -1,                                                SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE_BOSS),    "Twinrova Blue Warp",            "Spirit Temple Blue Warp",       ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_GROUP_DESERT_COLOSSUS,   ENTRANCE_TYPE_ONE_WAY,   "bw", 1},

    // Market
    { ENTR_HYRULE_FIELD_ON_BRIDGE_SPAWN,                  ENTR_MARKET_ENTRANCE_NEAR_GUARD_EXIT,               {SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_DAY), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_RUINS)},                                                    "Market Entrance South Exit",       "Hyrule Field Drawbridge Exit",     ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_FIELD,  ENTRANCE_TYPE_OVERWORLD, "hf"},
    { ENTR_MARKET_SOUTH_EXIT,                             ENTR_MARKET_ENTRANCE_NORTH_EXIT,                    {SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_DAY), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_RUINS)},                                                    "Market Entrance North Exit",       "Market South Exit",                ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { ENTR_MARKET_GUARD_HOUSE_0,                          ENTR_MARKET_ENTRANCE_OUTSIDE_GUARD_HOUSE,           {SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_DAY), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_ENTRANCE_RUINS)},                                                    "MK Entrance Guard House Entry",    "Guard House",                      ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "pots,poe", 1},
    { ENTR_MARKET_ENTRANCE_NORTH_EXIT,                    ENTR_MARKET_SOUTH_EXIT,                             {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market South Exit",                "Market Entrance North Exit",       ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { ENTR_CASTLE_GROUNDS_SOUTH_EXIT,                     ENTR_MARKET_DAY_CASTLE_EXIT,                        {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market Castle Exit",               "Castle Grounds South Exit",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_GOSSIP_STONE_EXIT, ENTR_MARKET_DAY_TEMPLE_EXIT,                        {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "Market Temple Exit",               "ToT Courtyard Gossip Stones Exit", ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD},
    { ENTR_SHOOTING_GALLERY_1,                            ENTR_MARKET_DAY_OUTSIDE_SHOOTING_GALLERY,           {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Shooting Gallery Entry",        "MK Shooting Gallery",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "child", 1},
    { ENTR_BOMBCHU_BOWLING_ALLEY_0,                       ENTR_MARKET_DAY_OUTSIDE_BOMBCHU_BOWLING,            {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Bombchu Bowling Entry",         "Bombchu Bowling",                  ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_TREASURE_BOX_SHOP_0,                           ENTR_MARKET_DAY_OUTSIDE_TREASURE_BOX_SHOP,          {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Treasure Chest Game Entry",     "Treasure Chest Game",              ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BACK_ALLEY_MAN_IN_GREEN_HOUSE,                 ENTR_BACK_ALLEY_DAY_OUTSIDE_MAN_IN_GREEN_HOUSE,     {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Man-in-Green House Entry",      "Man-in-Green's House",             ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_HAPPY_MASK_SHOP_0,                             ENTR_MARKET_DAY_OUTSIDE_HAPPY_MASK_SHOP,            {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Mask Shop Entry",               "Mask Shop",                        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BAZAAR_1,                                      ENTR_MARKET_DAY_OUTSIDE_BAZAAR,                     {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Bazaar Entry",                  "MK Bazaar",                        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "shop", 1},
    { ENTR_POTION_SHOP_MARKET_0,                          ENTR_MARKET_DAY_OUTSIDE_POTION_SHOP,                {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Potion Shop Entry",             "MK Potion Shop",                   ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_BOMBCHU_SHOP_1,                                ENTR_BACK_ALLEY_DAY_OUTSIDE_BOMBCHU_SHOP,           {SCENE_NO_SPAWN(SCENE_MARKET_DAY), SCENE_NO_SPAWN(SCENE_MARKET_NIGHT), SCENE_NO_SPAWN(SCENE_MARKET_RUINS), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_DAY), SCENE_NO_SPAWN(SCENE_BACK_ALLEY_NIGHT)}, "MK Bombchu Shop Entry",            "Bombchu Shop",                     ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTR_MARKET_ENTRANCE_OUTSIDE_GUARD_HOUSE,           ENTR_MARKET_GUARD_HOUSE_0,                          {{ SCENE_MARKET_GUARD_HOUSE }},                                                                                                                                                           "Guard House",                      "MK Entrance Guard House Entry",    ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "pots,poe"},
    { ENTR_MARKET_DAY_OUTSIDE_SHOOTING_GALLERY,           ENTR_SHOOTING_GALLERY_1,                            {{ SCENE_SHOOTING_GALLERY, 0x01 }},                                                                                                                                                       "MK Shooting Gallery",              "MK Shooting Gallery Entry",        ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_OUTSIDE_BOMBCHU_BOWLING,            ENTR_BOMBCHU_BOWLING_ALLEY_0,                       SINGLE_SCENE_INFO(SCENE_BOMBCHU_BOWLING_ALLEY),                                                                                                                                           "Bombchu Bowling",                  "MK Bombchu Bowling Entry",         ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_OUTSIDE_TREASURE_BOX_SHOP,          ENTR_TREASURE_BOX_SHOP_0,                           SINGLE_SCENE_INFO(SCENE_TREASURE_BOX_SHOP),                                                                                                                                               "Treasure Chest Game",              "MK Treasure Chest Game Entry",     ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_BACK_ALLEY_DAY_OUTSIDE_MAN_IN_GREEN_HOUSE,     ENTR_BACK_ALLEY_MAN_IN_GREEN_HOUSE,                 SINGLE_SCENE_INFO(SCENE_BACK_ALLEY_HOUSE),                                                                                                                                                "Man-in-Green's House",             "MK Man-in-Green House Entry",      ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_OUTSIDE_HAPPY_MASK_SHOP,            ENTR_HAPPY_MASK_SHOP_0,                             SINGLE_SCENE_INFO(SCENE_HAPPY_MASK_SHOP),                                                                                                                                                 "Mask Shop",                        "MK Mask Shop Entry",               ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_OUTSIDE_BAZAAR,                     ENTR_BAZAAR_1,                                      {{ SCENE_BAZAAR, 0x01 }},                                                                                                                                                                 "MK Bazaar",                        "MK Bazaar Entry",                  ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "shop"},
    { ENTR_MARKET_DAY_OUTSIDE_POTION_SHOP,                ENTR_POTION_SHOP_MARKET_0,                          SINGLE_SCENE_INFO(SCENE_POTION_SHOP_MARKET),                                                                                                                                              "MK Potion Shop",                   "MK Potion Shop Entry",             ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_BACK_ALLEY_DAY_OUTSIDE_BOMBCHU_SHOP,           ENTR_BOMBCHU_SHOP_1,                                SINGLE_SCENE_INFO(SCENE_BOMBCHU_SHOP),                                                                                                                                                    "Bombchu Shop",                     "MK Bombchu Shop Entry",            ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR},
    { ENTR_MARKET_DAY_TEMPLE_EXIT,                        ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_GOSSIP_STONE_EXIT, {SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS)},                            "ToT Courtyard Gossip Stones Exit", "Market Temple Exit",               ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "tot"},
    { ENTR_TEMPLE_OF_TIME_ENTRANCE,                       ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_OUTSIDE_TEMPLE,    {SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT), SCENE_NO_SPAWN(SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS)},                            "ToT Courtyard Temple Entry",       "Temple of Time Entrance",          ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "tot", 1},
    { ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_OUTSIDE_TEMPLE,    ENTR_TEMPLE_OF_TIME_ENTRANCE,                       SINGLE_SCENE_INFO(SCENE_TEMPLE_OF_TIME),                                                                                                                                                  "Temple of Time Entrance",          "ToT Courtyard Temple Entry",       ENTRANCE_GROUP_MARKET, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_INTERIOR,  "tot"},

    // Hyrule Castle
    { ENTR_MARKET_DAY_CASTLE_EXIT,                   ENTR_CASTLE_GROUNDS_SOUTH_EXIT,                {SCENE_NO_SPAWN(SCENE_HYRULE_CASTLE), SCENE_NO_SPAWN(SCENE_OUTSIDE_GANONS_CASTLE)}, "Castle Grounds South Exit",         "Market Castle Exit",                ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_MARKET,        ENTRANCE_TYPE_OVERWORLD, "outside ganon's castle"},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_DINS_HC,     ENTR_CASTLE_GROUNDS_GREAT_FAIRY_EXIT,          SINGLE_SCENE_INFO(SCENE_HYRULE_CASTLE),                                             "HC Boulder Crawlspace",             "HC Great Fairy Fountain",           ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR,  "", 1},
    { ENTRANCE_GROTTO_LOAD(GROTTO_HC_STORMS_OFFSET), ENTRANCE_GROTTO_EXIT(GROTTO_HC_STORMS_OFFSET), SINGLE_SCENE_INFO(SCENE_HYRULE_CASTLE),                                             "HC Storms Grotto Entry",            "HC Storms Grotto",                  ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO,    "bombable", 1},
    { ENTR_CASTLE_GROUNDS_GREAT_FAIRY_EXIT,          ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_DINS_HC,     {{ SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, 0x01 }},                                     "HC Great Fairy Fountain",           "HC Boulder Crawlspace",             ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR},
    { ENTRANCE_GROTTO_EXIT(GROTTO_HC_STORMS_OFFSET), ENTRANCE_GROTTO_LOAD(GROTTO_HC_STORMS_OFFSET), {{ SCENE_GROTTOS, 0x09 }},                                                          "HC Storms Grotto",                  "HC Storms Grotto Entry",            ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO,    "bombable"},
    { ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_OGC_DD,       ENTR_POTION_SHOP_KAKARIKO_1,                   SINGLE_SCENE_INFO(SCENE_OUTSIDE_GANONS_CASTLE),                                     "OGC Behind Pillar",                 "OGC Great Fairy Fountain",          ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR,  "outside ganon's castle", 1},
    { ENTR_INSIDE_GANONS_CASTLE_ENTRANCE,            ENTR_CASTLE_GROUNDS_RAINBOW_BRIDGE_EXIT,       SINGLE_SCENE_INFO(SCENE_OUTSIDE_GANONS_CASTLE),                                     "OGC Rainbow Bridge Exit",           "Inside Ganon's Castle Entrance",    ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,   "outside ganon's castle,gc", 1},
    { ENTR_POTION_SHOP_KAKARIKO_1,                   ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_OGC_DD,       {{ SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, 0x02 }},                                      "OGC Great Fairy Fountain",          "OGC Behind Pillar",                 ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR,  "outside ganon's castle"},
    { ENTR_CASTLE_GROUNDS_RAINBOW_BRIDGE_EXIT,       ENTR_INSIDE_GANONS_CASTLE_ENTRANCE,            SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE),                                      "Inside Ganon's Castle Entrance",    "OGC Rainbow Bridge Exit",           ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,   "outside ganon's castle,gc"},
    { ENTR_INSIDE_GANONS_CASTLE_1,                   ENTR_GANONS_TOWER_0,                           SINGLE_SCENE_INFO(SCENE_GANONS_TOWER),                                              "Ganon's Tower Entrance",            "Inside Ganon's Castle",             ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,   "gc"},
    { ENTR_GANONS_TOWER_0,                           ENTR_INSIDE_GANONS_CASTLE_1,                   SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE),                                      "Inside Ganon's Castle",             "Ganon's Tower Entrance",            ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON,   "gc"},
    { ENTR_OUTSIDE_GANONS_CASTLE_1_2,                -1,                                            SINGLE_SCENE_INFO(SCENE_OUTSIDE_GANONS_CASTLE),                                     "Ganon's Blue Warp",                 "Ganon's Castle Blue Warp",          ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_ONE_WAY,   "gc,bw", 1},

    // Doors
    { ENTRANCE_DOOR(0),     ENTRANCE_DOOR(1),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Lobby 2F", "Deku Tree 2F Middle Room", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(1),     ENTRANCE_DOOR(0),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree 2F Middle Room", "Deku Tree Lobby 2F", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(2),     ENTRANCE_DOOR(3),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree 2F Middle Room", "Deku Tree Falling Platform", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(3),     ENTRANCE_DOOR(2),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Falling Platform", "Deku Tree 2F Middle Room", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(4),     ENTRANCE_DOOR(5),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Lobby 3F", "Deku Tree Raising Platforms", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(5),     ENTRANCE_DOOR(4),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Raising Platforms", "Deku Tree Lobby 3F", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(6),     ENTRANCE_DOOR(7),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement", "Deku Tree Basement Scrub", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(7),     ENTRANCE_DOOR(6),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Scrub", "Deku Tree Basement", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(8),     ENTRANCE_DOOR(9),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Scrub", "Deku Tree Basement Water Room", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(9),     ENTRANCE_DOOR(8),   SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Water Room", "Deku Tree Basement Scrub", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(10),    ENTRANCE_DOOR(11),  SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Water Room", "Deku Tree Basement Torch Room", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(11),    ENTRANCE_DOOR(10),  SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Torch Room", "Deku Tree Basement Water Room", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(12),    ENTRANCE_DOOR(13),  SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Torch Room", "Deku Tree Basement Back Lobby", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(13),    ENTRANCE_DOOR(12),  SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Back Lobby", "Deku Tree Basement Torch Room", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(14),    ENTRANCE_DOOR(15),  SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Back Lobby", "Deku Tree Basement Back Room", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(15),    ENTRANCE_DOOR(14),  SINGLE_SCENE_INFO(SCENE_DEKU_TREE),    "Deku Tree Basement Back Room", "Deku Tree Basement Back Lobby", ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(16),    ENTRANCE_DOOR(17),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Lobby", "DC Behind Mouth", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(17),    ENTRANCE_DOOR(16),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Behind Mouth", "DC Lobby", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(18),    ENTRANCE_DOOR(19),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Behind Mouth", "DC Back Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(19),    ENTRANCE_DOOR(18),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Back Room", "DC Behind Mouth", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(20),    ENTRANCE_DOOR(21),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Lobby", "DC Blue Cave", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(21),    ENTRANCE_DOOR(20),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Blue Cave", "DC Lobby", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(22),    ENTRANCE_DOOR(23),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Blue Cave", "DC Blue Cave Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(23),    ENTRANCE_DOOR(22),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Blue Cave Room", "DC Blue Cave", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(24),    ENTRANCE_DOOR(25),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Blue Cave", "DC Lower Lizalfos", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(25),    ENTRANCE_DOOR(24),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Lower Lizalfos", "DC Blue Cave", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(26),    ENTRANCE_DOOR(27),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Lower Lizalfos", "DC Baby Dodongos", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(27),    ENTRANCE_DOOR(26),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Baby Dodongos", "DC Lower Lizalfos", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(28),    ENTRANCE_DOOR(29),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Baby Dodongos", "DC Baby Dodongos Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(29),    ENTRANCE_DOOR(28),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Baby Dodongos Room", "DC Baby Dodongos", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(30),    ENTRANCE_DOOR(31),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Lobby", "DC Stairs", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(31),    ENTRANCE_DOOR(30),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Stairs", "DC Lobby", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(32),    ENTRANCE_DOOR(33),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Stairs", "DC Room Below Stairs", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(33),    ENTRANCE_DOOR(32),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Room Below Stairs", "DC Stairs", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(34),    ENTRANCE_DOOR(35),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Above Stairs", "DC Room Above Stairs", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(35),    ENTRANCE_DOOR(34),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Room Above Stairs", "DC Above Stairs", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(36),    ENTRANCE_DOOR(37),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Room Above Stairs", "DC Front Bridge", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(37),    ENTRANCE_DOOR(36),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Front Bridge", "DC Room Above Stairs", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(38),    ENTRANCE_DOOR(39),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Front Bridge", "DC Spikes", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(39),    ENTRANCE_DOOR(38),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Spikes", "DC Front Bridge", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(40),    ENTRANCE_DOOR(41),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Spikes", "DC Upper Lizalfos", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(41),    ENTRANCE_DOOR(40),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Upper Lizalfos", "DC Spikes", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(42),    ENTRANCE_DOOR(43),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Upper Lizalfos", "DC Two Flames", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(43),    ENTRANCE_DOOR(42),  SINGLE_SCENE_INFO(SCENE_DODONGOS_CAVERN),   "DC Two Flames", "DC Upper Lizalfos", ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(44),    ENTRANCE_DOOR(45),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Beginning", "JJB Lift", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(45),    ENTRANCE_DOOR(44),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Lift", "JJB Beginning", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(46),    ENTRANCE_DOOR(47),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Lift", "JJB Holes", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(47),    ENTRANCE_DOOR(46),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Holes", "JJB Lift", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(48),    ENTRANCE_DOOR(49),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Holes", "JJB Pool", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(49),    ENTRANCE_DOOR(48),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Pool", "JJB Holes", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(50),    ENTRANCE_DOOR(51),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Holes", "JJB Water Switch", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(51),    ENTRANCE_DOOR(50),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Water Switch", "JJB Holes", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(52),    ENTRANCE_DOOR(53),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Water Switch", "JJB Lift", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(53),    ENTRANCE_DOOR(52),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Lift", "JJB Water Switch", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(54),    ENTRANCE_DOOR(55),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Holes", "JJB Fork", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(55),    ENTRANCE_DOOR(54),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Fork", "JJB Holes", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(56),    ENTRANCE_DOOR(57),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Fork", "JJB West", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(57),    ENTRANCE_DOOR(56),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB West", "JJB Fork", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(58),    ENTRANCE_DOOR(59),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Fork", "JJB North West", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(59),    ENTRANCE_DOOR(58),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB North West", "JJB Fork", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(60),    ENTRANCE_DOOR(61),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Fork", "JJB East", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(61),    ENTRANCE_DOOR(60),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB East", "JJB Fork", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(62),    ENTRANCE_DOOR(63),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Fork", "JJB North East", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(63),    ENTRANCE_DOOR(62),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB North East", "JJB Fork", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(64),    ENTRANCE_DOOR(65),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Fork", "JJB North", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(65),    ENTRANCE_DOOR(64),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB North", "JJB Fork", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(66),    ENTRANCE_DOOR(67),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Holes", "JJB Big Octo", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(67),    ENTRANCE_DOOR(66),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Big Octo", "JJB Holes", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(68),    ENTRANCE_DOOR(69),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Above Big Octo", "JJB Cubes", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(69),    ENTRANCE_DOOR(68),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Cubes", "JJB Above Big Octo", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(70),    ENTRANCE_DOOR(71),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Cubes", "JJB Above Lift", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(71),    ENTRANCE_DOOR(70),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Above Lift", "JJB Cubes", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(72),    ENTRANCE_DOOR(73),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB Lift Edge", "JJB East of Lift", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(73),    ENTRANCE_DOOR(72),  SINGLE_SCENE_INFO(SCENE_JABU_JABU),   "JJB East of Lift", "JJB Lift Edge", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(74),    ENTRANCE_DOOR(75),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Entrance", "Forest South Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(75),    ENTRANCE_DOOR(74),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest South Hall", "Forest Entrance", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(76),    ENTRANCE_DOOR(77),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest South Hall", "Forest Lobby", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(77),    ENTRANCE_DOOR(76),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Lobby", "Forest South Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(78),    ENTRANCE_DOOR(79),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Lobby", "Forest West Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(79),    ENTRANCE_DOOR(78),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest West Hall", "Forest Lobby", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(80),    ENTRANCE_DOOR(81),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest West Hall", "Forest Block Puzzle", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(81),    ENTRANCE_DOOR(80),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Block Puzzle", "Forest West Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(82),    ENTRANCE_DOOR(83),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Block Puzzle", "Forest NW Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(83),    ENTRANCE_DOOR(82),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NW Courtyard", "Forest Block Puzzle", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(84),    ENTRANCE_DOOR(85),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NW Courtyard", "Forest Floormaster", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(85),    ENTRANCE_DOOR(84),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Floormaster", "Forest NW Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(86),    ENTRANCE_DOOR(87),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NW Courtyard", "Forest Below Boss Key Chest", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(87),    ENTRANCE_DOOR(86),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Below Boss Key Chest", "Forest NW Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(88),    ENTRANCE_DOOR(89),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NW Courtyard", "Forest Courtyard Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(89),    ENTRANCE_DOOR(88),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Courtyard Hall", "Forest NW Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(90),    ENTRANCE_DOOR(91),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Courtyard Hall", "Forest NE Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(91),    ENTRANCE_DOOR(90),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NE Courtyard", "Forest Courtyard Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(92),    ENTRANCE_DOOR(93),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NW Courtyard", "Forest Lobby", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(93),    ENTRANCE_DOOR(92),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Lobby", "Forest NW Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(94),    ENTRANCE_DOOR(95),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NE Courtyard", "Forest Falling Floor", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(95),    ENTRANCE_DOOR(94),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Falling Floor", "Forest NE Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(96),    ENTRANCE_DOOR(97),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest NE Courtyard", "Forest Lobby", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(97),    ENTRANCE_DOOR(96),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Lobby", "Forest NE Courtyard", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(98),    ENTRANCE_DOOR(99),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Falling Floor", "Forest Green Poe", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(99),    ENTRANCE_DOOR(98),  SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Green Poe", "Forest Falling Floor", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(100),   ENTRANCE_DOOR(101), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Green Poe", "Forest East Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(101),   ENTRANCE_DOOR(100), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest East Hall", "Forest Green Poe", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(102),   ENTRANCE_DOOR(103), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest East Hall", "Forest Lobby", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(103),   ENTRANCE_DOOR(102), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Lobby", "Forest East Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(104),   ENTRANCE_DOOR(105), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Lobby", "Forest North Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(105),   ENTRANCE_DOOR(104), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest North Hall", "Forest Lobby", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(106),   ENTRANCE_DOOR(107), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest North Hall", "Forest North Room F1", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(107),   ENTRANCE_DOOR(106), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest North Room F1", "Forest North Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(108),   ENTRANCE_DOOR(109), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Block Puzzle", "Forest Twisted Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(109),   ENTRANCE_DOOR(108), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Twisted Hall", "Forest Block Puzzle", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(110),   ENTRANCE_DOOR(111), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Twisted Hall", "Forest Red Poe", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(111),   ENTRANCE_DOOR(110), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Red Poe", "Forest Twisted Hall", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(112),   ENTRANCE_DOOR(113), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Red Poe", "Forest North Room F2", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(113),   ENTRANCE_DOOR(112), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest North Room F2", "Forest Red Poe", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(114),   ENTRANCE_DOOR(115), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest North Room F2", "Forest Blue Poe", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(115),   ENTRANCE_DOOR(114), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Blue Poe", "Forest North Room F2", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(116),   ENTRANCE_DOOR(117), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Blue Poe", "Forest Twisted Room", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(117),   ENTRANCE_DOOR(116), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Twisted Room", "Forest Blue Poe", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(118),   ENTRANCE_DOOR(119), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Twisted Room", "Forest Frozen Eye", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(119),   ENTRANCE_DOOR(118), SINGLE_SCENE_INFO(SCENE_FOREST_TEMPLE),     "Forest Frozen Eye", "Forest Twisted Room", ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(120),   ENTRANCE_DOOR(121), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Lobby", "Fire Near Boss Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(121),   ENTRANCE_DOOR(120), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Near Boss Room", "Fire Lobby", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(122),   ENTRANCE_DOOR(123), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Lobby", "Fire West Loop Entrance", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(123),   ENTRANCE_DOOR(122), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Loop Entrance", "Fire Lobby", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(124),   ENTRANCE_DOOR(125), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Lobby", "Fire East Loop Entrance", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(125),   ENTRANCE_DOOR(124), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire East Loop Entrance", "Fire Lobby", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(126),   ENTRANCE_DOOR(127), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire East Loop", "Fire East Loop Entrance", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(127),   ENTRANCE_DOOR(126), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire East Loop Entrance", "Fire East Loop", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(128),   ENTRANCE_DOOR(129), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire East Loop", "Fire West Loop", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(129),   ENTRANCE_DOOR(128), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Loop", "Fire East Loop", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(130),   ENTRANCE_DOOR(131), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Loop", "Fire West Loop Entrance Switch", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(131),   ENTRANCE_DOOR(130), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Loop Entrance Switch", "Fire West Loop", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(132),   ENTRANCE_DOOR(133), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Lobby", "Fire Big Lava Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(133),   ENTRANCE_DOOR(132), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Big Lava Room", "Fire Lobby", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(134),   ENTRANCE_DOOR(135), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Big Lava Room", "Fire Big Lava Room North Goron", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(135),   ENTRANCE_DOOR(134), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Big Lava Room North Goron", "Fire Big Lava Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(136),   ENTRANCE_DOOR(137), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Big Lava Room", "Fire Above Big Lava Room North Goron", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(137),   ENTRANCE_DOOR(136), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Above Big Lava Room North Goron", "Fire Big Lava Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(138),   ENTRANCE_DOOR(139), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Big Lava Room", "Fire Big Lava Room South Goron", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(139),   ENTRANCE_DOOR(138), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Big Lava Room South Goron", "Fire Big Lava Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(140),   ENTRANCE_DOOR(141), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Big Lava Room", "Fire Fire Pillar", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(141),   ENTRANCE_DOOR(140), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Fire Pillar", "Fire Big Lava Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(142),   ENTRANCE_DOOR(143), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Above Fire Pillar", "Fire Below Boulder Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(143),   ENTRANCE_DOOR(142), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Below Boulder Maze", "Fire Above Fire Pillar", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(144),   ENTRANCE_DOOR(145), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Below Boulder Maze", "Fire Boulder Maze Start", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(145),   ENTRANCE_DOOR(144), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Boulder Maze Start", "Fire Below Boulder Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(146),   ENTRANCE_DOOR(147), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Above Boulder Maze", "Fire Room Above Boulder Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(147),   ENTRANCE_DOOR(146), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Room Above Boulder Maze", "Fire Above Boulder Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(148),   ENTRANCE_DOOR(149), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Room Above Boulder Maze", "Fire East Peak", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(149),   ENTRANCE_DOOR(148), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire East Peak", "Fire Room Above Boulder Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(150),   ENTRANCE_DOOR(151), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Boulder Maze Lower", "Fire Bridge Above Bridge", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(151),   ENTRANCE_DOOR(150), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Bridge Above Bridge", "Fire Boulder Maze Lower", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(152),   ENTRANCE_DOOR(153), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Boulder Maze Lower", "Fire Boulder Maze Side Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(153),   ENTRANCE_DOOR(152), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Boulder Maze Side Room", "Fire Boulder Maze Lower", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(154),   ENTRANCE_DOOR(155), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Boulder Maze", "Fire Wall Chase", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(155),   ENTRANCE_DOOR(154), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Chase", "Fire Boulder Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(156),   ENTRANCE_DOOR(157), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Chase Cage", "Fire Bridge Above Bridge", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(157),   ENTRANCE_DOOR(156), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Bridge Above Bridge", "Fire Wall Chase Cage", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(158),   ENTRANCE_DOOR(159), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Bridge Above Bridge", "Fire Wall Chase", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(159),   ENTRANCE_DOOR(158), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Chase", "Fire Bridge Above Bridge", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(160),   ENTRANCE_DOOR(161), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Chase", "Fire Corridor", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(161),   ENTRANCE_DOOR(160), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Corridor", "Fire Wall Chase", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(162),   ENTRANCE_DOOR(163), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Corridor", "Fire Wall Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(163),   ENTRANCE_DOOR(162), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze", "Fire Corridor", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(164),   ENTRANCE_DOOR(165), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze Upper", "Fire Wall Maze Core Upper", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(165),   ENTRANCE_DOOR(164), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze Core Upper", "Fire Wall Maze Upper", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(166),   ENTRANCE_DOOR(167), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze", "Fire Wall Maze Side Room", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(167),   ENTRANCE_DOOR(166), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze Side Room", "Fire Wall Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(168),   ENTRANCE_DOOR(169), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze", "Fire Wall Maze Core", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(169),   ENTRANCE_DOOR(168), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze Core", "Fire Wall Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(170),   ENTRANCE_DOOR(171), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze Core", "Fire Wall Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(171),   ENTRANCE_DOOR(170), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze", "Fire Wall Maze Core", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(172),   ENTRANCE_DOOR(173), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Wall Maze End", "Fire Upper Flare Dancer", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(173),   ENTRANCE_DOOR(172), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Upper Flare Dancer", "Fire Wall Maze End", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(174),   ENTRANCE_DOOR(175), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Above Upper Flare Dancer", "Fire West Climb", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(175),   ENTRANCE_DOOR(174), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Climb", "Fire Above Upper Flare Dancer", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(176),   ENTRANCE_DOOR(177), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Climb", "Fire West Peak", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(177),   ENTRANCE_DOOR(176), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Peak", "Fire West Climb", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(178),   ENTRANCE_DOOR(179), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire West Peak Basement", "Fire Hammer Stairs Top", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(179),   ENTRANCE_DOOR(178), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Hammer Stairs Top", "Fire West Peak Basement", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(180),   ENTRANCE_DOOR(181), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Hammer Stairs Bottom", "Fire Above Fire Wall Maze", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(181),   ENTRANCE_DOOR(180), SINGLE_SCENE_INFO(SCENE_FIRE_TEMPLE),     "Fire Above Fire Wall Maze", "Fire Hammer Stairs Bottom", ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(182),   ENTRANCE_DOOR(183), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 3F North", "Water Boss Ramp", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(183),   ENTRANCE_DOOR(182), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Boss Ramp", "Water 3F North", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(184),   ENTRANCE_DOOR(185), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 3F West", "Water Waterfall Front", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(185),   ENTRANCE_DOOR(184), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Waterfall Front", "Water 3F West", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(186),   ENTRANCE_DOOR(187), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Waterfall Back", "Water Skinny Pillar", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(187),   ENTRANCE_DOOR(186), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Skinny Pillar", "Water Waterfall Back", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(188),   ENTRANCE_DOOR(189), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Skinny Pillar Upper", "Water Dark Link", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(189),   ENTRANCE_DOOR(188), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Dark Link", "Water Skinny Pillar Upper", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(190),   ENTRANCE_DOOR(191), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Dark Link", "Water Past Dark Link", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(191),   ENTRANCE_DOOR(190), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Past Dark Link", "Water Dark Link", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(192),   ENTRANCE_DOOR(193), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Whirlpool", "Water 1F West", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(193),   ENTRANCE_DOOR(192), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 1F West", "Water Whirlpool", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(194),   ENTRANCE_DOOR(195), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 1F North", "Water Boulders Front", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(195),   ENTRANCE_DOOR(194), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Boulders Front", "Water 1F North", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(196),   ENTRANCE_DOOR(197), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Boulders Upper", "Water Boulders Hidden Room", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(197),   ENTRANCE_DOOR(196), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Boulders Hidden Room", "Water Boulders Upper", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(198),   ENTRANCE_DOOR(199), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Boulders Back", "Water Past Boulders", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(199),   ENTRANCE_DOOR(198), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Past Boulders", "Water Boulders Back", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(200),   ENTRANCE_DOOR(201), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Past Boulders", "Water Jets Past Boulders", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(201),   ENTRANCE_DOOR(200), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Jets Past Boulders", "Wate Past Bouldersr", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(202),   ENTRANCE_DOOR(203), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Jets Past Boulders", "Water Boulders Upper", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(203),   ENTRANCE_DOOR(202), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water Boulders Upper", "Water Jets Past Boulders", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(204),   ENTRANCE_DOOR(205), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 3F East", "Water 3F East Side Room", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(205),   ENTRANCE_DOOR(204), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 3F East Side Room", "Water 3F East", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(206),   ENTRANCE_DOOR(207), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 1F East", "Water 1F East Side Room", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(207),   ENTRANCE_DOOR(206), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 1F East Side Room", "Water 1F East", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(208),   ENTRANCE_DOOR(209), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 1F", "Water 1F Central", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(209),   ENTRANCE_DOOR(208), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 1F Central", "Water 1F", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(210),   ENTRANCE_DOOR(211), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 2F", "Water 2F Central", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(211),   ENTRANCE_DOOR(210), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 2F Central", "Water 2F", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(212),   ENTRANCE_DOOR(213), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 2F", "Water 2F East", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(213),   ENTRANCE_DOOR(212), SINGLE_SCENE_INFO(SCENE_WATER_TEMPLE),    "Water 2F East", "Water 2F", ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(214),   ENTRANCE_DOOR(215), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Beginning", "Shadow Whispering Walls", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(215),   ENTRANCE_DOOR(214), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Whispering Walls", "Shadow Beginning", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(216),   ENTRANCE_DOOR(217), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Whispering Walls", "Shadow Whispering Walls Side Room", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(217),   ENTRANCE_DOOR(216), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Whispering Walls Side Room", "Shadow Whispering Walls", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(218),   ENTRANCE_DOOR(219), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Whispering Walls", "Shadow Deadhand", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(219),   ENTRANCE_DOOR(218), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Deadhand", "Shadow Whispering Walls", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(220),   ENTRANCE_DOOR(221), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow First Beamos", "Shadow First Beamos Right", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(221),   ENTRANCE_DOOR(220), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow First Beamos Right", "Shadow First Beamos", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(222),   ENTRANCE_DOOR(223), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow First Beamos", "Shadow First Beamos Left", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(223),   ENTRANCE_DOOR(222), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow First Beamos Left", "Shadow First Beamos", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(224),   ENTRANCE_DOOR(225), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow First Beamos", "Shadow Hall to Huge Pit", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(225),   ENTRANCE_DOOR(224), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Hall to Huge Pit", "Shadow First Beamos", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(226),   ENTRANCE_DOOR(227), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Huge Pit Upper", "Shadow Invisible Blades", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(227),   ENTRANCE_DOOR(226), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Invisible Blades", "Shadow Huge Pit Upper", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(228),   ENTRANCE_DOOR(229), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Huge Pit Lower", "Shadow Invisible Spikes", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(229),   ENTRANCE_DOOR(228), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Invisible Spikes", "Shadow Huge Pit Lower", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(230),   ENTRANCE_DOOR(231), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Invisible Spikes", "Shadow Skull Jar", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(231),   ENTRANCE_DOOR(230), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Skull Jar", "Shadow Invisible Spikes", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(232),   ENTRANCE_DOOR(233), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Invisible Spikes", "Shadow Wind Tunnel Upper", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(233),   ENTRANCE_DOOR(232), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Wind Tunnel Upper", "Shadow Invisible Spikes", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(234),   ENTRANCE_DOOR(235), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Wind Tunnel Lower", "Shadow Wind Hint", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(235),   ENTRANCE_DOOR(234), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Wind Hint", "Shadow Wind Tunnel Lower", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(236),   ENTRANCE_DOOR(237), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Wind Tunnel Lower", "Shadow Room to Boat", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(237),   ENTRANCE_DOOR(236), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Room to Boat", "Shado Wind Tunnel Lowerw", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(238),   ENTRANCE_DOOR(239), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Room to Boat", "Shadow Dock", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(239),   ENTRANCE_DOOR(238), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Dock", "Shadow Room to Boat", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(240),   ENTRANCE_DOOR(241), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Past Chasm", "Shadow Near Boss Room", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(241),   ENTRANCE_DOOR(240), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Near Boss Room", "Shadow Past Chasm", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(242),   ENTRANCE_DOOR(243), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Beyond Boat", "Shadow Maze", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(243),   ENTRANCE_DOOR(242), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Maze", "Shadow Beyond Boat", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(244),   ENTRANCE_DOOR(245), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Maze", "Shadow X-Cross", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(245),   ENTRANCE_DOOR(244), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow X-Cross", "Shadow Maze", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(246),   ENTRANCE_DOOR(247), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Maze", "Shadow Three Skull Jars", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(247),   ENTRANCE_DOOR(246), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Three Skull Jars", "Shadow Maze", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(248),   ENTRANCE_DOOR(249), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Maze", "Shadow Wooden Spikes", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(249),   ENTRANCE_DOOR(248), SINGLE_SCENE_INFO(SCENE_SHADOW_TEMPLE),   "Shadow Wooden Spikes", "Shadow Maze", ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(250),   ENTRANCE_DOOR(251), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Adult Lobby", "Spirit Adult Sand Pit", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(251),   ENTRANCE_DOOR(250), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Adult Sand Pit", "Spirit Adult Lobby", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(252),   ENTRANCE_DOOR(253), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Adult Lobby", "Spirit Boulders", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(253),   ENTRANCE_DOOR(252), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Boulders", "Spirit Adult Lobby", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(254),   ENTRANCE_DOOR(255), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Boulders", "Spirit Past Boulders", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(255),   ENTRANCE_DOOR(254), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Past Boulders", "Spirit Boulders", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(256),   ENTRANCE_DOOR(257), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Adult Lobby", "Spirit Adult Climb", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(257),   ENTRANCE_DOOR(256), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Adult Climb", "Spirit Adult Lobby", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(258),   ENTRANCE_DOOR(259), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Statue East", "Spirit Adult Climb", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(259),   ENTRANCE_DOOR(258), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Adult Climb", "Spirit Statue East", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(260),   ENTRANCE_DOOR(261), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Statue East", "Spirit Stairs to Beamos Pits", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(261),   ENTRANCE_DOOR(260), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to Beamos Pits", "Spirit Statue East", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(262),   ENTRANCE_DOOR(263), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to Beamos Pits", "Spirit Beamos Pits", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(263),   ENTRANCE_DOOR(262), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Beamos Pits", "Spirit Stairs to Beamos Pits", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(264),   ENTRANCE_DOOR(265), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Beamos Pits", "Spirit Big Wall Lower", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(265),   ENTRANCE_DOOR(264), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Big Wall Lower", "Spirit Beamos Pits", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(266),   ENTRANCE_DOOR(267), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Beamos Pits", "Spirit Four Armos", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(267),   ENTRANCE_DOOR(266), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Four Armos", "Spirit Beamos Pits", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(268),   ENTRANCE_DOOR(269), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Four Armos", "Spirit Four Armos Side Room", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(269),   ENTRANCE_DOOR(268), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Four Armos Side Room", "Spirit Four Armos", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(270),   ENTRANCE_DOOR(271), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Four Armos", "Spirit Stairs to East Hand", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(271),   ENTRANCE_DOOR(270), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to East Hand", "Spirit Four Armos", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(272),   ENTRANCE_DOOR(273), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to East Hand", "Spirit East Iron Knuckle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(273),   ENTRANCE_DOOR(272), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit East Iron Knuckle", "Spirit Stairs to East Hand", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(274),   ENTRANCE_DOOR(275), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit East Iron Knuckle", "Spirit Exit to East Hand", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(275),   ENTRANCE_DOOR(274), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Exit to East Hand", "Spirit East Iron Knuckle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(276),   ENTRANCE_DOOR(277), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Big Wall Upper", "Spirit 4F Central", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(277),   ENTRANCE_DOOR(276), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit 4F Central", "Spiri Big Wall Uppert", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(278),   ENTRANCE_DOOR(279), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit 4F Central", "Spirit 4F Side Room", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(279),   ENTRANCE_DOOR(278), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit 4F Side Room", "Spirit 4F Central", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(280),   ENTRANCE_DOOR(281), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit 4F Central", "Spirit Big Mirror", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(281),   ENTRANCE_DOOR(280), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Big Mirror", "Spirit 4F Central", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(282),   ENTRANCE_DOOR(283), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Big Mirror", "Spirit Big Mirror Cave", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(283),   ENTRANCE_DOOR(282), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Big Mirror Cave", "Spirit Big Mirror", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(284),   ENTRANCE_DOOR(285), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Lobby", "Spirit Child Left", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(285),   ENTRANCE_DOOR(284), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Left", "Spirit Child Lobby", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(286),   ENTRANCE_DOOR(287), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Left Across Bridge", "Spirit Child Back Room", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(287),   ENTRANCE_DOOR(286), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Back Room", "Spirit Child Left Across Bridge", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(288),   ENTRANCE_DOOR(289), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Back Room", "Spirit Child Right Across Bridge", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(289),   ENTRANCE_DOOR(288), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Right Across Bridge", "Spirit Child Back Room", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(290),   ENTRANCE_DOOR(291), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Right", "Spirit Child Lobby", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(291),   ENTRANCE_DOOR(290), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Lobby", "Spirit Child Right", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(292),   ENTRANCE_DOOR(293), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Before Climb", "Spirit Child Climb", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(293),   ENTRANCE_DOOR(292), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Climb", "Spirit Child Before Climb", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(294),   ENTRANCE_DOOR(295), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Child Climb", "Spirit Statue", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(295),   ENTRANCE_DOOR(294), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Statue", "Spirit Child Climb", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(296),   ENTRANCE_DOOR(297), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Statue", "Spirit Stairs to Block Puzzle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(297),   ENTRANCE_DOOR(296), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to Block Puzzle", "Spirit Statue", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(298),   ENTRANCE_DOOR(299), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to Block Puzzle", "Spirit Block Puzzle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(299),   ENTRANCE_DOOR(298), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Block Puzzle", "Spirit Stairs to Block Puzzle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(300),   ENTRANCE_DOOR(301), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Block Puzzle", "Spirit Stairs to West Hand", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(301),   ENTRANCE_DOOR(300), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to West Hand", "Spirit Block Puzzle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(302),   ENTRANCE_DOOR(303), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Stairs to West Hand", "Spirit West Iron Knuckle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(303),   ENTRANCE_DOOR(302), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit West Iron Knuckle", "Spirit Stairs to West Hand", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(304),   ENTRANCE_DOOR(305), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit West Iron Knuckle", "Spirit Exit to West Hand", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(305),   ENTRANCE_DOOR(304), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Exit to West Hand", "Spirit West Iron Knuckle", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(306),   ENTRANCE_DOOR(307), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Statue", "Spirit Shortcut", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(307),   ENTRANCE_DOOR(306), SINGLE_SCENE_INFO(SCENE_SPIRIT_TEMPLE),   "Spirit Shortcut", "Spirit Statue", ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_GROUP_DESERT_COLOSSUS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(308),   ENTRANCE_DOOR(309), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well", "Bottom of the Well West Inner Room", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(309),   ENTRANCE_DOOR(310), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well West Inner Room", "Bottom of the Well", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(310),   ENTRANCE_DOOR(311), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well", "Bottom of the Well East Inner Room", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(311),   ENTRANCE_DOOR(310), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well East Inner Room", "Bottom of the Well", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(312),   ENTRANCE_DOOR(313), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well", "Bottom of the Well Coffins", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(313),   ENTRANCE_DOOR(312), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well Coffins", "Bottom of the Well", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(314),   ENTRANCE_DOOR(315), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well", "Bottom of the Well Dead Hand", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(315),   ENTRANCE_DOOR(314), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well Dead Hand", "Bottom of the Well", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(316),   ENTRANCE_DOOR(317), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well", "Bottom of the Well Pits", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(317),   ENTRANCE_DOOR(316), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well Pits", "Bottom of the Well", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(318),   ENTRANCE_DOOR(319), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well", "Bottom of the Well Cage", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(319),   ENTRANCE_DOOR(318), SINGLE_SCENE_INFO(SCENE_BOTTOM_OF_THE_WELL), "Bottom of the Well Cage", "Bottom of the Well", ENTRANCE_GROUP_KAKARIKO, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(320),   ENTRANCE_DOOR(321), SINGLE_SCENE_INFO(SCENE_ICE_CAVERN),   "Ice Cavern", "Ice Cavern Final Room", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(321),   ENTRANCE_DOOR(320), SINGLE_SCENE_INFO(SCENE_ICE_CAVERN),   "Ice Cavern Final Room", "Ice Cavern", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(322),   ENTRANCE_DOOR(323), SINGLE_SCENE_INFO(SCENE_ICE_CAVERN),   "Ice Cavern Final Room", "Ice Cavern Shortcut", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(323),   ENTRANCE_DOOR(322), SINGLE_SCENE_INFO(SCENE_ICE_CAVERN),   "Ice Cavern Shortcut", "Ice Cavern Final Room", ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_GROUP_ZORAS_FOUNTAIN, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(324),   ENTRANCE_DOOR(325), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Lobby", "GTG Maze", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(325),   ENTRANCE_DOOR(324), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Maze", "GTG Lobby", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(326),   ENTRANCE_DOOR(327), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Lobby", "GTG Sand Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(327),   ENTRANCE_DOOR(326), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Sand Room", "GTG Lobby", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(328),   ENTRANCE_DOOR(329), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Sand Room", "GTG West Lower", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(329),   ENTRANCE_DOOR(328), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG West Lower", "GTG Sand Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(330),   ENTRANCE_DOOR(331), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG West Upper", "GTG Heavy Block", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(331),   ENTRANCE_DOOR(330), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Heavy Block", "GTG West Upper", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(332),   ENTRANCE_DOOR(333), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Heavy Block", "GTG Room Behind Heavy Block", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(333),   ENTRANCE_DOOR(332), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Room Behind Heavy Block", "GTG Heavy Block", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(334),   ENTRANCE_DOOR(335), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Heavy Block Upper", "GTG Eye Statue Upper", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(335),   ENTRANCE_DOOR(334), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Eye Statue Upper", "GTG Heavy Block Upper", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(336),   ENTRANCE_DOOR(337), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Eye Statue Upper", "GTG Above Maze", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(337),   ENTRANCE_DOOR(336), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Above Maze", "GTG Eye Statue Upper", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(338),   ENTRANCE_DOOR(339), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Eye Statue", "GTG Hammer Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(339),   ENTRANCE_DOOR(338), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Hammer Room", "GTG Eye Statue", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(340),   ENTRANCE_DOOR(341), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Hammer Room", "GTG Lava Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(341),   ENTRANCE_DOOR(340), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Lava Room", "GTG Hammer Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(342),   ENTRANCE_DOOR(343), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Lava Room", "GTG Underwater Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(343),   ENTRANCE_DOOR(342), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Underwater Room", "GTG Lava Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(344),   ENTRANCE_DOOR(345), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Lava Room", "GTG Dinalfos", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(345),   ENTRANCE_DOOR(344), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Dinalfos", "GTG Lava Room", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(346),   ENTRANCE_DOOR(347), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Dinalfos", "GTG Lobby", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(347),   ENTRANCE_DOOR(346), SINGLE_SCENE_INFO(SCENE_GERUDO_TRAINING_GROUND), "GTG Lobby", "GTG Dinalfos", ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(348),   ENTRANCE_DOOR(349), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Entrance", "GC Lobby", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(349),   ENTRANCE_DOOR(348), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Lobby", "GC Entrance", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(350),   ENTRANCE_DOOR(351), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Lobby", "GC Spirit", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(351),   ENTRANCE_DOOR(350), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Spirit", "GC Lobby", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(352),   ENTRANCE_DOOR(353), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Spirit", "GC Spirit Before Switch", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(353),   ENTRANCE_DOOR(352), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Spirit Before Switch", "GC Spirit", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(354),   ENTRANCE_DOOR(355), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Spirit After Switch", "GC Spirit Final", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(355),   ENTRANCE_DOOR(354), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Spirit Final", "GC Spirit After Switch", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(356),   ENTRANCE_DOOR(357), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Lobby", "GC Light", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(357),   ENTRANCE_DOOR(356), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Light", "GC Lobby", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(358),   ENTRANCE_DOOR(359), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Light Triforce", "GC Light Boulders", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(359),   ENTRANCE_DOOR(358), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Light Boulders", "GC Light Triforce", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(360),   ENTRANCE_DOOR(361), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Light Boulders", "GC Light Final", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(361),   ENTRANCE_DOOR(360), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Light Final", "GC Light Boulders", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(362),   ENTRANCE_DOOR(363), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Lobby", "GC Fire Front", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(363),   ENTRANCE_DOOR(362), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Fire Front", "GC Lobby", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(364),   ENTRANCE_DOOR(365), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Fire Back", "GC Final", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(365),   ENTRANCE_DOOR(364), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Final", "GC Fire Back", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(366),   ENTRANCE_DOOR(367), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Lobby", "GC Shadow Front", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(367),   ENTRANCE_DOOR(366), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Shadow Front", "GC Lobby", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(368),   ENTRANCE_DOOR(369), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Shadow Back", "GC Shadow Trial", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(369),   ENTRANCE_DOOR(368), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Shadow Trial", "GC Shadow Back", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(370),   ENTRANCE_DOOR(371), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Lobby", "GC Water", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(371),   ENTRANCE_DOOR(370), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Water", "GC Lobby", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(372),   ENTRANCE_DOOR(373), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Water Back", "GC Water Block Front", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(373),   ENTRANCE_DOOR(372), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Water Block Front", "GC Water Back", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(374),   ENTRANCE_DOOR(375), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Water Block Back", "GC Water Final", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(375),   ENTRANCE_DOOR(374), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Water Final", "GC Water Block Back", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(376),   ENTRANCE_DOOR(377), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Lobby", "GC Forest", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(377),   ENTRANCE_DOOR(376), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Forest", "GC Lobby", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(378),   ENTRANCE_DOOR(379), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Forest Back", "GC Forest Wind Front", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(379),   ENTRANCE_DOOR(378), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Forest Wind Front", "GC Forest Back", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(380),   ENTRANCE_DOOR(381), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Forest Wind Back", "GC Forest Final", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(381),   ENTRANCE_DOOR(380), SINGLE_SCENE_INFO(SCENE_INSIDE_GANONS_CASTLE), "GC Forest Final", "GC Forest Wind Back", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(382),   ENTRANCE_DOOR(383), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Stairs 1", "Ganon's Tower Floor 1", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(383),   ENTRANCE_DOOR(382), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Floor 1", "Ganon's Tower Stairs 1", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(384),   ENTRANCE_DOOR(385), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Floor 1", "Ganon's Tower Stairs 2", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(385),   ENTRANCE_DOOR(384), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Stairs 2", "Ganon's Tower Floor 1", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(386),   ENTRANCE_DOOR(387), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Stairs 2", "Ganon's Tower Floor 2", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(387),   ENTRANCE_DOOR(386), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Floor 2", "Ganon's Tower Stairs 2", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(388),   ENTRANCE_DOOR(389), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Floor 2", "Ganon's Tower Stairs 3", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(389),   ENTRANCE_DOOR(388), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Stairs 3", "Ganon's Tower Floor 2", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(390),   ENTRANCE_DOOR(391), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Stairs 3", "Ganon's Tower Floor 3", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(391),   ENTRANCE_DOOR(390), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Floor 3", "Ganon's Tower Floor 4", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(392),   ENTRANCE_DOOR(393), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Floor 3", "Ganon's Tower Stairs 4", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
    { ENTRANCE_DOOR(393),   ENTRANCE_DOOR(392), SINGLE_SCENE_INFO(SCENE_GANONS_TOWER), "Ganon's Tower Stairs 4", "Ganon's Tower Floor 3", ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DOOR },
};
// clang-format on

// Check if Link is in the area and return that scene/entrance for tracking
int16_t LinkIsInArea(const EntranceData* entrance) {
    bool result = false;

    if (gPlayState == nullptr) {
        return -1;
    }

    // Handle detecting the current grotto
    if ((gPlayState->sceneNum == SCENE_FAIRYS_FOUNTAIN || gPlayState->sceneNum == SCENE_GROTTOS) &&
        entrance->type == ENTRANCE_TYPE_GROTTO) {
        if (entrance->index == (ENTRANCE_GROTTO_EXIT_START + currentGrottoId)) {
            // Return the grotto entrance for tracking
            return entrance->index;
        } else {
            return -1;
        }
    }

    // Otherwise check all scenes/spawns
    // Not all areas require a spawn position to differeniate between another area
    for (auto info : entrance->scenes) {
        // only check current scene when spawn info missing
        if (info.spawn == -1) {
            result = gPlayState->sceneNum == info.scene;
        } else if (gPlayState->sceneNum == SCENE_THIEVES_HIDEOUT) { // group by rooms, not spawn
            result = info.scene == SCENE_THIEVES_HIDEOUT && gPlayState->roomCtx.curRoom.num == info.spawn;
        } else { // Otherwise just check scene & spawn
            result = Entrance_SceneAndSpawnAre(info.scene, info.spawn);
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
        // the randomizer will have also overridden the water-based entrances, so check those too
        if ((index == ENTR_ZORAS_RIVER_WEST_EXIT && Entrance_GetIsEntranceDiscovered(ENTR_ZORAS_RIVER_3)) ||
            (index == ENTR_ZORAS_RIVER_3 && Entrance_GetIsEntranceDiscovered(ENTR_ZORAS_RIVER_WEST_EXIT))) {
            isDiscovered = true;
        } else if ((index == ENTR_HYRULE_FIELD_RIVER_EXIT && Entrance_GetIsEntranceDiscovered(ENTR_HYRULE_FIELD_14)) ||
                   (index == ENTR_HYRULE_FIELD_14 && Entrance_GetIsEntranceDiscovered(ENTR_HYRULE_FIELD_RIVER_EXIT))) {
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

void LoadFromPreset(const nlohmann::json& info) {
    presetLoaded = true;
    presetPos = { info.at("pos").at("x"), info.at("pos").at("y") };
    presetSize = { info.at("size").at("width"), info.at("size").at("height") };
}

// Used for verifying the names on both sides of entrance pairs match. Keeping for ease of use for further name changes
// later
// TODO: Figure out how to remove the need for duplicate entrance names so this is no longer necessary
void CheckEntranceNames() {
    SPDLOG_ERROR("Checking entrance names:");
    for (size_t i = 0; i < ARRAY_COUNT(entranceData); i++) {
        auto entrance = &entranceData[i];
        auto reverse = GetEntranceData(entrance->reverseIndex);
        if (entrance != nullptr && reverse != nullptr) {
            if (entrance->source != reverse->destination) {
                SPDLOG_ERROR("{}({}) -> {}({})", entrance->source, entrance->index, reverse->destination,
                             reverse->reverseIndex);
            }
        }
    }
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

                int16_t entranceIndex = byDest ? tempList[j].override : tempList[j].index;

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
    auto entranceCtx = Rando::Context::GetInstance()->GetEntranceShuffler();
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
                if (Entrance_EntranceIsNull(&entranceCtx->entranceOverrides[i])) {
                    continue;
                }
                const EntranceData* curEntrance = GetEntranceData(entranceCtx->entranceOverrides[i].index);
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
                        // "Remove" this entrance from the tempList by setting its values to zero
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
    gEntranceTrackingData = { 0 };
}

void InitEntranceTrackingData() {
    auto entranceCtx = Rando::Context::GetInstance()->GetEntranceShuffler();
    gEntranceTrackingData = { 0 };

    // Check if entrance randomization is disabled
    if (!OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        return;
    }

    // Set total and group counts
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        if (Entrance_EntranceIsNull(&entranceCtx->entranceOverrides[i])) {
            break;
        }
        const EntranceData* index = GetEntranceData(entranceCtx->entranceOverrides[i].index);
        const EntranceData* override = GetEntranceData(entranceCtx->entranceOverrides[i].override);

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
        srcListSortedByArea[i] = entranceCtx->entranceOverrides[i];
        destListSortedByArea[i] = entranceCtx->entranceOverrides[i];
        srcListSortedByType[i] = entranceCtx->entranceOverrides[i];
        destListSortedByType[i] = entranceCtx->entranceOverrides[i];
    }
    SortEntranceListByArea(srcListSortedByArea, 0);
    SortEntranceListByArea(destListSortedByArea, 1);
    SortEntranceListByType(srcListSortedByType, 0);
    SortEntranceListByType(destListSortedByType, 1);
}

void EntranceTrackerSettingsWindow::DrawElement() {

    ImGui::TextWrapped("The entrance tracker will only track shuffled entrances");
    Spacer(0);

    ImGui::TableNextColumn();
    SohGui::GetSohMenu()->MenuDrawItem(backgroundColorWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);

    SohGui::GetSohMenu()->MenuDrawItem(windowTypeWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);

    if (CVarGetInteger(CVAR_TRACKER_ENTRANCE("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        CVarCheckbox("Enable Dragging", CVAR_TRACKER_ENTRANCE("Draggable"), CheckboxOptions().Color(THEME_COLOR));
        CVarCheckbox("Only Enable While Paused", CVAR_TRACKER_ENTRANCE("ShowOnlyPaused"),
                     CheckboxOptions().Color(THEME_COLOR));
        CVarCombobox("Display Mode", CVAR_TRACKER_ENTRANCE("DisplayType"), showMode,
                     ComboboxOptions()
                         .LabelPosition(LabelPositions::Far)
                         .ComponentAlignment(ComponentAlignments::Right)
                         .Color(THEME_COLOR)
                         .DefaultIndex(0));
        if (CVarGetInteger(CVAR_TRACKER_ENTRANCE("DisplayType"), TRACKER_DISPLAY_ALWAYS) ==
            TRACKER_DISPLAY_COMBO_BUTTON) {
            CVarCombobox("Combo Button 1", CVAR_TRACKER_ENTRANCE("ComboButton1"), buttonStrings,
                         ComboboxOptions()
                             .LabelPosition(LabelPositions::Far)
                             .ComponentAlignment(ComponentAlignments::Right)
                             .Color(THEME_COLOR)
                             .DefaultIndex(TRACKER_COMBO_BUTTON_L));
            CVarCombobox("Combo Button 2", CVAR_TRACKER_ENTRANCE("ComboButton2"), buttonStrings,
                         ComboboxOptions()
                             .LabelPosition(LabelPositions::Far)
                             .ComponentAlignment(ComponentAlignments::Right)
                             .Color(THEME_COLOR)
                             .DefaultIndex(TRACKER_COMBO_BUTTON_L));
        }
    }

    if (ImGui::BeginTable("entranceTrackerSubSettings", 2,
                          ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("column 1", ImGuiTableColumnFlags_WidthStretch, 150.0f);
        ImGui::TableSetupColumn("column 2", ImGuiTableColumnFlags_WidthStretch, 150.0f);

        ImGui::TableNextColumn();

        ImGui::Text("Sort By");
        CVarRadioButton(
            "To", CVAR_TRACKER_ENTRANCE("SortBy"), 0,
            RadioButtonsOptions().Color(THEME_COLOR).Tooltip("Sort entrances by the original source entrance"));
        CVarRadioButton(
            "From", CVAR_TRACKER_ENTRANCE("SortBy"), 1,
            RadioButtonsOptions().Color(THEME_COLOR).Tooltip("Sort entrances by the overrided destination"));

        ImGui::Text("List Items");
        CVarCheckbox("Auto scroll", CVAR_TRACKER_ENTRANCE("AutoScroll"),
                     CheckboxOptions()
                         .Tooltip("Automatically scroll to the first available entrance in the current scene")
                         .Color(THEME_COLOR));
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        CVarCheckbox(
            "Highlight previous", CVAR_TRACKER_ENTRANCE("HighlightPrevious"),
            CheckboxOptions().Tooltip("Highlight the previous entrance that Link came from").Color(THEME_COLOR));
        CVarCheckbox(
            "Highlight available", CVAR_TRACKER_ENTRANCE("HighlightAvailable"),
            CheckboxOptions().Tooltip("Highlight available entrances in the current scene").Color(THEME_COLOR));
        ImGui::EndDisabled();
        CVarCheckbox("Hide undiscovered", CVAR_TRACKER_ENTRANCE("CollapseUndiscovered"),
                     CheckboxOptions()
                         .Tooltip("Collapse undiscovered entrances towards the bottom of each group")
                         .Color(THEME_COLOR));
        bool disableHideReverseEntrances =
            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) == RO_GENERIC_ON;
        static const char* disableHideReverseEntrancesText =
            "This option is disabled because \"Decouple Entrances\" is enabled.";
        CVarCheckbox("Hide reverse", CVAR_TRACKER_ENTRANCE("HideReverseEntrances"),
                     CheckboxOptions({ { .disabled = disableHideReverseEntrances,
                                         .disabledTooltip = disableHideReverseEntrancesText } })
                         .Tooltip("Hide reverse entrance transitions when Decouple Entrances is off")
                         .DefaultValue(true)
                         .Color(THEME_COLOR));

        ImGui::TableNextColumn();

        ImGui::Text("Group By");
        CVarRadioButton("Area", CVAR_TRACKER_ENTRANCE("GroupBy"), 0,
                        RadioButtonsOptions().Color(THEME_COLOR).Tooltip("Group entrances by their area"));
        CVarRadioButton("Type", CVAR_TRACKER_ENTRANCE("GroupBy"), 1,
                        RadioButtonsOptions().Color(THEME_COLOR).Tooltip("Group entrances by their entrance type"));

        ImGui::Text("Spoiler Reveal");
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        CVarCheckbox("Show Source", CVAR_TRACKER_ENTRANCE("ShowFrom"),
                     CheckboxOptions().Tooltip("Reveal the source for undiscovered entrances").Color(THEME_COLOR));
        CVarCheckbox("Show Destination", CVAR_TRACKER_ENTRANCE("ShowTo"),
                     CheckboxOptions().Tooltip("Reveal the destination for undiscovered entrances").Color(THEME_COLOR));
        ImGui::EndDisabled();
        ImGui::EndTable();
    }

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Legend")) {
        ImGui::TextColored(ImColor(COLOR_ORANGE), "Last Entrance");
        ImGui::TextColored(ImColor(COLOR_GREEN), "Available Entrances");
        ImGui::TextColored(ImColor(COLOR_GRAY), "Undiscovered Entrances");
        ImGui::TreePop();
    }
}

void EntranceTrackerWindow::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

void EntranceTrackerWindow::DrawElement() {
    Color_Background = CVarGetColor(CVAR_TRACKER_ENTRANCE("BgColor.Value"), Color_Bg_Default);
    if (CVarGetInteger(CVAR_TRACKER_ENTRANCE("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        if (CVarGetInteger(CVAR_TRACKER_ENTRANCE("ShowOnlyPaused"), 0) &&
            (gPlayState == nullptr || gPlayState->pauseCtx.state == 0)) {
            return;
        }

        if (CVarGetInteger(CVAR_TRACKER_ENTRANCE("DisplayType"), TRACKER_DISPLAY_ALWAYS) ==
            TRACKER_DISPLAY_COMBO_BUTTON) {
            int comboButton1Mask =
                buttons[CVarGetInteger(CVAR_TRACKER_ENTRANCE("ComboButton1"), TRACKER_COMBO_BUTTON_L)];
            int comboButton2Mask =
                buttons[CVarGetInteger(CVAR_TRACKER_ENTRANCE("ComboButton2"), TRACKER_COMBO_BUTTON_R)];
            OSContPad* trackerButtonsPressed =
                std::dynamic_pointer_cast<LUS::ControlDeck>(Ship::Context::GetRawInstance()->GetControlDeck())
                    ->GetPads();
            bool comboButtonsHeld = trackerButtonsPressed != nullptr &&
                                    trackerButtonsPressed[0].button & comboButton1Mask &&
                                    trackerButtonsPressed[0].button & comboButton2Mask;
            if (!comboButtonsHeld) {
                return;
            }
        }
    }
    if (presetLoaded) {
        ImGui::SetNextWindowSize(presetSize);
        ImGui::SetNextWindowPos(presetPos);
        presetLoaded = false;
    } else {
        ImGui::SetNextWindowSize(ImVec2(600, 375), ImGuiCond_FirstUseEver);
    }
    if (Trackers::BeginFloatWindows(
            "Entrance Tracker", mIsVisible, Color_Background,
            static_cast<TrackerWindowType>(CVarGetInteger(CVAR_TRACKER_ENTRANCE("WindowType"), TRACKER_WINDOW_WINDOW)),
            CVarGetInteger(CVAR_TRACKER_ENTRANCE("Draggable"), 1), ImGuiWindowFlags_NoScrollbar)) {
        if (!GameInteractor::IsSaveLoaded()) {
            ImGui::Text("Waiting for file load..."); // TODO Language
            Trackers::EndFloatWindows();
            return;
        }

        static ImGuiTextFilter locationSearch;

        uint8_t nextTreeState = 0;
        if (Button("Collapse All", ButtonOptions({ { .tooltip = "Collapse all entrance groups" } })
                                       .Color(THEME_COLOR)
                                       .Size(Sizes::Inline))) {
            nextTreeState = 1;
        }
        ImGui::SameLine();
        if (Button("Expand All", ButtonOptions({ { .tooltip = "Expand all entrance groups" } })
                                     .Color(THEME_COLOR)
                                     .Size(Sizes::Inline))) {
            nextTreeState = 2;
        }
        ImGui::SameLine();
        if (Button("Clear",
                   ButtonOptions({ { .tooltip = "Clear the search field" } }).Color(THEME_COLOR).Size(Sizes::Inline))) {
            locationSearch.Clear();
        }

        PushStyleCombobox(THEME_COLOR);
        if (locationSearch.Draw()) {
            nextTreeState = 2;
        }
        PopStyleCombobox();

        uint8_t destToggle = CVarGetInteger(CVAR_TRACKER_ENTRANCE("SortBy"), 0);
        uint8_t groupToggle = CVarGetInteger(CVAR_TRACKER_ENTRANCE("GroupBy"), 0);

        // Combine destToggle and groupToggle to get a range of 0-3
        uint8_t groupType = destToggle + (groupToggle * 2);
        size_t groupCount = groupToggle ? (size_t)ENTRANCE_TYPE_COUNT : (size_t)SPOILER_ENTRANCE_GROUP_COUNT;
        auto groupNames = groupToggle ? groupTypeNames : spoilerEntranceGroupNames;

        EntranceOverride* entranceList;

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
            default:
                assert(false);
                return;
        }

        // Begin tracker list
        ImGui::BeginChild("ChildEntranceTrackerLocations", ImVec2(0, -8));
        bool showTo = CVarGetInteger(CVAR_TRACKER_ENTRANCE("ShowTo"), 0);
        bool showFrom = CVarGetInteger(CVAR_TRACKER_ENTRANCE("ShowFrom"), 0);
        bool collapseUndiscovered = CVarGetInteger(CVAR_TRACKER_ENTRANCE("CollapseUndiscovered"), 0);
        bool highlightPrevious = CVarGetInteger(CVAR_TRACKER_ENTRANCE("HighlightPrevious"), 0);
        bool highlightAvailable = CVarGetInteger(CVAR_TRACKER_ENTRANCE("HighlightAvailable"), 0);
        bool hideReverse = CVarGetInteger(CVAR_TRACKER_ENTRANCE("HideReverseEntrances"), 1);
        bool autoScrollArea = CVarGetInteger(CVAR_TRACKER_ENTRANCE("AutoScroll"), 0);
        for (size_t i = 0; i < groupCount; i++) {
            std::string groupName = groupNames[i];

            uint16_t entranceCount = gEntranceTrackingData.GroupEntranceCounts[groupType][i];
            uint16_t startIndex = gEntranceTrackingData.GroupOffsets[groupType][i];

            bool doAreaScroll = false;
            int undiscovered = 0;
            std::vector<EntranceOverride> displayEntrances = {};

            // Loop over entrances first for filtering
            for (size_t entranceIdx = 0; entranceIdx < entranceCount; entranceIdx++) {
                size_t trueIdx = entranceIdx + startIndex;

                EntranceOverride entrance = entranceList[trueIdx];

                const EntranceData* original = GetEntranceData(entrance.index);
                const EntranceData* override = GetEntranceData(entrance.override);

                // If entrance is a dungeon, grotto, or interior entrance, the transition into that area has oneExit
                // set, which means we can filter the return transitions as redundant if entrances are not decoupled, as
                // this is redundant information. Also checks a setting, enabled by default, for hiding them. If all of
                // these conditions are met, we skip adding this entrance to any lists. However, if entrances are
                // decoupled, then all transitions need to be displayed, so we proceed with the filtering
                if ((original->type == ENTRANCE_TYPE_DUNGEON || original->type == ENTRANCE_TYPE_GROTTO ||
                     original->type == ENTRANCE_TYPE_INTERIOR) &&
                    (original->oneExit != 1 && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(
                                                   RSK_DECOUPLED_ENTRANCES) == RO_GENERIC_OFF) &&
                    hideReverse == 1) {
                    continue;
                }

                // Only show blue warps if bluewarp shuffle is on
                if ((original->metaTag.ends_with("bw") || override->metaTag.ends_with("bw")) &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) ==
                        RO_GENERIC_OFF) {
                    continue;
                }

                bool isDiscovered = IsEntranceDiscovered(entrance.index);

                bool showOverride = (!destToggle ? showTo : showFrom) || isDiscovered;
                bool showOriginal = (!destToggle ? showFrom : showTo) || isDiscovered;

                const char* origSrcAreaName = spoilerEntranceGroupNames[original->srcGroup].c_str();
                const char* origTypeName = groupTypeNames[original->type].c_str();
                const char* rplcSrcAreaName = spoilerEntranceGroupNames[override->srcGroup].c_str();
                const char* rplcTypeName = groupTypeNames[override->type].c_str();

                const char* origSrcName = showOriginal ? original->source.c_str() : "";
                const char* rplcDstName = showOverride ? override->destination.c_str() : "";

                // Filter for entrances by group name, type, source/destination names, and meta tags
                if ((!locationSearch.IsActive() && (showOriginal || showOverride || !collapseUndiscovered)) ||
                    ((showOriginal &&
                      (locationSearch.PassFilter(origSrcName) || locationSearch.PassFilter(origSrcAreaName) ||
                       locationSearch.PassFilter(origTypeName) ||
                       locationSearch.PassFilter(original->metaTag.c_str()))) ||
                     (showOverride &&
                      (locationSearch.PassFilter(rplcDstName) || locationSearch.PassFilter(rplcSrcAreaName) ||
                       locationSearch.PassFilter(rplcTypeName) ||
                       locationSearch.PassFilter(override->metaTag.c_str()))))) {

                    // Detect if a scroll should happen and remember the scene for that scroll
                    if (!doAreaScroll &&
                        (lastSceneOrEntranceDetected != LinkIsInArea(original) && LinkIsInArea(original) != -1)) {
                        lastSceneOrEntranceDetected = LinkIsInArea(original);
                        doAreaScroll = true;
                    }

                    displayEntrances.push_back(entrance);
                } else if (!isDiscovered) {
                    undiscovered++;
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

                        bool showOverride = (!destToggle ? showTo : showFrom) || isDiscovered;
                        bool showOriginal = (!destToggle ? showFrom : showTo) || isDiscovered;

                        const char* unknown = "???";

                        const char* origSrcName = showOriginal ? original->source.c_str() : unknown;
                        const char* rplcDstName = showOverride ? override->destination.c_str() : unknown;

                        uint32_t color = isDiscovered ? IM_COL32_WHITE : COLOR_GRAY;

                        // Handle highlighting and auto scroll
                        if ((original->index == lastEntranceIndex ||
                             (override->reverseIndex == lastEntranceIndex &&
                              OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) ==
                                  RO_GENERIC_OFF)) &&
                            highlightPrevious) {
                            color = COLOR_ORANGE;
                        } else if (LinkIsInArea(original) != -1) {
                            if (highlightAvailable) {
                                color = COLOR_GREEN;
                            }

                            if (doAreaScroll) {
                                doAreaScroll = false;
                                if (autoScrollArea) {
                                    ImGui::SetScrollHereY(0.0f);
                                }
                            }
                        }

                        ImGui::PushStyleColor(ImGuiCol_Text, color);

                        // Use a non-breaking space to keep the arrow from wrapping to a newline by itself
                        ImGui::TextWrapped("%s\u00A0-> %s", origSrcName, rplcDstName);

                        ImGui::PopStyleColor();
                    }

                    // Write collapsed undiscovered info
                    if (!locationSearch.IsActive() && undiscovered > 0) {
                        Spacer(0);
                        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_GRAY);
                        ImGui::TextWrapped("%d Undiscovered", undiscovered);
                        ImGui::PopStyleColor();
                    }

                    Spacer(0);
                    ImGui::TreePop();
                }
            }
        }
        ImGui::EndChild();
    }
    Trackers::EndFloatWindows();
}

void EntranceTrackerWindow::InitElement() {
    // Setup hooks for loading and clearing the entrance tracker data
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(
        [](int32_t fileNum) { InitEntranceTrackingData(); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>(
        [](int32_t fileNum) { ClearEntranceTrackingData(); });
}

void RegisterCheckTrackerWidgets() {
    backgroundColorWidget = { .name = "Background Color##EntranceTracker",
                              .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    backgroundColorWidget.CVar(CVAR_TRACKER_ENTRANCE("BgColor"))
        .Options(
            ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_Bg_Default).UseAlpha().ShowReset().ShowRandom());
    SohGui::GetSohMenu()->AddSearchWidget(
        { backgroundColorWidget, "Randomizer", "Entrance Tracker", "General Settings" });

    windowTypeWidget = { .name = "Window Type##EntranceTracker", .type = WidgetType::WIDGET_CVAR_COMBOBOX };
    windowTypeWidget.CVar(CVAR_TRACKER_ENTRANCE("WindowType"))
        .Options(ComboboxOptions()
                     .DefaultIndex(TRACKER_WINDOW_WINDOW)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Far)
                     .Color(THEME_COLOR)
                     .ComboMap(windowType));
    SohGui::GetSohMenu()->AddSearchWidget({ windowTypeWidget, "Randomizer", "Entrance Tracker", "General Settings" });
}

static RegisterMenuInitFunc menuInitFunc(RegisterCheckTrackerWidgets);
} // namespace EntranceTracker

namespace Trackers {
// Windowing stuff
bool BeginFloatWindows(std::string UniqueName, bool& open, Color_RGBA8& bgCol, TrackerWindowType windowType,
                       bool draggable, ImGuiWindowFlags flags) {
    ImGuiWindowFlags windowFlags = flags;

    if (windowFlags == 0) {
        windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoFocusOnAppearing;
    }

    if (windowType == TRACKER_WINDOW_FLOATING) {
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!draggable) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }
    }
    auto maybeParent = ImGui::GetCurrentWindow();
    ImGuiWindow* window = ImGui::FindWindowByName(UniqueName.c_str());
    ImVec4 bgColVec = VecFromRGBA8(bgCol);
    if (window != NULL && window->DockTabIsVisible && window->ParentWindow != NULL &&
        std::string(window->ParentWindow->Name).compare(0, strlen("Main - Deck"), "Main - Deck") == 0) {
        bgColVec.w = 1.0f;
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColVec);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    return ImGui::Begin(UniqueName.c_str(), &open, windowFlags);
}

void EndFloatWindows() {
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
} // namespace Trackers
} // namespace Trackers

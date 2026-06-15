#include "randomizer_entrance_tracker.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "soh/SohGui/SohGui.hpp"

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/randomizer/randomizer.h"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;

#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/randomizer_grotto.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
}

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "entrance.h"

using namespace UIWidgets;

#define COLOR_ORANGE IM_COL32(230, 159, 0, 255)
#define COLOR_GREEN IM_COL32(0, 158, 115, 255)
#define COLOR_GRAY IM_COL32(155, 155, 155, 255)

#define ENTRANCE_VIEW_LIST 0
#define ENTRANCE_VIEW_GRAPH 1

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

// ---- Flower/petal graph view state ----
struct GraphPetal {
    s16 entranceIndex;
    s16 overrideIndex;
    SpoilerEntranceGroup area;
    ImVec2 localOffset; // radial offset from the flower center, in world units
    float angle;
    const EntranceData* srcData;
    const EntranceData* dstData;
};

struct GraphFlower {
    SpoilerEntranceGroup area;
    ImVec2 center; // world-space position set by layout
    int petalStart;
    int petalCount;
    float radius; // petal ring radius
};

struct GraphEdge {
    int srcPetal;        // index into gGraphPetals
    int dstPetal;        // index into gGraphPetals, or -1 if the destination isn't a source petal
    SpoilerEntranceGroup dstArea; // fallback hub to point at when dstPetal == -1
};

static std::vector<GraphFlower> gGraphFlowers;
static std::vector<GraphPetal> gGraphPetals;
static std::vector<GraphEdge> gGraphEdges;
static bool gGraphBuilt = false;
static bool gLayoutDone = false;
static ImVec2 gGraphPan = ImVec2(0.0f, 0.0f);
static float gGraphZoom = 1.0f;
static int gSelectedEdge = -1; // index into gGraphEdges, persists until clicked elsewhere

static const float kPi = 3.14159265358979323846f;

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
    "Gerudo Fortress",
    "Haunted Wasteland",
    "Desert Colossus",
    "Market",
    "Hyrule Castle",
};

static std::string groupTypeNames[] = {
    "One Way", "Overworld", "Interior", "Fortress", "Grotto", "Dungeon",
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

    { ENTR_KAKARIKO_VILLAGE_OWL_DROP, -1, SINGLE_SCENE_INFO(SCENE_DEATH_MOUNTAIN_TRAIL), "DMT Owl Flight", "Kakariko Village Owl Drop", ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},
    { ENTR_HYRULE_FIELD_OWL_DROP,     -1, SINGLE_SCENE_INFO(SCENE_LAKE_HYLIA),           "LH Owl Flight",  "Hyrule Field Owl Drop",     ENTRANCE_GROUP_ONE_WAY, ENTRANCE_GROUP_ONE_WAY, ENTRANCE_TYPE_ONE_WAY},

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
    { ENTR_GERUDOS_FORTRESS_2,                              ENTR_THIEVES_HIDEOUT_1,                               {{ SCENE_THIEVES_HIDEOUT, 2 }},                  "TH 1 Torch Cell",                 "GF Near Grotto East",             ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_GERUDOS_FORTRESS_3,                              ENTR_THIEVES_HIDEOUT_2,                               {{ SCENE_THIEVES_HIDEOUT, 3 }},                  "TH Kitchen Corridor Lower",       "GF Near Grotto North",            ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
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
    { ENTR_THIEVES_HIDEOUT_1,                               ENTR_GERUDOS_FORTRESS_2,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Near Grotto East",             "TH 1 Torch Cell",                 ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
    { ENTR_THIEVES_HIDEOUT_2,                               ENTR_GERUDOS_FORTRESS_3,                              SINGLE_SCENE_INFO(SCENE_GERUDOS_FORTRESS),       "GF Near Grotto North",            "TH Kitchen Corridor Lower",       ENTRANCE_GROUP_GERUDO_FORTRESS, ENTRANCE_GROUP_GERUDO_FORTRESS,   ENTRANCE_TYPE_FORTRESS},
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

    // clang-format on
};

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

void LoadFromPreset(nlohmann::json info) {
    presetLoaded = true;
    presetPos = { info["pos"]["x"], info["pos"]["y"] };
    presetSize = { info["size"]["width"], info["size"]["height"] };
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
    gEntranceTrackingData = { 0 };
    gGraphBuilt = false;
    gLayoutDone = false;
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

    // A new seed's data invalidates the cached graph model (and its layout).
    gGraphBuilt = false;
    gLayoutDone = false;
}

// Compute the highlight color for an entrance, mirroring the list view's coloring rules so the
// two views always agree. White = discovered, gray = undiscovered, orange = last used,
// green = available in Link's current area.
static ImU32 GetEntranceStateColor(const EntranceData* src, const EntranceData* dst, bool highlightPrev,
                                   bool highlightAvail) {
    bool isDiscovered = IsEntranceDiscovered(src->index);
    ImU32 color = isDiscovered ? IM_COL32_WHITE : COLOR_GRAY;
    bool decoupledOff =
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) == RO_GENERIC_OFF;
    if ((src->index == lastEntranceIndex || (dst->reverseIndex == lastEntranceIndex && decoupledOff)) &&
        highlightPrev) {
        color = COLOR_ORANGE;
    } else if (LinkIsInArea(src) != -1) {
        if (highlightAvail) {
            color = COLOR_GREEN;
        }
    }
    return color;
}

// Deterministic force-directed placement of the (<=21) flower centers. Runs once and is cached
// via gLayoutDone; recomputed only on a new seed or an explicit layout reset.
static void LayoutGraph() {
    int n = (int)gGraphFlowers.size();
    if (n == 0) {
        gLayoutDone = true;
        return;
    }

    const float R0 = 600.0f;
    for (int i = 0; i < n; i++) {
        if (gGraphFlowers[i].area == ENTRANCE_GROUP_ONE_WAY) {
            gGraphFlowers[i].center = ImVec2(0.0f, 0.0f); // pinned at origin
        } else {
            float a = (2.0f * kPi * i) / (float)n;
            gGraphFlowers[i].center = ImVec2(R0 * cosf(a), R0 * sinf(a));
        }
    }

    // Map area -> flower index, then accumulate cross-area edge weights for spring attraction.
    int flowerIdxByArea[SPOILER_ENTRANCE_GROUP_COUNT];
    for (int i = 0; i < SPOILER_ENTRANCE_GROUP_COUNT; i++) {
        flowerIdxByArea[i] = -1;
    }
    for (int i = 0; i < n; i++) {
        flowerIdxByArea[gGraphFlowers[i].area] = i;
    }

    std::vector<std::vector<float>> adj(n, std::vector<float>(n, 0.0f));
    for (const auto& e : gGraphEdges) {
        SpoilerEntranceGroup srcArea = gGraphPetals[e.srcPetal].area;
        SpoilerEntranceGroup dstArea = (e.dstPetal >= 0) ? gGraphPetals[e.dstPetal].area : e.dstArea;
        if (srcArea == dstArea) {
            continue;
        }
        int si = flowerIdxByArea[srcArea];
        int di = flowerIdxByArea[dstArea];
        if (si < 0 || di < 0) {
            continue;
        }
        adj[si][di] += 1.0f;
        adj[di][si] += 1.0f;
    }

    const int kIter = 200;
    const float kRepel = 4.0e5f;
    const float kSpring = 0.003f;
    const float kRest = 400.0f;
    const float damping = 0.85f;
    const float maxStep = 60.0f;
    std::vector<ImVec2> vel(n, ImVec2(0.0f, 0.0f));

    for (int it = 0; it < kIter; it++) {
        std::vector<ImVec2> force(n, ImVec2(0.0f, 0.0f));

        // Repulsion between every pair of centers.
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                float dx = gGraphFlowers[i].center.x - gGraphFlowers[j].center.x;
                float dy = gGraphFlowers[i].center.y - gGraphFlowers[j].center.y;
                float d2 = dx * dx + dy * dy + 0.01f;
                float d = sqrtf(d2);
                float f = kRepel / d2;
                float ux = dx / d;
                float uy = dy / d;
                force[i].x += ux * f;
                force[i].y += uy * f;
                force[j].x -= ux * f;
                force[j].y -= uy * f;
            }
        }

        // Spring attraction along area adjacency.
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                float w = adj[i][j];
                if (w <= 0.0f) {
                    continue;
                }
                float dx = gGraphFlowers[j].center.x - gGraphFlowers[i].center.x;
                float dy = gGraphFlowers[j].center.y - gGraphFlowers[i].center.y;
                float d = sqrtf(dx * dx + dy * dy) + 0.01f;
                float f = kSpring * w * (d - kRest);
                float ux = dx / d;
                float uy = dy / d;
                force[i].x += ux * f;
                force[i].y += uy * f;
                force[j].x -= ux * f;
                force[j].y -= uy * f;
            }
        }

        // Integrate with damping; keep the one-way hub pinned.
        for (int i = 0; i < n; i++) {
            if (gGraphFlowers[i].area == ENTRANCE_GROUP_ONE_WAY) {
                continue;
            }
            vel[i].x = (vel[i].x + force[i].x) * damping;
            vel[i].y = (vel[i].y + force[i].y) * damping;
            float vmag = sqrtf(vel[i].x * vel[i].x + vel[i].y * vel[i].y);
            if (vmag > maxStep) {
                vel[i].x *= maxStep / vmag;
                vel[i].y *= maxStep / vmag;
            }
            gGraphFlowers[i].center.x += vel[i].x;
            gGraphFlowers[i].center.y += vel[i].y;
        }
    }

    // Recenter the centroid at the world origin.
    ImVec2 centroid(0.0f, 0.0f);
    for (int i = 0; i < n; i++) {
        centroid.x += gGraphFlowers[i].center.x;
        centroid.y += gGraphFlowers[i].center.y;
    }
    centroid.x /= n;
    centroid.y /= n;
    for (int i = 0; i < n; i++) {
        gGraphFlowers[i].center.x -= centroid.x;
        gGraphFlowers[i].center.y -= centroid.y;
    }

    gLayoutDone = true;
}

// Build the flower/petal graph from the same shuffled-override data the list view uses, applying
// the same hide-reverse and blue-warp filters so the views stay in sync.
static void BuildGraphModel() {
    gGraphFlowers.clear();
    gGraphPetals.clear();
    gGraphEdges.clear();

    bool hideReverse = CVarGetInteger(CVAR_TRACKER_ENTRANCE("HideReverseEntrances"), 1);
    bool decoupled =
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_DECOUPLED_ENTRANCES) == RO_GENERIC_ON;

    for (size_t g = 0; g < SPOILER_ENTRANCE_GROUP_COUNT; g++) {
        uint16_t count = gEntranceTrackingData.GroupEntranceCounts[ENTRANCE_SOURCE_AREA][g];
        uint16_t start = gEntranceTrackingData.GroupOffsets[ENTRANCE_SOURCE_AREA][g];
        if (count == 0) {
            continue;
        }

        GraphFlower flower;
        flower.area = (SpoilerEntranceGroup)g;
        flower.center = ImVec2(0.0f, 0.0f);
        flower.petalStart = (int)gGraphPetals.size();
        flower.petalCount = 0;
        flower.radius = 70.0f;

        for (uint16_t i = 0; i < count; i++) {
            EntranceOverride ov = srcListSortedByArea[start + i];
            const EntranceData* src = GetEntranceData(ov.index);
            const EntranceData* dst = GetEntranceData(ov.override);
            if (src == nullptr || dst == nullptr) {
                continue;
            }

            // Mirror the list's hide-reverse filter for redundant return transitions.
            if ((src->type == ENTRANCE_TYPE_DUNGEON || src->type == ENTRANCE_TYPE_GROTTO ||
                 src->type == ENTRANCE_TYPE_INTERIOR) &&
                (src->oneExit != 1 && !decoupled) && hideReverse) {
                continue;
            }
            // Mirror the list's blue-warp filter.
            if (src->metaTag.ends_with("bw") || dst->metaTag.ends_with("bw")) {
                continue;
            }

            GraphPetal p;
            p.entranceIndex = ov.index;
            p.overrideIndex = ov.override;
            p.area = (SpoilerEntranceGroup)g;
            p.localOffset = ImVec2(0.0f, 0.0f);
            p.angle = 0.0f;
            p.srcData = src;
            p.dstData = dst;
            gGraphPetals.push_back(p);
            flower.petalCount++;
        }

        if (flower.petalCount == 0) {
            continue;
        }
        gGraphFlowers.push_back(flower);
    }

    // Resolve which petal each override lands on (for edge endpoints).
    std::unordered_map<s16, int> entranceIndexToPetal;
    for (int i = 0; i < (int)gGraphPetals.size(); i++) {
        entranceIndexToPetal[gGraphPetals[i].entranceIndex] = i;
    }

    // Deterministic radial petal placement around each flower center. Keep the ring close to the
    // hub so petals read as belonging to their node.
    for (auto& f : gGraphFlowers) {
        int nn = f.petalCount;
        f.radius = std::clamp(34.0f + nn * 3.0f, 34.0f, 120.0f);
        for (int j = 0; j < nn; j++) {
            GraphPetal& p = gGraphPetals[f.petalStart + j];
            float ang = (2.0f * kPi * j) / (float)nn + (float)f.area * 0.3f;
            p.angle = ang;
            p.localOffset = ImVec2(f.radius * cosf(ang), f.radius * sinf(ang));
        }
    }

    // One edge per petal: source petal -> its override-destination petal (or that area's hub).
    for (int i = 0; i < (int)gGraphPetals.size(); i++) {
        GraphPetal& p = gGraphPetals[i];
        GraphEdge e;
        e.srcPetal = i;
        auto it = entranceIndexToPetal.find(p.overrideIndex);
        e.dstPetal = (it != entranceIndexToPetal.end()) ? it->second : -1;
        e.dstArea = p.dstData ? p.dstData->srcGroup : p.area;
        gGraphEdges.push_back(e);
    }

    gSelectedEdge = -1;

    if (!gLayoutDone) {
        LayoutGraph();
    }
    gGraphBuilt = true;
}

void ResetGraphLayout() {
    gLayoutDone = false;
    gGraphBuilt = false;
    gGraphPan = ImVec2(0.0f, 0.0f);
    gGraphZoom = 1.0f;
    gSelectedEdge = -1;
}

void EntranceTrackerWindow::DrawViewModeSelector() {
    CVarRadioButton("List", CVAR_TRACKER_ENTRANCE("ViewMode"), ENTRANCE_VIEW_LIST,
                    RadioButtonsOptions().Color(THEME_COLOR).Tooltip("Show entrances as a searchable text list"));
    ImGui::SameLine();
    CVarRadioButton("Graph", CVAR_TRACKER_ENTRANCE("ViewMode"), ENTRANCE_VIEW_GRAPH,
                    RadioButtonsOptions().Color(THEME_COLOR).Tooltip("Show entrances as a node graph of areas and "
                                                                     "connections"));
}

void EntranceTrackerWindow::DrawGraphView() {
    if (!gGraphBuilt) {
        BuildGraphModel();
    }

    if (gGraphFlowers.empty()) {
        ImGui::TextWrapped("No shuffled entrances to display.");
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    if (canvasSize.x < 50.0f) {
        canvasSize.x = 50.0f;
    }
    if (canvasSize.y < 50.0f) {
        canvasSize.y = 50.0f;
    }
    ImVec2 canvasEnd = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);
    ImVec2 canvasCenter = ImVec2(canvasPos.x + canvasSize.x * 0.5f, canvasPos.y + canvasSize.y * 0.5f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(canvasPos, canvasEnd, IM_COL32(20, 20, 24, 255));

    // Full-canvas input capture for pan/zoom.
    ImGui::InvisibleButton("##graphCanvas", canvasSize,
                           ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    if (active && (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Right))) {
        gGraphPan.x -= io.MouseDelta.x / gGraphZoom;
        gGraphPan.y -= io.MouseDelta.y / gGraphZoom;
    }
    if (hovered && io.MouseWheel != 0.0f) {
        float prevZoom = gGraphZoom;
        gGraphZoom = std::clamp(gGraphZoom * (1.0f + io.MouseWheel * 0.1f), 0.2f, 5.0f);
        // Keep the world point under the cursor fixed while zooming.
        ImVec2 m = io.MousePos;
        ImVec2 worldBefore = ImVec2((m.x - canvasCenter.x) / prevZoom + gGraphPan.x,
                                    (m.y - canvasCenter.y) / prevZoom + gGraphPan.y);
        ImVec2 worldAfter = ImVec2((m.x - canvasCenter.x) / gGraphZoom + gGraphPan.x,
                                   (m.y - canvasCenter.y) / gGraphZoom + gGraphPan.y);
        gGraphPan.x += worldBefore.x - worldAfter.x;
        gGraphPan.y += worldBefore.y - worldAfter.y;
    }

    auto W2S = [&](ImVec2 w) -> ImVec2 {
        return ImVec2((w.x - gGraphPan.x) * gGraphZoom + canvasCenter.x,
                      (w.y - gGraphPan.y) * gGraphZoom + canvasCenter.y);
    };
    auto inCanvas = [&](ImVec2 s) -> bool {
        return s.x >= canvasPos.x && s.x <= canvasEnd.x && s.y >= canvasPos.y && s.y <= canvasEnd.y;
    };
    auto withAlpha = [](ImU32 col, int a) -> ImU32 {
        return (col & 0x00FFFFFFu) | ((ImU32)a << IM_COL32_A_SHIFT);
    };

    bool highlightPrevious = CVarGetInteger(CVAR_TRACKER_ENTRANCE("HighlightPrevious"), 0);
    bool highlightAvailable = CVarGetInteger(CVAR_TRACKER_ENTRANCE("HighlightAvailable"), 0);
    bool showTo = CVarGetInteger(CVAR_TRACKER_ENTRANCE("ShowTo"), 0);
    // By default the graph only reveals entrances the player has discovered, growing as they
    // explore. The spoiler toggle shows the complete mapping up front.
    bool showAll = CVarGetInteger(CVAR_TRACKER_ENTRANCE("GraphShowAll"), 0);

    dl->PushClipRect(canvasPos, canvasEnd, true);

    // Per-frame lookup of each flower's world center by area.
    ImVec2 centerByArea[SPOILER_ENTRANCE_GROUP_COUNT];
    bool hasFlower[SPOILER_ENTRANCE_GROUP_COUNT] = { false };
    for (const auto& f : gGraphFlowers) {
        centerByArea[f.area] = f.center;
        hasFlower[f.area] = true;
    }

    auto petalWorld = [&](const GraphPetal& p) -> ImVec2 {
        ImVec2 c = centerByArea[p.area];
        return ImVec2(c.x + p.localOffset.x, c.y + p.localOffset.y);
    };
    auto distToSeg = [](ImVec2 p, ImVec2 a, ImVec2 b) -> float {
        float vx = b.x - a.x, vy = b.y - a.y;
        float wx = p.x - a.x, wy = p.y - a.y;
        float c1 = vx * wx + vy * wy;
        if (c1 <= 0.0f) {
            return sqrtf(wx * wx + wy * wy);
        }
        float c2 = vx * vx + vy * vy;
        if (c2 <= c1) {
            float dx = p.x - b.x, dy = p.y - b.y;
            return sqrtf(dx * dx + dy * dy);
        }
        float t = c1 / c2;
        float dx = p.x - (a.x + t * vx), dy = p.y - (a.y + t * vy);
        return sqrtf(dx * dx + dy * dy);
    };

    // Visibility: which petals/areas are revealed. Once a source entrance is discovered we also
    // reveal the destination area's hub so the connection has somewhere to land.
    std::vector<bool> petalVisible(gGraphPetals.size(), false);
    bool areaVisible[SPOILER_ENTRANCE_GROUP_COUNT] = { false };
    int visibleCount = 0;
    for (int i = 0; i < (int)gGraphPetals.size(); i++) {
        bool vis = showAll || IsEntranceDiscovered(gGraphPetals[i].entranceIndex);
        petalVisible[i] = vis;
        if (vis) {
            areaVisible[gGraphPetals[i].area] = true;
            visibleCount++;
        }
    }
    for (const auto& e : gGraphEdges) {
        if (!petalVisible[e.srcPetal]) {
            continue;
        }
        if (e.dstPetal >= 0 && petalVisible[e.dstPetal]) {
            areaVisible[gGraphPetals[e.dstPetal].area] = true;
        } else if (hasFlower[e.dstArea]) {
            areaVisible[e.dstArea] = true;
        }
    }

    // Resolve an edge's screen-space endpoints (and whether it should be shown at all).
    auto edgeScreen = [&](const GraphEdge& e, ImVec2& p0, ImVec2& p1) -> bool {
        if (!petalVisible[e.srcPetal]) {
            return false;
        }
        ImVec2 dstW;
        if (e.dstPetal >= 0 && petalVisible[e.dstPetal]) {
            dstW = petalWorld(gGraphPetals[e.dstPetal]);
        } else if (hasFlower[e.dstArea] && areaVisible[e.dstArea]) {
            dstW = centerByArea[e.dstArea];
        } else {
            return false;
        }
        p0 = W2S(petalWorld(gGraphPetals[e.srcPetal]));
        p1 = W2S(dstW);
        return true;
    };

    // (a) Edges first (behind nodes), tracking the nearest one under the cursor.
    int hoveredEdge = -1;
    float hoveredEdgeDist = 7.0f;
    for (int i = 0; i < (int)gGraphEdges.size(); i++) {
        const GraphEdge& e = gGraphEdges[i];
        ImVec2 p0, p1;
        if (!edgeScreen(e, p0, p1)) {
            continue;
        }
        if (!inCanvas(p0) && !inCanvas(p1)) {
            continue; // both endpoints off-canvas
        }
        const GraphPetal& sp = gGraphPetals[e.srcPetal];
        ImU32 color = GetEntranceStateColor(sp.srcData, sp.dstData, highlightPrevious, highlightAvailable);
        int alpha = (color == COLOR_GRAY) ? 45 : 130;
        float thickness = std::max(1.0f, 1.4f * gGraphZoom);
        dl->AddLine(p0, p1, withAlpha(color, alpha), thickness);

        if (hovered) {
            float d = distToSeg(io.MousePos, p0, p1);
            if (d < hoveredEdgeDist) {
                hoveredEdgeDist = d;
                hoveredEdge = i;
            }
        }
    }

    // Redraw the selected and hovered edges on top, brighter and thicker, so overlapping lines
    // can be disambiguated.
    auto emphasizeEdge = [&](int idx, int alpha, float extra) {
        if (idx < 0 || idx >= (int)gGraphEdges.size()) {
            return;
        }
        ImVec2 p0, p1;
        if (!edgeScreen(gGraphEdges[idx], p0, p1)) {
            return;
        }
        const GraphPetal& sp = gGraphPetals[gGraphEdges[idx].srcPetal];
        ImU32 color = GetEntranceStateColor(sp.srcData, sp.dstData, highlightPrevious, highlightAvailable);
        dl->AddLine(p0, p1, withAlpha(color, alpha), std::max(2.0f, 1.4f * gGraphZoom) + extra);
        dl->AddCircleFilled(p0, std::max(3.0f, 4.0f * gGraphZoom), withAlpha(color, alpha));
        dl->AddCircleFilled(p1, std::max(3.0f, 4.0f * gGraphZoom), withAlpha(color, alpha));
    };
    if (gSelectedEdge != hoveredEdge) {
        emphasizeEdge(gSelectedEdge, 200, 1.5f);
    }
    emphasizeEdge(hoveredEdge, 255, 2.5f);

    // (b) Flower centers (hubs) - only revealed areas.
    float hubR = std::max(4.0f, 14.0f * gGraphZoom);
    for (const auto& f : gGraphFlowers) {
        if (!areaVisible[f.area]) {
            continue;
        }
        ImVec2 c = W2S(f.center);
        ImVec2 margin(hubR + f.radius * gGraphZoom, hubR + f.radius * gGraphZoom);
        if (c.x + margin.x < canvasPos.x || c.x - margin.x > canvasEnd.x || c.y + margin.y < canvasPos.y ||
            c.y - margin.y > canvasEnd.y) {
            continue;
        }
        dl->AddCircleFilled(c, hubR, IM_COL32(70, 70, 90, 255));
        dl->AddCircle(c, hubR, IM_COL32(160, 160, 190, 255), 0, std::max(1.0f, 1.5f * gGraphZoom));

        if (gGraphZoom > 0.4f) {
            const char* name = spoilerEntranceGroupNames[f.area].c_str();
            ImVec2 ts = ImGui::CalcTextSize(name);
            dl->AddText(ImVec2(c.x - ts.x * 0.5f, c.y - hubR - ts.y - 2.0f), IM_COL32_WHITE, name);
        }
    }

    // (c) Petals (revealed only), plus hover detection for tooltips.
    float petalR = std::max(2.0f, 6.0f * gGraphZoom);
    int hoveredPetal = -1;
    float hoveredDist = petalR + 4.0f;
    for (int i = 0; i < (int)gGraphPetals.size(); i++) {
        if (!petalVisible[i]) {
            continue;
        }
        const GraphPetal& p = gGraphPetals[i];
        ImVec2 hubScreen = W2S(centerByArea[p.area]);
        ImVec2 pw = W2S(petalWorld(p));
        if (!inCanvas(pw)) {
            continue;
        }
        ImU32 color = GetEntranceStateColor(p.srcData, p.dstData, highlightPrevious, highlightAvailable);
        dl->AddLine(hubScreen, pw, withAlpha(color, 70), std::max(1.0f, gGraphZoom));
        dl->AddCircleFilled(pw, petalR, color);
        dl->AddCircle(pw, petalR, IM_COL32(0, 0, 0, 180), 0, 1.0f);

        if (hovered) {
            float dx = io.MousePos.x - pw.x;
            float dy = io.MousePos.y - pw.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist <= petalR + 4.0f && dist < hoveredDist) {
                hoveredDist = dist;
                hoveredPetal = i;
            }
        }

        if (gGraphZoom > 1.4f) {
            dl->AddText(ImVec2(pw.x + petalR + 2.0f, pw.y - ImGui::GetTextLineHeight() * 0.5f),
                        withAlpha(IM_COL32_WHITE, 200), p.srcData->source.c_str());
        }
    }

    dl->PopClipRect();

    if (visibleCount == 0) {
        const char* hint = "Explore to reveal entrances";
        ImVec2 ts = ImGui::CalcTextSize(hint);
        dl->AddText(ImVec2(canvasCenter.x - ts.x * 0.5f, canvasCenter.y - ts.y * 0.5f),
                    IM_COL32(170, 170, 170, 255), hint);
    }

    // Click without dragging selects the edge under the cursor (or clears the selection).
    // Petals take priority, so clicking a node never selects an edge behind it.
    if (hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        ImVec2 dd = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
        if (fabsf(dd.x) < 3.0f && fabsf(dd.y) < 3.0f) {
            gSelectedEdge = (hoveredPetal < 0) ? hoveredEdge : -1;
        }
    }

    // Tooltip: nearest hovered petal first, otherwise the hovered (or selected) edge.
    if (hoveredPetal >= 0) {
        const GraphPetal& p = gGraphPetals[hoveredPetal];
        bool isDiscovered = IsEntranceDiscovered(p.entranceIndex);
        const char* dstName = (isDiscovered || showTo) ? p.dstData->destination.c_str() : "???";
        ImGui::BeginTooltip();
        ImGui::Text("%s -> %s", p.srcData->source.c_str(), dstName);
        ImGui::EndTooltip();
    } else {
        int infoEdge = (hoveredEdge >= 0) ? hoveredEdge : -1;
        if (infoEdge >= 0) {
            const GraphPetal& sp = gGraphPetals[gGraphEdges[infoEdge].srcPetal];
            bool isDiscovered = IsEntranceDiscovered(sp.entranceIndex);
            const char* dstName = (isDiscovered || showTo) ? sp.dstData->destination.c_str() : "???";
            ImGui::BeginTooltip();
            ImGui::Text("%s -> %s", sp.srcData->source.c_str(), dstName);
            ImGui::EndTooltip();
        }
    }
}

void EntranceTrackerSettingsWindow::DrawElement() {

    ImGui::TextWrapped("The entrance tracker will only track shuffled entrances");
    Spacer(0);

    ImGui::TableNextColumn();
    SohGui::GetSohMenu()->MenuDrawItem(backgroundColorWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

    SohGui::GetSohMenu()->MenuDrawItem(windowTypeWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);

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

    ImGui::Text("Graph View");
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
    CVarCheckbox("Show all entrances (spoiler)", CVAR_TRACKER_ENTRANCE("GraphShowAll"),
                 CheckboxOptions()
                     .Tooltip("Reveal the entire entrance graph immediately instead of only entrances you have "
                              "discovered")
                     .Color(THEME_COLOR));
    ImGui::EndDisabled();
    if (Button("Reset Graph Layout",
               ButtonOptions({ { .tooltip = "Recompute the node layout and reset pan/zoom" } })
                   .Color(THEME_COLOR)
                   .Size(Sizes::Inline))) {
        ResetGraphLayout();
    }

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Legend")) {
        ImGui::TextColored(ImColor(COLOR_ORANGE), "Last Entrance");
        ImGui::TextColored(ImColor(COLOR_GREEN), "Available Entrances");
        ImGui::TextColored(ImColor(COLOR_GRAY), "Undiscovered Entrances");
        ImGui::TextWrapped("Graph view: each area is a hub, its entrances are nodes around it, and "
                           "lines connect each entrance to where it leads. Drag to pan, scroll to zoom.");
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

        DrawViewModeSelector();

        if (CVarGetInteger(CVAR_TRACKER_ENTRANCE("ViewMode"), ENTRANCE_VIEW_LIST) == ENTRANCE_VIEW_GRAPH) {
            DrawGraphView();
        } else {
            DrawListView();
        }
    }
    Trackers::EndFloatWindows();
}

void EntranceTrackerWindow::DrawListView() {
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

                // RANDOTODO: Only show blue warps if bluewarp shuffle is on
                if (original->metaTag.ends_with("bw") || override->metaTag.ends_with("bw")) {
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

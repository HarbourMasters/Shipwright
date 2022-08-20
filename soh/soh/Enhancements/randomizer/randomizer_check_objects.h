#pragma once
#include "randomizerTypes.h"
#include <string>
#include <vector>
#include <map>

// Check types based on main settings
typedef enum {
    RCTYPE_STANDARD, // Base set of rando checks
    RCTYPE_SKULL_TOKEN, // Gold Skulltulas
    RCTYPE_COW, // Cows
    RCTYPE_ADULT_TRADE, // Adult trade quest checks
    RCTYPE_FROG_SONG, // Frog song purple rupee checks
    RCTYPE_MAP_COMPASS, // Maps/Compasses
    RCTYPE_SMALL_KEY, // Small Keys
    RCTYPE_GF_KEY, // Gerudo Fortress Keys
    RCTYPE_BOSS_KEY, // Boss Keys
    RCTYPE_GANON_BOSS_KEY, // Ganon's boss key
    RCTYPE_SHOP, // shops/scrubs
    RCTYPE_CHEST_GAME, //todo replace this once we implement it, just using it to exclude for now
    RCTYPE_LINKS_POCKET, //todo this feels hacky
    RCTYPE_GOSSIP_STONE,
} RandomizerCheckType;

typedef enum {
    RCVORMQ_VANILLA,
    RCVORMQ_MQ,
    RCVORMQ_BOTH
} RandomizerCheckVanillaOrMQ;

typedef enum {
    RCAREA_KOKIRI_FOREST,
    RCAREA_LOST_WOODS,
    RCAREA_SACRED_FOREST_MEADOW,
    RCAREA_HYRULE_FIELD,
    RCAREA_LAKE_HYLIA,
    RCAREA_GERUDO_VALLEY,
    RCAREA_GERUDO_FORTRESS,
    RCAREA_WASTELAND,
    RCAREA_DESERT_COLOSSUS,
    RCAREA_MARKET,
    RCAREA_HYRULE_CASTLE,
    RCAREA_KAKARIKO_VILLAGE,
    RCAREA_GRAVEYARD,
    RCAREA_DEATH_MOUNTAIN_TRAIL,
    RCAREA_GORON_CITY,
    RCAREA_DEATH_MOUNTAIN_CRATER,
    RCAREA_ZORAS_RIVER,
    RCAREA_ZORAS_DOMAIN,
    RCAREA_ZORAS_FOUNTAIN,
    RCAREA_LON_LON_RANCH,
    RCAREA_DEKU_TREE,
    RCAREA_DODONGOS_CAVERN,
    RCAREA_JABU_JABUS_BELLY,
    RCAREA_FOREST_TEMPLE,
    RCAREA_FIRE_TEMPLE,
    RCAREA_WATER_TEMPLE,
    RCAREA_SPIRIT_TEMPLE,
    RCAREA_SHADOW_TEMPLE,
    RCAREA_BOTTOM_OF_THE_WELL,
    RCAREA_ICE_CAVERN,
    RCAREA_GERUDO_TRAINING_GROUND,
    RCAREA_GANONS_CASTLE,
    RCAREA_INVALID
} RandomizerCheckArea;

typedef struct {
    RandomizerCheck rc;
    RandomizerCheckVanillaOrMQ vOrMQ;
    RandomizerCheckType rcType;
    RandomizerCheckArea rcArea;
    bool visibleInImgui;
    std::string rcShortName;
    std::string rcSpoilerName;
} RandomizerCheckObject;

namespace RandomizerCheckObjects {
    bool AreaIsDungeon(RandomizerCheckArea area);
    bool AreaIsOverworld(RandomizerCheckArea area);
    std::string GetRCAreaName(RandomizerCheckArea area);
    std::map<RandomizerCheckArea, std::vector<RandomizerCheckObject>> GetAllRCObjects();
    void UpdateImGuiVisibility();
}

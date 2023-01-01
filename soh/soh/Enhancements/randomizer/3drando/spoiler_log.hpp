#pragma once

#include <array>
#include <string>
#include <string_view>

using RandomizerHash = std::array<std::string, 5>;


typedef enum {
    SPOILER_CHK_NONE,
    SPOILER_CHK_ALWAYS_COLLECTED,
    SPOILER_CHK_CHEST,
    SPOILER_CHK_COLLECTABLE,
    SPOILER_CHK_GOLD_SKULLTULA,
    SPOILER_CHK_ITEM_GET_INF,
    SPOILER_CHK_EVENT_CHK_INF,
    SPOILER_CHK_INF_TABLE,
    SPOILER_CHK_COW,
    SPOILER_CHK_MINIGAME,
    SPOILER_CHK_SCRUB,
    SPOILER_CHK_BIGGORON,
    SPOILER_CHK_GERUDO_MEMBERSHIP_CARD,
    SPOILER_CHK_POE_POINTS,
    SPOILER_CHK_SHOP_ITEM,
    SPOILER_CHK_MAGIC_BEANS,
    SPOILER_CHK_MERCHANT,
    SPOILER_CHK_GRAVEDIGGER,
    SPOILER_CHK_RANDOMIZER_INF,
} SpoilerCollectionCheckType;

// Location groups for checks, used to group the checks by logical location
typedef enum {
    GROUP_NO_GROUP,
    GROUP_KOKIRI_FOREST,              // 0x55, 0x28
    GROUP_LOST_WOODS,                 // 0x5B, 0x56
    GROUP_DUNGEON_DEKU_TREE,          // 0x00, 0x11
    GROUP_DUNGEON_FOREST_TEMPLE,      // 0x03
    GROUP_KAKARIKO,                   // 0x37, 0x42, 0x3F, 0x40, 0x41, 0x48, 0x52, 0x53
    GROUP_DUNGEON_BOTTOM_OF_THE_WELL, // 0x08
    GROUP_DUNGEON_SHADOW_TEMPLE,      // 0x07
    GROUP_DEATH_MOUNTAIN,             // 0x60, 0x61
    GROUP_GORON_CITY,                 // 0x62
    GROUP_DUNGEON_DODONGOS_CAVERN,    // 0x01, 0x12
    GROUP_DUNGEON_FIRE_TEMPLE,        // 0x04
    GROUP_ZORAS_RIVER,                // 0x54
    GROUP_ZORAS_DOMAIN,               // 0x58, 0x59
    GROUP_DUNGEON_JABUJABUS_BELLY,    // 0x02, 0x13
    GROUP_DUNGEON_ICE_CAVERN,         // 0x09
    GROUP_HYRULE_FIELD,               // 0x51
    GROUP_LON_LON_RANCH,              // 0x4C
    GROUP_LAKE_HYLIA,                 // 0x57
    GROUP_DUNGEON_WATER_TEMPLE,       // 0x05
    GROUP_GERUDO_VALLEY,              // 0x5A, 0x5D, 0x0C, 0x5E, 0x5C
    GROUP_GERUDO_TRAINING_GROUND,     // 0x0B
    GROUP_DUNGEON_SPIRIT_TEMPLE,      // 0x06
    GROUP_HYRULE_CASTLE,              // 0x10, 0x4B, 0x35, 0x42, 0x4D, 0x5F, 0x4A
    GROUP_DUNGEON_GANONS_CASTLE,      // 0x0A, 0x0D, 0x0E, 0x0F
    SPOILER_COLLECTION_GROUP_COUNT,
    // Grottos are all 0x3E
} SpoilerCollectionCheckGroup;

typedef enum {
    COLLECTTYPE_NORMAL,
    COLLECTTYPE_REPEATABLE,
    COLLECTTYPE_NEVER,
} SpoilerItemCollectType;

typedef enum {
    REVEALTYPE_NORMAL,
    REVEALTYPE_SCENE,
    REVEALTYPE_ALWAYS,
} SpoilerItemRevealType;

#define SPOILER_SPHERES_MAX 50
#define SPOILER_ITEMS_MAX 512
#define SPOILER_STRING_DATA_SIZE 16384

typedef struct {
    std::string LocationStr;
    std::string ItemStr;
    uint16_t LocationStrOffset;
    uint16_t ItemStrOffset;
    SpoilerCollectionCheckType CollectionCheckType;
    uint8_t LocationScene;
    uint8_t LocationFlag;
    SpoilerCollectionCheckGroup Group;
    SpoilerItemCollectType CollectType;
    SpoilerItemRevealType RevealType;
} SpoilerItemLocation;

typedef struct {
    uint8_t ItemCount;
    uint16_t ItemLocationsOffset;
} SpoilerSphere;

typedef struct {
    uint8_t SphereCount;
    uint16_t ItemLocationsCount;
    SpoilerSphere Spheres[SPOILER_SPHERES_MAX];
    SpoilerItemLocation ItemLocations[SPOILER_ITEMS_MAX];
    uint16_t SphereItemLocations[SPOILER_ITEMS_MAX];
    char StringData[SPOILER_STRING_DATA_SIZE];
    uint16_t GroupItemCounts[SPOILER_COLLECTION_GROUP_COUNT];
    uint16_t GroupOffsets[SPOILER_COLLECTION_GROUP_COUNT];
} SpoilerData;

void GenerateHash();
const RandomizerHash& GetRandomizerHash();

void WriteIngameSpoilerLog();

const char* SpoilerLog_Write(int language);
const SpoilerData& GetSpoilerData();
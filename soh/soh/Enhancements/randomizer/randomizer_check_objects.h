#pragma once
#include "randomizerTypes.h"
#include <string>
#include <unordered_map>

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
    RCTYPE_GANON_BOSS_KEY
} RandomizerCheckType;

typedef enum {
    RCVORMQ_VANILLA,
    RCVORMQ_MQ,
    RCVORMQ_BOTH
} RandomizerCheckVanillaOrMQ;

typedef enum {
    RCAREA_KOKIRI_FOREST,
} RandomizerCheckArea;

typedef struct {
    RandomizerCheck rc;
    RandomizerCheckVanillaOrMQ vOrMQ;
    RandomizerCheckType rcType;
    RandomizerCheckArea rcArea;
    std::string rcShortName;
    std::string rcSpoilerName;
} RandomizerCheckObject;

namespace RandomizerCheckObjects {
    std::unordered_map<RandomizerCheck, RandomizerCheckObject> GetAllRCObjects();
    RandomizerCheckObject GetRCObject(RandomizerCheck check);

}
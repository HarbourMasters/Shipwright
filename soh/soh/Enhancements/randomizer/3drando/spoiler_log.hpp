#pragma once

#include <array>
#include <string>
#include <string_view>
#include <cstdint>
#include "../randomizerTypes.h"

using RandomizerHash = std::array<std::string, 5>;


typedef enum {
    SPOILER_CHK_NONE,
    SPOILER_CHK_CHEST,
    SPOILER_CHK_COLLECTABLE,
    SPOILER_CHK_GOLD_SKULLTULA,
    SPOILER_CHK_ITEM_GET_INF,
    SPOILER_CHK_EVENT_CHK_INF,
    SPOILER_CHK_INF_TABLE,
    SPOILER_CHK_GRAVEDIGGER,
    SPOILER_CHK_RANDOMIZER_INF,
} SpoilerCollectionCheckType;

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
#define SPOILER_ITEMS_MAX RC_MAX
#define SPOILER_STRING_DATA_SIZE 16384

typedef struct {
    std::string LocationStr;
    std::string ItemStr;
    uint16_t LocationStrOffset;
    uint16_t ItemStrOffset;
    SpoilerCollectionCheckType CollectionCheckType;
    uint8_t LocationScene;
    uint8_t LocationFlag;
    RandomizerCheckArea Area;
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
    uint16_t AreaItemCounts[RCAREA_INVALID];
    uint16_t AreaOffsets[RCAREA_INVALID];
} SpoilerData;

void GenerateHash();
const RandomizerHash& GetRandomizerHash();

void WriteIngameSpoilerLog();

const char* SpoilerLog_Write();
const SpoilerData& GetSpoilerData();
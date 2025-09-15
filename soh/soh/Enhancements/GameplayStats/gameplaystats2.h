#pragma once

#include "soh/cvar_prefixes.h"
#include <stdint.h>
#include <string>
#include "imgui.h"

extern "C" {
uint64_t GetUnixTimestamp(void);
};

// New

typedef enum {
    TIMESTAMP_DEFEAT_GOHMA = 0xA0,
    TIMESTAMP_DEFEAT_KING_DODONGO,
    TIMESTAMP_DEFEAT_BARINADE,
    TIMESTAMP_DEFEAT_PHANTOM_GANON,
    TIMESTAMP_DEFEAT_VOLVAGIA,
    TIMESTAMP_DEFEAT_MORPHA,
    TIMESTAMP_DEFEAT_BONGO_BONGO,
    TIMESTAMP_DEFEAT_TWINROVA,
    TIMESTAMP_DEFEAT_GANONDORF,
    TIMESTAMP_DEFEAT_GANON,
    TIMESTAMP_BOSSRUSH_FINISH,
    TIMESTAMP_TRIFORCE_COMPLETED,
} GameplayStatTimestamp;

typedef enum {
    STAT_TYPE_SCENE,
    STAT_TYPE_ITEM,
    STAT_TYPE_EVENT,
    STAT_TYPE_ALL,
} GameplayStatEntryTypes;

typedef enum {
    STAT_ACTION_SAVE,
    STAT_ACTION_LOAD,
    STAT_ACTION_DELETE,
} GameplayStatsActions;

typedef struct {
    uint32_t entryType;
    std::string entryName;
    ImVec4 entryColor;
    uint32_t entryTimestamp;
} GameplayStatObject;

extern void GameplayStats_AddTimestamp(uint32_t entryId, uint32_t entryType);

// End
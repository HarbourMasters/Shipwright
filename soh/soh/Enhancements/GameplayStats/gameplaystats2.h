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
    STAT_TYPE_SCENE,
    STAT_TYPE_ITEM,
    STAT_TYPE_EVENT,
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

typedef struct {
    std::string entryName;
    ImVec4 entryColor;
} GameplayStatEntry;

extern void GameplayStats_AddTimestamp(uint32_t entryId, uint32_t entryType);

// End
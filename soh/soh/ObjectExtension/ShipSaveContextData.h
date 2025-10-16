#ifndef SHIPSAVECONTEXTDATA_H
#define SHIPSAVECONTEXTDATA_H

#include "soh/Enhancements/boss-rush/BossRush.h"
#include "soh/Enhancements/gameplaystats.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "z64save.h"
#ifdef __cplusplus
}
#endif

struct ShipRandomizerSaveContextData {
    u8 triforcePiecesCollected;
};

struct ShipBossRushSaveContextData {
    u32 isPaused;
    u8 options[BR_OPTIONS_MAX];
};

union ShipQuestSpecificSaveContextData {
    struct ShipRandomizerSaveContextData randomizer;
    struct ShipBossRushSaveContextData bossRush;
};

struct ShipQuestSaveContextData {
    u8 id;
    union ShipQuestSpecificSaveContextData data;
};

struct SceneTimestamp {
    u16 scene;
    u8 room;
    u32 sceneTime;
    u32 roomTime;
    u8 isRoom;
};

struct SohStats {
    char buildVersion[50];
    s16 buildVersionMajor;
    s16 buildVersionMinor;
    s16 buildVersionPatch;
    u8 heartPieces;
    u8 heartContainers;
    u8 dungeonKeys[19];
    u32 playTimer;
    u32 pauseTimer;
    u32 sceneTimer;
    u32 roomTimer;
    s16 sceneNum;
    s8 roomNum;
    bool gameComplete;
    u32 itemTimestamp[TIMESTAMP_MAX];
    struct SceneTimestamp sceneTimestamps[8191];
    u32 tsIdx;
    u32 count[COUNT_MAX];
    u32 entrancesDiscovered[SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT];
    u32 scenesDiscovered[SAVEFILE_SCENES_DISCOVERED_IDX_COUNT];
    bool rtaTiming;
    uint64_t fileCreatedAt;
};

struct ShipSaveContextData {
    u16 pendingSale;
    u16 pendingSaleMod;
    u8 pendingIceTrapCount;
    struct SohStats stats;
    FaroresWindData backupFW;
    struct ShipQuestSaveContextData quest;
    u8 maskMemory;
    u8 filenameLanguage;
    // TODO: Move non-rando specific flags to a new sohInf and move the remaining randomizerInf to
    // ShipRandomizerSaveContextData
    u16 randomizerInf[(RAND_INF_MAX + 15) / 16];
};

#ifdef __cplusplus
extern "C" {
#endif
uint8_t IsRando();
uint8_t IsBossRush();
uint8_t IsVanilla();
uint8_t IsMasterQuest();
struct ShipSaveContextData* GetGlobalShipSaveContextData();
void ShipSaveContextData_Init(SaveContext* saveContext);
void ShipSaveContextData_Free(SaveContext* saveContext);
#ifdef __cplusplus
ShipSaveContextData* GetShipSaveContextData(SaveContext* saveContext = nullptr);
}
#endif

#endif
#ifndef _RANDO_ENTRANCE_H_
#define _RANDO_ENTRANCE_H_

#include <stdint.h>

//Entrance Table Data: https://wiki.cloudmodding.com/oot/Entrance_Table_(Data)
//Accessed June 2021, published content date at the time was 14 March 2020, at 21:47

#define ENTRANCE_TABLE_SIZE ENTR_MAX

#define MAX_ENTRANCE_RANDO_USED_INDEX 0x0820
#define ENTRANCE_OVERRIDES_MAX_COUNT 267 // 19 one-way entrances + 124 two-way entrances (x2)
#define SHUFFLEABLE_BOSS_COUNT 8

#define SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT 66 // Max entrance rando index is 0x0820, (2080 / 32 == 65) + 1
#define SAVEFILE_SCENES_DISCOVERED_IDX_COUNT 4 // Max scene ID is 0x6E, (110 / 32 == 3) + 1

#define ENTRANCE_INFO_FIELD(continueBgm, displayTitleCard, endTransType, startTransType)               \
    (((continueBgm) ? ENTRANCE_INFO_CONTINUE_BGM_FLAG : 0) |                                           \
     ((displayTitleCard) ? ENTRANCE_INFO_DISPLAY_TITLE_CARD_FLAG : 0) |                                \
     (((endTransType) << ENTRANCE_INFO_END_TRANS_TYPE_SHIFT) & ENTRANCE_INFO_END_TRANS_TYPE_MASK) |    \
     (((startTransType) << ENTRANCE_INFO_START_TRANS_TYPE_SHIFT) & ENTRANCE_INFO_START_TRANS_TYPE_MASK))

typedef struct {
    uint16_t type;
    int16_t index;
    int16_t destination;
    int16_t override;
    int16_t overrideDestination;
} EntranceOverride;

#ifdef __cplusplus
extern "C" {
#endif

void Entrance_Init(void);
void Entrance_ResetEntranceTable(void);
uint8_t Entrance_EntranceIsNull(EntranceOverride* entranceOverride);
int16_t Entrance_GetOverride(int16_t index);
int16_t Entrance_OverrideNextIndex(int16_t nextEntranceIndex);
int16_t Entrance_PeekNextIndexOverride(int16_t nextEntranceIndex);
int16_t Entrance_OverrideDynamicExit(int16_t dynamicExitIndex);
uint32_t Entrance_SceneAndSpawnAre(uint8_t scene, uint8_t spawn);
void Entrance_SetGameOverEntrance(void);
void Entrance_SetSavewarpEntrance(void);
void Entrance_SetWarpSongEntrance(void);
void Entrance_OverrideBlueWarp(void);
void Entrance_HandleEponaState(void);
void Entrance_OverrideWeatherState(void);
void Entrance_OverrideGeurdoGuardCapture(void);
void Entrance_OverrideSpawnScene(int32_t sceneNum, int32_t spawn);
int32_t Entrance_OverrideSpawnSceneRoom(int32_t sceneNum, int32_t spawn, int32_t room);
void Entrance_EnableFW(void);
uint8_t Entrance_GetIsEntranceDiscovered(uint16_t entranceIndex);
void Entrance_SetEntranceDiscovered(uint16_t entranceIndex, uint8_t isReversedEntrance);
#ifdef __cplusplus
}
#endif

#endif //_RANDO_ENTRANCE_H_

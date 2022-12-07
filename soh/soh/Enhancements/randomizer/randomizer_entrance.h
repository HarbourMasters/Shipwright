#ifndef _RANDO_ENTRANCE_H_
#define _RANDO_ENTRANCE_H_

#include <stdint.h>

//Entrance Table Data: https://wiki.cloudmodding.com/oot/Entrance_Table_(Data)
//Accessed June 2021, published content date at the time was 14 March 2020, at 21:47

#define ENTRANCE_TABLE_SIZE 0x0614

#define DEKU_TREE_ENTRANCE 0x0000
#define DODONGOS_CAVERN_ENTRANCE 0x0004
#define JABU_JABUS_BELLY_ENTRANCE 0x0028
#define FOREST_TEMPLE_ENTRANCE 0x169
#define FIRE_TEMPLE_ENTRANCE 0x165
#define WATER_TEMPLE_ENTRANCE 0x0010
#define SPIRIT_TEMPLE_ENTRANCE 0x0082
#define SHADOW_TEMPLE_ENTRANCE 0x0037
#define BOTTOM_OF_THE_WELL_ENTRANCE 0x0098
#define GERUDO_TRAINING_GROUNDS_ENTRANCE 0x0008
#define ICE_CAVERN_ENTRANCE 0x0088
#define GANONS_CASTLE_ENTRANCE 0x0467
#define LINK_HOUSE_SAVEWARP_ENTRANCE 0x00BB

#define ENTRANCE_RANDO_GROTTO_LOAD_START 0x0700
#define ENTRANCE_RANDO_GROTTO_EXIT_START 0x0800
#define MAX_ENTRANCE_RANDO_USED_INDEX 0x0820

#define ENTRANCE_OVERRIDES_MAX_COUNT 256

#define SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT 66 // Max entrance rando index is 0x0820, (2080 / 32 == 65) + 1
#define SAVEFILE_SCENES_DISCOVERED_IDX_COUNT 4 // Max scene ID is 0x6E, (110 / 32 == 3) + 1

typedef struct {
    int16_t index;
    int16_t destination;
    int16_t blueWarp;
    int16_t override;
    int16_t overrideDestination;
} EntranceOverride;

void Entrance_Init(void);
void Entrance_ResetEntranceTable(void);
uint8_t Entrance_EntranceIsNull(EntranceOverride* entranceOverride);
int16_t  Entrance_GetOverride(int16_t index);
int16_t  Entrance_OverrideNextIndex(int16_t nextEntranceIndex);
int16_t  Entrance_OverrideDynamicExit(int16_t dynamicExitIndex);
uint32_t Entrance_SceneAndSpawnAre(uint8_t scene, uint8_t spawn);
void Entrance_SetSavewarpEntrance(void);
void Entrance_SetWarpSongEntrance(void);
void Entrance_OverrideBlueWarp(void);
void Entrance_OverrideCutsceneEntrance(uint16_t cutsceneCmd);
void Entrance_HandleEponaState(void);
void Entrance_OverrideWeatherState(void);
void Entrance_OverrideGeurdoGuardCapture(void);
void Entrance_OverrideSpawnScene(int32_t sceneNum, int32_t spawn);
void Entrance_EnableFW(void);
uint8_t Entrance_GetIsEntranceDiscovered(uint16_t entranceIndex);
void Entrance_SetEntranceDiscovered(uint16_t entranceIndex);

#endif //_RANDO_ENTRANCE_H_

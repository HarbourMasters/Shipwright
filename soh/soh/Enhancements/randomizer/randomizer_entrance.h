#ifndef _RANDO_ENTRANCE_H_
#define _RANDO_ENTRANCE_H_

#include <stdint.h>

//Entrance Table Data: https://wiki.cloudmodding.com/oot/Entrance_Table_(Data)
//Accessed June 2021, published content date at the time was 14 March 2020, at 21:47

#define ENTRANCE_TABLE_SIZE ENTR_MAX

#define DEKU_TREE_ENTRANCE ENTR_DEKU_TREE_0
#define DODONGOS_CAVERN_ENTRANCE ENTR_DODONGOS_CAVERN_0
#define JABU_JABUS_BELLY_ENTRANCE ENTR_JABU_JABU_0
#define FOREST_TEMPLE_ENTRANCE ENTR_FOREST_TEMPLE_0
#define FIRE_TEMPLE_ENTRANCE ENTR_FIRE_TEMPLE_0
#define WATER_TEMPLE_ENTRANCE ENTR_WATER_TEMPLE_0
#define SPIRIT_TEMPLE_ENTRANCE ENTR_SPIRIT_TEMPLE_0
#define SHADOW_TEMPLE_ENTRANCE ENTR_SHADOW_TEMPLE_0
#define BOTTOM_OF_THE_WELL_ENTRANCE ENTR_BOTTOM_OF_THE_WELL_0
#define GERUDO_TRAINING_GROUNDS_ENTRANCE ENTR_GERUDO_TRAINING_GROUND_0
#define ICE_CAVERN_ENTRANCE ENTR_ICE_CAVERN_0
#define GANONS_CASTLE_ENTRANCE ENTR_INSIDE_GANONS_CASTLE_0
#define LINK_HOUSE_SAVEWARP_ENTRANCE ENTR_LINKS_HOUSE_0

#define ENTRANCE_RANDO_GROTTO_LOAD_START 0x0700
#define ENTRANCE_RANDO_GROTTO_EXIT_START 0x0800
#define MAX_ENTRANCE_RANDO_USED_INDEX 0x0820

typedef enum {
    /* 0x00 */ GROTTO_COLOSSUS_OFFSET,
    /* 0x01 */ GROTTO_LH_OFFSET,
    /* 0x02 */ GROTTO_ZR_STORMS_OFFSET,
    /* 0x03 */ GROTTO_ZR_FAIRY_OFFSET,
    /* 0x04 */ GROTTO_ZR_OPEN_OFFSET,
    /* 0x05 */ GROTTO_DMC_HAMMER_OFFSET,
    /* 0x06 */ GROTTO_DMC_UPPER_OFFSET,
    /* 0x07 */ GROTTO_GORON_CITY_OFFSET,
    /* 0x08 */ GROTTO_DMT_STORMS_OFFSET,
    /* 0x09 */ GROTTO_DMT_COW_OFFSET,
    /* 0x0A */ GROTTO_KAK_OPEN_OFFSET,
    /* 0x0B */ GROTTO_KAK_REDEAD_OFFSET,
    /* 0x0C */ GROTTO_HC_STORMS_OFFSET,
    /* 0x0D */ GROTTO_HF_TEKTITE_OFFSET,
    /* 0x0E */ GROTTO_HF_NEAR_KAK_OFFSET,
    /* 0x0F */ GROTTO_HF_FAIRY_OFFSET,
    /* 0x10 */ GROTTO_HF_NEAR_MARKET_OFFSET,
    /* 0x11 */ GROTTO_HF_COW_OFFSET,
    /* 0x12 */ GROTTO_HF_INSIDE_FENCE_OFFSET,
    /* 0x13 */ GROTTO_HF_OPEN_OFFSET,
    /* 0x14 */ GROTTO_HF_SOUTHEAST_OFFSET,
    /* 0x15 */ GROTTO_LLR_OFFSET,
    /* 0x16 */ GROTTO_SFM_WOLFOS_OFFSET,
    /* 0x17 */ GROTTO_SFM_STORMS_OFFSET,
    /* 0x18 */ GROTTO_SFM_FAIRY_OFFSET,
    /* 0x19 */ GROTTO_LW_SCRUBS_OFFSET,
    /* 0x1A */ GROTTO_LW_NEAR_SHORTCUTS_OFFSET,
    /* 0x1B */ GROTTO_KF_STORMS_OFFSET,
    /* 0x1C */ GROTTO_ZD_STORMS_OFFSET,
    /* 0x1D */ GROTTO_GF_STORMS_OFFSET,
    /* 0x1E */ GROTTO_GV_STORMS_OFFSET,
    /* 0x1F */ GROTTO_GV_OCTOROK_OFFSET,
    /* 0x20 */ GROTTO_LW_DEKU_THEATRE_OFFSET,
    /* 0x21 */ GROTTO_OFFSET_MAX,
} GrottoEntranceOffsets;

#define ENTRANCE_RANDO_GROTTO_LOAD(index) ENTRANCE_RANDO_GROTTO_LOAD_START + index
#define ENTRANCE_RANDO_GROTTO_EXIT(index) ENTRANCE_RANDO_GROTTO_EXIT_START + index

#define ENTRANCE_OVERRIDES_MAX_COUNT 259 // 11 one-way entrances + 124 two-way entrances (x2)
#define SHUFFLEABLE_BOSS_COUNT 8

#define SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT 66 // Max entrance rando index is 0x0820, (2080 / 32 == 65) + 1
#define SAVEFILE_SCENES_DISCOVERED_IDX_COUNT 4 // Max scene ID is 0x6E, (110 / 32 == 3) + 1

#define ENTRANCE_INFO_FIELD(continueBgm, displayTitleCard, endTransType, startTransType)               \
    (((continueBgm) ? ENTRANCE_INFO_CONTINUE_BGM_FLAG : 0) |                                           \
     ((displayTitleCard) ? ENTRANCE_INFO_DISPLAY_TITLE_CARD_FLAG : 0) |                                \
     (((endTransType) << ENTRANCE_INFO_END_TRANS_TYPE_SHIFT) & ENTRANCE_INFO_END_TRANS_TYPE_MASK) |    \
     (((startTransType) << ENTRANCE_INFO_START_TRANS_TYPE_SHIFT) & ENTRANCE_INFO_START_TRANS_TYPE_MASK))

typedef struct {
    int16_t index;
    int16_t destination;
    int16_t blueWarp;
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
int16_t Entrance_OverrideDynamicExit(int16_t dynamicExitIndex);
uint32_t Entrance_SceneAndSpawnAre(uint8_t scene, uint8_t spawn);
void Entrance_SetGameOverEntrance(void);
void Entrance_SetSavewarpEntrance(void);
void Entrance_SetWarpSongEntrance(void);
void Entrance_OverrideBlueWarp(void);
void Entrance_OverrideCutsceneEntrance(uint16_t cutsceneCmd);
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

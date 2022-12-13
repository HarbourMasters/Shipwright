#ifndef Z64SAVE_H
#define Z64SAVE_H

#include "ultra64.h"
#include "z64math.h"
#include "z64audio.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/randomizer_inf.h"
#include "soh/Enhancements/gameplaystats.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"

typedef enum {
    /* 0x0 */ MAGIC_STATE_IDLE, // Regular gameplay
    /* 0x1 */ MAGIC_STATE_CONSUME_SETUP, // Sets the speed at which magic border flashes
    /* 0x2 */ MAGIC_STATE_CONSUME, // Consume magic until target is reached or no more magic is available
    /* 0x3 */ MAGIC_STATE_METER_FLASH_1, // Flashes border and freezes Dark Link
    /* 0x4 */ MAGIC_STATE_METER_FLASH_2, // Flashes border and draws yellow magic to preview target consumption
    /* 0x5 */ MAGIC_STATE_RESET, // Reset colors and return to idle
    /* 0x6 */ MAGIC_STATE_METER_FLASH_3, // Flashes border with no additional behaviour
    /* 0x7 */ MAGIC_STATE_CONSUME_LENS, // Magic slowly consumed by lens.
    /* 0x8 */ MAGIC_STATE_STEP_CAPACITY, // Step `magicCapacity` to full capacity
    /* 0x9 */ MAGIC_STATE_FILL, // Add magic until magicFillTarget is reached.
    /* 0xA */ MAGIC_STATE_ADD // Add requested magic
} MagicState;

typedef enum {
    /* 0 */ MAGIC_CONSUME_NOW, // Consume Magic immediately without preview
    /* 1 */ MAGIC_CONSUME_WAIT_NO_PREVIEW, // Sets consume target but waits to consume. No yellow magic preview to target consumption. Unused
    /* 2 */ MAGIC_CONSUME_NOW_ALT, // Identical behaviour to MAGIC_CONSUME_NOW. Unused
    /* 3 */ MAGIC_CONSUME_LENS, // Lens consumption
    /* 4 */ MAGIC_CONSUME_WAIT_PREVIEW, // Sets consume target but waits to consume. Draws yellow magic to target consumption
    /* 5 */ MAGIC_ADD // Sets a target to add magic
} MagicChangeType;

#define MAGIC_NORMAL_METER 0x30
#define MAGIC_DOUBLE_METER (2 * MAGIC_NORMAL_METER)

typedef struct {
    /* 0x00 */ u8 buttonItems[8]; // SOH [Enhancements] Changed from 4 to 8 to support Dpad equips
    /* 0x04 */ u8 cButtonSlots[7]; // SOH [Enhancements] Changed from 3 to 7 to support Dpad equips
    /* 0x08 */ u16 equipment; // a mask where each nibble corresponds to a type of equipment `EquipmentType`, and each nibble is a piece `EquipValue*`
} ItemEquips; // size = 0x0A

typedef struct {
    /* 0x00 */ u8 items[24];
    /* 0x18 */ s8 ammo[16];
    /* 0x28 */ u16 equipment; // a mask where each nibble corresponds to a type of equipment `EquipmentType`, and each bit to an owned piece `EquipInv*`
    /* 0x2C */ u32 upgrades;
    /* 0x30 */ u32 questItems;
    /* 0x34 */ u8 dungeonItems[20];
    /* 0x48 */ s8 dungeonKeys[19];
    /* 0x5B */ s8 defenseHearts;
    /* 0x5C */ s16 gsTokens;
} Inventory; // size = 0x5E

typedef struct {
    /*      */ u8 heartPieces;
    /*      */ u8 heartContainers;
    /*      */ u8 dungeonKeys[19];
    /*      */ u32 playTimer;
    /*      */ u32 pauseTimer;
    /*      */ bool gameComplete;
    /*      */ u32 timestamp[TIMESTAMP_MAX];
    /*      */ u32 count[COUNT_MAX];
    /*      */ u32 entrancesDiscovered[SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT];
    /*      */ u32 scenesDiscovered[SAVEFILE_SCENES_DISCOVERED_IDX_COUNT];
} SohStats;

typedef struct {
    /* 0x00 */ u32 chest;
    /* 0x04 */ u32 swch;
    /* 0x08 */ u32 clear;
    /* 0x0C */ u32 collect;
    /* 0x10 */ u32 unk;
    /* 0x14 */ u32 rooms;
    /* 0x18 */ u32 floors;
} SavedSceneFlags; // size = 0x1C

typedef struct {
    /* 0x00 */ s16 scene; // Upstream TODO: sceneId
    /* 0x02 */ Vec3s pos;
    /* 0x08 */ s16 angle;
} HorseData; // size = 0x0A

/**
 * The respawn mode names refer to the perceived player movement when respawning
 * "down": being on ground
 * "return": coming from the ground
 * "top": coming from the air
 */
typedef enum {
    /* 0x00 */ RESPAWN_MODE_DOWN,   /* Normal Void Outs */
    /* 0x01 */ RESPAWN_MODE_RETURN, /* Grotto Returnpoints */
    /* 0x02 */ RESPAWN_MODE_TOP,    /* Farore's Wind */
    /* 0x03 */ RESPAWN_MODE_MAX
} RespawnMode;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
    /* 0x0E */ s16 playerParams;
    /* 0x10 */ s16 entranceIndex;
    /* 0x12 */ u8 roomIndex;
    /* 0x13 */ s8 data;
    /* 0x14 */ u32 tempSwchFlags;
    /* 0x18 */ u32 tempCollectFlags;
} RespawnData; // size = 0x1C

typedef struct {
    /* 0x00 */ Vec3i pos;
    /* 0x0C */ s32 yaw;
    /* 0x10 */ s32 playerParams;
    /* 0x14 */ s32 entranceIndex;
    /* 0x18 */ s32 roomIndex;
    /* 0x1C */ s32 set;
    /* 0x20 */ s32 tempSwchFlags;
    /* 0x24 */ s32 tempCollectFlags;
} FaroresWindData; // size = 0x28

typedef struct {
    RandomizerCheck check;
    RandomizerGetData get;
} ItemLocationRando;

typedef struct {
    RandomizerCheck check;
    char hintText[200];
} HintLocationRando;

typedef struct {
    RandomizerSettingKey key;
    u8 value;
} RandoSetting;

typedef struct {
    /* 0x0000 */ s32 entranceIndex; // start of `save` substruct, originally called "memory"
    /* 0x0004 */ s32 linkAge; // 0: Adult; 1: Child (see enum `LinkAge`)
    /* 0x0008 */ s32 cutsceneIndex;
    /* 0x000C */ u16 dayTime; // "zelda_time"
    /* 0x0010 */ s32 nightFlag;
    /* 0x0014 */ s32 totalDays;
    /* 0x0018 */ s32 bgsDayCount; // increments with totalDays, can be cleared with `Environment_ClearBgsDayCount`
    /* 0x001C */ char newf[6]; // string "ZELDAZ". start of `info` substruct, originally called "information"
    /* 0x0022 */ u16 deaths;
    /* 0x0024 */ char playerName[8];
    /* 0x002C */ s16 n64ddFlag;
    /* 0x002E */ s16 healthCapacity; // "max_life"
    /* 0x0030 */ s16 health; // "now_life"
    /* 0x0032 */ s8 magicLevel; // 0 for no magic/new load, 1 for magic, 2 for double magic
    /* 0x0033 */ s8 magic; // current magic available for use
    /* 0x0034 */ s16 rupees;
    /* 0x0036 */ u16 swordHealth;
    /* 0x0038 */ u16 naviTimer;
    /* 0x003A */ u8 isMagicAcquired;
    /* 0x003B */ char unk_3B[0x01];
    /* 0x003C */ u8 isDoubleMagicAcquired;
    /* 0x003D */ u8 isDoubleDefenseAcquired;
    /* 0x003E */ u8 bgsFlag;
    /* 0x003F */ u8 ocarinaGameRoundNum;
    /* 0x0040 */ ItemEquips childEquips;
    /* 0x004A */ ItemEquips adultEquips;
    /* 0x0054 */ u32 unk_54; // this may be incorrect, currently used for alignment
    /* 0x0058 */ char unk_58[0x0E];
    /* 0x0066 */ s16 savedSceneNum; // Upstream TODO: sceneId
    /* 0x0068 */ ItemEquips equips;
    /* 0x0074 */ Inventory inventory;
    /* 0x00D4 */ SavedSceneFlags sceneFlags[124];
    /* 0x0E64 */ FaroresWindData fw;
    /* 0x0E8C */ char unk_E8C[0x10];
    /* 0x0E9C */ s32 gsFlags[6];
    /* 0x0EB4 */ char unk_EB4[0x4];
    /* 0x0EB8 */ s32 highScores[7];
    /* 0x0ED4 */ u16 eventChkInf[14]; // "event_chk_inf"
    /* 0x0EF0 */ u16 itemGetInf[4]; // "item_get_inf"
    /* 0x0EF8 */ u16 infTable[30]; // "inf_table"
    /* 0x0F34 */ char unk_F34[0x04];
    /* 0x0F38 */ u32 worldMapAreaData; // "area_arrival"
    /* 0x0F3C */ char unk_F3C[0x4];
    /* 0x0F40 */ u8 scarecrowLongSongSet;
    /* 0x0F41 */ OcarinaNote scarecrowLongSong[108]; // Upstream TODO: Audio
    /* 0x12A1 */ char unk_12A1[0x24];
    /* 0x12C5 */ u8 scarecrowSpawnSongSet;
    /* 0x12C6 */ OcarinaNote scarecrowSpawnSong[16]; // Upstream TODO: Audio
    /* 0x1346 */ char unk_1346[0x02];
    /* 0x1348 */ HorseData horseData;
    /* 0x1352 */ u16 checksum; // "check_sum"
    /* 0x1354 */ s32 fileNum; // "file_no"
    /* 0x1358 */ char unk_1358[0x0004];
    /* 0x135C */ s32 gameMode;
    /* 0x1360 */ s32 sceneSetupIndex; // "counter" // Upstream TODO: sceneLayer
    /* 0x1364 */ s32 respawnFlag; // "restart_flag"
    /* 0x1368 */ RespawnData respawn[RESPAWN_MODE_MAX]; // "restart_data"
    /* 0x13BC */ f32 entranceSpeed;
    /* 0x13C0 */ u16 entranceSound;
    /* 0x13C2 */ char unk_13C2[0x0001];
    /* 0x13C3 */ u8 retainWeatherMode;
    /* 0x13C4 */ s16 dogParams;
    /* 0x13C6 */ u8 textTriggerFlags;
    /* 0x13C7 */ u8 showTitleCard;
    /* 0x13C8 */ s16 nayrusLoveTimer;
    /* 0x13CA */ char unk_13CA[0x0002];
    /* 0x13CC */ s16 rupeeAccumulator;
    /* 0x13CE */ s16 timer1State;
    /* 0x13D0 */ s16 timer1Value;
    /* 0x13D2 */ s16 timer2State;
    /* 0x13D4 */ s16 timer2Value;
    /* 0x13D6 */ s16 timerX[2];
    /* 0x13DA */ s16 timerY[2];
    /* 0x13DE */ char unk_13DE[0x0002];
    /* 0x13E0 */ u8 seqId;
    /* 0x13E1 */ u8 natureAmbienceId;
    /* 0x13E2 */ u8 buttonStatus[9]; // SOH [Enhancements] Changed from 5 to 9 to support Dpad equips
    /* 0x13E7 */ u8 unk_13E7; // alpha related
    /* 0x13E8 */ u16 unk_13E8; // alpha type?
    /* 0x13EA */ u16 unk_13EA; // also alpha type?
    /* 0x13EC */ u16 unk_13EC; // alpha type counter?
    /* 0x13EE */ u16 unk_13EE; // previous alpha type?
    /* 0x13F0 */ s16 magicState; // determines magic meter behavior on each frame
    /* 0x13F2 */ s16 prevMagicState; // used to resume the previous state after adding or filling magic
    /* 0x13F4 */ s16 magicCapacity; // maximum magic available
    /* 0x13F6 */ s16 magicFillTarget; // target used to fill magic. Target can either be full capacity (Magic_Fill, magic upgrades), or the saved magic amount (loading a file, game over)
    /* 0x13F8 */ s16 magicTarget; // target for magic to step to when adding or consuming magic
    /* 0x13FA */ u16 eventInf[4]; // "event_inf"
    /* 0x1402 */ u16 mapIndex; // intended for maps/minimaps but commonly used as the dungeon index
    /* 0x1404 */ u16 minigameState;
    /* 0x1406 */ u16 minigameScore; // "yabusame_total"
    /* 0x1408 */ char unk_1408[0x0001];
    /* 0x1409 */ u8 language; // NTSC 0: Japanese; 1: English | PAL 0: English; 1: German; 2: French
    /* 0x140A */ u8 audioSetting;
    /* 0x140B */ char unk_140B[0x0001];
    /* 0x140C */ u8 zTargetSetting; // 0: Switch; 1: Hold
    /* 0x140E */ u16 forcedSeqId; // immediately start playing the sequence if set
    /* 0x1410 */ u8 cutsceneTransitionControl; // context dependent usage: can either trigger a delayed fade or control fill alpha
    /* 0x1411 */ char unk_1411[0x0001];
    /* 0x1412 */ u16 nextCutsceneIndex;
    /* 0x1414 */ u8 cutsceneTrigger;
    /* 0x1415 */ u8 chamberCutsceneNum;
    /* 0x1416 */ u16 nextDayTime; // "next_zelda_time"
    /* 0x1418 */ u8 transFadeDuration;
    /* 0x1419 */ u8 transWipeSpeed;
    /* 0x141A */ u16 skyboxTime;
    /* 0x141C */ u8 dogIsLost;
    /* 0x141D */ u8 nextTransitionType;
    /* 0x141E */ char unk_141E[0x0002];
    /* 0x1420 */ s16 worldMapArea;
    /* 0x1422 */ s16 sunsSongState; // controls the effects of suns song
    /* 0x1424 */ s16 healthAccumulator;
    /* 0x1426 */ u16 pendingSale;
    // #region SOH [General]
    // Upstream TODO: Move these to their own struct or name to more obviously specific to SoH
    /*        */ uint32_t isMasterQuest;
    /*        */ u8 mqDungeonCount;
    /*        */ u8 pendingIceTrapCount;
    /*        */ SohStats sohStats;
    /*        */ u8 temporaryWeapon;
    // #endregion
    // #region SOH [Randomizer]
    // Upstream TODO: Move these to their own struct or name to more obviously specific to Randomizer
    /*        */ RandoSetting randoSettings[300];
    /*        */ ItemLocationRando itemLocations[RC_MAX];
    /*        */ HintLocationRando hintLocations[50];
    /*        */ EntranceOverride entranceOverrides[ENTRANCE_OVERRIDES_MAX_COUNT];
    /*        */ char childAltarText[250];
    /*        */ char adultAltarText[750];
    /*        */ char ganonHintText[150];
    /*        */ char ganonText[250];
    /*        */ char warpMinuetText[100];
    /*        */ char warpBoleroText[100];
    /*        */ char warpSerenadeText[100];
    /*        */ char warpRequiemText[100];
    /*        */ char warpNocturneText[100];
    /*        */ char warpPreludeText[100];
    /*        */ u8 seedIcons[5];
    /*        */ u16 randomizerInf[9];
    /*        */ u16 adultTradeItems;
    // #endregion
} SaveContext; // size = 0x1428

typedef enum {
    /* 0x00 */ BTN_ENABLED,
    /* 0xFF */ BTN_DISABLED = 0xFF
} ButtonStatus;

typedef enum {
    /* 0x00 */ CHAMBER_CS_FOREST,
    /* 0x01 */ CHAMBER_CS_FIRE,
    /* 0x02 */ CHAMBER_CS_WATER,
    /* 0x03 */ CHAMBER_CS_SPIRIT,
    /* 0x04 */ CHAMBER_CS_SHADOW,
    /* 0x05 */ CHAMBER_CS_LIGHT
} ChamberCutsceneNum;

typedef enum {
    /* 0x00 */ HS_HBA,          // horseback archery
    /* 0x01 */ HS_POE_POINTS,
    /* 0x02 */ HS_FISHING,
    /* 0x03 */ HS_HORSE_RACE,
    /* 0x04 */ HS_MARATHON,
    /* 0x05 */ HS_UNK_05,
    /* 0x06 */ HS_DAMPE_RACE
} HighScores;

typedef enum {
    /* 0 */ SUNSSONG_INACTIVE,
    /* 1 */ SUNSSONG_START, // the suns ocarina effect signals that the song has finished playing
    /* 2 */ SUNSSONG_SPEED_TIME, // suns was played where time passes, speed up the advancement of time
    /* 3 */ SUNSSONG_SPECIAL // time does not advance, but signals the song was played. used for freezing redeads
} SunsSongState;

typedef enum {
    /* 0 */ GAMEMODE_NORMAL,
    /* 1 */ GAMEMODE_TITLE_SCREEN,
    /* 2 */ GAMEMODE_FILE_SELECT, // Note: only instance type transitions swap to file select
    /* 3 */ GAMEMODE_END_CREDITS
} GameMode;

typedef enum {
    /* 0 */ SCENE_LAYER_CHILD_DAY,
    /* 1 */ SCENE_LAYER_CHILD_NIGHT,
    /* 2 */ SCENE_LAYER_ADULT_DAY,
    /* 3 */ SCENE_LAYER_ADULT_NIGHT,
    /* 4 */ SCENE_LAYER_CUTSCENE_FIRST
} SceneLayer;

#define IS_CUTSCENE_LAYER (gSaveContext.sceneLayer >= SCENE_LAYER_CUTSCENE_FIRST)

typedef enum {
    /* 0 */ LINK_AGE_ADULT,
    /* 1 */ LINK_AGE_CHILD
} LinkAge;



/*
 *
 * SaveContext flags
 *
 */


/*
 * SaveContext.eventChkInf
 */

#define EVENTCHKINF_02 0x02
#define EVENTCHKINF_03 0x03
#define EVENTCHKINF_04 0x04
#define EVENTCHKINF_05 0x05
#define EVENTCHKINF_07 0x07
#define EVENTCHKINF_09 0x09
#define EVENTCHKINF_0A 0x0A
#define EVENTCHKINF_0B 0x0B
#define EVENTCHKINF_0C 0x0C
#define EVENTCHKINF_0F 0x0F
#define EVENTCHKINF_10 0x10
#define EVENTCHKINF_11 0x11
#define EVENTCHKINF_12 0x12
#define EVENTCHKINF_TALON_WOKEN_IN_CASTLE 0x13
#define EVENTCHKINF_TALON_RETURNED_FROM_CASTLE 0x14
#define EVENTCHKINF_15 0x15
#define EVENTCHKINF_16 0x16
#define EVENTCHKINF_EPONA_OBTAINED 0x18
#define EVENTCHKINF_1B 0x1B
#define EVENTCHKINF_1C 0x1C
#define EVENTCHKINF_1D 0x1D
#define EVENTCHKINF_1E 0x1E
#define EVENTCHKINF_20 0x20
#define EVENTCHKINF_21 0x21
#define EVENTCHKINF_22 0x22
#define EVENTCHKINF_23 0x23
#define EVENTCHKINF_25 0x25
#define EVENTCHKINF_2A 0x2A
#define EVENTCHKINF_2B 0x2B
#define EVENTCHKINF_2C 0x2C
#define EVENTCHKINF_2D 0x2D
#define EVENTCHKINF_2F 0x2F
#define EVENTCHKINF_30 0x30
#define EVENTCHKINF_31 0x31
#define EVENTCHKINF_32 0x32
#define EVENTCHKINF_33 0x33
#define EVENTCHKINF_37 0x37
#define EVENTCHKINF_38 0x38
#define EVENTCHKINF_39 0x39
#define EVENTCHKINF_3A 0x3A
#define EVENTCHKINF_3B 0x3B
#define EVENTCHKINF_3C 0x3C

// 0x40
#define EVENTCHKINF_40_INDEX 4
#define EVENTCHKINF_40_SHIFT 0
#define EVENTCHKINF_40_MASK (1 << EVENTCHKINF_40_SHIFT)
#define EVENTCHKINF_40 ((EVENTCHKINF_40_INDEX << 4) | EVENTCHKINF_40_SHIFT)

#define EVENTCHKINF_41 0x41
#define EVENTCHKINF_42 0x42
#define EVENTCHKINF_43 0x43
#define EVENTCHKINF_45 0x45
#define EVENTCHKINF_48 0x48
#define EVENTCHKINF_49 0x49
#define EVENTCHKINF_4A 0x4A
#define EVENTCHKINF_4B 0x4B
#define EVENTCHKINF_4C 0x4C
#define EVENTCHKINF_4D 0x4D
#define EVENTCHKINF_4E 0x4E
#define EVENTCHKINF_4F 0x4F
#define EVENTCHKINF_50 0x50
#define EVENTCHKINF_51 0x51
#define EVENTCHKINF_52 0x52
#define EVENTCHKINF_54 0x54
#define EVENTCHKINF_55 0x55
#define EVENTCHKINF_59 0x59
#define EVENTCHKINF_5A 0x5A
#define EVENTCHKINF_5B 0x5B
#define EVENTCHKINF_5C 0x5C
#define EVENTCHKINF_65 0x65
#define EVENTCHKINF_67 0x67
#define EVENTCHKINF_68 0x68
#define EVENTCHKINF_RAISED_LAKE_HYLIA_WATER 0x69
#define EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO 0x6A

// 0x6B
#define EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO_INDEX 6
#define EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO_SHIFT 11
#define EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO_MASK (1 << EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO_SHIFT)
#define EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO ((EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO_INDEX << 4) | EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO_SHIFT)

#define EVENTCHKINF_6E 0x6E
#define EVENTCHKINF_6F 0x6F
#define EVENTCHKINF_70 0x70
#define EVENTCHKINF_71 0x71
#define EVENTCHKINF_72 0x72
#define EVENTCHKINF_73 0x73
#define EVENTCHKINF_74 0x74
#define EVENTCHKINF_75 0x75
#define EVENTCHKINF_76 0x76
#define EVENTCHKINF_77 0x77
#define EVENTCHKINF_78 0x78
#define EVENTCHKINF_80 0x80
#define EVENTCHKINF_82 0x82
#define EVENTCHKINF_8C 0x8C
#define EVENTCHKINF_8D 0x8D
#define EVENTCHKINF_8E 0x8E
#define EVENTCHKINF_8F 0x8F

// 0x90-0x93
// carpenters freed from the gerudo
#define EVENTCHKINF_CARPENTERS_FREE_INDEX 9
#define EVENTCHKINF_CARPENTERS_FREE_SHIFT(n) (0 + (n))
#define EVENTCHKINF_CARPENTERS_FREE_MASK(n) (1 << EVENTCHKINF_CARPENTERS_FREE_SHIFT(n))
#define EVENTCHKINF_CARPENTERS_FREE(n) ((EVENTCHKINF_CARPENTERS_FREE_INDEX << 4) | EVENTCHKINF_CARPENTERS_FREE_SHIFT(n))
#define EVENTCHKINF_CARPENTERS_FREE_MASK_ALL (\
      EVENTCHKINF_CARPENTERS_FREE_MASK(0)     \
    | EVENTCHKINF_CARPENTERS_FREE_MASK(1)     \
    | EVENTCHKINF_CARPENTERS_FREE_MASK(2)     \
    | EVENTCHKINF_CARPENTERS_FREE_MASK(3)    )
#define GET_EVENTCHKINF_CARPENTERS_FREE_ALL() \
    CHECK_FLAG_ALL(gSaveContext.eventChkInf[EVENTCHKINF_CARPENTERS_FREE_INDEX], EVENTCHKINF_CARPENTERS_FREE_MASK_ALL)

#define EVENTCHKINF_94 0x94
#define EVENTCHKINF_95 0x95
#define EVENTCHKINF_96 0x96
#define EVENTCHKINF_9C 0x9C
#define EVENTCHKINF_A0 0xA0
#define EVENTCHKINF_A1 0xA1
#define EVENTCHKINF_A3 0xA3
#define EVENTCHKINF_A4 0xA4
#define EVENTCHKINF_A5 0xA5
#define EVENTCHKINF_A6 0xA6
#define EVENTCHKINF_A7 0xA7
#define EVENTCHKINF_A8 0xA8
#define EVENTCHKINF_A9 0xA9
#define EVENTCHKINF_AA 0xAA
#define EVENTCHKINF_AC 0xAC
#define EVENTCHKINF_AD 0xAD
#define EVENTCHKINF_B0 0xB0
#define EVENTCHKINF_B1 0xB1
#define EVENTCHKINF_B2 0xB2
#define EVENTCHKINF_B3 0xB3
#define EVENTCHKINF_B4 0xB4
#define EVENTCHKINF_B5 0xB5
#define EVENTCHKINF_B6 0xB6
#define EVENTCHKINF_B7 0xB7
#define EVENTCHKINF_B8 0xB8
#define EVENTCHKINF_B9 0xB9
#define EVENTCHKINF_BA 0xBA
#define EVENTCHKINF_BB 0xBB
#define EVENTCHKINF_BC 0xBC
#define EVENTCHKINF_BD 0xBD
#define EVENTCHKINF_BE 0xBE
#define EVENTCHKINF_BF 0xBF
#define EVENTCHKINF_C0 0xC0
#define EVENTCHKINF_C1 0xC1
#define EVENTCHKINF_C3 0xC3
#define EVENTCHKINF_C4 0xC4
#define EVENTCHKINF_C5 0xC5
#define EVENTCHKINF_C6 0xC6
#define EVENTCHKINF_C7 0xC7
#define EVENTCHKINF_C8 0xC8
#define EVENTCHKINF_C9 0xC9

// 0xD0-0xD6
#define EVENTCHKINF_SONGS_FOR_FROGS_INDEX 13
#define EVENTCHKINF_SONGS_FOR_FROGS_CHOIR_SHIFT  0
#define EVENTCHKINF_SONGS_FOR_FROGS_ZL_SHIFT     1
#define EVENTCHKINF_SONGS_FOR_FROGS_EPONA_SHIFT  2
#define EVENTCHKINF_SONGS_FOR_FROGS_SUNS_SHIFT   3
#define EVENTCHKINF_SONGS_FOR_FROGS_SARIA_SHIFT  4
#define EVENTCHKINF_SONGS_FOR_FROGS_SOT_SHIFT    5
#define EVENTCHKINF_SONGS_FOR_FROGS_STORMS_SHIFT 6
#define EVENTCHKINF_SONGS_FOR_FROGS_CHOIR_MASK  (1 << EVENTCHKINF_SONGS_FOR_FROGS_CHOIR_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_ZL_MASK     (1 << EVENTCHKINF_SONGS_FOR_FROGS_ZL_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_EPONA_MASK  (1 << EVENTCHKINF_SONGS_FOR_FROGS_EPONA_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_SUNS_MASK   (1 << EVENTCHKINF_SONGS_FOR_FROGS_SUNS_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_SARIA_MASK  (1 << EVENTCHKINF_SONGS_FOR_FROGS_SARIA_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_SOT_MASK    (1 << EVENTCHKINF_SONGS_FOR_FROGS_SOT_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_STORMS_MASK (1 << EVENTCHKINF_SONGS_FOR_FROGS_STORMS_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_CHOIR  ((EVENTCHKINF_SONGS_FOR_FROGS_INDEX << 4) | EVENTCHKINF_SONGS_FOR_FROGS_CHOIR_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_ZL     ((EVENTCHKINF_SONGS_FOR_FROGS_INDEX << 4) | EVENTCHKINF_SONGS_FOR_FROGS_ZL_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_EPONA  ((EVENTCHKINF_SONGS_FOR_FROGS_INDEX << 4) | EVENTCHKINF_SONGS_FOR_FROGS_EPONA_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_SUNS   ((EVENTCHKINF_SONGS_FOR_FROGS_INDEX << 4) | EVENTCHKINF_SONGS_FOR_FROGS_SUNS_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_SARIA  ((EVENTCHKINF_SONGS_FOR_FROGS_INDEX << 4) | EVENTCHKINF_SONGS_FOR_FROGS_SARIA_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_SOT    ((EVENTCHKINF_SONGS_FOR_FROGS_INDEX << 4) | EVENTCHKINF_SONGS_FOR_FROGS_SOT_SHIFT)
#define EVENTCHKINF_SONGS_FOR_FROGS_STORMS ((EVENTCHKINF_SONGS_FOR_FROGS_INDEX << 4) | EVENTCHKINF_SONGS_FOR_FROGS_STORMS_SHIFT)

// 0xDA-0xDE
#define EVENTCHKINF_DA_DB_DC_DD_DE_INDEX 13
#define EVENTCHKINF_DA_MASK (1 << 10)
#define EVENTCHKINF_DB_MASK (1 << 11)
#define EVENTCHKINF_DC_MASK (1 << 12)
#define EVENTCHKINF_DD_MASK (1 << 13)
#define EVENTCHKINF_DE_MASK (1 << 14)


/*
 * SaveContext.itemGetInf
 */

#define ITEMGETINF_TALON_BOTTLE 0x02
#define ITEMGETINF_03 0x03
#define ITEMGETINF_04 0x04
#define ITEMGETINF_05 0x05
#define ITEMGETINF_06 0x06
#define ITEMGETINF_07 0x07
#define ITEMGETINF_08 0x08
#define ITEMGETINF_09 0x09
#define ITEMGETINF_0A 0x0A
#define ITEMGETINF_0B 0x0B
#define ITEMGETINF_0C 0x0C
#define ITEMGETINF_0D 0x0D
#define ITEMGETINF_0E 0x0E
#define ITEMGETINF_0F 0x0F
#define ITEMGETINF_10 0x10
#define ITEMGETINF_11 0x11
#define ITEMGETINF_12 0x12
#define ITEMGETINF_13 0x13
#define ITEMGETINF_15 0x15
#define ITEMGETINF_16 0x16
#define ITEMGETINF_17 0x17

// 0x18-0x1A
#define ITEMGETINF_18_19_1A_INDEX 1
#define ITEMGETINF_18_SHIFT 8
#define ITEMGETINF_19_SHIFT 9
#define ITEMGETINF_1A_SHIFT 10
#define ITEMGETINF_18_MASK (1 << ITEMGETINF_18_SHIFT)
#define ITEMGETINF_19_MASK (1 << ITEMGETINF_19_SHIFT)
#define ITEMGETINF_1A_MASK (1 << ITEMGETINF_1A_SHIFT)
#define ITEMGETINF_18 ((ITEMGETINF_18_19_1A_INDEX << 4) | ITEMGETINF_18_SHIFT)
#define ITEMGETINF_19 ((ITEMGETINF_18_19_1A_INDEX << 4) | ITEMGETINF_19_SHIFT)
#define ITEMGETINF_1A ((ITEMGETINF_18_19_1A_INDEX << 4) | ITEMGETINF_1A_SHIFT)

#define ITEMGETINF_1B 0x1B
#define ITEMGETINF_1C 0x1C
#define ITEMGETINF_1D 0x1D
#define ITEMGETINF_1E 0x1E
#define ITEMGETINF_1F 0x1F
#define ITEMGETINF_23 0x23
#define ITEMGETINF_24 0x24
#define ITEMGETINF_25 0x25
#define ITEMGETINF_26 0x26
#define ITEMGETINF_2A 0x2A
#define ITEMGETINF_2C 0x2C
#define ITEMGETINF_2E 0x2E
#define ITEMGETINF_30 0x30
#define ITEMGETINF_31 0x31
#define ITEMGETINF_38 0x38
#define ITEMGETINF_39 0x39
#define ITEMGETINF_3A 0x3A
#define ITEMGETINF_3B 0x3B
#define ITEMGETINF_3F 0x3F


/*
 * SaveContext.infTable
 */

#define INFTABLE_00 0x00
#define INFTABLE_01 0x01
#define INFTABLE_03 0x03
#define INFTABLE_05 0x05
#define INFTABLE_0C 0x0C
#define INFTABLE_0E 0x0E
#define INFTABLE_10 0x10
#define INFTABLE_15 0x15
#define INFTABLE_17 0x17
#define INFTABLE_19 0x19
#define INFTABLE_1E 0x1E
#define INFTABLE_22 0x22
#define INFTABLE_24 0x24
#define INFTABLE_26 0x26
#define INFTABLE_28 0x28
#define INFTABLE_2A 0x2A
#define INFTABLE_2B 0x2B
#define INFTABLE_2E 0x2E
#define INFTABLE_2F 0x2F
#define INFTABLE_30 0x30
#define INFTABLE_41 0x41
#define INFTABLE_47 0x47
#define INFTABLE_51 0x51
#define INFTABLE_59 0x59
#define INFTABLE_61 0x61
#define INFTABLE_66 0x66
#define INFTABLE_6A 0x6A
#define INFTABLE_6C 0x6C
#define INFTABLE_71 0x71
#define INFTABLE_76 0x76
#define INFTABLE_77 0x77
#define INFTABLE_TALKED_TO_TALON_IN_RANCH_HOUSE 0x7E
#define INFTABLE_84 0x84
#define INFTABLE_85 0x85
#define INFTABLE_8B 0x8B
#define INFTABLE_8C 0x8C
#define INFTABLE_8D 0x8D
#define INFTABLE_8E 0x8E
#define INFTABLE_94 0x94
#define INFTABLE_97 0x97
#define INFTABLE_9A 0x9A
#define INFTABLE_A2 0xA2
#define INFTABLE_AB 0xAB
#define INFTABLE_B0 0xB0
#define INFTABLE_B1 0xB1
#define INFTABLE_B4 0xB4
#define INFTABLE_B6 0xB6
#define INFTABLE_B7 0xB7
#define INFTABLE_B8 0xB8
#define INFTABLE_B9 0xB9
#define INFTABLE_BC 0xBC
#define INFTABLE_C0 0xC0
#define INFTABLE_C1 0xC1
#define INFTABLE_C2 0xC2
#define INFTABLE_C3 0xC3
#define INFTABLE_C4 0xC4
#define INFTABLE_C5 0xC5
#define INFTABLE_C6 0xC6
#define INFTABLE_C7 0xC7
#define INFTABLE_C8 0xC8
#define INFTABLE_C9 0xC9
#define INFTABLE_CA 0xCA
#define INFTABLE_CB 0xCB
#define INFTABLE_CC 0xCC
#define INFTABLE_CD 0xCD
#define INFTABLE_CE 0xCE
#define INFTABLE_D0 0xD0
#define INFTABLE_D2 0xD2
#define INFTABLE_D4 0xD4
#define INFTABLE_D6 0xD6
#define INFTABLE_D8 0xD8
#define INFTABLE_D9 0xD9
#define INFTABLE_E0 0xE0
#define INFTABLE_E3 0xE3
#define INFTABLE_E6 0xE6
#define INFTABLE_EB 0xEB
#define INFTABLE_F0 0xF0
#define INFTABLE_F4 0xF4
#define INFTABLE_F8 0xF8
#define INFTABLE_FC 0xFC
#define INFTABLE_109 0x109
#define INFTABLE_10A 0x10A
#define INFTABLE_10B 0x10B
#define INFTABLE_10C 0x10C
#define INFTABLE_10D 0x10D
#define INFTABLE_10E 0x10E
#define INFTABLE_10F 0x10F
#define INFTABLE_113 0x113
#define INFTABLE_11A 0x11A
#define INFTABLE_11E 0x11E
#define INFTABLE_124 0x124
#define INFTABLE_129 0x129
#define INFTABLE_12A 0x12A
#define INFTABLE_138 0x138
#define INFTABLE_139 0x139
#define INFTABLE_140 0x140
#define INFTABLE_141 0x141
#define INFTABLE_142 0x142
#define INFTABLE_143 0x143
#define INFTABLE_144 0x144
#define INFTABLE_145 0x145
#define INFTABLE_146 0x146
#define INFTABLE_147 0x147
#define INFTABLE_160 0x160
#define INFTABLE_161 0x161
#define INFTABLE_162 0x162
#define INFTABLE_163 0x163
#define INFTABLE_164 0x164
#define INFTABLE_166 0x166
#define INFTABLE_16A 0x16A
#define INFTABLE_16C 0x16C
#define INFTABLE_170 0x170
#define INFTABLE_171 0x171
#define INFTABLE_172 0x172
#define INFTABLE_176 0x176
#define INFTABLE_178 0x178
#define INFTABLE_17C 0x17C
#define INFTABLE_17F 0x17F
#define INFTABLE_190 0x190
#define INFTABLE_191 0x191
#define INFTABLE_192 0x192
#define INFTABLE_193 0x193
#define INFTABLE_195 0x195
#define INFTABLE_196 0x196
#define INFTABLE_197 0x197
#define INFTABLE_198 0x198

// 0x199-0x19F
#define INFTABLE_199_19A_19B_19C_19D_19E_19F_INDEX 25
#define INFTABLE_199_MASK (1 << 9)
#define INFTABLE_19A_MASK (1 << 10)
#define INFTABLE_19B_MASK (1 << 11)
#define INFTABLE_19C_MASK (1 << 12)
#define INFTABLE_19D_MASK (1 << 13)
#define INFTABLE_19E_MASK (1 << 14)
#define INFTABLE_19F_MASK (1 << 15)

// 0x1A0-0x1AF
#define INFTABLE_1AX_INDEX 26
#define INFTABLE_1A0_SHIFT 0
#define INFTABLE_1A1_SHIFT 1
#define INFTABLE_1A2_SHIFT 2
#define INFTABLE_1A3_SHIFT 3
#define INFTABLE_1A4_SHIFT 4
#define INFTABLE_1A5_SHIFT 5
#define INFTABLE_1A6_SHIFT 6
#define INFTABLE_1A7_SHIFT 7
#define INFTABLE_1A8_SHIFT 8
#define INFTABLE_1A9_SHIFT 9
#define INFTABLE_1AB_SHIFT 11
#define INFTABLE_1AD_SHIFT 13

// 0x1D0-0x1DF
#define INFTABLE_1DX_INDEX 29


/*
 * SaveContext.eventInf
 */

// 0x00-0x0F
// horses related
#define EVENTINF_HORSES_INDEX 0
#define EVENTINF_HORSES_STATE_SHIFT 0
#define EVENTINF_HORSES_HORSETYPE_SHIFT 4
#define EVENTINF_HORSES_05_SHIFT 5
#define EVENTINF_HORSES_06_SHIFT 6
#define EVENTINF_HORSES_08_SHIFT 8
#define EVENTINF_HORSES_0A_SHIFT 10
#define EVENTINF_HORSES_0F_SHIFT 15 // unused?
#define EVENTINF_HORSES_STATE_MASK (0xF << EVENTINF_HORSES_STATE_SHIFT)
#define EVENTINF_HORSES_HORSETYPE_MASK (1 << EVENTINF_HORSES_HORSETYPE_SHIFT)
#define EVENTINF_HORSES_05_MASK (1 << EVENTINF_HORSES_05_SHIFT)
#define EVENTINF_HORSES_06_MASK (1 << EVENTINF_HORSES_06_SHIFT)
#define EVENTINF_HORSES_0F_MASK (1 << EVENTINF_HORSES_0F_SHIFT)
#define EVENTINF_HORSES_05 ((EVENTINF_HORSES_INDEX << 4) | EVENTINF_HORSES_05_SHIFT)
#define EVENTINF_HORSES_06 ((EVENTINF_HORSES_INDEX << 4) | EVENTINF_HORSES_06_SHIFT)
// Used in z_en_ta (Talon) to store Cucco game winning status
// and in z_en_ge1 (Gerudo) to store archery in-progress status
#define EVENTINF_HORSES_08 ((EVENTINF_HORSES_INDEX << 4) | EVENTINF_HORSES_08_SHIFT)
#define EVENTINF_CUCCO_GAME_WON EVENTINF_HORSES_08
// Used in z_en_ta (Talon) and z_en_ma3 (Malon) to store minigame finishing status
#define EVENTINF_HORSES_0A ((EVENTINF_HORSES_INDEX << 4) | EVENTINF_HORSES_0A_SHIFT)
#define EVENTINF_CUCCO_GAME_FINISHED EVENTINF_HORSES_0A

typedef enum {
    /* 0 */ EVENTINF_HORSES_STATE_0,
    /* 1 */ EVENTINF_HORSES_STATE_1,
    /* 2 */ EVENTINF_HORSES_STATE_2,
    /* 3 */ EVENTINF_HORSES_STATE_3,
    /* 4 */ EVENTINF_HORSES_STATE_4,
    /* 5 */ EVENTINF_HORSES_STATE_5,
    /* 6 */ EVENTINF_HORSES_STATE_6,
    /* 7 */ EVENTINF_HORSES_STATE_7
} EventInfHorsesState;

// "InRaceSeq"
#define GET_EVENTINF_HORSES_STATE() \
    ((gSaveContext.eventInf[EVENTINF_HORSES_INDEX] & EVENTINF_HORSES_STATE_MASK) >> EVENTINF_HORSES_STATE_SHIFT)
#define SET_EVENTINF_HORSES_STATE(v)                                                   \
    gSaveContext.eventInf[EVENTINF_HORSES_INDEX] =                                     \
        (gSaveContext.eventInf[EVENTINF_HORSES_INDEX] & ~EVENTINF_HORSES_STATE_MASK) | \
        ((v) << EVENTINF_HORSES_STATE_SHIFT)

#define GET_EVENTINF_HORSES_HORSETYPE() \
    ((gSaveContext.eventInf[EVENTINF_HORSES_INDEX] & EVENTINF_HORSES_HORSETYPE_MASK) >> EVENTINF_HORSES_HORSETYPE_SHIFT)
#define SET_EVENTINF_HORSES_HORSETYPE(v)                                                   \
    gSaveContext.eventInf[EVENTINF_HORSES_INDEX] =                                         \
        (gSaveContext.eventInf[EVENTINF_HORSES_INDEX] & ~EVENTINF_HORSES_HORSETYPE_MASK) | \
        ((v) << EVENTINF_HORSES_HORSETYPE_SHIFT)

#define SET_EVENTINF_HORSES_0F(v)                  \
    gSaveContext.eventInf[EVENTINF_HORSES_INDEX] = \
        (gSaveContext.eventInf[EVENTINF_HORSES_INDEX] & ~EVENTINF_HORSES_0F_MASK) | ((v) << EVENTINF_HORSES_0F_SHIFT)


#define EVENTINF_10 0x10

// 0x20-0x24
#define EVENTINF_20_21_22_23_24_INDEX 2
#define EVENTINF_20_MASK (1 << 0)
#define EVENTINF_21_MASK (1 << 1)
#define EVENTINF_22_MASK (1 << 2)
#define EVENTINF_23_MASK (1 << 3)
#define EVENTINF_24_MASK (1 << 4)

#define EVENTINF_30 0x30



#endif

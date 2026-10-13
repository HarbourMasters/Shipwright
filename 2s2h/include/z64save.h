#ifndef Z64SAVE_H
#define Z64SAVE_H

#include "ultra64.h"
#include "PR/os.h"
#include "z64item.h"
#include "z64math.h"
#include "unk.h"
#include "z64item.h"
#include "Rando/Types.h"

struct GameState;
struct PlayState;
struct FileSelectState;

typedef enum {
    /* 0 */ SAVE_AUDIO_STEREO,
    /* 1 */ SAVE_AUDIO_MONO,
    /* 2 */ SAVE_AUDIO_HEADSET,
    /* 3 */ SAVE_AUDIO_SURROUND
} AudioOption;

// TODO: properly name DOWN, RETURN and TOP
typedef enum RespawnMode {
    /* 0 */ RESPAWN_MODE_DOWN,                          // "RESTART_MODE_DOWN"
    /* 1 */ RESPAWN_MODE_RETURN,                        // "RESTART_MODE_RETURN"
    /* 2 */ RESPAWN_MODE_TOP,                           // "RESTART_MODE_TOP"
    /* 3 */ RESPAWN_MODE_UNK_3,                         // Related to grottos
    /* 4 */ RESPAWN_MODE_GORON,                         // "RESTART_MODE_GORON"
    /* 5 */ RESPAWN_MODE_ZORA,                          // "RESTART_MODE_ZORA"
    /* 6 */ RESPAWN_MODE_DEKU,                          // "RESTART_MODE_NUTS"
    /* 7 */ RESPAWN_MODE_HUMAN,                         // "RESTART_MODE_CHILD"
    /* 8 */ RESPAWN_MODE_MAX
} RespawnMode;

// 2S2H [Port] Increase the size of the Save Buffer to support more data, eg rando
#define SAVE_BUFFER_SIZE 0x4000 * 8
#define SAVE_BUFFER_SIZE_HALF (SAVE_BUFFER_SIZE / 2)

// 2S2H [Enhancement] Extended for file 3 support
typedef enum FileNum {
    /* 0 */ FILE_NUM_1,
    /* 1 */ FILE_NUM_2,
    /* 2 */ FILE_NUM_3,
    /* 3 */ FILE_NUM_MAX,
    /* 3 */ FILE_NUM_1_OWL_SAVE = FILE_NUM_MAX,
    /* 4 */ FILE_NUM_2_OWL_SAVE,
    /* 5 */ FILE_NUM_3_OWL_SAVE,
    /* 6 */ FILE_NUM_MAX_WITH_OWL_SAVE,
} FileNum;

#define FILE_NUM_OWL_SAVE_OFFSET FILE_NUM_1_OWL_SAVE

// 2S2H [Enhancement] Extended for file 3 support
typedef enum FlashSave {
    /*  0 */ FLASH_SAVE_FILE_1_NEW_CYCLE_SAVE,
    /*  1 */ FLASH_SAVE_FILE_1_NEW_CYCLE_SAVE_BACKUP,
    /*  2 */ FLASH_SAVE_FILE_2_NEW_CYCLE_SAVE,
    /*  3 */ FLASH_SAVE_FILE_2_NEW_CYCLE_SAVE_BACKUP,
    /*  4 */ FLASH_SAVE_FILE_3_NEW_CYCLE_SAVE,
    /*  5 */ FLASH_SAVE_FILE_3_NEW_CYCLE_SAVE_BACKUP,
    /*  6 */ FLASH_SAVE_FILE_1_OWL_SAVE,
    /*  7 */ FLASH_SAVE_FILE_1_OWL_SAVE_BACKUP,
    /*  8 */ FLASH_SAVE_FILE_2_OWL_SAVE,
    /*  9 */ FLASH_SAVE_FILE_2_OWL_SAVE_BACKUP,
    /* 10 */ FLASH_SAVE_FILE_3_OWL_SAVE,
    /* 11 */ FLASH_SAVE_FILE_3_OWL_SAVE_BACKUP,
    /* 12 */ FLASH_SAVE_SRAM_HEADER,
    /* 13 */ FLASH_SAVE_SRAM_HEADER_BACKUP,
    /* 14 */ FLASH_SAVE_MAX,
} FlashSave;

#define FLASH_SAVE_MAIN_MULTIPLIER 2
#define FLASH_SAVE_BACKUP_OFFSET 1

typedef enum {
    /*  0 */ MAGIC_STATE_IDLE, // Regular gameplay
    /*  1 */ MAGIC_STATE_CONSUME_SETUP, // Sets the speed at which the magic border flashes
    /*  2 */ MAGIC_STATE_CONSUME, // Consume magic until target is reached or no more magic is available
    /*  3 */ MAGIC_STATE_METER_FLASH_1, // Flashes border
    /*  4 */ MAGIC_STATE_METER_FLASH_2, // Flashes border and draws yellow magic to preview target consumption
    /*  5 */ MAGIC_STATE_RESET, // Reset colors and return to idle
    /*  6 */ MAGIC_STATE_METER_FLASH_3, // Flashes border with no additional behaviour
    /*  7 */ MAGIC_STATE_CONSUME_LENS, // Magic slowly consumed by Lens of Truth
    /*  8 */ MAGIC_STATE_STEP_CAPACITY, // Step `magicCapacity` to full capacity
    /*  9 */ MAGIC_STATE_FILL, // Add magic until magicFillTarget is reached
    /* 10 */ MAGIC_STATE_CONSUME_GORON_ZORA_SETUP,
    /* 11 */ MAGIC_STATE_CONSUME_GORON_ZORA, // Magic slowly consumed by Goron spiked rolling or Zora electric barrier.
    /* 12 */ MAGIC_STATE_CONSUME_GIANTS_MASK // Magic slowly consumed by Giant's Mask
} MagicState;

typedef enum {
    /* 0 */ MAGIC_CONSUME_NOW, // Consume magic immediately without preview
    /* 1 */ MAGIC_CONSUME_WAIT_NO_PREVIEW, // Sets consume target but waits to consume. No yellow magic preview to target consumption. Unused
    /* 2 */ MAGIC_CONSUME_NOW_ALT, // Identical behaviour to MAGIC_CONSUME_NOW. Unused
    /* 3 */ MAGIC_CONSUME_LENS, // Lens of Truth consumption
    /* 4 */ MAGIC_CONSUME_WAIT_PREVIEW, // Sets consume target but waits to consume. Show magic to be consumed in yellow.
    /* 5 */ MAGIC_CONSUME_GORON_ZORA, // Goron spiked rolling or Zora electric barrier slow consumption
    /* 6 */ MAGIC_CONSUME_GIANTS_MASK, // Giant's Mask slow consumption
    /* 7 */ MAGIC_CONSUME_DEITY_BEAM // Fierce Deity Beam consumption, consumed magic now and not via request
} MagicChangeType;

#define MAGIC_NORMAL_METER 0x30
#define MAGIC_DOUBLE_METER (2 * MAGIC_NORMAL_METER)

// See `R_MAGIC_DBG_SET_UPGRADE`
#define MAGIC_DBG_SET_UPGRADE_NO_ACTION 0
#define MAGIC_DBG_SET_UPGRADE_NORMAL_METER -1
#define MAGIC_DBG_SET_UPGRADE_DOUBLE_METER 1

#define ENV_HAZARD_TEXT_TRIGGER_HOTROOM (1 << 0)
#define ENV_HAZARD_TEXT_TRIGGER_UNDERWATER (1 << 1)

#define SECONDS_TO_TIMER(seconds) ((seconds) * 100)

#define OSTIME_TO_TIMER(osTime) ((osTime) * 64 / 3000 / 10000)
#define OSTIME_TO_TIMER_ALT(osTime) ((osTime) / 10000 * 64 / 3000)

// 1 centisecond = 10 milliseconds = 1/100 seconds
#define SECONDS_TO_TIMER_PRECISE(seconds, centiSeconds) ((seconds) * 100 + (centiSeconds))

typedef enum {
    /*  0 */ TIMER_ID_POSTMAN, // postman's counting minigame
    /*  1 */ TIMER_ID_MINIGAME_1, // minigame timer
    /*  2 */ TIMER_ID_2,
    /*  3 */ TIMER_ID_MOON_CRASH, // timer used for mooncrash on the clocktower roof
    /*  4 */ TIMER_ID_MINIGAME_2, // minigame timer
    /*  5 */ TIMER_ID_ENV_HAZARD, // environmental hazard timer (underwater or hot room)
    /*  6 */ TIMER_ID_GORON_RACE_UNUSED,
    /*  7 */ TIMER_ID_MAX,
    /* 99 */ TIMER_ID_NONE = 99
} TimerId;

typedef enum {
    /* 0 */ TIMER_COUNT_DOWN,
    /* 1 */ TIMER_COUNT_UP
} TimerDirection;

typedef enum {
    /*  0 */ TIMER_STATE_OFF,
    /*  1 */ TIMER_STATE_START,
    /*  2 */ TIMER_STATE_HOLD_TIMER, // Hold timer frozen at the screen center
    /*  3 */ TIMER_STATE_MOVING_TIMER, // Move timer to a target location
    /*  4 */ TIMER_STATE_COUNTING,
    /*  5 */ TIMER_STATE_STOP,
    /*  6 */ TIMER_STATE_6, // like `TIMER_STATE_STOP` but with extra minigame checks
    /*  7 */ TIMER_STATE_7, // stopped but still update `timerCurTimes`
    /*  8 */ TIMER_STATE_ENV_HAZARD_START,
    /*  9 */ TIMER_STATE_ALT_START,
    /* 10 */ TIMER_STATE_10, // precursor to `TIMER_STATE_ALT_COUNTING`
    /* 11 */ TIMER_STATE_ALT_COUNTING,
    /* 12 */ TIMER_STATE_12, // Updated paused time?
    /* 13 */ TIMER_STATE_POSTMAN_START,
    /* 14 */ TIMER_STATE_POSTMAN_COUNTING,
    /* 15 */ TIMER_STATE_POSTMAN_STOP,
    /* 16 */ TIMER_STATE_POSTMAN_END
} TimerState;

typedef enum BottleTimerState {
    /* 0 */ BOTTLE_TIMER_STATE_OFF,
    /* 1 */ BOTTLE_TIMER_STATE_COUNTING
} BottleTimerState;

typedef enum {
    /* 0 */ MINIGAME_STATUS_INACTIVE,
    /* 1 */ MINIGAME_STATUS_ACTIVE,
    /* 3 */ MINIGAME_STATUS_END = 3
} MinigameStatus;

typedef enum HudVisibility {
    /*  0 */ HUD_VISIBILITY_IDLE,
    /*  1 */ HUD_VISIBILITY_NONE,
    /*  2 */ HUD_VISIBILITY_NONE_ALT, // Identical to HUD_VISIBILITY_NONE
    /*  3 */ HUD_VISIBILITY_HEARTS_WITH_OVERWRITE, // Uses Interface_UpdateButtonAlphas so gives the opportunity to dim only disabled buttons
    /*  4 */ HUD_VISIBILITY_A,
    /*  5 */ HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE, // Uses Interface_UpdateButtonAlphas so gives the opportunity to dim only disabled buttons
    /*  6 */ HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_WITH_OVERWRITE, // Uses Interface_UpdateButtonAlphas so gives the opportunity to dim only disabled buttons
    /*  7 */ HUD_VISIBILITY_ALL_NO_MINIMAP_W_DISABLED, // Only raises button alphas if not disabled
    /*  8 */ HUD_VISIBILITY_B,
    /*  9 */ HUD_VISIBILITY_HEARTS_MAGIC,
    /* 10 */ HUD_VISIBILITY_B_ALT,
    /* 11 */ HUD_VISIBILITY_HEARTS,
    /* 12 */ HUD_VISIBILITY_A_B_MINIMAP,
    /* 13 */ HUD_VISIBILITY_HEARTS_MAGIC_WITH_OVERWRITE, // Uses Interface_UpdateButtonAlphas so gives the opportunity to dim only disabled buttons
    /* 14 */ HUD_VISIBILITY_HEARTS_MAGIC_C,
    /* 15 */ HUD_VISIBILITY_ALL_NO_MINIMAP,
    /* 16 */ HUD_VISIBILITY_A_B_C,
    /* 17 */ HUD_VISIBILITY_B_MINIMAP,
    /* 18 */ HUD_VISIBILITY_HEARTS_MAGIC_MINIMAP,
    /* 19 */ HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP,
    /* 20 */ HUD_VISIBILITY_B_MAGIC,
    /* 21 */ HUD_VISIBILITY_A_B,
    /* 22 */ HUD_VISIBILITY_A_B_HEARTS_MAGIC_MINIMAP,
    /* 50 */ HUD_VISIBILITY_ALL = 50,
    /* 52 */ HUD_VISIBILITY_NONE_INSTANT = 52
} HudVisibility;

// Based on sRupeesTextLocalization
typedef enum Language {
    /* 0 */ LANGUAGE_JPN,
    /* 1 */ LANGUAGE_ENG,
    /* 2 */ LANGUAGE_GER,
    /* 3 */ LANGUAGE_FRE,
    /* 4 */ LANGUAGE_SPA,
    /* 5 */ LANGUAGE_MAX
} Language;

typedef enum HighScore {
    /* 0 */ HS_BANK_RUPEES,
    /* 1 */ HS_UNK_1,
    /* 2 */ HS_FISHING, // Fishing flags
    /* 3 */ HS_BOAT_ARCHERY,
    /* 4 */ HS_HORSE_BACK_BALLOON,
    /* 5 */ HS_LOTTERY_GUESS, // Lottery code chosen by player (only uses lower three hex digits)
    /* 6 */ HS_SHOOTING_GALLERY, // High scores for both shooting galleries. Town uses lower 16 bits, Swamp uses higher 16 bits.
    /* 7 */ HS_MAX
} HighScore;

#define PICTO_PHOTO_WIDTH 160
#define PICTO_PHOTO_HEIGHT 112

#define PICTO_PHOTO_TOPLEFT_X ((SCREEN_WIDTH - PICTO_PHOTO_WIDTH) / 2)
#define PICTO_PHOTO_TOPLEFT_Y ((SCREEN_HEIGHT - PICTO_PHOTO_HEIGHT) / 2)

#define PICTO_PHOTO_SIZE (PICTO_PHOTO_WIDTH * PICTO_PHOTO_HEIGHT)
#define PICTO_PHOTO_COMPRESSED_SIZE (PICTO_PHOTO_SIZE * 5 / 8)

typedef struct SramContext {
    /* 0x00 */ u8* noFlashSaveBuf; // Never allocated memory
    /* 0x04 */ u8* saveBuf;
    /* 0x08 */ char unk_08[4];
    /* 0x0C */ s16 status;
    /* 0x10 */ u32 curPage;
    /* 0x14 */ u32 numPages;
    /* 0x18 */ OSTime startWriteOsTime;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ s16 unk_24;
} SramContext; // size = 0x28

typedef struct ItemEquips {
    /* 0x00 */ u8 buttonItems[4][4];                    // "register_item"
    /* 0x10 */ u8 cButtonSlots[4][4];                   // "register_item_pt"
    /* 0x20 */ u16 equipment;
} ItemEquips; // size = 0x22

typedef struct Inventory {
    /* 0x00 */ u8 items[48];                            // "item_register", first 24 elements are normal items and the other 24 are masks
    /* 0x30 */ s8 ammo[24];                             // "item_count"
    /* 0x48 */ u32 upgrades;                            // "non_equip_register" some bits are wallet upgrades
    /* 0x4C */ u32 questItems;                          // "collect_register"
    /* 0x50 */ u8 dungeonItems[10];                     // "key_compass_map"
    /* 0x5A */ s8 dungeonKeys[9];                       // "key_register"
    /* 0x63 */ s8 defenseHearts;
    /* 0x64 */ s8 strayFairies[10];                     // "orange_fairy"
    // 2S2H [Comment] These char[] are not null-terminated, they will not work correctly in string functions
    /* 0x6E */ char dekuPlaygroundPlayerName[3][8];     // "degnuts_memory_name" Stores playerName (8 char) over (3 days) when getting a new high score
} Inventory; // size = 0x88

typedef struct HorseData {
    /* 0x0 */ s16 sceneId;                             // "spot_no"
    /* 0x2 */ Vec3s pos;                               // "horse_x", "horse_y" and "horse_z"
    /* 0x8 */ s16 yaw;                                 // "horse_a"
} HorseData; // size = 0xA

typedef struct RespawnData {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
    /* 0x0E */ s16 playerParams;
    /* 0x10 */ u16 entrance;
    /* 0x12 */ u8 roomIndex;
    /* 0x13 */ s8 data;
    /* 0x14 */ u32 tempSwitchFlags;
    /* 0x18 */ u32 unk_18;
    /* 0x1C */ u32 tempCollectFlags;
} RespawnData; // size = 0x20

typedef struct PermanentSceneFlags {
    /* 0x00 */ u32 chest;
    /* 0x04 */ u32 switch0;
    /* 0x08 */ u32 switch1;
    /* 0x0C */ u32 clearedRoom;
    /* 0x10 */ u32 collectible;
    /* 0x14 */ u32 unk_14; // varies based on scene. For dungeons, floors visited.
    /* 0x18 */ u32 rooms;
} PermanentSceneFlags; // size = 0x1C

typedef struct CycleSceneFlags {
    /* 0x00 */ u32 chest;
    /* 0x04 */ u32 switch0;
    /* 0x08 */ u32 switch1;
    /* 0x0C */ u32 clearedRoom;
    /* 0x10 */ u32 collectible;
} CycleSceneFlags; // size = 0x14

typedef struct SaveOptions {
    /* 0x0 */ u16 optionId;                            // "option_id"
    /* 0x2 */ u8 language;                             // "j_n"
    /* 0x3 */ u8 audioSetting;                         // "s_sound"
    /* 0x4 */ u8 languageSetting;                      // "language"
    /* 0x5 */ u8 zTargetSetting;                       // "z_attention", 0: Switch; 1: Hold
} SaveOptions; // size = 0x6

typedef struct SavePlayerData {
    // 2S2H [Comment] These char[] are not null-terminated, they will not work correctly in string functions
    /* 0x00 */ char newf[6];                          // "newf"               Will always be "ZELDA3 for a valid save
    /* 0x06 */ u16 threeDayResetCount;                // "savect"
    /* 0x08 */ char playerName[8];                    // "player_name"
    /* 0x10 */ s16 healthCapacity;                    // "max_life"
    /* 0x12 */ s16 health;                            // "now_life"
    /* 0x14 */ s8 magicLevel; // 0 for no magic/new load, 1 for magic, 2 for double magic "magic_max"
    /* 0x15 */ s8 magic; // current magic available for use "magic_now"
    /* 0x16 */ s16 rupees;                            // "lupy_count"
    /* 0x18 */ u16 swordHealth;                       // "long_sword_hp"
    /* 0x1A */ u16 tatlTimer;                         // "navi_timer"
    /* 0x1C */ u8 isMagicAcquired;                    // "magic_mode"
    /* 0x1D */ u8 isDoubleMagicAcquired;              // "magic_ability"
    /* 0x1E */ u8 doubleDefense;                      // "life_ability"
    /* 0x1F */ u8 unk_1F;                             // "ocarina_round"
    /* 0x20 */ u8 owlWarpId; // See `OwlWarpId`, "first_memory"
    /* 0x22 */ u16 owlActivationFlags;                // "memory_warp_point"
    /* 0x24 */ u8 unk_24;                             // "last_warp_pt"
    /* 0x26 */ s16 savedSceneId;                      // "scene_data_ID"
} SavePlayerData; // size = 0x28

typedef struct SaveInfo {
    /* 0x000 */ SavePlayerData playerData;
    /* 0x028 */ ItemEquips equips;
    /* 0x04C */ Inventory inventory;
    /* 0x0D4 */ PermanentSceneFlags permanentSceneFlags[120];
    /* 0xDF4 */ UNK_TYPE1 unk_DF4[0x54];
    /* 0xE48 */ u32 dekuPlaygroundHighScores[3];
    /* 0xE54 */ u32 pictoFlags0;                       // Flags set by `PictoActor`s if pictograph is valid
    /* 0xE58 */ u32 pictoFlags1;                       // Flags set by Snap_ValidatePictograph() to record errors; volatile since that function is run many times in succession
    /* 0xE5C */ u32 unk_E5C;
    /* 0xE60 */ u32 unk_E60;
    /* 0xE64 */ u32 alienInfo[7];                      // Used by EnInvadepoh to hold alien spawn times and how many aliens the player has killed
    /* 0xE80 */ u32 scenesVisible[7];                  // tingle maps and clouded regions on pause map. Stores scenes bitwise for up to 224 scenes even though there are not that many scenes
    /* 0xE9C */ u32 skullTokenCount;                   // upper 16 bits store Swamp skulls, lower 16 bits store Ocean skulls
    /* 0xEA0 */ u32 unk_EA0;                           // Gossic stone heart piece flags
    /* 0xEA4 */ u32 unk_EA4;
    /* 0xEA8 */ u32 unk_EA8[2];                        // Related to blue warps
    /* 0xEB0 */ u32 stolenItems;                       // Items stolen by Takkuri and given to Curiosity Shop Man
    /* 0xEB4 */ u32 unk_EB4;
    /* 0xEB8 */ u32 highScores[HS_MAX];
    /* 0xED4 */ u8 weekEventReg[100];                  // "week_event_reg"
    /* 0xF38 */ u32 regionsVisited;                    // "area_arrival"
    /* 0xF3C */ u32 worldMapCloudVisibility;           // "cloud_clear"
    /* 0xF40 */ u8 unk_F40;                            // "oca_rec_flag"                   has scarecrows song
    /* 0xF41 */ u8 scarecrowSpawnSongSet;              // "oca_rec_flag8"
    /* 0xF42 */ u8 scarecrowSpawnSong[128];
    /* 0xFC2 */ s8 bombersCaughtNum;                   // "aikotoba_index"
    /* 0xFC3 */ s8 bombersCaughtOrder[5];              // "aikotoba_table"
    /* 0xFC8 */ s8 lotteryCodes[3][3];                 // "numbers_table", Preset lottery codes
    /* 0xFD1 */ s8 spiderHouseMaskOrder[6];            // "kinsta_color_table"
    /* 0xFD7 */ s8 bomberCode[5];                      // "bombers_aikotoba_table"
    /* 0xFDC */ HorseData horseData;
    /* 0xFE6 */ u16 checksum;                          // "check_sum"
} SaveInfo; // size = 0xFE8

// #region 2S2H
typedef struct DpadSaveInfo {
    u8 dpadItems[4][4];
    u8 dpadSlots[4][4];
} DpadSaveInfo;

typedef enum {
    SAVETYPE_VANILLA,
    SAVETYPE_RANDO,
} SaveType;

typedef struct RandoSaveCheck {
    RandoItemId randoItemId;
    bool shuffled;
    bool eligible;
    bool cycleObtained;
    bool obtained;
    bool skipped;
    u16 price; // Only applicable for shops/merchants
} RandoSaveCheck;

typedef struct RandoSaveInfo {
    u16 randoInf[(RANDO_INF_MAX + 15) / 16];
    u8 randoEvents[RE_MAX]; // This is purely for logic tracking, not to be used for anything else
    RandoSaveCheck randoSaveChecks[RC_MAX];
    u32 finalSeed;
    u32 randoSaveOptions[RO_MAX]; // Type here may change in the future
    u16 randoStartingItems[256]; // Max 256 starting items, using u16 in case we add more than 255 items
    s8 foundDungeonKeys[9]; // Tracks the number of dungeon keys found, opposed to the number of keys in the inventory
    u16 foundTriforcePieces;
    u8 sariaHintsAvailable;
    u16 sariaPriorityItems[16];
} RandoSaveInfo;

// These are values added by 2S2H that we need to be persisted to the save file
// See `ShipSaveContext` for values on the SaveContext that aren't persisted.
typedef struct ShipSaveInfo {
    DpadSaveInfo dpadEquips;
    s32 pauseSaveEntrance;
    SaveType saveType;
    uint64_t fileCreatedAt;
    uint64_t fileCompletedAt;
    uint64_t filePlaytime;
    RespawnData respawn[RESPAWN_MODE_MAX];
    char commitHash[8];
    RandoSaveInfo rando;
    u8 bombArrowsEquipped;
    u8 persistentBunnyHood;
} ShipSaveInfo;
// #endregion

typedef struct Save {
    /* 0x00 */ s32 entrance;        // "scene_no"
    /* 0x04 */ u8 equippedMask;     // "player_mask"
    /* 0x05 */ u8 isFirstCycle;     // "opening_flag"
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 linkAge;          // "link_age"
    /* 0x08 */ s32 cutsceneIndex;   // "day_time"
    /* 0x0C */ u16 time;            // "zelda_time"
    /* 0x0E */ u16 owlWarpId;       // See `OwlWarpId` enum
    /* 0x10 */ s32 isNight;         // "asahiru_fg"
    /* 0x14 */ s32 timeSpeedOffset; // "change_zelda_time"
    /* 0x18 */ s32 day;             // "totalday"
    /* 0x1C */ s32 eventDayCount;   // "eventday"
    /* 0x20 */ u8 playerForm;       // "player_character"
    /* 0x21 */ u8 snowheadCleared;  // "spring_flag"
    /* 0x22 */ u8 hasTatl;          // "bell_flag"
    /* 0x23 */ u8 isOwlSave;
    /* 0x24 */ SaveInfo saveInfo;
    /* 2S2H */ ShipSaveInfo shipSaveInfo;
} Save; // size = 0x100C

// #region 2S2H
typedef struct DpadSaveContext {
    u8 status[4];
} DpadSaveContext;

// These are values added by 2S2H that aren't persisted to the save file
// See `ShipSaveInfo` for values on the SaveContext that are persisted.
typedef struct ShipSaveContext {
    DpadSaveContext dpad;
    uint64_t lastTimeLog;
} ShipSaveContext;
// #endregion

typedef struct SaveContext {
    /* 0x0000 */ Save save;
    /* 0x100C */ u8 eventInf[8]; // "event_inf"
    /* 0x1014 */ u8 unk_1014;    // "stone_set_flag"
    /* 0x1015 */ u8 bButtonStatus;
    /* 0x1016 */ u16 jinxTimer;
    /* 0x1018 */ s16 rupeeAccumulator;                       // "lupy_udct"
    /* 0x101A */ u8 bottleTimerStates[BOTTLE_MAX];           // See the `BottleTimerState` enum. "bottle_status"
    /* 0x1020 */ OSTime bottleTimerStartOsTimes[BOTTLE_MAX]; // The osTime when the timer starts. "bottle_ostime"
    /* 0x1050 */ u64 bottleTimerTimeLimits[BOTTLE_MAX]; // The original total time given before the timer expires, in centiseconds (1/100th sec). "bottle_sub"
    /* 0x1080 */ u64 bottleTimerCurTimes[BOTTLE_MAX];   // The remaining time left before the timer expires, in centiseconds (1/100th sec). "bottle_time"
    /* 0x10B0 */ OSTime bottleTimerPausedOsTimes[BOTTLE_MAX]; // The cumulative osTime spent with the timer paused. "bottle_stop_time"
    /* 0x10E0 */ u8 pictoPhotoI5[PICTO_PHOTO_COMPRESSED_SIZE]; // buffer containing the pictograph photo, compressed to I5 from I8
    /* 0x3CA0 */ s32 fileNum;                           // "file_no"
    /* 0x3CA4 */ s16 powderKegTimer;                    // "big_bom_timer"
    /* 0x3CA6 */ u8 unk_3CA6;
    /* 0x3CA7 */ u8 unk_3CA7;                           // "day_night_flag"
    /* 0x3CA8 */ s32 gameMode;                          // "mode"
    /* 0x3CAC */ s32 sceneLayer;                        // "counter"
    /* 0x3CB0 */ s32 respawnFlag;                       // "restart_flag"
    /* 0x3CB4 */ RespawnData respawn[RESPAWN_MODE_MAX]; // "restart_data"
    /* 0x3DB4 */ f32 entranceSpeed;                     // "player_wipe_speedF"
    /* 0x3DB8 */ u16 entranceSound;                     // "player_wipe_door_SE"
    /* 0x3DBA */ u8 unk_3DBA;                           // "player_wipe_item"
    /* 0x3DBB */ u8 retainWeatherMode;                  // "next_walk"
    /* 0x3DBC */ s16 dogParams;                         // OoT leftover. "dog_flag"
    /* 0x3DBE */ u8 envHazardTextTriggerFlags;          // "guide_status"
    /* 0x3DBF */ u8 showTitleCard;                      // "name_display"
    /* 0x3DC0 */ s16 nayrusLoveTimer;                   // remnant of OoT, "shield_magic_timer"
    /* 0x3DC2 */ u8 unk_3DC2;                           // "pad1"
    /* 0x3DC8 */ OSTime postmanTimerStopOsTime; // The osTime when the timer stops for the postman minigame. "get_time"
    /* 0x3DD0 */ u8 timerStates[TIMER_ID_MAX];  // See the `TimerState` enum. "event_fg"
    /* 0x3DD7 */ u8 timerDirections[TIMER_ID_MAX]; // See the `TimerDirection` enum. "calc_flag"
    /* 0x3DE0 */ u64 timerCurTimes[TIMER_ID_MAX]; // For countdown, the remaining time left. For countup, the time since the start. In centiseconds (1/100th sec). "event_ostime"
    /* 0x3E18 */ u64 timerTimeLimits[TIMER_ID_MAX]; // The original total time given for the timer to count from, in centiseconds (1/100th sec). "event_sub"
    /* 0x3E50 */ OSTime timerStartOsTimes[TIMER_ID_MAX]; // The osTime when the timer starts. "func_time"
    /* 0x3E88 */ u64 timerStopTimes[TIMER_ID_MAX];  // The total amount of time taken between the start and end of the timer, in centiseconds (1/100th sec). "func_end_time"
    /* 0x3EC0 */ OSTime timerPausedOsTimes[TIMER_ID_MAX]; // The cumulative osTime spent with the timer paused. "func_stop_time"
    /* 0x3EF8 */ s16 timerX[TIMER_ID_MAX]; // "event_xp"
    /* 0x3F06 */ s16 timerY[TIMER_ID_MAX]; // "event_yp"
    /* 0x3F14 */ s16 unk_3F14;             // "character_change"
    // 2S2H [Custom Audio]. Was originally u8 seqId. Made 16 bit to allow for more than 255 sequences.
    /* 0x3F16 */ u16 seqId;                // "old_bgm"
    /* 0x3F17 */ u8 ambienceId;            // "old_env"
    /* 0x3F18 */ u8 buttonStatus[6];       // "button_item"
    /* 0x3F1E */ u8 hudVisibilityForceButtonAlphasByStatus; // if btn alphas are updated through Interface_UpdateButtonAlphas, instead update them through Interface_UpdateButtonAlphasByStatus "ck_fg"
    /* 0x3F20 */ u16 nextHudVisibility;  // triggers the hud to change visibility to the requested value. Reset to HUD_VISIBILITY_IDLE when target is reached "alpha_type"
    /* 0x3F22 */ u16 hudVisibility;      // current hud visibility "prev_alpha_type"
    /* 0x3F24 */ u16 hudVisibilityTimer; // number of frames in the transition to a new hud visibility. Used to step alpha "alpha_count"
    /* 0x3F26 */ u16 prevHudVisibility; // used to store and recover hud visibility for pause menu and text boxes "last_time_type"
    /* 0x3F28 */ s16 magicState;          // determines magic meter behavior on each frame "magic_flag"
    /* 0x3F2A */ s16 isMagicRequested;    // a request to add magic has been given "recovery_magic_flag"
    /* 0x3F2C */ s16 magicFlag;           // Set to 0 in func_80812D94(), otherwise unused "keep_magic_flag"
    /* 0x3F2E */ s16 magicCapacity;       // maximum magic available "magic_now_max"
    /* 0x3F30 */ s16 magicFillTarget;     // target used to fill magic "magic_now_now"
    /* 0x3F32 */ s16 magicToConsume;      // accumulated magic that is requested to be consumed "magic_used"
    /* 0x3F34 */ s16 magicToAdd;          // accumulated magic that is requested to be added "magic_recovery"
    /* 0x3F36 */ u16 mapIndex;            // set to enum DungeonSceneIndex when entering a dungeon related scene, or Map_GetMapIndexForOverworld on certain overworld scenes "scene_ID"
    /* 0x3F38 */ u16 minigameStatus;      // "yabusame_mode"
    /* 0x3F3A */ u16 minigameScore;       // "yabusame_total"
    /* 0x3F3C */ u16 minigameHiddenScore; // "yabusame_out_ct"
    /* 0x3F3E */ u8 unk_3F3E;             // "no_save"
    /* 0x3F3F */ u8 flashSaveAvailable;   // "flash_flag"
    /* 0x3F40 */ SaveOptions options;
    /* 0x3F46 */ u16 forcedSeqId;              // "NottoriBgm"
    /* 0x3F48 */ u8 cutsceneTransitionControl; // "fade_go"
    /* 0x3F4A */ u16 nextCutsceneIndex;        // "next_daytime"
    /* 0x3F4C */ u8 cutsceneTrigger;           // "doukidemo"
    /* 0x3F4D */ u8 chamberCutsceneNum;        // remnant of OoT "Kenjya_no"
    /* 0x3F4E */ u16 nextDayTime;              // "next_zelda_time"
    /* 0x3F50 */ u8 transFadeDuration;         // "fade_speed"
    /* 0x3F51 */ u8 transWipeSpeed;            // "wipe_speed"           transition related
    /* 0x3F52 */ u16 skyboxTime;               // "kankyo_time"
    /* 0x3F54 */ u8 dogIsLost;                 // "dog_event_flag"
    /* 0x3F55 */ u8 nextTransitionType;        // "next_wipe"
    /* 0x3F56 */ s16 worldMapArea;             // "area_type"
    /* 0x3F58 */ s16 sunsSongState;            // "sunmoon_flag"
    /* 0x3F5A */ s16 healthAccumulator;        // "life_mode"
    /* 0x3F5C */ s32 unk_3F5C;                 // "bet_rupees"
    /* 0x3F60 */ u8 screenScaleFlag;           // "framescale_flag"
    /* 0x3F64 */ f32 screenScale;              // "framescale_scale"
    /* 0x3F68 */ CycleSceneFlags cycleSceneFlags[120]; // Scene flags that are temporarily stored over the duration of a single 3-day cycle
    /* 0x48C8 */ u16 dungeonSceneSharedIndex; // similar to mapIndex, except values correspond to one of the four dungeons "scene_id_mix"
    /* 0x48CA */ u8 masksGivenOnMoon[27];     // bit-packed, masks given away on the Moon. "mask_mask_bit"
    /*  2S2H  */ ShipSaveContext shipSaveContext; 
} SaveContext; // size = 0x48C8

typedef enum ButtonStatus {
    /* 0x00 */ BTN_ENABLED,
    /* 0xFF */ BTN_DISABLED = 0xFF
} ButtonStatus;

typedef enum SunsSongState {
    /* 0 */ SUNSSONG_INACTIVE,
    /* 1 */ SUNSSONG_START, // the suns ocarina effect signals that the song has finished playing
    /* 2 */ SUNSSONG_SPEED_TIME, // suns was played where time passes, speed up the advancement of time
    /* 3 */ SUNSSONG_SPECIAL // time does not advance, but signals the song was played. used for freezing redeads
} SunsSongState;

typedef enum {
    /* 0 */ GAMEMODE_NORMAL,
    /* 1 */ GAMEMODE_TITLE_SCREEN,
    /* 2 */ GAMEMODE_FILE_SELECT,
    /* 3 */ GAMEMODE_END_CREDITS,
    /* 4 */ GAMEMODE_OWL_SAVE
} GameMode;

// linkAge still exists in MM, but is always set to 0 (always adult)
// There are remnants of these macros from OOT, but they are essentially useless
#define LINK_IS_CHILD (gSaveContext.save.linkAge == 1)
#define LINK_IS_ADULT (gSaveContext.save.linkAge == 0)

#define YEARS_CHILD 5
#define YEARS_ADULT 17
#define LINK_AGE_IN_YEARS (!LINK_IS_ADULT ? YEARS_CHILD : YEARS_ADULT)

#define CURRENT_DAY (((void)0, gSaveContext.save.day) % 5)
#define CURRENT_TIME ((void)0, gSaveContext.save.time)

// The day begins at CLOCK_TIME(6, 0) so it must be offset.
#define TIME_UNTIL_MOON_CRASH \
    ((4 - CURRENT_DAY) * DAY_LENGTH - (u16)(CURRENT_TIME - CLOCK_TIME(6, 0)))
#define TIME_UNTIL_NEW_DAY (DAY_LENGTH - (u16)(CURRENT_TIME - CLOCK_TIME(6, 0)))

#define GET_PLAYER_FORM ((void)0, gSaveContext.save.playerForm)

#define GET_OWL_STATUE_ACTIVATED(owlWarpId) (((void)0, gSaveContext.save.saveInfo.playerData.owlActivationFlags) & (u16)gBitFlags[(owlWarpId)])
#define SET_OWL_STATUE_ACTIVATED(owlWarpId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags = (((void)0, gSaveContext.save.saveInfo.playerData.owlActivationFlags) | (u16)gBitFlags[(owlWarpId)]))

#define SLOT(item) gItemSlots[item]
#define AMMO(item) gSaveContext.save.saveInfo.inventory.ammo[SLOT(item)]
#define INV_CONTENT(item) gSaveContext.save.saveInfo.inventory.items[SLOT(item)]
#define GET_INV_CONTENT(item) ((void)0, gSaveContext.save.saveInfo.inventory.items)[SLOT(item)]

#define CUR_FORM ((gSaveContext.save.playerForm == PLAYER_FORM_HUMAN) ? 0 : gSaveContext.save.playerForm)

#define GET_SAVE_EQUIPS_EQUIPMENT ((void)0, gSaveContext.save.saveInfo.equips.equipment)
#define GET_SAVE_INVENTORY_UPGRADES ((void)0, gSaveContext.save.saveInfo.inventory.upgrades)
#define GET_SAVE_INVENTORY_QUEST_ITEMS ((void)0, gSaveContext.save.saveInfo.inventory.questItems)

#define GET_CUR_EQUIP_VALUE(equip) ((GET_SAVE_EQUIPS_EQUIPMENT & gEquipMasks[equip]) >> gEquipShifts[equip])

#define CUR_UPG_VALUE(upg) ((gSaveContext.save.saveInfo.inventory.upgrades & gUpgradeMasks[upg]) >> gUpgradeShifts[upg])
#define GET_CUR_UPG_VALUE(upg) ((GET_SAVE_INVENTORY_UPGRADES & gUpgradeMasks[upg]) >> gUpgradeShifts[upg])

#define SET_EQUIP_VALUE(equip, value) (gSaveContext.save.saveInfo.equips.equipment = ((GET_SAVE_EQUIPS_EQUIPMENT & gEquipNegMasks[equip]) | (u16)((u16)(value) << gEquipShifts[equip])))

#define BUTTON_ITEM_EQUIP(form, button) (gSaveContext.save.saveInfo.equips.buttonItems[form][button])
#define CUR_FORM_EQUIP(button) BUTTON_ITEM_EQUIP(CUR_FORM, button)

#define C_SLOT_EQUIP(form, button) (gSaveContext.save.saveInfo.equips.cButtonSlots[form][button])
#define CHECK_QUEST_ITEM(item) (GET_SAVE_INVENTORY_QUEST_ITEMS & gBitFlags[item])
#define SET_QUEST_ITEM(item) (gSaveContext.save.saveInfo.inventory.questItems = (GET_SAVE_INVENTORY_QUEST_ITEMS | gBitFlags[item]))
#define REMOVE_QUEST_ITEM(item) (gSaveContext.save.saveInfo.inventory.questItems = (GET_SAVE_INVENTORY_QUEST_ITEMS & (-1 - gBitFlags[item])))
#define TOGGLE_QUEST_ITEM(item) (gSaveContext.save.saveInfo.inventory.questItems ^= (gBitFlags[item]))

#define GET_QUEST_HEART_PIECE_COUNT ((GET_SAVE_INVENTORY_QUEST_ITEMS & 0xF0000000) >> QUEST_HEART_PIECE_COUNT)
#define EQ_MAX_QUEST_HEART_PIECE_COUNT ((GET_SAVE_INVENTORY_QUEST_ITEMS & 0xF0000000) == (4 << QUEST_HEART_PIECE_COUNT))
#define LEQ_MAX_QUEST_HEART_PIECE_COUNT ((GET_SAVE_INVENTORY_QUEST_ITEMS & 0xF0000000) <= (4 << QUEST_HEART_PIECE_COUNT))
#define INCREMENT_QUEST_HEART_PIECE_COUNT (gSaveContext.save.saveInfo.inventory.questItems += (1 << QUEST_HEART_PIECE_COUNT))
#define DECREMENT_QUEST_HEART_PIECE_COUNT (gSaveContext.save.saveInfo.inventory.questItems -= (1 << QUEST_HEART_PIECE_COUNT))
#define RESET_HEART_PIECE_COUNT (gSaveContext.save.saveInfo.inventory.questItems ^= (4 << QUEST_HEART_PIECE_COUNT))

#define CHECK_DUNGEON_ITEM(item, dungeonSceneIndex) (gSaveContext.save.saveInfo.inventory.dungeonItems[(void)0, dungeonSceneIndex] & gBitFlags[item])
#define CHECK_DUNGEON_ITEM_ALT(item, dungeonSceneIndex) (gSaveContext.save.saveInfo.inventory.dungeonItems[dungeonSceneIndex] & gBitFlags[item])
#define SET_DUNGEON_ITEM(item, dungeonSceneIndex) (gSaveContext.save.saveInfo.inventory.dungeonItems[(void)0, dungeonSceneIndex] |= (u8)gBitFlags[item])
// 2S2H [Enhancements] We added this for save editor
#define REMOVE_DUNGEON_ITEM(item, dungeonIndex) (gSaveContext.save.saveInfo.inventory.dungeonItems[(void)0, dungeonIndex] &= ~(u8)gBitFlags[item])
#define DUNGEON_KEY_COUNT(dungeonSceneIndex) (gSaveContext.save.saveInfo.inventory.dungeonKeys[(void)0, dungeonSceneIndex])
#define GET_DUNGEON_FLOOR_VISITED(sceneId, floor) (gSaveContext.save.saveInfo.permanentSceneFlags[(sceneId)].unk_14 & gBitFlags[floor])
#define SET_DUNGEON_FLOOR_VISITED(sceneId, floor) (gSaveContext.save.saveInfo.permanentSceneFlags[(sceneId)].unk_14 |= gBitFlags[floor])
#define GET_ROOM_VISITED(sceneId, room) (((void)0, gSaveContext.save.saveInfo.permanentSceneFlags[(sceneId)].rooms) & (1 << (room)))
#define SET_ROOM_VISITED(sceneId, room) (gSaveContext.save.saveInfo.permanentSceneFlags[(sceneId)].rooms |= gBitFlags[room])
#define GET_CYCLE_CHEST_OPENED(sceneId, chestFlagId) ((void)0, gSaveContext.cycleSceneFlags[(sceneId)].chest) & (1 << (chestFlagId));


#define GET_CUR_FORM_BTN_ITEM(btn) ((u8)((btn) == EQUIP_SLOT_B ? BUTTON_ITEM_EQUIP(CUR_FORM, btn) : BUTTON_ITEM_EQUIP(0, btn)))
#define GET_CUR_FORM_BTN_SLOT(btn) ((u8)((btn) == EQUIP_SLOT_B ? C_SLOT_EQUIP(CUR_FORM, btn) : C_SLOT_EQUIP(0, btn)))

#define C_BTN_ITEM(btn)                                 \
    ((gSaveContext.buttonStatus[(btn)] != BTN_DISABLED) \
         ? BUTTON_ITEM_EQUIP(0, (btn))                  \
         : ((gSaveContext.hudVisibility == HUD_VISIBILITY_A_B_C) ? BUTTON_ITEM_EQUIP(0, (btn)) : ITEM_NONE))

#define SET_CUR_FORM_BTN_ITEM(btn, item)             \
    if ((btn) == EQUIP_SLOT_B) {                     \
        BUTTON_ITEM_EQUIP(CUR_FORM, (btn)) = (item); \
    } else {                                         \
        BUTTON_ITEM_EQUIP(0, (btn)) = (item);        \
    }                                                \
    (void)0

#define SET_CUR_FORM_BTN_SLOT(btn, item)        \
    if ((btn) == EQUIP_SLOT_B) {                \
        C_SLOT_EQUIP(CUR_FORM, (btn)) = (item); \
    } else {                                    \
        C_SLOT_EQUIP(0, (btn)) = (item);        \
    }                                           \
    (void)0

// #region 2S2H [DPad]
#define BTN_DPAD (BTN_DRIGHT | BTN_DLEFT | BTN_DDOWN | BTN_DUP)
#define DPAD_TO_HELD_ITEM(btn) (btn + EQUIP_SLOT_MAX) 
#define HELD_ITEM_TO_DPAD(heldBtn) (heldBtn - EQUIP_SLOT_MAX)
#define IS_HELD_DPAD(heldBtn) ((heldBtn >= DPAD_TO_HELD_ITEM(EQUIP_SLOT_D_RIGHT)) && (heldBtn <= DPAD_TO_HELD_ITEM(EQUIP_SLOT_D_UP)))

#define BTN_DPAD_EQUIP (GameInteractor_Dpad(GI_DPAD_EQUIP, BTN_DPAD))

#define CHECK_BTN_DPAD(input)                                                                                   \
    (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0) &&                                                      \
     (CHECK_BTN_ALL(input, BTN_DRIGHT) || CHECK_BTN_ALL(input, BTN_DLEFT) || CHECK_BTN_ALL(input, BTN_DDOWN) || \
      CHECK_BTN_ALL(input, BTN_DUP)))

#define DPAD_BUTTON(btn) (btn) // Translates between equip slot enum and button, in case we change how enum works

#define DPAD_BUTTON_ITEM_EQUIP(form, btn) (gSaveContext.save.shipSaveInfo.dpadEquips.dpadItems[form][DPAD_BUTTON(btn)])
#define DPAD_CUR_FORM_EQUIP(btn) BUTTON_ITEM_EQUIP(CUR_FORM, DPAD_BUTTON(btn)) // Unused

#define DPAD_SLOT_EQUIP(form, btn) (gSaveContext.save.shipSaveInfo.dpadEquips.dpadSlots[form][DPAD_BUTTON(btn)])

#define DPAD_GET_CUR_FORM_BTN_ITEM(btn) (DPAD_BUTTON_ITEM_EQUIP(0, DPAD_BUTTON(btn)))
#define DPAD_GET_CUR_FORM_BTN_SLOT(btn) (DPAD_SLOT_EQUIP(0, DPAD_BUTTON(btn)))

#define DPAD_BTN_ITEM(btn)                                                         \
    ((gSaveContext.shipSaveContext.dpad.status[(DPAD_BUTTON(btn))] != BTN_DISABLED) \
         ? DPAD_BUTTON_ITEM_EQUIP(0, (DPAD_BUTTON(btn)))                           \
         : ((gSaveContext.hudVisibility == HUD_VISIBILITY_A_B_C) ? DPAD_BUTTON_ITEM_EQUIP(0, (DPAD_BUTTON(btn))) : ITEM_NONE))

#define DPAD_SET_CUR_FORM_BTN_ITEM(btn, item)                          \
        DPAD_BUTTON_ITEM_EQUIP(0, (DPAD_BUTTON(btn))) = (item);        \
        (void)0

#define DPAD_SET_CUR_FORM_BTN_SLOT(btn, item)                     \
        DPAD_SLOT_EQUIP(0, (DPAD_BUTTON(btn))) = (item);          \
        (void)0

// #endregion

#define STOLEN_ITEM_NONE (0)

#define STOLEN_ITEM_1 ((gSaveContext.save.saveInfo.stolenItems & 0xFF000000) >> 0x18)
#define STOLEN_ITEM_2 ((gSaveContext.save.saveInfo.stolenItems & 0x00FF0000) >> 0x10)

#define SET_STOLEN_ITEM_1(itemId) \
    (gSaveContext.save.saveInfo.stolenItems = (gSaveContext.save.saveInfo.stolenItems & ~0xFF000000) | ((itemId & 0xFF) << 0x18))
#define SET_STOLEN_ITEM_2(itemId) \
    (gSaveContext.save.saveInfo.stolenItems = (gSaveContext.save.saveInfo.stolenItems & ~0x00FF0000) | ((itemId & 0xFF) << 0x10))

#define HIGH_SCORE(type) (gSaveContext.save.saveInfo.highScores[(type)])
#define GET_HIGH_SCORE(type) ((void)0, gSaveContext.save.saveInfo.highScores[(type)])

#define HS_GET_BANK_RUPEES() (HIGH_SCORE(HS_BANK_RUPEES) & 0xFFFF)
#define HS_SET_BANK_RUPEES(rupees) (HIGH_SCORE(HS_BANK_RUPEES) = ((HIGH_SCORE(HS_BANK_RUPEES) & 0xFFFF0000) | (rupees)))

#define HS_GET_HIGH_SCORE_3_LOWER() (HIGH_SCORE(HS_BOAT_ARCHERY) & 0xFFFF)
#define HS_SET_HIGH_SCORE_3_LOWER(score) (HIGH_SCORE(HS_BOAT_ARCHERY) = ((HIGH_SCORE(HS_BOAT_ARCHERY) & 0xFFFF0000) | (score)))
#define HS_GET_BOAT_ARCHERY_HIGH_SCORE() ((HIGH_SCORE(HS_BOAT_ARCHERY) & 0xFFFF0000) >> 0x10)
#define HS_SET_BOAT_ARCHERY_HIGH_SCORE(score) (HIGH_SCORE(HS_BOAT_ARCHERY) = ((HIGH_SCORE(HS_BOAT_ARCHERY) & 0xFFFF) | ((u16)(score) << 0x10)))

#define HS_GET_HORSE_BACK_BALLOON_TIME() ((s32)HIGH_SCORE(HS_HORSE_BACK_BALLOON))
#define HS_SET_HORSE_BACK_BALLOON_TIME(time) (HIGH_SCORE(HS_HORSE_BACK_BALLOON) = (time))

#define HS_GET_LOTTERY_CODE_GUESS() (HIGH_SCORE(HS_LOTTERY_GUESS) & 0xFFFF)
#define HS_SET_LOTTERY_CODE_GUESS(guess) (HIGH_SCORE(HS_LOTTERY_GUESS) = ((HIGH_SCORE(HS_LOTTERY_GUESS) & 0xFFFF0000) | ((guess) & 0xFFFF)))

#define HS_GET_TOWN_SHOOTING_GALLERY_HIGH_SCORE() ((s32)(HIGH_SCORE(HS_SHOOTING_GALLERY) & 0xFFFF))
#define HS_SET_TOWN_SHOOTING_GALLERY_HIGH_SCORE(score) (HIGH_SCORE(HS_SHOOTING_GALLERY) = (HIGH_SCORE(HS_SHOOTING_GALLERY) & 0xFFFF0000) | ((u16)(score)))
#define HS_GET_SWAMP_SHOOTING_GALLERY_HIGH_SCORE() ((s32)((HIGH_SCORE(HS_SHOOTING_GALLERY) & 0xFFFF0000) >> 0x10))
#define HS_SET_SWAMP_SHOOTING_GALLERY_HIGH_SCORE(score) (HIGH_SCORE(HS_SHOOTING_GALLERY) = (HIGH_SCORE(HS_SHOOTING_GALLERY) & 0xFFFF) | ((u16)(score) << 0x10))

// Note that the time that each alien spawns is stored as an offset from 2:30 AM. To get the actual time that they spawn, you need to add
// `CLOCK_TIME(2, 30)` to the value retrieved using this macro. These spawn time macros are only intended to be used with an `index` value
// ranging from 0 to 7; using an `index` of 8 or 9 will overwrite data used to track the alien kill count. You can technically supply an
// `index` value between 10 and 13 and have it work, however, since these indices are unused in the final game.
#define ALIEN_GET_SPAWN_TIME_OFFSET(index) \
    (((index % 2) == 0) ? ((gSaveContext.save.saveInfo.alienInfo[index >> 1] & 0xFFFF)) \
                        : ((gSaveContext.save.saveInfo.alienInfo[index >> 1] & ~0xFFFF) >> 0x10))

#define ALIEN_SET_SPAWN_TIME_OFFSET(index, spawnTime) \
    ((index % 2) == 0) ? (gSaveContext.save.saveInfo.alienInfo[index >> 1] = (gSaveContext.save.saveInfo.alienInfo[index >> 1] & ~0xFFFF) | (spawnTime & 0xFFFF)) \
                       : (gSaveContext.save.saveInfo.alienInfo[index >> 1] = (gSaveContext.save.saveInfo.alienInfo[index >> 1] & 0xFFFF) | ((spawnTime & 0xFFFF) << 0x10))

#define ALIEN_GET_KILL_COUNT() \
    gSaveContext.save.saveInfo.alienInfo[4] & 0xFF

#define ALIEN_SET_KILL_COUNT(count) \
    gSaveContext.save.saveInfo.alienInfo[4] = (gSaveContext.save.saveInfo.alienInfo[4] & ~0xFF) | (count & 0xFF)

/**
 * gSaveContext.save.saveInfo.weekEventReg
 */

#define PACK_WEEKEVENTREG_FLAG(index, mask) (((index) << 8) | (mask))

#define WEEKEVENTREG_00_01 PACK_WEEKEVENTREG_FLAG(0, 0x01)

/* Entrance cutscenes watched */

#define WEEKEVENTREG_ENTERED_TERMINA_FIELD               PACK_WEEKEVENTREG_FLAG(0, 0x02)
#define WEEKEVENTREG_ENTERED_IKANA_GRAVEYARD             PACK_WEEKEVENTREG_FLAG(0, 0x04)
#define WEEKEVENTREG_ENTERED_ROMANI_RANCH                PACK_WEEKEVENTREG_FLAG(0, 0x08)
#define WEEKEVENTREG_ENTERED_GORMAN_TRACK                PACK_WEEKEVENTREG_FLAG(0, 0x10)
#define WEEKEVENTREG_ENTERED_MOUNTAIN_VILLAGE_WINTER     PACK_WEEKEVENTREG_FLAG(0, 0x20)
#define WEEKEVENTREG_ENTERED_GORON_SHRINE                PACK_WEEKEVENTREG_FLAG(0, 0x40)
#define WEEKEVENTREG_ENTERED_SNOWHEAD                    PACK_WEEKEVENTREG_FLAG(0, 0x80)
#define WEEKEVENTREG_ENTERED_SOUTHERN_SWAMP_POISONED     PACK_WEEKEVENTREG_FLAG(1, 0x01)
#define WEEKEVENTREG_ENTERED_WOODFALL                    PACK_WEEKEVENTREG_FLAG(1, 0x02)
#define WEEKEVENTREG_ENTERED_DEKU_PALACE                 PACK_WEEKEVENTREG_FLAG(1, 0x04)
#define WEEKEVENTREG_ENTERED_GREAT_BAY_COAST             PACK_WEEKEVENTREG_FLAG(1, 0x08)
#define WEEKEVENTREG_ENTERED_PIRATES_FORTRESS            PACK_WEEKEVENTREG_FLAG(1, 0x10)
#define WEEKEVENTREG_ENTERED_ZORA_HALL                   PACK_WEEKEVENTREG_FLAG(1, 0x20)
#define WEEKEVENTREG_ENTERED_WATERFALL_RAPIDS            PACK_WEEKEVENTREG_FLAG(1, 0x40)
#define WEEKEVENTREG_ENTERED_IKANA_CANYON                PACK_WEEKEVENTREG_FLAG(1, 0x80)
// Attached to the scene but unused. Entrance cutscene is instead triggered by `ACTOR_OBJ_DEMO`
#define WEEKEVENTREG_ENTERED_IKANA_CASTLE                PACK_WEEKEVENTREG_FLAG(2, 0x01)
#define WEEKEVENTREG_ENTERED_STONE_TOWER                 PACK_WEEKEVENTREG_FLAG(2, 0x02)
#define WEEKEVENTREG_ENTERED_STONE_TOWER_INVERTED        PACK_WEEKEVENTREG_FLAG(2, 0x04)
#define WEEKEVENTREG_ENTERED_EAST_CLOCK_TOWN             PACK_WEEKEVENTREG_FLAG(2, 0x08)
#define WEEKEVENTREG_ENTERED_WEST_CLOCK_TOWN             PACK_WEEKEVENTREG_FLAG(2, 0x10)
#define WEEKEVENTREG_ENTERED_NORTH_CLOCK_TOWN            PACK_WEEKEVENTREG_FLAG(2, 0x20)
#define WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE             PACK_WEEKEVENTREG_FLAG(2, 0x40)
#define WEEKEVENTREG_ENTERED_SNOWHEAD_TEMPLE             PACK_WEEKEVENTREG_FLAG(2, 0x80)
// Attached to the scene but unused. Entrance cutscene is instead triggered by `ACTOR_OBJ_DEMO`
#define WEEKEVENTREG_ENTERED_PIRATES_FORTRESS_EXTERIOR   PACK_WEEKEVENTREG_FLAG(3, 0x01)
#define WEEKEVENTREG_ENTERED_STONE_TOWER_TEMPLE          PACK_WEEKEVENTREG_FLAG(3, 0x02)
#define WEEKEVENTREG_ENTERED_STONE_TOWER_TEMPLE_INVERTED PACK_WEEKEVENTREG_FLAG(3, 0x04)
// Unused as no cutscene is attached to this script
#define WEEKEVENTREG_ENTERED_THE_MOON                    PACK_WEEKEVENTREG_FLAG(3, 0x08)
#define WEEKEVENTREG_ENTERED_MOON_DEKU_TRIAL             PACK_WEEKEVENTREG_FLAG(3, 0x10)
#define WEEKEVENTREG_ENTERED_MOON_GORON_TRIAL            PACK_WEEKEVENTREG_FLAG(3, 0x20)
#define WEEKEVENTREG_ENTERED_MOON_ZORA_TRIAL             PACK_WEEKEVENTREG_FLAG(3, 0x40)

#define WEEKEVENTREG_03_80 PACK_WEEKEVENTREG_FLAG(3, 0x80)
#define WEEKEVENTREG_04_01 PACK_WEEKEVENTREG_FLAG(4, 0x01)
#define WEEKEVENTREG_04_02 PACK_WEEKEVENTREG_FLAG(4, 0x02)
#define WEEKEVENTREG_04_04 PACK_WEEKEVENTREG_FLAG(4, 0x04)
#define WEEKEVENTREG_04_08 PACK_WEEKEVENTREG_FLAG(4, 0x08)
#define WEEKEVENTREG_04_10 PACK_WEEKEVENTREG_FLAG(4, 0x10)
#define WEEKEVENTREG_04_20 PACK_WEEKEVENTREG_FLAG(4, 0x20)
#define WEEKEVENTREG_04_40 PACK_WEEKEVENTREG_FLAG(4, 0x40)
#define WEEKEVENTREG_04_80 PACK_WEEKEVENTREG_FLAG(4, 0x80)
#define WEEKEVENTREG_05_01 PACK_WEEKEVENTREG_FLAG(5, 0x01)
#define WEEKEVENTREG_05_02 PACK_WEEKEVENTREG_FLAG(5, 0x02)
#define WEEKEVENTREG_05_04 PACK_WEEKEVENTREG_FLAG(5, 0x04)
#define WEEKEVENTREG_05_08 PACK_WEEKEVENTREG_FLAG(5, 0x08)
#define WEEKEVENTREG_05_10 PACK_WEEKEVENTREG_FLAG(5, 0x10)
#define WEEKEVENTREG_05_20 PACK_WEEKEVENTREG_FLAG(5, 0x20)
#define WEEKEVENTREG_05_40 PACK_WEEKEVENTREG_FLAG(5, 0x40)
#define WEEKEVENTREG_05_80 PACK_WEEKEVENTREG_FLAG(5, 0x80)
#define WEEKEVENTREG_06_01 PACK_WEEKEVENTREG_FLAG(6, 0x01)
#define WEEKEVENTREG_06_02 PACK_WEEKEVENTREG_FLAG(6, 0x02)
#define WEEKEVENTREG_06_04 PACK_WEEKEVENTREG_FLAG(6, 0x04)
#define WEEKEVENTREG_06_08 PACK_WEEKEVENTREG_FLAG(6, 0x08)
#define WEEKEVENTREG_06_10 PACK_WEEKEVENTREG_FLAG(6, 0x10)
#define WEEKEVENTREG_06_20 PACK_WEEKEVENTREG_FLAG(6, 0x20)
#define WEEKEVENTREG_06_40 PACK_WEEKEVENTREG_FLAG(6, 0x40)
#define WEEKEVENTREG_06_80 PACK_WEEKEVENTREG_FLAG(6, 0x80)
#define WEEKEVENTREG_07_01 PACK_WEEKEVENTREG_FLAG(7, 0x01)
#define WEEKEVENTREG_07_02 PACK_WEEKEVENTREG_FLAG(7, 0x02)
#define WEEKEVENTREG_07_04 PACK_WEEKEVENTREG_FLAG(7, 0x04)
#define WEEKEVENTREG_07_08 PACK_WEEKEVENTREG_FLAG(7, 0x08)
#define WEEKEVENTREG_07_10 PACK_WEEKEVENTREG_FLAG(7, 0x10)
#define WEEKEVENTREG_07_20 PACK_WEEKEVENTREG_FLAG(7, 0x20)
#define WEEKEVENTREG_07_40 PACK_WEEKEVENTREG_FLAG(7, 0x40)

// Entrance cutscene watched to the prison where the deku princess is kept. Also set in door_warp1.c
#define WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE_PRISON PACK_WEEKEVENTREG_FLAG(7, 0x80)

// Related to Honey & Darling minigame
#define WEEKEVENTREG_08_01 PACK_WEEKEVENTREG_FLAG(8, 0x01)
#define WEEKEVENTREG_08_02 PACK_WEEKEVENTREG_FLAG(8, 0x02)
#define WEEKEVENTREG_08_04 PACK_WEEKEVENTREG_FLAG(8, 0x04)
#define WEEKEVENTREG_08_08 PACK_WEEKEVENTREG_FLAG(8, 0x08)
#define WEEKEVENTREG_08_10 PACK_WEEKEVENTREG_FLAG(8, 0x10)
#define WEEKEVENTREG_RECEIVED_DOGGY_RACETRACK_HEART_PIECE PACK_WEEKEVENTREG_FLAG(8, 0x20)

// This is set under three circumstances:
// 1. The player watches the cutscene of the Clock Tower opening.
// 2. The player sees the Clock Tower opening outside of a cutscene. After the first cycle, this
//    can be seen in Termina Field or in North, East, or West Clock Town.
// 3. The player enters Termina Field or North, South, East, or West Clock Town any time after
//    midnight on the Final Day.
// Thus, it is possible for the player to be in the final six hours and still have this unset; all
// the player needs to do is avoid certain areas.
#define WEEKEVENTREG_CLOCK_TOWER_OPENED PACK_WEEKEVENTREG_FLAG(8, 0x40)

#define WEEKEVENTREG_08_80 PACK_WEEKEVENTREG_FLAG(8, 0x80)

// This 5 flags are managed in a special way by EnElfgrp
#define WEEKEVENTREG_09_01 PACK_WEEKEVENTREG_FLAG(9, 0x01)
#define WEEKEVENTREG_09_02 PACK_WEEKEVENTREG_FLAG(9, 0x02)
#define WEEKEVENTREG_09_04 PACK_WEEKEVENTREG_FLAG(9, 0x04)
#define WEEKEVENTREG_09_08 PACK_WEEKEVENTREG_FLAG(9, 0x08)
#define WEEKEVENTREG_09_10 PACK_WEEKEVENTREG_FLAG(9, 0x10)

#define WEEKEVENTREG_09_20 PACK_WEEKEVENTREG_FLAG(9, 0x20)
#define WEEKEVENTREG_09_40 PACK_WEEKEVENTREG_FLAG(9, 0x40)
#define WEEKEVENTREG_09_80 PACK_WEEKEVENTREG_FLAG(9, 0x80)
#define WEEKEVENTREG_TALKED_TINGLE PACK_WEEKEVENTREG_FLAG(10, 0x01)
#define WEEKEVENTREG_10_02 PACK_WEEKEVENTREG_FLAG(10, 0x02)
#define WEEKEVENTREG_10_04 PACK_WEEKEVENTREG_FLAG(10, 0x04)
#define WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE PACK_WEEKEVENTREG_FLAG(10, 0x08)
#define WEEKEVENTREG_10_10 PACK_WEEKEVENTREG_FLAG(10, 0x10)
#define WEEKEVENTREG_10_20 PACK_WEEKEVENTREG_FLAG(10, 0x20)
#define WEEKEVENTREG_10_40 PACK_WEEKEVENTREG_FLAG(10, 0x40)
#define WEEKEVENTREG_10_80 PACK_WEEKEVENTREG_FLAG(10, 0x80)
#define WEEKEVENTREG_11_01 PACK_WEEKEVENTREG_FLAG(11, 0x01)
#define WEEKEVENTREG_11_02 PACK_WEEKEVENTREG_FLAG(11, 0x02)
#define WEEKEVENTREG_11_04 PACK_WEEKEVENTREG_FLAG(11, 0x04)
#define WEEKEVENTREG_11_08 PACK_WEEKEVENTREG_FLAG(11, 0x08)
#define WEEKEVENTREG_11_10 PACK_WEEKEVENTREG_FLAG(11, 0x10)
#define WEEKEVENTREG_11_20 PACK_WEEKEVENTREG_FLAG(11, 0x20)
#define WEEKEVENTREG_11_40 PACK_WEEKEVENTREG_FLAG(11, 0x40)
#define WEEKEVENTREG_11_80 PACK_WEEKEVENTREG_FLAG(11, 0x80)

// woodfall temple wood flower opened
#define WEEKEVENTREG_12_01 PACK_WEEKEVENTREG_FLAG(12, 0x01)

#define WEEKEVENTREG_12_02 PACK_WEEKEVENTREG_FLAG(12, 0x02)
#define WEEKEVENTREG_12_04 PACK_WEEKEVENTREG_FLAG(12, 0x04)
#define WEEKEVENTREG_SAVED_KOUME PACK_WEEKEVENTREG_FLAG(12, 0x08)
#define WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE PACK_WEEKEVENTREG_FLAG(12, 0x10)
#define WEEKEVENTREG_12_20 PACK_WEEKEVENTREG_FLAG(12, 0x20)
#define WEEKEVENTREG_12_40 PACK_WEEKEVENTREG_FLAG(12, 0x40)
#define WEEKEVENTREG_12_80 PACK_WEEKEVENTREG_FLAG(12, 0x80)
#define WEEKEVENTREG_13_01 PACK_WEEKEVENTREG_FLAG(13, 0x01)
#define WEEKEVENTREG_13_02 PACK_WEEKEVENTREG_FLAG(13, 0x02)
#define WEEKEVENTREG_13_04 PACK_WEEKEVENTREG_FLAG(13, 0x04)
#define WEEKEVENTREG_13_08 PACK_WEEKEVENTREG_FLAG(13, 0x08)
#define WEEKEVENTREG_13_10 PACK_WEEKEVENTREG_FLAG(13, 0x10)

// This flag marks that the player has finished the Oceanside Spider House and has exited.
// Used to identify if EnSth should be moved deeper into the house.
// This does NOT flag:
//   A) that the player has completed the house (Inventory_GetSkullTokenCount(play->sceneId))
//   B) that the player has collected a reward (WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_COLLECTED_REWARD)
//   C) that the player has collected the wallet (WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE)
#define WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_BUYER_MOVED_IN PACK_WEEKEVENTREG_FLAG(13, 0x20)

#define WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE PACK_WEEKEVENTREG_FLAG(13, 0x40)
// You only get a wallet if completed on Day 1: repeat gets silver rupee, other days get less
#define WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_COLLECTED_REWARD PACK_WEEKEVENTREG_FLAG(13, 0x80)

// PlayedMilkMinigame
// Attempted Cremia Cart Ride
#define WEEKEVENTREG_14_01 PACK_WEEKEVENTREG_FLAG(14, 0x01)

#define WEEKEVENTREG_14_02 PACK_WEEKEVENTREG_FLAG(14, 0x02)
#define WEEKEVENTREG_14_04 PACK_WEEKEVENTREG_FLAG(14, 0x04)
#define WEEKEVENTREG_DRANK_CHATEAU_ROMANI PACK_WEEKEVENTREG_FLAG(14, 0x08)
#define WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1 PACK_WEEKEVENTREG_FLAG(14, 0x10)
#define WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2 PACK_WEEKEVENTREG_FLAG(14, 0x20)
#define WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_3 PACK_WEEKEVENTREG_FLAG(14, 0x40)
#define WEEKEVENTREG_RECEIVED_DEKU_PLAYGROUND_HEART_PIECE PACK_WEEKEVENTREG_FLAG(14, 0x80)
#define WEEKEVENTREG_15_01 PACK_WEEKEVENTREG_FLAG(15, 0x01)
#define WEEKEVENTREG_15_02 PACK_WEEKEVENTREG_FLAG(15, 0x02)
#define WEEKEVENTREG_15_04 PACK_WEEKEVENTREG_FLAG(15, 0x04)
#define WEEKEVENTREG_15_08 PACK_WEEKEVENTREG_FLAG(15, 0x08)
#define WEEKEVENTREG_15_10 PACK_WEEKEVENTREG_FLAG(15, 0x10)
#define WEEKEVENTREG_15_20 PACK_WEEKEVENTREG_FLAG(15, 0x20)
#define WEEKEVENTREG_15_40 PACK_WEEKEVENTREG_FLAG(15, 0x40)
#define WEEKEVENTREG_15_80 PACK_WEEKEVENTREG_FLAG(15, 0x80)
#define WEEKEVENTREG_16_01 PACK_WEEKEVENTREG_FLAG(16, 0x01)
#define WEEKEVENTREG_16_02 PACK_WEEKEVENTREG_FLAG(16, 0x02)
#define WEEKEVENTREG_16_04 PACK_WEEKEVENTREG_FLAG(16, 0x04)
#define WEEKEVENTREG_16_08 PACK_WEEKEVENTREG_FLAG(16, 0x08)
#define WEEKEVENTREG_TALKED_KOUME_INJURED PACK_WEEKEVENTREG_FLAG(16, 0x10)
#define WEEKEVENTREG_16_20 PACK_WEEKEVENTREG_FLAG(16, 0x20)
#define WEEKEVENTREG_16_40 PACK_WEEKEVENTREG_FLAG(16, 0x40)
#define WEEKEVENTREG_16_80 PACK_WEEKEVENTREG_FLAG(16, 0x80)
#define WEEKEVENTREG_TALKED_KOUME_KIOSK_EMPTY PACK_WEEKEVENTREG_FLAG(17, 0x01)
#define WEEKEVENTREG_17_02 PACK_WEEKEVENTREG_FLAG(17, 0x02)
#define WEEKEVENTREG_17_04 PACK_WEEKEVENTREG_FLAG(17, 0x04)
#define WEEKEVENTREG_17_08 PACK_WEEKEVENTREG_FLAG(17, 0x08)
#define WEEKEVENTREG_17_10 PACK_WEEKEVENTREG_FLAG(17, 0x10)
#define WEEKEVENTREG_17_20 PACK_WEEKEVENTREG_FLAG(17, 0x20)
#define WEEKEVENTREG_17_40 PACK_WEEKEVENTREG_FLAG(17, 0x40)
#define WEEKEVENTREG_RECEIVED_LAND_TITLE_DEED PACK_WEEKEVENTREG_FLAG(17, 0x80)
#define WEEKEVENTREG_18_01 PACK_WEEKEVENTREG_FLAG(18, 0x01)
#define WEEKEVENTREG_18_02 PACK_WEEKEVENTREG_FLAG(18, 0x02)
#define WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_GORON PACK_WEEKEVENTREG_FLAG(18, 0x04)
#define WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_ZORA PACK_WEEKEVENTREG_FLAG(18, 0x08)
#define WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_DEKU PACK_WEEKEVENTREG_FLAG(18, 0x10)
#define WEEKEVENTREG_18_20 PACK_WEEKEVENTREG_FLAG(18, 0x20)
#define WEEKEVENTREG_18_40 PACK_WEEKEVENTREG_FLAG(18, 0x40)

// Player has Powder Keg purchasing privileges.
#define WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES PACK_WEEKEVENTREG_FLAG(18, 0x80)
#define WEEKEVENTREG_19_01 PACK_WEEKEVENTREG_FLAG(19, 0x01)
#define WEEKEVENTREG_19_02 PACK_WEEKEVENTREG_FLAG(19, 0x02)
#define WEEKEVENTREG_19_04 PACK_WEEKEVENTREG_FLAG(19, 0x04)
#define WEEKEVENTREG_19_08 PACK_WEEKEVENTREG_FLAG(19, 0x08)
#define WEEKEVENTREG_19_10 PACK_WEEKEVENTREG_FLAG(19, 0x10)
#define WEEKEVENTREG_19_20 PACK_WEEKEVENTREG_FLAG(19, 0x20)
#define WEEKEVENTREG_19_40 PACK_WEEKEVENTREG_FLAG(19, 0x40)
#define WEEKEVENTREG_19_80 PACK_WEEKEVENTREG_FLAG(19, 0x80)
#define WEEKEVENTREG_20_01 PACK_WEEKEVENTREG_FLAG(20, 0x01)

// woodfall temple purification cutscene watched
#define WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE PACK_WEEKEVENTREG_FLAG(20, 0x02)

#define WEEKEVENTREG_20_04 PACK_WEEKEVENTREG_FLAG(20, 0x04)
#define WEEKEVENTREG_20_08 PACK_WEEKEVENTREG_FLAG(20, 0x08)
#define WEEKEVENTREG_20_10 PACK_WEEKEVENTREG_FLAG(20, 0x10)
#define WEEKEVENTREG_20_20 PACK_WEEKEVENTREG_FLAG(20, 0x20)
#define WEEKEVENTREG_20_40 PACK_WEEKEVENTREG_FLAG(20, 0x40)
#define WEEKEVENTREG_20_80 PACK_WEEKEVENTREG_FLAG(20, 0x80)
#define WEEKEVENTREG_21_01 PACK_WEEKEVENTREG_FLAG(21, 0x01)
#define WEEKEVENTREG_21_02 PACK_WEEKEVENTREG_FLAG(21, 0x02)

// Player has spoken to Goron Graveyard's gravemaker while in Goron form.
#define WEEKEVENTREG_TALKED_GORON_GRAVEMAKER_AS_GORON PACK_WEEKEVENTREG_FLAG(21, 0x04)
// Player has spoken to formerly frozen Goron outside Goron Graveyard.
#define WEEKEVENTREG_TALKED_THAWED_GRAVEYARD_GORON PACK_WEEKEVENTREG_FLAG(21, 0x08)

#define WEEKEVENTREG_21_10 PACK_WEEKEVENTREG_FLAG(21, 0x10)

// Player talked with Romani before the alien invasion and agreed to help her
#define WEEKEVENTREG_PROMISED_TO_HELP_WITH_ALIENS PACK_WEEKEVENTREG_FLAG(21, 0x20)

#define WEEKEVENTREG_21_40 PACK_WEEKEVENTREG_FLAG(21, 0x40)
#define WEEKEVENTREG_21_80 PACK_WEEKEVENTREG_FLAG(21, 0x80)

// Player successfully defended Romani Ranch from the alien invasion on Night 1
#define WEEKEVENTREG_DEFENDED_AGAINST_ALIENS PACK_WEEKEVENTREG_FLAG(22, 0x01)

// Player received the Bottle of Milk from Romani for defending the ranch from the aliens for the first time
#define WEEKEVENTREG_RECEIVED_ALIENS_BOTTLE PACK_WEEKEVENTREG_FLAG(22, 0x02)

// Goron Elder's son has been calmed from Goron's Lullaby.
#define WEEKEVENTREG_CALMED_GORON_ELDERS_SON PACK_WEEKEVENTREG_FLAG(22, 0x04)

#define WEEKEVENTREG_22_08 PACK_WEEKEVENTREG_FLAG(22, 0x08)
#define WEEKEVENTREG_22_10 PACK_WEEKEVENTREG_FLAG(22, 0x10)
#define WEEKEVENTREG_22_20 PACK_WEEKEVENTREG_FLAG(22, 0x20)
#define WEEKEVENTREG_22_40 PACK_WEEKEVENTREG_FLAG(22, 0x40)
#define WEEKEVENTREG_RECEIVED_HONEY_AND_DARLING_HEART_PIECE PACK_WEEKEVENTREG_FLAG(22, 0x80)
#define WEEKEVENTREG_23_01 PACK_WEEKEVENTREG_FLAG(23, 0x01)
#define WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK PACK_WEEKEVENTREG_FLAG(23, 0x02)
#define WEEKEVENTREG_23_04 PACK_WEEKEVENTREG_FLAG(23, 0x04)
#define WEEKEVENTREG_23_08 PACK_WEEKEVENTREG_FLAG(23, 0x08)
#define WEEKEVENTREG_23_10 PACK_WEEKEVENTREG_FLAG(23, 0x10)
#define WEEKEVENTREG_23_20 PACK_WEEKEVENTREG_FLAG(23, 0x20)
#define WEEKEVENTREG_23_40 PACK_WEEKEVENTREG_FLAG(23, 0x40)
#define WEEKEVENTREG_RECEIVED_BEAVER_RACE_BOTTLE PACK_WEEKEVENTREG_FLAG(23, 0x80)
#define WEEKEVENTREG_24_01 PACK_WEEKEVENTREG_FLAG(24, 0x01)
#define WEEKEVENTREG_24_02 PACK_WEEKEVENTREG_FLAG(24, 0x02)
#define WEEKEVENTREG_24_04 PACK_WEEKEVENTREG_FLAG(24, 0x04)
#define WEEKEVENTREG_24_08 PACK_WEEKEVENTREG_FLAG(24, 0x08)

// The player has already talked as a Goron at least once to Goron elder
#define WEEKEVENTREG_24_10 PACK_WEEKEVENTREG_FLAG(24, 0x10)

// The player has already talked as a non-Goron at least once
#define WEEKEVENTREG_24_20 PACK_WEEKEVENTREG_FLAG(24, 0x20)

#define WEEKEVENTREG_24_40 PACK_WEEKEVENTREG_FLAG(24, 0x40)

// The player has talked to the Goron Child at least once
#define WEEKEVENTREG_24_80 PACK_WEEKEVENTREG_FLAG(24, 0x80)

#define WEEKEVENTREG_RECEIVED_BEAVER_BROS_HEART_PIECE PACK_WEEKEVENTREG_FLAG(25, 0x01)
#define WEEKEVENTREG_25_02 PACK_WEEKEVENTREG_FLAG(25, 0x02)
#define WEEKEVENTREG_25_04 PACK_WEEKEVENTREG_FLAG(25, 0x04)

// breman mask was already used
#define WEEKEVENTREG_25_08 PACK_WEEKEVENTREG_FLAG(25, 0x08)

#define WEEKEVENTREG_25_10 PACK_WEEKEVENTREG_FLAG(25, 0x10)
#define WEEKEVENTREG_25_20 PACK_WEEKEVENTREG_FLAG(25, 0x20)
#define WEEKEVENTREG_25_40 PACK_WEEKEVENTREG_FLAG(25, 0x40)
#define WEEKEVENTREG_25_80 PACK_WEEKEVENTREG_FLAG(25, 0x80)
#define WEEKEVENTREG_26_01 PACK_WEEKEVENTREG_FLAG(26, 0x01)
#define WEEKEVENTREG_26_02 PACK_WEEKEVENTREG_FLAG(26, 0x02)
#define WEEKEVENTREG_26_04 PACK_WEEKEVENTREG_FLAG(26, 0x04)
#define WEEKEVENTREG_26_08 PACK_WEEKEVENTREG_FLAG(26, 0x08)
#define WEEKEVENTREG_26_10 PACK_WEEKEVENTREG_FLAG(26, 0x10)
#define WEEKEVENTREG_26_20 PACK_WEEKEVENTREG_FLAG(26, 0x20)
#define WEEKEVENTREG_26_40 PACK_WEEKEVENTREG_FLAG(26, 0x40)
#define WEEKEVENTREG_26_80 PACK_WEEKEVENTREG_FLAG(26, 0x80)
#define WEEKEVENTREG_27_01 PACK_WEEKEVENTREG_FLAG(27, 0x01)
#define WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_UPPER_CLOCKTOWN PACK_WEEKEVENTREG_FLAG(27, 0x02)
#define WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_NORTH_CLOCKTOWN PACK_WEEKEVENTREG_FLAG(27, 0x04)
#define WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_UPPER_CLOCKTOWN PACK_WEEKEVENTREG_FLAG(27, 0x08)
#define WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_LOWER_CLOCKTOWN PACK_WEEKEVENTREG_FLAG(27, 0x10)
#define WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_LOWER_CLOCKTOWN PACK_WEEKEVENTREG_FLAG(27, 0x20)
#define WEEKEVENTREG_27_40 PACK_WEEKEVENTREG_FLAG(27, 0x40)
#define WEEKEVENTREG_27_80 PACK_WEEKEVENTREG_FLAG(27, 0x80)
#define WEEKEVENTREG_28_01 PACK_WEEKEVENTREG_FLAG(28, 0x01)
#define WEEKEVENTREG_28_02 PACK_WEEKEVENTREG_FLAG(28, 0x02)
#define WEEKEVENTREG_28_04 PACK_WEEKEVENTREG_FLAG(28, 0x04)
#define WEEKEVENTREG_28_08 PACK_WEEKEVENTREG_FLAG(28, 0x08)
#define WEEKEVENTREG_28_10 PACK_WEEKEVENTREG_FLAG(28, 0x10)
#define WEEKEVENTREG_28_20 PACK_WEEKEVENTREG_FLAG(28, 0x20)
#define WEEKEVENTREG_28_40 PACK_WEEKEVENTREG_FLAG(28, 0x40)
#define WEEKEVENTREG_28_80 PACK_WEEKEVENTREG_FLAG(28, 0x80)
#define WEEKEVENTREG_29_01 PACK_WEEKEVENTREG_FLAG(29, 0x01)
#define WEEKEVENTREG_29_02 PACK_WEEKEVENTREG_FLAG(29, 0x02)
#define WEEKEVENTREG_29_04 PACK_WEEKEVENTREG_FLAG(29, 0x04)
#define WEEKEVENTREG_29_08 PACK_WEEKEVENTREG_FLAG(29, 0x08)
#define WEEKEVENTREG_29_10 PACK_WEEKEVENTREG_FLAG(29, 0x10)
#define WEEKEVENTREG_29_20 PACK_WEEKEVENTREG_FLAG(29, 0x20)
#define WEEKEVENTREG_29_40 PACK_WEEKEVENTREG_FLAG(29, 0x40)
#define WEEKEVENTREG_29_80 PACK_WEEKEVENTREG_FLAG(29, 0x80)
#define WEEKEVENTREG_30_01 PACK_WEEKEVENTREG_FLAG(30, 0x01)
#define WEEKEVENTREG_30_02 PACK_WEEKEVENTREG_FLAG(30, 0x02)
#define WEEKEVENTREG_30_04 PACK_WEEKEVENTREG_FLAG(30, 0x04)
#define WEEKEVENTREG_30_08 PACK_WEEKEVENTREG_FLAG(30, 0x08)
#define WEEKEVENTREG_30_10 PACK_WEEKEVENTREG_FLAG(30, 0x10)
#define WEEKEVENTREG_30_20 PACK_WEEKEVENTREG_FLAG(30, 0x20)
#define WEEKEVENTREG_30_40 PACK_WEEKEVENTREG_FLAG(30, 0x40)
#define WEEKEVENTREG_30_80 PACK_WEEKEVENTREG_FLAG(30, 0x80)
#define WEEKEVENTREG_31_01 PACK_WEEKEVENTREG_FLAG(31, 0x01)
#define WEEKEVENTREG_31_02 PACK_WEEKEVENTREG_FLAG(31, 0x02)

// Unconfirmed: "Tatl's Second Cycle Text?"
#define WEEKEVENTREG_31_04 PACK_WEEKEVENTREG_FLAG(31, 0x04)

#define WEEKEVENTREG_31_08 PACK_WEEKEVENTREG_FLAG(31, 0x08)
#define WEEKEVENTREG_31_10 PACK_WEEKEVENTREG_FLAG(31, 0x10)
#define WEEKEVENTREG_31_20 PACK_WEEKEVENTREG_FLAG(31, 0x20)

//! @note: entrance cutscenes defined in `CutsceneEntry` can not use
//! any of the below weekEventFlags due to bitpacking

// Cremia asked the player to accompany her to town
#define WEEKEVENTREG_31_40 PACK_WEEKEVENTREG_FLAG(31, 0x40)
// Player is playing the Milk Run
#define WEEKEVENTREG_31_80 PACK_WEEKEVENTREG_FLAG(31, 0x80)

#define WEEKEVENTREG_RECEIVED_SEAHORSE_HEART_PIECE PACK_WEEKEVENTREG_FLAG(32, 0x01)
#define WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE PACK_WEEKEVENTREG_FLAG(32, 0x02)
#define WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE PACK_WEEKEVENTREG_FLAG(32, 0x04)
#define WEEKEVENTREG_32_08 PACK_WEEKEVENTREG_FLAG(32, 0x08)
#define WEEKEVENTREG_32_10 PACK_WEEKEVENTREG_FLAG(32, 0x10)
#define WEEKEVENTREG_32_20 PACK_WEEKEVENTREG_FLAG(32, 0x20)

// Woodfall Temple Frog Returned
#define WEEKEVENTREG_32_40 PACK_WEEKEVENTREG_FLAG(32, 0x40)
// Great Bay Temple Frog Returned
#define WEEKEVENTREG_32_80 PACK_WEEKEVENTREG_FLAG(32, 0x80)
// Southern Swamp Frog Returned
#define WEEKEVENTREG_33_01 PACK_WEEKEVENTREG_FLAG(33, 0x01)
// Laundry Pool Frog Returned
#define WEEKEVENTREG_33_02 PACK_WEEKEVENTREG_FLAG(33, 0x02)

#define WEEKEVENTREG_BOUGHT_CURIOSITY_SHOP_SPECIAL_ITEM PACK_WEEKEVENTREG_FLAG(33, 0x04)
#define WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG PACK_WEEKEVENTREG_FLAG(33, 0x08)
#define WEEKEVENTREG_33_10 PACK_WEEKEVENTREG_FLAG(33, 0x10)
#define WEEKEVENTREG_33_20 PACK_WEEKEVENTREG_FLAG(33, 0x20)
#define WEEKEVENTREG_33_40 PACK_WEEKEVENTREG_FLAG(33, 0x40)

// Mountain village is unfrozen
#define WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE PACK_WEEKEVENTREG_FLAG(33, 0x80)
// Spoken to FROG_YELLOW
#define WEEKEVENTREG_34_01 PACK_WEEKEVENTREG_FLAG(34, 0x01)

#define WEEKEVENTREG_34_02 PACK_WEEKEVENTREG_FLAG(34, 0x02)
#define WEEKEVENTREG_34_04 PACK_WEEKEVENTREG_FLAG(34, 0x04)
#define WEEKEVENTREG_TALKED_SWAMP_SPIDER_HOUSE_MAN PACK_WEEKEVENTREG_FLAG(34, 0x08)
#define WEEKEVENTREG_34_10 PACK_WEEKEVENTREG_FLAG(34, 0x10)
#define WEEKEVENTREG_34_20 PACK_WEEKEVENTREG_FLAG(34, 0x20)
#define WEEKEVENTREG_RECEIVED_MASK_OF_TRUTH PACK_WEEKEVENTREG_FLAG(34, 0x40)

// Cremia did Milk Run alone. Player didn't interact or didn't accept the ride
#define WEEKEVENTREG_34_80 PACK_WEEKEVENTREG_FLAG(34, 0x80)

// Bought each possible map from Tingle
#define WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN PACK_WEEKEVENTREG_FLAG(35, 0x01)
#define WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL PACK_WEEKEVENTREG_FLAG(35, 0x02)
#define WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD PACK_WEEKEVENTREG_FLAG(35, 0x04)
#define WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH PACK_WEEKEVENTREG_FLAG(35, 0x08)
#define WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY PACK_WEEKEVENTREG_FLAG(35, 0x10)
#define WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER PACK_WEEKEVENTREG_FLAG(35, 0x20)

#define WEEKEVENTREG_35_40 PACK_WEEKEVENTREG_FLAG(35, 0x40)

// Obtained Heart Piece from Five Frogs of the Frog Choir
#define WEEKEVENTREG_RECEIVED_FROG_CHOIR_HEART_PIECE PACK_WEEKEVENTREG_FLAG(35, 0x80)

// Player has spoken to certain shrine gorons in the winter
#define WEEKEVENTREG_36_01 PACK_WEEKEVENTREG_FLAG(36, 0x01)
#define WEEKEVENTREG_36_02 PACK_WEEKEVENTREG_FLAG(36, 0x02)
#define WEEKEVENTREG_36_04 PACK_WEEKEVENTREG_FLAG(36, 0x04)
#define WEEKEVENTREG_36_08 PACK_WEEKEVENTREG_FLAG(36, 0x08)
#define WEEKEVENTREG_36_10 PACK_WEEKEVENTREG_FLAG(36, 0x10)
#define WEEKEVENTREG_36_20 PACK_WEEKEVENTREG_FLAG(36, 0x20)
#define WEEKEVENTREG_36_40 PACK_WEEKEVENTREG_FLAG(36, 0x40)
#define WEEKEVENTREG_36_80 PACK_WEEKEVENTREG_FLAG(36, 0x80)
#define WEEKEVENTREG_37_01 PACK_WEEKEVENTREG_FLAG(37, 0x01)
#define WEEKEVENTREG_37_02 PACK_WEEKEVENTREG_FLAG(37, 0x02)
#define WEEKEVENTREG_37_04 PACK_WEEKEVENTREG_FLAG(37, 0x04)

#define WEEKEVENTREG_37_08 PACK_WEEKEVENTREG_FLAG(37, 0x08)
#define WEEKEVENTREG_37_10 PACK_WEEKEVENTREG_FLAG(37, 0x10)
#define WEEKEVENTREG_37_20 PACK_WEEKEVENTREG_FLAG(37, 0x20)
#define WEEKEVENTREG_37_40 PACK_WEEKEVENTREG_FLAG(37, 0x40)
#define WEEKEVENTREG_37_80 PACK_WEEKEVENTREG_FLAG(37, 0x80)
#define WEEKEVENTREG_38_01 PACK_WEEKEVENTREG_FLAG(38, 0x01)
#define WEEKEVENTREG_38_02 PACK_WEEKEVENTREG_FLAG(38, 0x02)
#define WEEKEVENTREG_38_04 PACK_WEEKEVENTREG_FLAG(38, 0x04)
#define WEEKEVENTREG_38_08 PACK_WEEKEVENTREG_FLAG(38, 0x08)
#define WEEKEVENTREG_38_10 PACK_WEEKEVENTREG_FLAG(38, 0x10)
#define WEEKEVENTREG_38_20 PACK_WEEKEVENTREG_FLAG(38, 0x20)
#define WEEKEVENTREG_38_40 PACK_WEEKEVENTREG_FLAG(38, 0x40)
#define WEEKEVENTREG_38_80 PACK_WEEKEVENTREG_FLAG(38, 0x80)
#define WEEKEVENTREG_39_01 PACK_WEEKEVENTREG_FLAG(39, 0x01)
#define WEEKEVENTREG_39_02 PACK_WEEKEVENTREG_FLAG(39, 0x02)
#define WEEKEVENTREG_39_04 PACK_WEEKEVENTREG_FLAG(39, 0x04)
#define WEEKEVENTREG_39_08 PACK_WEEKEVENTREG_FLAG(39, 0x08)
#define WEEKEVENTREG_39_10 PACK_WEEKEVENTREG_FLAG(39, 0x10)
#define WEEKEVENTREG_RECEIVED_EVAN_HEART_PIECE PACK_WEEKEVENTREG_FLAG(39, 0x20)
#define WEEKEVENTREG_39_40 PACK_WEEKEVENTREG_FLAG(39, 0x40)
#define WEEKEVENTREG_39_80 PACK_WEEKEVENTREG_FLAG(39, 0x80)
#define WEEKEVENTREG_40_01 PACK_WEEKEVENTREG_FLAG(40, 0x01)
#define WEEKEVENTREG_40_02 PACK_WEEKEVENTREG_FLAG(40, 0x02)
#define WEEKEVENTREG_40_04 PACK_WEEKEVENTREG_FLAG(40, 0x04)
#define WEEKEVENTREG_40_08 PACK_WEEKEVENTREG_FLAG(40, 0x08)
#define WEEKEVENTREG_40_10 PACK_WEEKEVENTREG_FLAG(40, 0x10)
#define WEEKEVENTREG_40_20 PACK_WEEKEVENTREG_FLAG(40, 0x20)
#define WEEKEVENTREG_40_40 PACK_WEEKEVENTREG_FLAG(40, 0x40)
#define WEEKEVENTREG_40_80 PACK_WEEKEVENTREG_FLAG(40, 0x80)
#define WEEKEVENTREG_41_01 PACK_WEEKEVENTREG_FLAG(41, 0x01)
#define WEEKEVENTREG_41_02 PACK_WEEKEVENTREG_FLAG(41, 0x02)
#define WEEKEVENTREG_41_04 PACK_WEEKEVENTREG_FLAG(41, 0x04)
#define WEEKEVENTREG_RECEIVED_GORON_RACE_BOTTLE PACK_WEEKEVENTREG_FLAG(41, 0x08)
#define WEEKEVENTREG_41_10 PACK_WEEKEVENTREG_FLAG(41, 0x10)
#define WEEKEVENTREG_41_20 PACK_WEEKEVENTREG_FLAG(41, 0x20)
#define WEEKEVENTREG_41_40 PACK_WEEKEVENTREG_FLAG(41, 0x40)
#define WEEKEVENTREG_41_80 PACK_WEEKEVENTREG_FLAG(41, 0x80)

// Used for storing the text ID offsets for the dogs in the Doggy Racetrack (56 entries)
// The number of weekEventRegs used needs to be kept in sync with RACEDOG_COUNT in z_en_aob_01.h
// PACK_WEEKEVENTREG_FLAG(42, 0x01) to PACK_WEEKEVENTREG_FLAG(48, 0x80)

#define WEEKEVENTREG_49_01 PACK_WEEKEVENTREG_FLAG(49, 0x01)
#define WEEKEVENTREG_49_02 PACK_WEEKEVENTREG_FLAG(49, 0x02)
#define WEEKEVENTREG_49_04 PACK_WEEKEVENTREG_FLAG(49, 0x04)
#define WEEKEVENTREG_49_08 PACK_WEEKEVENTREG_FLAG(49, 0x08)
#define WEEKEVENTREG_49_10 PACK_WEEKEVENTREG_FLAG(49, 0x10)
#define WEEKEVENTREG_49_20 PACK_WEEKEVENTREG_FLAG(49, 0x20)
#define WEEKEVENTREG_49_40 PACK_WEEKEVENTREG_FLAG(49, 0x40)
#define WEEKEVENTREG_49_80 PACK_WEEKEVENTREG_FLAG(49, 0x80)
#define WEEKEVENTREG_50_01 PACK_WEEKEVENTREG_FLAG(50, 0x01)
#define WEEKEVENTREG_50_02 PACK_WEEKEVENTREG_FLAG(50, 0x02)
#define WEEKEVENTREG_50_04 PACK_WEEKEVENTREG_FLAG(50, 0x04)
#define WEEKEVENTREG_PROMISED_MIDNIGHT_MEETING PACK_WEEKEVENTREG_FLAG(50, 0x08)
#define WEEKEVENTREG_50_10 PACK_WEEKEVENTREG_FLAG(50, 0x10)
#define WEEKEVENTREG_HAD_MIDNIGHT_MEETING PACK_WEEKEVENTREG_FLAG(50, 0x20)
#define WEEKEVENTREG_50_40 PACK_WEEKEVENTREG_FLAG(50, 0x40)
#define WEEKEVENTREG_RECEIVED_PENDANT_OF_MEMORIES PACK_WEEKEVENTREG_FLAG(50, 0x80)
#define WEEKEVENTREG_DELIVERED_PENDANT_OF_MEMORIES PACK_WEEKEVENTREG_FLAG(51, 0x01)
#define WEEKEVENTREG_51_02 PACK_WEEKEVENTREG_FLAG(51, 0x02)
#define WEEKEVENTREG_51_04 PACK_WEEKEVENTREG_FLAG(51, 0x04)
#define WEEKEVENTREG_51_08 PACK_WEEKEVENTREG_FLAG(51, 0x08)
#define WEEKEVENTREG_51_10 PACK_WEEKEVENTREG_FLAG(51, 0x10)
#define WEEKEVENTREG_ESCAPED_SAKONS_HIDEOUT PACK_WEEKEVENTREG_FLAG(51, 0x20)

// Set by Kafei
#define WEEKEVENTREG_COUPLES_MASK_CUTSCENE_FINISHED PACK_WEEKEVENTREG_FLAG(51, 0x40)

#define WEEKEVENTREG_51_80 PACK_WEEKEVENTREG_FLAG(51, 0x80)

// Protected Cremia
#define WEEKEVENTREG_ESCORTED_CREMIA PACK_WEEKEVENTREG_FLAG(52, 0x01)
// Lose Milk Run minigame
#define WEEKEVENTREG_52_02 PACK_WEEKEVENTREG_FLAG(52, 0x02)

#define WEEKEVENTREG_52_04 PACK_WEEKEVENTREG_FLAG(52, 0x04)
#define WEEKEVENTREG_52_08 PACK_WEEKEVENTREG_FLAG(52, 0x08)
#define WEEKEVENTREG_52_10 PACK_WEEKEVENTREG_FLAG(52, 0x10)
#define WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE PACK_WEEKEVENTREG_FLAG(52, 0x20)
#define WEEKEVENTREG_52_40 PACK_WEEKEVENTREG_FLAG(52, 0x40)
#define WEEKEVENTREG_52_80 PACK_WEEKEVENTREG_FLAG(52, 0x80)
#define WEEKEVENTREG_53_01 PACK_WEEKEVENTREG_FLAG(53, 0x01)
#define WEEKEVENTREG_RECEIVED_BUSINESS_SCRUB_HEART_PIECE PACK_WEEKEVENTREG_FLAG(53, 0x02)
#define WEEKEVENTREG_53_04 PACK_WEEKEVENTREG_FLAG(53, 0x04)
#define WEEKEVENTREG_GAVE_KOTAKE_MUSHROOM PACK_WEEKEVENTREG_FLAG(53, 0x08)
#define WEEKEVENTREG_RECEIVED_FREE_BLUE_POTION PACK_WEEKEVENTREG_FLAG(53, 0x10)
#define WEEKEVENTREG_53_20 PACK_WEEKEVENTREG_FLAG(53, 0x20)
#define WEEKEVENTREG_53_40 PACK_WEEKEVENTREG_FLAG(53, 0x40)
#define WEEKEVENTREG_53_80 PACK_WEEKEVENTREG_FLAG(53, 0x80)
#define WEEKEVENTREG_54_01 PACK_WEEKEVENTREG_FLAG(54, 0x01)
#define WEEKEVENTREG_54_02 PACK_WEEKEVENTREG_FLAG(54, 0x02)
#define WEEKEVENTREG_54_04 PACK_WEEKEVENTREG_FLAG(54, 0x04)
#define WEEKEVENTREG_54_08 PACK_WEEKEVENTREG_FLAG(54, 0x08)
#define WEEKEVENTREG_TALKED_ROMANI_ON_NIGHT_1 PACK_WEEKEVENTREG_FLAG(54, 0x10)
#define WEEKEVENTREG_54_20 PACK_WEEKEVENTREG_FLAG(54, 0x20)
#define WEEKEVENTREG_RECEIVED_SPIRIT_HOUSE_HEART_PIECE PACK_WEEKEVENTREG_FLAG(54, 0x40)
#define WEEKEVENTREG_54_80 PACK_WEEKEVENTREG_FLAG(54, 0x80)
#define WEEKEVENTREG_55_01 PACK_WEEKEVENTREG_FLAG(55, 0x01)

// Unconfirmed: "Link the Goron Claims His Reservation: 4:30 PM"
#define WEEKEVENTREG_55_02 PACK_WEEKEVENTREG_FLAG(55, 0x02)

#define WEEKEVENTREG_TALKED_PART_TIMER_AS_GORON PACK_WEEKEVENTREG_FLAG(55, 0x04)
#define WEEKEVENTREG_TALKED_PART_TIMER_AS_ZORA PACK_WEEKEVENTREG_FLAG(55, 0x08)
#define WEEKEVENTREG_TALKED_PART_TIMER_AS_DEKU PACK_WEEKEVENTREG_FLAG(55, 0x10)
#define WEEKEVENTREG_TALKED_ANJU_IN_LAUNDRY_POOL PACK_WEEKEVENTREG_FLAG(55, 0x20)
#define WEEKEVENTREG_55_40 PACK_WEEKEVENTREG_FLAG(55, 0x40)

// Gyorg has been defeated
#define WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE PACK_WEEKEVENTREG_FLAG(55, 0x80)

#define WEEKEVENTREG_56_01 PACK_WEEKEVENTREG_FLAG(56, 0x01)
#define WEEKEVENTREG_RECEIVED_MARINE_RESEARCH_LAB_FISH_HEART_PIECE PACK_WEEKEVENTREG_FLAG(56, 0x02)
#define WEEKEVENTREG_56_04 PACK_WEEKEVENTREG_FLAG(56, 0x04)
#define WEEKEVENTREG_56_08 PACK_WEEKEVENTREG_FLAG(56, 0x08)
#define WEEKEVENTREG_56_10 PACK_WEEKEVENTREG_FLAG(56, 0x10)
#define WEEKEVENTREG_56_20 PACK_WEEKEVENTREG_FLAG(56, 0x20)
#define WEEKEVENTREG_56_40 PACK_WEEKEVENTREG_FLAG(56, 0x40)
#define WEEKEVENTREG_56_80 PACK_WEEKEVENTREG_FLAG(56, 0x80)
#define WEEKEVENTREG_57_01 PACK_WEEKEVENTREG_FLAG(57, 0x01)
#define WEEKEVENTREG_57_02 PACK_WEEKEVENTREG_FLAG(57, 0x02)
#define WEEKEVENTREG_57_04 PACK_WEEKEVENTREG_FLAG(57, 0x04)
#define WEEKEVENTREG_57_08 PACK_WEEKEVENTREG_FLAG(57, 0x08)
#define WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_HUMAN PACK_WEEKEVENTREG_FLAG(57, 0x10)
#define WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_DEKU PACK_WEEKEVENTREG_FLAG(57, 0x20)
#define WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_GORON PACK_WEEKEVENTREG_FLAG(57, 0x40)
#define WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_ZORA PACK_WEEKEVENTREG_FLAG(57, 0x80)
#define WEEKEVENTREG_58_01 PACK_WEEKEVENTREG_FLAG(58, 0x01)
#define WEEKEVENTREG_58_02 PACK_WEEKEVENTREG_FLAG(58, 0x02)
#define WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_AS_NON_GORON PACK_WEEKEVENTREG_FLAG(58, 0x04)
#define WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_AS_GORON PACK_WEEKEVENTREG_FLAG(58, 0x08)
#define WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_SPRING_AS_NON_GORON PACK_WEEKEVENTREG_FLAG(58, 0x10)
#define WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_SPRING_AS_GORON PACK_WEEKEVENTREG_FLAG(58, 0x20)
#define WEEKEVENTREG_58_40 PACK_WEEKEVENTREG_FLAG(58, 0x40)
#define WEEKEVENTREG_58_80 PACK_WEEKEVENTREG_FLAG(58, 0x80)
#define WEEKEVENTREG_59_01 PACK_WEEKEVENTREG_FLAG(59, 0x01)
#define WEEKEVENTREG_59_02 PACK_WEEKEVENTREG_FLAG(59, 0x02)

// Unconfirmed: "Entered South Clock Town"
#define WEEKEVENTREG_59_04 PACK_WEEKEVENTREG_FLAG(59, 0x04)

#define WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE PACK_WEEKEVENTREG_FLAG(59, 0x08)
#define WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_QUIVER_UPGRADE PACK_WEEKEVENTREG_FLAG(59, 0x10)
#define WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE PACK_WEEKEVENTREG_FLAG(59, 0x20)
#define WEEKEVENTREG_59_40 PACK_WEEKEVENTREG_FLAG(59, 0x40)
#define WEEKEVENTREG_59_80 PACK_WEEKEVENTREG_FLAG(59, 0x80)
#define WEEKEVENTREG_60_01 PACK_WEEKEVENTREG_FLAG(60, 0x01)
#define WEEKEVENTREG_60_02 PACK_WEEKEVENTREG_FLAG(60, 0x02)
#define WEEKEVENTREG_60_04 PACK_WEEKEVENTREG_FLAG(60, 0x04)
#define WEEKEVENTREG_ATTENDED_MAYOR_MEETING PACK_WEEKEVENTREG_FLAG(60, 0x08)
#define WEEKEVENTREG_RECEIVED_MAYOR_HEART_PIECE PACK_WEEKEVENTREG_FLAG(60, 0x10)
#define WEEKEVENTREG_60_20 PACK_WEEKEVENTREG_FLAG(60, 0x20)
#define WEEKEVENTREG_TALKED_MAYOR_NIGHT_3 PACK_WEEKEVENTREG_FLAG(60, 0x40)
#define WEEKEVENTREG_60_80 PACK_WEEKEVENTREG_FLAG(60, 0x80)
#define WEEKEVENTREG_61_01 PACK_WEEKEVENTREG_FLAG(61, 0x01)
#define WEEKEVENTREG_61_02 PACK_WEEKEVENTREG_FLAG(61, 0x02)
#define WEEKEVENTREG_61_04 PACK_WEEKEVENTREG_FLAG(61, 0x04)
#define WEEKEVENTREG_61_08 PACK_WEEKEVENTREG_FLAG(61, 0x08)
#define WEEKEVENTREG_61_10 PACK_WEEKEVENTREG_FLAG(61, 0x10)
#define WEEKEVENTREG_61_20 PACK_WEEKEVENTREG_FLAG(61, 0x20)
#define WEEKEVENTREG_61_40 PACK_WEEKEVENTREG_FLAG(61, 0x40)
#define WEEKEVENTREG_61_80 PACK_WEEKEVENTREG_FLAG(61, 0x80)
#define WEEKEVENTREG_62_01 PACK_WEEKEVENTREG_FLAG(62, 0x01)
#define WEEKEVENTREG_62_02 PACK_WEEKEVENTREG_FLAG(62, 0x02)
#define WEEKEVENTREG_62_04 PACK_WEEKEVENTREG_FLAG(62, 0x04)
#define WEEKEVENTREG_62_08 PACK_WEEKEVENTREG_FLAG(62, 0x08)
#define WEEKEVENTREG_62_10 PACK_WEEKEVENTREG_FLAG(62, 0x10)
#define WEEKEVENTREG_62_20 PACK_WEEKEVENTREG_FLAG(62, 0x20)
#define WEEKEVENTREG_62_40 PACK_WEEKEVENTREG_FLAG(62, 0x40)
#define WEEKEVENTREG_62_80 PACK_WEEKEVENTREG_FLAG(62, 0x80)

// See `EnTimeTag_KickOut_WaitForTime` and `EnTimeTag_KickOut_WaitForTrigger`
#define WEEKEVENTREG_KICKOUT_WAIT PACK_WEEKEVENTREG_FLAG(63, 0x01)
// See `EnTimeTag_KickOut_WaitForTime` and `EnTimeTag_KickOut_WaitForTrigger`
#define WEEKEVENTREG_KICKOUT_TIME_PASSED PACK_WEEKEVENTREG_FLAG(63, 0x02)

#define WEEKEVENTREG_63_04 PACK_WEEKEVENTREG_FLAG(63, 0x04)
#define WEEKEVENTREG_63_08 PACK_WEEKEVENTREG_FLAG(63, 0x08)
#define WEEKEVENTREG_63_10 PACK_WEEKEVENTREG_FLAG(63, 0x10)
#define WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE PACK_WEEKEVENTREG_FLAG(63, 0x20)
#define WEEKEVENTREG_63_40 PACK_WEEKEVENTREG_FLAG(63, 0x40)

// Showed Couple's Mask at meeting
#define WEEKEVENTREG_RESOLVED_MAYOR_MEETING PACK_WEEKEVENTREG_FLAG(63, 0x80)

#define WEEKEVENTREG_64_01 PACK_WEEKEVENTREG_FLAG(64, 0x01)
#define WEEKEVENTREG_64_02 PACK_WEEKEVENTREG_FLAG(64, 0x02)
#define WEEKEVENTREG_64_04 PACK_WEEKEVENTREG_FLAG(64, 0x04)

// Two-bit field storing player form when first talked to Tingle that cycle
//  0 - Zora, 1 - Deku, 2 - Goron, 3 - Human
#define WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_LOW_BIT PACK_WEEKEVENTREG_FLAG(64, 0x08)
#define WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_HIGH_BIT PACK_WEEKEVENTREG_FLAG(64, 0x10)

#define WEEKEVENTREG_64_20 PACK_WEEKEVENTREG_FLAG(64, 0x20)
#define WEEKEVENTREG_64_40 PACK_WEEKEVENTREG_FLAG(64, 0x40)
#define WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_1 PACK_WEEKEVENTREG_FLAG(64, 0x80)
#define WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_1 PACK_WEEKEVENTREG_FLAG(65, 0x01)
#define WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_2 PACK_WEEKEVENTREG_FLAG(65, 0x02)
#define WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_2 PACK_WEEKEVENTREG_FLAG(65, 0x04)
#define WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_3 PACK_WEEKEVENTREG_FLAG(65, 0x08)
#define WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_3 PACK_WEEKEVENTREG_FLAG(65, 0x10)
#define WEEKEVENTREG_65_20 PACK_WEEKEVENTREG_FLAG(65, 0x20)
#define WEEKEVENTREG_65_40 PACK_WEEKEVENTREG_FLAG(65, 0x40)
#define WEEKEVENTREG_65_80 PACK_WEEKEVENTREG_FLAG(65, 0x80)

/* Bombers' Notebook Events */

#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ANJU                        PACK_WEEKEVENTREG_FLAG(66, 0x01)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_KAFEI                       PACK_WEEKEVENTREG_FLAG(66, 0x02)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN          PACK_WEEKEVENTREG_FLAG(66, 0x04)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_BOMB_SHOP_LADY              PACK_WEEKEVENTREG_FLAG(66, 0x08)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ROMANI                      PACK_WEEKEVENTREG_FLAG(66, 0x10)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_CREMIA                      PACK_WEEKEVENTREG_FLAG(66, 0x20)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_MAYOR_DOTOUR                PACK_WEEKEVENTREG_FLAG(66, 0x40)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA                PACK_WEEKEVENTREG_FLAG(66, 0x80)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_TOTO                        PACK_WEEKEVENTREG_FLAG(67, 0x01)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GORMAN                      PACK_WEEKEVENTREG_FLAG(67, 0x02)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN                     PACK_WEEKEVENTREG_FLAG(67, 0x04)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ROSA_SISTERS                PACK_WEEKEVENTREG_FLAG(67, 0x08)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_TOILET_HAND                 PACK_WEEKEVENTREG_FLAG(67, 0x10)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ANJUS_GRANDMOTHER           PACK_WEEKEVENTREG_FLAG(67, 0x20)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_KAMARO                      PACK_WEEKEVENTREG_FLAG(67, 0x40)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GROG                        PACK_WEEKEVENTREG_FLAG(67, 0x80)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS             PACK_WEEKEVENTREG_FLAG(68, 0x01)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_SHIRO                       PACK_WEEKEVENTREG_FLAG(68, 0x02)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GURU_GURU                   PACK_WEEKEVENTREG_FLAG(68, 0x04)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_BOMBERS                     PACK_WEEKEVENTREG_FLAG(68, 0x08)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROOM_KEY               PACK_WEEKEVENTREG_FLAG(68, 0x10)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_PROMISED_MIDNIGHT_MEETING       PACK_WEEKEVENTREG_FLAG(68, 0x20)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_PROMISED_TO_MEET_KAFEI          PACK_WEEKEVENTREG_FLAG(68, 0x40)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_LETTER_TO_KAFEI        PACK_WEEKEVENTREG_FLAG(68, 0x80)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI       PACK_WEEKEVENTREG_FLAG(69, 0x01)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_PENDANT_OF_MEMORIES    PACK_WEEKEVENTREG_FLAG(69, 0x02)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DELIVERED_PENDANT_OF_MEMORIES   PACK_WEEKEVENTREG_FLAG(69, 0x04)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_ESCAPED_SAKONS_HIDEOUT          PACK_WEEKEVENTREG_FLAG(69, 0x08)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_PROMISED_TO_HELP_WITH_ALIENS    PACK_WEEKEVENTREG_FLAG(69, 0x10)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DEFENDED_AGAINST_ALIENS         PACK_WEEKEVENTREG_FLAG(69, 0x20)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALIENS_BOTTLE            PACK_WEEKEVENTREG_FLAG(69, 0x40)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_ESCORTED_CREMIA                 PACK_WEEKEVENTREG_FLAG(69, 0x80)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROMANIS_MASK           PACK_WEEKEVENTREG_FLAG(70, 0x01)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KEATON_MASK            PACK_WEEKEVENTREG_FLAG(70, 0x02)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_PRIORITY_MAIL          PACK_WEEKEVENTREG_FLAG(70, 0x04)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DELIVERED_PRIORITY_MAIL         PACK_WEEKEVENTREG_FLAG(70, 0x08)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE             PACK_WEEKEVENTREG_FLAG(70, 0x10)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BOMBERS_NOTEBOOK       PACK_WEEKEVENTREG_FLAG(70, 0x20)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_MAYOR_HP               PACK_WEEKEVENTREG_FLAG(70, 0x40)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROSA_SISTERS_HP        PACK_WEEKEVENTREG_FLAG(70, 0x80)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_TOILET_HAND_HP         PACK_WEEKEVENTREG_FLAG(71, 0x01)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_SHORT_STORY_HP PACK_WEEKEVENTREG_FLAG(71, 0x02)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_LONG_STORY_HP  PACK_WEEKEVENTREG_FLAG(71, 0x04)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMAN_HP             PACK_WEEKEVENTREG_FLAG(71, 0x08)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK            PACK_WEEKEVENTREG_FLAG(71, 0x10)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALL_NIGHT_MASK         PACK_WEEKEVENTREG_FLAG(71, 0x20)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BUNNY_HOOD             PACK_WEEKEVENTREG_FLAG(71, 0x40)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GAROS_MASK             PACK_WEEKEVENTREG_FLAG(71, 0x80)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK    PACK_WEEKEVENTREG_FLAG(72, 0x01)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMANS_HAT           PACK_WEEKEVENTREG_FLAG(72, 0x02)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_COUPLES_MASK           PACK_WEEKEVENTREG_FLAG(72, 0x04)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BLAST_MASK             PACK_WEEKEVENTREG_FLAG(72, 0x08)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAMAROS_MASK           PACK_WEEKEVENTREG_FLAG(72, 0x10)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_STONE_MASK             PACK_WEEKEVENTREG_FLAG(72, 0x20)
#define WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BREMEN_MASK            PACK_WEEKEVENTREG_FLAG(72, 0x40)

#define WEEKEVENTREG_72_80 PACK_WEEKEVENTREG_FLAG(72, 0x80)
#define WEEKEVENTREG_73_01 PACK_WEEKEVENTREG_FLAG(73, 0x01)
#define WEEKEVENTREG_73_02 PACK_WEEKEVENTREG_FLAG(73, 0x02)
#define WEEKEVENTREG_73_04 PACK_WEEKEVENTREG_FLAG(73, 0x04)
#define WEEKEVENTREG_73_08 PACK_WEEKEVENTREG_FLAG(73, 0x08)

// Unconfirmed: "Bombers Hide & Seek started on Day 1?"
#define WEEKEVENTREG_73_10 PACK_WEEKEVENTREG_FLAG(73, 0x10)

#define WEEKEVENTREG_73_20 PACK_WEEKEVENTREG_FLAG(73, 0x20)
#define WEEKEVENTREG_73_40 PACK_WEEKEVENTREG_FLAG(73, 0x40)
#define WEEKEVENTREG_73_80 PACK_WEEKEVENTREG_FLAG(73, 0x80)
#define WEEKEVENTREG_74_01 PACK_WEEKEVENTREG_FLAG(74, 0x01)
#define WEEKEVENTREG_74_02 PACK_WEEKEVENTREG_FLAG(74, 0x02)
#define WEEKEVENTREG_74_04 PACK_WEEKEVENTREG_FLAG(74, 0x04)
#define WEEKEVENTREG_74_08 PACK_WEEKEVENTREG_FLAG(74, 0x08)
#define WEEKEVENTREG_74_10 PACK_WEEKEVENTREG_FLAG(74, 0x10)
#define WEEKEVENTREG_74_20 PACK_WEEKEVENTREG_FLAG(74, 0x20)
#define WEEKEVENTREG_74_40 PACK_WEEKEVENTREG_FLAG(74, 0x40)
#define WEEKEVENTREG_74_80 PACK_WEEKEVENTREG_FLAG(74, 0x80)
#define WEEKEVENTREG_75_01 PACK_WEEKEVENTREG_FLAG(75, 0x01)
#define WEEKEVENTREG_75_02 PACK_WEEKEVENTREG_FLAG(75, 0x02)
#define WEEKEVENTREG_75_04 PACK_WEEKEVENTREG_FLAG(75, 0x04)
#define WEEKEVENTREG_75_08 PACK_WEEKEVENTREG_FLAG(75, 0x08)
#define WEEKEVENTREG_RECEIVED_ROOM_KEY PACK_WEEKEVENTREG_FLAG(75, 0x10)
#define WEEKEVENTREG_75_20 PACK_WEEKEVENTREG_FLAG(75, 0x20)
#define WEEKEVENTREG_75_40 PACK_WEEKEVENTREG_FLAG(75, 0x40)
#define WEEKEVENTREG_RECEIVED_ROSA_SISTERS_HEART_PIECE PACK_WEEKEVENTREG_FLAG(75, 0x80)
#define WEEKEVENTREG_76_01 PACK_WEEKEVENTREG_FLAG(76, 0x01)
#define WEEKEVENTREG_76_02 PACK_WEEKEVENTREG_FLAG(76, 0x02)
#define WEEKEVENTREG_76_04 PACK_WEEKEVENTREG_FLAG(76, 0x04)
#define WEEKEVENTREG_76_08 PACK_WEEKEVENTREG_FLAG(76, 0x08)
#define WEEKEVENTREG_76_10 PACK_WEEKEVENTREG_FLAG(76, 0x10)
#define WEEKEVENTREG_76_20 PACK_WEEKEVENTREG_FLAG(76, 0x20)
#define WEEKEVENTREG_76_40 PACK_WEEKEVENTREG_FLAG(76, 0x40)
#define WEEKEVENTREG_76_80 PACK_WEEKEVENTREG_FLAG(76, 0x80)
#define WEEKEVENTREG_RECEIVED_POSTMAN_COUNTING_GAME_HEART_PIECE PACK_WEEKEVENTREG_FLAG(77, 0x01)
#define WEEKEVENTREG_77_02 PACK_WEEKEVENTREG_FLAG(77, 0x02)
#define WEEKEVENTREG_77_04 PACK_WEEKEVENTREG_FLAG(77, 0x04)
#define WEEKEVENTREG_77_08 PACK_WEEKEVENTREG_FLAG(77, 0x08)
#define WEEKEVENTREG_77_10 PACK_WEEKEVENTREG_FLAG(77, 0x10)
#define WEEKEVENTREG_77_20 PACK_WEEKEVENTREG_FLAG(77, 0x20)
#define WEEKEVENTREG_77_40 PACK_WEEKEVENTREG_FLAG(77, 0x40)

// The player has heard the Goron Shrine cheer as a Goron at least once.
#define WEEKEVENTREG_77_80 PACK_WEEKEVENTREG_FLAG(77, 0x80)

#define WEEKEVENTREG_78_01 PACK_WEEKEVENTREG_FLAG(78, 0x01)
#define WEEKEVENTREG_78_02 PACK_WEEKEVENTREG_FLAG(78, 0x02)
#define WEEKEVENTREG_78_04 PACK_WEEKEVENTREG_FLAG(78, 0x04)
#define WEEKEVENTREG_78_08 PACK_WEEKEVENTREG_FLAG(78, 0x08)
#define WEEKEVENTREG_78_10 PACK_WEEKEVENTREG_FLAG(78, 0x10)
#define WEEKEVENTREG_78_20 PACK_WEEKEVENTREG_FLAG(78, 0x20)
#define WEEKEVENTREG_78_40 PACK_WEEKEVENTREG_FLAG(78, 0x40)
#define WEEKEVENTREG_78_80 PACK_WEEKEVENTREG_FLAG(78, 0x80)
#define WEEKEVENTREG_79_01 PACK_WEEKEVENTREG_FLAG(79, 0x01)
#define WEEKEVENTREG_79_02 PACK_WEEKEVENTREG_FLAG(79, 0x02)
#define WEEKEVENTREG_79_04 PACK_WEEKEVENTREG_FLAG(79, 0x04)
#define WEEKEVENTREG_79_08 PACK_WEEKEVENTREG_FLAG(79, 0x08)
#define WEEKEVENTREG_79_10 PACK_WEEKEVENTREG_FLAG(79, 0x10)
#define WEEKEVENTREG_79_20 PACK_WEEKEVENTREG_FLAG(79, 0x20)
#define WEEKEVENTREG_SAKON_DEAD PACK_WEEKEVENTREG_FLAG(79, 0x40)
#define WEEKEVENTREG_RECEIVED_KEATON_HEART_PIECE PACK_WEEKEVENTREG_FLAG(79, 0x80)
#define WEEKEVENTREG_80_01 PACK_WEEKEVENTREG_FLAG(80, 0x01)
#define WEEKEVENTREG_80_02 PACK_WEEKEVENTREG_FLAG(80, 0x02)
#define WEEKEVENTREG_80_04 PACK_WEEKEVENTREG_FLAG(80, 0x04)

// Aveil has spotted Player
#define WEEKEVENTREG_80_08 PACK_WEEKEVENTREG_FLAG(80, 0x08)

#define WEEKEVENTREG_RECEIVED_PRIORITY_MAIL PACK_WEEKEVENTREG_FLAG(80, 0x10)
#define WEEKEVENTREG_80_20 PACK_WEEKEVENTREG_FLAG(80, 0x20)
#define WEEKEVENTREG_80_40 PACK_WEEKEVENTREG_FLAG(80, 0x40)
#define WEEKEVENTREG_80_80 PACK_WEEKEVENTREG_FLAG(80, 0x80)
#define WEEKEVENTREG_81_01 PACK_WEEKEVENTREG_FLAG(81, 0x01)
#define WEEKEVENTREG_81_02 PACK_WEEKEVENTREG_FLAG(81, 0x02)
#define WEEKEVENTREG_81_04 PACK_WEEKEVENTREG_FLAG(81, 0x04)
#define WEEKEVENTREG_81_08 PACK_WEEKEVENTREG_FLAG(81, 0x08)
#define WEEKEVENTREG_81_10 PACK_WEEKEVENTREG_FLAG(81, 0x10)
#define WEEKEVENTREG_81_20 PACK_WEEKEVENTREG_FLAG(81, 0x20)
#define WEEKEVENTREG_81_40 PACK_WEEKEVENTREG_FLAG(81, 0x40)
#define WEEKEVENTREG_81_80 PACK_WEEKEVENTREG_FLAG(81, 0x80)
#define WEEKEVENTREG_82_01 PACK_WEEKEVENTREG_FLAG(82, 0x01)
#define WEEKEVENTREG_82_02 PACK_WEEKEVENTREG_FLAG(82, 0x02)

// check if already healed Kamaro the Dancing Ghost
#define WEEKEVENTREG_82_04 PACK_WEEKEVENTREG_FLAG(82, 0x04)

// Related to Swordsman's log minigame
#define WEEKEVENTREG_82_08 PACK_WEEKEVENTREG_FLAG(82, 0x08)
#define WEEKEVENTREG_RECEIVED_FISHERMANS_JUMPING_GAME_HEART_PIECE PACK_WEEKEVENTREG_FLAG(82, 0x10)
#define WEEKEVENTREG_82_20 PACK_WEEKEVENTREG_FLAG(82, 0x20)
#define WEEKEVENTREG_82_40 PACK_WEEKEVENTREG_FLAG(82, 0x40)
#define WEEKEVENTREG_82_80 PACK_WEEKEVENTREG_FLAG(82, 0x80)
#define WEEKEVENTREG_83_01 PACK_WEEKEVENTREG_FLAG(83, 0x01)

// Knocked the Gerudo beehive down
#define WEEKEVENTREG_83_02 PACK_WEEKEVENTREG_FLAG(83, 0x02)

#define WEEKEVENTREG_83_04 PACK_WEEKEVENTREG_FLAG(83, 0x04)
#define WEEKEVENTREG_83_08 PACK_WEEKEVENTREG_FLAG(83, 0x08)
#define WEEKEVENTREG_83_10 PACK_WEEKEVENTREG_FLAG(83, 0x10)
#define WEEKEVENTREG_83_20 PACK_WEEKEVENTREG_FLAG(83, 0x20)
#define WEEKEVENTREG_83_40 PACK_WEEKEVENTREG_FLAG(83, 0x40)
#define WEEKEVENTREG_83_80 PACK_WEEKEVENTREG_FLAG(83, 0x80)
#define WEEKEVENTREG_84_01 PACK_WEEKEVENTREG_FLAG(84, 0x01)
#define WEEKEVENTREG_84_02 PACK_WEEKEVENTREG_FLAG(84, 0x02)
#define WEEKEVENTREG_84_04 PACK_WEEKEVENTREG_FLAG(84, 0x04)
#define WEEKEVENTREG_84_08 PACK_WEEKEVENTREG_FLAG(84, 0x08)
#define WEEKEVENTREG_84_10 PACK_WEEKEVENTREG_FLAG(84, 0x10)

// Unconfirmed: "Obtained Fierce Deity's Mask?"
// Also related to moon child
#define WEEKEVENTREG_84_20 PACK_WEEKEVENTREG_FLAG(84, 0x20)

#define WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME PACK_WEEKEVENTREG_FLAG(84, 0x40)
#define WEEKEVENTREG_84_80 PACK_WEEKEVENTREG_FLAG(84, 0x80)
#define WEEKEVENTREG_85_01 PACK_WEEKEVENTREG_FLAG(85, 0x01)

// Unconfirmed: "Bombers Hide & Seek in Progress"
#define WEEKEVENTREG_85_02 PACK_WEEKEVENTREG_FLAG(85, 0x02)

#define WEEKEVENTREG_85_04 PACK_WEEKEVENTREG_FLAG(85, 0x04)

// These get set per location, if Kotake tries to give the player a Red Potion to save Koume,
// but is unable to do so since all bottles are full.
#define WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_SHOP PACK_WEEKEVENTREG_FLAG(85, 0x08)
#define WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_WOODS PACK_WEEKEVENTREG_FLAG(85, 0x10)


#define WEEKEVENTREG_85_20 PACK_WEEKEVENTREG_FLAG(85, 0x20)
#define WEEKEVENTREG_85_40 PACK_WEEKEVENTREG_FLAG(85, 0x40)

// Unconfirmed: "Postman showing priority mail to Madame"
#define WEEKEVENTREG_85_80 PACK_WEEKEVENTREG_FLAG(85, 0x80)

#define WEEKEVENTREG_86_01 PACK_WEEKEVENTREG_FLAG(86, 0x01)
#define WEEKEVENTREG_86_02 PACK_WEEKEVENTREG_FLAG(86, 0x02)
#define WEEKEVENTREG_86_04 PACK_WEEKEVENTREG_FLAG(86, 0x04)
#define WEEKEVENTREG_LISTENED_ANJU_POSTMAN_CONVERSATION PACK_WEEKEVENTREG_FLAG(86, 0x08)
#define WEEKEVENTREG_86_10 PACK_WEEKEVENTREG_FLAG(86, 0x10)
#define WEEKEVENTREG_86_20 PACK_WEEKEVENTREG_FLAG(86, 0x20)
#define WEEKEVENTREG_86_40 PACK_WEEKEVENTREG_FLAG(86, 0x40)
#define WEEKEVENTREG_86_80 PACK_WEEKEVENTREG_FLAG(86, 0x80)
// Currently talking to a cow using the voice recognition unit
#define WEEKEVENTREG_TALKING_TO_COW_WITH_VOICE PACK_WEEKEVENTREG_FLAG(87, 0x01)
// Set by Anju
#define WEEKEVENTREG_COUPLES_MASK_CUTSCENE_STARTED PACK_WEEKEVENTREG_FLAG(87, 0x02)
#define WEEKEVENTREG_87_04 PACK_WEEKEVENTREG_FLAG(87, 0x04)
#define WEEKEVENTREG_87_08 PACK_WEEKEVENTREG_FLAG(87, 0x08)
#define WEEKEVENTREG_87_10 PACK_WEEKEVENTREG_FLAG(87, 0x10)
#define WEEKEVENTREG_87_20 PACK_WEEKEVENTREG_FLAG(87, 0x20)
#define WEEKEVENTREG_87_40 PACK_WEEKEVENTREG_FLAG(87, 0x40)
#define WEEKEVENTREG_87_80 PACK_WEEKEVENTREG_FLAG(87, 0x80)
#define WEEKEVENTREG_88_01 PACK_WEEKEVENTREG_FLAG(88, 0x01)
#define WEEKEVENTREG_88_02 PACK_WEEKEVENTREG_FLAG(88, 0x02)
#define WEEKEVENTREG_88_04 PACK_WEEKEVENTREG_FLAG(88, 0x04)
#define WEEKEVENTREG_88_08 PACK_WEEKEVENTREG_FLAG(88, 0x08)
#define WEEKEVENTREG_88_10 PACK_WEEKEVENTREG_FLAG(88, 0x10)
#define WEEKEVENTREG_88_20 PACK_WEEKEVENTREG_FLAG(88, 0x20)

// Goron shrine Gatekeeper has opened shrine.
#define WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE PACK_WEEKEVENTREG_FLAG(88, 0x40)
// Goron shrine Gatekeeper has opened shrine for player in Human form.
#define WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_HUMAN PACK_WEEKEVENTREG_FLAG(88, 0x80)
// Goron shrine Gatekeeper has opened shrine for player in Deku form.
#define WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_DEKU PACK_WEEKEVENTREG_FLAG(89, 0x01)
// Goron shrine Gatekeeper has opened shrine for player in Zora form.
#define WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_ZORA PACK_WEEKEVENTREG_FLAG(89, 0x02)
// Goron shrine Gatekeeper has opened shrine for player in Goron form.
#define WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE_FOR_GORON PACK_WEEKEVENTREG_FLAG(89, 0x04)

// Unconfirmed: "Postman has delivered priority mail"
#define WEEKEVENTREG_89_08 PACK_WEEKEVENTREG_FLAG(89, 0x08)

// Player tried but failed to defend Romani Ranch from the alien invasion on Night 1
// This is only set if the player is actually in the ranch when the aliens reach the barn.
// If the player isn't in the ranch when this happens, then this weekeventreg will remain unset.
#define WEEKEVENTREG_FAILED_TO_DEFEND_AGAINST_ALIENS PACK_WEEKEVENTREG_FLAG(89, 0x10)

#define WEEKEVENTREG_89_20 PACK_WEEKEVENTREG_FLAG(89, 0x20)

// Unconfirmed: "Postman is about to flee"
#define WEEKEVENTREG_89_40 PACK_WEEKEVENTREG_FLAG(89, 0x40)

#define WEEKEVENTREG_89_80 PACK_WEEKEVENTREG_FLAG(89, 0x80)

// Unconfirmed: "Postman fleeing town"
#define WEEKEVENTREG_90_01 PACK_WEEKEVENTREG_FLAG(90, 0x01)

#define WEEKEVENTREG_90_02 PACK_WEEKEVENTREG_FLAG(90, 0x02)
#define WEEKEVENTREG_90_04 PACK_WEEKEVENTREG_FLAG(90, 0x04)
#define WEEKEVENTREG_90_08 PACK_WEEKEVENTREG_FLAG(90, 0x08)
#define WEEKEVENTREG_RECEIVED_GOSSIP_STONE_GROTTO_HEART_PIECE PACK_WEEKEVENTREG_FLAG(90, 0x10)

// Related to Fishermans's jumping minigame
#define WEEKEVENTREG_90_20 PACK_WEEKEVENTREG_FLAG(90, 0x20)
#define WEEKEVENTREG_90_40 PACK_WEEKEVENTREG_FLAG(90, 0x40)
#define WEEKEVENTREG_90_80 PACK_WEEKEVENTREG_FLAG(90, 0x80)
#define WEEKEVENTREG_91_01 PACK_WEEKEVENTREG_FLAG(91, 0x01)
#define WEEKEVENTREG_91_02 PACK_WEEKEVENTREG_FLAG(91, 0x02)
#define WEEKEVENTREG_91_04 PACK_WEEKEVENTREG_FLAG(91, 0x04)
#define WEEKEVENTREG_91_08 PACK_WEEKEVENTREG_FLAG(91, 0x08)
#define WEEKEVENTREG_91_10 PACK_WEEKEVENTREG_FLAG(91, 0x10)
#define WEEKEVENTREG_91_20 PACK_WEEKEVENTREG_FLAG(91, 0x20)
#define WEEKEVENTREG_91_40 PACK_WEEKEVENTREG_FLAG(91, 0x40)
#define WEEKEVENTREG_91_80 PACK_WEEKEVENTREG_FLAG(91, 0x80)

// HORSE_RACE_STATE (3 entries)
// WEEKEVENTREG_92_01 to WEEKEVENTREG_92_04

#define WEEKEVENTREG_92_08 PACK_WEEKEVENTREG_FLAG(92, 0x08)
#define WEEKEVENTREG_92_10 PACK_WEEKEVENTREG_FLAG(92, 0x10)
#define WEEKEVENTREG_92_20 PACK_WEEKEVENTREG_FLAG(92, 0x20)
#define WEEKEVENTREG_92_40 PACK_WEEKEVENTREG_FLAG(92, 0x40)
#define WEEKEVENTREG_92_80 PACK_WEEKEVENTREG_FLAG(92, 0x80)
#define WEEKEVENTREG_93_01 PACK_WEEKEVENTREG_FLAG(93, 0x01)
#define WEEKEVENTREG_93_02 PACK_WEEKEVENTREG_FLAG(93, 0x02)
#define WEEKEVENTREG_93_04 PACK_WEEKEVENTREG_FLAG(93, 0x04)
#define WEEKEVENTREG_93_08 PACK_WEEKEVENTREG_FLAG(93, 0x08)
#define WEEKEVENTREG_93_10 PACK_WEEKEVENTREG_FLAG(93, 0x10)
#define WEEKEVENTREG_93_20 PACK_WEEKEVENTREG_FLAG(93, 0x20)
#define WEEKEVENTREG_93_40 PACK_WEEKEVENTREG_FLAG(93, 0x40)
#define WEEKEVENTREG_93_80 PACK_WEEKEVENTREG_FLAG(93, 0x80)
#define WEEKEVENTREG_94_01 PACK_WEEKEVENTREG_FLAG(94, 0x01)
#define WEEKEVENTREG_94_02 PACK_WEEKEVENTREG_FLAG(94, 0x02)
#define WEEKEVENTREG_94_04 PACK_WEEKEVENTREG_FLAG(94, 0x04)
#define WEEKEVENTREG_94_08 PACK_WEEKEVENTREG_FLAG(94, 0x08)
#define WEEKEVENTREG_94_10 PACK_WEEKEVENTREG_FLAG(94, 0x10)
#define WEEKEVENTREG_94_20 PACK_WEEKEVENTREG_FLAG(94, 0x20)
#define WEEKEVENTREG_94_40 PACK_WEEKEVENTREG_FLAG(94, 0x40)
#define WEEKEVENTREG_94_80 PACK_WEEKEVENTREG_FLAG(94, 0x80)
#define WEEKEVENTREG_95_01 PACK_WEEKEVENTREG_FLAG(95, 0x01)
#define WEEKEVENTREG_95_02 PACK_WEEKEVENTREG_FLAG(95, 0x02)
#define WEEKEVENTREG_95_04 PACK_WEEKEVENTREG_FLAG(95, 0x04)
#define WEEKEVENTREG_95_08 PACK_WEEKEVENTREG_FLAG(95, 0x08)
#define WEEKEVENTREG_95_10 PACK_WEEKEVENTREG_FLAG(95, 0x10)
#define WEEKEVENTREG_95_20 PACK_WEEKEVENTREG_FLAG(95, 0x20)
#define WEEKEVENTREG_95_40 PACK_WEEKEVENTREG_FLAG(95, 0x40)
#define WEEKEVENTREG_95_80 PACK_WEEKEVENTREG_FLAG(95, 0x80)
#define WEEKEVENTREG_96_01 PACK_WEEKEVENTREG_FLAG(96, 0x01)
#define WEEKEVENTREG_96_02 PACK_WEEKEVENTREG_FLAG(96, 0x02)
#define WEEKEVENTREG_96_04 PACK_WEEKEVENTREG_FLAG(96, 0x04)
#define WEEKEVENTREG_96_08 PACK_WEEKEVENTREG_FLAG(96, 0x08)
#define WEEKEVENTREG_96_10 PACK_WEEKEVENTREG_FLAG(96, 0x10)
#define WEEKEVENTREG_96_20 PACK_WEEKEVENTREG_FLAG(96, 0x20)
#define WEEKEVENTREG_96_40 PACK_WEEKEVENTREG_FLAG(96, 0x40)
#define WEEKEVENTREG_96_80 PACK_WEEKEVENTREG_FLAG(96, 0x80)
#define WEEKEVENTREG_97_01 PACK_WEEKEVENTREG_FLAG(97, 0x01)
#define WEEKEVENTREG_97_02 PACK_WEEKEVENTREG_FLAG(97, 0x02)
#define WEEKEVENTREG_97_04 PACK_WEEKEVENTREG_FLAG(97, 0x04)
#define WEEKEVENTREG_97_08 PACK_WEEKEVENTREG_FLAG(97, 0x08)
#define WEEKEVENTREG_97_10 PACK_WEEKEVENTREG_FLAG(97, 0x10)
#define WEEKEVENTREG_97_20 PACK_WEEKEVENTREG_FLAG(97, 0x20)
#define WEEKEVENTREG_97_40 PACK_WEEKEVENTREG_FLAG(97, 0x40)
#define WEEKEVENTREG_97_80 PACK_WEEKEVENTREG_FLAG(97, 0x80)
#define WEEKEVENTREG_98_01 PACK_WEEKEVENTREG_FLAG(98, 0x01)
#define WEEKEVENTREG_98_02 PACK_WEEKEVENTREG_FLAG(98, 0x02)
#define WEEKEVENTREG_98_04 PACK_WEEKEVENTREG_FLAG(98, 0x04)
#define WEEKEVENTREG_98_08 PACK_WEEKEVENTREG_FLAG(98, 0x08)
#define WEEKEVENTREG_98_10 PACK_WEEKEVENTREG_FLAG(98, 0x10)
#define WEEKEVENTREG_98_20 PACK_WEEKEVENTREG_FLAG(98, 0x20)
#define WEEKEVENTREG_98_40 PACK_WEEKEVENTREG_FLAG(98, 0x40)
#define WEEKEVENTREG_98_80 PACK_WEEKEVENTREG_FLAG(98, 0x80)
#define WEEKEVENTREG_99_01 PACK_WEEKEVENTREG_FLAG(99, 0x01)
#define WEEKEVENTREG_99_02 PACK_WEEKEVENTREG_FLAG(99, 0x02)
#define WEEKEVENTREG_99_04 PACK_WEEKEVENTREG_FLAG(99, 0x04)
#define WEEKEVENTREG_99_08 PACK_WEEKEVENTREG_FLAG(99, 0x08)
#define WEEKEVENTREG_99_10 PACK_WEEKEVENTREG_FLAG(99, 0x10)
#define WEEKEVENTREG_99_20 PACK_WEEKEVENTREG_FLAG(99, 0x20)
#define WEEKEVENTREG_99_40 PACK_WEEKEVENTREG_FLAG(99, 0x40)
#define WEEKEVENTREG_99_80 PACK_WEEKEVENTREG_FLAG(99, 0x80)

#define WEEKEVENTREG(index) (gSaveContext.save.saveInfo.weekEventReg[(index)])
#define GET_WEEKEVENTREG(index) ((void)0, WEEKEVENTREG(index))

#define CHECK_WEEKEVENTREG(flag) (WEEKEVENTREG((flag) >> 8) & ((flag) & 0xFF))
#define CHECK_WEEKEVENTREG_ALT(flag) (GET_WEEKEVENTREG((flag) >> 8) & ((flag) & 0xFF))
// #region 2S2H Originally these flags were all set with macros, for the port we want them to be in functions so we can hook into them
#define SET_WEEKEVENTREG(flag) (Flags_SetWeekEventReg(flag))
#define CLEAR_WEEKEVENTREG(flag) (Flags_ClearWeekEventReg(flag))
// #endregion

typedef enum {
    /* 0 */ WEEKEVENTREG_HORSE_RACE_STATE_END,
    /* 1 */ WEEKEVENTREG_HORSE_RACE_STATE_START,
    /* 2 */ WEEKEVENTREG_HORSE_RACE_STATE_2,
    /* 3 */ WEEKEVENTREG_HORSE_RACE_STATE_3,
    /* 4 */ WEEKEVENTREG_HORSE_RACE_STATE_4
} WeekEventRegHorseRaceState;

#define WEEKEVENTREG_HORSE_RACE_STATE_MASK 7

#define GET_WEEKEVENTREG_HORSE_RACE_STATE (WEEKEVENTREG(92) & WEEKEVENTREG_HORSE_RACE_STATE_MASK)

// #region 2S2H Originally these flags were all set with macros, for the port we want them to be in functions so we can hook into them
#define SET_WEEKEVENTREG_HORSE_RACE_STATE(state) (Flags_SetWeekEventRegHorseRace(state))
// #endregion

#define GET_WEEKEVENTREG_DOG_RACE_TEXT(index, baseTextId)                         \
    (index % 2) ? (baseTextId + (((WEEKEVENTREG(42 + (index / 2))) & 0xF0) >> 4)) \
                : (baseTextId + ((WEEKEVENTREG(42 + (index / 2))) & 0x0F));

#define SET_WEEKEVENTREG_DOG_RACE_TEXT_OFFSETS(index, packedOffsets)                    \
    {                                                                                   \
        WEEKEVENTREG(42 + index) = ((void)0, WEEKEVENTREG(42 + index)) | packedOffsets; \
    }

#define CLEAR_WEEKEVENTREG_DOG_RACE_TEXT_OFFSETS(index) \
    {                                                   \
        WEEKEVENTREG(42 + index) = 0;                   \
    }

/**
 * gSaveContext.eventInf
 */

// gSaveContext.eventInf[0] is used for dog race information (8 entries)
// EVENTINF_00 to EVENTINF_07

#define EVENTINF_10 0x10
#define EVENTINF_11 0x11
#define EVENTINF_12 0x12
#define EVENTINF_13 0x13
#define EVENTINF_14 0x14
#define EVENTINF_15 0x15
#define EVENTINF_16 0x16
#define EVENTINF_17 0x17
#define EVENTINF_20 0x20
#define EVENTINF_21 0x21
#define EVENTINF_22 0x22
#define EVENTINF_23 0x23
#define EVENTINF_24 0x24
#define EVENTINF_25 0x25
#define EVENTINF_26 0x26
// "Dawn of ... day". Must be triggered with respawnFlag -4 or -99
#define EVENTINF_TRIGGER_DAYTELOP 0x27
#define EVENTINF_30 0x30

// EVENTINF_31 is used to track if Player is within range of EnGakufu (2D Song Buttons Appearing on Wall)
#define EVENTINF_31 0x31
// EVENTINF_32 is used to track if Player has played the notes of EnGakufu (2D Song Buttons Appearing on Wall)
#define EVENTINF_32 0x32

#define EVENTINF_33 0x33

// Related to Deku playground minigame
#define EVENTINF_34 0x34
#define EVENTINF_35 0x35
#define EVENTINF_36 0x36
#define EVENTINF_37 0x37
#define EVENTINF_40 0x40

// Related to swamp boat (non-minigame)?
#define EVENTINF_41 0x41
#define EVENTINF_42 0x42
#define EVENTINF_43 0x43
#define EVENTINF_44 0x44
#define EVENTINF_45 0x45
#define EVENTINF_46 0x46
#define EVENTINF_47 0x47
#define EVENTINF_50 0x50
#define EVENTINF_51 0x51
#define EVENTINF_HAS_DAYTIME_TRANSITION_CS 0x52

// Enabled when various boss intro cutscene has been watched
#define EVENTINF_INTRO_CS_WATCHED_GOHT 0x53
#define EVENTINF_INTRO_CS_WATCHED_ODOLWA 0x54
#define EVENTINF_INTRO_CS_WATCHED_TWINMOLD 0x55
#define EVENTINF_INTRO_CS_WATCHED_GYORG 0x56
#define EVENTINF_INTRO_CS_WATCHED_IGOS_DU_IKANA 0x57
#define EVENTINF_INTRO_CS_WATCHED_WART 0x60
#define EVENTINF_INTRO_CS_WATCHED_MAJORA 0x61
#define EVENTINF_ENTR_CS_WATCHED_GOHT 0x62
#define EVENTINF_INTRO_CS_WATCHED_GOMESS 0x63

#define EVENTINF_64 0x64
#define EVENTINF_65 0x65
#define EVENTINF_66 0x66
#define EVENTINF_67 0x67

// Tracks which ammo is lost during the three day reset
#define EVENTINF_THREEDAYRESET_LOST_RUPEES 0x70
#define EVENTINF_THREEDAYRESET_LOST_BOMB_AMMO 0x71
#define EVENTINF_THREEDAYRESET_LOST_NUT_AMMO 0x72
#define EVENTINF_THREEDAYRESET_LOST_STICK_AMMO 0x73
#define EVENTINF_THREEDAYRESET_LOST_ARROW_AMMO 0x74

// Checked in Kankyo as one to store the day: gSaveContext.eventInf[7] & 0xE0
// EVENTINF_75 to EVENTINF_77

#define CHECK_EVENTINF(flag) (gSaveContext.eventInf[(flag) >> 4] & (1 << ((flag) & 0xF)))
// #region 2S2H Originally these flags were all set with macros, for the port we want them to be in functions so we can hook into them
#define SET_EVENTINF(flag) (Flags_SetEventInf(flag))
#define CLEAR_EVENTINF(flag) (Flags_ClearEventInf(flag))
#define CLEAR_EVENTINF_ALT(flag) (Flags_ClearEventInf(flag))
// #endregion

typedef enum {
    /* 0 */ EVENTINF_DOG_RACE_STATE_NOT_STARTED,
    /* 2 */ EVENTINF_DOG_RACE_STATE_STARTED = 2,
    /* 3 */ EVENTINF_DOG_RACE_STATE_ENDED
} EventInfDogRaceState;

#define EVENTINF_DOG_RACE_STATE_MASK 7

#define GET_EVENTINF_DOG_RACE_STATE (gSaveContext.eventInf[0] & EVENTINF_DOG_RACE_STATE_MASK)
#define SET_EVENTINF_DOG_RACE_STATE(state) \
    (gSaveContext.eventInf[0] = ((gSaveContext.eventInf[0] & (u8)~EVENTINF_DOG_RACE_STATE_MASK) | (state)))

#define GET_EVENTINF_DOG_RACE_SELECTED_DOG_INDEX ((gSaveContext.eventInf[0] & (u8)~EVENTINF_DOG_RACE_STATE_MASK) >> 3)
#define SET_EVENTINF_DOG_RACE_SELECTED_DOG_INDEX(selectedDogIndex) \
    (gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & EVENTINF_DOG_RACE_STATE_MASK) | (selectedDogIndex << 3))

// This is exactly the same as the selected dog index; they never need both of them at the same time.
#define GET_EVENTINF_DOG_RACE_RACE_STANDING ((gSaveContext.eventInf[0] & (u8)~EVENTINF_DOG_RACE_STATE_MASK) >> 3)
#define SET_EVENTINF_DOG_RACE_RACE_STANDING(raceStanding) \
    (gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & EVENTINF_DOG_RACE_STATE_MASK) | (raceStanding << 3))

#define EVENTINF_GET_7_E0 ((gSaveContext.eventInf[7] & 0xE0) >> 5)

#define EVENTINF_SET_7_E0(day, temp) \
    (temp) = gSaveContext.eventInf[7] & (u8)~0xE0; \
    (temp) |= (u8)((day) << 5); \
    gSaveContext.eventInf[7] = (temp)

typedef enum {
    // These first 4 values are also used represent the index of the temple the player is currently in
    /* 0 */ DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE,
    /* 1 */ DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE,
    /* 2 */ DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE,
    /* 3 */ DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE,

    /* 4 */ DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE_INVERTED,
    /* 5 */ DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE_BOSS,
    /* 6 */ DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE_BOSS,
    /* 7 */ DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE_BOSS,
    /* 8 */ DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE_BOSS
} DungeonSceneIndex;

#define STRAY_FAIRY_TOTAL 25 // total number of stray fairies, including those already in the Great Fairy Fountain
#define STRAY_FAIRY_SCATTERED_TOTAL 15 // original number of stray fairies in one dungeon area

void Sram_ActivateOwl(u8 owlWarpId);
void Sram_ClearFlagsAtDawnOfTheFirstDay(void);
void Sram_SaveEndOfCycle(struct PlayState* play);
void Sram_IncrementDay(void);
u16 Sram_CalcChecksum(void* data, size_t count);
void Sram_InitNewSave(void);
void Sram_InitDebugSave(void);
void Sram_ResetSaveFromMoonCrash(SramContext* sramCtx);
void Sram_OpenSave(struct FileSelectState* fileSelect, SramContext* sramCtx);
void func_8014546C(SramContext* sramCtx);
void func_801457CC(struct GameState* gameState, SramContext* sramCtx);
void Sram_EraseSave(struct FileSelectState* fileSelect2, SramContext* sramCtx, s32 fileNum);
void Sram_CopySave(struct FileSelectState* fileSelect2, SramContext* sramCtx);
void Sram_InitSave(struct FileSelectState* fileSelect2, SramContext* sramCtx);
void Sram_WriteSaveOptionsToBuffer(SramContext* sramCtx);
void Sram_LoadGlobalOptions(void);
void Sram_InitSram(struct GameState* gameState, SramContext* sramCtx);
void Sram_Alloc(struct GameState* gameState, SramContext* sramCtx);
void Sram_SaveSpecialEnterClockTown(struct PlayState* play);
void Sram_SaveSpecialNewDay(struct PlayState* play);
void Sram_SetFlashPagesDefault(SramContext* sramCtx, u32 curPage, u32 numPages);
void Sram_StartWriteToFlashDefault(SramContext* sramCtx);
void Sram_UpdateWriteToFlashDefault(SramContext* sramCtx);
void Sram_SetFlashPagesOwlSave(SramContext* sramCtx, s32 curPage, s32 numPages);
void Sram_StartWriteToFlashOwlSave(SramContext* sramCtx);
void Sram_UpdateWriteToFlashOwlSave(SramContext* sramCtx);
// #region 2S2H [Port] Moved from z_sram_NES.c to use function in enhancement - Persistent Owl Saves
void func_80147314(SramContext* sramCtx, s32 fileNum); // Removes Owl Saves
// #endregion

void SaveContext_Init(void);

extern u32 gSramSlotOffsets[];
extern u8 gAmmoItems[];
extern s32 gFlashSaveStartPages[];
extern s32 gFlashSaveNumPages[];
extern s32 gFlashSpecialSaveNumPages[];
extern s32 gFlashOwlSaveStartPages[];
extern s32 gFlashOwlSaveNumPages[];

extern SaveContext gSaveContext;

#endif

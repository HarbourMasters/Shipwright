#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "2s2h/BenGui/Notification.h"
#include "2s2h/SaveManager/SaveManager.h"
#include <ship/utils/binarytools/BinaryReader.h>
#include <string>
#include <spdlog/spdlog.h>
#include <ship/Context.h>
#include <ship/window/Window.h>

extern "C" {
#include "z64math.h"
#include "macros.h"
#include "src/overlays/gamestates/ovl_file_choose/z_file_select.h"
extern FileSelectState* gFileSelectState;
}

using json = nlohmann::json;

// WOA WOA WOA! What are you doing here?!
// The structs in this file should never be modified.

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_ItemEquips {
    /* 0x00 */ u8 buttonItems[4][4];  // "register_item"
    /* 0x10 */ u8 cButtonSlots[4][4]; // "register_item_pt"
    /* 0x20 */ u16 equipment;
} Legacy_ItemEquips; // size = 0x22

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_Inventory {
    /* 0x00 */ u8 items[48];        // "item_register", first 24 elements are normal items and the other 24 are masks
    /* 0x30 */ s8 ammo[24];         // "item_count"
    /* 0x48 */ u32 upgrades;        // "non_equip_register" some bits are wallet upgrades
    /* 0x4C */ u32 questItems;      // "collect_register"
    /* 0x50 */ u8 dungeonItems[10]; // "key_compass_map"
    /* 0x5A */ s8 dungeonKeys[9];   // "key_register"
    /* 0x63 */ s8 defenseHearts;
    /* 0x64 */ s8 strayFairies[10];                 // "orange_fairy"
    /* 0x6E */ char dekuPlaygroundPlayerName[3][8]; // "degnuts_memory_name" Stores playerName (8 char) over (3 days)
                                                    // when getting a new high score
} Legacy_Inventory;                                 // size = 0x88

typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
} Legacy_Vec3s; // size = 0x6

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_HorseData {
    /* 0x0 */ s16 sceneId;      // "spot_no"
    /* 0x2 */ Legacy_Vec3s pos; // "horse_x", "horse_y" and "horse_z"
    /* 0x8 */ s16 yaw;          // "horse_a"
} Legacy_HorseData;             // size = 0xA

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_RespawnData {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
    /* 0x0E */ s16 playerParams;
    /* 0x10 */ u16 entrance;
    /* 0x12 */ u8 roomIndex;
    /* 0x13 */ s8 data;
    /* 0x14 */ u32 tempSwitchFlags;
    /* 0x18 */ u32 unk_18;
    /* 0x1C */ u32 tempCollectFlags;
} Legacy_RespawnData; // size = 0x20

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_PermanentSceneFlags {
    /* 0x00 */ u32 chest;
    /* 0x04 */ u32 switch0;
    /* 0x08 */ u32 switch1;
    /* 0x0C */ u32 clearedRoom;
    /* 0x10 */ u32 collectible;
    /* 0x14 */ u32 unk_14; // varies based on scene. For dungeons, floors visited.
    /* 0x18 */ u32 rooms;
} Legacy_PermanentSceneFlags; // size = 0x1C

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_CycleSceneFlags {
    /* 0x00 */ u32 chest;
    /* 0x04 */ u32 switch0;
    /* 0x08 */ u32 switch1;
    /* 0x0C */ u32 clearedRoom;
    /* 0x10 */ u32 collectible;
} Legacy_CycleSceneFlags; // size = 0x14

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_SaveOptions {
    /* 0x0 */ u16 optionId;       // "option_id"
    /* 0x2 */ u8 language;        // "j_n"
    /* 0x3 */ u8 audioSetting;    // "s_sound"
    /* 0x4 */ u8 languageSetting; // "language"
    /* 0x5 */ u8 zTargetSetting;  // "z_attention", 0: Switch; 1: Hold
} Legacy_SaveOptions;             // size = 0x6

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_SavePlayerData {
    /* 0x00 */ char newf[6];             // "newf"               Will always be "ZELDA3 for a valid save
    /* 0x06 */ u16 threeDayResetCount;   // "savect"
    /* 0x08 */ char playerName[8];       // "player_name"
    /* 0x10 */ s16 healthCapacity;       // "max_life"
    /* 0x12 */ s16 health;               // "now_life"
    /* 0x14 */ s8 magicLevel;            // 0 for no magic/new load, 1 for magic, 2 for double magic "magic_max"
    /* 0x15 */ s8 magic;                 // current magic available for use "magic_now"
    /* 0x16 */ s16 rupees;               // "lupy_count"
    /* 0x18 */ u16 swordHealth;          // "long_sword_hp"
    /* 0x1A */ u16 tatlTimer;            // "navi_timer"
    /* 0x1C */ u8 isMagicAcquired;       // "magic_mode"
    /* 0x1D */ u8 isDoubleMagicAcquired; // "magic_ability"
    /* 0x1E */ u8 doubleDefense;         // "life_ability"
    /* 0x1F */ u8 unk_1F;                // "ocarina_round"
    /* 0x20 */ u8 unk_20;                // "first_memory"
    /* 0x22 */ u16 owlActivationFlags;   // "memory_warp_point"
    /* 0x24 */ u8 unk_24;                // "last_warp_pt"
    /* 0x26 */ s16 savedSceneId;         // "scene_data_ID"
} Legacy_SavePlayerData;                 // size = 0x28

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_SaveInfo {
    /* 0x000 */ Legacy_SavePlayerData playerData;
    /* 0x028 */ Legacy_ItemEquips equips;
    /* 0x04C */ Legacy_Inventory inventory;
    /* 0x0D4 */ Legacy_PermanentSceneFlags permanentSceneFlags[120];
    /* 0xDF4 */ s8 unk_DF4[0x54];
    /* 0xE48 */ u32 dekuPlaygroundHighScores[3];
    /* 0xE54 */ u32 pictoFlags0; // Flags set by `PictoActor`s if pictograph is valid
    /* 0xE58 */ u32 pictoFlags1; // Flags set by Snap_ValidatePictograph() to record errors; volatile since that
                                 // function is run many times in succession
    /* 0xE5C */ u32 unk_E5C;
    /* 0xE60 */ u32 unk_E60;
    /* 0xE64 */ u32 unk_E64[7];       // Invadepoh flags
    /* 0xE80 */ u32 scenesVisible[7]; // tingle maps and clouded regions on pause map. Stores scenes bitwise for up to
                                      // 224 scenes even though there are not that many scenes
    /* 0xE9C */ u32 skullTokenCount;  // upper 16 bits store Swamp skulls, lower 16 bits store Ocean skulls
    /* 0xEA0 */ u32 unk_EA0;          // Gossic stone heart piece flags
    /* 0xEA4 */ u32 unk_EA4;
    /* 0xEA8 */ u32 unk_EA8[2];  // Related to blue warps
    /* 0xEB0 */ u32 stolenItems; // Items stolen by Takkuri and given to Curiosity Shop Man
    /* 0xEB4 */ u32 unk_EB4;
    /* 0xEB8 */ u32 highScores[7];
    /* 0xED4 */ u8 weekEventReg[100];        // "week_event_reg"
    /* 0xF38 */ u32 regionsVisited;          // "area_arrival"
    /* 0xF3C */ u32 worldMapCloudVisibility; // "cloud_clear"
    /* 0xF40 */ u8 unk_F40;                  // "oca_rec_flag"                   has scarecrows song
    /* 0xF41 */ u8 scarecrowSpawnSongSet;    // "oca_rec_flag8"
    /* 0xF42 */ u8 scarecrowSpawnSong[128];
    /* 0xFC2 */ s8 bombersCaughtNum;        // "aikotoba_index"
    /* 0xFC3 */ s8 bombersCaughtOrder[5];   // "aikotoba_table"
    /* 0xFC8 */ s8 lotteryCodes[3][3];      // "numbers_table", Preset lottery codes
    /* 0xFD1 */ s8 spiderHouseMaskOrder[6]; // "kinsta_color_table"
    /* 0xFD7 */ s8 bomberCode[5];           // "bombers_aikotoba_table"
    /* 0xFDC */ Legacy_HorseData horseData;
    /* 0xFE6 */ u16 checksum; // "check_sum"
} Legacy_SaveInfo;            // size = 0xFE8

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_Save {
    /* 0x00 */ s32 entrance;    // "scene_no"
    /* 0x04 */ u8 equippedMask; // "player_mask"
    /* 0x05 */ u8 isFirstCycle; // "opening_flag"
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 linkAge;        // "link_age"
    /* 0x08 */ s32 cutsceneIndex; // "day_time"
    /* 0x0C */ u16 time;          // "zelda_time"
    /* 0x0E */ u16 owlSaveLocation;
    /* 0x10 */ s32 isNight;         // "asahiru_fg"
    /* 0x14 */ s32 timeSpeedOffset; // "change_zelda_time"
    /* 0x18 */ s32 day;             // "totalday"
    /* 0x1C */ s32 eventDayCount;   // "eventday"
    /* 0x20 */ u8 playerForm;       // "player_character"
    /* 0x21 */ u8 snowheadCleared;  // "spring_flag"
    /* 0x22 */ u8 hasTatl;          // "bell_flag"
    /* 0x23 */ u8 isOwlSave;
    /* 0x24 */ Legacy_SaveInfo saveInfo;
} Legacy_Save; // size = 0x100C

//
//   DO   NOT   MODIFY   THIS   FILE!
//
typedef struct Legacy_SaveContext {
    /* 0x0000 */ Legacy_Save save;
    /* 0x100C */ u8 eventInf[8]; // "event_inf"
    /* 0x1014 */ u8 unk_1014;    // "stone_set_flag"
    /* 0x1015 */ u8 bButtonStatus;
    /* 0x1016 */ u16 jinxTimer;
    /* 0x1018 */ s16 rupeeAccumulator;              // "lupy_udct"
    /* 0x101A */ u8 bottleTimerStates[6];           // See the `BottleTimerState` enum. "bottle_status"
    /* 0x1020 */ OSTime bottleTimerStartOsTimes[6]; // The osTime when the timer starts. "bottle_ostime"
    /* 0x1050 */ u64 bottleTimerTimeLimits[6];      // The original total time given before the timer expires, in
                                                    // centiseconds (1/100th sec). "bottle_sub"
    /* 0x1080 */ u64 bottleTimerCurTimes[6];        // The remaining time left before the timer expires, in centiseconds
                                                    // (1/100th sec). "bottle_time"
    /* 0x10B0 */ OSTime
        bottleTimerPausedOsTimes[6]; // The cumulative osTime spent with the timer paused. "bottle_stop_time"
    /* 0x10E0 */ u8
        pictoPhotoI5[((160 * 112) * 5 / 8)]; // buffer containing the pictograph photo, compressed to I5 from I8
    /* 0x3CA0 */ s32 fileNum;                // "file_no"
    /* 0x3CA4 */ s16 powderKegTimer;         // "big_bom_timer"
    /* 0x3CA6 */ u8 unk_3CA6;
    /* 0x3CA7 */ u8 unk_3CA7;                   // "day_night_flag"
    /* 0x3CA8 */ s32 gameMode;                  // "mode"
    /* 0x3CAC */ s32 sceneLayer;                // "counter"
    /* 0x3CB0 */ s32 respawnFlag;               // "restart_flag"
    /* 0x3CB4 */ Legacy_RespawnData respawn[8]; // "restart_data"
    /* 0x3DB4 */ f32 entranceSpeed;             // "player_wipe_speedF"
    /* 0x3DB8 */ u16 entranceSound;             // "player_wipe_door_SE"
    /* 0x3DBA */ u8 unk_3DBA;                   // "player_wipe_item"
    /* 0x3DBB */ u8 retainWeatherMode;          // "next_walk"
    /* 0x3DBC */ s16 dogParams;                 // OoT leftover. "dog_flag"
    /* 0x3DBE */ u8 envHazardTextTriggerFlags;  // "guide_status"
    /* 0x3DBF */ u8 showTitleCard;              // "name_display"
    /* 0x3DC0 */ s16 nayrusLoveTimer;           // remnant of OoT, "shield_magic_timer"
    /* 0x3DC2 */ u8 unk_3DC2;                   // "pad1"
    /* 0x3DC8 */ OSTime postmanTimerStopOsTime; // The osTime when the timer stops for the postman minigame. "get_time"
    /* 0x3DD0 */ u8 timerStates[7];             // See the `TimerState` enum. "event_fg"
    /* 0x3DD7 */ u8 timerDirections[7];         // See the `TimerDirection` enum. "calc_flag"
    /* 0x3DE0 */ u64 timerCurTimes[7]; // For countdown, the remaining time left. For countup, the time since the start.
                                       // In centiseconds (1/100th sec). "event_ostime"
    /* 0x3E18 */ u64 timerTimeLimits[7]; // The original total time given for the timer to count from, in centiseconds
                                         // (1/100th sec). "event_sub"
    /* 0x3E50 */ OSTime timerStartOsTimes[7]; // The osTime when the timer starts. "func_time"
    /* 0x3E88 */ u64 timerStopTimes[7]; // The total amount of time taken between the start and end of the timer, in
                                        // centiseconds (1/100th sec). "func_end_time"
    /* 0x3EC0 */ OSTime timerPausedOsTimes[7]; // The cumulative osTime spent with the timer paused. "func_stop_time"
    /* 0x3EF8 */ s16 timerX[7];                // "event_xp"
    /* 0x3F06 */ s16 timerY[7];                // "event_yp"
    /* 0x3F14 */ s16 unk_3F14;                 // "character_change"
    /* 0x3F16 */ u8 seqId;                     // "old_bgm"
    /* 0x3F17 */ u8 ambienceId;                // "old_env"
    /* 0x3F18 */ u8 buttonStatus[6];           // "button_item"
    /* 0x3F1E */ u8 hudVisibilityForceButtonAlphasByStatus; // if btn alphas are updated through
                                                            // Interface_UpdateButtonAlphas, instead update them through
                                                            // Interface_UpdateButtonAlphasByStatus "ck_fg"
    /* 0x3F20 */ u16 nextHudVisibility;  // triggers the hud to change visibility to the requested value. Reset to
                                         // HUD_VISIBILITY_IDLE when target is reached "alpha_type"
    /* 0x3F22 */ u16 hudVisibility;      // current hud visibility "prev_alpha_type"
    /* 0x3F24 */ u16 hudVisibilityTimer; // number of frames in the transition to a new hud visibility. Used to step
                                         // alpha "alpha_count"
    /* 0x3F26 */ u16
        prevHudVisibility; // used to store and recover hud visibility for pause menu and text boxes "last_time_type"
    /* 0x3F28 */ s16 magicState;          // determines magic meter behavior on each frame "magic_flag"
    /* 0x3F2A */ s16 isMagicRequested;    // a request to add magic has been given "recovery_magic_flag"
    /* 0x3F2C */ s16 magicFlag;           // Set to 0 in func_80812D94(), otherwise unused "keep_magic_flag"
    /* 0x3F2E */ s16 magicCapacity;       // maximum magic available "magic_now_max"
    /* 0x3F30 */ s16 magicFillTarget;     // target used to fill magic "magic_now_now"
    /* 0x3F32 */ s16 magicToConsume;      // accumulated magic that is requested to be consumed "magic_used"
    /* 0x3F34 */ s16 magicToAdd;          // accumulated magic that is requested to be added "magic_recovery"
    /* 0x3F36 */ u16 mapIndex;            // "scene_ID"
    /* 0x3F38 */ u16 minigameStatus;      // "yabusame_mode"
    /* 0x3F3A */ u16 minigameScore;       // "yabusame_total"
    /* 0x3F3C */ u16 minigameHiddenScore; // "yabusame_out_ct"
    /* 0x3F3E */ u8 unk_3F3E;             // "no_save"
    /* 0x3F3F */ u8 flashSaveAvailable;   // "flash_flag"
    /* 0x3F40 */ Legacy_SaveOptions options;
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
    /* 0x3F68 */ Legacy_CycleSceneFlags
        cycleSceneFlags[120];      // Scene flags that are temporarily stored over the duration of a single 3-day cycle
    /* 0x48C8 */ u16 dungeonIndex; // "scene_id_mix"
    /* 0x48CA */ u8 masksGivenOnMoon[27]; // bit-packed, masks given away on the Moon. "mask_mask_bit"
} Legacy_SaveContext;                     // size = 0x48C8

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_ItemEquips& itemEquips) {
    j = json{
        { "buttonItems", itemEquips.buttonItems },
        { "cButtonSlots", itemEquips.cButtonSlots },
        { "equipment", itemEquips.equipment },
    };
}

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_Inventory& inventory) {
    // Setup and copy u8 arrays to avoid json treating char[] as strings
    // These char[] are not null-terminated, so saving as strings causes overflow/corruption
    uint8_t dekuPlaygroundPlayerName[3][8];
    memcpy(dekuPlaygroundPlayerName, inventory.dekuPlaygroundPlayerName, sizeof(dekuPlaygroundPlayerName));

    j = json{
        { "items", inventory.items },
        { "ammo", inventory.ammo },
        { "upgrades", inventory.upgrades },
        { "questItems", inventory.questItems },
        { "dungeonItems", inventory.dungeonItems },
        { "dungeonKeys", inventory.dungeonKeys },
        { "defenseHearts", inventory.defenseHearts },
        { "strayFairies", inventory.strayFairies },
        { "dekuPlaygroundPlayerName", dekuPlaygroundPlayerName },
    };
}

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_PermanentSceneFlags& permanentSceneFlags) {
    j = json{
        { "chest", permanentSceneFlags.chest },
        { "switch0", permanentSceneFlags.switch0 },
        { "switch1", permanentSceneFlags.switch1 },
        { "clearedRoom", permanentSceneFlags.clearedRoom },
        { "collectible", permanentSceneFlags.collectible },
        { "unk_14", permanentSceneFlags.unk_14 },
        { "rooms", permanentSceneFlags.rooms },
    };
}

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_SavePlayerData& savePlayerData) {
    // Setup and copy u8 arrays to avoid json treating char[] as strings
    // These char[] are not null-terminated, so saving as strings causes overflow/corruption
    u8 newf[6];
    u8 playerName[8];
    memcpy(newf, savePlayerData.newf, sizeof(newf));
    memcpy(playerName, savePlayerData.playerName, sizeof(playerName));

    j = json{
        { "newf", newf },
        { "threeDayResetCount", savePlayerData.threeDayResetCount },
        { "playerName", playerName },
        { "healthCapacity", savePlayerData.healthCapacity },
        { "health", savePlayerData.health },
        { "magicLevel", savePlayerData.magicLevel },
        { "magic", savePlayerData.magic },
        { "rupees", savePlayerData.rupees },
        { "swordHealth", savePlayerData.swordHealth },
        { "tatlTimer", savePlayerData.tatlTimer },
        { "isMagicAcquired", savePlayerData.isMagicAcquired },
        { "isDoubleMagicAcquired", savePlayerData.isDoubleMagicAcquired },
        { "doubleDefense", savePlayerData.doubleDefense },
        { "unk_1F", savePlayerData.unk_1F },
        { "unk_20", savePlayerData.unk_20 },
        { "owlActivationFlags", savePlayerData.owlActivationFlags },
        { "unk_24", savePlayerData.unk_24 },
        { "savedSceneId", savePlayerData.savedSceneId },
    };
}

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_Vec3s& vec) {
    j = json{
        { "x", vec.x },
        { "y", vec.y },
        { "z", vec.z },
    };
}

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_HorseData& horseData) {
    j = json{
        { "sceneId", horseData.sceneId },
        { "pos", horseData.pos },
        { "yaw", horseData.yaw },
    };
}

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_SaveInfo& saveInfo) {
    j = json{
        { "playerData", saveInfo.playerData },
        { "equips", saveInfo.equips },
        { "inventory", saveInfo.inventory },
        { "permanentSceneFlags", saveInfo.permanentSceneFlags },
        { "unk_DF4", saveInfo.unk_DF4 },
        { "dekuPlaygroundHighScores", saveInfo.dekuPlaygroundHighScores },
        { "pictoFlags0", saveInfo.pictoFlags0 },
        { "pictoFlags1", saveInfo.pictoFlags1 },
        { "unk_E5C", saveInfo.unk_E5C },
        { "unk_E60", saveInfo.unk_E60 },
        { "unk_E64", saveInfo.unk_E64 },
        { "scenesVisible", saveInfo.scenesVisible },
        { "skullTokenCount", saveInfo.skullTokenCount },
        { "unk_EA0", saveInfo.unk_EA0 },
        { "unk_EA4", saveInfo.unk_EA4 },
        { "unk_EA8", saveInfo.unk_EA8 },
        { "stolenItems", saveInfo.stolenItems },
        { "unk_EB4", saveInfo.unk_EB4 },
        { "highScores", saveInfo.highScores },
        { "weekEventReg", saveInfo.weekEventReg },
        { "regionsVisited", saveInfo.regionsVisited },
        { "worldMapCloudVisibility", saveInfo.worldMapCloudVisibility },
        { "unk_F40", saveInfo.unk_F40 },
        { "scarecrowSpawnSongSet", saveInfo.scarecrowSpawnSongSet },
        { "scarecrowSpawnSong", saveInfo.scarecrowSpawnSong },
        { "bombersCaughtNum", saveInfo.bombersCaughtNum },
        { "bombersCaughtOrder", saveInfo.bombersCaughtOrder },
        { "lotteryCodes", saveInfo.lotteryCodes },
        { "spiderHouseMaskOrder", saveInfo.spiderHouseMaskOrder },
        { "bomberCode", saveInfo.bomberCode },
        { "horseData", saveInfo.horseData },
        { "checksum", saveInfo.checksum },
    };
}

//
//   DO   NOT   MODIFY   THIS   FILE!
//
void to_json(json& j, const Legacy_Save& save) {
    j = json{
        { "entrance", save.entrance },
        { "equippedMask", save.equippedMask },
        { "isFirstCycle", save.isFirstCycle },
        { "unk_06", save.unk_06 },
        { "linkAge", save.linkAge },
        { "cutsceneIndex", save.cutsceneIndex },
        { "time", save.time },
        { "owlSaveLocation", save.owlSaveLocation },
        { "isNight", save.isNight },
        { "timeSpeedOffset", save.timeSpeedOffset },
        { "day", save.day },
        { "eventDayCount", save.eventDayCount },
        { "playerForm", save.playerForm },
        { "snowheadCleared", save.snowheadCleared },
        { "hasTatl", save.hasTatl },
        { "isOwlSave", save.isOwlSave },
        { "saveInfo", save.saveInfo },
    };
}

void to_json(json& j, const Legacy_SaveContext& saveContext) {
    j = json{
        { "save", saveContext.save },
        { "eventInf", saveContext.eventInf },
        { "unk_1014", saveContext.unk_1014 },
        { "bButtonStatus", saveContext.bButtonStatus },
        { "jinxTimer", saveContext.jinxTimer },
        { "rupeeAccumulator", saveContext.rupeeAccumulator },
        { "bottleTimerStates", saveContext.bottleTimerStates },
        { "bottleTimerStartOsTimes", saveContext.bottleTimerStartOsTimes },
        { "bottleTimerTimeLimits", saveContext.bottleTimerTimeLimits },
        { "bottleTimerCurTimes", saveContext.bottleTimerCurTimes },
        { "bottleTimerPausedOsTimes", saveContext.bottleTimerPausedOsTimes },
        { "pictoPhotoI5", saveContext.pictoPhotoI5 },
    };
}

void BinarySaveConverter_ReadBufferToSave(Legacy_SaveContext* saveContext, std::shared_ptr<Ship::BinaryReader> reader) {
    // Save
    saveContext->save.entrance = reader->ReadInt32();
    saveContext->save.equippedMask = reader->ReadUByte();
    saveContext->save.isFirstCycle = reader->ReadUByte();
    saveContext->save.unk_06 = reader->ReadUByte();
    saveContext->save.linkAge = reader->ReadUByte();
    saveContext->save.cutsceneIndex = reader->ReadInt32();
    saveContext->save.time = reader->ReadUInt16();
    saveContext->save.owlSaveLocation = reader->ReadUInt16();
    saveContext->save.isNight = reader->ReadInt32();
    saveContext->save.timeSpeedOffset = reader->ReadInt32();
    saveContext->save.day = reader->ReadInt32();
    saveContext->save.eventDayCount = reader->ReadInt32();
    saveContext->save.playerForm = reader->ReadUByte();
    saveContext->save.snowheadCleared = reader->ReadUByte();
    saveContext->save.hasTatl = reader->ReadUByte();
    saveContext->save.isOwlSave = reader->ReadUByte();

    // Save.SaveInfo.SavePlayerData
    reader->Read(saveContext->save.saveInfo.playerData.newf, sizeof(saveContext->save.saveInfo.playerData.newf));
    saveContext->save.saveInfo.playerData.threeDayResetCount = reader->ReadUInt16();
    reader->Read(saveContext->save.saveInfo.playerData.playerName,
                 sizeof(saveContext->save.saveInfo.playerData.playerName));
    saveContext->save.saveInfo.playerData.healthCapacity = reader->ReadInt16();
    saveContext->save.saveInfo.playerData.health = reader->ReadInt16();
    saveContext->save.saveInfo.playerData.magicLevel = reader->ReadInt8();
    saveContext->save.saveInfo.playerData.magic = reader->ReadInt8();
    saveContext->save.saveInfo.playerData.rupees = reader->ReadInt16();
    saveContext->save.saveInfo.playerData.swordHealth = reader->ReadUInt16();
    saveContext->save.saveInfo.playerData.tatlTimer = reader->ReadUInt16();
    saveContext->save.saveInfo.playerData.isMagicAcquired = reader->ReadUByte();
    saveContext->save.saveInfo.playerData.isDoubleMagicAcquired = reader->ReadUByte();
    saveContext->save.saveInfo.playerData.doubleDefense = reader->ReadUByte();
    saveContext->save.saveInfo.playerData.unk_1F = reader->ReadUByte();
    saveContext->save.saveInfo.playerData.unk_20 = reader->ReadUByte();
    reader->ReadUByte(); // Align
    saveContext->save.saveInfo.playerData.owlActivationFlags = reader->ReadUInt16();
    saveContext->save.saveInfo.playerData.unk_24 = reader->ReadUByte();
    reader->ReadUByte(); // Align
    saveContext->save.saveInfo.playerData.savedSceneId = reader->ReadInt16();

    // Save.SaveInfo.ItemEquips
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            saveContext->save.saveInfo.equips.buttonItems[i][j] = reader->ReadUByte();
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            saveContext->save.saveInfo.equips.cButtonSlots[i][j] = reader->ReadUByte();
        }
    }
    saveContext->save.saveInfo.equips.equipment = reader->ReadUInt16();

    reader->ReadInt16(); // Align

    // Save.SaveInfo.Inventory
    for (int i = 0; i < ARRAY_COUNT(saveContext->save.saveInfo.inventory.items); i++) {
        saveContext->save.saveInfo.inventory.items[i] = reader->ReadUByte();
    }
    for (int i = 0; i < ARRAY_COUNT(saveContext->save.saveInfo.inventory.ammo); i++) {
        saveContext->save.saveInfo.inventory.ammo[i] = reader->ReadInt8();
    }
    saveContext->save.saveInfo.inventory.upgrades = reader->ReadUInt32();
    saveContext->save.saveInfo.inventory.questItems = reader->ReadUInt32();
    for (int i = 0; i < ARRAY_COUNT(saveContext->save.saveInfo.inventory.dungeonItems); i++) {
        saveContext->save.saveInfo.inventory.dungeonItems[i] = reader->ReadUByte();
    }
    for (int i = 0; i < ARRAY_COUNT(saveContext->save.saveInfo.inventory.dungeonKeys); i++) {
        saveContext->save.saveInfo.inventory.dungeonKeys[i] = reader->ReadInt8();
    }
    saveContext->save.saveInfo.inventory.defenseHearts = reader->ReadInt8();
    for (int i = 0; i < ARRAY_COUNT(saveContext->save.saveInfo.inventory.strayFairies); i++) {
        saveContext->save.saveInfo.inventory.strayFairies[i] = reader->ReadInt8();
    }
    for (int i = 0; i < ARRAY_COUNT(saveContext->save.saveInfo.inventory.dekuPlaygroundPlayerName); i++) {
        reader->Read(saveContext->save.saveInfo.inventory.dekuPlaygroundPlayerName[i],
                     sizeof(saveContext->save.saveInfo.inventory.dekuPlaygroundPlayerName[i]));
    }

    reader->ReadInt16(); // Align

    // Save.SaveInfo
    for (int i = 0; i < 120; i++) {
        saveContext->save.saveInfo.permanentSceneFlags[i].chest = reader->ReadUInt32();
        saveContext->save.saveInfo.permanentSceneFlags[i].switch0 = reader->ReadUInt32();
        saveContext->save.saveInfo.permanentSceneFlags[i].switch1 = reader->ReadUInt32();
        saveContext->save.saveInfo.permanentSceneFlags[i].clearedRoom = reader->ReadUInt32();
        saveContext->save.saveInfo.permanentSceneFlags[i].collectible = reader->ReadUInt32();
        saveContext->save.saveInfo.permanentSceneFlags[i].unk_14 = reader->ReadUInt32();
        saveContext->save.saveInfo.permanentSceneFlags[i].rooms = reader->ReadUInt32();
    }
    for (int i = 0; i < 0x54; i++) {
        saveContext->save.saveInfo.unk_DF4[i] = reader->ReadInt8();
    }
    for (int i = 0; i < 3; i++) {
        saveContext->save.saveInfo.dekuPlaygroundHighScores[i] = reader->ReadUInt32();
    }
    saveContext->save.saveInfo.pictoFlags0 = reader->ReadUInt32();
    saveContext->save.saveInfo.pictoFlags1 = reader->ReadUInt32();
    saveContext->save.saveInfo.unk_E5C = reader->ReadUInt32();
    saveContext->save.saveInfo.unk_E60 = reader->ReadUInt32();
    for (int i = 0; i < 7; i++) {
        saveContext->save.saveInfo.unk_E64[i] = reader->ReadUInt32();
    }
    for (int i = 0; i < 7; i++) {
        saveContext->save.saveInfo.scenesVisible[i] = reader->ReadUInt32();
    }
    saveContext->save.saveInfo.skullTokenCount = reader->ReadUInt32();
    saveContext->save.saveInfo.unk_EA0 = reader->ReadUInt32();
    saveContext->save.saveInfo.unk_EA4 = reader->ReadUInt32();
    for (int i = 0; i < 2; i++) {
        saveContext->save.saveInfo.unk_EA8[i] = reader->ReadUInt32();
    }
    saveContext->save.saveInfo.stolenItems = reader->ReadUInt32();
    saveContext->save.saveInfo.unk_EB4 = reader->ReadUInt32();
    for (int i = 0; i < HS_MAX; i++) {
        saveContext->save.saveInfo.highScores[i] = reader->ReadUInt32();
    }
    for (int i = 0; i < 100; i++) {
        saveContext->save.saveInfo.weekEventReg[i] = reader->ReadUByte();
    }
    saveContext->save.saveInfo.regionsVisited = reader->ReadUInt32();
    saveContext->save.saveInfo.worldMapCloudVisibility = reader->ReadUInt32();
    saveContext->save.saveInfo.unk_F40 = reader->ReadUByte();
    saveContext->save.saveInfo.scarecrowSpawnSongSet = reader->ReadUByte();
    for (int i = 0; i < 128; i++) {
        saveContext->save.saveInfo.scarecrowSpawnSong[i] = reader->ReadUByte();
    }
    saveContext->save.saveInfo.bombersCaughtNum = reader->ReadInt8();
    for (int i = 0; i < 5; i++) {
        saveContext->save.saveInfo.bombersCaughtOrder[i] = reader->ReadInt8();
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            saveContext->save.saveInfo.lotteryCodes[i][j] = reader->ReadInt8();
        }
    }
    for (int i = 0; i < 6; i++) {
        saveContext->save.saveInfo.spiderHouseMaskOrder[i] = reader->ReadInt8();
    }
    for (int i = 0; i < 5; i++) {
        saveContext->save.saveInfo.bomberCode[i] = reader->ReadInt8();
    }

    // Save.SaveInfo.HorseData
    saveContext->save.saveInfo.horseData.sceneId = reader->ReadInt16();
    saveContext->save.saveInfo.horseData.pos.x = reader->ReadInt16();
    saveContext->save.saveInfo.horseData.pos.y = reader->ReadInt16();
    saveContext->save.saveInfo.horseData.pos.z = reader->ReadInt16();
    saveContext->save.saveInfo.horseData.yaw = reader->ReadInt16();

    saveContext->save.saveInfo.checksum = 1;
}

bool BinarySaveConverter_HandleFileDropped(char* filePath) {
    try {
        std::ifstream fileStream(filePath, std::ios::binary | std::ios::ate);

        if (!fileStream.is_open()) {
            return false;
        }

        std::streamsize size = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!fileStream.read(buffer.data(), size)) {
            return false;
        }

        std::string sequence = "ZELD";
        std::string swappedSequence = "DLEZ";
        std::vector<size_t> saveOffsets = {};

        for (size_t i = 0; i <= size - sequence.size(); ++i) {
            if (std::equal(sequence.begin(), sequence.end(), buffer.begin() + i)) {
                saveOffsets.push_back(i);
            } else if (std::equal(swappedSequence.begin(), swappedSequence.end(), buffer.begin() + i)) {
                // Swap the entire file and start over
                std::vector<char> byteSwapBuffer(4);
                for (size_t i = 0; i < buffer.size(); i += 4) {
                    byteSwapBuffer[0] = buffer[i + 3];
                    byteSwapBuffer[1] = buffer[i + 2];
                    byteSwapBuffer[2] = buffer[i + 1];
                    byteSwapBuffer[3] = buffer[i];
                    memcpy(&buffer[i], byteSwapBuffer.data(), 4);
                }
                i = 0;
            }
        }

        if (saveOffsets.size() == 0) {
            SPDLOG_DEBUG("Not a valid binary save file");
            return false;
        }

        int saveSlot = SaveManager_GetOpenFileSlot();
        if (saveSlot == -1) {
            SPDLOG_ERROR("No save slot available");
            Notification::Emit({ .message = "No save slot available" });
            return true;
        }

        Legacy_SaveContext saveContext;
        auto reader = std::make_shared<Ship::BinaryReader>(buffer.data(), buffer.size());
        reader->SetEndianness(Ship::Endianness::Big);

        // Seek to front of first available save
        reader->Seek(saveOffsets[0] - 0x24, Ship::SeekOffsetType::Start);
        BinarySaveConverter_ReadBufferToSave(&saveContext, reader);
        nlohmann::json j;
        std::string fileName = SaveManager_GetFileName(saveSlot);

        j["newCycleSave"]["save"] = saveContext.save;
        j["type"] = "2S2H_SAVE";
        j["version"] = 4;

        // Based on where the save was found, determine if the two possible owl save offsets are in the list of save
        // offsets. If they are, we can assume that the save is the owl save associated with the other file above
        bool foundOwlSave = false;
        if (std::find(saveOffsets.begin(), saveOffsets.end(), saveOffsets[0] + 32768) != saveOffsets.end()) {
            foundOwlSave = true;
            reader->Seek(saveOffsets[0] + 32768 - 0x24, Ship::SeekOffsetType::Start);
        } else if (std::find(saveOffsets.begin(), saveOffsets.end(), saveOffsets[0] + 49152) != saveOffsets.end()) {
            foundOwlSave = true;
            reader->Seek(saveOffsets[0] + 49152 - 0x24, Ship::SeekOffsetType::Start);
        }

        if (foundOwlSave) {
            memset(&saveContext, 0, sizeof(Legacy_SaveContext));
            BinarySaveConverter_ReadBufferToSave(&saveContext, reader);

            j["owlSave"] = saveContext;
        }

        SaveManager_WriteSaveFile(fileName, j);

        // Reset the file select state to reload the save metadata
        if (gFileSelectState != NULL) {
            STOP_GAMESTATE(&gFileSelectState->state);
            SET_NEXT_GAMESTATE(&gFileSelectState->state, FileSelect_Init, sizeof(FileSelectState));
        }

        SPDLOG_INFO("Successfully imported save into slot {}", saveSlot);
        Notification::Emit({ .message = "Successfully imported save into slot", .suffix = std::to_string(saveSlot) });

        return true;
    } catch (std::exception& e) {
        SPDLOG_ERROR("Failed to load file: {}", e.what());
        Notification::Emit({ .message = "Failed to load file" });
        return false;
    } catch (...) {
        SPDLOG_ERROR("Failed to load file");
        Notification::Emit({ .message = "Failed to load file" });
        return false;
    }
}

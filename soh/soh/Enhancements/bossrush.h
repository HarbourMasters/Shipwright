#pragma once

#include "variables.h"

#define BOSSRUSH_OPTIONS_AMOUNT 11
#define BOSSRUSH_MAX_OPTIONS_ON_SCREEN 6

typedef enum {
    BR_OPTIONS_BOSSES,
    BR_OPTIONS_HEARTS,
    BR_OPTIONS_AMMO,
    BR_OPTIONS_HEAL,
    BR_OPTIONS_HYPERBOSSES,
    BR_OPTIONS_MAGIC,
    BR_OPTIONS_BGS,
    BR_OPTIONS_BOTTLE,
    BR_OPTIONS_LONGSHOT,
    BR_OPTIONS_HOVERBOOTS,
    BR_OPTIONS_TIMER
} BossRushOptionEnums;

typedef enum {
    BR_CHOICE_BOSSES_ALL,
    BR_CHOICE_BOSSES_CHILD,
    BR_CHOICE_BOSSES_ADULT,
    BR_CHOICE_BOSSES_GANONDORF_GANON
} BossRushBossesChoices;

typedef enum {
    BR_CHOICE_HEARTS_10,
    BR_CHOICE_HEARTS_15,
    BR_CHOICE_HEARTS_20,
    BR_CHOICE_HEARTS_3,
    BR_CHOICE_HEARTS_5,
    BR_CHOICE_HEARTS_7
} BossRushHeartsChoices;

typedef enum {
    BR_CHOICE_AMMO_LIMITED,
    BR_CHOICE_AMMO_FULL,
    BR_CHOICE_AMMO_MAXED
} BossRushAmmoChoices;

typedef enum {
    BR_CHOICE_HEAL_GANONDORF,
    BR_CHOICE_HEAL_EVERYBOSS,
    BR_CHOICE_HEAL_NEVER
} BossRushHealChoices;

typedef enum {
    BR_CHOICE_HYPERBOSSES_NO,
    BR_CHOICE_HYPERBOSSES_YES
} BossRushHyperBossesChoices;

typedef enum {
    BR_CHOICE_MAGIC_SINGLE,
    BR_CHOICE_MAGIC_DOUBLE
} BossRushMagicChoices;

typedef enum {
    BR_CHOICE_BGS_NO,
    BR_CHOICE_BGS_YES
} BossRushBgsChoices;

typedef enum {
    BR_CHOICE_BOTTLE_NO,
    BR_CHOICE_BOTTLE_EMPTY,
    BR_CHOICE_BOTTLE_FAIRY,
    BR_CHOICE_BOTTLE_REDPOTION,
    BR_CHOICE_BOTTLE_GREENPOTION,
    BR_CHOICE_BOTTLE_BLUEPOTION
} BossRushBottleChoices;

typedef enum {
    BR_CHOICE_LONGSHOT_NO,
    BR_CHOICE_LONGSHOT_YES
} BossRushLongshotChoices;

typedef enum {
    BR_CHOICE_HOVERBOOTS_NO,
    BR_CHOICE_HOVERBOOTS_YES
} BossRushHoverBootsChoices;

typedef enum {
    BR_CHOICE_TIMER_YES,
    BR_CHOICE_TIMER_NO
} BossRushTimerChoices;

#ifdef __cplusplus
extern "C" {
#endif
void BossRush_SpawnBlueWarps(PlayState* play);
void BossRush_HandleBlueWarp(PlayState* play, f32 warpPosX, f32 warpPosZ);
void BossRush_InitSave();
void BossRush_SetEquipment(uint8_t linkAge);
void BossRush_HandleCompleteBoss(PlayState* play);
const char* BossRush_GetSettingName(uint8_t optionIndex, uint8_t language);
const char* BossRush_GetSettingChoiceName(uint8_t optionIndex, uint8_t choiceIndex, uint8_t language);
uint8_t BossRush_GetSettingOptionsAmount(uint8_t optionIndex);
#ifdef __cplusplus
};
#endif

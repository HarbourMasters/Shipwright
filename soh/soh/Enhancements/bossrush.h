#pragma once

#include "variables.h"

#define BOSSRUSH_OPTIONS_AMOUNT 9
#define BOSSRUSH_MAX_OPTIONS_ON_SCREEN 6

typedef enum {
    BR_OPTIONS_BOSSES,
    BR_OPTIONS_HEARTS,
    BR_OPTIONS_AMMO,
    BR_OPTIONS_HEAL,
    BR_OPTIONS_HYPERBOSSES,
    BR_OPTIONS_BOTTLE,
    BR_OPTIONS_LONGSHOT,
    BR_OPTIONS_HOVERBOOTS,
    BR_OPTIONS_TIMER
} BossRushOptionEnums;

typedef enum {
    BR_OPTION_BOSSES_CHOICE_ALL,
    BR_OPTION_BOSSES_CHOICE_CHILD,
    BR_OPTION_BOSSES_CHOICE_ADULT
} BossRushOptionBossesChoices;

typedef enum {
    BR_OPTION_HEARTS_CHOICE_7,
    BR_OPTION_HEARTS_CHOICE_10,
    BR_OPTION_HEARTS_CHOICE_15,
    BR_OPTION_HEARTS_CHOICE_20,
    BR_OPTION_HEARTS_CHOICE_3,
    BR_OPTION_HEARTS_CHOICE_5
} BossRushOptionHeartsChoices;

typedef enum {
    BR_OPTION_AMMO_CHOICE_LIMITED,
    BR_OPTION_AMMO_CHOICE_FULL,
    BR_OPTION_AMMO_CHOICE_MAXED
} BossRushOptionAmmoChoices;

typedef enum {
    BR_OPTION_HEAL_CHOICE_GANONDORF,
    BR_OPTION_HEAL_CHOICE_EVERYBOSS,
    BR_OPTION_HEAL_CHOICE_NEVER
} BossRushOptionHealChoices;

typedef enum {
    BR_OPTION_HYPERBOSSES_CHOICE_NO,
    BR_OPTION_HYPERBOSSES_CHOICE_YES
} BossRushOptionHyperBossesChoices;

typedef enum {
    BR_OPTION_BOTTLE_CHOICE_NO,
    BR_OPTION_BOTTLE_CHOICE_EMPTY,
    BR_OPTION_BOTTLE_CHOICE_FAIRY
} BossRushOptionBottleChoices;

typedef enum {
    BR_OPTION_LONGSHOT_CHOICE_NO,
    BR_OPTION_LONGSHOT_CHOICE_YES
} BossRushOptionLongshotChoices;

typedef enum {
    BR_OPTION_HOVERBOOTS_CHOICE_NO,
    BR_OPTION_HOVERBOOTS_CHOICE_YES
} BossRushOptionHoverBootsChoices;

typedef enum {
    BR_OPTION_TIMER_CHOICE_YES,
    BR_OPTION_TIMER_CHOICE_NO
} BossRushOptionTimerChoices;

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

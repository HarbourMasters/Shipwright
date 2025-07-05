#pragma once

#include <libultraship/libultra/types.h>

#ifdef __cplusplus
extern "C" {
#endif
struct GameState;
struct FileChooseContext;

void FileChoose_UpdateBossRushMenu(struct GameState* gameState);
void FileChoose_DrawBossRushMenuWindowContents(struct FileChooseContext* fileChooseContext);
const char* BossRush_GetSettingName(u8 optionIndex, u8 language);
const char* BossRush_GetSettingChoiceName(u8 optionIndex, u8 choiceIndex, u8 language);
u8 BossRush_GetSettingOptionsAmount(u8 optionIndex);
#ifdef __cplusplus
};
#endif

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
    BR_OPTIONS_BUNNYHOOD,
    BR_OPTIONS_TIMER,
    BR_OPTIONS_MAX,
} BossRushOptionEnums;

typedef enum {
    BR_CHOICE_HYPERBOSSES_NO,
    BR_CHOICE_HYPERBOSSES_YES,
} BossRushHyperBossesChoices;
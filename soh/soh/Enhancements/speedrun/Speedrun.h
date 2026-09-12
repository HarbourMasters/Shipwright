#pragma once

#include <libultraship/libultra/types.h>

#ifdef __cplusplus
extern "C" {
#endif
struct GameState;
struct FileChooseContext;

void FileChoose_UpdateSpeedrunMenu(struct GameState* gameState);
void FileChoose_DrawSpeedrunMenuWindowContents(struct FileChooseContext* fileChooseContext);

// Snapshots the settings the new file plays with: the player's current ones, with the preset picked in the file select
// menu laid over them.
void Speedrun_InitSaveFile(void);

// Gyro aiming is not allowed in a speedrun, so this reads back as 0 there and unchanged everywhere else.
f32 Ship_FilterGyro(f32 gyroAxis);

// Whether debug mode applies to a file of this quest type.
bool Ship_QuestDebugEnabled(u8 questId);
#ifdef __cplusplus
};

#include "z64save.h"

// Registers the save section and the hooks that put settings back when a run is left. Call once at startup.
void Speedrun_Register();

// Whether the loaded file is a speedrun file, for code that can't reach gSaveContext.
bool Speedrun_IsActive();

void Speedrun_SaveSaveSection(SaveContext* saveContext, int sectionID, bool fullSave);
void Speedrun_LoadSaveSection();

// Unlocks menu and puts back settings that were replaced by a speedrun file. Safe to call when nothing was replaced.
void Speedrun_RestoreSettings();

namespace Ship {
class GuiWindow;
}

// Called by the menu every frame, before the other windows draw. While a speedrun file is loaded it keeps every window
// a run may not show hidden, and turns opening the menu into toggling the run info windows. True while locked, meaning
// the menu must not draw.
bool Speedrun_EnforceGuiLockdown(Ship::GuiWindow& menu);
#endif

#define SPEEDRUN_MAX_OPTIONS_ON_SCREEN 6

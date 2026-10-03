#pragma once

#include <libultraship/libultra/types.h>

#ifdef __cplusplus
extern "C" {
#endif
struct GameState;
struct FileChooseContext;

void FileChoose_UpdateSpeedrunMenu(struct GameState* gameState);
void FileChoose_DrawSpeedrunMenuWindowContents(struct FileChooseContext* fileChooseContext);

// Saves the settings the new file plays with: the player's current ones with the chosen preset applied.
void Speedrun_InitSaveFile(void);

// Gyro aiming is not allowed in a speedrun, so this returns 0 there.
f32 Ship_FilterGyro(f32 gyroAxis);

// Speedrun files show the in-game timer at a fixed spot, top right, whatever the cosmetics editor says.
void Ship_PinSpeedrunTimer(s32* left, s32* top);

// Whether debug mode applies to a file of this quest type.
bool Ship_QuestDebugEnabled(u8 questId);
#ifdef __cplusplus
};

// Registers the save section and the hook that restores settings when a run is exited. Call once at startup.
void Speedrun_Register();

namespace Ship {
class GuiWindow;
}

// Called by the menu every frame, before other windows draw. While a speedrun file is loaded it keeps disallowed
// windows hidden and makes opening the menu toggle the run info windows. Returns true when the menu must not draw.
bool Speedrun_EnforceGuiLockdown(Ship::GuiWindow& menu);
#endif

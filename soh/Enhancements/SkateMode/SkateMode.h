#ifndef SOH_SKATE_MODE_H
#define SOH_SKATE_MODE_H

/**
 * Skate Mode: a hotkey swaps Link's ground movement for skateboard physics.
 *
 * - SkatePhysics.c  : game-independent physics core
 * - SkateMode.cpp   : hotkey, settings and tuning (this API)
 * - z_player.c      : Player_Action_Skate, which drives Link from the physics core
 */

#include <stdint.h>
#include "SkatePhysics.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CVAR_SKATE(var) CVAR_ENHANCEMENT("SkateMode." var)

/** Whether Link should currently be on the board. */
uint8_t SkateMode_IsActive(void);
void SkateMode_SetActive(uint8_t active);
void SkateMode_Toggle(void);
/** Called for the keyboard hotkey (F8). */
void SkateMode_OnKeyboardHotkey(void);

/** Fills `tuning` with the defaults adjusted by the menu sliders. */
void SkateMode_GetTuning(SkateTuning* tuning);

uint8_t SkateMode_BailsEnabled(void);
uint8_t SkateMode_AutoPushEnabled(void);
uint8_t SkateMode_TerrainFrictionEnabled(void);
/** Zelda-style steering (stick relative to the camera) instead of Skate-style (relative to the board). */
uint8_t SkateMode_CameraRelativeSteering(void);
uint8_t SkateMode_DrawBoardEnabled(void);
float SkateMode_BoardScale(void);

#ifdef __cplusplus
}
#endif

#endif

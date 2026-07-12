#pragma once

// Shared declarations for the transformation-mask ability ports (GoronForm/ZoraForm).
//
// The ability code is adapted from MM's z_player.c (2Ship2Harkinian source) but grafted onto
// OOT's player action-function system: custom action functions are installed with
// Player_SetupAction and call back into OOT's own z_player.c helpers. Those helpers have no
// public header, but they have external linkage (same pattern as
// soh/Enhancements/randomizer/hook_handlers.cpp), so they are declared here.

// C++ headers must come before the extern "C" block: the game headers below transitively
// include libultraship headers, and their first inclusion must not happen with C linkage.
#include <libultraship/bridge.h>
#include "soh/Enhancements/mm/PlayerFormSystem.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

extern PlayState* gPlayState;

// z_player.c internals (external linkage, no public header)
s32 Player_SetupAction(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 flags);
void Player_AnimPlayOnce(PlayState* play, Player* player, LinkAnimationHeader* anim);
void Player_AnimPlayLoop(PlayState* play, Player* player, LinkAnimationHeader* anim);
void Player_AnimPlayOnceAdjusted(PlayState* play, Player* player, LinkAnimationHeader* anim);
void Player_AnimPlayLoopAdjusted(PlayState* play, Player* player, LinkAnimationHeader* anim);
void Player_AnimChangeOnceMorph(PlayState* play, Player* player, LinkAnimationHeader* anim);
void Player_AnimChangeLoopSlowMorph(PlayState* play, Player* player, LinkAnimationHeader* anim);
void Player_PlayVoiceSfx(Player* player, u16 sfxId);
void Player_PlayLandingSfx(Player* player);
s32 Player_GetMovementSpeedAndYaw(Player* player, f32* outSpeedTarget, s16* outYawTarget, f32 speedMode,
                                  PlayState* play);
s32 Player_DecelerateToZero(Player* player);
void Player_RequestQuake(PlayState* play, s32 speed, s32 y, s32 countdown);
void Player_RequestRumble(Player* player, s32 sourceStrength, s32 duration, s32 decreaseRate, s32 distSq);
void Player_SetInvulnerability(Player* player, s32 timer);
// Enter the tread-water swim action
void func_80838F18(PlayState* play, Player* player);
// Water-exit housekeeping: splash, sets JUMPING, clears IN_WATER, boot data (keeps actionFunc)
void func_8083D0A8(PlayState* play, Player* player, f32 yVelocity);
// Spawn the water-surface splash for the given y velocity; returns nonzero if spawned
s32 func_8083CFA8(PlayState* play, Player* player, f32 yVelocity, s32 splashScale);
// Return to the appropriate idle action (lock-on aware)
void func_80839F90(Player* player, PlayState* play);
// Roll/slide floor dust effect
s32 func_8084269C(PlayState* play, Player* player);

// Vanilla action functions (for detecting/re-entering vanilla states)
void Player_Action_Roll(Player* player, PlayState* play);
void Player_Action_Idle(Player* player, PlayState* play);
void Player_Action_80840450(Player* player, PlayState* play); // idle, hostile lock-on
void Player_Action_808407CC(Player* player, PlayState* play); // idle, friendly lock-on/parallel
void Player_Action_80840DE4(Player* player, PlayState* play); // walk (targeting)
void Player_Action_808414F8(Player* player, PlayState* play); // run
void Player_Action_8084170C(Player* player, PlayState* play); // backwalk/sidewalk
void Player_Action_8084D610(Player* player, PlayState* play); // tread water (surface idle)
void Player_Action_8084D7C4(Player* player, PlayState* play); // swim, no control
void Player_Action_8084D84C(Player* player, PlayState* play); // swim forward (surface)
void Player_Action_8084DAB4(Player* player, PlayState* play); // swim, z-targeting
void Player_Action_8084DC48(Player* player, PlayState* play); // dive
void Player_Action_8084E1EC(Player* player, PlayState* play); // surface with item/breath

// These macros live in z_player.c, mirror the values here
#define FORMS_SPEED_MODE_LINEAR 0.0f
#define FORMS_SPEED_MODE_CURVED 0.018f
#define PLAYER_ANIM_ADJUSTED_SPEED (2.0f / 3.0f)

// Whether the player is mid Goron roll (ball drawn instead of the skeleton). Read by
// PlayerFormDraw (ball rendering) and PlayerFormSystem (skip shadow-scale enforcement).
bool GoronForm_IsRolling(void);
// Spike-roll state, tint ramp [0,1] toward red, and squash factor for the ball draw
bool GoronForm_IsSpiked(void);
f32 GoronForm_GetChargeTint(void);
f32 GoronForm_GetSquash(void);

#ifdef __cplusplus
}
#endif

#pragma once

#include <libultraship/libultra.h>
#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Player;

// Returns true when child Link is allowed to use the Master Sword (randomizer shuffle).
s32 ChildLink2hMS_CanChildUseMasterSword(void);

// Returns true when the age requirement should be bypassed for Master Sword while child.
s32 ChildLink2hMS_ShouldBypassAgeReq(s32 equipType, s32 equipValue);

// Returns true when the currently held Master Sword should be treated as a two-handed weapon for child Link.
s32 ChildLink2hMS_IsTwoHanded(struct Player* player);

// Returns true when age restrictions should be treated as met for the given equip slot (used by pause menu).
s32 ChildLink2hMS_ShouldAllowEquip(s32 equipType, s32 equipValue);

// Optional override to map Master Sword to two-handed melee behavior for child Link.
s32 ChildLink2hMS_OverrideMeleeWeapon(s32 actionParam, s32 baseMeleeWeapon);

#ifdef __cplusplus
}
#endif

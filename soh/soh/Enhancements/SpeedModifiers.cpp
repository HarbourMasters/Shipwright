#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/BunnyHood.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

#define CVAR_SPEED_MODIFIER_VALUE_NAME CVAR_CHEAT("SpeedModifier.Value")

static f32 GetSpeedModifierFactor(bool inputAvailable) {
    f32 value = CVarGetFloat(CVAR_SPEED_MODIFIER_VALUE_NAME, 1.0f);
    if (value == 1.0f) {
        return 1.0f;
    }

    if (CVarGetInteger(CVAR_CHEAT("SpeedModifier.SpeedToggle"), 0)) {
        return gWalkSpeedToggle ? value : 1.0f;
    }

    if (inputAvailable) {
        s32 mod1Mask = CVarGetInteger(CVAR_CHEAT("SpeedModifier.Btn"), BTN_CUSTOM_MODIFIER1);
        Input* input = &gPlayState->state.input[0];
        if (mod1Mask != 0 && CHECK_BTN_ALL(input->cur.button, mod1Mask)) {
            return value;
        }
    }

    return 1.0f;
}

static f32 GetSpeedModifierJumpFactor() {
    if (CVarGetInteger(CVAR_CHEAT("SpeedModifier.DoesntChangeJump"), 0)) {
        return 1.0f;
    }
    return GetSpeedModifierFactor(true);
}

static bool ShouldAmplifyJump(Player* player) {
    return Ship_GetBunnyHoodJumpFactor(player) != 1.0f || GetSpeedModifierJumpFactor() != 1.0f;
}

static void RegisterSpeedModifiers() {
    bool speedModifierActive = CVarGetFloat(CVAR_SPEED_MODIFIER_VALUE_NAME, 1.0f) != 1.0f;
    bool bunnyHoodActive = Ship_GetBunnyHoodMode() != BUNNY_HOOD_VANILLA;

    // Airborne (jump) velocity. z_player clamps linearVelocity to the vanilla run speed limit when this returns true;
    // skip that clamp so the amplified running velocity carries into the jump.
    COND_VB_SHOULD(VB_PLAYER_LIMIT_JUMP_SPEED, speedModifierActive || bunnyHoodActive, {
        Player* player = va_arg(args, Player*);
        if (ShouldAmplifyJump(player)) {
            *should = false;
        }
    });

    // dive-into-water animation never clamped by vanilla, so re-clamp to vanilla run speed limit here unless jump be
    // amplified. This keeps dive vanilla-distance (e.g. Gerudo Valley canyon) for bunny hood "fast run" & "Don't affect
    // jump distance" option.
    COND_VB_SHOULD(VB_PLAYER_LIMIT_DIVE_XZ_SPEED, speedModifierActive || bunnyHoodActive, {
        Player* player = va_arg(args, Player*);
        if (!ShouldAmplifyJump(player)) {
            f32 maxSpeed = R_RUN_SPEED_LIMIT / 100.0f;
            player->linearVelocity = CLAMP(player->linearVelocity, -maxSpeed, maxSpeed);
        }
    });

    // Ground run speed target, multiplied in place.
    COND_VB_SHOULD(VB_PLAYER_MODIFY_RUN_SPEED, speedModifierActive || bunnyHoodActive, {
        Player* player = va_arg(args, Player*);
        f32* speedTarget = va_arg(args, f32*);
        *speedTarget *= Ship_GetBunnyHoodRunFactor(player) * GetSpeedModifierFactor(true);
    });

    // Swim speed multiplied in place. Called per speed z_player scales; bunny hood does not apply underwater.
    COND_VB_SHOULD(VB_PLAYER_MODIFY_SWIM_SPEED, speedModifierActive, {
        [[maybe_unused]] Player* player = va_arg(args, Player*);
        f32* value = va_arg(args, f32*);
        bool inputAvailable = va_arg(args, int) != 0;
        *value *= GetSpeedModifierFactor(inputAvailable);
    });
}

static RegisterShipInitFunc initFunc(RegisterSpeedModifiers, { CVAR_SPEED_MODIFIER_VALUE_NAME, CVAR_BUNNY_HOOD_NAME });

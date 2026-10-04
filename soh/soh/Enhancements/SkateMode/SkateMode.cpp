#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "SkateMode.h"

extern "C" {
#include "z64.h"
#include "macros.h"
extern PlayState* gPlayState;
}

#define CVAR_SKATE_ENABLED_NAME CVAR_SKATE("Enabled")
#define CVAR_SKATE_ENABLED_VALUE CVarGetInteger(CVAR_SKATE_ENABLED_NAME, 1)
#define CVAR_SKATE_BTN_NAME CVAR_SKATE("Btn")
#define CVAR_SKATE_BTN_VALUE CVarGetInteger(CVAR_SKATE_BTN_NAME, BTN_CUSTOM_MODIFIER1)

// Not saved: every session starts on foot.
static bool sSkateActive = false;

extern "C" uint8_t SkateMode_IsActive(void) {
    return sSkateActive && CVAR_SKATE_ENABLED_VALUE;
}

extern "C" void SkateMode_SetActive(uint8_t active) {
    if (sSkateActive == (active != 0)) {
        return;
    }

    sSkateActive = (active != 0);
    Notification::Emit({ .message = sSkateActive ? "Skate Mode: ON" : "Skate Mode: OFF", .remainingTime = 2.0f });
}

extern "C" void SkateMode_Toggle(void) {
    if (!CVAR_SKATE_ENABLED_VALUE || gPlayState == nullptr) {
        return;
    }

    SkateMode_SetActive(!sSkateActive);
}

extern "C" void SkateMode_OnKeyboardHotkey(void) {
    if (CVarGetInteger(CVAR_SKATE("KeyboardHotkey"), 1)) {
        SkateMode_Toggle();
    }
}

extern "C" void SkateMode_GetTuning(SkateTuning* tuning) {
    SkatePhysics_DefaultTuning(tuning);

    float turn = CVarGetFloat(CVAR_SKATE("TurnRate"), 1.0f);
    float ollie = CVarGetFloat(CVAR_SKATE("OlliePower"), 1.0f);
    float friction = CVarGetFloat(CVAR_SKATE("Friction"), 1.0f);

    tuning->pushImpulse *= CVarGetFloat(CVAR_SKATE("PushPower"), 1.0f);
    tuning->pushTopSpeed = CVarGetFloat(CVAR_SKATE("PushTopSpeed"), tuning->pushTopSpeed);
    tuning->maxSpeed = CVarGetFloat(CVAR_SKATE("MaxSpeed"), tuning->maxSpeed);
    if (tuning->maxSpeed < tuning->pushTopSpeed) {
        tuning->maxSpeed = tuning->pushTopSpeed;
    }
    tuning->rollFriction *= friction;
    tuning->drag *= friction;
    tuning->grip = CVarGetFloat(CVAR_SKATE("Grip"), tuning->grip);
    tuning->slopeGravity = CVarGetFloat(CVAR_SKATE("SlopeGravity"), tuning->slopeGravity);
    tuning->turnRateSlow *= turn;
    tuning->turnRateFast *= turn;
    tuning->ollieMin *= ollie;
    tuning->ollieMax *= ollie;
}

extern "C" uint8_t SkateMode_BailsEnabled(void) {
    return CVarGetInteger(CVAR_SKATE("Bails"), 1) != 0;
}

extern "C" uint8_t SkateMode_AutoPushEnabled(void) {
    return CVarGetInteger(CVAR_SKATE("AutoPush"), 0) != 0;
}

extern "C" uint8_t SkateMode_TerrainFrictionEnabled(void) {
    return CVarGetInteger(CVAR_SKATE("TerrainFriction"), 1) != 0;
}

extern "C" uint8_t SkateMode_CameraRelativeSteering(void) {
    return CVarGetInteger(CVAR_SKATE("CameraRelativeSteering"), 0) != 0;
}

extern "C" uint8_t SkateMode_DrawBoardEnabled(void) {
    return CVarGetInteger(CVAR_SKATE("DrawBoard"), 1) != 0;
}

extern "C" float SkateMode_BoardScale(void) {
    return CVarGetFloat(CVAR_SKATE("BoardScale"), 0.45f);
}

static void OnGameStateMainStartSkateHotkey() {
    const uint16_t mask = static_cast<uint16_t>(CVAR_SKATE_BTN_VALUE & 0xFFFF);

    if (gPlayState == nullptr || mask == 0) {
        return;
    }

    // Fires on the frame the last button of the combination goes down.
    if (CHECK_BTN_ANY(gPlayState->state.input[0].press.button, mask) &&
        CHECK_BTN_ALL(gPlayState->state.input[0].cur.button, mask)) {
        SkateMode_Toggle();
    }
}

static void RegisterSkateMode() {
    COND_HOOK(OnGameStateMainStart, CVAR_SKATE_ENABLED_VALUE, OnGameStateMainStartSkateHotkey);
    // Leaving to the title screen puts Link back on his feet.
    COND_HOOK(OnExitGame, CVAR_SKATE_ENABLED_VALUE, [](int32_t fileNum) { sSkateActive = false; });

    if (!CVAR_SKATE_ENABLED_VALUE) {
        sSkateActive = false;
    }
}

static RegisterShipInitFunc initFunc(RegisterSkateMode, { CVAR_SKATE_ENABLED_NAME });

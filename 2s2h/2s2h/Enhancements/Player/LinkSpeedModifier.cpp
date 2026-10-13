#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/BenPort.h"

extern "C" {
#include "variables.h"
extern Input* sPlayerControlInput;
}

#define CVAR_SPEED_MODIFIER_MODE_NAME "gCheats.SpeedModifier.Mode"
#define CVAR_SPEED_MODIFIER_TOGGLE_NAME "gCheats.SpeedModifier.Toggle"
#define CVAR_SPEED_MODIFIER_VALUE_NAME "gCheats.SpeedModifier.Value"
#define CVAR_SPEED_MODIFIER_BTN_NAME "gCheats.SpeedModifier.Btn"
#define CVAR_SPEED_MODIFIER_MODE CVarGetInteger(CVAR_SPEED_MODIFIER_MODE_NAME, 0)
#define CVAR_SPEED_MODIFIER_TOGGLE CVarGetInteger(CVAR_SPEED_MODIFIER_TOGGLE_NAME, 0)
#define CVAR_SPEED_MODIFIER_VALUE CVarGetFloat(CVAR_SPEED_MODIFIER_VALUE_NAME, 1.0f)
#define CVAR_SPEED_MODIFIER_BTN CVarGetInteger(CVAR_SPEED_MODIFIER_BTN_NAME, BTN_CUSTOM_MODIFIER1)

bool btnHeldOrToggled = false;

void RegisterLinkSpeedModifier() {
    // Reset in case they disabled while toggled
    btnHeldOrToggled = false;

    COND_VB_SHOULD(VB_SPEED_MODIFIER_WALK, CVAR_SPEED_MODIFIER_MODE, {
        f32* speedTarget = va_arg(args, f32*);

        if (CVAR_SPEED_MODIFIER_MODE == 1 || btnHeldOrToggled) {
            *speedTarget *= CVAR_SPEED_MODIFIER_VALUE;
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_SWIM, CVAR_SPEED_MODIFIER_MODE, {
        f32* incrStep = va_arg(args, f32*);
        f32* maxSpeed = va_arg(args, f32*);
        f32* speed = va_arg(args, f32*);
        f32* speedTarget = va_arg(args, f32*);
        f32 swimMod = 1.0f;

        // sControlInput is NULL to prevent inputs while surfacing after obtaining an underwater item so we want
        // to ignore it for that case
        if (sPlayerControlInput == NULL) {
            return;
        }

        if (CVAR_SPEED_MODIFIER_MODE == 1 || btnHeldOrToggled) {
            swimMod *= CVAR_SPEED_MODIFIER_VALUE;
            *maxSpeed *= swimMod;
            Math_AsymStepToF(speed, *speedTarget * 0.8f * swimMod, *incrStep, (fabsf(*speed) * 0.02f) + 0.05f);
            *should = false;
        }
    });

    COND_HOOK(OnPassPlayerInputs, CVAR_SPEED_MODIFIER_MODE >= 2, [](Input* input) {
        if (CVAR_SPEED_MODIFIER_MODE == 2) {
            if (CHECK_BTN_ALL(input->cur.button, CVAR_SPEED_MODIFIER_BTN)) {
                btnHeldOrToggled = true;
            } else {
                btnHeldOrToggled = false;
            }
        } else {
            if (CHECK_BTN_ALL(input->cur.button, CVAR_SPEED_MODIFIER_BTN) &&
                CHECK_BTN_ANY(input->press.button, CVAR_SPEED_MODIFIER_BTN)) {
                btnHeldOrToggled = !btnHeldOrToggled;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterLinkSpeedModifier, { CVAR_SPEED_MODIFIER_MODE_NAME });
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/controls/Mouse.h"
#include "soh/ShipInit.hpp"

#include <math.h>

extern "C" {
#include "global.h"
}

void RegisterFreeLookDoorCamRelease() {
    COND_VB_SHOULD(VB_RELEASE_DOORC_CAMERA, CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0), {
        Camera* camera = va_arg(args, Camera*);

        // Also release the door peek camera when free look moves it, reusing SetCameraManual's threshold.
        f32 freeLookX = -camera->play->state.input[0].cur.right_stick_x * 10.0f;
        f32 freeLookY = camera->play->state.input[0].cur.right_stick_y * 10.0f;
        Mouse_HandleThirdPerson(&freeLookX, &freeLookY);

        if (fabsf(freeLookX) >= 15.0f || fabsf(freeLookY) >= 15.0f) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFreeLookDoorCamRelease, { CVAR_SETTING("FreeLook.Enabled") });

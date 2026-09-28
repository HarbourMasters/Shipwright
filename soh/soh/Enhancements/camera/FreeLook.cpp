#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/controls/Mouse.h"
#include "soh/ShipInit.hpp"

#include <math.h>

extern "C" {
#include "global.h"
#include "src/overlays/actors/ovl_En_Horse/z_en_horse.h"
}

void RegisterFreeLookFixes() {
    bool freeLook = CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0);

    COND_VB_SHOULD(VB_RELEASE_DOORC_CAMERA, freeLook, {
        Camera* camera = va_arg(args, Camera*);

        // Also release the door peek camera when free look moves it, reusing SetCameraManual's threshold.
        f32 freeLookX = -camera->play->state.input[0].cur.right_stick_x * 10.0f;
        f32 freeLookY = camera->play->state.input[0].cur.right_stick_y * 10.0f;
        Mouse_HandleThirdPerson(&freeLookX, &freeLookY);

        if (fabsf(freeLookX) >= 15.0f || fabsf(freeLookY) >= 15.0f) {
            *should = true;
        }
    });

    // EnHorse_Init sets riderPos as a world position instead of an offset,
    // putting Link out of bounds until horse drawn.
    COND_ID_HOOK(OnActorInit, ACTOR_EN_HORSE, freeLook, [](void* refActor) {
        EnHorse* enHorse = reinterpret_cast<EnHorse*>(refActor);
        enHorse->riderPos.x = 0.0f;
        enHorse->riderPos.y = 70.0f;
        enHorse->riderPos.z = 0.0f;
    });
}

static RegisterShipInitFunc initFunc(RegisterFreeLookFixes, { CVAR_SETTING("FreeLook.Enabled") });

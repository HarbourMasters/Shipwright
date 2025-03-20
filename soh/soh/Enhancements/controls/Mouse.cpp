#include "Mouse.h"
#include "soh/OTRGlobals.h"
#include "z64player.h"
#include "global.h"
#include <Window.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

static Ship::Coords mouseCoord = {};
static Ship::Coords mouseCoordRel = {};

#define CVAR_ENABLE_MOUSE_NAME CVAR_SETTING("EnableMouse")
#define CVAR_ENABLE_MOUSE_DEFAULT 0
#define CVAR_ENABLE_MOUSE_VALUE CVarGetInteger(CVAR_ENABLE_MOUSE_NAME, CVAR_ENABLE_MOUSE_DEFAULT)
#define MOUSE_ENABLED (CVAR_ENABLE_MOUSE_VALUE && GetWindow()->IsMouseCaptured())

std::shared_ptr<Ship::Window> GetWindow() {
    return OTRGlobals::Instance->context->GetWindow();
}

extern "C" {
void Mouse_UpdatePos() {
    mouseCoord = GetWindow()->GetMousePos();
}

void Mouse_UpdatePosRel() {
    mouseCoordRel = GetWindow()->GetMouseDelta();
}

void Mouse_UpdateAll() {
    Mouse_UpdatePos();
    Mouse_UpdatePosRel();
}

void Mouse_HandleThirdPerson(f32* newCamX, f32* newCamY) {
    if (MOUSE_ENABLED) {
        *newCamX -= mouseCoordRel.x * 40.0f;
        *newCamY += mouseCoordRel.y * 40.0f;
    }
}

void Mouse_HandleFirstPerson(Player* player) {
    f32 xAxisMulti = CVarGetFloat(CVAR_SETTING("FirstPersonCameraSensitivity.X"), 1.0f);
    f32 yAxisMulti = CVarGetFloat(CVAR_SETTING("FirstPersonCameraSensitivity.Y"), 1.0f);
    s8 invertXAxisMulti = ((CVarGetInteger(CVAR_SETTING("Controls.InvertAimingXAxis"), 0)
                        && !CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0))
                            || (!CVarGetInteger(CVAR_SETTING("Controls.InvertAimingXAxis"), 0)
                        && CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0))) ? -1 : 1;
    s8 invertYAxisMulti = CVarGetInteger(CVAR_SETTING("Controls.InvertAimingYAxis"), 1) ? 1 : -1;
    if (MOUSE_ENABLED) {
        player->actor.focus.rot.y -= mouseCoordRel.x * 6.0f * xAxisMulti * invertXAxisMulti;
        player->actor.focus.rot.x += mouseCoordRel.y * 6.0f * yAxisMulti * invertYAxisMulti;
    }
}

void Mouse_RecenterCursor() {
    u32 width = GetWindow()->GetWidth();
    u32 height = GetWindow()->GetHeight();
    if (MOUSE_ENABLED) {
        GetWindow()->SetMousePos({(s32) (width/2), (s32) (height/2)});
    }
}

void Mouse_HandleShield(f32* sp50, f32* sp54) {
    if (MOUSE_ENABLED) {
        s32 width = GetWindow()->GetWidth();
        s32 height = GetWindow()->GetHeight();
        f32 xBound = 7200 / ((f32)width / 2);
        f32 yBound = 6000 / ((f32)height / 2);
        *sp50 += (mouseCoord.x - (width / 2)) * xBound * (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? 1 : -1);
        *sp54 += (mouseCoord.y - (height / 2)) * yBound;
        *sp50 = CLAMP(*sp50, -7200, 7200);
        *sp54 = CLAMP(*sp54, -6000, 6000);
    }
}

static s8 iterMouse = 0;
static f32 mouseQuickspinX[5] = {};
static f32 mouseQuickspinY[5] = {};
static u8 quickspinCount = 0;

void Mouse_UpdateQuickspinCount() {
    if (MOUSE_ENABLED) {
        quickspinCount = (quickspinCount + 1) % 5;
        mouseQuickspinX[quickspinCount] = mouseCoord.x;
        mouseQuickspinY[quickspinCount] = mouseCoord.y;
    } else {
        quickspinCount = 0;
    }
}

bool Mouse_HandleQuickspin(bool* should, s8* iter2, s8* sp3C) {
    s8 temp1;
    s8 temp2;
    s32 i;
    if (!MOUSE_ENABLED) {
        return *should = false;
    }

    for (i = 0; i < 4; i++, iter2++) {
        // Calculating angles as per z_lib.c:func_80077D10()
        f32 relY = mouseQuickspinY[i + 1] - mouseQuickspinY[i];
        f32 relX = mouseQuickspinX[i + 1] - mouseQuickspinX[i];
        s16 aTan = Math_Atan2S(relY, -relX);
        iterMouse = (u16)(aTan + 0x2000) >> 9; // See z_player.c:Player_ProcessControlStick()
        if ((*iter2 = iterMouse) < 0) {
            return *should = false;
        }
        *iter2 *= 2;
    }
    temp1 = sp3C[0] - sp3C[1];
    if (ABS(temp1) < 10) {
        return *should = false;
    }
    iter2 = &sp3C[1];
    for (i = 1; i < 3; i++, iter2++) {
        temp2 = *iter2 - *(iter2 + 1);
        if ((ABS(temp2) < 10) || (temp2 * temp1 < 0)) {
            return *should = false;
        }
    }

    return *should = true;
}

// Hook handlers

void Mouse_RegisterRecenterCursorOnShield() {
    COND_HOOK(OnPlayerHoldUpShield, true, Mouse_RecenterCursor);
}

void Mouse_RegisterHandleFirstPerson() {
    COND_HOOK(OnPlayerFirstPersonControl, true, Mouse_HandleFirstPerson);
}

void Mouse_RegisterHandleShield() {
    COND_HOOK(OnPlayerShieldControl, true, Mouse_HandleShield);
}

void Mouse_RegisterUpdateQuickspinCount() {
    COND_HOOK(OnPlayerProcessStick, true, Mouse_UpdateQuickspinCount);
}

void Mouse_RegisterHandleQuickspin() {
    REGISTER_VB_SHOULD(VB_SHOULD_QUICKSPIN, { Mouse_HandleQuickspin(should, va_arg(args, s8*), va_arg(args, s8*)); } );
}

static RegisterShipInitFunc initFunc_shieldRecenter(Mouse_RegisterRecenterCursorOnShield, { CVAR_ENABLE_MOUSE_NAME });
static RegisterShipInitFunc initFunc_firstPerson(Mouse_RegisterHandleFirstPerson, { CVAR_ENABLE_MOUSE_NAME });
static RegisterShipInitFunc initFunc_quickspinCount(Mouse_RegisterUpdateQuickspinCount, { CVAR_ENABLE_MOUSE_NAME });
static RegisterShipInitFunc initFunc_quickspin(Mouse_RegisterHandleQuickspin, { CVAR_ENABLE_MOUSE_NAME });
static RegisterShipInitFunc initFunc_shieldMove(Mouse_RegisterHandleShield, { CVAR_ENABLE_MOUSE_NAME });
} //extern "C"

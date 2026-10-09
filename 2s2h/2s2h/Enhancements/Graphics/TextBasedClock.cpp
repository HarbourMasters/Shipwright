#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenGui/HudEditor.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
}

#define CVAR_NAME "gEnhancements.Graphics.ClockType"
#define CVAR CVarGetInteger(CVAR_NAME, CLOCK_TYPE_ORIGINAL)

void DrawTextBasedClock() {
    HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
    if (HudEditor_IsActiveElementHidden()) {
        return;
    }

    if ((R_TIME_SPEED != 0) &&
        ((gPlayState->msgCtx.msgMode == MSGMODE_NONE) ||
         ((gPlayState->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) && !Play_InCsMode(gPlayState)) ||
         (gPlayState->msgCtx.msgMode == MSGMODE_NONE) ||
         ((gPlayState->msgCtx.currentTextId >= 0x100) && (gPlayState->msgCtx.currentTextId <= 0x200)) ||
         (gSaveContext.gameMode == GAMEMODE_END_CREDITS)) &&
        !FrameAdvance_IsEnabled(gPlayState) && !Environment_IsTimeStopped() && (gSaveContext.save.day <= 3)) {

        OPEN_DISPS(gPlayState->state.gfxCtx);

        if ((gPlayState->pauseCtx.state == PAUSE_STATE_OFF) &&
            (gPlayState->pauseCtx.debugEditor == DEBUG_EDITOR_NONE)) {
            Gfx_SetupDL39_Overlay(gPlayState->state.gfxCtx);

            u16 curMinutes = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) % 60;
            u16 curHours = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) / 60;

            u16 timeUntilMoonCrash = (s32)TIME_UNTIL_MOON_CRASH;
            u16 timeInMinutes = (s32)TIME_TO_MINUTES_F(timeUntilMoonCrash) % 60;
            u16 timeInHours = (s32)TIME_TO_MINUTES_F(timeUntilMoonCrash) / 60;

            char formattedTime[10];
            char formattedCrashTime[10];

            OPEN_PRINTER(OVERLAY_DISP);

            // Inverted time
            if (gSaveContext.save.timeSpeedOffset == -2) {
                GfxPrint_SetColor(&printer, 0, 204, 255, 255);
            } else {
                GfxPrint_SetColor(&printer, 255, 255, 255, 255);
            }

            // Scale starting position values by 8 for use with hud editor
            s16 posX = 13 * 8;
            s16 posY = 26 * 8;

            if (HudEditor_ShouldOverrideDraw()) {
                HudEditor_ModifyRectPosValues(&posX, &posY);
            }

            // scale back down and clamp to avoid negative values
            posX = std::max(posX / 8, 0);
            posY = std::max(posY / 8, 0);

            if (CVarGetInteger("gEnhancements.Graphics.24HoursClock", 0)) {
                sprintf(formattedTime, "%02d:%02d", curHours, curMinutes);
                GfxPrint_SetPos(&printer, posX + 1, posY);
            } else { // Format hours and minutes for 12-hour AM/PM clock
                char amPm[3] = "am";
                if (curHours >= 12) {
                    strcpy(amPm, "pm");
                    if (curHours > 12) {
                        curHours -= 12;
                    }
                }
                if (curHours == 0) {
                    curHours = 12;
                }
                sprintf(formattedTime, "%02d:%02d%s", curHours, curMinutes, amPm);
                GfxPrint_SetPos(&printer, posX, posY);
            }

            GfxPrint_Printf(&printer, "Day %d: %s", gSaveContext.save.day, formattedTime);
            GfxPrint_SetPos(&printer, posX, posY + 1);

            // Crash Countdown
            if ((CURRENT_DAY >= 4) ||
                ((CURRENT_DAY == 3) && (CURRENT_TIME >= (CLOCK_TIME(0, 0) + 5)) && (CURRENT_TIME < CLOCK_TIME(6, 0)))) {
                GfxPrint_SetColor(&printer, 255, 0, 0, 255);
                sprintf(formattedCrashTime, "%02d:%02d", timeInHours, timeInMinutes);
                GfxPrint_Printf(&printer, "Crash in %s", formattedCrashTime);
            }

            CLOSE_PRINTER(printer, OVERLAY_DISP);
        }

        CLOSE_DISPS(gPlayState->state.gfxCtx);
    }

    hudEditorActiveElement = HUD_EDITOR_ELEMENT_NONE;
}

void RegisterTextBasedClock() {
    COND_VB_SHOULD(VB_PREVENT_CLOCK_DISPLAY, CVAR == CLOCK_TYPE_TEXT_BASED, {
        *should = true;
        DrawTextBasedClock();
    });
}

static RegisterShipInitFunc initFunc(RegisterTextBasedClock, { CVAR_NAME });

#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenGui/HudEditor.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include "interface/parameter_static/parameter_static.h"
#include "misc/title_static/title_static.h"
Gfx* Gfx_DrawTexRectIA8(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop,
                        s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy);

Gfx* Gfx_DrawTexRectIA8_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                   s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                   s16 a);

Gfx* Gfx_DrawTexRectIA8_DropShadowOffset(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                         s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                                         s16 r, s16 g, s16 b, s16 a, s32 masks, s32 rects);
}

#define CVAR_NAME "gEnhancements.Graphics.ClockType"
#define CVAR CVarGetInteger(CVAR_NAME, CLOCK_TYPE_ORIGINAL)

#define SECONDS_IN_THREE_DAYS (3 * 24 * 60 * 60)
#define SECONDS_IN_SIX_HOURS (6 * 60 * 60)
#define CLOCK_SECTION_HALFWIDTH 24
#define CLOCK_SECTION_WIDTH 48

static TexturePtr sDigitTextures[] = {
    (TexturePtr)gFinalHoursClockDigit0Tex, (TexturePtr)gFinalHoursClockDigit1Tex, (TexturePtr)gFinalHoursClockDigit2Tex,
    (TexturePtr)gFinalHoursClockDigit3Tex, (TexturePtr)gFinalHoursClockDigit4Tex, (TexturePtr)gFinalHoursClockDigit5Tex,
    (TexturePtr)gFinalHoursClockDigit6Tex, (TexturePtr)gFinalHoursClockDigit7Tex, (TexturePtr)gFinalHoursClockDigit8Tex,
    (TexturePtr)gFinalHoursClockDigit9Tex, (TexturePtr)gFinalHoursClockColonTex,
};

static TexturePtr sFinalHoursDigitTextures[] = {
    (TexturePtr)gThreeDayClock3DSFinalHoursDigit0Tex, (TexturePtr)gThreeDayClock3DSFinalHoursDigit1Tex,
    (TexturePtr)gThreeDayClock3DSFinalHoursDigit2Tex, (TexturePtr)gThreeDayClock3DSFinalHoursDigit3Tex,
    (TexturePtr)gThreeDayClock3DSFinalHoursDigit4Tex, (TexturePtr)gThreeDayClock3DSFinalHoursDigit5Tex,
    (TexturePtr)gThreeDayClock3DSFinalHoursDigit6Tex, (TexturePtr)gThreeDayClock3DSFinalHoursDigit7Tex,
    (TexturePtr)gThreeDayClock3DSFinalHoursDigit8Tex, (TexturePtr)gThreeDayClock3DSFinalHoursDigit9Tex,
    (TexturePtr)gThreeDayClock3DSFinalHoursColonTex,
};

s16 finalHoursClockSlots[8];

void Draw3DSClock() {
    HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
    if (HudEditor_IsActiveElementHidden()) {
        return;
    }

    static s32 sPreviousTimeCheck = -1;

    if ((R_TIME_SPEED != 0) &&
        ((gPlayState->msgCtx.msgMode == MSGMODE_NONE) ||
         ((gPlayState->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) && !Play_InCsMode(gPlayState)) ||
         (gPlayState->msgCtx.msgMode == MSGMODE_NONE) ||
         ((gPlayState->msgCtx.currentTextId >= 0x100) && (gPlayState->msgCtx.currentTextId <= 0x200)) ||
         (gSaveContext.gameMode == GAMEMODE_END_CREDITS)) &&
        !FrameAdvance_IsEnabled(gPlayState) && !Environment_IsTimeStopped() && (gSaveContext.save.day <= 3)) {

        if ((gPlayState->pauseCtx.state == PAUSE_STATE_OFF) &&
            (gPlayState->pauseCtx.debugEditor == DEBUG_EDITOR_NONE)) {

            static s16 sThreeDayClockAlpha = 255;

            static s32 sFinalHoursIntro = 0;

            if (gPlayState->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) {
                sThreeDayClockAlpha = 255;
            } else {
                sThreeDayClockAlpha = gPlayState->interfaceCtx.bAlpha;
            }

            s16 posX = 160;
            s16 posY = 210;

            OPEN_DISPS(gPlayState->state.gfxCtx);

            Gfx_SetupDL42_Overlay(gPlayState->state.gfxCtx);
            gDPSetRenderMode(OVERLAY_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetAlphaCompare(OVERLAY_DISP++, G_AC_THRESHOLD);

            // Draw background of 3DS clock
            // TODO: Replace this with matrix/vertex handling to avoid gaps when scaled
            OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadow(
                OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSEdgeTex, CLOCK_SECTION_HALFWIDTH * 3, 12,
                posX - (CLOCK_SECTION_HALFWIDTH * 4), posY, CLOCK_SECTION_HALFWIDTH * 3, 12, 1 << 10, 1 << 10, 255, 255,
                255, sThreeDayClockAlpha);

            HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
            OVERLAY_DISP =
                Gfx_DrawTexRectIA8_DropShadow(OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSMiddleTex, CLOCK_SECTION_WIDTH,
                                              12, posX - CLOCK_SECTION_HALFWIDTH, posY, CLOCK_SECTION_WIDTH, 12,
                                              1 << 10, 1 << 10, 255, 255, 255, sThreeDayClockAlpha);

            HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
            OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadowOffset(
                OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSEdgeTex, CLOCK_SECTION_HALFWIDTH * 3, 12,
                posX + CLOCK_SECTION_HALFWIDTH, posY, CLOCK_SECTION_HALFWIDTH * 3, 12, 1 << 10, 1 << 10, 255, 255, 255,
                sThreeDayClockAlpha, 3, (CLOCK_SECTION_HALFWIDTH * 3) << 5);

            // Coloured day markers, which highlights the current day with its own colour
            u16 fillalpha = 64;
            if (gSaveContext.save.day <= 1) {
                fillalpha = 255;
            }
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 128, 255, (fillalpha * sThreeDayClockAlpha) / 255);

            HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
            OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSFillTex, CLOCK_SECTION_WIDTH,
                                              12, posX - CLOCK_SECTION_HALFWIDTH - CLOCK_SECTION_WIDTH, posY,
                                              CLOCK_SECTION_WIDTH, 12, 1 << 10, 1 << 10);

            fillalpha = 64;
            if (gSaveContext.save.day == 2) {
                fillalpha = 255;
            }
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 192, 0, (fillalpha * sThreeDayClockAlpha) / 255);

            HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
            OVERLAY_DISP =
                Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSFillTex, CLOCK_SECTION_WIDTH, 12,
                                   posX - CLOCK_SECTION_HALFWIDTH, posY, CLOCK_SECTION_WIDTH, 12, 1 << 10, 1 << 10);

            fillalpha = 64;
            if (gSaveContext.save.day >= 3) {
                fillalpha = 255;
            }
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 64, 0, (fillalpha * sThreeDayClockAlpha) / 255);

            HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
            OVERLAY_DISP =
                Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSFillTex, CLOCK_SECTION_WIDTH, 12,
                                   posX + CLOCK_SECTION_HALFWIDTH, posY, CLOCK_SECTION_WIDTH, 12, 1 << 10, 1 << 10);

            // This next part is for the little arrow that shows what the current time is

            s32 timeUntilCrash = (s32)TIME_TO_SECONDS_F(TIME_UNTIL_MOON_CRASH);

            // This is a safety measure to delay skipping the arrow marker back a day on the frame before the
            // day changes by just checking if time went backwards
            if (sPreviousTimeCheck != -1 && sPreviousTimeCheck > SECONDS_IN_THREE_DAYS - timeUntilCrash) {
                s32 trueTimeUntilCrash = timeUntilCrash;
                timeUntilCrash = SECONDS_IN_THREE_DAYS - sPreviousTimeCheck;
                sPreviousTimeCheck = SECONDS_IN_THREE_DAYS - trueTimeUntilCrash;
            } else {
                sPreviousTimeCheck = SECONDS_IN_THREE_DAYS - timeUntilCrash;
            }

            // Compute the position of the arrow, and then draw it
            s32 timeoffset = std::max(
                std::min(3 * CLOCK_SECTION_WIDTH - (timeUntilCrash * 3 * CLOCK_SECTION_WIDTH) / SECONDS_IN_THREE_DAYS,
                         3 * CLOCK_SECTION_WIDTH),
                0);

            u16 counterX = posX - (CLOCK_SECTION_HALFWIDTH * 3) + timeoffset;
            u16 counterY = posY - 4;

            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 0, 0, sThreeDayClockAlpha);

            HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
            OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSArrowTex, 8, 8, counterX - 4,
                                              posY + 4, 8, 8, 1 << 10, 1 << 10);

            u16 curMinutes = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) % 60;
            u16 curHours = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) / 60;

            // Final hours timer
            if ((CURRENT_DAY >= 4) ||
                ((CURRENT_DAY == 3) && (CURRENT_TIME >= (CLOCK_TIME(0, 0) + 5)) && (CURRENT_TIME < CLOCK_TIME(6, 0)))) {

                // This all just gets the time to display in each number slot
                s32 timeInSeconds = timeUntilCrash % 60;
                s32 timeInMinutes = (timeUntilCrash / 60) % 60;
                s32 timeInHours = (timeUntilCrash / 60) / 60;

                finalHoursClockSlots[0] = std::min(timeInHours / 10, 9);
                finalHoursClockSlots[1] = timeInHours % 10;
                finalHoursClockSlots[2] = 10;
                finalHoursClockSlots[3] = timeInMinutes / 10;
                finalHoursClockSlots[4] = timeInMinutes % 10;
                finalHoursClockSlots[5] = 10;

                finalHoursClockSlots[6] = timeInSeconds / 10;
                finalHoursClockSlots[7] = timeInSeconds % 10;

                u16 finalTimerSpacing = 8;

                u16 finalTimerPos = posX - finalTimerSpacing * 4 - finalTimerSpacing / 2;
                s16 i;

                s32 percentToCrash = std::min(std::max((timeUntilCrash * 255) / SECONDS_IN_SIX_HOURS, 0), 255);

                // This is a lerp between yellow and red based on the remaining time
                u16 finalHoursR1 = 255;
                u16 finalHoursR2 = 255;
                u16 finalHoursG1 = 0;
                u16 finalHoursG2 = 255;
                u16 finalHoursB1 = 0;
                u16 finalHoursB2 = 0;

                u16 finalHoursR = (((255 - percentToCrash) * finalHoursR1) + (percentToCrash * finalHoursR2)) / 255;
                u16 finalHoursG = (((255 - percentToCrash) * finalHoursG1) + (percentToCrash * finalHoursG2)) / 255;
                u16 finalHoursB = (((255 - percentToCrash) * finalHoursB1) + (percentToCrash * finalHoursB2)) / 255;

                // Controls the distance and speed modifier of the final hours clock intro animation
                // finalHoursOffset controls the distance offset for the intro animation
                // finalHoursModifier controls the speed of the intro animation - larger is slower
                s32 finalHoursOffset = 10;
                s32 finalHoursModifier = 2;
                if (sFinalHoursIntro < finalHoursOffset * finalHoursModifier) {
                    sFinalHoursIntro++;
                }

                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, finalHoursR, finalHoursG, finalHoursB,
                                (sThreeDayClockAlpha * sFinalHoursIntro) / (finalHoursOffset * finalHoursModifier));
                // This just loops through and draws all the numbers
                for (i = 0; i < 8; i++) {

                    HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                    OVERLAY_DISP = Gfx_DrawTexRectIA8(
                        OVERLAY_DISP, sFinalHoursDigitTextures[finalHoursClockSlots[i]], 16, 16, finalTimerPos,
                        posY - 14 - finalHoursOffset + (sFinalHoursIntro / finalHoursModifier), 16, 16, 1 << 10,
                        1 << 10);
                    finalTimerPos += finalTimerSpacing;
                }
                HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                OVERLAY_DISP = Gfx_DrawTexRectIA8(
                    OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSFinalHoursMoonTex, 16, 16, posX - 8,
                    posY - 28 - finalHoursOffset + (sFinalHoursIntro / finalHoursModifier), 16, 16, 1 << 10, 1 << 10);
            } else {

                sFinalHoursIntro = 0;

                // Draw the current time in a small box
                // TODO: Add scale slider to allow this box to be scaled independently of the main UI element

                // Draw the background box
                if (gSaveContext.save.timeSpeedOffset == -2) {
                    u16 pulseTime = ((s32)TIME_TO_SECONDS_F(CURRENT_TIME) % 120);
                    u16 pulse;
                    if (pulseTime < 60) {
                        pulse = (pulseTime * 255) / 60;
                    } else {
                        pulse = 255 - ((pulseTime - 60) * 255) / 60;
                    }

                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, pulse, pulse, 0, sThreeDayClockAlpha / 2);
                } else {
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, sThreeDayClockAlpha / 2);
                }
                HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSTimeBackdropTex,
                                                  CLOCK_SECTION_WIDTH, 16, counterX - CLOCK_SECTION_HALFWIDTH,
                                                  counterY - 4, CLOCK_SECTION_WIDTH, 16, 1 << 10, 1 << 10);

                counterX -= 8;

                // Draw the sun/moon next to the current time
                TexturePtr daynightmarker;
                if (curHours < 6 || curHours >= 18) {
                    daynightmarker = (TexturePtr)gThreeDayClockMoonHourTex;
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 200, 0, sThreeDayClockAlpha);
                } else {
                    daynightmarker = (TexturePtr)gThreeDayClockSunHourTex;
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 64, 0, sThreeDayClockAlpha);
                }
                HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                OVERLAY_DISP = Gfx_DrawTexRectI8(OVERLAY_DISP, daynightmarker, 24, 24, counterX - 11, counterY - 2, 12,
                                                 12, 1 << 11, 1 << 11);
                if (!CVarGetInteger("gEnhancements.Graphics.24HoursClock", 0)) {
                    curHours %= 12;
                    if (curHours == 0) {
                        curHours = 12;
                    }
                }

                u16 curTensHours = curHours / 10;
                curHours %= 10;

                u16 curTensMinutes = curMinutes / 10;
                curMinutes %= 10;

                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, sThreeDayClockAlpha);

                // Inverted time
                if (gSaveContext.save.timeSpeedOffset == -2) {
                    HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                    OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gThreeDayClock3DSSlowTimeTex, 16, 16,
                                                      counterX + 8 + 20, counterY - 4, 16, 16, 1 << 10, 1 << 10);

                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 64, 192, 255, sThreeDayClockAlpha);
                }

                // Digital time
                u16 timerSpacing = 6;

                if (curTensHours > 0 || CVarGetInteger("gEnhancements.Graphics.24HoursClock", 0)) {
                    timerSpacing = 4;

                    HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                    OVERLAY_DISP = Gfx_DrawTexRectI8(OVERLAY_DISP, sDigitTextures[curTensHours], 8, 8, counterX,
                                                     counterY, 8, 8, 1 << 10, 1 << 10);
                    counterX += timerSpacing;
                }

                HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                OVERLAY_DISP = Gfx_DrawTexRectI8(OVERLAY_DISP, sDigitTextures[curHours], 8, 8, counterX, counterY, 8, 8,
                                                 1 << 10, 1 << 10);
                counterX += timerSpacing;

                HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                OVERLAY_DISP = Gfx_DrawTexRectI8(OVERLAY_DISP, sDigitTextures[10], 8, 8, counterX, counterY, 8, 8,
                                                 1 << 10, 1 << 10);
                counterX += timerSpacing;

                HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                OVERLAY_DISP = Gfx_DrawTexRectI8(OVERLAY_DISP, sDigitTextures[curTensMinutes], 8, 8, counterX, counterY,
                                                 8, 8, 1 << 10, 1 << 10);
                counterX += timerSpacing;

                HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_CLOCK);
                OVERLAY_DISP = Gfx_DrawTexRectI8(OVERLAY_DISP, sDigitTextures[curMinutes], 8, 8, counterX, counterY, 8,
                                                 8, 1 << 10, 1 << 10);
            }
            gDPPipeSync(OVERLAY_DISP++);

            CLOSE_DISPS(gPlayState->state.gfxCtx);
        } else {
            sPreviousTimeCheck = SECONDS_IN_THREE_DAYS - (s32)TIME_TO_SECONDS_F(TIME_UNTIL_MOON_CRASH);
        }
    } else {
        sPreviousTimeCheck = SECONDS_IN_THREE_DAYS - (s32)TIME_TO_SECONDS_F(TIME_UNTIL_MOON_CRASH);
    }

    hudEditorActiveElement = HUD_EDITOR_ELEMENT_NONE;
}

void Register3DSClock() {
    COND_VB_SHOULD(VB_PREVENT_CLOCK_DISPLAY, CVAR == CLOCK_TYPE_3DS, {
        *should = true;
        Draw3DSClock();
    });
}

static RegisterShipInitFunc initFunc(Register3DSClock, { CVAR_NAME });

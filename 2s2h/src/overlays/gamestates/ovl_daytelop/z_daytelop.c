/*
 * File: z_daytelop.c
 * Overlay: ovl_daytelop
 * Description: "Dawn of ... day" screen
 */

#include "global.h"
#include "z_daytelop.h"
#include "z64save.h"
#include "z64shrink_window.h"
#include "misc/daytelop_static/daytelop_static.h"
#include "interface/icon_item_gameover_static/icon_item_gameover_static.h"

#include "BenPort.h"
#include "assets/2s2h_assets.h"

// unused
UNK_TYPE D_808158E0[] = {
    0x00000C00, 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C04, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00100000, 0x00000000, 0x00000000, 0x40000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x04000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F, 0x00000000, 0x00000000, 0x00000000,
    0x0070B000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x80000000, 0x00000000, 0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01A00020, 0x00000000, 0x00000000,
    0x00000000, 0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00001000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000010, 0x00000000, 0x00000000, 0x00000000, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x00100400, 0x00000000, 0x00000000, 0x00000022, 0x00000400,
    0x00000000, 0x00000000, 0x00000080, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x80000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x00000C00, 0x00000000, 0x00000000,
    0x40000000, 0x00000102, 0x00000000, 0x00000000, 0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x40000000, 0x00000070, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x40000000, 0x00000000, 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00100000, 0x00000000, 0x00000000, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00000400, 0x00000000, 0x00000000, 0x00000000,
    0x00100000, 0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000100, 0x00000400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000080, 0x00000000, 0x00000000,
    0x00000000, 0x00000080, 0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000400, 0x00100000, 0x00000000, 0x00000000, 0x00000400, 0x00000000, 0x00000000,
    0x00000001, 0x00000000,
};

void DayTelop_Update(DayTelopState* this, GameState* thisx) {
    static u8 D_80815FF0 = 0;
    s16 new_var;
    u8 temp_v0_2;

    this->transitionCountdown--;
    if (this->transitionCountdown == 0) {
        if (gSaveContext.save.day < 9) {
            gSaveContext.gameMode = GAMEMODE_NORMAL;
        } else {
            gSaveContext.nextCutsceneIndex = 0xFFF6;
            gSaveContext.save.day = 1;
        }

        STOP_GAMESTATE(&this->state);
        SET_NEXT_GAMESTATE(&this->state, Play_Init, sizeof(PlayState));

        gSaveContext.save.time = CLOCK_TIME(6, 0);
        gSceneSeqState = SCENESEQ_MORNING;
    } else if (this->transitionCountdown == 90) {
        this->fadeInState = DAYTELOP_HOURSTEXT_FADEIN;
        this->alpha = 0;
        D_80815FF0 = 30;
    }

    if (this->fadeInState == DAYTELOP_HOURSTEXT_FADEIN) {
        this->alpha += (s16)(ABS_ALT(this->alpha - 255) / D_80815FF0);

        D_80815FF0--;
        if (D_80815FF0 == 0) {
            this->fadeInState = DAYTELOP_HOURSTEXT_ON;
            this->alpha = 255;
        }
    }
}

TexturePtr sDayLeftTextures[] = {
    gDaytelopFirstDayLeftNESTex,
    gDaytelopSecondDayLeftNESTex,
    gDaytelopFinalDayLeftNESTex,

    // Last element is the new day
    gDaytelopNewDayLeftNESTex,
};

TexturePtr sDayRightTextures[] = {
    gDaytelopFirstDayRightNESTex,
    gDaytelopSecondDayRightNESTex,
    gDaytelopFinalDayRightNESTex,

    // Last element is the new day
    gDaytelopNewDayRightNESTex,
};

TexturePtr sHoursLeftTextures[] = {
    gDaytelop72HoursNESTex,
    gDaytelop48HoursNESTex,
    gDaytelop24HoursNESTex,

    // 2S2H [Port] Added empty texture to support "Dawn of" with the 4th day glitch and prevent crashing
    gEmptyTexture,
};

void DayTelop_Draw(DayTelopState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;

    // 2S2H [Port] Exit early for day 0 or less to avoid an OOB read on the texture arrays.
    // This is only possible when using the save editor while the "Dawn of" screen is up.
    if (CURRENT_DAY <= 0) {
        return;
    }

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL39_Opa(this->state.gfxCtx);

    if (gSaveContext.save.day >= 9) {
        // Draw a white screen
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        // #region 2S2H [Cosmetic] Account for different aspect ratios than 4:3
        gDPFillWideRectangle(POLY_OPA_DISP++, OTRGetRectDimensionFromLeftEdge(0), 0,
                             OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH), SCREEN_HEIGHT);
    }

    Gfx_SetupDL39_Opa(this->state.gfxCtx);

    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    if (gSaveContext.save.day >= 9) {
        gDPSetAlphaCompare(POLY_OPA_DISP++, G_AC_NONE);
    }

    // Draw the left side of the "Dawn of" texture
    if (gSaveContext.save.day < 9) {
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sDayLeftTextures[CURRENT_DAY - 1], G_IM_FMT_I, 128, 64, 0,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
    } else {
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sDayLeftTextures[ARRAY_COUNT(sDayLeftTextures) - 1], G_IM_FMT_I, 128,
                               64, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
    }
    gSPTextureRectangle(POLY_OPA_DISP++, 32 << 2, 77 << 2, (32 + 128) << 2, (77 + 64) << 2, G_TX_RENDERTILE, 0, 0,
                        0x0400, 0x0400);

    // Draw the right side of the "Dawn of" texture
    if (gSaveContext.save.day < 9) {
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sDayRightTextures[CURRENT_DAY - 1], G_IM_FMT_I, 128, 64, 0,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
    } else {
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sDayRightTextures[ARRAY_COUNT(sDayRightTextures) - 1], G_IM_FMT_I, 128,
                               64, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
    }
    gSPTextureRectangle(POLY_OPA_DISP++, 160 << 2, 77 << 2, (160 + 128) << 2, (77 + 64) << 2, G_TX_RENDERTILE, 0, 0,
                        0x0400, 0x0400);

    // Draw the "Hours left" texture
    if (gSaveContext.save.day < 9) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->alpha);
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sHoursLeftTextures[CURRENT_DAY - 1], G_IM_FMT_I, 144, 32, 0,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(POLY_OPA_DISP++, 88 << 2, 144 << 2, (88 + 144) << 2, (144 + 32) << 2, G_TX_RENDERTILE, 0, 0,
                            0x0400, 0x0400);
    }

    CLOSE_DISPS(gfxCtx);
}

void DayTelop_Main(GameState* thisx) {
    DayTelopState* this = (DayTelopState*)thisx;

    func_8012CF0C(this->state.gfxCtx, true, true, 0, 0, 0);

    OPEN_DISPS(this->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x09, this->daytelopStaticFile);
    gSPSegment(POLY_OPA_DISP++, 0x0C, this->gameoverStaticFile);
    CLOSE_DISPS(this->state.gfxCtx);

    DayTelop_Draw(this);
    DayTelop_Update(this, &this->state);
}

void DayTelop_Destroy(GameState* thisx) {
    ShrinkWindow_Destroy();
}

void DayTelop_Noop(DayTelopState* this) {
}

void DayTelop_LoadGraphics(DayTelopState* this) {
    size_t segmentSize = SEGMENT_ROM_SIZE(daytelop_static);

    this->daytelopStaticFile = THA_AllocTailAlign16(&this->state.tha, segmentSize);
    DmaMgr_SendRequest0(this->daytelopStaticFile, SEGMENT_ROM_START(daytelop_static), segmentSize);

    segmentSize = SEGMENT_ROM_SIZE(icon_item_gameover_static);
    this->gameoverStaticFile = THA_AllocTailAlign16(&this->state.tha, segmentSize);
    DmaMgr_SendRequest0(this->gameoverStaticFile, SEGMENT_ROM_START(icon_item_gameover_static), segmentSize);
}

void DayTelop_Init(GameState* thisx) {
    DayTelopState* this = (DayTelopState*)thisx;

    GameState_SetFramerateDivisor(&this->state, 1);
    Matrix_Init(&this->state);
    ShrinkWindow_Destroy();
    View_Init(&this->view, this->state.gfxCtx);
    this->state.main = DayTelop_Main;
    this->state.destroy = DayTelop_Destroy;
    this->transitionCountdown = 140;
    this->fadeInState = DAYTELOP_HOURSTEXT_OFF;

    if (gSaveContext.save.day < 9) {
        if (gSaveContext.save.day == 0) {
            Sram_ClearFlagsAtDawnOfTheFirstDay();
        }
        Sram_IncrementDay();
    }

    DayTelop_Noop(this);
    DayTelop_LoadGraphics(this);
    Audio_PlaySfx(NA_SE_OC_TELOP_IMPACT);
}

#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

#include "textures/nintendo_rogo_static/nintendo_rogo_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "soh_assets.h"

extern "C" {
#include "macros.h"
#include "z64.h"
#include "functions.h"
#include "variables.h"
#include "soh/Enhancements/enhancementTypes.h"
}

extern "C" {
extern void Title_Calc(TitleContext*);
extern void Title_SetupView(TitleContext*, f32, f32, f32);
}

#define LOGO_TO_DRAW_LUS 0
#define LOGO_TO_DRAW_N64 1

static bool shouldDrawIceOnSpinningLogo = false;

extern "C" void CustomLogoTitle_Draw(TitleContext* titleContext, uint8_t logoToDraw) {
    static s16 sTitleRotY = 0;
    static Lights1 sTitleLights = gdSPDefLights1(0x64, 0x64, 0x64, 0xFF, 0xFF, 0xFF, 0x45, 0x45, 0x45);

    u16 y;
    u16 idx;
    s32 pad1;
    Vec3f v3;
    Vec3f v1;
    Vec3f v2;
    s32 pad2[2];

    OPEN_DISPS(titleContext->state.gfxCtx);

    v3.x = 69;
    v3.y = 69;
    v3.z = 69;
    v2.x = -4949.148;
    v2.y = 4002.5417;
    v1.x = 0;
    v1.y = 0;
    v1.z = 0;
    v2.z = 1119.0837;

    func_8002EABC(&v1, &v2, &v3, titleContext->state.gfxCtx);
    gSPSetLights1(POLY_OPA_DISP++, sTitleLights);
    Title_SetupView(titleContext, 0, 150.0, 300.0);
    Gfx_SetupDL_25Opa(titleContext->state.gfxCtx);
    Matrix_Translate(-53.0, -5.0, 0, MTXMODE_NEW);
    Matrix_Scale(1.0, 1.0, 1.0, MTXMODE_APPLY);
    Matrix_RotateZYX(0, sTitleRotY, 0, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(titleContext->state.gfxCtx), G_MTX_LOAD);

    if (logoToDraw == LOGO_TO_DRAW_LUS) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gShipLogoDL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gNintendo64LogoDL);
    }

    Gfx_SetupDL_39Opa(titleContext->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF2, G_RM_OPA_CI | CVG_DST_WRAP);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
        COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
    if (CVarGetInteger(CVAR_COSMETIC("Title.NintendoLogo.Changed"), 0)) {
        Color_RGB8 nintendoLogoColor = CVarGetColor24(CVAR_COSMETIC("Title.NintendoLogo.Value"), Color_RGB8{0, 0, 255});
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, nintendoLogoColor.r, nintendoLogoColor.g, nintendoLogoColor.b, 128);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 170, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 255, 128);
    }

    gDPLoadMultiBlock(POLY_OPA_DISP++, nintendo_rogo_static_Tex_001800, 0x100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 11);

    for (idx = 0, y = 94; idx < 16; idx++, y += 2)
    {
        gDPLoadMultiTile(POLY_OPA_DISP++, (logoToDraw == LOGO_TO_DRAW_N64) ? nintendo_rogo_static_Tex_000000 : nintendo_rogo_static_Tex_LUS_000000, 0, G_TX_RENDERTILE, G_IM_FMT_I, G_IM_SIZ_8b, 192, 32,
                         0, idx * 2, 192 - 1, (idx + 1) * 2 - 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                         G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        
        gDPSetTileSize(POLY_OPA_DISP++, 0, 0, 0, (192 - 1) << G_TEXTURE_IMAGE_FRAC,
                       (2 - 1) << G_TEXTURE_IMAGE_FRAC);

        gDPSetTileSize(POLY_OPA_DISP++, 1, titleContext->uls, (titleContext->ult & 0x7F) - idx * 4, 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 388, y << 2, 1156, (y + 2) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    // Draw ice block around spinning N or ship.
    if (shouldDrawIceOnSpinningLogo) {
        f32 scale = 0.4f;

        gSPSegment(POLY_OPA_DISP++, 0x08,
                    (uintptr_t)Gfx_TwoTexScroll(titleContext->state.gfxCtx, 0, 0, (0 - 1) % 128, 32, 32, 1,
                                    0, (1 * -2) % 128, 32, 32));

        Matrix_Translate(0.0f, -10.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(titleContext->state.gfxCtx),
                    G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 50, 100, 255);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gEffIceFragment3DL);
    }

    Environment_FillScreen(titleContext->state.gfxCtx, 0, 0, 0, (s16)titleContext->coverAlpha, FILL_SCREEN_XLU);

    sTitleRotY += (300 * CVarGetFloat(CVAR_COSMETIC("N64Logo.SpinSpeed"), 1.0f));

    CLOSE_DISPS(titleContext->state.gfxCtx);
}

#define CVAR_BOOTSEQUENCE_NAME CVAR_ENHANCEMENT("BootSequence")
#define CVAR_BOOTSEQUENCE_DEFAULT BOOTSEQUENCE_DEFAULT
#define CVAR_BOOTSEQUENCE_VALUE CVarGetInteger(CVAR_BOOTSEQUENCE_NAME, CVAR_BOOTSEQUENCE_DEFAULT)

extern "C" void CustomLogoTitle_Main(TitleContext* titleContext) {
    static uint8_t logosSeen = 0;
    uint8_t logoToDraw;

    if (CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_DEFAULT) {
        if (logosSeen == 0) {
            logoToDraw = LOGO_TO_DRAW_LUS;
        } else {
            logoToDraw = LOGO_TO_DRAW_N64;
        }
    }

    if (CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_AUTHENTIC) {
        logoToDraw = LOGO_TO_DRAW_N64;
    }

    OPEN_DISPS(titleContext->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0, (uintptr_t)NULL);
    gSPSegment(POLY_OPA_DISP++, 1, (uintptr_t)titleContext->staticSegment);
    Gfx_SetupFrame(titleContext->state.gfxCtx, 0, 0, 0);
    Title_Calc(titleContext);
    CustomLogoTitle_Draw(titleContext, logoToDraw);

    if (titleContext->exit) {
        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = GAMEMODE_TITLE_SCREEN;
        titleContext->state.running = false;
        
        logosSeen++;

        if (CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_DEFAULT && logosSeen == 1) {
            SET_NEXT_GAMESTATE(&titleContext->state, Title_Init, TitleContext);
        }

        if ((CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_DEFAULT && logosSeen == 2) ||
            (CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_AUTHENTIC)) {
            SET_NEXT_GAMESTATE(&titleContext->state, Opening_Init, OpeningContext);
            logosSeen = 0;
        }
    }

    GameInteractor_ExecuteOnZTitleUpdate(titleContext);

    CLOSE_DISPS(titleContext->state.gfxCtx);
}

// // // //
// Always
// 

void OnZTitleInitReplaceTitleMainWithCustom(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;
    titleContext->state.main = (GameStateFunc)CustomLogoTitle_Main;
}

// Allows pressing A to skip the boot logo and go to the next state (opening or file select)
void OnZTitleUpdatePressButtonToSkip(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;

    if (CHECK_BTN_ANY(titleContext->state.input->press.button, BTN_A | BTN_B | BTN_START)) {
        // Force the title state to start fading to black and to last roughly 5 frames based on current fade in/out
        titleContext->visibleDuration = 0;
        titleContext->addAlpha = std::max<int16_t>((255 - titleContext->coverAlpha) / 5, 1);
    }
}

void RegisterCustomLogoTitle() {
    COND_HOOK(OnZTitleInit, true, OnZTitleInitReplaceTitleMainWithCustom);
    COND_HOOK(OnZTitleUpdate, true, OnZTitleUpdatePressButtonToSkip);
}

static RegisterShipInitFunc initFuncAlways(RegisterCustomLogoTitle);

// // // // // //
// Bootsequence
// 

void OnZTitleUpdateSkipToFileSelect(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;

    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.gameMode = GAMEMODE_FILE_SELECT;
    titleContext->state.running = false;

    SET_NEXT_GAMESTATE(&titleContext->state, FileChoose_Init, FileChooseContext);
}

void RegisterCustomLogoTitleBootsequence() {
    COND_HOOK(OnZTitleUpdate, CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_FILESELECT, OnZTitleUpdateSkipToFileSelect);
}

static RegisterShipInitFunc initFuncBootsequence(RegisterCustomLogoTitleBootsequence, { CVAR_BOOTSEQUENCE_NAME });

// // // // // //
// Let it Snow
// 

#define CVAR_LETITSNOW_NAME CVAR_GENERAL("LetItSnow")
#define CVAR_LETITSNOW_DEFAULT 0
#define CVAR_LETITSNOW_VALUE CVarGetInteger(CVAR_LETITSNOW_NAME, CVAR_LETITSNOW_DEFAULT)

void RegisterCustomLogoTitleLetItSnow() {
    shouldDrawIceOnSpinningLogo = CVAR_LETITSNOW_VALUE != CVAR_LETITSNOW_DEFAULT;
}

static RegisterShipInitFunc initFuncLetItSnow(RegisterCustomLogoTitleLetItSnow, { CVAR_LETITSNOW_NAME });

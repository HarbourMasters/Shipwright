#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

#include "textures/nintendo_rogo_static/nintendo_rogo_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

extern "C" {
#include "macros.h"
#include "z64.h"
#include "functions.h"
#include "variables.h"
}

extern "C" {
extern void Title_Calc(TitleContext*);
extern void Title_SetupView(TitleContext*, f32, f32, f32);
}

#define dgShipLogoDL "__OTR__textures/nintendo_rogo_static/gShipLogoDL"
static const ALIGN_ASSET(2) char gShipLogoDL[] = dgShipLogoDL;

#define dnintendo_rogo_static_Tex_LUS_000000 "__OTR__textures/nintendo_rogo_static/nintendo_rogo_static_Tex_LUS_000000"
static const ALIGN_ASSET(2) char nintendo_rogo_static_Tex_LUS_000000[] = dnintendo_rogo_static_Tex_LUS_000000;

extern "C" void CustomLogoTitle_Draw(TitleContext* titleContext) {
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
    if (CVarGetInteger(CVAR_ENHANCEMENT("AuthenticLogo"), 0)) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gNintendo64LogoDL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gShipLogoDL);
    }
    Gfx_SetupDL_39Opa(titleContext->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF2, G_RM_OPA_CI | CVG_DST_WRAP);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
        COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
    if (CVarGetInteger(CVAR_COSMETIC("Title.NintendoLogo.Changed"), 0)) {
        Color_RGB8 nintendoLogoColor = CVarGetColor24(CVAR_COSMETIC("Title.NintendoLogo.Value"), (Color_RGB8){0, 0, 255});
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
        gDPLoadMultiTile(POLY_OPA_DISP++, CVarGetInteger(CVAR_ENHANCEMENT("AuthenticLogo"), 0) ? nintendo_rogo_static_Tex_000000 : nintendo_rogo_static_Tex_LUS_000000, 0, G_TX_RENDERTILE, G_IM_FMT_I, G_IM_SIZ_8b, 192, 32,
                         0, idx * 2, 192 - 1, (idx + 1) * 2 - 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                         G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        
        gDPSetTileSize(POLY_OPA_DISP++, 0, 0, 0, (192 - 1) << G_TEXTURE_IMAGE_FRAC,
                       (2 - 1) << G_TEXTURE_IMAGE_FRAC);

        gDPSetTileSize(POLY_OPA_DISP++, 1, titleContext->uls, (titleContext->ult & 0x7F) - idx * 4, 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 388, y << 2, 1156, (y + 2) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    // Draw ice block around spinning N or ship.
    if (CVarGetInteger(CVAR_GENERAL("LetItSnow"), 0)) {
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

extern "C" void CustomLogoTitle_Main(TitleContext* titleContext) {
    OPEN_DISPS(titleContext->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0, (uintptr_t)NULL);
    gSPSegment(POLY_OPA_DISP++, 1, (uintptr_t)titleContext->staticSegment);
    Gfx_SetupFrame(titleContext->state.gfxCtx, 0, 0, 0);
    Title_Calc(titleContext);
    CustomLogoTitle_Draw(titleContext);

    if (titleContext->exit || CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0)) {
        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = 1;
        titleContext->state.running = false;

        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0))
            SET_NEXT_GAMESTATE(&titleContext->state, FileChoose_Init, FileChooseContext);
        else
            SET_NEXT_GAMESTATE(&titleContext->state, Opening_Init, OpeningContext);
    }

    GameInteractor_ExecuteOnZTitleUpdate(titleContext);

    CLOSE_DISPS(titleContext->state.gfxCtx);
}

// Allows pressing A to skip the boot logo and go to the next state (opening or file select)
void OnZTitleUpdateSkipLogoTitle(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;

    if (CHECK_BTN_ANY(titleContext->state.input->press.button, BTN_A | BTN_B | BTN_START)) {
        // Force the title state to start fading to black and to last roughly 5 frames based on current fade in/out
        titleContext->visibleDuration = 0;
        titleContext->addAlpha = (255 - titleContext->coverAlpha) / 5;
    }
}

void OnZTitleInitReplaceTitleMainWithCustom(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;
    titleContext->state.main = (GameStateFunc)CustomLogoTitle_Main;
}

void RegisterCustomLogoTitle() {
    COND_HOOK(OnZTitleInit, true, OnZTitleInitReplaceTitleMainWithCustom);
    COND_HOOK(OnZTitleUpdate, true, OnZTitleUpdateSkipLogoTitle);
}

static RegisterShipInitFunc initFunc(RegisterCustomLogoTitle);

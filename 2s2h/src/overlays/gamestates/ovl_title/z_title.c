/*
 * File: z_title.c
 * Overlay: ovl_title
 * Description: Nintendo 64 logo shown on startup
 */

#include "z_title.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "CIC6105.h"
#include "overlays/gamestates/ovl_opening/z_opening.h"
#include "misc/nintendo_rogo_static/nintendo_rogo_static.h"

#include "build.h"
#include "BenPort.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <stdlib.h>
#include <libultraship/bridge/consolevariablebridge.h>

#define dgShipLogoDL "__OTR__misc/nintendo_rogo_static/gShipLogoDL"
static const ALIGN_ASSET(2) char gShipLogoDL[] = dgShipLogoDL;

#define dgLUSLogoTextTex "__OTR__misc/nintendo_rogo_static/gLUSLogoTextTex"
static const ALIGN_ASSET(2) char gLUSLogoTextTex[] = dgLUSLogoTextTex;

const char* GetGameVersionString() {
    uint32_t gameVersion = ResourceMgr_GetGameVersion(0);
    switch (gameVersion) {
        case MM_NTSC_US_10:
            return "MM-US 1.0";
        case MM_NTSC_US_GC:
            return "MM-US GC";
        default:
            return "UNKNOWN";
    }
}

void ConsoleLogo_PrintBuildInfo(ConsoleLogoState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    GfxPrint printer;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL28_Opa(gfxCtx);

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, POLY_OPA_DISP);
    GfxPrint_SetColor(&printer, 131, 154, 255, 255);

    // if tag is empty (not a release build)
    bool showGitInfo = gGitCommitTag[0] == 0;

    if (showGitInfo) {
        GfxPrint_SetPos(&printer, 1, 23);
        GfxPrint_Printf(&printer, "%s", gBuildVersion);

        GfxPrint_SetPos(&printer, 1, 24);
        GfxPrint_Printf(&printer, "Git Branch: %s", gGitBranch);

        GfxPrint_SetPos(&printer, 1, 25);
        GfxPrint_Printf(&printer, "Git Commit: %s", gGitCommitHash);
    } else {
        GfxPrint_SetPos(&printer, 1, 25);
        GfxPrint_Printf(&printer, "%s", gBuildVersion);
    }
    GfxPrint_SetPos(&printer, 1, 26);
    GfxPrint_Printf(&printer, "%s", gBuildDate);

    GfxPrint_SetPos(&printer, 29, 26);
    GfxPrint_Printf(&printer, "%s", GetGameVersionString());

    POLY_OPA_DISP = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);

    CLOSE_DISPS(gfxCtx);
}

void ConsoleLogo_UpdateCounters(ConsoleLogoState* this) {
    if ((this->coverAlpha == 0) && (this->visibleDuration != 0)) {
        this->timer--;
        this->visibleDuration--;

        if (this->timer == 0) {
            this->timer = 400;
        }
    } else {
        this->coverAlpha += this->addAlpha;

        if (this->coverAlpha <= 0) {
            this->coverAlpha = 0;
            this->addAlpha = 12;
        } else if (this->coverAlpha >= 255) {
            this->coverAlpha = 255;
            this->exit = true;
        }
    }

    this->uls = this->ult & 0x7F;
    this->ult++;
}

void ConsoleLogo_RenderView(ConsoleLogoState* this, f32 x, f32 y, f32 z) {
    View* view = &this->view;
    Vec3f eye;
    Vec3f at;
    Vec3f up;

    eye.x = x;
    eye.y = y;
    eye.z = z;
    up.x = up.z = 0.0f;
    at.x = at.y = at.z = 0.0f;
    up.y = 1.0f;
    View_SetPerspective(view, 30.0f, 10.0f, 12800.0f);
    View_LookAt(view, &eye, &at, &up);
    View_Apply(view, VIEW_ALL);
}

void ConsoleLogo_Draw(GameState* thisx) {
    static s16 sTitleRotation = 0;
    static Lights1 sTitleLights = gdSPDefLights1(100, 100, 100, 255, 255, 255, 69, 69, 69);
    ConsoleLogoState* this = (ConsoleLogoState*)thisx;
    u16 y;
    u16 idx;
    Vec3f lightDir;
    Vec3f object;
    Vec3f eye;
    s32 pad[2];

    char* logoDL = gNintendo64LogoNDL;
    char* logoText = gNintendo64LogoTextTex;

    if (!CVarGetInteger("gEnhancements.Graphics.AuthenticLogo", 0)) {
        logoDL = gShipLogoDL;
        logoText = gLUSLogoTextTex;
    }

    OPEN_DISPS(this->state.gfxCtx);

    lightDir.x = 69.0f;
    lightDir.y = 69.0f;
    lightDir.z = 69.0f;

    object.x = 0.0f;
    object.y = 0.0f;
    object.z = 0.0f;

    eye.x = -4949.148f;
    eye.y = 4002.5417f;
    eye.z = 1119.0837f;

    Hilite_DrawOpa(&object, &eye, &lightDir, this->state.gfxCtx);

    gSPSetLights1(POLY_OPA_DISP++, sTitleLights);

    ConsoleLogo_RenderView(this, 0.0f, 150.0f, 300.0f);
    Gfx_SetupDL25_Opa(this->state.gfxCtx);
    Matrix_Translate(-53.0f, -5.0f, 0.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(0, sTitleRotation, 0, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gNintendo64LogoNDL);

    Gfx_SetupDL39_Opa(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_CLD_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
                      COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 255, 128);
    gDPLoadMultiBlock(POLY_OPA_DISP++, gNintendo64LogoTextShineTex, 0x100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                      G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 11);

    for (idx = 0, y = 94; idx < 16; idx++, y += 2) {
        // #region 2S2H [Port] Use LoadMultiTile so we can pass the resource path and control which bytes are loaded
        gDPLoadMultiTile(POLY_OPA_DISP++, logoText, 0, G_TX_RENDERTILE, G_IM_FMT_I, G_IM_SIZ_8b, 192, 32, 0, idx * 2,
                         192 - 1, (idx + 1) * 2 - 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gDPSetTileSize(POLY_OPA_DISP++, 0, 0, 0, (192 - 1) << G_TEXTURE_IMAGE_FRAC, (2 - 1) << G_TEXTURE_IMAGE_FRAC);
        // #endregion

        gDPSetTileSize(POLY_OPA_DISP++, 1, this->uls, (this->ult & 0x7F) - idx * 4, 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 97 << 2, y << 2, (97 + 192) << 2, (y + 2) << 2, G_TX_RENDERTILE, 0, 0,
                            1 << 10, 1 << 10);
    }

    if (!CVarGetInteger("gEnhancements.Graphics.AuthenticLogo", 0)) {
        ConsoleLogo_PrintBuildInfo(this);
    }

    Environment_FillScreen(this->state.gfxCtx, 0, 0, 0, this->coverAlpha, FILL_SCREEN_XLU);

    sTitleRotation += 300;

    CLOSE_DISPS(this->state.gfxCtx);
}

void ConsoleLogo_Main(GameState* thisx) {
    ConsoleLogoState* this = (ConsoleLogoState*)thisx;

    func_8012CF0C(this->state.gfxCtx, true, true, 0, 0, 0);

    OPEN_DISPS(this->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x01, this->staticSegment);

    ConsoleLogo_UpdateCounters(this);
    FrameInterpolation_StartRecord();
    ConsoleLogo_Draw(&this->state);
    FrameInterpolation_StopRecord();

    if (this->exit) {
        gSaveContext.seqId = NA_BGM_DISABLED;
        gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
        gSaveContext.gameMode = GAMEMODE_TITLE_SCREEN;

        STOP_GAMESTATE(&this->state);
        SET_NEXT_GAMESTATE(&this->state, TitleSetup_Init, sizeof(TitleSetupState));
    }

    GameInteractor_ExecuteOnConsoleLogoUpdate();

    CLOSE_DISPS(this->state.gfxCtx);
}

void ConsoleLogo_Destroy(GameState* thisx) {
    ConsoleLogoState* this = (ConsoleLogoState*)thisx;

    Sram_InitSram(&this->state, &this->sramCtx);
    ShrinkWindow_Destroy();
    CIC6105_Noop2();
}

void ConsoleLogo_Init(GameState* thisx) {
    ConsoleLogoState* this = (ConsoleLogoState*)thisx;
    uintptr_t segmentSize = SEGMENT_ROM_SIZE(nintendo_rogo_static);

    this->staticSegment = THA_AllocTailAlign16(&this->state.tha, segmentSize);
    DmaMgr_SendRequest0(this->staticSegment, SEGMENT_ROM_START(nintendo_rogo_static), segmentSize);

    GameState_SetFramerateDivisor(&this->state, 1);
    Matrix_Init(&this->state);
    ShrinkWindow_Init();
    View_Init(&this->view, this->state.gfxCtx);

    this->state.main = ConsoleLogo_Main;
    this->state.destroy = ConsoleLogo_Destroy;
    this->exit = false;

    // #region 2S2H [Debug] This value is set to indicate no controllers are connected, not only
    // do we not need this, but it conflicts with us wanting to load the debug file on the map select at boot.
    // if (!(PadMgr_GetValidControllersMask() & 1)) {
    //     gSaveContext.fileNum = 0xFEDC;
    // } else {
    gSaveContext.fileNum = 0xFF;
    // }
    // #endregion

    gSaveContext.flashSaveAvailable = true;
    Sram_Alloc(thisx, &this->sramCtx);
    this->ult = 0;
    this->timer = 20;
    this->coverAlpha = 255;
    this->addAlpha = -12;
    this->visibleDuration = 60;
}

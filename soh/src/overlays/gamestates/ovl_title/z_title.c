/*
 * File: z_title.c
 * Overlay: ovl_title
 * Description: Displays the Nintendo Logo
 */

#define NORMAL_GAMEPLAY

#include "global.h"
#include "alloca.h"
#include "textures/nintendo_rogo_static/nintendo_rogo_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include <soh/Enhancements/bootcommands.h>
#include <GameVersions.h>
#include <soh/SaveManager.h>
#include <string.h>

#include "time.h"

const char* GetGameVersionString(s32 index);
char* quote;

void Title_PrintBuildInfo(Gfx** gfxp) {
    Gfx* g;
    GfxPrint printer;

    g = *gfxp;
    g = Gfx_SetupDL_28(g);

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, g);
    GfxPrint_SetColor(&printer, 131, 154, 255, 255);

    //if tag is empty (not a release build)
    bool showGitInfo = gGitCommitTag[0] == 0;

    if (showGitInfo) {
        GfxPrint_SetPos(&printer, 1, 24);
        GfxPrint_Printf(&printer, "Git Branch: %s", gGitBranch);

        //truncate the commit to 7 characters
        char gGitCommitHashTruncated[8];
        strncpy(gGitCommitHashTruncated, gGitCommitHash, 7);
        gGitCommitHashTruncated[7] = 0;

        GfxPrint_SetPos(&printer, 1, 25);
        GfxPrint_Printf(&printer, "Git Commit: %s", gGitCommitHashTruncated);
    } else {
        GfxPrint_SetPos(&printer, 1, 25);
        GfxPrint_Printf(&printer, "%s", gBuildVersion);
    }
    GfxPrint_SetPos(&printer, 1, 26);
    GfxPrint_Printf(&printer, "%s", gBuildDate);

    u32 numVersions = ResourceMgr_GetNumGameVersions();
    s32 pos = 27 - numVersions;
    for (u32 i = 0; i < numVersions; i++) {
        GfxPrint_SetPos(&printer, 29, pos++);
        GfxPrint_Printf(&printer, "%s", GetGameVersionString(i));
    }

    g = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
    *gfxp = g;
}

const char* GetGameVersionString(s32 index) {
    uint32_t gameVersion = ResourceMgr_GetGameVersion(index);
    switch (gameVersion) {
        case OOT_NTSC_US_10:
            return "NTSC-U 1.0";
        case OOT_NTSC_US_11:
            return "NTSC-U 1.1";
        case OOT_NTSC_US_12:
            return "NTSC-U 1.2";
        case OOT_PAL_10:
            return "PAL 1.0";
        case OOT_PAL_11:
            return "PAL 1.1";
        case OOT_PAL_GC:
            return "PAL GC";
        case OOT_PAL_MQ:
            return "PAL MQ";
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
            return "PAL GC-D";
        case OOT_PAL_GC_MQ_DBG:
            return "PAL MQ-D";
        case OOT_IQUE_CN:
            return "IQUE CN";
        case OOT_IQUE_TW:
            return "IQUE TW";
        default:
            return "UNKNOWN";
    }
}

// Note: In other rom versions this function also updates unk_1D4, coverAlpha, addAlpha, visibleDuration to calculate
// the fade-in/fade-out + the duration of the n64 logo animation
void Title_Calc(TitleContext* this) {
#ifdef NORMAL_GAMEPLAY
    if ((this->coverAlpha == 0) && (this->visibleDuration != 0)) {
        this->visibleDuration--;
        this->unk_1D4--;
        if (this->unk_1D4 == 0) {
            this->unk_1D4 = 400;
        }
    } else {
        this->coverAlpha += this->addAlpha;
        if (this->coverAlpha <= 0) {
            this->coverAlpha = 0;
            this->addAlpha = 3;
        } else if (this->coverAlpha >= 0xFF) {
            this->coverAlpha = 0xFF;
            this->exit = 1;
        }
    }
    this->uls = this->ult & 0x7F;
    this->ult++;

    if (gSkipLogoTest || gLoadFileSelect) {
        this->exit = true;
    }

#else
    this->exit = true;
#endif
}

void Title_SetupView(TitleContext* this, f32 x, f32 y, f32 z) {
    View* view = &this->view;
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = x;
    eye.y = y;
    eye.z = z;
    up.x = up.z = 0.0f;
    lookAt.x = lookAt.y = lookAt.z = 0.0f;
    up.y = 1.0f;

    func_800AA460(view, 30.0f, 10.0f, 12800.0f);
    func_800AA358(view, &eye, &lookAt, &up);
    func_800AAA50(view, 0xF);
}

#define dgShipLogoDL "__OTR__textures/nintendo_rogo_static/gShipLogoDL"
static const ALIGN_ASSET(2) char gShipLogoDL[] = dgShipLogoDL;

#define dnintendo_rogo_static_Tex_LUS_000000 "__OTR__textures/nintendo_rogo_static/nintendo_rogo_static_Tex_LUS_000000"
static const ALIGN_ASSET(2) char nintendo_rogo_static_Tex_LUS_000000[] = dnintendo_rogo_static_Tex_LUS_000000;

void Title_Draw(TitleContext* this) {
    static s16 sTitleRotY = 0;
    static Lights1 sTitleLights = gdSPDefLights1(0x64, 0x64, 0x64, 0xFF, 0xFF, 0xFF, 0x45, 0x45, 0x45);

    u16 y;
    u16 idx;
    s32 pad1;
    Vec3f v3;
    Vec3f v1;
    Vec3f v2;
    s32 pad2[2];

    OPEN_DISPS(this->state.gfxCtx);

    v3.x = 69;
    v3.y = 69;
    v3.z = 69;
    v2.x = -4949.148;
    v2.y = 4002.5417;
    v1.x = 0;
    v1.y = 0;
    v1.z = 0;
    v2.z = 1119.0837;

    func_8002EABC(&v1, &v2, &v3, this->state.gfxCtx);
    gSPSetLights1(POLY_OPA_DISP++, sTitleLights);
    Title_SetupView(this, 0, 150.0, 300.0);
    Gfx_SetupDL_25Opa(this->state.gfxCtx);
    Matrix_Translate(-53.0, -5.0, 0, MTXMODE_NEW);
    Matrix_Scale(1.0, 1.0, 1.0, MTXMODE_APPLY);
    Matrix_RotateZYX(0, sTitleRotY, 0, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx), G_MTX_LOAD);
    if (CVarGetInteger(CVAR_ENHANCEMENT("AuthenticLogo"), 0)) {
        gSPDisplayList(POLY_OPA_DISP++, gNintendo64LogoDL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gShipLogoDL);
    }
    Gfx_SetupDL_39Opa(this->state.gfxCtx);
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

        gDPSetTileSize(POLY_OPA_DISP++, 1, this->uls, (this->ult & 0x7F) - idx * 4, 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 388, y << 2, 1156, (y + 2) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    // Draw ice cube around N64 logo.
    if (CVarGetInteger(CVAR_GENERAL("LetItSnow"), 0)) {
        f32 scale = 0.4f;

        gSPSegment(POLY_OPA_DISP++, 0x08,
                    Gfx_TwoTexScroll(this->state.gfxCtx, 0, 0, (0 - 1) % 128, 32, 32, 1,
                                    0, (1 * -2) % 128, 32, 32));

        Matrix_Translate(0.0f, -10.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                    G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 50, 100, 255);
        gSPDisplayList(POLY_OPA_DISP++, gEffIceFragment3DL);
    }

    Environment_FillScreen(this->state.gfxCtx, 0, 0, 0, (s16)this->coverAlpha, FILL_SCREEN_XLU);

    sTitleRotY += (300 * CVarGetFloat(CVAR_COSMETIC("N64Logo.SpinSpeed"), 1.0f));

    CLOSE_DISPS(this->state.gfxCtx);
}

void Title_Main(GameState* thisx) {
    TitleContext* this = (TitleContext*)thisx;

    OPEN_DISPS(this->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0, NULL);
    gSPSegment(POLY_OPA_DISP++, 1, this->staticSegment);
    Gfx_SetupFrame(this->state.gfxCtx, 0, 0, 0);
    Title_Calc(this);
    Title_Draw(this);

    if (!CVarGetInteger(CVAR_ENHANCEMENT("AuthenticLogo"), 0)) {
        Gfx* gfx = POLY_OPA_DISP;
        s32 pad;

        Title_PrintBuildInfo(&gfx);
        POLY_OPA_DISP = gfx;
    }

    if (this->exit || CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0)) {
        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = 1;
        this->state.running = false;

        if (gLoadFileSelect || CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0))
            SET_NEXT_GAMESTATE(&this->state, FileChoose_Init, FileChooseContext);
        else
            SET_NEXT_GAMESTATE(&this->state, Opening_Init, OpeningContext);
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

void Title_Destroy(GameState* thisx) {
    TitleContext* this = (TitleContext*)thisx;

    Sram_InitSram(&this->state);
}

void Title_Init(GameState* thisx) {
    //u32 size = 0;
    TitleContext* this = (TitleContext*)thisx;

    this->staticSegment = NULL;
    //this->staticSegment = GAMESTATE_ALLOC_MC(&this->state, size);
    osSyncPrintf("z_title.c\n");

    //ResourceMgr_LoadDirectory("nintendo_rogo_static*");

    R_UPDATE_RATE = 1;
    Matrix_Init(&this->state);
    View_Init(&this->view, this->state.gfxCtx);
    this->state.main = Title_Main;
    this->state.destroy = Title_Destroy;
    this->exit = false;
    gSaveContext.fileNum = 0xFF;
    this->ult = 0;
    this->unk_1D4 = 0x14;
    this->coverAlpha = 255;
    this->addAlpha = -3;
    this->visibleDuration = 0x3C;
}

/*
 * File: z_title.c
 * Overlay: ovl_title
 * Description: Displays the Nintendo Logo
 */

#define NORMAL_GAMEPLAY

#include "global.h"
#include "alloca.h"
#include "textures/nintendo_rogo_static/nintendo_rogo_static.h"
#include <soh/Enhancements/bootcommands.h>
#include <libultraship/GameVersions.h>
#include <soh/SaveManager.h>

#include "time.h"

const char* GetGameVersionString();
char* quote;

void Title_PrintBuildInfo(Gfx** gfxp) {
    Gfx* g;
    //GfxPrint* printer;
    GfxPrint printer;

    const char* gameVersionStr = GetGameVersionString();

    g = *gfxp;
    g = func_8009411C(g);
    //printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, g);
    GfxPrint_SetColor(&printer, 255, 155, 255, 255);
    GfxPrint_SetPos(&printer, 12, 20);

#ifdef _MSC_VER
    GfxPrint_Printf(&printer, "MSVC SHIP");
#elif __clang__
    GfxPrint_Printf(&printer, "CLANG SHIP");
#else
    GfxPrint_Printf(&printer, "GCC SHIP");
#endif

    GfxPrint_SetPos(&printer, 1, 4);
    GfxPrint_Printf(&printer, "Game Version: %s", gameVersionStr);
    GfxPrint_SetPos(&printer, 1, 5);
    GfxPrint_Printf(&printer, "Release Version: %s", gBuildVersion);

    GfxPrint_SetColor(&printer, 255, 255, 255, 255);
    GfxPrint_SetPos(&printer, 2, 22);
    GfxPrint_Printf(&printer, quote);
    GfxPrint_SetPos(&printer, 1, 25);
    GfxPrint_Printf(&printer, "Build Date:%s", gBuildDate);
    GfxPrint_SetPos(&printer, 3, 26);
    GfxPrint_Printf(&printer, "%s", gBuildTeam);
    g = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
    *gfxp = g;
}

const char* quotes[11] = {
    "My boy! This peace is what all true warriors strive for!",
    "Hmm. How can we help?",
    "Zelda! Duke Onkled is under attack by the evil forces of Ganon!",
    "I'm going to Gamelon to aid him.",
    "I'll take the Triforce of Courage to protect me.",
    "If you don't hear from me in a month, send Link.",
    "Enough! My ship sails in the morning.",
    "I wonder what's for dinner.",
    "You've saved me!",
    "After you've scrubbed all the floors in Hyrule, then we can talk about mercy! Take him away!",
    "Waaaahahahohohahahahahahaha"
};

const char* SetQuote() {
    srand(time(NULL));
    int randomQuote = rand() % 11;
    return quotes[randomQuote];
}

const char* GetGameVersionString() {
    uint32_t gameVersion = ResourceMgr_GetGameVersion();
    switch (gameVersion) {
        case OOT_NTSC_10:
            return "N64 NTSC 1.0";
        case OOT_NTSC_11:
            return "N64 NTSC 1.1";
        case OOT_NTSC_12:
            return "N64 NTSC 1.2";
        case OOT_PAL_10:
            return "N64 PAL 1.0";
        case OOT_PAL_11:
            return "N64 PAL 1.1";
        case OOT_PAL_GC:
            return "GC PAL";
        case OOT_PAL_MQ:
            return "GC PAL MQ";
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
            return "GC PAL DEBUG";
        case OOT_PAL_GC_MQ_DBG:
            return "GC PAL DEBUG MQ";
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

    char* n64LogoTex = ResourceMgr_LoadTexByName(nintendo_rogo_static_Tex_000000);

    func_8002EABC(&v1, &v2, &v3, this->state.gfxCtx);
    gSPSetLights1(POLY_OPA_DISP++, sTitleLights);
    Title_SetupView(this, 0, 150.0, 300.0);
    func_80093D18(this->state.gfxCtx);
    Matrix_Translate(-53.0, -5.0, 0, MTXMODE_NEW);
    Matrix_Scale(1.0, 1.0, 1.0, MTXMODE_APPLY);
    Matrix_RotateZYX(0, sTitleRotY, 0, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx), G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, gNintendo64LogoDL);
    func_800944C4(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF2, G_RM_OPA_CI | CVG_DST_WRAP);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
        COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 255, 128);

    gDPLoadMultiBlock(POLY_OPA_DISP++, nintendo_rogo_static_Tex_001800, 0x100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 11);

    for (idx = 0, y = 94; idx < 16; idx++, y += 2)
    {
        gDPLoadTextureBlock(POLY_OPA_DISP++, &n64LogoTex[0x180 * idx], G_IM_FMT_I,
            G_IM_SIZ_8b, 192, 2, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetTileSize(POLY_OPA_DISP++, 1, this->uls, (this->ult & 0x7F) - idx * 4, 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 388, y << 2, 1156, (y + 2) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    Environment_FillScreen(this->state.gfxCtx, 0, 0, 0, (s16)this->coverAlpha, FILL_SCREEN_XLU);

    sTitleRotY += 300;

    CLOSE_DISPS(this->state.gfxCtx);
}

void Title_Main(GameState* thisx) {
    TitleContext* this = (TitleContext*)thisx;

    OPEN_DISPS(this->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0, NULL);
    gSPSegment(POLY_OPA_DISP++, 1, this->staticSegment);
    func_80095248(this->state.gfxCtx, 0, 0, 0);
    Title_Calc(this);
    Title_Draw(this);

    if (1) {
        Gfx* gfx = POLY_OPA_DISP;
        s32 pad;

        Title_PrintBuildInfo(&gfx);
        POLY_OPA_DISP = gfx;
    }

    if (this->exit) {
        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = 1;
        this->state.running = false;

        if (gLoadFileSelect)
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
    FileChooseContext* FileChooseCtx = (FileChooseContext*)thisx;

    if (CVar_GetS32("gSkipLogoTitle",0)) {
        bool saveloading = false;
        Sram_InitSram(&this->state.init);
        gSaveContext.language = CVar_GetS32("gLanguages", 0);
        s32 selectedfile = CVar_GetS32("gSaveFileID", 0);
        if (selectedfile == 4) {
            if (CVar_GetS32("gDebugEnabled",0)) {
                selectedfile = 0xFF;
            } else {
                selectedfile = 3;
            }
        }
        if (selectedfile < 0) {
            selectedfile = 3; //If somehow the save file number under 0 revert back to 3 to prevent boot error
        }
        if(selectedfile == 3){
            saveloading = true;
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = 0xFF;
            gSaveContext.gameMode = 1;
            SET_NEXT_GAMESTATE(&this->state, FileChoose_Init, FileChooseContext);
            this->state.running = false;
            return;
        } else if (selectedfile == 0xFF || selectedfile > 3) {
            saveloading = true;
            Sram_InitDebugSave();
            gSaveContext.fileNum = selectedfile;
            SET_NEXT_GAMESTATE(&this->state, Select_Init, SelectContext);
            this->state.running = false;
            return;
        } else if (selectedfile >= 0 && selectedfile <= 2) {
            if (Save_Exist(selectedfile) == true) { //The file exist load it
                saveloading = true;
                gSaveContext.fileNum = selectedfile;
                Sram_OpenSave();
                Randomizer_LoadSettings("");
                Randomizer_LoadHintLocations("");
                Randomizer_LoadItemLocations("", true);
                gSaveContext.gameMode = 0;
                gSaveContext.magic = gSaveContext.magic;
                SET_NEXT_GAMESTATE(&this->state, Gameplay_Init, GlobalContext);
                this->state.running = false;
                return;
            } else { 
                if (CVar_GetS32("gCreateNewSave",0)) {
                    //File do not exist create a new save file
                    saveloading = true;
                    Sram_InitSram(&FileChooseCtx->state.init);
                    gSaveContext.fileNum = selectedfile;
                    Sram_InitSave(FileChooseCtx);
                    Sram_OpenSave();      
                    gSaveContext.gameMode = 0;
                    SET_NEXT_GAMESTATE(&this->state, Gameplay_Init, GlobalContext);
                    this->state.running = false;
                    return;
                } else {
                    //File do not exist but user do not wish to auto create a save file with blank name
                    saveloading = true;
                    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
                    gSaveContext.natureAmbienceId = 0xFF;
                    gSaveContext.gameMode = 1;
                    SET_NEXT_GAMESTATE(&this->state, FileChoose_Init, FileChooseContext);
                    this->state.running = false;
                    return;
                }
            }
        }
    } else {
        quote = SetQuote();

        this->staticSegment = NULL;
        //this->staticSegment = GAMESTATE_ALLOC_MC(&this->state, size);
        osSyncPrintf("z_title.c\n");
        //ASSERT(this->staticSegment != NULL);

        //ResourceMgr_CacheDirectory("nintendo_rogo_static*");

        // Disable vismono
        D_801614B0.a = 0;
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
}

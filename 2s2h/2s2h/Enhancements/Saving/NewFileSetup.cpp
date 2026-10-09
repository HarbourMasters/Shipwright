#include <libultraship/bridge/consolevariablebridge.h>
#include <cstring>
#include <string>
#include <vector>

#include <fstream>
#include <unordered_map>

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/PresetManager/PresetManager.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenPort.h"
#include "2s2h/Rando/MiscBehavior/MiscBehavior.h"
#include "2s2h/Rando/Spoiler/Spoiler.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "misc/title_static/title_static.h"
#include "2s2h_assets.h"

extern FileSelectState* gFileSelectState;
}

#define CVAR_NAME "gEnhancements.Saving.NewFileSetup"
#define CVAR CVarGetInteger(CVAR_NAME, 1)

typedef enum {
    NEW_FILE_SETUP_ROW_PRESET,
    NEW_FILE_SETUP_ROW_MODE,
    NEW_FILE_SETUP_ROW_SEED,
    NEW_FILE_SETUP_ROW_MAX,
} NewFileSetupRow;

typedef struct {
    const char* texture;
    s16 width;
    // Where the actual letters start inside the texture, so we can account for it when padding
    s16 inkX;
    s16 inkY;
    s16 inkWidth;
} SetupLabel;

#define SETUP_CHAR_WIDTH 10
#define SETUP_CHAR_HEIGHT 10
#define SETUP_CHARS_PER_BATCH 8
#define SETUP_LABEL_HEIGHT 16
#define SETUP_TITLE_X -93
#define SETUP_TITLE_Y 71
#define SETUP_HEADING_X -97
#define SETUP_SECTION_LIFT 4
#define SETUP_SECTION_Y(row) (40 - ((row)*40) + ((row) > 0 ? SETUP_SECTION_LIFT : 0))
#define SETUP_DIVIDER_Y(row) (SETUP_SECTION_Y(row) - 12)
#define SETUP_VALUE_Y(row) (SETUP_SECTION_Y(row) - 17)
#define SETUP_VALUE_X -95
#define SETUP_VALUE_GAP 20
#define SETUP_DIVIDER_LEFT -100
#define SETUP_DIVIDER_WIDTH 204
#define SETUP_DIVIDER_TEX_INK 204
#define SETUP_ARROW_MARGIN 10
#define SETUP_CYCLER_CENTER_X (SETUP_DIVIDER_LEFT + (SETUP_DIVIDER_WIDTH / 2))
#define SETUP_CYCLER_TEXT_WIDTH 160
#define SETUP_SEED_ICONS 5
#define SETUP_SEED_ICON_SIZE 16
#define SETUP_SEED_ICON_GAP 4

static s16 sRow = NEW_FILE_SETUP_ROW_PRESET;
static s16 sAlpha = 0;
static bool sIsRando = false;
static s16 sPresetIndex = 0;
static std::vector<std::string> sPresetNames;
static std::string sPresetDisplay;
static s16 sPresetDisplayWidth = 0;
static s16 sSeedIndex = 0;
static std::string sInputSeed;
static std::string sInputSeedDisplay;
static s16 sInputSeedDisplayWidth = 0;
static std::unordered_map<std::string, uint32_t> sSeedHashes;
static SetupLabel sTitle = { gFileSelNewFileTex, 56, 4, 3, 48 };
static SetupLabel sHeadings[NEW_FILE_SETUP_ROW_MAX] = {
    { gFileSelPresetHeaderTex, 48, 3, 4, 41 },
    { gFileSelModeHeaderTex, 40, 5, 4, 30 },
    { gFileSelSeedHeaderTex, 32, 2, 4, 28 },
};
static SetupLabel sModeChoices[] = {
    { gFileSelVanillaTex, 48, 5, 3, 38 },
    { gFileSelRandomizerTex, 80, 5, 3, 69 },
};
static SetupLabel sGenerateNew = { gFileSelGenerateNewTex, 88, 3, 3, 82 };
static SetupLabel sArrowLeft = { gFileSelArrowLeftTex, 16, 4, 4, 7 };
static SetupLabel sArrowRight = { gFileSelArrowRightTex, 16, 4, 4, 7 };
static s16 sCursorPrim[3] = { 255, 255, 255 };
static s16 sCursorEnv[3] = { 0, 0, 0 };
static s16 sCursorPulseDir = 1;
static s16 sCursorTimer = 20;
static const s16 sCursorPrimTargets[2][3] = { { 255, 255, 255 }, { 0, 255, 255 } };
static const s16 sCursorEnvTargets[2][3] = { { 0, 0, 0 }, { 0, 150, 150 } };

static bool RowIsVisible(s16 row) {
    return (row != NEW_FILE_SETUP_ROW_SEED) || sIsRando;
}

static s16 NextVisibleRow(s16 row, s16 step) {
    do {
        row = (s16)((row + step + NEW_FILE_SETUP_ROW_MAX) % NEW_FILE_SETUP_ROW_MAX);
    } while (!RowIsVisible(row));

    return row;
}

static uint32_t SeedHashForSpoiler(const std::string& fileName) {
    auto cached = sSeedHashes.find(fileName);
    if (cached != sSeedHashes.end()) {
        return cached->second;
    }

    uint32_t hash = 0;
    try {
        // performance optimization: only parse the top level of the spoiler file, since that's all we need
        auto topLevelOnly = [](int depth, nlohmann::json::parse_event_t, nlohmann::json&) { return depth <= 1; };

        std::ifstream stream(Ship::Context::GetPathRelativeToAppDirectory("randomizer/" + fileName, appShortName));
        nlohmann::json spoiler = nlohmann::json::parse(stream, topLevelOnly);

        if (spoiler.contains("finalSeed")) {
            hash = spoiler["finalSeed"].get<uint32_t>();
        }
    } catch (...) { hash = 0; }

    sSeedHashes[fileName] = hash;
    return hash;
}

// Some sort of vtx black magic, archez would understand it
static void SetQuadVtx(Vtx* vtx, s16 left, s16 top, s16 width, s16 height, s16 texWidth, s16 texHeight) {
    vtx[0].v.ob[0] = vtx[2].v.ob[0] = left;
    vtx[1].v.ob[0] = vtx[3].v.ob[0] = left + width;

    vtx[0].v.ob[1] = vtx[1].v.ob[1] = top;
    vtx[2].v.ob[1] = vtx[3].v.ob[1] = top - height;

    vtx[0].v.tc[0] = vtx[0].v.tc[1] = vtx[1].v.tc[1] = vtx[2].v.tc[0] = 0;
    vtx[1].v.tc[0] = vtx[3].v.tc[0] = texWidth << 5;
    vtx[2].v.tc[1] = vtx[3].v.tc[1] = texHeight << 5;

    for (s16 i = 0; i < 4; i++) {
        vtx[i].v.ob[2] = 0;
        vtx[i].v.flag = 0;
        vtx[i].v.cn[0] = vtx[i].v.cn[1] = vtx[i].v.cn[2] = vtx[i].v.cn[3] = 255;
    }
}

static void SetLabelCombine(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    CLOSE_DISPS(gfxCtx);
}

// Cursor pulsing from white to blue like the rest of the menus
static void UpdateCursorColor(void) {
    for (s16 i = 0; i < 3; i++) {
        s16 primStep = ABS_ALT(sCursorPrim[i] - sCursorPrimTargets[sCursorPulseDir][i]) / sCursorTimer;
        s16 envStep = ABS_ALT(sCursorEnv[i] - sCursorEnvTargets[sCursorPulseDir][i]) / sCursorTimer;

        sCursorPrim[i] += (sCursorPrim[i] >= sCursorPrimTargets[sCursorPulseDir][i]) ? -primStep : primStep;
        sCursorEnv[i] += (sCursorEnv[i] >= sCursorEnvTargets[sCursorPulseDir][i]) ? -envStep : envStep;
    }

    if (--sCursorTimer == 0) {
        for (s16 i = 0; i < 3; i++) {
            sCursorPrim[i] = sCursorPrimTargets[sCursorPulseDir][i];
            sCursorEnv[i] = sCursorEnvTargets[sCursorPulseDir][i];
        }
        sCursorTimer = 20;
        sCursorPulseDir ^= 1;
    }
}

static void SetValueColor(FileSelectState* fileSelect, bool isCurrent, bool rowSelected, u8 alpha) {
    OPEN_DISPS(fileSelect->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    if (!isCurrent) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    } else if (rowSelected) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sCursorPrim[0], sCursorPrim[1], sCursorPrim[2], alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, sCursorEnv[0], sCursorEnv[1], sCursorEnv[2], 255);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    }

    CLOSE_DISPS(fileSelect->state.gfxCtx);
}

static s16 GlyphAdvance(char c) {
    return (s16)(Ship_GetCharFontWidthNES((u8)c) * SETUP_CHAR_WIDTH / (f32)FONT_CHAR_TEX_WIDTH);
}

static s16 TextWidth(const char* text) {
    s16 width = 0;

    for (const char* c = text; *c != '\0'; c++) {
        width += GlyphAdvance(*c);
    }

    return width;
}

static void DrawText(FileSelectState* fileSelect, const char* text, s16 x, s16 top) {
    s16 length = (s16)strlen(text);

    if (length <= 0) {
        return;
    }

    Vtx* vtx = (Vtx*)GRAPH_ALLOC(fileSelect->state.gfxCtx, length * 4 * sizeof(Vtx));

    s16 pen = x;
    for (s16 i = 0; i < length; i++) {
        SetQuadVtx(&vtx[i * 4], pen, top, SETUP_CHAR_WIDTH, SETUP_CHAR_HEIGHT, FONT_CHAR_TEX_WIDTH,
                   FONT_CHAR_TEX_HEIGHT);
        pen += GlyphAdvance(text[i]);
    }

    OPEN_DISPS(fileSelect->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    for (s16 i = 0; i < length; i += SETUP_CHARS_PER_BATCH) {
        s16 count = (s16)MIN(SETUP_CHARS_PER_BATCH, length - i);

        gSPVertex(POLY_OPA_DISP++, (uintptr_t)&vtx[i * 4], count * 4, 0);

        for (s16 j = 0; j < count; j++) {
            FileSelect_DrawTexQuadI4(fileSelect->state.gfxCtx, Ship_GetCharFontTextureNES(text[i + j]), j * 4);
        }
    }

    CLOSE_DISPS(fileSelect->state.gfxCtx);
}

static void DrawLabel(FileSelectState* fileSelect, const SetupLabel* label, s16 inkLeft, s16 inkTop) {
    Vtx* vtx = (Vtx*)GRAPH_ALLOC(fileSelect->state.gfxCtx, 4 * sizeof(Vtx));

    // Positioned by the lettering rather than the texture corner, so the padding around it never matters
    SetQuadVtx(vtx, inkLeft - label->inkX, inkTop + label->inkY, label->width, SETUP_LABEL_HEIGHT, label->width,
               SETUP_LABEL_HEIGHT);

    OPEN_DISPS(fileSelect->state.gfxCtx);

    gSPVertex(POLY_OPA_DISP++, (uintptr_t)vtx, 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, label->texture, G_IM_FMT_IA, G_IM_SIZ_8b, label->width, SETUP_LABEL_HEIGHT, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(fileSelect->state.gfxCtx);
}

// These match the look of the dividers in the option menu
static void DrawDivider(FileSelectState* fileSelect, s16 top, u8 alpha) {
    Vtx* vtx = (Vtx*)GRAPH_ALLOC(fileSelect->state.gfxCtx, 4 * sizeof(Vtx));

    SetQuadVtx(vtx, SETUP_DIVIDER_LEFT, top, SETUP_DIVIDER_WIDTH, 2, SETUP_DIVIDER_TEX_INK, 2);

    OPEN_DISPS(fileSelect->state.gfxCtx);

    SetLabelCombine(fileSelect->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, (uintptr_t)vtx, 4, 0);
    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelOptionsDividerTex, G_IM_FMT_IA, 256, 2, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(fileSelect->state.gfxCtx);
}

static void DrawArrows(FileSelectState* fileSelect, s16 inkTop) {
    DrawLabel(fileSelect, &sArrowLeft, SETUP_DIVIDER_LEFT + SETUP_ARROW_MARGIN, inkTop);
    DrawLabel(fileSelect, &sArrowRight,
              SETUP_DIVIDER_LEFT + SETUP_DIVIDER_WIDTH - SETUP_ARROW_MARGIN - sArrowRight.inkWidth, inkTop);
}

static void DrawSeedIcons(FileSelectState* fileSelect, uint32_t hash, s16 centerX, s16 centerY, u8 alpha) {
    const s16 pitch = SETUP_SEED_ICON_SIZE + SETUP_SEED_ICON_GAP;
    s16 left = centerX - (((SETUP_SEED_ICONS * pitch) - SETUP_SEED_ICON_GAP) / 2);

    OPEN_DISPS(fileSelect->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, alpha);

    for (s16 i = 0; i < SETUP_SEED_ICONS; i++) {
        Rando::MiscBehavior::Sprite* sprite = Rando::MiscBehavior::GetSeedTexture((u8)(hash % 100));
        hash /= 100;

        Vtx* vtx = (Vtx*)GRAPH_ALLOC(fileSelect->state.gfxCtx, 4 * sizeof(Vtx));
        SetQuadVtx(vtx, left + (i * pitch), centerY + (SETUP_SEED_ICON_SIZE / 2), SETUP_SEED_ICON_SIZE,
                   SETUP_SEED_ICON_SIZE, sprite->width, sprite->height);

        gSPVertex(POLY_OPA_DISP++, (uintptr_t)vtx, 4, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sprite->tex, G_IM_FMT_RGBA, G_IM_SIZ_32b, sprite->width, sprite->height, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    }

    CLOSE_DISPS(fileSelect->state.gfxCtx);
}

static std::string FitToCycler(const std::string& text) {
    std::string fitted = text;

    while (fitted.length() > 4 && TextWidth(fitted.c_str()) > SETUP_CYCLER_TEXT_WIDTH) {
        fitted = fitted.substr(0, fitted.length() - 4) + "...";
    }

    return fitted;
}

static void UpdatePresetDisplay(void) {
    sPresetDisplay = FitToCycler(sPresetNames.empty() ? "" : sPresetNames[sPresetIndex]);
    sPresetDisplayWidth = TextWidth(sPresetDisplay.c_str());
}

static void RefreshPresetList(void) {
    sPresetNames.clear();
    sPresetNames.push_back("None");

    for (const auto& name : PresetManager_GetPresetNames()) {
        sPresetNames.push_back(name);
    }

    sPresetIndex = 0;
    UpdatePresetDisplay();
}

static void UpdateInputSeedDisplay(void) {
    std::string inputSeed = Ship_RemoveSpecialCharacters(CVarGetString("gRando.InputSeed", ""));

    if (inputSeed == sInputSeed) {
        return;
    }

    sInputSeed = inputSeed;
    sInputSeedDisplay = FitToCycler(inputSeed);
    sInputSeedDisplayWidth = TextWidth(sInputSeedDisplay.c_str());
}

static void RefreshSeedList(void) {
    Rando::Spoiler::RefreshOptions();
    // The list is never empty in practice, but clamping guards against indexing it with -1 if it were
    s32 last = (s32)Rando::Spoiler::spoilerOptions.size() - 1;
    sSeedIndex = (last < 0) ? 0 : (s16)CLAMP(CVarGetInteger("gRando.SpoilerFileIndex", 0), 0, last);

    UpdateInputSeedDisplay();
}

static void ApplySelection(void) {
    if ((sPresetIndex > 0) && (sPresetIndex < (s16)sPresetNames.size())) {
        PresetManager_ApplyPresetByName(sPresetNames[sPresetIndex]);
    }

    CVarSetInteger("gRando.Enabled", sIsRando ? 1 : 0);

    // OnFileCreate reads the pair to decide whether to generate a seed or replay a spoiler
    Rando::Spoiler::SelectSpoiler(sIsRando ? sSeedIndex : 0);

    CVarSave();
}

static void LeaveSetup(FileSelectState* fileSelect, s16 configMode) {
    sAlpha = 0;
    fileSelect->configMode = configMode;
}

extern "C" void FileSelect_RotateToNewFileSetup(GameState* thisx) {
    FileSelectState* fileSelect = (FileSelectState*)thisx;

    fileSelect->windowRot += 50.0f;

    if (fileSelect->windowRot >= 314.0f) {
        fileSelect->windowRot = 314.0f;
        fileSelect->configMode = CM_2S2H_NEW_FILE_SETUP;
    }
}

extern "C" void FileSelect_UpdateNewFileSetup(GameState* thisx) {
    FileSelectState* fileSelect = (FileSelectState*)thisx;
    Input* input = CONTROLLER1(&fileSelect->state);
    s16 presetCount = (s16)sPresetNames.size();

    sAlpha = (s16)MIN(sAlpha + 25, 255);

    UpdateInputSeedDisplay();

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CLOSE);
        LeaveSetup(fileSelect, CM_2S2H_NEW_FILE_SETUP_TO_MAIN);
        return;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START)) {
        Audio_PlaySfx(NA_SE_SY_FSEL_DECIDE_L);
        ApplySelection();
        LeaveSetup(fileSelect, CM_START_NAME_ENTRY);
        return;
    }

    bool up = (fileSelect->stickAdjY > 30) || CHECK_BTN_ALL(input->press.button, BTN_DUP);
    bool down = (fileSelect->stickAdjY < -30) || CHECK_BTN_ALL(input->press.button, BTN_DDOWN);

    if (up || down) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
        sRow = NextVisibleRow(sRow, up ? -1 : 1);
        return;
    }

    bool left = (fileSelect->stickAdjX < -30) || CHECK_BTN_ALL(input->press.button, BTN_DLEFT);
    bool right = (fileSelect->stickAdjX > 30) || CHECK_BTN_ALL(input->press.button, BTN_DRIGHT);

    if (!left && !right) {
        return;
    }

    Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);

    if (sRow == NEW_FILE_SETUP_ROW_MODE) {
        sIsRando = !sIsRando;
    } else if (sRow == NEW_FILE_SETUP_ROW_SEED) {
        s16 seedCount = (s16)Rando::Spoiler::spoilerOptions.size();
        sSeedIndex = (s16)((sSeedIndex + (right ? 1 : seedCount - 1)) % seedCount);
    } else {
        sPresetIndex = (s16)((sPresetIndex + (right ? 1 : presetCount - 1)) % presetCount);
        UpdatePresetDisplay();
    }
}

extern "C" void FileSelect_DrawNewFileSetup(GameState* thisx) {
    FileSelectState* fileSelect = (FileSelectState*)thisx;
    u8 alpha = (u8)sAlpha;

    if (alpha == 0) {
        return;
    }

    UpdateCursorColor();

    OPEN_DISPS(fileSelect->state.gfxCtx);

    Gfx_SetupDL42_Opa(fileSelect->state.gfxCtx);
    SetLabelCombine(fileSelect->state.gfxCtx);

    // Title and section headings are plain white; only values take the selection colours
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    DrawLabel(fileSelect, &sTitle, SETUP_TITLE_X, SETUP_TITLE_Y);

    for (s16 row = 0; row < NEW_FILE_SETUP_ROW_MAX; row++) {
        if (!RowIsVisible(row)) {
            continue;
        }
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        DrawLabel(fileSelect, &sHeadings[row], SETUP_HEADING_X, SETUP_SECTION_Y(row));
        DrawDivider(fileSelect, SETUP_DIVIDER_Y(row), alpha);
    }

    // Mode: both values side by side
    s16 valueX = SETUP_VALUE_X;
    for (s16 i = 0; i < (s16)ARRAY_COUNT(sModeChoices); i++) {
        SetValueColor(fileSelect, i == (sIsRando ? 1 : 0), sRow == NEW_FILE_SETUP_ROW_MODE, alpha);
        DrawLabel(fileSelect, &sModeChoices[i], valueX, SETUP_VALUE_Y(NEW_FILE_SETUP_ROW_MODE));
        valueX += sModeChoices[i].inkWidth + SETUP_VALUE_GAP;
    }

    // Seed: "Generate New" first, then every spoiler in the folder as its own icons
    if (RowIsVisible(NEW_FILE_SETUP_ROW_SEED)) {
        bool seedRowSelected = (sRow == NEW_FILE_SETUP_ROW_SEED);
        s16 seedY = SETUP_VALUE_Y(NEW_FILE_SETUP_ROW_SEED);

        SetValueColor(fileSelect, true, seedRowSelected, alpha);
        DrawArrows(fileSelect, seedY);

        if (sSeedIndex == 0 && !sInputSeedDisplay.empty()) {
            DrawText(fileSelect, sInputSeedDisplay.c_str(), SETUP_CYCLER_CENTER_X - (sInputSeedDisplayWidth / 2),
                     seedY - 1);
            SetLabelCombine(fileSelect->state.gfxCtx);
        } else if (sSeedIndex == 0) {
            DrawLabel(fileSelect, &sGenerateNew, SETUP_CYCLER_CENTER_X - (sGenerateNew.inkWidth / 2), seedY);
        } else if (sSeedIndex < (s16)Rando::Spoiler::spoilerOptions.size()) {
            DrawSeedIcons(fileSelect, SeedHashForSpoiler(Rando::Spoiler::spoilerOptions[sSeedIndex]),
                          SETUP_CYCLER_CENTER_X, seedY - 4, alpha);
            SetLabelCombine(fileSelect->state.gfxCtx);
        }
    }

    // Preset: one at a time
    bool presetRowSelected = (sRow == NEW_FILE_SETUP_ROW_PRESET);
    s16 presetY = SETUP_VALUE_Y(NEW_FILE_SETUP_ROW_PRESET);

    SetValueColor(fileSelect, true, presetRowSelected, alpha);
    DrawArrows(fileSelect, presetY);

    // Inherits the primitive SetValueColor set for this row, which is what tints the arrows too
    DrawText(fileSelect, sPresetDisplay.c_str(), SETUP_CYCLER_CENTER_X - (sPresetDisplayWidth / 2), presetY - 1);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(fileSelect->state.gfxCtx);
}

void RegisterNewFileSetup() {
    COND_VB_SHOULD(VB_FILE_SELECT_ROTATE_TO_NAME_ENTRY, CVAR, {
        FileSelectState* fileSelect = va_arg(args, FileSelectState*);

        sRow = NEW_FILE_SETUP_ROW_PRESET;
        sAlpha = 0;
        sIsRando = CVarGetInteger("gRando.Enabled", 0) != 0;
        RefreshPresetList();
        RefreshSeedList();

        fileSelect->configMode = CM_2S2H_ROTATE_TO_NEW_FILE_SETUP;
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterNewFileSetup, { CVAR_NAME });
static RegisterShipInitFunc seedListInitFunc(
    []() {
        if (gFileSelectState != NULL) {
            RefreshSeedList();
        }
    },
    { "gRando.SpoilerFile" });

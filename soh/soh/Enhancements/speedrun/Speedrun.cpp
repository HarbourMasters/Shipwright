#include "Speedrun.h"

#include "soh/Enhancements/Presets/Presets.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "soh/ShipInit.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh_assets.h"

#include <ship/Context.h>
#include <ship/config/Config.h>
#include <ship/window/Window.h>
#include <ship/window/gui/Gui.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "src/overlays/gamestates/ovl_file_choose/file_choose.h"
#include "objects/gameplay_keep/gameplay_keep.h"

void FileChoose_UpdateStickDirectionPromptAnim(GameState* thisx);
void FileChoose_DrawTextRec(GraphicsContext* gfxCtx, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t,
                            f32 dx, f32 dy);
int16_t OTRGetRectDimensionFromRightEdge(float v);
}

namespace fs = std::filesystem;

// The cvar blocks a speedrun file owns. They are copied onto the file when it is created and put back every time it is
// loaded. Everything else (controls, resolution, audio, cosmetics) stays with the player.
static const std::array<const char*, 4> sOwnedBlocks = {
    CVAR_PREFIX_ENHANCEMENT,
    CVAR_PREFIX_RANDOMIZER_ENHANCEMENT,
    CVAR_PREFIX_CHEAT,
    CVAR_PREFIX_DEVELOPER_TOOLS,
};

#define SPEEDRUN_PRESET_NONE "None"
#define SPEEDRUN_MAX_OPTIONS_ON_SCREEN 6

// Settings the player keeps even under a preset. Their own values are copied over the file's settings on create and
// load, and these are left out of the settings hash.
static const std::array<nlohmann::json::json_pointer, 8> sExemptSettings = {
    nlohmann::json::json_pointer("/gDeveloperTools/LogLevel"),
    nlohmann::json::json_pointer("/gDeveloperTools/ResourceLogging"),
    nlohmann::json::json_pointer("/gEnhancements/DrawLineupTick"),
    nlohmann::json::json_pointer("/gEnhancements/FastBottles"),
    nlohmann::json::json_pointer("/gEnhancements/FasterPauseMenu"),
    nlohmann::json::json_pointer("/gEnhancements/GraveHoles"),
    nlohmann::json::json_pointer("/gEnhancements/IncludeHeldInputsBufferWindow"),
    nlohmann::json::json_pointer("/gEnhancements/PauseBufferWindow"),
};

// {display name, preset name}, ending with "None" which has no preset.
static std::vector<std::pair<std::string, std::string>> sPresetChoices;
static nlohmann::json sSettings = nlohmann::json::object();
static std::string sPresetName = SPEEDRUN_PRESET_NONE;
static std::string sPresetKey;
static uint32_t sSettingsHash = 0;
static bool sMenuLocked = false;

// The only windows allowed while a speedrun file is loaded. Everything else, menu included, is hidden so settings can't
// be changed mid-run.
static const std::vector<std::string> sAllowedWindows = {
    "Time Splits", "Gameplay Stats", "Additional Timers", "Input Viewer", "Notifications Window", "Modal Window",
};

// What opening the menu toggles instead during a run.
static const std::vector<std::string> sRunToggleWindows = { "Time Splits", "Gameplay Stats" };

// Windows kept hidden while locked, and the ones that were open and get reopened on unlock.
static std::vector<std::shared_ptr<Ship::GuiWindow>> sHiddenWindows;
static std::vector<std::shared_ptr<Ship::GuiWindow>> sReopenWindows;

// The player's own settings are written here while a run replaces them, so they survive a crash mid-run.
static std::string GetBackupPath() {
    return Ship::Context::GetPathRelativeToAppDirectory("speedrun_settings_backup.json");
}

static nlohmann::json GetOwnedBlocks() {
    auto config = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();
    nlohmann::json blocks = nlohmann::json::object();

    for (const char* block : sOwnedBlocks) {
        if (config.contains("CVars") && config["CVars"].contains(block)) {
            blocks[block] = config["CVars"][block];
        } else {
            blocks[block] = nlohmann::json::object();
        }
    }

    return blocks;
}

static void SetOwnedBlocks(const nlohmann::json& blocks) {
    auto config = Ship::Context::GetRawInstance()->GetConfig();

    for (auto& item : blocks.items()) {
        config->SetBlock(spdlog::fmt_lib::format("{}.{}", "CVars", item.key()), item.value());
    }

    // SetBlock writes the config file, so reload cvars from it.
    Ship::Context::GetRawInstance()->GetConsoleVariables()->Load();
    ShipInit::InitAll();
}

// Copies the player's exempt settings onto blocks. Ones the player never set keep the preset's value.
static void OverlayExemptSettings(nlohmann::json& blocks, const nlohmann::json& custom) {
    for (const auto& path : sExemptSettings) {
        if (custom.contains(path)) {
            blocks[path] = custom[path];
        }
    }
}

// Saves the player's settings before a speedrun file replaces them. An existing backup is kept, since it means the
// last run never restored it and it still holds the player's real settings.
static void BackupSettings() {
    if (fs::exists(GetBackupPath())) {
        return;
    }

    std::ofstream file(GetBackupPath());
    if (!file.is_open()) {
        SPDLOG_ERROR("Speedrun: could not write settings backup");
        return;
    }

    file << GetOwnedBlocks().dump(4);
}

static void LockMenu() {
    if (sMenuLocked) {
        return;
    }

    auto gui = Ship::Context::GetRawInstance()->GetWindow()->GetGui();
    if (auto menu = gui->GetMenu()) {
        menu->Hide();
    }

    for (auto& window : SohGui::GetAllGuiWindows()) {
        if (window == nullptr ||
            std::find(sAllowedWindows.begin(), sAllowedWindows.end(), window->GetName()) != sAllowedWindows.end()) {
            continue;
        }
        if (window->IsVisible()) {
            sReopenWindows.push_back(window);
            window->Hide();
        }
        sHiddenWindows.push_back(window);
    }

    sMenuLocked = true;
}

static void UnlockMenu() {
    if (!sMenuLocked) {
        return;
    }

    sMenuLocked = false;
    for (auto& window : sReopenWindows) {
        window->Show();
    }
    sReopenWindows.clear();
    sHiddenWindows.clear();
}

bool Speedrun_EnforceGuiLockdown(Ship::GuiWindow& menu) {
    if (!sMenuLocked) {
        return false;
    }

    // Opening the menu (Esc) toggles the run info windows instead.
    if (menu.IsVisible()) {
        menu.Hide();

        auto gui = Ship::Context::GetRawInstance()->GetWindow()->GetGui();
        bool anyVisible = false;
        for (auto& name : sRunToggleWindows) {
            auto window = gui->GetGuiWindow(name);
            anyVisible |= window != nullptr && window->IsVisible();
        }
        for (auto& name : sRunToggleWindows) {
            if (auto window = gui->GetGuiWindow(name)) {
                anyVisible ? window->Hide() : window->Show();
            }
        }
    }

    for (auto& window : sHiddenWindows) {
        if (window->IsVisible()) {
            window->Hide();
        }
    }

    return true;
}

static void RestoreSettings() {
    UnlockMenu();

    if (fs::exists(GetBackupPath())) {
        try {
            std::ifstream file(GetBackupPath());
            nlohmann::json blocks = nlohmann::json::parse(file);
            file.close();
            SetOwnedBlocks(blocks);
        } catch (const std::exception& e) { SPDLOG_ERROR("Speedrun: could not read settings backup: {}", e.what()); }

        fs::remove(GetBackupPath());
    }
}

extern "C" f32 Ship_FilterGyro(f32 gyroAxis) {
    return IS_SPEEDRUN ? 0.0f : gyroAxis;
}

extern "C" void Ship_PinSpeedrunTimer(s32* left, s32* top) {
    if (IS_SPEEDRUN) {
        *left = OTRGetRectDimensionFromRightEdge(247);
        *top = 0;
    }
}

extern "C" bool Ship_QuestDebugEnabled(u8 questId) {
    return questId != QUEST_SPEEDRUN && questId != QUEST_SPEEDRUN_MASTER &&
           CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0);
}

// FNV-1a hash of the build version and the file's settings, without exempt settings. Same hash means same build and
// settings. nlohmann sorts object keys, so the dump is stable.
static uint32_t HashSettings() {
    nlohmann::json stripped = sSettings;
    for (const auto& path : sExemptSettings) {
        if (stripped.contains(path)) {
            stripped[path.parent_pointer()].erase(path.back());
        }
    }

    std::string data = std::string((const char*)gBuildVersion) + stripped.dump();
    uint32_t hash = 0x811C9DC5;

    for (char c : data) {
        hash ^= (uint8_t)c;
        hash *= 0x01000193;
    }

    return hash;
}

static void EmitHashNotification() {
    Notification::Emit({
        .prefix = "Speedrun",
        .message = sPresetName,
        .suffix = spdlog::fmt_lib::format("{:08X}", sSettingsHash),
        .remainingTime = 15.0f,
    });
}

extern "C" void FileChoose_UpdateSpeedrunMenu(GameState* gameState) {
    FileChoose_UpdateStickDirectionPromptAnim(gameState);
    FileChooseContext* fileChooseContext = (FileChooseContext*)gameState;
    Input* input = &fileChooseContext->state.input[0];
    bool dpad = CVarGetInteger(CVAR_SETTING("DpadInText"), 0);

    // "None" keeps the player's current settings.
    sPresetChoices = GetSpeedrunPresets();
    sPresetChoices.emplace_back(SPEEDRUN_PRESET_NONE, "");
    if (fileChooseContext->speedrunIndex >= sPresetChoices.size()) {
        fileChooseContext->speedrunIndex = 0;
        fileChooseContext->speedrunOffset = 0;
    }

    // Fade in elements after opening the speedrun menu
    fileChooseContext->speedrunUIAlpha += 25;
    if (fileChooseContext->speedrunUIAlpha > 255) {
        fileChooseContext->speedrunUIAlpha = 255;
    }

    // Animate up/down arrows.
    fileChooseContext->speedrunArrowOffset += 1;
    if (fileChooseContext->speedrunArrowOffset >= 30) {
        fileChooseContext->speedrunArrowOffset = 0;
    }

    uint8_t lastIndex = (uint8_t)(sPresetChoices.size() - 1);

    // Move menu selection up or down.
    if (ABS(fileChooseContext->stickRelY) > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
        // Move down
        if (fileChooseContext->stickRelY < -30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN))) {
            // When selecting past the last option, cycle back to the first option.
            if (fileChooseContext->speedrunIndex == lastIndex) {
                fileChooseContext->speedrunIndex = 0;
                fileChooseContext->speedrunOffset = 0;
            } else {
                fileChooseContext->speedrunIndex++;
                // When last visible option is selected when moving down, offset the list down by one.
                if (fileChooseContext->speedrunIndex - fileChooseContext->speedrunOffset >
                    SPEEDRUN_MAX_OPTIONS_ON_SCREEN - 1) {
                    fileChooseContext->speedrunOffset++;
                }
            }
        } else if (fileChooseContext->stickRelY > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DUP))) {
            // When selecting past the first option, cycle back to the last option and offset the list to view it
            // properly.
            if (fileChooseContext->speedrunIndex == 0) {
                fileChooseContext->speedrunIndex = lastIndex;
                fileChooseContext->speedrunOffset =
                    lastIndex >= SPEEDRUN_MAX_OPTIONS_ON_SCREEN ? lastIndex - SPEEDRUN_MAX_OPTIONS_ON_SCREEN + 1 : 0;
            } else {
                // When first visible option is selected when moving up, offset the list up by one.
                if (fileChooseContext->speedrunIndex == fileChooseContext->speedrunOffset) {
                    fileChooseContext->speedrunOffset--;
                }
                fileChooseContext->speedrunIndex--;
            }
        }

        Audio_PlaySfxGeneral(NA_SE_SY_FSEL_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        fileChooseContext->configMode = CM_SPEEDRUN_TO_QUEST;
        return;
    }

    // Go to name entry. The chosen preset is applied when the file gets created.
    if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START)) {
        Audio_PlaySfxGeneral(NA_SE_SY_FSEL_DECIDE_L, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        sPresetName = sPresetChoices[fileChooseContext->speedrunIndex].first;
        sPresetKey = sPresetChoices[fileChooseContext->speedrunIndex].second;
        FileChoose_StartNameEntryFromMenu(fileChooseContext);
    }
}

extern "C" void FileChoose_DrawSpeedrunMenuWindowContents(FileChooseContext* fileChooseContext) {
    OPEN_DISPS(fileChooseContext->state.gfxCtx);

    uint8_t listOffset = fileChooseContext->speedrunOffset;
    int16_t textAlpha = fileChooseContext->speedrunUIAlpha;
    uint8_t optionCount = (uint8_t)sPresetChoices.size();

    // Draw arrows to indicate that the list can scroll up or down.
    // Arrow up
    if (listOffset > 0) {
        uint16_t arrowUpX = 140;
        uint16_t arrowUpY = 76 - (fileChooseContext->speedrunArrowOffset / 10);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowUpTex, G_IM_FMT_IA, G_IM_SIZ_16b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSPWideTextureRectangle(POLY_OPA_DISP++, arrowUpX << 2, arrowUpY << 2, (arrowUpX + 8) << 2, (arrowUpY + 8) << 2,
                                G_TX_RENDERTILE, 0, 0, (1 << 11), (1 << 11));
    }
    // Arrow down
    if (optionCount - listOffset > SPEEDRUN_MAX_OPTIONS_ON_SCREEN) {
        uint16_t arrowDownX = 140;
        uint16_t arrowDownY = 181 + (fileChooseContext->speedrunArrowOffset / 10);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowDownTex, G_IM_FMT_IA, G_IM_SIZ_16b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSPWideTextureRectangle(POLY_OPA_DISP++, arrowDownX << 2, arrowDownY << 2, (arrowDownX + 8) << 2,
                                (arrowDownY + 8) << 2, G_TX_RENDERTILE, 0, 0, (1 << 11), (1 << 11));
    }

    for (uint8_t i = listOffset; i < optionCount && i - listOffset < SPEEDRUN_MAX_OPTIONS_ON_SCREEN; i++) {
        uint16_t textYOffset = (i - listOffset) * 16;
        bool selected = fileChooseContext->speedrunIndex == i;

        uint16_t finalKerning =
            Interface_DrawTextLine(fileChooseContext->state.gfxCtx, (char*)sPresetChoices[i].first.c_str(), 75,
                                   (87 + textYOffset), 255, 255, selected ? 80 : 255, textAlpha, 0.8f, true);

        // Draw arrows around selected option.
        if (selected) {
            Gfx_SetupDL_39Opa(fileChooseContext->state.gfxCtx);
            gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                                G_TX_NOLOD);
            FileChoose_DrawTextRec(fileChooseContext->state.gfxCtx, fileChooseContext->stickLeftPrompt.arrowColorR,
                                   fileChooseContext->stickLeftPrompt.arrowColorG,
                                   fileChooseContext->stickLeftPrompt.arrowColorB, textAlpha, 70.0f,
                                   static_cast<f32>(92 + textYOffset), 0.42f, 0, 0, -1.0f, 1.0f);
            FileChoose_DrawTextRec(fileChooseContext->state.gfxCtx, fileChooseContext->stickRightPrompt.arrowColorR,
                                   fileChooseContext->stickRightPrompt.arrowColorG,
                                   fileChooseContext->stickRightPrompt.arrowColorB, textAlpha,
                                   static_cast<f32>(81 + finalKerning), static_cast<f32>(92 + textYOffset), 0.42f, 0, 0,
                                   1.0f, 1.0f);
        }
    }

    CLOSE_DISPS(fileChooseContext->state.gfxCtx);
}

extern "C" void Speedrun_InitSaveFile(void) {
    nlohmann::json custom = GetOwnedBlocks();
    sSettings = custom;

    if (!sPresetKey.empty()) {
        // Applied to the file's copy only, never the player's config.
        sSettings = applyPresetToBlocks(sPresetKey, sSettings, { PRESET_SECTION_ENHANCEMENTS });
    }

    OverlayExemptSettings(sSettings, custom);

    // Runs are always timed in real time, whatever the "RTA Timing on new files" option says.
    gSaveContext.ship.stats.rtaTiming = 1;

    sSettingsHash = HashSettings();

    EmitHashNotification();
}

// only calls this for speedrun files
static void SaveSaveSection(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("presetName", sPresetName);
    SaveManager::Instance->SaveData("settingsHash", sSettingsHash);
    SaveManager::Instance->SaveData("settings", sSettings);
}

static void LoadSaveSection() {
    if (!IS_SPEEDRUN) {
        return;
    }

    // Lock even if the settings fail to load, it's still a speedrun file.
    LockMenu();

    SaveManager::Instance->LoadData("presetName", sPresetName);
    SaveManager::Instance->LoadData("settings", sSettings);

    if (!sSettings.is_object()) {
        SPDLOG_ERROR("Speedrun: file has no settings to load");
        return;
    }

    // Blocks missing from the file are cleared rather than left as the player's.
    nlohmann::json blocks = sSettings;
    for (const char* block : sOwnedBlocks) {
        if (!blocks.contains(block)) {
            blocks[block] = nlohmann::json::object();
        }
    }

    OverlayExemptSettings(blocks, GetOwnedBlocks());

    BackupSettings();
    SetOwnedBlocks(blocks);
    sSettingsHash = HashSettings();

    EmitHashNotification();
}

void Speedrun_Register() {
    SaveManager::Instance->AddLoadFunction("speedrun", 1, LoadSaveSection);
    SaveManager::Instance->AddSaveFunction("speedrun", 1, SaveSaveSection, true, SECTION_PARENT_NONE);

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](uint32_t fileNum) { RestoreSettings(); });

    REGISTER_VB_SHOULD(VB_SHOW_GAMEPLAY_TIMER, {
        if (IS_SPEEDRUN) {
            *should = true;
        }
    });

    // A backup left from last session means the game closed mid-run without restoring settings.
    RestoreSettings();
}

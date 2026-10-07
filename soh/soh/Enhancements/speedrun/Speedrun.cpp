#include "Speedrun.h"

#include "soh/Enhancements/Presets/Presets.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "soh/ShipInit.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/config/ConfigUpdaters.h"
#include "soh/util.h"

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

void FileChoose_UpdateStickDirectionPromptAnim(GameState* thisx);
int16_t OTRGetRectDimensionFromRightEdge(float v);
}

namespace fs = std::filesystem;

// The cvar blocks a speedrun file owns. They are copied onto the file when it is created and put back every time it is
// loaded. Everything else (controls, resolution, audio) stays with the player, except cosmetics which a run turns off.
static const std::array<const char*, 4> sOwnedBlocks = {
    CVAR_PREFIX_ENHANCEMENT,
    CVAR_PREFIX_RANDOMIZER_ENHANCEMENT,
    CVAR_PREFIX_CHEAT,
    CVAR_PREFIX_DEVELOPER_TOOLS,
};

#define SPEEDRUN_PRESET_NONE "None"
#define SPEEDRUN_MAX_OPTIONS_ON_SCREEN 4
#define SPEEDRUN_EXEMPT_ROWS 4

// {display name, preset name}, ending with "None" which has no preset.
static std::vector<std::pair<std::string, std::string>> sPresetChoices;
static nlohmann::json sSettings = nlohmann::json::object();
static uint32_t sConfigVersion = 0;
// Paths of settings the player keeps even under a preset, from the preset's "exempt" list. Copied over file's
// settings on create and load. Saved with file.
static nlohmann::json sExempt = nlohmann::json::array();
static std::string sPresetName = SPEEDRUN_PRESET_NONE;
static uint32_t sSettingsHash = 0;
static bool sMenuLocked = false;
// "Name: value" of each exempt setting for the preset picked in the menu, and which preset they were made for.
static std::vector<std::string> sExemptLines;
static int sExemptLinesIndex = -1;

// Everything else, menu included, is hidden so settings can't be changed mid-run.
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

static std::vector<nlohmann::json::json_pointer> GetExemptPaths(const nlohmann::json& exempt) {
    std::vector<nlohmann::json::json_pointer> paths;

    for (const auto& path : exempt) {
        try {
            paths.emplace_back(path.get<std::string>());
        } catch (const std::exception& e) { SPDLOG_ERROR("Speedrun: bad exempt path {}: {}", path.dump(), e.what()); }
    }

    return paths;
}

// Copies the player's exempt settings onto blocks. Ones the player never set keep the preset's value.
static void OverlayExemptSettings(nlohmann::json& blocks, const nlohmann::json& custom, const nlohmann::json& exempt) {
    for (const auto& path : GetExemptPaths(exempt)) {
        if (custom.contains(path)) {
            blocks[path] = custom[path];
        }
    }
}

// Saves the player's settings before a speedrun file replaces them. An existing backup is kept, since it means the
// last run never restored it and it still holds the player's real settings. Returns false if nothing is backed up.
static bool BackupSettings() {
    if (fs::exists(GetBackupPath())) {
        return true;
    }

    // Cosmetics are backed up with the owned blocks, since a run clears them too.
    auto config = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();
    nlohmann::json blocks = GetOwnedBlocks();
    blocks[CVAR_PREFIX_COSMETIC] = nlohmann::json::object();
    if (config.contains("CVars") && config["CVars"].contains(CVAR_PREFIX_COSMETIC)) {
        blocks[CVAR_PREFIX_COSMETIC] = config["CVars"][CVAR_PREFIX_COSMETIC];
    }

    nlohmann::json backup;
    backup["blocks"] = blocks;
    if (!SaveManager::WriteFileSafely(GetBackupPath(), backup.dump(4))) {
        SPDLOG_ERROR("Speedrun: could not write settings backup");
        return false;
    }
    return true;
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
            nlohmann::json backup = nlohmann::json::parse(file);
            file.close();
            SetOwnedBlocks(backup.value("blocks", nlohmann::json::object()));
        } catch (const std::exception& e) {
            // Keep the file, it may be the only copy of the player's settings.
            SPDLOG_ERROR("Speedrun: could not read settings backup: {}", e.what());
            return;
        }

        fs::remove(GetBackupPath());
    }
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

static void EmitHashNotification() {
    Notification::Emit({
        .prefix = "Speedrun",
        .message = sPresetName,
        .suffix = spdlog::fmt_lib::format("{:08X}", sSettingsHash),
        .remainingTime = 15.0f,
    });
}

// The settings a new file gets with this preset: the player's settings, then the preset, then the exempt ones put back.
static nlohmann::json BuildFileSettings(const std::string& presetKey, const nlohmann::json& custom,
                                        const nlohmann::json& exempt) {
    nlohmann::json settings = custom;

    if (!presetKey.empty()) {
        // Applied to the file's copy only, never the player's config.
        settings = applyPresetToBlocks(presetKey, settings, { PRESET_SECTION_ENHANCEMENTS });
    }

    OverlayExemptSettings(settings, custom, exempt);
    return settings;
}

static void UpdateExemptLines(uint8_t presetIndex) {
    if (sExemptLinesIndex == presetIndex) {
        return;
    }
    sExemptLinesIndex = presetIndex;
    sExemptLines.clear();

    const std::string& presetKey = sPresetChoices[presetIndex].second;
    nlohmann::json exempt = GetPresetExempt(presetKey);
    nlohmann::json settings = BuildFileSettings(presetKey, GetOwnedBlocks(), exempt);

    for (const auto& path : GetExemptPaths(exempt)) {
        std::string value = "default";
        if (settings.contains(path)) {
            const auto& entry = settings[path];
            value = entry.is_string() ? entry.get<std::string>() : entry.dump();
        }
        sExemptLines.push_back(spdlog::fmt_lib::format("{}: {}", path.back(), value));
    }
}

extern "C" void Speedrun_LoadPresetChoices(FileChooseContext* fileChooseContext) {
    // "None" keeps the player's current settings.
    sPresetChoices = GetSpeedrunPresets();
    sPresetChoices.emplace_back(SPEEDRUN_PRESET_NONE, "");
    if (fileChooseContext->speedrunIndex >= sPresetChoices.size()) {
        fileChooseContext->speedrunIndex = 0;
        fileChooseContext->speedrunOffset = 0;
    }
    // Player may have changed settings since the menu was last open.
    sExemptLinesIndex = -1;
}

extern "C" void FileChoose_UpdateSpeedrunMenu(GameState* gameState) {
    FileChoose_UpdateStickDirectionPromptAnim(gameState);
    FileChooseContext* fileChooseContext = (FileChooseContext*)gameState;
    Input* input = &fileChooseContext->state.input[0];

    FileChoose_UpdateListMenuAnim(&fileChooseContext->speedrunUIAlpha, &fileChooseContext->speedrunArrowOffset);
    FileChoose_MoveListCursor(fileChooseContext, &fileChooseContext->speedrunIndex, &fileChooseContext->speedrunOffset,
                              (uint8_t)sPresetChoices.size(), SPEEDRUN_MAX_OPTIONS_ON_SCREEN);

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        fileChooseContext->configMode = CM_SETTINGS_MENU_TO_QUEST;
        return;
    }

    // Go to name entry. The chosen preset is applied when the file gets created.
    if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START)) {
        Audio_PlaySfxGeneral(NA_SE_SY_FSEL_DECIDE_L, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        FileChoose_StartNameEntryFromMenu(fileChooseContext);
    }
}

extern "C" void FileChoose_DrawSpeedrunMenuWindowContents(FileChooseContext* fileChooseContext) {
    uint8_t listOffset = fileChooseContext->speedrunOffset;
    int16_t textAlpha = fileChooseContext->speedrunUIAlpha;
    uint8_t optionCount = (uint8_t)sPresetChoices.size();

    FileChoose_DrawListScrollArrows(fileChooseContext, listOffset, optionCount, SPEEDRUN_MAX_OPTIONS_ON_SCREEN,
                                    fileChooseContext->speedrunArrowOffset);

    for (uint8_t i = listOffset; i < optionCount && i - listOffset < SPEEDRUN_MAX_OPTIONS_ON_SCREEN; i++) {
        uint16_t textYOffset = (i - listOffset) * 16;
        bool selected = fileChooseContext->speedrunIndex == i;

        uint16_t finalKerning =
            Interface_DrawTextLine(fileChooseContext->state.gfxCtx, (char*)sPresetChoices[i].first.c_str(), 75,
                                   (87 + textYOffset), 255, 255, selected ? 80 : 255, textAlpha, 0.8f, true);

        // Draw arrows around selected option.
        if (selected) {
            FileChoose_DrawListCursorArrow(fileChooseContext, textAlpha, 70.0f, static_cast<f32>(92 + textYOffset),
                                           true);
            FileChoose_DrawListCursorArrow(fileChooseContext, textAlpha, static_cast<f32>(81 + finalKerning),
                                           static_cast<f32>(92 + textYOffset), false);
        }
    }

    // Exempt settings of the selected preset, with the values the new file will get, in columns under the list.
    UpdateExemptLines(fileChooseContext->speedrunIndex);
    for (size_t i = 0; i < sExemptLines.size(); i++) {
        int16_t x = 65 + (int16_t)(i / SPEEDRUN_EXEMPT_ROWS) * 100;
        int16_t y = 87 + SPEEDRUN_MAX_OPTIONS_ON_SCREEN * 16 + (int16_t)(i % SPEEDRUN_EXEMPT_ROWS) * 7;
        Interface_DrawTextLine(fileChooseContext->state.gfxCtx, (char*)sExemptLines[i].c_str(), x, y, 180, 180, 180,
                               textAlpha, 0.5f, false);
    }
}

extern "C" void Speedrun_InitSaveFile(u8 presetIndex) {
    const std::string& presetKey = sPresetChoices[presetIndex].second;
    sPresetName = sPresetChoices[presetIndex].first;

    nlohmann::json custom = GetOwnedBlocks();
    sConfigVersion = SOH::GetLatestConfigVersion();
    sExempt = GetPresetExempt(presetKey);
    sSettings = BuildFileSettings(presetKey, custom, sExempt);

    // Runs are always timed in real time, whatever the "RTA Timing on new files" option says.
    gSaveContext.ship.stats.rtaTiming = 1;

    sSettingsHash = SohUtils::Hash(std::string((const char*)gBuildVersion) + GetPresetFileContents(presetKey));

    EmitHashNotification();
}

// only calls this for speedrun files
static void SaveSaveSection(const SaveContext& saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("presetName", sPresetName);
    SaveManager::Instance->SaveData("settingsHash", sSettingsHash);
    SaveManager::Instance->SaveData("settings", sSettings);
    SaveManager::Instance->SaveData("configVersion", sConfigVersion);
    SaveManager::Instance->SaveData("exempt", sExempt);
}

static void LoadSaveSection() {
    if (!IS_SPEEDRUN) {
        return;
    }

    // Lock even if the settings fail to load, it's still a speedrun file.
    LockMenu();

    SaveManager::Instance->LoadData("presetName", sPresetName);
    SaveManager::Instance->LoadData("settingsHash", sSettingsHash);
    SaveManager::Instance->LoadData("settings", sSettings);
    SaveManager::Instance->LoadData("configVersion", sConfigVersion);
    SaveManager::Instance->LoadData("exempt", sExempt);

    if (!sExempt.is_array()) {
        sExempt = nlohmann::json::array();
    }

    if (!sSettings.is_object()) {
        SPDLOG_ERROR("Speedrun: file has no settings to load");
        return;
    }

    // Don't touch the player's settings unless they can be put back.
    if (!BackupSettings()) {
        return;
    }

    // Blocks missing from the file are cleared rather than left as the player's.
    nlohmann::json custom = GetOwnedBlocks();
    nlohmann::json blocks = sSettings;
    for (const char* block : sOwnedBlocks) {
        if (!blocks.contains(block)) {
            blocks[block] = nlohmann::json::object();
        }
    }

    // Settings from an older build are migrated after they're live, since the updaters work on cvars. The file keeps
    // its old settings and version, so the hash stays the same on every load.
    if (sConfigVersion < SOH::GetLatestConfigVersion()) {
        SetOwnedBlocks(blocks);
        SOH::RunVersionUpdatesFrom(sConfigVersion);
        // Updaters change cvars, not the config json GetOwnedBlocks reads.
        Ship::Context::GetRawInstance()->GetConsoleVariables()->Save();
        blocks = GetOwnedBlocks();
    }

    OverlayExemptSettings(blocks, custom, sExempt);

    // No cosmetics during a run. Some scale Link or his sword, which changes his hitbox and reach.
    blocks[CVAR_PREFIX_COSMETIC] = nlohmann::json::object();

    SetOwnedBlocks(blocks);

    EmitHashNotification();
}

void Speedrun_Register() {
    SaveManager::Instance->AddLoadFunction("speedrun", 1, LoadSaveSection);
    SaveManager::Instance->AddSaveFunction("speedrun", 1, SaveSaveSection, true, SECTION_PARENT_NONE);

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](uint32_t fileNum) { RestoreSettings(); });

    // No gyro aiming in a run. Cleared after the pad is read and before the frame uses it.
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateMainStart>([]() {
        if (IS_SPEEDRUN && gGameState != nullptr) {
            gGameState->input[0].cur.gyro_x = 0.0f;
            gGameState->input[0].cur.gyro_y = 0.0f;
        }
    });

    REGISTER_VB_SHOULD(VB_SHOW_GAMEPLAY_TIMER, {
        if (IS_SPEEDRUN) {
            *should = true;
        }
    });

    // A backup left from last session means the game closed mid-run without restoring settings.
    RestoreSettings();
}

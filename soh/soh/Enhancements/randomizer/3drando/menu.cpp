#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <ctime>

#include "cosmetics.hpp"
#include "menu.hpp"
#include "patch.hpp"
#include "preset.hpp"
#include "randomizer.hpp"
#include "settings.hpp"
#include "spoiler_log.hpp"
#include "location_access.hpp"
#include "debug.hpp"
#include <spdlog/spdlog.h>
#include "../../randomizer/randomizerTypes.h"

namespace {
bool seedChanged;
uint16_t pastSeedLength;
std::vector<std::string> presetEntries;
std::vector<Menu*> menuList;
Option* currentSetting;
Menu* currentMenu;
} // namespace

void PrintTopScreen() {
    SPDLOG_DEBUG("\x1b[2;11H%sOcarina of Time 3D Randomizer%s", CYAN, RESET);
    SPDLOG_DEBUG("\x1b[3;18H%s%s-%s%s", CYAN, RANDOMIZER_VERSION, COMMIT_NUMBER, RESET);
    SPDLOG_DEBUG("\x1b[4;10HA/B/D-pad: Navigate Menu\n");
    SPDLOG_DEBUG("            Select: Exit to Homebrew Menu\n");
    SPDLOG_DEBUG("                 Y: New Random Seed\n");
    SPDLOG_DEBUG("                 X: Input Custom Seed\n");
    SPDLOG_DEBUG("\x1b[11;7HCurrent Seed: %s", Settings::seed.c_str());
}

void MenuInit() {
    Settings::InitSettings();

    seedChanged = false;
    pastSeedLength = Settings::seed.length();

    Menu* main = new Menu("Main", MenuType::MainMenu, &Settings::mainMenu, MAIN_MENU);
    menuList.push_back(main);
    currentMenu = main;

    srand(time(NULL));

    if (!CreatePresetDirectories()) {
        printf("\x1b[20;5Failed to create preset directories.");
        printf("\x1b[21;5Loading presets might crash.");
    }

    // If cached presets exist, load them
    LoadCachedSettings();
    LoadCachedCosmetics();
    // If Randomize all settings in a category is selected
    // Re-randomize them
    Settings::RandomizeAllSettings();

    PrintTopScreen();

    PrintMainMenu();
}

#define KEY_DUP 0
#define KEY_DDOWN 1
#define KEY_DLEFT 2
#define KEY_DRIGHT 3
#define KEY_A 4
#define KEY_B 5
#define KEY_Y 6
#define KEY_X 7

void MoveCursor(uint32_t kDown, bool updatedByHeld) {
    // Option sub menus need special checking for locked options
    if (currentMenu->mode == OPTION_SUB_MENU) {
        // Cancel if holding and reached first/last selectable option
        if (updatedByHeld) {
            bool noSelectableOption = true;
            if (kDown & KEY_DUP) {
                for (int i = currentMenu->menuIdx - 1; i >= 0; i--) {
                    if (!currentMenu->settingsList->at(i)->IsHidden() &&
                        !currentMenu->settingsList->at(i)->IsLocked()) {
                        noSelectableOption = false;
                        break;
                    }
                }
            }
            if (kDown & KEY_DDOWN) {
                for (size_t i = currentMenu->menuIdx + 1; i < currentMenu->settingsList->size(); i++) {
                    if (!currentMenu->settingsList->at(i)->IsHidden() &&
                        !currentMenu->settingsList->at(i)->IsLocked()) {
                        noSelectableOption = false;
                        break;
                    }
                }
            }
            if (noSelectableOption) {
                return;
            }
        }
        // Loop through settings until an unlocked one is reached
        do {
            if ((kDown & KEY_DUP) != 0) {
                currentMenu->menuIdx--;
            }
            if ((kDown & KEY_DDOWN) != 0) {
                currentMenu->menuIdx++;
            }

            // Bounds checking
            if (currentMenu->menuIdx == currentMenu->settingsList->size()) {
                currentMenu->menuIdx = 0;
            } else if (currentMenu->menuIdx == 0xFFFF) {
                currentMenu->menuIdx = static_cast<uint16_t>(currentMenu->settingsList->size() - 1);
            }

            currentSetting = currentMenu->settingsList->at(currentMenu->menuIdx);
        } while (currentSetting->IsLocked() || currentSetting->IsHidden());
    }
    // All other menus except reset-to-defaults confirmation
    else if (currentMenu->mode != RESET_TO_DEFAULTS) {
        // Cancel if holding and reached first/last menu
        if (updatedByHeld) {
            if ((kDown & KEY_DUP && currentMenu->menuIdx == 0) ||
                (kDown & KEY_DDOWN && currentMenu->menuIdx == currentMenu->itemsList->size() - 1)) {
                return;
            }
        }

        if (kDown & KEY_DUP) {
            currentMenu->menuIdx--;
        }
        if (kDown & KEY_DDOWN) {
            currentMenu->menuIdx++;
        }

        // Bounds checking
        uint16_t max = -1;
        if (currentMenu->mode == LOAD_PRESET || currentMenu->mode == DELETE_PRESET) { // Number of presets if applicable
            max = presetEntries.size();
        } else if (currentMenu->mode == GENERATE_MODE) { // Generate menu: 2 options
            max = 2;
        } else if (currentMenu->itemsList != nullptr) {
            max = currentMenu->itemsList->size(); // Default max: Number of items in menu
        }
        if (currentMenu->menuIdx == max) {
            currentMenu->menuIdx = 0;
        } else if (currentMenu->menuIdx == 0xFFFF) {
            currentMenu->menuIdx = max - 1;
        }

        // Scroll Check
        if (currentMenu->menuIdx > currentMenu->settingBound + (MAX_SUBMENUS_ON_SCREEN - 1)) {
            currentMenu->settingBound = currentMenu->menuIdx - (MAX_SUBMENUS_ON_SCREEN - 1);
        } else if (currentMenu->menuIdx < currentMenu->settingBound) {
            currentMenu->settingBound = currentMenu->menuIdx;
        }
    }
}

void MenuUpdate(uint32_t kDown, bool updatedByHeld) {
    // Check for menu change
    // If user pressed A on a non-option, non-action menu, they're navigating to a new menu
    if (kDown & KEY_A && currentMenu->mode != OPTION_SUB_MENU && currentMenu->type != MenuType::Action) {
        if (currentMenu->itemsList->size() > currentMenu->menuIdx) {
            Menu* newMenu;
            newMenu = currentMenu->itemsList->at(currentMenu->menuIdx);
            menuList.push_back(newMenu);
            currentMenu = menuList.back();
            ModeChangeInit();
            kDown = 0;
        }
        // If they pressed B on any menu other than main, go backwards to the previous menu
    } else if (kDown & KEY_B && currentMenu->mode != MAIN_MENU) {
        // Want to reset generate menu when leaving
        if (currentMenu->mode == POST_GENERATE) {
            currentMenu->mode = GENERATE_MODE;
        }
        PrintTopScreen();
        menuList.pop_back();
        currentMenu = menuList.back();
        ModeChangeInit();
        kDown = 0;
    }

    if (currentMenu->mode != POST_GENERATE) {

        // New Random Seed
        if (kDown & KEY_Y) {
            pastSeedLength = Settings::seed.length();
            Settings::seed = std::to_string(rand());
            seedChanged = true;
        }

        // Input Custom Seed
        if (kDown & KEY_X) {
            pastSeedLength = Settings::seed.length();
            Settings::seed = GetInput("Enter Seed");
            seedChanged = true;
        }

        // Reprint seed if it changed
        if (seedChanged) {
            std::string spaces = "";
            spaces.append(pastSeedLength, ' ');
            printf("\x1b[11;21H%s", spaces.c_str());
            printf("\x1b[11;21H%s", Settings::seed.c_str());
            seedChanged = false;
        }
    }

    // Print current menu (if applicable)
    MoveCursor(kDown, updatedByHeld); // Move cursor, if applicable
    if (currentMenu->mode == MAIN_MENU) {
        PrintMainMenu();
        ClearDescription();
    } else if (currentMenu->mode == OPTION_SUB_MENU) {
        UpdateOptionSubMenu(kDown);
        PrintOptionSubMenu();
    } else if (currentMenu->mode == LOAD_PRESET) {
        UpdatePresetsMenu(kDown);
        PrintPresetsMenu();
    } else if (currentMenu->mode == DELETE_PRESET) {
        UpdatePresetsMenu(kDown);
        PrintPresetsMenu();
    } else if (currentMenu->mode == RESET_TO_DEFAULTS) {
        UpdateResetToDefaultsMenu(kDown);
        PrintResetToDefaultsMenu();
    } else if (currentMenu->mode == GENERATE_MODE) {
        UpdateGenerateMenu(kDown);
        if (currentMenu->mode != POST_GENERATE) {
            PrintGenerateMenu();
        }
    } else if (currentMenu->mode == SUB_MENU) {
        PrintSubMenu();
    }
}

void ModeChangeInit() {
    if (currentMenu->mode == OPTION_SUB_MENU) {
        // loop through until we reach an unlocked setting
        while (currentMenu->settingsList->at(currentMenu->menuIdx)->IsLocked() ||
               currentMenu->settingsList->at(currentMenu->menuIdx)->IsHidden()) {
            currentMenu->menuIdx++;
        }
        currentSetting = currentMenu->settingsList->at(currentMenu->menuIdx);

    } else if (currentMenu->mode == SAVE_PRESET) {
        ClearDescription();
        if (SaveSpecifiedPreset(GetInput("Preset Name").substr(0, 19), OptionCategory::Setting)) {
            printf("\x1b[24;5HPreset Saved!");
            printf("\x1b[26;5HPress B to return to the preset menu.");
        } else {
            printf("\x1b[24;5HFailed to save preset.");
            printf("\x1b[26;5HPress B to return to the preset menu.");
        }

    } else if (currentMenu->mode == LOAD_PRESET || currentMenu->mode == DELETE_PRESET) {
        presetEntries = GetSettingsPresets();

    } else if (currentMenu->mode == GENERATE_MODE) {
    }
}

void UpdateCustomCosmeticColors(uint32_t kDown) {
    if (kDown & KEY_A) {
        if (currentSetting->GetSelectedOptionText().compare(0, 8, Cosmetics::CUSTOM_COLOR_PREFIX) == 0) {
            std::string newColor = GetInput("Enter a 6 digit hex color").substr(0, 6);
            if (Cosmetics::ValidHexString(newColor)) {
                currentSetting->SetSelectedOptionText(Cosmetics::CustomColorOptionText(newColor));
            }
        }
    }
}

void UpdateOptionSubMenu(uint32_t kDown) {
    if ((kDown & KEY_DRIGHT) != 0) {
        currentSetting->NextOptionIndex();
    }
    if ((kDown & KEY_DLEFT) != 0) {
        currentSetting->PrevOptionIndex();
    }

    // Bounds checking
    currentSetting->SanitizeSelectedOptionIndex();

    currentSetting->SetVariable();
    Settings::ForceChange(kDown, currentSetting);
    UpdateCustomCosmeticColors(kDown);
}

void UpdatePresetsMenu(uint32_t kDown) {
    // clear any potential message
    ClearDescription();
    if (kDown & KEY_A && currentMenu->mode == LOAD_PRESET && !presetEntries.empty()) {
        if (LoadPreset(presetEntries[currentMenu->menuIdx], OptionCategory::Setting)) {
            Settings::ResolveExcludedLocationConflicts();
            for (Menu* menu : Settings::GetAllOptionMenus()) {
                menu->ResetMenuIndex();
            }
            printf("\x1b[24;5HPreset Loaded!");
        } else {
            printf("\x1b[24;5HFailed to load preset.");
        }
    } else if (kDown & KEY_A && currentMenu->mode == DELETE_PRESET && !presetEntries.empty()) {
        if (DeletePreset(presetEntries[currentMenu->menuIdx], OptionCategory::Setting)) {
            presetEntries.erase(presetEntries.begin() + currentMenu->menuIdx);
            if (currentMenu->menuIdx == presetEntries.size()) { // Catch when last preset is deleted
                currentMenu->menuIdx--;
            }
            printf("\x1b[24;5HPreset Deleted.");
        } else {
            printf("\x1b[24;5HFailed to delete preset.");
        }
    }
}

void UpdateResetToDefaultsMenu(uint32_t kDown) {
    // clear any potential message
    ClearDescription();
    if (kDown & KEY_A) {
        Settings::SetDefaultSettings();
        printf("\x1b[24;5HSettings have been reset to defaults.");
    }
}

void UpdateGenerateMenu(uint32_t kDown) {
    if ((kDown & KEY_A) != 0) {
        Settings::PlayOption = currentMenu->menuIdx;
        // GenerateRandomizer();
        // This is just a dummy mode to stop the prompt from appearing again
        currentMenu->mode = POST_GENERATE;
    }
}

void PrintMainMenu() {
    printf("\x1b[0;%dHMain Settings", 1 + (BOTTOM_WIDTH - 13) / 2);

    for (uint8_t i = 0; i < MAX_SUBMENUS_ON_SCREEN; i++) {
        if (i >= Settings::mainMenu.size())
            break;

        Menu* menu = Settings::mainMenu[i];

        uint8_t row = 3 + i;
        // make the current menu green
        if (currentMenu->menuIdx == i) {
            printf("\x1b[%d;%dH%s>", row, 2, GREEN);
            printf("\x1b[%d;%dH%s%s", row, 3, menu->name.c_str(), RESET);
        } else {
            printf("\x1b[%d;%dH%s", row, 3, menu->name.c_str());
        }
    }
}

void PrintOptionSubMenu() {
    // bounds checking incase settings go off screen
    // this is complicated to account for hidden settings and there's probably a better way to do it
    uint16_t hiddenSettings = 0;
    uint16_t visibleSettings = 0;
    for (uint16_t i = currentMenu->settingBound; visibleSettings < MAX_SUBMENU_SETTINGS_ON_SCREEN; i++) {
        if (i >= currentMenu->settingsList->size()) {
            break;
        }
        if (currentMenu->settingsList->at(i)->IsHidden()) {
            hiddenSettings++;
        } else {
            visibleSettings++;
        }
    }
    bool isLastVisibleSetting = true;
    for (size_t i = currentMenu->menuIdx + 1; i < currentMenu->settingsList->size(); i++) {
        if (!currentMenu->settingsList->at(i)->IsHidden()) {
            isLastVisibleSetting = false;
            break;
        }
    }
    if (currentMenu->menuIdx >=
        currentMenu->settingBound - (isLastVisibleSetting ? 0 : 1) + MAX_SUBMENU_SETTINGS_ON_SCREEN + hiddenSettings) {
        currentMenu->settingBound = currentMenu->menuIdx;
        uint8_t offset = 0;
        // skip over hidden settings
        while (offset < MAX_SUBMENU_SETTINGS_ON_SCREEN - (isLastVisibleSetting ? 1 : 2)) {
            currentMenu->settingBound--;
            if (currentMenu->settingBound == 0) {
                break;
            }
            offset += currentMenu->settingsList->at(currentMenu->settingBound)->IsHidden() ? 0 : 1;
        }
    } else if (currentMenu->menuIdx < currentMenu->settingBound + 1) {
        currentMenu->settingBound = std::max(currentMenu->menuIdx - 1, 0);
    }

    // print menu name
    printf("\x1b[0;%dH%s", 1 + (BOTTOM_WIDTH - currentMenu->name.length()) / 2, currentMenu->name.c_str());

    // keep count of hidden settings to not make blank spaces appear in the list
    hiddenSettings = 0;

    for (uint8_t i = 0; i - hiddenSettings < MAX_SUBMENU_SETTINGS_ON_SCREEN; i++) {
        // break if there are no more settings to print
        if (i + currentMenu->settingBound >= currentMenu->settingsList->size())
            break;

        Option* setting = currentMenu->settingsList->at(i + currentMenu->settingBound);

        uint8_t row = 3 + ((i - hiddenSettings) * 2);
        // make the current setting green
        if (currentMenu->menuIdx == i + currentMenu->settingBound) {
            printf("\x1b[%d;%dH%s>", row, 1, GREEN);
            printf("\x1b[%d;%dH%s:", row, 2, setting->GetName().data());
            printf("\x1b[%d;%dH%s%s", row, 26, setting->GetSelectedOptionText().data(), RESET);
            // dim to make a locked setting grey
        } else if (setting->IsLocked()) {
            printf("\x1b[%d;%dH%s%s:", row, 2, DIM, setting->GetName().data());
            printf("\x1b[%d;%dH%s%s", row, 26, setting->GetSelectedOptionText().data(), RESET);
            // don't display hidden settings
        } else if (setting->IsHidden()) {
            hiddenSettings++;
            continue;
        } else {
            printf("\x1b[%d;%dH%s:", row, 2, setting->GetName().data());
            printf("\x1b[%d;%dH%s", row, 26, setting->GetSelectedOptionText().data());
        }
    }

    PrintOptionDescription();
}

void PrintSubMenu() {
    printf("\x1b[0;%dH%s", 1 + (BOTTOM_WIDTH - currentMenu->name.length()) / 2, currentMenu->name.c_str());

    for (uint8_t i = 0; i < MAX_SUBMENUS_ON_SCREEN; i++) {
        if (i >= currentMenu->itemsList->size())
            break;

        uint8_t row = 3 + i;
        // make the current menu green
        if (currentMenu->menuIdx == currentMenu->settingBound + i) {
            printf("\x1b[%d;%dH%s>", row, 2, GREEN);
            printf("\x1b[%d;%dH%s%s", row, 3, currentMenu->itemsList->at(currentMenu->settingBound + i)->name.c_str(),
                   RESET);
        } else {
            printf("\x1b[%d;%dH%s", row, 3, currentMenu->itemsList->at(currentMenu->settingBound + i)->name.c_str());
        }
    }
}

void PrintPresetsMenu() {
    if (presetEntries.empty()) {
        printf("\x1b[10;4HNo Presets Detected!");
        printf("\x1b[12;4HPress B to return to the preset menu.");
        return;
    }

    if (currentMenu->mode == LOAD_PRESET) {
        printf("\x1b[0;%dHSelect a Preset to Load", 1 + (BOTTOM_WIDTH - 23) / 2);
    } else if (currentMenu->mode == DELETE_PRESET) {
        printf("\x1b[0;%dHSelect a Preset to Delete", 1 + (BOTTOM_WIDTH - 25) / 2);
    }

    for (uint8_t i = 0; i < MAX_SUBMENU_SETTINGS_ON_SCREEN; i++) {
        if (i >= presetEntries.size())
            break;

        std::string preset = presetEntries[i];

        uint8_t row = 3 + (i * 2);
        // make the current preset green
        if (currentMenu->menuIdx == i) {
            printf("\x1b[%d;%dH%s>", row, 14, GREEN);
            printf("\x1b[%d;%dH%s%s", row, 15, preset.c_str(), RESET);
        } else {
            printf("\x1b[%d;%dH%s", row, 15, preset.c_str());
        }
    }
}

void PrintResetToDefaultsMenu() {
  printf("\x1b[10;4HPress A to reset to default settings.");
  printf("\x1b[12;4HPress B to return to the preset menu.");
}

void PrintGenerateMenu() {
  printf("\x1b[3;%dHHow will you play?", 1+(BOTTOM_WIDTH-18)/2);
  std::vector<std::string> playOptions = {"3ds Console", "Citra Emulator"};

  for (uint8_t i = 0; i < playOptions.size(); i++) {

    std::string option = playOptions[i];
    uint8_t row = 6 + (i * 2);
    //make the current selection green
    if (currentMenu->menuIdx == i) {
      printf("\x1b[%d;%dH%s>",   row, 14, GREEN);
      printf("\x1b[%d;%dH%s%s",  row, 15, option.c_str(), RESET);
    } else {
      printf("\x1b[%d;%dH%s",    row, 15, option.c_str());
    }
  }
}

void ClearDescription() {
  //clear the previous description
  std::string spaces = "";
  spaces.append(9 * TOP_WIDTH, ' ');
  printf("\x1b[22;0H%s", spaces.c_str());
}

void PrintOptionDescription() {
  ClearDescription();
  std::string_view description = currentSetting->GetSelectedOptionDescription();

  printf("\x1b[22;0H%s", description.data());
}

std::string GenerateRandomizer(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations) {
    // if a blank seed was entered, make a random one
    srand(time(NULL));
    Settings::seed = std::to_string(rand());

    int ret = Playthrough::Playthrough_Init(std::hash<std::string>{}(Settings::seed), cvarSettings, excludedLocations);
    if (ret < 0) {
        if (ret == -1) { // Failed to generate after 5 tries
            printf("\n\nFailed to generate after 5 tries.\nPress B to go back to the menu.\nA different seed might be "
                   "successful.");
            SPDLOG_DEBUG("\nRANDOMIZATION FAILED COMPLETELY. PLZ FIX\n");
            return "";
        } else {
            printf("\n\nError %d with fill.\nPress Select to exit or B to go back to the menu.\n", ret);
            return "";
        }
    }

    // Restore settings that were set to a specific value for vanilla logic
    if (Settings::Logic.Is(LOGIC_VANILLA)) {
        for (Option* setting : Settings::vanillaLogicDefaults) {
            setting->RestoreDelayedOption();
        }
        Settings::Keysanity.RestoreDelayedOption();
    }
    std::ostringstream fileNameStream;
    for (int i = 0; i < Settings::hashIconIndexes.size(); i++) {
        if (i) {
            fileNameStream << '-';
        }
        if (Settings::hashIconIndexes[i] < 10) {
            fileNameStream << '0';
        }
        fileNameStream << std::to_string(Settings::hashIconIndexes[i]);
    }
    std::string fileName = fileNameStream.str();
    return "./Randomizer/" + fileName + ".json";
}

std::string GetInput(const char* hintText) {
    return std::string();
}

#include "FileSelectEnhancements.h"

#include "soh/OTRGlobals.h"
#include "soh/SohGui/SohModals.h"
#include "soh/SohGui/SohGui.hpp"

#include <array>
#include <string>

std::array<std::string, LANGUAGE_MAX> RandomizerSettingsMenuText[RSM_MAX] = {
    {
        // English
        "Start Randomizer",
        // German
        "Randomizer starten",
        // French
        "Commencer le Randomizer",
    },
    {
        // English
        "Generate New Randomizer Seed",
        // German
        "Neuen Randomizer Seed generieren",
        // French
        "Générer une nouvelle seed pour le Randomizer",
    },
    {
        // English
        "Open Randomizer Settings",
        // German
        "Randomizer Optionen öffnen",
        // French
        "Ouvrir les paramètres du Randomizer",
    },
    {
        // English
        "Generating...",
        // German
        "Generiere...",
        // French
        "Génération en cours...",
    },
    { // English
      "No randomizer seed loaded.\nPlease generate one first"
#if defined(__WIIU__) || defined(__SWITCH__)
      ".",
#else
      ",\nor drop a spoiler log on the game window.",
#endif
      // German
      "Kein Randomizer Seed gefunden.\nBitte generiere zuerst einen"
#if defined(__WIIU__) || defined(__SWITCH__)
      ".",
#else
      ",\noder ziehe ein Spoiler Log\nauf das Spielfenster.",
#endif
      // French
      "Aucune Seed de Randomizer actuellement disponible.\nGénérez-en une dans les \"Randomizer Settings\""
#if (defined(__WIIU__) || defined(__SWITCH__))
      "."
#else
      "\nou glissez un spoilerlog sur la fenêtre du jeu."
#endif
    },
};

std::array<std::string, LANGUAGE_MAX> ArchipelagoSettingsMenuText[ASM_MAX]{
    // ASM_START_ARCHIPELAGO
    {
        "Start Archipelago",
        "Start Archipelago",
        "Start Archipelago",
    },
    // ASM_CHANGE_CONNECTION_INFO
    {
        "Change Connection Info",
        "Change Connection Info",
        "Change Connection Info",
    },
    // ASM_SERVER_ADDRESS
    {
        "Server Address: ",
        "Server Address: ",
        "Server Address: ",
    },
    // ASM_SLOT_NAME
    {
        "Slot Name: ",
        "Slot Name: ",
        "Slot Name: ",
    },
    // ASM_NOT_CONNECTED
    {
        "Not Connected",
        "Not Connected",
        "Not Connected",
    },
    // ASM_CONNECTING
    {
        "Connecting...",
        "Connecting...",
        "Connecting...",
    },
    // ASM_CONNECTED
    {
        "Connected",
        "Connected",
        "Connected",
    },
    // ASM_LOADING_DATA
    {
        "Loading Data...",
        "Loading Data...",
        "Loading Data...",
    },
    // ASM_DATA_LOADED
    {
        "Ready to begin",
        "Ready to begin",
        "Ready to begin",
    },
    // ASM_STATUS
    {
        "Status:",
        "Status:",
        "Status:",
    },
    // ASM_CHAR_START_TO_CONNECT
    {
        "Start to automatically connect to this slot",
        "Start to automatically connect to this slot",
        "Start to automatically connect to this slot",
    },
    // ASM_CHAR_SELECT_CONNECTED_TO_OTHER_SLOT
    {
        "Connected to a different slot",
        "Connected to a different slot",
        "Connected to a different slot",
    },
    // ASM_CONNECT_AND_START_ARCHIPELAGO
    { "Connect and Start Archipelago", "Connect and Start Archipelago", "Connect and Start Archipelago" }
};

const char* SohFileSelect_GetRandomizerSettingText(uint8_t optionIndex, uint8_t language) {
    return RandomizerSettingsMenuText[optionIndex][language].c_str();
}

const char* SohFileSelect_GetArchipelagoSettingText(uint8_t optionIndex, uint8_t language) {
    return ArchipelagoSettingsMenuText[optionIndex][language].c_str();
}

void SohFileSelect_ShowPresetMenu() {
    SohGui::ShowEscMenu();
    CVarSetString(CVAR_SETTING("Menu.ActiveHeader"), "Settings");
    CVarSetString(CVAR_SETTING("Menu.SettingsSidebarSection"), "Presets");
    CVarSetInteger(CVAR_SETTING("Menu.HasSeenPresetModal"), 1);
}

void SohFileSelect_DismissPresetModal() {
    CVarSetInteger(CVAR_SETTING("Menu.HasSeenPresetModal"), 1);
}

void SohFileSelect_ShowPresetModal() {
    if (CVarGetInteger(CVAR_SETTING("Menu.HasSeenPresetModal"), 0)) {
        return;
    }
    std::shared_ptr<SohModalWindow> modal = static_pointer_cast<SohModalWindow>(
        Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Modal Window"));
    if (modal->IsPopupOpen("Take a look at our presets!")) {
        modal->DismissPopup();
    } else {
        modal->RegisterPopup("Take a look at our presets!",
                             "\nHey there! Ship comes with a ton of options, but none of them are on by default,\n"
                             "even in randomizer. If you haven't already, we highly recommend applying the\n"
                             "\"Enhancements - Curated Randomizer\" preset for a great, curated out of the\n"
                             "box rando experience.\n"
                             "\n"
                             "Afterwards, consider taking a look at the rest of the ESC menu to further tweak \n"
                             "the experience to your liking!\n ",
                             "Cool, show me the presets!", "Got it, just let me play!", SohFileSelect_ShowPresetMenu,
                             SohFileSelect_DismissPresetModal);
    }
}

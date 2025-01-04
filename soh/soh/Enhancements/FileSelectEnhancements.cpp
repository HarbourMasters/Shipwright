#include "FileSelectEnhancements.h"

#include "soh/OTRGlobals.h"

#include <array>
#include <string>
#include <vector>

std::array<std::string, LANGUAGE_MAX> RandomizerSettingsMenuText[RSM_MAX] = {     
    {
        // English
        "Start Randomizer",
        // German
        "Start Randomizer",
        // French
        "Start Randomizer",
    },
    {
        // English
        "Generate New Randomizer Seed",
        // German
        "Generate New Randomizer Seed",
        // French
        "Generate New Randomizer Seed",
    },
    {
        // English
        "Open Randomizer Settings",
        // German
        "Open Randomizer Settings",
        // French
        "Open Randomizer Settings",
    },
    {
        // English
        "Generating...",
        // German
        "Generating...",
        // French
        "Generating...",
    },
    {
        // English
        "No randomizer seed loaded.\nPlease generate one first"
    #if defined(__WIIU__) || defined(__SWITCH__)
        ".",
    #else
        ",\nor drop a spoiler log on the game window.",
    #endif
        // German
        "No randomizer seed loaded.\nPlease generate one first"
    #if defined(__WIIU__) || defined(__SWITCH__)
        ".",
    #else
        ",\nor drop a spoiler log on the game window.",
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

const char* SohFileSelect_GetSettingText(uint8_t optionIndex, uint8_t language) {
    return RandomizerSettingsMenuText[optionIndex][language].c_str();
}

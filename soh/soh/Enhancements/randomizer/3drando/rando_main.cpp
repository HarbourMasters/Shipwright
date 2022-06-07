#include "menu.hpp"
#include "hint_list.hpp"
#include "item_list.hpp"
#include "item_location.hpp"
#include "location_access.hpp"
#include "rando_main.hpp"
// #include <soh/Enhancements/randomizer.h>
#include <Cvar.h>
#include <GameSettings.h>

#define TICKS_PER_SEC 268123480.0

void RandoMain::GenerateRando(std::unordered_map<RandomizerSettingKey, u8> cvarSettings) {
    HintTable_Init();
    ItemTable_Init();
    LocationTable_Init();

    // std::string settingsFileName = "./randomizer/latest_settings.json";
    // CVar_SetString("gLoadedPreset", settingsFileName.c_str());

    std::string fileName = GenerateRandomizer(cvarSettings);
    CVar_SetString("gSpoilerLog", fileName.c_str());

    Game::SaveSettings();
    Game::LoadSettings();
    CVar_SetS32("gDroppedNewSpoilerFile", 1);
}
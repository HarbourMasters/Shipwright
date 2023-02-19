#include "menu.hpp"
#include "hint_list.hpp"
#include "item_list.hpp"
#include "item_location.hpp"
#include "location_access.hpp"
#include "rando_main.hpp"
// #include <soh/Enhancements/randomizer.h>
#include <libultraship/bridge.h>
#include <Window.h>
#include <libultraship/libultra/types.h>

#define TICKS_PER_SEC 268123480.0

void RandoMain::GenerateRando(std::unordered_map<RandomizerSettingKey, u8> cvarSettings, std::set<RandomizerCheck> excludedLocations,
    std::string seedInput) {
    HintTable_Init();
    ItemTable_Init();
    LocationTable_Init();

    // std::string settingsFileName = "./randomizer/latest_settings.json";
    // CVarSetString("gLoadedPreset", settingsFileName.c_str());

    std::string fileName = Ship::Window::GetPathRelativeToAppDirectory(GenerateRandomizer(cvarSettings, excludedLocations, seedInput).c_str());
    CVarSetString("gSpoilerLog", fileName.c_str());

    CVarSave();
    CVarLoad();
    CVarSetInteger("gNewSeedGenerated", 1);
}

std::array<Item, KEY_ENUM_MAX>* RandoMain::GetFullItemTable() {
    ItemTable_Init();

    return GetFullItemTable_();
}

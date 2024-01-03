#include "menu.hpp"
#include "hint_list.hpp"
#include "../static_data.h"
#include "../item_location.h"
#include "location_access.hpp"
#include "rando_main.hpp"
#include "../context.h"
// #include <soh/Enhancements/randomizer.h>
#include <libultraship/bridge.h>
#include <Context.h>
#include <libultraship/libultra/types.h>

void RandoMain::GenerateRando(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks,
    std::string seedString) {
    HintTable_Init();

    // std::string settingsFileName = "./randomizer/latest_settings.json";
    // CVarSetString("gLoadedPreset", settingsFileName.c_str());

    Rando::Context::GetInstance()->SetSeedGenerated(GenerateRandomizer(excludedLocations, enabledTricks, seedString));

    CVarSave();
    CVarLoad();
    Rando::Context::GetInstance()->SetPlandoLoaded(false);
}
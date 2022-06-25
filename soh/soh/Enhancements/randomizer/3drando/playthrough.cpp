#include "playthrough.hpp"

#include "custom_messages.hpp"
#include "fill.hpp"
#include "location_access.hpp"
#include "logic.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "randomizerTypes.h"

namespace Playthrough {

int Playthrough_Init(uint32_t seed, std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings) {
    // initialize the RNG with just the seed incase any settings need to be
    // resolved to something random
    Random_Init(seed);

    overrides.clear();
    CustomMessages::ClearMessages();
    ItemReset();
    HintReset();
    Areas::AccessReset();

    Settings::UpdateSettings(cvarSettings);
    // once the settings have been finalized turn them into a string for hashing
    std::string settingsStr;
    for (Menu* menu : Settings::GetAllOptionMenus()) {
        // don't go through non-menus
        if (menu->mode != OPTION_SUB_MENU) {
            continue;
        }

        for (size_t i = 0; i < menu->settingsList->size(); i++) {
            Option* setting = menu->settingsList->at(i);
            if (setting->IsCategory(OptionCategory::Setting)) {
                settingsStr += setting->GetSelectedOptionText();
            }
        }
    }
    unsigned int finalHash = std::hash<std::string>{}(Settings::seed + settingsStr);
    Random_Init(finalHash);

    Logic::UpdateHelpers();

    if (Settings::Logic.Is(LOGIC_VANILLA)) {
        VanillaFill(); // Just place items in their vanilla locations
    } else {           // Fill locations with logic
        int ret = Fill();
        if (ret < 0) {
            return ret;
        }
    }

    GenerateHash();
    WriteIngameSpoilerLog();

    if (Settings::GenerateSpoilerLog) {
        // write logs
        printf("\x1b[11;10HWriting Spoiler Log...");
        if (SpoilerLog_Write(cvarSettings[RSK_LANGUAGE])) {
            printf("Done");
        } else {
            printf("Failed");
        }
#ifdef ENABLE_DEBUG
        printf("\x1b[11;10HWriting Placement Log...");
        if (PlacementLog_Write()) {
            printf("Done\n");
        } else {
            printf("Failed\n");
        }
#endif
    }

    playthroughLocations.clear();
    wothLocations.clear();
    playthroughBeatable = false;

    return 1;
}

// used for generating a lot of seeds at once
int Playthrough_Repeat(int count /*= 1*/) {
    printf("\x1b[0;0HGENERATING %d SEEDS", count);
    uint32_t repeatedSeed = 0;
    for (int i = 0; i < count; i++) {
        repeatedSeed = rand() % 0xFFFFFFFF;
        Settings::seed = std::to_string(repeatedSeed);
        CitraPrint("testing seed: " + Settings::seed);
        ClearProgress();
        // Playthrough_Init(std::hash<std::string>{}(Settings::seed));
        printf("\x1b[15;15HSeeds Generated: %d\n", i + 1);
    }

    return 1;
}
} // namespace Playthrough
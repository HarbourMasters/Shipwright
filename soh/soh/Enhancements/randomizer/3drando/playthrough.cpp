#include "playthrough.hpp"

#include <libultraship/libultraship.h>
#include <boost_custom/container_hash/hash_32.hpp>
#include "custom_messages.hpp"
#include "fill.hpp"
#include "location_access.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include <variables.h>
#include "../option.h"

namespace Playthrough {

int Playthrough_Init(uint32_t seed, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks) {
    // initialize the RNG with just the seed incase any settings need to be
    // resolved to something random
    Random_Init(seed);

    auto ctx = Rando::Context::GetInstance();
    ctx->overrides.clear();
    CustomMessages::ClearMessages();
    ctx->ItemReset();
    ctx->HintReset();
    ctx->GetLogic()->Reset();
    Areas::AccessReset();

    ctx->GetSettings()->FinalizeSettings(excludedLocations, enabledTricks);
    // once the settings have been finalized turn them into a string for hashing
    std::string settingsStr;
    for (const Rando::OptionGroup& optionGroup : ctx->GetSettings()->GetOptionGroups()) {
        // don't go through non-menus
        if (optionGroup.GetContainsType() == Rando::OptionGroupType::SUBGROUP) {
            continue;
        }

        for (Rando::Option* option : optionGroup.GetOptions()) {
            if (option->IsCategory(Rando::OptionCategory::Setting)) {
                if (option->GetOptionCount() > 0) {
                    settingsStr += option->GetSelectedOptionText();
                }
            }
        }
    }

    if (CVarGetInteger("gRandomizerDontGenerateSpoiler", 0)) {
        settingsStr += (char*)gBuildVersion;
    }

    uint32_t finalHash = boost::hash_32<std::string>{}(std::to_string(ctx->GetSettings()->GetSeed()) + settingsStr);
    Random_Init(finalHash);
    ctx->GetSettings()->SetHash(std::to_string(finalHash));

    ctx->GetLogic()->UpdateHelpers();

    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
        VanillaFill(); // Just place items in their vanilla locations
    } else {           // Fill locations with logic
        int ret = Fill();
        if (ret < 0) {
            return ret;
        }
    }

    GenerateHash();
    WriteIngameSpoilerLog();

    if (true) {
        //TODO: Handle different types of file output (i.e. Spoiler Log, Plando Template, Patch Files, Race Files, etc.)
        // write logs
        printf("\x1b[11;10HWriting Spoiler Log...");
        if (SpoilerLog_Write()) {
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

    ctx->playthroughLocations.clear();
    ctx->playthroughBeatable = false;

    return 1;
}

// used for generating a lot of seeds at once
int Playthrough_Repeat(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks, int count /*= 1*/) {
    printf("\x1b[0;0HGENERATING %d SEEDS", count);
    auto ctx = Rando::Context::GetInstance();
    uint32_t repeatedSeed = 0;
    for (int i = 0; i < count; i++) {
        ctx->GetSettings()->SetSeedString(std::to_string(rand() % 0xFFFFFFFF));
        repeatedSeed = boost::hash_32<std::string>{}(ctx->GetSettings()->GetSeedString());
        ctx->GetSettings()->SetSeed(repeatedSeed % 0xFFFFFFFF);
        //CitraPrint("testing seed: " + std::to_string(Settings::seed));
        ClearProgress();
        Playthrough_Init(ctx->GetSettings()->GetSeed(), excludedLocations, enabledTricks);
        printf("\x1b[15;15HSeeds Generated: %d\n", i + 1);
    }

    return 1;
}
} // namespace Playthrough
#include "playthrough.hpp"

#include <libultraship/libultraship.h>
#include <boost_custom/container_hash/hash_32.hpp>
#include "fill.hpp"
#include "../location_access.h"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "variables.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "../option.h"
#include "soh/Enhancements/debugger/performanceTimer.h"

namespace Playthrough {

int Playthrough_Init(uint32_t seed, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks) {
    // initialize the RNG with just the seed incase any settings need to be
    // resolved to something random
    Random_Init(seed);

    auto ctx = Rando::Context::GetInstance();
    ctx->overrides.clear();
    ctx->ItemReset();
    ctx->HintReset();
    ctx->GetLogic()->Reset();
    StartPerformanceTimer(PT_REGION_RESET);
    Regions::AccessReset();
    StopPerformanceTimer(PT_REGION_RESET);

    ctx->FinalizeSettings(excludedLocations, enabledTricks);
    // once the settings have been finalized turn them into a string for hashing
    std::string settingsStr;
    auto& optionGroups = Rando::Settings::GetInstance()->GetOptionGroups();
    for (size_t i = 0; i < RSG_MAX; i++) {
        auto& optionGroup = optionGroups[i];
        // don't go through non-menus
        if (optionGroup.GetContainsType() == Rando::OptionGroupType::SUBGROUP) {
            continue;
        }

        for (Rando::Option* option : optionGroup.GetOptions()) {
            if (option->IsCategory(Rando::OptionCategory::Setting)) {
                if (option->GetOptionCount() > 0) {
                    if (i >= RSG_EXCLUDES_KOKIRI_FOREST && i <= RSG_EXCLUDES_GANONS_CASTLE) {
                        auto locationOption = static_cast<Rando::LocationOption*>(option);
                        settingsStr += option->GetOptionText(ctx->GetLocationOption(locationOption->GetKey()).Get());
                    } else if (i == RSG_TRICKS) {
                        auto trickOption = static_cast<Rando::TrickOption*>(option);
                        settingsStr += option->GetOptionText(ctx->GetTrickOption(trickOption->GetKey()).Get());
                    } else {
                        settingsStr += option->GetOptionText(ctx->GetOption(option->GetKey()).Get());
                    }
                }
            }
        }
    }

    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("DontGenerateSpoiler"), 0)) {
        settingsStr += (char*)gBuildVersion;
    }

    uint32_t finalHash = boost::hash_32<std::string>{}(std::to_string(ctx->GetSeed()) + settingsStr);
    Random_Init(finalHash);
    ctx->SetHash(std::to_string(finalHash));


    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
        VanillaFill(); // Just place items in their vanilla locations
    } else {           // Fill locations with logic
        int ret = Fill();
        if (ret < 0) {
            return ret;
        }
    }

    GenerateHash();

    if (true) {
        //TODO: Handle different types of file output (i.e. Spoiler Log, Plando Template, Patch Files, Race Files, etc.)
        // write logs
        SPDLOG_INFO("Writing Spoiler Log...");
        StartPerformanceTimer(PT_SPOILER_LOG);
        if (SpoilerLog_Write()) {
            SPDLOG_INFO("Writing Spoiler Log Done");
        } else {
            SPDLOG_ERROR("Writing Spoiler Log Failed");
        }
        StopPerformanceTimer(PT_SPOILER_LOG);
    }

    ctx->playthroughLocations.clear();
    ctx->playthroughBeatable = false;

    return 1;
}

// used for generating a lot of seeds at once
int Playthrough_Repeat(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks, int count /*= 1*/) {
    SPDLOG_INFO("GENERATING {} SEEDS", count);
    auto ctx = Rando::Context::GetInstance();
    uint32_t repeatedSeed = 0;
    for (int i = 0; i < count; i++) {
        ctx->SetSeedString(std::to_string(rand() % 0xFFFFFFFF));
        repeatedSeed = boost::hash_32<std::string>{}(ctx->GetSeedString());
        ctx->SetSeed(repeatedSeed % 0xFFFFFFFF);
        SPDLOG_DEBUG("testing seed: %d", repeatedSeed);
        ClearProgress();
        Playthrough_Init(ctx->GetSeed(), excludedLocations, enabledTricks);
        SPDLOG_INFO("Seeds Generated: {}", i + 1);
    }

    return 1;
}
} // namespace Playthrough
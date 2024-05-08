#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <ctime>

#include "menu.hpp"
#include "playthrough.hpp"
#include "randomizer.hpp"
#include "spoiler_log.hpp"
#include "location_access.hpp"
#include <spdlog/spdlog.h>
#include "../../randomizer/randomizerTypes.h"
#include <boost_custom/container_hash/hash_32.hpp>

namespace {
bool seedChanged;
uint16_t pastSeedLength;
std::vector<std::string> presetEntries;
Rando::Option* currentSetting;
} // namespace

bool GenerateRandomizer(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks,
    std::string seedInput) {
    const auto ctx = Rando::Context::GetInstance();

    srand(time(NULL));
    // if a blank seed was entered, make a random one
    if (seedInput.empty()) {
        seedInput = std::to_string(rand() % 0xFFFFFFFF);
    } else if (seedInput.rfind("seed_testing_count", 0) == 0 && seedInput.length() > 18) {
        int count;
        try {
            count = std::stoi(seedInput.substr(18), nullptr);
        } catch (std::invalid_argument &e) {
            count = 1;
        } catch (std::out_of_range &e) {
            count = 1;
        }
        Playthrough::Playthrough_Repeat(excludedLocations, enabledTricks, count);
        return false; // TODO: Not sure if this is correct but I don't think we support this functionality yet anyway.
    }

    ctx->GetSettings()->SetSeedString(seedInput);
    uint32_t seedHash = boost::hash_32<std::string>{}(ctx->GetSettings()->GetSeedString());
    ctx->GetSettings()->SetSeed(seedHash & 0xFFFFFFFF);

    ctx->ClearItemLocations();
    int ret = Playthrough::Playthrough_Init(ctx->GetSettings()->GetSeed(), excludedLocations, enabledTricks);
    if (ret < 0) {
        if (ret == -1) { // Failed to generate after 5 tries
            SPDLOG_ERROR("Failed to generate after 5 tries.");
            return false;
        } else {
            SPDLOG_ERROR("Error {} with fill.", ret);
            return false;
        }
    }

    // Restore settings that were set to a specific value for vanilla logic
    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
        for (Rando::Option* setting : ctx->GetSettings()->VanillaLogicDefaults) {
            setting->RestoreDelayedOption();
        }
        ctx->GetOption(RSK_KEYSANITY).RestoreDelayedOption();
    }
    return true;
}
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

std::string GenerateRandomizer(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks,
    std::string seedString) {
    auto ctx = Rando::Context::GetInstance();

    srand(time(NULL));
    // if a blank seed was entered, make a random one
    if (seedString.empty()) {
        seedString = std::to_string(rand() % 0xFFFFFFFF);
    } else if (seedString.rfind("seed_testing_count", 0) == 0 && seedString.length() > 18) {
        int count;
        try {
            count = std::stoi(seedString.substr(18), nullptr);
        } catch (std::invalid_argument &e) {
            count = 1;
        } catch (std::out_of_range &e) {
            count = 1;
        }
        Playthrough::Playthrough_Repeat(cvarSettings, excludedLocations, enabledTricks, count);
        return "";
    }

    ctx->GetSettings()->SetSeedString(seedString);
    uint32_t seedHash = boost::hash_32<std::string>{}(ctx->GetSettings()->GetSeedString());
    ctx->GetSettings()->SetSeed(seedHash & 0xFFFFFFFF);

    int ret = Playthrough::Playthrough_Init(ctx->GetSettings()->GetSeed(), cvarSettings, excludedLocations, enabledTricks);
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
    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
        for (Rando::Option* setting : ctx->GetSettings()->VanillaLogicDefaults) {
            setting->RestoreDelayedOption();
        }
        ctx->GetOption(RSK_KEYSANITY).RestoreDelayedOption();
    }
    std::ostringstream fileNameStream;
    for (int i = 0; i < ctx->hashIconIndexes.size(); i++) {
        if (i) {
            fileNameStream << '-';
        }
        if (ctx->hashIconIndexes[i] < 10) {
            fileNameStream << '0';
        }
        fileNameStream << std::to_string(ctx->hashIconIndexes[i]);
    }
    std::string fileName = fileNameStream.str();
    return "./Randomizer/" + fileName + ".json";
}
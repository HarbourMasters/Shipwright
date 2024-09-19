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
    ctx->StartPerformanceTimer(PT_WHOLE_SEED);

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

    ctx->StopPerformanceTimer(PT_WHOLE_SEED);
    SPDLOG_DEBUG("Full Seed Genration Time: {}ms", ctx->GetPerformanceTimer(PT_WHOLE_SEED).count());
    SPDLOG_DEBUG("LogicReset time: {}ms", ctx->GetPerformanceTimer(PT_LOGIC_RESET).count());
    SPDLOG_DEBUG("Area->Reset time: {}ms", ctx->GetPerformanceTimer(PT_AREA_RESET).count());
    SPDLOG_DEBUG("Total Entrance Shuffle time: {}ms", ctx->GetPerformanceTimer(PT_ENTRANCE_SHUFFLE).count());
    SPDLOG_DEBUG("Total Shopsanity time: {}ms", ctx->GetPerformanceTimer(PT_SHOPSANITY).count());
    SPDLOG_DEBUG("Total Dungeon Specific Items time: {}ms", ctx->GetPerformanceTimer(PT_OWN_DUNGEON).count());
    SPDLOG_DEBUG("Total Misc Limited Checks time: {}ms", ctx->GetPerformanceTimer(PT_LIMITED_CHECKS).count());
    SPDLOG_DEBUG("Total Advancment Checks time: {}ms", ctx->GetPerformanceTimer(PT_ADVANCEMENT_ITEMS).count());
    SPDLOG_DEBUG("Total Other Checks time: {}ms", ctx->GetPerformanceTimer(PT_REMAINING_ITEMS).count());
    SPDLOG_DEBUG("Total Playthrough Generation time: {}ms", ctx->GetPerformanceTimer(PT_PLAYTHROUGH_GENERATION).count());
    SPDLOG_DEBUG("Total PareDownPlaythrough time: {}ms", ctx->GetPerformanceTimer(PT_PARE_DOWN_PLAYTHROUGH).count());
    SPDLOG_DEBUG("Total WotH generation time: {}ms", ctx->GetPerformanceTimer(PT_WOTH).count());
    SPDLOG_DEBUG("Total Foolish generation time: {}ms", ctx->GetPerformanceTimer(PT_FOOLISH).count());
    SPDLOG_DEBUG("Total Overrides time: {}ms", ctx->GetPerformanceTimer(PT_OVERRIDES).count());
    SPDLOG_DEBUG("Total Hint Generation time: {}ms", ctx->GetPerformanceTimer(PT_HINTS).count());
    SPDLOG_DEBUG("SpoilerLog writing time: {}ms", ctx->GetPerformanceTimer(PT_SPOILER_LOG).count());
    SPDLOG_DEBUG("Total UpdateHelpers time: {}ms", ctx->GetPerformanceTimer(PT_UPDATE_HELPERS).count());
    SPDLOG_DEBUG("Total Event Access Calculation time: {}ms", ctx->GetPerformanceTimer(PT_EVENT_ACCESS).count());
    SPDLOG_DEBUG("Total ToD Access Calculation: {}ms", ctx->GetPerformanceTimer(PT_TOD_ACCESS).count());
    SPDLOG_DEBUG("Total Entrance Logic Calculation time: {}ms", ctx->GetPerformanceTimer(PT_ENTRANCE_LOGIC).count());
    SPDLOG_DEBUG("Total Check Logic Calculation time: {}ms", ctx->GetPerformanceTimer(PT_LOCATION_LOGIC).count());
    return true;
}
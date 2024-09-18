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
    ctx->metricStart = std::chrono::high_resolution_clock::now();
    ctx->updateHelpersDuration = std::chrono::milliseconds(0);
    ctx->entranceShuffleDuration = std::chrono::milliseconds(0);
    ctx->shopDuration = std::chrono::milliseconds(0);
    ctx->dungeonsDuration = std::chrono::milliseconds(0);
    ctx->limitedDuration = std::chrono::milliseconds(0);
    ctx->advancmentDuration = std::chrono::milliseconds(0);
    ctx->remainingDuration = std::chrono::milliseconds(0);
    ctx->playthroughDuration = std::chrono::milliseconds(0);
    ctx->pareDownDuration = std::chrono::milliseconds(0);
    ctx->WotHDuration = std::chrono::milliseconds(0);
    ctx->FoolishDuration = std::chrono::milliseconds(0);
    ctx->OverridesDuration = std::chrono::milliseconds(0);
    ctx->HintsDuration = std::chrono::milliseconds(0);

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

    std::chrono::duration<double, std::milli> duration = std::chrono::high_resolution_clock::now() - ctx->metricStart;
    SPDLOG_DEBUG("Full Seed Genration Time: {}ms", duration.count());
    duration = ctx->SCLResetEnd - ctx->SCLResetStart;
    SPDLOG_DEBUG("SCL reset time: {}ms", duration.count());
    duration = ctx->LogicResetEnd - ctx->LogicResetStart;
    SPDLOG_DEBUG("LogicReset time: {}ms", duration.count());
    duration = ctx->areaResetEnd - ctx->areaResetStart;
    SPDLOG_DEBUG("Area->Reset time: {}ms", duration.count());
    duration = ctx->logEnd - ctx->logStart;
    SPDLOG_DEBUG("SpoilerLog writing time: {}ms", duration.count());
    SPDLOG_DEBUG("Total UpdateHelpers time: {}ms", ctx->updateHelpersDuration.count());
    SPDLOG_DEBUG("Total Entrance Shuffle time: {}ms", ctx->entranceShuffleDuration.count());
    SPDLOG_DEBUG("Total SetAreas time: {}ms", ctx->setAreasDuration.count());
    SPDLOG_DEBUG("Total Shopsanity time: {}ms", ctx->shopDuration.count());
    SPDLOG_DEBUG("Total Dungeon Specific Items time: {}ms", ctx->dungeonsDuration.count());
    SPDLOG_DEBUG("Total Misc Limited Checks time: {}ms", ctx->limitedDuration.count());
    SPDLOG_DEBUG("Total Advancment Checks time: {}ms", ctx->advancmentDuration.count());
    SPDLOG_DEBUG("Total Other Checks time: {}ms", ctx->remainingDuration.count());
    SPDLOG_DEBUG("Total Playthrough Generation time: {}ms", ctx->playthroughDuration.count());
    SPDLOG_DEBUG("Total ParDownPlaythrough time: {}ms", ctx->pareDownDuration.count());
    SPDLOG_DEBUG("Total WotH generation time: {}ms", ctx->WotHDuration.count());
    SPDLOG_DEBUG("Total Foolish generation time: {}ms", ctx->FoolishDuration.count());
    SPDLOG_DEBUG("Total Overrides time: {}ms", ctx->OverridesDuration.count());
    SPDLOG_DEBUG("Total Hint Generation time: {}ms", ctx->HintsDuration.count());

    return true;
}
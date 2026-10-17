#include "tracker_state_export.h"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <ship/Context.h>

#include "item_location.h"
#include "location_access.h"
#include "randomizer_check_objects.h"
#include "randomizer_check_tracker.h"
#include "static_data.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
}

namespace CheckTracker::StateExport {
namespace {

using json = nlohmann::json;

std::atomic_bool writeRequested = false;

std::filesystem::path GetExportPath(int fileNum) {
    const std::filesystem::path savePath = Ship::Context::GetPathRelativeToAppDirectory("Save");
    return savePath / ("file" + std::to_string(fileNum + 1) + ".tracker.json");
}

void RemoveFile(int fileNum) {
    std::error_code error;
    const auto exportPath = GetExportPath(fileNum);
    std::filesystem::remove(exportPath, error);
    auto temporaryPath = exportPath;
    temporaryPath += ".tmp";
    std::filesystem::remove(temporaryPath, error);
}

template <typename T, size_t N> json NumberArray(const T (&values)[N]) {
    json result = json::array();
    for (const auto value : values) {
        result.push_back(static_cast<int64_t>(value));
    }
    return result;
}

const char* GetStatusName(RandomizerCheckStatus status) {
    switch (status) {
        case RCSHOW_UNCHECKED:
            return "unchecked";
        case RCSHOW_SEEN_OR_HINTED:
            return "seen";
        case RCSHOW_IDENTIFIED:
            return "identified";
        case RCSHOW_SCUMMED:
            return "scummed";
        case RCSHOW_COLLECTED:
            return "collected";
        case RCSHOW_SAVED:
            return "saved";
        default:
            return "unknown";
    }
}

std::string GetLogicCondition(Rando::ItemLocation* itemLocation) {
    const auto parentRegion = itemLocation->GetParentRegionKey();
    if (parentRegion < RR_ROOT || parentRegion >= RR_MAX) {
        return "";
    }

    for (const auto& locationAccess : areaTable[parentRegion].locations) {
        if (locationAccess.GetLocation() == itemLocation->GetRandomizerCheck()) {
            return locationAccess.GetConditionStr();
        }
    }
    return "";
}

json BuildInventory() {
    const auto& inventory = gSaveContext.inventory;
    const auto& randomizer = gSaveContext.ship.quest.data.randomizer;

    return {
        { "items", NumberArray(inventory.items) },
        { "ammo", NumberArray(inventory.ammo) },
        { "equipment", inventory.equipment },
        { "upgrades", inventory.upgrades },
        { "questItems", inventory.questItems },
        { "dungeonItems", NumberArray(inventory.dungeonItems) },
        { "dungeonKeys", NumberArray(inventory.dungeonKeys) },
        { "defenseHearts", inventory.defenseHearts },
        { "goldSkulltulaTokens", inventory.gsTokens },
        { "randomizerInf", NumberArray(gSaveContext.ship.randomizerInf) },
        { "randomizer",
          {
              { "triforcePiecesCollected", randomizer.triforcePiecesCollected },
              { "bombchuUpgradeLevel", randomizer.bombchuUpgradeLevel },
              { "silverRupees",
                {
                    { "shadowTempleInvisibleBlades", randomizer.silverShadowBlades },
                    { "shadowTempleHugePit", randomizer.silverShadowPit },
                    { "shadowTempleSpikes", randomizer.silverShadowSpikes },
                    { "spiritTempleChild", randomizer.silverSpiritChild },
                    { "spiritTempleSunBlock", randomizer.silverSpiritSun },
                    { "spiritTempleBoulders", randomizer.silverSpiritBoulders },
                    { "bottomOfTheWell", randomizer.silverBotw },
                    { "iceCavernSpinningScythe", randomizer.silverIceCavernBlades },
                    { "iceCavernPushBlock", randomizer.silverIceCavernBlock },
                    { "gerudoTrainingGroundSlopes", randomizer.silverGtgSlope },
                    { "gerudoTrainingGroundLava", randomizer.silverGtgLava },
                    { "gerudoTrainingGroundWater", randomizer.silverGtgWater },
                    { "ganonsCastleLightTrial", randomizer.silverGanonLight },
                    { "ganonsCastleForestTrial", randomizer.silverGanonForest },
                    { "ganonsCastleFireTrial", randomizer.silverGanonFire },
                    { "ganonsCastleSpiritTrial", randomizer.silverGanonSpirit },
                    { "dodongosCavernMq", randomizer.silverMqDodongosCavern },
                    { "shadowTempleMqInvisibleBlades", randomizer.silverMqShadowInvisibleBlades },
                    { "spiritTempleMqLobby", randomizer.silverMqSpiritLobby },
                    { "spiritTempleMqAdult", randomizer.silverMqSpiritBigWall },
                    { "ganonsCastleMqWaterTrial", randomizer.silverMqGanonWater },
                    { "ganonsCastleMqShadowTrial", randomizer.silverMqGanonShadow },
                } },
          } },
    };
}

json BuildChecks() {
    json checks = json::array();
    const auto& context = Rando::Context::GetInstance();

    for (const auto& location : Rando::StaticData::GetLocationTable()) {
        const auto check = location.GetRandomizerCheck();
        if (check == RC_UNKNOWN_CHECK || check == RC_MAX || !CheckTracker::IsVisibleInCheckTracker(check) ||
            !CheckTracker::IsAreaSpoiled(location.GetArea())) {
            continue;
        }

        auto* itemLocation = context->GetItemLocation(check);
        const auto status = itemLocation->GetCheckStatus();
        checks.push_back({
            { "id", check },
            { "name", location.GetShortName() },
            { "area",
              {
                  { "id", location.GetArea() },
                  { "name", RandomizerCheckObjects::GetRCAreaName(location.GetArea()) },
              } },
            { "type", location.GetRCType() },
            { "status", GetStatusName(status) },
            { "statusId", status },
            { "skipped", itemLocation->GetIsSkipped() },
            { "available", itemLocation->IsAvailable() },
            { "logic", GetLogicCondition(itemLocation) },
        });
    }
    return checks;
}

json BuildState() {
    const auto& context = Rando::Context::GetInstance();
    json hashIcons = json::array();
    for (const auto icon : context->hashIconIndexes) {
        hashIcons.push_back(icon);
    }

    return {
        { "schemaVersion", 1 },
        { "build",
          {
              { "version", std::string(gBuildVersion) },
              { "branch", std::string(gGitBranch) },
              { "commit", std::string(gGitCommitHash) },
              { "saveVersion",
                {
                    { "major", gSaveContext.ship.stats.buildVersionMajor },
                    { "minor", gSaveContext.ship.stats.buildVersionMinor },
                    { "patch", gSaveContext.ship.stats.buildVersionPatch },
                } },
          } },
        { "save",
          {
              { "slot", gSaveContext.fileNum + 1 },
              { "seed",
                {
                    { "input", context->GetSeedString() },
                    { "final", context->GetSeed() },
                    { "hash", context->GetHash() },
                    { "icons", std::move(hashIcons) },
                } },
          } },
        { "player",
          {
              { "age", gSaveContext.linkAge == LINK_AGE_CHILD ? "child" : "adult" },
              { "dayTime", gSaveContext.dayTime },
              { "isNight", gSaveContext.nightFlag != 0 },
              { "entrance", gSaveContext.entranceIndex },
              { "scene", gPlayState != nullptr ? gPlayState->sceneNum : gSaveContext.savedSceneNum },
              { "health", gSaveContext.health },
              { "healthCapacity", gSaveContext.healthCapacity },
              { "magic", gSaveContext.magic },
              { "magicLevel", gSaveContext.magicLevel },
              { "rupees", gSaveContext.rupees },
          } },
        { "inventory", BuildInventory() },
        { "checks", BuildChecks() },
    };
}

bool ReplaceFile(const std::filesystem::path& source, const std::filesystem::path& destination) {
#if defined(_WIN32)
    if (MoveFileExW(source.wstring().c_str(), destination.wstring().c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        return true;
    }
    SPDLOG_ERROR("Failed to replace tracker state export \"{}\": Windows error {}", destination.string(),
                 GetLastError());
    return false;
#else
    std::error_code error;
    std::filesystem::rename(source, destination, error);
    if (!error) {
        return true;
    }
    SPDLOG_ERROR("Failed to replace tracker state export \"{}\": {}", destination.string(), error.message());
    return false;
#endif
}

void Write() {
    const auto exportPath = GetExportPath(gSaveContext.fileNum);
    auto temporaryPath = exportPath;
    temporaryPath += ".tmp";

    std::error_code error;
    std::filesystem::create_directories(exportPath.parent_path(), error);
    if (error) {
        SPDLOG_ERROR("Failed to create tracker state export directory \"{}\": {}", exportPath.parent_path().string(),
                     error.message());
        return;
    }

    {
        std::ofstream output(temporaryPath, std::ios::binary | std::ios::trunc);
        if (!output.is_open()) {
            SPDLOG_ERROR("Failed to open tracker state export \"{}\" for writing", temporaryPath.string());
            return;
        }
        output << std::setw(1) << BuildState() << '\n';
        if (!output.good()) {
            SPDLOG_ERROR("Failed while writing tracker state export \"{}\"", temporaryPath.string());
            output.close();
            std::filesystem::remove(temporaryPath, error);
            return;
        }
    }

    if (!ReplaceFile(temporaryPath, exportPath)) {
        std::filesystem::remove(temporaryPath, error);
    }
}

} // namespace

void Init() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRandoSetCheckStatus>(
        [](RandomizerCheck rc, RandomizerCheckStatus status) { RequestWrite(); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRandoSetIsSkipped>(
        [](RandomizerCheck rc, bool isSkipped) { RequestWrite(); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveFile>(
        [](int32_t fileNum, int32_t sectionId) { RequestWrite(); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        if (!IS_RANDO) {
            RemoveFile(fileNum);
        }
    });

    if (!IsEnabled()) {
        RemoveFiles();
    }
}

bool IsEnabled() {
    return CVarGetInteger(CVAR_TRACKER_CHECK("ExternalStateExport"), 0) != 0;
}

void RequestWrite() {
    writeRequested = true;
}

void WriteIfRequested() {
    if (!writeRequested.exchange(false) || !IsEnabled() || !GameInteractor::IsSaveLoaded() || !IS_RANDO) {
        return;
    }
    Write();
}

void RemoveFiles() {
    writeRequested = false;
    for (int fileNum = 0; fileNum < SaveManager::MaxFiles; fileNum++) {
        RemoveFile(fileNum);
    }
}

} // namespace CheckTracker::StateExport

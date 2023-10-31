#pragma once

#include "randomizerTypes.h"
#include "item_location.h"
#include "item_override.h"
#include "3drando/text.hpp"
#include "hint.h"

#include <memory>
#include <array>
#include <map>
#include <nlohmann/json.hpp>

/**
 * @brief Singleton for storing and accessing dynamic Randomizer-related data
 *
 * The Context class is a class for storing and accessing dynamic Randomizer-related data.
 * Dynamic Data, in this context, refers to data that can be different for each randomizer seed,
 * i.e. item locations, hint contents and locations, settings, etc. This class is intended to be
 * used as a Singleton.
 */
namespace Rando {
class EntranceShuffler;
class Settings;
class Dungeons;
class DungeonInfo;
class TrialInfo;
class Trials;

class Context {
  public:
    Context();
    static std::shared_ptr<Context> CreateInstance();
    static std::shared_ptr<Context> GetInstance();
    Hint* GetHint(RandomizerHintKey hintKey);
    void AddHint(RandomizerHintKey hintId, Text text, RandomizerCheck hintedLocation, HintType hintType,
                 Text hintedRegion);
    ItemLocation* GetItemLocation(RandomizerCheck locKey);
    ItemLocation* GetItemLocation(size_t locKey);
    void PlaceItemInLocation(RandomizerCheck locKey, RandomizerGet item, bool applyEffectImmediately = false,
                             bool setHidden = false);
    std::vector<RandomizerCheck> allLocations;
    void AddLocation(RandomizerCheck loc, std::vector<RandomizerCheck>* destination = nullptr);
    template <typename Container>
    void AddLocations(const Container& locations, std::vector<RandomizerCheck>* destination = nullptr);
    void GenerateLocationPool();
    std::vector<RandomizerCheck> GetLocations(const std::vector<RandomizerCheck>& locationPool,
                                              Category categoryInclude, Category categoryExclude = Category::cNull);
    void AddExcludedOptions();
    void LocationReset();
    void ItemReset();
    void HintReset();
    void CreateItemOverrides();
    bool IsSeedGenerated();
    void SetSeedGenerated(bool seedGenerated = true);
    bool IsSpoilerLoaded();
    void SetSpoilerLoaded(bool spoilerLoaded = true);
    bool IsPlandoLoaded();
    void SetPlandoLoaded(bool plandoLoaded = true);
    std::shared_ptr<Settings> GetSettings();
    const std::shared_ptr<EntranceShuffler> GetEntranceShuffler();
    std::shared_ptr<Dungeons> GetDungeons();
    DungeonInfo* GetDungeon(size_t key);
    std::shared_ptr<Trials> GetTrials();
    TrialInfo* GetTrial(size_t key);
    Option& GetOption(RandomizerSettingKey key);
    Option& GetTrickOption(RandomizerTrick key);
    GetItemEntry GetFinalGIEntry(RandomizerCheck rc, bool checkObtainability = true, GetItemID ogItemId = GI_NONE);
    void ParseSpoiler(const char* spoilerFileName, bool plandoMode);
    void ParseHashIconIndexesJson(nlohmann::json spoilerFileJson);
    void ParseItemLocationsJson(nlohmann::json spoilerFileJson);
    void ParseHintJson(nlohmann::json spoilerFileJson);
    std::map<RandomizerCheck, ItemOverride> overrides = {};
    std::vector<std::vector<RandomizerCheck>> playthroughLocations = {};
    std::vector<RandomizerCheck> everyPossibleLocation = {};
    std::vector<RandomizerCheck> wothLocations = {};
    std::vector<RandomizerGet> possibleIceTrapModels = {};
    std::unordered_map<RandomizerCheck, RandomizerGet> iceTrapModels = {};
    std::array<uint8_t, 5> hashIconIndexes = {};
    std::unordered_map<std::string, RandomizerCheck> mSpoilerfileCheckNameToEnum;
    bool playthroughBeatable = false;
    bool allLocationsReachable = false;

  private:
    static std::weak_ptr<Context> mContext;
    std::unordered_map<std::string, RandomizerGet> mSpoilerfileGetNameToEnum;
    std::unordered_map<std::string, HintType> mSpoilerfileHintTypeNameToEnum;
    std::array<Hint, RH_MAX> hintTable = {};
    RandomizerCheck mEmeraldLoc;
    RandomizerCheck mRubyLoc;
    RandomizerCheck mSapphireLoc;
    RandomizerCheck mForestMedallionLoc;
    RandomizerCheck mFireMedallionLoc;
    RandomizerCheck mWaterMedallionLoc;
    RandomizerCheck mShadowMedallionLoc;
    RandomizerCheck mSpiritMedallionLoc;
    RandomizerCheck mLightMedallionLoc;
    std::array<ItemLocation, RC_MAX> itemLocationTable = {};
    std::shared_ptr<Settings> mSettings;
    std::shared_ptr<EntranceShuffler> mEntranceShuffler;
    std::shared_ptr<Dungeons> mDungeons;
    std::shared_ptr<Trials> mTrials;
    bool mSeedGenerated = false;
    bool mSpoilerLoaded = false;
    bool mPlandoLoaded = false;
};
} // namespace Rando
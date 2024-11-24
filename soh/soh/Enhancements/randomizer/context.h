#pragma once

#include "randomizerTypes.h"
#include "z64save.h"
#include "item_location.h"
#include "item_override.h"
#include "3drando/text.hpp"
#include "hint.h"
#include "fishsanity.h"
#include "trial.h"

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
class Logic;
class Settings;
class Dungeons;
class DungeonInfo;
class TrialInfo;
class Trials;
class Kaleido;

class Context {
  public:
    Context();
    static std::shared_ptr<Context> CreateInstance();
    static std::shared_ptr<Context> GetInstance();
    void InitStaticData();
    Hint* GetHint(RandomizerHint hintKey);
    void AddHint(const RandomizerHint hintId, const Hint hint);
    ItemLocation* GetItemLocation(RandomizerCheck locKey);
    ItemLocation* GetItemLocation(size_t locKey);
    ItemOverride& GetItemOverride(RandomizerCheck locKey);
    ItemOverride& GetItemOverride(size_t locKey);
    void PlaceItemInLocation(RandomizerCheck locKey, RandomizerGet item, bool applyEffectImmediately = false,
                             bool setHidden = false);
    std::vector<RandomizerCheck> allLocations;
    void AddLocation(RandomizerCheck loc, std::vector<RandomizerCheck>* destination = nullptr);
    template <typename Container>
    void AddLocations(const Container& locations, std::vector<RandomizerCheck>* destination = nullptr);
    void GenerateLocationPool();
    static std::vector<RandomizerCheck> GetLocations(const std::vector<RandomizerCheck>& locationPool, const RandomizerCheckType checkType);
    void AddExcludedOptions();
    void LocationReset();
    void ClearItemLocations();
    void ItemReset();
    void HintReset();
    void CreateItemOverrides();
    bool IsSeedGenerated() const;
    void SetSeedGenerated(bool seedGenerated = true);
    bool IsSpoilerLoaded() const;
    void SetSpoilerLoaded(bool spoilerLoaded = true);
    bool IsPlandoLoaded() const;
    void SetPlandoLoaded(bool plandoLoaded = true);
    std::shared_ptr<Settings> GetSettings();
    std::shared_ptr<EntranceShuffler> GetEntranceShuffler();
    std::shared_ptr<Dungeons> GetDungeons();
    std::shared_ptr<Fishsanity> GetFishsanity();
    DungeonInfo* GetDungeon(size_t key) const;
    std::shared_ptr<Logic> GetLogic();
    std::shared_ptr<Trials> GetTrials();
    std::shared_ptr<Kaleido> GetKaleido();
    TrialInfo* GetTrial(size_t key) const;
    TrialInfo* GetTrial(TrialKey key) const;
    static Sprite* GetSeedTexture(uint8_t index);
    Option& GetOption(RandomizerSettingKey key) const;
    TrickOption& GetTrickOption(RandomizerTrick key) const;
    GetItemEntry GetFinalGIEntry(RandomizerCheck rc, bool checkObtainability = true, GetItemID ogItemId = GI_NONE);
    void ParseSpoiler(const char* spoilerFileName, bool plandoMode);
    void ParseHashIconIndexesJson(nlohmann::json spoilerFileJson);
    void ParseItemLocationsJson(nlohmann::json spoilerFileJson);
    void WriteHintJson(nlohmann::ordered_json& spoilerFileJson);
    void ParseHintJson(nlohmann::json spoilerFileJson);
    std::map<RandomizerCheck, ItemOverride> overrides = {};
    std::vector<std::vector<RandomizerCheck>> playthroughLocations = {};
    std::vector<RandomizerCheck> everyPossibleLocation = {};
    std::vector<RandomizerGet> possibleIceTrapModels = {};
    std::unordered_map<RandomizerCheck, RandomizerGet> iceTrapModels = {};
    std::array<uint8_t, 5> hashIconIndexes = {};
    bool playthroughBeatable = false;
    bool allLocationsReachable = false;
    RandomizerArea GetAreaFromString(std::string str);

  private:
    static std::weak_ptr<Context> mContext;
    std::array<Hint, RH_MAX> hintTable = {};
    std::array<ItemLocation, RC_MAX> itemLocationTable = {};
    std::shared_ptr<Settings> mSettings;
    std::shared_ptr<EntranceShuffler> mEntranceShuffler;
    std::shared_ptr<Dungeons> mDungeons;
    std::shared_ptr<Logic> mLogic;
    std::shared_ptr<Trials> mTrials;
    std::shared_ptr<Fishsanity> mFishsanity;
    std::shared_ptr<Kaleido> mKaleido;
    bool mSeedGenerated = false;
    bool mSpoilerLoaded = false;
    bool mPlandoLoaded = false;
};
} // namespace Rando
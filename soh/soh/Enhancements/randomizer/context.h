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
    bool IsLocationShuffled(RandomizerCheck locKey);
    ItemOverride& GetItemOverride(RandomizerCheck locKey);
    ItemOverride& GetItemOverride(size_t locKey);
    void PlaceItemInLocation(RandomizerCheck locKey, RandomizerGet item, bool applyEffectImmediately = false,
                             bool setHidden = false);
    std::vector<RandomizerCheck> allLocations;
    std::vector<RandomizerCheck> overworldLocations;
    void AddLocation(RandomizerCheck loc, std::vector<RandomizerCheck>* destination = nullptr);
    template <typename Container>
    void AddLocations(const Container& locations, std::vector<RandomizerCheck>* destination = nullptr);
    bool IsQuestOfLocationActive(RandomizerCheck rc);
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

    /**
     * @brief Reset all RandomizerTrick keys.
     */
    void ResetTrickOptions();

    /**
     * @brief Runs before seed generation to ensure all options are compatible with each
     * other and resolve options that have been set to random (such as random trial count,
     * or starting age).
     *
     * @param excludedLocations Set of locations that should be forced to have junk items.
     * @param enabledTricks Set of tricks that should be considered logically possible. Tricks
     * are things that are possible to do in gameplay but are difficult, not intuitive or that
     * require more extensive game knowledge, i.e. opening invisible chests without the Lens of Truth.
     */
    void FinalizeSettings(const std::set<RandomizerCheck>& excludedLocations,
                          const std::set<RandomizerTrick>& enabledTricks);
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
    OptionValue& GetOption(RandomizerSettingKey key);
    OptionValue& GetTrickOption(RandomizerTrick key);
    OptionValue& GetLocationOption(RandomizerCheck key);

    /**
     * @brief Gets the resolved Light Arrow CutScene check condition.
     * There is no direct option for this, it is inferred based on the value of a few other options.
     *
     * @return RandoOptionLACSCondition
     */
    RandoOptionLACSCondition LACSCondition() const;
    GetItemEntry GetFinalGIEntry(RandomizerCheck rc, bool checkObtainability = true, GetItemID ogItemId = GI_NONE);
    void ParseSpoiler(const char* spoilerFileName);
    void ParseHashIconIndexesJson(nlohmann::json spoilerFileJson);
    void ParseItemLocationsJson(nlohmann::json spoilerFileJson);
    void WriteHintJson(nlohmann::ordered_json& spoilerFileJson);
    void ParseHintJson(nlohmann::json spoilerFileJson);
    std::map<RandomizerCheck, ItemOverride> overrides = {};
    std::vector<std::vector<RandomizerCheck>> playthroughLocations = {};
    std::vector<RandomizerCheck> everyPossibleLocation = {};
    std::vector<RandomizerGet> possibleIceTrapModels = {};
    std::unordered_map<RandomizerCheck, RandomizerGet> iceTrapModels = {};
    std::vector<OptionValue*> VanillaLogicDefaults = {};
    std::array<uint8_t, 5> hashIconIndexes = {};
    bool playthroughBeatable = false;
    bool allLocationsReachable = false;
    RandomizerArea GetAreaFromString(std::string str);

    /**
     * @brief Get the hash for the current seed.
     *
     * @return std::string
     */
    std::string GetHash() const;

    /**
     * @brief Get the Seed String
     *
     * @return const std::string&
     */
    const std::string& GetSeedString() const;

    /**
     * @brief Set the Seed String
     *
     * @param seedString
     */
    void SetSeedString(std::string seedString);

    /**
     * @brief Get the Seed
     *
     * @return const uint32_t
     */
    uint32_t GetSeed() const;

    /**
     * @brief Set the Seed
     *
     * @param seed
     */
    void SetSeed(uint32_t seed);

    /**
     * @brief Set the Seed Hash for the current seed.
     *
     * @param hash
     */
    void SetHash(std::string hash);

  private:
    static std::weak_ptr<Context> mContext;
    std::array<Hint, RH_MAX> hintTable = {};
    std::array<ItemLocation, RC_MAX> itemLocationTable = {};
    std::array<OptionValue, RSK_MAX> mOptions;
    std::array<OptionValue, RT_MAX> mTrickOptions;
    RandoOptionLACSCondition mLACSCondition = RO_LACS_VANILLA;
    std::shared_ptr<EntranceShuffler> mEntranceShuffler;
    std::shared_ptr<Dungeons> mDungeons;
    std::shared_ptr<Logic> mLogic;
    std::shared_ptr<Trials> mTrials;
    std::shared_ptr<Fishsanity> mFishsanity;
    std::shared_ptr<Kaleido> mKaleido;
    bool mSeedGenerated = false;
    bool mSpoilerLoaded = false;
    std::string mHash;
    std::string mSeedString;
    uint32_t mFinalSeed = 0;
};
} // namespace Rando
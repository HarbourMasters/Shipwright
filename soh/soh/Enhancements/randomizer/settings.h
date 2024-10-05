#pragma once

#include "option.h"
#include "randomizerTypes.h"
#include "3drando/spoiler_log.hpp"

#include <array>
#include <set>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace Rando {
class Settings {
  public:
    Settings();

    /**
     * @brief Hides or Unhides the price UI of Shopsanity based on settings.
     */
    void HandleShopsanityPriceUI();

    /**
     * @brief Creates the `Option` and `OptionGroup` objects. This happens after construction because certain
     * other events in the codebase need to happen before all of the `Option`s can be created.
     */
    void CreateOptions();

    /**
     * @brief Get a reference to the `Option` corresponding to the provided RandomizerSettingKey.
     *
     * @param key
     * @return Option&
     */
    Option& GetOption(RandomizerSettingKey key);

    /**
     * @brief Get a reference to the `Option` corresponding to the provided RandomizerTrick key.
     *
     * @param key
     * @return Option&
     */
    TrickOption& GetTrickOption(RandomizerTrick key);

    /**
     * @brief Reset all RandomizerTrick keys.
     */
    void ResetTrickOptions();

    /**
     * @brief Returns a reference to the entire array of options.
     *
     * @return const std::array<Option, RSK_MAX>&
     */
    const std::array<Option, RSK_MAX>& GetAllOptions() const;

    /**
     * @brief Get a list of Location Exclude `Option`s for the given
     * RandomizerCheckArea
     *
     * @param group
     * @return std::vector<Option*>&
     */
    std::vector<Option*>& GetExcludeOptionsForArea(RandomizerCheckArea area);

    /**
     * @brief Get a reference to all of the Exclude Location `Option` lists.
     *
     * @return const std::vector<std::vector<Option*>>&
     */
    const std::vector<std::vector<Option*>>& GetExcludeLocationsOptions() const;

    /**
     * @brief Gets the resolved Starting Age. Represents the actual starting age when the
     * RSK_STARTING_AGE option is set to Random.
     *
     * @return RandoOptionStartingAge
     */
    RandoOptionStartingAge ResolvedStartingAge() const;

    /**
     * @brief Gets the resolved Light Arrow CutScene check condition.
     * There is no direct option for this, it is inferred based on the value of a few other options.
     *
     * @return RandoOptionLACSCondition
     */
    RandoOptionLACSCondition LACSCondition() const;

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

    /**
     * @brief Get the list of `OptionGroup`s.
     *
     * @return const std::array<OptionGroup, RSG_MAX>&
     */
    const std::array<OptionGroup, RSG_MAX>& GetOptionGroups();

    /**
     * @brief Get the `OptionGroup` corresponding to the provided RandomizerSettingGroupKey
     *
     * @param key
     * @return const OptionGroup&
     */
    const OptionGroup& GetOptionGroup(RandomizerSettingGroupKey key);

    /**
     * @brief sets the `selectedOption` of all Options to the value of the CVar
     * corresponding to their `cvarName`s.
    */
    void SetAllFromCVar();

    /**
     * @brief Updates various properties of options based on the value of other options.
     * Used to update visibility, whether or not interaction is disabled, and what the
     * actual option values are. Actually changing option values should be handled in
     * `FinalizeSettings`
     *
     * For example, this function handles setting the maximum possible keyring count to 9
     * when Gerudo's Fortress options are set such that a keyring is possible for that
     * dungeon.
    */
    void UpdateOptionProperties();

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
    void FinalizeSettings(const std::set<RandomizerCheck>& excludedLocations, const std::set<RandomizerTrick>& enabledTricks);

    /**
     * @brief Parse Options from a JSON file.
     *
     * @param spoilerFileJson
     */
    void ParseJson(nlohmann::json spoilerFileJson);
    std::vector<Option*> VanillaLogicDefaults = {};
    std::map<RandomizerArea, std::vector<RandomizerTrick>> mTricksByArea = {};

  private:
    /**
     * @brief Create the list of description strings for `Option`s.
     */
    void CreateOptionDescriptions();
    std::array<Option, RSK_MAX> mOptions = {};
    std::array<std::string, RSK_MAX> mOptionDescriptions = {};
    std::array<OptionGroup, RSG_MAX> mOptionGroups = {};
    std::array<TrickOption, RT_MAX> mTrickOptions = {};
    std::vector<std::vector<Option*>> mExcludeLocationsOptionsAreas = {};
    std::unordered_map<std::string, RandomizerSettingKey> mSpoilerfileSettingNameToEnum;
    RandoOptionStartingAge mResolvedStartingAge =  RO_AGE_CHILD;
    RandoOptionLACSCondition mLACSCondition = RO_LACS_VANILLA;
    std::string mHash;
    std::string mSeedString;
    uint32_t mFinalSeed = 0;
    std::unordered_map<std::string, RandomizerTrick> mTrickNameToEnum;
};
} // namespace Rando

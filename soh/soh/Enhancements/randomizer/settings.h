#pragma once

#include "SeedContext.h"
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
     * @brief Hides or Unhides the UI of Mixed Entrance Pools
     */
    void HandleMixedEntrancePoolsUI();

    /**
     * @brief UI Callback for handling UI state of Starting Age shuffle.
     */
    void HandleStartingAgeUI();

    /**
     * @brief Creates the `Option` and `OptionGroup` objects. This happens after construction because certain
     * other events in the codebase need to happen before all of the `Option`s can be created.
     */
    void CreateOptions();

    /**
     * @brief Populates the map used to translate strings into RandomiserSettingKeys
     *
     * @return std::unordered_map<std::string, RandomizerSettingKey>
     */

    std::unordered_map<std::string, RandomizerSettingKey> PopulateOptionNameToEnum();

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
     * @brief Get the RandomizerTrick corresponding to the provided name.
     *
     * @param name
     * @return int RandomizerTrick index or -1 if not found
     */
    int GetRandomizerTrickByName(const std::string& name);

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
     * @brief Runs the Callback on every option, to ensure they are all
     * hidden/unhidden and/or disabled/enabled properly after applying a
     * preset or dropping a file.
     */
    void UpdateAllOptions();

    /**
     * @brief Parse Options from a JSON file.
     *
     * @param spoilerFileJson
     */
    void ParseJson(nlohmann::json spoilerFileJson);
    std::map<RandomizerArea, std::vector<RandomizerTrick>> mTricksByArea = {};

    /**
     * @brief Assigns a Rando::Context instance to this settings instance
     * with which to later instantiate and access OptionValues.
     *
     * @param ctx
     */
    void AssignContext(std::shared_ptr<Context> ctx);
    void ClearContext();

    /**
     * @brief Sets all the currently selected MenuIndexes to the currently assigned Rando::Context.
     */
    void SetAllToContext();

    static std::shared_ptr<Settings> GetInstance();

  private:
    /**
     * @brief Create the list of description strings for `Option`s.
     */
    void CreateOptionDescriptions();
    static std::shared_ptr<Settings> mInstance;
    std::shared_ptr<Context> mContext = nullptr;
    std::array<Option, RSK_MAX> mOptions = {};
    std::array<std::string, RSK_MAX> mOptionDescriptions = {};
    std::array<OptionGroup, RSG_MAX> mOptionGroups = {};
    std::array<TrickOption, RT_MAX> mTrickOptions = {};
    std::vector<std::vector<Option*>> mExcludeLocationsOptionsAreas = {};
    std::unordered_map<std::string, RandomizerTrick> mTrickNameToEnum;
};
} // namespace Rando

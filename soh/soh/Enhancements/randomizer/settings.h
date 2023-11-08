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
    void CreateOptions();
    Option& Setting(RandomizerSettingKey key);
    Option& GetTrickOption(RandomizerTrick key);
    const std::array<Option, RSK_MAX>& GetAllOptions() const;
    std::vector<Option*>& GetExcludeOptionsForGroup(SpoilerCollectionCheckGroup group);
    const std::vector<std::vector<Option*>>& GetExcludeLocationsOptions() const;
    RandoOptionStartingAge ResolvedStartingAge() const;
    RandoOptionLACSCondition LACSCondition() const;
    std::string GetHash() const;
    const std::string& GetSeedString() const;
    void SetSeedString(std::string seedString);
    const uint32_t GetSeed() const;
    void SetSeed(uint32_t seed);
    void SetHash(std::string hash);
    const std::array<OptionGroup, RSG_MAX>& GetOptionGroups();
    const OptionGroup& GetOptionGroup(RandomizerSettingGroupKey key);
    void SetAllFromCVar();
    void UpdateSettings(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks);
    void ParseJson(nlohmann::json spoilerFileJson);
    std::vector<Option*> VanillaLogicDefaults = {};

  private:
    void CreateOptionDescriptions();
    std::array<Option, RSK_MAX> mOptions = {};
    std::array<std::string, RSK_MAX> mOptionDescriptions = {};
    std::array<OptionGroup, RSG_MAX> mOptionGroups = {};
    std::array<Option, RT_MAX> mTrickOptions;
    std::vector<std::vector<Option*>> mExcludeLocationsOptionsGroups;
    std::unordered_map<std::string, RandomizerSettingKey> mSpoilerfileSettingNameToEnum;
    RandoOptionStartingAge mResolvedStartingAge;
    RandoOptionLACSCondition mLACSCondition;
    std::string mHash;
    std::string mSeedString;
    uint32_t mFinalSeed;
};
} // namespace Rando

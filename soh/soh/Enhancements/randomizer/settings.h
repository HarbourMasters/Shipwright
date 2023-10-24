#pragma once

#include "option.h"
#include "randomizerTypes.h"
#include "location.h"

#include <array>

namespace Rando {
class Settings {
    public:
        Settings();
        Option& Setting(RandomizerSettingKey key);
        const std::array<Option, RSK_MAX>& GetAllOptions() const;
        std::vector<Option *>& GetExcludeOptionsForGroup(SpoilerCollectionCheckGroup group);
        const std::vector<std::vector<Option *>>& GetExcludeLocationsOptions() const;
        RandoOptionStartingAge ResolvedStartingAge() const;
        std::string GetHash() const;
        std::array<Option, RT_MAX> trickOptions;
        const std::string& GetSeedString() const;
        void SetSeedString(std::string seedString);
        const uint32_t GetSeed() const;
        void SetSeed(uint32_t seed);
        void SetHash(std::string hash);
    private:
        std::array<Option, RSK_MAX> mOptions = {};
        std::vector<std::vector<Option *>> mExcludeLocationsOptionsGroups = {};
        RandoOptionStartingAge mResolvedStartingAge;
        std::string mHash;
        std::string mSeedString;
        uint32_t mSeed;
};
} // namespace Rando

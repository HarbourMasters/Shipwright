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
    private:
        std::array<Option, RSK_MAX> mOptions = {};
        std::vector<std::vector<Option *>> mExcludeLocationsOptionsGroups = {};
        RandoOptionStartingAge mResolvedStartingAge;
        std::string mHash;
};
} // namespace Rando

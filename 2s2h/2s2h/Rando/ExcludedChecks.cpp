#include "Rando.h"
#include "2s2h/Rando/StaticData/StaticData.h"
#include <libultraship/libultraship.h>
#include <libultraship/bridge/consolevariablebridge.h>

namespace Rando {

std::vector<RandoCheckId> GetExcludedChecksFromConfig() {
    auto allConfig = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();
    std::vector<RandoCheckId> excludedChecks;

    if (allConfig.find("CVars") != allConfig.end() && allConfig["CVars"].is_object() &&
        allConfig["CVars"].find("gRando") != allConfig["CVars"].end() && allConfig["CVars"]["gRando"].is_object() &&
        allConfig["CVars"]["gRando"].find("ExcludedChecks") != allConfig["CVars"]["gRando"].end()) {

        if (allConfig["CVars"]["gRando"]["ExcludedChecks"].is_array()) {
            auto excludedChecksStrings = allConfig["CVars"]["gRando"]["ExcludedChecks"].get<std::vector<std::string>>();
            for (auto& checkName : excludedChecksStrings) {
                auto randoCheckId = Rando::StaticData::GetCheckIdFromName(checkName.c_str());
                if (randoCheckId > RC_UNKNOWN && randoCheckId < RC_MAX) {
                    excludedChecks.push_back(randoCheckId);
                }
            }
        } else if (allConfig["CVars"]["gRando"]["ExcludedChecks"].is_string()) {
            CVarClear("gRando.ExcludedChecks");
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        }
    }

    std::sort(excludedChecks.begin(), excludedChecks.end());
    return excludedChecks;
}

void SetExcludedChecksInConfig(std::vector<RandoCheckId>& excludedChecks) {
    auto excludedChecksJson = nlohmann::json::array();

    for (auto& randoCheckId : excludedChecks) {
        if (randoCheckId > RC_UNKNOWN && randoCheckId < RC_MAX) {
            excludedChecksJson.push_back(Rando::StaticData::Checks[randoCheckId].name);
        }
    }

    Ship::Context::GetRawInstance()->GetConfig()->SetBlock("CVars.gRando.ExcludedChecks", excludedChecksJson);
    Ship::Context::GetRawInstance()->GetConfig()->Save();
}

} // namespace Rando

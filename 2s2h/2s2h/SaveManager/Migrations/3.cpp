#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

// For a short period of time, we hijacked the owl save location to store the pause save entrance.
// We want to instead use a new field, and migrate anyone that previously had this set.
void SaveManager_Migration_3(nlohmann::json& j) {
    int32_t owlSaveLocation = j["save"]["owlSaveLocation"].template get<int32_t>();

    if (owlSaveLocation > 9) {
        j["save"]["shipSaveInfo"]["pauseSaveEntrance"] = owlSaveLocation;
        j["save"]["owlSaveLocation"] = 0;
    } else {
        j["save"]["shipSaveInfo"]["pauseSaveEntrance"] = -1;
    }
}

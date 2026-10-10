#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

// Add Persistent Bunny Hood state to saves
void SaveManager_Migration_8(nlohmann::json& j) {
    if (!j["save"]["shipSaveInfo"].contains("persistentBunnyHood")) {
        j["save"]["shipSaveInfo"]["persistentBunnyHood"] = 0;
    }
}

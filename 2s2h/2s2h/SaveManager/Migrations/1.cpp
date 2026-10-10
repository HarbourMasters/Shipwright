#include "2s2h/SaveManager/SaveManager.h"

// To better support future migrations, we always store the `Save` at the `save` key to be consistent across normal
// saves and owl saves
void SaveManager_Migration_1(nlohmann::json& j) {
    bool isOwlSave = j.contains("save");

    if (!isOwlSave) {
        nlohmann::json saveContext;
        saveContext["save"] = j;

        j = saveContext;
    }
}

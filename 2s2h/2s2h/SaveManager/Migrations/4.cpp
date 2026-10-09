#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

// Final legacy migration, prior to merging owl saves into main save file.
void SaveManager_Migration_4(nlohmann::json& j) {
    nlohmann::json rootSave;
    rootSave["newCycleSave"] = j;

    j = rootSave;
}

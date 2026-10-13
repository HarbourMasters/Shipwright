#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

// This is the base migration, binary saves are imported as this version.
void SaveManager_Migration_5(nlohmann::json& j) {
    nlohmann::json dpadEquips;
    for (int i = 0; i < EQUIP_SLOT_D_MAX; i++) {
        for (int j = 0; j < EQUIP_SLOT_D_MAX; j++) {
            dpadEquips["dpadItems"][i][j] = ITEM_NONE;
            dpadEquips["dpadSlots"][i][j] = SLOT_NONE;
        }
    }

    // if shipSaveInfo doesn't exist, create it
    if (!j["save"].contains("shipSaveInfo")) {
        j["save"]["shipSaveInfo"]["dpadEquips"] = dpadEquips;
        j["save"]["shipSaveInfo"]["pauseSaveEntrance"] = -1;
    }
}

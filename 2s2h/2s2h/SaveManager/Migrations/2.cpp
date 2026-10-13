#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

void SaveManager_Migration_2(nlohmann::json& j) {
    nlohmann::json dpadEquips;
    for (int i = 0; i < EQUIP_SLOT_D_MAX; i++) {
        for (int j = 0; j < EQUIP_SLOT_D_MAX; j++) {
            dpadEquips["dpadItems"][i][j] = ITEM_NONE;
            dpadEquips["dpadSlots"][i][j] = SLOT_NONE;
        }
    }

    j["save"]["shipSaveInfo"]["dpadEquips"] = dpadEquips;
}
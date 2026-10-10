#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

// Add respawn and playtime info to saves
void SaveManager_Migration_7(nlohmann::json& j) {
    if (!j["save"]["shipSaveInfo"].contains("filePlaytime")) {
        j["save"]["shipSaveInfo"]["filePlaytime"] = 0;
    }
    if (!j["save"]["shipSaveInfo"].contains("respawn")) {
        j["save"]["shipSaveInfo"]["respawn"] = {};
    }

    for (int i = 0; i < RESPAWN_MODE_MAX; i++) {
        j["save"]["shipSaveInfo"]["respawn"][i] = {
            { "pos",
              {
                  { "x", 0.0 },
                  { "y", 0.0 },
                  { "z", 0.0 },
              } },
            { "yaw", 0 },
            { "playerParams", 0 },
            { "entrance", 0 },
            { "roomIndex", 0 },
            { "data", 0 },
            { "tempSwitchFlags", 0 },
            { "unk_18", 0 },
            { "tempCollectFlags", 0 },
        };
    }
}

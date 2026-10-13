#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

void SaveManager_Migration_6(nlohmann::json& j) {
    // if saveType doesn't exist, create it and set it to vanilla
    if (!j["save"]["shipSaveInfo"].contains("saveType")) {
        j["save"]["shipSaveInfo"]["saveType"] = SAVETYPE_VANILLA;
    }
    if (!j["save"]["shipSaveInfo"].contains("commitHash")) {
        u8 commitHash[8] = { 0 };
        j["save"]["shipSaveInfo"]["commitHash"] = commitHash;
    }
    if (!j["save"]["shipSaveInfo"].contains("fileCreatedAt")) {
        j["save"]["shipSaveInfo"]["fileCreatedAt"] = 0;
    }
    if (!j["save"]["shipSaveInfo"].contains("fileCompletedAt")) {
        j["save"]["shipSaveInfo"]["fileCompletedAt"] = 0;
    }
}

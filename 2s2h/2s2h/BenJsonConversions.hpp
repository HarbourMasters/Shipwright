#ifndef BenJsonConversions_hpp
#define BenJsonConversions_hpp

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "build.h"

extern "C" {
#include "z64save.h"
#include "macros.h"
}

using json = nlohmann::json;

inline void to_json(json& j, const DpadSaveInfo& dpadEquips) {
    j = json{
        { "dpadItems", dpadEquips.dpadItems },
        { "dpadSlots", dpadEquips.dpadSlots },
    };
}

inline void from_json(const json& j, DpadSaveInfo& dpadEquips) {
    for (int i = 0; i < ARRAY_COUNT(dpadEquips.dpadItems); i++) {
        j.at("dpadItems").at(i).get_to(dpadEquips.dpadItems[i]);
        j.at("dpadSlots").at(i).get_to(dpadEquips.dpadSlots[i]);
    }
}

inline void to_json(json& j, const RandoSaveCheck& randoSaveCheck) {
    j = json{
        { "randoItemId", randoSaveCheck.randoItemId },
        { "eligible", randoSaveCheck.eligible },
        { "cycleObtained", randoSaveCheck.cycleObtained },
        { "obtained", randoSaveCheck.obtained },
        { "shuffled", randoSaveCheck.shuffled },
        { "skipped", randoSaveCheck.skipped },
        { "price", randoSaveCheck.price },
    };
}

inline void from_json(const json& j, RandoSaveCheck& randoSaveCheck) {
    j.at("randoItemId").get_to(randoSaveCheck.randoItemId);
    j.at("eligible").get_to(randoSaveCheck.eligible);
    j.at("cycleObtained").get_to(randoSaveCheck.cycleObtained);
    j.at("obtained").get_to(randoSaveCheck.obtained);
    j.at("shuffled").get_to(randoSaveCheck.shuffled);
    j.at("skipped").get_to(randoSaveCheck.skipped);
    j.at("price").get_to(randoSaveCheck.price);
}

inline void to_json(json& j, const RandoSaveInfo& rando) {
    j = json{
        { "randoInf", rando.randoInf },
        { "randoEvents", rando.randoEvents },
        { "randoSaveChecks", rando.randoSaveChecks },
        { "finalSeed", rando.finalSeed },
        { "randoSaveOptions", rando.randoSaveOptions },
        { "randoStartingItems", rando.randoStartingItems },
        { "foundDungeonKeys", rando.foundDungeonKeys },
        { "foundTriforcePieces", rando.foundTriforcePieces },
        { "sariaHintsAvailable", rando.sariaHintsAvailable },
        { "sariaPriorityItems", rando.sariaPriorityItems },
    };
}

inline void from_json(const json& j, RandoSaveInfo& rando) {
    j.at("randoInf").get_to(rando.randoInf);
    j.at("randoEvents").get_to(rando.randoEvents);
    j.at("randoSaveChecks").get_to(rando.randoSaveChecks);
    j.at("finalSeed").get_to(rando.finalSeed);
    j.at("randoSaveOptions").get_to(rando.randoSaveOptions);
    j.at("randoStartingItems").get_to(rando.randoStartingItems);
    j.at("foundDungeonKeys").get_to(rando.foundDungeonKeys);
    j.at("foundTriforcePieces").get_to(rando.foundTriforcePieces);
    j.at("sariaHintsAvailable").get_to(rando.sariaHintsAvailable);
    j.at("sariaPriorityItems").get_to(rando.sariaPriorityItems);
}

inline void to_json(json& j, const Vec3f& vec) {
    j = json{
        { "x", vec.x },
        { "y", vec.y },
        { "z", vec.z },
    };
}

inline void from_json(const json& j, Vec3f& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

inline void to_json(json& j, const RespawnData& respawnData) {
    j = json{
        { "pos", respawnData.pos },
        { "yaw", respawnData.yaw },
        { "playerParams", respawnData.playerParams },
        { "entrance", respawnData.entrance },
        { "roomIndex", respawnData.roomIndex },
        { "data", respawnData.data },
        { "tempSwitchFlags", respawnData.tempSwitchFlags },
        { "unk_18", respawnData.unk_18 },
        { "tempCollectFlags", respawnData.tempCollectFlags },
    };
}

inline void from_json(const json& j, RespawnData& respawnData) {
    j.at("pos").get_to(respawnData.pos);
    j.at("yaw").get_to(respawnData.yaw);
    j.at("playerParams").get_to(respawnData.playerParams);
    j.at("entrance").get_to(respawnData.entrance);
    j.at("roomIndex").get_to(respawnData.roomIndex);
    j.at("data").get_to(respawnData.data);
    j.at("tempSwitchFlags").get_to(respawnData.tempSwitchFlags);
    j.at("unk_18").get_to(respawnData.unk_18);
    j.at("tempCollectFlags").get_to(respawnData.tempCollectFlags);
}

inline void to_json(json& j, const ShipSaveInfo& shipSaveInfo) {
    uint8_t commitHash[8];
    memcpy(commitHash, shipSaveInfo.commitHash, sizeof(commitHash));

    j = json{
        { "dpadEquips", shipSaveInfo.dpadEquips },
        { "pauseSaveEntrance", shipSaveInfo.pauseSaveEntrance },
        { "persistentBunnyHood", shipSaveInfo.persistentBunnyHood },
        { "saveType", shipSaveInfo.saveType },
        { "fileCreatedAt", shipSaveInfo.fileCreatedAt },
        { "fileCompletedAt", shipSaveInfo.fileCompletedAt },
        { "filePlaytime", shipSaveInfo.filePlaytime },
        { "respawn", shipSaveInfo.respawn },
        { "commitHash", commitHash },
    };

    if (shipSaveInfo.saveType == SAVETYPE_RANDO) {
        j["rando"] = shipSaveInfo.rando;
    }
}

inline void from_json(const json& j, ShipSaveInfo& shipSaveInfo) {
    j.at("dpadEquips").get_to(shipSaveInfo.dpadEquips);
    j.at("pauseSaveEntrance").get_to(shipSaveInfo.pauseSaveEntrance);
    j.at("persistentBunnyHood").get_to(shipSaveInfo.persistentBunnyHood);
    j.at("saveType").get_to(shipSaveInfo.saveType);
    j.at("fileCreatedAt").get_to(shipSaveInfo.fileCreatedAt);
    j.at("fileCompletedAt").get_to(shipSaveInfo.fileCompletedAt);
    j.at("filePlaytime").get_to(shipSaveInfo.filePlaytime);
    j.at("respawn").get_to(shipSaveInfo.respawn);
    j.at("commitHash").get_to(shipSaveInfo.commitHash);

    if (shipSaveInfo.saveType == SAVETYPE_RANDO) {
        if (strcmp(shipSaveInfo.commitHash, gGitCommitHash) != 0) {
            SPDLOG_ERROR("Randomizer saves cannot be loaded from a different version.");
            throw new std::runtime_error("Randomizer saves cannot be loaded from a different version.");
        }

        j.at("rando").get_to(shipSaveInfo.rando);
    }
}

inline void to_json(json& j, const ItemEquips& itemEquips) {
    j = json{
        { "buttonItems", itemEquips.buttonItems },
        { "cButtonSlots", itemEquips.cButtonSlots },
        { "equipment", itemEquips.equipment },
    };
}

inline void from_json(const json& j, ItemEquips& itemEquips) {
    j.at("equipment").get_to(itemEquips.equipment);
    // buttonItems and cButtonSlots are arrays of arrays, so we need to manually parse them
    for (int i = 0; i < ARRAY_COUNT(itemEquips.buttonItems); i++) {
        j.at("buttonItems").at(i).get_to(itemEquips.buttonItems[i]);
        j.at("cButtonSlots").at(i).get_to(itemEquips.cButtonSlots[i]);
    }
}

inline void to_json(json& j, const Inventory& inventory) {
    // Setup and copy u8 arrays to avoid json treating char[] as strings
    // These char[] are not null-terminated, so saving as strings causes overflow/corruption
    uint8_t dekuPlaygroundPlayerName[3][8];
    memcpy(dekuPlaygroundPlayerName, inventory.dekuPlaygroundPlayerName, sizeof(dekuPlaygroundPlayerName));

    j = json{
        { "items", inventory.items },
        { "ammo", inventory.ammo },
        { "upgrades", inventory.upgrades },
        { "questItems", inventory.questItems },
        { "dungeonItems", inventory.dungeonItems },
        { "dungeonKeys", inventory.dungeonKeys },
        { "defenseHearts", inventory.defenseHearts },
        { "strayFairies", inventory.strayFairies },
        { "dekuPlaygroundPlayerName", dekuPlaygroundPlayerName },
    };
}

inline void from_json(const json& j, Inventory& inventory) {
    j.at("items").get_to(inventory.items);
    j.at("ammo").get_to(inventory.ammo);
    j.at("upgrades").get_to(inventory.upgrades);
    j.at("questItems").get_to(inventory.questItems);
    j.at("dungeonItems").get_to(inventory.dungeonItems);
    j.at("dungeonKeys").get_to(inventory.dungeonKeys);
    j.at("defenseHearts").get_to(inventory.defenseHearts);
    j.at("strayFairies").get_to(inventory.strayFairies);
    // dekuPlaygroundPlayerName is an array of arrays, so we need to manually parse it
    for (int i = 0; i < ARRAY_COUNT(inventory.dekuPlaygroundPlayerName); i++) {
        j.at("dekuPlaygroundPlayerName").at(i).get_to(inventory.dekuPlaygroundPlayerName[i]);
    }
}

inline void to_json(json& j, const PermanentSceneFlags& permanentSceneFlags) {
    j = json{
        { "chest", permanentSceneFlags.chest },
        { "switch0", permanentSceneFlags.switch0 },
        { "switch1", permanentSceneFlags.switch1 },
        { "clearedRoom", permanentSceneFlags.clearedRoom },
        { "collectible", permanentSceneFlags.collectible },
        { "unk_14", permanentSceneFlags.unk_14 },
        { "rooms", permanentSceneFlags.rooms },
    };
}

inline void from_json(const json& j, PermanentSceneFlags& permanentSceneFlags) {
    j.at("chest").get_to(permanentSceneFlags.chest);
    j.at("switch0").get_to(permanentSceneFlags.switch0);
    j.at("switch1").get_to(permanentSceneFlags.switch1);
    j.at("clearedRoom").get_to(permanentSceneFlags.clearedRoom);
    j.at("collectible").get_to(permanentSceneFlags.collectible);
    j.at("unk_14").get_to(permanentSceneFlags.unk_14);
    j.at("rooms").get_to(permanentSceneFlags.rooms);
}

inline void to_json(json& j, const SavePlayerData& savePlayerData) {
    // Setup and copy u8 arrays to avoid json treating char[] as strings
    // These char[] are not null-terminated, so saving as strings causes overflow/corruption
    u8 newf[6];
    u8 playerName[8];
    memcpy(newf, savePlayerData.newf, sizeof(newf));
    memcpy(playerName, savePlayerData.playerName, sizeof(playerName));

    j = json{
        { "newf", newf },
        { "threeDayResetCount", savePlayerData.threeDayResetCount },
        { "playerName", playerName },
        { "healthCapacity", savePlayerData.healthCapacity },
        { "health", savePlayerData.health },
        { "magicLevel", savePlayerData.magicLevel },
        { "magic", savePlayerData.magic },
        { "rupees", savePlayerData.rupees },
        { "swordHealth", savePlayerData.swordHealth },
        { "tatlTimer", savePlayerData.tatlTimer },
        { "isMagicAcquired", savePlayerData.isMagicAcquired },
        { "isDoubleMagicAcquired", savePlayerData.isDoubleMagicAcquired },
        { "doubleDefense", savePlayerData.doubleDefense },
        { "unk_1F", savePlayerData.unk_1F },
        { "unk_20", savePlayerData.owlWarpId }, // TODO: Migrate save to use the new name?
        { "owlActivationFlags", savePlayerData.owlActivationFlags },
        { "unk_24", savePlayerData.unk_24 },
        { "savedSceneId", savePlayerData.savedSceneId },
    };
}

inline void from_json(const json& j, SavePlayerData& savePlayerData) {
    j.at("newf").get_to(savePlayerData.newf);
    j.at("threeDayResetCount").get_to(savePlayerData.threeDayResetCount);
    j.at("playerName").get_to(savePlayerData.playerName);
    j.at("healthCapacity").get_to(savePlayerData.healthCapacity);
    j.at("health").get_to(savePlayerData.health);
    j.at("magicLevel").get_to(savePlayerData.magicLevel);
    j.at("magic").get_to(savePlayerData.magic);
    j.at("rupees").get_to(savePlayerData.rupees);
    j.at("swordHealth").get_to(savePlayerData.swordHealth);
    j.at("tatlTimer").get_to(savePlayerData.tatlTimer);
    j.at("isMagicAcquired").get_to(savePlayerData.isMagicAcquired);
    j.at("isDoubleMagicAcquired").get_to(savePlayerData.isDoubleMagicAcquired);
    j.at("doubleDefense").get_to(savePlayerData.doubleDefense);
    j.at("unk_1F").get_to(savePlayerData.unk_1F);
    j.at("unk_20").get_to(savePlayerData.owlWarpId); // TODO: Migrate save to use the new name?
    j.at("owlActivationFlags").get_to(savePlayerData.owlActivationFlags);
    j.at("unk_24").get_to(savePlayerData.unk_24);
    j.at("savedSceneId").get_to(savePlayerData.savedSceneId);
}

inline void to_json(json& j, const Vec3s& vec) {
    j = json{
        { "x", vec.x },
        { "y", vec.y },
        { "z", vec.z },
    };
}

inline void from_json(const json& j, Vec3s& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

inline void to_json(json& j, const HorseData& horseData) {
    j = json{
        { "sceneId", horseData.sceneId },
        { "pos", horseData.pos },
        { "yaw", horseData.yaw },
    };
}

inline void from_json(const json& j, HorseData& horseData) {
    j.at("sceneId").get_to(horseData.sceneId);
    j.at("pos").get_to(horseData.pos);
    j.at("yaw").get_to(horseData.yaw);
}

inline void to_json(json& j, const SaveInfo& saveInfo) {
    j = json{
        { "playerData", saveInfo.playerData },
        { "equips", saveInfo.equips },
        { "inventory", saveInfo.inventory },
        { "permanentSceneFlags", saveInfo.permanentSceneFlags },
        { "unk_DF4", saveInfo.unk_DF4 },
        { "dekuPlaygroundHighScores", saveInfo.dekuPlaygroundHighScores },
        { "pictoFlags0", saveInfo.pictoFlags0 },
        { "pictoFlags1", saveInfo.pictoFlags1 },
        { "unk_E5C", saveInfo.unk_E5C },
        { "unk_E60", saveInfo.unk_E60 },
        { "unk_E64", saveInfo.alienInfo }, // TODO: Add migration to rename this?
        { "scenesVisible", saveInfo.scenesVisible },
        { "skullTokenCount", saveInfo.skullTokenCount },
        { "unk_EA0", saveInfo.unk_EA0 },
        { "unk_EA4", saveInfo.unk_EA4 },
        { "unk_EA8", saveInfo.unk_EA8 },
        { "stolenItems", saveInfo.stolenItems },
        { "unk_EB4", saveInfo.unk_EB4 },
        { "highScores", saveInfo.highScores },
        { "weekEventReg", saveInfo.weekEventReg },
        { "regionsVisited", saveInfo.regionsVisited },
        { "worldMapCloudVisibility", saveInfo.worldMapCloudVisibility },
        { "unk_F40", saveInfo.unk_F40 },
        { "scarecrowSpawnSongSet", saveInfo.scarecrowSpawnSongSet },
        { "scarecrowSpawnSong", saveInfo.scarecrowSpawnSong },
        { "bombersCaughtNum", saveInfo.bombersCaughtNum },
        { "bombersCaughtOrder", saveInfo.bombersCaughtOrder },
        { "lotteryCodes", saveInfo.lotteryCodes },
        { "spiderHouseMaskOrder", saveInfo.spiderHouseMaskOrder },
        { "bomberCode", saveInfo.bomberCode },
        { "horseData", saveInfo.horseData },
        { "checksum", saveInfo.checksum },
    };
}

inline void from_json(const json& j, SaveInfo& saveInfo) {
    j.at("playerData").get_to(saveInfo.playerData);
    j.at("equips").get_to(saveInfo.equips);
    j.at("inventory").get_to(saveInfo.inventory);
    j.at("permanentSceneFlags").get_to(saveInfo.permanentSceneFlags);
    j.at("unk_DF4").get_to(saveInfo.unk_DF4);
    j.at("dekuPlaygroundHighScores").get_to(saveInfo.dekuPlaygroundHighScores);
    j.at("pictoFlags0").get_to(saveInfo.pictoFlags0);
    j.at("pictoFlags1").get_to(saveInfo.pictoFlags1);
    j.at("unk_E5C").get_to(saveInfo.unk_E5C);
    j.at("unk_E60").get_to(saveInfo.unk_E60);
    j.at("unk_E64").get_to(saveInfo.alienInfo); // TODO: Add migration to rename this?
    j.at("scenesVisible").get_to(saveInfo.scenesVisible);
    j.at("skullTokenCount").get_to(saveInfo.skullTokenCount);
    j.at("unk_EA0").get_to(saveInfo.unk_EA0);
    j.at("unk_EA4").get_to(saveInfo.unk_EA4);
    j.at("unk_EA8").get_to(saveInfo.unk_EA8);
    j.at("stolenItems").get_to(saveInfo.stolenItems);
    j.at("unk_EB4").get_to(saveInfo.unk_EB4);
    j.at("highScores").get_to(saveInfo.highScores);
    j.at("weekEventReg").get_to(saveInfo.weekEventReg);
    j.at("regionsVisited").get_to(saveInfo.regionsVisited);
    j.at("worldMapCloudVisibility").get_to(saveInfo.worldMapCloudVisibility);
    j.at("unk_F40").get_to(saveInfo.unk_F40);
    j.at("scarecrowSpawnSongSet").get_to(saveInfo.scarecrowSpawnSongSet);
    j.at("scarecrowSpawnSong").get_to(saveInfo.scarecrowSpawnSong);
    j.at("bombersCaughtNum").get_to(saveInfo.bombersCaughtNum);
    j.at("bombersCaughtOrder").get_to(saveInfo.bombersCaughtOrder);
    // lotteryCodes is an array of arrays, so we need to manually parse it
    for (int i = 0; i < ARRAY_COUNT(saveInfo.lotteryCodes); i++) {
        j.at("lotteryCodes").at(i).get_to(saveInfo.lotteryCodes[i]);
    }
    j.at("spiderHouseMaskOrder").get_to(saveInfo.spiderHouseMaskOrder);
    j.at("bomberCode").get_to(saveInfo.bomberCode);
    j.at("horseData").get_to(saveInfo.horseData);
    j.at("checksum").get_to(saveInfo.checksum);
}

inline void to_json(json& j, const Save& save) {
    j = json{
        { "entrance", save.entrance },
        { "equippedMask", save.equippedMask },
        { "isFirstCycle", save.isFirstCycle },
        { "unk_06", save.unk_06 },
        { "linkAge", save.linkAge },
        { "cutsceneIndex", save.cutsceneIndex },
        { "time", save.time },
        { "owlSaveLocation", save.owlWarpId }, // TODO: Migrate save to use the new name?
        { "isNight", save.isNight },
        { "timeSpeedOffset", save.timeSpeedOffset },
        { "day", save.day },
        { "eventDayCount", save.eventDayCount },
        { "playerForm", save.playerForm },
        { "snowheadCleared", save.snowheadCleared },
        { "hasTatl", save.hasTatl },
        { "isOwlSave", save.isOwlSave },
        { "saveInfo", save.saveInfo },
        { "shipSaveInfo", save.shipSaveInfo },
    };
}

inline void from_json(const json& j, Save& save) {
    j.at("entrance").get_to(save.entrance);
    j.at("equippedMask").get_to(save.equippedMask);
    j.at("isFirstCycle").get_to(save.isFirstCycle);
    j.at("unk_06").get_to(save.unk_06);
    j.at("linkAge").get_to(save.linkAge);
    j.at("cutsceneIndex").get_to(save.cutsceneIndex);
    j.at("time").get_to(save.time);
    j.at("owlSaveLocation").get_to(save.owlWarpId); // TODO: Migrate save to use the new name?
    j.at("isNight").get_to(save.isNight);
    j.at("timeSpeedOffset").get_to(save.timeSpeedOffset);
    j.at("day").get_to(save.day);
    j.at("eventDayCount").get_to(save.eventDayCount);
    j.at("playerForm").get_to(save.playerForm);
    j.at("snowheadCleared").get_to(save.snowheadCleared);
    j.at("hasTatl").get_to(save.hasTatl);
    j.at("isOwlSave").get_to(save.isOwlSave);
    j.at("saveInfo").get_to(save.saveInfo);
    j.at("shipSaveInfo").get_to(save.shipSaveInfo);
}

inline void to_json(json& j, const SaveContext& saveContext) {
    j = json{
        { "save", saveContext.save },
        { "eventInf", saveContext.eventInf },
        { "unk_1014", saveContext.unk_1014 },
        { "bButtonStatus", saveContext.bButtonStatus },
        { "jinxTimer", saveContext.jinxTimer },
        { "rupeeAccumulator", saveContext.rupeeAccumulator },
        { "bottleTimerStates", saveContext.bottleTimerStates },
        { "bottleTimerStartOsTimes", saveContext.bottleTimerStartOsTimes },
        { "bottleTimerTimeLimits", saveContext.bottleTimerTimeLimits },
        { "bottleTimerCurTimes", saveContext.bottleTimerCurTimes },
        { "bottleTimerPausedOsTimes", saveContext.bottleTimerPausedOsTimes },
        { "pictoPhotoI5", saveContext.pictoPhotoI5 },
    };
}

inline void from_json(const json& j, SaveContext& saveContext) {
    j.at("save").get_to(saveContext.save);
    j.at("eventInf").get_to(saveContext.eventInf);
    j.at("unk_1014").get_to(saveContext.unk_1014);
    j.at("bButtonStatus").get_to(saveContext.bButtonStatus);
    j.at("jinxTimer").get_to(saveContext.jinxTimer);
    j.at("rupeeAccumulator").get_to(saveContext.rupeeAccumulator);
    j.at("bottleTimerStates").get_to(saveContext.bottleTimerStates);
    j.at("bottleTimerStartOsTimes").get_to(saveContext.bottleTimerStartOsTimes);
    j.at("bottleTimerTimeLimits").get_to(saveContext.bottleTimerTimeLimits);
    j.at("bottleTimerCurTimes").get_to(saveContext.bottleTimerCurTimes);
    j.at("bottleTimerPausedOsTimes").get_to(saveContext.bottleTimerPausedOsTimes);
    j.at("pictoPhotoI5").get_to(saveContext.pictoPhotoI5);
}

inline void to_json(json& j, const SaveOptions& saveOptions) {
    j = json{
        { "optionId", saveOptions.optionId },
        { "language", saveOptions.language },
        { "audioSetting", saveOptions.audioSetting },
        { "languageSetting", saveOptions.languageSetting },
        { "zTargetSetting", saveOptions.zTargetSetting },
    };
}

inline void from_json(const json& j, SaveOptions& saveOptions) {
    j.at("optionId").get_to(saveOptions.optionId);
    j.at("language").get_to(saveOptions.language);
    j.at("audioSetting").get_to(saveOptions.audioSetting);
    j.at("languageSetting").get_to(saveOptions.languageSetting);
    j.at("zTargetSetting").get_to(saveOptions.zTargetSetting);
}

#endif // BenJsonConversions_hpp

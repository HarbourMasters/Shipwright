#ifndef NETWORK_ANCHOR_JSON_CONVERSIONS_H
#define NETWORK_ANCHOR_JSON_CONVERSIONS_H
#ifdef __cplusplus

#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "Anchor.h"

extern "C" {
#include "z64.h"
}

using json = nlohmann::json;

inline void from_json(const json& j, Color_RGB8& color) {
    j.at("r").get_to(color.r);
    j.at("g").get_to(color.g);
    j.at("b").get_to(color.b);
}

inline void to_json(json& j, const Color_RGB8& color) {
    j = json{ { "r", color.r }, { "g", color.g }, { "b", color.b } };
}

inline void to_json(json& j, const Vec3f& vec) {
    j = json{ { "x", vec.x }, { "y", vec.y }, { "z", vec.z } };
}

inline void to_json(json& j, const Vec3s& vec) {
    j = json{ { "x", vec.x }, { "y", vec.y }, { "z", vec.z } };
}

inline void from_json(const json& j, Vec3f& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

inline void from_json(const json& j, Vec3s& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

inline void to_json(json& j, const PosRot& posRot) {
    j = json{ { "pos", posRot.pos }, { "rot", posRot.rot } };
}

inline void from_json(const json& j, PosRot& posRot) {
    j.at("pos").get_to(posRot.pos);
    j.at("rot").get_to(posRot.rot);
}

inline void from_json(const json& j, AnchorClient& client) {
    j.contains("clientId") ? j.at("clientId").get_to(client.clientId) : client.clientId = 0;
    j.contains("name") ? j.at("name").get_to(client.name) : client.name = "???";
    j.contains("color") ? j.at("color").get_to(client.color) : client.color = { 255, 255, 255 };
    j.contains("clientVersion") ? j.at("clientVersion").get_to(client.clientVersion) : client.clientVersion = "???";
    j.contains("teamId") ? j.at("teamId").get_to(client.teamId) : client.teamId = "default";
    j.contains("online") ? j.at("online").get_to(client.online) : client.online = false;
    j.contains("seed") ? j.at("seed").get_to(client.seed) : client.seed = 0;
    j.contains("isSaveLoaded") ? j.at("isSaveLoaded").get_to(client.isSaveLoaded) : client.isSaveLoaded = false;
    j.contains("isGameComplete") ? j.at("isGameComplete").get_to(client.isGameComplete) : client.isGameComplete = false;
    j.contains("sceneNum") ? j.at("sceneNum").get_to(client.sceneNum) : client.sceneNum = SCENE_ID_MAX;
    j.contains("entranceIndex") ? j.at("entranceIndex").get_to(client.entranceIndex) : client.entranceIndex = 0;
    j.contains("self") ? j.at("self").get_to(client.self) : client.self = false;
}

inline void to_json(json& j, const Inventory& inventory) {
    j = json{ { "items", inventory.items },
              { "ammo", inventory.ammo },
              { "equipment", inventory.equipment },
              { "upgrades", inventory.upgrades },
              { "questItems", inventory.questItems },
              { "dungeonItems", inventory.dungeonItems },
              { "dungeonKeys", inventory.dungeonKeys },
              { "defenseHearts", inventory.defenseHearts },
              { "gsTokens", inventory.gsTokens } };
}

inline void from_json(const json& j, Inventory& inventory) {
    j.at("items").get_to(inventory.items);
    j.at("ammo").get_to(inventory.ammo);
    j.at("equipment").get_to(inventory.equipment);
    j.at("upgrades").get_to(inventory.upgrades);
    j.at("questItems").get_to(inventory.questItems);
    j.at("dungeonItems").get_to(inventory.dungeonItems);
    j.at("dungeonKeys").get_to(inventory.dungeonKeys);
    j.at("defenseHearts").get_to(inventory.defenseHearts);
    j.at("gsTokens").get_to(inventory.gsTokens);
}

inline void to_json(json& j, const SohStats& sohStats) {
    j = json{
        { "entrancesDiscovered", sohStats.entrancesDiscovered },
        { "fileCreatedAt", sohStats.fileCreatedAt },
    };
}

inline void from_json(const json& j, SohStats& sohStats) {
    j.at("entrancesDiscovered").get_to(sohStats.entrancesDiscovered);
    j.at("fileCreatedAt").get_to(sohStats.fileCreatedAt);
}

inline void to_json(json& j, const ShipRandomizerSaveContextData& shipRandomizerSaveContextData) {
    j = json{
        { "triforcePiecesCollected", shipRandomizerSaveContextData.triforcePiecesCollected },
    };
}

inline void from_json(const json& j, ShipRandomizerSaveContextData& shipRandomizerSaveContextData) {
    j.at("triforcePiecesCollected").get_to(shipRandomizerSaveContextData.triforcePiecesCollected);
}

inline void to_json(json& j, const ShipQuestSpecificSaveContextData& shipQuestSpecificSaveContextData) {
    j = json{
        { "randomizer", shipQuestSpecificSaveContextData.randomizer },
    };
}

inline void from_json(const json& j, ShipQuestSpecificSaveContextData& shipQuestSpecificSaveContextData) {
    j.at("randomizer").get_to(shipQuestSpecificSaveContextData.randomizer);
}

inline void to_json(json& j, const ShipQuestSaveContextData& shipQuestSaveContextData) {
    j = json{
        { "id", shipQuestSaveContextData.id },
        { "data", shipQuestSaveContextData.data },
    };
}

inline void from_json(const json& j, ShipQuestSaveContextData& shipQuestSaveContextData) {
    j.at("id").get_to(shipQuestSaveContextData.id);
    j.at("data").get_to(shipQuestSaveContextData.data);
}

inline void to_json(json& j, const ShipSaveContextData& shipSaveContextData) {
    j = json{
        { "stats", shipSaveContextData.stats },
        { "quest", shipSaveContextData.quest },
        { "randomizerInf", shipSaveContextData.randomizerInf },
    };
}

inline void from_json(const json& j, ShipSaveContextData& shipSaveContextData) {
    j.at("stats").get_to(shipSaveContextData.stats);
    j.at("quest").get_to(shipSaveContextData.quest);
    j.at("randomizerInf").get_to(shipSaveContextData.randomizerInf);
}

inline void to_json(json& j, const SaveContext& saveContext) {
    std::vector<u32> sceneFlagsArray;
    for (const auto& sceneFlags : saveContext.sceneFlags) {
        sceneFlagsArray.push_back(sceneFlags.chest);
        sceneFlagsArray.push_back(sceneFlags.swch);
        sceneFlagsArray.push_back(sceneFlags.clear);
        sceneFlagsArray.push_back(sceneFlags.collect);
    }

    j = json{
        { "healthCapacity", saveContext.healthCapacity },
        { "magicLevel", saveContext.magicLevel },
        { "magicCapacity", saveContext.magicCapacity },
        { "isMagicAcquired", saveContext.isMagicAcquired },
        { "isDoubleMagicAcquired", saveContext.isDoubleMagicAcquired },
        { "isDoubleDefenseAcquired", saveContext.isDoubleDefenseAcquired },
        { "bgsFlag", saveContext.bgsFlag },
        { "swordHealth", saveContext.swordHealth },
        { "sceneFlags", sceneFlagsArray },
        { "eventChkInf", saveContext.eventChkInf },
        { "itemGetInf", saveContext.itemGetInf },
        { "infTable", saveContext.infTable },
        { "gsFlags", saveContext.gsFlags },
        { "inventory", saveContext.inventory },
        { "ship", saveContext.ship },
    };
}

inline void from_json(const json& j, SaveContext& saveContext) {
    j.at("healthCapacity").get_to(saveContext.healthCapacity);
    j.at("magicLevel").get_to(saveContext.magicLevel);
    j.at("magicCapacity").get_to(saveContext.magicCapacity);
    j.at("isMagicAcquired").get_to(saveContext.isMagicAcquired);
    j.at("isDoubleMagicAcquired").get_to(saveContext.isDoubleMagicAcquired);
    j.at("isDoubleDefenseAcquired").get_to(saveContext.isDoubleDefenseAcquired);
    j.at("bgsFlag").get_to(saveContext.bgsFlag);
    j.at("swordHealth").get_to(saveContext.swordHealth);
    std::vector<u32> sceneFlagsArray;
    j.at("sceneFlags").get_to(sceneFlagsArray);
    for (int i = 0; i < 124; i++) {
        saveContext.sceneFlags[i].chest = sceneFlagsArray[i * 4];
        saveContext.sceneFlags[i].swch = sceneFlagsArray[i * 4 + 1];
        saveContext.sceneFlags[i].clear = sceneFlagsArray[i * 4 + 2];
        saveContext.sceneFlags[i].collect = sceneFlagsArray[i * 4 + 3];
    }
    j.at("eventChkInf").get_to(saveContext.eventChkInf);
    j.at("itemGetInf").get_to(saveContext.itemGetInf);
    j.at("infTable").get_to(saveContext.infTable);
    j.at("gsFlags").get_to(saveContext.gsFlags);
    j.at("inventory").get_to(saveContext.inventory);
    j.at("ship").get_to(saveContext.ship);
}

#endif // __cplusplus
#endif // NETWORK_ANCHOR_JSON_CONVERSIONS_H

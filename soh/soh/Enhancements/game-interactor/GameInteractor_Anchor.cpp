#ifdef ENABLE_REMOTE_CONTROL

#include "GameInteractor_Anchor.h"
#include <libultraship/libultraship.h>
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include <soh/Enhancements/randomizer/adult_trade_shuffle.h>
#include <soh/Enhancements/randomizer/randomizer_check_tracker.h>
#include <soh/util.h>
#include <nlohmann/json.hpp>

extern "C" {
#include <variables.h>
#include "macros.h"
#include "z64scene.h"
#include "z64actor.h"
#include "functions.h"
extern "C" s16 gEnPartnerId;
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

using json = nlohmann::json;

void from_json(const json& j, Color_RGB8& color) {
    j.at("r").get_to(color.r);
    j.at("g").get_to(color.g);
    j.at("b").get_to(color.b);
}

void to_json(json& j, const Color_RGB8& color) {
    j = json{
        {"r", color.r},
        {"g", color.g},
        {"b", color.b}
    };
}

void to_json(json& j, const Vec3f& vec) {
    j = json{
        {"x", vec.x},
        {"y", vec.y},
        {"z", vec.z}
    };
}

void to_json(json& j, const Vec3s& vec) {
    j = json{
        {"x", vec.x},
        {"y", vec.y},
        {"z", vec.z}
    };
}

void from_json(const json& j, Vec3f& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

void from_json(const json& j, Vec3s& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

void to_json(json& j, const PosRot& posRot) {
    j = json{
        {"pos", posRot.pos},
        {"rot", posRot.rot}
    };
}

void from_json(const json& j, PosRot& posRot) {
    j.at("pos").get_to(posRot.pos);
    j.at("rot").get_to(posRot.rot);
}

void from_json(const json& j, AnchorClient& client) {
    j.contains("clientId") ? j.at("clientId").get_to(client.clientId) : client.clientId = 0;
    j.contains("clientVersion") ? j.at("clientVersion").get_to(client.clientVersion) : client.clientVersion = "???";
    j.contains("name") ? j.at("name").get_to(client.name) : client.name = "???";
    j.contains("color") ? j.at("color").get_to(client.color) : client.color = {255, 255, 255};
    j.contains("seed") ? j.at("seed").get_to(client.seed) : client.seed = "???";
    j.contains("scene") ? j.at("scene").get_to(client.scene) : client.scene = SCENE_ID_MAX;
    j.contains("posRot") ? j.at("posRot").get_to(client.posRot) : client.posRot = { -9999, -9999, -9999, 0, 0, 0 };
}

void to_json(json& j, const SavedSceneFlags& flags) {
    j = json{
        {"chest", flags.chest},
        {"swch", flags.swch},
        {"clear", flags.clear},
        {"collect", flags.collect},
    };
}

void from_json(const json& j, SavedSceneFlags& flags) {
    j.at("chest").get_to(flags.chest);
    j.at("swch").get_to(flags.swch);
    j.at("clear").get_to(flags.clear);
    j.at("collect").get_to(flags.collect);
}

void to_json(json& j, const Inventory& inventory) {
    j = json{
        {"items", inventory.items},
        {"ammo", inventory.ammo},
        {"equipment", inventory.equipment},
        {"upgrades", inventory.upgrades},
        {"questItems", inventory.questItems},
        {"dungeonItems", inventory.dungeonItems},
        {"dungeonKeys", inventory.dungeonKeys},
        {"defenseHearts", inventory.defenseHearts},
        {"gsTokens", inventory.gsTokens}
    };
}

void from_json(const json& j, Inventory& inventory) {
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

void to_json(json& j, const SohStats& sohStats) {
    j = json{
        {"fileCreatedAt", sohStats.fileCreatedAt},
    };
}

void from_json(const json& j, SohStats& sohStats) {
    j.contains("fileCreatedAt") ? j.at("fileCreatedAt").get_to(sohStats.fileCreatedAt) : gSaveContext.sohStats.fileCreatedAt;
}

void to_json(json& j, const SaveContext& saveContext) {
    j = json{
        {"healthCapacity", saveContext.healthCapacity},
        {"magicLevel", saveContext.magicLevel},
        {"magicCapacity", saveContext.magicCapacity},
        {"isMagicAcquired", saveContext.isMagicAcquired},
        {"isDoubleMagicAcquired", saveContext.isDoubleMagicAcquired},
        {"isDoubleDefenseAcquired", saveContext.isDoubleDefenseAcquired},
        {"bgsFlag", saveContext.bgsFlag},
        {"swordHealth", saveContext.swordHealth},
        {"sceneFlags", saveContext.sceneFlags},
        {"eventChkInf", saveContext.eventChkInf},
        {"itemGetInf", saveContext.itemGetInf},
        {"infTable", saveContext.infTable},
        {"randomizerInf", saveContext.randomizerInf},
        {"gsFlags", saveContext.gsFlags},
        {"inventory", saveContext.inventory},
        {"sohStats", saveContext.sohStats},
        {"adultTradeItems", saveContext.adultTradeItems},
        {"checkTrackerData", saveContext.checkTrackerData},
        {"triforcePiecesCollected", saveContext.triforcePiecesCollected},
    };
}

void from_json(const json& j, SaveContext& saveContext) {
    j.at("healthCapacity").get_to(saveContext.healthCapacity);
    j.at("magicLevel").get_to(saveContext.magicLevel);
    j.at("magicCapacity").get_to(saveContext.magicCapacity);
    j.at("isMagicAcquired").get_to(saveContext.isMagicAcquired);
    j.at("isDoubleMagicAcquired").get_to(saveContext.isDoubleMagicAcquired);
    j.at("isDoubleDefenseAcquired").get_to(saveContext.isDoubleDefenseAcquired);
    j.at("bgsFlag").get_to(saveContext.bgsFlag);
    j.at("swordHealth").get_to(saveContext.swordHealth);
    j.at("sceneFlags").get_to(saveContext.sceneFlags);
    j.at("eventChkInf").get_to(saveContext.eventChkInf);
    j.at("itemGetInf").get_to(saveContext.itemGetInf);
    j.at("infTable").get_to(saveContext.infTable);
    j.at("randomizerInf").get_to(saveContext.randomizerInf);
    j.at("gsFlags").get_to(saveContext.gsFlags);
    j.at("inventory").get_to(saveContext.inventory);
    j.at("sohStats").get_to(saveContext.sohStats);
    j.at("adultTradeItems").get_to(saveContext.adultTradeItems);
    j.at("checkTrackerData").get_to(saveContext.checkTrackerData);
    j.contains("triforcePiecesCollected") ? j.at("triforcePiecesCollected").get_to(saveContext.triforcePiecesCollected) : gSaveContext.triforcePiecesCollected;
}

std::map<uint32_t, AnchorClient> GameInteractorAnchor::AnchorClients = {};
std::vector<uint32_t> GameInteractorAnchor::FairyIndexToClientId = {};
std::string GameInteractorAnchor::clientVersion = "Anchor + Triforce Hunt 2";
std::string GameInteractorAnchor::seed = "00000";
std::vector<std::pair<uint16_t, int16_t>> receivedItems = {};
std::vector<AnchorMessage> anchorMessages = {};
uint32_t notificationId = 0;

void Anchor_DisplayMessage(AnchorMessage message = {}) {
    message.id = notificationId++;
    anchorMessages.push_back(message);
}

void Anchor_SendClientData() {
    GameInteractorAnchor::seed = std::accumulate(std::begin(gSaveContext.seedIcons), std::end(gSaveContext.seedIcons), std::string(), [](std::string a, int b) {
        return a + std::to_string(b);
    });

    nlohmann::json payload;
    payload["data"]["name"] = CVarGetString("gRemote.AnchorName", "");
    payload["data"]["color"] = CVarGetColor24("gRemote.AnchorColor", { 100, 255, 100 });
    payload["data"]["clientVersion"] = GameInteractorAnchor::clientVersion;
    payload["data"]["seed"] = GameInteractorAnchor::seed;
    payload["type"] = "UPDATE_CLIENT_DATA";
    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void GameInteractorAnchor::Enable() {
    if (isEnabled) {
        return;
    }

    isEnabled = true;
    GameInteractor::Instance->EnableRemoteInteractor();
    GameInteractor::Instance->RegisterRemoteJsonHandler([&](nlohmann::json payload) {
        HandleRemoteJson(payload);
    });
    GameInteractor::Instance->RegisterRemoteConnectedHandler([&]() {
        Anchor_DisplayMessage({ .message = "Connected to Anchor" });
        Anchor_SendClientData();
    });
    GameInteractor::Instance->RegisterRemoteDisconnectedHandler([&]() {
        Anchor_DisplayMessage({ .message = "Disconnected from Anchor" });
    });
}

void GameInteractorAnchor::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    GameInteractor::Instance->DisableRemoteInteractor();

    GameInteractorAnchor::AnchorClients.clear();
    if (GameInteractor::IsSaveLoaded()) {
        Anchor_SpawnClientFairies();
    }
}

void GameInteractorAnchor::TransmitJsonToRemote(nlohmann::json payload) {
    payload["roomId"] = CVarGetString("gRemote.AnchorRoomId", "");
    if (!payload.contains("quiet")) {
        SPDLOG_INFO("Sending payload:\n{}", payload.dump());
    }
    GameInteractor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ParseSaveStateFromRemote(nlohmann::json payload);
void Anchor_PushSaveStateToRemote();

void GameInteractorAnchor::HandleRemoteJson(nlohmann::json payload) {
    if (!payload.contains("type")) {
        return;
    }

    if (!payload.contains("quiet")) {
        SPDLOG_INFO("Received payload:\n{}", payload.dump());
    }

    if (payload["type"] == "GIVE_ITEM") {
        auto effect = new GameInteractionEffect::GiveItem();
        effect->parameters[0] = payload["modId"].get<uint16_t>();
        effect->parameters[1] = payload["getItemId"].get<int16_t>();
        CVarSetInteger("gFromGI", 1);
        CheckTracker::AddItemReceived(ItemTableManager::Instance->RetrieveItemEntry(effect->parameters[0], effect->parameters[1]));
        receivedItems.push_back({ payload["modId"].get<uint16_t>(), payload["getItemId"].get<int16_t>() });
        if (effect->Apply() == Possible) {
            GetItemEntry getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(effect->parameters[0], effect->parameters[1]);

            AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
            if (getItemEntry.getItemCategory != ITEM_CATEGORY_JUNK) {
                if (getItemEntry.modIndex == MOD_NONE) {
                    Anchor_DisplayMessage({
                        .itemIcon = SohUtils::GetIconNameFromItemID(getItemEntry.itemId),
                        .prefix = SohUtils::GetItemName(getItemEntry.itemId),
                        .message = "from",
                        .suffix = anchorClient.name
                    });
                } else if (getItemEntry.modIndex == MOD_RANDOMIZER) {
                    Anchor_DisplayMessage({
                        .itemIcon = SohUtils::GetIconNameFromItemID(SohUtils::GetItemIdIconFromRandomizerGet(getItemEntry.getItemId)),
                        .prefix = SohUtils::GetRandomizerItemName(getItemEntry.getItemId),
                        .message = "from",
                        .suffix = anchorClient.name
                    });
                }
            }
        }
        CVarClear("gFromGI");
    }
    if (payload["type"] == "SET_SCENE_FLAG") {
        auto effect = new GameInteractionEffect::SetSceneFlag();
        effect->parameters[0] = payload["sceneNum"].get<int16_t>();
        effect->parameters[1] = payload["flagType"].get<int16_t>();
        effect->parameters[2] = payload["flag"].get<int16_t>();
        effect->Apply();
    }
    if (payload["type"] == "SET_FLAG") {
        auto effect = new GameInteractionEffect::SetFlag();
        effect->parameters[0] = payload["flagType"].get<int16_t>();
        effect->parameters[1] = payload["flag"].get<int16_t>();
        effect->Apply();

        // If mweep flag replace ruto's letter
        if (
            payload["flagType"].get<int16_t>() == FLAG_EVENT_CHECK_INF && 
            payload["flag"].get<int16_t>() == EVENTCHKINF_KING_ZORA_MOVED &&
            Inventory_HasSpecificBottle(ITEM_LETTER_RUTO)
        ) {
            Inventory_ReplaceItem(gPlayState, ITEM_LETTER_RUTO, ITEM_BOTTLE);
        }
    }
    if (payload["type"] == "UNSET_SCENE_FLAG") {
        auto effect = new GameInteractionEffect::UnsetSceneFlag();
        effect->parameters[0] = payload["sceneNum"].get<int16_t>();
        effect->parameters[1] = payload["flagType"].get<int16_t>();
        effect->parameters[2] = payload["flag"].get<int16_t>();
        effect->Apply();
    }
    if (payload["type"] == "UNSET_FLAG") {
        auto effect = new GameInteractionEffect::UnsetFlag();
        effect->parameters[0] = payload["flagType"].get<int16_t>();
        effect->parameters[1] = payload["flag"].get<int16_t>();
        effect->Apply();
    }
    if (payload["type"] == "CLIENT_UPDATE") {
        uint32_t clientId = payload["clientId"].get<uint32_t>();

        if (GameInteractorAnchor::AnchorClients.contains(clientId)) {
            GameInteractorAnchor::AnchorClients[clientId].scene = payload["sceneNum"].get<int16_t>();
            GameInteractorAnchor::AnchorClients[clientId].posRot = payload["posRot"].get<PosRot>();
        }
    }
    if (payload["type"] == "PUSH_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
        Anchor_ParseSaveStateFromRemote(payload);
    }
    if (payload["type"] == "REQUEST_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
        Anchor_PushSaveStateToRemote();
    }
    if (payload["type"] == "ALL_CLIENT_DATA") {
        std::vector<AnchorClient> newClients = payload["clients"].get<std::vector<AnchorClient>>();

        // add new clients
        for (auto& client : newClients) {
            if (!GameInteractorAnchor::AnchorClients.contains(client.clientId)) {
                GameInteractorAnchor::AnchorClients[client.clientId] = {
                    client.clientId,
                    client.clientVersion,
                    client.name,
                    client.color,
                    client.seed,
                    SCENE_ID_MAX,
                    { -9999, -9999, -9999, 0, 0, 0 }
                };
                Anchor_DisplayMessage({
                    .prefix = client.name,
                    .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                    .message = "connected"
                });
            }
        }

        // remove clients that are no longer in the list
        std::vector<uint32_t> clientsToRemove;
        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (std::find_if(newClients.begin(), newClients.end(), [clientId = clientId](AnchorClient& c) { return c.clientId == clientId; }) == newClients.end()) {
                clientsToRemove.push_back(clientId);
            }
        }
        for (auto& clientId : clientsToRemove) {
            Anchor_DisplayMessage({
                    .prefix = GameInteractorAnchor::AnchorClients[clientId].name,
                    .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                    .message = "connected"
                });
            GameInteractorAnchor::AnchorClients.erase(clientId);
        }

        if (GameInteractor::IsSaveLoaded()) {
            Anchor_SpawnClientFairies();
        }
    }
    if (payload["type"] == "UPDATE_CLIENT_DATA") {
        uint32_t clientId = payload["clientId"].get<uint32_t>();
        if (GameInteractorAnchor::AnchorClients.contains(clientId)) {
            AnchorClient client = payload["data"].get<AnchorClient>();
            GameInteractorAnchor::AnchorClients[clientId].clientVersion = client.clientVersion;
            GameInteractorAnchor::AnchorClients[clientId].name = client.name;
            GameInteractorAnchor::AnchorClients[clientId].color = client.color;
            GameInteractorAnchor::AnchorClients[clientId].seed = client.seed;
        }
    }
    if (payload["type"] == "UPDATE_CHECK_DATA" && GameInteractor::IsSaveLoaded()) {
        auto check = payload["locationIndex"].get<uint32_t>();
        auto data = payload["checkData"].get<RandomizerCheckTrackerData>();
        CheckTracker::UpdateCheck(check, data);
    }
    if (payload["type"] == "UPDATE_BEANS_BOUGHT" && GameInteractor::IsSaveLoaded()) {
        BEANS_BOUGHT = payload["amount"].get<uint8_t>();
    }
    if (payload["type"] == "CONSUME_ADULT_TRADE_ITEM" && GameInteractor::IsSaveLoaded()) {
        uint8_t itemId = payload["itemId"].get<uint8_t>();
        gSaveContext.adultTradeItems &= ~ADULT_TRADE_FLAG(itemId);
	    Inventory_ReplaceItem(gPlayState, itemId, Randomizer_GetNextAdultTradeItem());
    }
    if (payload["type"] == "UPDATE_KEY_COUNT" && GameInteractor::IsSaveLoaded()) {
        gSaveContext.inventory.dungeonKeys[payload["sceneNum"].get<int16_t>()] = payload["amount"].get<int8_t>();
    }
    if (payload["type"] == "SERVER_MESSAGE") {
        Anchor_DisplayMessage({
            .prefix = "Server:",
            .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
            .message = payload["message"].get<std::string>(),
        });
    }
    if (payload["type"] == "DISABLE_ANCHOR") {
        GameInteractor::Instance->isRemoteInteractorEnabled = false;
        GameInteractorAnchor::Instance->isEnabled = false;
    }
}

void Anchor_PushSaveStateToRemote() {
    json payload = gSaveContext;
    payload["type"] = "PUSH_SAVE_STATE";
    // manually update current scene flags
    payload["sceneFlags"][gPlayState->sceneNum]["chest"] = gPlayState->actorCtx.flags.chest;
    payload["sceneFlags"][gPlayState->sceneNum]["swch"] = gPlayState->actorCtx.flags.swch;
    payload["sceneFlags"][gPlayState->sceneNum]["clear"] = gPlayState->actorCtx.flags.clear;
    payload["sceneFlags"][gPlayState->sceneNum]["collect"] = gPlayState->actorCtx.flags.collect;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_RequestSaveStateFromRemote() {
    nlohmann::json payload;
    payload["type"] = "REQUEST_SAVE_STATE";

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ParseSaveStateFromRemote(nlohmann::json payload) {
    SaveContext loadedData = payload.get<SaveContext>();

    gSaveContext.healthCapacity = loadedData.healthCapacity;
    gSaveContext.magicLevel = loadedData.magicLevel;
    gSaveContext.magicCapacity = gSaveContext.magic = loadedData.magicCapacity;
    gSaveContext.isMagicAcquired = loadedData.isMagicAcquired;
    gSaveContext.isDoubleMagicAcquired = loadedData.isDoubleMagicAcquired;
    gSaveContext.isDoubleDefenseAcquired = loadedData.isDoubleDefenseAcquired;
    gSaveContext.bgsFlag = loadedData.bgsFlag;
    gSaveContext.swordHealth = loadedData.swordHealth;
    // TODO: Packet to live update this
    gSaveContext.adultTradeItems = loadedData.adultTradeItems;

    for (int i = 0; i < 124; i++) {
        gSaveContext.sceneFlags[i] = loadedData.sceneFlags[i];
        if (gPlayState->sceneNum == i) {
            gPlayState->actorCtx.flags.chest = loadedData.sceneFlags[i].chest;
            gPlayState->actorCtx.flags.swch = loadedData.sceneFlags[i].swch;
            gPlayState->actorCtx.flags.clear = loadedData.sceneFlags[i].clear;
            gPlayState->actorCtx.flags.collect = loadedData.sceneFlags[i].collect;
        }
    }

    for (int i = 0; i < 14; i++) {
        gSaveContext.eventChkInf[i] = loadedData.eventChkInf[i];
    }

    for (int i = 0; i < 4; i++) {
        gSaveContext.itemGetInf[i] = loadedData.itemGetInf[i];
    }

    // Skip last row of infTable, don't want to sync swordless flag
    for (int i = 0; i < 29; i++) {
        gSaveContext.infTable[i] = loadedData.infTable[i];
    }

    for (int i = 0; i < 9; i++) {
        gSaveContext.randomizerInf[i] = loadedData.randomizerInf[i];
    }

    for (int i = 0; i < 6; i++) {
        gSaveContext.gsFlags[i] = loadedData.gsFlags[i];
    }

    CheckTracker::ClearAreaTotals();
    for (int i = 2; i < RC_MAX; i++) {
        if (loadedData.checkTrackerData[i].status == RCSHOW_SAVED || loadedData.checkTrackerData[i].skipped) {
            CheckTracker::AddToChecksCollected(static_cast<RandomizerCheck>(i));
        }
        gSaveContext.checkTrackerData[i].status = loadedData.checkTrackerData[i].status;
        gSaveContext.checkTrackerData[i].skipped = loadedData.checkTrackerData[i].skipped;
        gSaveContext.checkTrackerData[i].price = loadedData.checkTrackerData[i].price;
        gSaveContext.checkTrackerData[i].hintItem = loadedData.checkTrackerData[i].hintItem;
    }

    CheckTracker::UpdateAllOrdering();

    // Restore master sword state
    u8 hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_SWORD, 1);
    if (hasMasterSword) {
        loadedData.inventory.equipment |= 0x2;
    } else {
        loadedData.inventory.equipment &= ~0x2;
    }

    // TODO: Packet to live update ruto's letter replacement
    // Restore bottle contents (unless it's ruto's letter)
    for (int i = 0; i < 4; i++) {
        if (gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_NONE && gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_LETTER_RUTO) {
            loadedData.inventory.items[SLOT_BOTTLE_1 + i] = gSaveContext.inventory.items[SLOT_BOTTLE_1 + i];
        }
    }

    // Restore ammo if it's non-zero
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.inventory.ammo); i++) {
        if (gSaveContext.inventory.ammo[i] != 0) {
            loadedData.inventory.ammo[i] = gSaveContext.inventory.ammo[i];
        }
    }

    gSaveContext.inventory = loadedData.inventory;
    Anchor_DisplayMessage({ .message = "State loaded from remote!" });
};

uint8_t Anchor_GetClientScene(uint32_t fairyIndex) {
    uint32_t clientId = GameInteractorAnchor::FairyIndexToClientId[fairyIndex];
    if (GameInteractorAnchor::AnchorClients.find(clientId) == GameInteractorAnchor::AnchorClients.end()) {
        return SCENE_ID_MAX;
    }

    return GameInteractorAnchor::AnchorClients[clientId].scene;
}

PosRot Anchor_GetClientPosition(uint32_t fairyIndex) {
    uint32_t clientId = GameInteractorAnchor::FairyIndexToClientId[fairyIndex];
    if (GameInteractorAnchor::AnchorClients.find(clientId) == GameInteractorAnchor::AnchorClients.end()) {
        return {-9999.0, -9999.0, -9999.0, 0, 0, 0};
    }

    return GameInteractorAnchor::AnchorClients[clientId].posRot;
}

Color_RGB8 Anchor_GetClientColor(uint32_t fairyIndex) {
    uint32_t clientId = GameInteractorAnchor::FairyIndexToClientId[fairyIndex];
    if (GameInteractorAnchor::AnchorClients.find(clientId) == GameInteractorAnchor::AnchorClients.end()) {
        return {100, 255, 100};
    }

    return GameInteractorAnchor::AnchorClients[clientId].color;
}

void Anchor_SpawnClientFairies() {
    if (gPlayState == NULL) return;
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    while (actor != NULL) {
        if (gEnPartnerId == actor->id) {
            Actor_Kill(actor);
        }
        actor = actor->next;
    }

    GameInteractorAnchor::FairyIndexToClientId.clear();

    uint32_t i = 0;
    for (auto [clientId, client] : GameInteractorAnchor::AnchorClients) {
        GameInteractorAnchor::FairyIndexToClientId.push_back(clientId);
        Actor_Spawn(&gPlayState->actorCtx, gPlayState, gEnPartnerId, -9999.0, -9999.0, -9999.0, 0, 0, 0, 3 + i, false);
        i++;
    }
}

void Anchor_RegisterHooks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || gSaveContext.fileNum > 2) return;

        Anchor_SendClientData();
        Anchor_RequestSaveStateFromRemote();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        if (itemEntry.modIndex == MOD_NONE && (itemEntry.itemId == ITEM_KEY_SMALL || itemEntry.itemId == ITEM_KEY_BOSS || itemEntry.itemId == ITEM_SWORD_MASTER)) {
            return;
        }

        // If the item exists in receivedItems, remove it from the list and don't emit the packet
        auto it = std::find_if(receivedItems.begin(), receivedItems.end(), [itemEntry](std::pair<uint16_t, int16_t> pair) {
            return pair.first == itemEntry.tableId && pair.second == itemEntry.getItemId;
        });
        if (it != receivedItems.end()) {
            receivedItems.erase(it);
            return;
        }

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

        nlohmann::json payload;

        payload["type"] = "GIVE_ITEM";
        payload["modId"] = itemEntry.tableId;
        payload["getItemId"] = itemEntry.getItemId;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>([](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "SET_SCENE_FLAG";
        payload["sceneNum"] = sceneNum;
        payload["flagType"] = flagType;
        payload["flag"] = flag;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([](int16_t flagType, int16_t flag) {
        if (flagType == FLAG_INF_TABLE && flag == INFTABLE_SWORDLESS) {
            return;
        }

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "SET_FLAG";
        payload["flagType"] = flagType;
        payload["flag"] = flag;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>([](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "UNSET_SCENE_FLAG";
        payload["sceneNum"] = sceneNum;
        payload["flagType"] = flagType;
        payload["flag"] = flag;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([](int16_t flagType, int16_t flag) {
        if (flagType == FLAG_INF_TABLE && flag == INFTABLE_SWORDLESS) {
            return;
        }

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "UNSET_FLAG";
        payload["flagType"] = flagType;
        payload["flag"] = flag;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        static uint32_t lastPlayerCount = 0;
        uint32_t currentPlayerCount =  GameInteractorAnchor::AnchorClients.size();
        if (!GameInteractor::Instance->isRemoteInteractorConnected || gPlayState == NULL || !GameInteractor::Instance->IsSaveLoaded()) {
            lastPlayerCount = currentPlayerCount;
            return;
        }
        Player* player = GET_PLAYER(gPlayState);
        nlohmann::json payload;
        float currentPosition = player->actor.world.pos.x + player->actor.world.pos.y + player->actor.world.pos.z + player->actor.world.rot.y;
        static float lastPosition = 0.0f;

        if (currentPosition == lastPosition && currentPlayerCount == lastPlayerCount) return;

        payload["type"] = "CLIENT_UPDATE";
        payload["sceneNum"] = gPlayState->sceneNum;
        payload["posRot"] = player->actor.world;
        payload["quiet"] = true;

        lastPosition = currentPosition;
        lastPlayerCount = currentPlayerCount;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
}

void Anchor_UpdateCheckData(uint32_t locationIndex) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "UPDATE_CHECK_DATA";
    payload["locationIndex"] = locationIndex;
    payload["checkData"] = gSaveContext.checkTrackerData[locationIndex];
    if (gSaveContext.checkTrackerData[locationIndex].status == RCSHOW_COLLECTED) {
        payload["checkData"]["status"] = RCSHOW_SAVED;
    }

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_UpdateBeansBought(uint8_t amount) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "UPDATE_BEANS_BOUGHT";
    payload["amount"] = amount;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ConsumeAdultTradeItem(uint8_t itemId) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "CONSUME_ADULT_TRADE_ITEM";
    payload["itemId"] = itemId;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_UpdateKeyCount(int16_t sceneNum, int8_t amount) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "UPDATE_KEY_COUNT";
    payload["sceneNum"] = sceneNum;
    payload["amount"] = amount;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void AnchorPlayerLocationWindow::DrawElement() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin("AnchorPlayerLocationWindow", &mIsVisible, 
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoScrollbar
    );

    ImGui::Text("%s", CVarGetString("gRemote.AnchorName", ""));
    if (gPlayState != NULL) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s", SohUtils::GetSceneName(gPlayState->sceneNum).c_str());
    }
    for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
        ImGui::Text("%s", client.name.c_str());
        if (client.scene < SCENE_ID_MAX) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s", SohUtils::GetSceneName(client.scene).c_str());
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void AnchorLogWindow::DrawElement() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin("AnchorLogWindow", &mIsVisible,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoScrollbar
    );

    // Options to stack notifications on top or bottom, and left or right
    if (ImGui::Button(CVarGetInteger("gRemote.AnchorLogWindowX", 0) ? ICON_FA_CHEVRON_RIGHT : ICON_FA_CHEVRON_LEFT, ImVec2(20, 20))) {
        CVarSetInteger("gRemote.AnchorLogWindowX", !CVarGetInteger("gRemote.AnchorLogWindowX", 0));
    }
    ImGui::SameLine();
    if (ImGui::Button(CVarGetInteger("gRemote.AnchorLogWindowY", 0) ? ICON_FA_CHEVRON_DOWN : ICON_FA_CHEVRON_UP, ImVec2(20, 20))) {
        CVarSetInteger("gRemote.AnchorLogWindowY", !CVarGetInteger("gRemote.AnchorLogWindowY", 0));
    }

    // Store x/y position of window
    ImVec2 anchorPos = ImGui::GetWindowPos();
    ImVec2 anchorSize = ImGui::GetWindowSize();

    for (int index = 0; index < anchorMessages.size(); ++index) {
        auto& message = anchorMessages[index];
        int inverseIndex = -ABS(index - (anchorMessages.size() - 1));
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        if (message.remainingTime < 4.0f) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (message.remainingTime - 1) / 3.0f);
        } else {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        }
        ImGui::Begin(("anchorLog" + std::to_string(message.id)).c_str(), nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar
        );
        ImGui::SetWindowPos(ImVec2(
            // X position should take into account both the alignment and the width of the message window
            anchorPos.x + (CVarGetInteger("gRemote.AnchorLogWindowX", 0) ? 0 : -(ImGui::GetWindowSize().x - anchorSize.x)),
            // Y Position should take into account the stack direction and index of the message
            anchorPos.y + (CVarGetInteger("gRemote.AnchorLogWindowY", 0) ? (anchorSize.y + (ImGui::GetWindowSize().y * inverseIndex)) : -(ImGui::GetWindowSize().y * (inverseIndex + 1)))
        ));
        ImGui::SetWindowFontScale(1.8f);

        if (message.itemIcon != nullptr) {
            ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(message.itemIcon), ImVec2(24, 24));
            ImGui::SameLine();
        }
        if (!message.prefix.empty()) {
            ImGui::TextColored(message.prefixColor, "%s", message.prefix.c_str());
            ImGui::SameLine();
        }
        ImGui::TextColored(message.messageColor, "%s", message.message.c_str());
        if (!message.suffix.empty()) {
            ImGui::SameLine();
            ImGui::TextColored(message.suffixColor, "%s", message.suffix.c_str());
        }
        ImGui::End();
        ImGui::PopStyleVar();

        // decrement remainingTime
        message.remainingTime -= ImGui::GetIO().DeltaTime;

        // remove message if it has expired
        if (message.remainingTime <= 0) {
            anchorMessages.erase(anchorMessages.begin() + index);
            --index;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

#endif

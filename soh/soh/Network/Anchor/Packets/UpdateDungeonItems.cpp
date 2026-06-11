#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include <spdlog/spdlog.h>

extern "C" {
#include "include/macros.h"
}

/**
 * UPDATE_DUNGEON_ITEMS
 *
 * This is for 2 things, first is updating the dungeon items in vanilla saves, and second is
 * for ensuring the amount of keys used is synced as players are using them.
 */

void Anchor::SendPacket_UpdateDungeonItems() {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = UPDATE_DUNGEON_ITEMS;
    payload["targetTeamId"] = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    payload["addToQueue"] = true;
    payload["mapIndex"] = gSaveContext.mapIndex;
    payload["dungeonItems"] = gSaveContext.inventory.dungeonItems[gSaveContext.mapIndex];
    payload["dungeonKeys"] = gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex];

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_UpdateDungeonItems(nlohmann::json payload) {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    u16 mapIndex = payload.at("mapIndex").get<u16>();
    // dungeonKeys is shorter than dungeonItems (19 vs 20), so bound by the smaller of the two.
    if (mapIndex >= ARRAY_COUNT(gSaveContext.inventory.dungeonItems) ||
        mapIndex >= ARRAY_COUNT(gSaveContext.inventory.dungeonKeys)) {
        SPDLOG_ERROR("[Anchor] UPDATE_DUNGEON_ITEMS: mapIndex {} out of range", mapIndex);
        return;
    }
    gSaveContext.inventory.dungeonItems[mapIndex] = payload.at("dungeonItems").get<u8>();
    gSaveContext.inventory.dungeonKeys[mapIndex] = payload.at("dungeonKeys").get<s8>();
}

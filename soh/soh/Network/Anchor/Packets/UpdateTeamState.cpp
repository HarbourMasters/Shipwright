#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include "soh/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/OTRGlobals.h"
#include "soh/Notification/Notification.h"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * UPDATE_TEAM_STATE
 *
 * Pushes the current save state to the server for other teammates to use.
 * 
 * Fires when the server passes on a REQUEST_TEAM_STATE packet, or when this client saves the game
 * 
 * When sending this packet we will assume that the team queue has been emptied for this client, so the queue
 * stored in the server will be cleared.
 * 
 * When receiving this packet, if there is items in the team queue, we will play them back in order.
 */

void Anchor::SendPacket_UpdateTeamState() {
    if (!IsSaveLoaded()) {
        return;
    }

    json payload;
    payload["type"] = UPDATE_TEAM_STATE;
    payload["targetTeamId"] = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");

    // Assume the team queue has been emptied, so clear it
    payload["queue"] = json::array();

    payload["state"] = gSaveContext;
    // manually update current scene flags
    payload["state"]["sceneFlags"][gPlayState->sceneNum * 4] = gPlayState->actorCtx.flags.chest;
    payload["state"]["sceneFlags"][gPlayState->sceneNum * 4 + 1] = gPlayState->actorCtx.flags.swch;
    payload["state"]["sceneFlags"][gPlayState->sceneNum * 4 + 2] = gPlayState->actorCtx.flags.clear;
    payload["state"]["sceneFlags"][gPlayState->sceneNum * 4 + 3] = gPlayState->actorCtx.flags.collect;

    // The commented out code below is an attempt at sending the entire randomizer seed over, in hopes that a player doesn't have to generate the seed themselves
    // Currently it doesn't work :)
    if (IS_RANDO) {
        auto randoContext = Rando::Context::GetInstance();

        payload["state"]["rando"] = json::object();
        payload["state"]["rando"]["itemLocations"] = json::array();
        for (int i = 0; i < RC_MAX; i++) {
            payload["state"]["rando"]["itemLocations"][i] = json::array();
            // payload["state"]["rando"]["itemLocations"][i]["rgID"] = randoContext->GetItemLocation(i)->GetPlacedRandomizerGet();
            payload["state"]["rando"]["itemLocations"][i][0] = randoContext->GetItemLocation(i)->GetCheckStatus();
            payload["state"]["rando"]["itemLocations"][i][1] = (u8)randoContext->GetItemLocation(i)->GetIsSkipped();

            // if (randoContext->GetItemLocation(i)->GetPlacedRandomizerGet() == RG_ICE_TRAP) {
            //     payload["state"]["rando"]["itemLocations"][i]["fakeRgID"] = randoContext->GetItemOverride(i).LooksLike();
            //     payload["state"]["rando"]["itemLocations"][i]["trickName"] = json::object();
            //     payload["state"]["rando"]["itemLocations"][i]["trickName"]["english"] = randoContext->GetItemOverride(i).GetTrickName().GetEnglish();
            //     payload["state"]["rando"]["itemLocations"][i]["trickName"]["french"] = randoContext->GetItemOverride(i).GetTrickName().GetFrench();
            // }
            // if (randoContext->GetItemLocation(i)->HasCustomPrice()) {
            //     payload["state"]["rando"]["itemLocations"][i]["price"] = randoContext->GetItemLocation(i)->GetPrice();
            // }
        }

        // auto entranceCtx = randoContext->GetEntranceShuffler();
        // for (int i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        //     payload["state"]["rando"]["entrances"][i] = json::object();
        //     payload["state"]["rando"]["entrances"][i]["type"] = entranceCtx->entranceOverrides[i].type;
        //     payload["state"]["rando"]["entrances"][i]["index"] = entranceCtx->entranceOverrides[i].index;
        //     payload["state"]["rando"]["entrances"][i]["destination"] = entranceCtx->entranceOverrides[i].destination;
        //     payload["state"]["rando"]["entrances"][i]["override"] = entranceCtx->entranceOverrides[i].override;
        //     payload["state"]["rando"]["entrances"][i]["overrideDestination"] = entranceCtx->entranceOverrides[i].overrideDestination;
        // }

        // payload["state"]["rando"]["seed"] = json::array();
        // for (int i = 0; i < randoContext->hashIconIndexes.size(); i++) {
        //     payload["state"]["rando"]["seed"][i] = randoContext->hashIconIndexes[i];
        // }
        // payload["state"]["rando"]["inputSeed"] = randoContext->GetSeedString();
        // payload["state"]["rando"]["finalSeed"] = randoContext->GetSeed();
        
        // payload["state"]["rando"]["randoSettings"] = json::array();
        // for (int i = 0; i < RSK_MAX; i++) {
        //     payload["state"]["rando"]["randoSettings"][i] = randoContext->GetOption((RandomizerSettingKey(i))).GetSelectedOptionIndex();
        // }

        // payload["state"]["rando"]["masterQuestDungeonCount"] = randoContext->GetDungeons()->CountMQ();
        // payload["state"]["rando"]["masterQuestDungeons"] = json::array();
        // for (int i = 0; i < randoContext->GetDungeons()->GetDungeonListSize(); i++) {
        //     payload["state"]["rando"]["masterQuestDungeons"][i] = randoContext->GetDungeon(i)->IsMQ();
        // }
        // for (int i = 0; i < randoContext->GetTrials()->GetTrialListSize(); i++) {
        //     payload["state"]["rando"]["requiredTrials"][i] = randoContext->GetTrial(i)->IsRequired();
        // }
    }

    SendJsonToRemote(payload);
}

void Anchor::SendPacket_ClearTeamState() {
    json payload;
    payload["type"] = UPDATE_TEAM_STATE;
    payload["targetTeamId"] = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    payload["queue"] = json::array();
    payload["state"] = json::object();
    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_UpdateTeamState(nlohmann::json payload) {
    isHandlingUpdateTeamState = true;
    // This can happen in between file select and the game starting, so we cant use this check, but we need to ensure we
    // be careful to wrap PlayState usage in this check
    // if (!IsSaveLoaded()) {
    //     return;
    // }

    if (payload.contains("state")) {
        SaveContext loadedData = payload["state"].get<SaveContext>();

        gSaveContext.healthCapacity = loadedData.healthCapacity;
        gSaveContext.magicLevel = loadedData.magicLevel;
        gSaveContext.magicCapacity = gSaveContext.magic = loadedData.magicCapacity;
        gSaveContext.isMagicAcquired = loadedData.isMagicAcquired;
        gSaveContext.isDoubleMagicAcquired = loadedData.isDoubleMagicAcquired;
        gSaveContext.isDoubleDefenseAcquired = loadedData.isDoubleDefenseAcquired;
        gSaveContext.bgsFlag = loadedData.bgsFlag;
        gSaveContext.swordHealth = loadedData.swordHealth;
        gSaveContext.ship.quest = loadedData.ship.quest;

        for (int i = 0; i < 124; i++) {
            gSaveContext.sceneFlags[i] = loadedData.sceneFlags[i];
            if (IsSaveLoaded() && gPlayState->sceneNum == i) {
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

        for (int i = 0; i < ceil((RAND_INF_MAX + 15) / 16); i++) {
            gSaveContext.ship.randomizerInf[i] = loadedData.ship.randomizerInf[i];
        }

        for (int i = 0; i < 6; i++) {
            gSaveContext.gsFlags[i] = loadedData.gsFlags[i];
        }

        gSaveContext.ship.stats.fileCreatedAt = loadedData.ship.stats.fileCreatedAt;

        // Restore master sword state
        u8 hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, 1);
        if (hasMasterSword) {
            loadedData.inventory.equipment |= 0x2;
        } else {
            loadedData.inventory.equipment &= ~0x2;
        }

        // Restore bottle contents (unless it's ruto's letter)
        for (int i = 0; i < 4; i++) {
            if (gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_NONE && gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_LETTER_RUTO) {
                loadedData.inventory.items[SLOT_BOTTLE_1 + i] = gSaveContext.inventory.items[SLOT_BOTTLE_1 + i];
            }
        }

        // Restore ammo if it's non-zero, unless it's beans
        for (int i = 0; i < ARRAY_COUNT(gSaveContext.inventory.ammo); i++) {
            if (gSaveContext.inventory.ammo[i] != 0 && i != SLOT(ITEM_BEAN) && i != SLOT(ITEM_BEAN + 1)) {
                loadedData.inventory.ammo[i] = gSaveContext.inventory.ammo[i];
            }
        }

        gSaveContext.inventory = loadedData.inventory;

        // The commented out code below is an attempt at sending the entire randomizer seed over, in hopes that a player doesn't have to generate the seed themselves
        // Currently it doesn't work :)
        if (IS_RANDO && payload["state"].contains("rando")) {
            auto randoContext = Rando::Context::GetInstance();

            for (int i = 0; i < RC_MAX; i++) {
                // randoContext->GetItemLocation(i)->RefPlacedItem() = payload["state"]["rando"]["itemLocations"][i]["rgID"].get<RandomizerGet>();
                OTRGlobals::Instance->gRandoContext->GetItemLocation(i)->SetCheckStatus(payload["state"]["rando"]["itemLocations"][i][0].get<RandomizerCheckStatus>());
                OTRGlobals::Instance->gRandoContext->GetItemLocation(i)->SetIsSkipped(payload["state"]["rando"]["itemLocations"][i][0].get<u8>());

                // if (payload["state"]["rando"]["itemLocations"][i].contains("fakeRgID")) {
                //     randoContext->overrides.emplace(static_cast<RandomizerCheck>(i), Rando::ItemOverride(static_cast<RandomizerCheck>(i), payload["state"]["rando"]["itemLocations"][i]["fakeRgID"].get<RandomizerGet>()));
                //     randoContext->GetItemOverride(i).GetTrickName().english = payload["state"]["rando"]["itemLocations"][i]["trickName"]["english"].get<std::string>();
                //     randoContext->GetItemOverride(i).GetTrickName().french = payload["state"]["rando"]["itemLocations"][i]["trickName"]["french"].get<std::string>();
                // }
                // if (payload["state"]["rando"]["itemLocations"][i].contains("price")) {
                //     u16 price = payload["state"]["rando"]["itemLocations"][i]["price"].get<u16>();
                //     if (price > 0) {
                //         randoContext->GetItemLocation(i)->SetCustomPrice(price);
                //     }
                // }
            }

            // auto entranceCtx = randoContext->GetEntranceShuffler();
            // for (int i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
            //     entranceCtx->entranceOverrides[i].type = payload["state"]["rando"]["entrances"][i]["type"].get<u16>();
            //     entranceCtx->entranceOverrides[i].index = payload["state"]["rando"]["entrances"][i]["index"].get<s16>();
            //     entranceCtx->entranceOverrides[i].destination = payload["state"]["rando"]["entrances"][i]["destination"].get<s16>();
            //     entranceCtx->entranceOverrides[i].override = payload["state"]["rando"]["entrances"][i]["override"].get<s16>();
            //     entranceCtx->entranceOverrides[i].overrideDestination = payload["state"]["rando"]["entrances"][i]["overrideDestination"].get<s16>();
            // }

            // for (int i = 0; i < randoContext->hashIconIndexes.size(); i++) {
            //     randoContext->hashIconIndexes[i] = payload["state"]["rando"]["seed"][i].get<u8>();
            // }
            // randoContext->GetSettings()->SetSeedString(payload["state"]["rando"]["inputSeed"].get<std::string>());
            // randoContext->GetSettings()->SetSeed(payload["state"]["rando"]["finalSeed"].get<u32>());

            // for (int i = 0; i < RSK_MAX; i++) {
            //     randoContext->GetOption(RandomizerSettingKey(i)).SetSelectedIndex(payload["state"]["rando"]["randoSettings"][i].get<u8>());
            // }

            // randoContext->GetDungeons()->ClearAllMQ();
            // for (int i = 0; i < randoContext->GetDungeons()->GetDungeonListSize(); i++) {
            //     if (payload["state"]["rando"]["masterQuestDungeons"][i].get<bool>()) {
            //         randoContext->GetDungeon(i)->SetMQ();
            //     }
            // }
            
            // randoContext->GetTrials()->SkipAll();
            // for (int i = 0; i < randoContext->GetTrials()->GetTrialListSize(); i++) {
            //     if (payload["state"]["rando"]["requiredTrials"][i].get<bool>()) {
            //         randoContext->GetTrial(i)->SetAsRequired();
            //     }
            // }
        }

        Notification::Emit({
            .message = "Save updated from team",
        });
    }

    if (payload.contains("queue")) {
        for (auto& item : payload["queue"]) {
            nlohmann::json itemPayload = nlohmann::json::parse(item.get<std::string>());
            Anchor::Instance->OnIncomingJson(itemPayload);
        }
    }
    isHandlingUpdateTeamState = false;
}

#endif // ENABLE_REMOTE_CONTROL

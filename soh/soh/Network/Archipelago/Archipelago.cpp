#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#include "Archipelago.h"
#include "soh/util.h"
#include <apuuid.hpp>
#include <apclient.hpp>

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

#include "soh/Network/Archipelago/ArchipelagoConsoleWindow.h"
#include "soh/Network/Archipelago/ArchipelagoHintWindow.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"
#include "soh/SaveManager.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/OTRGlobals.h"
#include "soh/Network/Anchor/Anchor.h"
#include "soh/ObjectExtension/ObjectExtension.h"

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
}

uint8_t isArchipelagoParsing = 0;

ArchipelagoClient::ArchipelagoClient() {
    itemQueued = false;
    disconnecting = false;
    uri = "";
    password = "";
}

ArchipelagoClient& ArchipelagoClient::GetInstance() {
    static ArchipelagoClient Client;
    return Client;
}

bool ArchipelagoClient::StartClient() {
    if (apClient != nullptr) {
        apClient.reset();
    }

    disconnecting = false;
    retries = 0;
    uri = CVarGetString(CVAR_REMOTE_ARCHIPELAGO("ServerAddress"), "localhost:38281");
    password = CVarGetString(CVAR_REMOTE_ARCHIPELAGO("Password"), "");
    locationsScouted = false;
    hintsInitialized = false;

    uuid = ap_get_uuid(Ship::Context::GetPathRelativeToAppDirectory("ap-client-uuid"));
    const std::string cert = Ship::Context::LocateFileAcrossAppDirs("networking/cacert.pem");
    SPDLOG_DEBUG("Location of cert: " + cert);
    apClient = std::unique_ptr<APClient>(new APClient(uuid, AP_Client_consts::AP_GAME_NAME, uri, cert));

    CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatus"), 1); // Connecting

    apClient->set_socket_error_handler([&](const std::string& msg) {
        retries++;
        if (retries >= AP_Client_consts::MAX_RETRIES) {
            ArchipelagoConsole_SendMessage("[ERROR] Could not connect to server after several tries.\nAre the entered "
                                           "server address and port correct?");
            CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatus"), 2); // Connection error
            CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatusInGame"), 0);

            disconnecting = true;

            if (GameInteractor::IsSaveLoaded) {
                SohGui::ShowArchipelagoSettingsMenu();
            }
            return;
        }
        ArchipelagoConsole_SendMessage(std::string("[ERROR] " + msg).c_str());
    });

    apClient->set_room_info_handler([&]() {
        std::list<std::string> tags;
        if (CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("DeathLink"), 0)) {
            tags.push_back("DeathLink");
        }
        if (CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("DamageLink"), 0)) {
            tags.push_back("SharedDamage");
        }
        if (CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("TrapLink"), 0)) {
            tags.push_back("TrapLink");
        }
        apClient->ConnectSlot(CVarGetString(CVAR_REMOTE_ARCHIPELAGO("SlotName"), ""), password, 0b0101, tags,
                              { 0, 6, 3 });
    });

    apClient->set_slot_connected_handler([&](const nlohmann::json data) {
        CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatus"), 3); // slot connected
        ArchipelagoConsole_SendMessage("[LOG] Connected.");
        slotData = data;

        ArchipelagoHintWindow_ChangeHintableItems(slotData["hintable_items"]);

        std::string clientVersionMajor = AP_Client_consts::AP_WORLD_VERSION_MAJOR;
        std::string clientVersionMinor = AP_Client_consts::AP_WORLD_VERSION_MINOR;

        // Get APWorld version and split it by .
        std::string apworldVersion = slotData["apworld_version"];
        std::stringstream ss;
        ss << apworldVersion;
        std::string segment;
        std::vector<std::string> seglist;
        while (std::getline(ss, segment, '.')) {
            seglist.push_back(segment);
        }
        std::string apworldVersionMajor = seglist[0];
        std::string apworldVersionMinor = seglist[1];

        if (clientVersionMajor != apworldVersionMajor || clientVersionMinor != apworldVersionMinor) {
            disconnecting = true;
            std::string errorMessage =
                "[ERROR] Client version does not match the APWorld version that\nwas used to generate the multiworld.\n"
                "Supported version in this client is " +
                clientVersionMajor + "." + clientVersionMinor + ".x.\n" + "The used APWorld is on version " +
                apworldVersionMajor + "." + apworldVersionMinor +
                ".x instead.\nPlease use the SoH AP client matching the APWorld's version.\nAutomatically "
                "disconnecting...";
            ArchipelagoConsole_SendMessage(errorMessage.c_str());
            return;
        }

        // if we are already in game when we connect
        // we won't have to request an itemSynch
        if (GameInteractor::IsSaveLoaded(true)) {
            if (!isRightSaveLoaded()) {
                disconnecting = true;
                ArchipelagoConsole_SendMessage("[ERROR] Connected to incorrect slot, disconnecting...");
                return;
            }

            // save the connection details in case they changed
            SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.archiUri, uri,
                                            ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.archiUri));
            SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.slotName, GetSlotName(),
                                            ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.slotName));
            SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.roomPass, password,
                                            ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.roomPass));

            ResetQueue();
            SynchSentLocations();
            ArchipelagoClient::SetDataStorage("scene", gPlayState->sceneNum);
        }

        const int team_number = apClient->get_team_number();
        const int player_id = apClient->get_player_number();
        std::list<std::string> requests;
        std::stringstream hintNotificationKey;
        hintNotificationKey << "_read_hints_" << team_number << "_" << player_id;
        requests.emplace_back(hintNotificationKey.str());
        apClient->SetNotify({ hintNotificationKey.str() });
        apClient->Get(requests);
    });

    apClient->set_slot_refused_handler([&](const std::list<std::string>& msgs) {
        disconnecting = true;
        for (const std::string& msg : msgs) {
            ArchipelagoConsole_SendMessage(std::string("[ERROR] " + msg).c_str());
        }
    });

    apClient->set_items_received_handler([&](const std::list<APClient::NetworkItem>& items) {
        if (disconnecting) {
            return;
        }

        for (const APClient::NetworkItem& item : items) {
            ApItem apItem;
            const std::string game = apClient->get_player_game(item.player);
            apItem.itemName = apClient->get_item_name(item.item, AP_Client_consts::AP_GAME_NAME);
            apItem.locationName = apClient->get_location_name(item.location, game);
            apItem.playerName = apClient->get_player_alias(item.player);
            apItem.playerNumber = item.player;
            apItem.flags = item.flags;
            apItem.index = item.index;
            OnItemReceived(apItem);
        }
    });

    apClient->set_location_info_handler([&](const std::list<APClient::NetworkItem>& items) {
        if (disconnecting) {
            return;
        }

        scoutedItems.clear();

        for (const APClient::NetworkItem& item : items) {
            ApItem apItem;
            const std::string game = apClient->get_player_game(item.player);
            apItem.itemName = apClient->get_item_name(item.item, game);
            apItem.hintName = get_random_group_from_item(apItem.itemName, item.flags, game);
            apItem.locationName = apClient->get_location_name(item.location, AP_Client_consts::AP_GAME_NAME);
            apItem.playerName = apClient->get_player_alias(item.player);
            apItem.playerNumber = item.player;
            apItem.flags = item.flags;
            apItem.index = item.index;
            scoutedItems.push_back(apItem);
        }

        locationsScouted = true;
        newInitDataRecieved();
    }); // todo maybe move these functions to a lambda, since they don't have to be static anymore

    apClient->set_location_checked_handler([&](const std::list<int64_t> locations) {
        if (disconnecting) {
            return;
        }

        for (const int64_t apLoc : locations) {
            QueueExternalCheck(apLoc);
        }
    });

    apClient->set_print_json_handler([&](const APClient::PrintJSONArgs& arg) {
        if (disconnecting) {
            return;
        }

        APClient* client = apClient.get();

        // If we are supposed to limit console output, check if this data concerns this slot.
        if (CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("LimitConsoleToPlayer"), 0)) {
            // (If the slot the message was sent from is not the server or this slot) or (the reciever is this slot)
            // This requires checking if the arg.slot and arg.recieving pointers are nullptr before comparing them
            if ((arg.slot != nullptr && (*arg.slot <= 0 || *arg.slot != client->get_player_number())) ||
                (arg.receiving != nullptr && *arg.receiving != client->get_player_number())) {
                return;
            }
        }

        std::vector<AP_Text::ColoredTextNode> coloredNodes;

        for (const APClient::TextNode& node : arg.data) {
            AP_Text::TextColor color = AP_Text::TextColor::COLOR_DEFAULT;
            std::string text;

            if (node.type == "player_id") {
                int id = std::stoi(node.text);
                if (color == AP_Text::TextColor::COLOR_DEFAULT && id == client->get_player_number())
                    color = AP_Text::TextColor::COLOR_MAGENTA;
                else if (color == AP_Text::TextColor::COLOR_DEFAULT)
                    color = AP_Text::TextColor::COLOR_YELLOW;
                text = client->get_player_alias(id);
            } else if (node.type == "item_id") {
                int64_t id = std::stoll(node.text);
                if (color == AP_Text::TextColor::COLOR_DEFAULT) {
                    if (node.flags & APClient::ItemFlags::FLAG_ADVANCEMENT)
                        color = AP_Text::TextColor::COLOR_PLUM;
                    else if (node.flags & APClient::ItemFlags::FLAG_NEVER_EXCLUDE)
                        color = AP_Text::TextColor::COLOR_SLATEBLUE;
                    else if (node.flags & APClient::ItemFlags::FLAG_TRAP)
                        color = AP_Text::TextColor::COLOR_SALMON;
                    else
                        color = AP_Text::TextColor::COLOR_CYAN;
                }
                text = client->get_item_name(id, client->get_player_game(node.player));
            } else if (node.type == "location_id") {
                int64_t id = std::stoll(node.text);
                if (color == AP_Text::TextColor::COLOR_DEFAULT)
                    color = AP_Text::TextColor::COLOR_BLUE;
                text = client->get_location_name(id, client->get_player_game(node.player));
            } else if (node.type == "hint_status") {
                text = node.text;
                if (node.hintStatus == APClient::HINT_FOUND)
                    color = AP_Text::TextColor::COLOR_GREEN;
                else if (node.hintStatus == APClient::HINT_UNSPECIFIED)
                    color = AP_Text::TextColor::COLOR_GRAY;
                else if (node.hintStatus == APClient::HINT_NO_PRIORITY)
                    color = AP_Text::TextColor::COLOR_SLATEBLUE;
                else if (node.hintStatus == APClient::HINT_AVOID)
                    color = AP_Text::TextColor::COLOR_SALMON;
                else if (node.hintStatus == APClient::HINT_PRIORITY)
                    color = AP_Text::TextColor::COLOR_PLUM;
                else
                    color = AP_Text::TextColor::COLOR_RED; // unknown status -> red
            } else if (node.type == "ERROR") {
                color = AP_Text::TextColor::COLOR_ERROR;
                text = node.text;
            } else if (node.type == "LOG") {
                color = AP_Text::TextColor::COLOR_LOG;
                text = node.text;
            } else {
                color = AP_Text::TextColor::COLOR_WHITE;
                text = node.text;
            }

            AP_Text::ColoredTextNode Colornode;
            Colornode.color = color;
            Colornode.text = text;
            coloredNodes.push_back(Colornode);
        }

        ArchipelagoConsole_PrintJson(coloredNodes);
    });

    apClient->set_bounced_handler([&](const nlohmann::json data) {
        if (data.contains("tags")) {
            std::list<std::string> tags = data["tags"];
            bool deathLink = (std::find(tags.begin(), tags.end(), "DeathLink") != tags.end());
            bool damageLink = (std::find(tags.begin(), tags.end(), "SharedDamage") != tags.end());
            bool trapLink = (std::find(tags.begin(), tags.end(), "TrapLink") != tags.end());

            if ((deathLink || damageLink || trapLink) && data["data"]["source"] != apClient->get_slot()) {
                if (GameInteractor::IsSaveLoaded() && !GameInteractor::IsGameplayPaused()) {
                    if (deathLink) {
                        lastDeathLink = GetUnixTimestamp();

                        gSaveContext.health = 0;
                        std::string prefixText = std::string(data["data"]["source"]) + " died.";
                        Notification::Emit(
                            { .prefix = prefixText, .message = "Cause:", .suffix = data["data"]["cause"] });
                        std::string deathLinkMessage = "[LOG] Received death link from " +
                                                       std::string(data["data"]["source"]) +
                                                       ". Cause: " + std::string(data["data"]["cause"]);
                        ArchipelagoConsole_SendMessage(deathLinkMessage.c_str());
                    } else if (damageLink) {
                        uint16_t receivedDamage = data["data"]["damage_points"];

                        // Ignore incoming damage if it's less than a quarter heart.
                        if (receivedDamage >= 20) {
                            Player* player = GET_PLAYER(gPlayState);
                            // 80 received points is one full heart aka 16 health.
                            gSaveContext.health -= floor(receivedDamage / 5);

                            // Only use hit animations when the player is able to process it.
                            if (!(GameInteractor::IsGameplayPaused() || player->stateFlags2 & PLAYER_STATE2_CRAWLING ||
                                  gPlayState->sceneNum == SCENE_FISHING_POND)) {
                                // If received damage is 3 hearts or more, do a knockback. Otherwise just do a small hit
                                // animation.
                                if (receivedDamage >= 240) {
                                    GameInteractor::RawAction::KnockbackPlayer(1.0f);
                                } else {
                                    func_80837C0C(gPlayState, player, 0, 0, 0, 0, 0);
                                    player->invincibilityTimer = 10;
                                }
                            }
                        }

                        std::string damageLinkMessage =
                            "[LOG] Received damage link from " + std::string(data["data"]["source"]);
                        ArchipelagoConsole_SendMessage(damageLinkMessage.c_str());
                    } else if (trapLink) {
                        ++trapLinkCount;

                        gSaveContext.ship.pendingIceTrapCount++;

                        Notification::Emit(
                            { .prefix = std::string(data["data"]["source"]), .message = " sent a trap." });

                        std::string trapLinkMessage =
                            "[LOG] Received trap link from " + std::string(data["data"]["source"]);
                        ArchipelagoConsole_SendMessage(trapLinkMessage.c_str());
                    }
                }
            }
        }
    });

    apClient->set_set_reply_handler([&](const nlohmann::json data) {
        // hints
        const int team_number = apClient->get_team_number();
        const int player_number = apClient->get_player_number();
        std::stringstream hint_key;
        hint_key << "_read_hints_" << team_number << "_" << player_number;
        if (data["key"] == hint_key.str()) {
            UpdateHints(data["value"]);
            return;
        }

        std::unordered_set<std::string> games;
        for (const APClient::NetworkPlayer& player : apClient->get_players()) {
            games.emplace(apClient->get_player_game(player.slot));
        }
    });

    apClient->set_retrieved_handler([&](const std::map<std::string, nlohmann::json>& data) {
        const int team_number = apClient->get_team_number();
        const int player_number = apClient->get_player_number();
        std::stringstream hint_key;
        hint_key << "_read_hints_" << team_number << "_" << player_number;
        if (data.contains(hint_key.str())) {
            UpdateHints(data.at(hint_key.str()));
        }

        std::unordered_set<std::string> games;
        for (const APClient::NetworkPlayer& player : apClient->get_players()) {
            games.emplace(apClient->get_player_game(player.slot));
        }

        // item & location groups
        bool groups_received = false;
        for (const std::string& game : games) {
            const std::string item_group_key = "_read_item_name_groups_" + game;
            if (data.contains(item_group_key)) {
                groups_received = true;
                UpdateItemGroup(game, data.at(item_group_key));
            }
            const std::string location_group_key = "_read_location_name_groups_" + game;
            if (data.contains(location_group_key)) {
                groups_received = true;
                UpdateLocationGroup(game, data.at(location_group_key));
            }
        }

        // after getting grouping data, fetch location scouts
        // TODO only scout for locations if we're in the process of creating a save file
        // Foreign hints should be loaded from the save file as well
        if (groups_received) {
            ArchipelagoClient::InitForeignHints();
        }
    });

    return true;
}

bool ArchipelagoClient::StopClient() {
    disconnecting = true;
    return true;
}

void ArchipelagoClient::RequestInitData() {
    // To create a save file we'll need the following data:
    // Slot Data: recieved on connection, we already have this
    // Data package: recieved on connection, we already have this
    // Location Scouts, Asynch request done here
    // Location and Item groups, Asynch request done here

    CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatus"), 4); // fetching new save data

    // get location scouts
    StartLocationScouts();

    // request the item and location groups
    std::unordered_set<std::string> games;
    for (const APClient::NetworkPlayer& player : apClient->get_players()) {
        games.emplace(apClient->get_player_game(player.slot));
    }

    std::list<std::string> requests;
    for (const std::string& game : games) {
        requests.emplace_back("_read_item_name_groups_" + game);
        requests.emplace_back("_read_location_name_groups_" + game);
    }
    apClient->Get(requests);
}

// update the connection status if we have all data we need to initialize a save file
void ArchipelagoClient::newInitDataRecieved() {
    if (locationsScouted && hintsInitialized) {
        CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatus"), 5); // new save data fetched
    }
}

void ArchipelagoClient::GameLoaded() {

    // Load textures for the archipelago items that're shown in the notifications
    static bool sArchipelagoTexturesLoaded = false;
    if (!sArchipelagoTexturesLoaded) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage(
            "Archipelago Progressive Icon", "textures/parameter_static/gArchipelagoProgressive.png");
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage(
            "Archipelago Useful Icon", "textures/parameter_static/gArchipelagoUseful.png");
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage(
            "Archipelago Junk Icon", "textures/parameter_static/gArchipelagoJunk.png");

        sArchipelagoTexturesLoaded = true;
    }

    if (apClient == nullptr) {
        if (IS_ARCHIPELAGO) {
            CVarSetString(CVAR_REMOTE_ARCHIPELAGO("ServerAddress"), gSaveContext.ship.quest.data.archipelago.archiUri);
            CVarSetString(CVAR_REMOTE_ARCHIPELAGO("SlotName"), gSaveContext.ship.quest.data.archipelago.slotName);
            CVarSetString(CVAR_REMOTE_ARCHIPELAGO("Password"), gSaveContext.ship.quest.data.archipelago.roomPass);
            StartClient();
        }
        return;
    }

    // if its not an AP save, disconnect
    if (!IS_ARCHIPELAGO) {
        ArchipelagoConsole_SendMessage("[ERROR] Loaded save is not not an archipelago save, disconnecting...");
        disconnecting = true;
        return;
    }

    if (!isRightSaveLoaded()) {
        ArchipelagoConsole_SendMessage("Disconnecting from previous slot and connecting to this one...");
        CVarSetString(CVAR_REMOTE_ARCHIPELAGO("ServerAddress"), gSaveContext.ship.quest.data.archipelago.archiUri);
        CVarSetString(CVAR_REMOTE_ARCHIPELAGO("SlotName"), gSaveContext.ship.quest.data.archipelago.slotName);
        CVarSetString(CVAR_REMOTE_ARCHIPELAGO("Password"), gSaveContext.ship.quest.data.archipelago.roomPass);
        StartClient();
        return;
    }

    SynchItems();
    SynchSentLocations();
    SynchReceivedLocations();
}

void ArchipelagoClient::StartLocationScouts() {
    std::set<int64_t> missing_loc_set = apClient->get_missing_locations();
    std::set<int64_t> found_loc_set = apClient->get_checked_locations();
    std::list<int64_t> location_list;
    for (const int64_t loc_id : missing_loc_set) {
        location_list.emplace_back(loc_id);
    }
    for (const int64_t loc_id : found_loc_set) {
        location_list.emplace_back(loc_id);
    }
    apClient->LocationScouts(location_list);
}

void ArchipelagoClient::SynchItems() {
    // Send a Synch request to get any items we may have missed
    ResetQueue();
    apClient->Sync();
}

void ArchipelagoClient::SynchSentLocations() {
    // send already checked locations
    std::list<int64_t> checkedLocations;
    for (const auto& loc : Rando::StaticData::GetLocationTable()) {
        const RandomizerCheck rc = loc.GetRandomizerCheck();
        if (Rando::Context::GetInstance()->GetItemLocation(rc)->HasObtained()) {
            const int64_t apLocation = apClient->get_location_id(loc.GetName());
            checkedLocations.emplace_back(apLocation);
        }
    }

    apClient->LocationChecks(checkedLocations);
}

void ArchipelagoClient::SynchReceivedLocations() {
    // Open checks that have been found previously but went unsaved
    for (const int64_t apLoc : apClient->get_checked_locations()) {
        QueueExternalCheck(apLoc);
    }
}

void ArchipelagoClient::InitForeignHints() {
    foreignHints.clear();
    for (int h = RH_NONE; h < RH_MAX; h++) {
        foreignHints[(RandomizerHint)h] = {};
    }

    std::map<std::string, std::vector<std::array<int, 2>>> hintsData = slotData["static_hints"];
    for (const auto& hintData : hintsData) {
        RandomizerHint hintKey = static_cast<RandomizerHint>(Rando::StaticData::hintNameToEnum[hintData.first]);
        std::vector<ApForeignHint> foreignLocations;
        for (const auto& hintLocation : hintData.second) {
            ApForeignHint foreignHint;
            foreignHint.playerId = hintLocation[0];
            foreignHint.locationId = hintLocation[1];
            foreignHint.playerName = apClient->get_player_alias(foreignHint.playerId);
            const std::string& game = apClient->get_player_game(foreignHint.playerId);
            foreignHint.locationName = apClient->get_location_name(foreignHint.locationId, game);
            foreignHint.groupName = get_random_group_from_location(foreignHint.locationName, game);
            foreignLocations.push_back(foreignHint);
        }
        foreignHints[hintKey] = foreignLocations;
    }

    hintsInitialized = true;
    newInitDataRecieved();
}

void ArchipelagoClient::QueueExternalCheck(const int64_t apLocation) {
    const std::string checkName = apClient->get_location_name(apLocation, AP_Client_consts::AP_GAME_NAME);
    const uint32_t RC = static_cast<uint32_t>(Rando::StaticData::locationNameToEnum[checkName]);

    if (RC == RC_UNKNOWN_CHECK) {
        ArchipelagoConsole_SendMessage("[ERROR] Attempting to queue an unknown location (RC_UNKOWN_CHECK), skipping.");
        return;
    }

    // Don't queue checks we already have
    if (Rando::Context::GetInstance()->GetItemLocation(RC)->HasObtained()) {
        return;
    }

    GameInteractor_ExecuteOnRandomizerExternalCheck(RC);
}

bool ArchipelagoClient::IsConnected() const {
    if (apClient == nullptr) {
        return false;
    }

    CVarSetInteger(CVAR_REMOTE_ANCHOR("RoomSettings.SyncItemsAndFlags"), 0);
    if (Anchor::Instance->isConnected && Anchor::Instance->roomState.ownerClientId == Anchor::Instance->ownClientId) {
        Anchor::Instance->SendPacket_UpdateRoomState();
    }

    return apClient->get_state() == APClient::State::SLOT_CONNECTED;
}

void ArchipelagoClient::CheckLocation(RandomizerCheck sohCheckId) {
    if (sohCheckId == RC_UNKNOWN_CHECK) {
        ArchipelagoConsole_SendMessage("[ERROR] Trying to queue an unknown location (RC_UNKOWN_CHECK), skipping");
        return;
    }

    if (!IsConnected()) {
        return;
    }

    std::string apName = Rando::StaticData::GetLocation(sohCheckId)->GetName();
    if (apName.empty()) {
        return;
    }

    int64_t apItemId = apClient->get_location_id(std::string(apName));
    apClient->LocationChecks({ apItemId });
}

void ArchipelagoClient::OnItemReceived(const ApItem apItem) {

    // Don't queue up any items when we aren't in game
    // Any Items missed this way will get synched when we load the save
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    // Skip queueing any items we already have
    if (apItem.index < gSaveContext.ship.quest.data.archipelago.lastReceivedItemIndex) {
        return;
    }

    // Add item to the queue
    receiveQueue.push(apItem);
}

void ArchipelagoClient::QueueItem(const ApItem item) {
    RandomizerGet RG = Rando::StaticData::itemNameToEnum[item.itemName];
    if (RG == RG_NONE) {
        return;
    }

    if (OTRGlobals::Instance->gRandomizer->GetItemObtainabilityFromRandomizerGet(RG) != CAN_OBTAIN) {
        RG = RG_BLUE_RUPEE;
    }

    itemQueued = true;
    GameInteractor_ExecuteOnArchipelagoItemReceived(static_cast<int32_t>(RG));
}

void ArchipelagoClient::SendGameWon() {
    if (apClient == nullptr) {
        return;
    }

    apClient->StatusUpdate(APClient::ClientStatus::GOAL);
}

void ArchipelagoClient::SendMessageToConsole(const std::string message) {
    // local commands not implemented yet
    if (message.starts_with("/")) {
        ArchipelagoConsole_SendMessage(
            "Ship of Harkinian does not have any local commands.\nUse \"!help\" to see server commands instead.");
        return;
    }

    if (apClient == nullptr) {
        ArchipelagoConsole_SendMessage("[ERROR] Could not send message. Please Connect to your slot.");
        return;
    }

    apClient->Say(message);
}

void ArchipelagoClient::UpdateHints(const std::vector<nlohmann::json>& hints_json) {
    // update the hint table
    std::vector<AP_Hint::Hint> new_hints;
    const int player_number = apClient->get_player_number();
    for (const nlohmann::json& hint_data : hints_json) {
        AP_Hint::Hint new_hint;
        const int receiving_player_id = hint_data["receiving_player"];
        const int finding_player_id = hint_data["finding_player"];
        new_hint.index = new_hints.size();
        new_hint.receiving_player_name = apClient->get_player_alias(receiving_player_id);
        new_hint.finding_player_name = apClient->get_player_alias(finding_player_id);
        new_hint.location_name =
            apClient->get_location_name(hint_data["location"], apClient->get_player_game(finding_player_id));
        new_hint.item_name = apClient->get_item_name(hint_data["item"], apClient->get_player_game(finding_player_id));
        new_hint.entrance_name = hint_data["entrance"];
        new_hint.item_flags = hint_data["item_flags"];
        new_hint.found = hint_data["found"];
        new_hint.finding_player_id = finding_player_id;
        new_hint.location_id = hint_data["location"];
        new_hint.we_receive = player_number == receiving_player_id;
        new_hint.we_find = player_number == finding_player_id;
        switch (static_cast<int>(hint_data["status"])) {
            case APClient::HINT_NO_PRIORITY:
                new_hint.hint_status = AP_Hint::HintStatus::HINT_NO_PRIORITY;
                break;
            case APClient::HINT_AVOID:
                new_hint.hint_status = AP_Hint::HintStatus::HINT_AVOID;
                break;
            case APClient::HINT_PRIORITY:
                new_hint.hint_status = AP_Hint::HintStatus::HINT_PRIORITY;
                break;
            case APClient::HINT_FOUND:
                new_hint.hint_status = AP_Hint::HintStatus::HINT_FOUND;
                break;
            case APClient::HINT_UNSPECIFIED:
            default:
                new_hint.hint_status = AP_Hint::HintStatus::HINT_UNSPECIFIED;
                break;
        }
        new_hints.push_back(new_hint);
    }
    ArchipelagoHintWindow_UpdateHints(new_hints);
}

void ArchipelagoClient::UpdateItemGroup(const std::string game, const nlohmann::json& item_group_json) {
    item_groups[game] = item_group_json;
}

void ArchipelagoClient::UpdateLocationGroup(const std::string game, const nlohmann::json& location_group_json) {
    location_groups[game] = location_group_json;
}

std::string ArchipelagoClient::get_random_group_from_item(const std::string& item_name, unsigned int item_flags,
                                                          const std::string& game) {
    const std::unordered_map<std::string, std::vector<std::string>>& game_item_groups = item_groups[game];

    // find in which groups this item exists
    std::vector<std::string> containing_groups;
    for (const auto& group : game_item_groups) {
        if (group.first == "Everything") {
            continue;
        }

        for (const std::string& item : group.second) {
            if (item == item_name) {
                containing_groups.emplace_back(group.first);
            }
        }
    }

    // if the item isn't in a specific group
    // fall back to obscure names
    if (containing_groups.empty()) {
        if (item_flags & (APClient::FLAG_TRAP | APClient::FLAG_ADVANCEMENT)) {
            return "Progress Item";
        } else if (item_flags & APClient::FLAG_NEVER_EXCLUDE) {
            return "Item";
        } else {
            return "Junk";
        }
    }

    return RandomElement(containing_groups, false);
}

std::string ArchipelagoClient::get_random_group_from_location(const std::string& item_name, const std::string& game) {
    const std::unordered_map<std::string, std::vector<std::string>>& game_location_groups = location_groups[game];

    // find in which groups this item exists
    std::vector<std::string> containing_groups;
    for (const auto& group : game_location_groups) {
        if (group.first == "Everywhere") {
            continue;
        }

        for (const std::string& item : group.second) {
            if (item == item_name) {
                containing_groups.emplace_back(group.first);
            }
        }
    }

    // if the location isn't explicitly grouped
    // fall back to obscure location
    if (containing_groups.empty()) {
        return "world";
    }

    return RandomElement(containing_groups, false);
}

void ArchipelagoClient::Poll() {
    if (apClient == nullptr) {
        return;
    }

    if (disconnecting) {
        apClient->reset();
        apClient = nullptr;
        ResetQueue();
        disconnecting = false;
        CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatus"), 0); // disconnected
        CVarSetInteger(CVAR_REMOTE_ARCHIPELAGO("ConnectionStatusInGame"), 0);

        std::vector<AP_Hint::Hint> empty_hints = {};
        ArchipelagoHintWindow_UpdateHints(empty_hints);
        return;
    }

    // queue another item to be received
    if (!itemQueued && receiveQueue.size() > 0) {
        const ApItem item = receiveQueue.front();
        receiveQueue.pop();
        QueueItem(item);
    }

    apClient->poll();
}

void ArchipelagoClient::ResetQueue() {
    itemQueued = false;
    std::queue<ApItem> empty;
    std::swap(receiveQueue, empty);
}

void ArchipelagoClient::OnSceneInit(uint16_t sceneNum) {
    if (ArchipelagoClient::IsConnected() && GameInteractor::IsSaveLoaded(true)) {
        ArchipelagoClient::SetDataStorage("scene", sceneNum);
    }
}

void ArchipelagoClient::SetDataStorage(const std::string& key, const nlohmann::json& value) const {
    std::stringstream full_key;
    full_key << "oot_soh_" << key << "_" << apClient->get_team_number() << "_" << apClient->get_player_number();
    std::list<APClient::DataStorageOperation> operations = { { "replace", value } };
    apClient->Set(full_key.str(), 0, false, operations);
}
void ArchipelagoClient::OpenLocalHint(RandomizerCheck sohCheckId) {
    if (sohCheckId == RC_UNKNOWN_CHECK) {
        ArchipelagoConsole_SendMessage("[ERROR] Trying to hint an unknown location (RC_UNKOWN_CHECK), skipping");
        return;
    }

    if (!IsConnected()) {
        return;
    }

    Rando::ItemLocation* itemLoc = Rando::Context::GetInstance()->GetItemLocation(sohCheckId);
    if (itemLoc->HasObtained()) {
        return;
    }

    Rando::Item item = itemLoc->GetPlacedItem();
    if (item.GetCategory() == ITEM_CATEGORY_JUNK && !CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("FillerHints"), 0)) {
        return;
    }

    // Don't hint vanilla shop items, these aren't checks in archipelago
    if (item.GetRandomizerGet() >= RG_BUY_DEKU_NUTS_5 && item.GetRandomizerGet() <= RG_BUY_RED_POTION_50) {
        return;
    }

    std::string apName = Rando::StaticData::GetLocation(sohCheckId)->GetName();
    if (apName.empty()) {
        return;
    }

    int64_t apItemId = apClient->get_location_id(std::string(apName));
    apClient->CreateHints({ apItemId }, -1);
}

void ArchipelagoClient::OpenForeignHint(RandomizerHint randomizerHintId) {
    if (randomizerHintId == RH_NONE) {
        ArchipelagoConsole_SendMessage("[ERROR] Trying to hint an unknown location (RH_NONE), skipping");
        return;
    }

    if (!IsConnected()) {
        return;
    }

    for (const ApForeignHint& foreignHint : foreignHints[randomizerHintId]) {
        if (foreignHint.playerId == apClient->get_player_number() &&
            foreignHint.locationName == Rando::StaticData::GetLocation(RC_LINKS_POCKET)->GetName()) {
            continue;
        }
        apClient->CreateHints({ foreignHint.locationId }, foreignHint.playerId);
    }
}

void ArchipelagoClient::OnDialogCloseHook() {
    // Only open up the hint if we're using clear hints
    if (Rando::Context::GetInstance()->GetOption(RSK_HINT_CLARITY).IsNot(RO_HINT_CLARITY_CLEAR)) {
        return;
    }

    // Here we can check for the right message and open up its associated hint
    MessageContext* msgCtx = &(gPlayState->msgCtx);
    Actor* actor = msgCtx->talkActor;
    std::shared_ptr<Rando::Context> rndCtx = Rando::Context::GetInstance();
    std::vector<ApForeignHint>* hintList = nullptr;
    RandomizerHint rh = RH_NONE;
    switch (msgCtx->textId) {
        case TEXT_ANJU_PLEASE_BRING_MY_CUCCOS_BACK:
        case TEXT_ANJU_PLEASE_BRING_4_CUCCOS:
        case TEXT_ANJU_PLEASE_BRING_3_CUCCOS:
        case TEXT_ANJU_PLEASE_BRING_2_CUCCOS:
        case TEXT_ANJU_PLEASE_BRING_1_CUCCO: {
            OpenLocalHint(RC_KAK_ANJU_AS_CHILD);
            break;
        }
        case TEXT_SKULLTULA_PEOPLE_IM_CURSED: {
            RandomizerCheck tokenCheck = RC_UNKNOWN_CHECK;
            if (actor->params == 1 && rndCtx->GetOption(RSK_KAK_10_SKULLS_HINT)) {
                tokenCheck = RC_KAK_10_GOLD_SKULLTULA_REWARD;
            } else if (actor->params == 2 && rndCtx->GetOption(RSK_KAK_20_SKULLS_HINT)) {
                tokenCheck = RC_KAK_20_GOLD_SKULLTULA_REWARD;
            } else if (actor->params == 3 && rndCtx->GetOption(RSK_KAK_30_SKULLS_HINT)) {
                tokenCheck = RC_KAK_30_GOLD_SKULLTULA_REWARD;
            } else if (actor->params == 4 && rndCtx->GetOption(RSK_KAK_40_SKULLS_HINT)) {
                tokenCheck = RC_KAK_40_GOLD_SKULLTULA_REWARD;
            } else if (rndCtx->GetOption(RSK_KAK_50_SKULLS_HINT)) {
                tokenCheck = RC_KAK_50_GOLD_SKULLTULA_REWARD;
            }
            OpenLocalHint(tokenCheck);
        } break;
        case TEXT_SKULLTULA_PEOPLE_MAKE_YOU_VERY_RICH:
            if (rndCtx->GetOption(RSK_KAK_100_SKULLS_HINT)) {
                OpenLocalHint(RC_KAK_100_GOLD_SKULLTULA_REWARD);
            }
            break;
        case TEXT_MASK_SHOP_SIGN:
            if (rndCtx->GetOption(RSK_MASK_SHOP_HINT)) {
                OpenLocalHint(RC_DEKU_THEATER_SKULL_MASK);
                OpenLocalHint(RC_DEKU_THEATER_MASK_OF_TRUTH);
            }
            break;
        case TEXT_GHOST_SHOP_CARD_HAS_POINTS:
        case TEXT_GHOST_SHOP_EXPLAINATION:
            if (rndCtx->GetOption(RSK_BIG_POES_HINT)) {
                OpenLocalHint(RC_MARKET_10_BIG_POES);
            }
            break;
        case TEXT_MALON_EVERYONE_TURNING_EVIL:
        case TEXT_MALON_I_SING_THIS_SONG:
        case TEXT_MALON_HOW_IS_EPONA_DOING:
        case TEXT_MALON_OBSTICLE_COURSE:
        case TEXT_MALON_INGO_MUST_HAVE_BEEN_TEMPTED:
            if (rndCtx->GetOption(RSK_MALON_HINT)) {
                OpenLocalHint(RC_KF_LINKS_HOUSE_COW);
            }
            break;
        case TEXT_FROGS_UNDERWATER:
            if (rndCtx->GetOption(RSK_FROGS_HINT)) {
                OpenLocalHint(RC_ZR_FROGS_OCARINA_GAME);
            }
            break;
        case TEXT_GF_HBA_SIGN:
        case TEXT_HBA_NOT_ON_HORSE:
        case TEXT_HBA_INITIAL_EXPLAINATION:
        case TEXT_HBA_ALREADY_HAVE_1000:
            if (rndCtx->GetOption(RSK_HBA_HINT)) {
                OpenLocalHint(RC_GF_HBA_1000_POINTS);
                OpenLocalHint(RC_GF_HBA_1500_POINTS);
            }
            break;
        case TEXT_SCRUB_NO_WAY:
        case TEXT_SCRUB_CANT_AFFORD:
        case TEXT_SCRUB_CAPACITY_FULL:
        case TEXT_SCRUB_CANT_GET:
            if (rndCtx->GetOption(RSK_SCRUB_TEXT_HINT)) {
                ScrubIdentity* checkIdentity = ObjectExtension::GetInstance().Get<ScrubIdentity>(actor);
                if (checkIdentity != nullptr) {
                    RandomizerCheck scrubCheck = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf(
                        checkIdentity->identity.randomizerInf);
                    OpenLocalHint(scrubCheck);
                }
            }
            break;
        case TEXT_BEAN_SALESMAN_NOT_ENOUGH_MONEY:
        case TEXT_BEAN_SALESMAN_OH_WELL:
            if (rndCtx->GetOption(RSK_MERCHANT_TEXT_HINT) &&
                (rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_BEANS_ONLY) ||
                 rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))) {
                OpenLocalHint(RC_ZR_MAGIC_BEAN_SALESMAN);
            }
            break;
        case TEXT_GRANNYS_SHOP_CHANGE_YOUR_MIND:
        case TEXT_GRANNYS_SHOP_CANT_AFFORD:
            if (rndCtx->GetOption(RSK_MERCHANT_TEXT_HINT) &&
                (rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
                 rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))) {
                if (rndCtx->GetOption(RSK_SHUFFLE_ADULT_TRADE) || INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK) {
                    OpenLocalHint(RC_KAK_GRANNYS_SHOP);
                }
            }
            break;
        case TEXT_MEDIROGON_CANT_AFFORD:
        case TEXT_MEDIGORON_DECLINE:
            if (rndCtx->GetOption(RSK_MERCHANT_TEXT_HINT) &&
                (rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
                 rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))) {
                OpenLocalHint(RC_GC_MEDIGORON);
            }
            break;
        case TEXT_CARPET_SALESMAN_CUSTOM_FAIL_TO_BUY:
        case TEXT_CARPET_SALESMAN_DECLINE:
        case TEXT_CARPET_SALESMAN_CANT_AFFORD:
            if (rndCtx->GetOption(RSK_MERCHANT_TEXT_HINT) &&
                (rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
                 rndCtx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))) {
                OpenLocalHint(RC_WASTELAND_BOMBCHU_SALESMAN);
            }
            break;
        case TEXT_BIGGORON_BETTER_AT_SMITHING:
        case TEXT_BIGGORON_WAITING_FOR_YOU:
        case TEXT_BIGGORON_RETURN_AFTER_A_FEW_DAYS:
        case TEXT_BIGGORON_I_MAAAADE_THISSSS:
            if (rndCtx->GetOption(RSK_BIGGORON_HINT)) {
                OpenLocalHint(RC_DMT_TRADE_CLAIM_CHECK);
            }
            break;
        case TEXT_SHEIK_NEED_HOOK:
        case TEXT_SHEIK_HAVE_HOOK:
            switch (gPlayState->sceneNum) {
                case SCENE_TEMPLE_OF_TIME:
                    if (rndCtx->GetOption(RSK_OOT_HINT)) {
                        OpenLocalHint(RC_HF_OCARINA_OF_TIME_ITEM);
                        OpenLocalHint(RC_SONG_FROM_OCARINA_OF_TIME);
                    }
                    break;
                case SCENE_INSIDE_GANONS_CASTLE:
                    if (rndCtx->GetOption(RSK_SHEIK_LA_HINT) && INV_CONTENT(ITEM_ARROW_LIGHT) != ITEM_ARROW_LIGHT) {
                        OpenForeignHint(RH_SHEIK_HINT);
                    }
                    break;
            }
            break;
        case TEXT_FISHING_CLOUDY:
        case TEXT_FISHING_TRY_ANOTHER_LURE:
        case TEXT_FISHING_SECRETS:
        case TEXT_FISHING_GOOD_FISHERMAN:
        case TEXT_FISHING_DIFFERENT_POND:
        case TEXT_FISHING_SCRATCHING:
        case TEXT_FISHING_TRY_ANOTHER_LURE_WITH_SINKING_LURE:
            if (rndCtx->GetOption(RSK_LOACH_HINT)) {
                OpenLocalHint(RC_LH_HYRULE_LOACH);
            }
            break;
        case TEXT_GANONDORF:
            if (rndCtx->GetOption(RSK_GANONDORF_HINT)) {
                if (!CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER) ||
                    INV_CONTENT(ITEM_ARROW_LIGHT) != ITEM_ARROW_LIGHT) {
                    OpenForeignHint(RH_GANONDORF_HINT);
                }
            }
            break;
        case TEXT_DAMPES_DIARY:
            if (rndCtx->GetOption(RSK_DAMPES_DIARY_HINT)) {
                OpenForeignHint(RH_DAMPES_DIARY);
            }
            break;
        case TEXT_CHEST_GAME_PROCEED:
        case TEXT_CHEST_GAME_REAL_GAMBLER:
        case TEXT_CHEST_GAME_THANKS_A_LOT:
            if (rndCtx->GetOption(RSK_GREG_HINT)) {
                OpenForeignHint(RH_GREG_RUPEE);
            }
            break;
        case TEXT_ALTAR_CHILD:
            if (rndCtx->GetOption(RSK_TOT_ALTAR_HINT)) {
                OpenForeignHint(RH_ALTAR_CHILD);
            }
            break;
        case TEXT_ALTAR_ADULT:
            if (rndCtx->GetOption(RSK_TOT_ALTAR_HINT)) {
                OpenForeignHint(RH_ALTAR_ADULT);
            }
            break;
        case TEXT_SARIA_SFM:
        case TEXT_SARIAS_SONG_TALK_SARIA_AGAIN:
            if (rndCtx->GetOption(RSK_SARIA_HINT)) {
                OpenForeignHint(RH_SARIA_HINT);
            }
            break;
        case TEXT_MIDO_SPEAK_TO_MIDO_FIRST_TIME:
        case TEXT_MIDO_SPEAK_TO_MIDO_AGAIN:
        case TEXT_MIDO_HOME_AFTER_ZELDAS_LETTER:
        case TEXT_MIDO_HOME_BEFORE_ZELDAS_LETTER:
            if (rndCtx->GetOption(RSK_MIDO_HINT)) {
                OpenForeignHint(RH_MIDO_HINT);
            }
            break;
        case TEXT_FISHING_POND_START:
        case TEXT_FISHING_POND_START_MET:
            if (rndCtx->GetOption(RSK_FISHING_POLE_HINT)) {
                OpenForeignHint(RH_FISHING_POLE);
            }
            break;
        case TEXT_NEED_SPECIAL_KEY: {
            if (rndCtx->GetOption(RSK_BOSS_KEY_HINT)) {
                switch (gPlayState->sceneNum) {
                    case SCENE_FOREST_TEMPLE:
                        OpenForeignHint(RH_FOREST_BOSS_KEY_HINT);
                        break;
                    case SCENE_FIRE_TEMPLE:
                        OpenForeignHint(RH_FIRE_BOSS_KEY_HINT);
                        break;
                    case SCENE_WATER_TEMPLE:
                        OpenForeignHint(RH_WATER_BOSS_KEY_HINT);
                        break;
                    case SCENE_SHADOW_TEMPLE:
                        OpenForeignHint(RH_SHADOW_BOSS_KEY_HINT);
                        break;
                    case SCENE_SPIRIT_TEMPLE:
                        OpenForeignHint(RH_SPIRIT_BOSS_KEY_HINT);
                        break;
                    case SCENE_GANONS_TOWER:
                        OpenForeignHint(RH_GANONS_BOSS_KEY_HINT);
                        break;
                }
            }
            break;
        }
    }
}

void ArchipelagoClient::OnShopSlotChangeHook(uint8_t cursorIndex) {
    if (!IsConnected()) {
        return;
    }

    if (gPlayState->sceneNum == SCENE_HAPPY_MASK_SHOP) {
        return;
    }

    int slot = CheckTracker::GetStartingShopItem(gPlayState->sceneNum) + cursorIndex;
    if (CheckTracker::GetCheckArea() == RCAREA_KAKARIKO_VILLAGE && gPlayState->sceneNum == SCENE_BAZAAR) {
        slot = RC_KAK_BAZAAR_ITEM_1 + cursorIndex;
    }
    OpenLocalHint(static_cast<RandomizerCheck>(slot));
}

bool ArchipelagoClient::slotMatch(const std::string& slotName, const std::string& roomHash) {
    if (apClient == nullptr) {
        return false;
    }

    if (disconnecting) {
        return false;
    }

    const std::string seed = apClient->get_seed();
    const std::string slot = GetSlotName();

    const bool seedMatch = apClient->get_seed().compare(roomHash) == 0;
    const bool slotMatch = GetSlotName().compare(slotName) == 0;
    return seedMatch && slotMatch;
}

bool ArchipelagoClient::isRightSaveLoaded() const {
    const bool seedMatch = apClient->get_seed().compare(gSaveContext.ship.quest.data.archipelago.roomHash) == 0;
    const bool slotMatch = GetSlotName().compare(gSaveContext.ship.quest.data.archipelago.slotName) == 0;
    return seedMatch && slotMatch;
}

int ArchipelagoClient::GetSlot() const {
    if (apClient == nullptr) {
        return -1;
    }

    return apClient->get_player_number();
}

const std::string ArchipelagoClient::GetSlotName() const {
    if (apClient == nullptr) {
        return "";
    }

    return apClient->get_slot();
}

const std::string ArchipelagoClient::GetAlias() const {
    if (apClient == nullptr) {
        return "";
    }

    return apClient->get_player_alias(apClient->get_player_number());
}

const nlohmann::json ArchipelagoClient::GetSlotData() {
    return slotData;
}

int ArchipelagoClient::GetHintCost() const {
    if (!IsConnected()) {
        return 0;
    }
    return apClient->get_hint_cost_points();
}

int ArchipelagoClient::GetHintPoints() const {
    if (!IsConnected()) {
        return 0;
    }
    return apClient->get_hint_points();
}

const std::vector<ArchipelagoClient::ApItem>& ArchipelagoClient::GetScoutedItems() {
    return scoutedItems;
}

void ArchipelagoClient::UpdateHintStatus(int player, int location, AP_Hint::HintStatus status) {
    if (!IsConnected()) {
        return;
    }

    APClient::HintStatus ap_status = APClient::HINT_UNSPECIFIED;
    switch (status) {
        case AP_Hint::HintStatus::HINT_AVOID:
            ap_status = APClient::HINT_AVOID;
            break;
        case AP_Hint::HintStatus::HINT_FOUND:
            ap_status = APClient::HINT_FOUND;
            break;
        case AP_Hint::HintStatus::HINT_NO_PRIORITY:
            ap_status = APClient::HINT_NO_PRIORITY;
            break;
        case AP_Hint::HintStatus::HINT_PRIORITY:
            ap_status = APClient::HINT_PRIORITY;
            break;
        case AP_Hint::HintStatus::HINT_UNSPECIFIED:
            ap_status = APClient::HINT_UNSPECIFIED;
            break;
    }

    apClient->UpdateHint(player, location, ap_status);
}

uint8_t ArchipelagoClient::GetConnectionStatus() {
    if (!apClient) {
        return (uint8_t)APClient::State::DISCONNECTED;
    } else {
        return (uint8_t)apClient->get_state();
    }
}

void ArchipelagoClient::OnItemGiven(uint32_t rc, GetItemEntry gi, uint8_t isGiSkipped) {
    if (rc == RC_ARCHIPELAGO_RECEIVED_ITEM) {
        gSaveContext.ship.quest.data.archipelago.lastReceivedItemIndex++;
        ArchipelagoClient::GetInstance().itemQueued = false;
    } else {
        ArchipelagoClient::GetInstance().CheckLocation((RandomizerCheck)rc);

        if (isGiSkipped && gi.modIndex == MOD_RANDOMIZER &&
            (gi.getItemId == RG_ARCHIPELAGO_ITEM_PROGRESSIVE || gi.getItemId == RG_ARCHIPELAGO_ITEM_USEFUL ||
             gi.getItemId == RG_ARCHIPELAGO_ITEM_JUNK)) {

            const char* itemIcon = "";
            switch (gi.getItemId) {
                case RG_ARCHIPELAGO_ITEM_PROGRESSIVE:
                    itemIcon = "Archipelago Progressive Icon";
                    break;
                case RG_ARCHIPELAGO_ITEM_USEFUL:
                    itemIcon = "Archipelago Useful Icon";
                    break;
                case RG_ARCHIPELAGO_ITEM_JUNK:
                    itemIcon = "Archipelago Junk Icon";
                    break;
            }

            Notification::Emit(
                { .itemIcon = itemIcon,
                  .prefix = std::string(gSaveContext.ship.quest.data.archipelago.locations[rc].itemName),
                  .message = " for ",
                  .suffix = std::string(gSaveContext.ship.quest.data.archipelago.locations[rc].playerName) });
        }
    }
}

void ArchipelagoClient::SendDeathLink() {
    uint64_t currentTime = GetUnixTimestamp();
    if (apClient != nullptr && CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("DeathLink"), 0) &&
        (currentTime - lastDeathLink) > 10000) {
        nlohmann::json data{ { "time", apClient->get_server_time() },
                             { "cause", "Shipwrecked by King Harkinian." },
                             { "source", apClient->get_slot() } };
        apClient->Bounce(data, {}, {}, { "DeathLink" });

        Notification::Emit({ .message = "Sending Death Link" });
        ArchipelagoConsole_SendMessage("[LOG] Died, sending death link.");
    }
}

void ArchipelagoClient::SendDamageLink(int16_t amount) {
    if (apClient != nullptr && CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("DamageLink"), 0)) {
        // Every 80 points is 16 health. Don't emit anything under a quarter heart damage or if the player is getting
        // healed.
        if (amount <= -4) {
            uint16_t damagePoints = amount * -5;
            nlohmann::json data{ { "time", apClient->get_server_time() },
                                 { "uuid", apClient->get_player_number() },
                                 { "source", apClient->get_slot() },
                                 { "damage_points", damagePoints } };
            apClient->Bounce(data, {}, {}, { "SharedDamage" });

            ArchipelagoConsole_SendMessage("[LOG] Took damage, sending damage link.");
        }
    }
}

void ArchipelagoClient::SendTrapLink() {
    if (apClient != nullptr && CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("TrapLink"), 0)) {
        if (trapLinkCount > 0) {
            --trapLinkCount;
        } else {
            nlohmann::json data{ { "time", apClient->get_server_time() },
                                 { "source", apClient->get_slot() },
                                 { "trap_name", "Ice Trap" } };
            apClient->Bounce(data, {}, {}, { "TrapLink" });

            ArchipelagoConsole_SendMessage("[LOG] Recieved trap, sending trap link.");
        }
    }
}

void ArchipelagoClient::SetTags() {
    if (!ArchipelagoClient::IsConnected()) {
        return;
    }
    std::list<std::string> tags;
    if (CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("DeathLink"), 0)) {
        tags.push_back("DeathLink");
    }
    if (CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("DamageLink"), 0)) {
        tags.push_back("SharedDamage");
    }
    if (CVarGetInteger(CVAR_REMOTE_ARCHIPELAGO("TrapLink"), 0)) {
        tags.push_back("TrapLink");
    }
    apClient->ConnectUpdate(false, 1, true, tags);
}

std::vector<RandomizerGet> archipelagoIceTrapModels = {
    RG_MIRROR_SHIELD,
    RG_BOOMERANG,
    RG_LENS_OF_TRUTH,
    RG_MEGATON_HAMMER,
    RG_IRON_BOOTS,
    RG_HOVER_BOOTS,
    RG_STONE_OF_AGONY,
    RG_DINS_FIRE,
    RG_FARORES_WIND,
    RG_NAYRUS_LOVE,
    RG_FIRE_ARROWS,
    RG_ICE_ARROWS,
    RG_LIGHT_ARROWS,
    RG_DOUBLE_DEFENSE,
    RG_CLAIM_CHECK,
    RG_PROGRESSIVE_HOOKSHOT,
    RG_PROGRESSIVE_STRENGTH,
    RG_PROGRESSIVE_BOMB_BAG,
    RG_PROGRESSIVE_BOW,
    RG_PROGRESSIVE_SLINGSHOT,
    RG_PROGRESSIVE_WALLET,
    RG_PROGRESSIVE_SCALE,
    RG_PROGRESSIVE_MAGIC_METER,
};

RandomizerGet ArchipelagoClient::GetIceTrapItem() {
    return RandomElement(archipelagoIceTrapModels);
}

std::string ArchipelagoClient::GetApItemName(int64_t ApItemId) {
    if (!IsConnected()) {
        return "";
    }
    return apClient->get_item_name(ApItemId, apClient->get_game());
}

std::string ArchipelagoClient::GetApItemHint(RandomizerCheck rc, RandomizerGet rg) {
    std::string player_name = gSaveContext.ship.quest.data.archipelago.locations[rc].playerName;
    std::string item_hint = "Unknown Item";

    const auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_CLEAR) || Rando::StaticData::GetLocation(rc)->IsShop()) {
        item_hint = gSaveContext.ship.quest.data.archipelago.locations[rc].itemName;
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)) {
        item_hint = gSaveContext.ship.quest.data.archipelago.locations[rc].hintName;
    } else {
        switch (rg) {
            case RandomizerGet::RG_ARCHIPELAGO_ITEM_JUNK:
                item_hint = "Junk";
                break;
            case RandomizerGet::RG_ARCHIPELAGO_ITEM_USEFUL:
                item_hint = "Item";
                break;
            case RandomizerGet::RG_ARCHIPELAGO_ITEM_PROGRESSIVE:
                item_hint = "Progress Item";
                break;
            default:
                // keep as "Unknown Item"
                break;
        };
    }

    return item_hint + " for " + player_name;
}

std::string ArchipelagoClient::GetApLocationHint(RandomizerHint rh, uint8_t index) {
    ApForeignHint hintData = foreignHints[rh][index];
    std::string& location_name = hintData.locationName;
    std::string& player_name = hintData.playerName;
    std::string& location_group = hintData.groupName;

    if (!player_name.empty()) {
        if (player_name.back() == 's') {
            player_name += "' ";
        } else {
            player_name += "'s ";
        }
    }

    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_CLEAR)) {
        return player_name + location_name;
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)) {
        return player_name + location_group;
    }
    return player_name + "world";
}

extern "C" void Archipelago_InitSaveFile() {
    gSaveContext.ship.quest.data.archipelago.isArchipelago = 1;

    nlohmann::json slotData = ArchipelagoClient::GetInstance().GetSlotData();

    std::vector<ArchipelagoClient::ApItem> scoutedItems = ArchipelagoClient::GetInstance().GetScoutedItems();

    ArchipelagoClient& client = ArchipelagoClient::GetInstance();
    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.roomHash, client.apClient->get_seed(),
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.roomHash));
    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.slotName, client.apClient->get_slot(),
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.slotName));
    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.archiUri, client.uri,
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.archiUri));
    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.roomPass, client.password,
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.roomPass));

    for (uint32_t i = 0; i < scoutedItems.size(); i++) {
        RandomizerCheck rc = Rando::StaticData::locationNameToEnum[scoutedItems[i].locationName];

        SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.locations[rc].itemName,
                                        scoutedItems[i].itemName,
                                        ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[rc].itemName));
        SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.locations[rc].hintName,
                                        scoutedItems[i].hintName,
                                        ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[rc].hintName));
        SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.locations[rc].playerName,
                                        scoutedItems[i].playerName,
                                        ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[rc].playerName));
    }
}

extern "C" void Archipelago_InitConnection() {
    ArchipelagoClient::GetInstance().StartClient();
}

extern "C" void Archipelago_RequestInitData() {
    ArchipelagoClient::GetInstance().RequestInitData();
}

void SetArchipelagoParsing(uint8_t state) {
    isArchipelagoParsing = state;
}

uint8_t IsArchipelagoParsing() {
    return isArchipelagoParsing;
}

void LoadArchipelagoData() {
    SaveManager::Instance->LoadData("isArchipelago", gSaveContext.ship.quest.data.archipelago.isArchipelago);
    SaveManager::Instance->LoadData("lastReceivedItemIndex",
                                    gSaveContext.ship.quest.data.archipelago.lastReceivedItemIndex);

    SaveManager::Instance->LoadCharArray("roomHash", gSaveContext.ship.quest.data.archipelago.roomHash,
                                         ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.roomHash));
    SaveManager::Instance->LoadCharArray("slotName", gSaveContext.ship.quest.data.archipelago.slotName,
                                         ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.slotName));
    SaveManager::Instance->LoadCharArray("archiUri", gSaveContext.ship.quest.data.archipelago.archiUri,
                                         ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.archiUri));
    SaveManager::Instance->LoadCharArray("roomPass", gSaveContext.ship.quest.data.archipelago.roomPass,
                                         ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.roomPass));

    SaveManager::Instance->LoadArray(
        "locations", ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations), [](size_t i) {
            SaveManager::Instance->LoadStruct("", [&i]() {
                SaveManager::Instance->LoadCharArray(
                    "itemName", gSaveContext.ship.quest.data.archipelago.locations[i].itemName,
                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[i].itemName));
                SaveManager::Instance->LoadCharArray(
                    "hintName", gSaveContext.ship.quest.data.archipelago.locations[i].hintName,
                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[i].hintName));
                SaveManager::Instance->LoadCharArray(
                    "playerName", gSaveContext.ship.quest.data.archipelago.locations[i].playerName,
                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[i].playerName));
            });
        });

    SaveManager::Instance->LoadArray("hints", RH_MAX, [&](size_t hintKey) {
        nlohmann::json json;
        SaveManager::Instance->LoadData("", json);
        std::vector<ArchipelagoClient::ApForeignHint> loadedHints;
        std::string d = json.dump();
        for (auto foreignHintData : json["ForeignLocation"]) {
            ArchipelagoClient::ApForeignHint hint;
            hint.locationId = foreignHintData["LocationId"];
            hint.playerId = foreignHintData["PlayerId"];
            hint.locationName = foreignHintData["LocationName"];
            hint.groupName = foreignHintData["GroupName"];
            hint.playerName = foreignHintData["PlayerName"];
            loadedHints.push_back(hint);
        };
        ArchipelagoClient::GetInstance().foreignHints[static_cast<RandomizerHint>(hintKey)] = loadedHints;
    });
}

void SaveArchipelagoData(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("isArchipelago", saveContext->ship.quest.data.archipelago.isArchipelago);
    SaveManager::Instance->SaveData("lastReceivedItemIndex",
                                    saveContext->ship.quest.data.archipelago.lastReceivedItemIndex);

    SaveManager::Instance->SaveData("roomHash", saveContext->ship.quest.data.archipelago.roomHash);
    SaveManager::Instance->SaveData("slotName", saveContext->ship.quest.data.archipelago.slotName);
    SaveManager::Instance->SaveData("archiUri", saveContext->ship.quest.data.archipelago.archiUri);
    SaveManager::Instance->SaveData("roomPass", gSaveContext.ship.quest.data.archipelago.roomPass);

    SaveManager::Instance->SaveArray(
        "locations", ARRAY_COUNT(saveContext->ship.quest.data.archipelago.locations), [&](size_t i) {
            SaveManager::Instance->SaveStruct("", [&]() {
                SaveManager::Instance->SaveData("itemName",
                                                saveContext->ship.quest.data.archipelago.locations[i].itemName);
                SaveManager::Instance->SaveData("hintName",
                                                saveContext->ship.quest.data.archipelago.locations[i].hintName);
                SaveManager::Instance->SaveData("playerName",
                                                saveContext->ship.quest.data.archipelago.locations[i].playerName);
            });
        });

    SaveManager::Instance->SaveArray("hints", RH_MAX, [&](size_t hintKey) {
        const std::vector<ArchipelagoClient::ApForeignHint>& hints =
            ArchipelagoClient::GetInstance().foreignHints[(RandomizerHint)hintKey];
        SaveManager::Instance->SaveStruct("", [&]() {
            SaveManager::Instance->SaveData("HintKey",
                                            Rando::StaticData::hintNames[(uint32_t)hintKey].GetEnglish(MF_CLEAN));
            SaveManager::Instance->SaveArray("ForeignLocation", hints.size(), [&](size_t i) {
                SaveManager::Instance->SaveStruct("", [&]() {
                    SaveManager::Instance->SaveData("LocationId", hints[i].locationId);
                    SaveManager::Instance->SaveData("GroupName", hints[i].groupName);
                    SaveManager::Instance->SaveData("LocationName", hints[i].locationName);
                    SaveManager::Instance->SaveData("PlayerId", hints[i].playerId);
                    SaveManager::Instance->SaveData("PlayerName", hints[i].playerName);
                });
            });
        });
    });
}

void InitArchipelagoData(bool isDebug) {
    gSaveContext.ship.quest.data.archipelago.isArchipelago = 0;
    gSaveContext.ship.quest.data.archipelago.lastReceivedItemIndex = 0;

    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.roomHash, "",
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.roomHash));
    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.slotName, "",
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.slotName));
    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.archiUri, "",
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.archiUri));
    SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.roomPass, "",
                                    ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.roomPass));

    for (uint32_t i = 0; i < ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations); i++) {
        SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.locations[i].itemName, "",
                                        ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[i].itemName));
        SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.locations[i].hintName, "",
                                        ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[i].hintName));
        SohUtils::CopyStringToCharArray(gSaveContext.ship.quest.data.archipelago.locations[i].playerName, "",
                                        ARRAY_COUNT(gSaveContext.ship.quest.data.archipelago.locations[i].playerName));
    }
}

void RegisterArchipelago() {
    // make sure the client is constructed
    ArchipelagoClient::GetInstance();

    COND_HOOK(GameInteractor::OnGameFrameUpdate, true, []() { ArchipelagoClient::GetInstance().Poll(); });

    COND_HOOK(GameInteractor::PostLoadGame, true,
              [](int32_t file_id) { ArchipelagoClient::GetInstance().GameLoaded(); });

    COND_HOOK(GameInteractor::OnRandomizerItemGivenHooks, IS_ARCHIPELAGO,
              [](uint32_t rc, GetItemEntry gi, uint8_t isGiSkipped) {
                  ArchipelagoClient::GetInstance().OnItemGiven(rc, gi, isGiSkipped);
              });

    COND_HOOK(GameInteractor::OnPlayerDeath, IS_ARCHIPELAGO,
              []() { ArchipelagoClient::GetInstance().SendDeathLink(); });

    COND_HOOK(GameInteractor::OnPlayerHealthChange, IS_ARCHIPELAGO,
              [](int16_t amount) { ArchipelagoClient::GetInstance().SendDamageLink(amount); });

    COND_HOOK(GameInteractor::OnItemReceive, IS_ARCHIPELAGO, [](GetItemEntry itemEntry) {
        // If item Received is an Ice Trap, send a Trap Link
        if (itemEntry.itemId == RG_ICE_TRAP) {
            ArchipelagoClient::GetInstance().SendTrapLink();
        }
    });

    COND_HOOK(GameInteractor::OnSceneInit, IS_ARCHIPELAGO,
              [](int16_t sceneNum) { ArchipelagoClient::GetInstance().OnSceneInit(sceneNum); });

    COND_HOOK(GameInteractor::OnDialogClose, IS_ARCHIPELAGO,
              []() { ArchipelagoClient::GetInstance().OnDialogCloseHook(); });
    COND_HOOK(GameInteractor::OnShopSlotChange, IS_ARCHIPELAGO, [](uint8_t cursorIndex, int16_t price) {
        ArchipelagoClient::GetInstance().OnShopSlotChangeHook(cursorIndex);
    });
}

static RegisterShipInitFunc initFunc(RegisterArchipelago, { "IS_ARCHIPELAGO" });

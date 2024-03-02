#include "Archipelago.h"

#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXUserAgent.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <fstream>
#include "json/json.h"
#include "json/reader.h"
#include "json/value.h"
#include "json/writer.h"
#include <deque>
#include <set>
#include <string>
#include <chrono>
#include <functional>
#include <utility>
#include <vector>

constexpr int AP_OFFLINE_SLOT = 1404;
#define AP_OFFLINE_NAME "You"

//Setup Stuff
bool init = false;
bool auth = false;
bool refused = false;
bool multiworld = true;
bool isSSL = true;
bool ssl_success = false;
int ap_player_id;
std::string ap_player_name;
size_t ap_player_name_hash;
std::string ap_ip;
std::string ap_game;
std::string ap_passwd;
std::uint64_t ap_uuid = 0;
std::mt19937 rando;
AP_NetworkVersion client_version = {0,2,6}; // Default for compatibility reasons

//Deathlink Stuff
bool deathlinkstat = false;
bool deathlinksupported = false;
bool enable_deathlink = false;
int deathlink_amnesty = 0;
int cur_deathlink_amnesty = 0;

// Message System
std::deque<AP_Message*> messageQueue;
bool queueitemrecvmsg = true;

// Data Maps
std::map<int, AP_NetworkPlayer> map_players;
std::map<std::pair<std::string,int64_t>, std::string> map_location_id_name;
std::map<std::pair<std::string,int64_t>, std::string> map_item_id_name;

// Callback function pointers
void (*resetItemValues)();
void (*getitemfunc)(int64_t,bool);
void (*checklocfunc)(int64_t);
void (*locinfofunc)(std::vector<AP_NetworkItem>) = nullptr;
void (*recvdeath)() = nullptr;
void (*setreplyfunc)(AP_SetReply) = nullptr;

// Serverdata Management
std::map<std::string,AP_DataType> map_serverdata_typemanage;
AP_GetServerDataRequest resync_serverdata_request;
size_t last_item_idx = 0;

// Singleplayer Seed Info
std::string sp_save_path;
Json::Value sp_save_root;

//Misc Data for Clients
AP_RoomInfo lib_room_info;

//Server Data Stuff
std::map<std::string, AP_GetServerDataRequest*> map_server_data;

//Slot Data Stuff
std::map<std::string, void (*)(int)> map_slotdata_callback_int;
std::map<std::string, void (*)(std::string)> map_slotdata_callback_raw;
std::map<std::string, void (*)(std::map<int,int>)> map_slotdata_callback_mapintint;
std::vector<std::string> slotdata_strings;

// Datapackage Stuff
std::string const datapkg_cache_path = "APCpp_datapkg.cache";
Json::Value datapkg_cache;
std::set<std::string> datapkg_outdated_games;

ix::WebSocket webSocket;
Json::Reader reader;
Json::FastWriter writer;

Json::Value sp_ap_root;

// PRIV Func Declarations Start
void AP_Init_Generic();
bool parse_response(std::string msg, std::string &request);
void APSend(std::string req);
void WriteFileJSON(Json::Value val, std::string path);
std::string getItemName(std::string game, int64_t id);
std::string getLocationName(std::string game, int64_t id);
void parseDataPkg(Json::Value new_datapkg);
void parseDataPkg();
AP_NetworkPlayer getPlayer(int team, int slot);
// PRIV Func Declarations End

void AP_Init(const char* ip, const char* game, const char* player_name, const char* passwd) {
    multiworld = true;
    
    uint64_t milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    rando = std::mt19937(milliseconds_since_epoch);

    if (!strcmp(ip,"")) {
        ip = "archipelago.gg:38281";
        printf("AP: Using default Server Adress: '%s'\n", ip);
    } else {
        printf("AP: Using Server Adress: '%s'\n", ip);
    }
    ap_ip = ip;
    ap_game = game;
    ap_player_name = player_name;
    ap_passwd = passwd;

    printf("AP: Initializing...\n");

    //Connect to server
    ix::initNetSystem();
    webSocket.setUrl("wss://" + ap_ip);
    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                std::string request;
                if (parse_response(msg->str, request)) {
                    APSend(request);
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Open)
            {
                printf("AP: Connected to Archipelago\n");
            }
            else if (msg->type == ix::WebSocketMessageType::Error || msg->type == ix::WebSocketMessageType::Close)
            {
                auth = false;
                for (std::pair<std::string,AP_GetServerDataRequest*> itr : map_server_data) {
                    itr.second->status = AP_RequestStatus::Error;
                    map_server_data.erase(itr.first);
                }
                printf("AP: Error connecting to Archipelago. Retries: %d\n", msg->errorInfo.retries-1);
                if (msg->errorInfo.retries-1 >= 2 && isSSL && !ssl_success) {
                    printf("AP: SSL connection failed. Attempting unencrypted...\n");
                    webSocket.setUrl("ws://" + ap_ip);
                    isSSL = false;
                }
            }
        }
    );
    webSocket.setPingInterval(45);

    AP_NetworkPlayer archipelago {
        -1,
        0,
        "Archipelago",
        "Archipelago",
        "__Server"

    };
    map_players[0] = archipelago;
    AP_Init_Generic();
}

void AP_Init(const char* filename) {
    multiworld = false;
    std::ifstream mwfile(filename);
    reader.parse(mwfile,sp_ap_root);
    mwfile.close();
    sp_save_path = std::string(filename) + ".save";
    std::ifstream savefile(sp_save_path);
    reader.parse(savefile, sp_save_root);
    savefile.close();
    WriteFileJSON(sp_save_root, sp_save_path);
    ap_player_name = AP_OFFLINE_NAME;
    AP_Init_Generic();
}

void AP_Start() {
    init = true;
    if (multiworld) {
        webSocket.start();
    } else {
        if (!sp_save_root.get("init", false).asBool()) {
            sp_save_root["init"] = true;
            sp_save_root["checked_locations"] = Json::arrayValue;
            sp_save_root["store"] = Json::objectValue;
        }
        // Only game in the data package is our game
        ap_game = sp_ap_root["data_package"]["data"]["games"].getMemberNames()[0];
        Json::Value fake_msg;
        fake_msg[0]["cmd"] = "Connected";
        fake_msg[0]["slot"] = AP_OFFLINE_SLOT;
        fake_msg[0]["slot_info"][std::to_string(AP_OFFLINE_SLOT)]["game"] = ap_game;
        fake_msg[0]["players"] = Json::arrayValue;
        fake_msg[0]["players"][0]["team"] = 0;
        fake_msg[0]["players"][0]["slot"] = AP_OFFLINE_SLOT;
        fake_msg[0]["players"][0]["alias"] = AP_OFFLINE_NAME;
        fake_msg[0]["players"][0]["name"] = AP_OFFLINE_NAME;
        fake_msg[0]["checked_locations"] = sp_save_root["checked_locations"];
        fake_msg[0]["slot_data"] = sp_ap_root["slot_data"];
        std::string req;
        parse_response(writer.write(fake_msg), req);
        fake_msg.clear();
        fake_msg[0]["cmd"] = "DataPackage";
        fake_msg[0]["data"] = sp_ap_root["data_package"]["data"];
        parse_response(writer.write(fake_msg), req);
        fake_msg.clear();
        fake_msg[0]["cmd"] = "ReceivedItems";
        fake_msg[0]["index"] = 0;
        fake_msg[0]["items"] = Json::arrayValue;
        for (unsigned int i = 0; i < sp_save_root["checked_locations"].size(); i++) {
            Json::Value item;
            item["item"] = sp_ap_root["location_to_item"][sp_save_root["checked_locations"][i].asString()].asInt64();
            item["location"] = 0;
            item["player"] = ap_player_id;
            fake_msg[0]["items"].append(item);
        }
        parse_response(writer.write(fake_msg), req);
    }
}

void AP_Shutdown() {
    webSocket.stop();

    // Reset all states
    init = false;
    auth = false;
    refused = false;
    multiworld = true;
    isSSL = true;
    ssl_success = false;
    ap_player_id = 0;
    ap_player_name.clear();
    ap_ip.clear();
    ap_game.clear();
    ap_passwd.clear();
    ap_uuid = 0;
    rando = std::mt19937();
    client_version = {0,2,6};
    deathlinkstat = false;
    deathlinksupported = false;
    enable_deathlink = false;
    deathlink_amnesty = 0;
    cur_deathlink_amnesty = 0;
    while (AP_IsMessagePending()) AP_ClearLatestMessage();
    queueitemrecvmsg = true;
    map_players.clear();
    map_location_id_name.clear();
    map_item_id_name.clear();
    resetItemValues = nullptr;
    getitemfunc = nullptr;
    checklocfunc = nullptr;
    locinfofunc = nullptr;
    recvdeath = nullptr;
    setreplyfunc = nullptr;
    map_serverdata_typemanage.clear();
    int last_item_idx = 0;
    sp_save_path.clear();
    sp_save_root.clear();
    map_server_data.clear(); // Does this leak?
    map_slotdata_callback_int.clear();
    map_slotdata_callback_raw.clear();
    map_slotdata_callback_mapintint.clear();
    slotdata_strings.clear();
    datapkg_cache = Json::objectValue;
    sp_ap_root = Json::objectValue;
}

bool AP_IsInit() {
    return init;
}

void AP_SetClientVersion(AP_NetworkVersion* version) {
    client_version.major = version->major;
    client_version.minor = version->minor;
    client_version.build = version->build;
}

void AP_SendItem(int64_t idx) {
    AP_SendItem(std::set<int64_t>{ idx });
}
void AP_SendItem(std::set<int64_t> const& locations) {
    for (int64_t idx : locations) {
        printf("AP: Checked '%s'.\n", getLocationName(ap_game, idx).c_str());
    }
    if (multiworld) {
        Json::Value req_t;
        req_t[0]["cmd"] = "LocationChecks";
        req_t[0]["locations"] = Json::arrayValue;
        for (int64_t loc : locations) {
            req_t[0]["locations"].append(loc);
        };
        APSend(writer.write(req_t));
    } else {
        std::set<int64_t> new_locations;
        for (int64_t idx : locations) {
            bool was_previously_checked = false;
            for (auto itr : sp_save_root["checked_locations"]) {
                if (itr.asInt64() == idx) {
                    was_previously_checked = true;
                    break;
                }
            }
            if (!was_previously_checked) new_locations.insert(idx);
        }

        Json::Value fake_msg;
        fake_msg[0]["cmd"] = "ReceivedItems";
        fake_msg[0]["index"] = last_item_idx+1;
        fake_msg[0]["items"] = Json::arrayValue;
        for (int64_t idx : new_locations) {
            int64_t recv_item_id = sp_ap_root["location_to_item"].get(std::to_string(idx), 0).asInt64();
            if (recv_item_id == 0) continue;
            Json::Value item;
            item["item"] = recv_item_id;
            item["location"] = idx;
            item["player"] = ap_player_id;
            fake_msg[0]["items"].append(item);
        }
        std::string req;
        parse_response(writer.write(fake_msg), req);

        fake_msg.clear();
        fake_msg[0]["cmd"] = "RoomUpdate";
        fake_msg[0]["checked_locations"] = Json::arrayValue;
        for (int64_t idx : new_locations) {
            fake_msg[0]["checked_locations"].append(idx);
            sp_save_root["checked_locations"].append(idx);
        }
        WriteFileJSON(sp_save_root, sp_save_path);
        parse_response(writer.write(fake_msg), req);
    }
}

void AP_SendLocationScouts(std::set<int64_t> const& locations, int create_as_hint) {
    if (multiworld) {
        Json::Value req_t;
        req_t[0]["cmd"] = "LocationScouts";
        req_t[0]["locations"] = Json::arrayValue;
        for (int64_t loc : locations) {
            req_t[0]["locations"].append(loc);
        }
        req_t[0]["create_as_hint"] = create_as_hint;
        APSend(writer.write(req_t));
    } else {
        Json::Value fake_msg;
        fake_msg[0]["cmd"] = "LocationInfo";
        fake_msg[0]["locations"] = Json::arrayValue;
        for (int64_t loc : locations) {
            Json::Value netitem;
            netitem["item"] = sp_ap_root["location_to_item"].get(std::to_string(loc), 0).asInt64();
            netitem["location"] = loc;
            netitem["player"] = ap_player_id;
            netitem["flags"] = 0b001; // Hardcoded for SP seeds.
            fake_msg[0]["locations"].append(netitem);
        }
    }
}

void AP_StoryComplete() {
    if (!multiworld) return;
    Json::Value req_t;
    req_t[0]["cmd"] = "StatusUpdate";
    req_t[0]["status"] = 30; //CLIENT_GOAL
    APSend(writer.write(req_t));
}

void AP_DeathLinkSend() {
    if (!enable_deathlink || !multiworld) return;
    if (cur_deathlink_amnesty > 0) {
        cur_deathlink_amnesty--;
        return;
    }
    cur_deathlink_amnesty = deathlink_amnesty;
    std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
    Json::Value req_t;
    req_t[0]["cmd"] = "Bounce";
    req_t[0]["data"]["time"] = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
    req_t[0]["data"]["source"] = ap_player_name; // Name and Shame >:D
    req_t[0]["tags"][0] = "DeathLink";
    APSend(writer.write(req_t));
}

void AP_EnableQueueItemRecvMsgs(bool b) {
    queueitemrecvmsg = b;
}

void AP_SetItemClearCallback(void (*f_itemclr)()) {
    resetItemValues = f_itemclr;
}

void AP_SetItemRecvCallback(void (*f_itemrecv)(int64_t,bool)) {
    getitemfunc = f_itemrecv;
}

void AP_SetLocationCheckedCallback(void (*f_loccheckrecv)(int64_t)) {
    checklocfunc = f_loccheckrecv;
}

void AP_SetLocationInfoCallback(void (*f_locinfrecv)(std::vector<AP_NetworkItem>)) {
    locinfofunc = f_locinfrecv;
}

void AP_SetDeathLinkRecvCallback(void (*f_deathrecv)()) {
    recvdeath = f_deathrecv;
}

void AP_RegisterSlotDataIntCallback(std::string key, void (*f_slotdata)(int)) {
    map_slotdata_callback_int[key] = f_slotdata;
    slotdata_strings.push_back(key);
}

void AP_RegisterSlotDataRawCallback(std::string key, void (*f_slotdata)(std::string)) {
    map_slotdata_callback_raw[key] = f_slotdata;
    slotdata_strings.push_back(key);
}

void AP_RegisterSlotDataMapIntIntCallback(std::string key, void (*f_slotdata)(std::map<int,int>)) {
    map_slotdata_callback_mapintint[key] = f_slotdata;
    slotdata_strings.push_back(key);
}

void AP_SetDeathLinkSupported(bool supdeathlink) {
    deathlinksupported = supdeathlink;
}

bool AP_DeathLinkPending() {
    return deathlinkstat;
}

void AP_DeathLinkClear() {
    deathlinkstat = false;
}

bool AP_IsMessagePending() {
    return !messageQueue.empty();
}

AP_Message* AP_GetLatestMessage() {
    return messageQueue.front();
}

void AP_ClearLatestMessage() {
    if (AP_IsMessagePending()) {
        delete messageQueue.front();
        messageQueue.pop_front();
    }
}

void AP_Say(std::string text) {
    Json::Value req_t;
    req_t[0]["cmd"] = "Say";
    req_t[0]["text"] = text;
    APSend(writer.write(req_t));
}

int AP_GetRoomInfo(AP_RoomInfo* client_roominfo) {
    if (!auth) return 1;
    *client_roominfo = lib_room_info;
    return 0;
}

AP_ConnectionStatus AP_GetConnectionStatus() {
    if (!multiworld && auth) return AP_ConnectionStatus::Authenticated;
    if (refused) {
        return AP_ConnectionStatus::ConnectionRefused;
    }
    if (webSocket.getReadyState() == ix::ReadyState::Open) {
        if (auth) {
            return AP_ConnectionStatus::Authenticated;
        } else {
            return AP_ConnectionStatus::Connected;
        }
    }
    return AP_ConnectionStatus::Disconnected;
}

int AP_GetUUID() {
    return ap_uuid;
}

int AP_GetPlayerID() {
    return ap_player_id;
}

void AP_SetServerData(AP_SetServerDataRequest* request) {
    request->status = AP_RequestStatus::Pending;

    Json::Value req_t;
    req_t[0]["cmd"] = "Set";
    req_t[0]["key"] = request->key;
    switch (request->type) {
        case AP_DataType::Int:
            for (int i = 0; i < request->operations.size(); i++) {
                req_t[0]["operations"][i]["operation"] = request->operations[i].operation;
                req_t[0]["operations"][i]["value"] = *((int*)request->operations[i].value);
            }
            break;
        case AP_DataType::Double:
            for (int i = 0; i < request->operations.size(); i++) {
                req_t[0]["operations"][i]["operation"] = request->operations[i].operation;
                req_t[0]["operations"][i]["value"] = *((double*)request->operations[i].value);
            }
            break;
        default:
            for (int i = 0; i < request->operations.size(); i++) {
                req_t[0]["operations"][i]["operation"] = request->operations[i].operation;
                Json::Value data;
                reader.parse((*(std::string*)request->operations[i].value), data);
                req_t[0]["operations"][i]["value"] = data;
            }
            Json::Value default_val_json;
            reader.parse(*((std::string*)request->default_value), default_val_json);
            req_t[0]["default"] = default_val_json;
            break;
    }
    req_t[0]["want_reply"] = request->want_reply;
    map_serverdata_typemanage[request->key] = request->type;
    APSend(writer.write(req_t));
    request->status = AP_RequestStatus::Done;
}

void AP_RegisterSetReplyCallback(void (*f_setreply)(AP_SetReply)) {
    setreplyfunc = f_setreply;
}

void AP_SetNotify(std::map<std::string,AP_DataType> keylist) {
    Json::Value req_t;
    req_t[0]["cmd"] = "SetNotify";
    int i = 0;
    for (std::pair<std::string,AP_DataType> keytypepair : keylist) {
        req_t[0]["keys"][i] = keytypepair.first;
        map_serverdata_typemanage[keytypepair.first] = keytypepair.second;
        i++;
    }
    APSend(writer.write(req_t));
}

void AP_SetNotify(std::string key, AP_DataType type) {
    std::map<std::string,AP_DataType> keylist;
    keylist[key] = type;
    AP_SetNotify(keylist);
}

void AP_GetServerData(AP_GetServerDataRequest* request) {
    request->status = AP_RequestStatus::Pending;

    if (map_server_data.find(request->key) != map_server_data.end()) return;

    map_server_data[request->key] = request;

    Json::Value req_t;
    req_t[0]["cmd"] = "Get";
    req_t[0]["keys"][0] = request->key;
    APSend(writer.write(req_t));
}

std::string AP_GetPrivateServerDataPrefix() {
    return "APCpp" + std::to_string(ap_player_name_hash) + "APCpp" + std::to_string(ap_player_id) + "APCpp";
}

// PRIV

void AP_Init_Generic() {
    ap_player_name_hash = std::hash<std::string>{}(ap_player_name);
    std::ifstream datapkg_cache_file(datapkg_cache_path);
    reader.parse(datapkg_cache_file,datapkg_cache);;
    datapkg_cache_file.close();
}

bool parse_response(std::string msg, std::string &request) {
    Json::Value root;
    reader.parse(msg, root);
    for (unsigned int i = 0; i < root.size(); i++) {
        std::string cmd = root[i]["cmd"].asString();
        if (cmd == "RoomInfo") {
            lib_room_info.version.major = root[i]["version"]["major"].asInt();
            lib_room_info.version.minor = root[i]["version"]["minor"].asInt();
            lib_room_info.version.build = root[i]["version"]["build"].asInt();
            std::vector<std::string> serv_tags;
            for (auto itr : root[i]["tags"]) {
                serv_tags.push_back(itr.asString());
            }
            lib_room_info.tags = serv_tags;
            lib_room_info.password_required = root[i]["password"].asBool();
            std::map<std::string,int> serv_permissions;
            for (auto itr : root[i]["permissions"].getMemberNames()) {
                serv_permissions[itr] = root[i]["permissions"][itr].asInt();
            }
            lib_room_info.permissions = serv_permissions;
            lib_room_info.hint_cost = root[i]["hint_cost"].asInt();
            lib_room_info.location_check_points = root[i]["location_check_points"].asInt();
            std::map<std::string,std::string> serv_datapkg_checksums;
            for (auto itr : root[i]["datapackage_checksums"].getMemberNames()) {
                serv_datapkg_checksums[itr] = root[i]["datapackage_checksums"][itr].asString();
            }
            lib_room_info.datapackage_checksums = serv_datapkg_checksums;
            lib_room_info.seed_name = root[i]["seed_name"].asString();
            lib_room_info.time = root[i]["time"].asFloat();

            if (!auth) {
                Json::Value req_t;
                ap_uuid = rando();
                req_t[0]["cmd"] = "Connect";
                req_t[0]["game"] = ap_game;
                req_t[0]["name"] = ap_player_name;
                req_t[0]["password"] = ap_passwd;
                req_t[0]["uuid"] = ap_uuid;
                req_t[0]["tags"] = Json::arrayValue;
                req_t[0]["version"]["major"] = client_version.major;
                req_t[0]["version"]["minor"] = client_version.minor;
                req_t[0]["version"]["build"] = client_version.build;
                req_t[0]["version"]["class"] = "Version";
                req_t[0]["items_handling"] = 7; // Full Remote
                request = writer.write(req_t);
                return true;
            }
        } else if (cmd == "Connected") {
            // Avoid inconsistency if we disconnected before
            (*resetItemValues)();
            auth = true;
            ssl_success = auth && isSSL;
            refused = false;

            printf("AP: Authenticated\n");
            ap_player_id = root[i]["slot"].asInt();
            for (unsigned int j = 0; j < root[i]["checked_locations"].size(); j++) {
                //Sync checks with server
                int64_t loc_id = root[i]["checked_locations"][j].asInt64();
                (*checklocfunc)(loc_id);
            }
            for (unsigned int j = 0; j < root[i]["players"].size(); j++) {
                AP_NetworkPlayer player = {
                    root[i]["players"][j]["team"].asInt(),
                    root[i]["players"][j]["slot"].asInt(),
                    root[i]["players"][j]["name"].asString(),
                    root[i]["players"][j]["alias"].asString(),
                    "PLACEHOLDER"
                };
                player.game = root[i]["slot_info"][std::to_string(player.slot)]["game"].asString();
                map_players[root[i]["players"][j]["slot"].asInt()] = player;
            }
            if ((root[i]["slot_data"].get("death_link", false).asBool() || root[i]["slot_data"].get("DeathLink", false).asBool()) && deathlinksupported) enable_deathlink = true;
            if (root[i]["slot_data"]["death_link_amnesty"] != Json::nullValue)
                deathlink_amnesty = root[i]["slot_data"].get("death_link_amnesty", 0).asInt();
            else if (root[i]["slot_data"]["DeathLink_Amnesty"] != Json::nullValue)
                deathlink_amnesty = root[i]["slot_data"].get("DeathLink_Amnesty", 0).asInt();
            cur_deathlink_amnesty = deathlink_amnesty;
            for (std::string key : slotdata_strings) {
                if (map_slotdata_callback_int.count(key)) {
                    (*map_slotdata_callback_int.at(key))(root[i]["slot_data"][key].asInt());
                    } else if (map_slotdata_callback_raw.count(key)) {
                    (*map_slotdata_callback_raw.at(key))(writer.write(root[i]["slot_data"][key]));
                } else if (map_slotdata_callback_mapintint.count(key)) {
                    std::map<int,int> out;
                    for (auto itr : root[i]["slot_data"][key].getMemberNames()) {
                        out[std::stoi(itr)] = root[i]["slot_data"][key][itr.c_str()].asInt();
                    }
                    (*map_slotdata_callback_mapintint.at(key))(out);
                }
                
            }

            resync_serverdata_request.key = "APCppLastRecv" + ap_player_name + std::to_string(ap_player_id);
            resync_serverdata_request.value = &last_item_idx;
            resync_serverdata_request.type = AP_DataType::Int;
            AP_GetServerData(&resync_serverdata_request);

            AP_RoomInfo info;
            AP_GetRoomInfo(&info);
            Json::Value req_t = Json::arrayValue;
            if (enable_deathlink && deathlinksupported) {
                Json::Value setdeathlink;
                setdeathlink["cmd"] = "ConnectUpdate";
                setdeathlink["tags"][0] = "DeathLink";
                req_t.append(setdeathlink);
            }
            // Get datapackage for outdated games
            for (std::pair<std::string,std::string> game_pkg : info.datapackage_checksums) {
                if (datapkg_cache.get("games", Json::objectValue).get(game_pkg.first, Json::objectValue).get("checksum", "_None") != game_pkg.second) {
                    printf("AP: Cache outdated for game: %s\n", game_pkg.first.c_str());
                    datapkg_outdated_games.insert(game_pkg.first);
                }
            }
            if (!datapkg_outdated_games.empty()) {
                Json::Value resync_datapkg;
                resync_datapkg["cmd"] = "GetDataPackage";
                resync_datapkg["games"] = Json::arrayValue;
                resync_datapkg["games"].append(*datapkg_outdated_games.begin());
                req_t.append(resync_datapkg);
            } else {
                parseDataPkg();
                Json::Value sync;
                sync["cmd"] = "Sync";
                req_t.append(sync);
            }
            request = writer.write(req_t);
            return true;
        } else if (cmd == "DataPackage") {
            parseDataPkg(root[i]["data"]);
            Json::Value req_t;
            if (!datapkg_outdated_games.empty()) {
                req_t[0]["cmd"] = "GetDataPackage";
                req_t[0]["games"] = Json::arrayValue;
                req_t[0]["games"].append(*datapkg_outdated_games.begin());
            } else {
                req_t[0]["cmd"] = "Sync";
            }
            request = writer.write(req_t);
            return true;
        } else if (cmd == "Retrieved") {
            for (auto itr : root[i]["keys"].getMemberNames()) {
                if (!map_server_data.count(itr)) continue;
                AP_GetServerDataRequest* target = map_server_data[itr];
                switch (target->type) {
                    case AP_DataType::Int:
                        *((int*)target->value) = root[i]["keys"][itr].asInt();
                        break;
                    case AP_DataType::Double:
                        *((double*)target->value) = root[i]["keys"][itr].asDouble();
                        break;
                    case AP_DataType::Raw:
                        *((std::string*)target->value) = writer.write(root[i]["keys"][itr]);
                        break;
                }
                target->status = AP_RequestStatus::Done;
                map_server_data.erase(itr);
            }
        } else if (cmd == "SetReply") {
            if (setreplyfunc) {
                int int_val;
                int int_orig_val;
                double dbl_val;
                double dbl_orig_val;
                std::string raw_val;
                std::string raw_orig_val;
                AP_SetReply setreply;
                setreply.key = root[i]["key"].asString();
                switch (map_serverdata_typemanage[setreply.key]) {
                    case AP_DataType::Int:
                        int_val = root[i]["value"].asInt();
                        int_orig_val = root[i]["original_value"].asInt();
                        setreply.value = &int_val;
                        setreply.original_value = &int_orig_val;
                        break;
                    case AP_DataType::Double:
                        dbl_val = root[i]["value"].asDouble();
                        dbl_orig_val = root[i]["original_value"].asDouble();
                        setreply.value = &dbl_val;
                        setreply.original_value = &dbl_orig_val;
                        break;
                    default:
                        raw_val =  writer.write(root[i]["value"]);
                        raw_orig_val = writer.write(root[i]["original_value"]);
                        setreply.value = &raw_val;
                        setreply.original_value = &raw_orig_val;
                        break;
                }
                (*setreplyfunc)(setreply);
            }
        } else if (cmd == "PrintJSON") {
            const std::string printType = root[i].get("type","").asString();
            if (printType == "ItemSend" || printType == "ItemCheat") {
                if (getPlayer(0, root[i]["receiving"].asInt()).alias == getPlayer(0, ap_player_id).alias || getPlayer(0,root[i]["item"]["player"].asInt()).alias != getPlayer(0,ap_player_id).alias) continue;
                AP_NetworkPlayer recv_player = getPlayer(0, root[i]["receiving"].asInt());
                AP_ItemSendMessage* msg = new AP_ItemSendMessage;
                msg->type = AP_MessageType::ItemSend;
                msg->item = getItemName(recv_player.game, root[i]["item"]["item"].asInt64());
                msg->recvPlayer = recv_player.alias;
                msg->text = msg->item + std::string(" was sent to ") + msg->recvPlayer;
                messageQueue.push_back(msg);
            } else if (printType == "Hint") {
                AP_NetworkPlayer send_player = getPlayer(0, root[i]["item"]["player"].asInt());
                AP_NetworkPlayer recv_player = getPlayer(0, root[i]["receiving"].asInt());
                AP_HintMessage* msg = new AP_HintMessage;
                msg->type = AP_MessageType::Hint;
                msg->item = getItemName(recv_player.game,root[i]["item"]["item"].asInt64());
                msg->sendPlayer = send_player.alias;
                msg->recvPlayer = recv_player.alias;
                msg->location = getLocationName(send_player.game, root[i]["item"]["location"].asInt64());
                msg->checked = root[i]["found"].asBool();
                msg->text = std::string("Item ") + msg->item + std::string(" from ") + msg->sendPlayer + std::string(" to ") + msg->recvPlayer + std::string(" at ") + msg->location + std::string((msg->checked ? " (Checked)" : " (Unchecked)"));
                messageQueue.push_back(msg);
            } else if (printType == "Countdown") {
                AP_CountdownMessage* msg = new AP_CountdownMessage;
                msg->type = AP_MessageType::Countdown;
                msg->timer = root[i]["countdown"].asInt();
                msg->text = root[i]["data"][0]["text"].asString();
                messageQueue.push_back(msg);
            } else {
                AP_Message* msg = new AP_Message;
                msg->text = "";
                for (auto itr : root[i]["data"]) {
                    if (itr.get("type","").asString() == "player_id") {
                        msg->text += getPlayer(0, itr["text"].asInt()).alias;
                    } else if (itr.get("text","") != "") {
                        msg->text += itr["text"].asString();
                    }
                }
                messageQueue.push_back(msg);
            }
        } else if (cmd == "LocationInfo") {
            std::vector<AP_NetworkItem> locations;
            for (unsigned int j = 0; j < root[i]["locations"].size(); j++) {
                AP_NetworkItem item;
                item.item = root[i]["locations"][j]["item"].asInt64();
                item.location = root[i]["locations"][j]["location"].asInt64();
                AP_NetworkPlayer player = getPlayer(0, root[i]["locations"][j]["player"].asInt());
                item.player = player.slot;
                item.flags = root[i]["locations"][j]["flags"].asInt();
                item.itemName = getItemName(player.game, item.item);
                item.locationName = getLocationName(ap_game, item.location);
                item.playerName = player.alias;
                locations.push_back(item);
            }
            if (locinfofunc) {
                locinfofunc(locations);
            } else {
                printf("AP: Received LocationInfo but no handler registered!\n");
            }
        } else if (cmd == "ReceivedItems") {
            int item_idx = root[i]["index"].asInt();
            bool notify;
            for (unsigned int j = 0; j < root[i]["items"].size(); j++) {
                int64_t item_id = root[i]["items"][j]["item"].asInt64();
                notify = (item_idx == 0 && last_item_idx <= j && multiworld) || item_idx != 0;
                (*getitemfunc)(item_id, notify);
                if (queueitemrecvmsg && notify) {
                    AP_ItemRecvMessage* msg = new AP_ItemRecvMessage;
                    AP_NetworkPlayer sender = getPlayer(0, root[i]["items"][j]["player"].asInt());
                    msg->type = AP_MessageType::ItemRecv;
                    msg->item = getItemName(ap_game, item_id);
                    msg->sendPlayer = sender.alias;
                    msg->text = std::string("Received ") + msg->item + std::string(" from ") + msg->sendPlayer;
                    messageQueue.push_back(msg);
                }
            }
            last_item_idx = item_idx == 0 ? root[i]["items"].size() : last_item_idx + root[i]["items"].size();
            AP_SetServerDataRequest request;
            request.key = "APCppLastRecv" + ap_player_name + std::to_string(ap_player_id);
            AP_DataStorageOperation replac;
            replac.operation = "replace";
            replac.value = &last_item_idx;
            std::vector<AP_DataStorageOperation> operations;
            operations.push_back(replac);
            request.operations = operations;
            request.default_value = 0;
            request.type = AP_DataType::Int;
            request.want_reply = false;
            AP_SetServerData(&request);
        } else if (cmd == "RoomUpdate") {
            //Sync checks with server
            for (unsigned int j = 0; j < root[i]["checked_locations"].size(); j++) {
                int64_t loc_id = root[i]["checked_locations"][j].asInt64();
                (*checklocfunc)(loc_id);
            }
            //Update Player aliases if present
            for (auto itr : root[i].get("players", Json::arrayValue)) {
                map_players[itr["slot"].asInt()].alias = itr["alias"].asString();
            }
        } else if (cmd == "ConnectionRefused") {
            auth = false;
            refused = true;
            printf("AP: Archipelago Server has refused connection. Check Password / Name / IP and restart the Game.\n");
            fflush(stdout);
        } else if (cmd == "Bounced") {
            // Only expected Packages are DeathLink Packages. RIP
            if (!enable_deathlink) continue;
            for (unsigned int j = 0; j < root[i]["tags"].size(); j++) {
                if (root[i]["tags"][j].asString() == "DeathLink") {
                    // Suspicions confirmed ;-; But maybe we died, not them?
                    if (root[i]["data"]["source"].asString() == ap_player_name) break; // We already paid our penance
                    deathlinkstat = true;
                    std::string out = root[i]["data"]["source"].asString() + " killed you";
                    if (recvdeath != nullptr) {
                        (*recvdeath)();
                    }
                    break;
                }
            }
        }
    }
    return false;
}

void APSend(std::string req) {
    if (webSocket.getReadyState() != ix::ReadyState::Open) {
        printf("AP: Not Connected. Send will fail.\n");
        return;
    }
    webSocket.send(req);
}

void WriteFileJSON(Json::Value val, std::string path) {
    std::ofstream out;
    out.open(path);
    out.seekp(0);
    out << writer.write(val).c_str();
    out.flush();
    out.close();
}

void parseDataPkg(Json::Value new_datapkg) {
    for (std::string game : new_datapkg["games"].getMemberNames()) {
        Json::Value game_data = new_datapkg["games"][game];
        datapkg_cache["games"][game] = game_data;
        datapkg_outdated_games.erase(game);
        printf("AP: Game Cache updated for %s\n", game.c_str());
    }
    WriteFileJSON(datapkg_cache, datapkg_cache_path);
    parseDataPkg();
}

void parseDataPkg() {
    for (std::string game : datapkg_cache["games"].getMemberNames()) {
        Json::Value game_data = datapkg_cache["games"][game];
        for (std::string item_name : game_data["item_name_to_id"].getMemberNames()) {
            map_item_id_name[{game,game_data["item_name_to_id"][item_name].asInt64()}] = item_name;
        }
        for (std::string location : game_data["location_name_to_id"].getMemberNames()) {
            map_location_id_name[{game,game_data["location_name_to_id"][location].asInt64()}] = location;
        }
    }
}

std::string getItemName(std::string game, int64_t id) {
    std::pair<std::string,int64_t> item = {game,id};
    return map_item_id_name.count(item) ? map_item_id_name.at(item) : std::string("Unknown Item") + std::to_string(id) + " from " + game;
}

std::string getLocationName(std::string game, int64_t id) {
    std::pair<std::string,int64_t> location = {game,id};
    return map_location_id_name.count(location) ? map_location_id_name.at(location) : std::string("Unknown Location") + std::to_string(id) + " from " + game;
}

AP_NetworkPlayer getPlayer(int team, int slot) {
    return map_players[slot];
}

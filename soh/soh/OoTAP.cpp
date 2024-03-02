#include "OoTAP.h"
#include "imgui.h"
#include "Enhancements/randomizer/savefile.h"
#include "Enhancements/game-interactor/GameInteractor.h"
#include "Enhancements/randomizer/randomizer.h"
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include "soh/util.h"
#include "soh/OTRGlobals.h"
#include "OoTAPHelpers.h"
extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
PlayState* gPlayState;
SaveContext gSaveContext;
}

bool OoTAP_ENABLED = false;
bool OoTAP_STARTED = false;
float OoTAP_MsgFrameDuration = 240;
float OoTAP_MessageFadeoutDuration = OoTAP_MsgFrameDuration / 1.5;
std::vector<AdditionalMessage> additionalMessages;
std::vector<int64_t> cachedItems;
std::vector<RandomizerCheck> cachedLocations;
std::vector<AP_NetworkItem> itemLocations;
int currentChatIndex = 0;

int OoTAP_OPTION_FOREST;
int OoTAP_OPTION_KAKARIKO_GATE;
int OoTAP_OPTION_DOOR_OF_TIME;
int OoTAP_OPTION_FOUNTAIN;
int OoTAP_OPTION_CARPENTERS;
int OoTAP_OPTION_BRIDGE;
int OoTAP_OPTION_TRIALS;

void OoTAP_SendMessage(std::string msg) {
    AdditionalMessage add_msg;
    add_msg.text = msg;
    add_msg.current_duration = OoTAP_MsgFrameDuration;

    additionalMessages.push_back(add_msg);
}

void OoTAP_SetForest(int option) { OoTAP_OPTION_FOREST = option; }
void OoTAP_SetKakarikoGate(int option) {
    // OoTAP_OPTION_KAKARIKO_GATE = option;
    OoTAP_SendMessage("KAKARIKO GATE: '" + std::to_string(option) + "'");
}
void OoTAP_SetDoorOfTime(int option) { OoTAP_OPTION_DOOR_OF_TIME = option; }
void OoTAP_SetFountain(int option) { OoTAP_OPTION_FOUNTAIN = option; }
void OoTAP_SetCarpenters(int option) { OoTAP_OPTION_CARPENTERS = option; }
void OoTAP_SetBridge(int option) { OoTAP_OPTION_BRIDGE = option; }
void OoTAP_SetTrials(int option) { OoTAP_OPTION_TRIALS = option; }

void OoTAP_Enable(const char* ip, int port, const char* slotName, const char* password) {
    if (OoTAP_ENABLED) {
        return;
    }

    AP_Init((std::string(ip) + ":" + std::to_string(port)).c_str(), "Ocarina of Time", slotName, password);

    AP_SetClientVersion(new AP_NetworkVersion { 0, 4, 4 });
    AP_SetItemClearCallback(&OoTAP_ItemClear);
    AP_SetItemRecvCallback(&OoTAP_ItemReceive);
    AP_SetLocationCheckedCallback(&OoTAP_LocationChecked);
    AP_SetDeathLinkSupported(false);

    // AP_RegisterSlotDataRawCallback("Forest", &OoTAP_SetForest);
    AP_RegisterSlotDataIntCallback("open_kakariko", &OoTAP_SetKakarikoGate);
    // AP_RegisterSlotDataIntCallback("open_door_of_time", &OoTAP_SetDoorOfTime);
    // AP_RegisterSlotDataIntCallback("zora_fountain", &OoTAP_SetFountain);
    // AP_RegisterSlotDataIntCallback("gerudo_fortress", &OoTAP_SetCarpenters);
    // AP_RegisterSlotDataIntCallback("bridge", &OoTAP_SetBridge);
    // AP_RegisterSlotDataIntCallback("trials", &OoTAP_SetTrials);

    OoTAP_InitCheckLookup();

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() { OoTAP_Update(); });

    OoTAP_ENABLED = true;
    OoTAP_STARTED = false;

    OoTAP_SendMessage("Archipelago enabled.");
}

void OoTAP_Disable() {
    AP_Shutdown();

    cachedItems.clear();
    cachedLocations.clear();

    OoTAP_SendMessage("Archipelago disabled.");
    OoTAP_ENABLED = false;
    OoTAP_STARTED = false;
}

void OoTAP_Update() {
    if (!OoTAP_ENABLED) {
        return;
    }

    if (!OoTAP_STARTED) {
        OoTAP_Start();
    }

    if (GameInteractor::IsSaveLoaded()) {
        OoTAP_UpdateReceivedItems();
    }
}

void OoTAP_DrawStatus() {
    // go through the list in reverse so that
    // recent messages appear first
    for (int i = additionalMessages.size() - 1; i > 0; i--) {
        AdditionalMessage& msg = additionalMessages[i];

        float alpha = 1;
        if (msg.current_duration < OoTAP_MessageFadeoutDuration)
            alpha = msg.current_duration / OoTAP_MessageFadeoutDuration;

        ImGui::PushStyleColor(ImGuiCol_Text, { 1, 1, 1, alpha });
        ImGui::Text(msg.text.c_str());
        ImGui::PopStyleColor();

        msg.current_duration--;
        if (msg.current_duration <= 0)
            additionalMessages.erase(additionalMessages.begin() + i);
    }

    if (!OoTAP_ENABLED) {
        ImGui::Text("Archipelago is not enabled.");
        return;
    }
    if (!GameInteractor::IsSaveLoaded() || !OoTAP_STARTED) {
        ImGui::Text("Waiting for file load...");
        return;
    }

    if (AP_GetConnectionStatus() == AP_ConnectionStatus::Disconnected) {
        ImGui::Text("Connecting...");
        return;
    }
    if (AP_GetConnectionStatus() == AP_ConnectionStatus::ConnectionRefused) {
        ImGui::Text("Connection refused!\nCheck your IP, port, slot name, and password.");
        return;
    }

    if (AP_IsMessagePending())
    {
        AP_Message* msg = AP_GetLatestMessage();

        if (msg->type == AP_MessageType::ItemSend) {
            AP_ItemSendMessage* send_msg = static_cast<AP_ItemSendMessage*>(msg);
            OoTAP_SendMessage(send_msg->item + " was sent to " + send_msg->recvPlayer);
            AP_ClearLatestMessage();
        }
        else if (msg->type == AP_MessageType::Countdown) {
            AP_CountdownMessage* recv_msg = static_cast<AP_CountdownMessage*>(msg);
            OoTAP_SendMessage(recv_msg->text);
            AP_ClearLatestMessage();
        }
        else if (msg->type == AP_MessageType::ItemRecv) {
            AP_ClearLatestMessage();
        } else {
            if (
            // skip this message due to having no use
                msg->text != "Now that you are connected, you can use !help to list commands to run via the server. If your client supports it, you may have additional local commands you can list with /help."
            // skip commands
                && msg->text[0] != '!'
            ){
                OoTAP_SendMessage(msg->text);
            }
            AP_ClearLatestMessage();
        }
    }
}

void OoTAP_Start() {
    if (!OoTAP_ENABLED || !GameInteractor::IsSaveLoaded()) return;

    AP_Start();

    OTRGlobals::Instance->gRandomizer->ClearRandomizer();

    OoTAP_STARTED = true;
    OoTAP_SendMessage("Archipelago started.");
}

void OoTAP_LocationInfo(std::vector<AP_NetworkItem> locations) {
    itemLocations = locations;

    AP_NetworkItem test = locations[10];
    OoTAP_SendMessage(test.itemName + "-" + test.locationName + "-" + test.playerName);
}

void OoTAP_SendChatMsg(std::string msg) {
    AP_Say(msg);
}

void OoTAP_ItemClear() {
    // for (s32 i = 0; i < sizeof(gSaveContext.randomizerInf); i++) {
    //     gSaveContext.randomizerInf[i] = 0;
    // }

    // Reset triforce pieces collected
    gSaveContext.triforcePiecesCollected = 0;

    gSaveContext.cutsceneIndex = 0; // no intro cutscene
    // Starts pending ice traps out at 0 before potentially incrementing them down the line.
    gSaveContext.pendingIceTrapCount = 0;

    // clear items
    for (int i = 0; i < sizeof(gSaveContext.inventory.dungeonItems); i++) {
        gSaveContext.inventory.dungeonItems[i] = 0;
    }
    gSaveContext.inventory.equipment = 0;
    gSaveContext.inventory.questItems = 0;
    gSaveContext.inventory.upgrades = 0;
    for (int i = 0; i < sizeof(gSaveContext.inventory.dungeonKeys); i++) {
        gSaveContext.inventory.dungeonKeys[i] = 0;
        gSaveContext.sohStats.dungeonKeys[i] = 0;
    }

    Flags_SetInfTable(INFTABLE_GREETED_BY_SARIA);
    Flags_SetEventChkInf(EVENTCHKINF_FIRST_SPOKE_TO_MIDO);
    Flags_SetEventChkInf(EVENTCHKINF_MET_DEKU_TREE);
    Flags_SetEventChkInf(EVENTCHKINF_DEKU_TREE_OPENED_MOUTH);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_KAEPORA_IN_LAKE_HYLIA);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_MASTER_SWORD_CHAMBER);
    Flags_SetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL);
    Flags_SetEventChkInf(EVENTCHKINF_SHEIK_SPAWNED_AT_MASTER_SWORD_PEDESTAL);
    Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
    Flags_SetEventChkInf(EVENTCHKINF_RENTED_HORSE_FROM_INGO);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_POE_COLLECTOR_IN_RUINED_MARKET);
    Flags_SetEventChkInf(EVENTCHKINF_WATCHED_GANONS_CASTLE_COLLAPSE_CAUGHT_BY_GERUDO);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_NABOORU_IN_SPIRIT_TEMPLE);

    Flags_SetInfTable(INFTABLE_MET_CHILD_MALON_AT_CASTLE_OR_MARKET);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_INGO_AT_RANCH_BEFORE_TALON_RETURNS);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH);
    Flags_SetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
    Flags_SetInfTable(INFTABLE_CHILD_MALON_SAID_EPONA_WAS_AFRAID_OF_YOU);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_INGO_ONCE_AS_ADULT);

    // Ruto already met in jabu and spawns down the hole immediately
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_MEET_RUTO);
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_TALK_FIRST_TIME);
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_WANTS_TO_BE_TOSSED_TO_SAPPHIRE);

    // Skip cutscenes before Nabooru fight
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_NABOORU_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_NABOORU_ORDERED_TO_FIGHT_BY_TWINROVA);

    // Skip boss cutscenes
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_GOHMA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_PHANTOM_GANON_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_MORPHA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_TWINROVA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE);

    // Entered areas
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_HYRULE_FIELD);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DEATH_MOUNTAIN_TRAIL);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_KAKARIKO_VILLAGE);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_ZORAS_DOMAIN);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_HYRULE_CASTLE);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GORON_CITY);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_TEMPLE_OF_TIME);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DEKU_TREE);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DODONGOS_CAVERN);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_LAKE_HYLIA);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GERUDO_VALLEY);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GERUDOS_FORTRESS);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_LON_LON_RANCH);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_JABU_JABUS_BELLY);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GRAVEYARD);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_ZORAS_FOUNTAIN);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DESERT_COLOSSUS);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DEATH_MOUNTAIN_CRATER);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GANONS_CASTLE_EXTERIOR);
    Flags_SetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE);

    // skip the z target talk instructions by the kokiri shop
    gSaveContext.sceneFlags[SCENE_KOKIRI_FOREST].swch |= (1 << 0x1F);

    // Go away ruto (water temple first cutscene)
    gSaveContext.sceneFlags[SCENE_WATER_TEMPLE].swch |= (1 << 0x10);

    // no more kaepora
    gSaveContext.sceneFlags[SCENE_HYRULE_FIELD].swch |= (1 << 0xC);  // hyrule field kaepora outside kokiri forest
    gSaveContext.sceneFlags[SCENE_HYRULE_FIELD].swch |= (1 << 0xB);  // hyrule field kaepora outside lake hylia
    gSaveContext.sceneFlags[SCENE_LOST_WOODS].swch |= (1 << 0x7);  // lost woods kaepora pre-saria
    gSaveContext.sceneFlags[SCENE_LOST_WOODS].swch |= (1 << 0x8);  // lost woods kaepora post-saria
    gSaveContext.sceneFlags[SCENE_DESERT_COLOSSUS].swch |= (1 << 0x1F); // desert colossus kaepora
    gSaveContext.sceneFlags[SCENE_HYRULE_CASTLE].swch |= (1 << 0x5);  // hyrule castle kaepora

    // TODO: CHECK AP SETTING: ADULT TRADE ITEMS
    // gSaveContext.adultTradeItems = 0;

    // TODO: CHECK AP SETTING: STARTING AGE
    /*
    switch (startingAge) {
        case RO_AGE_ADULT: // Adult
            gSaveContext.linkAge = LINK_AGE_ADULT;
            gSaveContext.entranceIndex = ENTR_TEMPLE_OF_TIME_7;
            gSaveContext.savedSceneNum = SCENE_LON_LON_RANCH; // Set scene num manually to ToT
            break;
        case RO_AGE_CHILD: // Child
            gSaveContext.linkAge = LINK_AGE_CHILD;
            gSaveContext.savedSceneNum = -1;
            break;
    }
    */

    // TODO: CHECK AP SETTING: SPAWN LOCATION
    // gSaveContext.entranceIndex = -1;

    // TODO: CHECK AP SETTING: TRIAL REQUIREMENT
    /*
    for (u16 i = RAND_INF_TRIALS_DONE_LIGHT_TRIAL; i <= RAND_INF_TRIALS_DONE_SHADOW_TRIAL; i++) {
        if (!OTRGlobals::Instance->gRandomizer->IsTrialRequired((RandomizerInf)i)) {
            Flags_SetRandomizerInf((RandomizerInf)i);
        }
    }
    */

    // TODO: CHECK AP SETTING: SKIP CHILD ZELDA
    /*
    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_SONG_FROM_IMPA, (GetItemID)RG_ZELDAS_LULLABY);
    StartingItemGive(getItemEntry);

    // malon/talon back at ranch
    Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG);
    Flags_SetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_CASTLE);
    Flags_SetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE);

    // Set "Got Zelda's Letter" flag. Also ensures Saria is back at SFM.
    Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER);

    // Got item from impa
    Flags_SetEventChkInf(EVENTCHKINF_LEARNED_ZELDAS_LULLABY);

    gSaveContext.sceneFlags[SCENE_HYRULE_CASTLE].swch |= (1 << 0x4); // move milk crates in hyrule castle to moat

    // set this at the end to ensure we always start with the letter
    // this is for the off chance we got the weird egg from impa (which should never happen)
    INV_CONTENT(ITEM_LETTER_ZELDA) = ITEM_LETTER_ZELDA;
    */

    // TODO: CHECK AP SETTING: BIG POE COUNT
    // HIGH_SCORE(HS_POE_POINTS) = 1000 - (100 * big_poe_count);

    // TODO: CHECK AP SETTING: SKIP EPONA RACE
    // Flags_SetEventChkInf(EVENTCHKINF_EPONA_OBTAINED);

    // TODO: CHECK AP SETTING: KEYSANITY & OPEN LOWEST FIRE TEMPLE DOOR (IF NOT MQ)
    // gSaveContext.sceneFlags[SCENE_FIRE_TEMPLE].swch |= (1 << 0x17);

    // Opens locked Water Temple door in vanilla to prevent softlocks
    // West door on the middle level that leads to the water raising thing
    // Happens in 3DS rando and N64 rando as well
    if (!ResourceMgr_IsSceneMasterQuest(SCENE_WATER_TEMPLE)) {
        gSaveContext.sceneFlags[SCENE_WATER_TEMPLE].swch |= (1 << 0x15);
    }

    // TODO: FOREST OPTION!!
    // switch (OoTAP_OPTION_FOREST) {
    //     case 0:
    //         Flags_SetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD);
    //         // Fallthrough
    //     case 1:
    //         Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD);
    //         break;
    // }

    if (OoTAP_OPTION_DOOR_OF_TIME == 1) {
        Flags_SetEventChkInf(EVENTCHKINF_OPENED_THE_DOOR_OF_TIME);
    }

    if (OoTAP_OPTION_KAKARIKO_GATE == 0) {
        Flags_SetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD);
    }

    if (OoTAP_OPTION_CARPENTERS == 1 ||
        OoTAP_OPTION_CARPENTERS == 2) {
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(1));
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(2));
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(3));
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x02); // heard yells and unlocked doors
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x03);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x04);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x06);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x07);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x08);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x10);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x12);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x13);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0A); // picked up keys
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0E);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0F);
    }

    if (OoTAP_OPTION_CARPENTERS == 2) {
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(0));
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x01); // heard yell and unlocked door
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x05);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x11);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0C); // picked up key

        // TODO: CHECK AP SETTING: SHUFFLE GERUDO CARD
        // if (!Randomizer_GetSettingValue(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
        //     Item_Give(NULL, ITEM_GERUDO_CARD);
        // }
    }

    // TODO: CHECK AP SETTING: COMPLETE MASK QUEST
    /*
    if (Randomizer_GetSettingValue(RSK_COMPLETE_MASK_QUEST)) {
        Flags_SetInfTable(INFTABLE_GATE_GUARD_PUT_ON_KEATON_MASK);
        Flags_SetEventChkInf(EVENTCHKINF_PAID_BACK_BUNNY_HOOD_FEE);
        gSaveContext.itemGetInf[3] |= 0x100;  // Sold Keaton Mask
        gSaveContext.itemGetInf[3] |= 0x200;  // Sold Skull Mask
        gSaveContext.itemGetInf[3] |= 0x400;  // Sold Spooky Mask
        gSaveContext.itemGetInf[3] |= 0x800;  // bunny hood related
        gSaveContext.itemGetInf[3] |= 0x8000; // Obtained Mask of Truth
    }
    */

   // TODO: CHECK AP SETTING: PLANT MAGIC BEANS

   // TODO: CHECK AP SETTING: ZORA'S FOUNTAIN

   // TODO: CHECK AP SETTING: RAINBOW BRIDGE REQUIREMENT

   // TODO: CHECK AP SETTING: INTERIOR ENTRANCES
   
   // TODO: CHECK AP SETTING: GROTTO ENTRANCES

   // TODO: CHECK AP SETTING: DUNGEON ENTRANCES

   // TODO: CHECK AP SETTING: OVERWORLD ENTRANCES

   // TODO: CHECK AP SETTING: OWL DROPS

   // TODO: CHECK AP SETTING: WARP SONGS

   // TODO: CHECK AP SETTING: BOSS ENTRANCES

   // TODO: CHECK AP SETTING: TRIFORCE HUNT

   // TODO: CHECK AP SETTING: BOMBCHUS IN LOGIC

   // TODO: CHECK AP SETTING: MQ DUNGEONS

   // TODO: CHECK AP SETTING: BRIDGE REQUIREMENTS

   // TODO: CHECK AP SETTING: MAPS & COMPASSES

   // TODO: CHECK AP SETTING: SMALL KEYS

   // TODO: CHECK AP SETTING: THIEVES' HIDEOUT KEYS

   // TODO: CHECK AP SETTING: BOSS KEYS

   // TODO: CHECK AP SETTING: GANON'S BOSS KEY

   // TODO: CHECK AP SETTING: SHOPSANITY

   // TODO: CHECK AP SETTING: SHUFFLE CHILD ITEMS

    OoTAP_SendMessage("Items cleared");
}

void OoTAP_ItemReceive(int64_t id, bool notify) {
    cachedItems.push_back(id);
}

void OoTAP_UpdateReceivedItems() {
    if (!OoTAP_ENABLED || !OoTAP_STARTED || cachedItems.size() < 1) {
        return;
    }

    for (const int64_t& item : cachedItems) {
        int64_t id = OoTAP_APItemToGameItem(item);
        if (id < 0) {
            continue;
        }

        // individual item handling
        switch (id) {
            case ITEM_SINGLE_MAGIC:
                id = gSaveContext.isMagicAcquired ? ITEM_DOUBLE_MAGIC : ITEM_SINGLE_MAGIC;
                break;
            case ITEM_WALLET_ADULT:
                id = CUR_UPG_VALUE(UPG_WALLET) == 2 ? ITEM_WALLET_GIANT : ITEM_WALLET_ADULT;
                break;
            case ITEM_BRACELET:
                id = CUR_UPG_VALUE(UPG_STRENGTH) == 3 ? ITEM_GAUNTLETS_GOLD : CUR_UPG_VALUE(UPG_STRENGTH) == 2 ? ITEM_GAUNTLETS_SILVER : ITEM_BRACELET;
                break;
            case ITEM_SCALE_SILVER:
                id = CUR_UPG_VALUE(UPG_SCALE) == 2 ? ITEM_SCALE_GOLDEN : ITEM_SCALE_SILVER;
                break;
            case ITEM_BOMB_BAG_20:
                id = CUR_UPG_VALUE(UPG_BOMB_BAG) == 3 ? ITEM_BOMB_BAG_40 : CUR_UPG_VALUE(UPG_BOMB_BAG) == 2 ? ITEM_BOMB_BAG_30 : ITEM_BOMB_BAG_20;
                break;
            case ITEM_NUT_UPGRADE_30:
                id = CUR_UPG_VALUE(UPG_NUTS) == 2 ? ITEM_NUT_UPGRADE_40 : ITEM_NUT_UPGRADE_30;
                break;
            case ITEM_STICK_UPGRADE_20:
                id = CUR_UPG_VALUE(UPG_STICKS) == 2 ? ITEM_STICK_UPGRADE_30 : ITEM_STICK_UPGRADE_20;
                break;
            case ITEM_OCARINA_FAIRY:
                if (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY || INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_TIME)
                    id = ITEM_OCARINA_TIME;
                break;
            case ITEM_HOOKSHOT:
                if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT || INV_CONTENT(ITEM_HOOKSHOT) == ITEM_LONGSHOT)
                    id = ITEM_LONGSHOT;
                break;

            // give rupees for items you can't get without an upgrade
            case ITEM_BOMB:
                if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0)
                    id = ITEM_RUPEE_GREEN;
                break;
            case ITEM_BOMBS_5:
                if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0)
                    id = ITEM_RUPEE_BLUE;
                break;
            case ITEM_BOMBS_10:
                if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0)
                    id = ITEM_RUPEE_BLUE;
                break;
            case ITEM_BOMBS_20:
                if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0)
                    id = ITEM_RUPEE_RED;
                break;
            case ITEM_BOMBS_30:
                if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0)
                    id = ITEM_RUPEE_RED;
                break;
            case ITEM_ARROWS_SMALL:
                if (INV_CONTENT(ITEM_BOW) == ITEM_NONE)
                    id = ITEM_RUPEE_GREEN;
            case ITEM_ARROWS_MEDIUM:
                if (INV_CONTENT(ITEM_BOW) == ITEM_NONE)
                    id = ITEM_RUPEE_BLUE;
            case ITEM_ARROWS_LARGE:
                if (INV_CONTENT(ITEM_BOW) == ITEM_NONE)
                    id = ITEM_RUPEE_RED;
        }

        // handle item
        switch (id) {
            case ITEM_KEY_BOSS_FOREST:
                gSaveContext.inventory.dungeonItems[SCENE_FOREST_TEMPLE] |= gBitFlags[0];
                break;
            case ITEM_KEY_BOSS_FIRE:
                gSaveContext.inventory.dungeonItems[SCENE_FIRE_TEMPLE] |= gBitFlags[0];
                break;
            case ITEM_KEY_BOSS_WATER:
                gSaveContext.inventory.dungeonItems[SCENE_WATER_TEMPLE] |= gBitFlags[0];
                break;
            case ITEM_KEY_BOSS_SPIRIT:
                gSaveContext.inventory.dungeonItems[SCENE_SPIRIT_TEMPLE] |= gBitFlags[0];
                break;
            case ITEM_KEY_BOSS_SHADOW:
                gSaveContext.inventory.dungeonItems[SCENE_SHADOW_TEMPLE] |= gBitFlags[0];
                break;
            case ITEM_KEY_BOSS_GANON:
                gSaveContext.inventory.dungeonItems[SCENE_GANONS_TOWER] |= gBitFlags[0];
                break;
            case ITEM_COMPASS_DEKU:
                gSaveContext.inventory.dungeonItems[SCENE_DEKU_TREE] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_DODONGO:
                gSaveContext.inventory.dungeonItems[SCENE_DODONGOS_CAVERN] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_JABU:
                gSaveContext.inventory.dungeonItems[SCENE_JABU_JABU] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_FOREST:
                gSaveContext.inventory.dungeonItems[SCENE_FOREST_TEMPLE] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_FIRE:
                gSaveContext.inventory.dungeonItems[SCENE_FIRE_TEMPLE] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_WATER:
                gSaveContext.inventory.dungeonItems[SCENE_WATER_TEMPLE] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_SPIRIT:
                gSaveContext.inventory.dungeonItems[SCENE_SPIRIT_TEMPLE] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_SHADOW:
                gSaveContext.inventory.dungeonItems[SCENE_SHADOW_TEMPLE] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_WELL:
                gSaveContext.inventory.dungeonItems[SCENE_BOTTOM_OF_THE_WELL] |= gBitFlags[1];
                break;
            case ITEM_COMPASS_ICE_CAVERN:
                gSaveContext.inventory.dungeonItems[SCENE_ICE_CAVERN] |= gBitFlags[1];
                break;
            case ITEM_MAP_DEKU:
                gSaveContext.inventory.dungeonItems[SCENE_DEKU_TREE] |= gBitFlags[2];
                break;
            case ITEM_MAP_DODONGO:
                gSaveContext.inventory.dungeonItems[SCENE_DODONGOS_CAVERN] |= gBitFlags[2];
                break;
            case ITEM_MAP_JABU:
                gSaveContext.inventory.dungeonItems[SCENE_JABU_JABU] |= gBitFlags[2];
                break;
            case ITEM_MAP_FOREST:
                gSaveContext.inventory.dungeonItems[SCENE_FOREST_TEMPLE] |= gBitFlags[2];
                break;
            case ITEM_MAP_FIRE:
                gSaveContext.inventory.dungeonItems[SCENE_FIRE_TEMPLE] |= gBitFlags[2];
                break;
            case ITEM_MAP_WATER:
                gSaveContext.inventory.dungeonItems[SCENE_WATER_TEMPLE] |= gBitFlags[2];
                break;
            case ITEM_MAP_SPIRIT:
                gSaveContext.inventory.dungeonItems[SCENE_SPIRIT_TEMPLE] |= gBitFlags[2];
                break;
            case ITEM_MAP_SHADOW:
                gSaveContext.inventory.dungeonItems[SCENE_SHADOW_TEMPLE] |= gBitFlags[2];
                break;
            case ITEM_MAP_WELL:
                gSaveContext.inventory.dungeonItems[SCENE_BOTTOM_OF_THE_WELL] |= gBitFlags[2];
                break;
            case ITEM_MAP_ICE_CAVERN:
                gSaveContext.inventory.dungeonItems[SCENE_ICE_CAVERN] |= gBitFlags[2];
                break;
            case ITEM_KEY_SMALL_FOREST:
                gSaveContext.inventory.dungeonKeys[SCENE_FOREST_TEMPLE]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_FOREST_TEMPLE]++;
                break;
            case ITEM_KEY_SMALL_FIRE:
                gSaveContext.inventory.dungeonKeys[SCENE_FIRE_TEMPLE]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_FIRE_TEMPLE]++;
                break;
            case ITEM_KEY_SMALL_WATER:
                gSaveContext.inventory.dungeonKeys[SCENE_WATER_TEMPLE]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_WATER_TEMPLE]++;
                break;
            case ITEM_KEY_SMALL_SPIRIT:
                gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_SPIRIT_TEMPLE]++;
                break;
            case ITEM_KEY_SMALL_SHADOW:
                gSaveContext.inventory.dungeonKeys[SCENE_SHADOW_TEMPLE]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_SHADOW_TEMPLE]++;
                break;
            case ITEM_KEY_SMALL_WELL:
                gSaveContext.inventory.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL]++;
                break;
            case ITEM_KEY_SMALL_GTG:
                gSaveContext.inventory.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND]++;
                break;
            case ITEM_KEY_SMALL_HIDEOUT:
                gSaveContext.inventory.dungeonKeys[SCENE_THIEVES_HIDEOUT]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_THIEVES_HIDEOUT]++;
                break;
            case ITEM_KEY_SMALL_GANON:
                gSaveContext.inventory.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE]++;
                gSaveContext.sohStats.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE]++;
                break;
            case ITEM_KEY_RING_FOREST:
                gSaveContext.inventory.dungeonKeys[SCENE_FOREST_TEMPLE] = FOREST_TEMPLE_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_FOREST_TEMPLE] = FOREST_TEMPLE_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_FIRE:
                gSaveContext.inventory.dungeonKeys[SCENE_FIRE_TEMPLE] = FIRE_TEMPLE_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_FIRE_TEMPLE] = FIRE_TEMPLE_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_WATER:
                gSaveContext.inventory.dungeonKeys[SCENE_WATER_TEMPLE] = WATER_TEMPLE_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_WATER_TEMPLE] = WATER_TEMPLE_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_SPIRIT:
                gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] = SPIRIT_TEMPLE_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_SPIRIT_TEMPLE] = SPIRIT_TEMPLE_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_SHADOW:
                gSaveContext.inventory.dungeonKeys[SCENE_SHADOW_TEMPLE] = SHADOW_TEMPLE_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_SHADOW_TEMPLE] = SHADOW_TEMPLE_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_WELL:
                gSaveContext.inventory.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_GTG:
                gSaveContext.inventory.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_HIDEOUT:
                gSaveContext.inventory.dungeonKeys[SCENE_THIEVES_HIDEOUT] = GERUDO_FORTRESS_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_THIEVES_HIDEOUT] = GERUDO_FORTRESS_SMALL_KEY_MAX;
                break;
            case ITEM_KEY_RING_GANON:
                gSaveContext.inventory.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] = GANONS_CASTLE_SMALL_KEY_MAX;
                gSaveContext.sohStats.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] = GANONS_CASTLE_SMALL_KEY_MAX;
                break;
            case ITEM_ICE_TRAP:
                gSaveContext.pendingIceTrapCount++;
                break;
            case ITEM_DOUBLE_DEFENSE:
                gSaveContext.isDoubleDefenseAcquired = true;
                break;
            case ITEM_SINGLE_MAGIC:
                gSaveContext.isMagicAcquired = true;
                gSaveContext.magicFillTarget = 48;
                break;
            case ITEM_DOUBLE_MAGIC:
                gSaveContext.isDoubleMagicAcquired = true;
                gSaveContext.magicFillTarget = 96;
                break;
            case ITEM_TRIFORCE_PIECE:
                gSaveContext.triforcePiecesCollected++;
                break;
            case ITEM_HEART_PIECE:
            case ITEM_HEART_PIECE_2:
                gSaveContext.inventory.questItems += 1 << (QUEST_HEART_PIECE + 4);
                gSaveContext.sohStats.heartPieces++;
                break;
            case ITEM_HEART_CONTAINER:
                gSaveContext.healthCapacity += 0x10;
                gSaveContext.health += 0x10;
                gSaveContext.sohStats.heartContainers++;
                break;
            default:
                Item_Give(gPlayState, id);
                break;
        }

        std::string name = OoTAP_GetItemName(id);
        // OoTAP_SendMessage("Received item: " + name);
    }
    cachedItems.clear();
}

void OoTAP_LocationChecked(int64_t id) {
    cachedLocations.push_back(OoTAP_APCheckToRandoCheck(id));
}

void OoTAP_SendItem(RandomizerCheck check) {
    int64_t id = OoTAP_RandoCheckToAPCheck(check);
    if (id <= 0) {
        // unknown check, alert
        OoTAP_SendMessage("Unknown check! (" + std::to_string(check) + ")");
        return;
    }

    AP_SendItem(id);
}

bool OoTAP_IsRandoCheckChecked(RandomizerCheck check) {
    for (const RandomizerCheck loc : cachedLocations) {
        if (loc == check)
            return true;
    }

    return false;
}

void OoTAP_DefeatGanon() {
    AP_StoryComplete();
}

void OoTAP_Window::InitElement() {
}

void OoTAP_Window::DrawElement() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;

    const int dragging = CVarGetInteger("gArchipelago.WindowDragging", 1);
    float alpha = 0;
    if (dragging)
        alpha = 0.5;
    else {
        flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0, 0, 0, alpha });
    // ImGui::PushStyleColor(ImGuiCol_Text, { 1, 1, 1, alpha });
    ImGui::PushStyleColor(ImGuiCol_Border, { 0, 0, 0, 0 });
    ImGui::Begin("Archipelago", nullptr, flags);

    ImGui::SetWindowFontScale(CVarGetFloat("gArchipelago.WindowScale", 2));
    if (dragging) {
        ImGui::Text("Archipelago Window");
    }
    OoTAP_DrawStatus();

    ImGui::PopStyleColor();
    // ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
}
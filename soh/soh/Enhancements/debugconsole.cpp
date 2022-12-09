#include "debugconsole.h"
#include <ImGuiImpl.h>
#include <Utils.h>
#include "savestates.h"
#include <Console.h>

#include <vector>
#include <string>
#include "soh/OTRGlobals.h"
#include <soh/Enhancements/item-tables/ItemTableManager.h>


#define Path _Path
#define PATH_HACK
#include <Utils/StringHelper.h>

#include <Window.h>
#include <ImGui/imgui_internal.h>
#undef PATH_HACK
#undef Path

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

#include <Cvar.h>
#include "overlays/actors/ovl_En_Niw/z_en_niw.h"

#define CMD_REGISTER SohImGui::GetConsole()->AddCommand

uint32_t chaosEffectNoUI;
uint32_t chaosEffectGiantLink;
uint32_t chaosEffectMinishLink;
uint32_t chaosEffectPaperLink;
uint32_t chaosEffectResetLinkScale;
uint32_t chaosEffectInvisibleLink;
uint32_t chaosEffectOneHitKO;
uint32_t chaosEffectPacifistMode;
int32_t chaosEffectDefenseModifier;
uint32_t chaosEffectNoZ;
uint32_t chaosEffectReverseControls;
uint32_t chaosEffectGravityLevel = GRAVITY_LEVEL_NORMAL;
int32_t chaosEffectSpeedModifier;

static bool ActorSpawnHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if ((args.size() != 9) && (args.size() != 3) && (args.size() != 6)) {
        SohImGui::GetConsole()->SendErrorMessage("Not enough arguments passed to actorspawn");
        return CMD_FAILED;
    }

    if (gPlayState == nullptr) {
        SohImGui::GetConsole()->SendErrorMessage("PlayState == nullptr");
        return CMD_FAILED;
    }

    Player* player = GET_PLAYER(gPlayState);
    PosRot spawnPoint;
    const s16 actorId = std::stoi(args[1]);
    const s16 params = std::stoi(args[2]);

    spawnPoint = player->actor.world;

    switch (args.size()) {
        case 9:
            if (args[6][0] != ',') {
                spawnPoint.rot.x = std::stoi(args[6]);
            }
            if (args[7][0] != ',') {
                spawnPoint.rot.y = std::stoi(args[7]);
            }
            if (args[8][0] != ',') {
                spawnPoint.rot.z = std::stoi(args[8]);
            }
        case 6:
            if (args[3][0] != ',') {
                spawnPoint.pos.x = std::stoi(args[3]);
            }
            if (args[4][0] != ',') {
                spawnPoint.pos.y = std::stoi(args[4]);
            }
            if (args[5][0] != ',') {
                spawnPoint.pos.z = std::stoi(args[5]);
            }
    }

    if (Actor_Spawn(&gPlayState->actorCtx, gPlayState, actorId, spawnPoint.pos.x, spawnPoint.pos.y, spawnPoint.pos.z,
                    spawnPoint.rot.x, spawnPoint.rot.y, spawnPoint.rot.z, params, 0) == NULL) {
        SohImGui::GetConsole()->SendErrorMessage("Failed to spawn actor. Actor_Spawn returned NULL");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool GiveDekuShieldHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>&) {
    // Give Deku Shield to the player, and automatically equip it when they're child and have no shield currently equiped.
    Player* player = GET_PLAYER(gPlayState);
    Item_Give(gPlayState, ITEM_SHIELD_DEKU);
    if (LINK_IS_CHILD && player->currentShield == PLAYER_SHIELD_NONE) {
        player->currentShield = PLAYER_SHIELD_DEKU;
        Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_DEKU);
    }
    SohImGui::GetConsole()->SendInfoMessage("[SOH] Gave Deku Shield");
    return CMD_SUCCESS;
}

static bool KillPlayerHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>&) {
    gSaveContext.health = 0;
    SohImGui::GetConsole()->SendInfoMessage("[SOH] You've met with a terrible fate, haven't you?");
    return CMD_SUCCESS;
}

static bool SetPlayerHealthHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    int health;

    try {
        health = std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Health value must be an integer.");
        return CMD_FAILED;
    }

    if (health < 0) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Health value must be a positive integer");
        return CMD_SUCCESS;
    }

    gSaveContext.health = health * 0x10;

    SohImGui::GetConsole()->SendInfoMessage("[SOH] Player health updated to %d", health);
    return CMD_SUCCESS;
}

static bool LoadSceneHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>&) {
    gSaveContext.respawnFlag = 0;
    gSaveContext.seqId = 0xFF;
    gSaveContext.gameMode = 0;

    return CMD_SUCCESS;
}

static bool RupeeHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return CMD_FAILED;
    }

    int rupeeAmount;
    try {
        rupeeAmount = std::stoi(args[1]);
    }
    catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Rupee count must be an integer.");
        return CMD_FAILED;
    }

    if (rupeeAmount < 0) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Rupee count must be positive");
        return CMD_FAILED;
    }

    gSaveContext.rupees = rupeeAmount;

    SohImGui::GetConsole()->SendInfoMessage("Set rupee count to %u", rupeeAmount);
    return CMD_SUCCESS;
}

static bool SetPosHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string> args) {
    if (gPlayState == nullptr) {
        SohImGui::GetConsole()->SendErrorMessage("PlayState == nullptr");
        return CMD_FAILED;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (args.size() == 1) {
        SohImGui::GetConsole()->SendInfoMessage("Player position is [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x,
                                            player->actor.world.pos.y,
             player->actor.world.pos.z);
        return CMD_SUCCESS;
    }
    if (args.size() < 4)
        return CMD_FAILED;

    player->actor.world.pos.x = std::stof(args[1]);
    player->actor.world.pos.y = std::stof(args[2]);
    player->actor.world.pos.z = std::stof(args[3]);

    SohImGui::GetConsole()->SendInfoMessage("Set player position to [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x,
                                        player->actor.world.pos.y,
         player->actor.world.pos.z);
    return CMD_SUCCESS;
}

static bool ResetHandler(std::shared_ptr<Ship::Console> Console, std::vector<std::string> args) {
    if (gPlayState == nullptr) {
        SohImGui::GetConsole()->SendErrorMessage("PlayState == nullptr");
        return CMD_FAILED;
    }

    SET_NEXT_GAMESTATE(&gPlayState->state, TitleSetup_Init, GameState);
    gPlayState->state.running = false;
    return CMD_SUCCESS;
}

const static std::map<std::string, uint16_t> ammoItems{
    { "sticks", ITEM_STICK }, { "deku_sticks", ITEM_STICK },
    { "nuts", ITEM_NUT },     { "deku_nuts", ITEM_NUT },
    { "bombs", ITEM_BOMB },      { "arrows", ITEM_BOW },
    { "bombchus", ITEM_BOMBCHU }, { "chus", ITEM_BOMBCHU },
    { "beans", ITEM_BEAN },
    { "seeds", ITEM_SLINGSHOT }, { "deku_seeds", ITEM_SLINGSHOT },
    { "magic_beans", ITEM_BEAN },
};

static bool AmmoHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 3) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    int count;

    try {
        count = std::stoi(args[2]);
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("Ammo count must be an integer");
        return CMD_FAILED;
    }

    if (count < 0) {
        SohImGui::GetConsole()->SendErrorMessage("Ammo count must be positive");
        return CMD_FAILED;
    }

    const auto& it = ammoItems.find(args[1]);

    if (it == ammoItems.end()) {
        SohImGui::GetConsole()->SendErrorMessage("Invalid item passed");
        return CMD_FAILED;
    }

    // I dont think you can do OOB with just this
    AMMO(it->second) = count;

    //To use a change by uncomment this
    //Inventory_ChangeAmmo(it->second, count);
}

const static std::map<std::string, uint16_t> bottleItems{
    { "green_potion", ITEM_POTION_GREEN },
    { "red_potion", ITEM_POTION_RED },
    { "blue_potion", ITEM_POTION_BLUE },
    { "milk", ITEM_MILK },    { "half_milk", ITEM_MILK_HALF },           { "fairy", ITEM_FAIRY },
    { "bugs", ITEM_BUG }, { "fish", ITEM_FISH },         { "poe", ITEM_POE },
    { "big_poe", ITEM_BIG_POE },  { "blue_fire", ITEM_BLUE_FIRE }, { "rutos_letter", ITEM_LETTER_RUTO },
};

static bool BottleHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 3) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    unsigned int slot;
    try {
        slot = std::stoi(args[2]);
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Bottle slot must be an integer.");
        return CMD_FAILED;
    }

    if ((slot < 1) || (slot > 4)) {
        SohImGui::GetConsole()->SendErrorMessage("Invalid slot passed");
        return CMD_FAILED;
    }

    const auto& it = bottleItems.find(args[1]);

    if (it ==  bottleItems.end()) {
        SohImGui::GetConsole()->SendErrorMessage("Invalid item passed");
        return CMD_FAILED;
    }

    // I dont think you can do OOB with just this
    gSaveContext.inventory.items[0x11 + slot] = it->second;

    return CMD_SUCCESS;
}

static bool BHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    gSaveContext.equips.buttonItems[0] = std::stoi(args[1]);
    return CMD_SUCCESS;
}

static bool ItemHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 3) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    gSaveContext.inventory.items[std::stoi(args[1])] = std::stoi(args[2]);

    return CMD_SUCCESS;
}

static bool GiveItemHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string> args) {
    if (args.size() != 3) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }
    GetItemEntry getItemEntry = GET_ITEM_NONE;

    if (args[1].compare("vanilla") == 0) {
        getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, std::stoi(args[2]));
    } else if (args[1].compare("randomizer") == 0) {
        getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_RANDOMIZER, std::stoi(args[2]));
    } else {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Invalid argument passed, must be 'vanilla' or 'randomizer'");
        return CMD_FAILED;
    }

    GiveItemEntryWithoutActor(gPlayState, getItemEntry);

    return CMD_SUCCESS;
}

static bool EntranceHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    unsigned int entrance;

    try {
        entrance = std::stoi(args[1], nullptr, 16);
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Entrance value must be a Hex number.");
        return CMD_FAILED;
    }

    gPlayState->nextEntranceIndex = entrance;
    gPlayState->sceneLoadFlag = 0x14;
    gPlayState->fadeTransition = 11;
    gSaveContext.nextTransitionType = 11;
}

static bool VoidHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gPlayState != nullptr) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags = gPlayState->actorCtx.flags.tempSwch;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags = gPlayState->actorCtx.flags.tempCollect;
            gSaveContext.respawnFlag = 1;
            gPlayState->sceneLoadFlag = 0x14;
            gPlayState->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex;
            gPlayState->fadeTransition = 2;
            gSaveContext.nextTransitionType = 2;
    } else {
        SohImGui::GetConsole()->SendErrorMessage("gPlayState == nullptr");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool ReloadHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gPlayState != nullptr) {
        gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;
        gPlayState->sceneLoadFlag = 0x14;
        gPlayState->fadeTransition = 11;
        gSaveContext.nextTransitionType = 11;
    } else {
        SohImGui::GetConsole()->SendErrorMessage("gPlayState == nullptr");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool FWHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gPlayState != nullptr) {
        if (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 0) {
                gPlayState->sceneLoadFlag = 0x14;
                gPlayState->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex;
                gPlayState->fadeTransition = 5;
        } else {
            SohImGui::GetConsole()->SendErrorMessage("Farore's wind not set!");
        }
    }
    else {
        SohImGui::GetConsole()->SendErrorMessage("gPlayState == nullptr");
        return CMD_FAILED;
    }
    
    return CMD_SUCCESS;
}

static bool FileSelectHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gPlayState != nullptr) {
        SET_NEXT_GAMESTATE(&gPlayState->state, FileChoose_Init, FileChooseContext);
        gPlayState->state.running = 0;
    } else {
        SohImGui::GetConsole()->SendErrorMessage("gPlayState == nullptr");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool QuitHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    gPlayState->state.running = 0;
    return CMD_SUCCESS;
}

static bool SaveStateHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
    const SaveStateReturn rtn = OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::SAVE });

    switch (rtn) {
        case SaveStateReturn::SUCCESS:
            SohImGui::GetConsole()->SendInfoMessage("[SOH] Saved state to slot %u", slot);
            return CMD_SUCCESS;
        case SaveStateReturn::FAIL_WRONG_GAMESTATE:
            SohImGui::GetConsole()->SendErrorMessage("[SOH] Can not save a state outside of \"GamePlay\"");
            return CMD_FAILED;
    }
}

static bool LoadStateHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
    const SaveStateReturn rtn = OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::LOAD });

    switch (rtn) {
        case SaveStateReturn::SUCCESS:
            SohImGui::GetConsole()->SendInfoMessage("[SOH] Loaded state from slot (%u)", slot);
            return CMD_SUCCESS;
        case SaveStateReturn::FAIL_INVALID_SLOT:
            SohImGui::GetConsole()->SendErrorMessage("[SOH] Invalid State Slot Number (%u)", slot);
            return CMD_FAILED;
        case SaveStateReturn::FAIL_STATE_EMPTY:
            SohImGui::GetConsole()->SendErrorMessage("[SOH] State Slot (%u) is empty", slot);
            return CMD_FAILED;
        case SaveStateReturn::FAIL_WRONG_GAMESTATE:
            SohImGui::GetConsole()->SendErrorMessage("[SOH] Can not load a state outside of \"GamePlay\"");
            return CMD_FAILED;
    }

}

static bool StateSlotSelectHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }
    int slot;

    try {
        slot = std::stoi(args[1], nullptr, 10);
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] SaveState slot value must be a number.");
        return CMD_FAILED;
    }

    if (slot < 0) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Invalid slot passed. Slot must be between 0 and 2");
        return CMD_FAILED;
    }

    OTRGlobals::Instance->gSaveStateMgr->SetCurrentSlot(slot);
    SohImGui::GetConsole()->SendInfoMessage("[SOH] Slot %u selected",
                                        OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot());
    return CMD_SUCCESS;
}

static bool InvisibleHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectInvisibleLink = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        if (!chaosEffectInvisibleLink) {
            Player* player = GET_PLAYER(gPlayState);
            player->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        }

        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Invisible value must be a number.");
        return CMD_FAILED;
    }
}

static bool GiantLinkHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectGiantLink = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        if (chaosEffectGiantLink) {
            chaosEffectPaperLink = 0;
            chaosEffectMinishLink = 0;
        } else {
            chaosEffectResetLinkScale = 1;
        }

        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Giant value must be a number.");
        return CMD_FAILED;
    }
}

static bool MinishLinkHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectMinishLink = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        if (chaosEffectMinishLink) {
            chaosEffectPaperLink = 0;
            chaosEffectGiantLink = 0;
        } else {
            chaosEffectResetLinkScale = 1;
        }

        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Minish value must be a number.");
        return CMD_FAILED;
    }
}

static bool AddHeartContainerHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gSaveContext.healthCapacity >= 0x140)
        return CMD_FAILED;

    Health_GiveHearts(1);
    return CMD_SUCCESS;
}

static bool RemoveHeartContainerHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if ((gSaveContext.healthCapacity - 0x10) < 3)
        return CMD_FAILED;

    Health_RemoveHearts(1);
    return CMD_SUCCESS;
}

static bool GravityHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectGravityLevel = Ship::Math::clamp(std::stoi(args[1], nullptr, 10), GRAVITY_LEVEL_LIGHT, GRAVITY_LEVEL_HEAVY);
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Minish value must be a number.");
        return CMD_FAILED;
    }
}

static bool NoUIHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectNoUI = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] No UI value must be a number.");
        return CMD_FAILED;
    }
}

static bool FreezeHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    gSaveContext.pendingIceTrapCount++;
    return CMD_SUCCESS;
}

static bool DefenseModifierHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectDefenseModifier = std::stoi(args[1], nullptr, 10);
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Defense modifier value must be a number.");
        return CMD_FAILED;
    }
}

static bool DamageHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        int value = std::stoi(args[1], nullptr, 10);
        if (value < 0) {
            SohImGui::GetConsole()->SendErrorMessage("[SOH] Invalid value passed. Value must be greater than 0");
            return CMD_FAILED;
        }

        Player* player = GET_PLAYER(gPlayState);

        Health_ChangeBy(gPlayState, -value * 0x10);
        func_80837C0C(gPlayState, player, 0, 0, 0, 0, 0);
        player->invincibilityTimer = 28;

        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Damage value must be a number.");
        return CMD_FAILED;
    }
}

static bool HealHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        int value = std::stoi(args[1], nullptr, 10);
        if (value < 0) {
            SohImGui::GetConsole()->SendErrorMessage("[SOH] Invalid value passed. Value must be greater than 0");
            return CMD_FAILED;
        }

        Health_ChangeBy(gPlayState, value * 0x10);
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Heal value must be a number.");
        return CMD_FAILED;
    }
}

static bool FillMagicHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    Magic_Fill(gPlayState);
    return CMD_SUCCESS;
}

static bool EmptyMagicHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    gSaveContext.magic = 0;
    return CMD_SUCCESS;
}

static bool NoZHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
     if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectNoZ = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] NoZ value must be a number.");
        return CMD_FAILED;
    }
}

static bool OneHitKOHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectOneHitKO = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] One-hit KO value must be a number.");
        return CMD_FAILED;
    }
}

static bool PacifistHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectPacifistMode = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        // Force interface to update to make the buttons transparent
        gSaveContext.unk_13E8 = 50;
        Interface_Update(gPlayState);
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Pacifist value must be a number.");
        return CMD_FAILED;
    }
}

static bool PaperLinkHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectPaperLink = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        if (chaosEffectPaperLink) {
            chaosEffectMinishLink = 0;
            chaosEffectGiantLink = 0;
        } else {
            chaosEffectResetLinkScale = 1;
        }
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Paper Link value must be a number.");
        return CMD_FAILED;
    }
}

static bool RainstormHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        uint32_t rainstorm = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        if (rainstorm) {
            gPlayState->envCtx.unk_F2[0] = 20;    // rain intensity target
            gPlayState->envCtx.gloomySkyMode = 1; // start gloomy sky
            if ((gWeatherMode != 0) || gPlayState->envCtx.unk_17 != 0) {
                gPlayState->envCtx.unk_DE = 1;
            }
            gPlayState->envCtx.lightningMode = LIGHTNING_MODE_ON;
            Environment_PlayStormNatureAmbience(gPlayState);
        } else {
            gPlayState->envCtx.unk_F2[0] = 0;
            if (gPlayState->csCtx.state == CS_STATE_IDLE) {
                Environment_StopStormNatureAmbience(gPlayState);
            } else if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) == NA_BGM_NATURE_AMBIENCE) {
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
            }
            osSyncPrintf("\n\n\nE_wether_flg=[%d]", gWeatherMode);
            osSyncPrintf("\nrain_evt_trg=[%d]\n\n", gPlayState->envCtx.gloomySkyMode);
            if (gWeatherMode == 0 && (gPlayState->envCtx.gloomySkyMode == 1)) {
                gPlayState->envCtx.gloomySkyMode = 2; // end gloomy sky
            } else {
                gPlayState->envCtx.gloomySkyMode = 0;
                gPlayState->envCtx.unk_DE = 0;
            }
            gPlayState->envCtx.lightningMode = LIGHTNING_MODE_LAST;
        }


        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] rainstorm value must be a number.");
        return CMD_FAILED;
    }
}

static bool ReverseControlsHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectReverseControls = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Reverse controls value must be a number.");
        return CMD_FAILED;
    }
}

static bool UpdateRupeesHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        int value = std::stoi(args[1], nullptr, 10);
        Rupees_ChangeBy(value);
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Rupee value must be a number.");
        return CMD_FAILED;
    }
}

static bool SpeedModifierHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        chaosEffectSpeedModifier = std::stoi(args[1], nullptr, 10);
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Speed modifier value must be a number.");
        return CMD_FAILED;
    }
}

const static std::map<std::string, uint16_t> boots {
    { "kokiri", PLAYER_BOOTS_KOKIRI },
    { "iron", PLAYER_BOOTS_IRON },
    { "hover", PLAYER_BOOTS_HOVER },
};

static bool BootsHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    const auto& it = boots.find(args[1]);
    if (it == boots.end()) {
        SohImGui::GetConsole()->SendErrorMessage("Invalid boot type. Options are 'kokiri', 'iron' and 'hover'");
        return CMD_FAILED;
    }

    Player* player = GET_PLAYER(gPlayState);
    player->currentBoots = it->second;
    Inventory_ChangeEquipment(EQUIP_BOOTS, it->second + 1);
    Player_SetBootData(gPlayState, player);

    return CMD_SUCCESS;
}

static bool KnockbackHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    try {
        int value = std::stoi(args[1], nullptr, 10);
        if (value < 0) {
            SohImGui::GetConsole()->SendErrorMessage("[SOH] Invalid value passed. Value must be greater than 0");
            return CMD_FAILED;
        }

        Player* player = GET_PLAYER(gPlayState);
        func_8002F71C(gPlayState, &player->actor, value * 5, player->actor.world.rot.y + 0x8000, value * 5);
    
        return CMD_SUCCESS;
    } catch (std::invalid_argument const& ex) {
        SohImGui::GetConsole()->SendErrorMessage("[SOH] Knockback value must be a number.");
        return CMD_FAILED;
    }
}

static bool ElectrocuteHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    Player* player = GET_PLAYER(gPlayState);
    if (PlayerGrounded(player)) {
        func_80837C0C(gPlayState, player, 4, 0, 0, 0, 0);
        return CMD_SUCCESS;
    }

    return CMD_FAILED;
}

static bool BurnHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    Player* player = GET_PLAYER(gPlayState);
    if (PlayerGrounded(player)) {
        for (int i = 0; i < 18; i++) {
            player->flameTimers[i] = Rand_S16Offset(0, 200);
        }
        player->isBurning = true;
        func_80837C0C(gPlayState, player, 0, 0, 0, 0, 0);
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool CuccoStormHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    Player* player = GET_PLAYER(gPlayState);
    EnNiw* cucco = (EnNiw*)Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_NIW, player->actor.world.pos.x,
                                       player->actor.world.pos.y + 2200, player->actor.world.pos.z, 0, 0, 0, 0, 0);
    cucco->actionFunc = func_80AB70A0_nocutscene;
    return CMD_SUCCESS;
}

#define VARTYPE_INTEGER 0
#define VARTYPE_FLOAT   1
#define VARTYPE_STRING  2
#define VARTYPE_RGBA    3

static int CheckVarType(const std::string& input)
{
    int result = VARTYPE_STRING;

    if (input[0] == '#') {
        return VARTYPE_RGBA;
    }

    for (size_t i = 0; i < input.size(); i++)
    {
        if (!(std::isdigit(input[i]) || input[i] == '.'))
        {
            break;
        }
        else
        {
            if (input[i] == '.')
                result = VARTYPE_FLOAT;
            else if (std::isdigit(input[i]) && result != VARTYPE_FLOAT)
                result = VARTYPE_INTEGER;
        }
    }

    return result;
}

static bool SetCVarHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() < 3)
        return CMD_FAILED;

    int vType = CheckVarType(args[2]);

    if (vType == VARTYPE_STRING)
        CVar_SetString(args[1].c_str(), args[2].c_str());
    else if (vType == VARTYPE_FLOAT)
        CVar_SetFloat((char*)args[1].c_str(), std::stof(args[2]));
    else if (vType == VARTYPE_RGBA)
    {
        uint32_t val = std::stoul(&args[2].c_str()[1], nullptr, 16);
        Color_RGBA8 clr;
        clr.r = val >> 24;
        clr.g = val >> 16;
        clr.b = val >> 8;
        clr.a = val & 0xFF;
        CVar_SetRGBA((char*)args[1].c_str(), clr);
    }
    else
        CVar_SetS32(args[1].c_str(), std::stoi(args[2]));

    CVar_Save();

    //SohImGui::GetConsole()->SendInfoMessage("[SOH] Updated player position to [ %.2f, %.2f, %.2f ]", pos.x, pos.y, pos.z);
    return CMD_SUCCESS;
}

static bool GetCVarHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() < 2)
        return CMD_FAILED;

    CVar* cvar = CVar_Get(args[1].c_str());

    if (cvar != nullptr)
    {
        if (cvar->Type == CVarType::S32)
            SohImGui::GetConsole()->SendInfoMessage("[SOH] Variable %s is %i", args[1].c_str(), cvar->value.ValueS32);
        else if (cvar->Type == CVarType::Float)
            SohImGui::GetConsole()->SendInfoMessage("[SOH] Variable %s is %f", args[1].c_str(), cvar->value.ValueFloat);
        else if (cvar->Type == CVarType::String)
            SohImGui::GetConsole()->SendInfoMessage("[SOH] Variable %s is %s", args[1].c_str(), cvar->value.ValueStr);
        else if (cvar->Type == CVarType::RGBA)
            SohImGui::GetConsole()->SendInfoMessage("[SOH] Variable %s is %08X", args[1].c_str(), cvar->value.ValueRGBA);
    }
    else
    {
        SohImGui::GetConsole()->SendInfoMessage("[SOH] Could not find variable %s", args[1].c_str());
    }


    return CMD_SUCCESS;
}

void DebugConsole_Init(void) {
    // Console
    CMD_REGISTER("file_select", { FileSelectHandler, "Returns to the file select." });
    CMD_REGISTER("reset", { ResetHandler, "Resets the game." });
    CMD_REGISTER("quit", { QuitHandler, "Quits the game." });

    // Save States
    CMD_REGISTER("save_state", { SaveStateHandler, "Save a state." });
    CMD_REGISTER("load_state", { LoadStateHandler, "Load a state." });
    CMD_REGISTER("set_slot", { StateSlotSelectHandler, "Selects a SaveState slot", {
        { "Slot number", Ship::ArgumentType::NUMBER, }
    }});

    // Map & Location
    CMD_REGISTER("void", { VoidHandler, "Voids out of the current map." });
    CMD_REGISTER("reload", { ReloadHandler, "Reloads the current map." });
    CMD_REGISTER("fw", { FWHandler,"Spawns the player where Farore's Wind is set." });
    CMD_REGISTER("entrance", { EntranceHandler, "Sends player to the entered entrance (hex)", {
        { "entrance", Ship::ArgumentType::NUMBER }
    }});

    // Gameplay
    CMD_REGISTER("kill", { KillPlayerHandler, "Commit suicide." });

    CMD_REGISTER("map",  { LoadSceneHandler, "Load up kak?" });

    CMD_REGISTER("rupee", { RupeeHandler, "Set your rupee counter.", {
        {"amount", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("bItem", { BHandler, "Set an item to the B button.", {
        { "Item ID", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("givedekushield", { GiveDekuShieldHandler, "Gives a deku shield and equips it when Link is a child with no shield equiped." });

    CMD_REGISTER("spawn", { ActorSpawnHandler, "Spawn an actor.", { { "actor_id", Ship::ArgumentType::NUMBER },
                              { "data", Ship::ArgumentType::NUMBER },
                              { "x", Ship::ArgumentType::PLAYER_POS, true },
                              { "y", Ship::ArgumentType::PLAYER_POS, true },
                              { "z", Ship::ArgumentType::PLAYER_POS, true },
                              { "rx", Ship::ArgumentType::PLAYER_ROT, true },
                              { "ry", Ship::ArgumentType::PLAYER_ROT, true },
                              { "rz", Ship::ArgumentType::PLAYER_ROT, true }
    }});

    CMD_REGISTER("pos", { SetPosHandler, "Sets the position of the player.", {
        { "x", Ship::ArgumentType::PLAYER_POS, true },
        { "y", Ship::ArgumentType::PLAYER_POS, true },
        { "z", Ship::ArgumentType::PLAYER_POS, true }
    }});

    CMD_REGISTER("set", { SetCVarHandler,  "Sets a console variable.", {
        { "varName", Ship::ArgumentType::TEXT },
        { "varValue", Ship::ArgumentType::TEXT }
    }});

    CMD_REGISTER("get", { GetCVarHandler, "Gets a console variable.", {
        { "varName", Ship::ArgumentType::TEXT }
    }});
    
    CMD_REGISTER("ammo", { AmmoHandler, "Changes ammo of an item.", {
        { "item", Ship::ArgumentType::TEXT },
        { "count", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("bottle", { BottleHandler, "Changes item in a bottle slot.", {
        { "item", Ship::ArgumentType::TEXT },
        { "slot", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("give_item", { GiveItemHandler,  "Gives an item to the player as if it was given from an actor", {
        { "vanilla|randomizer", Ship::ArgumentType::TEXT },
        { "giveItemID", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("item", { ItemHandler,  "Sets item ID in arg 1 into slot arg 2. No boundary checks. Use with caution.", {
        { "slot", Ship::ArgumentType::NUMBER },
        { "item id", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("invisible", { InvisibleHandler, "Activate Link's Elvish cloak, making him appear invisible.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("giant_link", { GiantLinkHandler, "Turn Link into a giant Lonky boi.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("minish_link", { MinishLinkHandler, "Turn Link into a minish boi.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("add_heart_container", { AddHeartContainerHandler, "Give Link a heart! The maximum amount of hearts is 20!" });

    CMD_REGISTER("remove_heart_container", { RemoveHeartContainerHandler, "Remove a heart from Link. The minimal amount of hearts is 3." });

    CMD_REGISTER("gravity", { GravityHandler, "Set gravity level.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("no_ui", { NoUIHandler, "Disables the UI.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("freeze", { FreezeHandler, "Freezes Link in place" });

    CMD_REGISTER("defense_modifier", { DefenseModifierHandler, "Sets the defense modifier.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("damage", { DamageHandler, "Deal damage to Link.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("heal", { HealHandler, "Heals Link.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("fill_magic", { FillMagicHandler, "Fills magic." });

    CMD_REGISTER("empty_magic", { EmptyMagicHandler, "Empties magic." });

    CMD_REGISTER("no_z", { NoZHandler, "Disables Z-button presses.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("ohko", { OneHitKOHandler, "Activates one hit KO. Any damage kills Link and he cannot gain health in this mode.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("pacifist", { PacifistHandler, "Activates pacifist mode. Prevents Link from using his weapon.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("paper_link", { PaperLinkHandler, "Link but made out of paper.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("rainstorm", { RainstormHandler, "Activates rainstorm." });

    CMD_REGISTER("reverse_controls", { ReverseControlsHandler, "Reverses the controls.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("update_rupees", { UpdateRupeesHandler, "Adds rupees.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("speed_modifier", { SpeedModifierHandler, "Sets the speed modifier.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("boots", { BootsHandler, "Activates boots.", {
        { "type", Ship::ArgumentType::TEXT },
    }});

    CMD_REGISTER("knockback", { KnockbackHandler, "Knocks Link back.", {
        { "value", Ship::ArgumentType::NUMBER }
    }});

    CMD_REGISTER("electrocute", { ElectrocuteHandler, "Electrocutes Link." });

    CMD_REGISTER("burn", { BurnHandler, "Burns Link." });

    CMD_REGISTER("cucco_storm", { CuccoStormHandler, "Cucco Storm" });

    CVar_Load();
}

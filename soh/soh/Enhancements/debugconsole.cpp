#include "debugconsole.h"
#include "../libultraship/ImGuiImpl.h"
#include "savestates.h"
#include "Console.h"

#include <vector>
#include <string>
#include "soh/OTRGlobals.h"


#define Path _Path
#define PATH_HACK
#include <Utils/StringHelper.h>

#include "Window.h"
#include "Lib/ImGui/imgui_internal.h"
#undef PATH_HACK
#undef Path

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;
}

#include "Cvar.h"

#define CMD_REGISTER SohImGui::BindCmd

static bool ActorSpawnHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if ((args.size() != 9) && (args.size() != 3) && (args.size() != 6)) {
        SohImGui::console->SendErrorMessage("Not enough arguments passed to actorspawn");
        return CMD_FAILED;
    }

    if (gGlobalCtx == nullptr) {
        SohImGui::console->SendErrorMessage("GlobalCtx == nullptr");
        return CMD_FAILED;
    }

    Player* player = GET_PLAYER(gGlobalCtx);
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
        case 5:
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

    if (Actor_Spawn(&gGlobalCtx->actorCtx, gGlobalCtx, actorId, spawnPoint.pos.x, spawnPoint.pos.y, spawnPoint.pos.z,
                    spawnPoint.rot.x, spawnPoint.rot.y, spawnPoint.rot.z, params) == NULL) {
        SohImGui::console->SendErrorMessage("Failed to spawn actor. Actor_Spawn returned NULL");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}


static bool KillPlayerHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>&) {
    gSaveContext.health = 0;
    SohImGui::console->SendInfoMessage("[SOH] You've met with a terrible fate, haven't you?");
    return CMD_SUCCESS;
}

static bool SetPlayerHealthHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::console->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    int health;

    try {
        health = std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        SohImGui::console->SendErrorMessage("[SOH] Health value must be an integer.");
        return CMD_FAILED;
    }

    if (health < 0) {
        SohImGui::console->SendErrorMessage("[SOH] Health value must be a positive integer");
        return CMD_SUCCESS;
    }

    gSaveContext.health = health * 0x10;

    SohImGui::console->SendInfoMessage("[SOH] Player health updated to %d", health);
    return CMD_SUCCESS;
}


static bool LoadSceneHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>&) {
    gSaveContext.respawnFlag = 0;
    gSaveContext.seqId = 0xFF;
    gSaveContext.gameMode = 0;

    return CMD_SUCCESS;
}

static bool RuppeHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() < 2)
        return CMD_FAILED;

    int rupeeAmount;
    try {
        rupeeAmount = std::stoi(args[1]);
    }
    catch (std::invalid_argument const& ex) {
        SohImGui::console->SendErrorMessage("[SOH] Rupee count must be an integer.");
        return CMD_FAILED;
    }

    if (rupeeAmount < 0) {
        SohImGui::console->SendErrorMessage("[SOH] Rupee count must be positive");
        return CMD_FAILED;
    }

    gSaveContext.rupees = rupeeAmount;

    SohImGui::console->SendInfoMessage("Set rupee count to %u", rupeeAmount);
    return CMD_SUCCESS;
}

static bool SetPosHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string> args) {
    if (gGlobalCtx == nullptr) {
        SohImGui::console->SendErrorMessage("GlobalCtx == nullptr");
        return CMD_FAILED;
    }

    Player* player = GET_PLAYER(gGlobalCtx);

    if (args.size() == 1) {
        SohImGui::console->SendInfoMessage("Player position is [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x,
                                            player->actor.world.pos.y,
             player->actor.world.pos.z);
        return CMD_SUCCESS;
    }
    if (args.size() < 4)
        return CMD_FAILED;

    player->actor.world.pos.x = std::stof(args[1]);
    player->actor.world.pos.y = std::stof(args[2]);
    player->actor.world.pos.z = std::stof(args[3]);

    SohImGui::console->SendInfoMessage("Set player position to [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x,
                                        player->actor.world.pos.y,
         player->actor.world.pos.z);
    return CMD_SUCCESS;
}

static bool ResetHandler(std::shared_ptr<Ship::Console> Console, std::vector<std::string> args) {
    if (gGlobalCtx == nullptr) {
        SohImGui::console->SendErrorMessage("GlobalCtx == nullptr");
        return CMD_FAILED;
    }

    SET_NEXT_GAMESTATE(&gGlobalCtx->state, TitleSetup_Init, GameState);
    gGlobalCtx->state.running = false;
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
        SohImGui::console->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    int count;

    try {
        count = std::stoi(args[2]);
    } catch (std::invalid_argument const& ex) {
        SohImGui::console->SendErrorMessage("Ammo count must be an integer");
        return CMD_FAILED;
    }

    if (count < 0) {
        SohImGui::console->SendErrorMessage("Ammo count must be positive");
        return CMD_FAILED;
    }

    const auto& it = ammoItems.find(args[1]);

    if (it == ammoItems.end()) {
        SohImGui::console->SendErrorMessage("Invalid item passed");
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
        SohImGui::console->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    unsigned int slot;
    try {
        slot = std::stoi(args[2]);
    } catch (std::invalid_argument const& ex) {
        SohImGui::console->SendErrorMessage("[SOH] Bottle slot must be an integer.");
        return CMD_FAILED;
    }

    if ((slot < 1) || (slot > 4)) {
        SohImGui::console->SendErrorMessage("Invalid slot passed");
        return CMD_FAILED;
    }

    const auto& it = bottleItems.find(args[1]);

    if (it ==  bottleItems.end()) {
        SohImGui::console->SendErrorMessage("Invalid item passed");
        return CMD_FAILED;
    }

    // I dont think you can do OOB with just this
    gSaveContext.inventory.items[0x11 + slot] = it->second;

    return CMD_SUCCESS;
}

static bool BHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::console->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    gSaveContext.equips.buttonItems[0] = std::stoi(args[1]);
    return CMD_SUCCESS;
}

static bool ItemHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 3) {
        SohImGui::console->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    gSaveContext.inventory.items[std::stoi(args[1])] = std::stoi(args[2]);

    return CMD_SUCCESS;
}

static bool EntranceHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::console->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }

    unsigned int entrance;

    try {
        entrance = std::stoi(args[1], nullptr, 16);
    } catch (std::invalid_argument const& ex) {
        SohImGui::console->SendErrorMessage("[SOH] Entrance value must be a Hex number.");
        return CMD_FAILED;
    }

    gGlobalCtx->nextEntranceIndex = entrance;
    gGlobalCtx->sceneLoadFlag = 0x14;
    gGlobalCtx->fadeTransition = 11;
    gSaveContext.nextTransition = 11;
}

static bool VoidHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gGlobalCtx != nullptr) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags = gGlobalCtx->actorCtx.flags.tempSwch;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags = gGlobalCtx->actorCtx.flags.tempCollect;
            gSaveContext.respawnFlag = 1;
            gGlobalCtx->sceneLoadFlag = 0x14;
            gGlobalCtx->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex;
            gGlobalCtx->fadeTransition = 2;
            gSaveContext.nextTransition = 2;
    } else {
        SohImGui::console->SendErrorMessage("gGlobalCtx == nullptr");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool ReloadHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gGlobalCtx != nullptr) {
        gGlobalCtx->nextEntranceIndex = gSaveContext.entranceIndex;
        gGlobalCtx->sceneLoadFlag = 0x14;
        gGlobalCtx->fadeTransition = 11;
        gSaveContext.nextTransition = 11;
    } else {
        SohImGui::console->SendErrorMessage("gGlobalCtx == nullptr");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool FWHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gGlobalCtx != nullptr) {
        if (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 0) {
                gGlobalCtx->sceneLoadFlag = 0x14;
                gGlobalCtx->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex;
                gGlobalCtx->fadeTransition = 5;
        } else {
            SohImGui::console->SendErrorMessage("Farore's wind not set!");
        }
    }
    else {
        SohImGui::console->SendErrorMessage("gGlobalCtx == nullptr");
        return CMD_FAILED;
    }
    
    return CMD_SUCCESS;
}

static bool FileSelectHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (gGlobalCtx != nullptr) {
        SET_NEXT_GAMESTATE(&gGlobalCtx->state, FileChoose_Init, FileChooseContext);
        gGlobalCtx->state.running = 0;
    } else {
        SohImGui::console->SendErrorMessage("gGlobalCtx == nullptr");
        return CMD_FAILED;
    }
    return CMD_SUCCESS;
}

static bool QuitHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    gGlobalCtx->state.running = 0;
    return CMD_SUCCESS;
}

static bool SaveStateHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
    const SaveStateReturn rtn = OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::SAVE });

    switch (rtn) {
        case SaveStateReturn::SUCCESS:
            SohImGui::console->SendInfoMessage("[SOH] Saved state to slot %u", slot);
            return CMD_SUCCESS;
        case SaveStateReturn::FAIL_WRONG_GAMESTATE:
            SohImGui::console->SendErrorMessage("[SOH] Can not save a state outside of \"GamePlay\"");
            return CMD_FAILED;
    }
}

static bool LoadStateHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
    const SaveStateReturn rtn = OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::LOAD });

    switch (rtn) {
        case SaveStateReturn::SUCCESS:
            SohImGui::console->SendInfoMessage("[SOH] Loaded state from slot (%u)", slot);
            return CMD_SUCCESS;
        case SaveStateReturn::FAIL_INVALID_SLOT:
            SohImGui::console->SendErrorMessage("[SOH] Invalid State Slot Number (%u)", slot);
            return CMD_FAILED;
        case SaveStateReturn::FAIL_STATE_EMPTY:
            SohImGui::console->SendErrorMessage("[SOH] State Slot (%u) is empty", slot);
            return CMD_FAILED;
        case SaveStateReturn::FAIL_WRONG_GAMESTATE:
            SohImGui::console->SendErrorMessage("[SOH] Can not load a state outside of \"GamePlay\"");
            return CMD_FAILED;
    }

}

static bool StateSlotSelectHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        SohImGui::console->SendErrorMessage("[SOH] Unexpected arguments passed");
        return CMD_FAILED;
    }
    int slot;

    try {
        slot = std::stoi(args[1], nullptr, 10);
    } catch (std::invalid_argument const& ex) {
        SohImGui::console->SendErrorMessage("[SOH] SaveState slot value must be a number.");
        return CMD_FAILED;
    }

    if (slot < 0) {
        SohImGui::console->SendErrorMessage("[SOH] Invalid slot passed. Slot must be between 0 and 2");
        return CMD_FAILED;
    }

    OTRGlobals::Instance->gSaveStateMgr->SetCurrentSlot(slot);
    SohImGui::console->SendInfoMessage("[SOH] Slot %u selected",
                                        OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot());
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

    //SohImGui::console->SendInfoMessage("[SOH] Updated player position to [ %.2f, %.2f, %.2f ]", pos.x, pos.y, pos.z);
    return CMD_SUCCESS;
}


static bool GetCVarHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args) {
    if (args.size() < 2)
        return CMD_FAILED;

    CVar* cvar = CVar_Get(args[1].c_str());

    if (cvar != nullptr)
    {
        if (cvar->type == CVarType::S32)
            SohImGui::console->SendInfoMessage("[SOH] Variable %s is %i", args[1].c_str(), cvar->value.valueS32);
        else if (cvar->type == CVarType::Float)
            SohImGui::console->SendInfoMessage("[SOH] Variable %s is %f", args[1].c_str(), cvar->value.valueFloat);
        else if (cvar->type == CVarType::String)
            SohImGui::console->SendInfoMessage("[SOH] Variable %s is %s", args[1].c_str(), cvar->value.valueStr);
        else if (cvar->type == CVarType::RGBA)
            SohImGui::console->SendInfoMessage("[SOH] Variable %s is %08X", args[1].c_str(), cvar->value.valueRGBA);
    }
    else
    {
        SohImGui::console->SendInfoMessage("[SOH] Could not find variable %s", args[1].c_str());
    }


    return CMD_SUCCESS;
}

void DebugConsole_Init(void) {
    CMD_REGISTER("kill", { KillPlayerHandler, "Commit suicide." });
    CMD_REGISTER("map",  { LoadSceneHandler, "Load up kak?" });
    CMD_REGISTER("rupee", { RuppeHandler, "Set your rupee counter.", {
        {"amount", Ship::ArgumentType::NUMBER }
    }});
    CMD_REGISTER("bItem", { BHandler, "Set an item to the B button.", { { "Item ID", Ship::ArgumentType::NUMBER } } });
    CMD_REGISTER("health", { SetPlayerHealthHandler, "Set the health of the player.", { { "health", Ship::ArgumentType::NUMBER }
    }});
    CMD_REGISTER("spawn", { ActorSpawnHandler, "Spawn an actor.", { { "actor_id", Ship::ArgumentType::NUMBER },
                              { "data", Ship::ArgumentType::NUMBER },
                              { "x", Ship::ArgumentType::PLAYER_POS, true },
                              { "y", Ship::ArgumentType::PLAYER_POS, true },
                              { "z", Ship::ArgumentType::PLAYER_POS, true },
                              { "rx", Ship::ArgumentType::PLAYER_ROT, true },
                              { "ry", Ship::ArgumentType::PLAYER_ROT, true },
                              { "rz", Ship::ArgumentType::PLAYER_ROT, true }
    }});
    CMD_REGISTER("pos", { SetPosHandler, "Sets the position of the player.", { { "x", Ship::ArgumentType::PLAYER_POS, true },
                            { "y", Ship::ArgumentType::PLAYER_POS, true },
                            { "z", Ship::ArgumentType::PLAYER_POS, true }
    }});
    CMD_REGISTER("set", { SetCVarHandler,
                          "Sets a console variable.",
                          { { "varName", Ship::ArgumentType::TEXT }, { "varValue", Ship::ArgumentType::TEXT } } });
    CMD_REGISTER("get", { GetCVarHandler, "Gets a console variable.", { { "varName", Ship::ArgumentType::TEXT } } });
    CMD_REGISTER("reset", { ResetHandler, "Resets the game." });
    CMD_REGISTER("ammo", { AmmoHandler, "Changes ammo of an item.",
                           { { "item", Ship::ArgumentType::TEXT }, { "count", Ship::ArgumentType::NUMBER } } });

    CMD_REGISTER("bottle", { BottleHandler,
                       "Changes item in a bottle slot.",
                             { { "item", Ship::ArgumentType::TEXT }, { "slot", Ship::ArgumentType::NUMBER } } });

    CMD_REGISTER("item", { ItemHandler,
                             "Sets item ID in arg 1 into slot arg 2. No boundary checks. Use with caution.",
                           { { "slot", Ship::ArgumentType::NUMBER }, { "item id", Ship::ArgumentType::NUMBER } } });
    CMD_REGISTER("entrance", { EntranceHandler,
                               "Sends player to the entered entrance (hex)",
                               { { "entrance", Ship::ArgumentType::NUMBER } } });
    CMD_REGISTER("void", {VoidHandler, "Voids out of the current map.",});
    CMD_REGISTER("reload", {ReloadHandler, "Reloads the current map.",});
    CMD_REGISTER("file_select", {FileSelectHandler, "Returns to the file select.",});
    CMD_REGISTER("fw", {FWHandler,"Spawns the player where Farore's Wind is set.", });
    CMD_REGISTER("quit", {QuitHandler, "Quits the game.",});

    CMD_REGISTER("save_state", { SaveStateHandler, "Save a state." });
    CMD_REGISTER("load_state", { LoadStateHandler, "Load a state." });
    CMD_REGISTER("set_slot", { StateSlotSelectHandler, "Selects a SaveState slot", { {
                                   "Slot number",
                                   Ship::ArgumentType::NUMBER,
                               }
        } });
    CVar_Load();
}

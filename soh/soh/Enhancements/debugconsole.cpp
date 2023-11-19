#include "debugconsole.h"
#include <Utils.h>
#include "savestates.h"
#include "soh/ActorDB.h"

#include <vector>
#include <string>
#include "soh/OTRGlobals.h"
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"
#include "soh/Enhancements/audio/AudioEditor.h"

#define Path _Path
#define PATH_HACK
#include <Utils/StringHelper.h>

#include <Window.h>
#include <Context.h>
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

#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

#define CMD_REGISTER LUS::Context::GetInstance()->GetConsole()->AddCommand
// TODO: Commands should be using the output passed in.
#define ERROR_MESSAGE std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->SendErrorMessage
#define INFO_MESSAGE std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->SendInfoMessage

static bool ActorSpawnHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if ((args.size() != 9) && (args.size() != 3) && (args.size() != 6)) {
        ERROR_MESSAGE("Not enough arguments passed to actorspawn");
        return 1;
    }

    if (gPlayState == nullptr) {
        ERROR_MESSAGE("PlayState == nullptr");
        return 1;
    }

    Player* player = GET_PLAYER(gPlayState);
    PosRot spawnPoint;
    const s16 nameId = ActorDB::Instance->RetrieveId(args[1]);
    s16 actorId = 0;
    if (nameId == -1) {
        try {
            actorId = std::stoi(args[1]);
        } catch (std::invalid_argument const& ex) {
            ERROR_MESSAGE("Invalid actor ID");
            return 1;
        }
    } else {
        actorId = nameId;
    }
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
        ERROR_MESSAGE("Failed to spawn actor. Actor_Spawn returned NULL");
        return 1;
    }
    return 0;
}

static bool KillPlayerHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>&, std::string* output) {
    GameInteractionEffectBase* effect = new GameInteractionEffect::SetPlayerHealth();
    effect->parameters[0] = 0;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] You've met with a terrible fate, haven't you?");
        return 0;
    } else {
        ERROR_MESSAGE("[SOH] Command failed: Could not kill player.");
        return 1;
    }
}

static bool SetPlayerHealthHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    int health;

    try {
        health = std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Health value must be an integer.");
        return 1;
    }

    if (health < 0) {
        ERROR_MESSAGE("[SOH] Health value must be a positive integer");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::SetPlayerHealth();
    effect->parameters[0] = health;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Player health updated to %d", health);
        return 0;
    } else {
        ERROR_MESSAGE("[SOH] Command failed: Could not set player health.");
        return 1;
    }
}

static bool LoadSceneHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>&, std::string* output) {
    gSaveContext.respawnFlag = 0;
    gSaveContext.seqId = 0xFF;
    gSaveContext.gameMode = 0;

    return 0;
}

static bool RupeeHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        return 1;
    }

    int rupeeAmount;
    try {
        rupeeAmount = std::stoi(args[1]);
    }
    catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Rupee count must be an integer.");
        return 1;
    }

    if (rupeeAmount < 0) {
        ERROR_MESSAGE("[SOH] Rupee count must be positive");
        return 1;
    }

   gSaveContext.rupees = rupeeAmount;

    INFO_MESSAGE("Set rupee count to %u", rupeeAmount);
    return 0;
}

static bool SetPosHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string> args, std::string* output) {
    if (gPlayState == nullptr) {
        ERROR_MESSAGE("PlayState == nullptr");
        return 1;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (args.size() == 1) {
        INFO_MESSAGE("Player position is [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x,
                                            player->actor.world.pos.y,
             player->actor.world.pos.z);
        return 0;
    }
    if (args.size() < 4)
        return 1;

    player->actor.world.pos.x = std::stof(args[1]);
    player->actor.world.pos.y = std::stof(args[2]);
    player->actor.world.pos.z = std::stof(args[3]);

    INFO_MESSAGE("Set player position to [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x,
                                        player->actor.world.pos.y,
         player->actor.world.pos.z);
    return 0;
}

static bool ResetHandler(std::shared_ptr<LUS::Console> Console, std::vector<std::string> args, std::string* output) {
    if (gPlayState == nullptr) {
        ERROR_MESSAGE("PlayState == nullptr");
        return 1;
    }

    gPlayState->gameplayFrames = 0;
    SET_NEXT_GAMESTATE(&gPlayState->state, TitleSetup_Init, GameState);
    gPlayState->state.running = false;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnExitGame>(gSaveContext.fileNum);
    return 0;
}

const static std::map<std::string, uint16_t> ammoItems{ 
    { "sticks", ITEM_STICK }, { "nuts", ITEM_NUT },
    { "bombs", ITEM_BOMB },   { "seeds", ITEM_SLINGSHOT },
    { "arrows", ITEM_BOW },   { "bombchus", ITEM_BOMBCHU },
    { "beans", ITEM_BEAN }
};

static bool AddAmmoHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 3) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    int amount;

    try {
        amount = std::stoi(args[2]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("Ammo count must be an integer");
        return 1;
    }

    if (amount < 0) {
        ERROR_MESSAGE("Ammo count must be positive");
        return 1;
    }

    const auto& it = ammoItems.find(args[1]);
    if (it == ammoItems.end()) {
        ERROR_MESSAGE("Invalid ammo type. Options are 'sticks', 'nuts', 'bombs', 'seeds', 'arrows', 'bombchus' and 'beans'");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::AddOrTakeAmmo();
    effect->parameters[0] = amount;
    effect->parameters[1] = it->second;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Added ammo.");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not add ammo.");
        return 1;
    }
}

static bool TakeAmmoHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 3) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    int amount;

    try {
        amount = std::stoi(args[2]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("Ammo count must be an integer");
        return 1;
    }

    if (amount < 0) {
        ERROR_MESSAGE("Ammo count must be positive");
        return 1;
    }

    const auto& it = ammoItems.find(args[1]);
    if (it == ammoItems.end()) {
        ERROR_MESSAGE(
            "Invalid ammo type. Options are 'sticks', 'nuts', 'bombs', 'seeds', 'arrows', 'bombchus' and 'beans'");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::AddOrTakeAmmo();
    effect->parameters[0] = -amount;
    effect->parameters[1] = it->second;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Took ammo.");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not take ammo.");
        return 1;
    }
}

const static std::map<std::string, uint16_t> bottleItems{
    { "green_potion", ITEM_POTION_GREEN }, { "red_potion", ITEM_POTION_RED }, { "blue_potion", ITEM_POTION_BLUE },
    { "milk", ITEM_MILK },                 { "half_milk", ITEM_MILK_HALF },   { "fairy", ITEM_FAIRY },
    { "bugs", ITEM_BUG },                  { "fish", ITEM_FISH },             { "poe", ITEM_POE },
    { "big_poe", ITEM_BIG_POE },           { "blue_fire", ITEM_BLUE_FIRE },   { "rutos_letter", ITEM_LETTER_RUTO },
};

static bool BottleHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 3) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    unsigned int slot;
    try {
        slot = std::stoi(args[2]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Bottle slot must be an integer.");
        return 1;
    }

    if ((slot < 1) || (slot > 4)) {
        ERROR_MESSAGE("Invalid slot passed");
        return 1;
    }

    const auto& it = bottleItems.find(args[1]);

    if (it ==  bottleItems.end()) {
        ERROR_MESSAGE("Invalid item passed");
        return 1;
    }

    // I dont think you can do OOB with just this
    gSaveContext.inventory.items[0x11 + slot] = it->second;

    return 0;
}

static bool BHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    gSaveContext.equips.buttonItems[0] = std::stoi(args[1]);
    return 0;
}

static bool ItemHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 3) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    gSaveContext.inventory.items[std::stoi(args[1])] = std::stoi(args[2]);

    return 0;
}

static bool GiveItemHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string> args, std::string* output) {
    if (args.size() < 3) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    GetItemEntry getItemEntry = GET_ITEM_NONE;

    if (args[1].compare("vanilla") == 0) {
        getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, std::stoi(args[2]));
    } else if (args[1].compare("randomizer") == 0) {
        getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_RANDOMIZER, std::stoi(args[2]));
    } else {
        ERROR_MESSAGE("[SOH] Invalid argument passed, must be 'vanilla' or 'randomizer'");
        return 1;
    }

    GiveItemEntryWithoutActor(gPlayState, getItemEntry);

    return 0;
}

static bool EntranceHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    unsigned int entrance;

    try {
        entrance = std::stoi(args[1], nullptr, 16);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Entrance value must be a Hex number.");
        return 1;
    }

    gPlayState->nextEntranceIndex = entrance;
    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_INSTANT;
    gSaveContext.nextTransitionType = TRANS_TYPE_INSTANT;
}

static bool VoidHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (gPlayState != nullptr) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags = gPlayState->actorCtx.flags.tempSwch;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags = gPlayState->actorCtx.flags.tempCollect;
            gSaveContext.respawnFlag = 1;
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gPlayState->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex;
            gPlayState->transitionType = TRANS_TYPE_FADE_BLACK;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
    } else {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }
    return 0;
}

static bool ReloadHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (gPlayState != nullptr) {
        gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;
        gPlayState->transitionTrigger = TRANS_TRIGGER_START;
        gPlayState->transitionType = TRANS_TYPE_INSTANT;
        gSaveContext.nextTransitionType = TRANS_TYPE_INSTANT;
    } else {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }
    return 0;
}

const static std::map<std::string, uint16_t> fw_options {
    { "clear", 0}, {"warp", 1}, {"backup", 2}
};

static bool FWHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    const auto& it = fw_options.find(args[1]);
    if (it == fw_options.end()) {
        ERROR_MESSAGE("[SOH] Invalid option. Options are 'clear', 'warp', 'backup'");
        return 1;
    }
    
    if (gPlayState != nullptr) {
        FaroresWindData clear = {};
        switch(it->second) {
            case 0: //clear
                gSaveContext.fw = clear;
                INFO_MESSAGE("[SOH] Farore's wind point cleared! Reload scene to take effect.");
                return 0;
                break;
            case 1: //warp
                if (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 0) {
                    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                    gPlayState->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex;
                    gPlayState->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
                } else {
                    ERROR_MESSAGE("Farore's wind not set!");
                    return 1;
                }
                return 0;
                break;
            case 2: //backup
                if (CVarGetInteger("gBetterFW", 0)) {
                    gSaveContext.fw = gSaveContext.backupFW;
                    gSaveContext.fw.set = 1;
                    INFO_MESSAGE("[SOH] Backup FW data copied! Reload scene to take effect.");
                    return 0;
                } else {
                    ERROR_MESSAGE("Better Farore's Wind isn't turned on!");
                    return 1;
                }
                break;
        }
    }
    else {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }
    
    return 0;
}

static bool FileSelectHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (gPlayState != nullptr) {
        SET_NEXT_GAMESTATE(&gPlayState->state, FileChoose_Init, FileChooseContext);
        gPlayState->state.running = 0;
    } else {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }
    return 0;
}

static bool QuitHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    LUS::Context::GetInstance()->GetWindow()->Close();
    return 0;
}

static bool SaveStateHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
    const SaveStateReturn rtn = OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::SAVE });

    switch (rtn) {
        case SaveStateReturn::SUCCESS:
            INFO_MESSAGE("[SOH] Saved state to slot %u", slot);
            return 0;
        case SaveStateReturn::FAIL_WRONG_GAMESTATE:
            ERROR_MESSAGE("[SOH] Can not save a state outside of \"GamePlay\"");
            return 1;
    }
}

static bool LoadStateHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
    const SaveStateReturn rtn = OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::LOAD });

    switch (rtn) {
        case SaveStateReturn::SUCCESS:
            INFO_MESSAGE("[SOH] Loaded state from slot (%u)", slot);
            return 0;
        case SaveStateReturn::FAIL_INVALID_SLOT:
            ERROR_MESSAGE("[SOH] Invalid State Slot Number (%u)", slot);
            return 1;
        case SaveStateReturn::FAIL_STATE_EMPTY:
            ERROR_MESSAGE("[SOH] State Slot (%u) is empty", slot);
            return 1;
        case SaveStateReturn::FAIL_WRONG_GAMESTATE:
            ERROR_MESSAGE("[SOH] Can not load a state outside of \"GamePlay\"");
            return 1;
    }

}

static bool StateSlotSelectHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t slot;

    try {
        slot = std::stoi(args[1], nullptr, 10);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] SaveState slot value must be a number.");
        return 1;
    }

    if (slot < 0) {
        ERROR_MESSAGE("[SOH] Invalid slot passed. Slot must be between 0 and 2");
        return 1;
    }

    OTRGlobals::Instance->gSaveStateMgr->SetCurrentSlot(slot);
    INFO_MESSAGE("[SOH] Slot %u selected",
                                        OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot());
    return 0;
}

static bool InvisibleHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Invisible value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::InvisibleLink();
    GameInteractionEffectQueryResult result = 
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Invisible Link %s", state ? "enabled" : "disabled");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not %s Invisible Link.",
                                                state ? "enable" : "disable");
        return 1;
    }
}

static bool GiantLinkHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Giant value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyLinkSize();
    effect->parameters[0] = GI_LINK_SIZE_GIANT;
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Giant Link %s", state ? "enabled" : "disabled");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not %s Giant Link.",
                                                state ? "enable" : "disable");
        return 1;
    }
}

static bool MinishLinkHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Minish value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyLinkSize();
    effect->parameters[0] = GI_LINK_SIZE_MINISH;
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Minish Link %s", state ? "enabled" : "disabled");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not %s Minish Link.",
                                                state ? "enable" : "disable");
        return 1;
    }
}

static bool AddHeartContainerHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    int hearts;

    try {
        hearts = std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Hearts value must be an integer.");
        return 1;
    }

    if (hearts < 0) {
        ERROR_MESSAGE("[SOH] Hearts value must be a positive integer");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyHeartContainers();
    effect->parameters[0] = hearts;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Added %d heart containers", hearts);
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not add heart containers.");
        return 1;
    }
}

static bool RemoveHeartContainerHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    int hearts;

    try {
        hearts = std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Hearts value must be an integer.");
        return 1;
    }

    if (hearts < 0) {
        ERROR_MESSAGE("[SOH] Hearts value must be a positive integer");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyHeartContainers();
    effect->parameters[0] = -hearts;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Removed %d heart containers", hearts);
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not remove heart containers.");
        return 1;
    }
}

static bool GravityHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyGravity();

    try {
        effect->parameters[0] = LUS::Math::clamp(std::stoi(args[1], nullptr, 10), GI_GRAVITY_LEVEL_LIGHT, GI_GRAVITY_LEVEL_HEAVY);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Gravity value must be a number.");
        return 1;
    }
    
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Updated gravity.");
        return 0;
    } else {
        ERROR_MESSAGE("[SOH] Command failed: Could not update gravity.");
        return 1;
    }
}

static bool NoUIHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] No UI value must be a number.");
        return 1;
    }
    
    GameInteractionEffectBase* effect = new GameInteractionEffect::NoUI();
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] No UI %s", state ? "enabled" : "disabled");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not %s No UI.",
                                                state ? "enable" : "disable");
        return 1;
    }
}

static bool FreezeHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    GameInteractionEffectBase* effect = new GameInteractionEffect::FreezePlayer();
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Player frozen");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not freeze player.");
        return 1;
    }
}

static bool DefenseModifierHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyDefenseModifier();

    try {
        effect->parameters[0] = std::stoi(args[1], nullptr, 10);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Defense modifier value must be a number.");
        return 1;
    }

    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Defense modifier set to %d", effect->parameters[0]);
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not set defense modifier.");
        return 1;
    }
}

static bool DamageHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyHealth();

    try {
        int value = std::stoi(args[1], nullptr, 10);
        if (value < 0) {
            ERROR_MESSAGE("[SOH] Invalid value passed. Value must be greater than 0");
            return 1;
        }

        effect->parameters[0] = -value;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Damage value must be a number.");
        return 1;
    }

    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Player damaged");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not damage player.");
        return 1;
    }
}

static bool HealHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyHealth();

    try {
        int value = std::stoi(args[1], nullptr, 10);
        if (value < 0) {
            ERROR_MESSAGE("[SOH] Invalid value passed. Value must be greater than 0");
            return 1;
        }

        effect->parameters[0] = value;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Damage value must be a number.");
        return 1;
    }

    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Player healed");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not heal player.");
        return 1;
    }
}

static bool FillMagicHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    GameInteractionEffectBase* effect = new GameInteractionEffect::FillMagic();
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Magic filled");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not fill magic.");
        return 1;
    }
}

static bool EmptyMagicHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    GameInteractionEffectBase* effect = new GameInteractionEffect::EmptyMagic();
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Magic emptied");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not empty magic.");
        return 1;
    }
}

static bool NoZHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
     if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
     uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] NoZ value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::DisableZTargeting();
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] NoZ " + std::string(state ? "enabled" : "disabled"));
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not " +
                                                std::string(state ? "enable" : "disable") + " NoZ.");
        return 1;
    }
}

static bool OneHitKOHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] One-hit KO value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::OneHitKO();
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] One-hit KO " + std::string(state ? "enabled" : "disabled"));
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not " +
                                                std::string(state ? "enable" : "disable") + " One-hit KO.");
        return 1;
    }
}

static bool PacifistHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Pacifist value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::PacifistMode();
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Pacifist " + std::string(state ? "enabled" : "disabled"));
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not " +
                                                std::string(state ? "enable" : "disable") + " Pacifist.");
        return 1;
    }
}

static bool PaperLinkHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Paper Link value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyLinkSize();
    effect->parameters[0] = GI_LINK_SIZE_PAPER;
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Paper Link " + std::string(state ? "enabled" : "disabled"));
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not " +
                                                std::string(state ? "enable" : "disable") + " Paper Link.");
        return 1;
    }
}

static bool RainstormHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Rainstorm value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::WeatherRainstorm();
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Rainstorm " + std::string(state ? "enabled" : "disabled"));
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not " +
                                                std::string(state ? "enable" : "disable") + " Rainstorm.");
        return 1;
    }
}

static bool ReverseControlsHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    uint8_t state;

    try {
        state = std::stoi(args[1], nullptr, 10) == 0 ? 0 : 1;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Reverse controls value must be a number.");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ReverseControls();
    GameInteractionEffectQueryResult result =
        state ? GameInteractor::ApplyEffect(effect) : GameInteractor::RemoveEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Reverse controls " +
                                                std::string(state ? "enabled" : "disabled"));
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not " +
                                                std::string(state ? "enable" : "disable") + " Reverse controls.");
        return 1;
    }
}

static bool UpdateRupeesHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyRupees();

    try {
        effect->parameters[0] = std::stoi(args[1], nullptr, 10);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Rupee value must be a number.");
        return 1;
    }

    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Rupees updated");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not update rupees.");
        return 1;
    }
}

static bool SpeedModifierHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyRunSpeedModifier();

    try {
        effect->parameters[0] = std::stoi(args[1], nullptr, 10);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Speed modifier value must be a number.");
        return 1;
    }

    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Speed modifier updated");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not update speed modifier.");
        return 1;
    }
}

const static std::map<std::string, uint16_t> boots {
    { "kokiri", EQUIP_VALUE_BOOTS_KOKIRI },
    { "iron", EQUIP_VALUE_BOOTS_IRON },
    { "hover", EQUIP_VALUE_BOOTS_HOVER },
};

static bool BootsHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    const auto& it = boots.find(args[1]);
    if (it == boots.end()) {
        ERROR_MESSAGE("Invalid boot type. Options are 'kokiri', 'iron' and 'hover'");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::ForceEquipBoots();
    effect->parameters[0] = it->second;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Boots updated.");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not update boots.");
        return 1;
    }
}

const static std::map<std::string, uint16_t> shields {
    { "deku", ITEM_SHIELD_DEKU },
    { "hylian", ITEM_SHIELD_HYLIAN },
    { "mirror", ITEM_SHIELD_MIRROR },
};

static bool GiveShieldHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    const auto& it = shields.find(args[1]);
    if (it == shields.end()) {
        ERROR_MESSAGE("Invalid shield type. Options are 'deku', 'hylian' and 'mirror'");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::GiveOrTakeShield();
    effect->parameters[0] = it->second;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Gave shield.");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not give shield.");
        return 1;
    }
}

static bool TakeShieldHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    const auto& it = shields.find(args[1]);
    if (it == shields.end()) {
        ERROR_MESSAGE("Invalid shield type. Options are 'deku', 'hylian' and 'mirror'");
        return 1;
    }

    GameInteractionEffectBase* effect = new GameInteractionEffect::GiveOrTakeShield();
    effect->parameters[0] = it->second * -1;
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Took shield.");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not take shield.");
        return 1;
    }
}

static bool KnockbackHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }
    GameInteractionEffectBase* effect = new GameInteractionEffect::KnockbackPlayer();

    try {
        int value = std::stoi(args[1], nullptr, 10);
        if (value < 0) {
            ERROR_MESSAGE("[SOH] Invalid value passed. Value must be greater than 0");
            return 1;
        }

        effect->parameters[0] = value;
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] Knockback value must be a number.");
        return 1;
    }

    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);
    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Knockback applied");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not apply knockback.");
        return 1;
    }
}

static bool ElectrocuteHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    GameInteractionEffectBase* effect = new GameInteractionEffect::ElectrocutePlayer();
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Electrocuted player");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not electrocute player.");
        return 1;
    }
}

static bool BurnHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    GameInteractionEffectBase* effect = new GameInteractionEffect::BurnPlayer();
    GameInteractionEffectQueryResult result = GameInteractor::ApplyEffect(effect);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Burned player");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not burn player.");
        return 1;
    }
}

static bool CuccoStormHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    GameInteractionEffectQueryResult result = GameInteractor::RawAction::SpawnActor(ACTOR_EN_NIW, 0);

    if (result == GameInteractionEffectQueryResult::Possible) {
        INFO_MESSAGE("[SOH] Spawned cucco storm");
        return 0;
    } else {
        INFO_MESSAGE("[SOH] Command failed: Could not spawn cucco storm.");
        return 1;
    }
}

static bool GenerateRandoHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() == 1) {
        if (GenerateRandomizer()) {
            return 0;
        }
    }

    try {
        uint32_t value = std::stoi(args[1], NULL, 10);
        std::string seed = "";
        if (args.size() == 3) {
            int testing = std::stoi(args[1], nullptr, 10);
            seed = "seed_testing_count";
        }

        if (GenerateRandomizer(seed + std::to_string(value))){
            return 0;
        }
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[SOH] seed|count value must be a number.");
        return 1;
    }


    ERROR_MESSAGE("[SOH] Rando generation already in progress");
    return 1;
}

static bool CosmeticsHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    if (args[1].compare("reset") == 0) {
        CosmeticsEditor_ResetAll();
    } else if (args[1].compare("randomize") == 0) {
        CosmeticsEditor_RandomizeAll();
    } else {
        ERROR_MESSAGE("[SOH] Invalid argument passed, must be 'reset' or 'randomize'");
        return 1;
    }

    return 0;
}

static bool SfxHandler(std::shared_ptr<LUS::Console> Console, const std::vector<std::string>& args, std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[SOH] Unexpected arguments passed");
        return 1;
    }

    if (args[1].compare("reset") == 0) {
        AudioEditor_ResetAll();
    } else if (args[1].compare("randomize") == 0) {
        AudioEditor_RandomizeAll();
    } else {
        ERROR_MESSAGE("[SOH] Invalid argument passed, must be 'reset' or 'randomize'");
        return 1;
    }

    return 0;
}

void DebugConsole_Init(void) {
    // Console
    CMD_REGISTER("file_select", {FileSelectHandler, "Returns to the file select."});
    CMD_REGISTER("reset", {ResetHandler, "Resets the game."});
    CMD_REGISTER("quit", {QuitHandler, "Quits the game."});

    // Save States
    CMD_REGISTER("save_state", {SaveStateHandler, "Save a state."});
    CMD_REGISTER("load_state", {LoadStateHandler, "Load a state."});
    CMD_REGISTER("set_slot", {StateSlotSelectHandler, "Selects a SaveState slot", {
            {"Slot number", LUS::ArgumentType::NUMBER,}
    }});

    // Map & Location
    CMD_REGISTER("void", {VoidHandler, "Voids out of the current map."});
    CMD_REGISTER("reload", {ReloadHandler, "Reloads the current map."});
    CMD_REGISTER("fw", {FWHandler, "Spawns the player where Farore's Wind is set.", {
            {"clear|warp|backup", LUS::ArgumentType::TEXT}
    }});
    CMD_REGISTER("entrance", {EntranceHandler, "Sends player to the entered entrance (hex)", {
            {"entrance", LUS::ArgumentType::NUMBER}
    }});

    // Gameplay
    CMD_REGISTER("kill", {KillPlayerHandler, "Commit suicide."});

    CMD_REGISTER("map", {LoadSceneHandler, "Load up kak?"});

    CMD_REGISTER("rupee", {RupeeHandler, "Set your rupee counter.", {
            {"amount", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("bItem", {BHandler, "Set an item to the B button.", {
            {"Item ID", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("spawn", { ActorSpawnHandler, "Spawn an actor.", { { "actor name/id", LUS::ArgumentType::NUMBER }, // TODO there should be an actor_id arg type
                                                                  {"data", LUS::ArgumentType::NUMBER},
                                                                  {"x", LUS::ArgumentType::NUMBER, true},
                                                                  {"y", LUS::ArgumentType::NUMBER, true},
                                                                  {"z", LUS::ArgumentType::NUMBER, true},
                                                                  {"rx", LUS::ArgumentType::NUMBER, true},
                                                                  {"ry", LUS::ArgumentType::NUMBER, true},
                                                                  {"rz", LUS::ArgumentType::NUMBER, true}
    }});

    CMD_REGISTER("pos", {SetPosHandler, "Sets the position of the player.", {
            {"x", LUS::ArgumentType::NUMBER, true},
            {"y", LUS::ArgumentType::NUMBER, true},
            {"z", LUS::ArgumentType::NUMBER, true}
    }});

    CMD_REGISTER("addammo", {AddAmmoHandler, "Adds ammo of an item.", {
            {"sticks|nuts|bombs|seeds|arrows|bombchus|beans", LUS::ArgumentType::TEXT},
            {"count", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("takeammo", {TakeAmmoHandler, "Removes ammo of an item.", {
            {"sticks|nuts|bombs|seeds|arrows|bombchus|beans", LUS::ArgumentType::TEXT},
            {"count", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("bottle", {BottleHandler, "Changes item in a bottle slot.", {
            {"item", LUS::ArgumentType::TEXT},
            {"slot", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("give_item", {GiveItemHandler, "Gives an item to the player as if it was given from an actor", {
            {"vanilla|randomizer", LUS::ArgumentType::TEXT},
            {"giveItemID", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("item", {ItemHandler, "Sets item ID in arg 1 into slot arg 2. No boundary checks. Use with caution.", {
            {"slot", LUS::ArgumentType::NUMBER},
            {"item id", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("invisible", {InvisibleHandler, "Activate Link's Elvish cloak, making him appear invisible.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("giant_link", {GiantLinkHandler, "Turn Link into a giant Lonky boi.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("minish_link", {MinishLinkHandler, "Turn Link into a minish boi.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("add_heart_container",
                 {AddHeartContainerHandler, "Give Link a heart! The maximum amount of hearts is 20!"});

    CMD_REGISTER("remove_heart_container",
                 {RemoveHeartContainerHandler, "Remove a heart from Link. The minimal amount of hearts is 3."});

    CMD_REGISTER("gravity", {GravityHandler, "Set gravity level.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("no_ui", {NoUIHandler, "Disables the UI.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("freeze", {FreezeHandler, "Freezes Link in place"});

    CMD_REGISTER("defense_modifier", {DefenseModifierHandler, "Sets the defense modifier.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("damage", {DamageHandler, "Deal damage to Link.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("heal", {HealHandler, "Heals Link.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("fill_magic", {FillMagicHandler, "Fills magic."});

    CMD_REGISTER("empty_magic", {EmptyMagicHandler, "Empties magic."});

    CMD_REGISTER("no_z", {NoZHandler, "Disables Z-button presses.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("ohko", {OneHitKOHandler,
                          "Activates one hit KO. Any damage kills Link and he cannot gain health in this mode.", {
                                  {"value", LUS::ArgumentType::NUMBER}
                          }});

    CMD_REGISTER("pacifist", {PacifistHandler, "Activates pacifist mode. Prevents Link from using his weapon.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("paper_link", {PaperLinkHandler, "Link but made out of paper.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("rainstorm", {RainstormHandler, "Activates rainstorm."});

    CMD_REGISTER("reverse_controls", {ReverseControlsHandler, "Reverses the controls.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("update_rupees", {UpdateRupeesHandler, "Adds rupees.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("speed_modifier", {SpeedModifierHandler, "Sets the speed modifier.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("boots", {BootsHandler, "Activates boots.", {
            {"kokiri|iron|hover", LUS::ArgumentType::TEXT},
    }});

    CMD_REGISTER("giveshield", {GiveShieldHandler, "Gives a shield and equips it when Link is the right age for it.", {
            {"deku|hylian|mirror", LUS::ArgumentType::TEXT},
    }});

    CMD_REGISTER("takeshield", {TakeShieldHandler, "Takes a shield and unequips it if Link is wearing it.", {
            {"deku|hylian|mirror", LUS::ArgumentType::TEXT},
    }});

    CMD_REGISTER("knockback", {KnockbackHandler, "Knocks Link back.", {
            {"value", LUS::ArgumentType::NUMBER}
    }});

    CMD_REGISTER("electrocute", {ElectrocuteHandler, "Electrocutes Link."});

    CMD_REGISTER("burn", {BurnHandler, "Burns Link."});

    CMD_REGISTER("cucco_storm", {CuccoStormHandler, "Cucco Storm"});

    CMD_REGISTER("gen_rando", {GenerateRandoHandler, "Generate a randomizer seed", {
            {"seed|count", LUS::ArgumentType::NUMBER, true},
            {"testing", LUS::ArgumentType::NUMBER, true},
    }});

    CMD_REGISTER("cosmetics", {CosmeticsHandler, "Change cosmetics.", {
            {"reset|randomize", LUS::ArgumentType::TEXT},
    }});

    CMD_REGISTER("sfx", {SfxHandler, "Change SFX.", {
            {"reset|randomize", LUS::ArgumentType::TEXT},
    }});

    CVarSave();
    CVarLoad();
}

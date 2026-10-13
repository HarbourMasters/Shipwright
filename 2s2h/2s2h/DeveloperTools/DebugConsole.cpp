#include "DebugConsole.h"

#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/window/Window.h>
#include <ship/window/gui/ConsoleWindow.h>
#include "2s2h/BenPort.h"
#include <vector>
#include <string>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"

#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/gamestates/ovl_title/z_title.h"
}

#define CMD_REGISTER Ship::Context::GetRawInstance()->GetConsole()->AddCommand
// TODO: Commands should be using the output passed in.
#define ERROR_MESSAGE                                                                    \
    std::reinterpret_pointer_cast<Ship::ConsoleWindow>(                                  \
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console")) \
        ->SendErrorMessage
#define INFO_MESSAGE                                                                     \
    std::reinterpret_pointer_cast<Ship::ConsoleWindow>(                                  \
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console")) \
        ->SendInfoMessage

static bool ActorSpawnHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args,
                              std::string* output) {
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
    s16 actorId = 0;
    try {
        actorId = std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("Invalid actor ID");
        return 1;
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
                    spawnPoint.rot.x, spawnPoint.rot.y, spawnPoint.rot.z, params) == NULL) {
        ERROR_MESSAGE("Failed to spawn actor. Actor_Spawn returned NULL");
        return 1;
    }
    return 0;
}

static bool LoadSceneHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>&,
                             std::string* output) {
    gSaveContext.respawnFlag = 0;
    gSaveContext.seqId = NA_BGM_DISABLED;
    gSaveContext.gameMode = 0;

    return 0;
}

static bool SetPosHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string> args,
                          std::string* output) {
    if (gPlayState == nullptr) {
        ERROR_MESSAGE("PlayState == nullptr");
        return 1;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (args.size() == 1) {
        INFO_MESSAGE("Player position is [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x, player->actor.world.pos.y,
                     player->actor.world.pos.z);
        return 0;
    }
    if (args.size() < 4)
        return 1;

    player->actor.world.pos.x = std::stof(args[1]);
    player->actor.world.pos.y = std::stof(args[2]);
    player->actor.world.pos.z = std::stof(args[3]);

    INFO_MESSAGE("Set player position to [ %.2f, %.2f, %.2f ]", player->actor.world.pos.x, player->actor.world.pos.y,
                 player->actor.world.pos.z);
    return 0;
}

static bool ResetHandler(std::shared_ptr<Ship::Console> Console, std::vector<std::string> args, std::string* output) {
    if (gGameState == nullptr) {
        ERROR_MESSAGE("gGameState == nullptr");
        return 1;
    }

    STOP_GAMESTATE(gGameState);
    SET_NEXT_GAMESTATE(gGameState, ConsoleLogo_Init, sizeof(ConsoleLogoState));
    // GI-TODO
    // GameInteractor::Instance->ExecuteHooks<GameInteractor::OnExitGame>(gSaveContext.fileNum);
    return 0;
}

static bool BHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args,
                     std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[2S2H] Unexpected arguments passed");
        return 1;
    }

    u8 itemId;
    try {
        itemId = std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[2S2H] Item ID must be an integer.");
        return 1;
    }

    if (gPlayState == nullptr) {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }

    BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) = itemId;
    Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
    return 0;
}

static bool GiveItemHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string> args,
                            std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[2S2H] Unexpected arguments passed");
        return 1;
    }

    GetItemId getItemId;
    try {
        getItemId = (GetItemId)std::stoi(args[1]);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[2S2H] Item ID must be an integer.");
        return 1;
    }

    if (gPlayState == nullptr) {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }

    Player* player = GET_PLAYER(gPlayState);

    // Cheat by using Tatl to give the item
    if (player == nullptr || player->tatlActor == nullptr) {
        ERROR_MESSAGE("Unable to receive item");
        return 1;
    }

    if (!Actor_OfferGetItemFar(player->tatlActor, gPlayState, getItemId)) {
        ERROR_MESSAGE("Unable to receive item");
        return 1;
    }
    return 0;
}

static bool EntranceHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args,
                            std::string* output) {
    if (args.size() < 2) {
        ERROR_MESSAGE("[2S2H] Unexpected arguments passed");
        return 1;
    }

    unsigned int entrance;

    try {
        entrance = std::stoi(args[1], nullptr, 16);
    } catch (std::invalid_argument const& ex) {
        ERROR_MESSAGE("[2S2H] Entrance value must be a Hex number.");
        return 1;
    }

    gPlayState->nextEntrance = entrance;
    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_INSTANT;
    gSaveContext.nextTransitionType = TRANS_TYPE_INSTANT;
    return 0;
}

static bool VoidHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args,
                        std::string* output) {
    if (gPlayState != nullptr) {
        func_80169EFC(gPlayState);
    } else {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }
    return 0;
}

static bool ReloadHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args,
                          std::string* output) {
    if (gPlayState != nullptr) {
        gPlayState->nextEntrance = gSaveContext.save.entrance;
        gPlayState->transitionTrigger = TRANS_TRIGGER_START;
        gPlayState->transitionType = TRANS_TYPE_INSTANT;
        gSaveContext.nextTransitionType = TRANS_TYPE_INSTANT;
    } else {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }
    return 0;
}

static bool FileSelectHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args,
                              std::string* output) {
    if (gPlayState != nullptr) {
        STOP_GAMESTATE(&gPlayState->state);
        SET_NEXT_GAMESTATE(&gPlayState->state, FileSelect_Init, sizeof(FileSelectState));
    } else {
        ERROR_MESSAGE("gPlayState == nullptr");
        return 1;
    }
    return 0;
}

static bool QuitHandler(std::shared_ptr<Ship::Console> Console, const std::vector<std::string>& args,
                        std::string* output) {
    Ship::Context::GetRawInstance()->GetWindow()->Close();
    return 0;
}

void DebugConsole_Init(void) {
    // Console
    CMD_REGISTER("file_select", { FileSelectHandler, "Returns to the file select." });
    CMD_REGISTER("reset", { ResetHandler, "Resets the game." });
    CMD_REGISTER("quit", { QuitHandler, "Quits the game." });

    // Map & Location
    CMD_REGISTER("void", { VoidHandler, "Voids out of the current map." });
    CMD_REGISTER("reload", { ReloadHandler, "Reloads the current map." });
    CMD_REGISTER("entrance", { EntranceHandler,
                               "Sends player to the entered entrance (hex)",
                               { { "entrance", Ship::ArgumentType::NUMBER } } });

    // Gameplay
    CMD_REGISTER("give_item", { GiveItemHandler,
                                "Gives an item to the player as if it was given from an actor",
                                { { "giveItemID", Ship::ArgumentType::NUMBER } } });

    CMD_REGISTER("bItem", { BHandler, "Set an item to the B button.", { { "Item ID", Ship::ArgumentType::NUMBER } } });

    CMD_REGISTER("spawn", { ActorSpawnHandler,
                            "Spawn an actor.",
                            { { "actor id", Ship::ArgumentType::NUMBER },
                              { "data", Ship::ArgumentType::NUMBER },
                              { "x", Ship::ArgumentType::NUMBER, true },
                              { "y", Ship::ArgumentType::NUMBER, true },
                              { "z", Ship::ArgumentType::NUMBER, true },
                              { "rx", Ship::ArgumentType::NUMBER, true },
                              { "ry", Ship::ArgumentType::NUMBER, true },
                              { "rz", Ship::ArgumentType::NUMBER, true } } });

    CMD_REGISTER("pos", { SetPosHandler,
                          "Sets the position of the player.",
                          { { "x", Ship::ArgumentType::NUMBER, true },
                            { "y", Ship::ArgumentType::NUMBER, true },
                            { "z", Ship::ArgumentType::NUMBER, true } } });
}

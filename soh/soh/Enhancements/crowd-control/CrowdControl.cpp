#ifdef ENABLE_CROWD_CONTROL

#include "CrowdControl.h"
#include <libultraship/Cvar.h>
#include <libultraship/Console.h>
#include <libultraship/ImGuiImpl.h>
#include <nlohmann/json.hpp>
#include <regex>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;
}

#include "../debugconsole.h"

#define CMD_EXECUTE SohImGui::GetConsole()->Dispatch

void CrowdControl::InitCrowdControl() {
    SDLNet_Init();

    if (SDLNet_ResolveHost(&ip, "127.0.0.1", 43384) == -1) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    }

    ccThreadReceive = std::thread(&CrowdControl::ReceiveFromCrowdControl, this);
}

void CrowdControl::RunCrowdControl(CCPacket* packet) {
    bool paused = false;
    EffectResult lastResult = EffectResult::NotReady;
    bool isTimed = packet->timeRemaining > 0;

    while (connected) {
        EffectResult effectResult = ExecuteEffect(packet->effectType.c_str(), packet->effectValue, 0);
        if (effectResult == EffectResult::Success) {
            // If we have a success after being previously paused, we fire the Resumed event
            if (paused && packet->timeRemaining > 0) {
                paused = false;
                nlohmann::json dataSend;
                dataSend["id"] = packet->packetId;
                dataSend["type"] = 0;
                dataSend["timeRemaining"] = packet->timeRemaining;
                dataSend["status"] = EffectResult::Resumed;

                std::string jsonResponse = dataSend.dump();
                SDLNet_TCP_Send(tcpsock, jsonResponse.c_str(), jsonResponse.size() + 1);
            }

            // If time remaining has reached 0 or was 0, we have finished let's remove the command and end the thread
            if (packet->timeRemaining <= 0) {
                receivedCommandsMutex.lock();
                receivedCommands.erase(std::remove(receivedCommands.begin(), receivedCommands.end(), packet), receivedCommands.end());
                receivedCommandsMutex.unlock();
                RemoveEffect(packet->effectType.c_str());

                // If not timed, let's fire the one and only success
                if (!isTimed) {
                    nlohmann::json dataSend;
                    dataSend["id"] = packet->packetId;
                    dataSend["type"] = 0;
                    dataSend["timeRemaining"] = packet->timeRemaining;
                    dataSend["status"] = EffectResult::Success;

                    std::string jsonResponse = dataSend.dump();
                    SDLNet_TCP_Send(tcpsock, jsonResponse.c_str(), jsonResponse.size() + 1);

                    delete packet;
                }

                return;
            }

            // Decrement remaining time by the second that elapsed between thread runs
            packet->timeRemaining -= 1000;
            // We don't want to emit repeated events, so ensure we only emit changes in events
            if (effectResult != lastResult) {
                lastResult = effectResult;

                nlohmann::json dataSend;
                dataSend["id"] = packet->packetId;
                dataSend["type"] = 0;
                dataSend["timeRemaining"] = packet->timeRemaining;
                dataSend["status"] = effectResult;

                std::string jsonResponse = dataSend.dump();
                SDLNet_TCP_Send(tcpsock, jsonResponse.c_str(), jsonResponse.size() + 1);
            }
        } else if (effectResult == EffectResult::Retry && paused == false && packet->timeRemaining > 0) {
            paused = true;
            nlohmann::json dataSend;
            dataSend["id"] = packet->packetId;
            dataSend["type"] = 0;
            dataSend["timeRemaining"] = packet->timeRemaining;
            dataSend["status"] = EffectResult::Paused;

            std::string jsonResponse = dataSend.dump();
            SDLNet_TCP_Send(tcpsock, jsonResponse.c_str(), jsonResponse.size() + 1);
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void CrowdControl::ReceiveFromCrowdControl()
{
    printf("Waiting for a connection from Crowd Control...");

    while (!connected && CVar_GetS32("gCrowdControl", 0)) {
        tcpsock = SDLNet_TCP_Open(&ip);

        if (tcpsock) {
            connected = true;
            printf("Connected to Crowd Control!");
            break;
        }
    }

    while (connected && CVar_GetS32("gCrowdControl", 0) && tcpsock) {
        int len = SDLNet_TCP_Recv(tcpsock, &received, sizeof(received));

        if (!len || !tcpsock || len == -1) {
            printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
            break;
        }

        try {
            nlohmann::json dataReceived = nlohmann::json::parse(received);

            CCPacket* packet = new CCPacket();
            packet->packetId = dataReceived["id"];
            auto parameters = dataReceived["parameters"];
            if (parameters.size() > 0) {
                packet->effectValue = dataReceived["parameters"][0];
            }
            packet->effectType = dataReceived["code"].get<std::string>();

            if (packet->effectType == "high_gravity" ||
                packet->effectType == "low_gravity") {
                packet->effectCategory = "gravity";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "damage_multiplier"
                        || packet->effectType == "defense_multiplier"
            ) {
                packet->effectCategory = "defense";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "giant_link" ||
                packet->effectType == "minish_link" ||
                packet->effectType == "invisible" ||
                packet->effectType == "paper_link") {
                packet->effectCategory = "link_size";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "freeze" ||
                packet->effectType == "damage" ||
                packet->effectType == "heal" ||
                packet->effectType == "knockback" ||
                packet->effectType == "electrocute" ||
                packet->effectType == "burn" ||
                packet->effectType == "kill") {
                packet->effectCategory = "link_damage";
            }
            else if (packet->effectType == "hover_boots" ||
                packet->effectType == "iron_boots") {
                packet->effectCategory = "boots";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "add_heart_container" ||
                packet->effectType == "remove_heart_container") {
                packet->effectCategory = "heart_container";
            }
            else if (packet->effectType == "no_ui") {
                packet->effectCategory = "ui";
                packet->timeRemaining = 60000;
            }
            else if (packet->effectType == "fill_magic" ||
                packet->effectType == "empty_magic") {
                packet->effectCategory = "magic";
            }
            else if (packet->effectType == "ohko") {
                packet->effectCategory = "ohko";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "pacifist") {
                packet->effectCategory = "pacifist";
                packet->timeRemaining = 15000;
            }
            else if (packet->effectType == "rainstorm") {
                packet->effectCategory = "weather";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "reverse") {
                packet->effectCategory = "controls";
                packet->timeRemaining = 60000;
            }
            else if (packet->effectType == "add_rupees"
                        || packet->effectType == "remove_rupees"
            ) {
                packet->effectCategory = "rupees";
            }
            else if (packet->effectType == "increase_speed"
                        || packet->effectType == "decrease_speed"
            ) {
                packet->effectCategory = "speed";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "no_z") {
                packet->effectCategory = "no_z";
                packet->timeRemaining = 30000;
            }
            else if (packet->effectType == "spawn_wallmaster" ||
                packet->effectType == "spawn_arwing" ||
                packet->effectType == "spawn_darklink" ||
                packet->effectType == "spawn_stalfos" ||
                packet->effectType == "spawn_wolfos" ||
                packet->effectType == "spawn_freezard" ||
                packet->effectType == "spawn_keese" ||
                packet->effectType == "spawn_icekeese" ||
                packet->effectType == "spawn_firekeese" ||
                packet->effectType == "spawn_tektite" ||
                packet->effectType == "spawn_likelike" ||
                packet->effectType == "cucco_storm") {
                packet->effectCategory = "spawn";
            }
            else {
                packet->effectCategory = "none";
                packet->timeRemaining = 0;
            }

            // Check if running effect is possible
            EffectResult effectResult = ExecuteEffect(packet->effectType.c_str(), packet->effectValue, 1);
            if (effectResult == EffectResult::Retry || effectResult == EffectResult::Failure) {
                nlohmann::json dataSend;
                dataSend["id"] = packet->packetId;
                dataSend["type"] = 0;
                dataSend["timeRemaining"] = packet->timeRemaining;
                dataSend["status"] = effectResult;

                std::string jsonResponse = dataSend.dump();
                SDLNet_TCP_Send(tcpsock, jsonResponse.c_str(), jsonResponse.size() + 1);
            } else {
                bool anotherEffectOfCategoryActive = false;
                for (CCPacket* pack : receivedCommands) {
                    if (pack != packet && pack->effectCategory == packet->effectCategory && pack->packetId < packet->packetId) {
                        anotherEffectOfCategoryActive = true;

                        nlohmann::json dataSend;
                        dataSend["id"] = packet->packetId;
                        dataSend["type"] = 0;
                        dataSend["timeRemaining"] = packet->timeRemaining;
                        dataSend["status"] = EffectResult::Retry;

                        std::string jsonResponse = dataSend.dump();
                        SDLNet_TCP_Send(tcpsock, jsonResponse.c_str(), jsonResponse.size() + 1);

                        break;
                    }
                }

                if (anotherEffectOfCategoryActive != true) {
                    receivedCommandsMutex.lock();
                    receivedCommands.push_back(packet);
                    receivedCommandsMutex.unlock();
                    std::thread t = std::thread(&CrowdControl::RunCrowdControl, this, packet);
                    t.detach();
                }
            }
        } catch (nlohmann::json::parse_error& e) {
            printf("Error parsing JSON: %s\n", e.what());
            continue;
        }
    }

    if (connected) {
        SDLNet_TCP_Close(tcpsock);
        SDLNet_Quit();
        connected = false;
    }
}

CrowdControl::EffectResult CrowdControl::ExecuteEffect(std::string effectId, uint32_t value, bool dryRun) {
    // Don't execute effect and don't advance timer when the player is not in a proper loaded savefile
    // and when they're busy dying.
    if (gGlobalCtx == NULL || gGlobalCtx->gameOverCtx.state > 0 || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) {
        return EffectResult::Retry;
    }

    Player* player = GET_PLAYER(gGlobalCtx);

    if (player != NULL) {
        if (effectId == "add_heart_container") {
            if (gSaveContext.healthCapacity >= 0x140) {
                return EffectResult::Failure;
            }
            
            if (dryRun == 0) CMD_EXECUTE("add_heart_container");
            return EffectResult::Success;
        } else if (effectId == "remove_heart_container") {
            if ((gSaveContext.healthCapacity - 0x10) <= 0) {
                return EffectResult::Failure;
            }
            
            if (dryRun == 0) CMD_EXECUTE("remove_heart_container");
            return EffectResult::Success;
        } else if (effectId == "fill_magic") {
            if (!gSaveContext.magicAcquired) {
                return EffectResult::Failure;
            }

            if (gSaveContext.magic >= (gSaveContext.doubleMagic + 1) + 0x30) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE("fill_magic");
            return EffectResult::Success;
        } else if (effectId == "empty_magic") {
            if (!gSaveContext.magicAcquired || gSaveContext.magic <= 0) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE("empty_magic");
            return EffectResult::Success;
        } else if (effectId == "add_rupees") {
            if (dryRun == 0) CMD_EXECUTE(std::format("update_rupees {}", value));
            return EffectResult::Success;
        } else if (effectId == "remove_rupees") {
            if (gSaveContext.rupees - value < 0) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE(std::format("update_rupees -{}", value));
            return EffectResult::Success;
        }
    }

    if (player != NULL && !Player_InBlockingCsMode(gGlobalCtx, player) && gGlobalCtx->pauseCtx.state == 0
                                                                       && gGlobalCtx->msgCtx.msgMode == 0) {
        if (effectId == "high_gravity") {
            if (dryRun == 0) CMD_EXECUTE("gravity 2");
            return EffectResult::Success;
        } else if (effectId == "low_gravity") {
            if (dryRun == 0) CMD_EXECUTE("gravity 0");
            return EffectResult::Success;
        } else if (effectId == "kill"
                    || effectId == "freeze"
                    || effectId == "burn"
                    || effectId == "electrocute"
                    || effectId == "cucco_storm"
        ) {
            if (PlayerGrounded(player)) {
                if (dryRun == 0) CMD_EXECUTE(std::format("{}", effectId));
                return EffectResult::Success;
            }
            return EffectResult::Failure;
        } else if (effectId == "heal"
                    || effectId == "knockback"
        ) {
            if (dryRun == 0) CMD_EXECUTE(std::format("{} {}", effectId, value));
            return EffectResult::Success;
        } else if (effectId == "giant_link"
                    || effectId == "minish_link"
                    || effectId == "no_ui"
                    || effectId == "invisible"
                    || effectId == "paper_link"
                    || effectId == "no_z"
                    || effectId == "ohko"
                    || effectId == "pacifist"
                    || effectId == "rainstorm"
        ) {
            if (dryRun == 0) CMD_EXECUTE(std::format("{} 1", effectId));
            return EffectResult::Success;
        } else if (effectId == "reverse") {
            if (dryRun == 0) CMD_EXECUTE("reverse_controls 1"); 
            return EffectResult::Success;
        } else if (effectId == "iron_boots") {
            if (dryRun == 0) CMD_EXECUTE("boots iron");
            return EffectResult::Success;
        } else if (effectId == "hover_boots") {
            if (dryRun == 0) CMD_EXECUTE("boots hover");
            return EffectResult::Success;
        } else if (effectId == "give_dekushield") {
            if (dryRun == 0) CMD_EXECUTE("givedekushield");
            return EffectResult::Success;
        } else if (effectId == "spawn_wallmaster" 
            || effectId == "spawn_arwing" 
            || effectId == "spawn_darklink" 
            || effectId == "spawn_stalfos" 
            || effectId == "spawn_wolfos" 
            || effectId == "spawn_freezard" 
            || effectId == "spawn_keese" 
            || effectId == "spawn_icekeese"
            || effectId == "spawn_firekeese"
            || effectId == "spawn_tektite"
            || effectId == "spawn_likelike"
        ) {
            if (dryRun == 0) {
                if (CrowdControl::SpawnEnemy(effectId)) {
                    return EffectResult::Success;
                } else {
                    return EffectResult::Failure;
                }
            }
            return EffectResult::Success;
        } else if (effectId == "increase_speed") {
           if (dryRun == 0) CMD_EXECUTE("speed_modifier 2");
            return EffectResult::Success;
        } else if (effectId == "decrease_speed") {
           if (dryRun == 0) CMD_EXECUTE("speed_modifier -2");
            return EffectResult::Success;
        } else if (effectId == "damage_multiplier") {
            if (dryRun == 0) CMD_EXECUTE(std::format("defense_modifier -{}", value));
            return EffectResult::Success;
        } else if (effectId == "defense_multiplier") {
            if (dryRun == 0) CMD_EXECUTE(std::format("defense_modifier {}", value));
            return EffectResult::Success;
        } else if (effectId == "damage") {
            if ((gSaveContext.healthCapacity - 0x10) <= 0) {
                return EffectResult::Failure;
            }
            
            if (dryRun == 0) CMD_EXECUTE(std::format("{} {}", effectId, value));
            return EffectResult::Success;
        }
    }

    return EffectResult::Retry;
}

bool CrowdControl::SpawnEnemy(std::string effectId) {
    Player* player = GET_PLAYER(gGlobalCtx);

    int enemyId = 0;
    int enemyParams = 0;
    float posXOffset = 0;
    float posYOffset = 0;
    float posZOffset = 0;

    if (effectId == "spawn_wallmaster") {
        enemyId = 17;
    } else if (effectId == "spawn_arwing") {
        enemyId = 315;
        enemyParams = 1;
        posYOffset = 100;
    } else if (effectId == "spawn_darklink") {
        enemyId = 51;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_stalfos") {
        enemyId = 2;
        enemyParams = 2;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_wolfos") {
        enemyId = 431;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_freezard") {
        enemyId = 289;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_keese") {
        enemyId = 19;
        enemyParams = 2;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_icekeese") {
        enemyId = 19;
        enemyParams = 4;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_firekeese") {
        enemyId = 19;
        enemyParams = 1;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_tektite") {
        enemyId = 27;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == "spawn_likelike") {
        enemyId = 221;
        posXOffset = 75;
        posYOffset = 50;
    }

    return Actor_Spawn(&gGlobalCtx->actorCtx, gGlobalCtx, enemyId, player->actor.world.pos.x + posXOffset,
        player->actor.world.pos.y + posYOffset, player->actor.world.pos.z + posZOffset, 0, 0, 0, enemyParams);

}

void CrowdControl::RemoveEffect(std::string effectId) {
    if (gGlobalCtx == NULL) {
        return;
    }

    Player* player = GET_PLAYER(gGlobalCtx);

    if (player != NULL) {
        if (effectId == "giant_link"
                || effectId == "minish_link"
                || effectId == "no_ui"
                || effectId == "invisible"
                || effectId == "paper_link"
                || effectId == "no_z"
                || effectId == "ohko"
                || effectId == "pacifist"
                || effectId == "rainstorm"
        ) {
            CMD_EXECUTE(std::format("{} 0", effectId));
            return;
        } else if (effectId == "iron_boots" || effectId == "hover_boots") {
            CMD_EXECUTE("boots kokiri");
            return;
        } else if (effectId == "high_gravity" || effectId == "low_gravity") {
            CMD_EXECUTE("gravity 1");
            return;
        } else if (effectId == "reverse") {
            CMD_EXECUTE("reverse_controls 0");
            return;
        } else if (effectId == "increase_speed"
                    || effectId == "decrease_speed"
        ) {
            CMD_EXECUTE("speed_modifier 0");
            return;
        } else if (effectId == "damage_multiplier"
                    || effectId == "defense_multiplier"
        ) {
            CMD_EXECUTE("defense_modifier 0");
            return;
        }
    }
}
#endif

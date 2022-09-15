#ifdef ENABLE_CROWD_CONTROL

#include "CrowdControl.h"
#include <libultraship/Cvar.h>
#include <libultraship/Console.h>
#include <libultraship/ImGuiImpl.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
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
    uint8_t paused = 0;

    while (connected) {
        uint8_t anotherEffect = 0;

        nlohmann::json dataSend;

        dataSend["id"] = packet->packetId;
        dataSend["type"] = 0;

        dataSend["timeRemaining"] = packet->timeRemaining;

        for (CCPacket* pack : receivedCommands) {
            if (pack != packet && pack->effectCategory == packet->effectCategory && pack->packetId < packet->packetId) {
                anotherEffect = 1;
                dataSend["status"] = EffectResult::Retry;
                break;
            }
        }

        uint8_t returnSuccess = 0;
        if (anotherEffect == 0) {
            returnSuccess = ExecuteEffect(packet->effectType.c_str(), packet->effectValue);
            dataSend["status"] = returnSuccess == 1 ? EffectResult::Success : returnSuccess == 2 ? EffectResult::Failure : EffectResult::Retry;
        }

        std::string jsonResponse = dataSend.dump();
        SDLNet_TCP_Send(tcpsock, const_cast<char*> (jsonResponse.data()), jsonResponse.size() + 1);

        if (anotherEffect == 0) {
            if (returnSuccess == 2) {
                return;
            }

            if (returnSuccess == 1) {
                if (paused && packet->timeRemaining > 0) {
                    paused = 0;
                    nlohmann::json dataSend;
                    dataSend["id"] = packet->packetId;
                    dataSend["type"] = 0;
                    dataSend["timeRemaining"] = packet->timeRemaining;
                    dataSend["status"] = EffectResult::Resumed;

                    std::string jsonResponse = dataSend.dump();
                    SDLNet_TCP_Send(tcpsock, const_cast<char*> (jsonResponse.data()), jsonResponse.size() + 1);
                }

                if (packet->timeRemaining <= 0) {
                    receivedCommands.erase(std::remove(receivedCommands.begin(), receivedCommands.end(), packet), receivedCommands.end());
                    RemoveEffect(packet->effectType.c_str());
                    return;
                }

                packet->timeRemaining -= 1000;

                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else if (returnSuccess == 0 && paused == 0 && packet->timeRemaining > 0) {
                paused = 1;

                nlohmann::json dataSend;
                dataSend["id"] = packet->packetId;
                dataSend["type"] = 0;
                dataSend["timeRemaining"] = packet->timeRemaining;
                dataSend["status"] = EffectResult::Paused;

                std::string jsonResponse = dataSend.dump();
                SDLNet_TCP_Send(tcpsock, const_cast<char*> (jsonResponse.data()), jsonResponse.size() + 1);

                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            else {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void CrowdControl::ReceiveFromCrowdControl()
{
    printf("Waiting for a connection from Crowd Control...");

    while (!connected && CVar_GetS32("gCrowdControl", 0) == 1) {
        tcpsock = SDLNet_TCP_Open(&ip);

        if (tcpsock) {
            connected = true;
            printf("Connected to Crowd Control!");
            break;
        }
    }

    while (connected && CVar_GetS32("gCrowdControl", 0) == 1 && tcpsock) {
        int len = SDLNet_TCP_Recv(tcpsock, &received, 512);

        if (!len || !tcpsock || len == -1) {
            printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
            break;
        }

        nlohmann::json dataReceived = nlohmann::json::parse(received);

        CCPacket* packet = new CCPacket();
        packet->packetId = dataReceived["id"];
        packet->effectValue = dataReceived["type"];
        packet->effectType = dataReceived["code"].get<std::string>();

        if (strcmp(packet->effectType.c_str(), "high_gravity") == 0 ||
            strcmp(packet->effectType.c_str(), "low_gravity") == 0) {
            packet->effectCategory = "gravity";
            packet->timeRemaining = 30000;
        }
        else if (strcmp(packet->effectType.c_str(), "defense_modifier") == 0) {
            packet->effectCategory = "defense";
            packet->timeRemaining = 30000;
        }
        else if (strcmp(packet->effectType.c_str(), "giant_link") == 0 ||
            strcmp(packet->effectType.c_str(), "minish_link") == 0 ||
            strcmp(packet->effectType.c_str(), "invisible") == 0 ||
            strcmp(packet->effectType.c_str(), "paper_link") == 0) {
            packet->effectCategory = "link_size";
            packet->timeRemaining = 30000;
        }
        else if (strcmp(packet->effectType.c_str(), "freeze") == 0 ||
            strcmp(packet->effectType.c_str(), "damage") == 0 ||
            strcmp(packet->effectType.c_str(), "heal") == 0 ||
            strcmp(packet->effectType.c_str(), "knockback") == 0 ||
            strcmp(packet->effectType.c_str(), "electrocute") == 0 ||
            strcmp(packet->effectType.c_str(), "burn") == 0) {
            packet->effectCategory = "link_damage";
        }
        else if (strcmp(packet->effectType.c_str(), "hover_boots") == 0 ||
            strcmp(packet->effectType.c_str(), "iron_boots") == 0) {
            packet->effectCategory = "boots";
            packet->timeRemaining = 30000;
        }
        else if (strcmp(packet->effectType.c_str(), "add_heart_container") == 0 ||
            strcmp(packet->effectType.c_str(), "remove_heart_container") == 0) {
            packet->effectCategory = "heart_container";
        }
        else if (strcmp(packet->effectType.c_str(), "no_ui") == 0) {
            packet->effectCategory = "ui";
        }
        else if (strcmp(packet->effectType.c_str(), "fill_magic") == 0 ||
            strcmp(packet->effectType.c_str(), "empty_magic") == 0) {
            packet->effectCategory = "magic";
        }
        else if (strcmp(packet->effectType.c_str(), "ohko") == 0) {
            packet->effectCategory = "ohko";
        }
        else if (strcmp(packet->effectType.c_str(), "pacifist") == 0) {
            packet->effectCategory = "pacifist";
        }
        else if (strcmp(packet->effectType.c_str(), "rainstorm") == 0) {
            packet->effectCategory = "weather";
        }
        else if (strcmp(packet->effectType.c_str(), "reverse") == 0) {
            packet->effectCategory = "controls";
        }
        else if (strcmp(packet->effectType.c_str(), "rupees") == 0) {
            packet->effectCategory = "rupees";
        }
        else if (strcmp(packet->effectType.c_str(), "speed") == 0) {
            packet->effectCategory = "speed";
        }
        else if (strcmp(packet->effectType.c_str(), "wallmaster") == 0 ||
            strcmp(packet->effectType.c_str(), "cucco") == 0) {
            packet->effectCategory = "spawn";
        }
        else {
            packet->effectCategory = "none";
            packet->timeRemaining = 0;
        }

        receivedCommands.push_back(packet);
        std::thread t = std::thread(&CrowdControl::RunCrowdControl, this, packet);
        t.detach();
    }

    if (connected) {
        SDLNet_TCP_Close(tcpsock);
        SDLNet_Quit();
        connected = false;
    }
}

uint8_t CrowdControl::ExecuteEffect(const char* effectId, uint32_t value) {
    if (gGlobalCtx == NULL) {
        return 0;
    }

    Player* player = GET_PLAYER(gGlobalCtx);

    if (player != NULL) {
        if (strcmp(effectId, "add_heart_container") == 0) {
            if (gSaveContext.healthCapacity >= 0x140) {
                return 2;
            }
            
            CMD_EXECUTE("add_heart_container");
            return 1;
        } else if (strcmp(effectId, "remove_heart_container") == 0) {
            if ((gSaveContext.healthCapacity - 0x10) <= 0) {
                return 2;
            }
            
            CMD_EXECUTE("remove_heart_container");
            return 1;
        }
    }

    if (player != NULL && !Player_InBlockingCsMode(gGlobalCtx, player) && gGlobalCtx->pauseCtx.state == 0) {
        if (strcmp(effectId, "high_gravity") == 0) {
            CMD_EXECUTE("gravity 2");
            return 1;
        } else if (strcmp(effectId, "low_gravity") == 0) {
            CMD_EXECUTE("gravity 0");
            return 1;
        } else if (strcmp(effectId, "giant_link") == 0) {
            CMD_EXECUTE("giant_link 1");
            return 1;
        } else if (strcmp(effectId, "minish_link") == 0) {
            CMD_EXECUTE("minish_link 1");
            return 1;
        } else if (strcmp(effectId, "defense_modifier") == 0) {
            // TODO:
            // defenseModifier = value;
            return 1;
        } else if (strcmp(effectId, "kill") == 0) {
            if (PlayerGrounded(player)) {
                CMD_EXECUTE("kill");
                return 1;
            }
            return 0;
        } else if (strcmp(effectId, "cucco") == 0) {
            CMD_EXECUTE("cucco_storm");
            return 1;
        } else if (strcmp(effectId, "damage") == 0) {
            char buff[100];
            snprintf(buff, sizeof(buff), "damage %i", value);
            CMD_EXECUTE(std::string(buff));
            return 1;
        } else if (strcmp(effectId, "heal") == 0) {
            char buff[100];
            snprintf(buff, sizeof(buff), "heal %i", value);
            CMD_EXECUTE(std::string(buff));
            return 1;
        } else if (strcmp(effectId, "freeze") == 0) {
            if (PlayerGrounded(player)) {
                CMD_EXECUTE("freeze");
                return 1;
            }
            return 0;
        } else if (strcmp(effectId, "knockback") == 0) {
            char buff[100];
            snprintf(buff, sizeof(buff), "knockback %i", value);
            CMD_EXECUTE(std::string(buff));
            return 1;
        } else if (strcmp(effectId, "burn") == 0) {
            if (PlayerGrounded(player)) {
                CMD_EXECUTE("burn");
                return 1;
            }
            return 0;
        } else if (strcmp(effectId, "electrocute") == 0) {
            if (PlayerGrounded(player)) {
                CMD_EXECUTE("electrocute");
                return 1;
            }
            return 0;
        } else if (strcmp(effectId, "iron_boots") == 0) {
            CMD_EXECUTE("boots iron");
            return 1;
        } else if (strcmp(effectId, "hover_boots") == 0) {
            CMD_EXECUTE("boots hover");
            return 1;
        } else if (strcmp(effectId, "wallmaster") == 0) {
            // TODO: 
            // Actor_Spawn(&gGlobalCtx->actorCtx, gGlobalCtx, ACTOR_EN_WALLMAS, player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0, 0, 0, 0);
            return 1;
        } else if (strcmp(effectId, " ") == 0) {
            CMD_EXECUTE("no_ui 1");
            return 1;
        } else if (strcmp(effectId, "invisible") == 0) {
            CMD_EXECUTE("invisibles 1");
            return 1;
        }
    }

    return 0;
}

void CrowdControl::RemoveEffect(const char* effectId) {
    if (gGlobalCtx == NULL) {
        return;
    }

    Player* player = GET_PLAYER(gGlobalCtx);

    if (player != NULL) {
        if (strcmp(effectId, "giant_link") == 0) {
            CMD_EXECUTE("giant_link 0");
            return;
        } else if (strcmp(effectId, "minish_link") == 0) {
            CMD_EXECUTE("minish_link 0");
        } else if (strcmp(effectId, "defense_modifier") == 0) {
            // TODO: 
            // defenseModifier = 0;
            return;
        } else if (strcmp(effectId, "iron_boots") == 0 || strcmp(effectId, "hover_boots") == 0) {
            CMD_EXECUTE("boots kokiri");
            return;
        } else if (strcmp(effectId, "high_gravity") == 0 || strcmp(effectId, "low_gravity") == 0) {
            CMD_EXECUTE("gravity 1");
            return;
        } else if (strcmp(effectId, "no_ui") == 0) {
            CMD_EXECUTE("no_ui 0");
            return;
        } else if (strcmp(effectId, "invisible") == 0) {
            CMD_EXECUTE("invisible 0");
            return;
        }
    }
}
#endif

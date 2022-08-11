#ifdef ENABLE_CROWD_CONTROL

#include "CrowdControl.h"
#include "Cvar.h"
#include "Lib/json.hpp"
#include <spdlog/spdlog.h>
#include <regex>

#include "soh/Enhancements/debugconsole.h"

extern "C" u8 ExecuteEffect(const char* effectId, uint32_t value);
extern "C" void RemoveEffect(const char* effectId);

namespace Ship {
    namespace CrowdControl {
        void CrowdControl::InitCrowdControl() {
            SDLNet_Init();

            if (SDLNet_ResolveHost(&ip, "127.0.0.1", 43384) == -1) {
                printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
            }

            ccThreadReceive = std::thread(&CrowdControl::ReceiveFromCrowdControl, this);
        }

        void CrowdControl::RunCrowdControl(CCPacket* packet) {
            u8 paused = 0;

            while (connected) {
                u8 anotherEffect = 0;

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

                u8 returnSuccess = 0;
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
    }
}
#endif

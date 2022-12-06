#include <iostream>
#include <thread>
#include <SDL_net.h>
#include <spdlog/spdlog.h>
#include "Online.h"

std::thread initThread;
std::jthread receiveThread;

extern "C" void SetOnlinePlayerID(uint8_t player_id);
extern "C" void SetLinkPuppetData(PuppetPacket* packet, uint8_t player_id);
extern "C" void SetOnlineInventoryData(InventoryPacket* packet);

void OnlineClient::SendPacketMessage(OnlinePacket* packet, TCPsocket* sendTo) {
    if (*sendTo != nullptr) {
        SDLNet_TCP_Send(*sendTo, packet, sizeof(OnlinePacket));
    }
}

void OnlineClient::InitClient(char* ipAddr, int port) {
    SDLNet_Init();

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, ipAddr, port) == -1) {
        SPDLOG_INFO("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return;
    }

    initThread = std::thread(&OnlineClient::WaitForServerConnection, this, ip);
}

void OnlineClient::WaitForServerConnection(IPaddress ip) {
    while (!running) {
        client = SDLNet_TCP_Open(&ip);

        if (client != NULL) {
            running = true;
            break;
        }
    }

    receiveThread = std::jthread(&OnlineClient::RunClientReceive, this);
}

void OnlineClient::RunClientReceive() {
    OnlinePacket packet[MAX_PLAYERS];

    while (running) {
        int len = SDLNet_TCP_Recv(client, &packet, sizeof(OnlinePacket[MAX_PLAYERS]));

        if (len > 0) {
            for (size_t i = 0; i < MAX_PLAYERS; i++) {
                if (packet[i].puppetPacket.initialized == 1) {
                    if (packet[i].is_you == 1) {
                        SetOnlinePlayerID(packet[i].player_id);
                    }

                    SetLinkPuppetData(&packet[i].puppetPacket, packet[i].player_id);
                    SetOnlineInventoryData(&packet[i].inventoryPacket);
                }
            }
        }
    }
}

void OnlineClient::CloseClient() {
    SDLNet_TCP_Close(client);
    SDLNet_Quit();
}

OnlineClient::~OnlineClient() {
    running = false;
}

#include <iostream>
#include <thread>
#include <SDL_net.h>
#include <spdlog/spdlog.h>
#include "Online.h"

std::thread initThread;
std::jthread receiveThread;

extern "C" void SetLinkPuppetData(OnlinePacket* packet, uint8_t player_id);

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
    OnlinePacket packet;

    while (running) {
        int len = SDLNet_TCP_Recv(client, &packet, sizeof(OnlinePacket));

        if (len > 0) {
            SetLinkPuppetData(&packet, packet.player_id);
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

#include <iostream>
#include <thread>
#include <SDL_net.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include "Online.h"

TCPsocket server;

std::jthread clientThreads[32];
std::jthread connectionThread;

extern "C" void SetLinkPuppetData(OnlinePacket* packet, uint8_t player_id);

void OnlineServer::SendPacketMessage(OnlinePacket* packet, TCPsocket* sendTo) {
    if (*sendTo != nullptr) {
        SDLNet_TCP_Send(*sendTo, packet, sizeof(OnlinePacket));
    }
}

void OnlineServer::InitServer(int port) {
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, port);

    server = SDLNet_TCP_Open(&ip);

    running = true;
    connectionThread = std::jthread(&OnlineServer::WaitForClientConnections, this);
}

void OnlineServer::WaitForClientConnections() {
    while (running) {
        for (size_t i = 0; i < maxPlayers; i++) {
            if (clients[i] == NULL) {
                while (clients[i] == NULL) {
                    TCPsocket newClient = SDLNet_TCP_Accept(server);

                    if (newClient != NULL) {
                        clients[i] = newClient;
                        clientThreads[i] = std::jthread(&OnlineServer::RunServerReceive, this, clients[i]);
                        break;
                    }

                    Sleep(200);
                }
            }
        }
    }
}

void OnlineServer::RunServerReceive(TCPsocket receiveClient) {
    OnlinePacket packet;

    while (running) {
        if (receiveClient != NULL) {
            int len = SDLNet_TCP_Recv(receiveClient, &packet, sizeof(OnlinePacket));

            if (len > 0) {
                SetLinkPuppetData(&packet, packet.player_id);
            }
        }
    }
}

void OnlineServer::CloseServer() {
    for (size_t i = 0; i < maxPlayers; i++) {
        if (clients[i] != NULL) {
            SDLNet_TCP_Close(clients[i]);
        }
    }
    SDLNet_TCP_Close(server);
    SDLNet_Quit();
}

OnlineServer::~OnlineServer() {
    running = false;
}

#include <iostream>
#include <thread>
#include <SDL_net.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include "Online.h"

TCPsocket server;

OnlinePacket clientPackets[MAX_PLAYERS] = {};

std::jthread clientSendThreads[MAX_PLAYERS];
std::jthread clientReceiveThreads[MAX_PLAYERS];
std::jthread connectionThread;

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
        for (size_t i = 0; i < MAX_PLAYERS; i++) {
            if (clients[i] == NULL) {
                while (clients[i] == NULL) {
                    TCPsocket newClient = SDLNet_TCP_Accept(server);

                    if (newClient != NULL) {
                        clients[i] = newClient;
                        clientSendThreads[i] = std::jthread(&OnlineServer::RunServerSend, this, i);
                        clientReceiveThreads[i] = std::jthread(&OnlineServer::RunServerReceive, this, i);
                        break;
                    }
                }
            }
        }
    }
}

void OnlineServer::RunServerSend(int player_id) {
    while (clients[player_id] != NULL) {
        clientPackets[player_id].inventoryPacket.initialized = 1;
        clientPackets[player_id].puppetPacket.initialized = 1;
        clientPackets[player_id].player_id = player_id;

        clientPackets[player_id].is_you = 1;
        SDLNet_TCP_Send(clients[player_id], &clientPackets, sizeof(OnlinePacket[MAX_PLAYERS]));
        clientPackets[player_id].is_you = 0;
    }
}

void OnlineServer::RunServerReceive(int player_id) {
    while (clients[player_id] != NULL) {
        SDLNet_TCP_Recv(clients[player_id], &clientPackets[player_id], sizeof(OnlinePacket));
    }
}

void OnlineServer::CloseServer() {
    for (size_t i = 0; i < MAX_PLAYERS; i++) {
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

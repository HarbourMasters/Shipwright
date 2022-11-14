#include "Online.h"
#include <spdlog/spdlog.h>
#include <libultraship/Lib/Fast3D/U64/PR/ultra64/gbi.h>
#include <libultraship/ImGuiImpl.h>

#define MAX_CLIENTS 32

extern "C" void SetLinkPuppetData(Ship::Online::OnlinePacket * packet, u8 player_id);
extern "C" void Rupees_ChangeBy(s16 rupeeChange);
extern "C" u8 rupeesReceived;

namespace Ship {
namespace Online {

void InitOnline(char* ipAddr, int port)
{
    if (SDLNet_Init() == -1) {
        SPDLOG_INFO("SDLNet_Init: %s\n", SDLNet_GetError());
        return;
    }

    if (ipAddr == nullptr) {
        server.CreateServer(port);
    } else {
        client.CreateClient(ipAddr, port);
    }
}

void SendPacketMessage(OnlinePacket* packet, TCPsocket* sendTo) {
    if (*sendTo != nullptr) {
        SDLNet_TCP_Send(*sendTo, packet, sizeof(OnlinePacket));
    }
}

void Server::RunServer() {
    SPDLOG_INFO("Server Started!\n");
    SohImGui::GetGameOverlay()->TextDrawNotification(30.0f, true, "Server Started!");

    connectionsThread = std::thread(&Server::WaitForConnections, this);

    while (serverOpen) {
        for (size_t i = 0; i < MAX_PLAYERS; i++) {
            if (clientSockets[i] != nullptr) {
                int len = SDLNet_TCP_Recv(clientSockets[i], &serverPacket, sizeof(OnlinePacket));

                if (!len) {
                    SPDLOG_INFO("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
                } else if (len > 0) {
                    SetLinkPuppetData(&serverPacket, i);
                } else {
                    free(clientSockets[i]);
                    clientSockets[i] = nullptr;
                }
            }
        }
    }
}

void Server::WaitForConnections() {
    while (serverOpen) {
        /* try to accept a connection */
        for (size_t i = 0; i < MAX_PLAYERS; i++) {
            if (clientSockets[i] == nullptr) {
                clientSockets[i] = SDLNet_TCP_Accept(serverSocket);

                if (clientSockets[i] == nullptr) {
                    SDL_Delay(1000);
                    break;
                }

                /* get the clients IP and port number */
                IPaddress* remoteip;
                remoteip = SDLNet_TCP_GetPeerAddress(clientSockets[i]);
                if (!remoteip) {
                    SPDLOG_INFO("SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
                    return;
                }

                /* print out the clients IP and port number */
                Uint32 ipaddr;
                ipaddr = SDL_SwapBE32(remoteip->host);

                SPDLOG_INFO("Server connected to Client!\n");
                SohImGui::GetGameOverlay()->TextDrawNotification(30.0f, true, "Server connected to Client!");
            }
        }
    }
}

Server::~Server() {
    for (size_t i = 0; i < MAX_PLAYERS; i++) {
        SDLNet_TCP_Close(clientSockets[i]);
    }
    SDLNet_TCP_Close(serverSocket);
    onlineThread.join();
    connectionsThread.join();
}

void Server::CreateServer(int serverPort) {
    IPaddress ip;

    SPDLOG_INFO("Starting server...\n");

    if (SDLNet_ResolveHost(&ip, NULL, serverPort) == -1) {
        SPDLOG_INFO("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return;
    }

    serverSocket = SDLNet_TCP_Open(&ip);

    if (!serverSocket) {
        SPDLOG_INFO("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return;
    }

    serverOpen = true;

    onlineThread = std::thread(&Server::RunServer, this);
}

Client::~Client() {
    SDLNet_TCP_Close(clientSocket);
    onlineThread.join();
}

void Client::CreateClient(char* ipAddr, int port) {
    SPDLOG_INFO("Starting client...\n");
    IPaddress ip;

    if (SDLNet_ResolveHost(&ip, ipAddr, port) == -1) {
        SPDLOG_INFO("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return;
    }

    clientSocket = SDLNet_TCP_Open(&ip);
    if (!clientSocket) {
        SPDLOG_INFO("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return;
    }

    clientConnected = true;

    onlineThread = std::thread(&Client::RunClient, this);
}

void Client::RunClient() {
    SPDLOG_INFO("Client Connected to Server!\n");
    SohImGui::GetGameOverlay()->TextDrawNotification(30.0f, true, "Client Connected to Server!");

    while (clientConnected) {
        int len = SDLNet_TCP_Recv(clientSocket, &clientPacket, sizeof(OnlinePacket));
        if (!len) {
            SPDLOG_INFO("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
            SohImGui::GetGameOverlay()->TextDrawNotification(30.0f, true, "Client Error!");
        } else if (len > 0) {
            SetLinkPuppetData(&clientPacket, clientPacket.player_id);
        } else {
            clientConnected = false;
            SohImGui::GetGameOverlay()->TextDrawNotification(30.0f, true, "Client Disconnected!");
        }
    }
}

} // namespace Online
} // namespace Ship
#include <iostream>
#include <thread>
#include <SDL_net.h>
#include <spdlog/spdlog.h>
#include "Online.h"

std::thread initThread;
std::jthread receiveThread;

UDPsocket client;
UDPpacket* p;
IPaddress srvadd;

extern "C" void SetLinkPuppetData(PuppetPacket* packet, uint8_t player_id);
extern "C" void SetOnlineInventoryData(InventoryPacket* packet);

uint8_t player_id = -1;

void OnlineClient::SendPacketMessage(OnlinePacket* packet) {
    if (client != nullptr) {
        packet->player_id = player_id;
        memcpy(p->data, packet, sizeof(OnlinePacket));
        p->len = sizeof(OnlinePacket);
        SDLNet_UDP_Send(client, -1, p);
    }
}

void OnlineClient::InitClient(char* ipAddr, int port) {
    SDLNet_Init();

    p = SDLNet_AllocPacket(sizeof(OnlinePacket));

    SDLNet_ResolveHost(&srvadd, ipAddr, port);
    client = SDLNet_UDP_Open(0);

    p->address.host = srvadd.host;
    p->address.port = srvadd.port;

    running = true;
    receiveThread = std::jthread(&OnlineClient::RunClientReceive, this);
}

uint8_t binded = 0;

void OnlineClient::RunClientReceive() {

    while (running) {
        if (SDLNet_UDP_Recv(client, p)) {
            OnlinePacket onlinePacket;
            memcpy(&onlinePacket, p->data, sizeof(OnlinePacket));

            if (p->len == 1 && binded == 0) {
                memcpy(&player_id, p->data, 1);
                SDLNet_UDP_Bind(client, player_id, &srvadd);
                binded = 1;
            } else if (binded == 1 && onlinePacket.is_you == 0) {
                SetLinkPuppetData(&onlinePacket.puppetPacket, onlinePacket.player_id);
                SetOnlineInventoryData(&onlinePacket.inventoryPacket);
            }
        }
    }
}

void OnlineClient::CloseClient() {
    SDLNet_Quit();
}

OnlineClient::~OnlineClient() {
    running = false;
}

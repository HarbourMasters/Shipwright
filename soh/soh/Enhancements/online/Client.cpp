#include <iostream>
#include <thread>
#include <SDL_net.h>
#include <spdlog/spdlog.h>
#include "Online.h"

std::thread initThread;
std::jthread receiveThread;

UDPsocket client;
IPaddress srvadd;

extern "C" void SetLinkPuppetData(PuppetPacket* packet, uint8_t player_id);
extern "C" void SetGetItemData(int16_t itemId);
extern "C" void Online_DamagePlayer(uint8_t damageValue, uint8_t damageEffect, int16_t knockbackRotation);
extern "C" void Online_SetSceneFlag(uint8_t scene_num, uint8_t flag_type, int32_t flag_value);

uint8_t player_id = -1;

void OnlineClient::SendPuppetPacketMessage(PuppetPacket* packet) {
    if (client != nullptr) {
        UDPpacket* newPacket;

        newPacket = SDLNet_AllocPacket(sizeof(PuppetPacket));

        packet->player_id = player_id;
        packet->packet_type = 1;

        newPacket->address.host = srvadd.host;
        newPacket->address.port = srvadd.port;
        newPacket->len = sizeof(PuppetPacket);

        memcpy(newPacket->data, packet, sizeof(PuppetPacket));

        SDLNet_UDP_Send(client, -1, newPacket);

        SDLNet_FreePacket(newPacket);
    }
}

void OnlineClient::SendGetItemPacketMessage(int16_t itemId) {
    if (client != nullptr) {
        UDPpacket* newPacket;
        newPacket = SDLNet_AllocPacket(sizeof(GetItemPacket));

        GetItemPacket* getItemPacket = new GetItemPacket();

        getItemPacket->player_id = player_id;
        getItemPacket->packet_type = 2;
        getItemPacket->get_item = itemId;

        newPacket->address.host = srvadd.host;
        newPacket->address.port = srvadd.port;
        newPacket->len = sizeof(GetItemPacket);

        memcpy(newPacket->data, getItemPacket, sizeof(GetItemPacket));

        SDLNet_UDP_Send(client, -1, newPacket);

        SDLNet_FreePacket(newPacket);

        delete getItemPacket;
    }
}

void OnlineClient::SendDamagePacketMessage(DamagePacket* packet) {
    if (client != nullptr) {
        UDPpacket* newPacket;
        newPacket = SDLNet_AllocPacket(sizeof(DamagePacket));

        packet->packet_type = 3;

        newPacket->address.host = srvadd.host;
        newPacket->address.port = srvadd.port;
        newPacket->len = sizeof(DamagePacket);

        memcpy(newPacket->data, packet, sizeof(DamagePacket));

        SDLNet_UDP_Send(client, -1, newPacket);

        SDLNet_FreePacket(newPacket);
    }
}

void OnlineClient::SendSceneFlagPacketMessage(uint8_t scene_num, uint8_t flag_type, int32_t flag_value) {
    if (client != nullptr) {
        UDPpacket* newPacket;
        newPacket = SDLNet_AllocPacket(sizeof(SceneFlagPacket));

        SceneFlagPacket* sceneFlagPacket = new SceneFlagPacket();

        sceneFlagPacket->packet_type = 4;
        sceneFlagPacket->scene_id = scene_num;
        sceneFlagPacket->flag_type = flag_type;
        sceneFlagPacket->flag_value = flag_value;

        newPacket->address.host = srvadd.host;
        newPacket->address.port = srvadd.port;
        newPacket->len = sizeof(SceneFlagPacket);

        memcpy(newPacket->data, sceneFlagPacket, sizeof(SceneFlagPacket));

        SDLNet_UDP_Send(client, -1, newPacket);

        SDLNet_FreePacket(newPacket);
    }
}

void OnlineClient::CloseClient() {
    if (running) {
        running = false;
        SDLNet_UDP_Close(client);
        SDLNet_Quit();
    }
}

void OnlineClient::InitClient(char* ipAddr, int port) {
    CloseClient();

    SDLNet_Init();

    SDLNet_ResolveHost(&srvadd, ipAddr, port);
    client = SDLNet_UDP_Open(0);

    running = true;
    receiveThread = std::jthread(&OnlineClient::RunClientReceive, this);
}

void OnlineClient::RunClientReceive() {
    while (running) {
        UDPpacket* p;

        p = SDLNet_AllocPacket(1024);
        p->address.host = srvadd.host;
        p->address.port = srvadd.port;

        if (SDLNet_UDP_Recv(client, p)) {
            ConnectionPacket* connectionPacket = new ConnectionPacket();
            memcpy(connectionPacket, p->data, sizeof(ConnectionPacket));

            // Connection Packet
            if (connectionPacket->packet_type == 0) {
                SDLNet_UDP_Unbind(client, player_id);

                memcpy(&player_id, &connectionPacket->player_id, sizeof(connectionPacket->player_id));
                SDLNet_UDP_Bind(client, player_id, &srvadd);
            }

            // Puppet Packet
            if (connectionPacket->packet_type == 1) {
                PuppetPacket* puppetPacket = new PuppetPacket();
                memcpy(puppetPacket, p->data, sizeof(PuppetPacket));
                SetLinkPuppetData(puppetPacket, puppetPacket->player_id);
                delete puppetPacket;
            }

            // Get Item Packet
            if (connectionPacket->packet_type == 2) {
                GetItemPacket* getItemPacket = new GetItemPacket();
                memcpy(getItemPacket, p->data, sizeof(GetItemPacket));
                SetGetItemData(getItemPacket->get_item);
                delete getItemPacket;
            }

            // Damage Packet
            if (connectionPacket->packet_type == 3) {
                DamagePacket* damagePacket = new DamagePacket();
                memcpy(damagePacket, p->data, sizeof(DamagePacket));
                Online_DamagePlayer(damagePacket->damageValue, damagePacket->damageEffect,
                                    damagePacket->knockbackRotation);
                delete damagePacket;
            }

            // Scene Flag Packet
            if (connectionPacket->packet_type == 4) {
                SceneFlagPacket* sceneFlagPacket = new SceneFlagPacket();
                memcpy(sceneFlagPacket, p->data, sizeof(SceneFlagPacket));
                Online_SetSceneFlag(sceneFlagPacket->scene_id, sceneFlagPacket->flag_type, sceneFlagPacket->flag_value);
                delete sceneFlagPacket;
            }

            delete connectionPacket;
        }

        SDLNet_FreePacket(p);
    }
}
#pragma once

#include <iostream>
#include <thread>
#include <SDL_net.h>

typedef struct {
    float x, y, z;
} Vec3float; // size = 0x0C

typedef struct {
    int16_t x, y, z;
} Vec3short; // size = 0x0C

typedef struct {
    Vec3float pos;
    Vec3short rot;
} PosRotOnline;

typedef struct OnlinePacket {
    uint8_t player_id;

    PosRotOnline posRot;
    uint8_t biggoron_broken;
    uint16_t scene_id;
    uint16_t sound_id;
    uint8_t puppet_age;

    // SkelAnime Data
    Vec3short jointTable[24];

    uint8_t sheathType;
    uint8_t shieldType;
    uint8_t leftHandType;
    uint8_t rightHandType;

    int16_t faceType;
    uint8_t tunicType;
    uint8_t bootsType;

    uint8_t damageEffect;
    uint8_t damageValue;
} OnlinePacket;

class OnlineServer {
  public:
    bool running = false;
    int maxPlayers = 32;
    TCPsocket clients[32];

    void InitServer(int port);
    void SendPacketMessage(OnlinePacket* packet, TCPsocket* sendTo);

    ~OnlineServer();

  private:
    void WaitForClientConnections();
    void RunServerReceive(TCPsocket receiveClient);
    void CloseServer();

};

class OnlineClient {
  public:
    TCPsocket client;
    bool running = false;

    void InitClient(char* ipAddr, int port);
    void SendPacketMessage(OnlinePacket* packet, TCPsocket* sendTo);

    ~OnlineClient();

  private:
    void WaitForServerConnection(IPaddress ip);
    void RunClientReceive();
    void CloseClient();

};
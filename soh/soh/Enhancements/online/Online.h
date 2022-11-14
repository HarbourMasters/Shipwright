#pragma once

#include <thread>
#include <mutex>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>

#include <SDL_net.h>

namespace Ship {
namespace Online {

#define MAX_PLAYERS 32

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

void InitOnline(char* ipAddr, int port);
void SendPacketMessage(OnlinePacket* packet, TCPsocket* sendTo);

class Server {
  private:
    std::thread onlineThread;
    std::thread connectionsThread;

  public:
    IPaddress ip;
    int port;

    bool serverOpen = false;

    TCPsocket serverSocket;
    TCPsocket clientSockets[MAX_PLAYERS];

    ~Server();
    void WaitForConnections();
    void CreateServer(int serverPort);
    void RunServer();
};

class Client {
  private:
    std::thread onlineThread;

  public:
    IPaddress ip;
    int port;

    TCPsocket clientSocket;

    bool clientConnected = false;

    ~Client();
    void CreateClient(char* ipAddr, int port);
    void RunClient();
};

OnlinePacket serverPacket;
OnlinePacket clientPacket;

Server server = Server();
Client client = Client();

} // namespace Online
} // namespace Ship
#pragma once

#include <iostream>
#include <thread>
#include <SDL_net.h>

#define MAX_PLAYERS 4

typedef struct {
    float_t x, y, z;
} Vec3float; // size = 0x0C

typedef struct {
    int16_t x, y, z;
} Vec3short; // size = 0x0C

typedef struct {
    Vec3float pos;
    Vec3short rot;
} PosRotOnline;

typedef struct PuppetPacket {
    uint8_t packet_type;

    uint8_t player_id;

    PosRotOnline posRot;
    uint8_t biggoron_broken;
    uint16_t scene_id;
    uint16_t sound_id[4];

    float_t ocarina_freqScale;
    float_t ocarina_vol;
    uint8_t ocarina_pitch;

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
} PuppetPacket;

typedef struct GetItemPacket {
    uint8_t packet_type;
    uint8_t player_id;
    int16_t get_item;
} GetItemPacket;

typedef struct DamagePacket {
    uint8_t packet_type;
    uint8_t hit_player_id;
    uint8_t damageEffect;
    uint8_t damageValue;
    int16_t knockbackRotation;
} DamagePacket;

typedef struct SceneFlagPacket {
    uint8_t packet_type;
    uint8_t scene_id;
    uint8_t flag_type;
    int32_t flag_value;
} SceneFlagPacket;

typedef struct ConnectionPacket {
    uint8_t packet_type;
    uint8_t player_id;
} ConnectionPacket;

class OnlineClient {
  public:
    bool running = false;

    void InitClient(char* ipAddr, int port);
    void SendPuppetPacketMessage(PuppetPacket* packet);
    void SendGetItemPacketMessage(int16_t itemId);
    void SendDamagePacketMessage(DamagePacket* packet);
    void SendSceneFlagPacketMessage(uint8_t scene_num, uint8_t flag_type, int32_t flag_value);

    void CloseClient();

  private:
    void RunClientReceive();
};
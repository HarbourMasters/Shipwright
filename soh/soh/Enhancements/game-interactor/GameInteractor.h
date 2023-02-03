#pragma once

#ifndef GameInteractor_h
#define GameInteractor_h

#include "GameInteractionEffect.h"

typedef enum {
    /* 0x00 */ GI_LINK_SIZE_NORMAL,
    /* 0x01 */ GI_LINK_SIZE_GIANT,
    /* 0x02 */ GI_LINK_SIZE_MINISH,
    /* 0x03 */ GI_LINK_SIZE_PAPER,
    /* 0x04 */ GI_LINK_SIZE_RESET
} GILinkSize;

typedef enum {
    /* 0x00 */ GI_GRAVITY_LEVEL_LIGHT,
    /* 0x01 */ GI_GRAVITY_LEVEL_NORMAL,
    /* 0x02 */ GI_GRAVITY_LEVEL_HEAVY,
} GIGravityLevel;

#ifdef __cplusplus
extern "C" {
#endif
uint8_t GameInteractor_NoUIActive();
GILinkSize GameInteractor_GetLinkSize();
void GameInteractor_SetLinkSize(GILinkSize size);
uint8_t GameInteractor_InvisibleLinkActive();
uint8_t GameInteractor_OneHitKOActive();
uint8_t GameInteractor_PacifistModeActive();
uint8_t GameInteractor_DisableZTargetingActive();
uint8_t GameInteractor_ReverseControlsActive();
int32_t GameInteractor_DefenseModifier();
int32_t GameInteractor_RunSpeedModifier();
GIGravityLevel GameInteractor_GravityLevel();
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
#include <thread>
#include <SDL2/SDL_net.h>
#include <nlohmann/json.hpp>

class GameInteractor {
public:
    static GameInteractor* Instance;

    // Gsme State
    class State {
    public:
        static bool NoUIActive;
        static GILinkSize LinkSize;
        static bool InvisibleLinkActive;
        static bool OneHitKOActive;
        static bool PacifistModeActive;
        static bool DisableZTargetingActive;
        static bool ReverseControlsActive;
        static int32_t DefenseModifier;
        static int32_t RunSpeedModifier;
        static GIGravityLevel GravityLevel;

        static void SetPacifistMode(bool active);
    };

    // Remote
    // allow a remote forwarder to be set - to which we forward all messages
    void EnableRemoteInteractor(const char *host, Uint16 port);
    void DisableRemoteInteractor();
    void RegisterRemoteForwarder(std::function<void(nlohmann::json)> method);
    void TransmitMessageToRemote(nlohmann::json payload);

    // Effects
    static GameInteractionEffectQueryResult CanApplyEffect(GameInteractionEffectBase* effect);
    static GameInteractionEffectQueryResult ApplyEffect(GameInteractionEffectBase* effect);
    static GameInteractionEffectQueryResult RemoveEffect(GameInteractionEffectBase* effect);

    // Helpers
    static bool IsSaveLoaded();
    static bool IsGameplayPaused();
    static bool CanSpawnEnemy();

    class RawAction {
    public:
        static void AddOrRemoveHealthContainers(int16_t amount);
        static void AddOrRemoveMagic(int8_t amount);
        static void HealOrDamagePlayer(int16_t hearts);
        static void SetPlayerHealth(int16_t hearts);
        static void SetLinkInvisibility(bool active);
        static void SetWeatherStorm(bool active);
        static void ForceEquipBoots(int8_t boots);
        static void FreezePlayer();
        static void BurnPlayer();
        static void ElectrocutePlayer();
        static void KnockbackPlayer(float strength);
        static void GiveDekuShield();
        static void SpawnCuccoStorm();
        static void ForceInterfaceUpdate();

        static GameInteractionEffectQueryResult SpawnEnemyWithOffset(uint32_t enemyId, int32_t enemyParams);
    };
private:
    // Remote
    IPaddress remoteIP;
    TCPsocket remoteSocket;
    std::thread remoteThreadReceive;
    bool isRemoteInteractorConnected;
    bool isRemoteInteractorEnabled;
    std::function<void(nlohmann::json)> remoteForwarder;

    void ReceiveFromServer();
    void HandleRemoteMessage(char message[512]);
};

#endif /* __cplusplus */
#endif /* GameInteractor_h */

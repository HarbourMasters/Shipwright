#pragma once

#ifndef GameInteractor_h
#define GameInteractor_h

#include "GameInteractionEffect.h"

enum {
    GI_LINK_SIZE_NORMAL,
    GI_LINK_SIZE_GIANT,
    GI_LINK_SIZE_MINISH,
    GI_LINK_SIZE_PAPER
};

#define GRAVITY_LEVEL_NORMAL 1.0f
#define GRAVITY_LEVEL_LIGHT 0.0f
#define GRAVITY_LEVEL_HEAVY 2.0f

#ifdef __cplusplus
extern "C" {
#endif
uint8_t GameInteractor_NoUIActive();
uint8_t GameInteractor_GiantLinkActive();
uint8_t GameInteractor_MinishLinkActive();
uint8_t GameInteractor_PaperLinkActive();
uint8_t GameInteractor_InvisibleLinkActive();
uint8_t GameInteractor_ResetLinkScale();
void GameInteractor_SetResetLinkScale(uint8_t scale);
uint8_t GameInteractor_OneHitKOActive();
uint8_t GameInteractor_PacifistModeActive();
uint8_t GameInteractor_DisableZTargetingActive();
uint8_t GameInteractor_ReverseControlsActive();
int32_t GameInteractor_DefenseModifier();
int32_t GameInteractor_RunSpeedModifier();
uint8_t GameInteractor_GravityLevel();
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class GameInteractor {
public:
    static GameInteractor* Instance;

    // Gsme State
    class State {
    public:
        static bool NoUIActive;
        static bool GiantLinkActive;
        static bool MinishLinkActive;
        static bool PaperLinkActive;
        static bool InvisibleLinkActive;
        static bool ResetLinkScale;
        static bool OneHitKOActive;
        static bool PacifistModeActive;
        static bool DisableZTargetingActive;
        static bool ReverseControlsActive;
        static int32_t DefenseModifier;
        static int32_t RunSpeedModifier;
        static bool GravityLevel;
    };

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
        static void AddOrRemoveHealthContainers(int32_t amount);
        static void AddOrRemoveMagic(int32_t amount);
        static void HealOrDamagePlayer(int32_t hearts);
        static void SetPlayerHealth(uint32_t hearts);
        static void SetLinkSize(uint8_t size);
        static void SetLinkInvisibility(uint8_t effectState);
        static void SetLinkGravity(int32_t gravityLevel);
        static void SetPacifistMode(uint8_t effectState);
        static void SetWeatherStorm(uint8_t effectState);
        static void ForceEquipBoots(uint8_t boots);
        static void FreezePlayer();
        static void BurnPlayer();
        static void ElectrocutePlayer();
        static void KnockbackPlayer(uint8_t strength);
        static void GiveDekuShield();
        static void SpawnCuccoStorm();

        static GameInteractionEffectQueryResult SpawnEnemyWithOffset(uint32_t enemyId, int32_t enemyParams);
    };
};

#endif /* __cplusplus */
#endif /* GameInteractor_h */

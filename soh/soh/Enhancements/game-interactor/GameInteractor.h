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

#ifdef __cplusplus
class GameInteractor {
public:
    static GameInteractor* Instance;

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

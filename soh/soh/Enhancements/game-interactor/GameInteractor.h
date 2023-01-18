#pragma once

#ifndef GameInteractor_h
#define GameInteractor_h

#include "GameInteractionEffect.h"

#ifdef __cplusplus
class GameInteractor {
public:
    static GameInteractor* Instance;

    static GameInteractionEffectQueryResult CanApplyEffect(GameInteractionEffectBase* effect);
    static void ApplyEffect(GameInteractionEffectBase* effect);
    static void RemoveEffect(GameInteractionEffectBase* effect);
    static bool IsSaveLoaded();
    static bool IsGameplayPaused();
    static void AddOrRemoveHealthContainers(int32_t amount);
    static void AddOrRemoveMagic(int32_t amount);
    static void HealOrDamagePlayer(int32_t hearts);
    static void SetPacifistMode(uint8_t effectState);
    static void SetWeatherStorm(uint8_t effectState);
    static void ForceEquipBoots(uint8_t boots);
    static void FreezePlayer();
    static void BurnPlayer();
    static void ElectrocutePlayer();
    static void KnockbackPlayer(uint8_t strength);
    static void GiveDekuShield();
    static void SpawnCuccoStorm();
    static uint8_t SpawnEnemyWithOffset(uint32_t enemyId, int32_t enemyParams);
};

#endif /* __cplusplus */
#endif /* GameInteractor_h */

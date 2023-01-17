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
};

#endif /* __cplusplus */
#endif /* GameInteractor_h */

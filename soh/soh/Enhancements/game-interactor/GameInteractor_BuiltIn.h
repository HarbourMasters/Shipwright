#ifdef ENABLE_REMOTE_CONTROL

#ifdef __cplusplus
#include <SDL2/SDL_net.h>
#include <cstdint>
#include <thread>
#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>

#include "./GameInteractor.h"

typedef enum {
    modifyHeartContainers,
    fillMagic,
    emptyMagic,
    modifyRupees,
    noUI,
    modifyGravity,
    modifyHealth,
    setPlayerHealth,
    freezePlayer,
    burnPlayer,
    electrocutePlayer,
    knockbackPlayer,
    modifyLinkSize,
    invisibleLink,
    pacifistMode,
    disableZTargeting,
    weatherRainstorm,
    reverseControls,
    forceEquipBoots,
    modifyRunSpeedModifier,
    oneHitKO,
    modifyDefenseModifier,
    giveOrTakeShield,
    teleportPlayer,
    clearAssignedButtons,
    setTimeOfDay,
    setCollisionViewer,
    setCosmeticsColor,
    randomizeCosmetics,
    pressButton,
    pressRandomButton,
    addOrTakeAmmo,
    randomBombFuseTimer,
    disableLedgeGrabs,
    randomWind,
    randomBonks,
    playerInvincibility,
    slipperyFloor,
} GameInteractorBuiltInEffects;

class GameInteractorBuiltIn {
    private:
        bool isEnabled;

        void HandleRemoteJson(nlohmann::json payload);
        GameInteractionEffectBase* EffectFromJson(std::string name, nlohmann::json payload);
    public:
        static GameInteractorBuiltIn* Instance;
        void Enable();
        void Disable();
};
#endif
#endif

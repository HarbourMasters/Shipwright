#pragma once

#ifndef GameInteractor_h
#define GameInteractor_h

#include "libultraship/libultraship.h"
#include <libultraship/bridge.h>
#include "vanilla-behavior/GIVanillaBehavior.h"
#include "EventSystem_List.h"
#include <z64.h>

typedef enum {
    /* 0x00 */ GI_LINK_SIZE_NORMAL,
    /* 0x01 */ GI_LINK_SIZE_GIANT,
    /* 0x02 */ GI_LINK_SIZE_MINISH,
    /* 0x03 */ GI_LINK_SIZE_PAPER,
    /* 0x03 */ GI_LINK_SIZE_SQUISHED,
    /* 0x04 */ GI_LINK_SIZE_RESET
} GILinkSize;

typedef enum {
    /* 0x00 */ GI_GRAVITY_LEVEL_LIGHT,
    /* 0x01 */ GI_GRAVITY_LEVEL_NORMAL,
    /* 0x02 */ GI_GRAVITY_LEVEL_HEAVY,
} GIGravityLevel;

typedef enum {
    /* 0x00 */ GI_BUTTONS_CBUTTONS,
    /* 0x01 */ GI_BUTTONS_DPAD,
} GIButtonSet;

typedef enum {
    /*      */ GI_TIMEOFDAY_DAWN = 32768,
    /*      */ GI_TIMEOFDAY_NOON = 49152,
    /*      */ GI_TIMEOFDAY_DUSK = 0,
    /*      */ GI_TIMEOFDAY_MIDNIGHT = 16384,
} GITimeOfDay;

typedef enum {
    /* 0x00 */ GI_COSMETICS_TUNICS,
    /* 0x01 */ GI_COSMETICS_NAVI,
    /* 0x02 */ GI_COSMETICS_HAIR,
} GICosmeticCategories;

typedef enum {
    /* 0x00 */ GI_COLOR_RED,
    /* 0x01 */ GI_COLOR_GREEN,
    /* 0x02 */ GI_COLOR_BLUE,
    /* 0x03 */ GI_COLOR_ORANGE,
    /* 0x04 */ GI_COLOR_YELLOW,
    /* 0x05 */ GI_COLOR_PURPLE,
    /* 0x06 */ GI_COLOR_PINK,
    /* 0x07 */ GI_COLOR_BROWN,
    /* 0x08 */ GI_COLOR_BLACK,
} GIColors;

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
float GameInteractor_MovementSpeedMultiplier();
GIGravityLevel GameInteractor_GravityLevel();
uint32_t GameInteractor_GetEmulatedButtons();
void GameInteractor_SetEmulatedButtons(uint32_t buttons);
uint8_t GameInteractor_GetRandomBombFuseTimerActive();
uint8_t GameInteractor_GetDisableLedgeGrabsActive();
uint8_t GameInteractor_GetRandomWindActive();
uint8_t GameInteractor_GetRandomBonksActive();
uint8_t GameInteractor_GetSlipperyFloorActive();
uint8_t GameInteractor_SecondCollisionUpdate();
void GameInteractor_SetTriforceHuntPieceGiven(uint8_t state);
void GameInteractor_SetTriforceHuntCreditsWarpActive(uint8_t state);
bool GameInteractor_Should(GIVanillaBehavior flag, uint32_t result, ...);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <stdarg.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <functional>
#include <cstring>

#include <version>
#ifdef __cpp_lib_source_location
#include <source_location>
#else
#pragma message("Compiling without <source_location> support, the Hook Debugger will not be available")
#endif

#include "GameInteractionEffect.h"

enum HookType {
    HOOK_TYPE_NORMAL,
    HOOK_TYPE_ID,
    HOOK_TYPE_PTR,
    HOOK_TYPE_FILTER,
};

struct HookRegisteringInfo {
    bool valid;
    const char* file;
    std::uint_least32_t line;
    std::uint_least32_t column;
    const char* function;
    HookType type;

    HookRegisteringInfo()
        : valid(false), file("unknown file"), line(0), column(0), function("unknown function"), type(HOOK_TYPE_NORMAL) {
    }

    HookRegisteringInfo(const char* _file, std::uint_least32_t _line, std::uint_least32_t _column,
                        const char* _function, HookType _type)
        : valid(true), file(_file), line(_line), column(_column), function(_function), type(_type) {
        // Trim off user parent directories
        const char* trimmed = strstr(_file, "soh/soh/");
        if (trimmed != nullptr) {
            file = trimmed;
        }
    }
};

struct HookInfo {
    uint32_t calls;
    HookRegisteringInfo registering;
};

#ifdef __cpp_lib_source_location
#define GET_CURRENT_REGISTERING_INFO(type) \
    (HookRegisteringInfo{ location.file_name(), location.line(), location.column(), location.function_name(), type })
#else
#define GET_CURRENT_REGISTERING_INFO(type) (HookRegisteringInfo{})
#endif

#define REGISTER_VB_SHOULD(flagToCheck, body)                                    \
    REGISTER_LISTENER(OnVanillaBehavior, EVENT_PRIORITY_LOW, [](IEvent* event) { \
        auto* vbEvent = reinterpret_cast<OnVanillaBehavior*>(event);             \
        if (vbEvent->flag == flagToCheck) {                                      \
            bool* should = vbEvent->result;                                      \
            va_list args;                                                        \
            va_copy(args, *vbEvent->originalArgs);                               \
            body;                                                                \
            va_end(args);                                                        \
        }                                                                        \
    })

#define COND_HOOK(eventId, condition, body)                                    \
    {                                                                          \
        static ListenerID listenerId = -1;                                     \
        if (listenerId != -1) {                                                \
            UNREGISTER_LISTENER(eventId, listenerId);                          \
            listenerId = -1;                                                   \
        }                                                                      \
        if (condition) {                                                       \
            listenerId = REGISTER_LISTENER(eventId, EVENT_PRIORITY_LOW, body); \
        }                                                                      \
    }
#define COND_ID_HOOK(eventId, id, condition, body)                             \
    {                                                                          \
        static ListenerID listenerId = -1;                                     \
        if (listenerId != -1) {                                                \
            UNREGISTER_LISTENER(eventId, listenerId);                          \
            listenerId = -1;                                                   \
        }                                                                      \
        if (condition) {                                                       \
            listenerId = REGISTER_LISTENER(eventId, EVENT_PRIORITY_LOW, body); \
        }                                                                      \
    }
#define COND_VB_SHOULD(id, condition, body)                     \
    {                                                           \
        static ListenerID listenerId = -1;                      \
        if (listenerId != -1) {                                 \
            UNREGISTER_LISTENER(OnVanillaBehavior, listenerId); \
            listenerId = -1;                                    \
        }                                                       \
        if (condition) {                                        \
            listenerId = REGISTER_VB_SHOULD(id, body);          \
        }                                                       \
    }

class GameInteractor {
  public:
    static GameInteractor* Instance;

    // Game State
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
        static float MovementSpeedMultiplier;
        static GIGravityLevel GravityLevel;
        static uint32_t EmulatedButtons;
        static uint8_t RandomBombFuseTimerActive;
        static uint8_t DisableLedgeGrabsActive;
        static uint8_t RandomWindActive;
        static uint8_t RandomWindSecondsSinceLastDirectionChange;
        static uint8_t RandomBonksActive;
        static uint8_t SlipperyFloorActive;
        static uint8_t SecondCollisionUpdate;
        static uint8_t TriforceHuntPieceGiven;
        static uint8_t TriforceHuntCreditsWarpActive;

        static void SetPacifistMode(bool active);
    };

    // Effects
    static GameInteractionEffectQueryResult CanApplyEffect(GameInteractionEffectBase& effect);
    static GameInteractionEffectQueryResult ApplyEffect(GameInteractionEffectBase& effect);
    static GameInteractionEffectQueryResult RemoveEffect(RemovableGameInteractionEffect& effect);

    // Helpers
    static bool IsSaveLoaded(bool allowDbgSave = false);
    static bool IsGameplayPaused();
    static bool CanSpawnActor();
    static bool CanAddOrTakeAmmo(int16_t amount, int16_t item);

    class RawAction {
      public:
        static void SetSceneFlag(int16_t sceneNum, int16_t flagType, int16_t flag);
        static void UnsetSceneFlag(int16_t sceneNum, int16_t flagType, int16_t flag);
        static bool CheckFlag(int16_t flagType, int16_t flag);
        static void SetFlag(int16_t flagType, int16_t chestNum);
        static void UnsetFlag(int16_t flagType, int16_t chestNum);
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
        static void GiveOrTakeShield(int32_t shield);
        static void ForceInterfaceUpdate();
        static void UpdateActor(void* refActor);
        static void TeleportPlayer(int32_t nextEntrance);
        static void ClearAssignedButtons(uint8_t buttonSet);
        static void SetTimeOfDay(uint32_t time);
        static void SetCollisionViewer(bool active);
        static void EmulateButtonPress(int32_t button);
        static void AddOrTakeAmmo(int16_t amount, int16_t item);
        static void EmulateRandomButtonPress(uint32_t chancePercentage = 100);
        static void SetRandomWind(bool active);
        static void SetPlayerInvincibility(bool active);
        static void ClearCutscenePointer();

        static GameInteractionEffectQueryResult SpawnEnemyWithOffset(uint32_t enemyId, int32_t enemyParams,
                                                                     std::string nameTag = "");
        static GameInteractionEffectQueryResult SpawnActor(uint32_t actorId, int32_t actorParams,
                                                           std::string nameTag = "");
    };
};

void EventSystem_Register();

#undef GET_CURRENT_REGISTERING_INFO

#endif /* __cplusplus */
#endif /* GameInteractor_h */

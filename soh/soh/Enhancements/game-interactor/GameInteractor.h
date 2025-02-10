#pragma once

#ifndef GameInteractor_h
#define GameInteractor_h

#include "libultraship/libultraship.h"
#include "GameInteractionEffect.h"
#include "vanilla-behavior/GIVanillaBehavior.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
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

typedef enum {
    /*      */ GI_TP_DEST_LINKSHOUSE = ENTR_LINKS_HOUSE_CHILD_SPAWN,
    /*      */ GI_TP_DEST_MINUET = ENTR_SACRED_FOREST_MEADOW_WARP_PAD,
    /*      */ GI_TP_DEST_BOLERO = ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD,
    /*      */ GI_TP_DEST_SERENADE = ENTR_LAKE_HYLIA_WARP_PAD,
    /*      */ GI_TP_DEST_REQUIEM = ENTR_DESERT_COLOSSUS_WARP_PAD,
    /*      */ GI_TP_DEST_NOCTURNE = ENTR_GRAVEYARD_WARP_PAD,
    /*      */ GI_TP_DEST_PRELUDE = ENTR_TEMPLE_OF_TIME_WARP_PAD,
} GITeleportDestinations;

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
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
#include <stdarg.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>
#include <version>
#ifdef __cpp_lib_source_location
#include <source_location>
#else
#pragma message("Compiling without <source_location> support, the Hook Debugger will not be avaliable")
#endif

typedef uint32_t HOOK_ID;

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

    HookRegisteringInfo() : valid(false), file("unknown file"), line(0), column(0), function("unknown function"), type(HOOK_TYPE_NORMAL) {}

    HookRegisteringInfo(const char* _file, std::uint_least32_t _line, std::uint_least32_t _column, const char* _function, HookType _type) :
        valid(true), file(_file), line(_line), column(_column), function(_function), type(_type) {}
};

struct HookInfo {
    uint32_t calls;
    HookRegisteringInfo registering;

    HookInfo() : calls(0), registering(HookRegisteringInfo{}) {}
    HookInfo(HookRegisteringInfo _registering) : calls(0), registering(_registering) {}
};

#ifdef __cpp_lib_source_location
#define GET_CURRENT_REGISTERING_INFO(type) HookRegisteringInfo{location.file_name(), location.line(), location.column(), location.function_name(), type}
#else
#define GET_CURRENT_REGISTERING_INFO(type) HookRegisteringInfo{}
#endif

#define REGISTER_VB_SHOULD(flag, body)                                                  \
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnVanillaBehavior>( \
        flag, [](GIVanillaBehavior _, bool* should, va_list _originalArgs) {            \
            va_list args;                                                               \
            va_copy(args, _originalArgs);                                               \
            body;                                                                       \
            va_end(args);                                                               \
        })

#define COND_HOOK(hookType, condition, body)                                                     \
    {                                                                                            \
        static HOOK_ID hookId = 0;                                                               \
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::hookType>(hookId);          \
        hookId = 0;                                                                              \
        if (condition) {                                                                         \
            hookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::hookType>(body); \
        }                                                                                        \
    }
#define COND_ID_HOOK(hookType, id, condition, body)                                                       \
    {                                                                                                     \
        static HOOK_ID hookId = 0;                                                                        \
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::hookType>(hookId);              \
        hookId = 0;                                                                                       \
        if (condition) {                                                                                  \
            hookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::hookType>(id, body); \
        }                                                                                                 \
    }
#define COND_VB_SHOULD(id, condition, body)                                                               \
    {                                                                                                     \
        static HOOK_ID hookId = 0;                                                                        \
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnVanillaBehavior>(hookId); \
        hookId = 0;                                                                                       \
        if (condition) {                                                                                  \
            hookId = REGISTER_VB_SHOULD(id, body);                                                        \
        }                                                                                                 \
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
        static int32_t RunSpeedModifier;
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
    static GameInteractionEffectQueryResult CanApplyEffect(GameInteractionEffectBase* effect);
    static GameInteractionEffectQueryResult ApplyEffect(GameInteractionEffectBase* effect);
    static GameInteractionEffectQueryResult RemoveEffect(RemovableGameInteractionEffect* effect);

    // Game Hooks
    HOOK_ID nextHookId = 1;
    template <typename H> struct RegisteredGameHooks {
        inline static std::unordered_map<HOOK_ID, typename H::fn> functions;
        inline static std::unordered_map<int32_t, std::unordered_map<HOOK_ID, typename H::fn>> functionsForID;
        inline static std::unordered_map<uintptr_t, std::unordered_map<HOOK_ID, typename H::fn>> functionsForPtr;
        inline static std::unordered_map<HOOK_ID, std::pair<typename H::filter, typename H::fn>> functionsForFilter;

        //Used for the hook debugger
        inline static std::unordered_map<HOOK_ID, HookInfo> hookData;
    };

    template <typename H> struct HooksToUnregister {
        inline static std::vector<HOOK_ID> hooks;
        inline static std::vector<HOOK_ID> hooksForID;
        inline static std::vector<HOOK_ID> hooksForPtr;
        inline static std::vector<HOOK_ID> hooksForFilter;
    };

    template <typename H> std::unordered_map<uint32_t, HookInfo>* GetHookData() {
        return &RegisteredGameHooks<H>::hookData;
    }

    // General Hooks
    template <typename H> HOOK_ID RegisterGameHook(
        typename H::fn h
#ifdef __cpp_lib_source_location
        , const std::source_location location = std::source_location::current()
#endif
    ) {
        // Ensure hook id is unique and not 0, which is reserved for invalid hooks
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functions.find(this->nextHookId) != RegisteredGameHooks<H>::functions.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functions[this->nextHookId] = h;
        RegisteredGameHooks<H>::hookData[this->nextHookId] = HookInfo{GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_NORMAL)};
        return this->nextHookId++;
    }

    template <typename H> void UnregisterGameHook(HOOK_ID hookId) {
        if (hookId == 0) return;
        HooksToUnregister<H>::hooks.push_back(hookId);
    }

    template <typename H, typename... Args> void ExecuteHooks(Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooks) {
            RegisteredGameHooks<H>::functions.erase(hookId);
            RegisteredGameHooks<H>::hookData.erase(hookId);
        }
        HooksToUnregister<H>::hooks.clear();
        for (auto& hook : RegisteredGameHooks<H>::functions) {
            hook.second(std::forward<Args>(args)...);
            RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
        }
    }

    // ID based Hooks
    template <typename H> HOOK_ID RegisterGameHookForID(
        int32_t id, typename H::fn h
#ifdef __cpp_lib_source_location
        , const std::source_location location = std::source_location::current()
#endif
    ) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForID[id].find(this->nextHookId) != RegisteredGameHooks<H>::functionsForID[id].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForID[id][this->nextHookId] = h;
        RegisteredGameHooks<H>::hookData[this->nextHookId] = HookInfo{GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_ID)};
        return this->nextHookId++;
    }

    template <typename H> void UnregisterGameHookForID(HOOK_ID hookId) {
        if (hookId == 0) return;
        HooksToUnregister<H>::hooksForID.push_back(hookId);
    }

    template <typename H, typename... Args> void ExecuteHooksForID(int32_t id, Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooksForID) {
            for (auto it = RegisteredGameHooks<H>::functionsForID[id].begin(); it != RegisteredGameHooks<H>::functionsForID[id].end(); ) {
                if (it->first == hookId) {
                    it = RegisteredGameHooks<H>::functionsForID[id].erase(it);
                    HooksToUnregister<H>::hooksForID.erase(std::remove(HooksToUnregister<H>::hooksForID.begin(), HooksToUnregister<H>::hooksForID.end(), hookId), HooksToUnregister<H>::hooksForID.end());
                    RegisteredGameHooks<H>::hookData.erase(hookId);
                } else {
                    ++it;
                }
            }
        }
        for (auto& hook : RegisteredGameHooks<H>::functionsForID[id]) {
            hook.second(std::forward<Args>(args)...);
            RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
        }
    }

    // PTR based Hooks
    template <typename H> HOOK_ID RegisterGameHookForPtr(
        uintptr_t ptr, typename H::fn h
#ifdef __cpp_lib_source_location
        , const std::source_location location = std::source_location::current()
#endif
    ) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForPtr[ptr].find(this->nextHookId) != RegisteredGameHooks<H>::functionsForPtr[ptr].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForPtr[ptr][this->nextHookId] = h;
        RegisteredGameHooks<H>::hookData[this->nextHookId] = HookInfo{GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_PTR)};
        return this->nextHookId++;
    }

    template <typename H> void UnregisterGameHookForPtr(HOOK_ID hookId) {
        if (hookId == 0) return;
        HooksToUnregister<H>::hooksForPtr.push_back(hookId);
    }

    template <typename H, typename... Args> void ExecuteHooksForPtr(uintptr_t ptr, Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooksForPtr) {
            for (auto it = RegisteredGameHooks<H>::functionsForPtr[ptr].begin(); it != RegisteredGameHooks<H>::functionsForPtr[ptr].end(); ) {
                if (it->first == hookId) {
                    it = RegisteredGameHooks<H>::functionsForPtr[ptr].erase(it);
                    HooksToUnregister<H>::hooksForPtr.erase(std::remove(HooksToUnregister<H>::hooksForPtr.begin(), HooksToUnregister<H>::hooksForPtr.end(), hookId), HooksToUnregister<H>::hooksForPtr.end());
                    RegisteredGameHooks<H>::hookData.erase(hookId);
                } else {
                    ++it;
                }
            }
        }
        for (auto& hook : RegisteredGameHooks<H>::functionsForPtr[ptr]) {
            hook.second(std::forward<Args>(args)...);
            RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
        }
    }

    // Filter based Hooks
    template <typename H> HOOK_ID RegisterGameHookForFilter(
        typename H::filter f, typename H::fn h
#ifdef __cpp_lib_source_location
        , const std::source_location location = std::source_location::current()
#endif
    ) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForFilter.find(this->nextHookId) != RegisteredGameHooks<H>::functionsForFilter.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForFilter[this->nextHookId] = std::make_pair(f, h);
        RegisteredGameHooks<H>::hookData[this->nextHookId] = HookInfo{GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_FILTER)};
        return this->nextHookId++;
    }

    template <typename H> void UnregisterGameHookForFilter(HOOK_ID hookId) {
        if (hookId == 0) return;
        HooksToUnregister<H>::hooksForFilter.push_back(hookId);
    }

    template <typename H, typename... Args> void ExecuteHooksForFilter(Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooksForFilter) {
            RegisteredGameHooks<H>::functionsForFilter.erase(hookId);
            RegisteredGameHooks<H>::hookData.erase(hookId);
        }
        HooksToUnregister<H>::hooksForFilter.clear();
        for (auto& hook : RegisteredGameHooks<H>::functionsForFilter) {
            if (hook.second.first(std::forward<Args>(args)...)) {
                hook.second.second(std::forward<Args>(args)...);
                RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
            }
        }
    }

    class HookFilter {
    public:
        static auto ActorNotPlayer(Actor* actor) {
            return actor->id != ACTOR_PLAYER;
        }
        // For use with Should hooks
        static auto SActorNotPlayer(Actor* actor, bool* result) {
            return actor->id != ACTOR_PLAYER;
        }
        static auto ActorMatchIdAndParams(int16_t id, int16_t params) {
            return [id, params](Actor* actor) {
                return actor->id == id && actor->params == params;
            };
        }
        // For use with Should hooks
        static auto SActorMatchIdAndParams(int16_t id, int16_t params) {
            return [id, params](Actor* actor, bool* result) {
                return actor->id == id && actor->params == params;
            };
        }
    };

#define DEFINE_HOOK(name, args)                  \
    struct name {                                \
        typedef std::function<void args> fn;     \
        typedef std::function<bool args> filter; \
    }

#include "GameInteractor_HookTable.h"

#undef DEFINE_HOOK

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
        static void SetCosmeticsColor(uint8_t cosmeticCategory, uint8_t colorValue);
        static void RandomizeCosmeticsColors(bool excludeBiddingWarColors);
        static void EmulateButtonPress(int32_t button);
        static void AddOrTakeAmmo(int16_t amount, int16_t item);
        static void EmulateRandomButtonPress(uint32_t chancePercentage = 100);
        static void SetRandomWind(bool active);
        static void SetPlayerInvincibility(bool active);
        static void ClearCutscenePointer();

        static GameInteractionEffectQueryResult SpawnEnemyWithOffset(uint32_t enemyId, int32_t enemyParams);
        static GameInteractionEffectQueryResult SpawnActor(uint32_t actorId, int32_t actorParams);
    };
};

#undef GET_CURRENT_REGISTERING_INFO

#endif /* __cplusplus */
#endif /* GameInteractor_h */

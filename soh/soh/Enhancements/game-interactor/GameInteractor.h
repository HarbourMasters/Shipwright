#pragma once

#ifndef GameInteractor_h
#define GameInteractor_h

#include "GameInteractionEffect.h"
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
    /*      */ GI_TP_DEST_LINKSHOUSE = ENTR_LINKS_HOUSE_0,
    /*      */ GI_TP_DEST_MINUET = ENTR_SACRED_FOREST_MEADOW_2,
    /*      */ GI_TP_DEST_BOLERO = ENTR_DEATH_MOUNTAIN_CRATER_4,
    /*      */ GI_TP_DEST_SERENADE = ENTR_LAKE_HYLIA_8,
    /*      */ GI_TP_DEST_REQUIEM = ENTR_DESERT_COLOSSUS_5,
    /*      */ GI_TP_DEST_NOCTURNE = ENTR_GRAVEYARD_7,
    /*      */ GI_TP_DEST_PRELUDE = ENTR_TEMPLE_OF_TIME_7,
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

#include <vector>
#include <functional>

#define DEFINE_HOOK(name, type)         \
    struct name {                       \
        typedef std::function<type> fn; \
    }

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
    static GameInteractionEffectQueryResult RemoveEffect(GameInteractionEffectBase* effect);

    // Game Hooks
    template <typename H> struct RegisteredGameHooks { inline static std::vector<typename H::fn> functions; };
    template <typename H> void RegisterGameHook(typename H::fn h) { RegisteredGameHooks<H>::functions.push_back(h); }
    template <typename H, typename... Args> void ExecuteHooks(Args&&... args) {
        for (auto& fn : RegisteredGameHooks<H>::functions) {
            fn(std::forward<Args>(args)...);
        }
    }

    DEFINE_HOOK(OnLoadGame, void(int32_t fileNum));
    DEFINE_HOOK(OnExitGame, void(int32_t fileNum));
    DEFINE_HOOK(OnGameFrameUpdate, void());
    DEFINE_HOOK(OnItemReceive, void(GetItemEntry itemEntry));
    DEFINE_HOOK(OnSaleEnd, void(GetItemEntry itemEntry));
    DEFINE_HOOK(OnTransitionEnd, void(int16_t sceneNum));
    DEFINE_HOOK(OnSceneInit, void(int16_t sceneNum));
    DEFINE_HOOK(OnSceneFlagSet, void(int16_t sceneNum, int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnSceneFlagUnset, void(int16_t sceneNum, int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnFlagSet, void(int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnFlagUnset, void(int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnSceneSpawnActors, void());
    DEFINE_HOOK(OnPlayerUpdate, void());
    DEFINE_HOOK(OnOcarinaSongAction, void());
    DEFINE_HOOK(OnShopSlotChange, void(uint8_t cursorIndex, int16_t price));
    DEFINE_HOOK(OnActorInit, void(void* actor));
    DEFINE_HOOK(OnActorUpdate, void(void* actor));
    DEFINE_HOOK(OnActorKill, void(void* actor));
    DEFINE_HOOK(OnEnemyDefeat, void(void* actor));
    DEFINE_HOOK(OnPlayerBonk, void());
    DEFINE_HOOK(OnPlayDestroy, void());
    DEFINE_HOOK(OnPlayDrawEnd, void());

    DEFINE_HOOK(OnSaveFile, void(int32_t fileNum));
    DEFINE_HOOK(OnLoadFile, void(int32_t fileNum));
    DEFINE_HOOK(OnDeleteFile, void(int32_t fileNum));
    
    DEFINE_HOOK(OnDialogMessage, void());
    DEFINE_HOOK(OnPresentTitleCard, void());
    DEFINE_HOOK(OnInterfaceUpdate, void());
    DEFINE_HOOK(OnKaleidoscopeUpdate, void(int16_t inDungeonScene));
    
    DEFINE_HOOK(OnPresentFileSelect, void());
    DEFINE_HOOK(OnUpdateFileSelectSelection, void(uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileSelectConfirmationSelection, void(uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileCopySelection, void(uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileCopyConfirmationSelection, void(uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileEraseSelection, void(uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileEraseConfirmationSelection, void(uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileAudioSelection, void(uint8_t optionIndex));
    DEFINE_HOOK(OnUpdateFileTargetSelection, void(uint8_t optionIndex));
    DEFINE_HOOK(OnUpdateFileLanguageSelection, void(uint8_t optionIndex));
    DEFINE_HOOK(OnUpdateFileQuestSelection, void(uint8_t questIndex));
    DEFINE_HOOK(OnUpdateFileBossRushOptionSelection, void(uint8_t optionIndex, uint8_t optionValue));
    DEFINE_HOOK(OnUpdateFileNameSelection, void(int16_t charCode));
    
    DEFINE_HOOK(OnSetGameLanguage, void());

    // Helpers
    static bool IsSaveLoaded();
    static bool IsGameplayPaused();
    static bool CanSpawnActor();
    static bool CanAddOrTakeAmmo(int16_t amount, int16_t item);

    class RawAction {
    public:
        static void SetSceneFlag(int16_t sceneNum, int16_t flagType, int16_t flag);
        static void UnsetSceneFlag(int16_t sceneNum, int16_t flagType, int16_t flag);
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

#endif /* __cplusplus */
#endif /* GameInteractor_h */

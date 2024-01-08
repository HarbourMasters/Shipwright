#pragma once

#ifndef GameInteractor_h
#define GameInteractor_h

#include "libultraship/libultraship.h"
#include "GameInteractionEffect.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include <z64.h>

typedef enum {
    GI_SCHEME_SAIL,
    GI_SCHEME_CROWD_CONTROL,
} GIScheme;

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

typedef enum {
    // Vanilla condition: gSaveContext.showTitleCard
    GI_VB_SHOW_TITLE_CARD,
    // Opt: *EnWonderTalk2
    GI_VB_WONDER_TALK,
    // Opt: *ElfMsg
    GI_VB_NAVI_TALK,
    // Vanilla condition: INFTABLE_GREETED_BY_SARIA
    GI_VB_NOT_BE_GREETED_BY_SARIA,
    // Opt: *EnMd
    // Vanilla condition: EnMd->interactInfo.talkState == NPC_TALK_STATE_ACTION
    GI_VB_MOVE_MIDO_IN_KOKIRI_FOREST,
    // Opt: *EnMd
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)
    GI_VB_MIDO_CONSIDER_DEKU_TREE_DEAD,
    // Opt: *EnKo
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)
    GI_VB_OPEN_KOKIRI_FOREST,
    // Opt: *EnOwl
    // Vanilla condition: EnOwl->actor.xzDistToPlayer < targetDist
    GI_VB_OWL_INTERACTION,
    // Vanilla condition: EVENTCHKINF_TALON_RETURNED_FROM_CASTLE
    GI_VB_MALON_RETURN_FROM_CASTLE,
    // Vanilla condition: CUR_UPG_VALUE(UPG_STRENGTH) <= 0
    GI_VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD,
    /* Vanilla condition:
    ```
        LINK_IS_ADULT &&
        (gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_TEMPLE_OF_TIME) &&
        CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) &&
        CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
        !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
    ```
    */
    GI_VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS,
    // Vanilla condition: !CHECK_QUEST_ITEM(QUEST_SONG_SARIA)
    GI_VB_BE_ELIGIBLE_FOR_SARIAS_SONG,
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_SONG_EPONA)
    GI_VB_MALON_ALREADY_TAUGHT_EPONAS_SONG,
    // Vanilla condition: CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) && !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER)
    GI_VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER,
    // Vanilla condition: (!CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) && !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER)) && LINK_IS_ADULT
    GI_VB_SHIEK_PREPARE_TO_GIVE_SERENADE_OF_WATER,
    // Vanilla condition: !EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT and EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP
    GI_VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT,
    /* Vanilla Condition: 
    ```
        LINK_IS_ADULT &&
        gSaveContext.entranceIndex == ENTR_KAKARIKO_VILLAGE_0 &&
        Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP) &&
        Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP) &&
        Flags_GetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP) &&
        !Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL);
    ```
    */
    GI_VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW,
    // Opt: *EnGo2
    // Vanilla condition: CUR_CAPACITY(UPG_BOMB_BAG) >= 20 && this->waypoint > 7 && this->waypoint < 12
    GI_VB_BE_ELIGIBLE_FOR_CHILD_ROLLING_GORON_REWARD,
    // Vanilla condition: !CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON)
    GI_VB_BE_ELIGIBLE_FOR_GIANTS_KNIFE_PURCHASE,
    // Opt: *EnMs
    // Vanilla condition: gSaveContext.rupees >= sPrices[BEANS_BOUGHT]
    GI_VB_BE_ELIGIBLE_FOR_MAGIC_BEANS_PURCHASE,
    // Opt: *EnItem00
    // Vanilla condition: Flags_GetCollectible(play, this->collectibleFlag)
    GI_VB_ITEM00_DESPAWN,
    // Opt: *EnTk
    // Vanilla condition: gSaveContext.dayTime <= 0xC000 || gSaveContext.dayTime >= 0xE000 || LINK_IS_ADULT || play->sceneNum != SCENE_GRAVEYARD
    GI_VB_DAMPE_IN_GRAVEYARD_DESPAWN,
    // Opt: *EnTk
    // Vanilla condition: this->validDigHere == 1
    GI_VB_BE_VALID_GRAVEDIGGING_SPOT,
    // Opt: *EnTk
    // Vanilla condition: this->currentReward == 3
    GI_VB_BE_DAMPE_GRAVEDIGGING_GRAND_PRIZE,
    // Opt: *EnTk
    // Vanilla condition: !Flags_GetItemGetInf(ITEMGETINF_1C)
    GI_VB_DAMPE_GRAVEDIGGING_GRAND_PRIZE_BE_HEART_PIECE,
    // Opt: *EnShopnuts
    /* Vanilla Condition:
    ```
        ((this->actor.params == 0x0002) && (Flags_GetItemGetInf(ITEMGETINF_0B))) ||
        ((this->actor.params == 0x0009) && (Flags_GetInfTable(INFTABLE_192))) ||
        ((this->actor.params == 0x000A) && (Flags_GetInfTable(INFTABLE_193)))
    ```
    */
    GI_VB_BUSINESS_SCRUB_DESPAWN,
    // Opt: *EnCow
    // Vanilla condition: play->sceneNum == SCENE_LINKS_HOUSE && (!LINK_IS_ADULT || !Flags_GetEventChkInf(EVENTCHKINF_WON_COW_IN_MALONS_RACE))
    GI_VB_DESPAWN_HORSE_RACE_COW,
    // Opt: *EnHs
    // Vanilla condition: Flags_GetItemGetInf(ITEMGETINF_30)
    GI_VB_DESPAWN_GROG,
    // Opt: *EnKo
    // Vanilla condition: (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_ODD_POTION) ? true : false;
    GI_VB_SPAWN_LW_FADO,
    // Opt: *EnMk
    GI_VB_PLAY_EYEDROP_CREATION_ANIM,
    // Opt: *EnDs
    GI_VB_PLAY_ODD_POTION_ANIM,
    // Opt: *EnMk
    // Vanilla condition: INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_EYEDROPS
    GI_VB_USE_EYEDROP_DIALOGUE,
    // Opt: *EnMk
    // Vanilla condition: Flags_GetItemGetInf(ITEMGETINF_30)
    GI_VB_OFFER_BLUE_POTION,
    // Vanilla condition: Inventory_HasEmptyBottle() == 0
    GI_VB_NEED_BOTTLE_FOR_GRANNYS_ITEM,
    // Opt: *EnNiwLady
    GI_VB_SET_CUCCO_COUNT,
    // Opt: *EnKz
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)
    GI_VB_KING_ZORA_THANK_CHILD,
    // Opt: *EnKz
    // Vanilla condition: this->actor.textId == 0x401A
    GI_VB_BE_ABLE_TO_EXCHANGE_RUTOS_LETTER,
    // Opt: *EnKz
    // Vanilla condition: Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)
    GI_VB_KING_ZORA_BE_MOVED,
    // Vanilla condition: gSaveState.bgsFlag
    GI_VB_BIGGORON_CONSIDER_TRADE_COMPLETE,
    // Vanilla condition: gSaveState.bgsFlag
    GI_VB_BIGGORON_CONSIDER_SWORD_COLLECTED,
    // Vanilla condition: Environment_GetBgsDayCount() >= 3
    GI_VB_BIGGORON_CONSIDER_SWORD_FORGED,
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)
    GI_VB_GORONS_CONSIDER_FIRE_TEMPLE_FINISHED,
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_GORON_RUBY)
    GI_VB_GORONS_CONSIDER_DODONGOS_CAVERN_FINISHED,
    // Opt: *uint16_t
    // Vanilla condition: false
    GI_VB_OVERRIDE_LINK_THE_GORON_DIALOGUE,
    // Opt: *EnGo2
    GI_VB_EN_GO2_RESET_AFTER_GET_ITEM,

    /*** Play Cutscenes ***/

    GI_VB_PLAY_TRANSITION_CS,
    // Opt: *EventChkInf flag
    GI_VB_PLAY_ENTRANCE_CS,
    // Opt: *cutsceneId
    GI_VB_PLAY_ONEPOINT_CS,
    // Opt: *actor
    GI_VB_PLAY_ONEPOINT_ACTOR_CS,
    // Opt: *BgTreemouth
    GI_VB_PLAY_DEKU_TREE_INTRO_CS,
    // Vanilla condition: !EventChkInf except for spirit & shadow temple which are !medallion, and Jabu which always is true
    GI_VB_PLAY_BLUE_WARP_CS,
    GI_VB_PLAY_DARUNIAS_JOY_CS,
    GI_VB_PLAY_SHIEK_BLOCK_MASTER_SWORD_CS,
    // Vanilla condition: !EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL
    GI_VB_PLAY_PULL_MASTER_SWORD_CS,
    GI_VB_PLAY_DROP_FISH_FOR_JABU_CS,
    // Vanilla condition: player->getItemId == GI_GAUNTLETS_SILVER
    GI_VB_PLAY_NABOORU_CAPTURED_CS,
    GI_VB_PLAY_ZELDAS_LULLABY_CS,
    // Opt: *EnSa
    GI_VB_PLAY_SARIAS_SONG_CS,
    GI_VB_PLAY_PRELUDE_OF_LIGHT_CS,
    GI_VB_PLAY_MINUET_OF_FOREST_CS,
    GI_VB_PLAY_BOLERO_OF_FIRE_CS,
    GI_VB_PLAY_SERENADE_OF_WATER_CS,
    GI_VB_PLAY_EYEDROPS_CS,

    /*** Give Items ***/

    GI_VB_GIVE_ITEM_FROM_CHEST,
    GI_VB_GIVE_ITEM_FROM_BLUE_WARP,
    // Opt: *EnItem00
    GI_VB_GIVE_ITEM_FROM_ITEM_00,
    // Opt: *EnSi
    GI_VB_GIVE_ITEM_SKULL_TOKEN,
    // Opt: *EnCow
    GI_VB_GIVE_ITEM_FROM_COW,
    // Opt: *EnDns
    GI_VB_GIVE_ITEM_FROM_BUSINESS_SCRUB,
    // Opt: *EnMk
    GI_VB_GIVE_ITEM_FROM_LAB_DIVE,
    // Opt: *EnDs
    GI_VB_GIVE_ITEM_FROM_GRANNYS_SHOP,
    // Opt: *EnNiwLady
    GI_VB_GIVE_ITEM_FROM_ANJU_AS_CHILD,
    // Opt: *EnNiwLady
    GI_VB_GIVE_ITEM_FROM_ANJU_AS_ADULT,
    // Opt: *EnKz
    // Vanilla condition: !CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA)
    GI_VB_GIVE_ITEM_FROM_THAWING_KING_ZORA,
    // Opt: *EnGo2
    GI_VB_GIVE_ITEM_FROM_ROLLING_GORON_AS_CHILD,
    // Opt: *EnGo2
    GI_VB_GIVE_ITEM_FROM_ROLLING_GORON_AS_ADULT,
    // Opt: *EnJs
    GI_VB_GIVE_ITEM_FROM_CARPET_SALESMAN,
    // Opt: *EnGm
    GI_VB_GIVE_ITEM_FROM_MEDIGORON,
    // Opt: *EnMs
    GI_VB_GIVE_ITEM_FROM_MAGIC_BEAN_SALESMAN,
    // Opt: *EnFr
    GI_VB_GIVE_ITEM_FROM_FROGS,

    GI_VB_GIVE_ITEM_FAIRY_OCARINA,
    GI_VB_GIVE_ITEM_WEIRD_EGG,
    GI_VB_GIVE_ITEM_LIGHT_ARROW,
    GI_VB_GIVE_ITEM_STRENGTH_1,
    GI_VB_GIVE_ITEM_ZELDAS_LETTER,
    GI_VB_GIVE_ITEM_MASTER_SWORD,
    GI_VB_GIVE_ITEM_OCARINA_OF_TIME,
    GI_VB_GIVE_ITEM_KOKIRI_EMERALD,
    GI_VB_GIVE_ITEM_GORON_RUBY,
    GI_VB_GIVE_ITEM_ZORA_SAPPHIRE,
    GI_VB_GIVE_ITEM_LIGHT_MEDALLION,
    GI_VB_GIVE_ITEM_FOREST_MEDALLION,
    GI_VB_GIVE_ITEM_FIRE_MEDALLION,
    GI_VB_GIVE_ITEM_WATER_MEDALLION,
    GI_VB_GIVE_ITEM_SPIRIT_MEDALLION,
    GI_VB_GIVE_ITEM_SHADOW_MEDALLION,

    /*** Give Songs ***/

    GI_VB_GIVE_ITEM_ZELDAS_LULLABY,
    GI_VB_GIVE_ITEM_SARIAS_SONG,
    GI_VB_GIVE_ITEM_EPONAS_SONG,
    GI_VB_GIVE_ITEM_SUNS_SONG,
    GI_VB_GIVE_ITEM_SONG_OF_TIME,
    GI_VB_GIVE_ITEM_SONG_OF_STORMS,
    GI_VB_GIVE_ITEM_MINUET_OF_FOREST,
    GI_VB_GIVE_ITEM_BOLERO_OF_FIRE,
    GI_VB_GIVE_ITEM_SERENADE_OF_WATER,
    GI_VB_GIVE_ITEM_REQUIEM_OF_SPIRIT,
    GI_VB_GIVE_ITEM_NOCTURNE_OF_SHADOW,
    GI_VB_GIVE_ITEM_PRELUDE_OF_LIGHT,

    /*** Adult Trade ***/
    // Opt: *EnNiwLady
    GI_VB_TRADE_POCKET_CUCCO,
    // Opt: *EnHs
    GI_VB_TRADE_COJIRO,
    // Opt: *EnDs
    GI_VB_TRADE_ODD_MUSHROOM,
    // Opt: *EnKo
    GI_VB_TRADE_ODD_POTION,
    // Opt: *EnToryo
    GI_VB_TRADE_SAW,
    // Opt: *EnGo2
    GI_VB_TRADE_BROKEN_SWORD,
    // Opt: *EnKz,
    GI_VB_TRADE_PRESCRIPTION,
    // Opt: *EnMk
    GI_VB_TRADE_FROG,
    // Opt: *EnGo2
    GI_VB_TRADE_EYEDROPS,
    // Opt: *EnGo2
    GI_VB_TRADE_CLAIM_CHECK,

    GI_VB_TRADE_TIMER_ODD_MUSHROOM,
    GI_VB_TRADE_TIMER_EYEDROPS,
    GI_VB_TRADE_TIMER_FROG,
    // Opt: *EnNiwLady
    GI_VB_ANJU_SET_OBTAINED_TRADE_ITEM,

    /*** Fixes ***/
    // Vanilla condition: false
    GI_VB_FIX_SAW_SOFTLOCK,
} GIVanillaBehavior;

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
#include <thread>
#include <vector>
#include <functional>
#include <string>

#ifdef ENABLE_REMOTE_CONTROL
#include <SDL2/SDL_net.h>
#include <nlohmann/json.hpp>
#endif

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

    #ifdef ENABLE_REMOTE_CONTROL
    bool isRemoteInteractorEnabled;
    bool isRemoteInteractorConnected;

    void EnableRemoteInteractor();
    void DisableRemoteInteractor();
    void RegisterRemoteDataHandler(std::function<void(char payload[512])> method);
    void RegisterRemoteJsonHandler(std::function<void(nlohmann::json)> method);
    void RegisterRemoteConnectedHandler(std::function<void()> method);
    void RegisterRemoteDisconnectedHandler(std::function<void()> method);
    void TransmitDataToRemote(const char* payload);
    void TransmitJsonToRemote(nlohmann::json packet);
    #endif

    // Effects
    static GameInteractionEffectQueryResult CanApplyEffect(GameInteractionEffectBase* effect);
    static GameInteractionEffectQueryResult ApplyEffect(GameInteractionEffectBase* effect);
    static GameInteractionEffectQueryResult RemoveEffect(RemovableGameInteractionEffect* effect);

    // Game Hooks
    uint32_t nextHookId = 0;
    template <typename H> struct RegisteredGameHooks { inline static std::unordered_map<uint32_t, typename H::fn> functions; };
    template <typename H> struct HooksToUnregister { inline static std::vector<uint32_t> hooks; };
    template <typename H> uint32_t RegisterGameHook(typename H::fn h) {
        // Ensure hook id is unique
        while (RegisteredGameHooks<H>::functions.find(this->nextHookId) != RegisteredGameHooks<H>::functions.end()) {
            this->nextHookId++;
        }
        RegisteredGameHooks<H>::functions[this->nextHookId] = h;
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHook(uint32_t id) {
        HooksToUnregister<H>::hooks.push_back(id);
    }
    
    template <typename H, typename... Args> void ExecuteHooks(Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooks) {
            RegisteredGameHooks<H>::functions.erase(hookId);
        }
        HooksToUnregister<H>::hooks.clear();
        for (auto& hook : RegisteredGameHooks<H>::functions) {
            hook.second(std::forward<Args>(args)...);
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

    DEFINE_HOOK(OnVanillaBehavior, void(GIVanillaBehavior flag, bool* result, void* opt));

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

    DEFINE_HOOK(OnFileDropped, void(std::string filePath));
    DEFINE_HOOK(OnAssetAltChange, void());

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

    private:
    #ifdef ENABLE_REMOTE_CONTROL
        IPaddress remoteIP;
        TCPsocket remoteSocket;
        std::thread remoteThreadReceive;
        std::function<void(char payload[512])> remoteDataHandler;
        std::function<void(nlohmann::json)> remoteJsonHandler;
        std::function<void()> remoteConnectedHandler;
        std::function<void()> remoteDisconnectedHandler;

        void ReceiveFromServer();
        void HandleRemoteData(char payload[512]);
        void HandleRemoteJson(std::string payload);
    #endif
};

#endif /* __cplusplus */
#endif /* GameInteractor_h */

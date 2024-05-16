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
    VB_SHOW_TITLE_CARD,
    // Opt: *EnWonderTalk2
    VB_WONDER_TALK,
    // Opt: *ElfMsg
    VB_NAVI_TALK,
    // Vanilla condition: INFTABLE_GREETED_BY_SARIA
    VB_NOT_BE_GREETED_BY_SARIA,
    // Opt: *EnMd
    // Vanilla condition: EnMd->interactInfo.talkState == NPC_TALK_STATE_ACTION
    VB_MOVE_MIDO_IN_KOKIRI_FOREST,
    // Opt: *EnMd
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)
    VB_MIDO_CONSIDER_DEKU_TREE_DEAD,
    // Opt: *ObjDekujr
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)
    VB_DEKU_JR_CONSIDER_FOREST_TEMPLE_FINISHED,
    // Opt: *EnKo
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)
    VB_OPEN_KOKIRI_FOREST,
    // Opt: *EnOwl
    // Vanilla condition: EnOwl->actor.xzDistToPlayer < targetDist
    VB_OWL_INTERACTION,
    // Vanilla condition: EVENTCHKINF_TALON_RETURNED_FROM_CASTLE
    VB_MALON_RETURN_FROM_CASTLE,
    // Vanilla condition: CUR_UPG_VALUE(UPG_STRENGTH) <= 0
    VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD,
    /* Vanilla condition:
    ```
        LINK_IS_ADULT &&
        (gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_TEMPLE_OF_TIME) &&
        CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) &&
        CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
        !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
    ```
    */
    VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS,
    // Vanilla condition: !CHECK_QUEST_ITEM(QUEST_SONG_SARIA)
    VB_BE_ELIGIBLE_FOR_SARIAS_SONG,
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_SONG_EPONA)
    VB_MALON_ALREADY_TAUGHT_EPONAS_SONG,
    // Vanilla condition: CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) && !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER)
    VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER,
    // Vanilla condition: (!CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) && !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER)) && LINK_IS_ADULT
    VB_SHIEK_PREPARE_TO_GIVE_SERENADE_OF_WATER,
    // Vanilla condition: !EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT and EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP
    VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT,
    VB_BE_ELIGIBLE_FOR_RAINBOW_BRIDGE,
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
    VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW,
    // Opt: *EnGo2
    // Vanilla condition: CUR_CAPACITY(UPG_BOMB_BAG) >= 20 && this->waypoint > 7 && this->waypoint < 12
    VB_BE_ELIGIBLE_FOR_CHILD_ROLLING_GORON_REWARD,
    // Vanilla condition: !CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON)
    VB_BE_ELIGIBLE_FOR_GIANTS_KNIFE_PURCHASE,
    // Opt: *EnMs
    // Vanilla condition: gSaveContext.rupees >= sPrices[BEANS_BOUGHT]
    VB_BE_ELIGIBLE_FOR_MAGIC_BEANS_PURCHASE,
    // Opt: *EnItem00
    // Vanilla condition: Flags_GetCollectible(play, this->collectibleFlag)
    VB_ITEM00_DESPAWN,
    // Opt: *EnTk
    // Vanilla condition: gSaveContext.dayTime <= 0xC000 || gSaveContext.dayTime >= 0xE000 || LINK_IS_ADULT || play->sceneNum != SCENE_GRAVEYARD
    VB_DAMPE_IN_GRAVEYARD_DESPAWN,
    // Opt: *EnTk
    // Vanilla condition: this->validDigHere == 1
    VB_BE_VALID_GRAVEDIGGING_SPOT,
    // Opt: *EnTk
    // Vanilla condition: this->currentReward == 3
    VB_BE_DAMPE_GRAVEDIGGING_GRAND_PRIZE,
    // Opt: *EnTk
    // Vanilla condition: !Flags_GetItemGetInf(ITEMGETINF_1C)
    VB_DAMPE_GRAVEDIGGING_GRAND_PRIZE_BE_HEART_PIECE,
    // Opt: *EnShopnuts
    /* Vanilla Condition:
    ```
        ((this->actor.params == 0x0002) && (Flags_GetItemGetInf(ITEMGETINF_0B))) ||
        ((this->actor.params == 0x0009) && (Flags_GetInfTable(INFTABLE_192))) ||
        ((this->actor.params == 0x000A) && (Flags_GetInfTable(INFTABLE_193)))
    ```
    */
    VB_BUSINESS_SCRUB_DESPAWN,
    // Opt: *EnCow
    // Vanilla condition: play->sceneNum == SCENE_LINKS_HOUSE && (!LINK_IS_ADULT || !Flags_GetEventChkInf(EVENTCHKINF_WON_COW_IN_MALONS_RACE))
    VB_DESPAWN_HORSE_RACE_COW,
    // Opt: *EnHs
    // Vanilla condition: Flags_GetItemGetInf(ITEMGETINF_30)
    VB_DESPAWN_GROG,
    // Opt: *EnKo
    // Vanilla condition: (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_ODD_POTION) ? true : false;
    VB_SPAWN_LW_FADO,
    // Opt: *EnMk
    VB_PLAY_EYEDROP_CREATION_ANIM,
    // Opt: *EnDs
    VB_PLAY_ODD_POTION_ANIM,
    // Opt: *EnMk
    // Vanilla condition: INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_EYEDROPS
    VB_USE_EYEDROP_DIALOGUE,
    // Opt: *EnMk
    // Vanilla condition: Flags_GetItemGetInf(ITEMGETINF_30)
    VB_OFFER_BLUE_POTION,
    // Vanilla condition: Inventory_HasEmptyBottle() == 0
    VB_NEED_BOTTLE_FOR_GRANNYS_ITEM,
    // Opt: *EnNiwLady
    VB_SET_CUCCO_COUNT,
    // Opt: *EnKz
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)
    VB_KING_ZORA_THANK_CHILD,
    // Opt: *EnKz
    // Vanilla condition: this->actor.textId == 0x401A
    VB_BE_ABLE_TO_EXCHANGE_RUTOS_LETTER,
    // Opt: *EnKz
    // Vanilla condition: Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)
    VB_KING_ZORA_BE_MOVED,
    // Vanilla condition: gSaveState.bgsFlag
    VB_BIGGORON_CONSIDER_TRADE_COMPLETE,
    // Vanilla condition: gSaveState.bgsFlag
    VB_BIGGORON_CONSIDER_SWORD_COLLECTED,
    // Vanilla condition: Environment_GetBgsDayCount() >= 3
    VB_BIGGORON_CONSIDER_SWORD_FORGED,
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)
    VB_GORONS_CONSIDER_FIRE_TEMPLE_FINISHED,
    // Vanilla condition: CHECK_QUEST_ITEM(QUEST_GORON_RUBY)
    VB_GORONS_CONSIDER_DODONGOS_CAVERN_FINISHED,
    // Opt: *uint16_t
    // Vanilla condition: false
    VB_OVERRIDE_LINK_THE_GORON_DIALOGUE,
    // Vanilla condition: CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)
    VB_GORONS_CONSIDER_TUNIC_COLLECTED,
    // Opt: *EnSyatekiMan
    // Vanilla condition: (this->getItemId == GI_QUIVER_40) || (this->getItemId == GI_QUIVER_50)
    VB_BE_ELIGIBLE_FOR_ADULT_SHOOTING_GAME_REWARD,
    // Opt: *EnOkarinaTag
    // Vanilla condition: !Flags_GetEventChkInf(EVENTCHKINF_OPENED_THE_DOOR_OF_TIME)
    VB_BE_ELIGIBLE_TO_OPEN_DOT,
    // Opt: *BgDyYoseizo
    // Vanilla condition: see soh/src/overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.c
    VB_BE_ELIGIBLE_FOR_GREAT_FAIRY_REWARD,
    // Vanilla condition: see CheckCarpentersFreed in z_en_ge1 and z_en_ge2
    VB_GERUDOS_BE_FRIENDLY,
    // Vanilla condition: switch
    VB_GTG_GATE_BE_OPEN,

    /*** Play Cutscenes ***/

    VB_PLAY_TRANSITION_CS,
    // Opt: *EventChkInf flag
    VB_PLAY_ENTRANCE_CS,
    // Opt: *cutsceneId
    VB_PLAY_ONEPOINT_CS,
    // Opt: *actor
    VB_PLAY_ONEPOINT_ACTOR_CS,
    // Opt: *BgTreemouth
    VB_PLAY_DEKU_TREE_INTRO_CS,
    // Vanilla condition: !EventChkInf except for spirit & shadow temple which are !medallion, and Jabu which always is true
    VB_PLAY_BLUE_WARP_CS,
    VB_PLAY_DARUNIAS_JOY_CS,
    VB_PLAY_SHIEK_BLOCK_MASTER_SWORD_CS,
    // Vanilla condition: !EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL
    VB_PLAY_PULL_MASTER_SWORD_CS,
    VB_PLAY_DROP_FISH_FOR_JABU_CS,
    // Vanilla condition: player->getItemId == GI_GAUNTLETS_SILVER
    VB_PLAY_NABOORU_CAPTURED_CS,
    VB_PLAY_ZELDAS_LULLABY_CS,
    // Opt: *EnSa
    VB_PLAY_SARIAS_SONG_CS,
    VB_PLAY_PRELUDE_OF_LIGHT_CS,
    VB_PLAY_MINUET_OF_FOREST_CS,
    VB_PLAY_BOLERO_OF_FIRE_CS,
    VB_PLAY_SERENADE_OF_WATER_CS,
    VB_PLAY_EYEDROPS_CS,
    // Opt: *EnOkarinaTag
    VB_PLAY_DRAIN_WELL_CS,
    // Opt: *EnOkarinaTag
    // Vanilla condition: !CHECK_QUEST_ITEM(QUEST_SONG_SUN)
    VB_PLAY_SUNS_SONG_CS,
    // Opt: *EnOkarinaTag
    VB_PLAY_ROYAL_FAMILY_TOMB_CS,
    VB_PLAY_ROYAL_FAMILY_TOMB_EXPLODE,
    // Opt: *EnOkarinaTag
    VB_PLAY_DOOR_OF_TIME_CS,
    VB_PLAY_RAINBOW_BRIDGE_CS,
    // Opt: *EnBox
    VB_PLAY_SLOW_CHEST_CS,

    /*** Give Items ***/

    // Opt: *EnBox
    VB_GIVE_ITEM_FROM_CHEST,
    VB_GIVE_ITEM_FROM_BLUE_WARP,
    // Opt: *EnItem00
    VB_GIVE_ITEM_FROM_ITEM_00,
    // Opt: *EnSi
    VB_GIVE_ITEM_SKULL_TOKEN,
    // Opt: *EnCow
    VB_GIVE_ITEM_FROM_COW,
    // Opt: *EnDns
    VB_GIVE_ITEM_FROM_BUSINESS_SCRUB,
    // Opt: *EnMk
    VB_GIVE_ITEM_FROM_LAB_DIVE,
    // Opt: *EnDs
    VB_GIVE_ITEM_FROM_GRANNYS_SHOP,
    // Opt: *EnNiwLady
    VB_GIVE_ITEM_FROM_ANJU_AS_CHILD,
    // Opt: *EnNiwLady
    VB_GIVE_ITEM_FROM_ANJU_AS_ADULT,
    // Opt: *EnKz
    // Vanilla condition: !CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA)
    VB_GIVE_ITEM_FROM_THAWING_KING_ZORA,
    // Opt: *EnGo2
    VB_GIVE_ITEM_FROM_GORON,
    // Opt: *EnJs
    VB_GIVE_ITEM_FROM_CARPET_SALESMAN,
    // Opt: *EnGm
    VB_GIVE_ITEM_FROM_MEDIGORON,
    // Opt: *EnMs
    VB_GIVE_ITEM_FROM_MAGIC_BEAN_SALESMAN,
    // Opt: *EnFr
    VB_GIVE_ITEM_FROM_FROGS,
    // Opt: *EnSkj
    VB_GIVE_ITEM_FROM_OCARINA_MEMORY_GAME,
    // Opt: *EnSkj
    VB_GIVE_ITEM_FROM_SKULL_KID_SARIAS_SONG,
    VB_GIVE_ITEM_FROM_MAN_ON_ROOF,
    // Opt: *EnSyatekiMan
    VB_GIVE_ITEM_FROM_SHOOTING_GALLERY,
    // Opt: *EnExItem
    VB_GIVE_ITEM_FROM_TARGET_IN_WOODS,
    // Opt: *EnTa
    VB_GIVE_ITEM_FROM_TALONS_CHICKENS,
    // Opt: *EnDivingGame
    VB_GIVE_ITEM_FROM_DIVING_MINIGAME,
    // Opt: *EnGe1
    VB_GIVE_ITEM_FROM_HORSEBACK_ARCHERY,
    // Opt: *EnSth
    VB_GIVE_ITEM_FROM_SKULLTULA_REWARD,
    // Opt: *EnHy
    VB_GIVE_ITEM_FROM_LOST_DOG,
    // Opt: *EnBomBowlPit
    VB_GIVE_ITEM_FROM_BOMBCHU_BOWLING,

    VB_GIVE_ITEM_GERUDO_MEMBERSHIP_CARD,

    VB_GIVE_ITEM_FAIRY_OCARINA,
    VB_GIVE_ITEM_WEIRD_EGG,
    VB_GIVE_ITEM_LIGHT_ARROW,
    VB_GIVE_ITEM_STRENGTH_1,
    VB_GIVE_ITEM_ZELDAS_LETTER,
    VB_GIVE_ITEM_MASTER_SWORD,
    VB_GIVE_ITEM_OCARINA_OF_TIME,
    VB_GIVE_ITEM_KOKIRI_EMERALD,
    VB_GIVE_ITEM_GORON_RUBY,
    VB_GIVE_ITEM_ZORA_SAPPHIRE,
    VB_GIVE_ITEM_LIGHT_MEDALLION,
    VB_GIVE_ITEM_FOREST_MEDALLION,
    VB_GIVE_ITEM_FIRE_MEDALLION,
    VB_GIVE_ITEM_WATER_MEDALLION,
    VB_GIVE_ITEM_SPIRIT_MEDALLION,
    VB_GIVE_ITEM_SHADOW_MEDALLION,

    /*** Give Songs ***/

    VB_GIVE_ITEM_ZELDAS_LULLABY,
    VB_GIVE_ITEM_SARIAS_SONG,
    VB_GIVE_ITEM_EPONAS_SONG,
    VB_GIVE_ITEM_SUNS_SONG,
    VB_GIVE_ITEM_SONG_OF_TIME,
    VB_GIVE_ITEM_SONG_OF_STORMS,
    VB_GIVE_ITEM_MINUET_OF_FOREST,
    VB_GIVE_ITEM_BOLERO_OF_FIRE,
    VB_GIVE_ITEM_SERENADE_OF_WATER,
    VB_GIVE_ITEM_REQUIEM_OF_SPIRIT,
    VB_GIVE_ITEM_NOCTURNE_OF_SHADOW,
    VB_GIVE_ITEM_PRELUDE_OF_LIGHT,

    /*** Adult Trade ***/
    // Opt: *EnNiwLady
    VB_TRADE_POCKET_CUCCO,
    // Opt: *EnHs
    VB_TRADE_COJIRO,
    // Opt: *EnDs
    VB_TRADE_ODD_MUSHROOM,
    // Opt: *EnKo
    VB_TRADE_ODD_POTION,
    // Opt: *EnToryo
    VB_TRADE_SAW,
    // Opt: *EnKz,
    VB_TRADE_PRESCRIPTION,
    // Opt: *EnMk
    VB_TRADE_FROG,

    VB_TRADE_TIMER_ODD_MUSHROOM,
    VB_TRADE_TIMER_EYEDROPS,
    VB_TRADE_TIMER_FROG,
    // Opt: *EnNiwLady
    VB_ANJU_SET_OBTAINED_TRADE_ITEM,

    /*** Fixes ***/
    // Vanilla condition: false
    VB_FIX_SAW_SOFTLOCK,

    /*** Cheats? ***/
    VB_DEKU_STICK_BE_ON_FIRE,
    VB_DEKU_STICK_BREAK,
    VB_DEKU_STICK_BURN_DOWN,
    VB_DEKU_STICK_BURN_OUT,
    VB_DEKU_UPDATE_BURNING_DEKU_STICK,

    /*** Quick Boss Deaths ***/
    // Vanilla condition: true
    VB_PHANTOM_GANON_DEATH_SCENE,
    VB_NABOORU_KNUCKLE_DEATH_SCENE,
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
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

#ifdef ENABLE_REMOTE_CONTROL
#include <SDL2/SDL_net.h>
#include <nlohmann/json.hpp>
#endif

typedef uint32_t HOOK_ID;

#define DEFINE_HOOK(name, args)                  \
    struct name {                                \
        typedef std::function<void args> fn;     \
        typedef std::function<bool args> filter; \
    }

#define REGISTER_VB_SHOULD(flag, body) \
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnVanillaBehavior>(flag, [](GIVanillaBehavior _, bool* should, void* opt) body)

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
    HOOK_ID nextHookId = 1;
    template <typename H> struct RegisteredGameHooks {
        inline static std::unordered_map<HOOK_ID, typename H::fn> functions;
        inline static std::unordered_map<int32_t, std::unordered_map<HOOK_ID, typename H::fn>> functionsForID;
        inline static std::unordered_map<uintptr_t, std::unordered_map<HOOK_ID, typename H::fn>> functionsForPtr;
        inline static std::unordered_map<HOOK_ID, std::pair<typename H::filter, typename H::fn>> functionsForFilter;
    };
    template <typename H> struct HooksToUnregister {
        inline static std::vector<HOOK_ID> hooks;
        inline static std::vector<HOOK_ID> hooksForID;
        inline static std::vector<HOOK_ID> hooksForPtr;
        inline static std::vector<HOOK_ID> hooksForFilter;
    };

    // General Hooks
    template <typename H> HOOK_ID RegisterGameHook(typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functions.find(this->nextHookId) != RegisteredGameHooks<H>::functions.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functions[this->nextHookId] = h;
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHook(HOOK_ID hookId) {
        if (hookId == 0) return;
        HooksToUnregister<H>::hooks.push_back(hookId);
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

    // ID based Hooks
    template <typename H> HOOK_ID RegisterGameHookForID(int32_t id, typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForID[id].find(this->nextHookId) != RegisteredGameHooks<H>::functionsForID[id].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForID[id][this->nextHookId] = h;
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
                } else {
                    ++it;
                }
            }
        }
        for (auto& hook : RegisteredGameHooks<H>::functionsForID[id]) {
            hook.second(std::forward<Args>(args)...);
        }
    }

    // PTR based Hooks
    template <typename H> HOOK_ID RegisterGameHookForPtr(uintptr_t ptr, typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForPtr[ptr].find(this->nextHookId) != RegisteredGameHooks<H>::functionsForPtr[ptr].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForPtr[ptr][this->nextHookId] = h;
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
                } else {
                    ++it;
                }
            }
        }
        for (auto& hook : RegisteredGameHooks<H>::functionsForPtr[ptr]) {
            hook.second(std::forward<Args>(args)...);
        }
    }

    // Filter based Hooks
    template <typename H> HOOK_ID RegisterGameHookForFilter(typename H::filter f, typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX) this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForFilter.find(this->nextHookId) != RegisteredGameHooks<H>::functionsForFilter.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForFilter[this->nextHookId] = std::make_pair(f, h);
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHookForFilter(HOOK_ID hookId) {
        if (hookId == 0) return;
        HooksToUnregister<H>::hooksForFilter.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooksForFilter(Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooksForFilter) {
            RegisteredGameHooks<H>::functionsForFilter.erase(hookId);
        }
        HooksToUnregister<H>::hooksForFilter.clear();
        for (auto& hook : RegisteredGameHooks<H>::functionsForFilter) {
            if (hook.second.first(std::forward<Args>(args)...)) {
                hook.second.second(std::forward<Args>(args)...);
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

    DEFINE_HOOK(OnLoadGame, (int32_t fileNum));
    DEFINE_HOOK(OnExitGame, (int32_t fileNum));
    DEFINE_HOOK(OnGameFrameUpdate, ());
    DEFINE_HOOK(OnItemReceive, (GetItemEntry itemEntry));
    DEFINE_HOOK(OnSaleEnd, (GetItemEntry itemEntry));
    DEFINE_HOOK(OnTransitionEnd, (int16_t sceneNum));
    DEFINE_HOOK(OnSceneInit, (int16_t sceneNum));
    DEFINE_HOOK(OnSceneFlagSet, (int16_t sceneNum, int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnSceneFlagUnset, (int16_t sceneNum, int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnFlagSet, (int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnFlagUnset, (int16_t flagType, int16_t flag));
    DEFINE_HOOK(OnSceneSpawnActors, ());
    DEFINE_HOOK(OnPlayerUpdate, ());
    DEFINE_HOOK(OnOcarinaSongAction, ());
    DEFINE_HOOK(OnShopSlotChange, (uint8_t cursorIndex, int16_t price));
    DEFINE_HOOK(OnActorInit, (void* actor));
    DEFINE_HOOK(OnActorUpdate, (void* actor));
    DEFINE_HOOK(OnActorKill, (void* actor));
    DEFINE_HOOK(OnEnemyDefeat, (void* actor));
    DEFINE_HOOK(OnPlayerBonk, ());
    DEFINE_HOOK(OnPlayDestroy, ());
    DEFINE_HOOK(OnPlayDrawEnd, ());

    DEFINE_HOOK(OnVanillaBehavior, (GIVanillaBehavior flag, bool* result, void* opt));

    DEFINE_HOOK(OnSaveFile, (int32_t fileNum));
    DEFINE_HOOK(OnLoadFile, (int32_t fileNum));
    DEFINE_HOOK(OnDeleteFile, (int32_t fileNum));
    
    DEFINE_HOOK(OnDialogMessage, ());
    DEFINE_HOOK(OnPresentTitleCard, ());
    DEFINE_HOOK(OnInterfaceUpdate, ());
    DEFINE_HOOK(OnKaleidoscopeUpdate, (int16_t inDungeonScene));
    
    DEFINE_HOOK(OnPresentFileSelect, ());
    DEFINE_HOOK(OnUpdateFileSelectSelection, (uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileSelectConfirmationSelection, (uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileCopySelection, (uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileCopyConfirmationSelection, (uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileEraseSelection, (uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileEraseConfirmationSelection, (uint16_t optionIndex));
    DEFINE_HOOK(OnUpdateFileAudioSelection, (uint8_t optionIndex));
    DEFINE_HOOK(OnUpdateFileTargetSelection, (uint8_t optionIndex));
    DEFINE_HOOK(OnUpdateFileLanguageSelection, (uint8_t optionIndex));
    DEFINE_HOOK(OnUpdateFileQuestSelection, (uint8_t questIndex));
    DEFINE_HOOK(OnUpdateFileBossRushOptionSelection, (uint8_t optionIndex, uint8_t optionValue));
    DEFINE_HOOK(OnUpdateFileNameSelection, (int16_t charCode));
    
    DEFINE_HOOK(OnSetGameLanguage, ());

    DEFINE_HOOK(OnFileDropped, (std::string filePath));
    DEFINE_HOOK(OnAssetAltChange, ());
    DEFINE_HOOK(OnKaleidoUpdate, ());

    // Helpers
    static bool IsSaveLoaded(bool allowDbgSave = false);
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

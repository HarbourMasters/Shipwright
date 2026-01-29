#pragma once

#include <stdint.h>
#include "z64item.h"

#define MAX_TRICK_NAME_SIZE 50

#define TWO_ACTOR_PARAMS(a, b) ((((a) & 0xFFFF) << 16) | ((b) & 0xFFFF))

// This should probably go in a less rando-specific location
// but the best location will probably be in the modding engine
// which doesn't exist yet.
typedef enum {
#define DEFINE_ModIndex(enum) enum,
#include "randomizerEnums.h"
} ModIndex;
typedef enum {
#define DEFINE_TableIndex(enum) enum,
#include "randomizerEnums.h"
} TableIndex;

typedef struct {
    char tex[512];
    uint16_t width;
    uint16_t height;
    uint8_t im_fmt;
    uint8_t im_siz;
    uint8_t id;
} Sprite;

typedef enum {
#define DEFINE_HintType(enum) enum,
#include "randomizerEnums.h"
} HintType;

typedef enum {
#define DEFINE_LogicVal(enum) enum,
#include "randomizerEnums.h"
} LogicVal;

#define DEFINE_RAND_INF(enum) enum,

typedef enum {
#include "randomizer_inf.h"
    RAND_INF_MAX,
} RandomizerInf;

#undef DEFINE_RAND_INF

typedef enum {
#define DEFINE_RandomizerArea(enum) enum,
#include "randomizerEnums.h"
} RandomizerArea;

typedef enum {
#define DEFINE_TrialKey(enum) enum,
#include "randomizerEnums.h"
} TrialKey;

// Check types based on main settings
typedef enum {
#define DEFINE_RandomizerCheckType(enum) enum,
#include "randomizerEnums.h"
} RandomizerCheckType;

typedef enum {
#define DEFINE_RandomizerCheckQuest(enum) enum,
#include "randomizerEnums.h"
} RandomizerCheckQuest;

typedef enum {
#define DEFINE_RandomizerCheckArea(enum) enum,
#include "randomizerEnums.h"
} RandomizerCheckArea;

// Check tracker check visibility categories
typedef enum {
#define DEFINE_RandomizerCheckStatus(enum) enum,
#include "randomizerEnums.h"
} RandomizerCheckStatus;

typedef enum {
#define DEFINE_RandomizerRegion(enum) enum,
#include "randomizerEnums.h"
} RandomizerRegion;

typedef enum {
#define DEFINE_RandomizerCheck(enum) enum,
#include "randomizerEnums.h"
} RandomizerCheck;

// Randomizer tricks and glitches (beta) based on knowledge of what is achievable in Ship of Harkinian randomizer,
// excludes Item manipulation (e.g. RBA/GIM/SRM) and Wrong Warping and Ganon BK Skip
// TODO Fill and alphabetize (area based tricks must have area in name and come alphetized after general tricks)
// TODO test commented out tricks to see if consistent with console on SoH
// TODO add area specific tricks
typedef enum {
#define DEFINE_RandomizerTrick(enum) enum,
#include "randomizerEnums.h"
} RandomizerTrick;

// based on https://github.com/TestRunnerSRL/OoT-Randomizer/blob/e337d7f603b91a6bacb618fb32cc7fd70ed9ffca/ItemList.py
typedef enum {
#define DEFINE_RandomizerGet(enum) enum,
#include "randomizerEnums.h"
} RandomizerGet;

typedef enum {
#define DEFINE_RandomizerHint(enum) enum,
#include "randomizerEnums.h"
} RandomizerHint;

typedef enum {
#define DEFINE_RandomizerHintTextKey(enum) enum,
#include "randomizerEnums.h"
} RandomizerHintTextKey;

typedef struct {
    RandomizerGet rgID;
    RandomizerGet fakeRgID;
    char trickName[MAX_TRICK_NAME_SIZE];
} RandomizerGetData;

typedef enum {
#define DEFINE_RandomizerSettingGroupKey(enum) enum,
#include "randomizerEnums.h"
} RandomizerSettingGroupKey;

typedef enum {
#define DEFINE_RandomizerSettingKey(enum) enum,
#include "randomizerEnums.h"
} RandomizerSettingKey;

// Generic Settings (any binary option can use this)
//  off/on
typedef enum {
#define DEFINE_RandoOptionGenericOffOn(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGenericOffOn;

// no/yes
typedef enum {
#define DEFINE_RandoOptionGenericNoYes(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGenericNoYes;

// don't skip/skip
typedef enum {
#define DEFINE_RandoOptionGenericSkip(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGenericSkip;

// Closed Forest settings (On, Deku Only, Off)
typedef enum {
#define DEFINE_RandoOptionForest(enum) enum,
#include "randomizerEnums.h"
} RandoOptionForest;

// Door of Time settings (closed, song only, open)
typedef enum {
#define DEFINE_RandoOptionDoorOfTime(enum) enum,
#include "randomizerEnums.h"
} RandoOptionDoorOfTime;

// Zora's Fountain settings (closed, closed as child, open)
typedef enum {
#define DEFINE_RandoOptionZorasFountain(enum) enum,
#include "randomizerEnums.h"
} RandoOptionZorasFountain;

// Sleeping Waterfall settings (closed, open)
typedef enum {
#define DEFINE_RandoOptionSleepingWaterfall(enum) enum,
#include "randomizerEnums.h"
} RandoOptionSleepingWaterfall;

// Jabu-Jabu settings (closed, open)
typedef enum {
#define DEFINE_RandoOptionJabu(enum) enum,
#include "randomizerEnums.h"
} RandoOptionJabu;

// Starting Age settings (child, adult, random)
typedef enum {
#define DEFINE_RandoOptionStartingAge(enum) enum,
#include "randomizerEnums.h"
} RandoOptionStartingAge;

// Fortress Carpenters settings (normal, fast, free)
typedef enum {
#define DEFINE_RandoOptionGerudoFortress(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGerudoFortress;

// Kakariko Gate settings (closed/open)
typedef enum {
#define DEFINE_RandoOptionKakarikoGate(enum) enum,
#include "randomizerEnums.h"
} RandoOptionKakarikoGate;

// Rainbow Bridge settings (vanilla, always open, stones, medallions, dungeon rewards, dungeons, tokens)
typedef enum {
#define DEFINE_RandoOptionRainbowBridge(enum) enum,
#include "randomizerEnums.h"
} RandoOptionRainbowBridge;

// Bridge Reward Options settings (Standard rewards, Greg as reward, Greg as wildcard)
typedef enum {
#define DEFINE_RandoOptionBridgeRewards(enum) enum,
#include "randomizerEnums.h"
} RandoOptionBridgeRewards;

// Shopsanity settings (off, specific count, random)
typedef enum {
#define DEFINE_RandoOptionShopsanity(enum) enum,
#include "randomizerEnums.h"
} RandoOptionShopsanity;

// Shopsanity count settings (0-7 items)
typedef enum {
#define DEFINE_RandoOptionShopsanityCount(enum) enum,
#include "randomizerEnums.h"
} RandoOptionShopsanityCount;

// Shopsanity price ranges
typedef enum {
#define DEFINE_RandoOptionPrices(enum) enum,
#include "randomizerEnums.h"
} RandoOptionPrices;

// Scrubsanity settings (off, affordable, expensive, random)
typedef enum {
#define DEFINE_RandoOptionScrubsanity(enum) enum,
#include "randomizerEnums.h"
} RandoOptionScrubsanity;

// Ammo drop settings (on, "on+bombchu", off)
typedef enum {
#define DEFINE_RandoOptionAmmoDrops(enum) enum,
#include "randomizerEnums.h"
} RandoOptionAmmoDrops;

typedef enum {
#define DEFINE_RandoOptionBombchuBag(enum) enum,
#include "randomizerEnums.h"
} RandoOptionBombchuBag;

typedef enum {
#define DEFINE_RandoOptionBossSouls(enum) enum,
#include "randomizerEnums.h"
} RandoOptionBossSouls;

// Fishsanity settings (off, loach only, pond only, grottos only, both)
typedef enum {
#define DEFINE_RandoOptionsFishsanity(enum) enum,
#include "randomizerEnums.h"
} RandoOptionsFishsanity;

// Infinite Upgrades settings (off, progressive, condensed progressive)
typedef enum {
#define DEFINE_RandoOptionInfiniteUpgrades(enum) enum,
#include "randomizerEnums.h"
} RandoOptionInfiniteUpgrades;

// Any Dungeon Item (start with, vanilla, own dungeon, any dungeon,
// overworld, anywhere)
typedef enum {
#define DEFINE_RandoOptionDungeonItemLocation(enum) enum,
#include "randomizerEnums.h"
} RandoOptionDungeonItemLocation;

// Dungeon reward settings
typedef enum {
#define DEFINE_RandoOptionDungeonRewards(enum) enum,
#include "randomizerEnums.h"
} RandoOptionDungeonRewards;

// Keyring Settings
typedef enum {
#define DEFINE_RandoOptionKeyrings(enum) enum,
#include "randomizerEnums.h"
} RandoOptionKeyrings;

typedef enum {
#define DEFINE_RandoOptionKeyringForDungeon(enum) enum,
#include "randomizerEnums.h"
} RandoOptionKeyringForDungeon;

// Ganon's Boss Key Settings (vanilla, own dungeon, start with,
// overworld, anywhere, 100 GS reward)
typedef enum {
#define DEFINE_RandoOptionGanonsBossKey(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGanonsBossKey;

typedef enum {
#define DEFINE_RandoOptionLACSCondition(enum) enum,
#include "randomizerEnums.h"
} RandoOptionLACSCondition;

// LACS Reward Options settings (Standard rewards, Greg as reward, Greg as wildcard)
typedef enum {
#define DEFINE_RandoOptionLACSRewards(enum) enum,
#include "randomizerEnums.h"
} RandoOptionLACSRewards;

// Ganon's Trials
typedef enum {
#define DEFINE_RandoOptionGanonsTrials(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGanonsTrials;

// Shuffle Dungeon Entrance Settings (Off, on, plus ganon)
typedef enum {
#define DEFINE_RandoOptionDungeonEntranceShuffle(enum) enum,
#include "randomizerEnums.h"
} RandoOptionDungeonEntranceShuffle;

// Shuffle Boss Room Entrance Settings (Off, Age Restricted, Full)
typedef enum {
#define DEFINE_RandoOptionBossRoomEntranceShuffle(enum) enum,
#include "randomizerEnums.h"
} RandoOptionBossRoomEntranceShuffle;

// Shuffle Interior Entrance Settings (Off, simple, all)
typedef enum {
#define DEFINE_RandoOptionInteriorEntranceShuffle(enum) enum,
#include "randomizerEnums.h"
} RandoOptionInteriorEntranceShuffle;

// Song shuffle Settings (Song locations, Dungeon rewards, anywhere)
typedef enum {
#define DEFINE_RandoOptionSongShuffle(enum) enum,
#include "randomizerEnums.h"
} RandoOptionSongShuffle;

// Shuffle Merchants Settings (Off, Beans Only, All but Beans, All)
typedef enum {
#define DEFINE_RandoOptionShuffleMerchants(enum) enum,
#include "randomizerEnums.h"
} RandoOptionShuffleMerchants;

// Starting Ocarina Settings (off, fairy)
typedef enum {
#define DEFINE_RandoOptionStartingOcarina(enum) enum,
#include "randomizerEnums.h"
} RandoOptionStartingOcarina;

// Mask Quest Settings (vanilla, completed, shuffle)
typedef enum {
#define DEFINE_RandoOptionMaskQuest(enum) enum,
#include "randomizerEnums.h"
} RandoOptionMaskQuest;

// Item Pool Settings
typedef enum {
#define DEFINE_RandoOptionItemPool(enum) enum,
#include "randomizerEnums.h"
} RandoOptionItemPool;

// Ice Trap Settings
typedef enum {
#define DEFINE_RandoOptionIceTraps(enum) enum,
#include "randomizerEnums.h"
} RandoOptionIceTraps;

// Gossip Stone Hint Settings (no hints, needs nothing,
// needs mask of truth, needs stone of agony)
typedef enum {
#define DEFINE_RandoOptionGossipStones(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGossipStones;

// Hint Clarity Settings (obscure, ambiguous, clear)
typedef enum {
#define DEFINE_RandoOptionHintClarity(enum) enum,
#include "randomizerEnums.h"
} RandoOptionHintClarity;

// Hint Distribution Settings (useless, balanced, strong, very strong)
typedef enum {
#define DEFINE_RandoOptionHintDistribution(enum) enum,
#include "randomizerEnums.h"
} RandoOptionHintDistribution;

// Gerudo Fortress Key Settings (vanilla, any dungeon, overworld,
// anywhere)
typedef enum {
#define DEFINE_RandoOptionGerudoKeys(enum) enum,
#include "randomizerEnums.h"
} RandoOptionGerudoKeys;

// Tokensanity settings (off, dungeons, overworld, all)
typedef enum {
#define DEFINE_RandoOptionTokensanity(enum) enum,
#include "randomizerEnums.h"
} RandoOptionTokensanity;

// Freestanding Hearts/Rupees settings (off, dungeons, overworld, all)
typedef enum {
#define DEFINE_RandoOptionFreestanding(enum) enum,
#include "randomizerEnums.h"
} RandoOptionFreestanding;

// Shuffle Pots settings (off, dungeons, overworld, all)
typedef enum {
#define DEFINE_RandoOptionShufflePots(enum) enum,
#include "randomizerEnums.h"
} RandoOptionShufflePots;

// Shuffle Grass settings (off, dungeons, overworld, all)
typedef enum {
#define DEFINE_RandoOptionShuffleGrass(enum) enum,
#include "randomizerEnums.h"
} RandoOptionShuffleGrass;

// Shuffle Crates settings (off, dungeons, overworld, all)
typedef enum {
#define DEFINE_RandoOptionShuffleCrates(enum) enum,
#include "randomizerEnums.h"
} RandoOptionShuffleCrates;

// Link's Pocket Settings (dungeon reward, advancement, anything, nothing)
typedef enum {
#define DEFINE_RandoOptionLinksPocket(enum) enum,
#include "randomizerEnums.h"
} RandoOptionLinksPocket;

// Logic (glitchless/no logic)
typedef enum {
#define DEFINE_RandoOptionLogic(enum) enum,
#include "randomizerEnums.h"
} RandoOptionLogic;

// Damage Multiplier
typedef enum {
#define DEFINE_RandoOptionDamageMultiplier(enum) enum,
#include "randomizerEnums.h"
} RandoOptionDamageMultiplier;

// MQ Dungeons
typedef enum {
#define DEFINE_RandoOptionMQDungeons(enum) enum,
#include "randomizerEnums.h"
} RandoOptionMQDungeons;

// Triforce Hunt settings (off, win, Ganon's Boss Key)
typedef enum {
#define DEFINE_RandoOptionTriforceHunt(enum) enum,
#include "randomizerEnums.h"
} RandoOptionTriforceHunt;

typedef enum {
#define DEFINE_RandoOptionLocationInclusion(enum) enum,
#include "randomizerEnums.h"
} RandoOptionLocationInclusion;

typedef enum {
#define DEFINE_RandoOptionChestGame(enum) enum,
#include "randomizerEnums.h"
} RandoOptionChestGame;

typedef enum {
#define DEFINE_RandoOptionMQSet(enum) enum,
#include "randomizerEnums.h"
} RandoOptionMQSet;

typedef enum {
#define DEFINE_ItemObtainability(enum) enum,
#include "randomizerEnums.h"
} ItemObtainability;

typedef struct CheckIdentity {
    RandomizerInf randomizerInf;
    RandomizerCheck randomizerCheck;
} CheckIdentity;

typedef struct ScrubIdentity {
    CheckIdentity identity;
    GetItemID getItemId;
    int32_t itemPrice;
} ScrubIdentity;

typedef struct ShopItemIdentity {
    CheckIdentity identity;
    GetItemID ogItemId;
    int32_t enGirlAShopItem;
    int32_t itemPrice;
} ShopItemIdentity;

typedef enum {
#define DEFINE_TrackerWindowType(enum) enum,
#include "randomizerEnums.h"
} TrackerWindowType;

typedef enum {
#define DEFINE_TrackerDisplayType(enum) enum,
#include "randomizerEnums.h"
} TrackerDisplayType;

typedef enum {
#define DEFINE_TrackerComboButton(enum) enum,
#include "randomizerEnums.h"
} TrackerComboButton;

typedef enum {
#define DEFINE_TriforceHuntMessages(enum) enum,
#include "randomizerEnums.h"
} TriforceHuntMessages;

typedef enum {
#define DEFINE_RandomizerEnemy(enum) enum,
#include "randomizerEnums.h"
} RandomizerEnemy;

// RANDOTODO compare child long jumpslash range with adult short
typedef enum {
#define DEFINE_EnemyDistance(enum) enum,
#include "randomizerEnums.h"
} EnemyDistance;

typedef enum {
#define DEFINE_RandoWaterLevel(enum) enum,
#include "randomizerEnums.h"
} RandoWaterLevel;

#define ENTRANCE_GROTTO_LOAD_START 0x0700
#define ENTRANCE_GROTTO_EXIT_START 0x0800

#define ENTRANCE_GROTTO_LOAD(index) ENTRANCE_GROTTO_LOAD_START + index
#define ENTRANCE_GROTTO_EXIT(index) ENTRANCE_GROTTO_EXIT_START + index

typedef enum {
#define DEFINE_GrottoEntranceOffsets(enum) enum,
#include "randomizerEnums.h"
} GrottoEntranceOffsets;
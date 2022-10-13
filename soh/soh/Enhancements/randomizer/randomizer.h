#pragma once

#include <unordered_map>
#include <string>
#include "../../../include/ultra64.h"
#include "../../../include/z64item.h"
#include <memory>
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

#define NUM_NAVI_MESSAGES 19
#define NUM_ICE_TRAP_MESSAGES 23

typedef enum RandoOption {
  //Generic Settings (any binary option can use this)
  //(off/on, no/yes, don't skip/skip)
  RO_GENERIC_OFF,
  RO_GENERIC_ON,
  RO_GENERIC_NO = 0,
  RO_GENERIC_YES,
  RO_GENERIC_DONT_SKIP = 0,
  RO_GENERIC_SKIP,

  //Forest settings (closed, closed deku, open)
  RO_FOREST_CLOSED = 0,
  RO_FOREST_CLOSED_DEKU,
  RO_FOREST_OPEN,

  //Door of Time settings (open, song only, closed)
  RO_DOOROFTIME_OPEN = 0,
  RO_DOOROFTIME_SONGONLY,
  RO_DOOROFTIME_CLOSED,
  
  //Zora's Fountain settings (closed, closed as child, open)
  RO_ZF_CLOSED = 0,
  RO_ZF_CLOSED_CHILD,
  RO_ZF_OPEN,

  //Starting Age settings (child, adult, random)
  RO_AGE_CHILD = 0,
  RO_AGE_ADULT,
  RO_AGE_RANDOM,

  //Gerudo Fortress settings (normal, fast, open)
  RO_GF_NORMAL = 0,
  RO_GF_FAST,
  RO_GF_OPEN,

  //Rainbow Bridge settings (always open, vanilla, stones, medallions, dungeon rewards, dungeons, tokens)
  RO_BRIDGE_ALWAYS_OPEN = 0,
  RO_BRIDGE_VANILLA,
  RO_BRIDGE_STONES,
  RO_BRIDGE_MEDALLIONS,
  RO_BRIDGE_DUNGEON_REWARDS,
  RO_BRIDGE_DUNGEONS,
  RO_BRIDGE_TOKENS,

  //Shopsanity settings (off, 0-4 items, random)
  RO_SHOPSANITY_OFF = 0,
  RO_SHOPSANITY_ZERO_ITEMS,
  RO_SHOPSANITY_ONE_ITEM,
  RO_SHOPSANITY_TWO_ITEMS,
  RO_SHOPSANITY_THREE_ITEMS,
  RO_SHOPSANITY_FOUR_ITEMS,
  RO_SHOPSANITY_RANDOM,

  //Scrubsanity settings (off, affordable, expensive, random)
  RO_SCRUBS_OFF = 0,
  RO_SCRUBS_AFFORDABLE,
  RO_SCRUBS_EXPENSIVE,
  RO_SCRUBS_RANDOM,

  //Shuffle Merchants settings (off, on w/o hints, on w/hints)
  RO_MERCHANTS_OFF = 0,
  RO_MERCHANTS_ON_NO_HINTS,
  RO_MERCHANTS_ON_WITH_HINTS,

  //Bombchu settings (on, "on+bombchu", off)
  RO_BOMBCHU_DROPS_ON = 0,
  RO_BOMBCHU_DROPS_ON_PLUS,
  RO_BOMBCHU_DROPS_OFF,

  //Any Dungeon Item (start with, vanilla, own dungeon, any dungeon,
  //overworld, anywhere)
  RO_DUNGEON_ITEM_LOC_STARTWITH = 0,
  RO_DUNGEON_ITEM_LOC_VANILLA,
  RO_DUNGEON_ITEM_LOC_OWN_DUNGEON,
  RO_DUNGEON_ITEM_LOC_ANY_DUNGEON,
  RO_DUNGEON_ITEM_LOC_OVERWORLD,
  RO_DUNGEON_ITEM_LOC_ANYWHERE,

  //Starting Ocarina Settings (off, fairy)
  RO_STARTING_OCARINA_OFF = 0,
  RO_STARTING_OCARINA_FAIRY,

  //Item Pool Settings
  RO_ITEM_POOL_PLENTIFUL = 0,
  RO_ITEM_POOL_BALANCED,
  RO_ITEM_POOL_SCARCE,
  RO_ITEM_POOL_MINIMAL,

  //Ice Trap Settings
  RO_ICE_TRAPS_OFF = 0,
  RO_ICE_TRAPS_NORMAL,
  RO_ICE_TRAPS_EXTRA,
  RO_ICE_TRAPS_MAYHEM,
  RO_ICE_TRAPS_ONSLAUGHT,
  
  //Gossip Stone Hint Settings (no hints, needs nothing,
  //needs mask of truth, needs stone of agony)
  RO_GOSSIP_STONES_NONE = 0,
  RO_GOSSIP_STONES_NEED_NOTHING,
  RO_GOSSIP_STONES_NEED_TRUTH,
  RO_GOSSIP_STONES_NEED_STONE,
  
  //Hint Clarity Settings
  RO_HINT_CLARITY_OBSCURE = 0,
  RO_HINT_CLARITY_AMBIGUOUS,
  RO_HINT_CLARITY_CLEAR,

  //Gerudo Fortress Key Settings
  RO_GERUDO_KEYS_VANILLA = 0,
  RO_GERUDO_KEYS_ANY_DUNGEON,
  RO_GERUDO_KEYS_OVERWORLD,
  RO_GERUDO_KEYS_ANYWHERE,

  //Tokensanity settings
  RO_TOKENSANITY_OFF = 0,
  RO_TOKENSANITY_DUNGEONS,
  RO_TOKENSANITY_OVERWORLD,
  RO_TOKENSANITY_ALL,

  //Link's Pocket Settings
  RO_LINKS_POCKET_DUNGEON_REWARD = 0,
  RO_LINKS_POCKET_ADVANCEMENT,
  RO_LINKS_POCKET_ANYTHING,
  RO_LINKS_POCKET_NOTHING,

  //If you're going to add to this enum, please list the default option first
  //with a value of zero.

} RandoOption;

class Randomizer {
  private:
    std::unordered_map<RandomizerCheck, RandomizerGetData> itemLocations;
    std::unordered_map<RandomizerCheck, std::string> hintLocations;
    std::string childAltarText;
    std::string adultAltarText;
    std::string ganonHintText;
    std::string ganonText;
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;
    void ParseRandomizerSettingsFile(const char* spoilerFileName);
    void ParseHintLocationsFile(const char* spoilerFileName);
    void ParseRequiredTrialsFile(const char* spoilerFileName);
    void ParseItemLocationsFile(const char* spoilerFileName, bool silent);
    bool IsItemVanilla(RandomizerGet randoGet);
    GetItemEntry GetItemEntryFromRGData(RandomizerGetData rgData, GetItemID ogItemId, bool checkObtainability = true);

  public:
    Randomizer();
    ~Randomizer();

    static const std::string getItemMessageTableID;
    static const std::string hintMessageTableID;
    static const std::string merchantMessageTableID;
    static const std::string rupeeMessageTableID;
    static const std::string NaviRandoMessageTableID;
    static const std::string IceTrapRandoMessageTableID;

    // Public for now to be accessed by SaveManager, will be made private again soon :tm:
    std::unordered_map<RandomizerInf, bool> trialsRequired;
    std::unordered_map<RandomizerCheck, u16> merchantPrices;

    static Sprite* GetSeedTexture(uint8_t index);
    s16 GetItemModelFromId(s16 itemId);
    s32 GetItemIDFromGetItemID(s32 getItemId);
    bool SpoilerFileExists(const char* spoilerFileName);
    void LoadRandomizerSettings(const char* spoilerFileName);
    void LoadHintLocations(const char* spoilerFileName);
    void LoadMerchantMessages(const char* spoilerFileName);
    void LoadItemLocations(const char* spoilerFileName, bool silent);
    void LoadRequiredTrials(const char* spoilerFileName);
    bool IsTrialRequired(RandomizerInf trial);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    RandomizerGetData GetRandomizerGetDataFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    RandomizerGetData GetRandomizerGetDataFromKnownCheck(RandomizerCheck randomizerCheck);
    std::string GetChildAltarText() const;
    std::string GetAdultAltarText() const;
    std::string GetGanonText() const;
    std::string GetGanonHintText() const;
    ScrubIdentity IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData);
    ShopItemIdentity IdentifyShopItem(s32 sceneNum, u8 slotIndex);
    GetItemEntry GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId, bool checkObtainability = true);
    GetItemEntry GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId);
    GetItemID GetItemIdFromRandomizerGet(RandomizerGet randoGet, GetItemID ogItemId);
    ItemObtainability GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
    ItemObtainability GetItemObtainabilityFromRandomizerGet(RandomizerGet randomizerCheck);
    static void CreateCustomMessages();
    static std::string RandomizeRupeeName(std::string message, int language);
    static CustomMessageEntry GetRupeeMessage(u16 rupeeTextId);
    bool CheckContainsVanillaItem(RandomizerCheck randoCheck);
};

#ifdef __cplusplus
extern "C" {
#endif

void Rando_Init(void);

#ifdef __cplusplus
}
#endif


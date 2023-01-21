#pragma once

#include <algorithm>
#include <array>
#include <cstdio>
#include <functional>
#include <set>
#include <string>
#include <vector>
#include "spoiler_log.hpp"
#include "item_list.hpp"
#include "../randomizerTypes.h"

enum ItemOverride_Type {
    OVR_BASE_ITEM = 0,
    OVR_CHEST = 1,
    OVR_COLLECTABLE = 2,
    OVR_SKULL = 3,
    OVR_GROTTO_SCRUB = 4,
    OVR_DELAYED = 5,
    OVR_TEMPLE = 6,
};

typedef enum {
    DUNGEON_DEKU_TREE = 0,
    DUNGEON_DODONGOS_CAVERN,
    DUNGEON_JABUJABUS_BELLY,
    DUNGEON_FOREST_TEMPLE,
    DUNGEON_FIRE_TEMPLE,
    DUNGEON_WATER_TEMPLE,
    DUNGEON_SPIRIT_TEMPLE,
    DUNGEON_SHADOW_TEMPLE,
    DUNGEON_BOTTOM_OF_THE_WELL,
    DUNGEON_ICE_CAVERN,
    DUNGEON_GANONS_CASTLE_SECOND_PART,
    DUNGEON_GERUDO_TRAINING_GROUNDS,
    DUNGEON_GERUDO_FORTRESS,
    DUNGEON_GANONS_CASTLE_FIRST_PART,
    DUNGEON_GANONS_CASTLE_FLOOR_BENEATH_BOSS_CHAMBER,
    DUNGEON_GANONS_CASTLE_CRUMBLING,
    DUNGEON_TREASURE_CHEST_SHOP,
    DUNGEON_DEKU_TREE_BOSS_ROOM,
    DUNGEON_DODONGOS_CAVERN_BOSS_ROOM,
    DUNGEON_JABUJABUS_BELLY_BOSS_ROOM,
} DungeonId;

typedef union ItemOverride_Key {
    uint32_t all;
    struct {
        char pad_;
        uint8_t scene;
        uint8_t type;
        uint8_t flag;
    };
} ItemOverride_Key;

typedef union ItemOverride_Value {
    uint32_t all;
    struct {
        uint16_t itemId;
        uint8_t player;
        uint8_t looksLikeItemId;
    };
} ItemOverride_Value;

typedef struct ItemOverride {
    ItemOverride_Key key;
    ItemOverride_Value value;
} ItemOverride;

class Entrance;

enum class ItemLocationType {
    Base,
    Chest,
    Collectable,
    GSToken,
    GrottoScrub,
    Delayed,
    TempleReward,
    HintStone,
    OtherHint,
};

class SpoilerCollectionCheck {
  public:
    SpoilerCollectionCheckType type = SpoilerCollectionCheckType::SPOILER_CHK_NONE;
    uint8_t scene = 0;
    uint8_t flag = 0;

    SpoilerCollectionCheck() {}
    SpoilerCollectionCheck(SpoilerCollectionCheckType type_, uint8_t scene_, uint8_t flag_) : type(type_), scene(scene_), flag(flag_) {}

    static auto None() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_NONE, 0x00, 0x00);
    }

    static auto AlwaysCollected() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_ALWAYS_COLLECTED, 0x00, 0x00);
    }

    static auto ItemGetInf(uint8_t slot) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_ITEM_GET_INF, 0x00, slot);
    }

    static auto EventChkInf(uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_EVENT_CHK_INF, 0xFF, flag);
    }

    static auto InfTable(uint8_t offset, uint8_t bit) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_INF_TABLE, offset, bit);
    }

    static auto Collectable(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE, scene, flag);
    }

    static auto Chest(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_CHEST, scene, flag);
    }

    static auto Cow(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COW, scene, flag);
    }

    static auto Fishing(uint8_t bit) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MINIGAME, 0x00, bit);
    }

    static auto Scrub(uint8_t scene, uint8_t bit) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_SCRUB, scene, bit);
    }

    static auto Biggoron(uint8_t mask) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_BIGGORON, 0x00, mask);
    }

    static auto GerudoToken() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GERUDO_MEMBERSHIP_CARD, 0x00, 0x00);
    }

    static auto BigPoePoints() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_POE_POINTS, 0x00, 0x00);
    }
    
    static auto Gravedigger(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GRAVEDIGGER, scene, flag);
    }

    static auto ShopItem(uint8_t scene, uint8_t itemSlot) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_SHOP_ITEM, scene, itemSlot);
    }

    static auto MagicBeans(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MAGIC_BEANS, scene, flag);
    }

    static auto Merchant(int8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MERCHANT, scene, flag);
    }

    static auto RandomizerInf(int8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF, scene, flag);
    }
};

class ItemLocation {
public:
    ItemLocation() = default;
    ItemLocation(RandomizerCheck rc_, uint8_t scene_, ItemLocationType type_, uint8_t flag_, std::string name_, uint32_t hintKey_, uint32_t vanillaItem_, std::vector<Category> categories_, uint16_t price_ = 0, SpoilerCollectionCheck collectionCheck_ = SpoilerCollectionCheck(), SpoilerCollectionCheckGroup collectionCheckGroup_ = SpoilerCollectionCheckGroup::GROUP_NO_GROUP)
        : rc(rc_), scene(scene_), type(type_), flag(flag_), name(std::move(name_)), hintKey(hintKey_), vanillaItem(vanillaItem_), categories(std::move(categories_)), price(price_), collectionCheck(collectionCheck_), collectionCheckGroup(collectionCheckGroup_) {}

    ItemOverride_Key Key() const {
        ItemOverride_Key key;
        key.all = 0;

        key.scene = scene;
        key.type = static_cast<uint8_t>(type); //TODO make sure these match up
        key.flag = flag;
        return key;
    }

    RandomizerCheck GetRandomizerCheck() const {
      return rc;
    }

    SpoilerCollectionCheck GetCollectionCheck() const {
      return collectionCheck;
    }

    SpoilerCollectionCheckGroup GetCollectionCheckGroup() const {
      return collectionCheckGroup;
    }

    uint8_t GetScene() const {
      return scene;
    }

    uint8_t GetFlag() const {
      return flag;
    }

    bool IsAddedToPool() const {
      return addedToPool;
    }

    void AddToPool() {
      addedToPool = true;
    }

    const uint32_t GetHintKey() const {
        return hintKey;
    }

    void RemoveFromPool() {
      addedToPool = false;
    }

    const std::string& GetName() const {
      return name;
    }

    const Text& GetPlacedItemName() const {
      return ItemTable(placedItem).GetName();
    }

    const Item& GetPlacedItem() const {
      return ItemTable(placedItem);
    }

    uint32_t GetPlacedItemKey() const {
        return placedItem;
    }

    uint32_t GetPlaceduint32_t() const {
      return placedItem;
    }

    void SetPlacedItem(const uint32_t item) {
      placedItem = item;
      SetPrice(ItemTable(placedItem).GetPrice());
    }

    //Saves an item to be set as placedItem later
    void SetDelayedItem(const uint32_t item) {
      delayedItem = item;
    }

    //Place the vanilla item in this location
    void PlaceVanillaItem() {
      placedItem = vanillaItem;
    }

    void ApplyPlacedItemEffect() {
      ItemTable(placedItem).ApplyEffect();
    }

    //Set placedItem as item saved in SetDelayedItem
    void SaveDelayedItem() {
      placedItem = delayedItem;
      delayedItem = NONE;
    }

    uint16_t GetPrice() const {
      if (ItemTable(placedItem).GetItemType() == ITEMTYPE_SHOP) {
        return ItemTable(placedItem).GetPrice();
      }
      return price;
    }

    void SetPrice(uint16_t price_) {
      //don't override price if the price was set for shopsanity/scrubsanity
      if (hasShopsanityPrice || hasScrubsanityPrice) {
        return;
      }
      price = price_;
    }

    void SetShopsanityPrice(uint16_t price_) {
      price = price_;
      hasShopsanityPrice = true;
    }

    void SetScrubsanityPrice(uint16_t price_) {
      price = price_;
      hasScrubsanityPrice = true;
    }

    bool HasShopsanityPrice() const {
      return hasShopsanityPrice;
    }

    bool HasScrubsanityPrice() const {
      return hasScrubsanityPrice;
    }

    bool IsExcluded() const {
      return excludedOption.Value<bool>();
    }

    bool IsCategory(Category category) const {
      return std::any_of(categories.begin(), categories.end(),
                         [category](auto entry) { return entry == category; });
    }

    bool IsDungeon() const {
      return (type != ItemLocationType::GSToken && (scene < 0x0E || (scene > 0x10 && scene < 0x1A))) || (type == ItemLocationType::GSToken && scene < 0x0A);
    }

    bool IsOverworld() const {
      return !IsDungeon();
    }

    bool IsShop() const {
        return (scene >= 0x2C && scene <= 0x32);
    }

    Option * GetExcludedOption() {
      return &excludedOption;
    }

    const uint32_t Getuint32_t() const {
      return hintKey;
    }

    const HintText& GetHint() const {
      return Hint(hintKey);
    }

    bool IsHintedAt() const {
      return hintedAt;
    }

    void SetAsHinted() {
      hintedAt = true;
    }

    bool IsHidden() const {
      return hidden;
    }

    void SetHidden(const bool hidden_) {
      hidden = hidden_;
    }

    bool IsHintable() const {
      return isHintable;
    }

    void SetAsHintable() {
      isHintable = true;
    }

    void SetParentRegion(uint32_t region) {
      parentRegion = region;
    }

    uint32_t GetParentRegionKey() const {
      return parentRegion;
    }

    void AddExcludeOption() {
      //setting description  /*--------------------------------------------------*/
      std::string_view desc = "Decide which locations you want to exclude from\n"
                              "the location pool. Locations that require an item\n"
                              "to be placed at them based on your current\n"
                              "settings cannot be excluded and won't be shown\n"
                              "unless you change your settings.\n"
                              "\n"
                              "If you exclude too many locations, it might not\n"
                              "be possible to fill the world.";

      //add option to forbid any location from progress items
      if (name.length() < 23) {
        excludedOption = Option::Bool(name, {"Include", "Exclude"}, {desc});
      } else {
        //insert a newline character if the text is too long for one row
        size_t lastSpace = name.rfind(' ', 23);
        std::string settingText = name;
        settingText.replace(lastSpace, 1, "\n ");

        excludedOption = Option::Bool(settingText, {"Include", "Exclude"}, {desc});
      }
      
      // RANDOTODO: this without string compares and loops
      bool alreadyAdded = false;
      for(const Option* location : Settings::excludeLocationsOptionsVector[collectionCheckGroup]) {
        if (location->GetName() == excludedOption.GetName()) {
          alreadyAdded = true;
        }
      }
      if (!alreadyAdded) {
        Settings::excludeLocationsOptionsVector[collectionCheckGroup].push_back(&excludedOption);
      }
    }

    static auto Base(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::Base, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, collectionCheck, collectionCheckGroup};
    }

    static auto Chest(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::Chest, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_CHEST, scene, flag), collectionCheckGroup};
    }

    static auto Chest(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck, SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::Chest, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, collectionCheck, collectionCheckGroup};
    }

    static auto Collectable(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::Collectable, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE, scene, flag), collectionCheckGroup};
    }

    static auto Collectable(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck, SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::Collectable, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, collectionCheck, collectionCheckGroup};
    }

    static auto GSToken(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, std::vector<Category>&& categories, SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::GSToken, flag, std::move(name), hintKey, GOLD_SKULLTULA_TOKEN, std::move(categories), 0, SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GOLD_SKULLTULA, scene, flag), collectionCheckGroup};
    }

    static auto GrottoScrub(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::GrottoScrub, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, collectionCheck, collectionCheckGroup};
    }

    static auto Delayed(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::Delayed, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, collectionCheck, collectionCheckGroup};
    }

    static auto Reward(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey, const uint32_t vanillaItem, std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
        return ItemLocation{rc, scene, ItemLocationType::TempleReward, flag, std::move(name), hintKey, vanillaItem, std::move(categories), 0, collectionCheck, collectionCheckGroup};
    }

    static auto OtherHint(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, std::vector<Category>&& categories) {
        return ItemLocation{rc, scene, ItemLocationType::OtherHint, flag, std::move(name), NONE, NONE, std::move(categories)};
    }

    static auto HintStone(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, std::vector<Category>&& categories) {
        return ItemLocation{rc, scene, ItemLocationType::HintStone, flag, std::move(name), NONE, NONE, std::move(categories)};
    }

    void ResetVariables() {
      checked = false;
      addedToPool = false;
      placedItem = NONE;
      delayedItem = NONE;
      hintedAt = false;
      isHintable = false;
      price = 0;
      hasShopsanityPrice =  false;
      hasScrubsanityPrice =  false;
      hidden = false;
    }

private:
    RandomizerCheck rc;
    uint8_t scene;
    ItemLocationType type;
    uint8_t flag;
    bool checked = false;

    std::string name;
    uint32_t hintKey = NONE;
    uint32_t vanillaItem = NONE;
    bool hintedAt = false;
    std::vector<Category> categories;
    bool addedToPool = false;
    uint32_t placedItem = NONE;
    uint32_t delayedItem = NONE;
    Option excludedOption = Option::Bool(name, {"Include", "Exclude"}, {"", ""});
    uint16_t price = 0;
    SpoilerCollectionCheck collectionCheck;
    SpoilerCollectionCheckGroup collectionCheckGroup;
    bool isHintable = false;
    uint32_t parentRegion = NONE;
    bool hasShopsanityPrice = false;
    bool hasScrubsanityPrice = false;
    bool hidden = false;
};

class ItemOverride_Compare {
public:
    bool operator()(ItemOverride lhs, ItemOverride rhs) const {
        return lhs.key.all < rhs.key.all;
    }
};

void LocationTable_Init();

ItemLocation* Location(uint32_t locKey);
ItemLocation* Location(RandomizerCheck rc);

extern std::vector<std::vector<uint32_t>> ShopLocationLists;

extern std::vector<uint32_t> ScrubLocations;

extern std::vector<uint32_t> gossipStoneLocations;

extern std::vector<uint32_t> dungeonRewardLocations;
extern std::vector<uint32_t> overworldLocations;
extern std::vector<uint32_t> allLocations;
extern std::vector<uint32_t> everyPossibleLocation;

//set of overrides to write to the patch
extern std::set<ItemOverride, ItemOverride_Compare> overrides;
extern std::unordered_map<RandomizerCheck, uint8_t> iceTrapModels;

extern std::vector<std::vector<uint32_t>> playthroughLocations;
extern std::vector<uint32_t> wothLocations;
extern bool playthroughBeatable;
extern bool allLocationsReachable;
extern bool showItemProgress;

extern uint16_t itemsPlaced;

void GenerateLocationPool();
void PlaceItemInLocation(uint32_t loc, uint32_t item, bool applyEffectImmediately = false, bool setHidden = false);
std::vector<uint32_t> GetLocations(const std::vector<uint32_t>& locationPool, Category categoryInclude,
                                   Category categoryExclude = Category::cNull);
void LocationReset();
void ItemReset();
void HintReset();
void AddExcludedOptions();
void CreateItemOverrides();

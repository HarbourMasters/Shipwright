#pragma once

#include <string>
#include <variant>

#include "3drando/text.hpp"
#include "3drando/settings.hpp"
#include "3drando/hint_list.hpp"
#include "randomizerTypes.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

union ItemOverride_Value;

enum ItemType {
    ITEMTYPE_ITEM,
    ITEMTYPE_MAP,
    ITEMTYPE_COMPASS,
    ITEMTYPE_BOSSKEY,
    ITEMTYPE_SMALLKEY,
    ITEMTYPE_TOKEN,
    ITEMTYPE_FORTRESS_SMALLKEY,
    ITEMTYPE_EVENT,
    ITEMTYPE_DROP,
    ITEMTYPE_REFILL,
    ITEMTYPE_SONG,
    ITEMTYPE_SHOP,
    ITEMTYPE_DUNGEONREWARD
};

class RandoItem {
  public:
    RandoItem() = default;
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
              bool* logicVar_, uint32_t hintKey_, int itemId_, int objectId_, int gid_, int textId_, int field_,
              int chestAnimation_, GetItemCategory category_, int modIndex_, uint16_t price_ = 0);
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
              uint8_t* logicVar_, uint32_t hintKey_, int itemId_, int objectId_, int gid_, int textId_, int field_,
              int chestAnimation_, GetItemCategory category_, int modIndex_, uint16_t price_ = 0);
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
              bool* logicVar_, uint32_t hintKey_, uint16_t price = 0);
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
              uint8_t* logicVar_, uint32_t hintKey_, uint16_t price = 0);
    ~RandoItem();

    void ApplyEffect();
    void UndoEffect();

    ItemOverride_Value Value() const;
    
    const Text& GetName() const;
    bool IsAdvancement() const;
    int GetItemID() const;
    ItemType GetItemType() const;
    RandomizerGet GetRandomizerGet();
    uint16_t GetPrice() const;
    void SetPrice(uint16_t price_);
    void SetAsPlaythrough();
    bool IsPlaythrough() const;
    bool IsBottleItem() const;
    bool IsMajorItem() const;
    const uint32_t GetHintKey() const;
    const HintText& GetHint() const;
    bool operator==(const RandoItem& right) const;
    bool operator!=(const RandoItem& right) const;

  private:
    RandomizerGet randomizerGet;
    Text name;
    ItemType type;
    int getItemId;
    bool advancement;
    std::variant<bool*, uint8_t*> logicVar;
    uint32_t hintKey;
    int itemId;
    int objectId;
    int gid;
    int textId;
    int field;
    int chestAnimation;
    int modIndex;
    bool collectable;
    GetItemCategory category;
    uint16_t price;
    bool playthrough = false;
};

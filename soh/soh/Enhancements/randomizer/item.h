#pragma once

#include <string>
#include <variant>
#include <memory>

#include "3drando/text.hpp"
#include "randomizerTypes.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "3drando/hints.hpp"

enum ItemType {
    ITEMTYPE_ITEM,
    ITEMTYPE_EQUIP,
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

namespace Rando {
class Item {
  public:
    Item();
    Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
        LogicVal logicVal_, RandomizerHintTextKey hintKey_, uint16_t itemId_, uint16_t objectId_, uint16_t gid_,
        uint16_t textId_, uint16_t field_, int16_t chestAnimation_, GetItemCategory category_, uint16_t modIndex_,
        bool progressive_ = false, uint16_t price_ = 0);
    Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
        LogicVal logicVal_, RandomizerHintTextKey hintKey_, bool progressive_ = false, uint16_t price_ = 0);
    ~Item();

    void ApplyEffect() const;
    void UndoEffect() const;

    
    const Text& GetName() const;
    bool IsAdvancement() const;
    int GetItemID() const;
    ItemType GetItemType() const;
    LogicVal GetLogicVal() const;
    RandomizerGet GetRandomizerGet() const;
    uint16_t GetPrice() const;
    std::shared_ptr<GetItemEntry> GetGIEntry() const;
    GetItemEntry GetGIEntry_Copy() const;
    void SetPrice(uint16_t price_);
    void SetAsPlaythrough();
    void SetCustomDrawFunc(CustomDrawFunc) const;
    bool IsPlaythrough() const;
    bool IsBottleItem() const;
    bool IsMajorItem() const;
    RandomizerHintTextKey GetHintKey() const;
    const HintText& GetHint() const;
    bool operator==(const Item& right) const;
    bool operator!=(const Item& right) const;

  private:
    RandomizerGet randomizerGet;
    Text name;
    ItemType type;
    int16_t getItemId;
    bool advancement;
    LogicVal logicVal;
    RandomizerHintTextKey hintKey;
    bool progressive;
    uint16_t price;
    bool playthrough = false;
    std::shared_ptr<GetItemEntry> giEntry;
};
}

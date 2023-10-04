#pragma once

#include <string>
#include <variant>

#include "3drando/text.hpp"
#include "3drando/settings.hpp"
#include "3drando/hint_list.hpp"
#include "randomizerTypes.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

namespace Rando {
union ItemOverride_Value;
}

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
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
                     bool* logicVar_, uint32_t hintKey_, uint16_t itemId_, uint16_t objectId_, uint16_t gid_,
                     uint16_t textId_, uint16_t field_, int16_t chestAnimation_, GetItemCategory category_,
                     uint16_t modIndex_, bool progressive = false, uint16_t price_ = 0);
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
              uint8_t* logicVar_, uint32_t hintKey_, uint16_t itemId_, uint16_t objectId_, uint16_t gid_,
              uint16_t textId_, uint16_t field_, int16_t chestAnimation_, GetItemCategory category_, uint16_t modIndex_,
              bool progressive = false, uint16_t price_ = 0);
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
              bool* logicVar_, uint32_t hintKey_, bool progressive = false, uint16_t price = 0);
    RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
              uint8_t* logicVar_, uint32_t hintKey_, bool progressive = false, uint16_t price = 0);
    ~RandoItem();

    void ApplyEffect();
    void UndoEffect();

    Rando::ItemOverride_Value Value() const;
    
    const Text& GetName() const;
    bool IsAdvancement() const;
    int GetItemID() const;
    ItemType GetItemType() const;
    RandomizerGet GetRandomizerGet();
    uint16_t GetPrice() const;
    std::shared_ptr<GetItemEntry> GetGIEntry() const;
    GetItemEntry GetGIEntry_Copy();
    void SetPrice(uint16_t price_);
    void SetAsPlaythrough();
    void SetCustomDrawFunc(CustomDrawFunc);
    bool IsPlaythrough() const;
    bool IsBottleItem() const;
    bool IsMajorItem() const;
    uint32_t GetHintKey() const;
    const HintText& GetHint() const;
    bool operator==(const RandoItem& right) const;
    bool operator!=(const RandoItem& right) const;

  private:
    RandomizerGet randomizerGet;
    Text name;
    ItemType type;
    int16_t getItemId;
    bool advancement;
    std::variant<bool*, uint8_t*> logicVar;
    uint32_t hintKey;
    bool progressive = false;
    uint16_t price;
    bool playthrough = false;
   std::shared_ptr<GetItemEntry> giEntry = nullptr;
};

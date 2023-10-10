#pragma once

#include "randomizerTypes.h"
#include "item.h"
#include "3drando/text.hpp"
#include "static_data.h"

namespace Rando {
enum ItemOverride_Type {
    OVR_BASE_ITEM = 0,
    OVR_CHEST = 1,
    OVR_COLLECTABLE = 2,
    OVR_SKULL = 3,
    OVR_GROTTO_SCRUB = 4,
    OVR_DELAYED = 5,
    OVR_TEMPLE = 6,
};

typedef union ItemOverride_Key {
    uint32_t all;
    struct {
        char pad_;
        uint8_t scene;
        uint8_t type;
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

class ItemOverride_Compare {
  public:
    bool operator()(ItemOverride lhs, ItemOverride rhs) const {
        return lhs.key.all < rhs.key.all;
    }
};

class ItemLocation {
  public:
    RandomizerCheck GetRandomizerCheck() const;
    bool IsAddedToPool() const;
    void AddToPool();
    void RemoveFromPool();
    const RandoItem& GetPlacedItem() const;
    const Text& GetPlacedItemName() const;
    RandomizerGet GetPlacedRandomizerGet() const;
    void SetPlacedItem(const RandomizerGet item);
    void SetDelayedItem(const RandomizerGet item);
    RandomizerRegion GetParentRegionKey() const;
    void SetParentRegion (RandomizerRegion region);
    void PlaceVanillaItem();
    void ApplyPlacedItemEffect();
    void SaveDelayedItem();
    uint16_t GetPrice() const;
    void SetPrice(uint16_t price_);
    bool HasShopsanityPrice() const;
    void SetShopsanityPrice(uint16_t price_);
    bool HasScrubsanityPrice() const;
    void SetScrubsanityPrice(uint16_t price_);
    bool IsHintable() const;
    void SetAsHintable();
    bool IsHintedAt() const;
    void SetAsHinted();
    RandomizerHintKey GetHintKey() const;
    void SetHintKey(RandomizerHintKey hintKey);
    bool IsHidden() const;
    bool IsExcluded() const;
    void SetHidden(const bool hidden_);
    Rando::ItemOverride_Key Key() const;
    void ResetVariables();
  private:
    RandomizerCheck rc;
    RandomizerHintKey hintedBy;
    bool hintedAt = false;
    bool isHintable = false;
    bool addedToPool = false;
    RandomizerGet placedItem = RG_NONE;
    RandomizerGet delayedItem = RG_NONE;
    Option excludedOption = Option::Bool(StaticData::Location(rc)->GetName(), {"Include", "Exclude"});
    uint16_t price = 0;
    RandomizerRegion parentRegion = RR_NONE;
    bool hasShopsanityPrice = false;
    bool hasScrubsanityPrice = false;
    bool hidden = false;
};
} // namespace Rando
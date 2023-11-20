#pragma once

#include "randomizerTypes.h"
#include "3drando/text.hpp"
#include "static_data.h"
#include "settings.h"

namespace Rando {
class ItemLocation {
  public:
    ItemLocation();
    explicit ItemLocation(RandomizerCheck rc_);
    RandomizerCheck GetRandomizerCheck() const;
    bool IsAddedToPool() const;
    void AddToPool();
    void RemoveFromPool();
    const Item& GetPlacedItem() const;
    const Text& GetPlacedItemName() const;
    RandomizerGet GetPlacedRandomizerGet() const;
    void SetPlacedItem(RandomizerGet item);
    RandomizerGet& RefPlacedItem();
    void SetDelayedItem(RandomizerGet item);
    RandomizerRegion GetParentRegionKey() const;
    void SetParentRegion (RandomizerRegion region);
    void PlaceVanillaItem();
    void ApplyPlacedItemEffect() const;
    void SaveDelayedItem();
    uint16_t GetPrice() const;
    void SetPrice(uint16_t price_);
    bool HasCustomPrice() const;
    void SetCustomPrice(uint16_t price_);
    bool IsHintable() const;
    void SetAsHintable();
    bool IsHintedAt() const;
    void SetAsHinted();
    RandomizerHintKey GetHintKey() const;
    void SetHintKey(RandomizerHintKey hintKey);
    bool IsHidden() const;
    bool IsExcluded() const;
    void AddExcludeOption();
    Option* GetExcludedOption();
    void SetHidden(bool hidden_);
    bool IsVisible() const;
    void SetVisible(bool visibleInImGui_);
    void ResetVariables();

  private:
    RandomizerCheck rc;
    RandomizerHintKey hintedBy = RH_NONE;
    bool hintedAt = false;
    bool isHintable = false;
    bool addedToPool = false;
    RandomizerGet placedItem = RG_NONE;
    RandomizerGet delayedItem = RG_NONE;
    Option excludedOption = Option::Bool(StaticData::GetLocation(rc)->GetName(), {"Include", "Exclude"});
    uint16_t price = 0;
    RandomizerRegion parentRegion = RR_NONE;
    bool hasCustomPrice = false;
    bool hidden = false;
    bool visibleInImGui = false;
};
} // namespace Rando
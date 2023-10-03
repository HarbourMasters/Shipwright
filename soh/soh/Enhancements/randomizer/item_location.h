#include "randomizerTypes.h"
#include "item.h"
#include "3drando/text.hpp"

namespace Rando {
class ItemLocation {
  public:
    bool IsAddedToPool() const;
    void AddToPool();
    void RemoveFromPool();
    const RandoItem& GetPlacedItem() const;
    const Text& GetPlacedItemName() const;
    RandomizerGet GetPlacedRandomizerGet() const;
    void SetPlacedItem(const RandomizerGet item);
    void SetDelayedItem(const RandomizerGet item);
    void PlaceVanillaItem();
    void ApplyPlacedItemEffect();
    void SaveDelayedItem();
    uint16_t GetPrice() const;
    void SetPrice(uint16_t price_);
    bool HasShopsanityPrice() const;
    void SetShopsanityPrice(uint16_t price_);
    bool HasScrubsanityPrice() const;
    void SetScrubsanityPrice(uint16_t price_) const;
    bool IsHintedAt() const;
    void SetAsHinted();
    bool IsHidden() const;
    bool SetHidden(const bool hidden_);
    void ResetVariables();
  private:
    RandomizerCheck rc;
    bool hintedAt = false;
    bool addedToPool = false;
    RandomizerGet placedItem = RG_NONE;
    RandomizerGet delayedItem = RG_NONE;
    uint16_t price = 0;
    bool hasShopsanityPrice = false;
    bool hasScrubsanityPrice = false;
    bool hidden = false;
};
} // namespace Rando
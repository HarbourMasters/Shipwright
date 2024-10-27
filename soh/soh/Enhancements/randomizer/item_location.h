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
    std::set<RandomizerArea> GetAreas() const;
    RandomizerArea GetFirstArea() const;
    void MergeAreas (std::set<RandomizerArea> newAreas);
    void PlaceVanillaItem();
    void ApplyPlacedItemEffect() const;
    void SaveDelayedItem();
    uint16_t GetPrice() const;
    void SetPrice(uint16_t price_);
    bool HasCustomPrice() const;
    void SetCustomPrice(uint16_t price_);
    bool HasObtained() const;
    void SetCheckStatus(RandomizerCheckStatus status_);
    RandomizerCheckStatus GetCheckStatus();
    void SetIsSkipped(bool isSkipped_);
    bool GetIsSkipped();
    bool IsHintable() const;
    void SetAsHintable();
    bool IsAHintAccessible() const;
    void SetHintAccesible();
    const std::vector<RandomizerHint>& GetHintedBy() const;
    void AddHintedBy(RandomizerHint hintKey);
    bool IsHidden() const;
    bool IsExcluded() const;
    void AddExcludeOption();
    Option* GetExcludedOption();
    void SetHidden(bool hidden_);
    bool IsVisible() const;
    void SetVisible(bool visibleInImGui_);
    bool IsWothCandidate() const;
    void SetWothCandidate();
    bool IsFoolishCandidate() const;
    void SetBarrenCandidate();
    void ResetVariables();

  private:
    RandomizerCheck rc;
    std::vector<RandomizerHint> hintedBy = {};
    bool hintAccesible = false;
    bool isHintable = false;
    bool addedToPool = false;
    RandomizerGet placedItem = RG_NONE;
    RandomizerGet delayedItem = RG_NONE;
    Option excludedOption = Option::Bool(StaticData::GetLocation(rc)->GetName(), {"Include", "Exclude"}, OptionCategory::Setting, "", "", WidgetType::Checkbox, RO_LOCATION_INCLUDE);
    uint16_t price = 0;
    RandomizerRegion parentRegion = RR_NONE;
    std::set<RandomizerArea> areas = {};
    bool hasCustomPrice = false;
    bool hidden = false;
    bool visibleInImGui = false;
    bool wothCandidate = false;
    bool barrenCandidate = false;
    RandomizerCheckStatus status = RCSHOW_UNCHECKED;
    bool isSkipped = false;
};
} // namespace Rando
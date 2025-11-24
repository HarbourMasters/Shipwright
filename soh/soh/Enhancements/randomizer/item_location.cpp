#include "item_location.h"
#include "context.h"
#include "logic.h"

namespace Rando {
ItemLocation::ItemLocation() : rc(RC_UNKNOWN_CHECK) {
}
ItemLocation::ItemLocation(const RandomizerCheck rc_) : rc(rc_) {
}

RandomizerCheck ItemLocation::GetRandomizerCheck() const {
    return rc;
}

bool ItemLocation::IsAddedToPool() const {
    return addedToPool;
}

void ItemLocation::AddToPool() {
    addedToPool = true;
}

void ItemLocation::RemoveFromPool() {
    addedToPool = false;
}

const Item& ItemLocation::GetPlacedItem() const {
    return StaticData::RetrieveItem(placedItem);
}

RandomizerGet& ItemLocation::RefPlacedItem() {
    return placedItem;
}

const Text& ItemLocation::GetPlacedItemName() const {
    return StaticData::RetrieveItem(placedItem).GetName();
}

RandomizerGet ItemLocation::GetPlacedRandomizerGet() const {
    return placedItem;
}

void ItemLocation::SetPlacedItem(const RandomizerGet item) {
    placedItem = item;
    SetPrice(StaticData::RetrieveItem(placedItem).GetPrice());
}

void ItemLocation::SetDelayedItem(const RandomizerGet item) {
    delayedItem = item;
}

void ItemLocation::SaveDelayedItem() {
    placedItem = delayedItem;
    delayedItem = RG_NONE;
}

void ItemLocation::SetParentRegion(const RandomizerRegion region) {
    parentRegion = region;
}

// RANDOTODO only used in tracker now, could possibly be removed
RandomizerRegion ItemLocation::GetParentRegionKey() const {
    return parentRegion;
}

void ItemLocation::MergeAreas(std::set<RandomizerArea> newAreas) {
    areas.merge(newAreas);
    if (areas.size() >= 2) {
        // if we have more than 1 area, remove any RA_NONE as that's not a real area. can happen if an entrance is in 2
        // regions and 1 is disconnected
        areas.erase(RA_NONE);
    }
}

std::set<RandomizerArea> ItemLocation::GetAreas() const {
    return areas;
}

RandomizerArea ItemLocation::GetFirstArea() const {
    if (areas.empty()) {
        assert(false);
        return RA_NONE;
    } else {
        return *areas.begin();
    }
}

RandomizerArea ItemLocation::GetRandomArea() const {
    if (areas.empty()) {
        SPDLOG_DEBUG("Attempted to get random area of location with no areas: ");
        SPDLOG_DEBUG(Rando::StaticData::GetLocation(rc)->GetName());
        assert(false);
        return RA_NONE;
    } else {
        return RandomElementFromSet(areas);
    }
}

void ItemLocation::PlaceVanillaItem() {
    placedItem = StaticData::GetLocation(rc)->GetVanillaItem();
}

void ItemLocation::ApplyPlacedItemEffect() const {
    StaticData::RetrieveItem(placedItem).ApplyEffect();
}

uint16_t ItemLocation::GetPrice() const {
    // RANDOTODO if we ever change price of shop items, this needs replacing with proper price assignment in Fill
    if (StaticData::RetrieveItem(placedItem).GetItemType() == ITEMTYPE_SHOP) {
        return StaticData::RetrieveItem(placedItem).GetPrice();
    }
    return price;
}

void ItemLocation::SetPrice(const uint16_t price_) {
    if (hasCustomPrice) {
        return;
    }
    price = price_;
}

bool ItemLocation::HasCustomPrice() const {
    return hasCustomPrice;
}

void ItemLocation::SetCustomPrice(const uint16_t price_) {
    price = price_;
    hasCustomPrice = true;
}

bool ItemLocation::HasObtained() const {
    return status == RCSHOW_COLLECTED || status == RCSHOW_SAVED;
}

void ItemLocation::SetCheckStatus(RandomizerCheckStatus status_) {
    status = status_;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnRandoSetCheckStatus>(rc, status);
}

RandomizerCheckStatus ItemLocation::GetCheckStatus() {
    return status;
}

void ItemLocation::SetIsSkipped(bool isSkipped_) {
    isSkipped = isSkipped_;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnRandoSetIsSkipped>(rc, isSkipped);
}

bool ItemLocation::GetIsSkipped() {
    return isSkipped;
}

bool ItemLocation::IsHintable() const {
    return isHintable;
}

void ItemLocation::SetAsHintable() {
    isHintable = true;
}

bool ItemLocation::IsAHintAccessible() const {
    return hintAccesible;
}

void ItemLocation::SetHintAccesible() {
    hintAccesible = true;
}

const std::vector<RandomizerHint>& ItemLocation::GetHintedBy() const {
    return hintedBy;
}

void ItemLocation::AddHintedBy(const RandomizerHint hintKey) {
    hintedBy.push_back(hintKey);
}

bool ItemLocation::IsHidden() const {
    return hidden;
}

void ItemLocation::SetHidden(const bool hidden_) {
    hidden = hidden_;
}

bool ItemLocation::IsExcluded() {
    return excludedOption.Is(RO_LOCATION_EXCLUDE);
}

OptionValue& ItemLocation::GetExcludedOption() {
    return excludedOption;
}

void ItemLocation::SetExcludedOption(uint8_t val) {
    excludedOption.Set(val);
}

bool ItemLocation::IsVisible() const {
    return visibleInImGui;
}
void ItemLocation::SetVisible(const bool visibleInImGui_) {
    visibleInImGui = visibleInImGui_;
}

bool ItemLocation::IsWothCandidate() const {
    return wothCandidate;
}

void ItemLocation::SetWothCandidate() {
    wothCandidate = true;
}

bool ItemLocation::IsFoolishCandidate() const {
    return barrenCandidate;
}

void ItemLocation::SetBarrenCandidate() {
    barrenCandidate = true;
}

void ItemLocation::ResetVariables() {
    addedToPool = false;
    placedItem = RG_NONE;
    delayedItem = RG_NONE;
    isHintable = false;
    hintAccesible = false;
    hintedBy = {};
    price = 0;
    hasCustomPrice = false;
    hidden = false;
    wothCandidate = false;
    barrenCandidate = false;
    areas = {};
    status = RCSHOW_UNCHECKED;
    isSkipped = false;
    isAvailable = false;
}

bool ItemLocation::IsAvailable() const {
    return isAvailable;
}

void ItemLocation::SetAvailable(bool isAvailable_) {
    isAvailable = isAvailable_;
}
} // namespace Rando
#include "item_location.h"
#include "context.h"
#include "logic.h"

namespace Rando {
ItemLocation::ItemLocation() : rc(RC_UNKNOWN_CHECK) {}
ItemLocation::ItemLocation(const RandomizerCheck rc_) : rc(rc_) {}

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
    SetPrice (StaticData::RetrieveItem(placedItem).GetPrice());
}

void ItemLocation::SetDelayedItem(const RandomizerGet item) {
    delayedItem = item;
}

void ItemLocation::SaveDelayedItem () {
    placedItem = delayedItem;
    delayedItem = RG_NONE;
}

void ItemLocation::SetParentRegion(const RandomizerRegion region) {
    parentRegion = region;
}

RandomizerRegion ItemLocation::GetParentRegionKey() const {
    return parentRegion;
}

void ItemLocation::MergeAreas(std::set<RandomizerArea> newAreas) {
    areas.merge(newAreas);
    if (areas.size() >= 2){
        //if we have more than 1 area, remove any RA_NONE as that's not a real area. can happen if an entrance is in 2 regions and 1 is disconnected
        areas.erase(RA_NONE);
    }
}

std::set<RandomizerArea> ItemLocation::GetAreas() const {
    return areas;
}

RandomizerArea ItemLocation::GetFirstArea() const {
    if (areas.empty()){
        assert(false);
        return RA_NONE;
    } else {
        return *areas.begin();
    }
}

void ItemLocation::PlaceVanillaItem() {
    placedItem = StaticData::GetLocation(rc)->GetVanillaItem();
}

void ItemLocation::ApplyPlacedItemEffect() const {
    StaticData::RetrieveItem(placedItem).ApplyEffect();
}

uint16_t ItemLocation::GetPrice() const {
    //RANDOTODO if we ever change price of shop items, this needs replacing with proper price assignment in Fill
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
}

RandomizerCheckStatus ItemLocation::GetCheckStatus() {
    return status;
}

void ItemLocation::SetIsSkipped(bool isSkipped_) {
    isSkipped = isSkipped_;
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

bool ItemLocation::IsExcluded() const {
    return excludedOption.Value<bool>();
}

Option* ItemLocation::GetExcludedOption() {
    return &excludedOption;
}

void ItemLocation::AddExcludeOption() {
    if (const std::string name = StaticData::GetLocation(rc)->GetName(); name.length() < 23) {
        excludedOption = Option::Bool(name, {"Include", "Exclude"}, OptionCategory::Setting, "", "", WidgetType::Checkbox, RO_LOCATION_INCLUDE);
    } else {
        const size_t lastSpace = name.rfind(' ', 23);
        std::string settingText = name;
        settingText.replace(lastSpace, 1, "\n ");

        excludedOption = Option::Bool(settingText, {"Include", "Exclude"}, OptionCategory::Setting, "", "", WidgetType::Checkbox, RO_LOCATION_INCLUDE);
    }
    // RANDOTODO: this without string compares and loops
    bool alreadyAdded = false;
    const Location* loc = StaticData::GetLocation(rc);
    for (const Option* location : Context::GetInstance()->GetSettings()->GetExcludeOptionsForArea(loc->GetArea())) {
        if (location->GetName() == excludedOption.GetName()) {
            alreadyAdded = true;
        }
    }
    if (!alreadyAdded) {
        Context::GetInstance()->GetSettings()->GetExcludeOptionsForArea(loc->GetArea()).push_back(&excludedOption);
    }
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
}
}
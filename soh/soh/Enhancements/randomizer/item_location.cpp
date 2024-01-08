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

void ItemLocation::SetArea(RandomizerArea newArea) {
    area = newArea;
}

RandomizerArea ItemLocation::GetArea() const {
    return area;
}

void ItemLocation::PlaceVanillaItem() {
    placedItem = StaticData::GetLocation(rc)->GetVanillaItem();
}

void ItemLocation::ApplyPlacedItemEffect() const {
    StaticData::RetrieveItem(placedItem).ApplyEffect();
    auto ctx = Context::GetInstance();
    ctx->GetLogic()->UpdateHelpers();
}

uint16_t ItemLocation::GetPrice() const {
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
    return obtained;
}

void ItemLocation::MarkAsObtained() {
    obtained = true;
}

void ItemLocation::MarkAsNotObtained() {
    obtained = false;
}

bool ItemLocation::IsHintable() const {
    return isHintable;
}

void ItemLocation::SetAsHintable() {
    isHintable = true;
}

bool ItemLocation::IsHintedAt() const {
    return hintedAt;
}

void ItemLocation::SetAsHinted() {
    hintedAt = true;
}

const std::vector<RandomizerHintKey>& ItemLocation::GetHintedBy() const {
    return hintedBy;
}

void ItemLocation::AddHintedBy(const RandomizerHintKey hintKey) {
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
    for (const Option* location : Context::GetInstance()->GetSettings()->GetExcludeOptionsForGroup(loc->GetCollectionCheckGroup())) {
        if (location->GetName() == excludedOption.GetName()) {
            alreadyAdded = true;
        }
    }
    if (!alreadyAdded) {
        Context::GetInstance()->GetSettings()->GetExcludeOptionsForGroup(loc->GetCollectionCheckGroup()).push_back(&excludedOption);
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

bool ItemLocation::IsBarrenCandidate() const {
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
    hintedAt = false;
    hintedBy = {};
    price = 0;
    hasCustomPrice = false;
    hidden = false;
    wothCandidate = false;
    barrenCandidate = false;
    area = RA_NONE;
    obtained = false;
}
}
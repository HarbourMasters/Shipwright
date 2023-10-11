#include "item_location.h"

namespace Rando {
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

const RandoItem& ItemLocation::GetPlacedItem() const {
    return StaticData::RetrieveItem(placedItem);
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

void ItemLocation::SetParentRegion(RandomizerRegion region) {
    parentRegion = region;
}

RandomizerRegion ItemLocation::GetParentRegionKey() const {
    return parentRegion;
}

void ItemLocation::PlaceVanillaItem() {
    placedItem = StaticData::Location(rc)->GetVanillaItem();
}

void ItemLocation::ApplyPlacedItemEffect() {
    StaticData::RetrieveItem(placedItem).ApplyEffect();
}

uint16_t ItemLocation::GetPrice() const {
    if (StaticData::RetrieveItem(placedItem).GetItemType() == ITEMTYPE_SHOP) {
        return StaticData::RetrieveItem(placedItem).GetPrice();
    }
    return price;
}

void ItemLocation::SetPrice(uint16_t price_) {
    if (hasShopsanityPrice || hasScrubsanityPrice) {
        return;
    }
    price = price_;
}

bool ItemLocation::HasShopsanityPrice() const {
    return hasShopsanityPrice;
}

void ItemLocation::SetShopsanityPrice(uint16_t price_) {
    price = price_;
    hasShopsanityPrice = true;
}

bool ItemLocation::HasScrubsanityPrice() const {
    return hasScrubsanityPrice;
}

void ItemLocation::SetScrubsanityPrice(uint16_t price_) {
    price = price_;
    hasScrubsanityPrice = true;
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

RandomizerHintKey ItemLocation::GetHintKey() const {
    return hintedBy;
}

void ItemLocation::SetHintKey(RandomizerHintKey hintKey) {
    hintedBy = hintKey;
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

bool ItemLocation::IsVisible() const {
    return visibleInImGui;
}
void ItemLocation::SetVisible(bool visibleInImGui_) {
    visibleInImGui = visibleInImGui_;

}

ItemOverride_Key ItemLocation::Key() const {
    ItemOverride_Key key;
    key.all = 0;

    key.scene = StaticData::Location(rc)->GetScene();
    key.type = static_cast<uint8_t>(StaticData::Location(rc)->GetLocationType()); // TODO make sure these match up
    return key;
}

void ItemLocation::ResetVariables() {
    addedToPool = false;
    placedItem = RG_NONE;
    delayedItem = RG_NONE;
    isHintable = false;
    hintedAt = false;
    hintedBy = RH_NONE;
    price = 0;
    hasShopsanityPrice = false;
    hasScrubsanityPrice = false;
    hidden = false;
}
}
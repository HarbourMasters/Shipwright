#include "context.h"
#include "static_data.h"
#include "3drando/shops.hpp"
#include "3drando/dungeon.hpp"

#include <spdlog/spdlog.h>

namespace Rando {
std::weak_ptr<Context> Context::mContext;

Context::Context() {
    for (int i = 0; i < RC_MAX; i++) {
        itemLocationTable[i] = ItemLocation((RandomizerCheck)i);
    }
}

std::shared_ptr<Context> Context::CreateInstance() {
    if (mContext.expired()) {
        auto instance = std::make_shared<Context>();
        mContext = instance;
        return instance;
    }
    return GetInstance();
}

std::shared_ptr<Context> Context::GetInstance() {
    return mContext.lock();
}

Hint *Context::GetHint(RandomizerHintKey hintKey) {
    return &hintTable[hintKey];
}

void Context::AddHint(RandomizerHintKey hintId, Text text, RandomizerCheck hintedLocation, HintType hintType,
                      Text hintedRegion) {
    hintTable[hintId] = Hint(text, hintedLocation, hintType, hintedRegion);
    GetItemLocation(hintedLocation)->SetHintKey(hintId);
}

ItemLocation *Context::GetItemLocation(RandomizerCheck locKey) {
    return &(itemLocationTable[locKey]);
}

void Context::PlaceItemInLocation(RandomizerCheck locKey, RandomizerGet item, bool applyEffectImmediately,
                                  bool setHidden) {
    auto loc = GetItemLocation(locKey);
    SPDLOG_DEBUG("\n");
    SPDLOG_DEBUG(StaticData::RetrieveItem(item).GetName().GetEnglish());
    SPDLOG_DEBUG(" placed at ");
    SPDLOG_DEBUG(StaticData::GetLocation(locKey)->GetName());
    SPDLOG_DEBUG("\n\n");

    if (applyEffectImmediately || Settings::Logic.Is(LOGIC_NONE) || Settings::Logic.Is(LOGIC_VANILLA)) {
        Rando::StaticData::RetrieveItem(item).ApplyEffect();
    }

    //TODO? Show Progress

    // If we're placing a non-shop item in a shop location, we want to record it for custom messages
    if (StaticData::RetrieveItem(item).GetItemType() != ITEMTYPE_SHOP &&
        Rando::StaticData::GetLocation(locKey)->IsCategory(Category::cShop)) {
        int index = TransformShopIndex(GetShopIndex(locKey));
        NonShopItems[index].Name = Rando::StaticData::RetrieveItem(item).GetName();
        NonShopItems[index].Repurchaseable = Rando::StaticData::RetrieveItem(item).GetItemType() == ITEMTYPE_REFILL ||
                                             Rando::StaticData::RetrieveItem(item).GetHintKey() == RHT_PROGRESSIVE_BOMBCHUS;
    }

    loc->SetPlacedItem(item);
    if (setHidden) {
        loc->SetHidden(true);
    }
}

void Context::AddLocation(RandomizerCheck loc, std::vector<RandomizerCheck>* destination) {
    if (destination == nullptr) {
        destination = &allLocations;
    }
    destination->push_back(loc);
}

template<typename Container>
void Context::AddLocations(const Container& locations, std::vector<RandomizerCheck>* destination) {
    if (destination == nullptr) {
        destination = &allLocations;
    }
    destination->insert(destination->end(), std::cbegin(locations), std::cend(locations));
}

void Context::GenerateLocationPool() {
    allLocations.clear();
    AddLocation(RC_LINKS_POCKET);
    if (Settings::TriforceHunt.Is(TRIFORCE_HUNT_ON)) {
        AddLocation(RC_TRIFORCE_COMPLETED);
    }
    AddLocations(StaticData::overworldLocations);

    for (auto dungeon : Dungeon::dungeonList) {
        AddLocations(dungeon->GetDungeonLocations());
    }
}

void Context::AddExcludedOptions() {
    AddLocations(StaticData::overworldLocations, &everyPossibleLocation);
    for (auto dungeon : Dungeon::dungeonList) {
        AddLocations(dungeon->GetEveryLocation(), &everyPossibleLocation);
    }
    for (RandomizerCheck rc : everyPossibleLocation) {
        GetItemLocation(rc)->AddExcludeOption();
    }
}

std::vector<RandomizerCheck> Context::GetLocations(const std::vector<RandomizerCheck>& locationPool,
                                                   Category categoryInclude, Category categoryExclude) {
    std::vector<RandomizerCheck> locationsInCategory;
    for (RandomizerCheck locKey : locationPool) {
        if (StaticData::GetLocation(locKey)->IsCategory(categoryInclude) &&
            !StaticData::GetLocation(locKey)->IsCategory(categoryExclude)) {
            locationsInCategory.push_back(locKey);
        }
    }
    return locationsInCategory;
}

void Context::ItemReset() {
    for (RandomizerCheck il : allLocations) {
        GetItemLocation(il)->ResetVariables();
    }

    for (RandomizerCheck il : Rando::StaticData::dungeonRewardLocations) {
        GetItemLocation(il)->ResetVariables();
    }
}

void Context::LocationReset() {
    for (RandomizerCheck il : allLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (RandomizerCheck il : Rando::StaticData::dungeonRewardLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (RandomizerCheck il : Rando::StaticData::gossipStoneLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (RandomizerCheck il : Rando::StaticData::otherHintLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }
}

void Context::HintReset() {
    for (RandomizerCheck il : Rando::StaticData::gossipStoneLocations) {
        GetItemLocation(il)->ResetVariables();
        GetHint((RandomizerHintKey)(il - RC_DMC_GOSSIP_STONE + 1))->ResetVariables();
    }
}

void Context::CreateItemOverrides() {
    SPDLOG_DEBUG("NOW CREATING OVERRIDES\n\n");
    for (RandomizerCheck locKey : allLocations) {
        auto loc = Rando::StaticData::GetLocation(locKey);
        auto itemLoc = GetItemLocation(locKey);
        ItemOverride_Value val = Rando::StaticData::RetrieveItem(itemLoc->GetPlacedRandomizerGet()).Value();
        // If this is an ice trap, store the disguise model in iceTrapModels
        if (itemLoc->GetPlacedRandomizerGet() == RG_ICE_TRAP) {
            iceTrapModels[loc->GetRandomizerCheck()] = val.looksLikeItemId;
            // If this is ice trap is in a shop, change the name based on what the model will look like
            if (loc->IsCategory(Category::cShop)) {
                NonShopItems[TransformShopIndex(GetShopIndex(locKey))].Name = GetIceTrapName(val.looksLikeItemId);
            }
        }
        overrides.insert({
            .key = itemLoc->Key(),
            .value = val,
        });
        SPDLOG_DEBUG("\tScene: ");
        SPDLOG_DEBUG(std::to_string(itemLoc->Key().scene));
        SPDLOG_DEBUG("\tType: ");
        SPDLOG_DEBUG(std::to_string(itemLoc->Key().type));
        SPDLOG_DEBUG("\t");
        SPDLOG_DEBUG(loc->GetName());
        SPDLOG_DEBUG(": ");
        SPDLOG_DEBUG(itemLoc->GetPlacedItemName().GetEnglish());
        SPDLOG_DEBUG("\n");
    }
    SPDLOG_DEBUG("Overrides Created: ");
    SPDLOG_DEBUG(std::to_string(overrides.size()));
}
}

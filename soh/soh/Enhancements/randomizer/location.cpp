#include "location.hpp"
#include "3drando/hint_list.hpp"

RandomizerCheck Rando::Location::GetRandomizerCheck() const {
    return rc;
}

Rando::SpoilerCollectionCheck Rando::Location::GetCollectionCheck() const {
    return collectionCheck;
}

SpoilerCollectionCheckGroup Rando::Location::GetCollectionCheckGroup() const {
    return collectionCheckGroup;
}

SceneID Rando::Location::GetScene() const {
    return (SceneID)scene;
}

uint8_t Rando::Location::GetFlag() const {
    return flag;
}

const uint32_t Rando::Location::GetHintKey() const {
    return hintKey;
}

const std::string& Rando::Location::GetName() const {
    return name;
}

bool Rando::Location::IsCategory(Category category) const {
    return std::any_of(categories.begin(), categories.end(), [category](auto entry) { return entry == category; });
}

bool Rando::Location::IsDungeon() const {
    return (locationType != LocationType::GSToken &&
            (scene < SCENE_GANON_SONOGO || (scene > SCENE_TAKARAYA && scene < SCENE_GANON_FINAL))) ||
           (locationType == LocationType::GSToken && scene < SCENE_GANON);
}

bool Rando::Location::IsOverworld() const {
    return !IsDungeon();
}

bool Rando::Location::IsShop() const {
    return (scene >= SCENE_SHOP1 && scene <= SCENE_NIGHT_SHOP);
}

const uint32_t Rando::Location::Getuint32_t() const {
    return hintKey;
}

const HintText& Rando::Location::GetHint() const {
    return Hint(hintKey);
}

void Rando::Location::SetParentRegion(uint32_t region) {
    parentRegion = region;
}

uint32_t Rando::Location::GetParentRegionKey() const {
    return parentRegion;
}

Rando::Location
Rando::Location::Base(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
                      const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                      SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                      SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::Base, flag, std::move(name), hintKey, vanillaItem, std::move(categories),
                    collectionCheck, collectionCheckGroup);
}

Rando::Location
Rando::Location::Chest(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
                       const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                       SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::Chest, flag, std::move(name), hintKey, vanillaItem, std::move(categories),
                    SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_CHEST, scene, flag),
                    collectionCheckGroup);
}

Rando::Location
Rando::Location::Chest(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
                       const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                       SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                       SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::Chest, flag, std::move(name), hintKey, vanillaItem, std::move(categories),
                    collectionCheck, collectionCheckGroup);
}

Rando::Location Rando::Location::Collectable(
    RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
    const RandomizerGet vanillaItem, std::vector<Category>&& categories,
    SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(
        rc, scene, LocationType::Collectable, flag, std::move(name), hintKey, vanillaItem, std::move(categories),
        SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE, scene, flag), collectionCheckGroup);
}

Rando::Location Rando::Location::Collectable(
    RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
    const RandomizerGet vanillaItem, std::vector<Category>&& categories,
    SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
    SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::Collectable, flag, std::move(name), hintKey, vanillaItem,
                    std::move(categories), collectionCheck, collectionCheckGroup);
}

Rando::Location Rando::Location::GSToken(
    RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
    std::vector<Category>&& categories,
    SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::GSToken, flag, std::move(name), hintKey, RG_GOLD_SKULLTULA_TOKEN,
                    std::move(categories),
                    SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GOLD_SKULLTULA, scene, flag),
                    collectionCheckGroup);
}

Rando::Location Rando::Location::GrottoScrub(
    RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
    const RandomizerGet vanillaItem, std::vector<Category>&& categories,
    SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
    SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::GrottoScrub, flag, std::move(name), hintKey, vanillaItem,
                    std::move(categories), collectionCheck, collectionCheckGroup);
}

Rando::Location Rando::Location::Delayed(
    RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
    const RandomizerGet vanillaItem, std::vector<Category>&& categories,
    SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
    SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::Delayed, flag, std::move(name), hintKey, vanillaItem,
                    std::move(categories), collectionCheck, collectionCheckGroup);
}

Rando::Location Rando::Location::Reward(
    RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
    const RandomizerGet vanillaItem, std::vector<Category>&& categories,
    SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
    SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP) {
    return Location(rc, scene, LocationType::TempleReward, flag, std::move(name), hintKey, vanillaItem,
                    std::move(categories), collectionCheck, collectionCheckGroup);
}

Rando::Location Rando::Location::OtherHint(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name,
                                           std::vector<Category>&& categories) {
    return Location(rc, scene, LocationType::OtherHint, flag, std::move(name), NONE, RG_NONE, std::move(categories));
}

Rando::Location Rando::Location::HintStone(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name,
                                           std::vector<Category>&& categories) {
    return Location(rc, scene, LocationType::Base, flag, std::move(name), NONE, RG_NONE, std::move(categories));
}

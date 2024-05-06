#include "location.h"
#include "static_data.h"
#include <algorithm>

RandomizerCheck Rando::Location::GetRandomizerCheck() const {
    return rc;
}

Rando::SpoilerCollectionCheck Rando::Location::GetCollectionCheck() const {
    return collectionCheck;
}

SpoilerCollectionCheckGroup Rando::Location::GetCollectionCheckGroup() const {
    return collectionCheckGroup;
}

RandomizerCheckQuest Rando::Location::GetQuest() const {
    return quest;
}

RandomizerCheckArea Rando::Location::GetArea() const {
    return area;
}

RandomizerCheckType Rando::Location::GetRCType() const {
    return checkType;
}

ActorID Rando::Location::GetActorID() const {
    return actorId;
}

int32_t Rando::Location::GetActorParams() const {
    return actorParams;
}

SceneID Rando::Location::GetScene() const {
    return static_cast<SceneID>(scene);
}

uint8_t Rando::Location::GetFlag() const {
    return flag;
}

RandomizerHintTextKey Rando::Location::GetHintKey() const {
    return hintKey;
}

HintText* Rando::Location::GetHint() {
    return &StaticData::hintTextTable[hintKey];
}

const std::string& Rando::Location::GetName() const {
    return spoilerName;
}

const std::string& Rando::Location::GetShortName() const {
    return shortName;
}

Rando::LocationType Rando::Location::GetLocationType() const {
    return locationType;
}

bool Rando::Location::IsCategory(Category category) const {
    return std::any_of(categories.begin(), categories.end(), [category](auto entry) { return entry == category; });
}

bool Rando::Location::IsDungeon() const {
    return (locationType != LocationType::GSToken &&
            (scene < SCENE_GANONS_TOWER_COLLAPSE_INTERIOR ||
             (scene > SCENE_TREASURE_BOX_SHOP && scene < SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR))) ||
           (locationType == LocationType::GSToken && scene < SCENE_GANONS_TOWER);
}

bool Rando::Location::IsOverworld() const {
    return !IsDungeon();
}

bool Rando::Location::IsShop() const {
    return scene >= SCENE_BAZAAR && scene <= SCENE_BOMBCHU_SHOP;
}

bool Rando::Location::IsVanillaCompletion() const {
    return isVanillaCompletion;
}

uint32_t Rando::Location::Getuint32_t() const {
    return hintKey;
}

const HintText& Rando::Location::GetHint() const {
    return StaticData::hintTextTable[hintKey];
}

RandomizerGet Rando::Location::GetVanillaItem() const {
    return vanillaItem;
}

Rando::Location Rando::Location::Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                      RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_, int32_t actorParams_,
                                      uint8_t flag_, std::string&& shortName_, std::string&& spoilerName_,
                                      const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                      std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck,
                                      SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Base, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                      RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_, int32_t actorParams_,
                                      uint8_t flag_, std::string&& shortName_,
                                      const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                      std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck,
                                      SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Base, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                       RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_,
                                       int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                       std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                                       const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                       SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Chest, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_CHEST, scene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                       RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_,
                                       int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                       const RandomizerHintTextKey hintKey,
                                       const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                       SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Chest, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_CHEST, scene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                       RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_,
                                       int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                       std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                                       const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                       SpoilerCollectionCheck collectionCheck,
                                       SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Chest, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                       RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_,
                                       int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                       const RandomizerHintTextKey hintKey,
                                       const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                       SpoilerCollectionCheck collectionCheck,
                                       SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Chest, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Collectable(
    RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
    ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
    std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
    std::vector<Category>&& categories, SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Collectable, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_COLLECTABLE, scene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::Collectable(
    RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
    ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
    const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
    std::vector<Category>&& categories, SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Collectable, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_COLLECTABLE, scene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                             RandomizerCheckType checkType_, RandomizerCheckArea area_,
                                             ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                             std::string&& shortName_, std::string&& spoilerName_,
                                             const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             std::vector<Category>&& categories, const uint8_t collectFlag_,
                                             SpoilerCollectionCheckGroup collectionCheckGroup,
                                             bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Collectable, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_COLLECTABLE, scene_, collectFlag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                             RandomizerCheckType checkType_, RandomizerCheckArea area_,
                                             ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                             std::string&& shortName_,
                                             const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             std::vector<Category>&& categories, const uint8_t collectFlag_,
                                             SpoilerCollectionCheckGroup collectionCheckGroup,
                                             bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Collectable, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_COLLECTABLE, scene_, collectFlag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                             RandomizerCheckType checkType_, RandomizerCheckArea area_,
                                             ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                             std::string&& shortName_, std::string&& spoilerName_,
                                             const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck,
                                             SpoilerCollectionCheckGroup collectionCheckGroup,
                                             bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Collectable, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                             RandomizerCheckType checkType_, RandomizerCheckArea area_,
                                             ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                             std::string&& shortName_,
                                             const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck,
                                             SpoilerCollectionCheckGroup collectionCheckGroup,
                                             bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Collectable, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
                                         uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                                         std::vector<Category>&& categories,
                                         SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, RCTYPE_SKULL_TOKEN, area_, LocationType::GSToken, ACTOR_EN_SI, scene_, actorParams_,
                    flag_, std::move(shortName_), std::move(spoilerName_), hintKey, RG_GOLD_SKULLTULA_TOKEN,
                    std::move(categories), isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_GOLD_SKULLTULA, scene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
                                         uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         const RandomizerHintTextKey hintKey,
                                         std::vector<Category>&& categories,
                                         SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, RCTYPE_SKULL_TOKEN, area_, LocationType::GSToken, ACTOR_EN_SI, scene_, actorParams_,
                    flag_, std::move(shortName_), hintKey, RG_GOLD_SKULLTULA_TOKEN,
                    std::move(categories), isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_GOLD_SKULLTULA, scene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
                                         uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                                         std::vector<Category>&& categories, const uint8_t skullScene_,
                                         SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, RCTYPE_SKULL_TOKEN, area_, LocationType::GSToken, ACTOR_EN_SI, scene_, actorParams_,
                    flag_, std::move(shortName_), std::move(spoilerName_), hintKey, RG_GOLD_SKULLTULA_TOKEN,
                    std::move(categories), isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_GOLD_SKULLTULA, skullScene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
                                         uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         const RandomizerHintTextKey hintKey,
                                         std::vector<Category>&& categories, const uint8_t skullScene_,
                                         SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, RCTYPE_SKULL_TOKEN, area_, LocationType::GSToken, ACTOR_EN_SI, scene_, actorParams_,
                    flag_, std::move(shortName_), hintKey, RG_GOLD_SKULLTULA_TOKEN,
                    std::move(categories), isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_GOLD_SKULLTULA, skullScene_, flag_),
                    collectionCheckGroup};
}

Rando::Location Rando::Location::GrottoScrub(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                             RandomizerCheckType checkType_, RandomizerCheckArea area_,
                                             ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                             std::string&& shortName_, std::string&& spoilerName_,
                                             const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck,
                                             SpoilerCollectionCheckGroup collectionCheckGroup,
                                             bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::GrottoScrub, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::GrottoScrub(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                             RandomizerCheckType checkType_, RandomizerCheckArea area_,
                                             ActorID actorId_, uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                             std::string&& shortName_,
                                             const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck,
                                             SpoilerCollectionCheckGroup collectionCheckGroup,
                                             bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::GrottoScrub, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Delayed(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                         RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_,
                                         uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                                         const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                         SpoilerCollectionCheck collectionCheck,
                                         SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Delayed, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Delayed(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                         RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_,
                                         uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         const RandomizerHintTextKey hintKey,
                                         const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                         SpoilerCollectionCheck collectionCheck,
                                         SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::Delayed, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Reward(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                        RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_,
                                        int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                        std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                                        const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                        SpoilerCollectionCheck collectionCheck,
                                        SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::TempleReward, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::Reward(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                                        RandomizerCheckArea area_, ActorID actorId_, uint8_t scene_,
                                        int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                        const RandomizerHintTextKey hintKey,
                                        const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                                        SpoilerCollectionCheck collectionCheck,
                                        SpoilerCollectionCheckGroup collectionCheckGroup, bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::TempleReward, actorId_, scene_, actorParams_, flag_,
                    std::move(shortName_), hintKey, vanillaItem, std::move(categories),
                    isVanillaCompletion_, collectionCheck, collectionCheckGroup};
}

Rando::Location Rando::Location::OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                           RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_,
                                           uint8_t scene_,
                                           std::string&& shortName_, std::string&& spoilerName_,
                                           bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::OtherHint, actorId_, scene_, 0x00, 0x00,
                    std::move(shortName_), std::move(spoilerName_), RHT_NONE, RG_NONE, {}, isVanillaCompletion_};
}

Rando::Location Rando::Location::OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_,
                                           RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_,
                                           uint8_t scene_,
                                           std::string&& shortName_,
                                           bool isVanillaCompletion_) {
    return {rc, quest_, checkType_, area_, LocationType::OtherHint, actorId_, scene_, 0x00, 0x00,
                    std::move(shortName_), RHT_NONE, RG_NONE, {}, isVanillaCompletion_};
}

Rando::Location Rando::Location::HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
                                           uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                           std::string&& shortName_, std::string&& spoilerName_,
                                           std::vector<Category>&& categories, bool isVanillaCompletion_) {
    return {rc, quest_, RCTYPE_GOSSIP_STONE, area_, LocationType::Base, ACTOR_EN_GS, scene_, actorParams_, flag_,
                    std::move(shortName_), std::move(spoilerName_), RHT_NONE, RG_NONE, std::move(categories),
                    isVanillaCompletion_};
}

Rando::Location Rando::Location::HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
                                           uint8_t scene_, int32_t actorParams_, uint8_t flag_,
                                           std::string&& shortName_,
                                           std::vector<Category>&& categories, bool isVanillaCompletion_) {
    return {rc, quest_, RCTYPE_GOSSIP_STONE, area_, LocationType::Base, ACTOR_EN_GS, scene_, actorParams_, flag_,
                    std::move(shortName_), RHT_NONE, RG_NONE, std::move(categories),
                    isVanillaCompletion_};
}
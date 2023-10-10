#pragma once

#include <vector>

#include "3drando/spoiler_log.hpp"
#include "3drando/category.hpp"
#include "3drando/hints.hpp"

#include "z64item.h"
#include "randomizerTypes.h"
#include "z64actor_enum.h"
#include "z64scene.h"

namespace Rando {
typedef enum { RCQUEST_VANILLA, RCQUEST_MQ, RCQUEST_BOTH } RandomizerCheckQuest;
class SpoilerCollectionCheck {
  public:
    SpoilerCollectionCheckType type = SpoilerCollectionCheckType::SPOILER_CHK_NONE;
    uint8_t scene = 0;
    uint8_t flag = 0;

    SpoilerCollectionCheck() {
    }
    SpoilerCollectionCheck(SpoilerCollectionCheckType type_, uint8_t scene_, uint8_t flag_)
        : type(type_), scene(scene_), flag(flag_) {
    }

    static auto None() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_NONE, 0x00, 0x00);
    }

    static auto AlwaysCollected() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_ALWAYS_COLLECTED, 0x00, 0x00);
    }

    static auto ItemGetInf(uint8_t slot) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_ITEM_GET_INF, 0x00, slot);
    }

    static auto EventChkInf(uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_EVENT_CHK_INF, 0xFF, flag);
    }

    static auto InfTable(uint8_t offset, uint8_t bit) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_INF_TABLE, offset, bit);
    }

    static auto Collectable(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE, scene, flag);
    }

    static auto Chest(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_CHEST, scene, flag);
    }

    static auto Cow(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COW, scene, flag);
    }

    static auto Fishing(uint8_t bit) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MINIGAME, 0x00, bit);
    }

    static auto Scrub(uint8_t scene, uint8_t bit) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_SCRUB, scene, bit);
    }

    static auto GerudoToken() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GERUDO_MEMBERSHIP_CARD, 0x00, 0x00);
    }

    static auto BigPoePoints() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_POE_POINTS, 0x00, 0x00);
    }

    static auto Gravedigger(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GRAVEDIGGER, scene, flag);
    }

    static auto ShopItem(uint8_t scene, uint8_t itemSlot) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_SHOP_ITEM, scene, itemSlot);
    }

    static auto MagicBeans(uint8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MAGIC_BEANS, scene, flag);
    }

    static auto Merchant(int8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MERCHANT, scene, flag);
    }

    static auto RandomizerInf(int8_t scene, uint8_t flag) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF, scene, flag);
    }
};

enum class LocationType {
    Base,
    Chest,
    Collectable,
    GSToken,
    GrottoScrub,
    Delayed,
    TempleReward,
    HintStone,
    OtherHint,
};

class Location {
  public:
    Location() = default;
    Location(RandomizerCheck rc_, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
             RandomizerCheckArea area_, LocationType locationType_, ActorID actorId_, uint8_t scene_,
             int32_t actorParams_, uint8_t flag_, std::string shortName_, std::string spoilerName_,
             RandomizerHintTextKey hintKey_, RandomizerGet vanillaItem_, std::vector<Category> categories_,
             SpoilerCollectionCheck collectionCheck_ = SpoilerCollectionCheck(),
             SpoilerCollectionCheckGroup collectionCheckGroup_ = SpoilerCollectionCheckGroup::GROUP_NO_GROUP)
        : rc(rc_), quest(quest_), checkType(checkType_), area(area_), locationType(locationType_), actorId(actorId_),
          scene(scene_), actorParams(actorParams_), flag(flag_), shortName(std::move(shortName_)),
          spoilerName(spoilerName_), hintKey(hintKey_), vanillaItem(vanillaItem_), categories(std::move(categories_)),
          collectionCheck(collectionCheck_), collectionCheckGroup(collectionCheckGroup_) {
    }

    RandomizerCheck GetRandomizerCheck() const;
    SpoilerCollectionCheck GetCollectionCheck() const;
    SpoilerCollectionCheckGroup GetCollectionCheckGroup() const;
    SceneID GetScene() const;
    uint8_t GetFlag() const;
    RandomizerHintTextKey GetHintKey() const;
    HintText* GetHint();
    const std::string& GetName() const;
    const std::string& GetShortName() const;
    LocationType GetLocationType() const;
    bool IsCategory(Category category) const;
    bool IsDungeon() const;
    bool IsOverworld() const;
    bool IsShop() const;
    uint32_t Getuint32_t() const;
    const HintText& GetHint() const;
    RandomizerGet GetVanillaItem() const;

    static Location
    Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
         ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
         std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
         std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
         SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
          ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
          std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
          std::vector<Category>&& categories,
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
          ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
          std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
          std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_,
                std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_,
                std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
            ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
            std::string&& spoilerName_, const RandomizerHintTextKey hintKey, std::vector<Category>&& categories,
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    GrottoScrub(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_,
                std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Delayed(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
            ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
            std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
            std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Reward(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
           ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
           std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
           std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
           SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                              RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                              uint8_t flag_, std::string&& shortName_, std::string&& spoilerName_,
                              std::vector<Category>&& categories);

    static Location HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                              RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                              uint8_t flag_, std::string&& shortName_, std::string&& spoilerName_,
                              std::vector<Category>&& categories);

  private:
    RandomizerCheck rc;
    RandomizerCheckQuest quest;
    RandomizerCheckType checkType;
    RandomizerCheckArea area;
    ActorID actorId;
    uint8_t scene;
    int32_t actorParams;
    LocationType locationType;
    uint8_t flag;
    bool checked = false;
    std::string shortName;
    std::string spoilerName;
    RandomizerHintTextKey hintKey = RHT_NONE;
    RandomizerGet vanillaItem = RG_NONE;
    std::vector<Category> categories;
    SpoilerCollectionCheck collectionCheck;
    SpoilerCollectionCheckGroup collectionCheckGroup;
    bool isHintable = false;
};
} // namespace Rando
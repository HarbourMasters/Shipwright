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
             bool isVanillaCompletion_ = false, SpoilerCollectionCheck collectionCheck_ = SpoilerCollectionCheck(),
             SpoilerCollectionCheckGroup collectionCheckGroup_ = SpoilerCollectionCheckGroup::GROUP_NO_GROUP)
        : rc(rc_), quest(quest_), checkType(checkType_), area(area_), locationType(locationType_), actorId(actorId_),
          scene(scene_), actorParams(actorParams_), flag(flag_), shortName(std::move(shortName_)),
          spoilerName(spoilerName_), hintKey(hintKey_), vanillaItem(vanillaItem_), categories(std::move(categories_)),
          isVanillaCompletion(isVanillaCompletion_), collectionCheck(collectionCheck_),
          collectionCheckGroup(collectionCheckGroup_) {
    }

    RandomizerCheck GetRandomizerCheck() const;
    SpoilerCollectionCheck GetCollectionCheck() const;
    SpoilerCollectionCheckGroup GetCollectionCheckGroup() const;
    RandomizerCheckQuest GetQuest() const;
    RandomizerCheckArea GetArea() const;
    RandomizerCheckType GetRCType() const;
    ActorID GetActorID() const;
    int32_t GetActorParams() const;
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
    bool IsVanillaCompletion() const;
    uint32_t Getuint32_t() const;
    const HintText& GetHint() const;
    RandomizerGet GetVanillaItem() const;

    static Location Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                         RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                         uint8_t flag_, std::string&& shortName_, std::string&& spoilerName_,
                         const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                         std::vector<Category>&& categories,
                         SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                         SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
                         bool isVanillaCompletion_ = false);

    static Location
    Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
          ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
          std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
          std::vector<Category>&& categories,
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
          bool isVanillaCompletion_ = false);

    static Location
    Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
          ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
          std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
          std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
          bool isVanillaCompletion_ = false);

    static Location
    Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_,
                std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
                bool isVanillaCompletion_ = false);

    static Location
    Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_,
                std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories, uint8_t collectFlag_,
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
                bool isVanillaCompletion_ = false);

    static Location
    Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_,
                std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
                bool isVanillaCompletion_ = false);

    static Location
    GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
            uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
            std::string&& spoilerName_, const RandomizerHintTextKey hintKey, std::vector<Category>&& categories,
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
            bool isVanillaCompletion_ = true);

    /// @brief For certain scenes, the sceneId and the "Scene" in spoiler collection check later used to check the
    /// GS flags don't necessarily match. Use this constructor for those. scene_ should be the actual scene where
    /// the GS is located, skullScene_ is the value passed to GET_GS_FLAGS to get the correct skulltula. It is normal
    /// and expected that these don't always match, and the naming is a holdover from 3drando.
    /// @param rc 
    /// @param quest_ 
    /// @param area_ 
    /// @param scene_ 
    /// @param actorParams_ 
    /// @param flag_ 
    /// @param shortName_ 
    /// @param spoilerName_ 
    /// @param hintKey 
    /// @param categories 
    /// @param collectionCheckGroup 
    /// @param skullScene_ 
    /// @param isVanillaCompletion_ 
    /// @return
    static Location
    GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_,
            uint8_t scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
            std::string&& spoilerName_, const RandomizerHintTextKey hintKey, std::vector<Category>&& categories,
            uint8_t skullScene_,
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
            bool isVanillaCompletion_ = true);

    static Location
    GrottoScrub(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_,
                std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
                bool isVanillaCompletion_ = false);

    static Location
    Delayed(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
            ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
            std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
            std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP,
            bool isVanillaCompletion_ = false);

    static Location
    Reward(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
           ActorID actorId_, uint8_t scene, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
           std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
           std::vector<Category>&& categories, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
           SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP, bool isVanillaCompletion_ = false);

    static Location OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                              RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                              uint8_t flag_, std::string&& shortName_, std::string&& spoilerName_,
                              std::vector<Category>&& categories, bool isVanillaCompletion_ = false);

    static Location HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, uint8_t scene,
                              int32_t actorParams_, uint8_t flag_, std::string&& shortName_, std::string&& spoilerName_,
                              std::vector<Category>&& categories, bool isVanillaCompletion_ = false);

  private:
    RandomizerCheck rc;
    RandomizerCheckQuest quest;
    RandomizerCheckType checkType;
    RandomizerCheckArea area;
    LocationType locationType;
    ActorID actorId;
    uint8_t scene;
    int32_t actorParams;
    uint8_t flag;
    bool checked = false;
    std::string shortName;
    std::string spoilerName;
    RandomizerHintTextKey hintKey = RHT_NONE;
    RandomizerGet vanillaItem = RG_NONE;
    std::vector<Category> categories;
    bool isVanillaCompletion = false;
    SpoilerCollectionCheck collectionCheck;
    SpoilerCollectionCheckGroup collectionCheckGroup;
    bool isHintable = false;
};
} // namespace Rando
#pragma once

#include <vector>

#include "3drando/spoiler_log.hpp"
#include "3drando/category.hpp"
#include "3drando/hints.hpp"

#include "global.h"
#include "z64scene.h"
#include "z64item.h"
#include "randomizerTypes.h"

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

    static auto Biggoron(uint8_t mask) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_BIGGORON, 0x00, mask);
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
    Location(RandomizerCheck rc_, uint8_t scene_, LocationType locationType_, uint8_t flag_, std::string name_,
             uint32_t hintKey_, RandomizerGet vanillaItem_, std::vector<Category> categories_,
             SpoilerCollectionCheck collectionCheck_ = SpoilerCollectionCheck(),
             SpoilerCollectionCheckGroup collectionCheckGroup_ = SpoilerCollectionCheckGroup::GROUP_NO_GROUP)
        : rc(rc_), scene(scene_), locationType(locationType_), flag(flag_), name(std::move(name_)), hintKey(hintKey_),
          vanillaItem(vanillaItem_), categories(std::move(categories_)), collectionCheck(collectionCheck_),
          collectionCheckGroup(collectionCheckGroup_) {
    }

    RandomizerCheck GetRandomizerCheck() const;
    SpoilerCollectionCheck GetCollectionCheck() const;
    SpoilerCollectionCheckGroup GetCollectionCheckGroup() const;
    SceneID GetScene() const;
    uint8_t GetFlag() const;
    const uint32_t GetHintKey() const;
    const std::string& GetName() const;
    bool IsCategory(Category category) const;
    bool IsDungeon() const;
    bool IsOverworld() const;
    bool IsShop() const;
    const uint32_t Getuint32_t() const;
    const HintText& GetHint() const;
    void SetParentRegion(uint32_t region);
    uint32_t GetParentRegionKey() const;

    static Location
    Base(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
         const RandomizerGet vanillaItem, std::vector<Category>&& categories,
         SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
         SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Chest(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
          const RandomizerGet vanillaItem, std::vector<Category>&& categories,
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Chest(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
          const RandomizerGet vanillaItem, std::vector<Category>&& categories,
          SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Collectable(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Collectable(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    GSToken(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
            std::vector<Category>&& categories,
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    GrottoScrub(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Delayed(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
            const RandomizerGet vanillaItem, std::vector<Category>&& categories,
            SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Reward(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name, const uint32_t hintKey,
           const RandomizerGet vanillaItem, std::vector<Category>&& categories,
           SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
           SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location OtherHint(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name,
                              std::vector<Category>&& categories);

    static Location HintStone(RandomizerCheck rc, uint8_t scene, uint8_t flag, std::string&& name,
                              std::vector<Category>&& categories);

  private:
    RandomizerCheck rc;
    uint8_t scene;
    LocationType locationType;
    uint8_t flag;
    bool checked = false;
    std::string name;
    uint32_t hintKey = NONE;
    RandomizerGet vanillaItem = RG_NONE;
    std::vector<Category> categories;
    SpoilerCollectionCheck collectionCheck;
    SpoilerCollectionCheckGroup collectionCheckGroup;
    bool isHintable = false;
    uint32_t parentRegion = NONE;
};
} // namespace Rando
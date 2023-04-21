#pragma once

#include <vector>

#include "3drando/spoiler_log.hpp"
#include "3drando/category.hpp"
#include "3drando/hints.hpp"

#include "z64item.h"
#include "randomizerTypes.h"
#include "z64actor_enum.h"
#include "z64scene_enum.h"

namespace Rando {
typedef enum {
    RCTYPE_STANDARD,                   // Base set of rando checks
    RCTYPE_SKULL_TOKEN,                // Gold Skulltulas
    RCTYPE_COW,                        // Cows
    RCTYPE_ADULT_TRADE,                // Adult trade quest checks
    RCTYPE_FROG_SONG,                  // Frog song purple rupee checks
    RCTYPE_MAP_COMPASS,                // Maps/Compasses
    RCTYPE_SMALL_KEY,                  // Small Keys
    RCTYPE_GF_KEY,                     // Gerudo Fortress Keys
    RCTYPE_BOSS_KEY,                   // Boss Keys
    RCTYPE_GANON_BOSS_KEY,             // Ganon's boss key
    RCTYPE_SHOP,                       // shops
    RCTYPE_SCRUB,                      // scrubs
    RCTYPE_MERCHANT,                   // merchants
    RCTYPE_CHEST_GAME,                 // todo replace this once we implement it, just using it to exclude for now
    RCTYPE_LINKS_POCKET,               // todo this feels hacky
    RCTYPE_GOSSIP_STONE,
    RCTYPE_SONG_LOCATION,              // Song locations
    RCTYPE_BOSS_HEART_OR_OTHER_REWARD, // Boss heart container or lesser dungeon rewards (lens, ice arrow)
    RCTYPE_DUNGEON_REWARD,             // Dungeon rewards (blue warps)
    RCTYPE_OCARINA,                    // Ocarina locations
} RandomizerCheckType;

typedef enum { RCQUEST_VANILLA, RCQUEST_MQ, RCQUEST_BOTH } RandomizerCheckQuest;

typedef enum {
    RCAREA_KOKIRI_FOREST,
    RCAREA_LOST_WOODS,
    RCAREA_SACRED_FOREST_MEADOW,
    RCAREA_HYRULE_FIELD,
    RCAREA_LAKE_HYLIA,
    RCAREA_GERUDO_VALLEY,
    RCAREA_GERUDO_FORTRESS,
    RCAREA_WASTELAND,
    RCAREA_DESERT_COLOSSUS,
    RCAREA_MARKET,
    RCAREA_HYRULE_CASTLE,
    RCAREA_KAKARIKO_VILLAGE,
    RCAREA_GRAVEYARD,
    RCAREA_DEATH_MOUNTAIN_TRAIL,
    RCAREA_GORON_CITY,
    RCAREA_DEATH_MOUNTAIN_CRATER,
    RCAREA_ZORAS_RIVER,
    RCAREA_ZORAS_DOMAIN,
    RCAREA_ZORAS_FOUNTAIN,
    RCAREA_LON_LON_RANCH,
    RCAREA_DEKU_TREE,
    RCAREA_DODONGOS_CAVERN,
    RCAREA_JABU_JABUS_BELLY,
    RCAREA_FOREST_TEMPLE,
    RCAREA_FIRE_TEMPLE,
    RCAREA_WATER_TEMPLE,
    RCAREA_SPIRIT_TEMPLE,
    RCAREA_SHADOW_TEMPLE,
    RCAREA_BOTTOM_OF_THE_WELL,
    RCAREA_ICE_CAVERN,
    RCAREA_GERUDO_TRAINING_GROUND,
    RCAREA_GANONS_CASTLE,
    // If adding any more areas, Check Tracker will need a refactor
    RCAREA_INVALID
} RandomizerCheckArea;

class SpoilerCollectionCheck {
  public:
    SpoilerCollectionCheckType type = SpoilerCollectionCheckType::SPOILER_CHK_NONE;
    uint8_t scene = 0;

    SpoilerCollectionCheck() {
    }
    SpoilerCollectionCheck(SpoilerCollectionCheckType type_, uint8_t scene_)
        : type(type_), scene(scene_) {
    }

    static auto None() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_NONE, 0x00);
    }

    static auto AlwaysCollected() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_ALWAYS_COLLECTED, 0x00);
    }

    static auto ItemGetInf() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_ITEM_GET_INF, 0x00);
    }

    static auto EventChkInf() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_EVENT_CHK_INF, 0xFF);
    }

    static auto InfTable(uint8_t offset) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_INF_TABLE, offset);
    }

    static auto Collectable(uint8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE, scene);
    }

    static auto Chest(uint8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_CHEST, scene);
    }

    static auto Cow(uint8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_COW, scene);
    }

    static auto Fishing() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MINIGAME, 0x00);
    }

    static auto Scrub(uint8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_SCRUB, scene);
    }

    static auto Biggoron() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_BIGGORON, 0x00);
    }

    static auto GerudoToken() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GERUDO_MEMBERSHIP_CARD, 0x00);
    }

    static auto BigPoePoints() {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_POE_POINTS, 0x00);
    }

    static auto Gravedigger(uint8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_GRAVEDIGGER, scene);
    }

    static auto ShopItem(uint8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_SHOP_ITEM, scene);
    }

    static auto MagicBeans(uint8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MAGIC_BEANS, scene);
    }

    static auto Merchant(int8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_MERCHANT, scene);
    }

    static auto RandomizerInf(int8_t scene) {
        return SpoilerCollectionCheck(SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF, scene);
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
             int32_t actorParams_, std::string shortName_, std::string spoilerName_, uint32_t hintKey_,
             RandomizerGet vanillaItem_, std::vector<Category> categories_,
             SpoilerCollectionCheck collectionCheck_ = SpoilerCollectionCheck(),
             SpoilerCollectionCheckGroup collectionCheckGroup_ = SpoilerCollectionCheckGroup::GROUP_NO_GROUP)
        : rc(rc_), quest(quest_), checkType(checkType_), area(area_), locationType(locationType_), actorId(actorId_),
          scene(scene_), actorParams(actorParams_), shortName(std::move(shortName_)), spoilerName(spoilerName_),
          hintKey(hintKey_), vanillaItem(vanillaItem_), categories(std::move(categories_)),
          collectionCheck(collectionCheck_), collectionCheckGroup(collectionCheckGroup_) {
    }

    RandomizerCheck GetRandomizerCheck() const;
    SpoilerCollectionCheck GetCollectionCheck() const;
    SpoilerCollectionCheckGroup GetCollectionCheckGroup() const;
    SceneID GetScene() const;
    const uint32_t GetHintKey() const;
    const std::string& GetName() const;
    const std::string& GetShortName() const;
    bool IsCategory(Category category) const;
    bool IsDungeon() const;
    bool IsOverworld() const;
    bool IsShop() const;
    const uint32_t Getuint32_t() const;
    const HintText& GetHint() const;
    void SetParentRegion(uint32_t region);
    uint32_t GetParentRegionKey() const;

    static Location
    Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
         ActorID actorId_, uint8_t scene, int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_,
         const uint32_t hintKey, const RandomizerGet vanillaItem, std::vector<Category>&& categories,
         SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
         SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
          ActorID actorId_, uint8_t scene, int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_,
          const uint32_t hintKey, const RandomizerGet vanillaItem, std::vector<Category>&& categories,
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
          ActorID actorId_, uint8_t scene, int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_,
          const uint32_t hintKey, const RandomizerGet vanillaItem, std::vector<Category>&& categories,
          SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
          SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                std::string&& shortName_, std::string&& spoilerName_, const uint32_t hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                std::string&& shortName_, std::string&& spoilerName_, const uint32_t hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
            ActorID actorId_, uint8_t scene, int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_,
            const uint32_t hintKey, std::vector<Category>&& categories,
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    GrottoScrub(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                std::string&& shortName_, std::string&& spoilerName_, const uint32_t hintKey,
                const RandomizerGet vanillaItem, std::vector<Category>&& categories,
                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Delayed(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
            ActorID actorId_, uint8_t scene, int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_,
            const uint32_t hintKey, const RandomizerGet vanillaItem, std::vector<Category>&& categories,
            SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
            SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location
    Reward(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_,
           ActorID actorId_, uint8_t scene, int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_,
           const uint32_t hintKey, const RandomizerGet vanillaItem, std::vector<Category>&& categories,
           SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
           SpoilerCollectionCheckGroup collectionCheckGroup = SpoilerCollectionCheckGroup::GROUP_NO_GROUP);

    static Location OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                              RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                              std::string&& shortName_, std::string&& spoilerName_, std::vector<Category>&& categories);

    static Location HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_,
                              RandomizerCheckArea area_, ActorID actorId_, uint8_t scene, int32_t actorParams_,
                              std::string&& shortName_, std::string&& spoilerName_, std::vector<Category>&& categories);

  private:
    RandomizerCheck rc;
    RandomizerCheckQuest quest;
    RandomizerCheckType checkType;
    RandomizerCheckArea area;
    ActorID actorId;
    uint8_t scene;
    int32_t actorParams;
    LocationType locationType;
    bool checked = false;
    std::string shortName;
    std::string spoilerName;
    uint32_t hintKey = NONE;
    RandomizerGet vanillaItem = RG_NONE;
    std::vector<Category> categories;
    SpoilerCollectionCheck collectionCheck;
    SpoilerCollectionCheckGroup collectionCheckGroup;
    bool isHintable = false;
    uint32_t parentRegion = NONE;
};
} // namespace Rando
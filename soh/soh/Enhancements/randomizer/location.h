#pragma once

#include <utility>
#include <vector>

#include "3drando/spoiler_log.hpp"
#include "3drando/hints.hpp"

#include "randomizerTypes.h"
#include "z64actor_enum.h"
#include "z64scene.h"
#include "../../util.h"

namespace Rando {
class SpoilerCollectionCheck {
  public:
    SpoilerCollectionCheckType type = SPOILER_CHK_NONE;
    uint8_t scene = 0;
    uint16_t flag = 0;

    SpoilerCollectionCheck() = default;
    SpoilerCollectionCheck(const SpoilerCollectionCheckType type_, const uint8_t scene_, const uint16_t flag_) : type(type_), scene(scene_), flag(flag_) {}

    static auto ItemGetInf(const uint8_t slot) {
        return SpoilerCollectionCheck(SPOILER_CHK_ITEM_GET_INF, 0x00, slot);
    }

    static auto EventChkInf(const uint8_t flag) {
        return SpoilerCollectionCheck(SPOILER_CHK_EVENT_CHK_INF, 0xFF, flag);
    }

    static auto InfTable(const uint16_t flag) {
        return SpoilerCollectionCheck(SPOILER_CHK_INF_TABLE, 0xFF, flag);
    }

    static auto Collectable(const uint8_t scene, const uint8_t flag) {
        return SpoilerCollectionCheck(SPOILER_CHK_COLLECTABLE, scene, flag);
    }

    static auto Chest(const uint8_t scene, const uint8_t flag) {
        return SpoilerCollectionCheck(SPOILER_CHK_CHEST, scene, flag);
    }

    static auto RandomizerInf(const uint16_t flag) {
        return SpoilerCollectionCheck(SPOILER_CHK_RANDOMIZER_INF, 0x00, flag);
    }
};

class Location {
  public:
  //RANDOTODO fix wacky ordering
    Location() : rc(RC_UNKNOWN_CHECK), quest(RCQUEST_BOTH), checkType(RCTYPE_STANDARD), area(RCAREA_INVALID), actorId(ACTOR_ID_MAX), scene(SCENE_ID_MAX), actorParams(0),
                 hintKey(RHT_NONE), vanillaItem(RG_NONE), isVanillaCompletion(false), collectionCheck(SpoilerCollectionCheck()) {}

    Location(const RandomizerCheck rc_, const RandomizerCheckQuest quest_, const RandomizerCheckType checkType_, const RandomizerCheckArea area_, const ActorID actorId_,
             const SceneID scene_, const int32_t actorParams_, std::string shortName_, std::string spoilerName_, const RandomizerHintTextKey hintKey_,
             const RandomizerGet vanillaItem_, const bool isVanillaCompletion_ = false, const SpoilerCollectionCheck collectionCheck_ = SpoilerCollectionCheck(),
             const int vanillaPrice_ = 0)
        : rc(rc_), quest(quest_), checkType(checkType_), area(area_), actorId(actorId_),
          scene(scene_), actorParams(actorParams_), shortName(std::move(shortName_)),
          spoilerName(std::move(spoilerName_)), hintKey(hintKey_), vanillaItem(vanillaItem_),
          isVanillaCompletion(isVanillaCompletion_), collectionCheck(collectionCheck_), vanillaPrice(vanillaPrice_) {}

    Location(const RandomizerCheck rc_, const RandomizerCheckQuest quest_, const RandomizerCheckType checkType_, const RandomizerCheckArea area_, const ActorID actorId_,
             const SceneID scene_, const int32_t actorParams_, std::string shortName_, const RandomizerHintTextKey hintKey_, const RandomizerGet vanillaItem_,
             const bool isVanillaCompletion_ = false, const SpoilerCollectionCheck collectionCheck_ = SpoilerCollectionCheck(),
             const int vanillaPrice_ = 0)
        : rc(rc_), quest(quest_), checkType(checkType_), area(area_), actorId(actorId_), scene(scene_), actorParams(actorParams_), shortName(shortName_),
          spoilerName(SpoilerNameFromShortName(shortName_, area_)), hintKey(hintKey_), vanillaItem(vanillaItem_), isVanillaCompletion(isVanillaCompletion_),
          collectionCheck(collectionCheck_), vanillaPrice(vanillaPrice_) {}

    static std::string SpoilerNameFromShortName(std::string shortName, RandomizerCheckArea area) {
        if (area < 0 || area >= RCAREA_INVALID) {
            return shortName;
        }
        return SohUtils::GetRandomizerCheckAreaPrefix(area) + " " + shortName;
    }

    RandomizerCheck GetRandomizerCheck() const;
    SpoilerCollectionCheck GetCollectionCheck() const;
    RandomizerCheckQuest GetQuest() const;
    RandomizerCheckArea GetArea() const;
    RandomizerCheckType GetRCType() const;
    ActorID GetActorID() const;
    int32_t GetActorParams() const;
    SceneID GetScene() const;
    RandomizerHintTextKey GetHintKey() const;
    HintText* GetHint();
    const std::string& GetName() const;
    const std::string& GetShortName() const;
    bool IsDungeon() const;
    bool IsOverworld() const;
    bool IsShop() const;
    bool IsVanillaCompletion() const;
    uint32_t Getuint32_t() const;
    const HintText& GetHint() const;
    RandomizerGet GetVanillaItem() const;
    int16_t GetVanillaPrice() const;

    static Location Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                         std::string&& shortName_, std::string&& spoilerName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem,
                         SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), bool isVanillaCompletion_ = false, uint16_t vanillaPrice_ = 0);

    static Location Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                         std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                         bool isVanillaCompletion_ = false, uint16_t vanillaPrice_ = 0);

    static Location Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                         int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem,
                         SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), bool isVanillaCompletion_ = false, uint16_t vanillaPrice_ = 0);

    static Location Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                         int32_t actorParams_, std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem,
                         SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), bool isVanillaCompletion_ = false, uint16_t vanillaPrice_ = 0);

    static Location Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                          int32_t actorParams_, uint8_t flag_, std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem, bool isVanillaCompletion_ = false);

    static Location Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                          int32_t actorParams_, std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem, SpoilerCollectionCheck collectionCheck,
                          bool isVanillaCompletion_ = false);

    static Location Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_, uint8_t flag_,
                          std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem, bool isVanillaCompletion_ = false);

    static Location Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                          std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem, SpoilerCollectionCheck collectionCheck,
                          bool isVanillaCompletion_ = false);

    static Location Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_, uint8_t flag_,
                                std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem, bool isVanillaCompletion_ = false);

    static Location Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                                int32_t actorParams_, uint8_t flag_, std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem,
                                bool isVanillaCompletion_ = false);

    static Location Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                                std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem, SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(),
                                bool isVanillaCompletion_ = false);

    static Location Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_,
                                SceneID scene_, int32_t actorParams_, std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem,
                                SpoilerCollectionCheck collectionCheck = SpoilerCollectionCheck(), bool isVanillaCompletion_ = false);

    static Location GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, SceneID scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                            RandomizerHintTextKey hintKey);

    /// @brief For certain scenes, the sceneId and the "Scene" in spoiler collection check later used to check the
    /// GS flags don't necessarily match. Use this constructor (or the next one) for those. scene_ should be the actual scene where
    /// the GS is located, skullScene_ is the value passed to GET_GS_FLAGS to get the correct skulltula. It is normal
    /// and expected that these don't always match, and the naming is a holdover from 3drando.
    /// @param rc 
    /// @param quest_ 
    /// @param scene_ 
    /// @param actorParams_ 
    /// @param flag_ 
    /// @param shortName_ 
    /// @param hintKey 
    /// @param skullScene_ 
    /// @return
    static Location GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, SceneID scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                            RandomizerHintTextKey hintKey, uint8_t skullScene_);

    static Location GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, SceneID scene_, int32_t actorParams_, uint8_t flag_,
                            std::string&& shortName_, RandomizerHintTextKey hintKey, uint8_t skullScene_);

    static Location Fish(RandomizerCheck rc, RandomizerCheckQuest quest_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                         RandomizerInf flag_, std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem);

    static Location GrottoFish(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, int32_t actorParams_,
                         RandomizerInf flag_, std::string&& shortName_, RandomizerHintTextKey hintKey);

    static Location OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_, std::string&& shortName_,
                              std::string&& spoilerName_);

    static Location OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, ActorID actorId_, SceneID scene_, std::string&& shortName_);

    static Location OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_, std::string&& shortName_);

    static Location HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, SceneID scene_, int32_t actorParams_, std::string&& shortName_);

    static Location HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, SceneID scene_, int32_t actorParams_, std::string&& shortName_);

  private:
    RandomizerCheck rc;
    RandomizerCheckQuest quest;
    RandomizerCheckType checkType;
    RandomizerCheckArea area;
    ActorID actorId;
    SceneID scene;
    int32_t actorParams;
    bool checked = false;
    std::string shortName;
    std::string spoilerName;
    RandomizerHintTextKey hintKey;
    RandomizerGet vanillaItem;
    bool isVanillaCompletion;
    SpoilerCollectionCheck collectionCheck;
    int16_t vanillaPrice;
    bool isHintable = false;
};
} // namespace Rando
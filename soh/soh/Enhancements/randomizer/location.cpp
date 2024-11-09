#include "location.h"
#include "static_data.h"
#include <algorithm>
#include <assert.h>

RandomizerCheck Rando::Location::GetRandomizerCheck() const {
    return rc;
}

Rando::SpoilerCollectionCheck Rando::Location::GetCollectionCheck() const {
    return collectionCheck;
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
    return scene;
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

bool Rando::Location::IsDungeon() const {
    return (checkType != RCTYPE_SKULL_TOKEN &&
            (scene < SCENE_THIEVES_HIDEOUT || scene == SCENE_INSIDE_GANONS_CASTLE ||
             (scene > SCENE_TREASURE_BOX_SHOP && scene < SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR))) ||
           (checkType == RCTYPE_SKULL_TOKEN && scene < SCENE_GANONS_TOWER);
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

int16_t Rando::Location::GetVanillaPrice() const {
    return vanillaPrice;
}

RandomizerCheckArea GetAreaFromScene(uint8_t scene) {
    switch (scene) {
        case SCENE_LINKS_HOUSE:
        case SCENE_KOKIRI_FOREST:
        case SCENE_KOKIRI_SHOP:
        case SCENE_MIDOS_HOUSE:
        case SCENE_KNOW_IT_ALL_BROS_HOUSE:
        case SCENE_TWINS_HOUSE:
        case SCENE_SARIAS_HOUSE:
            return RCAREA_KOKIRI_FOREST;

        case SCENE_LOST_WOODS:
            return RCAREA_LOST_WOODS;

        case SCENE_SACRED_FOREST_MEADOW:
            return RCAREA_SACRED_FOREST_MEADOW;

        case SCENE_HYRULE_FIELD:
            return RCAREA_HYRULE_FIELD;

        case SCENE_LAKE_HYLIA:
        case SCENE_FISHING_POND:
        case SCENE_LAKESIDE_LABORATORY:
            return RCAREA_LAKE_HYLIA;

        case SCENE_GERUDO_VALLEY:
        case SCENE_CARPENTERS_TENT:
            return RCAREA_GERUDO_VALLEY;

        case SCENE_GERUDOS_FORTRESS:
        case SCENE_THIEVES_HIDEOUT:
            return RCAREA_GERUDO_FORTRESS;

        case SCENE_HAUNTED_WASTELAND:
            return RCAREA_WASTELAND;

        case SCENE_DESERT_COLOSSUS:
            return RCAREA_DESERT_COLOSSUS;

        case SCENE_MARKET_ENTRANCE_DAY:
        case SCENE_MARKET_ENTRANCE_NIGHT:
        case SCENE_MARKET_ENTRANCE_RUINS:
        case SCENE_BACK_ALLEY_DAY:
        case SCENE_BACK_ALLEY_NIGHT:
        case SCENE_BACK_ALLEY_HOUSE:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS:
        case SCENE_TREASURE_BOX_SHOP:
        case SCENE_BOMBCHU_BOWLING_ALLEY:
        case SCENE_DOG_LADY_HOUSE:
        case SCENE_MARKET_GUARD_HOUSE:
        case SCENE_POTION_SHOP_MARKET:
        case SCENE_BOMBCHU_SHOP:
        case SCENE_HAPPY_MASK_SHOP:
        case SCENE_TEMPLE_OF_TIME:
            return RCAREA_MARKET;

        case SCENE_HYRULE_CASTLE:
        case SCENE_CASTLE_COURTYARD_GUARDS_DAY:
        case SCENE_CASTLE_COURTYARD_GUARDS_NIGHT:
        case SCENE_CASTLE_COURTYARD_ZELDA:
        case SCENE_OUTSIDE_GANONS_CASTLE:
            return RCAREA_HYRULE_CASTLE;

        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
        case SCENE_HOUSE_OF_SKULLTULA:
        case SCENE_POTION_SHOP_GRANNY:
        case SCENE_IMPAS_HOUSE:
        case SCENE_POTION_SHOP_KAKARIKO:
        case SCENE_TEST01:
            return RCAREA_KAKARIKO_VILLAGE;

        case SCENE_GRAVEYARD:
        case SCENE_ROYAL_FAMILYS_TOMB:
        case SCENE_GRAVEKEEPERS_HUT:
            return RCAREA_GRAVEYARD;

        case SCENE_DEATH_MOUNTAIN_TRAIL:
            return RCAREA_DEATH_MOUNTAIN_TRAIL;

        case SCENE_GORON_CITY:
        case SCENE_GORON_SHOP:
            return RCAREA_GORON_CITY;

        case SCENE_DEATH_MOUNTAIN_CRATER:
            return RCAREA_DEATH_MOUNTAIN_CRATER;

        case SCENE_ZORAS_RIVER:
            return RCAREA_ZORAS_RIVER;

        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORA_SHOP:
            return RCAREA_ZORAS_DOMAIN;

        case SCENE_ZORAS_FOUNTAIN:
            return RCAREA_ZORAS_FOUNTAIN;

        case SCENE_LON_LON_RANCH:
        case SCENE_LON_LON_BUILDINGS:
        case SCENE_STABLE:
            return RCAREA_LON_LON_RANCH;

        case SCENE_DEKU_TREE:
        case SCENE_DEKU_TREE_BOSS:
            return RCAREA_DEKU_TREE;

        case SCENE_DODONGOS_CAVERN:
        case SCENE_DODONGOS_CAVERN_BOSS:
            return RCAREA_DODONGOS_CAVERN;

        case SCENE_JABU_JABU:
        case SCENE_JABU_JABU_BOSS:
            return RCAREA_JABU_JABUS_BELLY;

        case SCENE_FOREST_TEMPLE:
        case SCENE_FOREST_TEMPLE_BOSS:
            return RCAREA_FOREST_TEMPLE;

        case SCENE_FIRE_TEMPLE:
        case SCENE_FIRE_TEMPLE_BOSS:
            return RCAREA_FIRE_TEMPLE;

        case SCENE_WATER_TEMPLE:
        case SCENE_WATER_TEMPLE_BOSS:
            return RCAREA_WATER_TEMPLE;

        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SPIRIT_TEMPLE_BOSS:
            return RCAREA_SPIRIT_TEMPLE;

        case SCENE_SHADOW_TEMPLE:
        case SCENE_SHADOW_TEMPLE_BOSS:
            return RCAREA_SHADOW_TEMPLE;

        case SCENE_BOTTOM_OF_THE_WELL:
            return RCAREA_BOTTOM_OF_THE_WELL;

        case SCENE_ICE_CAVERN:
            return RCAREA_ICE_CAVERN;

        case SCENE_GERUDO_TRAINING_GROUND:
            return RCAREA_GERUDO_TRAINING_GROUND;

        case SCENE_INSIDE_GANONS_CASTLE:
        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
        case SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR:
        case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
        case SCENE_GANONS_TOWER:
        case SCENE_GANON_BOSS:
            return RCAREA_GANONS_CASTLE;
        default:
            assert(false);
            return RCAREA_INVALID;
    }
}


Rando::Location Rando::Location::Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                                      std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                      SpoilerCollectionCheck collectionCheck, bool isVanillaCompletion_, uint16_t vanillaPrice_) {
    return { rc, quest_, checkType_, GetAreaFromScene(scene_), actorId_, scene_, actorParams_, std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem,
                    isVanillaCompletion_, collectionCheck, vanillaPrice_ };
}

Rando::Location Rando::Location::Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                                      int32_t actorParams_, std::string&& shortName_, std::string&& spoilerName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                      SpoilerCollectionCheck collectionCheck, bool isVanillaCompletion_, uint16_t vanillaPrice_) {
    return { rc, quest_, checkType_, area_, actorId_, scene_, actorParams_, std::move(shortName_), std::move(spoilerName_), hintKey, vanillaItem, isVanillaCompletion_, collectionCheck, vanillaPrice_ };
}

Rando::Location Rando::Location::Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                                      std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem, SpoilerCollectionCheck collectionCheck,
                                      bool isVanillaCompletion_, uint16_t vanillaPrice_) {
    return { rc, quest_, checkType_, GetAreaFromScene(scene_), actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_, collectionCheck , vanillaPrice_ };
}

Rando::Location Rando::Location::Base(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                                      int32_t actorParams_, std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                      SpoilerCollectionCheck collectionCheck, bool isVanillaCompletion_, uint16_t vanillaPrice_) {
    return { rc, quest_, checkType_, area_, actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_, collectionCheck, vanillaPrice_  };
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                                       uint8_t flag_, std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem, bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, GetAreaFromScene(scene_), actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_CHEST, scene_, flag_) };
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                                       int32_t actorParams_, uint8_t flag_, std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                       bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, area_, actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_CHEST, scene_, flag_) };
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                                       std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem, SpoilerCollectionCheck collectionCheck,
                                       bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, GetAreaFromScene(scene_), actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_, collectionCheck };
}

Rando::Location Rando::Location::Chest(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                                       int32_t actorParams_, std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                       SpoilerCollectionCheck collectionCheck, bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, area_, actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_, collectionCheck };
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                                             uint8_t flag_, std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem, bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, GetAreaFromScene(scene_), actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_COLLECTABLE, scene_, flag_) };
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                                             int32_t actorParams_, uint8_t flag_, std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, area_, actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_,
                    SpoilerCollectionCheck(SPOILER_CHK_COLLECTABLE, scene_, flag_) };
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, ActorID actorId_, SceneID scene_, int32_t actorParams_,
                                             std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem, SpoilerCollectionCheck collectionCheck,
                                             bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, GetAreaFromScene(scene_), actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_, collectionCheck };
}

Rando::Location Rando::Location::Collectable(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckType checkType_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_,
                                             int32_t actorParams_, std::string&& shortName_, const RandomizerHintTextKey hintKey, const RandomizerGet vanillaItem,
                                             SpoilerCollectionCheck collectionCheck, bool isVanillaCompletion_) {
    return { rc, quest_, checkType_, area_, actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, isVanillaCompletion_, collectionCheck };
}

Rando::Location Rando::Location::GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, SceneID scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         const RandomizerHintTextKey hintKey) {
    return { rc, quest_, RCTYPE_SKULL_TOKEN, GetAreaFromScene(scene_), ACTOR_EN_SI, scene_, actorParams_, std::move(shortName_), hintKey, RG_GOLD_SKULLTULA_TOKEN, true,
                    SpoilerCollectionCheck(SPOILER_CHK_GOLD_SKULLTULA, scene_, flag_) };
}

Rando::Location Rando::Location::GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, SceneID scene_, int32_t actorParams_, uint8_t flag_, std::string&& shortName_,
                                         const RandomizerHintTextKey hintKey, const uint8_t skullScene_) {
    return { rc, quest_, RCTYPE_SKULL_TOKEN, GetAreaFromScene(scene_), ACTOR_EN_SI, scene_, actorParams_, std::move(shortName_), hintKey, RG_GOLD_SKULLTULA_TOKEN, true,
                    SpoilerCollectionCheck(SPOILER_CHK_GOLD_SKULLTULA, skullScene_, flag_) };
}

Rando::Location Rando::Location::GSToken(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, SceneID scene_, int32_t actorParams_, uint8_t flag_,
                                         std::string&& shortName_, const RandomizerHintTextKey hintKey, const uint8_t skullScene_) {
    return { rc, quest_, RCTYPE_SKULL_TOKEN, area_, ACTOR_EN_SI, scene_, actorParams_, std::move(shortName_), hintKey, RG_GOLD_SKULLTULA_TOKEN, true,
                    SpoilerCollectionCheck(SPOILER_CHK_GOLD_SKULLTULA, skullScene_, flag_) };
}

Rando::Location Rando::Location::OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_, std::string&& shortName_,
                                           std::string&& spoilerName_) {
    return { rc, quest_, RCTYPE_STATIC_HINT, area_, actorId_, scene_, 0x00, std::move(shortName_), std::move(spoilerName_), RHT_NONE, RG_NONE, false };
}

Rando::Location Rando::Location::OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, ActorID actorId_, SceneID scene_, std::string&& shortName_) {
    return { rc, quest_, RCTYPE_STATIC_HINT, GetAreaFromScene(scene_), actorId_, scene_, 0x00, std::move(shortName_), RHT_NONE, RG_NONE, false };
}

Rando::Location Rando::Location::OtherHint(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, ActorID actorId_, SceneID scene_, std::string&& shortName_) {
    return { rc, quest_, RCTYPE_STATIC_HINT, area_, actorId_, scene_, 0x00, std::move(shortName_), RHT_NONE, RG_NONE, false };
}

Rando::Location Rando::Location::HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, SceneID scene_, int32_t actorParams_, std::string&& shortName_) {
    return { rc, quest_, RCTYPE_GOSSIP_STONE, GetAreaFromScene(scene_), ACTOR_EN_GS, scene_, actorParams_, std::move(shortName_), RHT_NONE, RG_NONE, false };
}

Rando::Location Rando::Location::Fish(RandomizerCheck rc, RandomizerCheckQuest quest_, ActorID actorId_, SceneID scene_, int32_t actorParams_, RandomizerInf flag_,
                                      std::string&& shortName_, RandomizerHintTextKey hintKey, RandomizerGet vanillaItem) {
    return {rc, quest_, RCTYPE_FISH, GetAreaFromScene(scene_), actorId_, scene_, actorParams_, std::move(shortName_), hintKey, vanillaItem, false,
            SpoilerCollectionCheck(SPOILER_CHK_RANDOMIZER_INF, scene_, flag_)};
}

Rando::Location Rando::Location::GrottoFish(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, int32_t actorParams_, RandomizerInf flag_,
                                      std::string&& shortName_, RandomizerHintTextKey hintKey) {
    return {rc, quest_, RCTYPE_FISH, area_, ACTOR_EN_FISH, SCENE_GROTTOS, actorParams_, std::move(shortName_), hintKey, RG_FISH, false,
            SpoilerCollectionCheck(SPOILER_CHK_RANDOMIZER_INF, SCENE_GROTTOS, flag_)};
}

Rando::Location Rando::Location::HintStone(RandomizerCheck rc, RandomizerCheckQuest quest_, RandomizerCheckArea area_, SceneID scene_, int32_t actorParams_, std::string&& shortName_) {
    return { rc, quest_, RCTYPE_GOSSIP_STONE, area_, ACTOR_EN_GS, scene_, actorParams_, std::move(shortName_), RHT_NONE, RG_NONE, false };
}

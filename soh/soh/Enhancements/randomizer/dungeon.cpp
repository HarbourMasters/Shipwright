#include "dungeon.h"

#include "3drando/pool_functions.hpp"
#include "static_data.h"
#include "context.h"

namespace Rando {
DungeonInfo::DungeonInfo(std::string name_, const RandomizerHintTextKey hintKey_, const RandomizerGet map_,
                         const RandomizerGet compass_, const RandomizerGet smallKey_, const RandomizerGet keyRing_,
                         const RandomizerGet bossKey_, RandomizerArea area_, const uint8_t vanillaKeyCount_,
                         const uint8_t mqKeyCount_, const RandomizerSettingKey mqSetting_)
    : name(std::move(name_)), hintKey(hintKey_), map(map_), compass(compass_), smallKey(smallKey_), keyRing(keyRing_),
      bossKey(bossKey_), area(area_), vanillaKeyCount(vanillaKeyCount_), mqKeyCount(mqKeyCount_),
      mqSetting(mqSetting_) {
}
DungeonInfo::DungeonInfo()
    : hintKey(RHT_NONE), map(RG_NONE), compass(RG_NONE), smallKey(RG_NONE), keyRing(RG_NONE), bossKey(RG_NONE) {
}
DungeonInfo::~DungeonInfo() = default;

const std::string& DungeonInfo::GetName() const {
    return name;
}

void DungeonInfo::SetMQ() {
    masterQuest = true;
}

void DungeonInfo::ClearMQ() {
    masterQuest = false;
}

bool DungeonInfo::IsMQ() const {
    return masterQuest;
}

void DungeonInfo::SetKeyRing() {
    hasKeyRing = true;
}

void DungeonInfo::ClearKeyRing() {
    hasKeyRing = false;
}

bool DungeonInfo::HasKeyRing() const {
    return hasKeyRing;
}

bool DungeonInfo::IsVanilla() const {
    return !masterQuest;
}

uint8_t DungeonInfo::GetSmallKeyCount() const {
    return masterQuest ? mqKeyCount : vanillaKeyCount;
}

RandomizerHintTextKey DungeonInfo::GetHintKey() const {
    return hintKey;
}

RandomizerArea DungeonInfo::GetArea() const {
    return area;
}

RandomizerGet DungeonInfo::GetSmallKey() const {
    return smallKey;
}

RandomizerGet DungeonInfo::GetKeyRing() const {
    return keyRing;
}

RandomizerGet DungeonInfo::GetMap() const {
    return map;
}

RandomizerGet DungeonInfo::GetCompass() const {
    return compass;
}

RandomizerGet DungeonInfo::GetBossKey() const {
    return bossKey;
}

RandomizerSettingKey DungeonInfo::GetMQSetting() const {
    return mqSetting;
}

void DungeonInfo::SetDungeonKnown(bool known) {
    isDungeonModeKnown = known;
}

void DungeonInfo::PlaceVanillaMap() const {
    if (map == RG_NONE) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto mapLocation = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_MAP;
    })[0];
    Context::GetInstance()->PlaceItemInLocation(mapLocation, map);
}

void DungeonInfo::PlaceVanillaCompass() const {
    if (compass == RG_NONE) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto compassLocation = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_COMPASS;
    })[0];
    Context::GetInstance()->PlaceItemInLocation(compassLocation, compass);
}

void DungeonInfo::PlaceVanillaBossKey() const {
    if (bossKey == RG_NONE || bossKey == RG_GANONS_CASTLE_BOSS_KEY) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto bossKeyLocation = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_BOSS_KEY;
    })[0];
    Context::GetInstance()->PlaceItemInLocation(bossKeyLocation, bossKey);
}

void DungeonInfo::PlaceVanillaSmallKeys() const {
    if (smallKey == RG_NONE) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto smallKeyLocations = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_SMALL_KEY;
    });
    for (const auto location : smallKeyLocations) {
        Context::GetInstance()->PlaceItemInLocation(location, smallKey);
    }
}

// Gets the chosen dungeon locations for a playthrough (so either MQ or Vanilla)
std::vector<RandomizerCheck> DungeonInfo::GetDungeonLocations() const {
    return locations;
}

Dungeons::Dungeons() {
    dungeonList[DEKU_TREE] = DungeonInfo("Deku Tree", RHT_DEKU_TREE, RG_DEKU_TREE_MAP, RG_DEKU_TREE_COMPASS, RG_NONE,
                                         RG_NONE, RG_NONE, RA_DEKU_TREE, 0, 0, RSK_MQ_DEKU_TREE);
    dungeonList[DODONGOS_CAVERN] =
        DungeonInfo("Dodongo's Cavern", RHT_DODONGOS_CAVERN, RG_DODONGOS_CAVERN_MAP, RG_DODONGOS_CAVERN_COMPASS,
                    RG_NONE, RG_NONE, RG_NONE, RA_DODONGOS_CAVERN, 0, 0, RSK_MQ_DODONGOS_CAVERN);
    dungeonList[JABU_JABUS_BELLY] =
        DungeonInfo("Jabu Jabu's Belly", RHT_JABU_JABUS_BELLY, RG_JABU_JABUS_BELLY_MAP, RG_JABU_JABUS_BELLY_COMPASS,
                    RG_NONE, RG_NONE, RG_NONE, RA_JABU_JABUS_BELLY, 0, 0, RSK_MQ_JABU_JABU);
    dungeonList[FOREST_TEMPLE] = DungeonInfo(
        "Forest Temple", RHT_FOREST_TEMPLE, RG_FOREST_TEMPLE_MAP, RG_FOREST_TEMPLE_COMPASS, RG_FOREST_TEMPLE_SMALL_KEY,
        RG_FOREST_TEMPLE_KEY_RING, RG_FOREST_TEMPLE_BOSS_KEY, RA_FOREST_TEMPLE, 5, 6, RSK_MQ_FOREST_TEMPLE);
    dungeonList[FIRE_TEMPLE] = DungeonInfo("Fire Temple", RHT_FIRE_TEMPLE, RG_FIRE_TEMPLE_MAP, RG_FIRE_TEMPLE_COMPASS,
                                           RG_FIRE_TEMPLE_SMALL_KEY, RG_FIRE_TEMPLE_KEY_RING, RG_FIRE_TEMPLE_BOSS_KEY,
                                           RA_FIRE_TEMPLE, 8, 5, RSK_MQ_FIRE_TEMPLE);
    dungeonList[WATER_TEMPLE] = DungeonInfo(
        "Water Temple", RHT_WATER_TEMPLE, RG_WATER_TEMPLE_MAP, RG_WATER_TEMPLE_COMPASS, RG_WATER_TEMPLE_SMALL_KEY,
        RG_WATER_TEMPLE_KEY_RING, RG_WATER_TEMPLE_BOSS_KEY, RA_WATER_TEMPLE, 6, 2, RSK_MQ_WATER_TEMPLE);
    dungeonList[SPIRIT_TEMPLE] = DungeonInfo(
        "Spirit Temple", RHT_SPIRIT_TEMPLE, RG_SPIRIT_TEMPLE_MAP, RG_SPIRIT_TEMPLE_COMPASS, RG_SPIRIT_TEMPLE_SMALL_KEY,
        RG_SPIRIT_TEMPLE_KEY_RING, RG_SPIRIT_TEMPLE_BOSS_KEY, RA_SPIRIT_TEMPLE, 5, 7, RSK_MQ_SPIRIT_TEMPLE);
    dungeonList[SHADOW_TEMPLE] = DungeonInfo(
        "Shadow Temple", RHT_SHADOW_TEMPLE, RG_SHADOW_TEMPLE_MAP, RG_SHADOW_TEMPLE_COMPASS, RG_SHADOW_TEMPLE_SMALL_KEY,
        RG_SHADOW_TEMPLE_KEY_RING, RG_SHADOW_TEMPLE_BOSS_KEY, RA_SHADOW_TEMPLE, 5, 6, RSK_MQ_SHADOW_TEMPLE);
    dungeonList[BOTTOM_OF_THE_WELL] =
        DungeonInfo("Bottom of the Well", RHT_BOTTOM_OF_THE_WELL, RG_BOTTOM_OF_THE_WELL_MAP,
                    RG_BOTTOM_OF_THE_WELL_COMPASS, RG_BOTTOM_OF_THE_WELL_SMALL_KEY, RG_BOTTOM_OF_THE_WELL_KEY_RING,
                    RG_NONE, RA_BOTTOM_OF_THE_WELL, 3, 2, RSK_MQ_BOTTOM_OF_THE_WELL);
    dungeonList[ICE_CAVERN] = DungeonInfo("Ice Cavern", RHT_ICE_CAVERN, RG_ICE_CAVERN_MAP, RG_ICE_CAVERN_COMPASS,
                                          RG_NONE, RG_NONE, RG_NONE, RA_ICE_CAVERN, 0, 0, RSK_MQ_ICE_CAVERN);
    dungeonList[GERUDO_TRAINING_GROUND] = DungeonInfo(
        "Gerudo Training Ground", RHT_GERUDO_TRAINING_GROUND, RG_NONE, RG_NONE, RG_GERUDO_TRAINING_GROUND_SMALL_KEY,
        RG_GERUDO_TRAINING_GROUND_KEY_RING, RG_NONE, RA_GERUDO_TRAINING_GROUND, 9, 3, RSK_MQ_GTG);
    dungeonList[GANONS_CASTLE] =
        DungeonInfo("Ganon's Castle", RHT_GANONS_CASTLE, RG_NONE, RG_NONE, RG_GANONS_CASTLE_SMALL_KEY,
                    RG_GANONS_CASTLE_KEY_RING, RG_GANONS_CASTLE_BOSS_KEY, RA_GANONS_CASTLE, 2, 3, RSK_MQ_GANONS_CASTLE);
}

Dungeons::~Dungeons() = default;

DungeonInfo* Dungeons::GetDungeon(const DungeonKey key) {
    return &dungeonList[key];
}

DungeonInfo* Dungeons::GetDungeonFromScene(const uint16_t scene) {
    switch (scene) {
        case SCENE_DEKU_TREE:
            return &dungeonList[DEKU_TREE];
        case SCENE_DODONGOS_CAVERN:
            return &dungeonList[DODONGOS_CAVERN];
        case SCENE_JABU_JABU:
            return &dungeonList[JABU_JABUS_BELLY];
        case SCENE_FOREST_TEMPLE:
            return &dungeonList[FOREST_TEMPLE];
        case SCENE_FIRE_TEMPLE:
            return &dungeonList[FIRE_TEMPLE];
        case SCENE_WATER_TEMPLE:
            return &dungeonList[WATER_TEMPLE];
        case SCENE_SPIRIT_TEMPLE:
            return &dungeonList[SPIRIT_TEMPLE];
        case SCENE_SHADOW_TEMPLE:
            return &dungeonList[SHADOW_TEMPLE];
        case SCENE_BOTTOM_OF_THE_WELL:
            return &dungeonList[BOTTOM_OF_THE_WELL];
        case SCENE_ICE_CAVERN:
            return &dungeonList[ICE_CAVERN];
        case SCENE_GERUDO_TRAINING_GROUND:
            return &dungeonList[GERUDO_TRAINING_GROUND];
        case SCENE_INSIDE_GANONS_CASTLE:
            return &dungeonList[GANONS_CASTLE];
        default:
            return nullptr;
    }
}

size_t Dungeons::CountMQ() {
    size_t count = 0;
    for (DungeonInfo& dungeon : dungeonList) {
        if (dungeon.IsMQ()) {
            count++;
        }
    }
    return count;
}

void Dungeons::ClearAllMQ() {
    for (DungeonInfo& dungeon : dungeonList) {
        dungeon.ClearMQ();
    }
}

std::array<DungeonInfo*, 12> Dungeons::GetDungeonList() {
    std::array<DungeonInfo*, 12> dungeonList_{};
    for (size_t i = 0; i < dungeonList.size(); i++) {
        dungeonList_[i] = &dungeonList[i];
    }
    return dungeonList_;
}

size_t Dungeons::GetDungeonListSize() const {
    return dungeonList.size();
}

void Dungeons::ParseJson(nlohmann::json spoilerFileJson) {
    nlohmann::json mqDungeonsJson = spoilerFileJson["masterQuestDungeons"];

    for (auto& dungeon : dungeonList) {
        dungeon.ClearMQ();

        if (std::find(mqDungeonsJson.begin(), mqDungeonsJson.end(), dungeon.GetName()) != mqDungeonsJson.end()) {
            dungeon.SetMQ();
        }
    }
}
} // namespace Rando

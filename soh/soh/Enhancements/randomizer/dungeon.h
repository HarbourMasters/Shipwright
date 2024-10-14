#pragma once

#include "randomizerTypes.h"

#include <array>
#include <vector>
#include <string>
#include "nlohmann/json.hpp"

namespace Rando {
class DungeonInfo {
  public:
    DungeonInfo(std::string name_, RandomizerHintTextKey hintKey_, RandomizerGet map_, RandomizerGet compass_,
                RandomizerGet smallKey_, RandomizerGet keyRing_, RandomizerGet bossKey_, RandomizerArea area_, 
                uint8_t vanillaKeyCount_, uint8_t mqKeyCount_, RandomizerSettingKey mqSetting_,
                std::vector<RandomizerCheck> vanillaLocations_,
                std::vector<RandomizerCheck> mqLocations_, std::vector<RandomizerCheck> sharedLocations_,
                std::vector<RandomizerCheck> bossRoomLocations_);
    DungeonInfo();
    ~DungeonInfo();

    const std::string& GetName() const;
    void SetMQ();
    void ClearMQ();
    bool IsMQ() const;
    void SetKeyRing();
    void ClearKeyRing();
    bool HasKeyRing() const;
    bool IsVanilla() const;
    uint8_t GetSmallKeyCount() const;
    RandomizerHintTextKey GetHintKey() const;
    RandomizerArea GetArea() const;
    RandomizerGet GetSmallKey() const;
    RandomizerGet GetKeyRing() const;
    RandomizerGet GetMap() const;
    RandomizerGet GetCompass() const;
    RandomizerGet GetBossKey() const;
    RandomizerSettingKey GetMQSetting() const;
    void SetDungeonKnown(bool known);
    void PlaceVanillaMap() const;
    void PlaceVanillaCompass() const;
    void PlaceVanillaBossKey() const;
    void PlaceVanillaSmallKeys() const;
    std::vector<RandomizerCheck> GetDungeonLocations() const;
    std::vector<RandomizerCheck> GetEveryLocation() const;

  private:
    std::string name;
    RandomizerHintTextKey hintKey;
    RandomizerArea area;
    RandomizerGet map;
    RandomizerGet compass;
    RandomizerGet smallKey;
    RandomizerGet keyRing;
    RandomizerGet bossKey;
    RandomizerSettingKey mqSetting;
    bool isDungeonModeKnown = true;
    uint8_t vanillaKeyCount{};
    uint8_t mqKeyCount{};
    bool masterQuest = false;
    bool hasKeyRing = false;
    std::vector<RandomizerCheck> vanillaLocations;
    std::vector<RandomizerCheck> mqLocations;
    std::vector<RandomizerCheck> sharedLocations;
    std::vector<RandomizerCheck> bossRoomLocations;
};

typedef enum {
    DEKU_TREE,
    DODONGOS_CAVERN,
    JABU_JABUS_BELLY,
    FOREST_TEMPLE,
    FIRE_TEMPLE,
    WATER_TEMPLE,
    SPIRIT_TEMPLE,
    SHADOW_TEMPLE,
    BOTTOM_OF_THE_WELL,
    ICE_CAVERN,
    GERUDO_TRAINING_GROUNDS,
    GANONS_CASTLE
} DungeonKey;

class Dungeons {
  public:
    Dungeons();
    ~Dungeons();
    DungeonInfo* GetDungeon(DungeonKey key);
    DungeonInfo* GetDungeonFromScene(uint16_t scene);
    size_t CountMQ();
    void ClearAllMQ();
    /// @brief Returns a new array of pointers to the DungeonInfo entries.
    /// It returns an array of pointers rather than a pointer to an array so that
    /// this new array can be shuffled for the purposes of randomizing MQ dungeons.
    /// If you want an individual DungeonInfo entry you should use the GetDungeon
    /// function from either here or the Context class.
    /// @return 
    std::array<DungeonInfo*, 12> GetDungeonList();
    size_t GetDungeonListSize() const;
    void ParseJson(nlohmann::json spoilerFileJson);
  private:
    std::array<DungeonInfo, 12> dungeonList;
};
}
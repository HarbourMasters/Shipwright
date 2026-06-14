#pragma once

#include "randomizerTypes.h"

#include <array>
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include "z64save.h"
#include "z64scene.h"

namespace Rando {
class DungeonInfo {
  public:
    DungeonInfo(std::string name_, RandomizerHintTextKey hintKey_, RandomizerGet map_, RandomizerGet compass_,
                RandomizerGet smallKey_, RandomizerGet keyRing_, RandomizerGet bossKey_, RandomizerGet reward_,
                RandomizerArea area_, uint8_t vanillaKeyCount_, uint8_t mqKeyCount_, RandomizerSettingKey mqSetting_,
                SceneID scene_, std::vector<uint8_t> vanillaDoorFlags_, std::vector<uint8_t> randoDoorFlags_,
                std::vector<uint8_t> MQDoorFlags_);
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
    RandomizerGet GetReward() const;
    int8_t GetUsedSmallKeys(SaveContext* saveContext) const;
    int8_t GetCurrentSmallKeys(SaveContext* saveContext) const;
    int8_t GetTotalSmallKeys(SaveContext* saveContext) const;
    RandomizerSettingKey GetMQSetting() const;
    const std::vector<uint8_t>* GetDoorFlags() const;
    void SetDungeonKnown(bool known);
    void PlaceVanillaMap() const;
    void PlaceVanillaCompass() const;
    void PlaceVanillaBossKey() const;
    void PlaceVanillaSmallKeys() const;
    std::vector<RandomizerCheck> GetDungeonLocations() const;
    std::vector<RandomizerCheck> locations;

  private:
    std::string name;
    RandomizerHintTextKey hintKey;
    RandomizerGet map;
    RandomizerGet compass;
    RandomizerGet smallKey;
    RandomizerGet keyRing;
    RandomizerGet bossKey;
    RandomizerGet reward;
    RandomizerArea area;
    uint8_t vanillaKeyCount{};
    uint8_t mqKeyCount{};
    RandomizerSettingKey mqSetting;
    bool isDungeonModeKnown = true;
    bool masterQuest = false;
    bool hasKeyRing = false;
    SceneID scene;
    std::vector<uint8_t> vanillaDoorFlags;
    // Specifically non-MQ Rando, to handle an edge case in water temple
    std::vector<uint8_t> randoDoorFlags;
    std::vector<uint8_t> MQDoorFlags;
};

int8_t FindUsedSmallKeys(const SaveContext* saveContext, const SceneID scene, const std::vector<uint8_t>* DoorFlags);
int8_t FindCurrentSmallKeys(const SaveContext* saveContext, const SceneID scene);
int8_t FindTotalSmallKeys(const SaveContext* saveContext, const SceneID scene, const std::vector<uint8_t>* DoorFlags);

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
    GERUDO_TRAINING_GROUND,
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
} // namespace Rando

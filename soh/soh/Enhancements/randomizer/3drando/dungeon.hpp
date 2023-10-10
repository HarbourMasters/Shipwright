#pragma once

#include <array>
#include <string>
#include <vector>

#include "keys.hpp"
#include "../randomizerTypes.h"

namespace Dungeon {
class DungeonInfo {
public:
  DungeonInfo(std::string name_, RandomizerHintTextKey hintKey_, RandomizerGet map_, RandomizerGet compass_, RandomizerGet smallKey_,
              RandomizerGet keyRing_, RandomizerGet bossKey_,
              uint8_t vanillaKeyCount_, uint8_t mqKeyCount_,
                std::vector<RandomizerCheck> vanillaLocations_,
                std::vector<RandomizerCheck> mqLocations_,
                std::vector<RandomizerCheck> sharedLocations_,
                std::vector<RandomizerCheck> bossRoomLocations_);
    ~DungeonInfo();

    const std::string& GetName() const {
        return name;
    }

    void SetMQ() {
        masterQuest = true;
    }

    void ClearMQ() {
        masterQuest = false;
    }

    bool IsMQ() const {
        return masterQuest;
    }

    void SetKeyRing() {
        hasKeyRing = true;
    }

    void ClearKeyRing() {
        hasKeyRing = false;
    }

    bool HasKeyRing() const {
        return hasKeyRing;
    }

    bool IsVanilla() const {
        return !masterQuest;
    }

    uint8_t GetSmallKeyCount() const {
        return (masterQuest) ? mqKeyCount : vanillaKeyCount;
    }

    RandomizerHintTextKey GetHintKey() const;
    RandomizerGet GetSmallKey() const;
    RandomizerGet GetKeyRing() const;
    RandomizerGet GetMap() const;
    RandomizerGet GetCompass() const;
    RandomizerGet GetBossKey() const;

    void PlaceVanillaMap();
    void PlaceVanillaCompass();
    void PlaceVanillaBossKey();
    void PlaceVanillaSmallKeys();

    // Gets the chosen dungeon locations for a playthrough (so either MQ or Vanilla)
    std::vector<RandomizerCheck> GetDungeonLocations() const;

    // Gets all dungeon locations (MQ + Vanilla)
    std::vector<RandomizerCheck> GetEveryLocation() const;

private:
    std::string name;
    RandomizerHintTextKey hintKey;
    RandomizerGet map;
    RandomizerGet compass;
    RandomizerGet smallKey;
    RandomizerGet keyRing;
    RandomizerGet bossKey;
    uint8_t vanillaKeyCount;
    uint8_t mqKeyCount;
    bool masterQuest = false;
    bool hasKeyRing = false;
    std::vector<RandomizerCheck> vanillaLocations;
    std::vector<RandomizerCheck> mqLocations;
    std::vector<RandomizerCheck> sharedLocations;
    std::vector<RandomizerCheck> bossRoomLocations;
};

extern DungeonInfo DekuTree;
extern DungeonInfo DodongosCavern;
extern DungeonInfo JabuJabusBelly;
extern DungeonInfo ForestTemple;
extern DungeonInfo FireTemple;
extern DungeonInfo WaterTemple;
extern DungeonInfo SpiritTemple;
extern DungeonInfo ShadowTemple;
extern DungeonInfo BottomOfTheWell;
extern DungeonInfo IceCavern;
extern DungeonInfo GerudoTrainingGrounds;
extern DungeonInfo GanonsCastle;

using DungeonArray = std::array<DungeonInfo*, 12>;

extern const DungeonArray dungeonList;
} // namespace Dungeon

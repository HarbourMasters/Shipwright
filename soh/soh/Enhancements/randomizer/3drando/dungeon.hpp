#pragma once

#include <array>
#include <string>
#include <vector>

#include "keys.hpp"

namespace Dungeon {
class DungeonInfo {
public:
    DungeonInfo(std::string name_, uint32_t hintKey_, uint32_t map_, uint32_t compass_, uint32_t smallKey_, uint32_t keyRing_, uint32_t bossKey_,
              uint8_t vanillaKeyCount_, uint8_t mqKeyCount_,
                std::vector<uint32_t> vanillaLocations_,
                std::vector<uint32_t> mqLocations_,
                std::vector<uint32_t> sharedLocations_,
                std::vector<uint32_t> bossRoomLocations_);
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

    uint32_t GetHintKey() const;
    uint32_t GetSmallKey() const;
    uint32_t GetKeyRing() const;
    uint32_t GetMap() const;
    uint32_t GetCompass() const;
    uint32_t GetBossKey() const;

    void PlaceVanillaMap();
    void PlaceVanillaCompass();
    void PlaceVanillaBossKey();
    void PlaceVanillaSmallKeys();

    // Gets the chosen dungeon locations for a playthrough (so either MQ or Vanilla)
    std::vector<uint32_t> GetDungeonLocations() const;

    // Gets all dungeon locations (MQ + Vanilla)
    std::vector<uint32_t> GetEveryLocation() const;

private:
    std::string name;
    uint32_t hintKey;
    uint32_t map;
    uint32_t compass;
    uint32_t smallKey;
    uint32_t keyRing;
    uint32_t bossKey;
    uint8_t vanillaKeyCount;
    uint8_t mqKeyCount;
    bool masterQuest = false;
    bool hasKeyRing = false;
    std::vector<uint32_t> vanillaLocations;
    std::vector<uint32_t> mqLocations;
    std::vector<uint32_t> sharedLocations;
    std::vector<uint32_t> bossRoomLocations;
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

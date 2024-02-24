#pragma once

#include <array>
#include <unordered_map>
#include "randomizerTypes.h"
#include "item.h"
#include "location.h"

namespace Rando {
struct Position{
    SceneID scene;
    Vec3f pos;
};

struct Identifier {
    SceneID scene;
    RandomizerCheckQuest quest;
    int16_t params;
};
/**
 * @brief Singleton for storing and accessing static Randomizer-related data
 * 
 * The StaticData class is a class for storing and accessing static Randomizer-related data.
 * Static Data, in this context, refers to data that is the same for every single randomizer seed,
 * i.e. the item and location tables. This class is intended to be used as a Singleton.
 */
class StaticData {
    private:
      static std::array<Item, RG_MAX> itemTable;
      static std::array<Location, RC_MAX> locationTable;

    public:
      static void InitItemTable();
      static Item& RetrieveItem(const RandomizerGet rgid);
      static Item& ItemFromGIID(const int giid);
      static std::array<Item, RG_MAX>& GetItemTable();
      static void InitLocationTable();
      static Location* GetLocation(RandomizerCheck locKey);
      static std::array<Rando::Location, RC_MAX>& GetLocationTable();
      static std::vector<RandomizerCheck> overworldLocations;
      static std::vector<RandomizerCheck> dungeonRewardLocations;
      static std::vector<std::vector<RandomizerCheck>> shopLocationLists;
      static std::vector<RandomizerCheck> scrubLocations;
      static std::vector<RandomizerCheck> gossipStoneLocations;
      static std::vector<RandomizerCheck> otherHintLocations;
      static std::vector<RandomizerCheck> pondFishLocations;
      static std::vector<RandomizerCheck> overworldFishLocations;
      static std::array<std::pair<RandomizerCheck, RandomizerCheck>, 17> randomizerFishingPondFish;
      static std::unordered_map<int8_t, RandomizerCheck> randomizerGrottoFishMap;
      static std::unordered_map<Position, RandomizerCheck> silverRupeeMap;
      static std::unordered_map<Identifier, RandomizerGet> silverTrackerMap;
      StaticData();
      ~StaticData();
};
}

namespace std {
    template<>
    struct hash<Rando::Position> {
        inline size_t operator()(const Rando::Position& pos) const {
            return hash<int>{}(pos.scene) ^ hash<float>{}(pos.pos.x) ^ hash<float>{}(pos.pos.y) ^ hash<float>{}(pos.pos.z);
        }
    };

    template<>
    struct hash<Rando::Identifier> {
        inline size_t operator()(const Rando::Identifier& id) const {
            return hash<int>{}(id.scene) ^ hash<int>{}(id.quest) ^ hash<int>{}(id.params);
        }
    };

    template<>
    struct equal_to<Rando::Position> {
        inline bool operator()(const Rando::Position& a, const Rando::Position& b) const {
            return a.scene == b.scene && a.pos.x == b.pos.x && a.pos.y == b.pos.y && a.pos.z == b.pos.z;
        }
    };

    template<>
    struct equal_to<Rando::Identifier> {
        inline bool operator()(const Rando::Identifier& a, const Rando::Identifier& b) const {
            return a.scene == b.scene && a.params == b.params && a.quest == b.quest;
        }
    };
}
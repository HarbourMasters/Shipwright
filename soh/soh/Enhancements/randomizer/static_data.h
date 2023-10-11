#pragma once

#include <array>
#include "randomizerTypes.h"
#include "item.h"
#include "location.h"
/**
 * @brief Singleton for storing and accessing static Randomizer-related data
 * 
 * The StaticData class is a class for storing and accessing static Randomizer-related data.
 * Static Data, in this context, refers to data that is the same for every single randomizer seed,
 * i.e. the item and location tables. This class is intended to be used as a Singleton.
 */
class StaticData {
    private:
      static std::array<Rando::Item, RG_MAX> itemTable;
      static std::array<Rando::Location, RC_MAX> locationTable;

    public:
      static void InitItemTable();
      static Rando::Item& RetrieveItem(const RandomizerGet rgid);
      static Rando::Item& ItemFromGIID(const int giid);
      static std::array<Rando::Item, RG_MAX>& GetItemTable();
      static void InitLocationTable();
      static Rando::Location* Location(RandomizerCheck locKey);
      static std::array<Rando::Location, RC_MAX>& GetLocationTable();
      static std::vector<RandomizerCheck> overworldLocations;
      static std::vector<RandomizerCheck> dungeonRewardLocations;
      static std::vector<std::vector<RandomizerCheck>> shopLocationLists;
      static std::vector<RandomizerCheck> scrubLocations;
      static std::vector<RandomizerCheck> gossipStoneLocations;
      StaticData();
      ~StaticData();

};
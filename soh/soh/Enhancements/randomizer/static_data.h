#pragma once

#include <array>
#include "randomizerTypes.h"
#include "item.h"
/**
 * @brief Singleton for storing and accessing static Randomizer-related data
 * 
 * The StaticData class is a class for storing and accessing static Randomizer-related data.
 * Static Data, in this context, refers to data that is the same for every single randomizer seed,
 * i.e. the item and location tables. This class is intended to be used as a Singleton.
 */
class StaticData {
    private:
      static std::array<RandoItem, RG_MAX> itemTable;

    public:
      static void InitItemTable();
      static RandoItem& RetrieveItem(const RandomizerGet rgid);
      static RandoItem& ItemFromGIID(const int giid);
      StaticData();
      ~StaticData();

};
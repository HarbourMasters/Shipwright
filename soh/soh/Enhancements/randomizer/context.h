#pragma once

#include "../randomizer/randomizerTypes.h"
#include "item_location.h"
#include "3drando/text.hpp"
#include "hint.h"

#include <memory>
#include <array>

/**
 * @brief Singleton for storing and accessing dynamic Randomizer-related data
 * 
 * The Context class is a class for storing and accessing dynamic Randomizer-related data.
 * Dynamic Data, in this context, refers to data that can be different for each randomizer seed,
 * i.e. item locations, hint contents and locations, settings, etc. This class is intended to be
 * used as a Singleton.
 */
namespace Rando{
class Context {
    public:
        static std::shared_ptr<Context> CreateInstance();
        static std::shared_ptr<Context> GetInstance();
        RandoHint* GetHint(RandomizerHintKey hintKey);
        void AddHint(RandomizerHintKey hintId, Text text, RandomizerCheck hintedLocation, HintType hintType,
                     Text hintedRegion);
        ItemLocation *GetItemLocation(RandomizerCheck locKey);
        void PlaceItemInLocation(RandomizerCheck locKey, RandomizerGet item, bool applyEffectImmediately = false,
                                 bool setHidden = false);
        std::vector<RandomizerCheck> allLocations;
        void AddLocation(RandomizerCheck loc, std::vector<RandomizerCheck>* destination = nullptr);
        template<typename Container>
        void AddLocations(const Container& locations, std::vector<RandomizerCheck>* destination = nullptr);
        void GenerateLocationPool();
        std::vector<RandomizerCheck> GetLocations(const std::vector<RandomizerCheck>& locationPool,
                                                  Category categoryInclude, Category categoryExclude = Category::cNull);
        void LocationReset();
        void ItemReset();
        void CreateItemOverrides();
        std::set<ItemOverride, ItemOverride_Compare> overrides;
        std::vector<std::vector<RandomizerCheck>> playthroughLocations;
        std::vector<RandomizerCheck> wothLocations;
        std::unordered_map<RandomizerCheck, uint8_t> iceTrapModels;
        bool playthroughBeatable = false;
        bool allLocationsReachable = false;
      private:
        static std::weak_ptr<Context> mContext;
        std::array<RandoHint, RH_MAX> hintTable;
        std::array<ItemLocation, RC_MAX> itemLocationTable;
};
}
#pragma once

#include <array>
#include <map>
#include <unordered_map>
#include "randomizerTypes.h"
#include "item.h"
#include "location.h"

namespace Rando {
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
      static void HintTable_Init();
      static void HintTable_Init_Item();
      static void HintTable_Init_Exclude_Overworld();
      static void HintTable_Init_Exclude_Dungeon();
      static Item& RetrieveItem(const RandomizerGet rgid);
      static Item& ItemFromGIID(const int giid);
      static std::array<Item, RG_MAX>& GetItemTable();// is there a reason this is a function and not just an exposed table?
      static void InitLocationTable();
      static Location* GetLocation(RandomizerCheck locKey);
      static std::array<Rando::Location, RC_MAX>& GetLocationTable();
      static std::unordered_map<std::string, uint32_t> PopulateTranslationMap(std::unordered_map<uint32_t, CustomMessage> input);
      static std::unordered_map<std::string, uint32_t> PopulateTranslationMap(std::unordered_map<uint32_t, RandomizerHintTextKey> input);
      static std::multimap<std::tuple<s16, s16, s32>, RandomizerCheck> CheckFromActorMultimap;
      static std::vector<RandomizerCheck> GetOverworldLocations();
      static std::vector<RandomizerCheck> GetAllDungeonLocations();
      static std::vector<RandomizerCheck> dungeonRewardLocations;
      static std::vector<RandomizerCheck> GetShopLocations();
      static std::vector<RandomizerCheck> GetScrubLocations();
      static std::vector<RandomizerCheck> GetMerchantLocations();
      static std::vector<RandomizerCheck> GetAdultTradeLocations();
      static std::vector<RandomizerCheck> GetGossipStoneLocations();
      static std::vector<RandomizerCheck> GetStaticHintLocations();
      static std::vector<RandomizerCheck> GetPondFishLocations();
      static std::vector<RandomizerCheck> GetOverworldFishLocations();
      static std::array<std::pair<RandomizerCheck, RandomizerCheck>, 17> randomizerFishingPondFish;
      static std::unordered_map<int8_t, RandomizerCheck> randomizerGrottoFishMap;
      static std::vector<RandomizerHint> oldVerHintOrder;
      static uint16_t oldVerGossipStoneStart;
      static std::unordered_map<std::string, RandomizerCheck> locationNameToEnum;
      static std::unordered_map<std::string, RandomizerGet> itemNameToEnum;
      static std::unordered_map<uint32_t, CustomMessage> hintNames;
      static std::unordered_map<std::string, uint32_t> hintNameToEnum;
      static std::unordered_map<uint32_t, CustomMessage> hintTypeNames;
      static std::unordered_map<std::string, uint32_t> hintTypeNameToEnum;
      static std::unordered_map<RandomizerCheck, RandomizerHint> gossipStoneCheckToHint;
      static std::unordered_map<uint32_t, RandomizerHintTextKey> areaNames;
      static std::unordered_map<std::string, uint32_t> areaNameToEnum;
      static std::unordered_map<uint32_t, RandomizerHintTextKey> trialData;
      static std::unordered_map<std::string, uint32_t> trialNameToEnum;
      static std::unordered_map<RandomizerHint, StaticHintInfo> staticHintInfoMap;
      static std::unordered_map<u32, RandomizerHint> stoneParamsToHint;
      static std::unordered_map<u32, RandomizerHint> grottoChestParamsToHint;
      static std::array<HintText, RHT_MAX> hintTextTable;

      StaticData();
      ~StaticData();
};
}
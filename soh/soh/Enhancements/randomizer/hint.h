#pragma once

#include "3drando/text.hpp"
#include "../custom-message/CustomMessageManager.h"
#include "randomizerTypes.h"
#include <vector>
#include "nlohmann/json.hpp"
using oJson = nlohmann::ordered_json;

namespace Rando {
class Hint {
     public:
     Hint();
     Hint(RandomizerHint ownKey_, 
          HintType hintType_,
          std::string distributionName_,
          std::vector<RandomizerHintTextKey> hintKeys_,
          std::vector<RandomizerCheck> locations_,
          std::vector<RandomizerArea> areas_ = {},
          std::vector<TrialKey> trials_ = {});
     Hint(RandomizerHint ownKey_, 
          HintType hintType_,
          std::vector<RandomizerHintTextKey> hintKeys_,
          std::vector<RandomizerCheck> locations_ = {},
          std::vector<RandomizerArea> areas_ = {},
          std::vector<TrialKey> trials_ = {},
          bool yourPocket_ = false, 
          int num_ = 0);
     Hint(RandomizerHint ownKey_, std::vector<CustomMessage> messages_);
     Hint(RandomizerHint ownKey_, nlohmann::json json_);
     const std::vector<std::string> GetAllMessageStrings(MessageFormat format = MF_AUTO_FORMAT) const ;
     const CustomMessage GetMessage(MessageFormat format = MF_AUTO_FORMAT, uint8_t id = 0) const ;
     std::vector<RandomizerCheck> GetHintedLocations();
     void AddHintedLocation (RandomizerCheck location);
     HintType GetHintType();
     void SetHintType (HintType type);
     std::vector<RandomizerArea> GetHintedAreas();
     void AddHintedArea (RandomizerArea area);
     const std::string& GetDistribution();
     void SetDistribution (std::string distribution);
     void ResetVariables();
     oJson toJSON();
     void logHint(oJson& jsonData);
     const CustomMessage GetItemName(uint8_t slot, bool mysterious = false) const;
     void ExtrapolateDataFromLocations();
     void SetLocationsAsHinted();
     bool IsEnabled();
     std::vector<RandomizerHintTextKey> GetHintKeys();
     std::vector<RandomizerGet> GetHintedItems();
     std::vector<uint8_t> GetChosenNames();
     std::vector<TrialKey> GetHintedTrials();
     static CustomMessage GetBridgeReqsText();
     static CustomMessage GetGanonBossKeyText();
     bool IsAddedToPool(); //what is this going to do again? 
     void AddToPool();

     private:
     RandomizerHint ownKey = RH_NONE;
     HintType hintType = HINT_TYPE_HINT_KEY;
     std::string distribution = "";
     std::vector<RandomizerHintTextKey> hintKeys = {};
     std::vector<RandomizerCheck> locations = {};
     std::vector<RandomizerArea> areas = {};
     std::vector<TrialKey> trials = {};
     bool yourPocket = false;
     int num = 0;
     std::vector<CustomMessage> messages = {};
     std::vector<RandomizerGet> items = {};
     bool enabled = false;
     std::vector<uint8_t> chosenName = {};
};
}
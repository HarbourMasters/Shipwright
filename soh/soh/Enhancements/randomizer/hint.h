#pragma once

#include "3drando/text.hpp"
#include "../custom-message/CustomMessageManager.h"
#include "randomizerTypes.h"
#include <vector>
using json = nlohmann::ordered_json;

namespace Rando {
class Hint {
     public:
     Hint();
     Hint(RandomizerHint ownKey_, 
          HintType hintType_,
          std::string distributionName_,
          std::vector<RandomizerHintTextKey> hintKeys_,
          std::vector<RandomizerCheck> locations_,
          std::vector<TrialKey> trials_);
     Hint(RandomizerHint ownKey_, std::vector<CustomMessage> messages_);
     const std::vector<std::string> GetAllMessageStrings() const;
     const CustomMessage& GetMessage(uint8_t id = 0) const;
     std::vector<RandomizerCheck> GetHintedLocations();
     void AddHintedLocation (RandomizerCheck location);
     HintType GetHintType();
     void SetHintType (HintType type);
     std::vector<RandomizerArea> GetHintedAreas();
     void AddHintedArea (RandomizerArea area);
     const std::string& GetDistribution();
     void SetDistribution (std::string distribution);
     void ResetVariables();
     nlohmann::json GetLog();
     void logHint(nlohmann::json& jsonData);
     const CustomMessage GetItemName(uint8_t slot, bool mysterious = false) const;
     void ExtrapolateDataFromLocations();
     void SetLocationsAsHinted();
     bool IsAddedToPool(); //what is this going to do again? 
     void AddToPool();

     protected:
     RandomizerHint ownKey = RH_NONE;
     uint8_t num = 0;
     bool yourPocket = false;
     std::vector<CustomMessage> messages = {};
     std::vector<RandomizerHintTextKey> hintKeys = {};
     std::vector<RandomizerCheck> locations = {};
     std::vector<Item> items = {};
     std::vector<TrialKey> trials = {};
     HintType hintType = HINT_TYPE_HINT_KEY;
     std::vector<RandomizerArea> areas = {};
     std::string distribution = "";
     bool enabled = false;
     std::string logMap = "Gossip Stone Hints";
     std::vector<uint8_t> chosenName = {};
};

class StaticHint : public Hint {
     public :
     StaticHint(){};
     StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                bool yourPocket_ = false);
     StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                uint8_t num);
     StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerArea> areas_ = {});
};

class AltarHint : public StaticHint {
     public :
     AltarHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerCheck> locations_);
};
}
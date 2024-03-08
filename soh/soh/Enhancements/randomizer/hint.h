#pragma once

#include "3drando/text.hpp"
#include "../custom-message/CustomMessageManager.h"
#include "randomizerTypes.h"
#include <vector>
using json = nlohmann::ordered_json;

//#define QM_WHITE "%w" //RANDOTODO put somewhere better when converting things to customMessage
//#define QM_RED "%r"
//#define QM_GREEN "%g"
//#define QM_BLUE "%b"
//#define QM_LBLUE "%c"
//#define QM_PINK "%p"
//#define QM_YELLOW "%y"
//#define QM_BLACK "%B"


namespace Rando {
class Hint {
    public:
    Hint();
    Hint(RandomizerHint ownKey_, 
         HintType hintType_,
         std::string distributionName_,
         std::vector<RandomizerHintTextKey> hintKeys_,
         std::vector<RandomizerCheck> locations_,
         std::vector<std::string> colors_,
         std::vector<TrialKey> trials_);
    Hint(RandomizerHint ownKey_, HintType hintType_, 
    std::vector<RandomizerHintTextKey> hintKeys_,
     std::vector<RandomizerGet> items, std::vector<TextBoxType> boxTypes_ = {},
      bool yourPocket = false, std::string distributionName_ = ""); //RANDOTODO reorgainise if used
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
    bool IsAddedToPool();
    void AddToPool();
    std::map<std::string, std::string> GetLog();
    void logHint(json& jsonData);
    const Text GetItemName(RandomizerCheck hintedCheck, bool mysterious = false) const;
    void GetAreasAndItemsFromLocations();
    void GetLocationsAndAreaFromItems(std::vector<RandomizerGet> items);
    void SetLocationsAsHinted();

    protected:
    RandomizerHint ownKey;
    uint8_t num = 0;
    bool capital = false;
    bool yourPocket = false;
    std::vector<CustomMessage> messages = {};
    std::vector<TextBoxType> boxTypes = {};
    std::vector<uint8_t> colors = {};
    std::vector<RandomizerHintTextKey> hintKeys = {};
    std::vector<RandomizerCheck> locations = {};
    std::vector<Item> items = {};
    std::vector<TrialKey> trials = {};
    HintType hintType = HINT_TYPE_TEXT;
    std::vector<RandomizerArea> areas = {};
    std::string distribution = "";
    bool addedToPool = false;
};

class StaticHint : public Hint {
     public :
     StaticHint(){};
     StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                std::vector<uint8_t> colors_, bool yourPocket_ = false, bool capital_ = false);
     StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                bool yourPocket_ = false, bool capital_ = false);
     StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                uint8_t num);
     StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerArea> areas_);
     void logHint(json& jsonData);
};

class AltarHint : public StaticHint {
     public :
     AltarHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerCheck> locations_, std::vector<uint8_t> colors_ = {});
};
}
#pragma once

#include <string>
#include <vector>
#include <list>

#include "logic.hpp"
#include "hint_list.hpp"
#include "keys.hpp"
#include "fill.hpp"

typedef bool (*ConditionFn)();

class EventAccess {
public:


    explicit EventAccess(bool* event_, std::vector<ConditionFn> conditions_met_)
        : event(event_) {
        conditions_met.resize(2);
        for (size_t i = 0; i < conditions_met_.size(); i++) {
            conditions_met[i] = conditions_met_[i];
        }
    }

    bool ConditionsMet() const {
        if (Settings::Logic.Is(LOGIC_NONE) || Settings::Logic.Is(LOGIC_VANILLA)) {
            return true;
        } else if (Settings::Logic.Is(LOGIC_GLITCHLESS)) {
            return conditions_met[0]();
        } else if (Settings::Logic.Is(LOGIC_GLITCHED)) {
            if (conditions_met[0]()) {
                return true;
            } else if (conditions_met[1] != NULL) {
                return conditions_met[1]();
            }
        }
        return false;
    }

    bool CheckConditionAtAgeTime(bool& age, bool& time) {

      Logic::IsChild = false;
      Logic::IsAdult = false;
      Logic::AtDay   = false;
      Logic::AtNight = false;

      time = true;
      age = true;

      Logic::UpdateHelpers();
      return ConditionsMet();
    }

    void EventOccurred() {
        *event = true;
    }

    bool GetEvent() const {
        return *event;
    }

private:
    bool* event;
    std::vector<ConditionFn> conditions_met;
};

//this class is meant to hold an item location with a boolean function to determine its accessibility from a specific area
class LocationAccess {
public:

    explicit LocationAccess(uint32_t location_, std::vector<ConditionFn> conditions_met_)
        : location(location_) {
        conditions_met.resize(2);
        for (size_t i = 0; i < conditions_met_.size(); i++) {
            conditions_met[i] = conditions_met_[i];
        }
    }

    bool GetConditionsMet() const {
        if (Settings::Logic.Is(LOGIC_NONE) || Settings::Logic.Is(LOGIC_VANILLA)) {
            return true;
        } else if (Settings::Logic.Is(LOGIC_GLITCHLESS)) {
            return conditions_met[0]();
        } else if (Settings::Logic.Is(LOGIC_GLITCHED)) {
            if (conditions_met[0]()) {
                return true;
            } else if (conditions_met[1] != NULL) {
                return conditions_met[1]();
            }
        }
        return false;
    }

    bool CheckConditionAtAgeTime(bool& age, bool& time) const;

    bool ConditionsMet() const;

    uint32_t GetLocation() const {
        return location;
    }

private:
    uint32_t location;
    std::vector<ConditionFn> conditions_met;

    //Makes sure shop locations are buyable
    bool CanBuy() const;
};

class Entrance;
enum class EntranceType;

class Area {
public:
    Area();
    Area(std::string regionName_, std::string scene_, uint32_t hintKey_,
         bool timePass_,
         std::vector<EventAccess> events_,
         std::vector<LocationAccess> locations_,
         std::list<Entrance> exits_);
    ~Area();

    std::string regionName;
    std::string scene;
    uint32_t     hintKey;
    bool        timePass;
    std::vector<EventAccess> events;
    std::vector<LocationAccess> locations;
    std::list<Entrance> exits;
    std::list<Entrance*> entrances;
    //^ The above exits are now stored in a list instead of a vector because
    //the entrance randomization algorithm plays around with pointers to these
    //entrances a lot. By putting the entrances in a list, we don't have to
    //worry about a vector potentially reallocating itself and invalidating all our
    //entrance pointers.

    bool childDay = false;
    bool childNight = false;
    bool adultDay = false;
    bool adultNight = false;
    bool addedToPool = false;

    bool UpdateEvents(SearchMode mode);

    void AddExit(uint32_t parentKey, uint32_t newExitKey, ConditionFn condition);

    void RemoveExit(Entrance* exitToRemove);

    void SetAsPrimary(uint32_t exitToBePrimary);

    Entrance* GetExit(uint32_t exit);

    bool Child() const {
      return childDay || childNight;
    }

    bool Adult() const {
      return adultDay || adultNight;
    }

    bool BothAgesCheck() const {
      return Child() && Adult();
    }

    bool HasAccess() const {
      return Child() || Adult();
    }

    bool AllAccess() const {
      return childDay && childNight && adultDay && adultNight;
    }

    //Check to see if an exit can be access as both ages at both times of day
    bool CheckAllAccess(uint32_t exitKey);

    const HintText& GetHint() const {
      return Hint(hintKey);
    }

    //Here checks conditional access based on whether or not both ages have
    //access to this area. For example: if there are rocks that block a path
    //which both child and adult can access, adult having hammer can give
    //both child and adult access to the path.
    bool HereCheck(ConditionFn condition) {

      //store current age variables
      bool pastAdult = Logic::IsAdult;
      bool pastChild = Logic::IsChild;

      //set age access as this areas ages
      Logic::IsChild = Child();
      Logic::IsAdult = Adult();

      //update helpers and check condition as well as having at least child or adult access
      Logic::UpdateHelpers();
      bool hereVal = condition() && (Logic::IsAdult || Logic::IsChild);

      //set back age variables
      Logic::IsChild = pastChild;
      Logic::IsAdult = pastAdult;
      Logic::UpdateHelpers();

      return hereVal;
    }

    bool CanPlantBeanCheck() const;
    bool AllAccountedFor() const;

    void ResetVariables();

    void printAgeTimeAccess() const {
      auto message = "Child Day:   " + std::to_string(childDay)   + "\t"
                     "Child Night: " + std::to_string(childNight) + "\t"
                     "Adult Day:   " + std::to_string(adultDay)   + "\t"
                     "Adult Night: " + std::to_string(adultNight);
      //CitraPrint(message);
    }
};

extern std::array<Area, KEY_ENUM_MAX> areaTable;
extern std::vector<EventAccess> grottoEvents;

bool Here(const AreaKey area, ConditionFn condition);
bool CanPlantBean(const AreaKey area);
bool BothAges(const AreaKey area);
bool ChildCanAccess(const AreaKey area);
bool AdultCanAccess(const AreaKey area);
bool HasAccessTo(const AreaKey area);

#define DAY_NIGHT_CYCLE true
#define NO_DAY_NIGHT_CYCLE false

namespace Areas {

  extern void AccessReset();
  extern void ResetAllLocations();
  extern bool HasTimePassAccess(uint8_t age);
  extern void DumpWorldGraph(std::string str);
} //namespace Exits

void  AreaTable_Init();
Area* AreaTable(const uint32_t areaKey);
std::vector<Entrance*> GetShuffleableEntrances(EntranceType type, bool onlyPrimary = true);

// Overworld
void AreaTable_Init_LostWoods();
void AreaTable_Init_HyruleField();
void AreaTable_Init_CastleTown();
void AreaTable_Init_Kakariko();
void AreaTable_Init_DeathMountain();
void AreaTable_Init_ZorasDomain();
void AreaTable_Init_GerudoValley();
// Dungeons
void AreaTable_Init_DekuTree();
void AreaTable_Init_DodongosCavern();
void AreaTable_Init_JabuJabusBelly();
void AreaTable_Init_ForestTemple();
void AreaTable_Init_FireTemple();
void AreaTable_Init_WaterTemple();
void AreaTable_Init_SpiritTemple();
void AreaTable_Init_ShadowTemple();
void AreaTable_Init_BottomOfTheWell();
void AreaTable_Init_IceCavern();
void AreaTable_Init_GerudoTrainingGrounds();
void AreaTable_Init_GanonsCastle();

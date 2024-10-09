#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>

#include "../randomizerTypes.h"
#include "../context.h"
#include "../logic.h"

typedef bool (*ConditionFn)();

// I hate this but every alternative I can think of right now is worse
extern Rando::Context* ctx;
extern std::shared_ptr<Rando::Logic> logic;

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
        auto ctx = Rando::Context::GetInstance();
        if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_NO_LOGIC) || ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
            return true;
        } else if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
            return conditions_met[0]();
        } else if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHED)) {
            if (conditions_met[0]()) {
                return true;
            } else if (conditions_met[1] != NULL) {
                return conditions_met[1]();
            }
        }
        return false;
    }

    bool CheckConditionAtAgeTime(bool& age, bool& time) {

      logic->IsChild = false;
      logic->IsAdult = false;
      logic->AtDay   = false;
      logic->AtNight = false;

      time = true;
      age = true;

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

std::string CleanCheckConditionString(std::string condition);

#define LOCATION(check, condition) LocationAccess(check, {[]{return condition;}}, CleanCheckConditionString(#condition))

//this class is meant to hold an item location with a boolean function to determine its accessibility from a specific area
class LocationAccess {
public:

    explicit LocationAccess(RandomizerCheck location_, std::vector<ConditionFn> conditions_met_)
        : location(location_), condition_str("") {
        conditions_met.resize(2);
        for (size_t i = 0; i < conditions_met_.size(); i++) {
            conditions_met[i] = conditions_met_[i];
        }
    }

    explicit LocationAccess(RandomizerCheck location_, std::vector<ConditionFn> conditions_met_, std::string condition_str_)
        : location(location_), condition_str(condition_str_) {
        conditions_met.resize(2);
        for (size_t i = 0; i < conditions_met_.size(); i++) {
            conditions_met[i] = conditions_met_[i];
        }
    }

    bool GetConditionsMet() const {
        auto ctx = Rando::Context::GetInstance();
        if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_NO_LOGIC) || ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
            return true;
        } else if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
            return conditions_met[0]();
        } else if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHED)) {
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

    RandomizerCheck GetLocation() const {
        return location;
    }

    std::string GetConditionStr() const {
        return condition_str;
    }

protected:
    RandomizerCheck location;
    std::vector<ConditionFn> conditions_met;
    std::string condition_str;

    //Makes sure shop locations are buyable
    bool CanBuy() const;
};

bool CanBuyAnother(RandomizerCheck rc);

namespace Rando {
    class Entrance;
    enum class EntranceType;
}

class Region {
public:
    Region();
    Region(std::string regionName_, std::string scene_, std::set<RandomizerArea> areas,
         bool timePass_,
         std::vector<EventAccess> events_,
         std::vector<LocationAccess> locations_,
         std::list<Rando::Entrance> exits_);
    ~Region();

    std::string regionName;
    std::string scene;
    std::set<RandomizerArea> areas;
    bool timePass;
    std::vector<EventAccess> events;
    std::vector<LocationAccess> locations;
    std::list<Rando::Entrance> exits;
    std::list<Rando::Entrance*> entrances;
    //^ The above exits are now stored in a list instead of a vector because
    //the entrance randomization algorithm plays around with pointers to these
    //entrances a lot. By putting the entrances in a list, we don't have to
    //worry about a vector potentially reallocating itself and invalidating all our
    //entrance pointers.

    bool childDay = false;
    bool childNight = false;
    bool adultDay = false;
    bool adultNight = false;
    bool addedToPool = false;;

    void ApplyTimePass();

    bool UpdateEvents();

    void AddExit(RandomizerRegion parentKey, RandomizerRegion newExitKey, ConditionFn condition);

    void RemoveExit(Rando::Entrance* exitToRemove);

    void SetAsPrimary(RandomizerRegion exitToBePrimary);

    Rando::Entrance* GetExit(RandomizerRegion exit);

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
    bool CheckAllAccess(RandomizerRegion exitKey);

    std::set<RandomizerArea> GetAllAreas() const{
        return areas;
    }

    void ReplaceAreas(std::set<RandomizerArea> newAreas) {
        areas = newAreas;
    }

    //Here checks conditional access based on whether or not both ages have
    //access to this area. For example: if there are rocks that block a path
    //which both child and adult can access, adult having hammer can give
    //both child and adult access to the path.
    bool Here(ConditionFn condition) {

      //store current age variables
      bool pastAdult = logic->IsAdult;
      bool pastChild = logic->IsChild;

      //set age access as this areas ages
      logic->IsChild = Child();
      logic->IsAdult = Adult();

      //heck condition as well as having at least child or adult access
      bool hereVal = condition() && (logic->IsAdult || logic->IsChild);

      //set back age variables
      logic->IsChild = pastChild;
      logic->IsAdult = pastAdult;

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
    }
};

extern std::array<Region, RR_MAX> areaTable;
extern std::vector<EventAccess> grottoEvents;

bool Here(const RandomizerRegion region, ConditionFn condition); //RANDOTODO make a less stupid way to check own at either age than self referncing with this
bool CanPlantBean(const RandomizerRegion region);
bool BothAges(const RandomizerRegion region);
bool ChildCanAccess(const RandomizerRegion region);
bool AdultCanAccess(const RandomizerRegion region);
bool HasAccessTo(const RandomizerRegion region);

#define DAY_NIGHT_CYCLE true
#define NO_DAY_NIGHT_CYCLE false

namespace Regions {

  extern void AccessReset();
  extern void ResetAllLocations();
  extern bool HasTimePassAccess(uint8_t age);
  extern void DumpWorldGraph(std::string str);
} //namespace Exits

void  RegionTable_Init();
Region* RegionTable(const RandomizerRegion regionKey);
std::vector<Rando::Entrance*> GetShuffleableEntrances(Rando::EntranceType type, bool onlyPrimary = true);
Rando::Entrance* GetEntrance(const std::string name);

// Overworld
void RegionTable_Init_LostWoods();
void RegionTable_Init_HyruleField();
void RegionTable_Init_CastleTown();
void RegionTable_Init_Kakariko();
void RegionTable_Init_DeathMountain();
void RegionTable_Init_ZorasDomain();
void RegionTable_Init_GerudoValley();
// Dungeons
void RegionTable_Init_DekuTree();
void RegionTable_Init_DodongosCavern();
void RegionTable_Init_JabuJabusBelly();
void RegionTable_Init_ForestTemple();
void RegionTable_Init_FireTemple();
void RegionTable_Init_WaterTemple();
void RegionTable_Init_SpiritTemple();
void RegionTable_Init_ShadowTemple();
void RegionTable_Init_BottomOfTheWell();
void RegionTable_Init_IceCavern();
void RegionTable_Init_GerudoTrainingGrounds();
void RegionTable_Init_GanonsCastle();

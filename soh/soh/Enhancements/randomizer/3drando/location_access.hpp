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

RandomizerArea GetFirstArea() const{
    if (areas.empty()){
      assert(false);
      return RA_NONE;
    } else {
      return *areas.begin();
    }
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

/*This logic covers checks that exist in the shared areas of MQ spirit from a glitchless standpoint.
  This room has Quantum logic that I am currently handling with this function, however this is NOT suitable for glitch logic as it relies on specific ages
  In this chunk there are 3 possibilities for passing a check, but first I have to talk about parallel universes.

  In MQ Spirit key logic, we mostly care about 2 possibilities for how the player can spend keys, creating 2 Parralel universes
  In the first universe, the player did not enter spirit as adult until after climbing as child, thus child spends keys linearly, only needing 2 to reach statue room.
  In the second universe, the player went in as adult, possibly out of logic, and started wasting the keys to lock child out.
  These Universes converge when the player has 7 keys (meaning adult can no longer lock child out) and adult is known to be able to reach Statue room. This creates "Certain Access", which is tracked seperatly for each age.
  Child Certain Access is simple, if we have 7 keys and child access, it's Certain Access.
  Adult Certain Access is also simple, adult is not key locked, so if they make it to a location, it's Certain Access.
  Things get complicated when we handle the overlap of the 2 universes,
  though an important detail is that if we have Certain Access as either age, we don't need to checked the overlap because overlap logic is strictly stricter than either Certain Access.
  
  In order to track the first universe, the logic allows technical child access with the minimum number of keys, and then checks in this function for if we have 7 keys to determine if that is Certain or not.
  This is for technical reasons, as areas with no access at all will simply not be checked.
  Normally we would need to do similar shenanigans to track the second universe, however adult must have go through statue room to waste keys, 
  so can go back there and get new keys for Child to use if they do, and the navigation logic for shared MQ spirit from Statue Room is very simple for Adult.
  Additionally, we don't need to know if adult can actually reach spirit temple or climb to statue room, because if the player can't do that, then universe 2 can't happen anyway,
  and if the player does so out of logic, they can do it again, as the only consumable used sets a permanent flag.

  The Adult Navigation logic is as such:
  - Broken Wall room is 6 key locked, because if the player tries to spend 6 keys in a way that would block adults access, they would have to give child access instead.
  - The child side hammer switch for the time travelling chest is 7 key locked for adult
  - Reaching gauntlets hand is 7 key locked
  - Going back into big block room is complex, but the only check there is child only so not a concern
  - Everything else is possible with basic adult movement, or is impossible for child to reach glitchlessly
  Anything 7 key locked does not need to be checked as shared, as all child access is Certain and because of this workaround we don't need to fake Adult access, meaning that is also Certain.
  All of this combined means that when checking if adult can reach a location in universe 2, we only have to ask if it is a 6 key locked location or not.

  Knowing all of this this, we can confirm things are logical in 3 different ways:
  - If we have Adult Access, we know it is Certain Access, so they can get checks alone.
  - If we have 7 keys, child has Certain Access as we know they cannot be locked out, so can get checks alone, otherwise we check the logical overlap
  - If Child and Adult can get the check (ignoring actual adult access to the location), and the location is either not 6 key locked or we have 6 keys, we can get the check with the overlap*/
  bool MQSpiritShared(ConditionFn condition, bool IsBrokenWall, bool anyAge = false) {
    //if we have Certain Access as child, we can check anyAge and if true, resolve a condition with Here as if adult is here it's also Certain Access
    if (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7)){
      if (anyAge){
        return Here(condition);
      }
      return condition();
    //else, if we are here as adult, we have Certain Access from that and don't need special handling for checking adult
    } else if (Adult() && logic->IsAdult){
      return condition();
    //if we do not have Certain Access, we need to check the overlap by seeing if we are both here as child and meet the adult universe's access condition
    //We only need to do it as child, as only child access matters for this check, as adult access is assumed based on keys
    } else if (Child() && logic->IsChild && (!IsBrokenWall || logic->SmallKeys(RR_SPIRIT_TEMPLE, 6))) {
      bool result = false;
      //store current age variables
      bool pastAdult = logic->IsAdult;
      bool pastChild = logic->IsChild;

      //First check if the check is possible as child
      logic->IsChild = true;
      logic->IsAdult = false;
      result = condition();
      //If so, check again as adult. both have to be true for result to be true
      if (result) {
        logic->IsChild = false;
        logic->IsAdult = true;
        result = condition();
      }
      
      //set back age variables
      logic->IsChild = pastChild;
      logic->IsAdult = pastAdult;
      return result;
    }
    return false;
  }
};

extern std::array<Region, RR_MAX> areaTable;
extern std::vector<EventAccess> grottoEvents;

bool Here(const RandomizerRegion region, ConditionFn condition); //RANDOTODO make a less stupid way to check own at either age than self referncing with this
bool MQSpiritSharedStatueRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge = false); 
bool MQSpiritSharedBrokenWallRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge = false); 
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

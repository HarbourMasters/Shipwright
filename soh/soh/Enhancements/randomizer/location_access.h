#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>

#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/context.h"
#include "soh/Enhancements/randomizer/logic.h"

#define TIME_PASSES true
#define TIME_DOESNT_PASS false

typedef bool (*ConditionFn)();

// I hate this but every alternative I can think of right now is worse
extern Rando::Context* ctx;
extern std::shared_ptr<Rando::Logic> logic;

class Region;

class EventAccess {
  public:
    explicit EventAccess(bool* event_, ConditionFn condition_function_)
        : event(event_), condition_function(condition_function_) {
    }

    bool ConditionsMet() const {
        auto ctx = Rando::Context::GetInstance();
        if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
            return condition_function();
        }
        return true;
    }

    bool CheckConditionAtAgeTime(bool& age, bool& time);

    void EventOccurred() {
        *event = true;
    }

    bool GetEvent() const {
        return *event;
    }

  private:
    bool* event;
    ConditionFn condition_function;
};

std::string CleanCheckConditionString(std::string condition);

#define LOCATION(check, condition) \
    LocationAccess(                \
        check, [] { return condition; }, CleanCheckConditionString(#condition))

// this class is meant to hold an item location with a boolean function to determine its accessibility from a specific
// area
class LocationAccess {
  public:
    explicit LocationAccess(RandomizerCheck location_, ConditionFn condition_function_)
        : location(location_), condition_function(condition_function_), condition_str("") {
    }

    explicit LocationAccess(RandomizerCheck location_, ConditionFn condition_function_, std::string condition_str_)
        : location(location_), condition_function(condition_function_), condition_str(condition_str_) {
    }

    bool GetConditionsMet() const {
        auto ctx = Rando::Context::GetInstance();
        if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
            return condition_function();
        }
        return true;
    }

    bool CheckConditionAtAgeTime(bool& age, bool& time) const;

    bool ConditionsMet(Region* parentRegion, bool calculatingAvailableChecks) const;

    RandomizerCheck GetLocation() const {
        return location;
    }

    std::string GetConditionStr() const {
        return condition_str;
    }

  protected:
    RandomizerCheck location;
    ConditionFn condition_function;
    std::string condition_str;

    // Makes sure shop locations are buyable
    bool CanBuy(bool calculatingAvailableChecks) const;
};

bool CanBuyAnother(uint16_t price);
bool CanBuyAnother(RandomizerCheck rc);

namespace Rando {
class Entrance;
enum class EntranceType;
} // namespace Rando

class Region {
  public:
    Region();
    Region(std::string regionName_, SceneID scene_, bool timePass, std::set<RandomizerArea> areas,
           std::vector<EventAccess> events_, std::vector<LocationAccess> locations_, std::list<Rando::Entrance> exits_);
    Region(std::string regionName_, SceneID scene_, std::vector<EventAccess> events_,
           std::vector<LocationAccess> locations_, std::list<Rando::Entrance> exits_);
    ~Region();

    std::string regionName;
    SceneID scene;
    bool timePass;
    std::set<RandomizerArea> areas;
    std::vector<EventAccess> events;
    std::vector<LocationAccess> locations;
    std::list<Rando::Entrance> exits;
    std::list<Rando::Entrance*> entrances;
    //^ The above exits are now stored in a list instead of a vector because
    // the entrance randomization algorithm plays around with pointers to these
    // entrances a lot. By putting the entrances in a list, we don't have to
    // worry about a vector potentially reallocating itself and invalidating all our
    // entrance pointers.

    bool childDay = false;
    bool childNight = false;
    bool adultDay = false;
    bool adultNight = false;
    bool addedToPool = false;

    bool TimePass();

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

    // Check to see if an exit can be access as both ages at both times of day
    bool CheckAllAccess(RandomizerRegion exitKey);

    std::set<RandomizerArea> GetAllAreas() const {
        return areas;
    }

    RandomizerArea GetFirstArea() const {
        if (areas.empty()) {
            assert(false);
            return RA_NONE;
        } else {
            return *areas.begin();
        }
    }

    void ReplaceAreas(std::set<RandomizerArea> newAreas) {
        areas = newAreas;
    }

    // Here checks conditional access based on whether or not both ages have
    // access to this area. For example: if there are rocks that block a path
    // which both child and adult can access, adult having hammer can give
    // both child and adult access to the path.
    bool Here(ConditionFn condition) {
        // store current age variables
        bool pastAdult = logic->IsAdult;
        bool pastChild = logic->IsChild;

        // set age access as this areas ages
        logic->IsChild = Child();
        logic->IsAdult = Adult();

        // check condition as well as having at least child or adult access
        bool hereVal = condition() && (logic->IsAdult || logic->IsChild);

        // set back age variables
        logic->IsChild = pastChild;
        logic->IsAdult = pastAdult;

        return hereVal;
    }

    bool CanPlantBeanCheck() const;
    bool AllAccountedFor() const;
    bool MQSpiritShared(ConditionFn condition, bool IsBrokenWall, bool anyAge = false);

    void ResetVariables();

    void printAgeTimeAccess();
};

extern std::array<Region, RR_MAX> areaTable;
extern std::vector<EventAccess> grottoEvents;

bool Here(const RandomizerRegion region,
          ConditionFn
              condition); // RANDOTODO make a less stupid way to check own at either age than self referencing with this
bool MQSpiritSharedStatueRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge = false);
bool MQSpiritSharedBrokenWallRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge = false);
bool CanPlantBean(const RandomizerRegion region);
bool BothAges(const RandomizerRegion region);
bool ChildCanAccess(const RandomizerRegion region);
bool AdultCanAccess(const RandomizerRegion region);

namespace Regions {
extern void AccessReset();
extern void ResetAllLocations();
extern bool HasTimePassAccess(uint8_t age);
extern void DumpWorldGraph(std::string str);
} // namespace Regions

void RegionTable_Init();
Region* RegionTable(const RandomizerRegion regionKey);
std::vector<Rando::Entrance*> GetShuffleableEntrances(Rando::EntranceType type, bool onlyPrimary = true);
Rando::Entrance* GetEntrance(RandomizerRegion source, RandomizerRegion destination);

// Overworld
void RegionTable_Init_KokiriForest();
void RegionTable_Init_LostWoods();
void RegionTable_Init_SacredForestMeadow();
void RegionTable_Init_HyruleField();
void RegionTable_Init_LakeHylia();
void RegionTable_Init_LonLonRanch();
void RegionTable_Init_Market();
void RegionTable_Init_TempleOfTime();
void RegionTable_Init_CastleGrounds();
void RegionTable_Init_Kakariko();
void RegionTable_Init_Graveyard();
void RegionTable_Init_DeathMountainTrail();
void RegionTable_Init_GoronCity();
void RegionTable_Init_DeathMountainCrater();
void RegionTable_Init_ZoraRiver();
void RegionTable_Init_ZorasDomain();
void RegionTable_Init_ZorasFountain();
void RegionTable_Init_GerudoValley();
void RegionTable_Init_GerudoFortress();
void RegionTable_Init_HauntedWasteland();
void RegionTable_Init_DesertColossus();
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
void RegionTable_Init_ThievesHideout();
void RegionTable_Init_GerudoTrainingGround();
void RegionTable_Init_GanonsCastle();
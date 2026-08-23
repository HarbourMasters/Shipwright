#pragma once

#include <array>
#include <string>
#include <vector>
#include <list>
#include <set>

#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/randomizer/logic.h"

#define TIME_PASSES true
#define TIME_DOESNT_PASS false

typedef bool (*ConditionFn)();

// I hate this but every alternative I can think of right now is worse
extern Rando::Context* ctx;
extern std::shared_ptr<Rando::Logic> logic;

class Region;

std::string CleanConditionString(std::string condition);

typedef Cost (*CostFn)();

// What a path spent getting where it is
struct PathCost {
    // Seconds in hot regions since the last cool one
    uint16_t heat = 0;
    // Health units spent on purpose since the last heal
    uint16_t damage = 0;
    // Bottled fairies the path is still carrying
    uint8_t fairies = 0;

    bool Dominates(const PathCost& other) const {
        return heat <= other.heat && damage <= other.damage && fairies >= other.fairies;
    }
};

// The Pareto minimal costs a region is reachable with at one agetime. Nothing in here dominates
// anything else in here, and a non empty set means reachable.
class PathCostSet {
  public:
    // Every route an edge offers can land here, so leave room for a few ways in
    static constexpr uint8_t MAX_COSTS = 8;

    bool Empty() const {
        return count == 0;
    }

    void Clear() {
        count = 0;
    }

    uint8_t Count() const {
        return count;
    }

    const PathCost& operator[](uint8_t i) const {
        return costs[i];
    }

    // Is some way in we already know at least as cheap as this one?
    bool Covers(const PathCost& cost) const {
        for (uint8_t i = 0; i < count; i++) {
            if (costs[i].Dominates(cost)) {
                return true;
            }
        }
        return false;
    }

    // Adds a cost unless something already here is at least as cheap. Returns whether the set changed.
    bool Insert(PathCost cost);

  private:
    std::array<PathCost, MAX_COSTS> costs{};
    uint8_t count = 0;
};

// One way of getting somewhere: what it asks for, and what it spends. Something with several
// routes offers each of them, and the Pareto set keeps whichever ones are worth having, so a free
// route and a route that costs health both stay open instead of one hiding the other.
struct Route {
    ConditionFn condition;
    std::string condition_str;
    // Null is free
    CostFn cost_function;

    Cost GetCost() const {
        return cost_function == nullptr ? Cost{} : cost_function();
    }
};

// ROUTE(condition) is free, ROUTE(condition, cost) is not
#define ROUTE_PICK(_1, _2, NAME, ...) NAME
#define ROUTE(...) ROUTE_PICK(__VA_ARGS__, ROUTE_COST, ROUTE_FREE)(__VA_ARGS__)
#define ROUTE_FREE(condition)                                                             \
    Route {                                                                               \
        []() -> bool { return !!(condition); }, CleanConditionString(#condition), nullptr \
    }
#define ROUTE_COST(condition, cost)                                                                             \
    Route {                                                                                                     \
        []() -> bool { return !!(condition); }, CleanConditionString(#condition), []() -> Cost { return cost; } \
    }

// Put the logic in one agetime on one path, so routes see the age, the time and what the path
// is still carrying. The age has to be in place first, as a tunic only counts for the age wearing it.
void EnterPath(const PathCost& path, bool& age, bool& time);

// Drop the path resources once the walk is done
void LeavePath();

// Can the path still pay this, and live through it?
bool Payable(const PathCost& path, const Cost& cost);

// What the path is left carrying once it has paid
PathCost Pay(const PathCost& path, const Cost& cost);

// Does this route's own condition hold? Only glitchless logic asks
bool RouteMet(const Route& route);

// Is any route open on this path? For the places that want reachability and not the price of it
bool AnyRouteMet(const std::vector<Route>& routes, const PathCost& path, bool& age, bool& time);

// The routes joined up the way they read in the logic files, for tooltips and the spoiler log
std::string RoutesToString(const std::vector<Route>& routes);

#define EVENT_ACCESS(event, condition) EventAccess(event, #event, { ROUTE(condition) })

// An event with more than one way to set it off, each ROUTE with its own price
#define EVENT_ROUTES(event, ...) EventAccess(event, #event, { __VA_ARGS__ })

// Somewhere the player can bottle a fairy, filling every bottle they have for logic's purposes
#define FAIRY_REFILL(condition) EventAccess({ ROUTE(condition) })

class EventAccess {
  public:
    explicit EventAccess(LogicVal event_, std::string event_str_, std::vector<Route> routes_)
        : event(event_), event_str(std::move(event_str_)), routes(std::move(routes_)) {
    }

    // A fairy refill sets no logic value, the region it sits in refills the path instead
    explicit EventAccess(std::vector<Route> routes_)
        : event(LOGIC_NONE), event_str("FAIRY_REFILL"), routes(std::move(routes_)), fairyRefill(true) {
    }

    const std::vector<Route>& GetRoutes() const {
        return routes;
    }

    // Whether any route holds, ignoring what it costs. A fairy refill is free, so this is all it needs
    bool ConditionsMet() const;

    void EventOccurred() {
        logic->Set(event, true);
    }

    bool GetEvent() const {
        return logic->Get(event);
    }

    bool IsFairyRefill() const {
        return fairyRefill;
    }

    const std::string& GetEventStr() const {
        return event_str;
    }

    std::string GetConditionStr() const {
        return RoutesToString(routes);
    }

  private:
    LogicVal event;
    std::string event_str;
    std::vector<Route> routes;
    // This is not an event at all, it marks the region as somewhere fairies can be bottled
    bool fairyRefill = false;
};

#define LOCATION(check, condition) LocationAccess(check, { ROUTE(condition) })

// A check with more than one way to reach it, each ROUTE with its own price
#define LOCATION_ROUTES(check, ...) LocationAccess(check, { __VA_ARGS__ })

// this class is meant to hold an item location with a boolean function to determine its accessibility from a specific
// area
class LocationAccess {
  public:
    explicit LocationAccess(RandomizerCheck location_, std::vector<Route> routes_)
        : location(location_), routes(std::move(routes_)) {
    }

    const std::vector<Route>& GetRoutes() const {
        return routes;
    }

    bool ConditionsMet(Region* parentRegion, bool calculatingAvailableChecks) const;

    RandomizerCheck GetLocation() const {
        return location;
    }

    std::string GetConditionStr() const {
        return RoutesToString(routes);
    }

  protected:
    RandomizerCheck location;
    std::vector<Route> routes;
};

uint16_t GetCheckPrice(RandomizerCheck check = RC_UNKNOWN_CHECK);
uint16_t GetWalletCapacity();

namespace Rando {
class Entrance;
enum class EntranceType;
} // namespace Rando

struct SpiritLogicData {
    // the minimum number of keys that guarantees Child can reach this region
    uint8_t childKeys;
    // The minimum number of keys that guarantees Child can reach this region if they have reverse access
    uint8_t childRevKeys;
    // the minimum number of keys that guarantees Adult can reach this region
    uint8_t adultKeys;
    // the minimum number of keys that guarantees Adult can reach this region with reverse entry
    uint8_t adultRevKeys;
    // The area access condition to reach this region as Child, from the first lock,
    // including the minimum number of keys for ambiguous access
    // 1 key is always assumed to be required
    ConditionFn childAccess;
    // The area access condition to reach this region as Adult, from the first lock
    // including the minimum number of keys for ambiguous access
    // 1 key is always assumed to be required on vanilla
    ConditionFn adultAccess;
    // The area access condition to reach this region from the boss door,
    ConditionFn reverseAccess;
};

// Rooms are numbered from 0, so this stands in for "no room given"
#define ROOM_NONE 0xFF

// An access bool the search was seeded with has no costs behind it, which means arriving fresh
void SeedPathCost(bool access, PathCostSet& costs);

class Region {
  public:
    Region();
    Region(std::string regionName_, SceneID scene_, bool timePass, std::set<RandomizerArea> areas,
           std::vector<EventAccess> events_, std::vector<LocationAccess> locations_, std::list<Rando::Entrance> exits_);
    Region(std::string regionName_, SceneID scene_, std::vector<EventAccess> events_,
           std::vector<LocationAccess> locations_, std::list<Rando::Entrance> exits_);
    Region(std::string regionName_, SceneID scene_, uint8_t room_, std::vector<EventAccess> events_,
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

    // Which room of the scene this region sits in, for the hot room lookup
    uint8_t room = ROOM_NONE;
    // The hot room timer runs here, so getting here does not clear the heat spent so far
    bool hot = false;
    // There is a fire here the player can stand in to grind down to a sixteenth of a heart
    bool canBurnToOne = false;
    // Being here heals, which puts back any health the path spent
    bool heals = false;
    // The heal here is a Great Fairy, so it only happens with Zelda's Lullaby
    bool healNeedsLullaby = false;
    // A fairy can be bottled here, so the path leaves with full bottles
    bool fairyRefill = false;
    // What the cheapest ways in cost, per agetime. An access bool set without any costs behind it
    // means the search was seeded here, which costs nothing.
    PathCostSet childDayCosts;
    PathCostSet childNightCosts;
    PathCostSet adultDayCosts;
    PathCostSet adultNightCosts;

    RandomizerRegion randomizerRegionKey = RR_NONE;

    bool TimePass();

    // Does the path get its health back here? A Great Fairy wants Zelda's Lullaby first
    bool CanHeal() const;

    // Can the path bottle a fairy here, filling every bottle it has?
    bool CanRefillFairies() const;

    void ApplyTimePass();

    bool UpdateEvents();

    void AddExit(RandomizerRegion parentKey, RandomizerRegion newExitKey, ConditionFn condition,
                 std::string conditionStr);

    void SetFairyRefill();

    void RemoveExit(Rando::Entrance* exitToRemove);

    void SetAsPrimary(RandomizerRegion exitToBePrimary);

    Rando::Entrance* GetExit(RandomizerRegion exit);

    bool Child() const {
        return childDay || childNight;
    }

    bool Adult() const {
        return adultDay || adultNight;
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
    bool AnyAgeTime(ConditionFn condition) const {
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

    bool AllAccountedFor() const;

    void ResetVariables();

    void printAgeTimeAccess();
    static std::map<RandomizerRegion, SpiritLogicData> spiritLogicData;
};

extern std::array<Region, RR_MAX> areaTable;
extern std::vector<EventAccess> grottoEvents;

bool AnyAgeTime(ConditionFn condition);
bool SpiritShared(
    RandomizerRegion region, ConditionFn condition, bool anyAge = false, RandomizerRegion otherRegion = RR_NONE,
    ConditionFn otherCondition = [] { return false; }, RandomizerRegion thirdRegion = RR_NONE,
    ConditionFn thirdCondition = [] { return false; });
bool SpiritCertainAccess(RandomizerRegion region);
bool CanPlantBean(RandomizerGet bean);

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

void RegionTable_Init_Root();
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
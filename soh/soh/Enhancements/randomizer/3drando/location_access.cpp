#include "location_access.hpp"

#include "../dungeon.h"
#include "../static_data.h"
#include "../context.h"
#include "item_pool.hpp"
#include "spoiler_log.hpp"
#include "../trial.h"
#include "../entrance.h"
#include "soh/Enhancements/debugger/performanceTimer.h"

#include <fstream>

//generic grotto event list
std::vector<EventAccess> grottoEvents;

//set the logic to be a specific age and time of day and see if the condition still holds
bool LocationAccess::CheckConditionAtAgeTime(bool& age, bool& time) const {

  logic->IsChild = false;
  logic->IsAdult = false;
  logic->AtDay   = false;
  logic->AtNight = false;

  time = true;
  age = true;

  return GetConditionsMet();
}

bool LocationAccess::ConditionsMet() const {
  //WARNING enterance validation can run this after resetting the access for sphere 0 validation
  //When refactoring ToD access, either fix the above or do not assume that we
  //have any access at all just because this is being run
  Region* parentRegion = RegionTable(Rando::Context::GetInstance()->GetItemLocation(location)->GetParentRegionKey());
  bool conditionsMet = false;

  if ((parentRegion->childDay   && CheckConditionAtAgeTime(logic->IsChild, logic->AtDay))   ||
      (parentRegion->childNight && CheckConditionAtAgeTime(logic->IsChild, logic->AtNight)) ||
      (parentRegion->adultDay   && CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay))   ||
      (parentRegion->adultNight && CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight))) {
        conditionsMet = true;
  }

  return conditionsMet && CanBuy();
}

bool LocationAccess::CanBuy() const {
  return CanBuyAnother(location);
}

bool CanBuyAnother(RandomizerCheck rc) {
  uint16_t price = ctx->GetItemLocation(rc)->GetPrice();

  if (price > 500) {
    return logic->HasItem(RG_TYCOON_WALLET);
  } else if (price > 200) {
    return logic->HasItem(RG_GIANT_WALLET);
  } else if (price > 99) {
    return logic->HasItem(RG_ADULT_WALLET);
  } else if (price > 0) {
    return logic->HasItem(RG_CHILD_WALLET);
  }
  return true;
}

Region::Region() = default;
Region::Region(std::string regionName_, std::string scene_, std::set<RandomizerArea> areas,
         bool timePass_,
         std::vector<EventAccess> events_,
         std::vector<LocationAccess> locations_,
         std::list<Rando::Entrance> exits_)
  : regionName(std::move(regionName_)),
    scene(std::move(scene_)),
    areas(areas),
    timePass(timePass_),
    events(std::move(events_)),
    locations(std::move(locations_)),
    exits(std::move(exits_)) {}

Region::~Region() = default;

void Region::ApplyTimePass(){
  if (timePass) {
    StartPerformanceTimer(PT_TOD_ACCESS);
    if (Child()) {
      childDay = true;
      childNight = true;
      RegionTable(RR_ROOT)->childDay = true;
      RegionTable(RR_ROOT)->childNight = true;
    }
    if (Adult()) {
      adultDay = true;
      adultNight = true;
      RegionTable(RR_ROOT)->adultDay = true;
      RegionTable(RR_ROOT)->adultNight = true;
    }
    StopPerformanceTimer(PT_TOD_ACCESS);
  }
}

bool Region::UpdateEvents() {
  bool eventsUpdated =  false;
  StartPerformanceTimer(PT_EVENT_ACCESS);
  for (EventAccess& event : events) {
    
    //If the event has already happened, there's no reason to check it
    if (event.GetEvent()) {
      continue;
    }

    if ((childDay   && event.CheckConditionAtAgeTime(logic->IsChild, logic->AtDay))    ||
        (childNight && event.CheckConditionAtAgeTime(logic->IsChild, logic->AtNight))  ||
        (adultDay   && event.CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay))    ||
        (adultNight && event.CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight))) {
          event.EventOccurred();
          eventsUpdated = true;
    }
  }
  StopPerformanceTimer(PT_EVENT_ACCESS);
  return eventsUpdated;
}

void Region::AddExit(RandomizerRegion parentKey, RandomizerRegion newExitKey, ConditionFn condition) {
  Rando::Entrance newExit = Rando::Entrance(newExitKey, {condition});
  newExit.SetParentRegion(parentKey);
  exits.push_front(newExit);
}

//The exit will be completely removed from this region
void Region::RemoveExit(Rando::Entrance* exitToRemove) {
  exits.remove_if([exitToRemove](const auto exit){return &exit == exitToRemove;});
}

void Region::SetAsPrimary(RandomizerRegion exitToBePrimary) {
  for (auto& exit : exits) {
    if (exit.Getuint32_t() == exitToBePrimary) {
      exit.SetAsPrimary();
      return;
    }
  }
}

Rando::Entrance* Region::GetExit(RandomizerRegion exitToReturn) {
  for (auto& exit : exits) {
    if (exit.Getuint32_t() == exitToReturn) {
      return &exit;
    }
  }
  //auto message = "ERROR: EXIT " + RegionTable(exitToReturn)->regionName + " DOES NOT EXIST IN " + this->regionName;
  //CitraPrint(message);
  return nullptr;
}

bool Region::CanPlantBeanCheck() const {
    return Rando::Context::GetInstance()->GetLogic()->GetAmmo(ITEM_BEAN) > 0 && BothAgesCheck();
}

bool Region::AllAccountedFor() const {
  for (const EventAccess& event : events) {
    if (!event.GetEvent()) {
      return false;
    }
  }

  for (const LocationAccess& loc : locations) {
    if (!(Rando::Context::GetInstance()->GetItemLocation(loc.GetLocation())->IsAddedToPool())) {
      return false;
    }
  }

  for (const auto& exit : exits) {
    if (!exit.GetConnectedRegion()->AllAccess()) {
      return false;
    }
  }

  return AllAccess();
}

bool Region::CheckAllAccess(const RandomizerRegion exitKey) {
  if (!AllAccess()) {
    return false;
  }

  for (Rando::Entrance& exit : exits) {
    if (exit.GetConnectedRegionKey() == exitKey) {
      return exit.CheckConditionAtAgeTime(logic->IsChild, logic->AtDay)   &&
             exit.CheckConditionAtAgeTime(logic->IsChild, logic->AtNight) &&
             exit.CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay)   &&
             exit.CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight);
    }
  }
  return false;
}

void Region::ResetVariables() {
  childDay = false;
  childNight = false;
  adultDay = false;
  adultNight = false;
  addedToPool = false;
  for (auto& exit : exits) {
    exit.RemoveFromPool();
  }
}

std::array<Region, RR_MAX> areaTable;

bool Here(const RandomizerRegion region, ConditionFn condition) {
  return areaTable[region].Here(condition);
}

bool MQSpiritSharedStatueRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge) {
  return areaTable[region].MQSpiritShared(condition, false, anyAge);
}

bool MQSpiritSharedBrokenWallRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge) {
  return areaTable[region].MQSpiritShared(condition, true, anyAge);
}

bool CanPlantBean(const RandomizerRegion region) {
  return areaTable[region].CanPlantBeanCheck();
}

bool BothAges(const RandomizerRegion region) {
  return areaTable[region].BothAgesCheck();
}

bool ChildCanAccess(const RandomizerRegion region) {
  return areaTable[region].Child();
}

bool AdultCanAccess(const RandomizerRegion region) {
  return areaTable[region].Adult();
}

bool HasAccessTo(const RandomizerRegion region) {
  return areaTable[region].HasAccess();
}

Rando::Context* ctx;
std::shared_ptr<Rando::Logic> logic;

void RegionTable_Init() {
  using namespace Rando;
  ctx = Context::GetInstance().get();
  logic = ctx->GetLogic(); //RANDOTODO do not hardcode, instead allow accepting a Logic class somehow
  grottoEvents = {
      EventAccess(&logic->GossipStoneFairy, { [] { return logic->CallGossipFairy(); } }),
      EventAccess(&logic->ButterflyFairy, { [] { return logic->ButterflyFairy || (logic->CanUse(RG_STICKS)); } }),
      EventAccess(&logic->BugShrub, { [] { return logic->CanCutShrubs(); } }),
      EventAccess(&logic->LoneFish, { [] { return true; } }),
  };
  //Clear the array from any previous playthrough attempts. This is important so that
  //locations which appear in both MQ and Vanilla dungeons don't get set in both areas.
  areaTable.fill(Region("Invalid Region", "Invalid Region", {}, NO_DAY_NIGHT_CYCLE, {}, {}, {}));

                       //name, scene, hint text,                       events, locations, exits
  areaTable[RR_ROOT] = Region("Root", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LINKS_POCKET,       true),
                  LOCATION(RC_TRIFORCE_COMPLETED, logic->GetSaveContext()->triforcePiecesCollected >= ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_REQUIRED).Value<uint8_t>();),
                  LOCATION(RC_SARIA_SONG_HINT,    logic->CanUse(RG_SARIAS_SONG)),
                }, {
                  //Exits
                  Entrance(RR_ROOT_EXITS, {[]{return true;}}),
  });

  areaTable[RR_ROOT_EXITS] = Region("Root Exits", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_CHILD_SPAWN,             {[]{return logic->IsChild;}}),
                  Entrance(RR_ADULT_SPAWN,             {[]{return logic->IsAdult;}}),
                  Entrance(RR_MINUET_OF_FOREST_WARP,   {[]{return logic->CanUse(RG_MINUET_OF_FOREST);}}),
                  Entrance(RR_BOLERO_OF_FIRE_WARP,     {[]{return logic->CanUse(RG_BOLERO_OF_FIRE)     && logic->CanLeaveForest();}}),
                  Entrance(RR_SERENADE_OF_WATER_WARP,  {[]{return logic->CanUse(RG_SERENADE_OF_WATER)  && logic->CanLeaveForest();}}),
                  Entrance(RR_NOCTURNE_OF_SHADOW_WARP, {[]{return logic->CanUse(RG_NOCTURNE_OF_SHADOW) && logic->CanLeaveForest();}}),
                  Entrance(RR_REQUIEM_OF_SPIRIT_WARP,  {[]{return logic->CanUse(RG_REQUIEM_OF_SPIRIT)  && logic->CanLeaveForest();}}),
                  Entrance(RR_PRELUDE_OF_LIGHT_WARP,   {[]{return logic->CanUse(RG_PRELUDE_OF_LIGHT)   && logic->CanLeaveForest();}}),
  });

  areaTable[RR_CHILD_SPAWN] = Region("Child Spawn", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KF_LINKS_HOUSE, {[]{return true;}}),
  });

  areaTable[RR_ADULT_SPAWN] = Region("Adult Spawn", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[RR_MINUET_OF_FOREST_WARP] = Region("Minuet of Forest Warp", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_BOLERO_OF_FIRE_WARP] = Region("Bolero of Fire Warp", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_SERENADE_OF_WATER_WARP] = Region("Serenade of Water Warp", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_REQUIEM_OF_SPIRIT_WARP] = Region("Requiem of Spirit Warp", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_NOCTURNE_OF_SHADOW_WARP] = Region("Nocturne of Shadow Warp", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GRAVEYARD_WARP_PAD_REGION, {[]{return true;}}),
  });

  areaTable[RR_PRELUDE_OF_LIGHT_WARP] = Region("Prelude of Light Warp", "", {RA_LINKS_POCKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

  // Overworld
  RegionTable_Init_LostWoods();
  RegionTable_Init_HyruleField();
  RegionTable_Init_CastleTown();
  RegionTable_Init_Kakariko();
  RegionTable_Init_DeathMountain();
  RegionTable_Init_ZorasDomain();
  RegionTable_Init_GerudoValley();
  // Dungeons
  RegionTable_Init_DekuTree();
  RegionTable_Init_DodongosCavern();
  RegionTable_Init_JabuJabusBelly();
  RegionTable_Init_ForestTemple();
  RegionTable_Init_FireTemple();
  RegionTable_Init_WaterTemple();
  RegionTable_Init_SpiritTemple();
  RegionTable_Init_ShadowTemple();
  RegionTable_Init_BottomOfTheWell();
  RegionTable_Init_IceCavern();
  RegionTable_Init_GerudoTrainingGrounds();
  RegionTable_Init_GanonsCastle();

  //Set parent regions
  for (uint32_t i = RR_ROOT; i <= RR_GANONS_CASTLE; i++) {
    for (LocationAccess& locPair : areaTable[i].locations) {
      RandomizerCheck location = locPair.GetLocation();
      Rando::Context::GetInstance()->GetItemLocation(location)->SetParentRegion((RandomizerRegion)i);
    }
    for (Entrance& exit : areaTable[i].exits) {
      exit.SetParentRegion((RandomizerRegion)i);
      exit.SetName();
      exit.GetConnectedRegion()->entrances.push_front(&exit);
    }
  }
  /*
  //Events
}, {
  //Locations
}, {
  //Exits
*/
}

void ReplaceFirstInString(std::string& s, std::string const& toReplace, std::string const& replaceWith) {
    size_t pos = s.find(toReplace);
    if (pos == std::string::npos) {
        return;
    }
    s.replace(pos, toReplace.length(), replaceWith);
}

void ReplaceAllInString(std::string& s, std::string const& toReplace, std::string const& replaceWith) {
    std::string buf;
    size_t pos = 0;
    size_t prevPos;

    buf.reserve(s.size());

    while (true) {
        prevPos = pos;
        pos = s.find(toReplace, pos);
        if (pos == std::string::npos) {
            break;
        }
        buf.append(s, prevPos, pos - prevPos);
        buf += replaceWith;
        pos += toReplace.size();
    }

    buf.append(s, prevPos, s.size() - prevPos);
    s.swap(buf);
}

std::string CleanCheckConditionString(std::string condition) {
    ReplaceAllInString(condition, "logic->", "");
    ReplaceAllInString(condition, "ctx->", "");
    ReplaceAllInString(condition, ".Value<uint8_t>()", "");
    ReplaceAllInString(condition, "GetSaveContext()->", "");
    return condition;
}

namespace Regions {

  const auto GetAllRegions() {
    static const size_t regionCount = RR_MAX - (RR_NONE + 1);

    static std::array<RandomizerRegion, regionCount> allRegions = {};

    static bool intialized = false;
    if (!intialized) {
      for (size_t i = 0; i < regionCount; i++) {
        allRegions[i] = (RandomizerRegion)((RR_NONE + 1) + i);
      }
      intialized = true;
    }

    return allRegions;
  }

  void AccessReset() {
    auto ctx = Rando::Context::GetInstance();
    for (const RandomizerRegion region : GetAllRegions()) {
      RegionTable(region)->ResetVariables();
    }

    if(/*Settings::HasNightStart TODO:: Randomize Starting Time*/ false) {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          RegionTable(RR_ROOT)->childNight = true;
        } else {
          RegionTable(RR_ROOT)->adultNight = true;
        }
      } else {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          RegionTable(RR_ROOT)->childDay = true;
        } else {
          RegionTable(RR_ROOT)->adultDay = true;
        }
    }
  }

  //Reset exits and clear items from locations
  void ResetAllLocations() {
    auto ctx = Rando::Context::GetInstance();
      for (const RandomizerRegion region : GetAllRegions()) {
      RegionTable(region)->ResetVariables();
      //Erase item from every location in this exit
      for (LocationAccess& locPair : RegionTable(region)->locations) {
          RandomizerCheck location = locPair.GetLocation();
          Rando::Context::GetInstance()->GetItemLocation(location)->ResetVariables();
      }
    }

    if (/*Settings::HasNightStart TODO:: Randomize Starting Time*/ false) {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          RegionTable(RR_ROOT)->childNight = true;
        } else {
          RegionTable(RR_ROOT)->adultNight = true;
        }
    } else {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          RegionTable(RR_ROOT)->childDay = true;
        } else {
          RegionTable(RR_ROOT)->adultDay = true;
        }
    }
  }

  bool HasTimePassAccess(uint8_t age) {
      for (const RandomizerRegion regionKey : GetAllRegions()) {
      auto region = RegionTable(regionKey);
      if (region->timePass && ((age == RO_AGE_CHILD && region->Child()) || (age == RO_AGE_ADULT && region->Adult()))) {
        return true;
      }
    }
    return false;
  }

  // Will dump a file which can be turned into a visual graph using graphviz
  // https://graphviz.org/download/
  // Use command: dot -Tsvg <filename> -o world.svg
  // Then open in a browser and CTRL + F to find the area of interest
  void DumpWorldGraph(std::string str) {
    std::ofstream worldGraph;
    worldGraph.open (str + ".dot");
    worldGraph << "digraph {\n\tcenter=true;\n";

    for (const RandomizerRegion regionKey : GetAllRegions()) {
      auto region = RegionTable(regionKey);
      for (auto exit : region->exits) {
        if (exit.GetConnectedRegion()->regionName != "Invalid Region") {
          std::string parent = exit.GetParentRegion()->regionName;
          if (region->childDay) {
            parent += " CD";
          }
          if (region->childNight) {
            parent += " CN";
          }
          if (region->adultDay) {
            parent += " AD";
          }
          if (region->adultNight) {
            parent += " AN";
          }
          Region* connected = exit.GetConnectedRegion();
          auto connectedStr = connected->regionName;
          if (connected->childDay) {
            connectedStr += " CD";
          }
          if (connected->childNight) {
            connectedStr += " CN";
          }
          if (connected->adultDay) {
            connectedStr += " AD";
          }
          if (connected->adultNight) {
            connectedStr += " AN";
          }
          worldGraph << "\t\"" + parent + "\"[shape=\"plain\"];\n";
          worldGraph << "\t\"" + connectedStr + "\"[shape=\"plain\"];\n";
          worldGraph << "\t\"" + parent + "\" -> \"" + connectedStr + "\"\n";
        }
      }
    }
    worldGraph << "}";
    worldGraph.close();
  }

} //namespace Regions

Region* RegionTable(const RandomizerRegion regionKey) {
  if (regionKey > RR_MAX) {
    printf("\x1b[1;1HERROR: AREAKEY TOO BIG");
  }
  return &(areaTable[regionKey]);
}

//Retrieve all the shuffable entrances of a specific type
std::vector<Rando::Entrance*> GetShuffleableEntrances(Rando::EntranceType type, bool onlyPrimary /*= true*/) {
  std::vector<Rando::Entrance*> entrancesToShuffle = {};
  for (RandomizerRegion region : Regions::GetAllRegions()) {
    for (auto& exit : RegionTable(region)->exits) {
      if ((exit.GetType() == type || type == Rando::EntranceType::All) && (exit.IsPrimary() || !onlyPrimary) && exit.GetType() != Rando::EntranceType::None) {
        entrancesToShuffle.push_back(&exit);
      }
    }
  }
  return entrancesToShuffle;
}

// Get the specific entrance by name
Rando::Entrance* GetEntrance(const std::string name) {
  for (RandomizerRegion region : Regions::GetAllRegions()) {
    for (auto& exit : RegionTable(region)->exits) {
      if (exit.GetName() == name) {
        return &exit;
      }
    }
  }

  return nullptr;
}

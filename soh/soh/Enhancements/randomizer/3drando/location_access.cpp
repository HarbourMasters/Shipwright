#include "location_access.hpp"

#include "dungeon.hpp"
#include "item_list.hpp"
#include "item_location.hpp"
#include "item_pool.hpp"
#include "logic.hpp"
#include "settings.hpp"
#include "spoiler_log.hpp"
#include "trial.hpp"
#include "entrance.hpp"

#include <fstream>
#include <iostream>

using namespace Logic;
using namespace Settings;

//generic grotto event list
std::vector<EventAccess> grottoEvents = {
  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || (CanUse(STICKS));}}),
  EventAccess(&BugShrub,         {[]{return CanCutShrubs;}}),
  EventAccess(&LoneFish,         {[]{return true;}}),
};

//set the logic to be a specific age and time of day and see if the condition still holds
bool LocationAccess::CheckConditionAtAgeTime(bool& age, bool& time) const {

  IsChild = false;
  IsAdult = false;
  AtDay   = false;
  AtNight = false;

  time = true;
  age = true;

  UpdateHelpers();
  return GetConditionsMet();
}

bool LocationAccess::ConditionsMet() const {

  Area* parentRegion = AreaTable(Location(location)->GetParentRegionKey());
  bool conditionsMet = false;

  if ((parentRegion->childDay   && CheckConditionAtAgeTime(IsChild, AtDay))   ||
      (parentRegion->childNight && CheckConditionAtAgeTime(IsChild, AtNight)) ||
      (parentRegion->adultDay   && CheckConditionAtAgeTime(IsAdult, AtDay))   ||
      (parentRegion->adultNight && CheckConditionAtAgeTime(IsAdult, AtNight))) {
        conditionsMet = true;
  }

  return conditionsMet && CanBuy();
}

bool LocationAccess::CanBuy() const {
  //Not a shop location, don't need to check if buyable
  if (!(Location(location)->IsCategory(Category::cShop))) {
    return true;
  }

  //Check if wallet is large enough to buy item
  bool SufficientWallet = true;
  if (Location(location)->GetPrice() > 500) {
    SufficientWallet = ProgressiveWallet >= 3;
  } else if (Location(location)->GetPrice() > 200) {
    SufficientWallet = ProgressiveWallet >= 2;
  } else if (Location(location)->GetPrice() > 99) {
    SufficientWallet = ProgressiveWallet >= 1;
  }

  bool OtherCondition = true;
  uint32_t placed = Location(location)->GetPlacedItemKey();
  //Need bottle to buy bottle items, only logically relevant bottle items included here
  if (placed == BUY_BLUE_FIRE || placed == BUY_BOTTLE_BUG || placed == BUY_FISH || placed == BUY_FAIRYS_SPIRIT) {
    OtherCondition = HasBottle;
  }
  //If bombchus in logic, need to have found chus to buy; if not just need bomb bag
  else if (placed == BUY_BOMBCHU_10 || placed == BUY_BOMBCHU_20) {
    OtherCondition = (!BombchusInLogic && Bombs) || (BombchusInLogic && FoundBombchus);
  }

  return SufficientWallet && OtherCondition;
}

Area::Area() = default;
Area::Area(std::string regionName_, std::string scene_, uint32_t hintKey_,
         bool timePass_,
         std::vector<EventAccess> events_,
         std::vector<LocationAccess> locations_,
         std::list<Entrance> exits_)
  : regionName(std::move(regionName_)),
    scene(std::move(scene_)),
    hintKey(hintKey_),
    timePass(timePass_),
    events(std::move(events_)),
    locations(std::move(locations_)),
    exits(std::move(exits_)) {}

Area::~Area() = default;

bool Area::UpdateEvents(SearchMode mode) {

  if (timePass && mode != SearchMode::TimePassAccess) {
    if (Child()) {
      childDay = true;
      childNight = true;
      AreaTable(ROOT)->childDay = true;
      AreaTable(ROOT)->childNight = true;
    }
    if (Adult()) {
      adultDay = true;
      adultNight = true;
      AreaTable(ROOT)->adultDay = true;
      AreaTable(ROOT)->adultNight = true;
    }
  }

  bool eventsUpdated =  false;

  for (EventAccess& event : events) {
    //If the event has already happened, there's no reason to check it
    if (event.GetEvent()) {
      continue;
    }

    if ((childDay   && event.CheckConditionAtAgeTime(IsChild, AtDay))    ||
        (childNight && event.CheckConditionAtAgeTime(IsChild, AtNight))  ||
        (adultDay   && event.CheckConditionAtAgeTime(IsAdult, AtDay))    ||
        (adultNight && event.CheckConditionAtAgeTime(IsAdult, AtNight))) {
          event.EventOccurred();
          eventsUpdated = true;
    }
  }
  return eventsUpdated;
}

void Area::AddExit(uint32_t parentKey, uint32_t newExitKey, ConditionFn condition) {
  Entrance newExit = Entrance(newExitKey, {condition});
  newExit.SetParentRegion(parentKey);
  exits.push_front(newExit);
}

//The exit will be completely removed from this area
void Area::RemoveExit(Entrance* exitToRemove) {
  exits.remove_if([exitToRemove](const auto exit){return &exit == exitToRemove;});
}

void Area::SetAsPrimary(uint32_t exitToBePrimary) {
  for (auto& exit : exits) {
    if (exit.Getuint32_t() == exitToBePrimary) {
      exit.SetAsPrimary();
      return;
    }
  }
}

Entrance* Area::GetExit(uint32_t exitToReturn) {
  for (auto& exit : exits) {
    if (exit.Getuint32_t() == exitToReturn) {
      return &exit;
    }
  }
  //auto message = "ERROR: EXIT " + AreaTable(exitToReturn)->regionName + " DOES NOT EXIST IN " + this->regionName;
  //CitraPrint(message);
  return nullptr;
}

bool Area::CanPlantBeanCheck() const {
  return (Logic::MagicBean || Logic::MagicBeanPack) && BothAgesCheck();
}

bool Area::AllAccountedFor() const {
  for (const EventAccess& event : events) {
    if (!event.GetEvent()) {
      return false;
    }
  }

  for (const LocationAccess& loc : locations) {
    if (!(Location(loc.GetLocation())->IsAddedToPool())) {
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

bool Area::CheckAllAccess(const uint32_t exitKey) {
  if (!AllAccess()) {
    return false;
  }

  for (Entrance& exit : exits) {
    if (exit.Getuint32_t() == exitKey) {
      return exit.CheckConditionAtAgeTime(Logic::IsChild, Logic::AtDay)   &&
             exit.CheckConditionAtAgeTime(Logic::IsChild, Logic::AtNight) &&
             exit.CheckConditionAtAgeTime(Logic::IsAdult, Logic::AtDay)   &&
             exit.CheckConditionAtAgeTime(Logic::IsAdult, Logic::AtNight);
    }
  }
  return false;
}

void Area::ResetVariables() {
  childDay = false;
  childNight = false;
  adultDay = false;
  adultNight = false;
  addedToPool = false;
  for (auto& exit : exits) {
    exit.RemoveFromPool();
  }
}

std::array<Area, KEY_ENUM_MAX> areaTable;

bool Here(const uint32_t area, ConditionFn condition) {
  return areaTable[area].HereCheck(condition);
}

bool CanPlantBean(const uint32_t area) {
  return areaTable[area].CanPlantBeanCheck();
}

bool BothAges(const uint32_t area) {
  return areaTable[area].BothAgesCheck();
}

bool ChildCanAccess(const uint32_t area) {
  return areaTable[area].Child();
}

bool AdultCanAccess(const uint32_t area) {
  return areaTable[area].Adult();
}

bool HasAccessTo(const uint32_t area) {
  return areaTable[area].HasAccess();
}



void AreaTable_Init() {
  //Clear the array from any previous playthrough attempts. This is important so that
  //locations which appear in both MQ and Vanilla dungeons don't get set in both areas.
  areaTable.fill(Area("Invalid Area", "Invalid Area", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {}));

                       //name, scene, hint text,                       events, locations, exits
  areaTable[ROOT] = Area("Root", "", LINKS_POCKET, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LINKS_POCKET, {[]{return true;}}),
                  LocationAccess(TRIFORCE_COMPLETED, { [] { return CanCompleteTriforce;}}),
                }, {
                  //Exits
                  Entrance(ROOT_EXITS, {[]{return true;}})
  });

  areaTable[ROOT_EXITS] = Area("Root Exits", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(CHILD_SPAWN,             {[]{return IsChild;}}),
                  Entrance(ADULT_SPAWN,             {[]{return IsAdult;}}),
                  Entrance(MINUET_OF_FOREST_WARP,   {[]{return CanPlay(MinuetOfForest);}}),
                  Entrance(BOLERO_OF_FIRE_WARP,     {[]{return CanPlay(BoleroOfFire)     && CanLeaveForest;}}),
                  Entrance(SERENADE_OF_WATER_WARP,  {[]{return CanPlay(SerenadeOfWater)  && CanLeaveForest;}}),
                  Entrance(NOCTURNE_OF_SHADOW_WARP, {[]{return CanPlay(NocturneOfShadow) && CanLeaveForest;}}),
                  Entrance(REQUIEM_OF_SPIRIT_WARP,  {[]{return CanPlay(RequiemOfSpirit)  && CanLeaveForest;}}),
                  Entrance(PRELUDE_OF_LIGHT_WARP,   {[]{return CanPlay(PreludeOfLight)   && CanLeaveForest;}}),
  });

  areaTable[CHILD_SPAWN] = Area("Child Spawn", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KF_LINKS_HOUSE, {[]{return true;}}),
  });

  areaTable[ADULT_SPAWN] = Area("Adult Spawn", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[MINUET_OF_FOREST_WARP] = Area("Minuet of Forest Warp", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[BOLERO_OF_FIRE_WARP] = Area("Bolero of Fire Warp", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DMC_CENTRAL_LOCAL, {[]{return true;}}),
  });

  areaTable[SERENADE_OF_WATER_WARP] = Area("Serenade of Water Warp", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[REQUIEM_OF_SPIRIT_WARP] = Area("Requiem of Spirit Warp", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[NOCTURNE_OF_SHADOW_WARP] = Area("Nocturne of Shadow Warp", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GRAVEYARD_WARP_PAD_REGION, {[]{return true;}}),
  });

  areaTable[PRELUDE_OF_LIGHT_WARP] = Area("Prelude of Light Warp", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(TEMPLE_OF_TIME, {[]{return true;}}),
  });

  // Overworld
  AreaTable_Init_LostWoods();
  AreaTable_Init_HyruleField();
  AreaTable_Init_CastleTown();
  AreaTable_Init_Kakariko();
  AreaTable_Init_DeathMountain();
  AreaTable_Init_ZorasDomain();
  AreaTable_Init_GerudoValley();
  // Dungeons
  AreaTable_Init_DekuTree();
  AreaTable_Init_DodongosCavern();
  AreaTable_Init_JabuJabusBelly();
  AreaTable_Init_ForestTemple();
  AreaTable_Init_FireTemple();
  AreaTable_Init_WaterTemple();
  AreaTable_Init_SpiritTemple();
  AreaTable_Init_ShadowTemple();
  AreaTable_Init_BottomOfTheWell();
  AreaTable_Init_IceCavern();
  AreaTable_Init_GerudoTrainingGrounds();
  AreaTable_Init_GanonsCastle();

  //Set parent regions
  for (uint32_t i = ROOT; i <= GANONS_CASTLE; i++) {
    for (LocationAccess& locPair : areaTable[i].locations) {
      uint32_t location = locPair.GetLocation();
      Location(location)->SetParentRegion(i);
    }
    for (Entrance& exit : areaTable[i].exits) {
      exit.SetParentRegion(i);
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

namespace Areas {

  const auto GetAllAreas() {
    static const size_t areaCount = MARKER_AREAS_END - (MARKER_AREAS_START + 1);

    static std::array<uint32_t, areaCount> allAreas = {};

    static bool intialized = false;
    if (!intialized) {
      for (size_t i = 0; i < areaCount; i++) {
        allAreas[i] = (MARKER_AREAS_START + 1) + i;
      }
      intialized = true;
    }

    return allAreas;
  }

  void AccessReset() {
      for (const uint32_t area : GetAllAreas()) {
      AreaTable(area)->ResetVariables();
    }

    if(Settings::HasNightStart) {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childNight = true;
        } else {
          AreaTable(ROOT)->adultNight = true;
        }
      } else {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childDay = true;
        } else {
          AreaTable(ROOT)->adultDay = true;
        }
    }
  }

  //Reset exits and clear items from locations
  void ResetAllLocations() {
      for (const uint32_t area : GetAllAreas()) {
      AreaTable(area)->ResetVariables();
      //Erase item from every location in this exit
      for (LocationAccess& locPair : AreaTable(area)->locations) {
          uint32_t location = locPair.GetLocation();
          Location(location)->ResetVariables();
      }
    }

    if(Settings::HasNightStart) {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childNight = true;
        } else {
          AreaTable(ROOT)->adultNight = true;
        }
      } else {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childDay = true;
        } else {
          AreaTable(ROOT)->adultDay = true;
        }
    }
  }

  bool HasTimePassAccess(uint8_t age) {
      for (const uint32_t areaKey : GetAllAreas()) {
      auto area = AreaTable(areaKey);
      if (area->timePass && ((age == AGE_CHILD && area->Child()) || (age == AGE_ADULT && area->Adult()))) {
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

    for (const uint32_t areaKey : GetAllAreas()) {
      auto area = AreaTable(areaKey);
      for (auto exit : area->exits) {
        if (exit.GetConnectedRegion()->regionName != "Invalid Area") {
          std::string parent = exit.GetParentRegion()->regionName;
          if (area->childDay) {
            parent += " CD";
          }
          if (area->childNight) {
            parent += " CN";
          }
          if (area->adultDay) {
            parent += " AD";
          }
          if (area->adultNight) {
            parent += " AN";
          }
          Area* connected = exit.GetConnectedRegion();
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

} //namespace Areas

Area* AreaTable(const uint32_t areaKey) {
  if (areaKey > KEY_ENUM_MAX) {
    printf("\x1b[1;1HERROR: AREAKEY TOO BIG");
  }
  return &(areaTable[areaKey]);
}

//Retrieve all the shuffable entrances of a specific type
std::vector<Entrance*> GetShuffleableEntrances(EntranceType type, bool onlyPrimary /*= true*/) {
  std::vector<Entrance*> entrancesToShuffle = {};
    for (uint32_t area : Areas::GetAllAreas()) {
    for (auto& exit: AreaTable(area)->exits) {
      if ((exit.GetType() == type || type == EntranceType::All) && (exit.IsPrimary() || !onlyPrimary) && exit.GetType() != EntranceType::None) {
        entrancesToShuffle.push_back(&exit);
      }
    }
  }
  return entrancesToShuffle;
}

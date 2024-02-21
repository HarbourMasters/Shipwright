#include "location_access.hpp"

#include "../dungeon.h"
#include "../static_data.h"
#include "../context.h"
#include "item_pool.hpp"
#include "spoiler_log.hpp"
#include "../trial.h"
#include "../entrance.h"

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

  logic->UpdateHelpers();
  return GetConditionsMet();
}

bool LocationAccess::ConditionsMet() const {

  Area* parentRegion = AreaTable(Rando::Context::GetInstance()->GetItemLocation(location)->GetParentRegionKey());
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
  auto ctx = Rando::Context::GetInstance();
  //Not a shop or scrub location, don't need to check if buyable
  if (!(Rando::StaticData::GetLocation(location)->IsCategory(Category::cShop)) && !(Rando::StaticData::GetLocation(location)->IsCategory(Category::cDekuScrub))) {
    return true;
  }

  //Check if wallet is large enough to buy item
  bool SufficientWallet = true;
  if (ctx->GetItemLocation(location)->GetPrice() > 500) {
    SufficientWallet = logic->ProgressiveWallet >= 4;
  } else if (ctx->GetItemLocation(location)->GetPrice() > 200) {
    SufficientWallet = logic->ProgressiveWallet >= 3;
  } else if (ctx->GetItemLocation(location)->GetPrice() > 99) {
    SufficientWallet = logic->ProgressiveWallet >= 2;
  } else if (ctx->GetItemLocation(location)->GetPrice() > 0) {
    SufficientWallet = logic->ProgressiveWallet >= 1;
  }

  bool OtherCondition = true;
  RandomizerGet placed = ctx->GetItemLocation(location)->GetPlacedRandomizerGet();
  //Need bottle to buy bottle items, only logically relevant bottle items included here
  if (placed == RG_BUY_BLUE_FIRE || placed == RG_BUY_BOTTLE_BUG || placed == RG_BUY_FISH ||
      placed == RG_BUY_FAIRYS_SPIRIT) {
      OtherCondition = logic->HasBottle;
  }
  // If bombchus in logic, need to have found chus to buy; if not just need bomb bag
  else if (placed == RG_BUY_BOMBCHU_10 || placed == RG_BUY_BOMBCHU_20) {
      OtherCondition =
          (!ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->Bombs) || (ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->FoundBombchus);
  }

  return SufficientWallet && OtherCondition;
}

Area::Area() = default;
Area::Area(std::string regionName_, std::string scene_, RandomizerArea area,
         bool timePass_,
         std::vector<EventAccess> events_,
         std::vector<LocationAccess> locations_,
         std::list<Rando::Entrance> exits_)
  : regionName(std::move(regionName_)),
    scene(std::move(scene_)),
    area(area),
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
      AreaTable(RR_ROOT)->childDay = true;
      AreaTable(RR_ROOT)->childNight = true;
    }
    if (Adult()) {
      adultDay = true;
      adultNight = true;
      AreaTable(RR_ROOT)->adultDay = true;
      AreaTable(RR_ROOT)->adultNight = true;
    }
  }

  bool eventsUpdated =  false;

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
  return eventsUpdated;
}

void Area::AddExit(RandomizerRegion parentKey, RandomizerRegion newExitKey, ConditionFn condition) {
  Rando::Entrance newExit = Rando::Entrance(newExitKey, {condition});
  newExit.SetParentRegion(parentKey);
  exits.push_front(newExit);
}

//The exit will be completely removed from this area
void Area::RemoveExit(Rando::Entrance* exitToRemove) {
  exits.remove_if([exitToRemove](const auto exit){return &exit == exitToRemove;});
}

void Area::SetAsPrimary(RandomizerRegion exitToBePrimary) {
  for (auto& exit : exits) {
    if (exit.Getuint32_t() == exitToBePrimary) {
      exit.SetAsPrimary();
      return;
    }
  }
}

Rando::Entrance* Area::GetExit(RandomizerRegion exitToReturn) {
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
  return (logic->MagicBean || logic->MagicBeanPack) && BothAgesCheck();
}

bool Area::AllAccountedFor() const {
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

bool Area::CheckAllAccess(const RandomizerRegion exitKey) {
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

std::array<Area, RR_MAX> areaTable;

bool Here(const RandomizerRegion area, ConditionFn condition) {
  return areaTable[area].HereCheck(condition);
}

bool CanPlantBean(const RandomizerRegion area) {
  return areaTable[area].CanPlantBeanCheck();
}

bool BothAges(const RandomizerRegion area) {
  return areaTable[area].BothAgesCheck();
}

bool ChildCanAccess(const RandomizerRegion area) {
  return areaTable[area].Child();
}

bool AdultCanAccess(const RandomizerRegion area) {
  return areaTable[area].Adult();
}

bool HasAccessTo(const RandomizerRegion area) {
  return areaTable[area].HasAccess();
}

std::shared_ptr<Rando::Context> randoCtx;
std::shared_ptr<Rando::Logic> logic;

void AreaTable_Init() {
  using namespace Rando;
  randoCtx = Context::GetInstance();
  logic = randoCtx->GetLogic();
  grottoEvents = {
      EventAccess(&logic->GossipStoneFairy, { [] { return logic->GossipStoneFairy || logic->CanSummonGossipFairy; } }),
      EventAccess(&logic->ButterflyFairy, { [] { return logic->ButterflyFairy || (logic->CanUse(RG_STICKS)); } }),
      EventAccess(&logic->BugShrub, { [] { return logic->CanCutShrubs; } }),
      EventAccess(&logic->LoneFish, { [] { return true; } }),
  };
  //Clear the array from any previous playthrough attempts. This is important so that
  //locations which appear in both MQ and Vanilla dungeons don't get set in both areas.
  areaTable.fill(Area("Invalid Area", "Invalid Area", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {}));

                       //name, scene, hint text,                       events, locations, exits
  areaTable[RR_ROOT] = Area("Root", "", RA_LINKS_POCKET, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LINKS_POCKET,       {[]{return true;}}),
                  LocationAccess(RC_TRIFORCE_COMPLETED, {[]{return logic->CanCompleteTriforce;}}),
                  LocationAccess(RC_SARIA_SONG_HINT,    {[]{return logic->CanUse(RG_SARIAS_SONG);}}),
                }, {
                  //Exits
                  Entrance(RR_ROOT_EXITS, {[]{return true;}})
  });

  areaTable[RR_ROOT_EXITS] = Area("Root Exits", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_CHILD_SPAWN,             {[]{return logic->IsChild;}}),
                  Entrance(RR_ADULT_SPAWN,             {[]{return logic->IsAdult;}}),
                  Entrance(RR_MINUET_OF_FOREST_WARP,   {[]{return logic->CanUse(RG_MINUET_OF_FOREST);}}),
                  Entrance(RR_BOLERO_OF_FIRE_WARP,     {[]{return logic->CanUse(RG_BOLERO_OF_FIRE)     && logic->CanLeaveForest;}}),
                  Entrance(RR_SERENADE_OF_WATER_WARP,  {[]{return logic->CanUse(RG_SERENADE_OF_WATER)  && logic->CanLeaveForest;}}),
                  Entrance(RR_NOCTURNE_OF_SHADOW_WARP, {[]{return logic->CanUse(RG_NOCTURNE_OF_SHADOW) && logic->CanLeaveForest;}}),
                  Entrance(RR_REQUIEM_OF_SPIRIT_WARP,  {[]{return logic->CanUse(RG_REQUIEM_OF_SPIRIT)  && logic->CanLeaveForest;}}),
                  Entrance(RR_PRELUDE_OF_LIGHT_WARP,   {[]{return logic->CanUse(RG_PRELUDE_OF_LIGHT)   && logic->CanLeaveForest;}}),
  });

  areaTable[RR_CHILD_SPAWN] = Area("Child Spawn", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KF_LINKS_HOUSE, {[]{return true;}}),
  });

  areaTable[RR_ADULT_SPAWN] = Area("Adult Spawn", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[RR_MINUET_OF_FOREST_WARP] = Area("Minuet of Forest Warp", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_BOLERO_OF_FIRE_WARP] = Area("Bolero of Fire Warp", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_SERENADE_OF_WATER_WARP] = Area("Serenade of Water Warp", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_REQUIEM_OF_SPIRIT_WARP] = Area("Requiem of Spirit Warp", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_NOCTURNE_OF_SHADOW_WARP] = Area("Nocturne of Shadow Warp", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GRAVEYARD_WARP_PAD_REGION, {[]{return true;}}),
  });

  areaTable[RR_PRELUDE_OF_LIGHT_WARP] = Area("Prelude of Light Warp", "", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
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

namespace Areas {

  const auto GetAllAreas() {
    static const size_t areaCount = RR_MAX - (RR_NONE + 1);

    static std::array<RandomizerRegion, areaCount> allAreas = {};

    static bool intialized = false;
    if (!intialized) {
      for (size_t i = 0; i < areaCount; i++) {
        allAreas[i] = (RandomizerRegion)((RR_NONE + 1) + i);
      }
      intialized = true;
    }

    return allAreas;
  }

  void AccessReset() {
    auto ctx = Rando::Context::GetInstance();
      for (const RandomizerRegion area : GetAllAreas()) {
      AreaTable(area)->ResetVariables();
    }

    if(/*Settings::HasNightStart TODO:: Randomize Starting Time*/ false) {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          AreaTable(RR_ROOT)->childNight = true;
        } else {
          AreaTable(RR_ROOT)->adultNight = true;
        }
      } else {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          AreaTable(RR_ROOT)->childDay = true;
        } else {
          AreaTable(RR_ROOT)->adultDay = true;
        }
    }
  }

  //Reset exits and clear items from locations
  void ResetAllLocations() {
    auto ctx = Rando::Context::GetInstance();
      for (const RandomizerRegion area : GetAllAreas()) {
      AreaTable(area)->ResetVariables();
      //Erase item from every location in this exit
      for (LocationAccess& locPair : AreaTable(area)->locations) {
          RandomizerCheck location = locPair.GetLocation();
          Rando::Context::GetInstance()->GetItemLocation(location)->ResetVariables();
      }
    }

    if (/*Settings::HasNightStart TODO:: Randomize Starting Time*/ false) {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          AreaTable(RR_ROOT)->childNight = true;
        } else {
          AreaTable(RR_ROOT)->adultNight = true;
        }
    } else {
        if(ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
          AreaTable(RR_ROOT)->childDay = true;
        } else {
          AreaTable(RR_ROOT)->adultDay = true;
        }
    }
  }

  bool HasTimePassAccess(uint8_t age) {
      for (const RandomizerRegion areaKey : GetAllAreas()) {
      auto area = AreaTable(areaKey);
      if (area->timePass && ((age == RO_AGE_CHILD && area->Child()) || (age == RO_AGE_ADULT && area->Adult()))) {
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

    for (const RandomizerRegion areaKey : GetAllAreas()) {
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

Area* AreaTable(const RandomizerRegion areaKey) {
  if (areaKey > RR_MAX) {
    printf("\x1b[1;1HERROR: AREAKEY TOO BIG");
  }
  return &(areaTable[areaKey]);
}

//Retrieve all the shuffable entrances of a specific type
std::vector<Rando::Entrance*> GetShuffleableEntrances(Rando::EntranceType type, bool onlyPrimary /*= true*/) {
  std::vector<Rando::Entrance*> entrancesToShuffle = {};
  for (RandomizerRegion area : Areas::GetAllAreas()) {
    for (auto& exit : AreaTable(area)->exits) {
      if ((exit.GetType() == type || type == Rando::EntranceType::All) && (exit.IsPrimary() || !onlyPrimary) && exit.GetType() != Rando::EntranceType::None) {
        entrancesToShuffle.push_back(&exit);
      }
    }
  }
  return entrancesToShuffle;
}

// Get the specific entrance by name
Rando::Entrance* GetEntrance(const std::string name) {
  for (RandomizerRegion area : Areas::GetAllAreas()) {
    for (auto& exit : AreaTable(area)->exits) {
      if (exit.GetName() == name) {
        return &exit;
      }
    }
  }

  return nullptr;
}

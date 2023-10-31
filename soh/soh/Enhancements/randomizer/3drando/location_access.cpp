#include "location_access.hpp"

#include "../dungeon.h"
#include "../static_data.h"
#include "../context.h"
#include "item_pool.hpp"
#include "logic.hpp"
#include "spoiler_log.hpp"
#include "../trial.h"
#include "../entrance.h"

#include <fstream>
#include <iostream>

using namespace Logic;

//generic grotto event list
std::vector<EventAccess> grottoEvents = {
  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || (CanUse(RG_STICKS));}}),
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

  Area* parentRegion = AreaTable(Rando::Context::GetInstance()->GetItemLocation(location)->GetParentRegionKey());
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
  auto ctx = Rando::Context::GetInstance();
  //Not a shop location, don't need to check if buyable
  if (!(Rando::StaticData::GetLocation(location)->IsCategory(Category::cShop))) {
    return true;
  }

  //Check if wallet is large enough to buy item
  bool SufficientWallet = true;
  if (ctx->GetItemLocation(location)->GetPrice() > 500) {
    SufficientWallet = ProgressiveWallet >= 3;
  } else if (ctx->GetItemLocation(location)->GetPrice() > 200) {
    SufficientWallet = ProgressiveWallet >= 2;
  } else if (ctx->GetItemLocation(location)->GetPrice() > 99) {
    SufficientWallet = ProgressiveWallet >= 1;
  }

  bool OtherCondition = true;
  RandomizerGet placed = ctx->GetItemLocation(location)->GetPlacedRandomizerGet();
  //Need bottle to buy bottle items, only logically relevant bottle items included here
  if (placed == RG_BUY_BLUE_FIRE || placed == RG_BUY_BOTTLE_BUG || placed == RG_BUY_FISH ||
      placed == RG_BUY_FAIRYS_SPIRIT) {
      OtherCondition = HasBottle;
  }
  // If bombchus in logic, need to have found chus to buy; if not just need bomb bag
  else if (placed == RG_BUY_BOMBCHU_10 || placed == RG_BUY_BOMBCHU_20) {
      OtherCondition =
          (!ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && Bombs) || (ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && FoundBombchus);
  }

  return SufficientWallet && OtherCondition;
}

Area::Area() = default;
Area::Area(std::string regionName_, std::string scene_, RandomizerHintTextKey hintKey_,
         bool timePass_,
         std::vector<EventAccess> events_,
         std::vector<LocationAccess> locations_,
         std::list<Rando::Entrance> exits_)
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
  return (Logic::MagicBean || Logic::MagicBeanPack) && BothAgesCheck();
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

void AreaTable_Init() {
  using namespace Rando;
  randoCtx = Context::GetInstance();
  //Clear the array from any previous playthrough attempts. This is important so that
  //locations which appear in both MQ and Vanilla dungeons don't get set in both areas.
  areaTable.fill(Area("Invalid Area", "Invalid Area", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {}));

                       //name, scene, hint text,                       events, locations, exits
  areaTable[RR_ROOT] = Area("Root", "", RHT_LINKS_POCKET, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LINKS_POCKET,       {[]{return true;}}),
                  LocationAccess(RC_TRIFORCE_COMPLETED, {[]{return CanCompleteTriforce;}}),
                  LocationAccess(RC_SARIA_SONG_HINT,    {[]{return CanPlay(SariasSong);}}),
                }, {
                  //Exits
                  Entrance(RR_ROOT_EXITS, {[]{return true;}})
  });

  areaTable[RR_ROOT_EXITS] = Area("Root Exits", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_CHILD_SPAWN,             {[]{return IsChild;}}),
                  Entrance(RR_ADULT_SPAWN,             {[]{return IsAdult;}}),
                  Entrance(RR_MINUET_OF_FOREST_WARP,   {[]{return CanPlay(MinuetOfForest);}}),
                  Entrance(RR_BOLERO_OF_FIRE_WARP,     {[]{return CanPlay(BoleroOfFire)     && CanLeaveForest;}}),
                  Entrance(RR_SERENADE_OF_WATER_WARP,  {[]{return CanPlay(SerenadeOfWater)  && CanLeaveForest;}}),
                  Entrance(RR_NOCTURNE_OF_SHADOW_WARP, {[]{return CanPlay(NocturneOfShadow) && CanLeaveForest;}}),
                  Entrance(RR_REQUIEM_OF_SPIRIT_WARP,  {[]{return CanPlay(RequiemOfSpirit)  && CanLeaveForest;}}),
                  Entrance(RR_PRELUDE_OF_LIGHT_WARP,   {[]{return CanPlay(PreludeOfLight)   && CanLeaveForest;}}),
  });

  areaTable[RR_CHILD_SPAWN] = Area("Child Spawn", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KF_LINKS_HOUSE, {[]{return true;}}),
  });

  areaTable[RR_ADULT_SPAWN] = Area("Adult Spawn", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[RR_MINUET_OF_FOREST_WARP] = Area("Minuet of Forest Warp", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_BOLERO_OF_FIRE_WARP] = Area("Bolero of Fire Warp", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_SERENADE_OF_WATER_WARP] = Area("Serenade of Water Warp", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_REQUIEM_OF_SPIRIT_WARP] = Area("Requiem of Spirit Warp", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_NOCTURNE_OF_SHADOW_WARP] = Area("Nocturne of Shadow Warp", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GRAVEYARD_WARP_PAD_REGION, {[]{return true;}}),
  });

  areaTable[RR_PRELUDE_OF_LIGHT_WARP] = Area("Prelude of Light Warp", "", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
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
    for (auto& exit: AreaTable(area)->exits) {
      if ((exit.GetType() == type || type == Rando::EntranceType::All) && (exit.IsPrimary() || !onlyPrimary) && exit.GetType() != Rando::EntranceType::None) {
        entrancesToShuffle.push_back(&exit);
      }
    }
  }
  return entrancesToShuffle;
}

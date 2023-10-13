#include "entrance.hpp"

#include "fill.hpp"
#include "settings.hpp"
#include "item_pool.hpp"
#include "../context.h"
#include "spoiler_log.hpp"
#include "hints.hpp"
#include "location_access.hpp"
#include "soh/Enhancements/randomizer/static_data.h"

#include <vector>
#include <utility>
#include <set>
#include <map>
#include <spdlog/spdlog.h>

std::list<EntranceOverride> entranceOverrides = {};
bool noRandomEntrances = false;
static bool entranceShuffleFailure = false;
static int totalRandomizableEntrances = 0;
static int curNumRandomizedEntrances = 0;

typedef struct {
    EntranceType type;
    RandomizerRegion parentRegion;
    RandomizerRegion connectedRegion;
    int16_t index;
    int16_t blueWarp;
} EntranceLinkInfo;

EntranceLinkInfo NO_RETURN_ENTRANCE = {EntranceType::None, RR_NONE, RR_NONE, -1};

typedef struct {
    std::list<RandomizerRegion> targetRegions;
    std::list<EntranceType> allowedTypes;
} PriorityEntrance;
                                 //primary,          secondary
using EntranceInfoPair = std::pair<EntranceLinkInfo, EntranceLinkInfo>;
using EntrancePair = std::pair<Entrance*, Entrance*>;
using EntrancePools = std::map<EntranceType, std::vector<Entrance*>>;

//The entrance randomization algorithm used here is a direct copy of
//the algorithm used in the original N64 randomizer (except now in C++ instead
//of python). It may be easier to understand the algorithm by looking at the
//base randomizer's code instead:
// https://github.com/TestRunnerSRL/OoT-Randomizer/blob/Dev/EntranceShuffle.py

// Updates the user on how many entrances are currently shuffled
static void DisplayEntranceProgress() {
  std::string dots = ".";
  float progress = (float)curNumRandomizedEntrances / (float)totalRandomizableEntrances;
  if (progress > 0.33) {
    dots += ".";
  } else {
    dots += " ";
  }
  if (progress > 0.66) {
    dots += ".";
  } else {
    dots += " ";
  }
  printf("\x1b[7;29H%s", dots.c_str());
  #ifdef ENABLE_DEBUG
    if (curNumRandomizedEntrances == totalRandomizableEntrances) {
      Areas::DumpWorldGraph("Finish Validation");
    }
  #endif
}

void SetAllEntrancesData(std::vector<EntranceInfoPair>& entranceShuffleTable) {
  for (auto& entrancePair: entranceShuffleTable) {

    auto& forwardEntry = entrancePair.first;
    auto& returnEntry = entrancePair.second;

    //set data
    Entrance* forwardEntrance = AreaTable(forwardEntry.parentRegion)->GetExit(forwardEntry.connectedRegion);
    forwardEntrance->SetIndex(forwardEntry.index);
    forwardEntrance->SetBlueWarp(forwardEntry.blueWarp);
    forwardEntrance->SetType(forwardEntry.type);
    forwardEntrance->SetAsPrimary();

    // When decouple entrances is on, mark it for entrances except boss rooms
    if (Settings::DecoupleEntrances && forwardEntry.type != EntranceType::ChildBoss &&
      forwardEntry.type != EntranceType::AdultBoss) {
      forwardEntrance->SetDecoupled();
    }

    if (returnEntry.parentRegion != RR_NONE) {
      Entrance* returnEntrance = AreaTable(returnEntry.parentRegion)->GetExit(returnEntry.connectedRegion);
      returnEntrance->SetIndex(returnEntry.index);
      returnEntrance->SetBlueWarp(returnEntry.blueWarp);
      returnEntrance->SetType(returnEntry.type);
      forwardEntrance->BindTwoWay(returnEntrance);

      // Mark reverse entrance as decoupled
      if (Settings::DecoupleEntrances && returnEntry.type != EntranceType::ChildBoss &&
        returnEntry.type != EntranceType::AdultBoss) {
        returnEntrance->SetDecoupled();
      }
    }
  }
}

static std::vector<Entrance*> AssumeEntrancePool(std::vector<Entrance*>& entrancePool) {
  std::vector<Entrance*> assumedPool = {};
  for (Entrance* entrance : entrancePool) {
    totalRandomizableEntrances++;
    Entrance* assumedForward = entrance->AssumeReachable();
    if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
      Entrance* assumedReturn = entrance->GetReverse()->AssumeReachable();
      if (!(Settings::MixedEntrancePools && (Settings::ShuffleOverworldEntrances || Settings::ShuffleInteriorEntrances.Is(SHUFFLEINTERIORS_ALL)))) {
        auto type = entrance->GetType();
        if (((type == EntranceType::Dungeon || type == EntranceType::GrottoGrave) && entrance->GetReverse()->GetName() != "Spirit Temple Entryway -> Desert Colossus From Spirit Entryway") ||
             (type == EntranceType::Interior && Settings::ShuffleInteriorEntrances.Is(SHUFFLEINTERIORS_ALL))) {
               // In most cases, Dungeon, Grotto/Grave and Simple Interior exits shouldn't be assumed able to give access to their parent region
               assumedReturn->SetCondition([]{return false;});
             }
      }
      assumedForward->BindTwoWay(assumedReturn);
    }
    assumedPool.push_back(assumedForward);
  }
  return assumedPool;
}

static std::vector<Entrance*> BuildOneWayTargets(std::vector<EntranceType> typesToInclude, std::vector<std::pair<RandomizerRegion, RandomizerRegion>> exclude = {}/*, target_region_names*/) {
  std::vector<Entrance*> oneWayEntrances = {};
  // Get all entrances of the specified type
  for (EntranceType poolType : typesToInclude) {
    AddElementsToPool(oneWayEntrances, GetShuffleableEntrances(poolType, false));
  }
  // Filter out any that are passed in the exclusion list
  FilterAndEraseFromPool(oneWayEntrances, [&exclude](Entrance* entrance){
    std::pair<RandomizerRegion, RandomizerRegion> entranceBeingChecked (entrance->GetParentRegionKey(), entrance->GetConnectedRegionKey());
    return ElementInContainer(entranceBeingChecked, exclude);
  });

  // The code below is part of the function in ootr, but no use of the function ever provides target_region_names
  // if target_region_names:
  //     return [entrance.get_new_target() for entrance in valid_one_way_entrances
  //             if entrance.connected_region.name in target_region_names]

  std::vector<Entrance*> newTargets = {};
  for (Entrance* entrance : oneWayEntrances) {
    newTargets.push_back(entrance->GetNewTarget());
  }
  return newTargets;
}

//returns restrictive entrances and soft entrances in an array of size 2 (restrictive vector is index 0, soft is index 1)
static std::array<std::vector<Entrance*>, 2> SplitEntrancesByRequirements(std::vector<Entrance*>& entrancesToSplit, std::vector<Entrance*>& assumedEntrances) {
  //First, disconnect all root assumed entrances and save which regions they were originally connected to, so we can reconnect them later
  std::map<Entrance*, RandomizerRegion> originalConnectedRegions = {};
  std::set<Entrance*> entrancesToDisconnect = {};
  for (Entrance* entrance : assumedEntrances) {
    entrancesToDisconnect.insert(entrance);
    if (entrance->GetReverse() != nullptr) {
      entrancesToDisconnect.insert(entrance->GetReverse());
    }
  }

  //disconnect each entrance temporarily to find restrictive vs soft entrances
  //soft entrances are ones that can be accessed by both ages (child/adult) at both times of day (day/night)
  //restrictive entrances are ones that do not meet this criteria
  for (Entrance* entrance : entrancesToDisconnect) {
    if (entrance->GetConnectedRegionKey() != RR_NONE) {
      originalConnectedRegions[entrance] = entrance->Disconnect();
    }
  }

  std::vector<Entrance*> restrictiveEntrances = {};
  std::vector<Entrance*> softEntrances = {};

  Logic::LogicReset();
  // Apply the effects of all advancement items to search for entrance accessibility
  std::vector<RandomizerGet> items = 
    FilterFromPool(ItemPool, [](const RandomizerGet i){ return Rando::StaticData::RetrieveItem(i).IsAdvancement();});
  for (RandomizerGet unplacedItem : items) {
    Rando::StaticData::RetrieveItem(unplacedItem).ApplyEffect();
  }
  // run a search to see what's accessible
  GetAccessibleLocations({});

  for (Entrance* entrance : entrancesToSplit) {
    // if an entrance is accessible at all times of day by both ages, it's a soft entrance with no restrictions
    if (entrance->ConditionsMet(true)) {
      softEntrances.push_back(entrance);
    } else {
      restrictiveEntrances.push_back(entrance);
    }
  }

  //Reconnect all disconnected entrances
  for (Entrance* entrance : entrancesToDisconnect) {
    entrance->Connect(originalConnectedRegions[entrance]);
  }

  return {restrictiveEntrances, softEntrances};
}

static bool AreEntrancesCompatible(Entrance* entrance, Entrance* target, std::vector<EntrancePair>& rollbacks) {

  //Entrances shouldn't connect to their own scene, fail in this situation
  if (entrance->GetParentRegion()->scene != "" && entrance->GetParentRegion()->scene == target->GetConnectedRegion()->scene) {
    auto message = "Entrance " + entrance->GetName() + " attempted to connect with own scene target " + target->to_string() + ". Connection failed.\n";
    SPDLOG_DEBUG(message);
    return false;
  }

  // One way entrances shouldn't lead to the same scene as other already chosen one way entrances
  auto type = entrance->GetType();
  const std::vector<EntranceType> oneWayTypes = {EntranceType::OwlDrop, EntranceType::Spawn, EntranceType::WarpSong};
  if (ElementInContainer(type, oneWayTypes)) {
    for (auto& rollback : rollbacks) {
      if (rollback.first->GetConnectedRegion()->scene == target->GetConnectedRegion()->scene) {
        auto message = "A one way entrance already leads to " + target->to_string() + ". Connection failed.\n";
        SPDLOG_DEBUG(message);
        return false;
      }
    }
  }

  return true;
}

//Change connections between an entrance and a target assumed entrance, in order to test the connections afterwards if necessary
static void ChangeConnections(Entrance* entrance, Entrance* targetEntrance) {
  auto message = "Attempting to connect " + entrance->GetName() + " to " + targetEntrance->to_string() + "\n";
  SPDLOG_DEBUG(message);
  entrance->Connect(targetEntrance->Disconnect());
  entrance->SetReplacement(targetEntrance->GetReplacement());
  if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
    targetEntrance->GetReplacement()->GetReverse()->Connect(entrance->GetReverse()->GetAssumed()->Disconnect());
    targetEntrance->GetReplacement()->GetReverse()->SetReplacement(entrance->GetReverse());
  }
}

// In the event that we need to retry shuffling an entire group we can restore the
// original connections to reset the entrance and target entrance.
static void RestoreConnections(Entrance* entrance, Entrance* targetEntrance) {
  targetEntrance->Connect(entrance->Disconnect());
  entrance->SetReplacement(nullptr);
  if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
    entrance->GetReverse()->GetAssumed()->Connect(targetEntrance->GetReplacement()->GetReverse()->Disconnect());
    targetEntrance->GetReplacement()->GetReverse()->SetReplacement(nullptr);
  }
}

static void DeleteTargetEntrance(Entrance* targetEntrance) {
  if (targetEntrance->GetConnectedRegionKey() != RR_NONE) {
    targetEntrance->Disconnect();
  }
  if (targetEntrance->GetParentRegionKey() != RR_NONE) {
    targetEntrance->GetParentRegion()->RemoveExit(targetEntrance);
    targetEntrance->SetParentRegion(RR_NONE);
  }
}

static void ConfirmReplacement(Entrance* entrance, Entrance* targetEntrance) {
  DeleteTargetEntrance(targetEntrance);
  if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
    auto replacedReverse = targetEntrance->GetReplacement()->GetReverse();
    DeleteTargetEntrance(replacedReverse->GetReverse()->GetAssumed());
  }
}

// Returns whether or not we can affirm the entrance can never be accessed as the given age
static bool EntranceUnreachableAs(Entrance* entrance, uint8_t age, std::vector<Entrance*>& alreadyChecked) {

  if (entrance == nullptr) {
    SPDLOG_DEBUG("Entrance is nullptr in EntranceUnreachableAs()");
    return true;
  }

  alreadyChecked.push_back(entrance);
  auto type = entrance->GetType();

  // The following cases determine when we say an entrance is not safe to affirm unreachable as the given age
  if (type == EntranceType::WarpSong || type == EntranceType::Overworld) {
    // Note that we consider all overworld entrances as potentially accessible as both ages, to be completely safe
    return false;
  } else if (type == EntranceType::OwlDrop) {
    return age == AGE_ADULT;
  } else if (type == EntranceType::Spawn && entrance->GetConnectedRegionKey() == RR_KF_LINKS_HOUSE) {
    return age == AGE_ADULT;
  } else if (type == EntranceType::Spawn && entrance->GetConnectedRegionKey() == RR_TEMPLE_OF_TIME) {
    return age == AGE_CHILD;
  }

  // Other entrances such as Interior, Dungeon or Grotto are fine unless they have a parent which is one of the above cases
  // Recursively check parent entrances to verify that they are also not reachable as the wrong age
  auto& parentEntrances = entrance->GetParentRegion()->entrances;
  for (Entrance* parentEntrance : parentEntrances) {

    //if parentEntrance is in alreadyChecked, then continue
    if (ElementInContainer(parentEntrance, alreadyChecked)) {
      continue;
    }

    bool unreachable = EntranceUnreachableAs(parentEntrance, age, alreadyChecked);
    if (!unreachable) {
      return false;
    }
  }

  return true;
}

static bool ValidateWorld(Entrance* entrancePlaced) {
  SPDLOG_DEBUG("Validating world\n");

  //check certain conditions when certain types of ER are enabled
  EntranceType type = EntranceType::None;
  if (entrancePlaced != nullptr) {
    type = entrancePlaced->GetType();
  }

  bool checkPoeCollectorAccess  = (Settings::ShuffleOverworldEntrances || Settings::ShuffleInteriorEntrances.Is(SHUFFLEINTERIORS_ALL)) && (entrancePlaced == nullptr || Settings::MixedEntrancePools ||
                                 type == EntranceType::Interior || type == EntranceType::SpecialInterior || type == EntranceType::Overworld || type == EntranceType::Spawn || type == EntranceType::WarpSong || type == EntranceType::OwlDrop);
  bool checkOtherEntranceAccess = (Settings::ShuffleOverworldEntrances || Settings::ShuffleInteriorEntrances.Is(SHUFFLEINTERIORS_ALL) || Settings::ShuffleOverworldSpawns) && (entrancePlaced == nullptr || Settings::MixedEntrancePools ||
                                 type == EntranceType::SpecialInterior || type == EntranceType::Overworld || type == EntranceType::Spawn || type == EntranceType::WarpSong || type == EntranceType::OwlDrop);

  // Search the world to verify that all necessary conditions are still being held
  // Conditions will be checked during the search and any that fail will be figured out
  // afterwards
  Logic::LogicReset();
  GetAccessibleLocations({}, SearchMode::ValidateWorld, "", checkPoeCollectorAccess, checkOtherEntranceAccess);

  if (!Settings::DecoupleEntrances) {
  // Unless entrances are decoupled, we don't want the player to end up through certain entrances as the wrong age
  // This means we need to hard check that none of the relevant entrances are ever reachable as that age
  // This is mostly relevant when mixing entrance pools or shuffling special interiors (such as windmill or kak potion shop)
  // Warp Songs and Overworld Spawns can also end up inside certain indoors so those need to be handled as well
    std::array<std::string, 3> childForbidden = {"OGC Great Fairy Fountain -> Castle Grounds", "GV Carpenter Tent -> GV Fortress Side", "Ganon's Castle Entryway -> Castle Grounds From Ganon's Castle"};
    std::array<std::string, 2> adultForbidden = {"HC Great Fairy Fountain -> Castle Grounds", "HC Storms Grotto -> Castle Grounds"};

    auto allShuffleableEntrances = GetShuffleableEntrances(EntranceType::All, false);
    for (auto& entrance: allShuffleableEntrances) {

      std::vector<Entrance*> alreadyChecked = {};

      if (entrance->IsShuffled()) {
        if (entrance->GetReplacement() != nullptr) {

          auto replacementName = entrance->GetReplacement()->GetName();
          alreadyChecked.push_back(entrance->GetReplacement()->GetReverse());

          if (ElementInContainer(replacementName, childForbidden) && !EntranceUnreachableAs(entrance, AGE_CHILD, alreadyChecked)) {
            auto message = replacementName + " is replaced by an entrance with a potential child access\n";
            SPDLOG_DEBUG(message);
            return false;
          } else if (ElementInContainer(replacementName, adultForbidden) && !EntranceUnreachableAs(entrance, AGE_ADULT, alreadyChecked)) {
            auto message = replacementName + " is replaced by an entrance with a potential adult access\n";
            SPDLOG_DEBUG(message);
            return false;
          }
        }
      } else {
        auto name = entrance->GetName();
        alreadyChecked.push_back(entrance->GetReverse());

        if (ElementInContainer(name, childForbidden) && !EntranceUnreachableAs(entrance, AGE_CHILD, alreadyChecked)) {
          auto message = name + " is potentially accessible as child\n";
          SPDLOG_DEBUG(message);
          return false;
        } else if (ElementInContainer(name, adultForbidden) && !EntranceUnreachableAs(entrance, AGE_ADULT, alreadyChecked)) {
          auto message = name + " is potentially accessible as adult\n";
          SPDLOG_DEBUG(message);
          return false;
        }
      }
    }
  }

  if (Settings::ShuffleInteriorEntrances.IsNot(SHUFFLEINTERIORS_OFF) && Settings::GossipStoneHints.IsNot(HINTS_NO_HINTS) &&
  (entrancePlaced == nullptr || type == EntranceType::Interior || type == EntranceType::SpecialInterior)) {
    //When cows are shuffled, ensure both Impa's House entrances are in the same hint area because the cow is reachable from both sides
    if (Settings::ShuffleCows) {
      auto impasHouseFrontHintRegion = GetHintRegionHintKey(RR_KAK_IMPAS_HOUSE);
      auto impasHouseBackHintRegion = GetHintRegionHintKey(RR_KAK_IMPAS_HOUSE_BACK);
      if (impasHouseFrontHintRegion != RHT_NONE && impasHouseBackHintRegion != RHT_NONE && impasHouseBackHintRegion != RHT_LINKS_POCKET && impasHouseFrontHintRegion != RHT_LINKS_POCKET && impasHouseBackHintRegion != impasHouseFrontHintRegion) {
        auto message = "Kak Impas House entrances are not in the same hint area\n";
        SPDLOG_DEBUG(message);
        return false;
      }
    }
  }

  // If all locations aren't reachable, that means that one of the conditions failed when searching
  if (!Rando::Context::GetInstance()->allLocationsReachable) {
    if (checkOtherEntranceAccess) {
      // At least one valid starting region with all basic refills should be reachable without using any items at the beginning of the seed
      if (!AreaTable(RR_KOKIRI_FOREST)->HasAccess() && !AreaTable(RR_KAKARIKO_VILLAGE)->HasAccess()) {
        SPDLOG_DEBUG("Invalid starting area\n");
        return false;
      }

      // Check that a region where time passes is always reachable as both ages without having collected any items
      if (!Areas::HasTimePassAccess(AGE_CHILD) || !Areas::HasTimePassAccess(AGE_ADULT)) {
        SPDLOG_DEBUG("Time passing is not guaranteed as both ages\n");
        return false;
      }

      // The player should be able to get back to ToT after going through time, without having collected any items
      // This is important to ensure that the player never loses access to the pedestal after going through time
      if (Settings::ResolvedStartingAge == AGE_CHILD && !AreaTable(RR_TEMPLE_OF_TIME)->Adult()) {
        SPDLOG_DEBUG("Path to Temple of Time as adult is not guaranteed\n");
        return false;
      } else if (Settings::ResolvedStartingAge == AGE_ADULT && !AreaTable(RR_TEMPLE_OF_TIME)->Child()) {
        SPDLOG_DEBUG("Path to Temple of Time as child is not guaranteed\n");
        return false;
      }
    }

    // The Big Poe shop should always be accessible as adult without the need to use any bottles
    // This is important to ensure that players can never lock their only bottles by filling them with Big Poes they can't sell
    if (checkPoeCollectorAccess) {
      if (!AreaTable(RR_MARKET_GUARD_HOUSE)->Adult()) {
        SPDLOG_DEBUG("Big Poe Shop access is not guarenteed as adult\n");
        return false;
      }
    }
    SPDLOG_DEBUG("All Locations NOT REACHABLE\n");
    return false;
  }
  return true;
}

static bool ReplaceEntrance(Entrance* entrance, Entrance* target, std::vector<EntrancePair>& rollbacks) {

  if (!AreEntrancesCompatible(entrance, target, rollbacks)) {
    return false;
  }
  ChangeConnections(entrance, target);
  if (ValidateWorld(entrance)) {
    #ifdef ENABLE_DEBUG
      std::string ticks = std::to_string(svcGetSystemTick());
      auto message = "Dumping World Graph at " + ticks + "\n";
      //SPDLOG_DEBUG(message);
      //Areas::DumpWorldGraph(ticks);
    #endif
    rollbacks.push_back(EntrancePair{entrance, target});
    curNumRandomizedEntrances++;
    DisplayEntranceProgress();
    return true;
  } else {
    #ifdef ENABLE_DEBUG
      std::string ticks = std::to_string(svcGetSystemTick());
      auto message = "Dumping World Graph at " + ticks + "\n";
      //SPDLOG_DEBUG(message);
      //Areas::DumpWorldGraph(ticks);
    #endif
    if (entrance->GetConnectedRegionKey() != RR_NONE) {
      RestoreConnections(entrance, target);
    }
  }
  DisplayEntranceProgress();
  return false;
}

// Connect one random entrance from entrance pools to one random target in the respective target pool.
// Entrance chosen will have one of the allowed types.
// Target chosen will lead to one of the allowed regions.
static bool PlaceOneWayPriorityEntrance(std::string priorityName, std::list<RandomizerRegion>& allowedRegions, std::list<EntranceType>& allowedTypes, std::vector<EntrancePair>& rollbacks, EntrancePools oneWayEntrancePools, EntrancePools oneWayTargetEntrancePools) {
  // Combine the entrances for allowed types in one list.
  // Shuffle this list.
  // Pick the first one not already set, not adult spawn, that has a valid target entrance.
  // Assemble then clear entrances from the pool and target pools as appropriate.
  std::vector<Entrance*> availPool = {};
  for (auto& pool : oneWayEntrancePools) {
    auto entranceType = pool.first;
    if (ElementInContainer(entranceType, allowedTypes)) {
      AddElementsToPool(availPool, pool.second);
    }
  }
  Shuffle(availPool);

  for (Entrance* entrance : availPool) {
    if (entrance->GetReplacement() != nullptr) {
      continue;
    }
    // Only allow Adult Spawn as sole Nocturne access if hints != mask.
    // Otherwise, child access is required here (adult access assumed or guaranteed later).
    if (entrance->GetParentRegionKey() == RR_ADULT_SPAWN) {
      if (priorityName != "Nocturne" || Settings::GossipStoneHints.Is(HINTS_MASK_OF_TRUTH)) {
        continue;
      }
    }
    // If not shuffling dungeons, Nocturne requires adult access
    if (!Settings::ShuffleDungeonEntrances && priorityName == "Nocturne") {
      if (entrance->GetType() != EntranceType::WarpSong && entrance->GetParentRegionKey() != RR_ADULT_SPAWN) {
        continue;
      }
    }
    for (Entrance* target : oneWayTargetEntrancePools[entrance->GetType()]) {
      RandomizerRegion targetRegionKey = target->GetConnectedRegionKey();
      if (targetRegionKey != RR_NONE && ElementInContainer(targetRegionKey, allowedRegions)) {
        if (ReplaceEntrance(entrance, target, rollbacks)) {
          // Return once the entrance has been replaced
          return true;
        }
      }
    }
  }
  #ifdef ENABLE_DEBUG
    auto message = "ERROR: Unable to place priority one-way entrance for " + priorityName + "\n";
    SPDLOG_DEBUG(message);
    PlacementLog_Write();
  #endif
  return false;
}

// Once the first entrance to Impas House has been placed, try to place the next one immediately to reduce chances of failure.
static bool PlaceOtherImpasHouseEntrance(std::vector<Entrance*> entrances, std::vector<Entrance*> targetEntrances, std::vector<EntrancePair>& rollbacks) {
  // Get the other impas house entrance
  auto otherImpaTargets = FilterFromPool(targetEntrances, [](const Entrance* target) {
      return (target->GetConnectedRegionKey() == RR_KAK_IMPAS_HOUSE ||
              target->GetConnectedRegionKey() == RR_KAK_IMPAS_HOUSE_BACK);
  });
  if (otherImpaTargets.empty()) {
    return true;
  }

  Entrance* otherImpaTarget = otherImpaTargets[0];
  auto m = "Now Placing Other Impa Target: " + otherImpaTarget->GetName() + "\n";
  SPDLOG_DEBUG(m);
  RandomizerRegion otherImpaRegion = otherImpaTarget->GetConnectedRegionKey() != RR_KAK_IMPAS_HOUSE_BACK
                                         ? RR_KAK_IMPAS_HOUSE_BACK
                                         : RR_KAK_IMPAS_HOUSE;
  for (Entrance* entrance : entrances) {
    // If the entrance is already connected or it doesn't have the same hint region as the already placed impas house entrance, then don't try to use it
    if (entrance->GetConnectedRegionKey() != RR_NONE ||
        (GetHintRegionHintKey(otherImpaRegion) != GetHintRegionHintKey(entrance->GetParentRegionKey()))) {
        continue;
    }
    // If the placement succeeds, we return true
    if (ReplaceEntrance(entrance, otherImpaTarget, rollbacks)) {
      return true;
    }
  }
  SPDLOG_DEBUG("No available entrances for placing other impa region.\n");
  return false;
}

// Shuffle entrances by placing them instead of entrances in the provided target entrances list
static bool ShuffleEntrances(std::vector<Entrance*>& entrances, std::vector<Entrance*>& targetEntrances, std::vector<EntrancePair>& rollbacks) {

  Shuffle(entrances);

  //place all entrances in the pool, validating after every placement
  for (Entrance* entrance : entrances) {
    if (entrance->GetConnectedRegionKey() != RR_NONE) {
      continue;
    }

    Shuffle(targetEntrances);
    for (Entrance* target : targetEntrances) {
      if (target->GetConnectedRegionKey() == RR_NONE) {
        continue;
      }

      // Store whether or not we're about to attempt placing an entrance to Impas House
      bool attemptedImpasHousePlacement = (target->GetConnectedRegionKey() == RR_KAK_IMPAS_HOUSE || target->GetConnectedRegionKey() == RR_KAK_IMPAS_HOUSE_BACK);

      if (ReplaceEntrance(entrance, target, rollbacks)) {
        // If shuffle cows is enabled and the last entrance was one to Impas House,
        // then immediately attempt to place the other entrance to Impas House
        if (Settings::ShuffleCows && attemptedImpasHousePlacement) {
          if (!PlaceOtherImpasHouseEntrance(entrances, targetEntrances, rollbacks)) {
            return false;
          }
        }
        break;
      }
    }

    if (entrance->GetConnectedRegionKey() == RR_NONE) {
      return false;
    }
  }

  //all entrances were validly connected
  return true;
}

static bool ShuffleOneWayPriorityEntrances(std::map<std::string, PriorityEntrance>& oneWayPriorities, EntrancePools oneWayEntrancePools, EntrancePools oneWayTargetEntrancePools, int retryCount = 2) {
  while (retryCount > 0) {
    retryCount--;
    std::vector<EntrancePair> rollbacks = {};

    bool success = true;
    for (auto& priority : oneWayPriorities) {
      std::string key = priority.first;
      auto& regions = priority.second.targetRegions;
      auto& types = priority.second.allowedTypes;
      success = PlaceOneWayPriorityEntrance(key, regions, types, rollbacks, oneWayEntrancePools, oneWayTargetEntrancePools);
      if (!success) {
        for (auto& pair : rollbacks) {
          RestoreConnections(pair.first, pair.second);
        }
        break;
      }
    }
    if (!success) {
      continue;
    }
    // If there are no issues, log the connections and continue
    for (auto& pair : rollbacks) {
      ConfirmReplacement(pair.first, pair.second);
    }
    break;
  }

  if (retryCount <= 0) {
    SPDLOG_DEBUG("Entrance placement attempt count for one way priorities exceeded. Restarting randomization completely\n");
    entranceShuffleFailure = true;
    return false;
  }
  return true;
}

static void ShuffleEntrancePool(std::vector<Entrance*>& entrancePool, std::vector<Entrance*>& targetEntrances, int retryCount = 20) {
  noRandomEntrances = false;

  auto splitEntrances = SplitEntrancesByRequirements(entrancePool, targetEntrances);

  auto& restrictiveEntrances = splitEntrances[0];
  auto& softEntrances = splitEntrances[1];

  int retries = retryCount;
  while (retries > 0) {
    if (retries != retryCount) {
      #ifdef ENABLE_DEBUG
        std::string ticks = std::to_string(svcGetSystemTick());
        auto message = "Failed to connect entrances. Retrying " + std::to_string(retries) + " more times.\nDumping World Graph at " + ticks + "\n";
        SPDLOG_DEBUG(message);
        //Areas::DumpWorldGraph(ticks);
      #endif
    }
    retries--;

    std::vector<EntrancePair> rollbacks = {};

    //Shuffle Restrictive Entrances first while more regions are available in
    //order to heavily reduce the chances of the placement failing
    bool success = ShuffleEntrances(restrictiveEntrances, targetEntrances, rollbacks);
    if (success) {
      success = ShuffleEntrances(softEntrances, targetEntrances, rollbacks);
      if(!success) {
        for (auto& pair : rollbacks) {
          RestoreConnections(pair.first, pair.second);
          curNumRandomizedEntrances--;
        }
        continue;
      }
    } else {
      for (auto& pair : rollbacks) {
        RestoreConnections(pair.first, pair.second);
        curNumRandomizedEntrances--;
      }
      continue;
    }

    //If there are no issues, log the connections and continue
    for (auto& pair : rollbacks) {
      ConfirmReplacement(pair.first, pair.second);
    }
    break;
  }

  if (retries <= 0) {
    SPDLOG_DEBUG("Entrance placement attempt count exceeded. Restarting randomization completely");
    entranceShuffleFailure = true;
  }
}

static void SetShuffledEntrances(EntrancePools entrancePools) {
  for (auto& pool : entrancePools) {
    for (Entrance* entrance : pool.second) {
      entrance->SetAsShuffled();
      if (entrance->GetReverse() != nullptr) {
        entrance->GetReverse()->SetAsShuffled();
      }
    }
  }
}

//Process for setting up the shuffling of all entrances to be shuffled
int ShuffleAllEntrances() {

  totalRandomizableEntrances = 0;
  curNumRandomizedEntrances = 0;

  std::vector<EntranceInfoPair> entranceShuffleTable = {
      // Parent Region                     Connected Region                      index   blue warp
      { { EntranceType::Dungeon, RR_KF_OUTSIDE_DEKU_TREE, RR_DEKU_TREE_ENTRYWAY, 0x0000 },
        { EntranceType::Dungeon, RR_DEKU_TREE_ENTRYWAY, RR_KF_OUTSIDE_DEKU_TREE, 0x0209, 0x0457 } },
      { { EntranceType::Dungeon, RR_DEATH_MOUNTAIN_TRAIL, RR_DODONGOS_CAVERN_ENTRYWAY, 0x0004 },
        { EntranceType::Dungeon, RR_DODONGOS_CAVERN_ENTRYWAY, RR_DEATH_MOUNTAIN_TRAIL, 0x0242, 0x047A } },
      { { EntranceType::Dungeon, RR_ZORAS_FOUNTAIN, RR_JABU_JABUS_BELLY_ENTRYWAY, 0x0028 },
        { EntranceType::Dungeon, RR_JABU_JABUS_BELLY_ENTRYWAY, RR_ZORAS_FOUNTAIN, 0x0221, 0x010E } },
      { { EntranceType::Dungeon, RR_SACRED_FOREST_MEADOW, RR_FOREST_TEMPLE_ENTRYWAY, 0x0169 },
        { EntranceType::Dungeon, RR_FOREST_TEMPLE_ENTRYWAY, RR_SACRED_FOREST_MEADOW, 0x0215, 0x0608 } },
      { { EntranceType::Dungeon, RR_DMC_CENTRAL_LOCAL, RR_FIRE_TEMPLE_ENTRYWAY, 0x0165 },
        { EntranceType::Dungeon, RR_FIRE_TEMPLE_ENTRYWAY, RR_DMC_CENTRAL_LOCAL, 0x024A, 0x0564 } },
      { { EntranceType::Dungeon, RR_LAKE_HYLIA, RR_WATER_TEMPLE_ENTRYWAY, 0x0010 },
        { EntranceType::Dungeon, RR_WATER_TEMPLE_ENTRYWAY, RR_LAKE_HYLIA, 0x021D, 0x060C } },
      { { EntranceType::Dungeon, RR_DESERT_COLOSSUS, RR_SPIRIT_TEMPLE_ENTRYWAY, 0x0082 },
        { EntranceType::Dungeon, RR_SPIRIT_TEMPLE_ENTRYWAY, RR_DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY, 0x01E1, 0x0610 } },
      { { EntranceType::Dungeon, RR_GRAVEYARD_WARP_PAD_REGION, RR_SHADOW_TEMPLE_ENTRYWAY, 0x0037 },
        { EntranceType::Dungeon, RR_SHADOW_TEMPLE_ENTRYWAY, RR_GRAVEYARD_WARP_PAD_REGION, 0x0205, 0x0580 } },
      { { EntranceType::Dungeon, RR_KAKARIKO_VILLAGE, RR_BOTTOM_OF_THE_WELL_ENTRYWAY, 0x0098 },
        { EntranceType::Dungeon, RR_BOTTOM_OF_THE_WELL_ENTRYWAY, RR_KAKARIKO_VILLAGE, 0x02A6 } },
      { { EntranceType::Dungeon, RR_ZORAS_FOUNTAIN, RR_ICE_CAVERN_ENTRYWAY, 0x0088 },
        { EntranceType::Dungeon, RR_ICE_CAVERN_ENTRYWAY, RR_ZORAS_FOUNTAIN, 0x03D4 } },
      { { EntranceType::Dungeon, RR_GERUDO_FORTRESS, RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY, 0x0008 },
        { EntranceType::Dungeon, RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY, RR_GERUDO_FORTRESS, 0x03A8 } },
      { { EntranceType::GanonDungeon, RR_GANONS_CASTLE_LEDGE, RR_GANONS_CASTLE_ENTRYWAY, 0x0467 },
        { EntranceType::GanonDungeon, RR_GANONS_CASTLE_ENTRYWAY, RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE, 0x023D } },

      { { EntranceType::Interior, RR_KOKIRI_FOREST, RR_KF_MIDOS_HOUSE, 0x0433 },
        { EntranceType::Interior, RR_KF_MIDOS_HOUSE, RR_KOKIRI_FOREST, 0x0443 } },
      { { EntranceType::Interior, RR_KOKIRI_FOREST, RR_KF_SARIAS_HOUSE, 0x0437 },
        { EntranceType::Interior, RR_KF_SARIAS_HOUSE, RR_KOKIRI_FOREST, 0x0447 } },
      { { EntranceType::Interior, RR_KOKIRI_FOREST, RR_KF_HOUSE_OF_TWINS, 0x009C },
        { EntranceType::Interior, RR_KF_HOUSE_OF_TWINS, RR_KOKIRI_FOREST, 0x033C } },
      { { EntranceType::Interior, RR_KOKIRI_FOREST, RR_KF_KNOW_IT_ALL_HOUSE, 0x00C9 },
        { EntranceType::Interior, RR_KF_KNOW_IT_ALL_HOUSE, RR_KOKIRI_FOREST, 0x026A } },
      { { EntranceType::Interior, RR_KOKIRI_FOREST, RR_KF_KOKIRI_SHOP, 0x00C1 },
        { EntranceType::Interior, RR_KF_KOKIRI_SHOP, RR_KOKIRI_FOREST, 0x0266 } },
      { { EntranceType::Interior, RR_LAKE_HYLIA, RR_LH_LAB, 0x0043 },
        { EntranceType::Interior, RR_LH_LAB, RR_LAKE_HYLIA, 0x03CC } },
      { { EntranceType::Interior, RR_LH_FISHING_ISLAND, RR_LH_FISHING_HOLE, 0x045F },
        { EntranceType::Interior, RR_LH_FISHING_HOLE, RR_LH_FISHING_ISLAND, 0x0309 } },
      { { EntranceType::Interior, RR_GV_FORTRESS_SIDE, RR_GV_CARPENTER_TENT, 0x03A0 },
        { EntranceType::Interior, RR_GV_CARPENTER_TENT, RR_GV_FORTRESS_SIDE, 0x03D0 } },
      { { EntranceType::Interior, RR_MARKET_ENTRANCE, RR_MARKET_GUARD_HOUSE, 0x007E },
        { EntranceType::Interior, RR_MARKET_GUARD_HOUSE, RR_MARKET_ENTRANCE, 0x026E } },
      { { EntranceType::Interior, RR_THE_MARKET, RR_MARKET_MASK_SHOP, 0x0530 },
        { EntranceType::Interior, RR_MARKET_MASK_SHOP, RR_THE_MARKET, 0x01D1 } },
      { { EntranceType::Interior, RR_THE_MARKET, RR_MARKET_BOMBCHU_BOWLING, 0x0507 },
        { EntranceType::Interior, RR_MARKET_BOMBCHU_BOWLING, RR_THE_MARKET, 0x03BC } },
      { { EntranceType::Interior, RR_THE_MARKET, RR_MARKET_POTION_SHOP, 0x0388 },
        { EntranceType::Interior, RR_MARKET_POTION_SHOP, RR_THE_MARKET, 0x02A2 } },
      { { EntranceType::Interior, RR_THE_MARKET, RR_MARKET_TREASURE_CHEST_GAME, 0x0063 },
        { EntranceType::Interior, RR_MARKET_TREASURE_CHEST_GAME, RR_THE_MARKET, 0x01D5 } },
      { { EntranceType::Interior, RR_MARKET_BACK_ALLEY, RR_MARKET_BOMBCHU_SHOP, 0x0528 },
        { EntranceType::Interior, RR_MARKET_BOMBCHU_SHOP, RR_MARKET_BACK_ALLEY, 0x03C0 } },
      { { EntranceType::Interior, RR_MARKET_BACK_ALLEY, RR_MARKET_MAN_IN_GREEN_HOUSE, 0x043B },
        { EntranceType::Interior, RR_MARKET_MAN_IN_GREEN_HOUSE, RR_MARKET_BACK_ALLEY, 0x0067 } },
      { { EntranceType::Interior, RR_KAKARIKO_VILLAGE, RR_KAK_CARPENTER_BOSS_HOUSE, 0x02FD },
        { EntranceType::Interior, RR_KAK_CARPENTER_BOSS_HOUSE, RR_KAKARIKO_VILLAGE, 0x0349 } },
      { { EntranceType::Interior, RR_KAKARIKO_VILLAGE, RR_KAK_HOUSE_OF_SKULLTULA, 0x0550 },
        { EntranceType::Interior, RR_KAK_HOUSE_OF_SKULLTULA, RR_KAKARIKO_VILLAGE, 0x04EE } },
      { { EntranceType::Interior, RR_KAKARIKO_VILLAGE, RR_KAK_IMPAS_HOUSE, 0x039C },
        { EntranceType::Interior, RR_KAK_IMPAS_HOUSE, RR_KAKARIKO_VILLAGE, 0x0345 } },
      { { EntranceType::Interior, RR_KAK_IMPAS_LEDGE, RR_KAK_IMPAS_HOUSE_BACK, 0x05C8 },
        { EntranceType::Interior, RR_KAK_IMPAS_HOUSE_BACK, RR_KAK_IMPAS_LEDGE, 0x05DC } },
      { { EntranceType::Interior, RR_KAK_BACKYARD, RR_KAK_ODD_POTION_BUILDING, 0x0072 },
        { EntranceType::Interior, RR_KAK_ODD_POTION_BUILDING, RR_KAK_BACKYARD, 0x034D } },
      { { EntranceType::Interior, RR_THE_GRAVEYARD, RR_GRAVEYARD_DAMPES_HOUSE, 0x030D },
        { EntranceType::Interior, RR_GRAVEYARD_DAMPES_HOUSE, RR_THE_GRAVEYARD, 0x0355 } },
      { { EntranceType::Interior, RR_GORON_CITY, RR_GC_SHOP, 0x037C },
        { EntranceType::Interior, RR_GC_SHOP, RR_GORON_CITY, 0x03FC } },
      { { EntranceType::Interior, RR_ZORAS_DOMAIN, RR_ZD_SHOP, 0x0380 },
        { EntranceType::Interior, RR_ZD_SHOP, RR_ZORAS_DOMAIN, 0x03C4 } },
      { { EntranceType::Interior, RR_LON_LON_RANCH, RR_LLR_TALONS_HOUSE, 0x004F },
        { EntranceType::Interior, RR_LLR_TALONS_HOUSE, RR_LON_LON_RANCH, 0x0378 } },
      { { EntranceType::Interior, RR_LON_LON_RANCH, RR_LLR_STABLES, 0x02F9 },
        { EntranceType::Interior, RR_LLR_STABLES, RR_LON_LON_RANCH, 0x042F } },
      { { EntranceType::Interior, RR_LON_LON_RANCH, RR_LLR_TOWER, 0x05D0 },
        { EntranceType::Interior, RR_LLR_TOWER, RR_LON_LON_RANCH, 0x05D4 } },
      { { EntranceType::Interior, RR_THE_MARKET, RR_MARKET_BAZAAR, 0x052C },
        { EntranceType::Interior, RR_MARKET_BAZAAR, RR_THE_MARKET, 0x03B8 } },
      { { EntranceType::Interior, RR_THE_MARKET, RR_MARKET_SHOOTING_GALLERY, 0x016D },
        { EntranceType::Interior, RR_MARKET_SHOOTING_GALLERY, RR_THE_MARKET, 0x01CD } },
      { { EntranceType::Interior, RR_KAKARIKO_VILLAGE, RR_KAK_BAZAAR, 0x00B7 },
        { EntranceType::Interior, RR_KAK_BAZAAR, RR_KAKARIKO_VILLAGE, 0x0201 } },
      { { EntranceType::Interior, RR_KAKARIKO_VILLAGE, RR_KAK_SHOOTING_GALLERY, 0x003B },
        { EntranceType::Interior, RR_KAK_SHOOTING_GALLERY, RR_KAKARIKO_VILLAGE, 0x0463 } },
      { { EntranceType::Interior, RR_DESERT_COLOSSUS, RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN, 0x0588 },
        { EntranceType::Interior, RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN, RR_DESERT_COLOSSUS, 0x057C } },
      { { EntranceType::Interior, RR_HYRULE_CASTLE_GROUNDS, RR_HC_GREAT_FAIRY_FOUNTAIN, 0x0578 },
        { EntranceType::Interior, RR_HC_GREAT_FAIRY_FOUNTAIN, RR_CASTLE_GROUNDS, 0x0340 } },
      { { EntranceType::Interior, RR_GANONS_CASTLE_GROUNDS, RR_OGC_GREAT_FAIRY_FOUNTAIN, 0x04C2 },
        { EntranceType::Interior, RR_OGC_GREAT_FAIRY_FOUNTAIN, RR_CASTLE_GROUNDS,
          0x03E8 } }, // 0x3E8 is an unused entrance index repruposed to differentiate between the HC and OGC fairy
                      // fountain exits (normally they both use 0x340)
      { { EntranceType::Interior, RR_DMC_LOWER_NEARBY, RR_DMC_GREAT_FAIRY_FOUNTAIN, 0x04BE },
        { EntranceType::Interior, RR_DMC_GREAT_FAIRY_FOUNTAIN, RR_DMC_LOWER_LOCAL, 0x0482 } },
      { { EntranceType::Interior, RR_DEATH_MOUNTAIN_SUMMIT, RR_DMT_GREAT_FAIRY_FOUNTAIN, 0x0315 },
        { EntranceType::Interior, RR_DMT_GREAT_FAIRY_FOUNTAIN, RR_DEATH_MOUNTAIN_SUMMIT, 0x045B } },
      { { EntranceType::Interior, RR_ZORAS_FOUNTAIN, RR_ZF_GREAT_FAIRY_FOUNTAIN, 0x0371 },
        { EntranceType::Interior, RR_ZF_GREAT_FAIRY_FOUNTAIN, RR_ZORAS_FOUNTAIN, 0x0394 } },

      { { EntranceType::SpecialInterior, RR_KOKIRI_FOREST, RR_KF_LINKS_HOUSE, 0x0272 },
        { EntranceType::SpecialInterior, RR_KF_LINKS_HOUSE, RR_KOKIRI_FOREST, 0x0211 } },
      { { EntranceType::SpecialInterior, RR_TOT_ENTRANCE, RR_TEMPLE_OF_TIME, 0x0053 },
        { EntranceType::SpecialInterior, RR_TEMPLE_OF_TIME, RR_TOT_ENTRANCE, 0x0472 } },
      { { EntranceType::SpecialInterior, RR_KAKARIKO_VILLAGE, RR_KAK_WINDMILL, 0x0453 },
        { EntranceType::SpecialInterior, RR_KAK_WINDMILL, RR_KAKARIKO_VILLAGE, 0x0351 } },
      { { EntranceType::SpecialInterior, RR_KAKARIKO_VILLAGE, RR_KAK_POTION_SHOP_FRONT, 0x0384 },
        { EntranceType::SpecialInterior, RR_KAK_POTION_SHOP_FRONT, RR_KAKARIKO_VILLAGE, 0x044B } },
      { { EntranceType::SpecialInterior, RR_KAK_BACKYARD, RR_KAK_POTION_SHOP_BACK, 0x03EC },
        { EntranceType::SpecialInterior, RR_KAK_POTION_SHOP_BACK, RR_KAK_BACKYARD, 0x04FF } },

      // Grotto Loads use an entrance index of 0x0700 + their grotto id. The id is used as index for the
      // grottoLoadTable in soh/soh/Enhancements/randomizer/randomizer_grotto.c
      // Grotto Returns use an entrance index of 0x0800 + their grotto id. The id is used as index for the
      // grottoReturnTable in soh/soh/Enhancements/randomizer/randomizer_grotto.c
      { { EntranceType::GrottoGrave, RR_DESERT_COLOSSUS, RR_COLOSSUS_GROTTO, 0x0700 },
        { EntranceType::GrottoGrave, RR_COLOSSUS_GROTTO, RR_DESERT_COLOSSUS, 0x0800 } },
      { { EntranceType::GrottoGrave, RR_LAKE_HYLIA, RR_LH_GROTTO, 0x0701 },
        { EntranceType::GrottoGrave, RR_LH_GROTTO, RR_LAKE_HYLIA, 0x0801 } },
      { { EntranceType::GrottoGrave, RR_ZORAS_RIVER, RR_ZR_STORMS_GROTTO, 0x0702 },
        { EntranceType::GrottoGrave, RR_ZR_STORMS_GROTTO, RR_ZORAS_RIVER, 0x0802 } },
      { { EntranceType::GrottoGrave, RR_ZORAS_RIVER, RR_ZR_FAIRY_GROTTO, 0x0703 },
        { EntranceType::GrottoGrave, RR_ZR_FAIRY_GROTTO, RR_ZORAS_RIVER, 0x0803 } },
      { { EntranceType::GrottoGrave, RR_ZORAS_RIVER, RR_ZR_OPEN_GROTTO, 0x0704 },
        { EntranceType::GrottoGrave, RR_ZR_OPEN_GROTTO, RR_ZORAS_RIVER, 0x0804 } },
      { { EntranceType::GrottoGrave, RR_DMC_LOWER_NEARBY, RR_DMC_HAMMER_GROTTO, 0x0705 },
        { EntranceType::GrottoGrave, RR_DMC_HAMMER_GROTTO, RR_DMC_LOWER_LOCAL, 0x0805 } },
      { { EntranceType::GrottoGrave, RR_DMC_UPPER_NEARBY, RR_DMC_UPPER_GROTTO, 0x0706 },
        { EntranceType::GrottoGrave, RR_DMC_UPPER_GROTTO, RR_DMC_UPPER_LOCAL, 0x0806 } },
      { { EntranceType::GrottoGrave, RR_GC_GROTTO_PLATFORM, RR_GC_GROTTO, 0x0707 },
        { EntranceType::GrottoGrave, RR_GC_GROTTO, RR_GC_GROTTO_PLATFORM, 0x0807 } },
      { { EntranceType::GrottoGrave, RR_DEATH_MOUNTAIN_TRAIL, RR_DMT_STORMS_GROTTO, 0x0708 },
        { EntranceType::GrottoGrave, RR_DMT_STORMS_GROTTO, RR_DEATH_MOUNTAIN_TRAIL, 0x0808 } },
      { { EntranceType::GrottoGrave, RR_DEATH_MOUNTAIN_SUMMIT, RR_DMT_COW_GROTTO, 0x0709 },
        { EntranceType::GrottoGrave, RR_DMT_COW_GROTTO, RR_DEATH_MOUNTAIN_SUMMIT, 0x0809 } },
      { { EntranceType::GrottoGrave, RR_KAK_BACKYARD, RR_KAK_OPEN_GROTTO, 0x070A },
        { EntranceType::GrottoGrave, RR_KAK_OPEN_GROTTO, RR_KAK_BACKYARD, 0x080A } },
      { { EntranceType::GrottoGrave, RR_KAKARIKO_VILLAGE, RR_KAK_REDEAD_GROTTO, 0x070B },
        { EntranceType::GrottoGrave, RR_KAK_REDEAD_GROTTO, RR_KAKARIKO_VILLAGE, 0x080B } },
      { { EntranceType::GrottoGrave, RR_HYRULE_CASTLE_GROUNDS, RR_HC_STORMS_GROTTO, 0x070C },
        { EntranceType::GrottoGrave, RR_HC_STORMS_GROTTO, RR_CASTLE_GROUNDS, 0x080C } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_TEKTITE_GROTTO, 0x070D },
        { EntranceType::GrottoGrave, RR_HF_TEKTITE_GROTTO, RR_HYRULE_FIELD, 0x080D } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_NEAR_KAK_GROTTO, 0x070E },
        { EntranceType::GrottoGrave, RR_HF_NEAR_KAK_GROTTO, RR_HYRULE_FIELD, 0x080E } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_FAIRY_GROTTO, 0x070F },
        { EntranceType::GrottoGrave, RR_HF_FAIRY_GROTTO, RR_HYRULE_FIELD, 0x080F } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_NEAR_MARKET_GROTTO, 0x0710 },
        { EntranceType::GrottoGrave, RR_HF_NEAR_MARKET_GROTTO, RR_HYRULE_FIELD, 0x0810 } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_COW_GROTTO, 0x0711 },
        { EntranceType::GrottoGrave, RR_HF_COW_GROTTO, RR_HYRULE_FIELD, 0x0811 } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_INSIDE_FENCE_GROTTO, 0x0712 },
        { EntranceType::GrottoGrave, RR_HF_INSIDE_FENCE_GROTTO, RR_HYRULE_FIELD, 0x0812 } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_OPEN_GROTTO, 0x0713 },
        { EntranceType::GrottoGrave, RR_HF_OPEN_GROTTO, RR_HYRULE_FIELD, 0x0813 } },
      { { EntranceType::GrottoGrave, RR_HYRULE_FIELD, RR_HF_SOUTHEAST_GROTTO, 0x0714 },
        { EntranceType::GrottoGrave, RR_HF_SOUTHEAST_GROTTO, RR_HYRULE_FIELD, 0x0814 } },
      { { EntranceType::GrottoGrave, RR_LON_LON_RANCH, RR_LLR_GROTTO, 0x0715 },
        { EntranceType::GrottoGrave, RR_LLR_GROTTO, RR_LON_LON_RANCH, 0x0815 } },
      { { EntranceType::GrottoGrave, RR_SFM_ENTRYWAY, RR_SFM_WOLFOS_GROTTO, 0x0716 },
        { EntranceType::GrottoGrave, RR_SFM_WOLFOS_GROTTO, RR_SFM_ENTRYWAY, 0x0816 } },
      { { EntranceType::GrottoGrave, RR_SACRED_FOREST_MEADOW, RR_SFM_STORMS_GROTTO, 0x0717 },
        { EntranceType::GrottoGrave, RR_SFM_STORMS_GROTTO, RR_SACRED_FOREST_MEADOW, 0x0817 } },
      { { EntranceType::GrottoGrave, RR_SACRED_FOREST_MEADOW, RR_SFM_FAIRY_GROTTO, 0x0718 },
        { EntranceType::GrottoGrave, RR_SFM_FAIRY_GROTTO, RR_SACRED_FOREST_MEADOW, 0x0818 } },
      { { EntranceType::GrottoGrave, RR_LW_BEYOND_MIDO, RR_LW_SCRUBS_GROTTO, 0x0719 },
        { EntranceType::GrottoGrave, RR_LW_SCRUBS_GROTTO, RR_LW_BEYOND_MIDO, 0x0819 } },
      { { EntranceType::GrottoGrave, RR_THE_LOST_WOODS, RR_LW_NEAR_SHORTCUTS_GROTTO, 0x071A },
        { EntranceType::GrottoGrave, RR_LW_NEAR_SHORTCUTS_GROTTO, RR_THE_LOST_WOODS, 0x081A } },
      { { EntranceType::GrottoGrave, RR_KOKIRI_FOREST, RR_KF_STORMS_GROTTO, 0x071B },
        { EntranceType::GrottoGrave, RR_KF_STORMS_GROTTO, RR_KOKIRI_FOREST, 0x081B } },
      { { EntranceType::GrottoGrave, RR_ZORAS_DOMAIN, RR_ZD_STORMS_GROTTO, 0x071C },
        { EntranceType::GrottoGrave, RR_ZD_STORMS_GROTTO, RR_ZORAS_DOMAIN, 0x081C } },
      { { EntranceType::GrottoGrave, RR_GERUDO_FORTRESS, RR_GF_STORMS_GROTTO, 0x071D },
        { EntranceType::GrottoGrave, RR_GF_STORMS_GROTTO, RR_GERUDO_FORTRESS, 0x081D } },
      { { EntranceType::GrottoGrave, RR_GV_FORTRESS_SIDE, RR_GV_STORMS_GROTTO, 0x071E },
        { EntranceType::GrottoGrave, RR_GV_STORMS_GROTTO, RR_GV_FORTRESS_SIDE, 0x081E } },
      { { EntranceType::GrottoGrave, RR_GV_GROTTO_LEDGE, RR_GV_OCTOROK_GROTTO, 0x071F },
        { EntranceType::GrottoGrave, RR_GV_OCTOROK_GROTTO, RR_GV_GROTTO_LEDGE, 0x081F } },
      { { EntranceType::GrottoGrave, RR_LW_BEYOND_MIDO, RR_DEKU_THEATER, 0x0720 },
        { EntranceType::GrottoGrave, RR_DEKU_THEATER, RR_LW_BEYOND_MIDO, 0x0820 } },

      // Graves have their own specified entrance indices
      { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD, RR_GRAVEYARD_SHIELD_GRAVE, 0x004B },
        { EntranceType::GrottoGrave, RR_GRAVEYARD_SHIELD_GRAVE, RR_THE_GRAVEYARD, 0x035D } },
      { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD, RR_GRAVEYARD_HEART_PIECE_GRAVE, 0x031C },
        { EntranceType::GrottoGrave, RR_GRAVEYARD_HEART_PIECE_GRAVE, RR_THE_GRAVEYARD, 0x0361 } },
      { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD, RR_GRAVEYARD_COMPOSERS_GRAVE, 0x002D },
        { EntranceType::GrottoGrave, RR_GRAVEYARD_COMPOSERS_GRAVE, RR_THE_GRAVEYARD, 0x050B } },
      { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD, RR_GRAVEYARD_DAMPES_GRAVE, 0x044F },
        { EntranceType::GrottoGrave, RR_GRAVEYARD_DAMPES_GRAVE, RR_THE_GRAVEYARD, 0x0359 } },

      { { EntranceType::Overworld, RR_KOKIRI_FOREST, RR_LW_BRIDGE_FROM_FOREST, 0x05E0 },
        { EntranceType::Overworld, RR_LW_BRIDGE, RR_KOKIRI_FOREST, 0x020D } },
      { { EntranceType::Overworld, RR_KOKIRI_FOREST, RR_THE_LOST_WOODS, 0x011E },
        { EntranceType::Overworld, RR_LW_FOREST_EXIT, RR_KOKIRI_FOREST, 0x0286 } },
      { { EntranceType::Overworld, RR_THE_LOST_WOODS, RR_GC_WOODS_WARP, 0x04E2 },
        { EntranceType::Overworld, RR_GC_WOODS_WARP, RR_THE_LOST_WOODS, 0x04D6 } },
      { { EntranceType::Overworld, RR_THE_LOST_WOODS, RR_ZORAS_RIVER, 0x01DD },
        { EntranceType::Overworld, RR_ZORAS_RIVER, RR_THE_LOST_WOODS, 0x04DA } },
      { { EntranceType::Overworld, RR_LW_BEYOND_MIDO, RR_SFM_ENTRYWAY, 0x00FC },
        { EntranceType::Overworld, RR_SFM_ENTRYWAY, RR_LW_BEYOND_MIDO, 0x01A9 } },
      { { EntranceType::Overworld, RR_LW_BRIDGE, RR_HYRULE_FIELD, 0x0185 },
        { EntranceType::Overworld, RR_HYRULE_FIELD, RR_LW_BRIDGE, 0x04DE } },
      { { EntranceType::Overworld, RR_HYRULE_FIELD, RR_LAKE_HYLIA, 0x0102 },
        { EntranceType::Overworld, RR_LAKE_HYLIA, RR_HYRULE_FIELD, 0x0189 } },
      { { EntranceType::Overworld, RR_HYRULE_FIELD, RR_GERUDO_VALLEY, 0x0117 },
        { EntranceType::Overworld, RR_GERUDO_VALLEY, RR_HYRULE_FIELD, 0x018D } },
      { { EntranceType::Overworld, RR_HYRULE_FIELD, RR_MARKET_ENTRANCE, 0x0276 },
        { EntranceType::Overworld, RR_MARKET_ENTRANCE, RR_HYRULE_FIELD, 0x01FD } },
      { { EntranceType::Overworld, RR_HYRULE_FIELD, RR_KAKARIKO_VILLAGE, 0x00DB },
        { EntranceType::Overworld, RR_KAKARIKO_VILLAGE, RR_HYRULE_FIELD, 0x017D } },
      { { EntranceType::Overworld, RR_HYRULE_FIELD, RR_ZR_FRONT, 0x00EA },
        { EntranceType::Overworld, RR_ZR_FRONT, RR_HYRULE_FIELD, 0x0181 } },
      { { EntranceType::Overworld, RR_HYRULE_FIELD, RR_LON_LON_RANCH, 0x0157 },
        { EntranceType::Overworld, RR_LON_LON_RANCH, RR_HYRULE_FIELD, 0x01F9 } },
      { { EntranceType::Overworld, RR_LAKE_HYLIA, RR_ZORAS_DOMAIN, 0x0328 },
        { EntranceType::Overworld, RR_ZORAS_DOMAIN, RR_LAKE_HYLIA, 0x0560 } },
      { { EntranceType::Overworld, RR_GV_FORTRESS_SIDE, RR_GERUDO_FORTRESS, 0x0129 },
        { EntranceType::Overworld, RR_GERUDO_FORTRESS, RR_GV_FORTRESS_SIDE, 0x022D } },
      { { EntranceType::Overworld, RR_GF_OUTSIDE_GATE, RR_WASTELAND_NEAR_FORTRESS, 0x0130 },
        { EntranceType::Overworld, RR_WASTELAND_NEAR_FORTRESS, RR_GF_OUTSIDE_GATE, 0x03AC } },
      { { EntranceType::Overworld, RR_WASTELAND_NEAR_COLOSSUS, RR_DESERT_COLOSSUS, 0x0123 },
        { EntranceType::Overworld, RR_DESERT_COLOSSUS, RR_WASTELAND_NEAR_COLOSSUS, 0x0365 } },
      { { EntranceType::Overworld, RR_MARKET_ENTRANCE, RR_THE_MARKET, 0x00B1 },
        { EntranceType::Overworld, RR_THE_MARKET, RR_MARKET_ENTRANCE, 0x0033 } },
      { { EntranceType::Overworld, RR_THE_MARKET, RR_CASTLE_GROUNDS, 0x0138 },
        { EntranceType::Overworld, RR_CASTLE_GROUNDS, RR_THE_MARKET, 0x025A } },
      { { EntranceType::Overworld, RR_THE_MARKET, RR_TOT_ENTRANCE, 0x0171 },
        { EntranceType::Overworld, RR_TOT_ENTRANCE, RR_THE_MARKET, 0x025E } },
      { { EntranceType::Overworld, RR_KAKARIKO_VILLAGE, RR_THE_GRAVEYARD, 0x00E4 },
        { EntranceType::Overworld, RR_THE_GRAVEYARD, RR_KAKARIKO_VILLAGE, 0x0195 } },
      { { EntranceType::Overworld, RR_KAK_BEHIND_GATE, RR_DEATH_MOUNTAIN_TRAIL, 0x013D },
        { EntranceType::Overworld, RR_DEATH_MOUNTAIN_TRAIL, RR_KAK_BEHIND_GATE, 0x0191 } },
      { { EntranceType::Overworld, RR_DEATH_MOUNTAIN_TRAIL, RR_GORON_CITY, 0x014D },
        { EntranceType::Overworld, RR_GORON_CITY, RR_DEATH_MOUNTAIN_TRAIL, 0x01B9 } },
      { { EntranceType::Overworld, RR_GC_DARUNIAS_CHAMBER, RR_DMC_LOWER_LOCAL, 0x0246 },
        { EntranceType::Overworld, RR_DMC_LOWER_NEARBY, RR_GC_DARUNIAS_CHAMBER, 0x01C1 } },
      { { EntranceType::Overworld, RR_DEATH_MOUNTAIN_SUMMIT, RR_DMC_UPPER_LOCAL, 0x0147 },
        { EntranceType::Overworld, RR_DMC_UPPER_NEARBY, RR_DEATH_MOUNTAIN_SUMMIT, 0x01BD } },
      { { EntranceType::Overworld, RR_ZR_BEHIND_WATERFALL, RR_ZORAS_DOMAIN, 0x0108 },
        { EntranceType::Overworld, RR_ZORAS_DOMAIN, RR_ZR_BEHIND_WATERFALL, 0x019D } },
      { { EntranceType::Overworld, RR_ZD_BEHIND_KING_ZORA, RR_ZORAS_FOUNTAIN, 0x0225 },
        { EntranceType::Overworld, RR_ZORAS_FOUNTAIN, RR_ZD_BEHIND_KING_ZORA, 0x01A1 } },

      { { EntranceType::Overworld, RR_GV_LOWER_STREAM, RR_LAKE_HYLIA, 0x0219 }, NO_RETURN_ENTRANCE },

      { { EntranceType::OwlDrop, RR_LH_OWL_FLIGHT, RR_HYRULE_FIELD, 0x027E }, NO_RETURN_ENTRANCE },
      { { EntranceType::OwlDrop, RR_DMT_OWL_FLIGHT, RR_KAK_IMPAS_ROOFTOP, 0x0554 }, NO_RETURN_ENTRANCE },

      { { EntranceType::Spawn, RR_CHILD_SPAWN, RR_KF_LINKS_HOUSE, 0x00BB }, NO_RETURN_ENTRANCE },
      { { EntranceType::Spawn, RR_ADULT_SPAWN, RR_TEMPLE_OF_TIME, 0x0282 },
        NO_RETURN_ENTRANCE }, // 0x282 is an unused entrance index repurposed to differentiate between
                              // Adult Spawn and prelude of light (normally they both use 0x5F4)
      { { EntranceType::WarpSong, RR_MINUET_OF_FOREST_WARP, RR_SACRED_FOREST_MEADOW, 0x0600 }, NO_RETURN_ENTRANCE },
      { { EntranceType::WarpSong, RR_BOLERO_OF_FIRE_WARP, RR_DMC_CENTRAL_LOCAL, 0x04F6 }, NO_RETURN_ENTRANCE },
      { { EntranceType::WarpSong, RR_SERENADE_OF_WATER_WARP, RR_LAKE_HYLIA, 0x0604 }, NO_RETURN_ENTRANCE },
      { { EntranceType::WarpSong, RR_REQUIEM_OF_SPIRIT_WARP, RR_DESERT_COLOSSUS, 0x01F1 }, NO_RETURN_ENTRANCE },
      { { EntranceType::WarpSong, RR_NOCTURNE_OF_SHADOW_WARP, RR_GRAVEYARD_WARP_PAD_REGION, 0x0568 },
        NO_RETURN_ENTRANCE },
      { { EntranceType::WarpSong, RR_PRELUDE_OF_LIGHT_WARP, RR_TEMPLE_OF_TIME, 0x05F4 }, NO_RETURN_ENTRANCE },

      { { EntranceType::ChildBoss, RR_DEKU_TREE_BOSS_ENTRYWAY, RR_DEKU_TREE_BOSS_ROOM, 0x040F },
        { EntranceType::ChildBoss, RR_DEKU_TREE_BOSS_ROOM, RR_DEKU_TREE_BOSS_ENTRYWAY, 0x0252, 0x0457 } },
      { { EntranceType::ChildBoss, RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, RR_DODONGOS_CAVERN_BOSS_ROOM, 0x040B },
        { EntranceType::ChildBoss, RR_DODONGOS_CAVERN_BOSS_ROOM, RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, 0x00C5, 0x047A } },
      { { EntranceType::ChildBoss, RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, RR_JABU_JABUS_BELLY_BOSS_ROOM, 0x0301 },
        { EntranceType::ChildBoss, RR_JABU_JABUS_BELLY_BOSS_ROOM, RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, 0x0407, 0x010E } },
      { { EntranceType::AdultBoss, RR_FOREST_TEMPLE_BOSS_ENTRYWAY, RR_FOREST_TEMPLE_BOSS_ROOM, 0x000C },
        { EntranceType::AdultBoss, RR_FOREST_TEMPLE_BOSS_ROOM, RR_FOREST_TEMPLE_BOSS_ENTRYWAY, 0x024E, 0x0608 } },
      { { EntranceType::AdultBoss, RR_FIRE_TEMPLE_BOSS_ENTRYWAY, RR_FIRE_TEMPLE_BOSS_ROOM, 0x0305 },
        { EntranceType::AdultBoss, RR_FIRE_TEMPLE_BOSS_ROOM, RR_FIRE_TEMPLE_BOSS_ENTRYWAY, 0x0175, 0x0564 } },
      { { EntranceType::AdultBoss, RR_WATER_TEMPLE_BOSS_ENTRYWAY, RR_WATER_TEMPLE_BOSS_ROOM, 0x0417 },
        { EntranceType::AdultBoss, RR_WATER_TEMPLE_BOSS_ROOM, RR_WATER_TEMPLE_BOSS_ENTRYWAY, 0x0423, 0x060C } },
      { { EntranceType::AdultBoss, RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, RR_SPIRIT_TEMPLE_BOSS_ROOM, 0x008D },
        { EntranceType::AdultBoss, RR_SPIRIT_TEMPLE_BOSS_ROOM, RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, 0x02F5, 0x0610 } },
      { { EntranceType::AdultBoss, RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, RR_SHADOW_TEMPLE_BOSS_ROOM, 0x0413 },
        { EntranceType::AdultBoss, RR_SHADOW_TEMPLE_BOSS_ROOM, RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, 0x02B2, 0x0580 } },
  };

  std::map<std::string, PriorityEntrance> priorityEntranceTable = {
      { "Bolero", { { RR_DMC_CENTRAL_LOCAL }, { EntranceType::OwlDrop, EntranceType::WarpSong } } },
      { "Nocturne",
        { { RR_GRAVEYARD_WARP_PAD_REGION }, { EntranceType::OwlDrop, EntranceType::Spawn, EntranceType::WarpSong } } },
      { "Requiem",
        { { RR_DESERT_COLOSSUS, RR_DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY },
          { EntranceType::OwlDrop, EntranceType::Spawn, EntranceType::WarpSong } } },
  };

  entranceShuffleFailure = false;
  SetAllEntrancesData(entranceShuffleTable);

  EntrancePools oneWayEntrancePools = {};
  EntrancePools entrancePools = {};
  std::map<std::string, PriorityEntrance> oneWayPriorities = {};

  // Owl Drops
  if (Settings::ShuffleOwlDrops) {
    oneWayEntrancePools[EntranceType::OwlDrop] = GetShuffleableEntrances(EntranceType::OwlDrop);
  }

  // Spawns
  if (Settings::ShuffleOverworldSpawns) {
    oneWayEntrancePools[EntranceType::Spawn] = GetShuffleableEntrances(EntranceType::Spawn);
  }

  // Warpsongs
  if (Settings::ShuffleWarpSongs) {
    oneWayEntrancePools[EntranceType::WarpSong] = GetShuffleableEntrances(EntranceType::WarpSong);
    // In Glitchless, there aren't any other ways to access these areas
    if (Settings::Logic.Is(LOGIC_GLITCHLESS)) {
      oneWayPriorities["Bolero"] = priorityEntranceTable["Bolero"];
      oneWayPriorities["Nocturne"] = priorityEntranceTable["Nocturne"];
      if (!Settings::ShuffleDungeonEntrances && !Settings::ShuffleOverworldEntrances) {
        oneWayPriorities["Requiem"] = priorityEntranceTable["Requiem"];
      }
    }
  }

  // Shuffle Bosses
  if (Settings::ShuffleBossEntrances.IsNot(SHUFFLEBOSSES_OFF)) {
    if (Settings::ShuffleBossEntrances.Is(SHUFFLEBOSSES_FULL)) {
      entrancePools[EntranceType::Boss] = GetShuffleableEntrances(EntranceType::ChildBoss);
      AddElementsToPool(entrancePools[EntranceType::Boss], GetShuffleableEntrances(EntranceType::AdultBoss));
      // If forest is closed, ensure Ghoma is inside the Deku tree
      // Deku tree being in its vanilla location is handled below
      if (Settings::OpenForest.Is(OPENFOREST_CLOSED) && !(Settings::ShuffleOverworldEntrances || Settings::ShuffleInteriorEntrances)) {
        FilterAndEraseFromPool(entrancePools[EntranceType::Boss], [](const Entrance* entrance){return entrance->GetParentRegionKey()    == RR_DEKU_TREE_BOSS_ENTRYWAY &&
                                                                                                      entrance->GetConnectedRegionKey() == RR_DEKU_TREE_BOSS_ROOM;});
      }
    } else {
      entrancePools[EntranceType::ChildBoss] = GetShuffleableEntrances(EntranceType::ChildBoss);
      entrancePools[EntranceType::AdultBoss] = GetShuffleableEntrances(EntranceType::AdultBoss);
      // If forest is closed, ensure Ghoma is inside the Deku tree
      if (Settings::OpenForest.Is(OPENFOREST_CLOSED) && !(Settings::ShuffleOverworldEntrances || Settings::ShuffleInteriorEntrances)) {
        FilterAndEraseFromPool(entrancePools[EntranceType::ChildBoss], [](const Entrance* entrance){return entrance->GetParentRegionKey()    == RR_DEKU_TREE_BOSS_ENTRYWAY &&
                                                                                                           entrance->GetConnectedRegionKey() == RR_DEKU_TREE_BOSS_ROOM;});
      }
    }
  }

  //Shuffle Dungeon Entrances
  if (Settings::ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF)) {
    entrancePools[EntranceType::Dungeon] = GetShuffleableEntrances(EntranceType::Dungeon);
	//Add Ganon's Castle, if set to On + Ganon
    if (Settings::ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_GANON)) {
      AddElementsToPool(entrancePools[EntranceType::Dungeon], GetShuffleableEntrances(EntranceType::GanonDungeon));
    }
    //If forest is closed don't allow a forest escape via spirit temple hands
    if (Settings::OpenForest.Is(OPENFOREST_CLOSED) && !(Settings::ShuffleOverworldEntrances || Settings::ShuffleInteriorEntrances)) {
      FilterAndEraseFromPool(entrancePools[EntranceType::Dungeon], [](const Entrance* entrance){return entrance->GetParentRegionKey()    == RR_KF_OUTSIDE_DEKU_TREE &&
                                                                                                       entrance->GetConnectedRegionKey() == RR_DEKU_TREE_ENTRYWAY;});
    }
    if (Settings::DecoupleEntrances) {
      for (Entrance* entrance : entrancePools[EntranceType::Dungeon]) {
        entrancePools[EntranceType::DungeonReverse].push_back(entrance->GetReverse());
      }
    }
  }

  // Interior entrances
  if (Settings::ShuffleInteriorEntrances.IsNot(SHUFFLEINTERIORS_OFF)) {
    entrancePools[EntranceType::Interior] = GetShuffleableEntrances(EntranceType::Interior);
    // Special interiors
    if (Settings::ShuffleInteriorEntrances.Is(SHUFFLEINTERIORS_ALL)) {
      AddElementsToPool(entrancePools[EntranceType::Interior], GetShuffleableEntrances(EntranceType::SpecialInterior));
    }
    if (Settings::DecoupleEntrances) {
      for (Entrance* entrance : entrancePools[EntranceType::Interior]) {
        entrancePools[EntranceType::InteriorReverse].push_back(entrance->GetReverse());
      }
    }
  }

  //grotto entrances
  if (Settings::ShuffleGrottoEntrances) {
    entrancePools[EntranceType::GrottoGrave] = GetShuffleableEntrances(EntranceType::GrottoGrave);

    if (Settings::DecoupleEntrances) {
      for (Entrance* entrance : entrancePools[EntranceType::GrottoGrave]) {
        entrancePools[EntranceType::GrottoGraveReverse].push_back(entrance->GetReverse());
      }
    }
  }

  //overworld entrances
  if (Settings::ShuffleOverworldEntrances) {
    bool excludeOverworldReverse = Settings::MixOverworld && !Settings::DecoupleEntrances;
    entrancePools[EntranceType::Overworld] = GetShuffleableEntrances(EntranceType::Overworld, excludeOverworldReverse);
    // Only shuffle GV Lower Stream -> Lake Hylia if decoupled entrances are on
    if (!Settings::DecoupleEntrances) {
      FilterAndEraseFromPool(entrancePools[EntranceType::Overworld], [](const Entrance* entrance){return entrance->GetParentRegionKey()    == RR_GV_LOWER_STREAM &&
                                                                                                         entrance->GetConnectedRegionKey() == RR_LAKE_HYLIA;});
    }
  }

  // Set shuffled entrances as such
  SetShuffledEntrances(entrancePools);
  SetShuffledEntrances(oneWayEntrancePools);

  //combine entrance pools if mixing pools. Only continue if more than one pool is selected.
  int totalMixedPools = (Settings::MixDungeons ? 1 : 0) + (Settings::MixOverworld ? 1 : 0) + (Settings::MixInteriors ? 1 : 0) + (Settings::MixGrottos ? 1 : 0);
  if (totalMixedPools < 2) {
    Settings::MixedEntrancePools.SetSelectedIndex(OFF);
    Settings::MixDungeons.SetSelectedIndex(OFF);
    Settings::MixOverworld.SetSelectedIndex(OFF);
    Settings::MixInteriors.SetSelectedIndex(OFF);
    Settings::MixGrottos.SetSelectedIndex(OFF);
  }
  if (Settings::MixedEntrancePools) {
    std::set<EntranceType> poolsToMix = {};
    if (Settings::MixDungeons) {
      poolsToMix.insert(EntranceType::Dungeon);
      // Insert reverse entrances when decoupled entrances is on
      if (Settings::DecoupleEntrances) {
        poolsToMix.insert(EntranceType::DungeonReverse);
      }
    }
    if (Settings::MixOverworld) {
      poolsToMix.insert(EntranceType::Overworld);
    }
    if (Settings::MixInteriors) {
      poolsToMix.insert(EntranceType::Interior);
      if (Settings::DecoupleEntrances) {
        poolsToMix.insert(EntranceType::InteriorReverse);
      }
    }
    if (Settings::MixGrottos) {
      poolsToMix.insert(EntranceType::GrottoGrave);
      if (Settings::DecoupleEntrances) {
        poolsToMix.insert(EntranceType::GrottoGraveReverse);
      }
    }

    for (auto& pool : entrancePools) {

      auto type = pool.first;

      if (poolsToMix.count(type) > 0) {
        AddElementsToPool(entrancePools[EntranceType::Mixed], pool.second);
        entrancePools[type].clear();
      }
    }
  }

  // Build target entrance pools and set the assumption for entrances being reachable
  EntrancePools oneWayTargetEntrancePools = {};
  for (auto& pool : oneWayEntrancePools) {

    std::vector<EntranceType> validTargetTypes = {};
    EntranceType poolType = pool.first;

    if (poolType == EntranceType::OwlDrop) {
      validTargetTypes = {EntranceType::WarpSong, EntranceType::OwlDrop, EntranceType::Overworld, EntranceType::Extra};
      oneWayTargetEntrancePools[poolType] = BuildOneWayTargets(validTargetTypes, {std::make_pair(RR_PRELUDE_OF_LIGHT_WARP, RR_TEMPLE_OF_TIME)});
      // Owl Drops are only accessible as child, so targets should reflect that
      for (Entrance* target : oneWayTargetEntrancePools[poolType]) {
        target->SetCondition([]{return Logic::IsChild;});
      }

    } else if (poolType == EntranceType::Spawn) {
      validTargetTypes = {EntranceType::Spawn, EntranceType::WarpSong, EntranceType::OwlDrop, EntranceType::Overworld, EntranceType::Interior, EntranceType::SpecialInterior, EntranceType::GrottoGrave, EntranceType::Extra};
      oneWayTargetEntrancePools[poolType] = BuildOneWayTargets(validTargetTypes);

    } else if (poolType == EntranceType::WarpSong) {
      validTargetTypes = {EntranceType::Spawn, EntranceType::WarpSong, EntranceType::OwlDrop, EntranceType::Overworld, EntranceType::Interior, EntranceType::SpecialInterior, EntranceType::GrottoGrave, EntranceType::Extra};
      oneWayTargetEntrancePools[poolType] = BuildOneWayTargets(validTargetTypes);
    }
    // for target in one_way_target_entrance_pools[pool_type]:
    //           target.add_rule((lambda entrances=entrance_pool: (lambda state, **kwargs: any(entrance.connected_region == None for entrance in entrances)))())
  }

  // Disconnect all one way entrances at this point (they need to be connected during all of the above process)
  for (auto& pool : oneWayEntrancePools) {
    for (Entrance* entrance : pool.second) {
      totalRandomizableEntrances++;
      entrance->Disconnect();
    }
  }

  // Assume entrance pools for each type
  EntrancePools targetEntrancePools = {};
  for (auto& pool : entrancePools) {
    targetEntrancePools[pool.first] = AssumeEntrancePool(pool.second);
  }

  //distribution stuff

  //check placed on-way entrances
  //remove replaced entrances so we don't place two in one target
  //remvoe priority targets if any placed entrances point at their regions

  // Place priority entrances
  ShuffleOneWayPriorityEntrances(oneWayPriorities, oneWayEntrancePools, oneWayTargetEntrancePools);
  if (entranceShuffleFailure) {
    return ENTRANCE_SHUFFLE_FAILURE;
  }

  // Delete all targets that we just placed from one way target pools so
  // multiple one way entrances don't use the same target
  std::vector<Entrance*> replacedEntrances = {};
  for (auto& pool : oneWayEntrancePools) {
    for (Entrance* entrance : pool.second) {
      if (entrance->GetReplacement() != nullptr) {
        replacedEntrances.push_back(entrance);
      }
    }
  }
  for (auto& pool : oneWayTargetEntrancePools) {
    for (Entrance* remainingTarget : pool.second) {
      auto replacement = remainingTarget->GetReplacement();
      if (ElementInContainer(replacement, replacedEntrances)) {
        DeleteTargetEntrance(remainingTarget);
      }
    }
  }

  // Shuffle all one way entrances among pools to shuffle
  for (auto& pool : oneWayEntrancePools) {
    ShuffleEntrancePool(pool.second, oneWayTargetEntrancePools[pool.first], 5);
    if (entranceShuffleFailure) {
      return ENTRANCE_SHUFFLE_FAILURE;
    }
    // Delete all targets that we just placed from other one way target pools so
    // multiple one way entrances don't use the same target
    replacedEntrances = FilterFromPool(pool.second, [](Entrance* entrance){
      return entrance->GetReplacement() != nullptr;
    });
    for (auto& targetPool : oneWayTargetEntrancePools) {
      for (Entrance* remainingTarget : targetPool.second) {
        auto replacement = remainingTarget->GetReplacement();
        if (ElementInContainer(replacement, replacedEntrances)) {
          DeleteTargetEntrance(remainingTarget);
        }
      }
    }
    // Delete all unused extra targets after placing a one way pool, since the
    // unused targets won't ever be replaced
    for (Entrance* unusedTarget : oneWayTargetEntrancePools[pool.first]) {
      DeleteTargetEntrance(unusedTarget);
    }
  }

  //shuffle all entrances among pools to shuffle
  for (auto& pool : entrancePools) {
    ShuffleEntrancePool(pool.second, targetEntrancePools[pool.first]);
    if (entranceShuffleFailure) {
      return ENTRANCE_SHUFFLE_FAILURE;
    }
  }

  return ENTRANCE_SHUFFLE_SUCCESS;
}

// Save the set of shuffled entrances that will be sent to the patch
void CreateEntranceOverrides() {
  entranceOverrides.clear();
  if (noRandomEntrances) {
    return;
  }
  SPDLOG_DEBUG("\nCREATING ENTRANCE OVERRIDES\n");
  auto allShuffleableEntrances = GetShuffleableEntrances(EntranceType::All, false);

  for (Entrance* entrance : allShuffleableEntrances) {

    //Double-check to make sure the entrance is actually shuffled
    if (!entrance->IsShuffled()) {
      continue;
    }

    auto message = "Setting " + entrance->to_string() + "\n";
    SPDLOG_DEBUG(message);

    int16_t originalIndex = entrance->GetIndex();
    int16_t originalBlueWarp = entrance->GetBlueWarp();
    int16_t replacementIndex = entrance->GetReplacement()->GetIndex();

    int16_t destinationIndex = -1;
    int16_t replacementDestinationIndex = -1;

    // Only set destination indices for two way entrances and when decouple entrances
    // is off
    if (entrance->GetReverse() != nullptr && !Settings::DecoupleEntrances) {
      replacementDestinationIndex = entrance->GetReplacement()->GetReverse()->GetIndex();
      destinationIndex = entrance->GetReverse()->GetIndex();
    }

    entranceOverrides.push_back({
      .index = originalIndex,
      .destination = destinationIndex,
      .blueWarp = originalBlueWarp,
      .override = replacementIndex,
      .overrideDestination = replacementDestinationIndex,
    });

    message = "\tOriginal: " + std::to_string(originalIndex) + "\n";
    SPDLOG_DEBUG(message);
    message = "\tReplacement " + std::to_string(replacementIndex) + "\n";
    SPDLOG_DEBUG(message);
  }
}

std::vector<std::list<Entrance*>> playthroughEntrances;

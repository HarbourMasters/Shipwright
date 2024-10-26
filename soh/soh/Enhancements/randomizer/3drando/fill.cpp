#include "fill.hpp"

#include "custom_messages.hpp"
#include "../dungeon.h"
#include "../context.h"
#include "item_pool.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "starting_inventory.hpp"
#include "hints.hpp"
#include "shops.hpp"
#include "pool_functions.hpp"
//#include "debug.hpp"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/debugger/performanceTimer.h"

#include <vector>
#include <list>
#include <set>
#include <spdlog/spdlog.h>

using namespace CustomMessages;
using namespace Rando;


static bool placementFailure = false;


PriceSettingsStruct shopsanityPrices = {RSK_SHOPSANITY_PRICES, 
                                        RSK_SHOPSANITY_PRICES_FIXED_PRICE,
                                        RSK_SHOPSANITY_PRICES_RANGE_1,
                                        RSK_SHOPSANITY_PRICES_RANGE_2,
                                        RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT,
                                        RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT,
                                        RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT,
                                        RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT,
                                        RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT,
                                        RSK_SHOPSANITY_PRICES_AFFORDABLE};
  
PriceSettingsStruct scrubPrices = {RSK_SCRUBS_PRICES, 
                                   RSK_SCRUBS_PRICES_FIXED_PRICE,
                                   RSK_SCRUBS_PRICES_RANGE_1,
                                   RSK_SCRUBS_PRICES_RANGE_2,
                                   RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT,
                                   RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT,
                                   RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT,
                                   RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT,
                                   RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT,
                                   RSK_SCRUBS_PRICES_AFFORDABLE};

PriceSettingsStruct merchantPrices = {RSK_MERCHANT_PRICES, 
                                      RSK_MERCHANT_PRICES_FIXED_PRICE,
                                      RSK_MERCHANT_PRICES_RANGE_1,
                                      RSK_MERCHANT_PRICES_RANGE_2,
                                      RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT,
                                      RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT,
                                      RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT,
                                      RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT,
                                      RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT,
                                      RSK_MERCHANT_PRICES_AFFORDABLE};

static void RemoveStartingItemsFromPool() {
  for (RandomizerGet startingItem : StartingInventory) {
    for (size_t i = 0; i < ItemPool.size(); i++) {
      if (startingItem == RG_BIGGORON_SWORD) {
        if (ItemPool[i] == RG_GIANTS_KNIFE || ItemPool[i] == RG_BIGGORON_SWORD) {
          ItemPool[i] = GetJunkItem();
        }
        continue;
      } else if (startingItem == ItemPool[i] || (Rando::StaticData::RetrieveItem(startingItem).IsBottleItem() &&
                                                 Rando::StaticData::RetrieveItem(ItemPool[i]).IsBottleItem())) {
        if (AdditionalHeartContainers > 0 && 
          (startingItem == RG_PIECE_OF_HEART || startingItem == RG_TREASURE_GAME_HEART)) {
          ItemPool[i] = RG_HEART_CONTAINER;
          AdditionalHeartContainers--;
        } else {
          ItemPool[i] = GetJunkItem();
        }
        break;
      }
    }
  }
}

static void PropagateTimeTravel(GetAccessibleLocationsStruct& gals, RandomizerGet ignore = RG_NONE, 
                              bool stopOnBeatable = false, bool addToPlaythrough = false){
  //special check for temple of time
  if(gals.haveTimeAccess && gals.foundTempleOfTime && gals.validatedStartingRegion){
    if (!RegionTable(RR_ROOT)->Adult() && RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->Child()) { //RANDOTODO: sphere weirdness, other age locations not propagated in this sphere
      RegionTable(RR_ROOT)->adultDay   = RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->childDay;
      RegionTable(RR_ROOT)->adultNight = RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->childNight;
      ProcessRegion(RegionTable(RR_ROOT), gals, ignore, stopOnBeatable, addToPlaythrough);
    } else if (!RegionTable(RR_ROOT)->Child() && RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->Adult()){
      RegionTable(RR_ROOT)->childDay   = RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->adultDay;
      RegionTable(RR_ROOT)->childNight = RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->adultNight;
      ProcessRegion(RegionTable(RR_ROOT), gals, ignore, stopOnBeatable, addToPlaythrough);
    }
  }
}

//This function will propagate Time of Day access through the entrance
static bool UpdateToDAccess(Entrance* entrance, Region* connection) {
  StartPerformanceTimer(PT_TOD_ACCESS);

  bool ageTimePropogated = false;
  Region* parent = entrance->GetParentRegion();

  if (!connection->childDay && parent->childDay && entrance->CheckConditionAtAgeTime(logic->IsChild, logic->AtDay)) {
    connection->childDay = true;
    ageTimePropogated = true;
  }
  if (!connection->childNight && parent->childNight && entrance->CheckConditionAtAgeTime(logic->IsChild, logic->AtNight)) {
    connection->childNight = true;
    ageTimePropogated = true;
  }
  if (!connection->adultDay && parent->adultDay && entrance->CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay)) {
    connection->adultDay = true;
    ageTimePropogated = true;
  }
  if (!connection->adultNight && parent->adultNight && entrance->CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight)) {
    connection->adultNight = true;
    ageTimePropogated = true;
  }

  StopPerformanceTimer(PT_TOD_ACCESS);
  return ageTimePropogated;
}

// Check if key locations in the overworld are accessable
static void ValidateOtherEntrance(GetAccessibleLocationsStruct& gals) {
  auto ctx = Rando::Context::GetInstance();
  // Condition for validating Temple of Time Access
  if (!gals.foundTempleOfTime && ((ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD && RegionTable(RR_TEMPLE_OF_TIME)->Adult()) || 
                            (ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_ADULT && RegionTable(RR_TEMPLE_OF_TIME)->Child()))) {
    gals.foundTempleOfTime = true;
  }
  // Condition for validating a valid starting region
  if (!gals.validatedStartingRegion) {
    bool childAccess = ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD || RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->Child();
    bool adultAccess = ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_ADULT || RegionTable(RR_TOT_BEYOND_DOOR_OF_TIME)->Adult();

    Region* kokiri = RegionTable(RR_KOKIRI_FOREST);
    Region* kakariko = RegionTable(RR_KAKARIKO_VILLAGE);

    if ((childAccess && (kokiri->Child() || kakariko->Child())) ||// RANDOTODO when proper ammo logic is done, this could probably be made optional
        (adultAccess && (kokiri->Adult() || kakariko->Adult()))) {
       gals.validatedStartingRegion = true;
       ApplyStartingInventory(); // RANDOTODO when proper ammo logic is done, this could be moved to the start
    }
  }
}

// Apply all items that are necessary for checking all location access
static void ApplyAllAdvancmentItems(){
  std::vector<RandomizerGet> itemsToPlace =
      FilterFromPool(ItemPool, [](const auto i) { return Rando::StaticData::RetrieveItem(i).IsAdvancement(); });
  for (RandomizerGet unplacedItem : itemsToPlace) {
      Rando::StaticData::RetrieveItem(unplacedItem).ApplyEffect();
  }
}

// Check if everything in an entrance rando seed that needs to be avalible without items, is,
// and if so allow obtaining items in logic
static void ValidateSphereZero(GetAccessibleLocationsStruct& gals){
  auto ctx = Rando::Context::GetInstance();
  // Condition for verifying everything required for sphere 0, expanding search to all locations
  if (logic->CanEmptyBigPoes && gals.validatedStartingRegion && gals.foundTempleOfTime && gals.haveTimeAccess) {
    // Apply all items that are necessary for checking all location access
    ApplyAllAdvancmentItems();
    // Reset access as the non-starting age
    if (ctx->GetSettings()->ResolvedStartingAge() == RO_AGE_CHILD) {
      for (RandomizerRegion regionKey : gals.regionPool) {
        RegionTable(regionKey)->adultDay = false;
        RegionTable(regionKey)->adultNight = false;
      }
    } else {
      for (RandomizerRegion regionKey : gals.regionPool) {
        RegionTable(regionKey)->childDay = false;
        RegionTable(regionKey)->childNight = false;
      }
    }
    //RANDOTODO do we want to keep the region pool after this reset? 
    //It's a lot of potential looping over regions we have no access to
    gals.sphereZeroComplete = true;
  } 
}

//This function handles each possible exit
void ProcessExits(Region* region, GetAccessibleLocationsStruct& gals, RandomizerGet ignore = RG_NONE, 
                  bool stopOnBeatable = false, bool addToPlaythrough = false){
  auto ctx = Rando::Context::GetInstance();
  for (auto& exit : region->exits) {
    Region* exitRegion = exit.GetConnectedRegion();
    //Update Time of Day Access for the exit
    if (UpdateToDAccess(&exit, exitRegion)) {
      gals.logicUpdated = true;
      if (!gals.sphereZeroComplete){
        if (!gals.foundTempleOfTime || !gals.validatedStartingRegion){
          ValidateOtherEntrance(gals);
        }
        ValidateSphereZero(gals);
      }
      //process the region we just expanded to, to reduce looping
      ProcessRegion(exitRegion, gals, ignore, stopOnBeatable, addToPlaythrough);
    }

    //If the exit is accessible and hasn't been added yet, add it to the pool
    //RANDOTODO do we want to add the region after the loop now, considering we
    //are processing the new region immediately. Maybe a reverse for loop in ProcessRegion?
    if (!exitRegion->addedToPool && exit.ConditionsMet()) {
      exitRegion->addedToPool = true;
      gals.regionPool.push_back(exit.GetConnectedRegionKey());
    }

    if (addToPlaythrough){
      // RANDOTODO Should this match the regular spheres?
      // Add shuffled entrances to the entrance playthrough
      // Include bluewarps when unshuffled but dungeon or boss shuffle is on
      if((exit.IsShuffled() || (exit.GetType() == Rando::EntranceType::BlueWarp && 
          (ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES) || ctx->GetOption(RSK_SHUFFLE_BOSS_ENTRANCES)))) &&
          !exit.IsAddedToPool() && !ctx->GetEntranceShuffler()->HasNoRandomEntrances()) {
        gals.entranceSphere.push_back(&exit);
        exit.AddToPool();
        // Don't list a two-way coupled entrance from both directions
        if (exit.GetReverse() != nullptr && exit.GetReplacement()->GetReverse() != nullptr && !exit.IsDecoupled()) {
          exit.GetReplacement()->GetReverse()->AddToPool();
        }
      }
    }
  }
}


//Get the max number of tokens that can possibly be useful
static int GetMaxGSCount() {
  auto ctx = Rando::Context::GetInstance();
  //If bridge or LACS is set to tokens, get how many are required
  int maxBridge = 0;
  int maxLACS = 0;
  if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)) {
    maxBridge = ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>();
  }
  if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
    maxLACS = ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>();
  }
  maxBridge = std::max(maxBridge, maxLACS);
  //Get the max amount of GS which could be useful from token reward locations
  int maxUseful = 0;
  //If the highest advancement item is a token, we know it is useless since it won't lead to an otherwise useful item
  if (ctx->GetItemLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD)->GetPlacedItem().IsAdvancement() && ctx->GetItemLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD)->GetPlacedItem().GetItemType() != ITEMTYPE_TOKEN) {
    maxUseful = 100;
  } else if (ctx->GetItemLocation(RC_KAK_50_GOLD_SKULLTULA_REWARD)->GetPlacedItem().IsAdvancement() &&
             ctx->GetItemLocation(RC_KAK_50_GOLD_SKULLTULA_REWARD)->GetPlacedItem().GetItemType() != ITEMTYPE_TOKEN) {
      maxUseful = 50;
  } else if (ctx->GetItemLocation(RC_KAK_40_GOLD_SKULLTULA_REWARD)->GetPlacedItem().IsAdvancement() &&
             ctx->GetItemLocation(RC_KAK_40_GOLD_SKULLTULA_REWARD)->GetPlacedItem().GetItemType() != ITEMTYPE_TOKEN) {
      maxUseful = 40;
  } else if (ctx->GetItemLocation(RC_KAK_30_GOLD_SKULLTULA_REWARD)->GetPlacedItem().IsAdvancement() &&
             ctx->GetItemLocation(RC_KAK_30_GOLD_SKULLTULA_REWARD)->GetPlacedItem().GetItemType() != ITEMTYPE_TOKEN) {
      maxUseful = 30;
  } else if (ctx->GetItemLocation(RC_KAK_20_GOLD_SKULLTULA_REWARD)->GetPlacedItem().IsAdvancement() &&
             ctx->GetItemLocation(RC_KAK_20_GOLD_SKULLTULA_REWARD)->GetPlacedItem().GetItemType() != ITEMTYPE_TOKEN) {
      maxUseful = 20;
  } else if (ctx->GetItemLocation(RC_KAK_10_GOLD_SKULLTULA_REWARD)->GetPlacedItem().IsAdvancement() &&
             ctx->GetItemLocation(RC_KAK_10_GOLD_SKULLTULA_REWARD)->GetPlacedItem().GetItemType() != ITEMTYPE_TOKEN) {
      maxUseful = 10;
  }
  //Return max of the two possible reasons tokens could be important, minus the tokens in the starting inventory
  return std::max(maxUseful, maxBridge) - ctx->GetOption(RSK_STARTING_SKULLTULA_TOKEN).Value<uint8_t>();
}

std::string GetShopItemBaseName(std::string itemName) {
  std::string baseName = itemName.erase(0, 4); //Delete "Buy "
  //Delete amount, if present (so when it looks like Buy Deku Nut (10) remove the (10))
  if (baseName.find("(") != std::string::npos) {
    baseName = baseName.erase(baseName.find("("));
  }
  //Do the same for [] (only applies to red potions, other things with [] also have a ())
  if (baseName.find("[") != std::string::npos) {
    baseName = baseName.erase(baseName.find("["));
  }
  return baseName;
}

std::vector<RandomizerCheck> GetEmptyLocations(std::vector<RandomizerCheck> targetLocations) {
    auto ctx = Rando::Context::GetInstance();
    return FilterFromPool(targetLocations, [ctx](const auto loc) {
        return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_NONE;
    });
}

std::vector<RandomizerCheck> GetAllEmptyLocations() {
    auto ctx = Rando::Context::GetInstance();
    return FilterFromPool(ctx->allLocations, [ctx](const auto loc) {
        return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_NONE;
    });
}

bool IsBombchus(RandomizerGet item, bool includeShops = false){
  return (item >= RG_BOMBCHU_5 && item <= RG_BOMBCHU_20) || item == RG_PROGRESSIVE_BOMBCHUS || 
    (includeShops && (item == RG_BUY_BOMBCHUS_10 || item == RG_BUY_BOMBCHUS_20));
}

bool IsBeatableWithout(RandomizerCheck excludedCheck, bool replaceItem, RandomizerGet ignore = RG_NONE){ //RANDOTODO make excludCheck an ItemLocation
  auto ctx = Rando::Context::GetInstance();
  RandomizerGet copy = ctx->GetItemLocation(excludedCheck)->GetPlacedRandomizerGet(); //Copy out item
  ctx->GetItemLocation(excludedCheck)->SetPlacedItem(RG_NONE); //Write in empty item
  ctx->playthroughBeatable = false;
  logic->Reset();
  CheckBeatable(ignore);
  if (replaceItem){
    ctx->GetItemLocation(excludedCheck)->SetPlacedItem(copy); //Immediately put item back
  }
  return ctx->playthroughBeatable;
}

// Reset non-Logic-class logic, and optionally apply the initial inventory
void ResetLogic(std::shared_ptr<Context>& ctx, GetAccessibleLocationsStruct& gals, bool applyInventory = false){
  gals.timePassChildDay = true;
  gals.timePassChildNight = true;
  gals.timePassAdultDay = true;
  gals.timePassAdultNight = true;
  gals.haveTimeAccess = true;
  gals.foundTempleOfTime = true;
  gals.validatedStartingRegion = true;
  gals.sphereZeroComplete = true;
  if (applyInventory){
    ApplyStartingInventory();
  }
  Regions::AccessReset();
  ctx->LocationReset();
} 

//Generate the playthrough, so we want to add advancement items, unless we know to ignore them
void AddToPlaythrough(LocationAccess& locPair, GetAccessibleLocationsStruct& gals){
  auto ctx = Rando::Context::GetInstance();
  RandomizerCheck loc = locPair.GetLocation();
  Rando::ItemLocation* location = ctx->GetItemLocation(loc);
  RandomizerGet locItem = location->GetPlacedRandomizerGet();
  //Item is an advancement item, figure out if it should be added to this sphere
  if (!ctx->playthroughBeatable && location->GetPlacedItem().IsAdvancement()) {
    ItemType type = location->GetPlacedItem().GetItemType();

    //Decide whether to exclude this location
    //This preprocessing is done to reduce the amount of searches performed in PareDownPlaythrough
    //Want to exclude:
    //1) Tokens after the last potentially useful one (the last one that gives an advancement item or last for token bridge)
    //2) Buy items of the same type, after the first (So only see Buy Deku Nut of any amount once)
    bool exclude = true;
    //Exclude tokens after the last possibly useful one
    if (type == ITEMTYPE_TOKEN && gals.gsCount < gals.maxGsCount) {
      gals.gsCount++;
      exclude = false;
    }
    //Handle buy items
    //If ammo drops are off, don't do this step, since buyable ammo becomes logically important
    // TODO: Reimplement Ammo Drops setting
    else if (/*AmmoDrops.IsNot(AMMODROPS_NONE) &&*/ type == ITEMTYPE_SHOP) {
      //Only check each buy item once
      auto buyItem = location->GetPlacedItem().GetLogicVal();
      //Buy item not in list to ignore, add it to list and write to playthrough
      if (std::find(gals.buyIgnores.begin(), gals.buyIgnores.end(), buyItem) == gals.buyIgnores.end()) {
        exclude = false;
        gals.buyIgnores.push_back(buyItem);
      }
    }
    //Add all other advancement items
    else if (type != ITEMTYPE_TOKEN && (/*AmmoDrops.Is(AMMODROPS_NONE) ||*/ type != ITEMTYPE_SHOP)) {
      exclude = false;
    }
    //Has not been excluded, add to playthrough
    if (!exclude) {
      gals.itemSphere.push_back(loc);
    }
  }
  //Triforce has been found, seed is beatable, nothing else in this or future spheres matters
  else if (location->GetPlacedRandomizerGet() == RG_TRIFORCE) {
    gals.itemSphere.clear();
    gals.itemSphere.push_back(loc);
    ctx->playthroughBeatable = true;
  }
}

void ApplyOrStoreItem(Rando::ItemLocation* loc, GetAccessibleLocationsStruct& gals, bool addToPlaythrough){
  if (addToPlaythrough){
    gals.newItemLocations.push_back(loc);
  } else {
    loc->ApplyPlacedItemEffect();
  }
  gals.logicUpdated = true;
}

// Adds the contents of a location to the current progression and optionally playthrough
bool AddCheckToLogic(LocationAccess& locPair, GetAccessibleLocationsStruct& gals, RandomizerGet ignore, bool stopOnBeatable, bool addToPlaythrough=false){
  auto ctx = Rando::Context::GetInstance();
  StartPerformanceTimer(PT_LOCATION_LOGIC);
  RandomizerCheck loc = locPair.GetLocation();
  Rando::ItemLocation* location = ctx->GetItemLocation(loc);
  RandomizerGet locItem = location->GetPlacedRandomizerGet();

  if (!location->IsAddedToPool() && locPair.ConditionsMet()) {
    location->AddToPool();

    if (locItem == RG_NONE) {
      gals.accessibleLocations.push_back(loc); //Empty location, consider for placement
    } else {
      //If ignore has a value, we want to check if the item location should be considered or not
      //This is necessary due to the below preprocessing for playthrough generation
      if (ignore != RG_NONE) {
        ItemType type = location->GetPlacedItem().GetItemType();
        //If we want to ignore tokens, only add if not a token
        if (ignore == RG_GOLD_SKULLTULA_TOKEN && type != ITEMTYPE_TOKEN) {
          ApplyOrStoreItem(location, gals, addToPlaythrough);
        }
        //If we want to ignore bombchus, only add if bombchu is not in the name
        else if (IsBombchus(ignore) && IsBombchus(locItem, true)) {
          ApplyOrStoreItem(location, gals, addToPlaythrough);
        }
        //We want to ignore a specific Buy item. Buy items with different RandomizerGets are recognised by a shared GetLogicVal
        else if (ignore != RG_GOLD_SKULLTULA_TOKEN && IsBombchus(ignore)) {
          if ((type == ITEMTYPE_SHOP && Rando::StaticData::GetItemTable()[ignore].GetLogicVal() != location->GetPlacedItem().GetLogicVal()) || type != ITEMTYPE_SHOP) {
            ApplyOrStoreItem(location, gals, addToPlaythrough);
          }
        }
      }
      //If it doesn't, we can just add the location
      else {
        ApplyOrStoreItem(location, gals, addToPlaythrough); //Add item to cache to be considered in logic next iteration
      }
    }
    if (addToPlaythrough){
      AddToPlaythrough(locPair, gals);
    }
    //All we care about is if the game is beatable, used to pare down playthrough
    if (location->GetPlacedRandomizerGet() == RG_TRIFORCE && stopOnBeatable) {
      StopPerformanceTimer(PT_LOCATION_LOGIC);
      return true; //Return early for efficiency
    }
  }
  StopPerformanceTimer(PT_LOCATION_LOGIC);
  return false;
}

void ProcessRegion(Region* region, GetAccessibleLocationsStruct& gals, RandomizerGet ignore, 
                   bool stopOnBeatable, bool addToPlaythrough){
  
  if (gals.haveTimeAccess) {
    region->ApplyTimePass();
  } else {
    // If we're checking for TimePass access do that for each region as it's being updated.
    // TimePass Access is satisfied when every AgeTime can reach a region with TimePass
    // without the aid of TimePass. During this mode, TimePass won't update ToD access
    // in any region.
    //RANDOTODO can probably be removed after a ToD rework that accounts for having Dampe time access
    if (region->timePass) {
      if (region->childDay) {
        gals.timePassChildDay = true;
      }
      if (region->childNight) {
        gals.timePassChildNight = true;
      }
      if (region->adultDay) {
        gals.timePassAdultDay = true;
      }
      if (region->adultNight) {
        gals.timePassAdultNight = true;
      }
    }
    // Condition for validating that all startring AgeTimes have timepass access
    if (gals.timePassChildDay && gals.timePassChildNight && 
        gals.timePassAdultDay && gals.timePassAdultNight) {
      gals.haveTimeAccess = true;
      region->ApplyTimePass();
    }
  }

  if (region->UpdateEvents()){
    gals.logicUpdated = true;
    //if we are working in spheres, reset the sphere on an event being enabled to avoid sphere skipping
    if (addToPlaythrough){
      gals.resetSphere = true;
    }
  }
  
  ProcessExits(region, gals, ignore, stopOnBeatable, addToPlaythrough);
  
  PropagateTimeTravel(gals, ignore, stopOnBeatable, addToPlaythrough);
  for (size_t k = 0; k < region->locations.size(); k++) {
    if(AddCheckToLogic(region->locations[k], gals, ignore, stopOnBeatable, addToPlaythrough)){
      Rando::Context::GetInstance()->playthroughBeatable = true;
      return;
    }
  }
}

// Return any of the targetLocations that are accessible in logic
std::vector<RandomizerCheck> ReachabilitySearch(const std::vector<RandomizerCheck>& targetLocations, RandomizerGet ignore /* = RG_NONE*/) {
  auto ctx = Rando::Context::GetInstance();
  GetAccessibleLocationsStruct gals(0);
  ResetLogic(ctx, gals, true);
  do {
    gals.InitLoop();
    for (size_t i = 0; i < gals.regionPool.size(); i++) {
      ProcessRegion(RegionTable(gals.regionPool[i]), gals, ignore);
    }
  } while (gals.logicUpdated);
  erase_if(gals.accessibleLocations, [&targetLocations, ctx](RandomizerCheck loc){
    for (RandomizerCheck allowedLocation : targetLocations) {
      if (loc == allowedLocation || ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() != RG_NONE) {
        return false;
      }
    }
    return true;
  });
  return gals.accessibleLocations;
}

// Create the playthrough for the seed
void GeneratePlaythrough() {
  auto ctx = Rando::Context::GetInstance();
  ctx->playthroughBeatable = false;
  logic->Reset();
  GetAccessibleLocationsStruct gals(GetMaxGSCount());
  ResetLogic(ctx, gals, true);
  do {
    gals.InitLoop();
    for (size_t i = 0; i < gals.regionPool.size(); i++) {
      ProcessRegion(RegionTable(gals.regionPool[i]), gals, RG_NONE, false, true);
      if (gals.resetSphere){
        gals.resetSphere = false;
        i = -1;
      }
    }
    if (gals.itemSphere.size() > 0) {
      ctx->playthroughLocations.push_back(gals.itemSphere);
    }
    if (gals.entranceSphere.size() > 0 && !ctx->GetEntranceShuffler()->HasNoRandomEntrances()) {
      ctx->GetEntranceShuffler()->playthroughEntrances.push_back(gals.entranceSphere);
    }
  } while (gals.logicUpdated);
}

// return if the seed is currently beatable or not
bool CheckBeatable(RandomizerGet ignore /* = RG_NONE*/) {
  auto ctx = Rando::Context::GetInstance();
  GetAccessibleLocationsStruct gals(0);
  ResetLogic(ctx, gals, true);
  do {
    gals.InitLoop();
    for (size_t i = 0; i < gals.regionPool.size(); i++) {
      ProcessRegion(RegionTable(gals.regionPool[i]), gals, ignore, true);
      if (ctx->playthroughBeatable == true){
        return true;
      }
    }
  } while (gals.logicUpdated);
  return false;
}

// Check if the currently randomised set of entrances is a valid game map.
void ValidateEntrances(bool checkPoeCollectorAccess, bool checkOtherEntranceAccess) {
  auto ctx = Rando::Context::GetInstance();
  GetAccessibleLocationsStruct gals(0);
  ResetLogic(ctx, gals, !checkOtherEntranceAccess);

  ctx->allLocationsReachable = false;
  if (checkPoeCollectorAccess){
    logic->CanEmptyBigPoes = false;
  }

  if (checkOtherEntranceAccess){
    gals.foundTempleOfTime = false;
    gals.validatedStartingRegion = false;
    //Variables for Time Pass access
    gals.timePassChildDay = false;
    gals.timePassChildNight = false;
    gals.timePassAdultDay = false;
    gals.timePassAdultNight = false;
    gals.haveTimeAccess = false;
    gals.sphereZeroComplete = false;
    RegionTable(RR_ROOT)->childNight = true;
    RegionTable(RR_ROOT)->adultNight = true;
    RegionTable(RR_ROOT)->childDay = true;
    RegionTable(RR_ROOT)->adultDay = true;
  } else {
    ApplyAllAdvancmentItems();
  }

  do {
    gals.InitLoop();
    for (size_t i = 0; i < gals.regionPool.size(); i++) {
      ProcessRegion(RegionTable(gals.regionPool[i]), gals);
    }
  } while (gals.logicUpdated);
  if (gals.sphereZeroComplete) {
    ctx->allLocationsReachable = true;
    //RANDOTODO a size check here before getting the exact fails would be a minor optimization
    //and a full list of location failures would be useful for logging when it does fail
    for (const RandomizerCheck loc : ctx->allLocations) {
      if (!ctx->GetItemLocation(loc)->IsAddedToPool()) {
          ctx->allLocationsReachable = false;
          auto message = "Location " +
                         Rando::StaticData::GetLocation(ctx->GetItemLocation(loc)->GetRandomizerCheck())->GetName() +
                         " not reachable\n";
          SPDLOG_DEBUG(message);
        #ifndef ENABLE_DEBUG
          break;
        #endif
      }
    }
  }
}

void LookForExternalArea(Region* currentRegion, std::set<Region*> &alreadyChecked, std::set<RandomizerArea> &areas, bool LowPriorityMode=false){
  for (const auto& entrance : currentRegion->entrances) {
    //if the region is arealess and hasn't already been checked, recursivly check what connects to it
    //if this entrance does not pass areas, only process it if we are in low priority mode
    if ((LowPriorityMode || entrance->DoesSpreadAreas()) && !alreadyChecked.contains(entrance->GetParentRegion())){
      std::set<RandomizerArea> otherAreas = entrance->GetParentRegion()->GetAllAreas();
      alreadyChecked.insert(entrance->GetParentRegion());
      if (otherAreas.size() == 0) {
        LookForExternalArea(entrance->GetParentRegion(), alreadyChecked, areas, LowPriorityMode);
      //If we find a valid area we should add it.
      //If it's Links Pocket or RA_NONE, do not propagate those, they are not real areas.
      //This check is likely to fail if a region somehow is both in Link's Pocket and elsewhere, but this should never happen
      } else if (*otherAreas.begin() > RA_LINKS_POCKET){
        areas.merge(otherAreas);
      }
    }
  }
}

void SetAreas(){
  auto ctx = Rando::Context::GetInstance();
//RANDOTODO give entrances an enum like RandomizerCheck, the give them all areas here,
//then use those areas to not need to recursivly find ItemLocation areas when an identifying entrance's area
  for (int regionType = 0; regionType < RR_MARKER_AREAS_END; regionType++) {
    Region* region = &areaTable[regionType];
    std::set<RandomizerArea> areas = region->GetAllAreas();
    std::set<Region*> regionsToSet = {region};
    if (areas.empty()) {
      LookForExternalArea(region, regionsToSet, areas);
      //If we found nothing, try again in low priority mode to try every entrance
      if (areas.empty()) {
        LookForExternalArea(region, regionsToSet, areas, true);
        //If we still found nothing, we're disconnected, use RA_NONE to represent that
        if (areas.empty()){
          areas.insert(RA_NONE);
        }
      }
    }
    for (auto regionToSet : regionsToSet) {
      regionToSet->ReplaceAreas(areas);
      for (auto& loc : regionToSet->locations){
        ctx->GetItemLocation(loc.GetLocation())->MergeAreas(areas);
      }
    }
  }
}

//Remove unnecessary items from playthrough by removing their location, and checking if game is still beatable
//To reduce searches, some preprocessing is done in playthrough generation to avoid adding obviously unnecessary items
static void PareDownPlaythrough() {
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> toAddBackItem;
  //Start at sphere before Ganon's and count down
  for (int i = ctx->playthroughLocations.size() - 2; i >= 0; i--) {
    //Check each item location in sphere
    std::vector<int> erasableIndices;
    std::vector<RandomizerCheck> sphere = ctx->playthroughLocations.at(i);
    for (int j = sphere.size() - 1; j >= 0; j--) {
      RandomizerCheck loc = sphere.at(j);
      RandomizerGet locGet = ctx->GetItemLocation(loc)->GetPlacedRandomizerGet(); //Copy out item

      RandomizerGet ignore = RG_NONE;
      if (locGet == RG_GOLD_SKULLTULA_TOKEN || IsBombchus(locGet, true)
        || Rando::StaticData::RetrieveItem(locGet).GetItemType() == ITEMTYPE_SHOP) {
        ignore = locGet;
      }

      //Playthrough is still beatable without this item, therefore it can be removed from playthrough section.
      if (IsBeatableWithout(loc, false, ignore)) {
        ctx->playthroughLocations[i].erase(ctx->playthroughLocations[i].begin() + j);
        ctx->GetItemLocation(loc)->SetDelayedItem(locGet); //Game is still beatable, don't add back until later
        toAddBackItem.push_back(loc);
      }
      else {
        ctx->GetItemLocation(loc)->SetPlacedItem(locGet); //Immediately put item back so game is beatable again
      }
    }
  }

  //Some spheres may now be empty, remove these
  for (int i = ctx->playthroughLocations.size() - 2; i >= 0; i--) {
    if (ctx->playthroughLocations.at(i).size() == 0) {
      ctx->playthroughLocations.erase(ctx->playthroughLocations.begin() + i);
    }
  }

  //Now we can add back items which were removed previously
  for (RandomizerCheck loc : toAddBackItem) {
    ctx->GetItemLocation(loc)->SaveDelayedItem();
  }
}

//Very similar to PareDownPlaythrough except it sets WotH candidacy of Way of the Hero items
//Way of the Hero items are more specific than playthrough items in that they are items which *must*
// be obtained to logically be able to complete the seed, rather than playthrough items which
// are just possible items you *can* collect to complete the seed.
static void CalculateWotH() {
  auto ctx = Rando::Context::GetInstance();
  //size - 1 so Triforce is not counted
  for (size_t i = 0; i < ctx->playthroughLocations.size() - 1; i++) {
    for (size_t j = 0; j < ctx->playthroughLocations[i].size(); j++) {
      //If removing this item and no other item caused the game to become unbeatable, then it is strictly necessary,
      //so add it unless it is in Links Pocket or an isolated place.
      auto itemLoc = ctx->GetItemLocation(ctx->playthroughLocations[i][j]);
      if (itemLoc->IsHintable() && *itemLoc->GetAreas().begin() > RA_LINKS_POCKET &&
          !(IsBeatableWithout(ctx->playthroughLocations[i][j], true))) {
        itemLoc->SetWothCandidate();
      }
    }
  }
  ctx->playthroughBeatable = true;
  logic->Reset();
  ReachabilitySearch(ctx->allLocations);
}

static bool FindIfBarren(Rando::ItemLocation* itemLoc, std::array<bool, RA_MAX> NotBarren){
  std::set<RandomizerArea> locAreas = itemLoc->GetAreas();
  for (auto locArea: locAreas){
    if (NotBarren[locArea]){
      return false;
    }
  }
  return true;
}

//Calculate barren locations and assign Barren Candidacy to all locations inside those areas
static void CalculateBarren() {
  auto ctx = Rando::Context::GetInstance();
  std::array<bool, RA_MAX> NotBarren = {}; //I would invert this but the "initialise all as true" syntax wasn't working
  //Isolated Areas and Link's Pocket are never Hinted Barren
  NotBarren[RA_NONE] = true;
  NotBarren[RA_LINKS_POCKET] = true; 

  for (RandomizerCheck loc : ctx->allLocations) {
    Rando::ItemLocation* itemLoc = ctx->GetItemLocation(loc);
    std::set<RandomizerArea> locAreas = itemLoc->GetAreas();
    for (auto locArea: locAreas){
      // If a location has a major item or is a way of the hero location, it is not barren
      if (NotBarren[locArea] == false && (itemLoc->GetPlacedItem().IsMajorItem() || itemLoc->IsWothCandidate())) {
        NotBarren[locArea] = true;
      } 
    }
  }

  for (RandomizerCheck loc : ctx->allLocations) {
    Rando::ItemLocation* itemLoc = ctx->GetItemLocation(loc);
    if (FindIfBarren(itemLoc, NotBarren)){
      itemLoc->SetBarrenCandidate();
    }
  }
}

//Will place things completely randomly, no logic checks are performed
static void FastFill(std::vector<RandomizerGet> items, std::vector<RandomizerCheck> locations, bool endOnItemsEmpty = false) {
  auto ctx = Rando::Context::GetInstance();
  //Loop until locations are empty, or also end if items are empty and the parameters specify to end then
  while (!locations.empty() && (!endOnItemsEmpty || !items.empty())) {
    if (items.empty() && !endOnItemsEmpty) {
      items.push_back(GetJunkItem());
    }

    RandomizerCheck loc = RandomElement(locations, true);
    ctx->GetItemLocation(loc)->SetAsHintable();
    ctx->PlaceItemInLocation(loc, RandomElement(items, true));
  }
}

/*
| The algorithm places items in the world in reverse.
| This means we first assume we have every item in the item pool and
| remove an item and try to place it somewhere that is still reachable
| This method helps distribution of items locked behind many requirements.
| - OoT Randomizer
*/
static void AssumedFill(const std::vector<RandomizerGet>& items, const std::vector<RandomizerCheck>& allowedLocations,
                        bool setLocationsAsHintable = false) {
    auto ctx = Rando::Context::GetInstance();
    if (items.size() > allowedLocations.size()) {
        SPDLOG_ERROR("ERROR: MORE ITEMS THAN LOCATIONS IN GIVEN LISTS");
        SPDLOG_DEBUG("Items:\n");
        // NOLINTNEXTLINE(clang-diagnostic-unused-variable)
        for (const RandomizerGet item : items) {
            SPDLOG_DEBUG("\t");
            SPDLOG_DEBUG(Rando::StaticData::RetrieveItem(item).GetName().GetEnglish());
            SPDLOG_DEBUG("\n");
        }
        SPDLOG_DEBUG("\nAllowed Locations:\n");
        // NOLINTNEXTLINE(clang-diagnostic-unused-variable)
        for (const RandomizerCheck loc : allowedLocations) {
            SPDLOG_DEBUG("\t");
            SPDLOG_DEBUG(Rando::StaticData::GetLocation(loc)->GetName());
            SPDLOG_DEBUG("\n");
        }
        placementFailure = true;
        return;
    }

    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_NO_LOGIC)) {
        FastFill(items, GetEmptyLocations(allowedLocations), true);
        return;
    }

    // keep retrying to place everything until it works or takes too long
    int retries = 10;
    bool unsuccessfulPlacement = false;
    std::vector<RandomizerCheck> attemptedLocations;
    do {
        retries--;
        if (retries <= 0) {
            placementFailure = true;
            return;
        }
        unsuccessfulPlacement = false;
        std::vector<RandomizerGet> itemsToPlace = items;

        // copy all not yet placed advancement items so that we can apply their effects for the fill algorithm
        std::vector<RandomizerGet> itemsToNotPlace =
            FilterFromPool(ItemPool, [](const auto i) { return Rando::StaticData::RetrieveItem(i).IsAdvancement(); });

        // shuffle the order of items to place
        Shuffle(itemsToPlace);
        while (!itemsToPlace.empty()) {
            RandomizerGet item = std::move(itemsToPlace.back());
            Rando::StaticData::RetrieveItem(item).SetAsPlaythrough();
            itemsToPlace.pop_back();

            // assume we have all unplaced items
            logic->Reset();
            for (RandomizerGet unplacedItem : itemsToPlace) {
                Rando::StaticData::RetrieveItem(unplacedItem).ApplyEffect();
            }
            for (RandomizerGet unplacedItem : itemsToNotPlace) {
                Rando::StaticData::RetrieveItem(unplacedItem).ApplyEffect();
            }

            // get all accessible locations that are allowed
            const std::vector<RandomizerCheck> accessibleLocations = ReachabilitySearch(allowedLocations);

            // retry if there are no more locations to place items
            if (accessibleLocations.empty()) {

                SPDLOG_DEBUG("\nCANNOT PLACE ");
                SPDLOG_DEBUG(Rando::StaticData::RetrieveItem(item).GetName().GetEnglish());
                SPDLOG_DEBUG(". TRYING AGAIN...\n");

#ifdef ENABLE_DEBUG
                Regions::DumpWorldGraph(Rando::StaticData::RetrieveItem(item).GetName().GetEnglish());
                PlacementLog_Write();
#endif

                // reset any locations that got an item
                for (RandomizerCheck loc : attemptedLocations) {
                    ctx->GetItemLocation(loc)->SetPlacedItem(RG_NONE);
                    //itemsPlaced--;
                }
                attemptedLocations.clear();

                unsuccessfulPlacement = true;
                break;
            }

            // place the item within one of the allowed locations
            RandomizerCheck selectedLocation = RandomElement(accessibleLocations);
            ctx->PlaceItemInLocation(selectedLocation, item);
            attemptedLocations.push_back(selectedLocation);

            // This tells us the location went through the randomization algorithm
            // to distinguish it from locations which did not or that the player already
            // knows
            if (setLocationsAsHintable) {
                ctx->GetItemLocation(selectedLocation)->SetAsHintable();
            }

            // If ALR is off, then we check beatability after placing the item.
            // If the game is beatable, then we can stop placing items with logic.
            if (!ctx->GetOption(RSK_ALL_LOCATIONS_REACHABLE)) {
                ctx->playthroughBeatable = false;
                logic->Reset();
                CheckBeatable();
                if (ctx->playthroughBeatable) {
                    SPDLOG_DEBUG("Game beatable, now placing items randomly. " + std::to_string(itemsToPlace.size()) +
                                " major items remaining.\n\n");
                    FastFill(itemsToPlace, GetEmptyLocations(allowedLocations), true);
                    return;
                }
            }
        }
    } while (unsuccessfulPlacement);
}

//This function will specifically randomize dungeon rewards for the End of Dungeons
//setting, or randomize one dungeon reward to Link's Pocket if that setting is on
static void RandomizeDungeonRewards() {
  auto ctx = Rando::Context::GetInstance();
  std::array<uint32_t, 9> rDungeonRewardOverrides{};
  //quest item bit mask of each stone/medallion for the savefile
  // static constexpr std::array<uint32_t, 9> bitMaskTable = {
  //   0x00040000, //Kokiri Emerald
  //   0x00080000, //Goron Ruby
  //   0x00100000, //Zora Sapphire
  //   0x00000001, //Forest Medallion
  //   0x00000002, //Fire Medallion
  //   0x00000004, //Water Medallion
  //   0x00000008, //Spirit Medallion
  //   0x00000010, //Shadow Medallion
  //   0x00000020, //Light Medallion
  // };
  int baseOffset = Rando::StaticData::RetrieveItem(RG_KOKIRI_EMERALD).GetItemID();

  //End of Dungeons includes Link's Pocket
  if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
    //get stones and medallions
    std::vector<RandomizerGet> rewards = FilterAndEraseFromPool(ItemPool, [](const auto i) {return Rando::StaticData::RetrieveItem(i).GetItemType() == ITEMTYPE_DUNGEONREWARD;});

    // If there are less than 9 dungeon rewards, prioritize the actual dungeons
    // for placement instead of Link's Pocket
    if (rewards.size() < 9) {
      ctx->PlaceItemInLocation(RC_LINKS_POCKET, RG_GREEN_RUPEE);
    }

    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) { //Place dungeon rewards in vanilla locations
      for (RandomizerCheck loc : Rando::StaticData::dungeonRewardLocations) {
        ctx->GetItemLocation(loc)->PlaceVanillaItem();
      }
    } else { //Randomize dungeon rewards with assumed fill
      AssumedFill(rewards, Rando::StaticData::dungeonRewardLocations);
    }

    for (size_t i = 0; i < Rando::StaticData::dungeonRewardLocations.size(); i++) {
      const auto index = ctx->GetItemLocation(Rando::StaticData::dungeonRewardLocations[i])->GetPlacedItem().GetItemID() - baseOffset;
      rDungeonRewardOverrides[i] = index;

      //set the player's dungeon reward on file creation instead of pushing it to them at the start.
      //This is done mainly because players are already familiar with seeing their dungeon reward
      //before opening up their file
      // if (i == Rando::StaticData::dungeonRewardLocations.size()-1) {
      //   LinksPocketRewardBitMask = bitMaskTable[index];
      // }
    }
  } else if (ctx->GetOption(RSK_LINKS_POCKET).Is(RO_LINKS_POCKET_DUNGEON_REWARD)) {
    //get 1 stone/medallion
    std::vector<RandomizerGet> rewards = FilterFromPool(
        ItemPool, [](const auto i) { return Rando::StaticData::RetrieveItem(i).GetItemType() == ITEMTYPE_DUNGEONREWARD; });
    // If there are no remaining stones/medallions, then Link's pocket won't get one
    if (rewards.empty()) {
      ctx->PlaceItemInLocation(RC_LINKS_POCKET, RG_GREEN_RUPEE);
      return;
    }
    RandomizerGet startingReward = RandomElement(rewards, true);

    //LinksPocketRewardBitMask = bitMaskTable[Rando::StaticData::RetrieveItem(startingReward).GetItemID() - baseOffset];
    ctx->PlaceItemInLocation(RC_LINKS_POCKET, startingReward);
    //erase the stone/medallion from the Item Pool
    FilterAndEraseFromPool(ItemPool, [startingReward](const RandomizerGet i) {return i == startingReward;});
  }
}

//Fills any locations excluded by the player with junk items so that advancement items
//can't be placed there.
static void FillExcludedLocations() {
  auto ctx = Rando::Context::GetInstance();
  //Only fill in excluded locations that don't already have something and are forbidden
  std::vector<RandomizerCheck> excludedLocations = FilterFromPool(ctx->allLocations, [ctx](const auto loc){
    return ctx->GetItemLocation(loc)->IsExcluded();
  });

  for (RandomizerCheck loc : excludedLocations) {
    PlaceJunkInExcludedLocation(loc);
  }
}

//Function to handle the Own Dungeon setting
static void RandomizeOwnDungeon(const Rando::DungeonInfo* dungeon) {
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerGet> dungeonItems;

  // Search and filter for locations that match the hint region of the dungeon
  // This accounts for boss room shuffle so that own dungeon items can be placed
  // in the shuffled boss room
  std::vector<RandomizerCheck> dungeonLocations = FilterFromPool(ctx->allLocations, [dungeon, ctx](const auto loc) {
    return ctx->GetItemLocation(loc)->GetAreas().contains(dungeon->GetArea());
  });

  //filter out locations that may be required to have songs placed at them
  dungeonLocations = FilterFromPool(dungeonLocations, [ctx](const auto loc) {
    if (ctx->GetOption(RSK_SHUFFLE_SONGS).Is(RO_SONG_SHUFFLE_SONG_LOCATIONS)) {
      return !(Rando::StaticData::GetLocation(loc)->GetRCType() == RCTYPE_SONG_LOCATION);
    }
    if (ctx->GetOption(RSK_SHUFFLE_SONGS).Is(RO_SONG_SHUFFLE_DUNGEON_REWARDS)) {
      return !(Rando::StaticData::GetLocation(loc)->GetRCType() == RCTYPE_BOSS_HEART_OR_OTHER_REWARD ||
             loc == RC_SHEIK_IN_ICE_CAVERN ||
             loc == RC_SONG_FROM_IMPA);
    }
    return true;
  });

  //Add specific items that need be randomized within this dungeon
  if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) && dungeon->GetSmallKey() != RG_NONE) {
    std::vector<RandomizerGet> dungeonSmallKeys = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){ return (i == dungeon->GetSmallKey()) || (i == dungeon->GetKeyRing());});
    AddElementsToPool(dungeonItems, dungeonSmallKeys);
  }

  if ((ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) && dungeon->GetBossKey() != RG_GANONS_CASTLE_BOSS_KEY) ||
      (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OWN_DUNGEON) && dungeon->GetBossKey() == RG_GANONS_CASTLE_BOSS_KEY)) {
        auto dungeonBossKey = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){ return i == dungeon->GetBossKey();});
        AddElementsToPool(dungeonItems, dungeonBossKey);
  }

  //randomize boss key and small keys together for even distribution
  AssumedFill(dungeonItems, dungeonLocations);

  //randomize map and compass separately since they're not progressive
  if (ctx->GetOption(RSK_SHUFFLE_MAPANDCOMPASS).Is(RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) && dungeon->GetMap() != RG_NONE && dungeon->GetCompass() != RG_NONE) {
    auto dungeonMapAndCompass = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){ return i == dungeon->GetMap() || i == dungeon->GetCompass();});
    AssumedFill(dungeonMapAndCompass, dungeonLocations);
  }
}

/*Randomize items restricted to a certain set of locations.
  The fill order of location groups is as follows:
    - Own Dungeon
    - Any Dungeon
    - Overworld
  Small Keys, Gerudo Keys, Boss Keys, Ganon's Boss Key, and/or dungeon rewards
  will be randomized together if they have the same setting. Maps and Compasses
  are randomized separately once the dungeon advancement items have all been placed.*/
static void RandomizeDungeonItems() {
  auto ctx = Rando::Context::GetInstance();

  //Get Any Dungeon and Overworld group locations
  std::vector<RandomizerCheck> anyDungeonLocations = Rando::StaticData::GetAllDungeonLocations();

  //Create Any Dungeon and Overworld item pools
  std::vector<RandomizerGet> anyDungeonItems;
  std::vector<RandomizerGet> overworldItems;

  for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
    if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANY_DUNGEON)) {
      auto dungeonKeys = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){return (i == dungeon->GetSmallKey()) || (i == dungeon->GetKeyRing());});
      AddElementsToPool(anyDungeonItems, dungeonKeys);
    } else if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OVERWORLD)) {
      auto dungeonKeys = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){return (i == dungeon->GetSmallKey()) || (i == dungeon->GetKeyRing());});
      AddElementsToPool(overworldItems, dungeonKeys);
    }

    if (ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANY_DUNGEON) && dungeon->GetBossKey() != RG_GANONS_CASTLE_BOSS_KEY) {
      auto bossKey = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){return i == dungeon->GetBossKey();});
      AddElementsToPool(anyDungeonItems, bossKey);
    } else if (ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OVERWORLD) && dungeon->GetBossKey() != RG_GANONS_CASTLE_BOSS_KEY) {
      auto bossKey = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){return i == dungeon->GetBossKey();});
      AddElementsToPool(overworldItems, bossKey);
    }

    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANY_DUNGEON)) {
      auto ganonBossKey = FilterAndEraseFromPool(ItemPool, [](const auto i){return i == RG_GANONS_CASTLE_BOSS_KEY;});
      AddElementsToPool(anyDungeonItems, ganonBossKey);
    } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OVERWORLD)) {
        auto ganonBossKey = FilterAndEraseFromPool(ItemPool, [](const auto i) { return i == RG_GANONS_CASTLE_BOSS_KEY; });
      AddElementsToPool(overworldItems, ganonBossKey);
    }
  }

  if (ctx->GetOption(RSK_GERUDO_KEYS).Is(RO_GERUDO_KEYS_ANY_DUNGEON)) {
      auto gerudoKeys = FilterAndEraseFromPool(ItemPool, [](const auto i) { return i == RG_GERUDO_FORTRESS_SMALL_KEY || i == RG_GERUDO_FORTRESS_KEY_RING; });
    AddElementsToPool(anyDungeonItems, gerudoKeys);
  } else if (ctx->GetOption(RSK_GERUDO_KEYS).Is(RO_GERUDO_KEYS_OVERWORLD)) {
      auto gerudoKeys = FilterAndEraseFromPool(ItemPool, [](const auto i) { return i == RG_GERUDO_FORTRESS_SMALL_KEY || i == RG_GERUDO_FORTRESS_KEY_RING; });
    AddElementsToPool(overworldItems, gerudoKeys);
  }

  if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_ANY_DUNGEON)) {
      auto rewards = FilterAndEraseFromPool(
          ItemPool, [](const auto i) { return Rando::StaticData::RetrieveItem(i).GetItemType() == ITEMTYPE_DUNGEONREWARD; });
    AddElementsToPool(anyDungeonItems, rewards);
  } else if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_OVERWORLD)) {
      auto rewards = FilterAndEraseFromPool(
          ItemPool, [](const auto i) { return Rando::StaticData::RetrieveItem(i).GetItemType() == ITEMTYPE_DUNGEONREWARD; });
    AddElementsToPool(overworldItems, rewards);
  }

  //Randomize Any Dungeon and Overworld pools
  AssumedFill(anyDungeonItems, anyDungeonLocations, true);
  AssumedFill(overworldItems, Rando::StaticData::GetOverworldLocations(), true);

  //Randomize maps and compasses after since they're not advancement items
  for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
    if (ctx->GetOption(RSK_SHUFFLE_MAPANDCOMPASS).Is(RO_DUNGEON_ITEM_LOC_ANY_DUNGEON)) {
      auto mapAndCompassItems = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){return i == dungeon->GetMap() || i == dungeon->GetCompass();});
      AssumedFill(mapAndCompassItems, anyDungeonLocations, true);
    } else if (ctx->GetOption(RSK_SHUFFLE_MAPANDCOMPASS).Is(RO_DUNGEON_ITEM_LOC_OVERWORLD)) {
      auto mapAndCompassItems = FilterAndEraseFromPool(ItemPool, [dungeon](const RandomizerGet i){return i == dungeon->GetMap() || i == dungeon->GetCompass();});
      AssumedFill(mapAndCompassItems, Rando::StaticData::GetOverworldLocations(), true);
    }
  }
}

static void RandomizeLinksPocket() {
  auto ctx = Rando::Context::GetInstance();
  if (ctx->GetOption(RSK_LINKS_POCKET).Is(RO_LINKS_POCKET_ADVANCEMENT)) {
   //Get all the advancement items                                                                                                     don't include tokens
      std::vector<RandomizerGet> advancementItems = FilterAndEraseFromPool(ItemPool, [](const auto i) {
          return Rando::StaticData::RetrieveItem(i).IsAdvancement() && Rando::StaticData::RetrieveItem(i).GetItemType() != ITEMTYPE_TOKEN;
      });
   //select a random one
   RandomizerGet startingItem = RandomElement(advancementItems, true);
   //add the others back
   AddElementsToPool(ItemPool, advancementItems);

   ctx->PlaceItemInLocation(RC_LINKS_POCKET, startingItem);
 } else if (ctx->GetOption(RSK_LINKS_POCKET).Is(RO_LINKS_POCKET_NOTHING)) {
   ctx->PlaceItemInLocation(RC_LINKS_POCKET, RG_GREEN_RUPEE);
 }
}

void VanillaFill() {
  auto ctx = Rando::Context::GetInstance();
  //Perform minimum needed initialization
  RegionTable_Init();
  ctx->GenerateLocationPool();
  GenerateItemPool();
  GenerateStartingInventory();
  //Place vanilla item in each location
  RandomizeDungeonRewards();
  for (RandomizerCheck loc : ctx->allLocations) {
    ctx->GetItemLocation(loc)->PlaceVanillaItem();
  }
  //If necessary, handle ER stuff
  if (ctx->GetOption(RSK_SHUFFLE_ENTRANCES)) {
    SPDLOG_INFO("Shuffling Entrances...");
    ctx->GetEntranceShuffler()->ShuffleAllEntrances();
    SPDLOG_INFO("Shuffling Entrances Done");
  }
  // Populate the playthrough for entrances so they are placed in the spoiler log
  GeneratePlaythrough();
  //Finish up
  ctx->CreateItemOverrides();
  ctx->GetEntranceShuffler()->CreateEntranceOverrides();
  CreateWarpSongTexts();
}

void ClearProgress() {
}

int Fill() {
  auto ctx = Rando::Context::GetInstance();
  int retries = 0;
  SPDLOG_INFO("Starting seed generation...");
  while(retries < 5) {
    SPDLOG_INFO("Attempt {}...", retries + 1);
    placementFailure = false;
    //showItemProgress = false;
    ctx->playthroughLocations.clear();
    ctx->GetEntranceShuffler()->playthroughEntrances.clear();
    RegionTable_Init(); //Reset the world graph to intialize the proper locations
    ctx->ItemReset(); //Reset shops incase of shopsanity random
    ctx->GenerateLocationPool();
    GenerateItemPool();
    GenerateStartingInventory();
    RemoveStartingItemsFromPool();
    FillExcludedLocations();

    //Temporarily add shop items to the ItemPool so that entrance randomization
    //can validate the world using deku/hylian shields
    StartPerformanceTimer(PT_ENTRANCE_SHUFFLE);
    AddElementsToPool(ItemPool, GetMinVanillaShopItems(8)); //assume worst case shopsanity 7
    if (ctx->GetOption(RSK_SHUFFLE_ENTRANCES)) {
      SPDLOG_INFO("Shuffling Entrances...");
      if (ctx->GetEntranceShuffler()->ShuffleAllEntrances() == ENTRANCE_SHUFFLE_FAILURE) {
        retries++;
        ClearProgress();
        continue;
      }
      SPDLOG_INFO("Shuffling Entrances Done");
    }
    SetAreas();
    //erase temporary shop items
    FilterAndEraseFromPool(ItemPool, [](const auto item) { return Rando::StaticData::RetrieveItem(item).GetItemType() == ITEMTYPE_SHOP; });
    StopPerformanceTimer(PT_ENTRANCE_SHUFFLE);

    //ctx->showItemProgress = true;
    //Place shop items first, since a buy shield is needed to place a dungeon reward on Gohma due to access

    StartPerformanceTimer(PT_SHOPSANITY);
    if (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_OFF)) {
      SPDLOG_INFO("Placing Vanilla Shop Items...");
      PlaceVanillaShopItems(); //Place vanilla shop items in vanilla location
    } else {
      SPDLOG_INFO("Shuffling Shop Items");
      int total_replaced = 0;
      if (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_RANDOM) || ctx->GetOption(RSK_SHOPSANITY_COUNT).IsNot(RO_SHOPSANITY_COUNT_ZERO_ITEMS)) { //Shopsanity 1-7, random
        /*
        Indices from OoTR. So shopsanity one will overwrite 7, three will overwrite 7, 5, 8, etc.
          8 6    2 4
          7 5    1 3
        */
        const std::array<int, 8> indices = { 7, 5, 8, 6, 3, 1, 4, 2 };
        //Overwrite appropriate number of shop items
        #define LOCATIONS_PER_SHOP 8
        for (size_t i = 0; i < Rando::StaticData::GetShopLocations().size() / LOCATIONS_PER_SHOP; i++) {
          int num_to_replace = GetShopsanityReplaceAmount(); //1-7 shop items will be overwritten, depending on settings
          total_replaced += num_to_replace;
          for (int j = 0; j < num_to_replace; j++) {
            int itemindex = indices[j];
            RandomizerCheck rc = Rando::StaticData::GetShopLocations()[i * LOCATIONS_PER_SHOP + itemindex - 1];
            Rando::ItemLocation* itemLoc = ctx->GetItemLocation(rc);
            uint16_t shopsanityPrice = GetRandomPrice(Rando::StaticData::GetLocation(rc), shopsanityPrices);
            itemLoc->SetCustomPrice(shopsanityPrice);
          }
        }
        #undef LOCATIONS_PER_SHOP
      }
      //Get all locations and items that don't have a shopsanity price attached
      std::vector<RandomizerCheck> shopLocations = {};
      //Get as many vanilla shop items as the total number of shop items minus the number of replaced items
      //So shopsanity 0 will get all 64 vanilla items, shopsanity 4 will get 32, etc.
      std::vector<RandomizerGet> shopItems = GetMinVanillaShopItems(total_replaced);

      for (RandomizerCheck& randomizerCheck : Rando::StaticData::GetShopLocations()) {
        if (!(ctx->GetItemLocation(randomizerCheck)->HasCustomPrice())) {
          shopLocations.push_back(randomizerCheck);
        }
      }
      //Place the shop items which will still be at shop locations
      AssumedFill(shopItems, shopLocations);
    }

    //Add prices to scrubs
    auto scrubLoc = Rando::StaticData::GetScrubLocations();
    if (ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_ALL)) {
      for (size_t i = 0; i < scrubLoc.size(); i++) {
        ctx->GetItemLocation(scrubLoc[i])->SetCustomPrice(
          GetRandomPrice(Rando::StaticData::GetLocation(scrubLoc[i]), scrubPrices)
        );
      }
    } else {
      for (size_t i = 0; i < scrubLoc.size(); i++) {
        ctx->GetItemLocation(scrubLoc[i])->SetCustomPrice(
          Rando::StaticData::GetLocation(scrubLoc[i])->GetVanillaPrice()
        );
      }
    }

    //set merchant prices
    if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_BEANS_ONLY) ||
        ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL)){
      ctx->GetItemLocation(RC_ZR_MAGIC_BEAN_SALESMAN)->SetCustomPrice(
        GetRandomPrice(Rando::StaticData::GetLocation(RC_ZR_MAGIC_BEAN_SALESMAN), merchantPrices)
      );
    } else {
      ctx->GetItemLocation(RC_ZR_MAGIC_BEAN_SALESMAN)->SetCustomPrice(
        Rando::StaticData::GetLocation(RC_ZR_MAGIC_BEAN_SALESMAN)->GetVanillaPrice()
      );
    }

    auto merchantLoc = Rando::StaticData::GetMerchantLocations();

    if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
        ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL)){
      for (size_t i = 0; i < merchantLoc.size(); i++) {
        ctx->GetItemLocation(merchantLoc[i])->SetCustomPrice(
          GetRandomPrice(Rando::StaticData::GetLocation(merchantLoc[i]), merchantPrices)
        );
      }
    } else {
      for (size_t i = 0; i < merchantLoc.size(); i++) {
        ctx->GetItemLocation(merchantLoc[i])->SetCustomPrice(
          Rando::StaticData::GetLocation(merchantLoc[i])->GetVanillaPrice()
        );
      }
    }
    StopPerformanceTimer(PT_SHOPSANITY);

    StartPerformanceTimer(PT_OWN_DUNGEON);
    //Place dungeon rewards
    SPDLOG_INFO("Shuffling and Placing Dungeon Items...");
    RandomizeDungeonRewards();

    //Place dungeon items restricted to their Own Dungeon
    for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
      RandomizeOwnDungeon(dungeon);
    }
    StopPerformanceTimer(PT_OWN_DUNGEON);

    StartPerformanceTimer(PT_LIMITED_CHECKS);
    //Then Place songs if song shuffle is set to specific locations
    if (ctx->GetOption(RSK_SHUFFLE_SONGS).IsNot(RO_SONG_SHUFFLE_ANYWHERE)) {

      //Get each song
      std::vector<RandomizerGet> songs = FilterAndEraseFromPool(
          ItemPool, [](const auto i) { return Rando::StaticData::RetrieveItem(i).GetItemType() == ITEMTYPE_SONG; });

      //Get each song location
      std::vector<RandomizerCheck> songLocations;
      if (ctx->GetOption(RSK_SHUFFLE_SONGS).Is(RO_SONG_SHUFFLE_SONG_LOCATIONS)) {
          songLocations = FilterFromPool(
              ctx->allLocations, [](const auto loc) { return Rando::StaticData::GetLocation(loc)->GetRCType() == RCTYPE_SONG_LOCATION; });

      } else if (ctx->GetOption(RSK_SHUFFLE_SONGS).Is(RO_SONG_SHUFFLE_DUNGEON_REWARDS)) {
          songLocations = FilterFromPool(ctx->allLocations, [](const auto loc) {
            return Rando::StaticData::GetLocation(loc)->GetRCType() == RCTYPE_BOSS_HEART_OR_OTHER_REWARD ||
                   loc == RC_SHEIK_IN_ICE_CAVERN ||
                   loc == RC_SONG_FROM_IMPA;
          });
      }

      AssumedFill(songs, songLocations, true);
    }

    //Then place dungeon items that are assigned to restrictive location pools
    RandomizeDungeonItems();
    SPDLOG_INFO("Dungeon Items Done");

    //Then place Link's Pocket Item if it has to be an advancement item
    RandomizeLinksPocket();
    StopPerformanceTimer(PT_LIMITED_CHECKS);


    StartPerformanceTimer(PT_ADVANCEMENT_ITEMS);
    SPDLOG_INFO("Shuffling Advancement Items");
    //Then place the rest of the advancement items
    std::vector<RandomizerGet> remainingAdvancementItems =
        FilterAndEraseFromPool(ItemPool, [](const auto i) { return Rando::StaticData::RetrieveItem(i).IsAdvancement(); });
    AssumedFill(remainingAdvancementItems, ctx->allLocations, true);
    StopPerformanceTimer(PT_ADVANCEMENT_ITEMS);

    StartPerformanceTimer(PT_REMAINING_ITEMS);
    //Fast fill for the rest of the pool
    SPDLOG_INFO("Shuffling Remaining Items");
    std::vector<RandomizerGet> remainingPool = FilterAndEraseFromPool(ItemPool, [](const auto i) { return true; });
    FastFill(remainingPool, GetAllEmptyLocations(), false);
    StopPerformanceTimer(PT_REMAINING_ITEMS);


    StartPerformanceTimer(PT_PLAYTHROUGH_GENERATION);
    GeneratePlaythrough();
    StopPerformanceTimer(PT_PLAYTHROUGH_GENERATION);
    //Successful placement, produced beatable result
    if(ctx->playthroughBeatable && !placementFailure) {

      SPDLOG_INFO("Calculating Playthrough...");
      StartPerformanceTimer(PT_PARE_DOWN_PLAYTHROUGH);
      PareDownPlaythrough();
      StopPerformanceTimer(PT_PARE_DOWN_PLAYTHROUGH);

      StartPerformanceTimer(PT_WOTH);
      CalculateWotH();
      StopPerformanceTimer(PT_WOTH);

      StartPerformanceTimer(PT_FOOLISH);
      CalculateBarren(); 
      StopPerformanceTimer(PT_FOOLISH);
      SPDLOG_INFO("Calculating Playthrough Done");

      StartPerformanceTimer(PT_OVERRIDES);
      ctx->CreateItemOverrides();
      ctx->GetEntranceShuffler()->CreateEntranceOverrides();
      StopPerformanceTimer(PT_OVERRIDES);

      StartPerformanceTimer(PT_HINTS);
      CreateAllHints();
      CreateWarpSongTexts();
      StopPerformanceTimer(PT_HINTS);
      SPDLOG_DEBUG("Number of retries {}", retries);
      return 1;
    }
    //Unsuccessful placement
    if(retries < 4) {
      SPDLOG_DEBUG("Failed to generate a beatable seed. Retrying...");
      Regions::ResetAllLocations();
      logic->Reset();
      ClearProgress();
    }
    retries++;
  }
  //All retries failed
  return -1;
}

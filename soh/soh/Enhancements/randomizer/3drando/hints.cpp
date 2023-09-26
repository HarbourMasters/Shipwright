#include "hints.hpp"

#include "custom_messages.hpp"
#include "dungeon.hpp"
#include "item_location.hpp"
#include "item_pool.hpp"
#include "logic.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "fill.hpp"
#include "hint_list.hpp"
#include "trial.hpp"
#include "entrance.hpp"
#include "z64item.h"
#include <spdlog/spdlog.h>
#include "../randomizerTypes.h"

using namespace CustomMessages;
using namespace Logic;
using namespace Settings;
using namespace Trial;

std::unordered_map<HintType, std::string> hintTypeNames = {
    { HINT_TYPE_TRIAL, "Trial" },
    { HINT_TYPE_ALWAYS, "Always" },
    { HINT_TYPE_WOTH, "WotH" },
    { HINT_TYPE_BARREN, "Barren" },
    { HINT_TYPE_ENTRANCE, "Entrance" },
    { HINT_TYPE_SOMETIMES, "Sometimes" },
    { HINT_TYPE_RANDOM, "Random"},
    { HINT_TYPE_ITEM, "Item" },
    { HINT_TYPE_SONG, "Song" },
    { HINT_TYPE_OVERWORLD, "Overworld" },
    { HINT_TYPE_DUNGEON, "Dungeon" },
    { HINT_TYPE_JUNK, "Junk" },
    { HINT_TYPE_NAMED_ITEM, "NamedItem" },
};

constexpr std::array<HintSetting, 4> hintSettingTable{{
  // Useless hints
  {
    .dungeonsWothLimit = 2,
    .dungeonsBarrenLimit = 1,
    .namedItemsRequired = false,
    .distTable = {{
      {.type = HINT_TYPE_TRIAL,     .order =  1, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_ALWAYS,    .order =  2, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_WOTH,      .order =  3, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_BARREN,    .order =  4, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_ENTRANCE,  .order =  5, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_SOMETIMES, .order =  6, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_RANDOM,    .order =  7, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_ITEM,      .order =  8, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_SONG,      .order =  9, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_OVERWORLD, .order = 10, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_DUNGEON,   .order = 11, .weight =  0, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_JUNK,      .order = 12, .weight = 99, .fixed = 0, .copies = 0},
      {.type = HINT_TYPE_NAMED_ITEM, .order = 13, .weight =  0, .fixed = 0, .copies = 0},
    }},
  },

  // Balanced hints
  {
    .dungeonsWothLimit = 2,
    .dungeonsBarrenLimit = 1,
    .namedItemsRequired = true,
    .distTable = {{
      {.type = HINT_TYPE_TRIAL,     .order =  1, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ALWAYS,    .order =  2, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_WOTH,      .order =  3, .weight =  7, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_BARREN,    .order =  4, .weight =  4, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ENTRANCE,  .order =  5, .weight =  6, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_SOMETIMES, .order =  6, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_RANDOM,    .order =  7, .weight = 12, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ITEM,      .order =  8, .weight = 10, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_SONG,      .order =  9, .weight =  2, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_OVERWORLD, .order = 10, .weight =  4, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_DUNGEON,   .order = 11, .weight =  3, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_JUNK,      .order = 12, .weight =  6, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_NAMED_ITEM, .order = 13, .weight =  0, .fixed = 0, .copies = 1},
    }},
  },

  // Strong hints
  {
    .dungeonsWothLimit = 2,
    .dungeonsBarrenLimit = 1,
    .namedItemsRequired = true,
    .distTable = {{
      {.type = HINT_TYPE_TRIAL,     .order =  1, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ALWAYS,    .order =  2, .weight =  0, .fixed = 0, .copies = 2},
      {.type = HINT_TYPE_WOTH,      .order =  3, .weight = 12, .fixed = 0, .copies = 2},
      {.type = HINT_TYPE_BARREN,    .order =  4, .weight = 12, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ENTRANCE,  .order =  5, .weight =  4, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_SOMETIMES, .order =  6, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_RANDOM,    .order =  7, .weight =  8, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ITEM,      .order =  8, .weight =  8, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_SONG,      .order =  9, .weight =  4, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_OVERWORLD, .order = 10, .weight =  6, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_DUNGEON,   .order = 11, .weight =  6, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_JUNK,      .order = 12, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_NAMED_ITEM, .order = 13, .weight =  0, .fixed = 0, .copies = 1},
    }},
  },

  // Very strong hints
  {
    .dungeonsWothLimit = 40,
    .dungeonsBarrenLimit = 40,
    .namedItemsRequired = true,
    .distTable = {{
      {.type = HINT_TYPE_TRIAL,     .order =  1, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ALWAYS,    .order =  2, .weight =  0, .fixed = 0, .copies = 2},
      {.type = HINT_TYPE_WOTH,      .order =  3, .weight = 15, .fixed = 0, .copies = 2},
      {.type = HINT_TYPE_BARREN,    .order =  4, .weight = 15, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ENTRANCE,  .order =  5, .weight = 10, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_SOMETIMES, .order =  6, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_RANDOM,    .order =  7, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_ITEM,      .order =  8, .weight =  5, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_SONG,      .order =  9, .weight =  2, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_OVERWORLD, .order = 10, .weight =  7, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_DUNGEON,   .order = 11, .weight =  7, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_JUNK,      .order = 12, .weight =  0, .fixed = 0, .copies = 1},
      {.type = HINT_TYPE_NAMED_ITEM, .order = 13, .weight =  0, .fixed = 0, .copies = 1},
    }},
  },
}};

std::array<DungeonInfo, 10> dungeonInfoData;

Text childAltarText;
Text adultAltarText;
Text ganonText;
Text ganonHintText;
Text sheikText;
Text sariaText;
Text dampesText;
Text gregText;
Text warpMinuetText;
Text warpBoleroText;
Text warpSerenadeText;
Text warpRequiemText;
Text warpNocturneText;
Text warpPreludeText;

std::string lightArrowHintLoc;
std::string sariaHintLoc;
std::string dampeHintLoc;

Text& GetChildAltarText() {
  return childAltarText;
}

Text& GetAdultAltarText() {
  return adultAltarText;
}

Text& GetGanonText() {
  return ganonText;
}

Text& GetGanonHintText() {
  return ganonHintText;
}

Text& GetDampeHintText() {
  return dampesText;
}

Text& GetGregHintText() {
  return gregText;
}

Text& GetSheikHintText() {
  return sheikText;
}

Text& GetSariaHintText() {
  return sariaText;
}

Text& GetWarpMinuetText() {
  return warpMinuetText;
}

Text& GetWarpBoleroText() {
  return warpBoleroText;
}

Text& GetWarpSerenadeText() {
  return warpSerenadeText;
}

Text& GetWarpRequiemText() {
  return warpRequiemText;
}

Text& GetWarpNocturneText() {
  return warpNocturneText;
}

Text& GetWarpPreludeText() {
  return warpPreludeText;
}

std::string GetLightArrowHintLoc() {
    return lightArrowHintLoc;
}

std::string GetDampeHintLoc() {
    return dampeHintLoc;
}

std::string GetSariaHintLoc() {
  return sariaHintLoc;
}

Area* GetHintRegion(const uint32_t area) {

  std::vector<uint32_t> alreadyChecked = {};
  std::vector<uint32_t> spotQueue = {area};

  while (!spotQueue.empty()) {
    uint32_t region = spotQueue.back();
    alreadyChecked.push_back(region);
    spotQueue.pop_back();

    if (AreaTable(region)->hintKey != NONE) {
      return AreaTable(region);
    }

    //add unchecked entrances to spot queue
    bool checked = false;
    for (auto& entrance : AreaTable(region)->entrances) {
      for (uint32_t checkedEntrance : alreadyChecked) {
        if (entrance->GetParentRegionKey() == checkedEntrance) {
          checked = true;
          break;
        }
      }

      if (!checked) {
        spotQueue.insert(spotQueue.begin(), entrance->GetParentRegionKey());
      }
    }
  }

  return AreaTable(NONE);
}

uint32_t GetHintRegionHintKey(const uint32_t area) {
    return GetHintRegion(area)->hintKey;
}

uint32_t GetHintRegionuint32_t(const uint32_t area) {
  return GetHintRegion(area)->hintKey;
}

uint32_t GetLocationRegionuint32_t(const uint32_t location) {
  return GetHintRegion(Location(location)->GetParentRegionKey())->hintKey;
}

static std::vector<uint32_t> GetAccessibleGossipStones(const uint32_t hintedLocation = GANON) {
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for gossip stone locations.
  uint32_t originalItem = Location(hintedLocation)->GetPlaceduint32_t();
  Location(hintedLocation)->SetPlacedItem(NONE);

  LogicReset();
  auto accessibleGossipStones = GetAccessibleLocations(gossipStoneLocations);
  //Give the item back to the location
  Location(hintedLocation)->SetPlacedItem(originalItem);

  return accessibleGossipStones;
}

static void AddHint(Text hint, const uint32_t gossipStone, const std::vector<uint8_t>& colors = {}, HintType hintType = HINT_TYPE_ITEM, const uint32_t hintedLocation = NONE) {
  //save hints as dummy items for writing to the spoiler log
  NewItem(gossipStone, Item{RG_HINT, hint, ITEMTYPE_EVENT, GI_RUPEE_BLUE_LOSE, false, &noVariable, NONE});
  Location(gossipStone)->SetPlacedItem(gossipStone);
  Location(gossipStone)->SetHintedLocation(hintedLocation);
  Location(gossipStone)->SetHintType(hintType);
  Location(gossipStone)->SetHintedRegion(GetHintRegion(Location(hintedLocation)->GetParentRegionKey())->GetHint().GetText().GetEnglish());
}

static void CreateLocationHint(const std::vector<uint32_t>& possibleHintLocations) {
  //return if there aren't any hintable locations or gossip stones available
  if (possibleHintLocations.empty()) {
      SPDLOG_DEBUG("\tNO LOCATIONS TO HINT\n\n");
    return;
  }

  uint32_t hintedLocation = RandomElement(possibleHintLocations);
  const std::vector<uint32_t> accessibleGossipStones = GetAccessibleGossipStones(hintedLocation);

  SPDLOG_DEBUG("\tLocation: ");
  SPDLOG_DEBUG(Location(hintedLocation)->GetName());
  SPDLOG_DEBUG("\n");

  SPDLOG_DEBUG("\tItem: ");
  SPDLOG_DEBUG(Location(hintedLocation)->GetPlacedItemName().GetEnglish());
  SPDLOG_DEBUG("\n");

  if (accessibleGossipStones.empty()) {
      SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
    return;
  }

  uint32_t gossipStone = RandomElement(accessibleGossipStones);
  Location(hintedLocation)->SetAsHinted();

  //make hint text
  Text locationHintText = Location(hintedLocation)->GetHint().GetText();
  Text itemHintText = Location(hintedLocation)->GetPlacedItem().GetHint().GetText();
  Text prefix = Hint(PREFIX).GetText();
  
  Text finalHint = prefix + locationHintText + " #"+itemHintText+"#.";
  SPDLOG_DEBUG("\tMessage: ");
  SPDLOG_DEBUG(finalHint.english);
  SPDLOG_DEBUG("\n\n");

  AddHint(finalHint, gossipStone, {QM_GREEN, QM_RED}, HINT_TYPE_ITEM, hintedLocation);
}

static void CreateWothHint(uint8_t* remainingDungeonWothHints) {
    // get locations that are in the current playthrough
    std::vector<uint32_t> possibleHintLocations = {};
    // iterate through playthrough locations by sphere
    std::vector<uint32_t> wothHintLocations =
        FilterFromPool(wothLocations, [remainingDungeonWothHints](uint32_t loc) {
            return Location(loc)->IsHintable() &&    // only filter hintable locations
                   !(Location(loc)->IsHintedAt()) && // only filter locations that haven't been hinted at
                   (Location(loc)->IsOverworld() ||
                    (Location(loc)->IsDungeon() &&
                     (*remainingDungeonWothHints) > 0)); // make sure we haven't surpassed the woth dungeon limit
        });
    AddElementsToPool(possibleHintLocations, wothHintLocations);

    // If no more locations can be hinted at for woth, then just try to get another hint
    if (possibleHintLocations.empty()) {
        SPDLOG_DEBUG("\tNO LOCATIONS TO HINT\n\n");
        return;
    }
    uint32_t hintedLocation = RandomElement(possibleHintLocations);

    SPDLOG_DEBUG("\tLocation: ");
    SPDLOG_DEBUG(Location(hintedLocation)->GetName());
    SPDLOG_DEBUG("\n");

    SPDLOG_DEBUG("\tItem: ");
    SPDLOG_DEBUG(Location(hintedLocation)->GetPlacedItemName().GetEnglish());
    SPDLOG_DEBUG("\n");

    // get an accessible gossip stone
    const std::vector<uint32_t> gossipStoneLocations = GetAccessibleGossipStones(hintedLocation);

    if (gossipStoneLocations.empty()) {
        SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
        return;
    }
    Location(hintedLocation)->SetAsHinted();
    uint32_t gossipStone = RandomElement(gossipStoneLocations);

    if (Location(hintedLocation)->IsDungeon()) {
        *remainingDungeonWothHints -= 1;
    }

    // form hint text
    Text locationText = GetHintRegion(Location(hintedLocation)->GetParentRegionKey())->GetHint().GetText();
    Text finalWothHint = Hint(PREFIX).GetText() + "#" + locationText + "#" + Hint(WAY_OF_THE_HERO).GetText();
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalWothHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalWothHint, gossipStone, { QM_LBLUE }, HINT_TYPE_WOTH, hintedLocation);
}

static void CreateBarrenHint(uint8_t* remainingDungeonBarrenHints, std::vector<uint32_t>& barrenLocations) {
    // remove dungeon locations if necessary
    if (*remainingDungeonBarrenHints < 1) {
        barrenLocations =
            FilterFromPool(barrenLocations, [](const uint32_t loc) { return !(Location(loc)->IsDungeon()); });
    }

    if (barrenLocations.empty()) {
        return;
    }

    uint32_t hintedLocation = RandomElement(barrenLocations, true);

    SPDLOG_DEBUG("\tLocation: ");
    SPDLOG_DEBUG(Location(hintedLocation)->GetName());
    SPDLOG_DEBUG("\n");

    SPDLOG_DEBUG("\tItem: ");
    SPDLOG_DEBUG(Location(hintedLocation)->GetPlacedItemName().GetEnglish());
    SPDLOG_DEBUG("\n");

    // get an accessible gossip stone
    const std::vector<uint32_t> gossipStoneLocations = GetAccessibleGossipStones(hintedLocation);
    if (gossipStoneLocations.empty()) {
        SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
        return;
    }
    Location(hintedLocation)->SetAsHinted();
    uint32_t gossipStone = RandomElement(gossipStoneLocations);

    if (Location(hintedLocation)->IsDungeon()) {
        *remainingDungeonBarrenHints -= 1;
    }

    // form hint text
    Text locationText = GetHintRegion(Location(hintedLocation)->GetParentRegionKey())->GetHint().GetText();
    Text finalBarrenHint =
        Hint(PREFIX).GetText() + Hint(PLUNDERING).GetText() + "#" + locationText + "#" + Hint(FOOLISH).GetText();
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalBarrenHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalBarrenHint, gossipStone, { QM_PINK }, HINT_TYPE_BARREN, hintedLocation);

    // get rid of all other locations in this same barren region
    barrenLocations = FilterFromPool(barrenLocations, [hintedLocation](uint32_t loc) {
        return GetHintRegion(Location(loc)->GetParentRegionKey())->hintKey !=
               GetHintRegion(Location(hintedLocation)->GetParentRegionKey())->hintKey;
    });
}

static void CreateRandomLocationHint(const bool goodItem = false) {
  const std::vector<uint32_t> possibleHintLocations = FilterFromPool(allLocations, [goodItem](const uint32_t loc) {
    return Location(loc)->IsHintable() && !(Location(loc)->IsHintedAt()) && (!goodItem || Location(loc)->GetPlacedItem().IsMajorItem());
  });
  //If no more locations can be hinted at, then just try to get another hint
  if (possibleHintLocations.empty()) {
    SPDLOG_DEBUG("\tNO LOCATIONS TO HINT\n\n");
    return;
  }
  uint32_t hintedLocation = RandomElement(possibleHintLocations);

  SPDLOG_DEBUG("\tLocation: ");
  SPDLOG_DEBUG(Location(hintedLocation)->GetName());
  SPDLOG_DEBUG("\n");

  SPDLOG_DEBUG("\tItem: ");
  SPDLOG_DEBUG(Location(hintedLocation)->GetPlacedItemName().GetEnglish());
  SPDLOG_DEBUG("\n");

  //get an acessible gossip stone
  const std::vector<uint32_t> gossipStoneLocations = GetAccessibleGossipStones(hintedLocation);
  if (gossipStoneLocations.empty()) {
      SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
    return;
  }
  Location(hintedLocation)->SetAsHinted();
  uint32_t gossipStone = RandomElement(gossipStoneLocations);

  //form hint text
  Text itemText = Location(hintedLocation)->GetPlacedItem().GetHint().GetText();
  Text locationText = GetHintRegion(Location(hintedLocation)->GetParentRegionKey())->GetHint().GetText();
  // RANDOTODO: reconsider dungeon vs non-dungeon item location hints when boss shuffle mixed pools happens
  if (Location(hintedLocation)->IsDungeon()) {
    Text finalHint = Hint(PREFIX).GetText()+"#"+locationText+"# "+Hint(HOARDS).GetText()+" #"+itemText+"#.";
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalHint, gossipStone, {QM_GREEN, QM_RED}, HINT_TYPE_NAMED_ITEM, hintedLocation);
  } else {
    Text finalHint = Hint(PREFIX).GetText()+"#"+itemText+"# "+Hint(CAN_BE_FOUND_AT).GetText()+" #"+locationText+"#.";
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalHint, gossipStone, { QM_RED, QM_GREEN }, HINT_TYPE_NAMED_ITEM, hintedLocation);
  }
}

static void CreateGoodItemHint() {
  CreateRandomLocationHint(true);
}

static void CreateJunkHint() {
  //duplicate junk hints are possible for now
  const HintText junkHint = RandomElement(GetHintCategory(HintCategory::Junk));
  LogicReset();
  const std::vector<uint32_t> gossipStones = GetAccessibleLocations(gossipStoneLocations);
  if (gossipStones.empty()) {
      SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
    return;
  }
  uint32_t gossipStone = RandomElement(gossipStones);
  Text hint = junkHint.GetText();

  SPDLOG_DEBUG("\tMessage: ");
  SPDLOG_DEBUG(hint.english);
  SPDLOG_DEBUG("\n\n");

  AddHint(hint, gossipStone, { QM_PINK }, HINT_TYPE_JUNK);
}

static std::vector<uint32_t> CalculateBarrenRegions() {
  std::vector<uint32_t> barrenLocations = {};
  std::vector<uint32_t> potentiallyUsefulLocations = {};

  for (uint32_t loc : allLocations) {
    // If a location has a major item or is a way of the hero location, it is not barren
    if (Location(loc)->GetPlacedItem().IsMajorItem() || ElementInContainer(loc, wothLocations)) {
      AddElementsToPool(potentiallyUsefulLocations, std::vector{loc});
    } else {
      // Link's Pocket & Triforce Hunt "reward" shouldn't be considered for barren areas because it's clear what
      // they have to a player.
      if (loc != LINKS_POCKET && loc != TRIFORCE_COMPLETED) { 
        AddElementsToPool(barrenLocations, std::vector{loc});
      }
    }
  }

  // Leave only locations at barren regions in the list
  auto finalBarrenLocations = FilterFromPool(barrenLocations, [&potentiallyUsefulLocations](uint32_t loc){
    for (uint32_t usefulLoc : potentiallyUsefulLocations) {
      uint32_t barrenKey = GetLocationRegionuint32_t(loc);
      uint32_t usefulKey = GetLocationRegionuint32_t(usefulLoc);
      if (barrenKey == usefulKey) {
        return false;
      }
    }
    return true;
  });

  return finalBarrenLocations;
}

static void CreateTrialHints() {
    //six trials
  if (RandomGanonsTrials && GanonsTrialsCount.Is(6)) {

    //get a random gossip stone
    auto gossipStones = GetAccessibleGossipStones();
    auto gossipStone = RandomElement(gossipStones, false);

    //make hint
    auto hint = Hint(PREFIX).GetText() + Hint(SIX_TRIALS).GetText();
    AddHint(hint, gossipStone, { QM_PINK }, HINT_TYPE_TRIAL);

    //zero trials
  } else if (RandomGanonsTrials && GanonsTrialsCount.Is(0)) {

    //get a random gossip stone
    auto gossipStones = GetAccessibleGossipStones();
    auto gossipStone = RandomElement(gossipStones, false);

    //make hint
    auto hint = Hint(PREFIX).GetText() + Hint(ZERO_TRIALS).GetText();
    AddHint(hint, gossipStone, { QM_YELLOW }, HINT_TYPE_TRIAL);

    //4 or 5 required trials
  } else if (GanonsTrialsCount.Is(5) || GanonsTrialsCount.Is(4)) {

    //get skipped trials
    std::vector<TrialInfo*> trials = {};
    trials.assign(trialList.begin(), trialList.end());
    auto skippedTrials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsSkipped();});

    //create a hint for each skipped trial
    for (auto& trial : skippedTrials) {
      //get a random gossip stone
      auto gossipStones = GetAccessibleGossipStones();
      auto gossipStone = RandomElement(gossipStones, false);

      //make hint
      auto hint = Hint(PREFIX).GetText()+"#"+trial->GetName()+"#"+Hint(FOUR_TO_FIVE_TRIALS).GetText();
      AddHint(hint, gossipStone, { QM_YELLOW }, HINT_TYPE_TRIAL);
    }
    //1 to 3 trials
  } else if (GanonsTrialsCount.Value<uint8_t>() >= 1 && GanonsTrialsCount.Value<uint8_t>() <= 3) {
    //get requried trials
    std::vector<TrialInfo*> trials = {};
    trials.assign(trialList.begin(), trialList.end());
    auto requiredTrials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsRequired();});

    //create a hint for each required trial
    for (auto& trial : requiredTrials) {
      //get a random gossip stone
      auto gossipStones = GetAccessibleGossipStones();
      auto gossipStone = RandomElement(gossipStones, false);

      //make hint
      auto hint = Hint(PREFIX).GetText()+"#"+trial->GetName()+"#"+Hint(ONE_TO_THREE_TRIALS).GetText();
      AddHint(hint, gossipStone, { QM_PINK }, HINT_TYPE_TRIAL);
    }
  }
}

void CreateGanonText() {

  //funny ganon line
  ganonText = RandomElement(GetHintCategory(HintCategory::GanonLine)).GetText();
  CreateMessageFromTextObject(0x70CB, 0, 2, 3, AddColorsAndFormat(ganonText));

  //Get the location of the light arrows
  auto lightArrowLocation = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->GetPlaceduint32_t() == LIGHT_ARROWS;});

  //If there is no light arrow location, it was in the player's inventory at the start
  auto hint = Hint(LIGHT_ARROW_LOCATION_HINT);
  if (lightArrowLocation.empty()) {
    ganonHintText = hint.GetText()+Hint(YOUR_POCKET).GetText();
    lightArrowHintLoc = "Link's Pocket";
  } else {
    ganonHintText = hint.GetText()+GetHintRegion(Location(lightArrowLocation[0])->GetParentRegionKey())->GetHint().GetText();
    lightArrowHintLoc = Location(lightArrowLocation[0])->GetName();
  }
  ganonHintText = ganonHintText + "!";

  CreateMessageFromTextObject(0x70CC, 0, 2, 3, AddColorsAndFormat(ganonHintText));
}

//Find the location which has the given itemKey and create the generic altar text for the reward
static Text BuildDungeonRewardText(const uint32_t itemKey) {
  uint32_t location = FilterFromPool(allLocations, [itemKey](const uint32_t loc){return Location(loc)->GetPlaceduint32_t() == itemKey;})[0];
  Location(location)->SetAsHinted();

  std::string rewardString = "$" + std::to_string(itemKey - KOKIRI_EMERALD);

  // RANDOTODO implement colors for locations
  return Text()+rewardString+GetHintRegion(Location(location)->GetParentRegionKey())->GetHint().GetText().Capitalize()+"...^";
}

static Text BuildDoorOfTimeText() {
  std::string itemObtained;
  Text doorOfTimeText;

  if (OpenDoorOfTime.Is(OPENDOOROFTIME_OPEN)) {
    itemObtained = "$o";
    doorOfTimeText = Hint(CHILD_ALTAR_TEXT_END_DOTOPEN).GetText();

  } else if (OpenDoorOfTime.Is(OPENDOOROFTIME_SONGONLY)) {
    itemObtained = "$c";
    doorOfTimeText = Hint(CHILD_ALTAR_TEXT_END_DOTSONGONLY).GetText();

  } else if (OpenDoorOfTime.Is(OPENDOOROFTIME_CLOSED)) {
    itemObtained = "$i";
    doorOfTimeText = Hint(CHILD_ALTAR_TEXT_END_DOTCLOSED).GetText();
  }

  return Text()+itemObtained+doorOfTimeText;
}

//insert the required number into the hint and set the singular/plural form
static Text BuildCountReq(const uint32_t req, const Option& count) {
  Text requirement = Hint(req).GetTextCopy();
    if (count.Value<uint8_t>() == 1) {
    requirement.SetForm(SINGULAR);
  } else {
    requirement.SetForm(PLURAL);
  }
  requirement.Replace("%d", std::to_string(count.Value<uint8_t>()));
  return requirement;
}

static Text BuildBridgeReqsText() {
  Text bridgeText;

  if (Bridge.Is(RAINBOWBRIDGE_OPEN)) {
    bridgeText = Hint(BRIDGE_OPEN_HINT).GetText();

  } else if (Bridge.Is(RAINBOWBRIDGE_VANILLA)) {
    bridgeText = Hint(BRIDGE_VANILLA_HINT).GetText();

  } else if (Bridge.Is(RAINBOWBRIDGE_STONES)) {
    bridgeText = BuildCountReq(BRIDGE_STONES_HINT, BridgeStoneCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_MEDALLIONS)) {
    bridgeText = BuildCountReq(BRIDGE_MEDALLIONS_HINT, BridgeMedallionCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_REWARDS)) {
    bridgeText = BuildCountReq(BRIDGE_REWARDS_HINT, BridgeRewardCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_DUNGEONS)) {
    bridgeText = BuildCountReq(BRIDGE_DUNGEONS_HINT, BridgeDungeonCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_TOKENS)) {
    bridgeText = BuildCountReq(BRIDGE_TOKENS_HINT, BridgeTokenCount);
  
  } else if (Bridge.Is(RAINBOWBRIDGE_GREG)) {
    bridgeText = Hint(BRIDGE_GREG_HINT).GetText();
  }

  return Text()+"$l"+bridgeText+"^";
}

static Text BuildGanonBossKeyText() {
  Text ganonBossKeyText;

  if (GanonsBossKey.Is(GANONSBOSSKEY_START_WITH)) {
    ganonBossKeyText = Hint(GANON_BK_START_WITH_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_VANILLA)) {
    ganonBossKeyText = Hint(GANON_BK_VANILLA_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_OWN_DUNGEON)) {
    ganonBossKeyText = Hint(GANON_BK_OWN_DUNGEON_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_ANY_DUNGEON)) {
    ganonBossKeyText = Hint(GANON_BK_ANY_DUNGEON_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_OVERWORLD)) {
    ganonBossKeyText = Hint(GANON_BK_OVERWORLD_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_ANYWHERE)) {
    ganonBossKeyText = Hint(GANON_BK_ANYWHERE_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_FINAL_GS_REWARD)) {
    ganonBossKeyText = Hint(GANON_BK_SKULLTULA_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_VANILLA)) {
    ganonBossKeyText = Hint(LACS_VANILLA_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_STONES)) {
    ganonBossKeyText = BuildCountReq(LACS_STONES_HINT, LACSStoneCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_MEDALLIONS)) {
    ganonBossKeyText = BuildCountReq(LACS_MEDALLIONS_HINT, LACSMedallionCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
    ganonBossKeyText = BuildCountReq(LACS_REWARDS_HINT, LACSRewardCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
    ganonBossKeyText = BuildCountReq(LACS_DUNGEONS_HINT, LACSDungeonCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_TOKENS)) {
    ganonBossKeyText = BuildCountReq(LACS_TOKENS_HINT, LACSTokenCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_TRIFORCE_HUNT)) {
    ganonBossKeyText = Hint(GANON_BK_TRIFORCE_HINT).GetText();
  }

  return Text()+"$b"+ganonBossKeyText+"^";
}

void CreateAltarText() {

  //Child Altar Text
  if (AltarHintText) {
    childAltarText = Hint(SPIRITUAL_STONE_TEXT_START).GetText()+"^"+
    //Spiritual Stones
        (StartingKokiriEmerald.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(KOKIRI_EMERALD)) +
        (StartingGoronRuby.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                            : BuildDungeonRewardText(GORON_RUBY)) +
        (StartingZoraSapphire.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                              : BuildDungeonRewardText(ZORA_SAPPHIRE)) +
    //How to open Door of Time, the event trigger is necessary to read the altar multiple times
    BuildDoorOfTimeText();
  } else {
    childAltarText = BuildDoorOfTimeText();
  }

  CreateMessageFromTextObject(0x7040, 0, 2, 3, AddColorsAndFormat(childAltarText, {QM_GREEN, QM_RED, QM_BLUE}));

  //Adult Altar Text
  adultAltarText = Hint(ADULT_ALTAR_TEXT_START).GetText() + "^";
  if (AltarHintText) {
    adultAltarText = adultAltarText +
    //Medallion Areas
        (StartingLightMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(LIGHT_MEDALLION)) +
        (StartingForestMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(FOREST_MEDALLION)) +
        (StartingFireMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(FIRE_MEDALLION)) +
        (StartingWaterMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(WATER_MEDALLION)) +
        (StartingSpiritMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(SPIRIT_MEDALLION)) +
        (StartingShadowMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(SHADOW_MEDALLION));
  }
  adultAltarText = adultAltarText + 
  //Bridge requirement
  BuildBridgeReqsText()+

  //Ganons Boss Key requirement
  BuildGanonBossKeyText()+

  //End
  Hint(ADULT_ALTAR_TEXT_END).GetText();
  CreateMessageFromTextObject(0x7088, 0, 2, 3, AddColorsAndFormat(adultAltarText, {QM_RED, QM_YELLOW, QM_GREEN, QM_RED, QM_BLUE, QM_YELLOW, QM_PINK, QM_RED, QM_RED, QM_RED, QM_RED}));
}

void CreateMerchantsHints() {

  Text medigoronItemText = Location(GC_MEDIGORON)->GetPlacedItem().GetHint().GetText();
  Text grannyItemText = Location(KAK_GRANNYS_SHOP)->GetPlacedItem().GetHint().GetText();
  Text carpetSalesmanItemText = Location(WASTELAND_BOMBCHU_SALESMAN)->GetPlacedItem().GetHint().GetText();
  Text carpetSalesmanItemClearText = Location(WASTELAND_BOMBCHU_SALESMAN)->GetPlacedItem().GetHint().GetClear();

  Text grannyCapitalItemText = grannyItemText.Capitalize();

  Text medigoronText = Hint(MEDIGORON_DIALOG_FIRST).GetText()+medigoronItemText+Hint(MEDIGORON_DIALOG_SECOND).GetText();
  Text grannyText = grannyCapitalItemText+Hint(GRANNY_DIALOG).GetText();
  Text carpetSalesmanTextOne = Hint(CARPET_SALESMAN_DIALOG_FIRST).GetText()+carpetSalesmanItemText+Hint(CARPET_SALESMAN_DIALOG_SECOND).GetText();
  Text carpetSalesmanTextTwo = Hint(CARPET_SALESMAN_DIALOG_THIRD).GetText()+carpetSalesmanItemClearText+Hint(CARPET_SALESMAN_DIALOG_FOURTH).GetText();

  CreateMessageFromTextObject(0x9120, 0, 2, 3, AddColorsAndFormat(medigoronText, {QM_RED, QM_GREEN}));
  CreateMessageFromTextObject(0x9121, 0, 2, 3, AddColorsAndFormat(grannyText, {QM_RED, QM_GREEN}));
  CreateMessageFromTextObject(0x6077, 0, 2, 3, AddColorsAndFormat(carpetSalesmanTextOne, {QM_RED, QM_GREEN}));
  CreateMessageFromTextObject(0x6078, 0, 2, 3, AddColorsAndFormat(carpetSalesmanTextTwo, {QM_RED, QM_YELLOW, QM_RED}));
}

void CreateDampesDiaryText() {
  if (!DampeHintText) {
    dampesText = Text();
    dampeHintLoc = "";
    return;
  }

  uint32_t item = PROGRESSIVE_HOOKSHOT;
  uint32_t location = FilterFromPool(allLocations, [item](const uint32_t loc){return Location(loc)->GetPlaceduint32_t() == item;})[0];
  Text area = GetHintRegion(Location(location)->GetParentRegionKey())->GetHint().GetText();
  Text temp1 = Text{
    "Whoever reads this, please enter %g", 
    "Toi qui lit ce journal, rends-toi dans %g",
    "Wer immer dies liest, der möge folgenden Ort aufsuchen: %g"
  };

  Text temp2 = {
    "%w. I will let you have my stretching, shrinking keepsake.^I'm waiting for you.&--Dampé",
    "%w. Et peut-être auras-tu droit à mon précieux %rtrésor%w.^Je t'attends...&--Igor",
    "%w. Ihm gebe ich meinen langen, kurzen Schatz.^Ich warte!&Boris"
  };
  
  dampesText = temp1 + area + temp2;
  dampeHintLoc = Location(location)->GetName();
}

void CreateGregRupeeHint() {
  if (!GregHintText) {
    gregText = Text();
    return;
  }

  uint32_t location = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->GetPlacedItemKey() == GREG_RUPEE;})[0];
  Text area = GetHintRegion(Location(location)->GetParentRegionKey())->GetHint().GetText();

  Text temp1 = Text{
    "By the way, if you're interested, I saw the shiniest %gGreen Rupee%w somewhere in%g ",
    "Au fait, si ça t'intéresse, j'ai aperçu le plus éclatant des %gRubis Verts%w quelque part à %g",
    ""
  };

  Text temp2 = {
    "%w.^It's said to have %rmysterious powers%w...^But then, it could just be another regular rupee.&Oh well.",
    "%w. On dit qu'il possède des pouvoirs mystérieux... Mais bon, ça pourrait juste être un autre rubis ordinaire.",
    ""
  };

    gregText = temp1 + area + temp2;
}

void CreateSheikText() {
  //Get the location of the light arrows
  auto lightArrowLocation = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->GetPlaceduint32_t() == LIGHT_ARROWS;});
  lightArrowHintLoc = Location(lightArrowLocation[0])->GetName();
  Text area = GetHintRegion(Location(lightArrowLocation[0])->GetParentRegionKey())->GetHint().GetText();
  Text temp1 = Text{
    "I overheard Ganondorf say that he misplaced the %rLight Arrows%w in&%g",
    "J'ai entendu dire que Ganondorf aurait caché les %rFlèches de Lumière%w dans %g",
    ""
  };
  Text temp2 = Text{"%w.", "%w.", "%w."};
  sheikText = temp1 + area + temp2;
}

void CreateSariaText() {
  //Get the location of the light arrows
  auto magicLocation = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->GetPlaceduint32_t() == PROGRESSIVE_MAGIC_METER;});
  sariaHintLoc = Location(magicLocation[0])->GetName();
  Text area = GetHintRegion(Location(magicLocation[0])->GetParentRegionKey())->GetHint().GetText();
  Text temp1 = Text{
    "Did you feel the %gsurge of magic%w recently? A mysterious bird told me it came from %g",
    "As-tu récemment ressenti une vague de %gpuissance magique%w? Un mystérieux hibou m'a dit  qu'elle provenait du %g",
    ""
  };
  Text temp2 = Text{
    "%w.^You should check that place out, @!$C", 
    "%w. Tu devrais aller y jeter un coup d'oeil, @!$C", 
    "%w.$C"
  };
  sariaText = temp1 + area + temp2;
}


void CreateWarpSongTexts() {
  if (!ShuffleWarpSongs) {
    warpMinuetText = Text();
    warpBoleroText = Text();
    warpSerenadeText = Text();
    warpRequiemText = Text();
    warpNocturneText = Text();
    warpPreludeText = Text();
    return;
  }

  auto warpSongEntrances = GetShuffleableEntrances(EntranceType::WarpSong, false);

  for (auto entrance : warpSongEntrances) {
    Text resolvedHint;
    // Start with entrance location text
    auto region = entrance->GetConnectedRegion()->regionName;
    resolvedHint = Text{"","",""} + region;

    auto destination = entrance->GetConnectedRegion()->GetHint().GetText();
    // Prefer hint location when available
    if (destination.GetEnglish() != "No Hint") {
      resolvedHint = destination;
    }

    switch (entrance->GetIndex()) {
      case 0x0600: // minuet
        warpMinuetText = resolvedHint;
        break;
      case 0x04F6: // bolero
        warpBoleroText = resolvedHint;
        break;
      case 0x0604: // serenade
        warpSerenadeText = resolvedHint;
        break;
      case 0x01F1: // requiem
        warpRequiemText = resolvedHint;
        break;
      case 0x0568: // nocturne
        warpNocturneText = resolvedHint;
        break;
      case 0x05F4: // prelude
        warpPreludeText = resolvedHint;
        break;
    }
  }
}

void CreateAllHints() {

  SPDLOG_DEBUG("\nNOW CREATING HINTS\n");
  const HintSetting& hintSetting = hintSettingTable[Settings::HintDistribution.Value<uint8_t>()];

  uint8_t remainingDungeonWothHints = hintSetting.dungeonsWothLimit;
  uint8_t remainingDungeonBarrenHints = hintSetting.dungeonsBarrenLimit;

  // Add 'always' location hints
  if (hintSetting.distTable[static_cast<int>(HINT_TYPE_ALWAYS)].copies > 0) {
    // Only filter locations that had a random item placed at them (e.g. don't get cow locations if shuffle cows is off)
    auto alwaysHintLocations = FilterFromPool(allLocations, [](const uint32_t loc){
        return ((Location(loc)->GetHint().GetType() == HintCategory::Always) ||
                // If we have Rainbow Bridge set to Greg, add a hint for where Greg is
                (Bridge.Is(RAINBOWBRIDGE_GREG) && !GregHintText && Location(loc)->GetPlacedItemKey() == GREG_RUPEE)) &&
               Location(loc)->IsHintable()        && !(Location(loc)->IsHintedAt());
    });

    for (auto& hint : conditionalAlwaysHints) {
        uint32_t loc = hint.first;
        if (hint.second() && Location(loc)->IsHintable() && !Location(loc)->IsHintedAt()) {
            alwaysHintLocations.push_back(loc);
        }
    }

    for (uint32_t location : alwaysHintLocations) {
      CreateLocationHint({location});
    }
  }

  //Add 'trial' location hints
  if (hintSetting.distTable[static_cast<int>(HINT_TYPE_TRIAL)].copies > 0) {
    CreateTrialHints();
  }

  //create a vector with each hint type proportional to it's weight in the distribution setting.
  //ootr uses a weighted probability function to decide hint types, but selecting randomly from
  //this vector will do for now
  std::vector<HintType> remainingHintTypes = {};
  for (HintDistributionSetting hds : hintSetting.distTable) {
    remainingHintTypes.insert(remainingHintTypes.end(), hds.weight, hds.type);
  }
  Shuffle(remainingHintTypes);

  //get barren regions
  auto barrenLocations = CalculateBarrenRegions();

  //Calculate dungeon woth/barren info

  std::vector<std::string> dungeonNames = {"Deku Tree", "Dodongos Cavern", "Jabu Jabus Belly", "Forest Temple", "Fire Temple",
                                           "Water Temple", "Spirit Temple", "Shadow Temple", "Bottom of the Well", "Ice Cavern"};
  //Get list of all barren dungeons
  std::vector<std::string> barrenDungeons;
  for (uint32_t barrenLocation : barrenLocations) {
    std::string barrenRegion = GetHintRegion(Location(barrenLocation)->GetParentRegionKey())->scene;
    bool isDungeon = std::find(dungeonNames.begin(), dungeonNames.end(), barrenRegion) != dungeonNames.end();
    //If it hasn't already been added to the list and is a dungeon, add to list
    if (isDungeon && std::find(barrenDungeons.begin(), barrenDungeons.end(), barrenRegion) == barrenDungeons.end()) {
      barrenDungeons.push_back(barrenRegion);
    }
  }
  SPDLOG_DEBUG("\nBarren Dungeons:\n");
  for (std::string barrenDungeon : barrenDungeons) {
      SPDLOG_DEBUG(barrenDungeon + "\n");
  }

  //Get list of all woth dungeons
  std::vector<std::string> wothDungeons;
  for (uint32_t wothLocation : wothLocations) {
    std::string wothRegion = GetHintRegion(Location(wothLocation)->GetParentRegionKey())->scene;
    bool isDungeon = std::find(dungeonNames.begin(), dungeonNames.end(), wothRegion) != dungeonNames.end();
    //If it hasn't already been added to the list and is a dungeon, add to list
    if (isDungeon && std::find(wothDungeons.begin(), wothDungeons.end(), wothRegion) == wothDungeons.end()) {
      wothDungeons.push_back(wothRegion);
    }
  }
  SPDLOG_DEBUG("\nWoth Dungeons:\n");
  for (std::string wothDungeon : wothDungeons) {
      SPDLOG_DEBUG(wothDungeon + "\n");
  }

  //Set DungeonInfo array for each dungeon
  for (uint32_t i = 0; i < dungeonInfoData.size(); i++) {
    std::string dungeonName = dungeonNames[i];
    if (std::find(barrenDungeons.begin(), barrenDungeons.end(), dungeonName) != barrenDungeons.end()) {
      dungeonInfoData[i] = DungeonInfo::DUNGEON_BARREN;
    } else if (std::find(wothDungeons.begin(), wothDungeons.end(), dungeonName) != wothDungeons.end()) {
      dungeonInfoData[i] = DungeonInfo::DUNGEON_WOTH;
    } else {
      dungeonInfoData[i] = DungeonInfo::DUNGEON_NEITHER;
    }
  }

  //while there are still gossip stones remaining
  while (FilterFromPool(gossipStoneLocations, [](const uint32_t loc){return Location(loc)->GetPlaceduint32_t() == NONE;}).size() != 0) {
    //TODO: fixed hint types

    if (remainingHintTypes.empty()) {
      break;
    }

    //get a random hint type from the remaining hints
    HintType type = RandomElement(remainingHintTypes, true);

    SPDLOG_DEBUG("Attempting to make hint of type: ");
    SPDLOG_DEBUG(hintTypeNames.find(type)->second);
    SPDLOG_DEBUG("\n");

    //create the appropriate hint for the type
    if (type == HINT_TYPE_WOTH) {
      CreateWothHint(&remainingDungeonWothHints);

    } else if (type == HINT_TYPE_BARREN) {
      CreateBarrenHint(&remainingDungeonBarrenHints, barrenLocations);

    } else if (type == HINT_TYPE_SOMETIMES){
      std::vector<uint32_t> sometimesHintLocations = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->GetHint().GetType() == HintCategory::Sometimes && Location(loc)->IsHintable() && !(Location(loc)->IsHintedAt());});
      CreateLocationHint(sometimesHintLocations);

    } else if (type == HINT_TYPE_RANDOM) {
      CreateRandomLocationHint();

    } else if (type == HINT_TYPE_ITEM) {
      CreateGoodItemHint();

    } else if (type == HINT_TYPE_SONG){
      std::vector<uint32_t> songHintLocations = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->IsCategory(Category::cSong) && Location(loc)->IsHintable() && !(Location(loc)->IsHintedAt());});
      CreateLocationHint(songHintLocations);

    } else if (type == HINT_TYPE_OVERWORLD){
      std::vector<uint32_t> overworldHintLocations = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->IsOverworld() && Location(loc)->IsHintable() && !(Location(loc)->IsHintedAt());});
      CreateLocationHint(overworldHintLocations);

    } else if (type == HINT_TYPE_DUNGEON){
      std::vector<uint32_t> dungeonHintLocations = FilterFromPool(allLocations, [](const uint32_t loc){return Location(loc)->IsDungeon() && Location(loc)->IsHintable() && !(Location(loc)->IsHintedAt());});
      CreateLocationHint(dungeonHintLocations);

    } else if (type == HINT_TYPE_JUNK) {
      CreateJunkHint();
    }
  }

  //If any gossip stones failed to have a hint placed on them for some reason, place a junk hint as a failsafe.
  for (uint32_t gossipStone : FilterFromPool(gossipStoneLocations, [](const uint32_t loc){return Location(loc)->GetPlaceduint32_t() == NONE;})) {
    const HintText junkHint = RandomElement(GetHintCategory(HintCategory::Junk));
    AddHint(junkHint.GetText(), gossipStone, {QM_PINK});
  }

  //Getting gossip stone locations temporarily sets one location to not be reachable.
  //Call the function one last time to get rid of false positives on locations not
  //being reachable.
  GetAccessibleLocations({});
}

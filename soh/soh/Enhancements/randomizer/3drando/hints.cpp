#include "hints.hpp"

#include "custom_messages.hpp"
#include "dungeon.hpp"
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
#include "../context.h"

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

Area* GetHintRegion(const RandomizerRegion area) {

  std::vector<RandomizerRegion> alreadyChecked = {};
  std::vector<RandomizerRegion> spotQueue = {area};

  while (!spotQueue.empty()) {
    RandomizerRegion region = spotQueue.back();
    alreadyChecked.push_back(region);
    spotQueue.pop_back();

    if (AreaTable(region)->hintKey != RHT_NONE) {
      return AreaTable(region);
    }

    //add unchecked entrances to spot queue
    bool checked = false;
    for (auto& entrance : AreaTable(region)->entrances) {
      for (RandomizerRegion checkedEntrance : alreadyChecked) {
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

  return AreaTable(RR_NONE);
}

RandomizerHintTextKey GetHintRegionHintKey(const RandomizerRegion area) {
    return GetHintRegion(area)->hintKey;
}

uint32_t GetHintRegionuint32_t(const RandomizerRegion area) {
  return GetHintRegion(area)->hintKey;
}

uint32_t GetLocationRegionuint32_t(const RandomizerCheck location) {
  return GetHintRegion(Rando::Context::GetInstance()->GetItemLocation(location)->GetParentRegionKey())->hintKey;
}

static std::vector<RandomizerCheck> GetAccessibleGossipStones(const RandomizerCheck hintedLocation = RC_GANON) {
  auto ctx = Rando::Context::GetInstance();
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for gossip stone locations.
  RandomizerGet originalItem = ctx->GetItemLocation(hintedLocation)->GetPlacedRandomizerGet();
  ctx->GetItemLocation(hintedLocation)->SetPlacedItem(RG_NONE);

  LogicReset();
  auto accessibleGossipStones = GetAccessibleLocations(Rando::StaticData::gossipStoneLocations);
  //Give the item back to the location
  ctx->GetItemLocation(hintedLocation)->SetPlacedItem(originalItem);

  return accessibleGossipStones;
}

bool IsReachableWithout(uint32_t locToCheck, uint32_t excludedCheck, bool resetAfter = true){
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for this check
  uint32_t originalItem = ctx->GetItemLocation(excludedCheck)->GetPlaceduint32_t();
  ctx->GetItemLocation(excludedCheck)->SetPlacedItem(NONE);
  LogicReset();
  const std::vector<uint32_t> rechableWithout = GetAccessibleLocations({locToCheck});
  ctx->GetItemLocation(excludedCheck)->SetPlacedItem(originalItem);
  if (resetAfter){
    //if resetAfter is on, reset logic we are done
    LogicReset();
  }
  if (rechableWithout.empty()) {
    return false;
  }
  return true;
}

bool IsReachableWithout(uint32_t locToCheck, uint32_t excludedCheck, bool resetAfter = true){
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for this check
  uint32_t originalItem = Location(excludedCheck)->GetPlaceduint32_t();
  Location(excludedCheck)->SetPlacedItem(NONE);
  LogicReset();
  const std::vector<uint32_t> rechableWithout = GetAccessibleLocations({locToCheck});
  Location(excludedCheck)->SetPlacedItem(originalItem);
  if (resetAfter){
    //if resetAfter is on, reset logic we are done
    LogicReset();
  }
  if (rechableWithout.empty()) {
    return false;
  }
  return true;
}

bool IsReachableWithout(uint32_t locToCheck, uint32_t excludedCheck, bool resetAfter = true){
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for this check
  uint32_t originalItem = ctx->GetItemLocation(excludedCheck)->GetPlaceduint32_t();
  ctx->GetItemLocation(excludedCheck)->SetPlacedItem(NONE);
  LogicReset();
  const std::vector<uint32_t> rechableWithout = GetAccessibleLocations({locToCheck});
  ctx->GetItemLocation(excludedCheck)->SetPlacedItem(originalItem);
  if (resetAfter){
    //if resetAfter is on, reset logic we are done
    LogicReset();
  }
  if (rechableWithout.empty()) {
    return false;
  }
  return true;
}

bool IsReachableWithout(uint32_t locToCheck, uint32_t excludedCheck, bool resetAfter = true){
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for this check
  uint32_t originalItem = Location(excludedCheck)->GetPlaceduint32_t();
  Location(excludedCheck)->SetPlacedItem(NONE);
  LogicReset();
  const std::vector<uint32_t> rechableWithout = GetAccessibleLocations({locToCheck});
  Location(excludedCheck)->SetPlacedItem(originalItem);
  if (resetAfter){
    //if resetAfter is on, reset logic we are done
    LogicReset();
  }
  if (rechableWithout.empty()) {
    return false;
  }
  return true;
}

static void AddHint(Text hint, const RandomizerCheck gossipStone, const std::vector<uint8_t>& colors = {}, HintType hintType = HINT_TYPE_ITEM, const RandomizerCheck hintedLocation = RC_UNKNOWN_CHECK) {
  //save hints as dummy items for writing to the spoiler log
  //NewItem(gossipStone, Item{RG_HINT, hint, ITEMTYPE_EVENT, GI_RUPEE_BLUE_LOSE, false, &noVariable, NONE});
  //GetLocation(gossipStone)->SetPlacedItem(gossipStone);

  auto ctx = Rando::Context::GetInstance();
  ctx->AddHint((RandomizerHintKey)((gossipStone - RC_DMC_GOSSIP_STONE) + 1), hint, hintedLocation, hintType, GetHintRegion(ctx->GetItemLocation(hintedLocation)->GetParentRegionKey())->GetHint().GetText());
  ctx->GetItemLocation(gossipStone)->SetPlacedItem(RG_HINT);
}

static void CreateLocationHint(const std::vector<RandomizerCheck>& possibleHintLocations) {
  auto ctx = Rando::Context::GetInstance();
  //return if there aren't any hintable locations or gossip stones available
  if (possibleHintLocations.empty()) {
      SPDLOG_DEBUG("\tNO LOCATIONS TO HINT\n\n");
    return;
  }

  RandomizerCheck hintedLocation = RandomElement(possibleHintLocations);
  const std::vector<RandomizerCheck> accessibleGossipStones = GetAccessibleGossipStones(hintedLocation);

  SPDLOG_DEBUG("\tLocation: ");
  SPDLOG_DEBUG(Rando::StaticData::GetLocation(hintedLocation)->GetName());
  SPDLOG_DEBUG("\n");

  SPDLOG_DEBUG("\tItem: ");
  SPDLOG_DEBUG(ctx->GetItemLocation(hintedLocation)->GetPlacedItemName().GetEnglish());
  SPDLOG_DEBUG("\n");

  if (accessibleGossipStones.empty()) {
      SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
    return;
  }

  RandomizerCheck gossipStone = RandomElement(accessibleGossipStones);
  ctx->GetItemLocation(hintedLocation)->SetAsHinted();

  //make hint text
  Text locationHintText = Rando::StaticData::GetLocation(hintedLocation)->GetHint()->GetText();
  Text itemHintText = ctx->GetItemLocation(hintedLocation)->GetPlacedItem().GetHint().GetText();
  Text prefix = Hint(RHT_PREFIX).GetText();
  
  Text finalHint = prefix + "%r" + locationHintText + " #%g" + itemHintText + "#%w.";
  SPDLOG_DEBUG("\tMessage: ");
  SPDLOG_DEBUG(finalHint.english);
  SPDLOG_DEBUG("\n\n");

  AddHint(finalHint, gossipStone, {QM_GREEN, QM_RED}, HINT_TYPE_ITEM, hintedLocation);
}

static void CreateWothHint(uint8_t* remainingDungeonWothHints) {
    auto ctx = Rando::Context::GetInstance();
    // get locations that are in the current playthrough
    std::vector<RandomizerCheck> possibleHintLocations = {};
    // iterate through playthrough locations by sphere
    std::vector<RandomizerCheck> wothHintLocations =
        FilterFromPool(ctx->wothLocations, [remainingDungeonWothHints, ctx](RandomizerCheck loc) {
            return ctx->GetItemLocation(loc)->IsHintable() &&    // only filter hintable locations
                   !(ctx->GetItemLocation(loc)->IsHintedAt()) && // only filter locations that haven't been hinted at
                   (Rando::StaticData::GetLocation(loc)->IsOverworld() ||
                    (Rando::StaticData::GetLocation(loc)->IsDungeon() &&
                     (*remainingDungeonWothHints) > 0)); // make sure we haven't surpassed the woth dungeon limit
        });
    AddElementsToPool(possibleHintLocations, wothHintLocations);

    // If no more locations can be hinted at for woth, then just try to get another hint
    if (possibleHintLocations.empty()) {
        SPDLOG_DEBUG("\tNO LOCATIONS TO HINT\n\n");
        return;
    }
    RandomizerCheck hintedLocation = RandomElement(possibleHintLocations);

    SPDLOG_DEBUG("\tLocation: ");
    SPDLOG_DEBUG(Rando::StaticData::GetLocation(hintedLocation)->GetName());
    SPDLOG_DEBUG("\n");

    SPDLOG_DEBUG("\tItem: ");
    SPDLOG_DEBUG(ctx->GetItemLocation(hintedLocation)->GetPlacedItemName().GetEnglish());
    SPDLOG_DEBUG("\n");

    // get an accessible gossip stone
    const std::vector<RandomizerCheck> gossipStoneLocations = GetAccessibleGossipStones(hintedLocation);

    if (gossipStoneLocations.empty()) {
        SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
        return;
    }
    ctx->GetItemLocation(hintedLocation)->SetAsHinted();
    RandomizerCheck gossipStone = RandomElement(gossipStoneLocations);

    if (Rando::StaticData::GetLocation(hintedLocation)->IsDungeon()) {
        *remainingDungeonWothHints -= 1;
    }

    // form hint text
    Text locationText = GetHintRegion(ctx->GetItemLocation(hintedLocation)->GetParentRegionKey())->GetHint().GetText();
    Text finalWothHint = Hint(RHT_PREFIX).GetText() + "%r#" + locationText + "#%w" + Hint(RHT_WAY_OF_THE_HERO).GetText();
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalWothHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalWothHint, gossipStone, { QM_LBLUE }, HINT_TYPE_WOTH, hintedLocation);
}

static void CreateBarrenHint(uint8_t* remainingDungeonBarrenHints, std::vector<RandomizerCheck>& barrenLocations) {
    auto ctx = Rando::Context::GetInstance();
    // remove dungeon locations if necessary
    if (*remainingDungeonBarrenHints < 1) {
        barrenLocations =
            FilterFromPool(barrenLocations, [](const RandomizerCheck loc) { 
              return !(Rando::StaticData::GetLocation(loc)->IsDungeon()); 
            });
    }

    if (barrenLocations.empty()) {
        return;
    }

    RandomizerCheck hintedLocation = RandomElement(barrenLocations, true);

    SPDLOG_DEBUG("\tLocation: ");
    SPDLOG_DEBUG(Rando::StaticData::GetLocation(hintedLocation)->GetName());
    SPDLOG_DEBUG("\n");

    SPDLOG_DEBUG("\tItem: ");
    SPDLOG_DEBUG(ctx->GetItemLocation(hintedLocation)->GetPlacedItemName().GetEnglish());
    SPDLOG_DEBUG("\n");

    // get an accessible gossip stone
    const std::vector<RandomizerCheck> gossipStoneLocations = GetAccessibleGossipStones(hintedLocation);
    if (gossipStoneLocations.empty()) {
        SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
        return;
    }
    ctx->GetItemLocation(hintedLocation)->SetAsHinted();
    RandomizerCheck gossipStone = RandomElement(gossipStoneLocations);

    if (Rando::StaticData::GetLocation(hintedLocation)->IsDungeon()) {
        *remainingDungeonBarrenHints -= 1;
    }

    // form hint text
    Text locationText = GetHintRegion(ctx->GetItemLocation(hintedLocation)->GetParentRegionKey())->GetHint().GetText();
    Text finalBarrenHint =
        Hint(RHT_PREFIX).GetText() + Hint(RHT_PLUNDERING).GetText() + "%r#" + locationText + "#%w" + Hint(RHT_FOOLISH).GetText();
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalBarrenHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalBarrenHint, gossipStone, { QM_PINK }, HINT_TYPE_BARREN, hintedLocation);

    // get rid of all other locations in this same barren region
    barrenLocations = FilterFromPool(barrenLocations, [hintedLocation, ctx](RandomizerCheck loc) {
        return GetHintRegion(ctx->GetItemLocation(loc)->GetParentRegionKey())->hintKey !=
               GetHintRegion(ctx->GetItemLocation(hintedLocation)->GetParentRegionKey())->hintKey;
    });
}

static void CreateRandomLocationHint(const bool goodItem = false) {
  auto ctx = Rando::Context::GetInstance();
  const std::vector<RandomizerCheck> possibleHintLocations = FilterFromPool(ctx->allLocations, [goodItem, ctx](const RandomizerCheck loc) {
    return ctx->GetItemLocation(loc)->IsHintable() && !(ctx->GetItemLocation(loc)->IsHintedAt()) && (!goodItem || ctx->GetItemLocation(loc)->GetPlacedItem().IsMajorItem());
  });
  //If no more locations can be hinted at, then just try to get another hint
  if (possibleHintLocations.empty()) {
    SPDLOG_DEBUG("\tNO LOCATIONS TO HINT\n\n");
    return;
  }
  RandomizerCheck hintedLocation = RandomElement(possibleHintLocations);

  SPDLOG_DEBUG("\tLocation: ");
  SPDLOG_DEBUG(Rando::StaticData::GetLocation(hintedLocation)->GetName());
  SPDLOG_DEBUG("\n");

  SPDLOG_DEBUG("\tItem: ");
  SPDLOG_DEBUG(ctx->GetItemLocation(hintedLocation)->GetPlacedItemName().GetEnglish());
  SPDLOG_DEBUG("\n");

  //get an acessible gossip stone
  const std::vector<RandomizerCheck> gossipStoneLocations = GetAccessibleGossipStones(hintedLocation);
  if (gossipStoneLocations.empty()) {
      SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
    return;
  }
  ctx->GetItemLocation(hintedLocation)->SetAsHinted();
  RandomizerCheck gossipStone = RandomElement(gossipStoneLocations);

  //form hint text
  Text itemText = ctx->GetItemLocation(hintedLocation)->GetPlacedItem().GetHint().GetText();
  Text locationText = GetHintRegion(ctx->GetItemLocation(hintedLocation)->GetParentRegionKey())->GetHint().GetText();
  // RANDOTODO: reconsider dungeon vs non-dungeon item location hints when boss shuffle mixed pools happens
  if (Rando::StaticData::GetLocation(hintedLocation)->IsDungeon()) {
    Text finalHint = Hint(RHT_PREFIX).GetText()+"%r#"+locationText+"#%w "+Hint(RHT_HOARDS).GetText()+" %g#"+itemText+"#%w.";
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalHint, gossipStone, {QM_GREEN, QM_RED}, HINT_TYPE_NAMED_ITEM, hintedLocation);
  } else {
    Text finalHint = Hint(RHT_PREFIX).GetText()+"%r#"+itemText+"#%w "+Hint(RHT_CAN_BE_FOUND_AT).GetText()+" %g#"+locationText+"#%w.";
    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(finalHint.english);
    SPDLOG_DEBUG("\n\n");
    AddHint(finalHint, gossipStone, { QM_RED, QM_GREEN }, HINT_TYPE_NAMED_ITEM, hintedLocation);
  }
}

static void CreateJunkHint() {
  //duplicate junk hints are possible for now
  const HintText junkHint = RandomElement(GetHintCategory(HintCategory::Junk));
  LogicReset();
  const std::vector<RandomizerCheck> gossipStones = GetAccessibleLocations(Rando::StaticData::gossipStoneLocations);
  if (gossipStones.empty()) {
      SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
    return;
  }
  RandomizerCheck gossipStone = RandomElement(gossipStones);
  Text hint = junkHint.GetText();

  SPDLOG_DEBUG("\tMessage: ");
  SPDLOG_DEBUG(hint.english);
  SPDLOG_DEBUG("\n\n");

  AddHint(hint, gossipStone, { QM_PINK }, HINT_TYPE_JUNK);
}

static std::vector<RandomizerCheck> CalculateBarrenRegions() {
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> barrenLocations = {};
  std::vector<RandomizerCheck> potentiallyUsefulLocations = {};

  for (RandomizerCheck loc : ctx->allLocations) {
    // If a location has a major item or is a way of the hero location, it is not barren
    if (ctx->GetItemLocation(loc)->GetPlacedItem().IsMajorItem() || ElementInContainer(loc, ctx->wothLocations)) {
      AddElementsToPool(potentiallyUsefulLocations, std::vector{loc});
    } else {
      // Link's Pocket & Triforce Hunt "reward" shouldn't be considered for barren areas because it's clear what
      // they have to a player.
      if (loc != RC_LINKS_POCKET && loc != RC_TRIFORCE_COMPLETED) { 
        AddElementsToPool(barrenLocations, std::vector{loc});
      }
    }
  }

  // Leave only locations at barren regions in the list
  auto finalBarrenLocations = FilterFromPool(barrenLocations, [&potentiallyUsefulLocations](RandomizerCheck loc){
    for (RandomizerCheck usefulLoc : potentiallyUsefulLocations) {
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
    auto hint = Hint(RHT_PREFIX).GetText() + Hint(RHT_SIX_TRIALS).GetText();
    AddHint(hint, gossipStone, { QM_PINK }, HINT_TYPE_TRIAL);

    //zero trials
  } else if (RandomGanonsTrials && GanonsTrialsCount.Is(0)) {

    //get a random gossip stone
    auto gossipStones = GetAccessibleGossipStones();
    auto gossipStone = RandomElement(gossipStones, false);

    //make hint
    auto hint = Hint(RHT_PREFIX).GetText() + Hint(RHT_ZERO_TRIALS).GetText();
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
      auto hint = Hint(RHT_PREFIX).GetText()+"#"+trial->GetName()+"#"+Hint(RHT_FOUR_TO_FIVE_TRIALS).GetText();
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
      auto hint = Hint(RHT_PREFIX).GetText()+"#"+trial->GetName()+"#"+Hint(RHT_ONE_TO_THREE_TRIALS).GetText();
      AddHint(hint, gossipStone, { QM_PINK }, HINT_TYPE_TRIAL);
    }
  }
}

void CreateGanonAndSheikText() {
  auto ctx = Rando::Context::GetInstance();
  //funny ganon line
  ganonText = RandomElement(GetHintCategory(HintCategory::GanonLine)).GetText();
  CreateMessageFromTextObject(0x70CB, 0, 2, 3, AddColorsAndFormat(ganonText));

  if(Settings::LightArrowHintText){
    //Get the location of the light arrows
    auto lightArrowLocation = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_LIGHT_ARROWS;
    });
    Text lightArrowArea = GetHintRegion(ctx->GetItemLocation(lightArrowLocation[0])->GetParentRegionKey())->GetHint().GetText();
    std::vector<RandomizerCheck> locsToCheck = {RC_GANONDORF_HINT};
    

    //If there is no light arrow location, it was in the player's inventory at the start
    auto hint = Hint(RHT_LIGHT_ARROW_LOCATION_HINT);
    if (lightArrowLocation.empty()) {
      ganonHintText = hint.GetText()+Hint(RHT_YOUR_POCKET).GetText();
      lightArrowHintLoc = "Link's Pocket";
    } else {
      ganonHintText = hint.GetText() + "%r" + lightArrowArea;
      lightArrowHintLoc = Rando::StaticData::GetLocation(lightArrowLocation[0])->GetName();
  }
  ganonHintText = ganonHintText + "!";

  CreateMessageFromTextObject(0x70CC, 0, 2, 3, AddColorsAndFormat(ganonHintText));
  ctx->AddHint(RH_GANONDORF_HINT, ganonHintText, lightArrowLocation[0], HINT_TYPE_STATIC, GetHintRegion(ctx->GetItemLocation(lightArrowLocation[0])->GetParentRegionKey())->GetHint().GetText());
}

//Find the location which has the given itemKey and create the generic altar text for the reward
static Text BuildDungeonRewardText(const RandomizerGet itemKey, bool isChild) {
    auto ctx = Rando::Context::GetInstance();
    RandomizerCheck altarLoc = RC_ALTAR_HINT_ADULT;
    if(isChild){altarLoc = RC_ALTAR_HINT_CHILD;}
    RandomizerCheck location = FilterFromPool(ctx->allLocations, [itemKey, ctx](const RandomizerCheck loc) {
        return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == itemKey;
    })[0];
    if (IsReachableWithout(altarLoc, location, true) || ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON)){ //RANDOTODO check if works properly
      ctx->GetItemLocation(location)->SetAsHinted();
    }

    std::string rewardString = "$" + std::to_string(itemKey - RG_KOKIRI_EMERALD);

    // RANDOTODO implement colors for locations
    return Text() + rewardString +
           GetHintRegion(ctx->GetItemLocation(location)->GetParentRegionKey())->GetHint().GetText().Capitalize() +
           "...^";
}

static Text BuildDoorOfTimeText() {
  std::string itemObtained;
  Text doorOfTimeText;

  if (OpenDoorOfTime.Is(OPENDOOROFTIME_OPEN)) {
    itemObtained = "$o";
    doorOfTimeText = Hint(RHT_CHILD_ALTAR_TEXT_END_DOTOPEN).GetText();

  } else if (OpenDoorOfTime.Is(OPENDOOROFTIME_SONGONLY)) {
    itemObtained = "$c";
    doorOfTimeText = Hint(RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY).GetText();

  } else if (OpenDoorOfTime.Is(OPENDOOROFTIME_CLOSED)) {
    itemObtained = "$i";
    doorOfTimeText = Hint(RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED).GetText();
  }

  return Text()+itemObtained+doorOfTimeText;
}

//insert the required number into the hint and set the singular/plural form
static Text BuildCountReq(const RandomizerHintTextKey req, const Option& count) {
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
    bridgeText = Hint(RHT_BRIDGE_OPEN_HINT).GetText();

  } else if (Bridge.Is(RAINBOWBRIDGE_VANILLA)) {
    bridgeText = Hint(RHT_BRIDGE_VANILLA_HINT).GetText();

  } else if (Bridge.Is(RAINBOWBRIDGE_STONES)) {
    bridgeText = BuildCountReq(RHT_BRIDGE_STONES_HINT, BridgeStoneCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_MEDALLIONS)) {
    bridgeText = BuildCountReq(RHT_BRIDGE_MEDALLIONS_HINT, BridgeMedallionCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_REWARDS)) {
    bridgeText = BuildCountReq(RHT_BRIDGE_REWARDS_HINT, BridgeRewardCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_DUNGEONS)) {
    bridgeText = BuildCountReq(RHT_BRIDGE_DUNGEONS_HINT, BridgeDungeonCount);

  } else if (Bridge.Is(RAINBOWBRIDGE_TOKENS)) {
    bridgeText = BuildCountReq(RHT_BRIDGE_TOKENS_HINT, BridgeTokenCount);
  
  } else if (Bridge.Is(RAINBOWBRIDGE_GREG)) {
    bridgeText = Hint(RHT_BRIDGE_GREG_HINT).GetText();
  }

  return Text()+"$l"+bridgeText+"^";
}

static Text BuildGanonBossKeyText() {
  Text ganonBossKeyText;

  if (GanonsBossKey.Is(GANONSBOSSKEY_START_WITH)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_START_WITH_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_VANILLA)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_VANILLA_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_OWN_DUNGEON)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_OWN_DUNGEON_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_ANY_DUNGEON)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_ANY_DUNGEON_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_OVERWORLD)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_OVERWORLD_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_ANYWHERE)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_ANYWHERE_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_FINAL_GS_REWARD)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_SKULLTULA_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_VANILLA)) {
    ganonBossKeyText = Hint(RHT_LACS_VANILLA_HINT).GetText();

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_STONES)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_STONES_HINT, LACSStoneCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_MEDALLIONS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_MEDALLIONS_HINT, LACSMedallionCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_REWARDS_HINT, LACSRewardCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_DUNGEONS_HINT, LACSDungeonCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_TOKENS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_TOKENS_HINT, LACSTokenCount);

  } else if (GanonsBossKey.Is(GANONSBOSSKEY_TRIFORCE_HUNT)) {
    ganonBossKeyText = Hint(RHT_GANON_BK_TRIFORCE_HINT).GetText();
  }

  return Text()+"$b"+ganonBossKeyText+"^";
}

void CreateAltarText() {
  auto ctx = Rando::Context::GetInstance();

  //Child Altar Text
  if (AltarHintText) {
    childAltarText = Hint(RHT_SPIRITUAL_STONE_TEXT_START).GetText()+"^"+
    //Spiritual Stones
        (StartingKokiriEmerald.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_KOKIRI_EMERALD, true)) +
        (StartingGoronRuby.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                            : BuildDungeonRewardText(RG_GORON_RUBY, true)) +
        (StartingZoraSapphire.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                              : BuildDungeonRewardText(RG_ZORA_SAPPHIRE, true)) +
    //How to open Door of Time, the event trigger is necessary to read the altar multiple times
    BuildDoorOfTimeText();
  } else {
    childAltarText = BuildDoorOfTimeText();
  }
  ctx->AddHint(RH_ALTAR_CHILD, childAltarText, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text());

  CreateMessageFromTextObject(0x7040, 0, 2, 3, AddColorsAndFormat(childAltarText, {QM_GREEN, QM_RED, QM_BLUE}));

  //Adult Altar Text
  adultAltarText = Hint(RHT_ADULT_ALTAR_TEXT_START).GetText() + "^";
  if (AltarHintText) {
    adultAltarText = adultAltarText +
    //Medallion Areas
        (StartingLightMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_LIGHT_MEDALLION, false)) +
        (StartingForestMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(RG_FOREST_MEDALLION, false)) +
        (StartingFireMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_FIRE_MEDALLION, false)) +
        (StartingWaterMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_WATER_MEDALLION, false)) +
        (StartingSpiritMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(RG_SPIRIT_MEDALLION, false)) +
        (StartingShadowMedallion.Value<uint8_t>() ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(RG_SHADOW_MEDALLION, false));
  }
  adultAltarText = adultAltarText + 
  //Bridge requirement
  BuildBridgeReqsText()+

  //Ganons Boss Key requirement
  BuildGanonBossKeyText()+

  //End
  Hint(RHT_ADULT_ALTAR_TEXT_END).GetText();
  CreateMessageFromTextObject(0x7088, 0, 2, 3, AddColorsAndFormat(adultAltarText, {QM_RED, QM_YELLOW, QM_GREEN, QM_RED, QM_BLUE, QM_YELLOW, QM_PINK, QM_RED, QM_RED, QM_RED, QM_RED}));
  ctx->AddHint(RH_ALTAR_ADULT, adultAltarText, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text());
}

void CreateMerchantsHints() {
    auto ctx = Rando::Context::GetInstance();
    Text medigoronItemText = ctx->GetItemLocation(RC_GC_MEDIGORON)->GetPlacedItem().GetHint().GetText();
    Text grannyItemText = ctx->GetItemLocation(RC_KAK_GRANNYS_SHOP)->GetPlacedItem().GetHint().GetText();
    Text carpetSalesmanItemText =
        ctx->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPlacedItem().GetHint().GetText();
    Text carpetSalesmanItemClearText =
        ctx->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPlacedItem().GetHint().GetClear();

    Text grannyCapitalItemText = grannyItemText.Capitalize();

    Text medigoronText =
        Hint(RHT_MEDIGORON_DIALOG_FIRST).GetText() + medigoronItemText + Hint(RHT_MEDIGORON_DIALOG_SECOND).GetText();
    Text grannyText = grannyCapitalItemText + Hint(RHT_GRANNY_DIALOG).GetText();
    Text carpetSalesmanTextOne = Hint(RHT_CARPET_SALESMAN_DIALOG_FIRST).GetText() + carpetSalesmanItemText +
                                 Hint(RHT_CARPET_SALESMAN_DIALOG_SECOND).GetText();
    Text carpetSalesmanTextTwo = Hint(RHT_CARPET_SALESMAN_DIALOG_THIRD).GetText() + carpetSalesmanItemClearText +
                                 Hint(RHT_CARPET_SALESMAN_DIALOG_FOURTH).GetText();

    CreateMessageFromTextObject(0x9120, 0, 2, 3, AddColorsAndFormat(medigoronText, { QM_RED, QM_GREEN }));
    CreateMessageFromTextObject(0x9121, 0, 2, 3, AddColorsAndFormat(grannyText, { QM_RED, QM_GREEN }));
    CreateMessageFromTextObject(0x6077, 0, 2, 3, AddColorsAndFormat(carpetSalesmanTextOne, { QM_RED, QM_GREEN }));
    CreateMessageFromTextObject(0x6078, 0, 2, 3,
                                AddColorsAndFormat(carpetSalesmanTextTwo, { QM_RED, QM_YELLOW, QM_RED }));
    ctx->AddHint(RH_MEDIGORON, medigoronText, RC_GC_MEDIGORON, HINT_TYPE_STATIC, GetHintRegion(RR_GORON_CITY)->GetHint().GetText());
    ctx->AddHint(RH_GRANNYS_SHOP, grannyText, RC_KAK_GRANNYS_SHOP, HINT_TYPE_STATIC, GetHintRegion(RR_KAKARIKO_VILLAGE)->GetHint().GetText());
    ctx->AddHint(RH_WASTELAND_BOMBCHU_SALESMAN, carpetSalesmanTextOne, RC_WASTELAND_BOMBCHU_SALESMAN, HINT_TYPE_STATIC, GetHintRegion(RR_HAUNTED_WASTELAND)->GetHint().GetText());
}

void CreateDampesDiaryText() {
  auto ctx = Rando::Context::GetInstance();
  if (!DampeHintText) {
    dampesText = Text();
    dampeHintLoc = "";
    return;
  }

  RandomizerGet item = RG_PROGRESSIVE_HOOKSHOT;
  RandomizerCheck location = FilterFromPool(ctx->allLocations, [item, ctx](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == item;
  })[0];
  Text area = GetHintRegion(ctx->GetItemLocation(location)->GetParentRegionKey())->GetHint().GetText();
  Text temp1 = Text{
    "Whoever reads this, please enter %r", 
    "Toi qui lit ce journal, rends-toi dans %r",
    "Wer immer dies liest, der möge folgenden Ort aufsuchen: %r"
  };

  Text temp2 = {
    "%w. I will let you have my stretching, shrinking keepsake.^I'm waiting for you.&--Dampé",
    "%w. Et peut-être auras-tu droit à mon précieux %rtrésor%w.^Je t'attends...&--Igor",
    "%w. Ihm gebe ich meinen langen, kurzen Schatz.^Ich warte!&Boris"
  };
  
  dampesText = temp1 + area + temp2;
  dampeHintLoc = Rando::StaticData::GetLocation(location)->GetName();
  ctx->AddHint(RH_DAMPES_DIARY, dampesText, location, HINT_TYPE_STATIC, area);
}

void CreateGregRupeeHint() {
  auto ctx = Rando::Context::GetInstance();
  if (!GregHintText) {
    gregText = Text();
    return;
  }

  RandomizerCheck location = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_GREG_RUPEE;
  })[0];
  Text area = GetHintRegion(ctx->GetItemLocation(location)->GetParentRegionKey())->GetHint().GetText();

  Text temp1 = Text{
    "By the way, if you're interested, I saw the shiniest %gGreen Rupee%w somewhere in%r ",
    "Au fait, si ça t'intéresse, j'ai aperçu le plus éclatant des %gRubis Verts%w quelque part à %r",
    ""
  };

  Text temp2 = {
    "%w.^It's said to have %cmysterious powers%w...^But then, it could just be another regular rupee.&Oh well.",
    "%w. On dit qu'il possède des pouvoirs mystérieux... Mais bon, ça pourrait juste être un autre rubis ordinaire.",
    ""
  };

    gregText = temp1 + area + temp2;
    ctx->AddHint(RH_GREG_RUPEE, gregText, location, HINT_TYPE_STATIC, area);
}


void CreateSariaText() {
  if(Settings::SariaHintText){
    auto ctx = Rando::Context::GetInstance();
    //Get the location of a magic upgrade
    auto magicLocation = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc){return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_PROGRESSIVE_MAGIC_METER;})[0];
    sariaHintLoc = Rando::StaticData::GetLocation(magicLocation)->GetName();
    Text area = GetHintRegion(ctx->GetItemLocation(magicLocation)->GetParentRegionKey())->GetHint().GetText();
    Text temp1 = Text{
      "Did you feel the %gsurge of magic%w recently? A mysterious bird told me it came from %r",
      "As-tu récemment ressenti une vague de %gpuissance magique%w? Un mystérieux hibou m'a dit  qu'elle provenait du %r",
      ""
    };
    Text temp2 = Text{
      "%w.^You should check that place out, @!$C", 
      "%w. Tu devrais aller y jeter un coup d'oeil, @!$C", 
      "%w.$C"
    };
    sariaText = temp1 + area + temp2;

    if (IsReachableWithout({RC_SARIA_SONG_HINT},magicLocation,true)){
      ctx->GetItemLocation(magicLocation)->SetAsHinted();
    }
  }
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
  auto ctx = Rando::Context::GetInstance();
  SPDLOG_DEBUG("\nNOW CREATING HINTS\n");
  const HintSetting& hintSetting = hintSettingTable[Settings::HintDistribution.Value<uint8_t>()];

  uint8_t remainingDungeonWothHints = hintSetting.dungeonsWothLimit;
  uint8_t remainingDungeonBarrenHints = hintSetting.dungeonsBarrenLimit;

  // Apply Special hint exclusions with no requirements
  if (Settings::Kak10GSHintText){
      Location(KAK_10_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (Settings::Kak20GSHintText){
      Location(KAK_20_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (Settings::Kak30GSHintText){
      Location(KAK_30_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (Settings::Kak40GSHintText){
      Location(KAK_40_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (Settings::Kak50GSHintText){
      Location(KAK_50_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (Settings::skipChildZelda){
      Location(SONG_FROM_IMPA)->SetAsHinted();
  }

  // Add 'always' location hints
  if (hintSetting.distTable[static_cast<int>(HINT_TYPE_ALWAYS)].copies > 0) {
      // Only filter locations that had a random item placed at them (e.g. don't get cow locations if shuffle cows is
      // off)
      auto alwaysHintLocations = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
          return ((Rando::StaticData::GetLocation(loc)->GetHint()->GetType() == HintCategory::Always) ||
                  // If we have Rainbow Bridge set to Greg, add a hint for where Greg is
                  (Bridge.Is(RAINBOWBRIDGE_GREG) &&
                   ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_GREG_RUPEE)) &&
                 ctx->GetItemLocation(loc)->IsHintable() && !(ctx->GetItemLocation(loc)->IsHintedAt());
      });

      for (auto& hint : conditionalAlwaysHints) {
          RandomizerCheck loc = hint.first;
          if (hint.second() && ctx->GetItemLocation(loc)->IsHintable()) {
              alwaysHintLocations.push_back(loc);
          }
      }

      for (RandomizerCheck location : alwaysHintLocations) {
          CreateLocationHint({ location });
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
  for (RandomizerCheck barrenLocation : barrenLocations) {
    std::string barrenRegion = GetHintRegion(ctx->GetItemLocation(barrenLocation)->GetParentRegionKey())->scene;
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
  for (RandomizerCheck wothLocation : ctx->wothLocations) {
    std::string wothRegion = GetHintRegion(ctx->GetItemLocation(wothLocation)->GetParentRegionKey())->scene;
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
  while (FilterFromPool(Rando::StaticData::gossipStoneLocations, [ctx](const RandomizerCheck loc) {
             return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_NONE;
         }).size() != 0) {
      // TODO: fixed hint types

      if (remainingHintTypes.empty()) {
          break;
      }

      // get a random hint type from the remaining hints
      HintType type = RandomElement(remainingHintTypes, true);

      SPDLOG_DEBUG("Attempting to make hint of type: ");
      SPDLOG_DEBUG(hintTypeNames.find(type)->second);
      SPDLOG_DEBUG("\n");

      // create the appropriate hint for the type
      if (type == HINT_TYPE_WOTH) {
          CreateWothHint(&remainingDungeonWothHints);

      } else if (type == HINT_TYPE_BARREN) {
          CreateBarrenHint(&remainingDungeonBarrenHints, barrenLocations);

      } else if (type == HINT_TYPE_SOMETIMES) {
          std::vector<RandomizerCheck> sometimesHintLocations =
              FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
                  return ctx->GetHint(ctx->GetItemLocation(loc)->GetHintKey())->GetHintType() == HINT_TYPE_SOMETIMES &&
                         ctx->GetItemLocation(loc)->IsHintable() && !(ctx->GetItemLocation(loc)->IsHintedAt());
              });
          CreateLocationHint(sometimesHintLocations);

      } else if (type == HINT_TYPE_RANDOM) {
          CreateRandomLocationHint();

      } else if (type == HINT_TYPE_ITEM) {
        CreateRandomLocationHint(true);

      } else if (type == HINT_TYPE_SONG) {
          std::vector<RandomizerCheck> songHintLocations =
              FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
                  return Rando::StaticData::GetLocation(loc)->IsCategory(Category::cSong) &&
                         ctx->GetItemLocation(loc)->IsHintable() && !(ctx->GetItemLocation(loc)->IsHintedAt());
              });
          CreateLocationHint(songHintLocations);

      } else if (type == HINT_TYPE_OVERWORLD) {
          std::vector<RandomizerCheck> overworldHintLocations =
              FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
                  return Rando::StaticData::GetLocation(loc)->IsOverworld() && ctx->GetItemLocation(loc)->IsHintable() &&
                         !(ctx->GetItemLocation(loc)->IsHintedAt());
              });
          CreateLocationHint(overworldHintLocations);

      } else if (type == HINT_TYPE_DUNGEON) {
          std::vector<RandomizerCheck> dungeonHintLocations =
              FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
                  return Rando::StaticData::GetLocation(loc)->IsDungeon() && ctx->GetItemLocation(loc)->IsHintable() &&
                         !(ctx->GetItemLocation(loc)->IsHintedAt());
              });
          CreateLocationHint(dungeonHintLocations);

      } else if (type == HINT_TYPE_JUNK) {
          CreateJunkHint();
      }
  }

  //If any gossip stones failed to have a hint placed on them for some reason, place a junk hint as a failsafe.
  for (RandomizerCheck gossipStone : FilterFromPool(Rando::StaticData::gossipStoneLocations, [ctx](const RandomizerCheck loc) {
           return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_NONE;
       })) {
      const HintText junkHint = RandomElement(GetHintCategory(HintCategory::Junk));
      AddHint(junkHint.GetText(), gossipStone, { QM_PINK });
  }

  //Getting gossip stone locations temporarily sets one location to not be reachable.
  //Call the function one last time to get rid of false positives on locations not
  //being reachable.
  GetAccessibleLocations({});
}

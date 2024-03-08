#include "hints.hpp"

#include "item_pool.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "fill.hpp"
#include "hint_list.hpp"
#include "../trial.h"
#include "../entrance.h"
#include "z64item.h"
#include <spdlog/spdlog.h>
#include "../randomizerTypes.h"
#include "../context.h"
#include "pool_functions.hpp"
#include "../hint.h"


using namespace Rando;

const Text& HintText::GetText() const {
  auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_OBSCURE)) {
        return GetObscure();
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)) {
        return GetAmbiguous();
    } else {
        return GetClear();
    }
}

const Text HintText::GetTextCopy() const {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_OBSCURE)) {
        return GetObscure();
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)) {
        return GetAmbiguous();
    } else {
        return GetClear();
    }
}

RandomizerHintTextKey GetRandomJunkHint(){ 
  //temp code to handle random junk hints now I work in keys instead of a vector of HintText
  // Will be replaced with a better system once more customisable hint pools are added
  uint32_t range = RHT_JUNK_SG_8 - RHT_JUNK02;
  return (RandomizerHintTextKey)(Random(0, range) + RHT_JUNK02);
}

RandomizerHintTextKey GetRandomGanonJoke(){ 
  uint32_t range = RHT_GANON_JOKE11 - RHT_GANON_JOKE01;
  return (RandomizerHintTextKey)(Random(0, range) + RHT_GANON_JOKE01);
}

std::array<std::pair<RandomizerCheck,RandomizerSettingKey>, 14> staticHintLocations = {{
  {RC_KAK_10_GOLD_SKULLTULA_REWARD,  RSK_KAK_10_SKULLS_HINT},
  {RC_KAK_20_GOLD_SKULLTULA_REWARD,  RSK_KAK_20_SKULLS_HINT},
  {RC_KAK_30_GOLD_SKULLTULA_REWARD,  RSK_KAK_30_SKULLS_HINT},
  {RC_KAK_40_GOLD_SKULLTULA_REWARD,  RSK_KAK_40_SKULLS_HINT},
  {RC_KAK_50_GOLD_SKULLTULA_REWARD,  RSK_KAK_50_SKULLS_HINT},
  {RC_KAK_100_GOLD_SKULLTULA_REWARD, RSK_KAK_100_SKULLS_HINT},
  {RC_SONG_FROM_IMPA,                RSK_SKIP_CHILD_ZELDA},
  {RC_ZR_FROGS_OCARINA_GAME,         RSK_FROGS_HINT},
  {RC_DMT_TRADE_CLAIM_CHECK,         RSK_BIGGORON_HINT},
  {RC_MARKET_10_BIG_POES,            RSK_BIG_POES_HINT},
  {RC_KAK_ANJU_AS_CHILD,             RSK_CHICKENS_HINT},
  {RC_KF_LINKS_HOUSE_COW,            RSK_MALON_HINT},
  {RC_GF_HBA_1000_POINTS,            RSK_HBA_HINT},
  {RC_GF_HBA_1500_POINTS,            RSK_HBA_HINT},
  }};

std::unordered_map<RandomizerCheck, RandomizerHint> gossipStoneCheckToHint {
  {RC_COLOSSUS_GOSSIP_STONE,                 RH_COLOSSUS_GOSSIP_STONE},
  {RC_DMC_GOSSIP_STONE,                      RH_DMC_GOSSIP_STONE},
  {RC_DMC_UPPER_GROTTO_GOSSIP_STONE,         RH_DMC_UPPER_GROTTO_GOSSIP_STONE},
  {RC_DMT_GOSSIP_STONE,                      RH_DMT_GOSSIP_STONE},
  {RC_DMT_STORMS_GROTTO_GOSSIP_STONE,        RH_DMT_STORMS_GROTTO_GOSSIP_STONE},
  {RC_DODONGOS_CAVERN_GOSSIP_STONE,          RH_DODONGOS_CAVERN_GOSSIP_STONE},
  {RC_ZF_FAIRY_GOSSIP_STONE,                 RH_ZF_FAIRY_GOSSIP_STONE},
  {RC_GC_MAZE_GOSSIP_STONE,                  RH_GC_MAZE_GOSSIP_STONE},
  {RC_GC_MEDIGORON_GOSSIP_STONE,             RH_GC_MEDIGORON_GOSSIP_STONE},
  {RC_GV_GOSSIP_STONE,                       RH_GV_GOSSIP_STONE},
  {RC_GRAVEYARD_GOSSIP_STONE,                RH_GRAVEYARD_GOSSIP_STONE},
  {RC_HC_MALON_GOSSIP_STONE,                 RH_HC_MALON_GOSSIP_STONE},
  {RC_HC_ROCK_WALL_GOSSIP_STONE,             RH_HC_ROCK_WALL_GOSSIP_STONE},
  {RC_HC_STORMS_GROTTO_GOSSIP_STONE,         RH_HC_STORMS_GROTTO_GOSSIP_STONE},
  {RC_HF_COW_GROTTO_GOSSIP_STONE,            RH_HF_COW_GROTTO_GOSSIP_STONE},
  {RC_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE,    RH_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE},
  {RC_HF_OPEN_GROTTO_GOSSIP_STONE,           RH_HF_OPEN_GROTTO_GOSSIP_STONE},
  {RC_HF_SOUTHEAST_GROTTO_GOSSIP_STONE,      RH_HF_SOUTHEAST_GROTTO_GOSSIP_STONE},
  {RC_ZF_JABU_GOSSIP_STONE,                  RH_ZF_JABU_GOSSIP_STONE},
  {RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,        RH_KF_DEKU_TREE_LEFT_GOSSIP_STONE},
  {RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE,       RH_KF_DEKU_TREE_RIGHT_GOSSIP_STONE},
  {RC_KF_GOSSIP_STONE,                       RH_KF_GOSSIP_STONE},
  {RC_KF_STORMS_GROTTO_GOSSIP_STONE,         RH_KF_STORMS_GROTTO_GOSSIP_STONE},
  {RC_KAK_OPEN_GROTTO_GOSSIP_STONE,          RH_KAK_OPEN_GROTTO_GOSSIP_STONE},
  {RC_LH_LAB_GOSSIP_STONE,                   RH_LH_LAB_GOSSIP_STONE},
  {RC_LH_SOUTHEAST_GOSSIP_STONE,             RH_LH_SOUTHEAST_GOSSIP_STONE},
  {RC_LH_SOUTHWEST_GOSSIP_STONE,             RH_LH_SOUTHWEST_GOSSIP_STONE},
  {RC_LW_GOSSIP_STONE,                       RH_LW_GOSSIP_STONE},
  {RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE, RH_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE},
  {RC_SFM_MAZE_LOWER_GOSSIP_STONE,           RH_SFM_MAZE_LOWER_GOSSIP_STONE},
  {RC_SFM_MAZE_UPPER_GOSSIP_STONE,           RH_SFM_MAZE_UPPER_GOSSIP_STONE},
  {RC_SFM_SARIA_GOSSIP_STONE,                RH_SFM_SARIA_GOSSIP_STONE},
  {RC_TOT_LEFT_CENTER_GOSSIP_STONE,          RH_TOT_LEFT_CENTER_GOSSIP_STONE},
  {RC_TOT_LEFTMOST_GOSSIP_STONE,             RH_TOT_LEFTMOST_GOSSIP_STONE},
  {RC_TOT_RIGHT_CENTER_GOSSIP_STONE,         RH_TOT_RIGHT_CENTER_GOSSIP_STONE},
  {RC_TOT_RIGHTMOST_GOSSIP_STONE,            RH_TOT_RIGHTMOST_GOSSIP_STONE},
  {RC_ZD_GOSSIP_STONE,                       RH_ZD_GOSSIP_STONE},
  {RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,           RH_ZR_NEAR_DOMAIN_GOSSIP_STONE},
  {RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,          RH_ZR_NEAR_GROTTOS_GOSSIP_STONE},
  {RC_ZR_OPEN_GROTTO_GOSSIP_STONE,           RH_ZR_OPEN_GROTTO_GOSSIP_STONE}
};

bool FilterWotHLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return ctx->GetItemLocation(loc)->IsWothCandidate();
}

bool FilterBarrenLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return ctx->GetItemLocation(loc)->IsBarrenCandidate();
}

bool FilterSongLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return Rando::StaticData::GetLocation(loc)->IsCategory(Category::cSong);
}

bool FilterOverworldLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return Rando::StaticData::GetLocation(loc)->IsOverworld();
}

bool FilterDungeonLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return Rando::StaticData::GetLocation(loc)->IsDungeon();
}

bool FilterGoodItems(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return ctx->GetItemLocation(loc)->GetPlacedItem().IsMajorItem();
}

bool NoFilter(RandomizerCheck loc){
  return true;
}

const std::array<HintSetting, 4> hintSettingTable{{
  // Useless hints
  {
    .alwaysCopies = 0,
    .trialCopies = 0,
    .junkWeight = 1, //RANDOTODO when the hint pool is not implicitly an itemLocations, handle junk like other hint types
    .distTable = {} /*RANDOTODO Instead of loading a function into this,
    apply this filter on all possible hintables in advance and then filter by what is acually in the seed at the start of generation.
    This allows the distTable to hold the current status in hint generation (reducing potential doubled work) and
    will make handling custom hint pools easier later*/
  },
  // Balanced hints
  {
    .alwaysCopies = 1,
    .trialCopies = 1,
    .junkWeight = 6,
    .distTable = {
      {"WotH",       HINT_TYPE_WOTH,          7,  0, 1, FilterWotHLocations,      2},
      {"Barren",     HINT_TYPE_FOOLISH,        4,  0, 1, FilterBarrenLocations,    1},
      //("Entrance",   HINT_TYPE_ENTRANCE,      6,  0, 1), //not yet implemented
      {"Song",       HINT_TYPE_ITEM_LOCATION, 2,  0, 1, FilterSongLocations},
      {"Overworld",  HINT_TYPE_ITEM_LOCATION, 4,  0, 1, FilterOverworldLocations},
      {"Dungeon",    HINT_TYPE_ITEM_LOCATION, 3,  0, 1, FilterDungeonLocations},
      {"Named Item", HINT_TYPE_ITEM_AREA,    10,  0, 1, FilterGoodItems},
      {"Random"    , HINT_TYPE_ITEM_AREA,    12,  0, 1, NoFilter}
    }
  },
  // Strong hints
  {
    .alwaysCopies = 2,
    .trialCopies = 1,
    .junkWeight = 0,
    .distTable = {
      {"WotH",       HINT_TYPE_WOTH,         12, 0, 2, FilterWotHLocations,      2},
      {"Barren",     HINT_TYPE_FOOLISH,       12, 0, 1, FilterBarrenLocations,    1},
      //{"Entrance",   HINT_TYPE_ENTRANCE,      4, 0, 1}, //not yet implemented
      {"Song",       HINT_TYPE_ITEM_LOCATION, 4, 0, 1, FilterSongLocations},
      {"Overworld",  HINT_TYPE_ITEM_LOCATION, 6, 0, 1, FilterOverworldLocations},
      {"Dungeon",    HINT_TYPE_ITEM_LOCATION, 6, 0, 1, FilterDungeonLocations},
      {"Named Item", HINT_TYPE_ITEM_AREA,     8, 0, 1, FilterGoodItems},
      {"Random"    , HINT_TYPE_ITEM_AREA,     8, 0, 1, NoFilter},
    },
  },
  // Very strong hints
  {
    .alwaysCopies = 2,
    .trialCopies = 1,
    .junkWeight = 0,
    .distTable = {
      {"WotH",       HINT_TYPE_WOTH,         15, 0, 2, FilterWotHLocations},
      {"Barren",     HINT_TYPE_FOOLISH,       15, 0, 1, FilterBarrenLocations},
      //{"Entrance",   HINT_TYPE_ENTRANCE,     10, 0, 1}, //not yet implemented
      {"Song",       HINT_TYPE_ITEM_LOCATION, 2, 0, 1, FilterSongLocations},
      {"Overworld",  HINT_TYPE_ITEM_LOCATION, 7, 0, 1, FilterOverworldLocations},
      {"Dungeon",    HINT_TYPE_ITEM_LOCATION, 7, 0, 1, FilterDungeonLocations},
      {"Named Item", HINT_TYPE_ITEM_AREA,     5, 0, 1, FilterGoodItems},
    },
  },
}};

std::array<DungeonHintInfo, 10> dungeonInfoData;

static std::vector<RandomizerCheck> GetEmptyGossipStones() {
  auto emptyGossipStones = GetEmptyLocations(Rando::StaticData::gossipStoneLocations);
  return emptyGossipStones;
}

static std::vector<RandomizerCheck> GetAccessibleGossipStones(const RandomizerCheck hintedLocation = RC_GANON) {
  auto ctx = Rando::Context::GetInstance();
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for gossip stone locations.
  RandomizerGet originalItem = ctx->GetItemLocation(hintedLocation)->GetPlacedRandomizerGet();
  ctx->GetItemLocation(hintedLocation)->SetPlacedItem(RG_NONE);

  ctx->GetLogic()->Reset();
  auto accessibleGossipStones = GetAccessibleLocations(Rando::StaticData::gossipStoneLocations);
  //Give the item back to the location
  ctx->GetItemLocation(hintedLocation)->SetPlacedItem(originalItem);

  return accessibleGossipStones;
}

bool IsReachableWithout(std::vector<RandomizerCheck> locsToCheck, RandomizerCheck excludedCheck, bool resetAfter = true){
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for this check RANDOTODO convert excludedCheck to an ItemLocation
  auto ctx = Rando::Context::GetInstance();
  RandomizerGet originalItem = ctx->GetItemLocation(excludedCheck)->GetPlacedRandomizerGet();
  ctx->GetItemLocation(excludedCheck)->SetPlacedItem(RG_NONE);
  ctx->GetLogic()->Reset();
  const auto rechableWithout = GetAccessibleLocations(locsToCheck);
  ctx->GetItemLocation(excludedCheck)->SetPlacedItem(originalItem);
  if (resetAfter){
    //if resetAfter is on, reset logic we are done
    ctx->GetLogic()->Reset();
  }
  if (rechableWithout.empty()) {
    return false;
  }
  return true;
}

static void SetAllInRegionAsHinted(RandomizerArea area, std::vector<RandomizerCheck> locations){
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> locsInRegion = FilterFromPool(locations, [area, ctx](const RandomizerCheck loc){
                                        return ctx->GetItemLocation(loc)->GetArea() == area;
                                      });
  for(RandomizerCheck loc: locsInRegion){
    ctx->GetItemLocation(loc)->SetAsHinted();
  }
}

static void AddGossipStoneHint( const RandomizerCheck gossipStone,
                                const HintType hintType,
                                const std::string distributionName,
                                const std::vector<RandomizerHintTextKey> hintKeys,
                                const std::vector<RandomizerCheck> locations,
                                const std::vector<uint8_t>& colors,
                                const std::vector<TrialKey> trials) {

  auto ctx = Rando::Context::GetInstance();
  ctx->AddHint(gossipStoneCheckToHint[gossipStone], Hint(gossipStoneCheckToHint[gossipStone], hintType, distributionName, hintKeys, locations, colors, trials));
  ctx->GetItemLocation(gossipStone)->SetPlacedItem(RG_HINT); //RANDOTODO, better gossip stone to location to hint key system
}

static void AddGossipStoneHintCopies(uint8_t copies,
                                     const HintType hintType,
                                     const std::string distributionName,
                                     const std::vector<RandomizerHintTextKey> hintKeys = {},
                                     const std::vector<RandomizerCheck> locations = {},
                                     const std::vector<uint8_t>& colors = {},
                                     const std::vector<TrialKey> trials = {},
                                     RandomizerCheck firstStone = RC_UNKNOWN_CHECK){

  if (firstStone != RC_UNKNOWN_CHECK && copies > 0){
      AddGossipStoneHint(firstStone, hintType, distributionName, hintKeys, locations, colors, trials);
      copies -= 1;
  }
  for(int c=0; c<copies; c++){
    //get a random gossip stone
    auto gossipStones = GetEmptyGossipStones();
    if (gossipStones.empty()) {
      SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT\n\n");
      return;
    }
    auto gossipStone = RandomElement(gossipStones, false);
    AddGossipStoneHint(gossipStone, hintType, distributionName, hintKeys, locations, colors, trials);
  }
}

static bool CreateHint(RandomizerCheck location, uint8_t copies, HintType type, std::string distribution){
  auto ctx = Rando::Context::GetInstance();

  //get a gossip stone accessible without the hinted item
  std::vector<RandomizerCheck> gossipStoneLocations = GetAccessibleGossipStones(location);
  if (gossipStoneLocations.empty()) { 
      SPDLOG_DEBUG("\tNO IN LOGIC GOSSIP STONE\n\n"); 
      return false;
  }
  RandomizerCheck gossipStone = RandomElement(gossipStoneLocations);

  //make hint text
  std::vector<uint8_t> colors;
  if (type == HINT_TYPE_WOTH){
    colors = {QM_RED, QM_LBLUE};
  }
  else if(type == HINT_TYPE_FOOLISH){
    colors = {QM_RED, QM_PINK};
  }
  else if (type == HINT_TYPE_ITEM_LOCATION){
    colors = {QM_RED, QM_GREEN};
  }
  else if (type == HINT_TYPE_ITEM_AREA){
    if (Rando::StaticData::GetLocation(location)->IsDungeon()) {
      colors = {QM_RED, QM_GREEN};
    } else {
      colors = {QM_GREEN, QM_RED};
    }
  } 

  ctx->GetItemLocation(location)->SetAsHinted();

  AddGossipStoneHintCopies(copies, type, distribution, {}, {location}, colors, {}, gossipStone);
  return true;
}


static RandomizerCheck CreateRandomHint(std::vector<RandomizerCheck>& possibleHintLocations,
                                        uint8_t copies,
                                        HintType type,
                                        std::string distributionName) {
  auto ctx = Rando::Context::GetInstance();

  //return if there aren't any hintable locations or gossip stones available
  if (GetEmptyGossipStones().size() < copies) {
    SPDLOG_DEBUG("\tNOT ENOUGH GOSSIP STONES TO PLACE HINTS\n\n");
    return RC_UNKNOWN_CHECK;
  }

  RandomizerCheck hintedLocation;
  bool placed = false;
  while (!placed){
    if (possibleHintLocations.empty()) {
        SPDLOG_DEBUG("\tNO LOCATIONS TO HINT\n\n");
      return RC_UNKNOWN_CHECK;
    }
    hintedLocation = RandomElement(possibleHintLocations, true); //removing the location to avoid it being hinted again on fail
   
    SPDLOG_DEBUG("\tLocation: ");
    SPDLOG_DEBUG(Rando::StaticData::GetLocation(hintedLocation)->GetName());
    SPDLOG_DEBUG("\n");

    SPDLOG_DEBUG("\tItem: ");
    SPDLOG_DEBUG(ctx->GetItemLocation(hintedLocation)->GetPlacedItemName().GetEnglish());
    SPDLOG_DEBUG("\n");

    placed = CreateHint(hintedLocation, copies, type, distributionName);
  }
  if (type == HINT_TYPE_FOOLISH){
     SetAllInRegionAsHinted(ctx->GetItemLocation(hintedLocation)->GetArea(), possibleHintLocations);
  }
  return hintedLocation;
}

static std::vector<RandomizerCheck> FilterHintability(std::vector<RandomizerCheck>& locations,
                                                      std::function<bool(RandomizerCheck)> extraFilter = NoFilter){
  auto ctx = Rando::Context::GetInstance();
  return FilterFromPool(locations, [extraFilter, ctx](const RandomizerCheck loc) {
    return ctx->GetItemLocation(loc)->IsHintable() && !(ctx->GetItemLocation(loc)->IsHintedAt())
    && extraFilter(loc);
  });
}

static void CreateJunkHints(uint8_t numHints) {
  for(uint8_t c = 0; c < numHints; c++){
    //duplicate junk hints are possible for now
    AddGossipStoneHintCopies(1, HINT_TYPE_TEXT, "Junk", {GetRandomJunkHint()});
  }
}

static void CreateTrialHints(uint8_t copies) {
  if (copies > 0) {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_TRIAL_COUNT).Is(6)) {//six trials
      AddGossipStoneHintCopies(copies, HINT_TYPE_TEXT, "Trial", {RHT_SIX_TRIALS}, {}, {QM_PINK});
    } else if (ctx->GetOption(RSK_TRIAL_COUNT).Is(0)) {//zero trials
      AddGossipStoneHintCopies(copies, HINT_TYPE_TEXT, "Trial", {RHT_ZERO_TRIALS}, {}, {QM_YELLOW});
    } else {
      std::vector<uint8_t> colors;
      std::vector<TrialInfo*> trials = ctx->GetTrials()->GetTrialList(); //there's probably a way to remove this assignment
      if (ctx->GetOption(RSK_TRIAL_COUNT).Value<uint8_t>() >= 4) {//4 or 5 required trials, get skipped trials
        trials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsSkipped();});
        colors = {QM_YELLOW};
      } else {//1 to 3 trials, get requried trials
        auto requiredTrials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsRequired();});
        colors = {QM_PINK};
      }
      for (auto& trial : trials) {//create a hint for each hinted trial
        AddGossipStoneHintCopies(copies, HINT_TYPE_TRIAL, "Trial", {}, {}, colors, {trial->GetKey()});
      }
    }
  }
}

void CreateWarpSongTexts() {
  auto ctx = Rando::Context::GetInstance();
  if (ctx->GetOption(RSK_SHUFFLE_WARP_SONGS)){
    auto warpSongEntrances = GetShuffleableEntrances(EntranceType::WarpSong, false);
    for (auto entrance : warpSongEntrances) {
      auto destination = entrance->GetConnectedRegion()->GetArea();
      switch (entrance->GetIndex()) {
        case 0x0600: // minuet
          ctx->AddHint(RH_MINUET_WARP_LOC,
                       StaticHint(RH_MINUET_WARP_LOC, HINT_TYPE_STATIC_ENTRANCE, {RHT_WARP_SONG}, {destination}));
          break;
        case 0x04F6: // bolero
          ctx->AddHint(RH_BOLERO_WARP_LOC,
                StaticHint(RH_BOLERO_WARP_LOC, HINT_TYPE_STATIC_ENTRANCE, {RHT_WARP_SONG}, {destination}));
          break;
        case 0x0604: // serenade
          ctx->AddHint(RH_SERENADE_WARP_LOC,
                StaticHint(RH_SERENADE_WARP_LOC, HINT_TYPE_STATIC_ENTRANCE, {RHT_WARP_SONG}, {destination}));
          break;
        case 0x01F1: // requiem
          ctx->AddHint(RH_REQUIEM_WARP_LOC,
                StaticHint(RH_REQUIEM_WARP_LOC, HINT_TYPE_STATIC_ENTRANCE, {RHT_WARP_SONG}, {destination}));
          break;
        case 0x0568: // nocturne
          ctx->AddHint(RH_NOCTURNE_WARP_LOC,
                StaticHint(RH_NOCTURNE_WARP_LOC, HINT_TYPE_STATIC_ENTRANCE, {RHT_WARP_SONG}, {destination}));
          break;
        case 0x05F4: // prelude
          ctx->AddHint(RH_PRELUDE_WARP_LOC,
                StaticHint(RH_PRELUDE_WARP_LOC, HINT_TYPE_STATIC_ENTRANCE, {RHT_WARP_SONG}, {destination}));
          break;
        default:
          break;
      }
    }
  }
}

int32_t getRandomWeight(int32_t totalWeight){
  if (totalWeight <= 1){
    return 1;
  }
  return Random(1,totalWeight);
}

static void DistributeHints(std::vector<uint8_t>& selected, size_t stoneCount, std::vector<HintDistributionSetting> distTable, uint8_t junkWieght, bool addFixed = true){
  int32_t totalWeight = junkWieght;

  for (HintDistributionSetting setting: distTable){
    totalWeight += setting.weight;
    if (addFixed){
      selected[setting.type] += setting.fixed;
      stoneCount -= setting.fixed * setting.copies;
    }
  }
  int32_t currentWeight = getRandomWeight(totalWeight);
  while(stoneCount > 0 && totalWeight > 0){
    for (uint8_t distribution = 0; distribution < distTable.size(); distribution++){
      currentWeight -= distTable[distribution].weight;
      if (currentWeight <= 0){
        if (stoneCount >= distTable[distribution].copies){
          selected[distribution] += 1;
          stoneCount -= distTable[distribution].copies;
          break;
        }
        else {
          totalWeight -= distTable[distribution].weight;
          distTable[distribution].weight = 0;
          break;
        }
      }
    }
    //if there's still weight, then it's junk
    if (currentWeight > 0){
      selected[selected.size()-1] += 1;
      stoneCount -= 1;
    }
    currentWeight = getRandomWeight(totalWeight);
  }
  //if stones are left, assign junk
  if (stoneCount > 0){
    selected[selected.size()-1] += stoneCount;
  }
}

uint8_t PlaceHints(std::vector<uint8_t>& selectedHints,
                std::vector<HintDistributionSetting>& distTable){
  auto ctx = Rando::Context::GetInstance();
  uint8_t curSlot = 0;
  for (HintDistributionSetting distribution : distTable){
    std::vector<RandomizerCheck> hintTypePool = FilterHintability(ctx->allLocations, distribution.filter);
    for (uint8_t numHint = 0; numHint < selectedHints[curSlot]; numHint++){

      SPDLOG_DEBUG("Attempting to make hint of type: ");
      SPDLOG_DEBUG(hintTypeNames[static_cast<int>(distribution.type)]);
      SPDLOG_DEBUG("\n");

      RandomizerCheck hintedLocation = RC_UNKNOWN_CHECK;

      hintedLocation = CreateRandomHint(hintTypePool, distribution.copies, distribution.type, distribution.name);
      if (hintedLocation == RC_UNKNOWN_CHECK){ //if hint failed to place
        uint8_t hintsToRemove = (selectedHints[curSlot] - numHint) * distribution.copies;
        selectedHints[curSlot] = 0;
        distTable[curSlot].copies = 0;
        return hintsToRemove;
      }
      if(Rando::StaticData::GetLocation(hintedLocation)->IsDungeon()){
        distribution.dungeonLimit -= 1;
        if (distribution.dungeonLimit == 0){
          FilterFromPool(hintTypePool, FilterOverworldLocations);
        }
      }
    }
    selectedHints[curSlot] = 0;
    curSlot += 1;
  }
  CreateJunkHints(selectedHints[selectedHints.size() - 1]);
  return 0;
}

void CreateStoneHints() {
  auto ctx = Rando::Context::GetInstance();
  SPDLOG_DEBUG("\nNOW CREATING HINTS\n");
  const HintSetting& hintSetting = hintSettingTable[ctx->GetOption(RSK_HINT_DISTRIBUTION).Value<uint8_t>()];
  std::vector<HintDistributionSetting> distTable = hintSetting.distTable;

  // Apply Static hint exclusions with no in-game requirements
  for (int c = 0; c < staticHintLocations.size(); c++){
      if(ctx->GetOption(staticHintLocations[c].second)){
          ctx->GetItemLocation(staticHintLocations[c].first)->SetAsHinted();
      }
  }

  // Add 'always' location hints
  if (hintSetting.alwaysCopies > 0) {
      // Only filter locations that had a random item placed at them (e.g. don't get cow locations if shuffle cows is
      // off)
      auto alwaysHintLocations = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
          return ((Rando::StaticData::GetLocation(loc)->GetHint()->GetType() == HintCategory::Always) ||
                  // If we have Rainbow Bridge set to Greg, add a hint for where Greg is
                  (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG) &&
                   ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_GREG_RUPEE)) &&
                  ctx->GetItemLocation(loc)->IsHintable() &&
                  !(ctx->GetOption(RSK_GREG_HINT) && (IsReachableWithout({RC_GREG_HINT}, loc, true)));
      });

      for (auto& hint : conditionalAlwaysHints) {
          RandomizerCheck loc = hint.first;
          if (hint.second() && ctx->GetItemLocation(loc)->IsHintable()) {
              alwaysHintLocations.push_back(loc);
          }
      }

    for (RandomizerCheck location : alwaysHintLocations) {
      CreateHint(location, hintSetting.alwaysCopies, HINT_TYPE_ITEM_LOCATION, "Always");
    }
  }

  //Add 'trial' location hints
  if (ctx->GetOption(RSK_GANONS_TRIALS).IsNot(RO_GANONS_TRIALS_SKIP)) {
    CreateTrialHints(hintSetting.trialCopies);
  }

  size_t totalStones = GetEmptyGossipStones().size();
  std::vector<uint8_t> selectedHints = {};
  for (uint8_t c=0; c < distTable.size(); c++){
    selectedHints.push_back(0);
  }
  selectedHints.push_back(0);
  DistributeHints(selectedHints, totalStones, distTable, hintSetting.junkWeight);

  while(totalStones != 0){
    totalStones = PlaceHints(selectedHints, distTable);
    if (totalStones != 0){
      DistributeHints(selectedHints, totalStones, distTable, hintSetting.junkWeight, false);
    }
  }

  //Getting gossip stone locations temporarily sets one location to not be reachable.
  //Call the function one last time to get rid of false positives on locations not
  //being reachable.
  GetAccessibleLocations({});
}

std::vector<RandomizerCheck> FindItemsAndMarkHinted(std::vector<RandomizerGet> items, std::vector<RandomizerCheck> hintChecks){
  std::vector<RandomizerCheck> locations = {};
  auto ctx = Rando::Context::GetInstance();
  for (uint8_t c = 0; c < items.size(); c++){
    std::vector<RandomizerCheck> found = FilterFromPool(ctx->allLocations, [items, ctx, c](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == items[c];});
    if (found.size() > 0){
      locations.push_back(found[0]);
    }
    if (!ctx->GetItemLocation(found[0])->IsHintedAt() && IsReachableWithout(hintChecks,found[0],true)){
      ctx->GetItemLocation(found[0])->SetAsHinted();
    }
  }
}

void CreateAltarText() {
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> stoneLocs = {};
  std::vector<RandomizerCheck> medallionLocs = {};

  if (ctx->GetOption(RSK_TOT_ALTAR_HINT)) {
    stoneLocs = FindItemsAndMarkHinted({RG_KOKIRI_EMERALD, RG_GORON_RUBY, RG_ZORA_SAPPHIRE}, {RC_ALTAR_HINT_CHILD});
    medallionLocs = FindItemsAndMarkHinted({RG_LIGHT_MEDALLION, RG_FOREST_MEDALLION, RG_FIRE_MEDALLION, RG_WATER_MEDALLION, RG_SPIRIT_MEDALLION, RG_SHADOW_MEDALLION},
                                            {RC_ALTAR_HINT_ADULT});
  }
  ctx->AddHint(RH_ALTAR_CHILD, AltarHint(RH_ALTAR_CHILD, HINT_TYPE_ALTAR_CHILD, stoneLocs, {QM_GREEN, QM_RED, QM_BLUE}));
  ctx->AddHint(RH_ALTAR_ADULT, AltarHint(RH_ALTAR_ADULT, HINT_TYPE_ALTAR_ADULT, medallionLocs, { QM_RED, QM_YELLOW, QM_GREEN, QM_RED, QM_BLUE, QM_YELLOW, QM_PINK, QM_RED, QM_RED, QM_RED, QM_RED }));
}

void CreateStaticLocationHints() {
  auto ctx = Rando::Context::GetInstance();
  if (!ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_OFF)){

    ctx->AddHint(RH_MEDIGORON, 
                 StaticHint(RH_MEDIGORON, HINT_TYPE_MERCHANT, {RHT_MEDIGORON_HINT}, {RC_GC_MEDIGORON}, {QM_GREEN, QM_YELLOW, QM_GREEN}, false));

    ctx->AddHint(RH_GRANNY, 
                 StaticHint(RH_GRANNY, HINT_TYPE_MERCHANT, {RHT_GRANNY_HINT}, {RC_KAK_GRANNYS_SHOP}, {QM_GREEN, QM_YELLOW, QM_GREEN}, false, true));
    
    ctx->AddHint(RH_CARPET_SALESMAN, 
                 StaticHint(RH_CARPET_SALESMAN, HINT_TYPE_MERCHANT, {RHT_CARPET_SALESMAN_DIALOG_HINTED}, {RC_WASTELAND_BOMBCHU_SALESMAN}, {QM_RED, QM_GREEN, QM_YELLOW, QM_GREEN}, false));
  }

  if (ctx->GetOption(RSK_SHUFFLE_MAGIC_BEANS)){
    ctx->AddHint(RH_BEAN_SALESMAN, 
                 StaticHint(RH_BEAN_SALESMAN, HINT_TYPE_MERCHANT, {RHT_BEAN_SALESMAN_HINT}, {RC_ZR_MAGIC_BEAN_SALESMAN}, {QM_RED, QM_GREEN, QM_YELLOW, QM_GREEN}, false));
  }

  if (ctx->GetOption(RSK_HBA_HINT)){
    ctx->AddHint(RH_HBA_HINT, 
                 StaticHint(RH_HBA_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_HBA_HINT_SIGN, RHT_HBA_HINT_NOT_ON_HORSE, RHT_HBA_HINT_INITIAL, RHT_HBA_HINT_HAVE_1000},
                      {RC_GF_HBA_1000_POINTS, RC_GF_HBA_1500_POINTS}));
  }
  if (ctx->GetOption(RSK_MALON_HINT)){
    ctx->AddHint(RH_MALON_HINT, 
                 StaticHint(RH_MALON_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_MALON_HINT_TURNING_EVIL, RHT_MALON_HINT_HOW_IS_EPONA, RHT_MALON_HINT_OBSTICLE_COURSE, RHT_MALON_HINT_INGO_TEMPTED},
                      {RC_KF_LINKS_HOUSE_COW}));
  }
  if (ctx->GetOption(RSK_CHICKENS_HINT)){
    ctx->AddHint(RH_CHICKENS_HINT, 
                 StaticHint(RH_CHICKENS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_CHICKENS_HINT},{RC_KAK_ANJU_AS_CHILD}));
  }
  if (ctx->GetOption(RSK_BIG_POES_HINT)){
    ctx->AddHint(RH_BIG_POES_HINT, 
                 StaticHint(RH_BIG_POES_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_BIG_POES_HINT},{RC_MARKET_10_BIG_POES}));
  }
  if (ctx->GetOption(RSK_BIGGORON_HINT)){
    ctx->AddHint(RH_BIGGORON_HINT, 
                 StaticHint(RH_BIGGORON_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_BIGGORON_HINT},{RC_DMT_TRADE_CLAIM_CHECK}));
  }
  if (ctx->GetOption(RSK_FROGS_HINT)){
    ctx->AddHint(RH_FROGS_HINT, 
                 StaticHint(RH_FROGS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_FROGS_HINT},{RC_ZR_FROGS_OCARINA_GAME}));
  }
  if (ctx->GetOption(RSK_KAK_10_SKULLS_HINT)){
    ctx->AddHint(RH_KAK_10_SKULLS_HINT,
                 StaticHint(RH_KAK_10_SKULLS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_SKULLS_HINT}, {RC_KAK_10_GOLD_SKULLTULA_REWARD}, (uint8_t)10));
  }
  if (ctx->GetOption(RSK_KAK_20_SKULLS_HINT)){
    ctx->AddHint(RH_KAK_20_SKULLS_HINT,
                 StaticHint(RH_KAK_20_SKULLS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_SKULLS_HINT}, {RC_KAK_20_GOLD_SKULLTULA_REWARD}, (uint8_t)20));
  }
  if (ctx->GetOption(RSK_KAK_30_SKULLS_HINT)){
    ctx->AddHint(RH_KAK_30_SKULLS_HINT,
                 StaticHint(RH_KAK_30_SKULLS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_SKULLS_HINT}, {RC_KAK_30_GOLD_SKULLTULA_REWARD}, (uint8_t)30));
  }
  if (ctx->GetOption(RSK_KAK_40_SKULLS_HINT)){
    ctx->AddHint(RH_KAK_40_SKULLS_HINT,
                 StaticHint(RH_KAK_40_SKULLS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_SKULLS_HINT}, {RC_KAK_40_GOLD_SKULLTULA_REWARD}, (uint8_t)40));
  }
  if (ctx->GetOption(RSK_KAK_50_SKULLS_HINT)){
    ctx->AddHint(RH_KAK_50_SKULLS_HINT,
                 StaticHint(RH_KAK_50_SKULLS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_SKULLS_HINT}, {RC_KAK_50_GOLD_SKULLTULA_REWARD}, (uint8_t)50));
  }
  if (ctx->GetOption(RSK_KAK_100_SKULLS_HINT)){
    ctx->AddHint(RH_KAK_100_SKULLS_HINT,
                 StaticHint(RH_KAK_100_SKULLS_HINT, HINT_TYPE_STATIC_LOCATION, {RHT_SKULLS_HINT}, {RC_KAK_100_GOLD_SKULLTULA_REWARD}, (uint8_t)100));
  }
}

void CreateStaticItemHint(RandomizerHint hintKey, std::vector<RandomizerHintTextKey> hintTextKeys,
                          std::vector<RandomizerGet> items, std::vector<RandomizerCheck> hintChecks, bool yourPocket = false) {
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> locations = FindItemsAndMarkHinted(items, hintChecks);
  ctx->AddHint(hintKey, StaticHint(hintKey, HINT_TYPE_STATIC_ITEM, hintTextKeys, locations, yourPocket));
}

void CreateAllHints(){
  auto ctx = Rando::Context::GetInstance();

  CreateAltarText();

  if (ctx->GetOption(RSK_LIGHT_ARROWS_HINT)){
    if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
      if (ctx->GetOption(RSK_TRIAL_COUNT).Is(0)){
        CreateStaticItemHint(RH_ENDGAME_HINT, {RHT_GANONDORF_HINT_LA_AND_MS}, {RG_LIGHT_ARROWS, RG_MASTER_SWORD}, {RC_GANONDORF_HINT}, true);
      } else {
        CreateStaticItemHint(RH_ENDGAME_HINT, {RHT_GANONDORF_HINT_LA_AND_MS, RHT_SHEIK_HINT_LA_AND_MS}, {RG_LIGHT_ARROWS, RG_MASTER_SWORD}, {RC_GANONDORF_HINT, RC_SHEIK_HINT_GC, RC_SHEIK_HINT_MQ_GC}, true);
      }
    } else {
      if (ctx->GetOption(RSK_TRIAL_COUNT).Is(0)){
        CreateStaticItemHint(RH_ENDGAME_HINT, {RHT_GANONDORF_HINT_LA_ONLY}, {RG_LIGHT_ARROWS}, {RC_GANONDORF_HINT}, true);
      } else {
        CreateStaticItemHint(RH_ENDGAME_HINT, {RHT_GANONDORF_HINT_LA_ONLY, RHT_SHEIK_HINT_LA_ONLY}, {RG_LIGHT_ARROWS}, {RC_GANONDORF_HINT, RC_SHEIK_HINT_GC, RC_SHEIK_HINT_MQ_GC}, true);
      }
    }
  } 

  if (ctx->GetOption(RSK_DAMPES_DIARY_HINT)){
    CreateStaticItemHint(RH_DAMPES_DIARY, {RHT_DAMPE_DIARY}, {RG_PROGRESSIVE_HOOKSHOT}, {RC_DAMPE_HINT});
  }

  if (ctx->GetOption(RSK_GREG_HINT)){
    CreateStaticItemHint(RH_GREG_RUPEE, {RHT_GREG_HINT}, {RG_GREG_RUPEE}, {RC_GREG_HINT});
  }

  if (ctx->GetOption(RSK_SARIA_HINT)){
    CreateStaticItemHint(RH_SARIA_HINT, {RHT_SARIA_HINT}, {RG_PROGRESSIVE_MAGIC_METER}, {RC_SARIA_SONG_HINT, RC_SONG_FROM_SARIA});
  }

  if (ctx->GetOption(RSK_FISHING_POLE_HINT)){
    CreateStaticItemHint(RH_FISHING_POLE, {RHT_FISHING_POLE_HINT}, {RG_FISHING_POLE}, {RC_FISHING_POLE_HINT});
  }

  CreateStaticLocationHints();

  if (ctx->GetOption(RSK_GOSSIP_STONE_HINTS).IsNot(RO_GOSSIP_STONES_NONE)) {
    printf("\x1b[10;10HCreating Hints...");
    CreateStoneHints();
    printf("Done");
  }
}

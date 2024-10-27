#include "hints.hpp"

#include "item_pool.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "fill.hpp"
#include "../trial.h"
#include "../entrance.h"
#include "z64item.h"
#include <spdlog/spdlog.h>
#include "../randomizerTypes.h"
#include "pool_functions.hpp"
#include "../hint.h"
#include "../static_data.h"


using namespace Rando;

HintDistributionSetting::HintDistributionSetting(std::string _name, 
                          HintType _type, 
                          uint32_t _weight, 
                          uint8_t _fixed, 
                          uint8_t _copies, 
                          std::function<bool(RandomizerCheck)> _filter,
                          uint8_t _dungeonLimit){
    name = _name;
    type = _type;
    weight = _weight;
    fixed = _fixed;
    copies = _copies;
    filter = _filter;
    dungeonLimit = _dungeonLimit;
  }

HintText::HintText(CustomMessage clearText_, std::vector<CustomMessage> ambiguousText_, std::vector<CustomMessage> obscureText_)
: clearText(std::move(clearText_)), ambiguousText(std::move(ambiguousText_)), obscureText(std::move(obscureText_)){}

const CustomMessage& HintText::GetClear() const {
    return clearText;
}

const CustomMessage& HintText::GetObscure() const {
    return obscureText.size() > 0 ? RandomElement(obscureText) : clearText;
}

const CustomMessage& HintText::GetObscure(uint8_t selection) const {
    if (obscureText.size() > selection){
        return obscureText[selection];
    } else if (obscureText.size() > 0) {
        return obscureText[0];
    }
    return clearText;
}

const CustomMessage& HintText::GetAmbiguous() const {
    return ambiguousText.size() > 0 ? RandomElement(ambiguousText) : clearText;
}

const CustomMessage& HintText::GetAmbiguous(uint8_t selection) const {
    if (ambiguousText.size() > selection){
        return ambiguousText[selection];
    } else if (ambiguousText.size() > 0) {
        return ambiguousText[0];
    }
    return clearText;
}

uint8_t HintText::GetAmbiguousSize() const{
    return ambiguousText.size();
}

uint8_t HintText::GetObscureSize() const{
    return obscureText.size();
}

const CustomMessage& HintText::GetHintMessage(uint8_t selection) const {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_OBSCURE)) {
        return GetObscure(selection);
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)) {
        return GetAmbiguous(selection);
    } else {
        return GetClear();
    }
}

const CustomMessage HintText::GetMessageCopy() const {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_OBSCURE)) {
        return GetObscure();
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)) {
        return GetAmbiguous();
    } else {
        return GetClear();
    }
}

bool HintText::operator==(const HintText& right) const {
    return obscureText == right.obscureText &&
            ambiguousText == right.ambiguousText &&
            clearText == right.clearText;
}

bool HintText::operator!=(const HintText& right) const {
    return !operator==(right);
}

StaticHintInfo::StaticHintInfo(HintType _type, std::vector<RandomizerHintTextKey> _hintKeys, RandomizerSettingKey _setting,
                               std::variant<bool, uint8_t> _condition, std::vector<RandomizerCheck> _targetChecks,
                               std::vector<RandomizerGet> _targetItems, std::vector<RandomizerCheck> _hintChecks, bool _yourPocket, int _num):
                               type(_type), hintKeys(_hintKeys), setting(_setting), condition(_condition), targetChecks(_targetChecks),
                               targetItems(_targetItems), hintChecks(_hintChecks), yourPocket(_yourPocket), num(_num){}

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


bool FilterWotHLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return ctx->GetItemLocation(loc)->IsWothCandidate();
}

bool FilterFoolishLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return ctx->GetItemLocation(loc)->IsFoolishCandidate();
}

bool FilterSongLocations(RandomizerCheck loc){
  auto ctx = Rando::Context::GetInstance();
  return Rando::StaticData::GetLocation(loc)->GetRCType() == RCTYPE_SONG_LOCATION;
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
      {"WotH",       HINT_TYPE_WOTH,      7,   0, 1, FilterWotHLocations,      2},
      {"Foolish",    HINT_TYPE_FOOLISH,   4,   0, 1, FilterFoolishLocations,   1},
      //("Entrance",   HINT_TYPE_ENTRANCE,      6,  0, 1), //not yet implemented
      {"Song",       HINT_TYPE_ITEM,      2,   0, 1, FilterSongLocations},
      {"Overworld",  HINT_TYPE_ITEM,      4,   0, 1, FilterOverworldLocations},
      {"Dungeon",    HINT_TYPE_ITEM,      3,   0, 1, FilterDungeonLocations},
      {"Named Item", HINT_TYPE_ITEM_AREA, 10,  0, 1, FilterGoodItems},
      {"Random"    , HINT_TYPE_ITEM_AREA, 12,  0, 1, NoFilter}
    }
  },
  // Strong hints
  {
    .alwaysCopies = 2,
    .trialCopies = 1,
    .junkWeight = 0,
    .distTable = {
      {"WotH",       HINT_TYPE_WOTH,      12, 0, 2, FilterWotHLocations,      2},
      {"Foolish",    HINT_TYPE_FOOLISH,   12, 0, 1, FilterFoolishLocations,   1},
      //{"Entrance",   HINT_TYPE_ENTRANCE,      4, 0, 1}, //not yet implemented
      {"Song",       HINT_TYPE_ITEM,      4,  0, 1, FilterSongLocations},
      {"Overworld",  HINT_TYPE_ITEM,      6,  0, 1, FilterOverworldLocations},
      {"Dungeon",    HINT_TYPE_ITEM,      6,  0, 1, FilterDungeonLocations},
      {"Named Item", HINT_TYPE_ITEM_AREA, 8,  0, 1, FilterGoodItems},
      {"Random"    , HINT_TYPE_ITEM_AREA, 8,  0, 1, NoFilter},
    },
  },
  // Very strong hints
  {
    .alwaysCopies = 2,
    .trialCopies = 1,
    .junkWeight = 0,
    .distTable = {
      {"WotH",       HINT_TYPE_WOTH,      15, 0, 2, FilterWotHLocations},
      {"Foolish",    HINT_TYPE_FOOLISH,   15, 0, 1, FilterFoolishLocations},
      //{"Entrance",   HINT_TYPE_ENTRANCE,     10, 0, 1}, //not yet implemented
      {"Song",       HINT_TYPE_ITEM,      2,  0, 1, FilterSongLocations},
      {"Overworld",  HINT_TYPE_ITEM,      7,  0, 1, FilterOverworldLocations},
      {"Dungeon",    HINT_TYPE_ITEM,      7,  0, 1, FilterDungeonLocations},
      {"Named Item", HINT_TYPE_ITEM_AREA, 5,  0, 1, FilterGoodItems},
    },
  },
}};

uint8_t StonesRequiredBySettings() {
    auto ctx = Rando::Context::GetInstance();
    uint8_t stones = 0;
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)) {
        stones = ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT).Value<uint8_t>();
    } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
        stones = ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>() - 6;
    } else if ((ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)) && (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON))) {
        stones = ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>() - 6;
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
        stones = std::max<uint8_t>({ stones, ctx->GetOption(RSK_LACS_STONE_COUNT).Value<uint8_t>() });
    } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
        stones = std::max<uint8_t>({ stones, (uint8_t)(ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>() - 6 )});
    } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
        stones = std::max<uint8_t>({ stones, (uint8_t)(ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>() - 6 )});
    }
    return stones;
}

uint8_t MedallionsRequiredBySettings() {
    auto ctx = Rando::Context::GetInstance();
    uint8_t medallions = 0;
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS)) {
        medallions = ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT).Value<uint8_t>();
    } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
        medallions = ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>() - 3;
    } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS) && ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
        medallions = ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>() - 3;
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_MEDALLIONS)) {
        medallions = std::max({ medallions, ctx->GetOption(RSK_LACS_MEDALLION_COUNT).Value<uint8_t>() });
    } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
        medallions = std::max({ medallions, (uint8_t)(ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>() - 3 )});
    } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS) && ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
        medallions = std::max({ medallions, (uint8_t)(ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>() - 3 )});
    }
    return medallions;
}

uint8_t TokensRequiredBySettings() {
    auto ctx = Rando::Context::GetInstance();
    uint8_t tokens = 0;
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)) {
        tokens = ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>();
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
        tokens = std::max<uint8_t>({ tokens, ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>() });
    }
    return tokens;
}

std::vector<std::pair<RandomizerCheck, std::function<bool()>>> conditionalAlwaysHints = {
    std::make_pair(RC_MARKET_10_BIG_POES, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return ctx->GetOption(RSK_BIG_POE_COUNT).Value<uint8_t>() >= 3 && !ctx->GetOption(RSK_BIG_POES_HINT);
                   }), // Remember, the option's value being 3 means 4 are required
    std::make_pair(RC_DEKU_THEATER_MASK_OF_TRUTH, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_MASK_SHOP_HINT) && !ctx->GetOption(RSK_COMPLETE_MASK_QUEST);
                   }),
    std::make_pair(RC_SONG_FROM_OCARINA_OF_TIME, []() { 
                        auto ctx = Rando::Context::GetInstance();
                        return StonesRequiredBySettings() < 2 && !ctx->GetOption(RSK_OOT_HINT);
                   }),
    std::make_pair(RC_HF_OCARINA_OF_TIME_ITEM, []() { return StonesRequiredBySettings() < 2; }),
    std::make_pair(RC_SHEIK_IN_KAKARIKO, []() { return MedallionsRequiredBySettings() < 5; }),
    std::make_pair(RC_DMT_TRADE_CLAIM_CHECK, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_BIGGORON_HINT);
                   }),
    std::make_pair(RC_KAK_30_GOLD_SKULLTULA_REWARD, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_KAK_30_SKULLS_HINT) && TokensRequiredBySettings() < 30;
                   }),
    std::make_pair(RC_KAK_40_GOLD_SKULLTULA_REWARD, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_KAK_40_SKULLS_HINT) && TokensRequiredBySettings() < 40;
                   }),
    std::make_pair(RC_KAK_50_GOLD_SKULLTULA_REWARD, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_KAK_50_SKULLS_HINT) && TokensRequiredBySettings() < 50;
                   }),
    std::make_pair(RC_ZR_FROGS_OCARINA_GAME, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_FROGS_HINT);
                   }),
    std::make_pair(RC_KF_LINKS_HOUSE_COW, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_MALON_HINT);
                   }),
    std::make_pair(RC_KAK_100_GOLD_SKULLTULA_REWARD, []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_KAK_100_SKULLS_HINT) && TokensRequiredBySettings() < 100;
                   }),
};

static std::vector<RandomizerCheck> GetEmptyGossipStones() {
  auto emptyGossipStones = GetEmptyLocations(Rando::StaticData::GetGossipStoneLocations());
  return emptyGossipStones;
}

static std::vector<RandomizerCheck> GetAccessibleGossipStones(const RandomizerCheck hintedLocation = RC_GANON) {
  auto ctx = Rando::Context::GetInstance();
  //temporarily remove the hinted location's item, and then perform a
  //reachability search for gossip stone locations.
  RandomizerGet originalItem = ctx->GetItemLocation(hintedLocation)->GetPlacedRandomizerGet();
  ctx->GetItemLocation(hintedLocation)->SetPlacedItem(RG_NONE);

  ctx->GetLogic()->Reset();
  auto accessibleGossipStones = ReachabilitySearch(Rando::StaticData::GetGossipStoneLocations());
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
  const auto rechableWithout = ReachabilitySearch(locsToCheck);
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

static void SetAllInAreaAsHintAccesible(RandomizerArea area, std::vector<RandomizerCheck> locations){
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> locsInArea = FilterFromPool(locations, [area, ctx](const RandomizerCheck loc){
                                                            return ctx->GetItemLocation(loc)->GetAreas().contains(area);
                                                          });
  for(RandomizerCheck loc: locsInArea){
    ctx->GetItemLocation(loc)->SetHintAccesible();
  }
}

static void AddGossipStoneHint( const RandomizerCheck gossipStone,
                                const HintType hintType,
                                const std::string distributionName,
                                const std::vector<RandomizerHintTextKey> hintKeys,
                                const std::vector<RandomizerCheck> locations,
                                const std::vector<RandomizerArea> areas,
                                const std::vector<TrialKey> trials) {
  auto ctx = Rando::Context::GetInstance();
  ctx->AddHint(StaticData::gossipStoneCheckToHint[gossipStone], Hint(StaticData::gossipStoneCheckToHint[gossipStone], hintType, distributionName, hintKeys, locations, areas, trials));
  ctx->GetItemLocation(gossipStone)->SetPlacedItem(RG_HINT); //RANDOTODO, better gossip stone to location to hint key system
}

static void AddGossipStoneHintCopies(uint8_t copies,
                                     const HintType hintType,
                                     const std::string distributionName,
                                     const std::vector<RandomizerHintTextKey> hintKeys = {},
                                     const std::vector<RandomizerCheck> locations = {},
                                     const std::vector<RandomizerArea> areas = {},
                                     const std::vector<TrialKey> trials = {},
                                     RandomizerCheck firstStone = RC_UNKNOWN_CHECK){

  if (firstStone != RC_UNKNOWN_CHECK && copies > 0){
      AddGossipStoneHint(firstStone, hintType, distributionName, hintKeys, locations, areas, trials);
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
    AddGossipStoneHint(gossipStone, hintType, distributionName, hintKeys, locations, areas, trials);
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
  RandomizerArea area = RandomElementFromSet(ctx->GetItemLocation(location)->GetAreas());

  //Set that hints are accesible
  ctx->GetItemLocation(location)->SetHintAccesible();
  if (type == HINT_TYPE_FOOLISH){
    SetAllInAreaAsHintAccesible(area, ctx->allLocations);
  }

  AddGossipStoneHintCopies(copies, type, distribution, {}, {location}, {area}, {}, gossipStone);
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
  return hintedLocation;
}

static std::vector<RandomizerCheck> FilterHintability(std::vector<RandomizerCheck>& locations,
                                                      std::function<bool(RandomizerCheck)> extraFilter = NoFilter){
  auto ctx = Rando::Context::GetInstance();
  return FilterFromPool(locations, [extraFilter, ctx](const RandomizerCheck loc) {
    return ctx->GetItemLocation(loc)->IsHintable() && !(ctx->GetItemLocation(loc)->IsAHintAccessible())
    && extraFilter(loc);
  });
}

static void CreateJunkHints(uint8_t numHints) {
  for(uint8_t c = 0; c < numHints; c++){
    //duplicate junk hints are possible for now
    AddGossipStoneHintCopies(1, HINT_TYPE_HINT_KEY, "Junk", {GetRandomJunkHint()});
  }
}

static void CreateTrialHints(uint8_t copies) {
  if (copies > 0) {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_TRIAL_COUNT).Is(6)) {//six trials
      AddGossipStoneHintCopies(copies, HINT_TYPE_HINT_KEY, "Trial", {RHT_SIX_TRIALS});
    } else if (ctx->GetOption(RSK_TRIAL_COUNT).Is(0)) {//zero trials
      AddGossipStoneHintCopies(copies, HINT_TYPE_HINT_KEY, "Trial", {RHT_ZERO_TRIALS});
    } else {
      std::vector<TrialInfo*> trials = ctx->GetTrials()->GetTrialList(); //there's probably a way to remove this assignment
      if (ctx->GetOption(RSK_TRIAL_COUNT).Value<uint8_t>() >= 4) {//4 or 5 required trials, get skipped trials
        trials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsSkipped();});
      } else {//1 to 3 trials, get requried trials
        auto requiredTrials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsRequired();});
      }
      for (auto& trial : trials) {//create a hint for each hinted trial
        AddGossipStoneHintCopies(copies, HINT_TYPE_TRIAL, "Trial", {}, {}, {}, {trial->GetTrialKey()});
      }
    }
  }
}

void CreateWarpSongTexts() {
  auto ctx = Rando::Context::GetInstance();
  if (ctx->GetOption(RSK_WARP_SONG_HINTS)){
    auto warpSongEntrances = GetShuffleableEntrances(EntranceType::WarpSong, false);
    for (auto entrance : warpSongEntrances) {
      //RANDOTODO make random
      RandomizerArea destination = entrance->GetConnectedRegion()->GetFirstArea();
      switch (entrance->GetIndex()) {
        case 0x0600: // minuet RANDOTODO make into entrance hints when they are added
          ctx->AddHint(RH_MINUET_WARP_LOC, Hint(RH_MINUET_WARP_LOC, HINT_TYPE_AREA, "", {RHT_WARP_SONG}, {}, {destination}));
          break;
        case 0x04F6: // bolero
          ctx->AddHint(RH_BOLERO_WARP_LOC, Hint(RH_BOLERO_WARP_LOC, HINT_TYPE_AREA, "", {RHT_WARP_SONG}, {}, {destination}));
          break;
        case 0x0604: // serenade
          ctx->AddHint(RH_SERENADE_WARP_LOC, Hint(RH_SERENADE_WARP_LOC, HINT_TYPE_AREA, "", {RHT_WARP_SONG}, {}, {destination}));
          break;
        case 0x01F1: // requiem
          ctx->AddHint(RH_REQUIEM_WARP_LOC, Hint(RH_REQUIEM_WARP_LOC, HINT_TYPE_AREA, "", {RHT_WARP_SONG}, {}, {destination}));
          break;
        case 0x0568: // nocturne
          ctx->AddHint(RH_NOCTURNE_WARP_LOC, Hint(RH_NOCTURNE_WARP_LOC, HINT_TYPE_AREA, "", {RHT_WARP_SONG}, {}, {destination}));
          break;
        case 0x05F4: // prelude
          ctx->AddHint(RH_PRELUDE_WARP_LOC, Hint(RH_PRELUDE_WARP_LOC, HINT_TYPE_AREA, "", {RHT_WARP_SONG}, {}, {destination}));
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
  int32_t totalWeight = junkWieght; //Start with our Junk Weight, the natural chance of a junk hint

  for (size_t c=0; c < distTable.size(); c++){ //Gather the weights of each distribution and, if it's the first pass, apply fixed hints
    totalWeight += distTable[c].weight;         //Note that PlaceHints will set weights of distributions to zero if it can't place anything from them
    if (addFixed){
      selected[c] += distTable[c].fixed;
      stoneCount -= distTable[c].fixed * distTable[c].copies;
    }
  }
  int32_t currentWeight = getRandomWeight(totalWeight); //Initialise with the first random weight from 1 to the total. 
  while(stoneCount > 0 && totalWeight > 0){//Loop until we run out of stones or have no TotalWeight. 0 totalWeight means junkWeight is 0
                                           //and that all weights have been 0'd out for another reason, and skips to placing all junk hints
    for (size_t distribution = 0; distribution < distTable.size(); distribution++){
      currentWeight -= distTable[distribution].weight; //go over each distribution, subtracting the weight each time. Once we reach zero or less,
      if (currentWeight <= 0){                         //tell the system to make 1 of that hint, unless not enough stones remain
        if (stoneCount >= distTable[distribution].copies && distTable[distribution].copies > 0){
          selected[distribution] += 1; //if we have enough stones, and copies are not zero, assign 1 to this hint type, remove the stones, and break
          stoneCount -= distTable[distribution].copies;
          break; //This leaves the whole for loop
        } else { //If we don't have the stones, or copies is 0 despite there being the wieght to trigger a hit, temporerally set wieght to zero
          totalWeight -= distTable[distribution].weight; //Unlike PlaceHint, distTable is passed by value here, making this temporary
          distTable[distribution].weight = 0;            //this is so we can still roll this hint type if more stones free up later 
          break;
        }
      }
    }
    //if there's still weight then it's junk, as the leftover weight is junkWeight
    if (currentWeight > 0){ //zero TotalWeight breaks the while loop and hits the fallback, so skipping this is fine in that case
      selected[selected.size()-1] += 1;
      stoneCount -= 1;
    }
    currentWeight = getRandomWeight(totalWeight);
  }
  //if stones are left, assign junk to every remaining stone as a fallback.
  if (stoneCount > 0){
    selected[selected.size()-1] += stoneCount;
  }
}

uint8_t PlaceHints(std::vector<uint8_t>& selectedHints, std::vector<HintDistributionSetting>& distTable){
  auto ctx = Rando::Context::GetInstance();
  uint8_t curSlot = 0;
  for (HintDistributionSetting distribution : distTable){
    std::vector<RandomizerCheck> hintTypePool = FilterHintability(ctx->allLocations, distribution.filter);
    for (uint8_t numHint = 0; numHint < selectedHints[curSlot]; numHint++){
      hintTypePool = FilterHintability(hintTypePool);
      SPDLOG_DEBUG("Attempting to make hint of type: " + StaticData::hintTypeNames[distribution.type].GetEnglish(MF_CLEAN) + "\n");
      RandomizerCheck hintedLocation = RC_UNKNOWN_CHECK;

      hintedLocation = CreateRandomHint(hintTypePool, distribution.copies, distribution.type, distribution.name);

      if (hintedLocation == RC_UNKNOWN_CHECK){ //if hint failed to place, remove all wieght and copies then return the number of stones to redistribute
        uint8_t hintsToRemove = (selectedHints[curSlot] - numHint) * distribution.copies;
        selectedHints[curSlot] = 0;   //as distTable is passed by refernce here, these changes stick for the rest of this seed generation
        distTable[curSlot].copies = 0;//and prevent future distribution from choosing this slot
        distTable[curSlot].weight = 0;
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

  // Apply impa's song exclusions when zelda is skipped
  if(ctx->GetOption(RSK_SKIP_CHILD_ZELDA)){
    ctx->GetItemLocation(RC_SONG_FROM_IMPA)->SetHintAccesible();
  }

  // Add 'always' location hints
  std::vector<RandomizerCheck> alwaysHintLocations = {};
  if (hintSetting.alwaysCopies > 0) {
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)){
      // If we have Rainbow Bridge set to Greg and the greg hint isn't useful, add a hint for where Greg is
      // Do we really need this with the greg hint?
      auto gregLocations = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
      return (
              (ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_GREG_RUPEE)) &&
               ctx->GetItemLocation(loc)->IsHintable() &&
               !(ctx->GetOption(RSK_GREG_HINT) && (IsReachableWithout({RC_GREG_HINT}, loc, true)));
      });
      if (gregLocations.size() > 0){
        alwaysHintLocations.push_back(gregLocations[0]);
      }
    }

    for (auto& hint : conditionalAlwaysHints) {
        RandomizerCheck loc = hint.first;
        if (hint.second() && ctx->GetItemLocation(loc)->IsHintable()) {
            alwaysHintLocations.push_back(loc);
        }
    }

    for (RandomizerCheck location : alwaysHintLocations) {
      CreateHint(location, hintSetting.alwaysCopies, HINT_TYPE_ITEM, "Always");
    }
  }

  //Add 'trial' location hints
  if (ctx->GetOption(RSK_GANONS_TRIALS).IsNot(RO_GANONS_TRIALS_SKIP)) {
    CreateTrialHints(hintSetting.trialCopies);
  }

  size_t totalStones = GetEmptyGossipStones().size();
  std::vector<uint8_t> selectedHints = {};
  for (size_t c=0; c < distTable.size(); c++){
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
  ReachabilitySearch({});
}

std::vector<RandomizerCheck> FindItemsAndMarkHinted(std::vector<RandomizerGet> items, std::vector<RandomizerCheck> hintChecks){
  std::vector<RandomizerCheck> locations = {};
  auto ctx = Rando::Context::GetInstance();
  for (size_t c = 0; c < items.size(); c++){
    std::vector<RandomizerCheck> found = FilterFromPool(ctx->allLocations, [items, ctx, c](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == items[c];});
    if (found.size() > 0){
      locations.push_back(found[0]);
      //RANDOTODO make the called functions of this always return true if empty hintChecks are provided
      if (!ctx->GetItemLocation(found[0])->IsAHintAccessible() && (hintChecks.size() == 0 || IsReachableWithout(hintChecks, found[0],true))){
        ctx->GetItemLocation(found[0])->SetHintAccesible();
      }
    } else {
      locations.push_back(RC_UNKNOWN_CHECK);
    }
  }
  return locations;
}

void CreateChildAltarHint() {
  auto ctx = Rando::Context::GetInstance();
  if (!ctx->GetHint(RH_ALTAR_CHILD)->IsEnabled()){
    std::vector<RandomizerCheck> stoneLocs = {};
    if (ctx->GetOption(RSK_TOT_ALTAR_HINT)) {
      //force marking the rewards as hinted if they are at the end of dungeons as they can be inffered
      if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)){
        stoneLocs = FindItemsAndMarkHinted({RG_KOKIRI_EMERALD, RG_GORON_RUBY, RG_ZORA_SAPPHIRE}, {});
      } else {
        stoneLocs = FindItemsAndMarkHinted({RG_KOKIRI_EMERALD, RG_GORON_RUBY, RG_ZORA_SAPPHIRE}, {RC_ALTAR_HINT_CHILD});
      }
    }
    std::vector<RandomizerArea> stoneAreas = {};
    for (auto loc : stoneLocs){
      stoneAreas.push_back(RandomElementFromSet(ctx->GetItemLocation(loc)->GetAreas()));
    }
    ctx->AddHint(RH_ALTAR_CHILD, Hint(RH_ALTAR_CHILD, HINT_TYPE_ALTAR_CHILD, {}, stoneLocs, stoneAreas));
  }
}

void CreateAdultAltarHint() {
  auto ctx = Rando::Context::GetInstance();
  if (!ctx->GetHint(RH_ALTAR_ADULT)->IsEnabled()){
    std::vector<RandomizerCheck> medallionLocs = {};
    if (ctx->GetOption(RSK_TOT_ALTAR_HINT)) {
      //force marking the rewards as hinted if they are at the end of dungeons as they can be inffered
      if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)){
      medallionLocs = FindItemsAndMarkHinted({RG_LIGHT_MEDALLION, RG_FOREST_MEDALLION, RG_FIRE_MEDALLION, RG_WATER_MEDALLION, RG_SPIRIT_MEDALLION, RG_SHADOW_MEDALLION},
                                              {});
      } else {
        medallionLocs = FindItemsAndMarkHinted({RG_LIGHT_MEDALLION, RG_FOREST_MEDALLION, RG_FIRE_MEDALLION, RG_WATER_MEDALLION, RG_SPIRIT_MEDALLION, RG_SHADOW_MEDALLION},
                                              {RC_ALTAR_HINT_ADULT});
      }
    }
    std::vector<RandomizerArea> medallionAreas = {};
    for (auto loc : medallionLocs){
      medallionAreas.push_back(RandomElementFromSet(ctx->GetItemLocation(loc)->GetAreas()));
    }
    ctx->AddHint(RH_ALTAR_ADULT, Hint(RH_ALTAR_ADULT, HINT_TYPE_ALTAR_ADULT, {}, medallionLocs, medallionAreas));
  }
}

void CreateStaticHintFromData(RandomizerHint hint, StaticHintInfo staticData){
  auto ctx = Rando::Context::GetInstance();
  if (!ctx->GetHint(hint)->IsEnabled()){
    Option option = ctx->GetOption(staticData.setting);
    if ((std::holds_alternative<bool>(staticData.condition) && option.Is(std::get<bool>(staticData.condition))) ||
        (std::holds_alternative<uint8_t>(staticData.condition) && option.Is(std::get<uint8_t>(staticData.condition)))){

      std::vector<RandomizerCheck> locations = {};
      if (staticData.targetItems.size() > 0){
        locations = FindItemsAndMarkHinted(staticData.targetItems, staticData.hintChecks);
      } else {
        for(auto check: staticData.targetChecks){
          locations.push_back(check);
        }
      }
      std::vector<RandomizerArea> areas = {};
      for (auto loc : locations){
        ctx->GetItemLocation(loc)->SetHintAccesible();
        areas.push_back(RandomElementFromSet(ctx->GetItemLocation(loc)->GetAreas()));
      }
      //hintKeys are defaulted to in the hint object and do not need to be specified
      ctx->AddHint(hint, Hint(hint, staticData.type, {}, locations, areas, {}, staticData.yourPocket, staticData.num));
    }
  }
}

void CreateStaticItemHint(RandomizerHint hintKey, std::vector<RandomizerHintTextKey> hintTextKeys,
                          std::vector<RandomizerGet> items, std::vector<RandomizerCheck> hintChecks, bool yourPocket = false) {
  //RANDOTODO choose area in case there are multiple
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> locations = FindItemsAndMarkHinted(items, hintChecks);
  std::vector<RandomizerArea> areas = {};
  for (auto loc : locations){
    areas.push_back(RandomElementFromSet(ctx->GetItemLocation(loc)->GetAreas()));
  }
  ctx->AddHint(hintKey, Hint(hintKey, HINT_TYPE_AREA, hintTextKeys, locations, areas, {}, yourPocket));
}

void CreateGanondorfJoke(){
  auto ctx = Rando::Context::GetInstance();
  if (!ctx->GetHint(RH_GANONDORF_JOKE)->IsEnabled()){
    ctx->AddHint(RH_GANONDORF_JOKE, Hint(RH_GANONDORF_JOKE, HINT_TYPE_HINT_KEY, {GetRandomGanonJoke()}));
  }
}

void CreateGanondorfHint(){
  auto ctx = Rando::Context::GetInstance();
  if (ctx->GetOption(RSK_GANONDORF_HINT) && !ctx->GetHint(RH_GANONDORF_HINT)->IsEnabled()){
    if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
      CreateStaticItemHint(RH_GANONDORF_HINT, {RHT_GANONDORF_HINT_LA_ONLY, RHT_GANONDORF_HINT_MS_ONLY, RHT_GANONDORF_HINT_LA_AND_MS}, 
                           {RG_LIGHT_ARROWS, RG_MASTER_SWORD}, {RC_GANONDORF_HINT}, true);
    } else {
      CreateStaticItemHint(RH_GANONDORF_HINT, {RHT_GANONDORF_HINT_LA_ONLY}, {RG_LIGHT_ARROWS}, {RC_GANONDORF_HINT}, true);
    }
  }
}

void CreateStaticHints(){
  CreateChildAltarHint();
  CreateAdultAltarHint();
  CreateGanondorfJoke();
  CreateGanondorfHint();
  for (auto[hint, staticData] : StaticData::staticHintInfoMap){
    CreateStaticHintFromData(hint, staticData);
  }
}

void CreateAllHints(){
  auto ctx = Rando::Context::GetInstance();

  CreateStaticHints();

  if (ctx->GetOption(RSK_GOSSIP_STONE_HINTS).IsNot(RO_GOSSIP_STONES_NONE)) {
    SPDLOG_INFO("Creating Hints...");
    CreateStoneHints();
    SPDLOG_INFO("Creating Hints Done");
  }
}

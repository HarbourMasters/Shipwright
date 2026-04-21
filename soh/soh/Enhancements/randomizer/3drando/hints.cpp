#include "hints.hpp"

#include "random.hpp"
#include "fill.hpp"
#include "../trial.h"
#include "../entrance.h"
#include <spdlog/spdlog.h>
#include "../randomizerTypes.h"
#include "pool_functions.hpp"
#include "../hint.h"
#include "../static_data.h"

using namespace Rando;

HintDistributionSetting::HintDistributionSetting(std::string _name, HintType _type, uint32_t _weight, uint8_t _fixed,
                                                 uint8_t _copies, std::function<bool(RandomizerCheck)> _filter,
                                                 uint8_t _dungeonLimit) {
    name = _name;
    type = _type;
    weight = _weight;
    fixed = _fixed;
    copies = _copies;
    filter = _filter;
    dungeonLimit = _dungeonLimit;
}

HintText::HintText(CustomMessage clearText_, std::vector<CustomMessage> ambiguousText_,
                   std::vector<CustomMessage> obscureText_)
    : clearText(std::move(clearText_)), ambiguousText(std::move(ambiguousText_)), obscureText(std::move(obscureText_)) {
}

const CustomMessage& HintText::GetClear() const {
    return clearText;
}

const CustomMessage& HintText::GetObscure() const {
    return obscureText.size() > 0 ? RandomElement(obscureText) : clearText;
}

const CustomMessage& HintText::GetObscure(size_t selection) const {
    if (obscureText.size() > selection) {
        return obscureText[selection];
    } else if (obscureText.size() > 0) {
        return obscureText[0];
    }
    return clearText;
}

const CustomMessage& HintText::GetAmbiguous() const {
    return ambiguousText.size() > 0 ? RandomElement(ambiguousText) : clearText;
}

const CustomMessage& HintText::GetAmbiguous(size_t selection) const {
    if (ambiguousText.size() > selection) {
        return ambiguousText[selection];
    } else if (ambiguousText.size() > 0) {
        return ambiguousText[0];
    }
    return clearText;
}

size_t HintText::GetAmbiguousSize() const {
    return ambiguousText.size();
}

size_t HintText::GetObscureSize() const {
    return obscureText.size();
}

const CustomMessage& HintText::GetHintMessage(size_t selection) const {
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
    return obscureText == right.obscureText && ambiguousText == right.ambiguousText && clearText == right.clearText;
}

bool HintText::operator!=(const HintText& right) const {
    return !operator==(right);
}

StaticHintInfo::StaticHintInfo(HintType _type, std::vector<RandomizerHintTextKey> _hintKeys,
                               RandomizerSettingKey _setting, std::variant<bool, uint8_t> _condition,
                               std::vector<RandomizerCheck> _targetChecks, std::vector<RandomizerGet> _targetItems,
                               std::vector<RandomizerCheck> _hintChecks, bool _yourPocket, int _num)
    : type(_type), hintKeys(_hintKeys), setting(_setting), condition(_condition), targetChecks(_targetChecks),
      targetItems(_targetItems), hintChecks(_hintChecks), yourPocket(_yourPocket), num(_num) {
}

RandomizerHintTextKey GetRandomJunkHint() {
    // Temp code to handle random junk hints now I work in keys instead of a vector of HintText
    // Will be replaced with a better system once more customisable hint pools are added
    uint32_t range = RHT_JUNK71 - RHT_JUNK01;
    return (RandomizerHintTextKey)(Random(0, range) + RHT_JUNK01);
}

RandomizerHintTextKey GetRandomGanonJoke() {
    uint32_t range = RHT_GANON_JOKE11 - RHT_GANON_JOKE01;
    return (RandomizerHintTextKey)(Random(0, range) + RHT_GANON_JOKE01);
}

bool FilterWotHLocations(RandomizerCheck loc) {
    auto ctx = Rando::Context::GetInstance();
    return ctx->GetItemLocation(loc)->IsWothCandidate();
}

bool FilterFoolishLocations(RandomizerCheck loc) {
    auto ctx = Rando::Context::GetInstance();
    return ctx->GetItemLocation(loc)->IsFoolishCandidate();
}

bool FilterSongLocations(RandomizerCheck loc) {
    auto ctx = Rando::Context::GetInstance();
    return Rando::StaticData::GetLocation(loc)->GetRCType() == RCTYPE_SONG_LOCATION;
}

bool FilterOverworldLocations(RandomizerCheck loc) {
    auto ctx = Rando::Context::GetInstance();
    return Rando::StaticData::GetLocation(loc)->IsOverworld();
}

bool FilterDungeonLocations(RandomizerCheck loc) {
    auto ctx = Rando::Context::GetInstance();
    return Rando::StaticData::GetLocation(loc)->IsDungeon();
}

bool FilterGoodItems(RandomizerCheck loc) {
    auto ctx = Rando::Context::GetInstance();
    return ctx->GetItemLocation(loc)->GetPlacedItem().IsMajorItem();
}

bool NoFilter(RandomizerCheck loc) {
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

struct BridgeReqConfig {
    RandomizerSettingKey bridgeDirectKey;
    RandomizerSettingKey lacsDirectKey;
    RandoOptionRainbowBridge bridgeEnum;
    RandoOptionGanonsBossKey lacsEnum;
    uint8_t offset;
};

static constexpr BridgeReqConfig StonesConfig{ RSK_RAINBOW_BRIDGE_STONE_COUNT, RSK_LACS_STONE_COUNT, RO_BRIDGE_STONES,
                                               RO_GANON_BOSS_KEY_LACS_STONES, 6 };
static constexpr BridgeReqConfig MedallionsConfig{ RSK_RAINBOW_BRIDGE_MEDALLION_COUNT, RSK_LACS_MEDALLION_COUNT,
                                                   RO_BRIDGE_MEDALLIONS, RO_GANON_BOSS_KEY_LACS_MEDALLIONS, 3 };
static constexpr BridgeReqConfig TokensConfig{ RSK_RAINBOW_BRIDGE_TOKEN_COUNT, RSK_LACS_TOKEN_COUNT, RO_BRIDGE_TOKENS,
                                               RO_GANON_BOSS_KEY_LACS_TOKENS, 0 };

static uint8_t RequiredBySettings(const BridgeReqConfig& cfg) {
    auto ctx = Rando::Context::GetInstance();
    uint8_t count = 0;
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(cfg.bridgeEnum)) {
        count = ctx->GetOption(cfg.bridgeDirectKey).Get();
    } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
        count = ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Get() - cfg.offset;
    } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS) &&
               ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
        count = ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Get() - cfg.offset;
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(cfg.lacsEnum)) {
        count = std::max<uint8_t>(count, ctx->GetOption(cfg.lacsDirectKey).Get());
    } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
        count = std::max<uint8_t>(count, (uint8_t)(ctx->GetOption(RSK_LACS_REWARD_COUNT).Get() - cfg.offset));
    } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS) &&
               ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
        count = std::max<uint8_t>(count, (uint8_t)(ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Get() - cfg.offset));
    }
    return count;
}

static uint8_t StonesRequiredBySettings() {
    return RequiredBySettings(StonesConfig);
}
static uint8_t MedallionsRequiredBySettings() {
    return RequiredBySettings(MedallionsConfig);
}
static uint8_t TokensRequiredBySettings() {
    return RequiredBySettings(TokensConfig);
}

// An 'always' hint that only applies under certain settings. Suppressed when the user
// has enabled `dedicatedHint` (since a dedicated hint renders the gossip-stone hint redundant),
// or when `extra` is present and returns false. RSK_NONE in `dedicatedHint` means no suppression.
struct ConditionalAlwaysHint {
    RandomizerCheck loc;
    RandomizerSettingKey dedicatedHint;
    std::function<bool()> extra;
};

std::vector<ConditionalAlwaysHint> conditionalAlwaysHints = {
    // clang-format off
    { RC_MARKET_10_BIG_POES,            RSK_BIG_POES_HINT,       []() { return Rando::Context::GetInstance()->GetOption(RSK_BIG_POE_COUNT).Get() > 3; } },
    { RC_DEKU_THEATER_MASK_OF_TRUTH,    RSK_MASK_SHOP_HINT,      []() { return !Rando::Context::GetInstance()->GetOption(RSK_MASK_QUEST); } },
    { RC_SONG_FROM_OCARINA_OF_TIME,     RSK_OOT_HINT,            []() { return StonesRequiredBySettings() < 2; } },
    { RC_HF_OCARINA_OF_TIME_ITEM,       RSK_NONE,                []() { return StonesRequiredBySettings() < 2; } },
    { RC_SHEIK_IN_KAKARIKO,             RSK_NONE,                []() { return MedallionsRequiredBySettings() < 5; } },
    { RC_DMT_TRADE_CLAIM_CHECK,         RSK_BIGGORON_HINT,       nullptr },
    { RC_KAK_30_GOLD_SKULLTULA_REWARD,  RSK_KAK_30_SKULLS_HINT,  []() { return TokensRequiredBySettings() < 30; } },
    { RC_KAK_40_GOLD_SKULLTULA_REWARD,  RSK_KAK_40_SKULLS_HINT,  []() { return TokensRequiredBySettings() < 40; } },
    { RC_KAK_50_GOLD_SKULLTULA_REWARD,  RSK_KAK_50_SKULLS_HINT,  []() { return TokensRequiredBySettings() < 50; } },
    { RC_ZR_FROGS_OCARINA_GAME,         RSK_FROGS_HINT,          nullptr },
    { RC_KF_LINKS_HOUSE_COW,            RSK_MALON_HINT,          nullptr },
    { RC_KAK_100_GOLD_SKULLTULA_REWARD, RSK_KAK_100_SKULLS_HINT, []() { return TokensRequiredBySettings() < 100; } },
    // clang-format on
};

static bool ConditionalAlwaysHintApplies(const ConditionalAlwaysHint& h) {
    auto ctx = Rando::Context::GetInstance();
    if (h.dedicatedHint != RSK_NONE && ctx->GetOption(h.dedicatedHint)) {
        return false;
    }
    return !h.extra || h.extra();
}

static std::vector<RandomizerCheck> GetEmptyGossipStones() {
    auto emptyGossipStones = GetEmptyLocations(Rando::StaticData::GetGossipStoneLocations());
    return emptyGossipStones;
}

static std::vector<RandomizerCheck> GetAccessibleGossipStones(const RandomizerCheck hintedLocation = RC_GANON) {
    auto ctx = Rando::Context::GetInstance();
    // temporarily remove the hinted location's item, and then perform a
    // reachability search for gossip stone locations.
    RandomizerGet originalItem = ctx->GetItemLocation(hintedLocation)->GetPlacedRandomizerGet();
    ctx->GetItemLocation(hintedLocation)->SetPlacedItem(RG_NONE);

    ctx->GetLogic()->Reset();
    auto accessibleGossipStones = ReachabilitySearch(Rando::StaticData::GetGossipStoneLocations());
    // Give the item back to the location
    ctx->GetItemLocation(hintedLocation)->SetPlacedItem(originalItem);

    return accessibleGossipStones;
}

bool IsReachableWithout(std::vector<RandomizerCheck> locsToCheck, RandomizerCheck excludedCheck,
                        bool resetAfter = true) {
    // temporarily remove the hinted location's item, and then perform a
    // reachability search for this check RANDOTODO convert excludedCheck to an ItemLocation
    auto ctx = Rando::Context::GetInstance();
    RandomizerGet originalItem = ctx->GetItemLocation(excludedCheck)->GetPlacedRandomizerGet();
    ctx->GetItemLocation(excludedCheck)->SetPlacedItem(RG_NONE);
    ctx->GetLogic()->Reset();
    const auto rechableWithout = ReachabilitySearch(locsToCheck);
    ctx->GetItemLocation(excludedCheck)->SetPlacedItem(originalItem);
    if (resetAfter) {
        // if resetAfter is on, reset logic we are done
        ctx->GetLogic()->Reset();
    }
    if (rechableWithout.empty()) {
        return false;
    }
    return true;
}

static void SetAllInAreaAsHintAccesible(RandomizerArea area, std::vector<RandomizerCheck> locations) {
    auto ctx = Rando::Context::GetInstance();
    std::vector<RandomizerCheck> locsInArea = FilterFromPool(locations, [area, ctx](const RandomizerCheck loc) {
        return ctx->GetItemLocation(loc)->GetAreas().contains(area);
    });
    for (RandomizerCheck loc : locsInArea) {
        ctx->GetItemLocation(loc)->SetHintAccesible();
    }
}

static void AddGossipStoneHint(const RandomizerCheck gossipStone, const HintType hintType,
                               const std::string distributionName, const std::vector<RandomizerHintTextKey> hintKeys,
                               const std::vector<RandomizerCheck> locations, const std::vector<RandomizerArea> areas,
                               const std::vector<TrialKey> trials) {
    auto ctx = Rando::Context::GetInstance();
    ctx->AddHint(StaticData::gossipStoneCheckToHint[gossipStone],
                 Hint(StaticData::gossipStoneCheckToHint[gossipStone], hintType, distributionName, hintKeys, locations,
                      areas, trials));
    ctx->GetItemLocation(gossipStone)
        ->SetPlacedItem(RG_HINT); // RANDOTODO, better gossip stone to location to hint key system
}

static void AddGossipStoneHintCopies(uint8_t copies, const HintType hintType, const std::string distributionName,
                                     const std::vector<RandomizerHintTextKey> hintKeys = {},
                                     const std::vector<RandomizerCheck> locations = {},
                                     const std::vector<RandomizerArea> areas = {},
                                     const std::vector<TrialKey> trials = {},
                                     RandomizerCheck firstStone = RC_UNKNOWN_CHECK) {

    if (firstStone != RC_UNKNOWN_CHECK && copies > 0) {
        AddGossipStoneHint(firstStone, hintType, distributionName, hintKeys, locations, areas, trials);
        copies -= 1;
    }
    for (int c = 0; c < copies; c++) {
        // get a random gossip stone
        auto gossipStones = GetEmptyGossipStones();
        if (gossipStones.empty()) {
            SPDLOG_DEBUG("\tNO GOSSIP STONES TO PLACE HINT");
            return;
        }
        auto gossipStone = RandomElement(gossipStones, false);
        AddGossipStoneHint(gossipStone, hintType, distributionName, hintKeys, locations, areas, trials);
    }
}

static bool CreateHint(RandomizerCheck location, uint8_t copies, HintType type, std::string distribution) {
    auto ctx = Rando::Context::GetInstance();

    // get a gossip stone accessible without the hinted item
    std::vector<RandomizerCheck> gossipStoneLocations = GetAccessibleGossipStones(location);
    if (gossipStoneLocations.empty()) {
        SPDLOG_DEBUG("\tNO IN LOGIC GOSSIP STONE");
        return false;
    }
    RandomizerCheck gossipStone = RandomElement(gossipStoneLocations);
    RandomizerArea area = ctx->GetItemLocation(location)->GetRandomArea();

    // Set that hints are accesible
    ctx->GetItemLocation(location)->SetHintAccesible();
    if (type == HINT_TYPE_FOOLISH) {
        SetAllInAreaAsHintAccesible(area, ctx->allLocations);
    }

    AddGossipStoneHintCopies(copies, type, distribution, {}, { location }, { area }, {}, gossipStone);
    return true;
}

static RandomizerCheck CreateRandomHint(std::vector<RandomizerCheck>& possibleHintLocations, uint8_t copies,
                                        HintType type, std::string distributionName) {
    auto ctx = Rando::Context::GetInstance();

    // return if there aren't any hintable locations or gossip stones available
    if (GetEmptyGossipStones().size() < copies) {
        SPDLOG_DEBUG("\tNOT ENOUGH GOSSIP STONES TO PLACE HINTS");
        return RC_UNKNOWN_CHECK;
    }

    RandomizerCheck hintedLocation;
    bool placed = false;
    while (!placed) {
        if (possibleHintLocations.empty()) {
            SPDLOG_DEBUG("\tNO LOCATIONS TO HINT");
            return RC_UNKNOWN_CHECK;
        }
        hintedLocation =
            RandomElement(possibleHintLocations, true); // removing the location to avoid it being hinted again on fail

        SPDLOG_DEBUG("\tLocation: {}", Rando::StaticData::GetLocation(hintedLocation)->GetName());
        SPDLOG_DEBUG("\tItem: {}", ctx->GetItemLocation(hintedLocation)->GetPlacedItemName().GetEnglish());

        placed = CreateHint(hintedLocation, copies, type, distributionName);
    }
    return hintedLocation;
}

static std::vector<RandomizerCheck> FilterHintability(std::vector<RandomizerCheck>& locations,
                                                      std::function<bool(RandomizerCheck)> extraFilter = NoFilter) {
    auto ctx = Rando::Context::GetInstance();
    return FilterFromPool(locations, [extraFilter, ctx](const RandomizerCheck loc) {
        return ctx->GetItemLocation(loc)->IsHintable() && !(ctx->GetItemLocation(loc)->IsAHintAccessible()) &&
               extraFilter(loc);
    });
}

static void CreateTrialHints(uint8_t copies) {
    if (copies > 0) {
        auto ctx = Rando::Context::GetInstance();
        if (ctx->GetOption(RSK_TRIAL_COUNT).Is(6)) { // six trials
            AddGossipStoneHintCopies(copies, HINT_TYPE_HINT_KEY, "Trial", { RHT_SIX_TRIALS });
        } else if (ctx->GetOption(RSK_TRIAL_COUNT).Is(0)) { // zero trials
            AddGossipStoneHintCopies(copies, HINT_TYPE_HINT_KEY, "Trial", { RHT_ZERO_TRIALS });
        } else {
            std::vector<TrialInfo*> trials =
                ctx->GetTrials()->GetTrialList();             // there's probably a way to remove this assignment
            if (ctx->GetOption(RSK_TRIAL_COUNT).Get() >= 4) { // 4 or 5 required trials, get skipped trials
                trials = FilterFromPool(trials, [](TrialInfo* trial) { return trial->IsSkipped(); });
            } else { // 1 to 3 trials, get requried trials
                auto requiredTrials = FilterFromPool(trials, [](TrialInfo* trial) { return trial->IsRequired(); });
            }
            for (auto& trial : trials) { // create a hint for each hinted trial
                AddGossipStoneHintCopies(copies, HINT_TYPE_TRIAL, "Trial", {}, {}, {}, { trial->GetTrialKey() });
            }
        }
    }
}

void CreateWarpSongTexts() {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_WARP_SONG_HINTS)) {
        auto warpSongEntrances = GetShuffleableEntrances(EntranceType::WarpSong, false);
        for (auto entrance : warpSongEntrances) {
            // RANDOTODO make random
            RandomizerArea destination = entrance->GetConnectedRegion()->GetFirstArea();
            switch (entrance->GetIndex()) {
                case 0x0600: // minuet RANDOTODO make into entrance hints when they are added
                    ctx->AddHint(RH_MINUET_WARP_LOC,
                                 Hint(RH_MINUET_WARP_LOC, HINT_TYPE_AREA, "", { RHT_WARP_SONG }, {}, { destination }));
                    break;
                case 0x04F6: // bolero
                    ctx->AddHint(RH_BOLERO_WARP_LOC,
                                 Hint(RH_BOLERO_WARP_LOC, HINT_TYPE_AREA, "", { RHT_WARP_SONG }, {}, { destination }));
                    break;
                case 0x0604: // serenade
                    ctx->AddHint(RH_SERENADE_WARP_LOC, Hint(RH_SERENADE_WARP_LOC, HINT_TYPE_AREA, "", { RHT_WARP_SONG },
                                                            {}, { destination }));
                    break;
                case 0x01F1: // requiem
                    ctx->AddHint(RH_REQUIEM_WARP_LOC,
                                 Hint(RH_REQUIEM_WARP_LOC, HINT_TYPE_AREA, "", { RHT_WARP_SONG }, {}, { destination }));
                    break;
                case 0x0568: // nocturne
                    ctx->AddHint(RH_NOCTURNE_WARP_LOC, Hint(RH_NOCTURNE_WARP_LOC, HINT_TYPE_AREA, "", { RHT_WARP_SONG },
                                                            {}, { destination }));
                    break;
                case 0x05F4: // prelude
                    ctx->AddHint(RH_PRELUDE_WARP_LOC,
                                 Hint(RH_PRELUDE_WARP_LOC, HINT_TYPE_AREA, "", { RHT_WARP_SONG }, {}, { destination }));
                    break;
                default:
                    break;
            }
        }
    }
}

static int32_t getRandomWeight(uint32_t totalWeight) {
    return totalWeight <= 1 ? 1 : Random(1, totalWeight);
}

static void DistributeAndPlaceHints(std::vector<HintDistributionSetting>& distTable, size_t totalStones) {
    auto ctx = Rando::Context::GetInstance();
    const uint8_t junkIdx = distTable.size();

    // Apply fixed hints upfront (they don't participate in weighted selection)
    for (size_t i = 0; i < distTable.size(); i++) {
        if (distTable[i].fixed == 0) {
            continue;
        }
        uint8_t placed = 0;
        for (uint8_t c = 0; c < distTable[i].fixed; c++) {
            std::vector<RandomizerCheck> hintPool = FilterHintability(ctx->allLocations, distTable[i].filter);
            SPDLOG_DEBUG("Attempting fixed hint of type: {}",
                         StaticData::hintTypeNames[distTable[i].type].GetEnglish(MF_CLEAN));
            RandomizerCheck fixedLoc =
                CreateRandomHint(hintPool, distTable[i].copies, distTable[i].type, distTable[i].name);
            if (fixedLoc == RC_UNKNOWN_CHECK) {
                distTable[i].weight = 0;
                distTable[i].copies = 0;
                break;
            }
            placed++;
            if (Rando::StaticData::GetLocation(fixedLoc)->IsDungeon()) {
                distTable[i].dungeonLimit -= 1;
                if (distTable[i].dungeonLimit == 0) {
                    hintPool = FilterFromPool(hintPool, FilterOverworldLocations);
                }
            }
        }
        totalStones -= placed * distTable[i].copies;
    }

    while (totalStones > 0) {
        // Pick a weighted distribution type (junk included)
        uint32_t totalWeight = 0;
        for (size_t i = 0; i < distTable.size(); i++) {
            totalWeight += distTable[i].weight;
        }

        // No weighted types left, fill remaining with junk
        if (totalWeight == 0) {
            for (size_t c = 0; c < totalStones; c++) {
                // duplicate junk hints are possible for now
                AddGossipStoneHintCopies(1, HINT_TYPE_HINT_KEY, "Junk", { GetRandomJunkHint() });
            }
            return;
        }

        uint32_t roll = getRandomWeight(totalWeight);
        uint32_t cursor = 0;
        uint8_t chosenType = junkIdx;
        for (size_t i = 0; i < distTable.size(); i++) {
            cursor += distTable[i].weight;
            if (roll <= cursor) {
                chosenType = static_cast<uint8_t>(i);
                break;
            }
        }

        if (chosenType == junkIdx) {
            AddGossipStoneHintCopies(1, HINT_TYPE_HINT_KEY, "Junk", { GetRandomJunkHint() });
            totalStones -= 1;
            continue;
        }

        auto& dist = distTable[chosenType];

        // Need at least `copies` stones to place one instance of this type
        if (dist.copies == 0 || totalStones < dist.copies) {
            dist.weight = 0;
            dist.copies = 0;
            continue;
        }

        // Build hint pool and attempt placement
        std::vector<RandomizerCheck> hintPool = FilterHintability(ctx->allLocations, dist.filter);
        SPDLOG_DEBUG("Attempting to make hint of type: {}", StaticData::hintTypeNames[dist.type].GetEnglish(MF_CLEAN));

        RandomizerCheck hintedLocation = CreateRandomHint(hintPool, dist.copies, dist.type, dist.name);
        if (hintedLocation == RC_UNKNOWN_CHECK) {
            // Placement failed, disable this type entirely
            dist.weight = 0;
            dist.copies = 0;
            continue;
        }

        // Track dungeon limit
        if (Rando::StaticData::GetLocation(hintedLocation)->IsDungeon()) {
            dist.dungeonLimit -= 1;
            if (dist.dungeonLimit == 0) {
                hintPool = FilterFromPool(hintPool, FilterOverworldLocations);
            }
        }

        totalStones -= dist.copies;
    }
}

void CreateStoneHints() {
    auto ctx = Rando::Context::GetInstance();
    SPDLOG_DEBUG("NOW CREATING HINTS");
    const HintSetting& hintSetting = hintSettingTable[ctx->GetOption(RSK_HINT_DISTRIBUTION).Get()];
    std::vector<HintDistributionSetting> distTable = hintSetting.distTable;

    // Apply impa's song exclusions when zelda is skipped
    if (ctx->GetOption(RSK_SKIP_CHILD_ZELDA)) {
        ctx->GetItemLocation(RC_SONG_FROM_IMPA)->SetHintAccesible();
    }
    if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_ADULT) || !ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
        ctx->GetItemLocation(RC_TOT_MASTER_SWORD)->SetHintAccesible();
    }

    // Add 'always' location hints
    std::vector<RandomizerCheck> alwaysHintLocations = {};
    if (hintSetting.alwaysCopies > 0) {
        if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)) {
            // If we have Rainbow Bridge set to Greg and the greg hint isn't useful, add a hint for where Greg is
            // Do we really need this with the greg hint?
            auto gregLocations = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
                return ((ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_GREG_RUPEE)) &&
                       ctx->GetItemLocation(loc)->IsHintable() &&
                       !(ctx->GetOption(RSK_GREG_HINT) && IsReachableWithout({ RC_GREG_HINT }, loc, true));
            });
            if (gregLocations.size() > 0) {
                alwaysHintLocations.push_back(gregLocations[0]);
            }
        }

        for (const auto& hint : conditionalAlwaysHints) {
            if (ConditionalAlwaysHintApplies(hint) && ctx->GetItemLocation(hint.loc)->IsHintable()) {
                alwaysHintLocations.push_back(hint.loc);
            }
        }

        for (RandomizerCheck location : alwaysHintLocations) {
            CreateHint(location, hintSetting.alwaysCopies, HINT_TYPE_ITEM, "Always");
        }
    }

    // Add 'trial' location hints
    if (ctx->GetOption(RSK_GANONS_TRIALS).IsNot(RO_GANONS_TRIALS_SKIP)) {
        CreateTrialHints(hintSetting.trialCopies);
    }

    size_t totalStones = GetEmptyGossipStones().size();
    distTable.push_back({ "Junk", HINT_TYPE_HINT_KEY, hintSetting.junkWeight, 0, 1, NoFilter });
    DistributeAndPlaceHints(distTable, totalStones);

    // Getting gossip stone locations temporarily sets one location to not be reachable.
    // Call the function one last time to get rid of false positives on locations not
    // being reachable.
    ReachabilitySearch({});
}

std::vector<RandomizerCheck> FindItemsAndMarkHinted(std::vector<RandomizerGet> items,
                                                    std::vector<RandomizerCheck> hintChecks) {
    std::vector<RandomizerCheck> locations = {};
    auto ctx = Rando::Context::GetInstance();
    for (size_t c = 0; c < items.size(); c++) {
        std::vector<RandomizerCheck> found =
            FilterFromPool(ctx->allLocations, [items, ctx, c](const RandomizerCheck loc) {
                return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == items[c];
            });
        if (found.size() > 0) {
            locations.push_back(found[0]);
            // RANDOTODO make the called functions of this always return true if empty hintChecks are provided
            if (!ctx->GetItemLocation(found[0])->IsAHintAccessible() &&
                (hintChecks.size() == 0 || IsReachableWithout(hintChecks, found[0], true))) {
                ctx->GetItemLocation(found[0])->SetHintAccesible();
            }
        } else {
            locations.push_back(RC_UNKNOWN_CHECK);
        }
    }
    return locations;
}

static void CreateAltarHint(RandomizerHint hintKey, HintType hintType, std::vector<RandomizerGet> rewards,
                            RandomizerCheck altarCheck) {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetHint(hintKey)->IsEnabled()) {
        return;
    }
    std::vector<RandomizerCheck> locs = {};
    std::vector<RandomizerArea> areas = {};
    if (ctx->GetOption(RSK_TOT_ALTAR_HINT)) {
        // force marking the rewards as hinted if they are at the end of dungeons as they can be inferred
        const bool rewardsInferrable =
            ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON) ||
            ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_VANILLA);
        locs = FindItemsAndMarkHinted(rewards, rewardsInferrable ? std::vector<RandomizerCheck>{}
                                                                 : std::vector<RandomizerCheck>{ altarCheck });
        for (auto loc : locs) {
            if (loc != RC_UNKNOWN_CHECK) {
                areas.push_back(ctx->GetItemLocation(loc)->GetRandomArea());
            }
        }
    }
    ctx->AddHint(hintKey, Hint(hintKey, hintType, {}, locs, areas));
}

void CreateChildAltarHint() {
    CreateAltarHint(RH_ALTAR_CHILD, HINT_TYPE_ALTAR_CHILD, { RG_KOKIRI_EMERALD, RG_GORON_RUBY, RG_ZORA_SAPPHIRE },
                    RC_ALTAR_HINT_CHILD);
}

void CreateAdultAltarHint() {
    CreateAltarHint(RH_ALTAR_ADULT, HINT_TYPE_ALTAR_ADULT,
                    { RG_LIGHT_MEDALLION, RG_FOREST_MEDALLION, RG_FIRE_MEDALLION, RG_WATER_MEDALLION,
                      RG_SPIRIT_MEDALLION, RG_SHADOW_MEDALLION },
                    RC_ALTAR_HINT_ADULT);
}

void CreateStaticHintFromData(RandomizerHint hint, StaticHintInfo staticData) {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->GetHint(hint)->IsEnabled()) {
        OptionValue& option = ctx->GetOption(staticData.setting);
        if ((std::holds_alternative<bool>(staticData.condition) && option.Is(std::get<bool>(staticData.condition))) ||
            (std::holds_alternative<uint8_t>(staticData.condition) &&
             option.Is(std::get<uint8_t>(staticData.condition)))) {

            std::vector<RandomizerCheck> locations = {};
            if (staticData.targetItems.size() > 0) {
                locations = FindItemsAndMarkHinted(staticData.targetItems, staticData.hintChecks);
            } else {
                for (auto check : staticData.targetChecks) {
                    locations.push_back(check);
                }
            }
            std::vector<RandomizerArea> areas = {};
            for (auto loc : locations) {
                ctx->GetItemLocation(loc)->SetHintAccesible();
                if (ctx->GetItemLocation(loc)->GetAreas().empty()) {
                    // If we get to here then it means a location got through with no area assignment, which means
                    // something went wrong elsewhere.
                    SPDLOG_DEBUG("Attempted to hint location with no areas: ");
                    SPDLOG_DEBUG(Rando::StaticData::GetLocation(loc)->GetName());
                    // assert(false);
                    areas.push_back(RA_NONE);
                } else {
                    areas.push_back(ctx->GetItemLocation(loc)->GetRandomArea());
                }
            }
            // hintKeys are defaulted to in the hint object and do not need to be specified
            ctx->AddHint(hint,
                         Hint(hint, staticData.type, {}, locations, areas, {}, staticData.yourPocket, staticData.num));
        }
    }
}

void CreateStaticItemHint(RandomizerHint hintKey, std::vector<RandomizerHintTextKey> hintTextKeys,
                          std::vector<RandomizerGet> items, std::vector<RandomizerCheck> hintChecks,
                          bool yourPocket = false) {
    // RANDOTODO choose area in case there are multiple
    auto ctx = Rando::Context::GetInstance();
    std::vector<RandomizerCheck> locations = FindItemsAndMarkHinted(items, hintChecks);
    std::vector<RandomizerArea> areas = {};
    for (auto loc : locations) {
        areas.push_back(ctx->GetItemLocation(loc)->GetRandomArea());
    }
    ctx->AddHint(hintKey, Hint(hintKey, HINT_TYPE_AREA, hintTextKeys, locations, areas, {}, yourPocket));
}

void CreateGanondorfJoke() {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->GetHint(RH_GANONDORF_JOKE)->IsEnabled()) {
        ctx->AddHint(RH_GANONDORF_JOKE, Hint(RH_GANONDORF_JOKE, HINT_TYPE_HINT_KEY, { GetRandomGanonJoke() }));
    }
}

void CreateGanondorfHint() {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_GANONDORF_HINT) && !ctx->GetHint(RH_GANONDORF_HINT)->IsEnabled()) {
        if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD) && ctx->GetOption(RSK_STARTING_MASTER_SWORD).Is(RO_GENERIC_OFF)) {
            CreateStaticItemHint(
                RH_GANONDORF_HINT,
                { RHT_GANONDORF_HINT_LA_ONLY, RHT_GANONDORF_HINT_MS_ONLY, RHT_GANONDORF_HINT_LA_AND_MS },
                { RG_LIGHT_ARROWS, RG_MASTER_SWORD }, { RC_GANONDORF_HINT }, true);
        } else {
            CreateStaticItemHint(RH_GANONDORF_HINT, { RHT_GANONDORF_HINT_LA_ONLY }, { RG_LIGHT_ARROWS },
                                 { RC_GANONDORF_HINT }, true);
        }
    }
}

void CreateStaticHints() {
    CreateChildAltarHint();
    CreateAdultAltarHint();
    CreateGanondorfJoke();
    CreateGanondorfHint();
    for (auto [hint, staticData] : StaticData::staticHintInfoMap) {
        CreateStaticHintFromData(hint, staticData);
    }
}

void CreateAllHints() {
    auto ctx = Rando::Context::GetInstance();

    CreateStaticHints();

    if (ctx->GetOption(RSK_GOSSIP_STONE_HINTS).IsNot(RO_GOSSIP_STONES_NONE)) {
        SPDLOG_INFO("Creating Hints...");
        CreateStoneHints();
        SPDLOG_INFO("Creating Hints Done");
    }
}

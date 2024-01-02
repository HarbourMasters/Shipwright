#include "hints.hpp"

#include "custom_messages.hpp"
#include "item_pool.hpp"
#include "logic.hpp"
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

using namespace CustomMessages;
using namespace Logic;
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

std::array<std::string, HINT_TYPE_MAX> hintTypeNames = {
    "Static",
    "Trial",
    "WotH",
    "Barren",
    "Entrance",
    "Item Area",
    "Item Location",
    "Junk"
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
      {"Barren",     HINT_TYPE_BARREN,        4,  0, 1, FilterBarrenLocations,    1},
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
      {"Barren",     HINT_TYPE_BARREN,       12, 0, 1, FilterBarrenLocations,    1},
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
      {"Barren",     HINT_TYPE_BARREN,       15, 0, 1, FilterBarrenLocations},
      //{"Entrance",   HINT_TYPE_ENTRANCE,     10, 0, 1}, //not yet implemented
      {"Song",       HINT_TYPE_ITEM_LOCATION, 2, 0, 1, FilterSongLocations},
      {"Overworld",  HINT_TYPE_ITEM_LOCATION, 7, 0, 1, FilterOverworldLocations},
      {"Dungeon",    HINT_TYPE_ITEM_LOCATION, 7, 0, 1, FilterDungeonLocations},
      {"Named Item", HINT_TYPE_ITEM_AREA,     5, 0, 1, FilterGoodItems},
    },
  },
}};

Text AutoFormatHintText(Text unformattedHintText) {
  std::array<std::string, LANGUAGE_MAX> strings;
  for (int i = 0; i < LANGUAGE_MAX; i++) {
    std::string textStr = unformattedHintText.GetForLanguage(i);
    // RANDOTODO: don't just make manual exceptions
    bool needsAutomaticNewlines = true;
    if (textStr == "Erreur 0x69a504:&Traduction manquante^C'est de la faute à Purple Hato!&J'vous jure!" ||
      textStr == "Mon très cher @:&Viens vite au château, je t'ai préparé&un délicieux gâteau...^À bientôt, Princesse Zelda" ||
      textStr == "What about Zelda makes you think&she'd be a better ruler than I?^I saved Lon Lon Ranch,&fed the hungry,&and my castle floats." ||
      textStr == "Many tricks are up my sleeve,&to save yourself&you'd better leave!" ||
      textStr == "I've learned this spell,&it's really neat,&I'll keep it later&for your treat!" ||
      textStr == "Sale petit garnement,&tu fais erreur!&C'est maintenant que marque&ta dernière heure!" ||
      textStr == "Gamin, ton destin achève,&sous mon sort tu périras!&Cette partie ne fut pas brève,&et cette mort, tu subiras!" ||
      textStr == "Oh! It's @.&I was expecting someone called Sheik.&Do you know what happened to them?" ||
      textStr == "Ah, c'est @.&J'attendais un certain Sheik.&Tu sais ce qui lui est arrivé?" ||
      textStr == "They say \"Forgive me, but-^Your script will not be used.&....After all...^The one writing the rest of the script...&will be me.\"") {
    needsAutomaticNewlines = false;
  }

    if (needsAutomaticNewlines) {
        // insert newlines either manually or when encountering a '&'
        constexpr size_t lineLength = 34;
        size_t lastNewline = 0;
        while (lastNewline + lineLength < textStr.length()) {
            size_t carrot = textStr.find('^', lastNewline);
            size_t ampersand = textStr.find('&', lastNewline);
            size_t lastSpace = textStr.rfind(' ', lastNewline + lineLength);
            size_t lastPeriod = textStr.rfind('.', lastNewline + lineLength);
            // replace '&' first if it's within the newline range
            if (ampersand < lastNewline + lineLength) {
                lastNewline = ampersand;
                // or move the lastNewline cursor to the next line if a '^' is encountered
            } else if (carrot < lastNewline + lineLength) {
                lastNewline = carrot + 1;
                // some lines need to be split but don't have spaces, look for periods instead
            } else if (lastSpace == std::string::npos) {
                textStr.replace(lastPeriod, 1, ".&");
                lastNewline = lastPeriod + 2;
            } else {
                textStr.replace(lastSpace, 1, "&");
                lastNewline = lastSpace + 1;
            }
        }
    }

    // todo add colors (see `AddColorsAndFormat` in `custom_messages.cpp`)
    textStr.erase(std::remove(textStr.begin(), textStr.end(), '#'), textStr.end());
    strings[i] = textStr;
  }

  return Text(strings[0], strings[1], strings[2]);
}

std::array<DungeonHintInfo, 10> dungeonInfoData;

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

std::string masterSwordHintLoc;
std::string lightArrowHintLoc;
std::string dampeHintLoc;
std::string gregHintLoc;
std::string sariaHintLoc;


Text& GetChildAltarText() {
  return childAltarText;
}

Text& GetAdultAltarText() {
  return adultAltarText;
}

void SetGanonText(Text text){
  ganonText = text;
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

std::string GetMasterSwordHintLoc() {
    return masterSwordHintLoc;
}

std::string GetLightArrowHintLoc() {
    return lightArrowHintLoc;
}

std::string GetDampeHintLoc() {
    return dampeHintLoc;
}

std::string GetGregHintLoc() {
    return gregHintLoc;
}

std::string GetSariaHintLoc() {
  return sariaHintLoc;
}

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

  LogicReset();
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
  LogicReset();
  const auto rechableWithout = GetAccessibleLocations(locsToCheck);
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

static void SetAllInRegionAsHinted(RandomizerArea area, std::vector<RandomizerCheck> locations){
  auto ctx = Rando::Context::GetInstance();
  std::vector<RandomizerCheck> locsInRegion = FilterFromPool(locations, [area, ctx](const RandomizerCheck loc){
                                        return ctx->GetItemLocation(loc)->GetArea() == area;
                                      });
  for(RandomizerCheck loc: locsInRegion){
    ctx->GetItemLocation(loc)->SetAsHinted();
  }
}

static void AddGossipStoneHint(Text hint, 
                                const RandomizerCheck gossipStone,
                                const std::vector<uint8_t>& colors,
                                HintType hintType,
                                std::string distributionName,
                                const RandomizerCheck hintedLocation) {
  //save hints as dummy items for writing to the spoiler log
  //NewItem(gossipStone, Item{RG_HINT, hint, ITEMTYPE_EVENT, GI_RUPEE_BLUE_LOSE, false, &noVariable, NONE});
  //GetLocation(gossipStone)->SetPlacedItem(gossipStone);

  auto ctx = Rando::Context::GetInstance();
  ctx->AddHint((RandomizerHintKey)((gossipStone - RC_COLOSSUS_GOSSIP_STONE) + 1), AutoFormatHintText(hint), hintedLocation, hintType, distributionName, ctx->GetItemLocation(hintedLocation)->GetArea());
  ctx->GetItemLocation(gossipStone)->SetPlacedItem(RG_HINT); //RANDOTODO, better gossip stone to location to hint key system
}

static void AddGossipStoneHintCopies(uint8_t copies, 
                                      Text hint, 
                                      std::vector<uint8_t> colours, 
                                      HintType type, 
                                      std::string distributionName,
                                      RandomizerCheck location = RC_UNKNOWN_CHECK, 
                                      RandomizerCheck firstStone = RC_UNKNOWN_CHECK){
  if (firstStone != RC_UNKNOWN_CHECK && copies > 0){
      AddGossipStoneHint(hint, firstStone, colours, type, distributionName, location);
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

    AddGossipStoneHint(hint, gossipStone, colours, type, distributionName, location);
  }
}

static bool CreateHint(RandomizerCheck hintedLocation, uint8_t copies, HintType type, std::string distributionName){
  auto ctx = Rando::Context::GetInstance();
  //get a gossip stone accessible without the hinted item
  std::vector<RandomizerCheck> gossipStoneLocations;
  gossipStoneLocations = GetAccessibleGossipStones(hintedLocation);

  if (gossipStoneLocations.empty()) { 
      SPDLOG_DEBUG("\tNO IN LOGIC GOSSIP STONE\n\n"); 
      return false;
  }

  RandomizerCheck gossipStone = RandomElement(gossipStoneLocations);

  //make hint text
  Text finalHint;
  Text prefix = ::Hint(RHT_PREFIX).GetText();
  std::vector<uint8_t> colours;
  if (type == HINT_TYPE_WOTH){
    Text areaText = ::Hint(ctx->GetItemLocation(hintedLocation)->GetArea()).GetText();
    finalHint = prefix + "%r#" + areaText + "#%w" + ::Hint(RHT_WAY_OF_THE_HERO).GetText();
    colours = {QM_LBLUE};
  }
  else if(type == HINT_TYPE_BARREN){
    Text areaText = ::Hint(ctx->GetItemLocation(hintedLocation)->GetArea()).GetText();
    finalHint = prefix + ::Hint(RHT_PLUNDERING).GetText() + "%r#" + areaText + "#%w" + ::Hint(RHT_FOOLISH).GetText();
    colours = {QM_PINK};
  }
  else {
    Text itemText = ctx->GetItemLocation(hintedLocation)->GetPlacedItem().GetHint().GetText();
    if (type == HINT_TYPE_ITEM_LOCATION){
      Text locationText = Rando::StaticData::GetLocation(hintedLocation)->GetHint()->GetText();
      finalHint = prefix + "%r" + locationText + " %g#"+itemText+"#%w.";
      colours = {QM_GREEN, QM_RED};
    }
    else if (type == HINT_TYPE_ITEM_AREA){
      Text areaText = ::Hint(ctx->GetItemLocation(hintedLocation)->GetArea()).GetText();
      if (Rando::StaticData::GetLocation(hintedLocation)->IsDungeon()) {
        finalHint = prefix+"%r#"+areaText+"#%w "+::Hint(RHT_HOARDS).GetText()+" %g#"+itemText+"#%w.";
        colours = {QM_GREEN, QM_RED};
      } else {
        finalHint = prefix+"%r#"+itemText+"#%w "+::Hint(RHT_CAN_BE_FOUND_AT).GetText()+" %g#"+areaText+"#%w.";
        colours = {QM_RED, QM_GREEN};
      }
    }
    else{
      SPDLOG_DEBUG("\tINVALID HINT TYPE\n\n");
      return false;
    }
  }
  
  ctx->GetItemLocation(hintedLocation)->SetAsHinted();

  SPDLOG_DEBUG("\tMessage: ");
  SPDLOG_DEBUG(finalHint.english);
  SPDLOG_DEBUG("\n\n");

  AddGossipStoneHintCopies(copies, finalHint, colours, type, distributionName, hintedLocation, gossipStone);

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
  if (type == HINT_TYPE_BARREN){
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
    const HintText junkHint = RandomElement(GetHintCategory(HintCategory::Junk));
    Text hint = junkHint.GetText();

    SPDLOG_DEBUG("\tMessage: ");
    SPDLOG_DEBUG(hint.english);
    SPDLOG_DEBUG("\n\n");

    AddGossipStoneHintCopies(1, hint, {QM_PINK}, HINT_TYPE_JUNK, "Junk");
  }
}

static void CreateTrialHints(uint8_t copies) {
  if (copies > 0) {
    auto ctx = Rando::Context::GetInstance();
  Text prefix = ::Hint(RHT_PREFIX).GetText();
      //six trials
    if (ctx->GetOption(RSK_GANONS_TRIALS).IsNot(RO_GANONS_TRIALS_SKIP) && ctx->GetOption(RSK_TRIAL_COUNT).Is(6)) {
      AddGossipStoneHintCopies(copies, prefix + ::Hint(RHT_SIX_TRIALS).GetText(), {QM_PINK}, HINT_TYPE_TRIAL, "Trial");
      //zero trials
    } else if (ctx->GetOption(RSK_GANONS_TRIALS).IsNot(RO_GANONS_TRIALS_SKIP) && ctx->GetOption(RSK_TRIAL_COUNT).Is(0)) {
      AddGossipStoneHintCopies(copies, prefix + ::Hint(RHT_ZERO_TRIALS).GetText(), {QM_YELLOW}, HINT_TYPE_TRIAL, "Trial");
      //4 or 5 required trials
    } else if (ctx->GetOption(RSK_TRIAL_COUNT).Is(5) || ctx->GetOption(RSK_TRIAL_COUNT).Is(4)) {
      //get skipped trials
      std::vector<TrialInfo*> trials = {};
      auto trialList = ctx->GetTrials()->GetTrialList();
    trials.assign(trialList.begin(), trialList.end());
      auto skippedTrials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsSkipped();});

      //create a hint for each skipped trial
      for (auto& trial : skippedTrials) {
        //make hint
        auto hint = prefix+"#"+trial->GetName()+"#"+::Hint(RHT_FOUR_TO_FIVE_TRIALS).GetText();
        AddGossipStoneHintCopies(copies, hint, {QM_YELLOW}, HINT_TYPE_TRIAL, "Trial");
      }
      //1 to 3 trials
    } else if (ctx->GetOption(RSK_TRIAL_COUNT).Value<uint8_t>() >= 1 && ctx->GetOption(RSK_TRIAL_COUNT).Value<uint8_t>() <= 3) {
      //get requried trials
      std::vector<TrialInfo*> trials = {};
      auto trialList = ctx->GetTrials()->GetTrialList();
    trials.assign(trialList.begin(), trialList.end());
      auto requiredTrials = FilterFromPool(trials, [](TrialInfo* trial){return trial->IsRequired();});

      //create a hint for each required trial
      for (auto& trial : requiredTrials) {
        //make hint
        auto hint = prefix+"#"+trial->GetName()+"#"+::Hint(RHT_ONE_TO_THREE_TRIALS).GetText();
        AddGossipStoneHintCopies(copies, hint, {QM_PINK}, HINT_TYPE_TRIAL, "Trial");
      }
    }
  }
}

//RANDOTODO clean this mess up once starting items are expanded
void CreateGanonAndSheikText() {
  auto ctx = Rando::Context::GetInstance();
  if(ctx->GetOption(RSK_LIGHT_ARROWS_HINT)){
    //Get the location of the light arrows
    auto lightArrowLocation = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_LIGHT_ARROWS;
    });
    RandomizerArea lightArrowArea = ctx->GetItemLocation(lightArrowLocation[0])->GetArea();
    Text LightArrowAreaText = ::Hint(lightArrowArea).GetText();
    std::vector<RandomizerCheck> locsToCheck = {RC_GANONDORF_HINT};

    //If there is no light arrow location, it was in the player's inventory at the start
    auto hint = ::Hint(RHT_LIGHT_ARROW_LOCATION_HINT);
    if (lightArrowLocation.empty()) {
      ganonHintText = hint.GetText()+::Hint(RHT_YOUR_POCKET).GetText();
      lightArrowHintLoc = "Link's Pocket";
    } else {
      ganonHintText = hint.GetText() + "%r" + LightArrowAreaText + "%w";
      lightArrowHintLoc = Rando::StaticData::GetLocation(lightArrowLocation[0])->GetName();
    }
    ganonHintText = ganonHintText + "!";

    //Get the location of the master sword
    auto masterSwordLocation = FilterFromPool(ctx->allLocations, [ctx](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == RG_MASTER_SWORD;
    });
    Text masterSwordArea = ::Hint(ctx->GetItemLocation(masterSwordLocation[0])->GetArea()).GetText();

    if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
      // Add second text box
      ganonHintText = ganonHintText + "^";
      if (masterSwordLocation.empty()) {
        ganonHintText = ganonHintText + ::Hint(RHT_MASTER_SWORD_LOCATION_HINT).GetText() + "%r" + ::Hint(RHT_YOUR_POCKET).GetText() + "%w";
        masterSwordHintLoc = "Link's Pocket";
      } else {
        ganonHintText = ganonHintText + ::Hint(RHT_MASTER_SWORD_LOCATION_HINT).GetText() + "%r" + masterSwordArea + "%w";
        masterSwordHintLoc = Rando::StaticData::GetLocation(masterSwordLocation[0])->GetName();
      }
      ganonHintText = ganonHintText + "!";
    }

    //CreateMessageFromTextObject(0x70CC, 0, 2, 3, AddColorsAndFormat(ganonHintText));
    ctx->AddHint(RH_GANONDORF_HINT, AutoFormatHintText(ganonHintText), lightArrowLocation[0], HINT_TYPE_STATIC, "Static", lightArrowArea);

    if(!ctx->GetOption(RSK_TRIAL_COUNT).Is(0)){
      sheikText = ::Hint(RHT_SHEIK_LIGHT_ARROW_HINT).GetText() + LightArrowAreaText + "%w.";
      locsToCheck = {RC_GANONDORF_HINT, RC_SHEIK_HINT_GC, RC_SHEIK_HINT_MQ_GC};

      if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
        sheikText = sheikText + "^" + ::Hint(RHT_SHEIK_MASTER_SWORD_LOCATION_HINT).GetText() + masterSwordArea + "%w.";
      }
    }

    if (IsReachableWithout(locsToCheck, lightArrowLocation[0], true)) {
      ctx->GetItemLocation(lightArrowLocation[0])->SetAsHinted();
    }
    ctx->AddHint(RH_SHEIK_LIGHT_ARROWS, AutoFormatHintText(sheikText), lightArrowLocation[0], HINT_TYPE_STATIC, "Static", lightArrowArea);

    if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
      if (IsReachableWithout(locsToCheck, masterSwordLocation[0], true)) {
        ctx->GetItemLocation(masterSwordLocation[0])->AddHintedBy(RH_GANONDORF_HINT);
        ctx->GetItemLocation(masterSwordLocation[0])->SetAsHinted();
      }
    }
  }
}

//Find the location which has the given itemKey and create the generic altar text for the reward
static Text BuildDungeonRewardText(const RandomizerGet itemKey, bool isChild) {
    auto ctx = Rando::Context::GetInstance();
    RandomizerCheck altarLoc = RC_ALTAR_HINT_ADULT;
    if(isChild){altarLoc = RC_ALTAR_HINT_CHILD;}
    RandomizerCheck location = FilterFromPool(ctx->allLocations, [itemKey, ctx](const RandomizerCheck loc) {
        return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == itemKey;
    })[0];
    if (IsReachableWithout({altarLoc}, location, true) || ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)){ //RANDOTODO check if works properly
      ctx->GetItemLocation(location)->SetAsHinted();
    }

    std::string rewardString = "$" + std::to_string(itemKey - RG_KOKIRI_EMERALD);

    // RANDOTODO implement colors for locations
    return Text() + rewardString +
           ::Hint(ctx->GetItemLocation(location)->GetArea()).GetText().Capitalize() +
           "...^";
}

static Text BuildDoorOfTimeText() {
  auto ctx = Rando::Context::GetInstance();
  std::string itemObtained;
  Text doorOfTimeText;

  if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_OPEN)) {
    itemObtained = "$o";
    doorOfTimeText = ::Hint(RHT_CHILD_ALTAR_TEXT_END_DOTOPEN).GetText();

  } else if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_SONGONLY)) {
    itemObtained = "$c";
    doorOfTimeText = ::Hint(RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY).GetText();

  } else if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_CLOSED)) {
    itemObtained = "$i";
    doorOfTimeText = ::Hint(RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED).GetText();
  }

  return Text()+itemObtained+doorOfTimeText;
}

//insert the required number into the hint and set the singular/plural form
static Text BuildCountReq(const RandomizerHintTextKey req, const Rando::Option& count) {
  Text requirement = ::Hint(req).GetTextCopy();
    if (count.Value<uint8_t>() == 1) {
    requirement.SetForm(SINGULAR);
  } else {
    requirement.SetForm(PLURAL);
  }
  requirement.Replace("%d", std::to_string(count.Value<uint8_t>()));
  return requirement;
}

static Text BuildBridgeReqsText() {
  auto ctx = Rando::Context::GetInstance();
  Text bridgeText;

  if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_ALWAYS_OPEN)) {
    bridgeText = ::Hint(RHT_BRIDGE_OPEN_HINT).GetText();

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_VANILLA)) {
      bridgeText = ::Hint(RHT_BRIDGE_VANILLA_HINT).GetText();

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)) {
      bridgeText = BuildCountReq(RHT_BRIDGE_STONES_HINT, ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT));

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS)) {
      bridgeText = BuildCountReq(RHT_BRIDGE_MEDALLIONS_HINT, ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT));

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
      bridgeText = BuildCountReq(RHT_BRIDGE_REWARDS_HINT, ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT));

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)) {
      bridgeText = BuildCountReq(RHT_BRIDGE_DUNGEONS_HINT, ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT));

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)) {
      bridgeText = BuildCountReq(RHT_BRIDGE_TOKENS_HINT, ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT));

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)) {
      bridgeText = ::Hint(RHT_BRIDGE_GREG_HINT).GetText();
  }

  return Text()+"$l"+bridgeText+"^";
}

static Text BuildGanonBossKeyText() {
  auto ctx = Rando::Context::GetInstance();
  Text ganonBossKeyText;

  if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_STARTWITH)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_START_WITH_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_VANILLA)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_VANILLA_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OWN_DUNGEON)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_OWN_DUNGEON_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANY_DUNGEON)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_ANY_DUNGEON_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OVERWORLD)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_OVERWORLD_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANYWHERE)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_ANYWHERE_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_SKULLTULA_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_VANILLA)) {
    ganonBossKeyText = ::Hint(RHT_LACS_VANILLA_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_STONES_HINT, ctx->GetOption(RSK_LACS_STONE_COUNT));

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_MEDALLIONS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_MEDALLIONS_HINT, ctx->GetOption(RSK_LACS_MEDALLION_COUNT));

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_REWARDS_HINT, ctx->GetOption(RSK_LACS_REWARD_COUNT));

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_DUNGEONS_HINT, ctx->GetOption(RSK_LACS_DUNGEON_COUNT));

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
    ganonBossKeyText = BuildCountReq(RHT_LACS_TOKENS_HINT, ctx->GetOption(RSK_LACS_TOKEN_COUNT));

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_TRIFORCE_HUNT)) {
    ganonBossKeyText = ::Hint(RHT_GANON_BK_TRIFORCE_HINT).GetText();
  }

  return Text()+"$b"+ganonBossKeyText+"^";
}

void CreateAltarText() {
  auto ctx = Rando::Context::GetInstance();

  //Child Altar Text
  if (ctx->GetOption(RSK_TOT_ALTAR_HINT)) {
    childAltarText = ::Hint(RHT_SPIRITUAL_STONE_TEXT_START).GetText()+"^"+
    //Spiritual Stones
    // TODO: Starting Inventory Dungeon Rewards
        (/*StartingKokiriEmerald.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_KOKIRI_EMERALD, true)) +
        (/*StartingGoronRuby.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                            : BuildDungeonRewardText(RG_GORON_RUBY, true)) +
        (/*StartingZoraSapphire.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                              : BuildDungeonRewardText(RG_ZORA_SAPPHIRE, true)) +
    //How to open Door of Time, the event trigger is necessary to read the altar multiple times
    BuildDoorOfTimeText();
  } else {
    childAltarText = BuildDoorOfTimeText();
  }
  ctx->AddHint(RH_ALTAR_CHILD, childAltarText, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static");

  //CreateMessageFromTextObject(0x7040, 0, 2, 3, AddColorsAndFormat(childAltarText, {QM_GREEN, QM_RED, QM_BLUE}));

  //Adult Altar Text
  adultAltarText = ::Hint(RHT_ADULT_ALTAR_TEXT_START).GetText() + "^";
  if (ctx->GetOption(RSK_TOT_ALTAR_HINT)) {
    adultAltarText = adultAltarText +
    //Medallion Areas
        (/*StartingLightMedallion.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_LIGHT_MEDALLION, false)) +
        (/*StartingForestMedallion.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(RG_FOREST_MEDALLION, false)) +
        (/*StartingFireMedallion.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_FIRE_MEDALLION, false)) +
        (/*StartingWaterMedallion.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                                : BuildDungeonRewardText(RG_WATER_MEDALLION, false)) +
        (/*StartingSpiritMedallion.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(RG_SPIRIT_MEDALLION, false)) +
        (/*StartingShadowMedallion.Value<uint8_t>()*/false ? Text{ "##", "##", "##" }
                                                  : BuildDungeonRewardText(RG_SHADOW_MEDALLION, false));
  }
  adultAltarText = adultAltarText + 
  //Bridge requirement
  BuildBridgeReqsText()+

  //Ganons Boss Key requirement
  BuildGanonBossKeyText()+

  //End
  ::Hint(RHT_ADULT_ALTAR_TEXT_END).GetText();
  //CreateMessageFromTextObject(0x7088, 0, 2, 3, AddColorsAndFormat(adultAltarText, {QM_RED, QM_YELLOW, QM_GREEN, QM_RED, QM_BLUE, QM_YELLOW, QM_PINK, QM_RED, QM_RED, QM_RED, QM_RED}));
  ctx->AddHint(RH_ALTAR_ADULT, adultAltarText, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static");
}

void CreateMerchantsHints() {
    auto ctx = Rando::Context::GetInstance();
    Text medigoronItemText = ctx->GetItemLocation(RC_GC_MEDIGORON)->GetPlacedItem().GetHint().GetText();
    Text grannyItemText = ctx->GetItemLocation(RC_KAK_GRANNYS_SHOP)->GetPlacedItem().GetHint().GetText().Capitalize();
    Text carpetSalesmanItemText =
        ctx->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPlacedItem().GetHint().GetText();
    Text carpetSalesmanItemClearText =
        ctx->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPlacedItem().GetHint().GetClear();


    Text medigoronText =
        ::Hint(RHT_MEDIGORON_DIALOG_FIRST).GetText() + medigoronItemText + ::Hint(RHT_MEDIGORON_DIALOG_SECOND).GetText();
    Text grannyText = grannyItemText + ::Hint(RHT_GRANNY_DIALOG).GetText();
    Text carpetSalesmanTextOne = ::Hint(RHT_CARPET_SALESMAN_DIALOG_FIRST).GetText() + carpetSalesmanItemText +
                                 ::Hint(RHT_CARPET_SALESMAN_DIALOG_SECOND).GetText();
    Text carpetSalesmanTextTwo = ::Hint(RHT_CARPET_SALESMAN_DIALOG_THIRD).GetText() + carpetSalesmanItemClearText +
                                 ::Hint(RHT_CARPET_SALESMAN_DIALOG_FOURTH).GetText();
    
    // CreateMessageFromTextObject(0x9120, 0, 2, 3, AddColorsAndFormat(medigoronText, { QM_RED, QM_GREEN }));
    // CreateMessageFromTextObject(0x9121, 0, 2, 3, AddColorsAndFormat(grannyText, { QM_RED, QM_GREEN }));
    // CreateMessageFromTextObject(0x6077, 0, 2, 3, AddColorsAndFormat(carpetSalesmanTextOne, { QM_RED, QM_GREEN }));
    // CreateMessageFromTextObject(0x6078, 0, 2, 3,
                                // AddColorsAndFormat(carpetSalesmanTextTwo, { QM_RED, QM_YELLOW, QM_RED }));
    // ctx->AddHint(RH_MEDIGORON, AutoFormatHintText(medigoronText), RC_GC_MEDIGORON, HINT_TYPE_STATIC, GetHintRegion(RR_GORON_CITY)->GetHint().GetText());
    // ctx->AddHint(RH_GRANNYS_SHOP, AutoFormatHintText(grannyText), RC_KAK_GRANNYS_SHOP, HINT_TYPE_STATIC, GetHintRegion(RR_KAKARIKO_VILLAGE)->GetHint().GetText());
    // ctx->AddHint(RH_WASTELAND_BOMBCHU_SALESMAN, AutoFormatHintText(carpetSalesmanTextOne), RC_WASTELAND_BOMBCHU_SALESMAN, HINT_TYPE_STATIC, GetHintRegion(RR_HAUNTED_WASTELAND)->GetHint().GetText());
}

//RANDOTODO add Better Links Pocket and starting item handling once more starting items are added
void CreateSpecialItemHint(uint32_t item, RandomizerHintKey hintKey, std::vector<RandomizerCheck> hints, RandomizerHintTextKey text1, RandomizerHintTextKey text2, Text& textLoc, std::string& nameLoc, bool condition, bool yourpocket = false) {
  auto ctx = Rando::Context::GetInstance();
  if(condition){
      RandomizerCheck location = FilterFromPool(ctx->allLocations, [item, ctx](const RandomizerCheck loc) {
      return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == item;
  })[0];

    if (IsReachableWithout(hints,location,true)){
      ctx->GetItemLocation(location)->SetAsHinted();
    }
    
    RandomizerArea area = ctx->GetItemLocation(location)->GetArea();
    textLoc = ::Hint(text1).GetText() + ::Hint(area).GetText() + ::Hint(text2).GetText();
    nameLoc = Rando::StaticData::GetLocation(location)->GetName();
    ctx->AddHint(hintKey, AutoFormatHintText(textLoc), location, HINT_TYPE_STATIC, "Static", area);
  } else {
    textLoc = Text();
    nameLoc = "";
  }
}

void CreateWarpSongTexts() {
  auto ctx = Rando::Context::GetInstance();
  if (!ctx->GetOption(RSK_WARP_SONG_HINTS)) {
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
    auto destination = entrance->GetConnectedRegion()->GetArea();
    Text resolvedHint = ::Hint(destination).GetText();
    // If no Hint Area, use Region text
    if (destination == RA_NONE) {
      resolvedHint = Text{"","",""} + entrance->GetConnectedRegion()->regionName;
    }

    switch (entrance->GetIndex()) {
      case 0x0600: // minuet RANDOTODO make an entrance enum and use it here
        warpMinuetText = resolvedHint;
        ctx->AddHint(RH_MINUET_WARP_LOC, resolvedHint, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", destination);
        break;
      case 0x04F6: // bolero
        warpBoleroText = resolvedHint;
        ctx->AddHint(RH_BOLERO_WARP_LOC, resolvedHint, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", destination);
        break;
      case 0x0604: // serenade
        warpSerenadeText = resolvedHint;
        ctx->AddHint(RH_SERENADE_WARP_LOC, resolvedHint, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", destination);
        break;
      case 0x01F1: // requiem
        warpRequiemText = resolvedHint;
        ctx->AddHint(RH_REQUIEM_WARP_LOC, resolvedHint, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", destination);
        break;
      case 0x0568: // nocturne
          warpNocturneText = resolvedHint;
          ctx->AddHint(RH_NOCTURNE_WARP_LOC, resolvedHint, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", destination);
          break;
      case 0x05F4: // prelude
        warpPreludeText = resolvedHint;
        ctx->AddHint(RH_PRELUDE_WARP_LOC, resolvedHint, RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", destination);
        break;
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

  // Apply Special hint exclusions with no requirements
  if (ctx->GetOption(RSK_KAK_10_SKULLS_HINT)){
      ctx->GetItemLocation(RC_KAK_10_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (ctx->GetOption(RSK_KAK_20_SKULLS_HINT)){
      ctx->GetItemLocation(RC_KAK_20_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (ctx->GetOption(RSK_KAK_30_SKULLS_HINT)){
      ctx->GetItemLocation(RC_KAK_30_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (ctx->GetOption(RSK_KAK_40_SKULLS_HINT)){
      ctx->GetItemLocation(RC_KAK_40_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (ctx->GetOption(RSK_KAK_50_SKULLS_HINT)){
      ctx->GetItemLocation(RC_KAK_50_GOLD_SKULLTULA_REWARD)->SetAsHinted();
  }
  if (ctx->GetOption(RSK_FROGS_HINT)){
      ctx->GetItemLocation(RC_ZR_FROGS_OCARINA_GAME)->SetAsHinted();
  }
  if (ctx->GetOption(RSK_SKIP_CHILD_ZELDA)){
      ctx->GetItemLocation(RC_SONG_FROM_IMPA)->SetAsHinted();
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
  CreateTrialHints(hintSetting.trialCopies);

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

void CreateAllHints(){
  auto ctx = Rando::Context::GetInstance();
  CreateGanonAndSheikText();
  CreateAltarText();
  CreateSpecialItemHint(RG_PROGRESSIVE_HOOKSHOT, RH_DAMPES_DIARY, {RC_DAMPE_HINT}, RHT_DAMPE_DIARY01, RHT_DAMPE_DIARY02, dampesText, dampeHintLoc, (bool)ctx->GetOption(RSK_DAMPES_DIARY_HINT));
  CreateSpecialItemHint(RG_GREG_RUPEE, RH_GREG_RUPEE, {RC_GREG_HINT}, RHT_GREG_HINT01, RHT_GREG_HINT02, gregText, gregHintLoc, (bool)ctx->GetOption(RSK_GREG_HINT));
  CreateSpecialItemHint(RG_PROGRESSIVE_MAGIC_METER, RH_SARIA, {RC_SARIA_SONG_HINT, RC_SONG_FROM_SARIA}, RHT_SARIA_TEXT01, RHT_SARIA_TEXT02, sariaText, sariaHintLoc, (bool)ctx->GetOption(RSK_SARIA_HINT));

  if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ON_HINT)) {
    CreateMerchantsHints();
  }
  if (ctx->GetOption(RSK_GOSSIP_STONE_HINTS).IsNot(RO_GOSSIP_STONES_NONE)) {
    printf("\x1b[10;10HCreating Hints...");
    CreateStoneHints();
    printf("Done");
  }
}

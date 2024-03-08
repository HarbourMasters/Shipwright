#include "hint.h"
#include "map"
#include "string"
#include "context.h"
#include <spdlog/spdlog.h>

namespace Rando {
Hint::Hint(){}
Hint::Hint(RandomizerHint ownKey_, 
           HintType hintType_,
           std::string distribution_,
           std::vector<RandomizerHintTextKey> hintKeys_,
           std::vector<RandomizerCheck> locations_,
           std::vector<uint8_t> colors_,
           std::vector<TrialKey> trials_)
    : ownKey(ownKey_), hintType(hintType_), hintKeys(hintKeys_), distribution(std::move(distribution_)), locations(locations_), colors(colors_), trials(trials_) {
        GetAreasAndItemsFromLocations();
        SetLocationsAsHinted();
}

Hint::Hint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerGet> items, std::vector<TextBoxType> boxTypes_,
           bool yourPocket_, std::string distributionName_)
    : ownKey(ownKey_), hintType(hintType_), hintKeys(hintKeys_), boxTypes(boxTypes_), yourPocket(yourPocket_), distribution(std::move(distributionName_)) {
        GetLocationsAndAreaFromItems(items);
        SetLocationsAsHinted();
}

StaticHint::StaticHint(){}

StaticHint::StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                       std::vector<uint8_t> colors_, bool yourPocket_, bool capital_){
  ownKey = ownKey_;
  hintType = hintType_;
  hintKeys = hintKeys_;
  locations = locations_;
  colors = colors_;
  yourPocket = yourPocket_;
  capital = capital_;
  GetAreasAndItemsFromLocations();
  SetLocationsAsHinted();
}

StaticHint::StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                       bool yourPocket_, bool capital_){
  ownKey = ownKey_;
  hintType = hintType_;
  hintKeys = hintKeys_;
  locations = locations_;
  yourPocket = yourPocket_;
  capital = capital_;
  GetAreasAndItemsFromLocations();
  SetLocationsAsHinted();
}

StaticHint::StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerCheck> locations_, 
                       uint8_t num_){
  ownKey = ownKey_;
  hintType = hintType_;
  hintKeys = hintKeys_;
  locations = locations_;
  num = num_;
  GetAreasAndItemsFromLocations();
  SetLocationsAsHinted();
}

StaticHint::StaticHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerHintTextKey> hintKeys_, std::vector<RandomizerArea> areas_){
  ownKey = ownKey_;
  hintType = hintType_;
  hintKeys = hintKeys_;
  areas = areas_;
  GetAreasAndItemsFromLocations();
  SetLocationsAsHinted();
}

AltarHint::AltarHint(RandomizerHint ownKey_, HintType hintType_, std::vector<RandomizerCheck> locations_, std::vector<uint8_t> colors_){
  ownKey = ownKey_;
  hintType = hintType_;
  locations = locations_;
  colors = colors_;
  capital = true;
  boxTypes = {TEXTBOX_TYPE_BLUE};
  GetAreasAndItemsFromLocations();
  SetLocationsAsHinted();
}

const CustomMessage& Hint::GetMessage(uint8_t id) const {
    auto ctx = Rando::Context::GetInstance();
    if (hintType == HINT_TYPE_TEXT){
        return messages[id];
    }

    CustomMessage hintText = CustomMessage("ERROR:NO HINTTEXTKEY FOUND");

    if (id < messages.size()){
      return messages[id];
    } 

    if (id < hintKeys.size()){
        hintText = ::GetHintText(hintKeys[id]).GetText();
    } else if (ctx->GetOption(RSK_TOT_ALTAR_HINT) && hintType == HINT_TYPE_ALTAR_CHILD) {
        hintText = ::GetHintText(RHT_CHILD_ALTAR_STONES).GetText();
    } else if (ctx->GetOption(RSK_TOT_ALTAR_HINT) && hintType == HINT_TYPE_ALTAR_ADULT) {
        hintText = ::GetHintText(RHT_ADULT_ALTAR_MEDALLIONS).GetText(); //RANDOTODO add more defaults
    } else if (hintType == HINT_TYPE_TRIAL) {
      if (ctx->GetTrial(trials[0])->IsRequired()) {
        hintText = ::GetHintText(RHT_TRIAL_ON).GetText();
      } else {
        hintText = ::GetHintText(RHT_TRIAL_OFF).GetText();
      }
    } else if (hintType == HINT_TYPE_WOTH) {
      hintText = ::GetHintText(RHT_WAY_OF_THE_HERO).GetText();
    } else if (hintType == HINT_TYPE_FOOLISH) {
      hintText = ::GetHintText(RHT_FOOLISH).GetText();
    } else if (hintType == HINT_TYPE_ITEM_LOCATION) {
      if (locations.size() > 0) {
        hintText = Rando::StaticData::GetLocation(locations[0])->GetHint()->GetText();
      } else {
        CustomMessage("ERROR: ITEM LOCATION HINT WITH NO LOCATION");
      }
    } else if (hintType == HINT_TYPE_ITEM_AREA) {
      if (locations.size() > 0) {
        if (Rando::StaticData::GetLocation(locations[0])->IsDungeon()) {
          hintText = ::GetHintText(RHT_HOARDS).GetText();
        } else {
          hintText = ::GetHintText(RHT_CAN_BE_FOUND_AT).GetText();
        }
      } else {
        CustomMessage("ERROR: ITEM AREA HINT WITH NO LOCATION"); //RANDOTODO get isDungeon from area?
      }
    }

    if (hintType == HINT_TYPE_MERCHANT || hintType == HINT_TYPE_ITEM_LOCATION || hintType == HINT_TYPE_STATIC_LOCATION){
      //if we write items
      bool mysterious = hintType == HINT_TYPE_MERCHANT && ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ON_NO_HINT);
      for(uint8_t b = 0; b < locations.size(); b++){
          hintText.Replace(std::move("{{" + std::to_string(b+1) + "}}"), GetItemName(locations[b], mysterious));
      }
    } else if (hintType == HINT_TYPE_TRIAL){
      //If we write trials
      for(uint8_t b = 0; b < trials.size(); b++){
        hintText.Replace(std::move("{{" + std::to_string(b+1) + "}}"), ctx->GetTrial(trials[b])->GetName());
      }
    } else if (hintType == HINT_TYPE_ITEM_AREA){
      //If we write items and areas
      for(uint8_t b = 0; b < items.size(); b++){
        hintText.Replace(std::move("{{" + std::to_string((b*2)+1) + "}}"), items[b].GetName());
        hintText.Replace(std::move("{{" + std::to_string((b*2)+2) + "}}"), ::GetHintText(areas[b]).GetText());
      }
    } else { 
      //If we write areas
      for(uint8_t b = 0; b < areas.size(); b++){
          CustomMessage areaText;
          if ((areas[b] == RA_LINKS_POCKET || areas[b] == RA_NONE) && yourPocket){
              areaText = ::GetHintText(RHT_YOUR_POCKET).GetText();
          } else {
              areaText = ::GetHintText(areas[b]).GetText();
          }
          hintText.Replace(std::move("{{" + std::to_string(b+1) + "}}"), areaText);
      }
    }

    if (num != 0){
        hintText.InsertNumber(num);
    }

    if (hintType == HINT_TYPE_ALTAR_CHILD){
        if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_OPEN)) {
            hintText += CustomMessage(::GetHintText(RHT_CHILD_ALTAR_TEXT_END_DOTOPEN).GetText());
        } else if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_SONGONLY)) {
            hintText += CustomMessage(::GetHintText(RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY).GetText());
        } else {
            hintText += CustomMessage(::GetHintText(RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED).GetText());
        }
    } else if (hintType == HINT_TYPE_ALTAR_ADULT){
      hintText += GetBridgeReqsText() + GetGanonBossKeyText() + ::GetHintText(RHT_ADULT_ALTAR_TEXT_END).GetText();
    }
    
    if (id < boxTypes.size()){
        hintText.SetTextBoxType(boxTypes[id]);
    }

    hintText.AutoFormat();
    return hintText;
}

void Hint::AddHintedLocation(RandomizerCheck location) {
    locations.push_back(location);
}

std::vector<RandomizerCheck> Hint::GetHintedLocations() {
    return locations;
}

void Hint::SetHintType(HintType type) {
    hintType = type;
}

HintType Hint::GetHintType() {
    return hintType;
}

void Hint::AddHintedArea(RandomizerArea area) {
    areas.push_back(area);
}

std::vector<RandomizerArea> Hint::GetHintedAreas() {
    return areas;
}

void Hint::SetDistribution(std::string distributionName) {
    distribution = distributionName;
}

const std::string& Hint::GetDistribution() {
    return distribution;
}

void Hint::ResetVariables() {
    locations = {};
    areas = {};
    addedToPool = false;
}

// RANDOTODO decide how to seed ambiguous hints
std::map<std::string, std::string> Hint::GetLog() {
    std::map<std::string, std::string> log = {};
    std::string textString = "";
    auto ctx = Rando::Context::GetInstance();
    for (int c = 0; c < messages.size(); c++){
        if (c > 0){
            textString += ",/n";
        }
        textString = textString + messages[c].GetEnglish();
    }
    log["hint"] = textString;
    if (distribution != ""){
        log["distribution"] = distribution;
    }
    log["type"] = hintTypeNames[hintType];
    if (locations.size() > 0 && hintType != HINT_TYPE_FOOLISH){
        std::string locString = "";
        for (int c = 0; c < locations.size(); c++){
            if (c > 0){
                locString += ", ";
            }
            locString += Rando::StaticData::GetLocation(locations[c])->GetName();
        }
        log["location"] = locString;
    }
    if (items.size() > 0 && hintType != HINT_TYPE_FOOLISH){
      std::string itemString = "";
      for (int c = 0; c < items.size(); c++){
          if (c > 0){
              itemString += ", ";
          }
          itemString += ctx->GetItemLocation(locations[c])->GetPlacedItemName().GetEnglish();
      }
      log["item"] = itemString;
    }
    if (areas.size() > 0){
        std::string areaString = "";
        for (int c = 0; c < areas.size(); c++){
            if (c > 0){
                areaString += ", ";
            }
            areaString += ::GetHintText(areas[c]).GetText().Capitalize().GetEnglish();
        }
        log["area"] = areaString;
    }
    return log;
}

// RANDOTODO add logging control
void Hint::logHint(json& jsonData){
    jsonData["Hint Stones"][hintKeyNames[ownKey]] = GetLog();
}

void StaticHint::logHint(json& jsonData){
    jsonData["Static Hints"][hintKeyNames[ownKey]] = GetLog();
}

const Text Hint::GetItemName(RandomizerCheck hintedCheck, bool mysterious) const { 
  auto ctx = Rando::Context::GetInstance();
  RandomizerGet targetRG = ctx->GetItemLocation(hintedCheck)->GetPlacedRandomizerGet();
  Text itemName;
  
  if (mysterious){
    itemName = ::GetHintText(RHT_MYSTERIOUS_ITEM).GetText();
  } else if (!ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS) && targetRG == RG_ICE_TRAP) { //RANDOTODO store in item hint instead of item
      targetRG = ctx->overrides[hintedCheck].LooksLike();
      itemName = {
          ctx->overrides[hintedCheck].GetTrickName().english,
          ctx->overrides[hintedCheck].GetTrickName().french,
          ctx->overrides[hintedCheck].GetTrickName().english
      };
  } else {
      itemName = ctx->GetItemLocation(hintedCheck)->GetPlacedItem().GetHint().GetText();
  }
  if (capital){
      itemName = itemName.Capitalize();
  }
  return itemName;
}

void Hint::GetAreasAndItemsFromLocations(){
    auto ctx = Rando::Context::GetInstance();
    for(uint8_t b = 0; b < locations.size(); b++){
      areas[b] = ctx->GetItemLocation(locations[b])->GetArea();
      items[b] = ctx->GetItemLocation(locations[b])->GetPlacedItem();
    }
}

void Hint::GetLocationsAndAreaFromItems(std::vector<RandomizerGet> items){
    auto ctx = Rando::Context::GetInstance();
    for (uint8_t c = 0; c < items.size(); c++){
        Text areaText;
        std::vector<RandomizerCheck> found = FilterFromPool(ctx->allLocations, [items, ctx, c](const RandomizerCheck loc) {
        return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == items[c];});
        if (found.size() > 0){
            locations.push_back(found[0]);
            areas.push_back(ctx->GetItemLocation(found[0])->GetArea());
        } else {
            locations.push_back(RC_UNKNOWN_CHECK);
            areas.push_back(RA_NONE);
        }
    }
}

void Hint::SetLocationsAsHinted(){
    auto ctx = Rando::Context::GetInstance();
    for (uint8_t count = 0; count < locations.size(); count++){
        ctx->GetItemLocation(locations[count])->AddHintedBy(ownKey);
    }
}

static CustomMessage GetBridgeReqsText() {
  auto ctx = Rando::Context::GetInstance();
  CustomMessage bridgeMessage;

  if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_ALWAYS_OPEN)) {
    return ::GetHintText(RHT_BRIDGE_OPEN_HINT).GetText();

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_VANILLA)) {
    return  ::GetHintText(RHT_BRIDGE_VANILLA_HINT).GetText();

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)) {
    bridgeMessage = ::GetHintText(RHT_BRIDGE_VANILLA_HINT).GetText();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS)) {
    bridgeMessage = ::GetHintText(RHT_BRIDGE_MEDALLIONS_HINT).GetText();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
    bridgeMessage = ::GetHintText(RHT_BRIDGE_REWARDS_HINT).GetText();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)) {
    bridgeMessage = ::GetHintText(RHT_BRIDGE_DUNGEONS_HINT).GetText();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)) {
    bridgeMessage = ::GetHintText(RHT_BRIDGE_TOKENS_HINT).GetText();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)) {
    return  ::GetHintText(RHT_BRIDGE_GREG_HINT).GetText();
  }

  return bridgeMessage;
}

static CustomMessage GetGanonBossKeyText() {
  auto ctx = Rando::Context::GetInstance();
  CustomMessage ganonBossKeyMessage;

  if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_STARTWITH)) {
    return ::GetHintText(RHT_GANON_BK_START_WITH_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_VANILLA)) {
    return ::GetHintText(RHT_GANON_BK_VANILLA_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OWN_DUNGEON)) {
    return ::GetHintText(RHT_GANON_BK_OWN_DUNGEON_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANY_DUNGEON)) {
    return ::GetHintText(RHT_GANON_BK_ANY_DUNGEON_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OVERWORLD)) {
    return ::GetHintText(RHT_GANON_BK_OVERWORLD_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANYWHERE)) {
    return ::GetHintText(RHT_GANON_BK_ANYWHERE_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
    return ::GetHintText(RHT_GANON_BK_SKULLTULA_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_VANILLA)) {
    return ::GetHintText(RHT_LACS_VANILLA_HINT).GetText();

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
    ganonBossKeyMessage = ::GetHintText(RHT_LACS_STONES_HINT).GetText();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_STONE_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_MEDALLIONS)) {
    ganonBossKeyMessage = ::GetHintText(RHT_LACS_MEDALLIONS_HINT).GetText();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_MEDALLION_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
    ganonBossKeyMessage = ::GetHintText(RHT_LACS_REWARDS_HINT).GetText();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
    ganonBossKeyMessage = ::GetHintText(RHT_LACS_DUNGEONS_HINT).GetText();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
    ganonBossKeyMessage = ::GetHintText(RHT_LACS_TOKENS_HINT).GetText();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>());

  } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_TRIFORCE_HUNT)) {
    return ::GetHintText(RHT_GANON_BK_TRIFORCE_HINT).GetText();
  }

  return ganonBossKeyMessage;
}

}
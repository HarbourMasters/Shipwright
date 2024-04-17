#include "hint.h"
#include "map"
#include "string"
#include "context.h"
#include <spdlog/spdlog.h>
#include "static_data.h"

namespace Rando {
Hint::Hint(){}

Hint::Hint(RandomizerHint ownKey_, 
           HintType hintType_,
           std::string distribution_,
           std::vector<RandomizerHintTextKey> hintKeys_,
           std::vector<RandomizerCheck> locations_,
           std::vector<RandomizerArea> areas_,
           std::vector<TrialKey> trials_)
    : ownKey(ownKey_), hintType(hintType_), distribution(std::move(distribution_)), hintKeys(hintKeys_), locations(locations_), areas(areas_), trials(trials_) {
  ExtrapolateDataFromLocations();
  SetLocationsAsHinted();
  enabled = true;
}

Hint::Hint(RandomizerHint ownKey_, 
           HintType hintType_,
           std::vector<RandomizerHintTextKey> hintKeys_,
           std::vector<RandomizerCheck> locations_,
           std::vector<RandomizerArea> areas_,
           std::vector<TrialKey> trials_,
           bool yourPocket_,
           int num_)
    : ownKey(ownKey_), hintType(hintType_), hintKeys(hintKeys_), locations(locations_), areas(areas_), trials(trials_), yourPocket(yourPocket_), num(num_) {
  ExtrapolateDataFromLocations();
  SetLocationsAsHinted();
  enabled = true;
}

Hint::Hint(RandomizerHint ownKey_, std::vector<CustomMessage> messages_) 
: ownKey(ownKey_), messages(messages_){
  hintType = HINT_TYPE_MESSAGE;
  enabled = true;
}

Hint::Hint(RandomizerHint ownKey_, nlohmann::json json_){
  ownKey = ownKey_;
  HintType type = (HintType)Rando::StaticData::hintTypeNameToEnum[json_["type"].get<std::string>()];

  if (json_.contains("enabled") && !json_["enabled"].get<bool>()){
    return;
  }
  if (json_.contains("distribution")){
      distribution = json_["distribution"].get<std::string>();
  }            
  if (json_.contains("locations")){
      for (auto loc: json_["locations"]){
          locations.push_back((RandomizerCheck)StaticData::locationNameToEnum[loc.get<std::string>()]);
      }
  }
  if (json_.contains("areas")){
      for (auto area: json_["areas"]){
          areas.push_back((RandomizerArea)Rando::StaticData::areaNameToEnum[area]);
      }
  }
  if (json_.contains("trials")){
      for (auto trial: json_["trials"]){
          trials.push_back((TrialKey)Rando::StaticData::trialNameToEnum[trial]);
      }
  }
  if (json_.contains("hintKeys")){
      for (auto hintKey: json_["hintKeys"]){
          hintKeys.push_back((RandomizerHintTextKey)hintKey.get<uint32_t>());
      }
  }
  if (json_.contains("chosenName")){
    for (auto name: json_["chosenName"]){
        chosenName.push_back(name.get<uint32_t>());
    }
  }
  ExtrapolateDataFromLocations();
  SetLocationsAsHinted();
  enabled = true;
}

const std::vector<std::string> Hint::GetAllMessageStrings() const {
  std::vector<std::string> hintMessages = {};
  uint8_t numMessages = std::max(messages.size(), hintKeys.size());
  if (numMessages == 0){
    numMessages = 1; //RANDOTODO make std::max actually fucking work for 3 arguments
  }
  for (int c = 0; c < numMessages; c++){
    hintMessages.push_back(GetMessage(c).GetForCurrentLanguage());
  }
  return hintMessages;
}

const CustomMessage Hint::GetMessage(uint8_t id) const {
    auto ctx = Rando::Context::GetInstance();
    if (hintType == HINT_TYPE_HINT_KEY){
        return StaticData::hintTextTable[hintKeys[id]].GetMessage();
    }

    CustomMessage hintText = CustomMessage("ERROR:NO HINTTEXTKEY FOUND");

    if (hintType == HINT_TYPE_MESSAGE){
      if (id < messages.size()){
        return messages[id];
      }
    }

    if (id < hintKeys.size()){
        hintText = StaticData::hintTextTable[hintKeys[id]].GetMessage();
    } else if (ctx->GetOption(RSK_TOT_ALTAR_HINT) && hintType == HINT_TYPE_ALTAR_CHILD) {
        hintText = StaticData::hintTextTable[RHT_CHILD_ALTAR_STONES].GetMessage();
    } else if (ctx->GetOption(RSK_TOT_ALTAR_HINT) && hintType == HINT_TYPE_ALTAR_ADULT) {
        hintText = StaticData::hintTextTable[RHT_ADULT_ALTAR_MEDALLIONS].GetMessage();
    } else if (hintType == HINT_TYPE_TRIAL) {
      if (ctx->GetTrial(trials[0])->IsRequired()) {
        hintText = StaticData::hintTextTable[RHT_TRIAL_ON].GetMessage();
      } else {
        hintText = StaticData::hintTextTable[RHT_TRIAL_OFF].GetMessage();
      }
    } else if (hintType == HINT_TYPE_WOTH) {
      hintText = StaticData::hintTextTable[RHT_WAY_OF_THE_HERO].GetMessage();
    } else if (hintType == HINT_TYPE_FOOLISH) {
      hintText = StaticData::hintTextTable[RHT_FOOLISH].GetMessage();
    } else if (hintType == HINT_TYPE_ITEM) {
      if (items.size() > 0) {
        hintText = StaticData::GetItemTable()[items[0]].GetName();
      } else {
        hintText = CustomMessage("ERROR: ITEM HINT WITH NO ITEMS");
      }
    } else if (hintType == HINT_TYPE_ITEM_AREA) {
      if (locations.size() > 0) {
        if (Rando::StaticData::GetLocation(locations[0])->IsDungeon()) {
          hintText = StaticData::hintTextTable[RHT_HOARDS].GetMessage();
        } else {
          hintText = StaticData::hintTextTable[RHT_CAN_BE_FOUND_AT].GetMessage();
        }
      } else {
         hintText = CustomMessage("ERROR: ITEM AREA HINT WITH NO LOCATION"); //RANDOTODO get isDungeon from area?
      }
    }

    std::vector<CustomMessage> toInsert = {};

    switch (hintType){
      case HINT_TYPE_ITEM:{
        //if we write items
        for(uint8_t b = 0; b < locations.size(); b++){
          toInsert.push_back(StaticData::GetItemTable()[items[b]].GetName());
        }
        break;}
      case HINT_TYPE_MERCHANT:{
          bool mysterious = hintType == HINT_TYPE_MERCHANT && ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ON_NO_HINT);
          for(uint8_t b = 0; b < locations.size(); b++){
            toInsert.push_back(GetItemName(b, mysterious));
          }
          break;}
      case HINT_TYPE_TRIAL:{
        //If we write trials
        for(uint8_t b = 0; b < trials.size(); b++){
          toInsert.push_back(ctx->GetTrial(trials[b])->GetName());
        }
        break;}
      case HINT_TYPE_ITEM_AREA:{
        //If we write items and areas
        for(uint8_t b = 0; b < items.size(); b++){
          toInsert.push_back(StaticData::GetItemTable()[items[b]].GetName());
          toInsert.push_back(StaticData::hintTextTable[Rando::StaticData::areaNames[areas[b]]].GetMessage());
        }
        break;}
      case HINT_TYPE_ALTAR_CHILD:
      case HINT_TYPE_ALTAR_ADULT:
      case HINT_TYPE_AREA:
      case HINT_TYPE_WOTH:
      case HINT_TYPE_FOOLISH:{
        //If we write areas
        for(uint8_t b = 0; b < areas.size(); b++){
          CustomMessage areaText;
          if ((areas[b] == RA_LINKS_POCKET || areas[b] == RA_NONE) && yourPocket){
            areaText = StaticData::hintTextTable[RHT_YOUR_POCKET].GetMessage();
          } else {
            areaText = StaticData::hintTextTable[Rando::StaticData::areaNames[areas[b]]].GetMessage();
          }
          toInsert.push_back(areaText);
        }
        break;}
      case HINT_TYPE_HINT_KEY:
      case HINT_TYPE_MESSAGE:
      case HINT_TYPE_ENTRANCE:
      case HINT_TYPE_MAX:
        break;
    }

    hintText.InsertNames(toInsert);

    if (hintType == HINT_TYPE_ALTAR_CHILD){
        if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_OPEN)) {
            hintText += CustomMessage(StaticData::hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTOPEN].GetMessage());
        } else if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_SONGONLY)) {
            hintText += CustomMessage(StaticData::hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY].GetMessage());
        } else {
            hintText += CustomMessage(StaticData::hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED].GetMessage());
        }
    } else if (hintType == HINT_TYPE_ALTAR_ADULT){
      hintText += GetBridgeReqsText() + GetGanonBossKeyText() + StaticData::hintTextTable[RHT_ADULT_ALTAR_TEXT_END].GetMessage();
    }

    if (num != 0){
      hintText.InsertNumber(num);
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
  ownKey = RH_NONE;
  num = 0;
  yourPocket = false;
  messages = {};
  hintKeys = {};
  locations = {};
  items = {};
  trials = {};
  hintType = HINT_TYPE_HINT_KEY;
  areas = {};
  distribution = "";
  enabled = false;
  chosenName = {};
}

oJson Hint::toJSON() {
  auto ctx = Rando::Context::GetInstance();
  nlohmann::ordered_json log = {};
  if (enabled){
    std::vector<std::string> hintMessages = GetAllMessageStrings();
    if (hintMessages.size() > 0){
      log["messages"] = hintMessages;
    }
    if (distribution != ""){
      log["distribution"] = distribution;
    }
    log["type"] = Rando::StaticData::hintTypeNames[hintType].GetForCurrentLanguage();
    if (hintKeys.size() > 0){
      std::vector<std::string> hintKeyStrings = {};
      for (uint c = 0; c < hintKeys.size(); c++){
        hintKeyStrings[c] = std::to_string(hintKeys[c]);
      }
      log["hintKeys"] = hintKeyStrings;
    }
    if (hintType != HINT_TYPE_FOOLISH){
      if (locations.size() > 0){
        std::vector<std::string> locStrings = {};
        for (uint c = 0; c < locations.size(); c++){
          locStrings[c] = Rando::StaticData::GetLocation(locations[c])->GetName();//RANDOTODO change to CustomMessage when VB is done
        }
        log["locations"] = locStrings;
      }
      if (items.size() > 0){
        std::vector<std::string> itemStrings = {};
        for (uint c = 0; c < items.size(); c++){
          itemStrings[c] = StaticData::GetItemTable()[items[c]].GetName().GetEnglish();//RANDOTODO change to CustomMessage
        }
        log["items"] = itemStrings;
      }
      if (chosenName.size() > 0){
        std::vector<std::string> nameStrings = {};
        for (uint c = 0; c < chosenName.size(); c++){
          nameStrings[c] = std::to_string(chosenName[c]);
        }
        log["chosenName"] = nameStrings;
      }
    }
    if (areas.size() > 0){
      std::vector<std::string> areaStrings = {};
      for (uint c = 0; c < areas.size(); c++){
        areaStrings[c] = StaticData::hintTextTable[StaticData::areaNames[areas[c]]].GetMessage().GetForCurrentLanguage();
      }
      log["areas"] = areaStrings;
    }
    if (trials.size() > 0){
      std::vector<std::string> trialStrings = {};
      for (uint c = 0; c < trials.size(); c++){
        trialStrings[c] = ctx->GetTrial(trials[c])->GetName().GetForCurrentLanguage();
      }
      log["trials"] = trialStrings;
    }
  }
  return log;
}

// RANDOTODO add logging control
void Hint::logHint(oJson& jsonData){
  std::string logMap = "Static Hints";
  if (ownKey < RH_GANONDORF_HINT){
    logMap = "Gossip Stone Hints";
  }
  if (enabled){
    jsonData[logMap][Rando::StaticData::hintNames[ownKey].GetForCurrentLanguage()] = toJSON();
  }
}

const CustomMessage Hint::GetItemName(uint8_t slot, bool mysterious) const { 
  auto ctx = Rando::Context::GetInstance();
  RandomizerCheck hintedCheck = locations[slot];
  RandomizerGet targetRG = ctx->GetItemLocation(hintedCheck)->GetPlacedRandomizerGet();
  CustomMessage itemName;
  
  if (mysterious){
    itemName = StaticData::hintTextTable[RHT_MYSTERIOUS_ITEM].GetMessage();
  } else if (!ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS) && targetRG == RG_ICE_TRAP) { //RANDOTODO store in item hint instead of item
      targetRG = ctx->overrides[hintedCheck].LooksLike();
      itemName = {
          ctx->overrides[hintedCheck].GetTrickName().english,
          ctx->overrides[hintedCheck].GetTrickName().french,
          ctx->overrides[hintedCheck].GetTrickName().english
      };
  } else {
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)){
      itemName = ctx->GetItemLocation(hintedCheck)->GetPlacedItem().GetHint().GetAmbiguous(chosenName[slot]);
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_OBSCURE)){
      itemName = ctx->GetItemLocation(hintedCheck)->GetPlacedItem().GetHint().GetObscure(chosenName[slot]);
    } else {
      itemName = ctx->GetItemLocation(hintedCheck)->GetPlacedItem().GetHint().GetClear();
    }
  }
  return itemName;
}

void Hint::ExtrapolateDataFromLocations(){
  auto ctx = Rando::Context::GetInstance();
  bool doArea = true;
  if (areas.size() > 0){
    doArea = false;
  }
  for(uint8_t c = 0; c < locations.size(); c++){
    if (doArea){
      areas[c] = ctx->GetItemLocation(locations[c])->GetArea();
    }
    items[c] = ctx->GetItemLocation(locations[c])->GetPlacedRandomizerGet();
    if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)){
      chosenName[c] = Random(0, StaticData::GetItemTable()[items[c]].GetHint().GetAmbiguousSize()-1);
    } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_OBSCURE)){
      chosenName[c] = Random(0, StaticData::GetItemTable()[items[c]].GetHint().GetObscureSize()-1);
    }
  }
}

void Hint::SetLocationsAsHinted(){
  auto ctx = Rando::Context::GetInstance();
  for (uint8_t count = 0; count < locations.size(); count++){
      ctx->GetItemLocation(locations[count])->AddHintedBy(ownKey);
  }
}

bool Hint::IsEnabled(){
  return enabled;
}

std::vector<RandomizerHintTextKey> Hint::GetHintKeys(){
  return hintKeys;
}

std::vector<RandomizerGet> Hint::GetHintedItems(){
  return items;
}

std::vector<uint8_t> Hint::GetChosenNames(){
  return chosenName;
}

std::vector<TrialKey> Hint::GetHintedTrials(){
  return trials;
}

CustomMessage Hint::GetBridgeReqsText() {
  auto ctx = Rando::Context::GetInstance();
  CustomMessage bridgeMessage;

  if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_ALWAYS_OPEN)) {
    return StaticData::hintTextTable[RHT_BRIDGE_OPEN_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_VANILLA)) {
    return StaticData::hintTextTable[RHT_BRIDGE_VANILLA_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_VANILLA_HINT].GetMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_MEDALLIONS_HINT].GetMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_REWARDS_HINT].GetMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_DUNGEONS_HINT].GetMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_TOKENS_HINT].GetMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)) {
    return  StaticData::hintTextTable[RHT_BRIDGE_GREG_HINT].GetMessage();
  }
  return bridgeMessage;
}

CustomMessage Hint::GetGanonBossKeyText() {
  auto ctx = Rando::Context::GetInstance();
  CustomMessage ganonBossKeyMessage;

  if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_STARTWITH)) {
    return StaticData::hintTextTable[RHT_GANON_BK_START_WITH_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_VANILLA)) {
    return StaticData::hintTextTable[RHT_GANON_BK_VANILLA_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OWN_DUNGEON)) {
    return StaticData::hintTextTable[RHT_GANON_BK_OWN_DUNGEON_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANY_DUNGEON)) {
    return StaticData::hintTextTable[RHT_GANON_BK_ANY_DUNGEON_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OVERWORLD)) {
    return StaticData::hintTextTable[RHT_GANON_BK_OVERWORLD_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANYWHERE)) {
    return StaticData::hintTextTable[RHT_GANON_BK_ANYWHERE_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
    return StaticData::hintTextTable[RHT_GANON_BK_SKULLTULA_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_VANILLA)) {
    return StaticData::hintTextTable[RHT_LACS_VANILLA_HINT].GetMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_STONES_HINT].GetMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_STONE_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_MEDALLIONS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_MEDALLIONS_HINT].GetMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_MEDALLION_COUNT).Value<uint8_t>());
  }
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_REWARDS_HINT].GetMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_DUNGEONS_HINT].GetMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_TOKENS_HINT].GetMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>());
  }
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_TRIFORCE_HUNT)) {
    return StaticData::hintTextTable[RHT_GANON_BK_TRIFORCE_HINT].GetMessage();
  }
  return ganonBossKeyMessage;
}

}
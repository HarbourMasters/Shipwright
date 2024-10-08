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
  FillGapsInData();
  SetLocationsAsHinted();
  NamesChosen();
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
  FillGapsInData();
  SetLocationsAsHinted();
  NamesChosen();
  enabled = true;
}

Hint::Hint(RandomizerHint ownKey_, std::vector<CustomMessage> messages_) 
: ownKey(ownKey_), messages(messages_){
  hintType = HINT_TYPE_MESSAGE;
  enabled = true;
}

Hint::Hint(RandomizerHint ownKey_, nlohmann::json json_){
  ownKey = ownKey_;
  if (json_.contains("enabled") && !json_["enabled"].get<bool>()){
    return;
  }
  enabled = true;

  if (json_.contains("type")){
    hintType = (HintType)StaticData::hintTypeNameToEnum[json_["type"].get<std::string>()];
  }

  if (hintType == HINT_TYPE_MESSAGE){
    if (json_.contains("messages")){
      for (auto message: json_["messages"]){
        messages.push_back(CustomMessage(message.get<std::string>()));
      }
    } else if (json_.contains("message")){
      messages.push_back(CustomMessage(json_["message"].get<std::string>()));
    }
  }
  
  if (json_.contains("distribution")){
    distribution = json_["distribution"].get<std::string>();
  }

  if (json_.contains("locations")){
    for (auto loc: json_["locations"]){
      locations.push_back(StaticData::locationNameToEnum[loc.get<std::string>()]);
    }
  } else if (json_.contains("location")){
    locations.push_back(StaticData::locationNameToEnum[json_["location"].get<std::string>()]);
  }

  if (json_.contains("itemNamesChosen")){
    for (auto name: json_["itemNamesChosen"]){
      itemNamesChosen.push_back(name.get<uint8_t>());
    }
  } else if (json_.contains("itemNameChosen")){
    itemNamesChosen.push_back(json_["itemNameChosen"].get<uint8_t>());
  }

  if (json_.contains("areas")){
    for (auto area: json_["areas"]){
      areas.push_back((RandomizerArea)Rando::StaticData::areaNameToEnum[area]);
    }
  } else if (json_.contains("area")){
    areas.push_back((RandomizerArea)Rando::StaticData::areaNameToEnum[json_["area"]]);
  }

  if (json_.contains("areaNamesChosen")){
    for (auto name: json_["areaNamesChosen"]){
      areaNamesChosen.push_back(name.get<uint8_t>());
    }
  } else if (json_.contains("areaNameChosen")){
    areaNamesChosen.push_back(json_["areaNameChosen"].get<uint8_t>());
  }

  if (json_.contains("trials")){
    for (auto trial: json_["trials"]){
      trials.push_back((TrialKey)Rando::StaticData::trialNameToEnum[trial]);
    }
  } else if (json_.contains("trial")){
    trials.push_back((TrialKey)Rando::StaticData::trialNameToEnum[json_["trial"]]);
  }

  if (json_.contains("hintKeys")){
    for (auto hintKey: json_["hintKeys"]){
      hintKeys.push_back((RandomizerHintTextKey)hintKey.get<uint32_t>());
    }
  } else if (json_.contains("hintKey")){
    hintKeys.push_back((RandomizerHintTextKey)json_["hintKey"].get<uint32_t>());
  }

  if (json_.contains("hintTextsChosen")){
    for (auto name: json_["hintTextsChosen"]){
      hintTextsChosen.push_back(name.get<uint8_t>());
    }
  } else if (json_.contains("hintTextChosen")){
    hintTextsChosen.push_back(json_["hintTextChosen"].get<uint8_t>());
  }

  if (json_.contains("num")){
    num = json_["num"].get<int>();
  }

  FillGapsInData();
  SetLocationsAsHinted();
}

void Hint::FillGapsInData(){
  auto ctx = Rando::Context::GetInstance();
  if (locations.size() == 0 && StaticData::staticHintInfoMap.contains(ownKey)){
    locations = StaticData::staticHintInfoMap[ownKey].targetChecks;
  }
  bool fillAreas = true;
  bool fillItems = true;
  if (areas.size() > 0){
    fillAreas = false;
  }
  if (items.size() > 0){
    fillItems = false;
  }
  for(uint8_t c = 0; c < locations.size(); c++){
    //if area matters for the hint, it should be specified and not left to this
    if (fillAreas){
      areas.push_back(*ctx->GetItemLocation(locations[c])->GetAreas().begin());
    }
    if (fillItems){
      items.push_back(ctx->GetItemLocation(locations[c])->GetPlacedRandomizerGet());
    }
  }
}

void Hint::SetLocationsAsHinted() const {
  auto ctx = Rando::Context::GetInstance();
  for (uint8_t count = 0; count < locations.size(); count++){
      ctx->GetItemLocation(locations[count])->AddHintedBy(ownKey);
  }
}

uint8_t GetRandomHintTextEntry(const HintText hintText){
  auto ctx = Rando::Context::GetInstance();
  uint8_t size = 0;
  if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS)){
    size = hintText.GetAmbiguousSize();
  } else if (ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_OBSCURE)){
    size = hintText.GetObscureSize();
  }
  if (size > 0){
    return Random(0, size);
  }
  return 0;
}

void Hint::NamesChosen(){
  auto ctx = Rando::Context::GetInstance();
  std::vector<uint8_t> namesTemp = {};
  bool saveNames = false;
  uint8_t numMessages = GetNumberOfMessages();
  for (uint8_t c = 0; c < numMessages; c++){
    uint8_t selection = GetRandomHintTextEntry(GetHintText(c));
    if (selection > 0){
      saveNames = true;
    }
    namesTemp.push_back(selection);
  }
  if (saveNames) {
    hintTextsChosen = namesTemp;
  }

  if (hintType == HINT_TYPE_ITEM || hintType == HINT_TYPE_ITEM_AREA){
    for(uint8_t c = 0; c < locations.size(); c++){
      namesTemp = {};
      saveNames = false;
      uint8_t selection = GetRandomHintTextEntry(GetItemHintText(c));
      if (selection > 0){
        saveNames = true;
      }
      namesTemp.push_back(selection);
    }
    if (saveNames) {
      itemNamesChosen = namesTemp;
    }
  }

  if (hintType == HINT_TYPE_FOOLISH || hintType == HINT_TYPE_ITEM_AREA || hintType == HINT_TYPE_WOTH ||
      hintType == HINT_TYPE_ALTAR_CHILD || hintType == HINT_TYPE_ALTAR_ADULT){
    namesTemp = {};
    saveNames = false;
    for(uint8_t c = 0; c < areas.size(); c++){
      uint8_t selection = GetRandomHintTextEntry(GetAreaHintText(c));
      if (selection > 0){
        saveNames = true;
      }
      namesTemp.push_back(selection);
    }
    if (saveNames) {
      areaNamesChosen = namesTemp;
    }
  }
}

uint8_t Hint::GetNumberOfMessages() const {
  size_t numMessages = std::max(messages.size(), hintKeys.size());
  if (StaticData::staticHintInfoMap.contains(ownKey)){
    numMessages = std::max(StaticData::staticHintInfoMap[ownKey].hintKeys.size(), numMessages);
  }
  if (numMessages == 0){
      numMessages = 1; //RANDOTODO make std::max actually fucking work for 3 arguments
  }
  return numMessages;
}

const std::vector<std::string> Hint::GetAllMessageStrings(MessageFormat format) const {
  std::vector<std::string> hintMessages = {};
  uint8_t numMessages = GetNumberOfMessages();
  for (int c = 0; c < numMessages; c++){
    hintMessages.push_back(GetHintMessage(format, c).GetForCurrentLanguage(format));
  }
  return hintMessages;
}

const HintText Hint::GetHintText(uint8_t id) const {
  auto ctx = Rando::Context::GetInstance();
  if (hintKeys.size() > id){
    return StaticData::hintTextTable[hintKeys[id]];
  }
// If a static hint, load default from staticHintInfoMap
  if (StaticData::staticHintInfoMap.contains(ownKey) && StaticData::staticHintInfoMap[ownKey].hintKeys.size() > id){
    return StaticData::hintTextTable[StaticData::staticHintInfoMap[ownKey].hintKeys[id]];
  }

  switch (hintType){
    case HINT_TYPE_HINT_KEY:
      return StaticData::hintTextTable[0]; 
      break;
    case HINT_TYPE_TRIAL:
      if (ctx->GetTrial(trials[0])->IsRequired()) {
        return StaticData::hintTextTable[RHT_TRIAL_ON];
      } else {
        return StaticData::hintTextTable[RHT_TRIAL_OFF];
      }
    case HINT_TYPE_WOTH:
      return StaticData::hintTextTable[RHT_WAY_OF_THE_HERO];
    case HINT_TYPE_FOOLISH:
      return StaticData::hintTextTable[RHT_FOOLISH];
    case HINT_TYPE_ITEM:
      if (locations.size() > 0) {
        return *StaticData::GetLocation(locations[0])->GetHint();
      } else {
        return CustomMessage("ERROR: ITEM HINT WITH NO LOCATIONS OR HINT KEY");
      }
    case HINT_TYPE_ITEM_AREA:
      if (locations.size() > 0) {
        if (StaticData::GetLocation(locations[0])->IsDungeon()) {
          return StaticData::hintTextTable[RHT_HOARDS];
        } else {
          return StaticData::hintTextTable[RHT_CAN_BE_FOUND_AT];
        }
      } else {
        return CustomMessage("ERROR: ITEM AREA HINT WITH NO LOCATION"); //RANDOTODO get isDungeon from area?
      }
    default:
      return CustomMessage("ERROR: NO HINTKEY PROVIDED AND HINT TYPE HAS NO DEFAULT");
  }
}

const CustomMessage Hint::GetHintMessage(MessageFormat format, uint8_t id) const {
  auto ctx = Rando::Context::GetInstance();
  CustomMessage hintText = CustomMessage("");

  uint8_t chosenMessage = 0;
  if (hintTextsChosen.size() > id){
    chosenMessage = id;
  }

  if (hintType == HINT_TYPE_MESSAGE){
    if (id < messages.size()){
      hintText = messages[id];
    }
  } else if (hintType == HINT_TYPE_ALTAR_CHILD){
    if (ctx->GetOption(RSK_TOT_ALTAR_HINT)){
      hintText = StaticData::hintTextTable[RHT_CHILD_ALTAR_STONES].GetHintMessage();
    }
    if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_OPEN)) {
        hintText += CustomMessage(StaticData::hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTOPEN].GetHintMessage());
    } else if (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_SONGONLY)) {
        hintText += CustomMessage(StaticData::hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY].GetHintMessage());
    } else {
        hintText += CustomMessage(StaticData::hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED].GetHintMessage());
    }
  } else if (hintType == HINT_TYPE_ALTAR_ADULT){
    if (ctx->GetOption(RSK_TOT_ALTAR_HINT)){
      hintText = StaticData::hintTextTable[RHT_ADULT_ALTAR_MEDALLIONS].GetHintMessage();
    }
    hintText += GetBridgeReqsText() + GetGanonBossKeyText() + StaticData::hintTextTable[RHT_ADULT_ALTAR_TEXT_END].GetHintMessage();
  } else {
    hintText = GetHintText(id).GetHintMessage(chosenMessage);
  }

  std::vector<CustomMessage> toInsert = {};

  switch (hintType){
    case HINT_TYPE_ITEM:{
      //if we write items
      for(uint8_t b = 0; b < locations.size(); b++){
        toInsert.push_back(GetItemName(b)); 
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
        toInsert.push_back(GetItemName(b));
        toInsert.push_back(GetAreaName(b));
      }
      break;}
    case HINT_TYPE_ALTAR_CHILD:
    case HINT_TYPE_ALTAR_ADULT:
    case HINT_TYPE_AREA:
    case HINT_TYPE_WOTH:
    case HINT_TYPE_FOOLISH:{
      //If we write areas
      for(uint8_t b = 0; b < areas.size(); b++){
        toInsert.push_back(GetAreaName(b));
      }
      break;}
    default:
      break;
  }

  hintText.InsertNames(toInsert);

  if (num != 0){
    hintText.InsertNumber(num);
  }

  if (format == MF_FORMATTED){
    hintText.Format();
  } else if (format == MF_AUTO_FORMAT){
    hintText.AutoFormat();
  } else if (format == MF_CLEAN){
    hintText.Clean();
  }

  return hintText;
}

oJson Hint::toJSON() {
  auto ctx = Rando::Context::GetInstance();
  nlohmann::ordered_json log = {};
  if (enabled){
    log["type"] = StaticData::hintTypeNames[hintType].GetForCurrentLanguage(MF_CLEAN);
    
    std::vector<std::string> hintMessages = GetAllMessageStrings(MF_CLEAN);
    if (hintMessages.size() == 1){
      log["message"] = hintMessages[0];
    } else if (hintMessages.size() > 1){
      log["messages"] = hintMessages;
    }

    if (distribution != ""){
      log["distribution"] = distribution;
    }
    
    if (hintType != HINT_TYPE_FOOLISH){
      if (!(StaticData::staticHintInfoMap.contains(ownKey) && 
          StaticData::staticHintInfoMap[ownKey].targetChecks.size() > 0)){
        if (locations.size() == 1){
          log["location"] = StaticData::GetLocation(locations[0])->GetName();//RANDOTODO change to CustomMessage when VB is done;
        } else if (locations.size() > 1){
          //If we have defaults, no need to write more
          std::vector<std::string> locStrings = {};
          for (size_t c = 0; c < locations.size(); c++){
            locStrings.push_back(StaticData::GetLocation(locations[c])->GetName());//RANDOTODO change to CustomMessage when VB is done
          }
          log["locations"] = locStrings;
        }
      }
      
      if (!(StaticData::staticHintInfoMap.contains(ownKey) &&
          StaticData::staticHintInfoMap[ownKey].targetItems.size() > 0)){
        if (items.size() == 1){
          log["item"] = StaticData::GetItemTable()[items[0]].GetName().GetEnglish();//RANDOTODO change to CustomMessage;
        } else if (items.size() > 1){
          std::vector<std::string> itemStrings = {};
          for (size_t c = 0; c < items.size(); c++){
            itemStrings.push_back(StaticData::GetItemTable()[items[c]].GetName().GetEnglish());//RANDOTODO change to CustomMessage
          }
          log["items"] = itemStrings;
        }
      }

      if (itemNamesChosen.size() == 1){
        log["itemNameChosen"] = itemNamesChosen[0];
      } else if (itemNamesChosen.size() > 1){
        std::vector<uint8_t> nameNums = {};
        for (size_t c = 0; c < itemNamesChosen.size(); c++){
          nameNums.push_back(itemNamesChosen[c]);
        }
        log["itemNamesChosen"] = nameNums;
      }
    }
    if (areas.size() == 1){
      log["area"] = StaticData::hintTextTable[StaticData::areaNames[areas[0]]].GetClear().GetForCurrentLanguage(MF_CLEAN);
    } else if (areas.size() > 0 && 
          !(StaticData::staticHintInfoMap.contains(ownKey) && StaticData::staticHintInfoMap[ownKey].targetChecks.size() > 0)){
      // If we got locations from defaults, areas are derived from them and don't need logging
      std::vector<std::string> areaStrings = {};
      for (size_t c = 0; c < areas.size(); c++){
        areaStrings.push_back(StaticData::hintTextTable[StaticData::areaNames[areas[c]]].GetClear().GetForCurrentLanguage(MF_CLEAN));
      }
      log["areas"] = areaStrings;
    }

    if (areaNamesChosen.size() == 1){
      log["areaNameChosen"] = areaNamesChosen[0];
    } else if (areaNamesChosen.size() > 1){
      std::vector<uint8_t> nameNums = {};
      for (size_t c = 0; c < areaNamesChosen.size(); c++){
        nameNums.push_back(areaNamesChosen[c]);
      }
      log["areaNamesChosen"] = nameNums;
    }

    if (trials.size() == 1){
      log["trial"] = ctx->GetTrial(trials[0])->GetName().GetForCurrentLanguage(MF_CLEAN);
    } else if (trials.size() > 0){
      std::vector<std::string> trialStrings = {};
      for (size_t c = 0; c < trials.size(); c++){
        trialStrings.push_back(ctx->GetTrial(trials[c])->GetName().GetForCurrentLanguage(MF_CLEAN));
      }
      log["trials"] = trialStrings;
    }

    if (hintKeys.size() == 1){
      log["hintKey"] = hintKeys[0];
    } else if (hintKeys.size() > 1){
      std::vector<uint32_t> hintKeyNums = {};
      for (size_t c = 0; c < hintKeys.size(); c++){
        hintKeyNums.push_back(hintKeys[c]);
      }
      log["hintKeys"] = hintKeyNums;
    }

    if (hintTextsChosen.size() == 1){
      log["hintTextChosen"] = hintTextsChosen[0];
    } else if (hintTextsChosen.size() > 1){
      std::vector<uint8_t> nameNums = {};
      for (size_t c = 0; c < hintTextsChosen.size(); c++){
        nameNums.push_back(hintTextsChosen[c]);
      }
      log["hintTextsChosen"] = nameNums;
    }

    if (num != 0){
      log["num"] = num;
    }

  }
  return log;
}

void Hint::logHint(oJson& jsonData){
  auto ctx = Rando::Context::GetInstance();
  std::string logMap = "Static Hints";
  bool staticHint = true;
  if (ownKey < RH_GANONDORF_HINT){
    logMap = "Gossip Stone Hints";
    staticHint = false;
  }
  if (enabled &&
      (!(staticHint && (hintType == HINT_TYPE_ITEM) && ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_CLEAR)))){
      //skip if not enabled or if a static hint with no possible variance
    jsonData[logMap][Rando::StaticData::hintNames[ownKey].GetForCurrentLanguage(MF_CLEAN)] = toJSON();
  }
}

const HintText Hint::GetItemHintText(uint8_t slot, bool mysterious) const {
  //RANDOTODO make unreliant on locations, or make Hint accept ItemLocation
  auto ctx = Rando::Context::GetInstance();
  RandomizerCheck hintedCheck = locations[slot];
  RandomizerGet targetRG = ctx->GetItemLocation(hintedCheck)->GetPlacedRandomizerGet();
  if (mysterious){
    return StaticData::hintTextTable[RHT_MYSTERIOUS_ITEM];
  } else if (!ctx->GetOption(RSK_HINT_CLARITY).Is(RO_HINT_CLARITY_AMBIGUOUS) && targetRG == RG_ICE_TRAP) { //RANDOTODO store in item hint instead of item
      return HintText(CustomMessage({ctx->overrides[hintedCheck].GetTrickName()}));
  } else {
    return ctx->GetItemLocation(hintedCheck)->GetPlacedItem().GetHint();
  }
}

const HintText Hint::GetAreaHintText(uint8_t slot) const { 
  CustomMessage areaText;
  if (yourPocket && areas[slot] == RA_LINKS_POCKET){
    return StaticData::hintTextTable[RHT_YOUR_POCKET];
  } else {
    return StaticData::hintTextTable[Rando::StaticData::areaNames[areas[slot]]];
  }
}


const CustomMessage Hint::GetItemName(uint8_t slot, bool mysterious) const { 
  uint8_t nameNum = 0;
  if (itemNamesChosen.size() > slot){
    nameNum = itemNamesChosen[slot];
  }
  return GetItemHintText(slot, mysterious).GetHintMessage(nameNum);
}

const CustomMessage Hint::GetAreaName(uint8_t slot) const { 
  uint8_t nameNum = 0;
  if (areaNamesChosen.size() > slot){
    nameNum = areaNamesChosen[slot];
  }
  return GetAreaHintText(slot).GetHintMessage(nameNum);
}


CustomMessage Hint::GetBridgeReqsText() {
  auto ctx = Rando::Context::GetInstance();
  CustomMessage bridgeMessage;

  if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_ALWAYS_OPEN)) {
    return StaticData::hintTextTable[RHT_BRIDGE_OPEN_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_VANILLA)) {
    return StaticData::hintTextTable[RHT_BRIDGE_VANILLA_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_STONES_HINT].GetHintMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_MEDALLIONS_HINT].GetHintMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_REWARDS_HINT].GetHintMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_DUNGEONS_HINT].GetHintMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)) {
    bridgeMessage = StaticData::hintTextTable[RHT_BRIDGE_TOKENS_HINT].GetHintMessage();
    bridgeMessage.InsertNumber(ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)) {
    return  StaticData::hintTextTable[RHT_BRIDGE_GREG_HINT].GetHintMessage();
  }
  return bridgeMessage;
}

CustomMessage Hint::GetGanonBossKeyText() {
  auto ctx = Rando::Context::GetInstance();
  CustomMessage ganonBossKeyMessage;

  if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_STARTWITH)) {
    return StaticData::hintTextTable[RHT_GANON_BK_START_WITH_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_VANILLA)) {
    return StaticData::hintTextTable[RHT_GANON_BK_VANILLA_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OWN_DUNGEON)) {
    return StaticData::hintTextTable[RHT_GANON_BK_OWN_DUNGEON_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANY_DUNGEON)) {
    return StaticData::hintTextTable[RHT_GANON_BK_ANY_DUNGEON_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OVERWORLD)) {
    return StaticData::hintTextTable[RHT_GANON_BK_OVERWORLD_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANYWHERE)) {
    return StaticData::hintTextTable[RHT_GANON_BK_ANYWHERE_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
    return StaticData::hintTextTable[RHT_GANON_BK_SKULLTULA_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_VANILLA)) {
    return StaticData::hintTextTable[RHT_LACS_VANILLA_HINT].GetHintMessage();
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_STONES_HINT].GetHintMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_STONE_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_MEDALLIONS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_MEDALLIONS_HINT].GetHintMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_MEDALLION_COUNT).Value<uint8_t>());
  }
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_REWARDS_HINT].GetHintMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_DUNGEONS_HINT].GetHintMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>());
  } 
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
    ganonBossKeyMessage = StaticData::hintTextTable[RHT_LACS_TOKENS_HINT].GetHintMessage();
    ganonBossKeyMessage.InsertNumber(ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>());
  }
  else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_TRIFORCE_HUNT)) {
    return StaticData::hintTextTable[RHT_GANON_BK_TRIFORCE_HINT].GetHintMessage();
  }
  return ganonBossKeyMessage;
}


void Hint::AddHintedLocation(RandomizerCheck location) {
    locations.push_back(location);
}

std::vector<RandomizerCheck> Hint::GetHintedLocations() const {
    return locations;
}

void Hint::SetHintType(HintType type) {
    hintType = type;
}

HintType Hint::GetHintType() const {
    return hintType;
}

void Hint::AddHintedArea(RandomizerArea area) {
    areas.push_back(area);
}

std::vector<RandomizerArea> Hint::GetHintedAreas() const {
    return areas;
}

void Hint::SetDistribution(std::string distributionName) {
    distribution = distributionName;
}

const std::string& Hint::GetDistribution() const {
    return distribution;
}

bool Hint::IsEnabled() const{
  return enabled;
}

std::vector<RandomizerHintTextKey> Hint::GetHintTextKeys() const{
  return hintKeys;
}

std::vector<RandomizerGet> Hint::GetHintedItems() const{
  return items;
}

std::vector<uint8_t> Hint::GetItemNamesChosen() const{
  return itemNamesChosen;
}

std::vector<uint8_t> Hint::GetHintTextsChosen() const{
  return hintTextsChosen;
}
std::vector<uint8_t> Hint::GetAreaTextsChosen() const{
  return areaNamesChosen;
}

std::vector<TrialKey> Hint::GetHintedTrials() const{
  return trials;
}

int Hint::GetNum(){
  return num;
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
  itemNamesChosen = {};
  hintTextsChosen = {};
  areaNamesChosen = {};
}

}
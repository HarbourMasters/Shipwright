#include "spoiler_log.hpp"

#include "../dungeon.h"
#include "../static_data.h"
#include "../settings.h"
#include "../entrance.h"
#include "random.hpp"
#include "../trial.h"
#include "hints.hpp"
#include "pool_functions.hpp"
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include <nlohmann/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <variables.h>

#include <Context.h>
#include <soh/OTRGlobals.h>

#include "consolevariablebridge.h"

using json = nlohmann::ordered_json;
using namespace Rando;

json jsonData;
std::map<RandomizerHintTextKey, Rando::ItemLocation*> hintedLocations;

extern std::array<std::string, 17> hintCategoryNames;
extern Region* GetHintRegion(uint32_t);

namespace {
std::string placementtxt;
} // namespace

void GenerateHash() {
    auto ctx = Rando::Context::GetInstance();
    std::string hash = ctx->GetHash();
    // adds leading 0s to the hash string if it has less than 10 digits.
    while (hash.length() < 10) {
        hash = "0" + hash;
    }
    for (size_t i = 0, j = 0; i < ctx->hashIconIndexes.size(); i++, j += 2) {
        int number = std::stoi(hash.substr(j, 2));
        ctx->hashIconIndexes[i] = number;
    }
}

static auto GetGeneralPath() {
    return "./randomizer/haha.xml";
}

static auto GetSpoilerLogPath() {
    return GetGeneralPath();
}

static auto GetPlacementLogPath() {
  return GetGeneralPath();
}

// Writes the location to the specified node.
static void WriteLocation(
    std::string sphere, const RandomizerCheck locationKey, const bool withPadding = false) {
  Rando::Location* location = Rando::StaticData::GetLocation(locationKey);
  Rando::ItemLocation* itemLocation = Rando::Context::GetInstance()->GetItemLocation(locationKey);

  switch (gSaveContext.language) {
        case LANGUAGE_ENG:
        default:
            jsonData["playthrough"][sphere][location->GetName()] = itemLocation->GetPlacedItemName().GetEnglish();
            break;
        case LANGUAGE_FRA:
            jsonData["playthrough"][sphere][location->GetName()] = itemLocation->GetPlacedItemName().GetFrench();
            break;
    }
}

//Writes a shuffled entrance to the specified node
static void WriteShuffledEntrance(std::string sphereString, Entrance* entrance) {
  int16_t originalIndex = entrance->GetIndex();
  int16_t destinationIndex = -1;
  int16_t replacementIndex = entrance->GetReplacement()->GetIndex();
  int16_t replacementDestinationIndex = -1;
  std::string name = GetEntranceData(originalIndex)->source;
  std::string text = GetEntranceData(replacementIndex)->destination;

  // Track the reverse destination, useful for savewarp handling
  if (entrance->GetReverse() != nullptr) {
    destinationIndex = entrance->GetReverse()->GetIndex();
    // When decouple is off we track the replacement's reverse destination, useful for recording visited entrances
    if (!entrance->IsDecoupled()) {
      replacementDestinationIndex = entrance->GetReplacement()->GetReverse()->GetIndex();
    }
  }

  json entranceJson = json::object({
    {"type", entrance->GetType()},
    {"index", originalIndex},
    {"destination", destinationIndex},
    {"override", replacementIndex},
    {"overrideDestination", replacementDestinationIndex},
  });

  jsonData["entrances"].push_back(entranceJson);

  // When decoupled entrances is off, handle saving reverse entrances
  if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
    json reverseEntranceJson = json::object({
      {"type", entrance->GetReverse()->GetType()},
      {"index", replacementDestinationIndex},
      {"destination", replacementIndex},
      {"override", destinationIndex},
      {"overrideDestination", originalIndex},
    });

    jsonData["entrances"].push_back(reverseEntranceJson);
  }

  switch (gSaveContext.language) {
        case LANGUAGE_ENG:
        case LANGUAGE_FRA:
        default:
            jsonData["entrancesMap"][sphereString][name] = text;
            break;
    }
}

// Writes the settings (without excluded locations, starting inventory and tricks) to the spoilerLog document.
static void WriteSettings() {
    auto ctx = Rando::Context::GetInstance();
    std::array<Rando::Option, RSK_MAX> options = Rando::Settings::GetInstance()->GetAllOptions();
    for (const Rando::Option& option : options) {
      if (option.GetName() != ""){
        jsonData["settings"][option.GetName()] = option.GetOptionText(ctx->GetOption(option.GetKey()).Get());
      }
    }
}

// Removes any line breaks from s.
std::string RemoveLineBreaks(std::string s) {
  s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
  return s;
}

// Writes the excluded locations to the spoiler log, if there are any.
static void WriteExcludedLocations() {
  auto ctx = Rando::Context::GetInstance();

  for (size_t i = 1; i < Rando::Settings::GetInstance()->GetExcludeLocationsOptions().size(); i++) {
    for (const auto& location : Rando::Settings::GetInstance()->GetExcludeLocationsOptions()[i]) {
      if (ctx->GetLocationOption(static_cast<RandomizerCheck>(location->GetKey())).Get() == RO_LOCATION_INCLUDE) {
        continue;
      }

      jsonData["excludedLocations"].push_back(RemoveLineBreaks(location->GetName()));

    }
  }
}

// Writes the starting inventory to the spoiler log, if there is any.
static void WriteStartingInventory() {
    auto ctx = Rando::Context::GetInstance();
    const Rando::OptionGroup& optionGroup = Rando::Settings::GetInstance()->GetOptionGroup(RSG_STARTING_INVENTORY);
    for (const Rando::OptionGroup* subGroup : optionGroup.GetSubGroups()) {
        if (subGroup->GetContainsType() == Rando::OptionGroupType::DEFAULT) {
            for (Rando::Option* option : subGroup->GetOptions()) {
                jsonData["settings"][option->GetName()] = option->GetOptionText(ctx->GetOption(option->GetKey()).Get());
            }
        }
    }
}

//Writes the enabled tricks to the spoiler log, if there are any.
static void WriteEnabledTricks() {
  auto ctx = Rando::Context::GetInstance();

  for (const auto& setting : Rando::Settings::GetInstance()->GetOptionGroup(RSG_TRICKS).GetOptions()) {
    if (ctx->GetOption(setting->GetKey()).IsNot(RO_GENERIC_ON)) {
      continue;
    }
    jsonData["enabledTricks"].push_back(RemoveLineBreaks(setting->GetName()).c_str());
  }
}

// Writes the Master Quest dungeons to the spoiler log, if there are any.
static void WriteMasterQuestDungeons() {
    auto ctx = Rando::Context::GetInstance();
    for (const auto* dungeon : ctx->GetDungeons()->GetDungeonList()) {
        std::string dungeonName;
        if (dungeon->IsVanilla()) {
            continue;
        }
        jsonData["masterQuestDungeons"].push_back(dungeon->GetName());
    }
}

// Writes the required trials to the spoiler log, if there are any.
static void WriteChosenOptions() {
  auto ctx = Rando::Context::GetInstance();
  for (const auto& trial : ctx->GetTrials()->GetTrialList()) {
    if (trial->IsRequired()) {
      std::string trialName = trial->GetName().GetForCurrentLanguage(MF_CLEAN);
      jsonData["requiredTrials"].push_back(RemoveLineBreaks(trialName));
    }
  }
  if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_ADULT)){
    jsonData["SelectedStartingAge"] = "Adult";
  } else {
    jsonData["SelectedStartingAge"] = "Child";
  }
}

// Writes the intended playthrough to the spoiler log, separated into spheres.
static void WritePlaythrough() {
  auto ctx = Rando::Context::GetInstance();

  for (uint32_t i = 0; i < ctx->playthroughLocations.size(); ++i) {
    auto sphereNum = std::to_string(i);
    std::string sphereString =  "sphere ";
    if (i < 10) sphereString += "0";
    sphereString += sphereNum;
    for (const RandomizerCheck key : ctx->playthroughLocations[i]) {
      WriteLocation(sphereString, key, true);
    }
  }
}

//Write the randomized entrance playthrough to the spoiler log, if applicable
static void WriteShuffledEntrances() {
  auto ctx = Rando::Context::GetInstance();
  for (uint32_t i = 0; i < ctx->GetEntranceShuffler()->playthroughEntrances.size(); ++i) {
    auto sphereNum = std::to_string(i);
    std::string sphereString = "sphere ";
    if (i < 10) sphereString += "0";
    sphereString += sphereNum;
    for (Entrance* entrance : ctx->GetEntranceShuffler()->playthroughEntrances[i]) {
        WriteShuffledEntrance(sphereString, entrance);
    }
  }
}

Rando::ItemLocation* GetItemLocation(RandomizerGet item) {
    auto ctx = Rando::Context::GetInstance();
    return ctx->GetItemLocation(FilterFromPool(ctx->allLocations, [item, ctx](const RandomizerCheck loc) {
        return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == item;
    })[0]);
}

static void WriteAllLocations() {
    auto ctx = Rando::Context::GetInstance();
    for (const RandomizerCheck key : ctx->allLocations) {
        Rando::ItemLocation* location = ctx->GetItemLocation(key);
        std::string placedItemName;

        switch (gSaveContext.language) {
          case 0:
          default:
            placedItemName = location->GetPlacedItemName().english;
            break;
          case 2:
            placedItemName = location->GetPlacedItemName().french;
            break;
        }

        // If it's a simple item (not an ice trap, doesn't have a price)
        // just add the name of the item and move on
        if (!location->HasCustomPrice() &&
            location->GetPlacedRandomizerGet() != RG_ICE_TRAP) {
            
            jsonData["locations"][Rando::StaticData::GetLocation(location->GetRandomizerCheck())->GetName()] = placedItemName;
            continue;
        }

        // We're dealing with a complex item, build out the json object for it
        jsonData["locations"][Rando::StaticData::GetLocation(location->GetRandomizerCheck())->GetName()]["item"] = placedItemName;

        if (location->HasCustomPrice()) {
            jsonData["locations"][Rando::StaticData::GetLocation(location->GetRandomizerCheck())->GetName()]["price"] =
                location->GetPrice();
        }
        if (location->IsAHintAccessible()) {
          hintedLocations.emplace(Rando::StaticData::GetLocation(key)->GetHintKey(), location);
        }

        if (location->GetPlacedRandomizerGet() == RG_ICE_TRAP) {
          switch (gSaveContext.language) {
              case 0:
              default:
                  jsonData["locations"][Rando::StaticData::GetLocation(location->GetRandomizerCheck())->GetName()]["model"] =
                      Rando::StaticData::RetrieveItem(ctx->overrides[location->GetRandomizerCheck()].LooksLike()).GetName().english;
                  jsonData["locations"][Rando::StaticData::GetLocation(location->GetRandomizerCheck())->GetName()]["trickName"] = 
                      ctx->overrides[location->GetRandomizerCheck()].GetTrickName().english;
                  break;
              case 2:
                  jsonData["locations"][Rando::StaticData::GetLocation(location->GetRandomizerCheck())->GetName()]["model"] =
                      Rando::StaticData::RetrieveItem(ctx->overrides[location->GetRandomizerCheck()].LooksLike()).GetName().french;
                  jsonData["locations"][Rando::StaticData::GetLocation(location->GetRandomizerCheck())->GetName()]["trickName"] =
                      ctx->overrides[location->GetRandomizerCheck()].GetTrickName().french;
                  break;
          }
      }
    }
}

const char* SpoilerLog_Write() {
    auto ctx = Rando::Context::GetInstance();

    jsonData.clear();

    jsonData["version"] = (char*) gBuildVersion;
    jsonData["git_branch"] = (char*) gGitBranch;
    jsonData["git_commit"] = (char*) gGitCommitHash;
    jsonData["seed"] = ctx->GetSeedString();
    jsonData["finalSeed"] = ctx->GetSeed();

    // Write Hash
    int index = 0;
    for (uint8_t seed_value : ctx->hashIconIndexes) {
        jsonData["file_hash"][index] = seed_value;
        index++;
    }

    WriteSettings();
    WriteExcludedLocations();
    WriteStartingInventory();
    WriteEnabledTricks(); 
    WriteMasterQuestDungeons();
    WriteChosenOptions();
    WritePlaythrough();

    ctx->playthroughLocations.clear();
    ctx->playthroughBeatable = false;

    ctx->WriteHintJson(jsonData);
    WriteShuffledEntrances();
    WriteAllLocations();

    if (!std::filesystem::exists(Ship::Context::GetPathRelativeToAppDirectory("Randomizer"))) {
        std::filesystem::create_directory(Ship::Context::GetPathRelativeToAppDirectory("Randomizer"));
    }

    std::string jsonString = jsonData.dump(4);
    std::ostringstream fileNameStream;
    for (uint8_t i = 0; i < ctx->hashIconIndexes.size(); i ++) {
        if (i) {
            fileNameStream << '-';
        }
        if (ctx->hashIconIndexes[i] < 10) {
            fileNameStream << '0';
        }
        fileNameStream << std::to_string(ctx->hashIconIndexes[i]);
    }
    std::string fileName = fileNameStream.str();
    std::ofstream jsonFile(Ship::Context::GetPathRelativeToAppDirectory(
        (std::string("Randomizer/") + fileName + std::string(".json")).c_str()));
    jsonFile << std::setw(4) << jsonString << std::endl;
    jsonFile.close();

    CVarSetString(CVAR_GENERAL("SpoilerLog"), (std::string("./Randomizer/") + fileName + std::string(".json")).c_str());

    // Note: probably shouldn't return this without making sure this string is stored somewhere, but
    // this return value is currently only used in playthrough.cpp as a true/false. Even if the pointer
    // is no longer valid it would still not be nullptr if the spoilerfile was written, so it works but
    // should probably be changed for correctness later on.
    return fileName.c_str();
}

void PlacementLog_Msg(std::string_view msg) {
    placementtxt += msg;
}

void PlacementLog_Clear() {
    placementtxt = "";
}


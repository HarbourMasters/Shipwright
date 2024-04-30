#include "spoiler_log.hpp"

#include "../dungeon.h"
#include "../static_data.h"
#include "../context.h"
#include "../entrance.h"
#include "random.hpp"
#include "../trial.h"
#include "tinyxml2.h"
#include "utils.hpp"
#include "shops.hpp"
#include "hints.hpp"
#include "pool_functions.hpp"
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
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

#include "consolevariablebridge.h"

using json = nlohmann::ordered_json;
using namespace Rando;

json jsonData;
std::map<RandomizerHintTextKey, Rando::ItemLocation*> hintedLocations;

extern std::array<std::string, 17> hintCategoryNames;
extern Area* GetHintRegion(uint32_t);

namespace {
std::string placementtxt;
} // namespace

static SpoilerData spoilerData;

void GenerateHash() {
    auto ctx = Rando::Context::GetInstance();
    std::string hash = ctx->GetSettings()->GetHash();
    // adds leading 0s to the hash string if it has less than 10 digits.
    while (hash.length() < 10) {
        hash = "0" + hash;
    }
    for (size_t i = 0, j = 0; i < ctx->hashIconIndexes.size(); i++, j += 2) {
        int number = std::stoi(hash.substr(j, 2));
        ctx->hashIconIndexes[i] = number;
    }

    // Clear out spoiler log data here, in case we aren't going to re-generate it
    // spoilerData = { 0 };
}

const SpoilerData& GetSpoilerData() {
  return spoilerData;
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

void WriteIngameSpoilerLog() {
    auto ctx = Rando::Context::GetInstance();
    uint16_t spoilerItemIndex = 0;
    uint32_t spoilerStringOffset = 0;
    uint16_t spoilerSphereItemoffset = 0;
    uint16_t spoilerGroupOffset = 0;
    // Intentionally junk value so we trigger the 'new group, record some stuff' code
    uint8_t currentGroup = SpoilerCollectionCheckGroup::SPOILER_COLLECTION_GROUP_COUNT;
    bool spoilerOutOfSpace = false;

    // Create map of string data offsets for all _unique_ item locations and names in the playthrough
    // Some item names, like gold skulltula tokens, can appear many times in a playthrough
    std::unordered_map<uint32_t, uint16_t>
        itemLocationsMap; // Map of LocationKey to an index into spoiler data item locations
    itemLocationsMap.reserve(ctx->allLocations.size());
    std::unordered_map<std::string, uint16_t>
        stringOffsetMap; // Map of strings to their offset into spoiler string data array
    stringOffsetMap.reserve(ctx->allLocations.size() * 2);

    // Sort all locations by their group, so the in-game log can show a group of items by simply starting/ending at
    // certain indices
    std::stable_sort(ctx->allLocations.begin(), ctx->allLocations.end(), [](const RandomizerCheck& a, const RandomizerCheck& b) {
        auto groupA = Rando::StaticData::GetLocation(a)->GetCollectionCheckGroup();
        auto groupB = Rando::StaticData::GetLocation(b)->GetCollectionCheckGroup();
        return groupA < groupB;
    });

    for (const RandomizerCheck key : ctx->allLocations) {
        auto loc = Rando::StaticData::GetLocation(key);
        auto itemLocation = ctx->GetItemLocation(key);

        // Hide excluded locations from ingame tracker
        // if (loc->IsExcluded()) {
        //     continue;
        // }
        // Beehives
        if (!ctx->GetOption(RSK_SHUFFLE_BEEHIVES) && loc->IsCategory(Category::cBeehive)) {
            continue;
        }
        // Cows
        else if (!ctx->GetOption(RSK_SHUFFLE_COWS) && loc->IsCategory(Category::cCow)) {
            continue;
        }
        // Merchants
        else if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_OFF) && loc->IsCategory(Category::cMerchant)) {
            continue;
        }
        // Adult Trade
        else if (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && loc->IsCategory(Category::cAdultTrade)) {
            continue;
        }
        // Chest Minigame
        else if (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_GENERIC_OFF) &&
                 loc->IsCategory(Category::cChestMinigame)) {
            continue;
        }
        // Gerudo Fortress
        else if ((ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_NORMAL) &&
                  (loc->IsCategory(Category::cVanillaGFSmallKey) || loc->GetHintKey() == RHT_GF_GERUDO_MEMBERSHIP_CARD)) ||
                 (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_FAST) && loc->IsCategory(Category::cVanillaGFSmallKey) &&
                  loc->GetHintKey() != RHT_GF_NORTH_F1_CARPENTER)) {
            continue;
        }

        // Copy at most 51 chars from the name and location name to avoid issues with names that don't fit on screen
        const char* nameFormatStr = "%.51s";

        auto locName = loc->GetName();
        if (stringOffsetMap.find(locName) == stringOffsetMap.end()) {
            if (spoilerStringOffset + locName.size() + 1 >= SPOILER_STRING_DATA_SIZE) {
                spoilerOutOfSpace = true;
                break;
            } else {
                stringOffsetMap[locName] = spoilerStringOffset;
                spoilerStringOffset +=
                    sprintf(&spoilerData.StringData[spoilerStringOffset], nameFormatStr, locName.c_str()) + 1;
            }
        }
        // PURPLE TODO: LOCALIZATION
        auto locItem = itemLocation->GetPlacedItemName().GetEnglish();
        if (itemLocation->GetPlacedRandomizerGet() == RG_ICE_TRAP && loc->IsCategory(Category::cShop)) {
            locItem = NonShopItems[TransformShopIndex(GetShopIndex(key))].Name.GetEnglish();
        }
        if (stringOffsetMap.find(locItem) == stringOffsetMap.end()) {
            if (spoilerStringOffset + locItem.size() + 1 >= SPOILER_STRING_DATA_SIZE) {
                spoilerOutOfSpace = true;
                break;
            } else {
                stringOffsetMap[locItem] = spoilerStringOffset;
                spoilerStringOffset +=
                    sprintf(&spoilerData.StringData[spoilerStringOffset], nameFormatStr, locItem.c_str()) + 1;
            }
        }

        spoilerData.ItemLocations[spoilerItemIndex].LocationStrOffset = stringOffsetMap[locName];
        spoilerData.ItemLocations[spoilerItemIndex].ItemStrOffset = stringOffsetMap[locItem];
        spoilerData.ItemLocations[spoilerItemIndex].LocationStr = locName;
        spoilerData.ItemLocations[spoilerItemIndex].ItemStr = locItem;
        spoilerData.ItemLocations[spoilerItemIndex].CollectionCheckType = loc->GetCollectionCheck().type;
        spoilerData.ItemLocations[spoilerItemIndex].LocationScene = loc->GetCollectionCheck().scene;
        spoilerData.ItemLocations[spoilerItemIndex].LocationFlag = loc->GetCollectionCheck().flag;

        // Collect Type and Reveal Type
        if (key == RC_GANON) {
            spoilerData.ItemLocations[spoilerItemIndex].CollectType = COLLECTTYPE_NEVER;
            spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_ALWAYS;
        } else if (key == RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS) {
            spoilerData.ItemLocations[spoilerItemIndex].CollectType = COLLECTTYPE_REPEATABLE;
            spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_ALWAYS;
        }
        // Shops
        else if (loc->IsShop()) {
            if (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_OFF)) {
                spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_ALWAYS;
            } else {
                spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_SCENE;
            }
            if (itemLocation->GetPlacedItem().GetItemType() == ITEMTYPE_REFILL ||
                itemLocation->GetPlacedItem().GetItemType() == ITEMTYPE_SHOP ||
                itemLocation->GetPlacedItem().GetHintKey() == RHT_PROGRESSIVE_BOMBCHUS) {
                spoilerData.ItemLocations[spoilerItemIndex].CollectType = COLLECTTYPE_REPEATABLE;
            }
        }
        // Gold Skulltulas
        else if (loc->IsCategory(Category::cSkulltula) &&
                 ((ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OFF)) ||
                  (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_DUNGEONS) && !loc->IsDungeon()) ||
                  (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OVERWORLD) && loc->IsDungeon()))) {
            spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_ALWAYS;
        }
        // Deku Scrubs
        else if (loc->IsCategory(Category::cDekuScrub) && !loc->IsCategory(Category::cDekuScrubUpgrades) &&
                 ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_OFF)) {
            spoilerData.ItemLocations[spoilerItemIndex].CollectType = COLLECTTYPE_REPEATABLE;
            spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_ALWAYS;
        }

        auto checkGroup = loc->GetCollectionCheckGroup();
        spoilerData.ItemLocations[spoilerItemIndex].Group = checkGroup;

        // Group setup
        if (checkGroup != currentGroup) {
            currentGroup = checkGroup;
            spoilerData.GroupOffsets[currentGroup] = spoilerGroupOffset;
        }
        ++spoilerData.GroupItemCounts[currentGroup];
        ++spoilerGroupOffset;

        itemLocationsMap[key] = spoilerItemIndex++;
    }
    spoilerData.ItemLocationsCount = spoilerItemIndex;

    if (/*Settings::IngameSpoilers TODO: Remove: don't think we have any need for this*/ false) {
        bool playthroughItemNotFound = false;
        // Write playthrough data to in-game spoiler log
        if (!spoilerOutOfSpace) {
            for (uint32_t i = 0; i < ctx->playthroughLocations.size(); i++) {
                if (i >= SPOILER_SPHERES_MAX) {
                    spoilerOutOfSpace = true;
                    break;
                }
                spoilerData.Spheres[i].ItemLocationsOffset = spoilerSphereItemoffset;
                for (uint32_t loc = 0; loc < ctx->playthroughLocations[i].size(); ++loc) {
                    if (spoilerSphereItemoffset >= SPOILER_ITEMS_MAX) {
                        spoilerOutOfSpace = true;
                        break;
                    }

                    const auto foundItemLoc = itemLocationsMap.find(ctx->playthroughLocations[i][loc]);
                    if (foundItemLoc != itemLocationsMap.end()) {
                        spoilerData.SphereItemLocations[spoilerSphereItemoffset++] = foundItemLoc->second;
                    } else {
                        playthroughItemNotFound = true;
                    }
                    ++spoilerData.Spheres[i].ItemCount;
                }
                ++spoilerData.SphereCount;
            }
        }
        if (spoilerOutOfSpace || playthroughItemNotFound) {
            SPDLOG_ERROR("In-game spoiler log is out of space, playthrough data will not be written");
        }
    }
}

// Writes the location to the specified node.
static void WriteLocation(
    std::string sphere, const RandomizerCheck locationKey, const bool withPadding = false) {
  Rando::Location* location = Rando::StaticData::GetLocation(locationKey);
  Rando::ItemLocation* itemLocation = Rando::Context::GetInstance()->GetItemLocation(locationKey);

  // auto node = parentNode->InsertNewChildElement("location");
  switch (gSaveContext.language) {
        case LANGUAGE_ENG:
        default:
            jsonData["playthrough"][sphere][location->GetName()] = itemLocation->GetPlacedItemName().GetEnglish();
            break;
        case LANGUAGE_FRA:
            jsonData["playthrough"][sphere][location->GetName()] = itemLocation->GetPlacedItemName().GetFrench();
            break;
    }
  // node->SetAttribute("name", location->GetName().c_str());
  // node->SetText(location->GetPlacedItemName().GetEnglish().c_str());

  // if (withPadding) {
  //   constexpr int16_t LONGEST_NAME = 56; // The longest name of a location.
  //   constexpr int16_t PRICE_ATTRIBUTE = 12; // Length of a 3-digit price attribute.

  //   // Insert a padding so we get a kind of table in the XML document.
  //   int16_t requiredPadding = LONGEST_NAME - location->GetName().length();
  //   if (location->IsCategory(Category::cShop)) {
  //     // Shop items have short location names, but come with an additional price attribute.
  //     requiredPadding -= PRICE_ATTRIBUTE;
  //   }
  //   if (requiredPadding >= 0) {
  //     std::string padding(requiredPadding, ' ');
  //     node->SetAttribute("_", padding.c_str());
  //   }
  // }

  // if (location->IsCategory(Category::cShop)) {
  //   char price[6];
  //   sprintf(price, "%03d", location->GetPrice());
  //   node->SetAttribute("price", price);
  // }
  // if (!location->IsAddedToPool()) {
  //   #ifdef ENABLE_DEBUG
  //     node->SetAttribute("not-added", true);
  //   #endif
  // }
}

//Writes a shuffled entrance to the specified node
static void WriteShuffledEntrance(std::string sphereString, Entrance* entrance) {
  int16_t originalIndex = entrance->GetIndex();
  int16_t destinationIndex = -1;
  int16_t replacementIndex = entrance->GetReplacement()->GetIndex();
  int16_t replacementDestinationIndex = -1;
  std::string name = entrance->GetName();
  std::string text = entrance->GetConnectedRegion()->regionName + " from " + entrance->GetReplacement()->GetParentRegion()->regionName;

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
static void WriteSettings(const bool printAll = false) {
    // auto parentNode = spoilerLog.NewElement("settings");
    auto ctx = Rando::Context::GetInstance();
    auto allOptionGroups = ctx->GetSettings()->GetOptionGroups();
    for (const Rando::OptionGroup& optionGroup : allOptionGroups) {
        if (optionGroup.GetName() == "Timesaver Settings") {
            for (const Rando::Option* option : optionGroup.GetOptions()) {
                if (option->GetName() == "Big Poe Target Count" || option->GetName() == "Cuccos to return" ||
                    option->GetName() == "Skip Epona Race" || option->GetName() == "Skip Tower Escape" ||
                    option->GetName() == "Skip Child Stealth" || option->GetName() == "Complete Mask Quest" ||
                    option->GetName() == "Skip Scarecrow's Song" ||
                    option->GetName() == "Enable Glitch-Useful Cutscenes") {
                    std::string settingName = optionGroup.GetName() + ":" + option->GetName();
                    jsonData["settings"][settingName] = option->GetSelectedOptionText();
                }
            }
            continue;
        }
        if (optionGroup.GetContainsType() == Rando::OptionGroupType::DEFAULT && optionGroup.PrintInSpoiler()) {
            for (const Rando::Option* option : optionGroup.GetOptions()) {
                std::string settingName = optionGroup.GetName() + ":" + option->GetName();
                jsonData["settings"][settingName] = option->GetSelectedOptionText();
            }
        }
    }

    // spoilerLog.RootElement()->InsertEndChild(parentNode);

    //     for (const uint32_t key : allLocations) {
    //       ItemLocation* location = GetLocation(key);
    //       settingsJsonData["locations"][location->GetName()] = location->GetPlacedItemName().english;
    //   }
}

// Writes the excluded locations to the spoiler log, if there are any.
static void WriteExcludedLocations() {
  // auto parentNode = spoilerLog.NewElement("excluded-locations");
  auto ctx = Rando::Context::GetInstance();

  for (size_t i = 1; i < ctx->GetSettings()->GetExcludeLocationsOptions().size(); i++) {
    for (const auto& location : ctx->GetSettings()->GetExcludeLocationsOptions()[i]) {
      if (location->GetSelectedOptionIndex() == RO_LOCATION_INCLUDE) {
        continue;
      }

      jsonData["excludedLocations"].push_back(RemoveLineBreaks(location->GetName()));

      // tinyxml2::XMLElement* node = spoilerLog.NewElement("location");
      // node->SetAttribute("name", RemoveLineBreaks(location->GetName()).c_str());
      // parentNode->InsertEndChild(node);
    }
  }

  // if (!parentNode->NoChildren()) {
  //   spoilerLog.RootElement()->InsertEndChild(parentNode);
  // }
}

// Writes the starting inventory to the spoiler log, if there is any.
static void WriteStartingInventory() {
    auto ctx = Rando::Context::GetInstance();
    const Rando::OptionGroup& optionGroup = ctx->GetSettings()->GetOptionGroup(RSG_STARTING_INVENTORY);
    for (const Rando::OptionGroup* subGroup : optionGroup.GetSubGroups()) {
        if (subGroup->GetContainsType() == Rando::OptionGroupType::DEFAULT) {
            for (const Rando::Option* option : subGroup->GetOptions()) {
                jsonData["settings"][option->GetName()] = option->GetSelectedOptionText();
            }
        }
    }
}

// Writes the enabled tricks to the spoiler log, if there are any.
static void WriteEnabledTricks(tinyxml2::XMLDocument& spoilerLog) {
  //auto parentNode = spoilerLog.NewElement("enabled-tricks");
  auto ctx = Rando::Context::GetInstance();

  for (const auto& setting : ctx->GetSettings()->GetOptionGroup(RSG_TRICKS).GetOptions()) {
    if (setting->GetSelectedOptionIndex() != RO_GENERIC_ON/* || !setting->IsCategory(OptionCategory::Setting)*/) {
      continue;
    }
    jsonData["enabledTricks"].push_back(RemoveLineBreaks(setting->GetName()).c_str());
    //auto node = parentNode->InsertNewChildElement("trick");
    //node->SetAttribute("name", RemoveLineBreaks(setting->GetName()).c_str());
  }

  // if (!parentNode->NoChildren()) {
  //  spoilerLog.RootElement()->InsertEndChild(parentNode);
  //}
}

// Writes the enabled glitches to the spoiler log, if there are any.
// TODO: Implement Glitches
// static void WriteEnabledGlitches(tinyxml2::XMLDocument& spoilerLog) {
//   auto parentNode = spoilerLog.NewElement("enabled-glitches");

//   for (const auto& setting : Settings::glitchCategories) {
//     if (setting->Value<uint8_t>() == 0) {
//       continue;
//     }

//     auto node = parentNode->InsertNewChildElement("glitch-category");
//     node->SetAttribute("name", setting->GetName().c_str());
//     node->SetText(setting->GetSelectedOptionText().c_str());
//   }

//   for (const auto& setting : Settings::miscGlitches) {
//     if (!setting->Value<bool>()) {
//       continue;
//     }

//     auto node = parentNode->InsertNewChildElement("misc-glitch");
//     node->SetAttribute("name", RemoveLineBreaks(setting->GetName()).c_str());
//   }

//   if (!parentNode->NoChildren()) {
//     spoilerLog.RootElement()->InsertEndChild(parentNode);
//   }
// }

// Writes the Master Quest dungeons to the spoiler log, if there are any.
static void WriteMasterQuestDungeons(tinyxml2::XMLDocument& spoilerLog) {
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
static void WriteRequiredTrials() {
    auto ctx = Rando::Context::GetInstance();
    for (const auto& trial : ctx->GetTrials()->GetTrialList()) {
        if (trial->IsRequired()) {
            std::string trialName = trial->GetName().GetForCurrentLanguage(MF_CLEAN);
            jsonData["requiredTrials"].push_back(RemoveLineBreaks(trialName));
        }
    }
}

// Writes the intended playthrough to the spoiler log, separated into spheres.
static void WritePlaythrough() {
  // auto playthroughNode = spoilerLog.NewElement("playthrough");
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

  // spoilerLog.RootElement()->InsertEndChild(playthroughNode);
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
    auto spoilerLog = tinyxml2::XMLDocument(false);
    spoilerLog.InsertEndChild(spoilerLog.NewDeclaration());

    auto rootNode = spoilerLog.NewElement("spoiler-log");
    spoilerLog.InsertEndChild(rootNode);

    jsonData.clear();

    jsonData["version"] = (char*) gBuildVersion;
    jsonData["seed"] = ctx->GetSettings()->GetSeedString();
    jsonData["finalSeed"] = ctx->GetSettings()->GetSeed();

    // Write Hash
    int index = 0;
    for (uint8_t seed_value : ctx->hashIconIndexes) {
        jsonData["file_hash"][index] = seed_value;
        index++;
    }

    WriteSettings();
    WriteExcludedLocations();
    WriteStartingInventory();
    WriteEnabledTricks(spoilerLog); //RANDOTODO clean up spoilerLog refernces
    //if (Settings::Logic.Is(LOGIC_GLITCHED)) {
    //    WriteEnabledGlitches(spoilerLog);
    //}
    WriteMasterQuestDungeons(spoilerLog);
    WriteRequiredTrials();
    WritePlaythrough();

    ctx->playthroughLocations.clear();
    ctx->playthroughBeatable = false;

    ctx->WriteHintJson(jsonData);
    WriteShuffledEntrances();
    WriteAllLocations();

    if (!std::filesystem::exists(LUS::Context::GetPathRelativeToAppDirectory("Randomizer"))) {
        std::filesystem::create_directory(LUS::Context::GetPathRelativeToAppDirectory("Randomizer"));
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
    std::ofstream jsonFile(LUS::Context::GetPathRelativeToAppDirectory(
        (std::string("Randomizer/") + fileName + std::string(".json")).c_str()));
    jsonFile << std::setw(4) << jsonString << std::endl;
    jsonFile.close();

    CVarSetString("gSpoilerLog", (std::string("./Randomizer/") + fileName + std::string(".json")).c_str());

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

bool PlacementLog_Write() {
    auto placementLog = tinyxml2::XMLDocument(false);
    placementLog.InsertEndChild(placementLog.NewDeclaration());

    auto rootNode = placementLog.NewElement("placement-log");
    placementLog.InsertEndChild(rootNode);

    // rootNode->SetAttribute("version", Settings::version.c_str());
    // rootNode->SetAttribute("seed", Settings::seed);
    // TODO: Do we even use this?

    // WriteSettings(placementLog, true); // Include hidden settings.
    // WriteExcludedLocations(placementLog);
    // WriteStartingInventory(placementLog);
    WriteEnabledTricks(placementLog);
    //WriteEnabledGlitches(placementLog);
    WriteMasterQuestDungeons(placementLog);
    //WriteRequiredTrials(placementLog);

    placementtxt = "\n" + placementtxt;

    auto node = rootNode->InsertNewChildElement("log");
    auto contentNode = node->InsertNewText(placementtxt.c_str());
    contentNode->SetCData(true);

    return true;
}

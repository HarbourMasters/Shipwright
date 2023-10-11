#include "spoiler_log.hpp"

#include "dungeon.hpp"
#include "../static_data.h"
#include "../context.h"
#include "entrance.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "trial.hpp"
#include "tinyxml2.h"
#include "utils.hpp"
#include "shops.hpp"
#include "hints.hpp"
#include "../randomizer_tricks.h"
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

using json = nlohmann::ordered_json;

json jsonData;
std::map<RandomizerHintTextKey, Rando::ItemLocation*> hintedLocations;

extern std::unordered_map<HintType, std::string> hintTypeNames;
extern std::array<std::string, 17> hintCategoryNames;
extern Area* GetHintRegion(uint32_t);

namespace {
std::string placementtxt;
} // namespace

static SpoilerData spoilerData;

void GenerateHash() {
    std::string hash = Settings::hash;
    // adds leading 0s to the hash string if it has less than 10 digits.
    while (hash.length() < 10) {
        hash = "0" + hash;
    }
    for (size_t i = 0, j = 0; i < Settings::hashIconIndexes.size(); i++, j += 2) {
        int number = std::stoi(hash.substr(j, 2));
        Settings::hashIconIndexes[i] = number;
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
        auto groupA = StaticData::Location(a)->GetCollectionCheckGroup();
        auto groupB = StaticData::Location(b)->GetCollectionCheckGroup();
        return groupA < groupB;
    });

    for (const RandomizerCheck key : ctx->allLocations) {
        auto loc = StaticData::Location(key);
        auto itemLocation = ctx->GetItemLocation(key);

        // Hide excluded locations from ingame tracker
        // if (loc->IsExcluded()) {
        //     continue;
        // }
        // Cows
        if (!Settings::ShuffleCows && loc->IsCategory(Category::cCow)) {
            continue;
        }
        // Merchants
        else if (Settings::ShuffleMerchants.Is(SHUFFLEMERCHANTS_OFF) && loc->IsCategory(Category::cMerchant)) {
            continue;
        }
        // Adult Trade
        else if (!Settings::ShuffleAdultTradeQuest && loc->IsCategory(Category::cAdultTrade)) {
            continue;
        }
        // Chest Minigame
        else if (Settings::ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_OFF) &&
                 loc->IsCategory(Category::cChestMinigame)) {
            continue;
        }
        // Gerudo Fortress
        else if ((Settings::GerudoFortress.Is(GERUDOFORTRESS_OPEN) &&
                  (loc->IsCategory(Category::cVanillaGFSmallKey) || loc->GetHintKey() == RHT_GF_GERUDO_MEMBERSHIP_CARD)) ||
                 (Settings::GerudoFortress.Is(GERUDOFORTRESS_FAST) && loc->IsCategory(Category::cVanillaGFSmallKey) &&
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
            if (Settings::Shopsanity.Is(SHOPSANITY_OFF)) {
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
                 ((Settings::Tokensanity.Is(TOKENSANITY_OFF)) ||
                  (Settings::Tokensanity.Is(TOKENSANITY_DUNGEONS) && !loc->IsDungeon()) ||
                  (Settings::Tokensanity.Is(TOKENSANITY_OVERWORLD) && loc->IsDungeon()))) {
            spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_ALWAYS;
        }
        // Deku Scrubs
        else if (loc->IsCategory(Category::cDekuScrub) && !loc->IsCategory(Category::cDekuScrubUpgrades) &&
                 Settings::Scrubsanity.Is(SCRUBSANITY_OFF)) {
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

    if (Settings::IngameSpoilers) {
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
            printf("%sError!%s ", YELLOW, WHITE);
        }
    }
}

// Writes the location to the specified node.
static void WriteLocation(
    std::string sphere, const RandomizerCheck locationKey, const bool withPadding = false) {
  Rando::Location* location = StaticData::Location(locationKey);
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
  int16_t originalBlueWarp = entrance->GetBlueWarp();
  int16_t replacementBlueWarp = -1;
  int16_t replacementIndex = entrance->GetReplacement()->GetIndex();
  int16_t replacementDestinationIndex = -1;
  std::string name = entrance->GetName();
  std::string text = entrance->GetConnectedRegion()->regionName + " from " + entrance->GetReplacement()->GetParentRegion()->regionName;

  if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
    destinationIndex = entrance->GetReverse()->GetIndex();
    replacementDestinationIndex = entrance->GetReplacement()->GetReverse()->GetIndex();
    replacementBlueWarp = entrance->GetReplacement()->GetReverse()->GetBlueWarp();
  }

  json entranceJson = json::object({
    {"index", originalIndex},
    {"destination", destinationIndex},
    {"blueWarp", originalBlueWarp},
    {"override", replacementIndex},
    {"overrideDestination", replacementDestinationIndex},
  });

  jsonData["entrances"].push_back(entranceJson);

  // When decoupled entrances is off, handle saving reverse entrances with blue warps
  if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
    json reverseEntranceJson = json::object({
      {"index", replacementDestinationIndex},
      {"destination", replacementIndex},
      {"blueWarp", replacementBlueWarp},
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

  std::vector<Menu*> allMenus = Settings::GetAllOptionMenus();

  for (const Menu* menu : allMenus) {
    if (menu->name == "Item Usability Settings" ||
        menu->name == "Multiplayer Settings") continue;

    if (menu->name == "Timesaver Settings") {
      for (const Option* setting : *menu->settingsList) {
        if (setting->GetName() == "Big Poe Target Count" ||
            setting->GetName() == "Cuccos to return" ||
            setting->GetName() == "Skip Epona Race" ||
            setting->GetName() == "Skip Tower Escape" ||
            setting->GetName() == "Skip Child Stealth" ||
            setting->GetName() == "Complete Mask Quest" ||
            setting->GetName() == "Skip Scarecrow's Song" ||
            setting->GetName() == "Enable Glitch-Useful Cutscenes") {
            std::string settingName = menu->name + ":" + setting->GetName();
            jsonData["settings"][settingName] = setting->GetSelectedOptionText();
        }
      }
      continue;
    }

    //This is a menu of settings, write them
    if (menu->mode == OPTION_SUB_MENU && menu->printInSpoiler) {
      for (const Option* setting : *menu->settingsList) {
        std::string settingName = menu->name + ":" + setting->GetName();
        jsonData["settings"][settingName] = setting->GetSelectedOptionText();
      }


      // for (const Option* setting : *menu->settingsList) {
      //   if (printAll || (!setting->IsHidden() && setting->IsCategory(OptionCategory::Setting))) {
      //     auto node = parentNode->InsertNewChildElement("setting");
      //     node->SetAttribute("name", RemoveLineBreaks(setting->GetName()).c_str());
      //     node->SetText(setting->GetSelectedOptionText().c_str());
      //   }
      // }
    }
  }

  // 3drando doesn't have a "skip child zelda" setting, manually add it to the spoilerfile
  jsonData["settings"]["Skip Child Zelda"] = Settings::skipChildZelda;

  // 3drando uses an MQ dungeon count of 13 to mean random, manually add that to the spoilerfile as a bool
  if (Settings::MQDungeonCount.GetSelectedOptionIndex() == 0) {
    jsonData["settings"]["World Settings:MQ Dungeons"] = "None";
  } else if (Settings::MQDungeonCount.GetSelectedOptionIndex() == 13) {
    jsonData["settings"]["World Settings:MQ Dungeons"] = "Random Number";
  } else {
    jsonData["settings"]["World Settings:MQ Dungeons"] = "Set Number";
  }

  // spoilerLog.RootElement()->InsertEndChild(parentNode);

  //     for (const uint32_t key : allLocations) {
  //       ItemLocation* location = Location(key);
  //       settingsJsonData["locations"][location->GetName()] = location->GetPlacedItemName().english;
  //   }
}

// Writes the excluded locations to the spoiler log, if there are any.
static void WriteExcludedLocations() {
  // auto parentNode = spoilerLog.NewElement("excluded-locations");

  for (size_t i = 1; i < Settings::excludeLocationsOptionsVector.size(); i++) {
    for (const auto& location : Settings::excludeLocationsOptionsVector[i]) {
      if (location->GetSelectedOptionIndex() == INCLUDE) {
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
  std::vector<std::vector<Option *>*> startingInventoryOptions = {
    &Settings::startingItemsOptions,
    &Settings::startingSongsOptions,
    &Settings::startingEquipmentOptions,
    &Settings::startingStonesMedallionsOptions,
    &Settings::startingOthersOptions
  };

  for (std::vector<Option*>* menu : startingInventoryOptions) {
      for (size_t i = 0; i < menu->size(); ++i) {
          const auto setting = menu->at(i);
          // Starting Songs
          if (setting->GetName() == "Start with Zelda's Lullaby" || 
              setting->GetName() == "Start with Epona's Song" ||
              setting->GetName() == "Start with Saria's Song" || 
              setting->GetName() == "Start with Sun's Song" ||
              setting->GetName() == "Start with Song of Time" || 
              setting->GetName() == "Start with Song of Storms" ||
              setting->GetName() == "Start with Minuet of Forest" || 
              setting->GetName() == "Start with Bolero of Fire" ||
              setting->GetName() == "Start with Serenade of Water" || 
              setting->GetName() == "Start with Requiem of Spirit" ||
              setting->GetName() == "Start with Nocturne of Shadow" || 
              setting->GetName() == "Start with Prelude of Light") {
              jsonData["settings"][setting->GetName()] = setting->GetSelectedOptionText();
          }
      }
  }
  for (std::vector<Option *>* menu : startingInventoryOptions) {
    for (size_t i = 0; i < menu->size(); ++i) {
      const auto setting = menu->at(i);
   
      // we need to write these every time because we're not clearing jsondata, so
      // the default logic of only writing it when we aren't using the default value
      // doesn't work, and because it'd be bad to set every single possible starting
      // inventory item as "false" in the json, we're just going to check
      // to see if the name is one of the 3 we're using rn
      if (setting->GetName() == "Start with Consumables" ||
          setting->GetName() == "Start with Max Rupees" ||
          setting->GetName() == "Gold Skulltula Tokens" ||
          setting->GetName() == "Start with Fairy Ocarina" ||
          setting->GetName() == "Start with Kokiri Sword" ||
          setting->GetName() == "Start with Deku Shield") {
        jsonData["settings"][setting->GetName()] = setting->GetSelectedOptionText();
      }
    }
  }
}

// Writes the enabled tricks to the spoiler log, if there are any.
static void WriteEnabledTricks(tinyxml2::XMLDocument& spoilerLog) {
  //auto parentNode = spoilerLog.NewElement("enabled-tricks");

  for (const auto& setting : Settings::trickOptions) {
    if (setting->GetSelectedOptionIndex() != TRICK_ENABLED/* || !setting->IsCategory(OptionCategory::Setting)*/) {
      continue;
    }
    jsonData["enabledTricks"].push_back(RemoveLineBreaks(RandomizerTricks::GetRTName((RandomizerTrick)std::stoi(setting->GetName()))).c_str());
    //auto node = parentNode->InsertNewChildElement("trick");
    //node->SetAttribute("name", RemoveLineBreaks(setting->GetName()).c_str());
  }

  // if (!parentNode->NoChildren()) {
  //  spoilerLog.RootElement()->InsertEndChild(parentNode);
  //}
}

// Writes the enabled glitches to the spoiler log, if there are any.
static void WriteEnabledGlitches(tinyxml2::XMLDocument& spoilerLog) {
  auto parentNode = spoilerLog.NewElement("enabled-glitches");

  for (const auto& setting : Settings::glitchCategories) {
    if (setting->Value<uint8_t>() == 0) {
      continue;
    }

    auto node = parentNode->InsertNewChildElement("glitch-category");
    node->SetAttribute("name", setting->GetName().c_str());
    node->SetText(setting->GetSelectedOptionText().c_str());
  }

  for (const auto& setting : Settings::miscGlitches) {
    if (!setting->Value<bool>()) {
      continue;
    }

    auto node = parentNode->InsertNewChildElement("misc-glitch");
    node->SetAttribute("name", RemoveLineBreaks(setting->GetName()).c_str());
  }

  if (!parentNode->NoChildren()) {
    spoilerLog.RootElement()->InsertEndChild(parentNode);
  }
}

// Writes the Master Quest dungeons to the spoiler log, if there are any.
static void WriteMasterQuestDungeons(tinyxml2::XMLDocument& spoilerLog) {
    for (const auto* dungeon : Dungeon::dungeonList) {
        std::string dungeonName;
        if (dungeon->IsVanilla()) {
            continue;
        }
        jsonData["masterQuestDungeons"].push_back(dungeon->GetName());
    }
}

// Writes the required trials to the spoiler log, if there are any.
static void WriteRequiredTrials() {
    for (const auto& trial : Trial::trialList) {
        if (trial->IsRequired()) {
            std::string trialName;
            switch (gSaveContext.language) {
                case LANGUAGE_FRA:
                    trialName = trial->GetName().GetFrench();
                    break;
                case LANGUAGE_ENG:
                default:
                    trialName = trial->GetName().GetEnglish();
                    break;
            }
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
  for (uint32_t i = 0; i < playthroughEntrances.size(); ++i) {
    auto sphereNum = std::to_string(i);
    std::string sphereString = "sphere ";
    if (i < 10) sphereString += "0";
    sphereString += sphereNum;
    for (Entrance* entrance : playthroughEntrances[i]) {
      WriteShuffledEntrance(sphereString, entrance);
    }
  }
}

// Writes the WOTH locations to the spoiler log, if there are any.
static void WriteWayOfTheHeroLocation(tinyxml2::XMLDocument& spoilerLog) {
    auto parentNode = spoilerLog.NewElement("way-of-the-hero-locations");

    for (const RandomizerCheck key : Rando::Context::GetInstance()->wothLocations) {
        // WriteLocation(parentNode, key, true);
    }

    if (!parentNode->NoChildren()) {
        spoilerLog.RootElement()->InsertEndChild(parentNode);
    }
}

std::string AutoFormatHintTextString(std::string unformattedHintTextString) {
  std::string textStr = unformattedHintTextString;

  // RANDOTODO: don't just make manual exceptions
  bool needsAutomaicNewlines = true;
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
    needsAutomaicNewlines = false;
  }

  if (needsAutomaicNewlines) {
    //insert newlines either manually or when encountering a '&'
    constexpr size_t lineLength = 34;
    size_t lastNewline = 0;
    while (lastNewline + lineLength < textStr.length()) {
      size_t carrot     = textStr.find('^', lastNewline);
      size_t ampersand  = textStr.find('&', lastNewline);
      size_t lastSpace  = textStr.rfind(' ', lastNewline + lineLength);
      size_t lastPeriod = textStr.rfind('.', lastNewline + lineLength);
      //replace '&' first if it's within the newline range
      if (ampersand < lastNewline + lineLength) {
        lastNewline = ampersand;
      //or move the lastNewline cursor to the next line if a '^' is encountered
      } else if (carrot < lastNewline + lineLength) {
        lastNewline = carrot + 1;
      //some lines need to be split but don't have spaces, look for periods instead
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

  return textStr;
}

Rando::ItemLocation* GetItemLocation(RandomizerGet item) {
    auto ctx = Rando::Context::GetInstance();
    return ctx->GetItemLocation(FilterFromPool(ctx->allLocations, [item, ctx](const RandomizerCheck loc){return ctx->GetItemLocation(loc)->GetPlacedRandomizerGet() == item;})[0]);
}

// Writes the hints to the spoiler log, if they are enabled.
static void WriteHints(int language) {
    std::string unformattedGanonText;
    std::string unformattedGanonHintText;
    std::string unformattedDampesText;
    std::string unformattedGregText;

    switch (language) {
        case 0:
        default:
            unformattedGanonText = GetGanonText().GetEnglish();
            unformattedGanonHintText = GetGanonHintText().GetEnglish();
            unformattedDampesText = GetDampeHintText().GetEnglish();
            unformattedGregText = GetGregHintText().GetEnglish();
            jsonData["warpMinuetText"] = GetWarpMinuetText().GetEnglish();
            jsonData["warpBoleroText"] = GetWarpBoleroText().GetEnglish();
            jsonData["warpSerenadeText"] = GetWarpSerenadeText().GetEnglish();
            jsonData["warpRequiemText"] = GetWarpRequiemText().GetEnglish();
            jsonData["warpNocturneText"] = GetWarpNocturneText().GetEnglish();
            jsonData["warpPreludeText"] = GetWarpPreludeText().GetEnglish();
            jsonData["childAltar"]["hintText"] = GetChildAltarText().GetEnglish();
            jsonData["adultAltar"]["hintText"] = GetAdultAltarText().GetEnglish();
            break;
        case 2:
            unformattedGanonText = GetGanonText().GetFrench();
            unformattedGanonHintText = GetGanonHintText().GetFrench();
            unformattedDampesText = GetDampeHintText().GetFrench();
            unformattedGregText = GetGregHintText().GetFrench();
            jsonData["warpMinuetText"] = GetWarpMinuetText().GetFrench();
            jsonData["warpBoleroText"] = GetWarpBoleroText().GetFrench();
            jsonData["warpSerenadeText"] = GetWarpSerenadeText().GetFrench();
            jsonData["warpRequiemText"] = GetWarpRequiemText().GetFrench();
            jsonData["warpNocturneText"] = GetWarpNocturneText().GetFrench();
            jsonData["warpPreludeText"] = GetWarpPreludeText().GetFrench();
            jsonData["childAltar"]["hintText"] = GetChildAltarText().GetFrench();
            jsonData["adultAltar"]["hintText"] = GetAdultAltarText().GetFrench();
            break;
    }

    Rando::ItemLocation* emeraldLoc = GetItemLocation(RG_KOKIRI_EMERALD);
    Rando::ItemLocation* rubyLoc = GetItemLocation(RG_GORON_RUBY);
    Rando::ItemLocation* sapphireLoc = GetItemLocation(RG_ZORA_SAPPHIRE);
    std::string emeraldArea;
    std::string erubyArea;
    std::string sapphireArea;

    jsonData["childAltar"]["rewards"]["emeraldLoc"] = StaticData::Location(emeraldLoc->GetRandomizerCheck())->GetName();
    jsonData["childAltar"]["rewards"]["rubyLoc"] = StaticData::Location(rubyLoc->GetRandomizerCheck())->GetName();
    jsonData["childAltar"]["rewards"]["sapphireLoc"] =
        StaticData::Location(sapphireLoc->GetRandomizerCheck())->GetName();

    Rando::ItemLocation* forestMedallionLoc = GetItemLocation(RG_FOREST_MEDALLION);
    Rando::ItemLocation* fireMedallionLoc = GetItemLocation(RG_FIRE_MEDALLION);
    Rando::ItemLocation* waterMedallionLoc = GetItemLocation(RG_WATER_MEDALLION);
    Rando::ItemLocation* shadowMedallionLoc = GetItemLocation(RG_SHADOW_MEDALLION);
    Rando::ItemLocation* spiritMedallionLoc = GetItemLocation(RG_SPIRIT_MEDALLION);
    Rando::ItemLocation* lightMedallionLoc = GetItemLocation(RG_LIGHT_MEDALLION);

    jsonData["adultAltar"]["rewards"]["forestMedallionLoc"] =
        StaticData::Location(forestMedallionLoc->GetRandomizerCheck())->GetName();
    jsonData["adultAltar"]["rewards"]["fireMedallionLoc"] =
        StaticData::Location(fireMedallionLoc->GetRandomizerCheck())->GetName();
    jsonData["adultAltar"]["rewards"]["waterMedallionLoc"] =
        StaticData::Location(waterMedallionLoc->GetRandomizerCheck())->GetName();
    jsonData["adultAltar"]["rewards"]["shadowMedallionLoc"] =
        StaticData::Location(shadowMedallionLoc->GetRandomizerCheck())->GetName();
    jsonData["adultAltar"]["rewards"]["spiritMedallionLoc"] =
        StaticData::Location(spiritMedallionLoc->GetRandomizerCheck())->GetName();
    jsonData["adultAltar"]["rewards"]["lightMedallionLoc"] =
        StaticData::Location(lightMedallionLoc->GetRandomizerCheck())->GetName();

    std::string ganonText = AutoFormatHintTextString(unformattedGanonText);
    std::string ganonHintText = AutoFormatHintTextString(unformattedGanonHintText);
    std::string dampesText = AutoFormatHintTextString(unformattedDampesText);
    std::string gregText = AutoFormatHintTextString(unformattedGregText);

    jsonData["ganonText"] = ganonText;
    jsonData["ganonHintText"] = ganonHintText;
    jsonData["ganonHintLoc"] = GetGanonHintLoc();
    jsonData["dampeText"] = dampesText;
    jsonData["dampeHintLoc"] = GetDampeHintLoc();
    jsonData["gregText"] = gregText;
    jsonData["gregLoc"] = StaticData::Location(GetItemLocation(RG_GREG_RUPEE)->GetRandomizerCheck())->GetName();

    if (Settings::GossipStoneHints.Is(HINTS_NO_HINTS)) {
        return;
    }

    //TODO: Hint Refactor
    // for (const uint32_t key : gossipStoneLocations) {
    //     ItemLocation* location = Location(key);
    //     ItemLocation* hintedLocation = Location(location->GetHintedLocation());
    //     std::string unformattedHintTextString;
    //     switch (language) {
    //         case 0:
    //         default:
    //             unformattedHintTextString = location->GetPlacedItemName().GetEnglish();
    //             break;
    //         case 2:
    //             unformattedHintTextString = location->GetPlacedItemName().GetFrench();
    //             break;
    //     }

    //     HintType hintType = location->GetHintType();

    //     std::string textStr = AutoFormatHintTextString(unformattedHintTextString);
    //     jsonData["hints"][location->GetName()]["hint"] = textStr;
    //     jsonData["hints"][location->GetName()]["type"] = hintTypeNames.find(hintType)->second;
    //     if (hintType == HINT_TYPE_ITEM || hintType == HINT_TYPE_NAMED_ITEM || hintType == HINT_TYPE_WOTH) {
    //         jsonData["hints"][location->GetName()]["item"] = hintedLocation->GetPlacedItemName().GetEnglish();
    //         if (hintType != HINT_TYPE_NAMED_ITEM || hintType == HINT_TYPE_WOTH) {
    //             jsonData["hints"][location->GetName()]["location"] = hintedLocation->GetName();
    //         }
    //     }
    //     if (hintType != HINT_TYPE_TRIAL && hintType != HINT_TYPE_JUNK) {
    //         jsonData["hints"][location->GetName()]["area"] = location->GetHintedRegion();
    //     }
    // }
}

static void WriteAllLocations(int language) {
    auto ctx = Rando::Context::GetInstance();
    for (const RandomizerCheck key : ctx->allLocations) {
        Rando::ItemLocation* location = ctx->GetItemLocation(key);
        std::string placedItemName;

        switch (language) {
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
        if (!location->HasScrubsanityPrice() &&
            !location->HasShopsanityPrice() &&
            location->GetPlacedRandomizerGet() != RG_ICE_TRAP) {
            
            jsonData["locations"][StaticData::Location(location->GetRandomizerCheck())->GetName()] = placedItemName;
            continue;
        }

        // We're dealing with a complex item, build out the json object for it
        jsonData["locations"][StaticData::Location(location->GetRandomizerCheck())->GetName()]["item"] = placedItemName;

        if (location->HasScrubsanityPrice() || location->HasShopsanityPrice()) {
            jsonData["locations"][StaticData::Location(location->GetRandomizerCheck())->GetName()]["price"] =
                location->GetPrice();
        }
        if (location->IsHintedAt()) {
          hintedLocations.emplace(StaticData::Location(key)->GetHintKey(), location);
        }

        if (location->GetPlacedRandomizerGet() == RG_ICE_TRAP) {
          switch (language) {
              case 0:
              default:
                  jsonData["locations"][StaticData::Location(location->GetRandomizerCheck())->GetName()]["model"] =
                      StaticData::ItemFromGIID(ctx->iceTrapModels[location->GetRandomizerCheck()]).GetName().english;
                  jsonData["locations"][StaticData::Location(location->GetRandomizerCheck())->GetName()]["trickName"] = 
                      GetIceTrapName(ctx->iceTrapModels[location->GetRandomizerCheck()]).english;
                  break;
              case 2:
                  jsonData["locations"][StaticData::Location(location->GetRandomizerCheck())->GetName()]["model"] =
                      StaticData::ItemFromGIID(ctx->iceTrapModels[location->GetRandomizerCheck()]).GetName().french;
                  jsonData["locations"][StaticData::Location(location->GetRandomizerCheck())->GetName()]["trickName"] =
                      GetIceTrapName(ctx->iceTrapModels[location->GetRandomizerCheck()]).french;
                  break;
          }
      }
    }
}

//static void WriteHintData(int language) {
//    for (auto [hintKey, item_location] : hintedLocations) {
//        ItemLocation *hint_location = Location(hintKey);
//        jsonData["hints"][hint_location->GetName()] = { { "text", hint_location->GetPlacedItemName().GetEnglish() },
//                                                        { "item", item_location->GetPlacedItemName().GetEnglish() },
//                                                        { "itemLocation", item_location->GetName() },
//                                                        { "locationArea", item_location->GetParentRegionKey() } };
//    }
//}

const char* SpoilerLog_Write(int language) {
    auto spoilerLog = tinyxml2::XMLDocument(false);
    spoilerLog.InsertEndChild(spoilerLog.NewDeclaration());

    auto rootNode = spoilerLog.NewElement("spoiler-log");
    spoilerLog.InsertEndChild(rootNode);

    jsonData.clear();

    jsonData["version"] = (char*) gBuildVersion;
    jsonData["seed"] = Settings::seedString;

    // Write Hash
    int index = 0;
    for (uint8_t seed_value : Settings::hashIconIndexes) {
        jsonData["file_hash"][index] = seed_value;
        index++;
    }

    WriteSettings();
    WriteExcludedLocations();
    WriteStartingInventory();
    WriteEnabledTricks(spoilerLog);
    //if (Settings::Logic.Is(LOGIC_GLITCHED)) {
    //    WriteEnabledGlitches(spoilerLog);
    //}
    WriteMasterQuestDungeons(spoilerLog);
    WriteRequiredTrials();
    WritePlaythrough();
    //WriteWayOfTheHeroLocation(spoilerLog);

    auto ctx = Rando::Context::GetInstance();
    ctx->playthroughLocations.clear();
    ctx->playthroughBeatable = false;
    ctx->wothLocations.clear();

    WriteHints(language);
    WriteShuffledEntrances();
    WriteAllLocations(language);
    //WriteHintData(language);

    if (!std::filesystem::exists(LUS::Context::GetPathRelativeToAppDirectory("Randomizer"))) {
        std::filesystem::create_directory(LUS::Context::GetPathRelativeToAppDirectory("Randomizer"));
    }

    std::string jsonString = jsonData.dump(4);
    std::ostringstream fileNameStream;
    for (int i = 0; i < Settings::hashIconIndexes.size(); i ++) {
        if (i) {
            fileNameStream << '-';
        }
        if (Settings::hashIconIndexes[i] < 10) {
            fileNameStream << '0';
        }
        fileNameStream << std::to_string(Settings::hashIconIndexes[i]);
    }
    std::string fileName = fileNameStream.str();
    std::ofstream jsonFile(LUS::Context::GetPathRelativeToAppDirectory(
        (std::string("Randomizer/") + fileName + std::string(".json")).c_str()));
    jsonFile << std::setw(4) << jsonString << std::endl;
    jsonFile.close();

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

    rootNode->SetAttribute("version", Settings::version.c_str());
    rootNode->SetAttribute("seed", Settings::seed);

    // WriteSettings(placementLog, true); // Include hidden settings.
    // WriteExcludedLocations(placementLog);
    // WriteStartingInventory(placementLog);
    WriteEnabledTricks(placementLog);
    WriteEnabledGlitches(placementLog);
    WriteMasterQuestDungeons(placementLog);
    //WriteRequiredTrials(placementLog);

    placementtxt = "\n" + placementtxt;

    auto node = rootNode->InsertNewChildElement("log");
    auto contentNode = node->InsertNewText(placementtxt.c_str());
    contentNode->SetCData(true);

    return true;
}

#include "spoiler_log.hpp"

#include "dungeon.hpp"
#include "item_list.hpp"
#include "item_location.hpp"
#include "entrance.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "trial.hpp"
#include "tinyxml2.h"
#include "utils.hpp"
#include "shops.hpp"
#include "hints.hpp"
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

#include <libultraship/Window.h>

using json = nlohmann::json;

json jsonData;

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
    itemLocationsMap.reserve(allLocations.size());
    std::unordered_map<std::string, uint16_t>
        stringOffsetMap; // Map of strings to their offset into spoiler string data array
    stringOffsetMap.reserve(allLocations.size() * 2);

    // Sort all locations by their group, so the in-game log can show a group of items by simply starting/ending at
    // certain indices
    std::stable_sort(allLocations.begin(), allLocations.end(), [](const uint32_t& a, const uint32_t& b) {
        auto groupA = Location(a)->GetCollectionCheckGroup();
        auto groupB = Location(b)->GetCollectionCheckGroup();
        return groupA < groupB;
    });

    for (const uint32_t key : allLocations) {
        auto loc = Location(key);

        // Hide excluded locations from ingame tracker
        if (loc->IsExcluded()) {
            continue;
        }
        // Cows
        else if (!Settings::ShuffleCows && loc->IsCategory(Category::cCow)) {
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
                  (loc->IsCategory(Category::cVanillaGFSmallKey) || loc->GetHintKey() == GF_GERUDO_MEMBERSHIP_CARD)) ||
                 (Settings::GerudoFortress.Is(GERUDOFORTRESS_FAST) && loc->IsCategory(Category::cVanillaGFSmallKey) &&
                  loc->GetHintKey() != GF_NORTH_F1_CARPENTER)) {
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
        auto locItem = loc->GetPlacedItemName().GetEnglish();
        if (loc->GetPlacedItemKey() == ICE_TRAP && loc->IsCategory(Category::cShop)) {
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
        if (key == GANON) {
            spoilerData.ItemLocations[spoilerItemIndex].CollectType = COLLECTTYPE_NEVER;
            spoilerData.ItemLocations[spoilerItemIndex].RevealType = REVEALTYPE_ALWAYS;
        } else if (key == MARKET_BOMBCHU_BOWLING_BOMBCHUS) {
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
            if (loc->GetPlacedItem().GetItemType() == ITEMTYPE_REFILL ||
                loc->GetPlacedItem().GetItemType() == ITEMTYPE_SHOP ||
                loc->GetPlacedItem().GetHintKey() == PROGRESSIVE_BOMBCHUS) {
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
            for (uint32_t i = 0; i < playthroughLocations.size(); i++) {
                if (i >= SPOILER_SPHERES_MAX) {
                    spoilerOutOfSpace = true;
                    break;
                }
                spoilerData.Spheres[i].ItemLocationsOffset = spoilerSphereItemoffset;
                for (uint32_t loc = 0; loc < playthroughLocations[i].size(); ++loc) {
                    if (spoilerSphereItemoffset >= SPOILER_ITEMS_MAX) {
                        spoilerOutOfSpace = true;
                        break;
                    }

                    const auto foundItemLoc = itemLocationsMap.find(playthroughLocations[i][loc]);
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
    std::string sphere, const uint32_t locationKey, const bool withPadding = false) {
  ItemLocation* location = Location(locationKey);

  // auto node = parentNode->InsertNewChildElement("location");
  switch (gSaveContext.language) {
        case LANGUAGE_ENG:
        default:
            jsonData["playthrough"][sphere][location->GetName()] = location->GetPlacedItemName().GetEnglish();
            break;
        case LANGUAGE_FRA:
            jsonData["playthrough"][sphere][location->GetName()] = location->GetPlacedItemName().GetFrench();
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
static void WriteShuffledEntrance(
  tinyxml2::XMLElement* parentNode,
  Entrance* entrance,
  const bool withPadding = false
) {
  auto node = parentNode->InsertNewChildElement("entrance");
  node->SetAttribute("name", entrance->GetName().c_str());
  auto text = entrance->GetConnectedRegion()->regionName + " from " + entrance->GetReplacement()->GetParentRegion()->regionName;
  node->SetText(text.c_str());

  if (withPadding) {
    constexpr int16_t LONGEST_NAME = 56; //The longest name of a vanilla entrance

    //Insert padding so we get a kind of table in the XML document
    int16_t requiredPadding = LONGEST_NAME - entrance->GetName().length();
    if (requiredPadding > 0) {
      std::string padding(requiredPadding, ' ');
      node->SetAttribute("_", padding.c_str());
    }
  }
}

// Writes the settings (without excluded locations, starting inventory and tricks) to the spoilerLog document.
static void WriteSettings(const bool printAll = false) {
  // auto parentNode = spoilerLog.NewElement("settings");

  std::vector<Menu*> allMenus = Settings::GetAllOptionMenus();

  for (const Menu* menu : allMenus) {
    if (menu->name == "Cosmetic Settings" ||
        menu->name == "Ingame Defaults" ||
        menu->name == "Item Usability Settings" ||
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
  auto parentNode = spoilerLog.NewElement("enabled-tricks");

  for (const auto& setting : Settings::trickOptions) {
    if (setting->GetSelectedOptionIndex() != TRICK_ENABLED || !setting->IsCategory(OptionCategory::Setting)) {
      continue;
    }

    auto node = parentNode->InsertNewChildElement("trick");
    node->SetAttribute("name", RemoveLineBreaks(setting->GetName()).c_str());
  }

  if (!parentNode->NoChildren()) {
    spoilerLog.RootElement()->InsertEndChild(parentNode);
  }
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

  for (uint32_t i = 0; i < playthroughLocations.size(); ++i) {
    auto sphereNum = std::to_string(i);
    std::string sphereString =  "sphere ";
    if (sphereNum.length() == 1) sphereString += "0";
    sphereString += sphereNum;
    for (const uint32_t key : playthroughLocations[i]) {
      WriteLocation(sphereString, key, true);
    }
  }

  // spoilerLog.RootElement()->InsertEndChild(playthroughNode);
}

//Write the randomized entrance playthrough to the spoiler log, if applicable
static void WriteShuffledEntrances(tinyxml2::XMLDocument& spoilerLog) {
    if (!Settings::ShuffleEntrances || noRandomEntrances) {
        return;
    }

    auto playthroughNode = spoilerLog.NewElement("entrance-playthrough");

    for (uint32_t i = 0; i < playthroughEntrances.size(); ++i) {
        auto sphereNode = playthroughNode->InsertNewChildElement("sphere");
        sphereNode->SetAttribute("level", i + 1);

        for (Entrance* entrance : playthroughEntrances[i]) {
            WriteShuffledEntrance(sphereNode, entrance, true);
        }
    }

    spoilerLog.RootElement()->InsertEndChild(playthroughNode);
}

// Writes the WOTH locations to the spoiler log, if there are any.
static void WriteWayOfTheHeroLocation(tinyxml2::XMLDocument& spoilerLog) {
    auto parentNode = spoilerLog.NewElement("way-of-the-hero-locations");

    for (const uint32_t key : wothLocations) {
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
      textStr == "Ah, c'est @.&J'attendais un certain Sheik.&Tu sais ce qui lui est arrivé?") {
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

// Writes the hints to the spoiler log, if they are enabled.
static void WriteHints(int language) {
    std::string unformattedGanonText;
    std::string unformattedGanonHintText;

    switch (language) {
        case 0:
        default:
            unformattedGanonText = GetGanonText().GetEnglish();
            unformattedGanonHintText = GetGanonHintText().GetEnglish();
            jsonData["childAltarText"] = GetChildAltarText().GetEnglish();
            jsonData["adultAltarText"] = GetAdultAltarText().GetEnglish();
            break;
        case 2:
            unformattedGanonText = GetGanonText().GetFrench();
            unformattedGanonHintText = GetGanonHintText().GetFrench();
            jsonData["childAltarText"] = GetChildAltarText().GetFrench();
            jsonData["adultAltarText"] = GetAdultAltarText().GetFrench();
            break;
    }

    std::string ganonText = AutoFormatHintTextString(unformattedGanonText);
    std::string ganonHintText = AutoFormatHintTextString(unformattedGanonHintText);

    jsonData["ganonText"] = ganonText;
    jsonData["ganonHintText"] = ganonHintText;

    if (Settings::GossipStoneHints.Is(HINTS_NO_HINTS)) {
        return;
    }

    for (const uint32_t key : gossipStoneLocations) {
        ItemLocation* location = Location(key);
        std::string unformattedHintTextString;
        switch (language) {
            case 0:
            default:
                unformattedHintTextString = location->GetPlacedItemName().GetEnglish();
                break;
            case 2:
                unformattedHintTextString = location->GetPlacedItemName().GetFrench();
                break;
        }

        std::string textStr = AutoFormatHintTextString(unformattedHintTextString);
        jsonData["hints"][location->GetName()] = textStr;
    }
}

static void WriteAllLocations(int language) {
    for (const uint32_t key : allLocations) {
        ItemLocation* location = Location(key);
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

        // Eventually check for other things here like fake name
        if (location->HasScrubsanityPrice() || location->HasShopsanityPrice()) {
            jsonData["locations"][location->GetName()]["item"] = placedItemName;
            if (location->GetPlacedItemKey() == ICE_TRAP && location->IsCategory(Category::cShop)) {
                switch (language) {
                    case 0:
                    default:
                        jsonData["locations"][location->GetName()]["model"] =
                            ItemFromGIID(iceTrapModels[location->GetRandomizerCheck()]).GetName().english;
                        jsonData["locations"][location->GetName()]["trickName"] = 
                            NonShopItems[TransformShopIndex(GetShopIndex(key))].Name.english;
                        break;
                    case 2:
                        jsonData["locations"][location->GetName()]["model"] =
                            ItemFromGIID(iceTrapModels[location->GetRandomizerCheck()]).GetName().french;
                        jsonData["locations"][location->GetName()]["trickName"] =
                            NonShopItems[TransformShopIndex(GetShopIndex(key))].Name.french;
                        break;
                }
            }
            jsonData["locations"][location->GetName()]["price"] = location->GetPrice();
        } else if (location->GetPlacedItemKey() == ICE_TRAP && iceTrapModels.contains(location->GetRandomizerCheck())) {
            jsonData["locations"][location->GetName()]["item"] = placedItemName;
            switch (language) {
                case 0:
                default:
                    jsonData["locations"][location->GetName()]["model"] =
                        ItemFromGIID(iceTrapModels[location->GetRandomizerCheck()]).GetName().english;
                    break;
                case 2:
                    jsonData["locations"][location->GetName()]["model"] =
                        ItemFromGIID(iceTrapModels[location->GetRandomizerCheck()]).GetName().french;
                    break;
            }
        } else {
          jsonData["locations"][location->GetName()] = placedItemName;
        }
    }
}

const char* SpoilerLog_Write(int language) {
    auto spoilerLog = tinyxml2::XMLDocument(false);
    spoilerLog.InsertEndChild(spoilerLog.NewDeclaration());

    auto rootNode = spoilerLog.NewElement("spoiler-log");
    spoilerLog.InsertEndChild(rootNode);

    rootNode->SetAttribute("version", Settings::version.c_str());
    rootNode->SetAttribute("seed", Settings::seed.c_str());

    jsonData.clear();

    // Write Hash
    int index = 0;
    for (uint8_t seed_value : Settings::hashIconIndexes) {
        jsonData["file_hash"][index] = seed_value;
        index++;
    }

    WriteSettings();
    WriteExcludedLocations();
    WriteStartingInventory();
    //WriteEnabledTricks(spoilerLog);
    //if (Settings::Logic.Is(LOGIC_GLITCHED)) {
    //    WriteEnabledGlitches(spoilerLog);
    //}
    WriteMasterQuestDungeons(spoilerLog);
    WriteRequiredTrials();
    WritePlaythrough();
    //WriteWayOfTheHeroLocation(spoilerLog);

    playthroughLocations.clear();
    playthroughBeatable = false;
    wothLocations.clear();

    WriteHints(language);
    //WriteShuffledEntrances(spoilerLog);
    WriteAllLocations(language);

    if (!std::filesystem::exists(Ship::Window::GetPathRelativeToAppDirectory("Randomizer"))) {
        std::filesystem::create_directory(Ship::Window::GetPathRelativeToAppDirectory("Randomizer"));
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
    std::ofstream jsonFile(Ship::Window::GetPathRelativeToAppDirectory(
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
    rootNode->SetAttribute("seed", Settings::seed.c_str());

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

#include "preset.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

#include "category.hpp"
#include "settings.hpp"
#include "tinyxml2.h"
#include "utils.hpp"

namespace fs = std::filesystem;

static const std::string CACHED_SETTINGS_FILENAME = "CACHED_SETTINGS";
static const std::string CACHED_COSMETICS_FILENAME = "CACHED_COSMETICS";

static std::string_view GetBasePath(OptionCategory category) {
  static constexpr std::array<std::string_view, 2> paths{
    "/3ds/presets/oot3dr/settings/",
    "/3ds/presets/oot3dr/cosmetics/",
  };

  switch(category) {
    case OptionCategory::Setting :
    case OptionCategory::Cosmetic :
      return paths[static_cast<size_t>(category)];
    case OptionCategory::Toggle :
      break;
  }
  return "";
}

//Creates preset directories if they don't exist
bool CreatePresetDirectories() {
  //Create the 3ds directory if it doesn't exist
  std::filesystem::create_directory("./3ds");
  //Create the presets directory if it doesn't exist
  std::filesystem::create_directory("./3ds/presets");
  //Create the oot3d directory if it doesn't exist
  std::filesystem::create_directory("./3ds/presets/oot3dr");
  //Create the cosmetics directory if it doesn't exist
  std::filesystem::create_directory("./3ds/presets/oot3dr/cosmetics");
  //Create the settings directory if it doesn't exist
  std::filesystem::create_directory("./3ds/presets/oot3dr/settings");

  return true;
}

//Gets the preset filenames
std::vector<std::string> GetSettingsPresets() {
  std::vector<std::string> presetEntries = {};
  for (const auto& entry : fs::directory_iterator(GetBasePath(OptionCategory::Setting))) {
    if(entry.is_regular_file() && entry.path().stem().string() != CACHED_SETTINGS_FILENAME) {
      presetEntries.push_back(entry.path().stem().string());
    }
  }
  return presetEntries;
}

static std::string PresetPath(std::string_view presetName, OptionCategory category) {
  return std::string(GetBasePath(category)).append(presetName).append(".xml");
}

// Presets are now saved as XML files using the tinyxml2 library.
// Documentation: https://leethomason.github.io/tinyxml2/index.html
bool SavePreset(std::string_view presetName, OptionCategory category) {
  using namespace tinyxml2;

  XMLDocument preset = XMLDocument(false);

  // Create and insert the XML declaration
  preset.InsertEndChild(preset.NewDeclaration());

  // Create the root node
  XMLElement* rootNode = preset.NewElement("settings");
  preset.InsertEndChild(rootNode);

  for (Menu* menu : Settings::GetAllOptionMenus()) {
    if (menu->mode != OPTION_SUB_MENU) {
      continue;
    }
    for (const Option* setting : *menu->settingsList) {
      if (!setting->IsCategory(category)) {
        continue;
      }

      XMLElement* newSetting = rootNode->InsertNewChildElement("setting");
      newSetting->SetAttribute("name", RemoveLineBreaks(setting->GetName()).c_str());
      newSetting->SetText(setting->GetSelectedOptionText().c_str());
    }
  }

  XMLError e = preset.SaveFile(PresetPath(presetName, category).c_str());
  return e == XML_SUCCESS;
}

//Read the preset XML file
bool LoadPreset(std::string_view presetName, OptionCategory category) {
  using namespace tinyxml2;

  XMLDocument preset;
  XMLError e = preset.LoadFile(PresetPath(presetName, category).c_str());
  if (e != XML_SUCCESS) {
    return false;
  }

  XMLElement* rootNode = preset.RootElement();
  if (strcmp(rootNode->Name(), "settings") != 0) {
      // We do not have our <settings> root node, so it may be the old structure. We don't support that one anymore.
      return false;
  }

  XMLElement* curNode = rootNode->FirstChildElement();

  for (Menu* menu : Settings::GetAllOptionMenus()) {
    if (menu->mode != OPTION_SUB_MENU) {
      continue;
    }

    for (Option* setting : *menu->settingsList) {
      if (!setting->IsCategory(category)) {
        continue;
      }

      // Since presets are saved linearly, we can simply loop through the nodes as
      // we loop through the settings to find most of the matching elements.
      const std::string& settingToFind = RemoveLineBreaks(setting->GetName());
      if (settingToFind == RemoveLineBreaks(curNode->Attribute("name"))) {
        setting->SetSelectedIndexByString(curNode->GetText());
        curNode = curNode->NextSiblingElement();
      } else {
        // If the current setting and element don't match, then search
        // linearly from the beginning. This will get us back on track if the
        // next setting and element line up with each other.
        curNode = rootNode->FirstChildElement();
        while (curNode != nullptr) {
          if (settingToFind == RemoveLineBreaks(curNode->Attribute("name"))) {
            setting->SetSelectedIndexByString(curNode->GetText());
            curNode = curNode->NextSiblingElement();
            break;
          }
          curNode = curNode->NextSiblingElement();
        }
      }

      // Reset to the beginning if we reached the end.
      if (curNode == nullptr) {
        curNode = rootNode->FirstChildElement();
      }
    }
  }
  return true;
}

//Delete the selected preset
bool DeletePreset(std::string_view presetName, OptionCategory category) {
  const std::string filepath = PresetPath(presetName, category);

  std::filesystem::remove(filepath);

  return true;
}

//Saves the new preset to a file
bool SaveSpecifiedPreset(std::string_view presetName, OptionCategory category) {
  //don't save if the user cancelled
  if (presetName.empty()) {
    return false;
  }
  return SavePreset(presetName, category);
}

void SaveCachedSettings() {
  SavePreset(CACHED_SETTINGS_FILENAME, OptionCategory::Setting);
}

void LoadCachedSettings() {
  //If cache file exists, load it
  for (const auto& entry : fs::directory_iterator(GetBasePath(OptionCategory::Setting))) {
    if(entry.path().stem().string() == CACHED_SETTINGS_FILENAME) {
      //File exists, open
      LoadPreset(CACHED_SETTINGS_FILENAME, OptionCategory::Setting);
    }
  }
}

bool SaveCachedCosmetics() {
  return SavePreset(CACHED_COSMETICS_FILENAME, OptionCategory::Cosmetic);
}

void LoadCachedCosmetics() {
  //If cache file exists, load it
  for (const auto& entry : fs::directory_iterator(GetBasePath(OptionCategory::Cosmetic))) {
    if(entry.path().stem().string() == CACHED_COSMETICS_FILENAME) {
      //File exists, open
      LoadPreset(CACHED_COSMETICS_FILENAME, OptionCategory::Cosmetic);
    }
  }
}

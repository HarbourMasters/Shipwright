#include "CosmeticsEditor.h"

#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <math.h>
#include <tinyxml2.h>
#include <fast/resource/type/DisplayList.h>
#include <ship/resource/archive/Archive.h>

#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/OTRGlobals.h"

extern "C" {
#include "macros.h"
#include "soh/cvar_prefixes.h"
}

static constexpr const char* CUSTOM_COSMETIC_GROUP = "Custom";
static constexpr const char* CUSTOM_CVAR_PREFIX = "gCosmetics.Custom.";

struct CustomCosmeticBinding {
    std::string materialPath;
    size_t commandIndex = 0;
    bool isPrimColor = true;
    uint8_t defaultA = 255;
    uint8_t primM = 0;
    uint8_t primL = 0;
};

struct CustomCosmeticEntry {
    std::string displayName;
    std::string category;
    std::string colorCvar;
    std::string changedCvar;
    Color_RGBA8 defaultColor = { 255, 255, 255, 255 };
    std::vector<CustomCosmeticBinding> bindings;
};

static std::vector<CustomCosmeticEntry> customCosmeticEntries;

static bool IsCustomArchive(const std::shared_ptr<Ship::Archive>& archive) {
    if (archive == nullptr) {
        return false;
    }

    const auto& archivePath = archive->GetPath();
    return archivePath.find("\\mods\\") != std::string::npos || archivePath.find("/mods/") != std::string::npos;
}

static int GetCustomMaterialSortOrder(const std::string& materialPath) {
    if (materialPath.starts_with("objects/object_link_child/") ||
        materialPath.starts_with("__OTR__objects/object_link_child/")) {
        return 0;
    }
    if (materialPath.starts_with("objects/object_link_boy/") ||
        materialPath.starts_with("__OTR__objects/object_link_boy/")) {
        return 1;
    }

    return 2;
}

static void SanitizeCustomKey(std::string& value) {
    for (auto it = value.begin(); it != value.end();) {
        if (!std::isalnum(static_cast<unsigned char>(*it))) {
            it = value.erase(it);
        } else {
            ++it;
        }
    }

    if (value.empty()) {
        value = "Entry";
    }
}

static bool TryLoadCustomDisplayListXml(Ship::ArchiveManager* archiveManager, Ship::ResourceManager* resourceManager,
                                        const std::string& materialPath, tinyxml2::XMLDocument& document,
                                        std::shared_ptr<Fast::DisplayList>& material, tinyxml2::XMLElement*& root) {
    auto file = archiveManager->LoadFile(materialPath);
    if (file == nullptr || !file->IsLoaded || file->Buffer == nullptr) {
        return false;
    }

    document.Parse(file->Buffer->data(), file->Buffer->size());
    if (document.Error()) {
        return false;
    }

    root = document.FirstChildElement();
    if (root == nullptr || std::string(root->Name()) != "DisplayList") {
        return false;
    }

    material = std::dynamic_pointer_cast<Fast::DisplayList>(resourceManager->LoadResource(materialPath));
    return material != nullptr;
}

static size_t FindDisplayListInstructionIndex(const Fast::DisplayList& displayList, const Gfx& expected,
                                              size_t searchStart) {
    for (size_t i = searchStart; i < displayList.Instructions.size(); i++) {
        const Gfx& current = displayList.Instructions[i];
        if (current.words.w0 == expected.words.w0 && current.words.w1 == expected.words.w1) {
            return i;
        }
    }

    return SIZE_MAX;
}

static Color_RGBA8 GetCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    if (CVarGetInteger(entry.changedCvar.c_str(), 0)) {
        return CVarGetColor(entry.colorCvar.c_str(), entry.defaultColor);
    }

    return entry.defaultColor;
}

static std::string GetCustomCosmeticCvar(const CustomCosmeticEntry& entry) {
    return entry.colorCvar.substr(0, entry.colorCvar.size() - std::string(".Value").size());
}

static std::string GetCustomCosmeticRainbowCvar(const CustomCosmeticEntry& entry) {
    return GetCustomCosmeticCvar(entry) + ".Rainbow";
}

static std::string GetCustomCosmeticLockedCvar(const CustomCosmeticEntry& entry) {
    return GetCustomCosmeticCvar(entry) + ".Locked";
}

void ApplyCustomCosmetics() {
    auto resourceManager = Ship::Context::GetInstance()->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();

    for (auto& entry : customCosmeticEntries) {
        Color_RGBA8 color = GetCustomCosmeticColor(entry);

        for (const auto& binding : entry.bindings) {
            if (!IsCustomArchive(archiveManager->GetArchiveFromFile(binding.materialPath))) {
                continue;
            }

            auto material =
                std::dynamic_pointer_cast<Fast::DisplayList>(resourceManager->LoadResource(binding.materialPath));
            if (material == nullptr || binding.commandIndex >= material->Instructions.size()) {
                continue;
            }

            if (binding.isPrimColor) {
                material->Instructions[binding.commandIndex] =
                    gsDPSetPrimColor(binding.primM, binding.primL, color.r, color.g, color.b, binding.defaultA);
            } else {
                material->Instructions[binding.commandIndex] =
                    gsDPSetEnvColor(color.r, color.g, color.b, binding.defaultA);
            }
        }
    }
}

static void SetCustomCosmeticColor(const CustomCosmeticEntry& entry, Color_RGBA8 color) {
    std::string rainbowCvar = GetCustomCosmeticRainbowCvar(entry);
    CVarSetColor(entry.colorCvar.c_str(), color);
    CVarSetInteger(rainbowCvar.c_str(), 0);
    CVarSetInteger(entry.changedCvar.c_str(), 1);
    ShipInit::Init(entry.colorCvar.c_str());
    ShipInit::Init(rainbowCvar.c_str());
    ShipInit::Init(entry.changedCvar.c_str());
    ApplyCustomCosmetics();
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

static void ResetCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    std::string cvar = GetCustomCosmeticCvar(entry);
    std::string rainbowCvar = GetCustomCosmeticRainbowCvar(entry);
    std::string lockedCvar = GetCustomCosmeticLockedCvar(entry);

    CVarClear(entry.changedCvar.c_str());
    CVarClear(rainbowCvar.c_str());
    CVarClear(lockedCvar.c_str());
    CVarClear(entry.colorCvar.c_str());
    CVarClear((std::string(entry.colorCvar.c_str()) + ".R").c_str());
    CVarClear((std::string(entry.colorCvar.c_str()) + ".G").c_str());
    CVarClear((std::string(entry.colorCvar.c_str()) + ".B").c_str());
    CVarClear((std::string(entry.colorCvar.c_str()) + ".A").c_str());
    CVarClear((std::string(entry.colorCvar.c_str()) + ".Type").c_str());

    ShipInit::Init(cvar.c_str());
    ShipInit::Init(entry.colorCvar.c_str());
    ShipInit::Init(rainbowCvar.c_str());
    ShipInit::Init(lockedCvar.c_str());
    ShipInit::Init(entry.changedCvar.c_str());
    ApplyCustomCosmetics();
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

static void RandomizeCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    Color_RGBA8 color = { static_cast<uint8_t>(rand() % 256), static_cast<uint8_t>(rand() % 256),
                          static_cast<uint8_t>(rand() % 256), 255 };
    SetCustomCosmeticColor(entry, color);
}

void ScanCustomCosmetics() {
    customCosmeticEntries.clear();

    auto resourceManager = Ship::Context::GetInstance()->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();
    auto materialPaths = archiveManager->ListFiles("*");
    std::unordered_map<std::string, size_t> entryIndicesByKey;

    for (const auto& materialPath : *materialPaths) {
        if (!IsCustomArchive(archiveManager->GetArchiveFromFile(materialPath))) {
            continue;
        }

        tinyxml2::XMLDocument document;
        std::shared_ptr<Fast::DisplayList> material;
        tinyxml2::XMLElement* root = nullptr;
        if (!TryLoadCustomDisplayListXml(archiveManager.get(), resourceManager.get(), materialPath, document, material,
                                         root)) {
            continue;
        }

        size_t searchStart = 0;
        for (auto* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            std::string childName = child->Name();
            bool isPrimColor = childName == "SetPrimColor";
            if (!isPrimColor && childName != "SetEnvColor") {
                continue;
            }

            const char* cosmeticEntry = child->Attribute("CosmeticEntry");
            const char* cosmeticCategory = child->Attribute("CosmeticCategory");
            if (cosmeticEntry == nullptr || cosmeticEntry[0] == '\0') {
                continue;
            }

            std::string key = cosmeticEntry;
            SanitizeCustomKey(key);
            Gfx expectedInstruction;
            if (isPrimColor) {
                expectedInstruction =
                    gsDPSetPrimColor(child->IntAttribute("M"), child->IntAttribute("L"), child->IntAttribute("R"),
                                     child->IntAttribute("G"), child->IntAttribute("B"), child->IntAttribute("A"));
            } else {
                expectedInstruction = gsDPSetEnvColor(child->IntAttribute("R"), child->IntAttribute("G"),
                                                      child->IntAttribute("B"), child->IntAttribute("A"));
            }

            size_t commandIndex = FindDisplayListInstructionIndex(*material, expectedInstruction, searchStart);
            if (commandIndex == SIZE_MAX) {
                continue;
            }
            searchStart = commandIndex + 1;

            size_t entryIndex = 0;
            if (auto it = entryIndicesByKey.find(key); it != entryIndicesByKey.end()) {
                entryIndex = it->second;
            } else {
                entryIndex = customCosmeticEntries.size();
                entryIndicesByKey[key] = entryIndex;

                CustomCosmeticEntry entry;
                entry.displayName = cosmeticEntry;
                entry.category = (cosmeticCategory != nullptr) ? cosmeticCategory : "";
                entry.colorCvar = std::string(CUSTOM_CVAR_PREFIX) + key + ".Value";
                entry.changedCvar = std::string(CUSTOM_CVAR_PREFIX) + key + ".Changed";
                entry.defaultColor = { static_cast<uint8_t>(child->IntAttribute("R")),
                                       static_cast<uint8_t>(child->IntAttribute("G")),
                                       static_cast<uint8_t>(child->IntAttribute("B")),
                                       static_cast<uint8_t>(child->IntAttribute("A")) };
                customCosmeticEntries.push_back(std::move(entry));
            }

            CustomCosmeticBinding binding;
            binding.materialPath = materialPath;
            binding.commandIndex = commandIndex;
            binding.isPrimColor = isPrimColor;
            binding.defaultA = static_cast<uint8_t>(child->IntAttribute("A"));
            binding.primM = static_cast<uint8_t>(child->IntAttribute("M"));
            binding.primL = static_cast<uint8_t>(child->IntAttribute("L"));
            customCosmeticEntries[entryIndex].bindings.push_back(std::move(binding));
        }
    }

    std::stable_sort(customCosmeticEntries.begin(), customCosmeticEntries.end(),
                     [](const CustomCosmeticEntry& lhs, const CustomCosmeticEntry& rhs) {
                         int lhsOrder = 2;
                         int rhsOrder = 2;

                         for (const auto& binding : lhs.bindings) {
                             lhsOrder = std::min(lhsOrder, GetCustomMaterialSortOrder(binding.materialPath));
                         }
                         for (const auto& binding : rhs.bindings) {
                             rhsOrder = std::min(rhsOrder, GetCustomMaterialSortOrder(binding.materialPath));
                         }

                         if (lhsOrder != rhsOrder) {
                             return lhsOrder < rhsOrder;
                         }

                         if (lhs.category.empty() != rhs.category.empty()) {
                             return !lhs.category.empty();
                         }

                         if (lhs.category != rhs.category) {
                             return lhs.category < rhs.category;
                         }

                         return lhs.displayName < rhs.displayName;
                     });

    ApplyCustomCosmetics();
}

static void DrawCustomCosmeticRow(const CustomCosmeticEntry& entry) {
    std::string cvar = GetCustomCosmeticCvar(entry);
    std::string rainbowCvar = GetCustomCosmeticRainbowCvar(entry);
    std::string lockedCvar = GetCustomCosmeticLockedCvar(entry);

    if (UIWidgets::CVarColorPicker(entry.displayName.c_str(), cvar.c_str(), entry.defaultColor, false, 0,
                                   THEME_COLOR)) {
        CVarSetInteger(rainbowCvar.c_str(), 0);
        CVarSetInteger(entry.changedCvar.c_str(), 1);
        ShipInit::Init(rainbowCvar.c_str());
        ShipInit::Init(entry.changedCvar.c_str());
        ApplyCustomCosmetics();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + entry.displayName).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        RandomizeCustomCosmeticColor(entry);
    }

    ImGui::SameLine();
    if (UIWidgets::CVarCheckbox(("Rainbow##" + entry.displayName).c_str(), rainbowCvar.c_str(),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
        CVarSetInteger(entry.changedCvar.c_str(), 1);
        ShipInit::Init(entry.changedCvar.c_str());
        ApplyCustomCosmetics();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::SameLine();
    UIWidgets::CVarCheckbox(("Locked##" + entry.displayName).c_str(), lockedCvar.c_str(),
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR));

    if (CVarGetInteger(entry.changedCvar.c_str(), 0)) {
        ImGui::SameLine();
        if (UIWidgets::Button(("Reset##" + entry.displayName).c_str(),
                              UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
            ResetCustomCosmeticColor(entry);
        }
    }
}

void DrawCustomCosmetics() {
    if (customCosmeticEntries.empty()) {
        return;
    }

    ImGui::Text("%s", CUSTOM_COSMETIC_GROUP);
    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + std::string(CUSTOM_COSMETIC_GROUP)).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        for (const auto& entry : customCosmeticEntries) {
            RandomizeCustomCosmeticColor(entry);
        }
    }
    ImGui::SameLine();
    if (UIWidgets::Button(("Reset##" + std::string(CUSTOM_COSMETIC_GROUP)).c_str(),
                          UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
        for (const auto& entry : customCosmeticEntries) {
            ResetCustomCosmeticColor(entry);
        }
    }

    UIWidgets::Spacer();
    std::string currentCategory;
    for (const auto& entry : customCosmeticEntries) {
        if (entry.category != currentCategory) {
            currentCategory = entry.category;
            if (!currentCategory.empty()) {
                ImGui::Text("%s", currentCategory.c_str());
                UIWidgets::Spacer();
            }
        }
        DrawCustomCosmeticRow(entry);
    }
    UIWidgets::Separator(true, true, 2.0f, 2.0f);
}

void UpdateCustomCosmeticsRainbow(int hue, float rainbowSpeed, int& index) {
    for (const auto& entry : customCosmeticEntries) {
        if (CVarGetInteger(GetCustomCosmeticRainbowCvar(entry).c_str(), 0)) {
            double frequency = 2 * M_PI / (360 * rainbowSpeed);
            Color_RGBA8 newColor;
            newColor.r = static_cast<uint8_t>(sin(frequency * (hue + index) + 0) * 127) + 128;
            newColor.g = static_cast<uint8_t>(sin(frequency * (hue + index) + (2 * M_PI / 3)) * 127) + 128;
            newColor.b = static_cast<uint8_t>(sin(frequency * (hue + index) + (4 * M_PI / 3)) * 127) + 128;
            newColor.a = 255;
            CVarSetColor(entry.colorCvar.c_str(), newColor);
        }
        if (!CVarGetInteger(CVAR_COSMETIC("RainbowSync"), 0)) {
            index += static_cast<int>(60 * rainbowSpeed);
        }
    }
}

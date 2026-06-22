#include "CosmeticsEditor.h"

#include <string>
#include <functional>
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
    CosmeticOption option;
    std::string baseCvar;
    std::string valuesCvar;
    std::string rainbowCvar;
    std::string lockedCvar;
    std::string changedCvar;
    std::string category;
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
    if (CVarGetInteger(entry.option.changedCvar, 0)) {
        return CVarGetColor(entry.option.valuesCvar, entry.option.defaultColor);
    }

    return entry.option.defaultColor;
}

void ApplyCustomCosmetics() {
    auto resourceManager = Ship::Context::GetRawInstance()->GetResourceManager();
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
    CVarSetColor(entry.option.valuesCvar, color);
    CVarSetInteger(entry.option.rainbowCvar, 0);
    CVarSetInteger(entry.option.changedCvar, 1);
    ShipInit::Init(entry.option.valuesCvar);
    ShipInit::Init(entry.option.rainbowCvar);
    ShipInit::Init(entry.option.changedCvar);
    ApplyCustomCosmetics();
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

static void ResetCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    ResetColor(const_cast<CosmeticOption&>(entry.option));
    ApplyCustomCosmetics();
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

static void RandomizeCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    Color_RGBA8 color = { static_cast<uint8_t>(rand() % 256), static_cast<uint8_t>(rand() % 256),
                          static_cast<uint8_t>(rand() % 256), 255 };
    SetCustomCosmeticColor(entry, color);
}

static void DrawCustomCosmeticColorRow(const char* label, const char* cvar, Color_RGBA8 defaultColor,
                                       const char* rainbowCvar, const char* lockedCvar, const char* changedCvar,
                                       const std::function<void()>& onColorChanged,
                                       const std::function<void()>& onRandomize,
                                       const std::function<void()>& onRainbowToggle,
                                       const std::function<void()>& onReset) {
    if (UIWidgets::CVarColorPicker(label, cvar, defaultColor, false, 0, THEME_COLOR)) {
        onColorChanged();
    }

    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + std::string(label)).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        onRandomize();
    }

    ImGui::SameLine();
    if (UIWidgets::CVarCheckbox(("Rainbow##" + std::string(label)).c_str(), rainbowCvar,
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
        onRainbowToggle();
    }

    ImGui::SameLine();
    UIWidgets::CVarCheckbox(("Locked##" + std::string(label)).c_str(), lockedCvar,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR));

    if (CVarGetInteger(changedCvar, 0)) {
        ImGui::SameLine();
        if (UIWidgets::Button(("Reset##" + std::string(label)).c_str(),
                              UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
            onReset();
        }
    }
}

void ScanCustomCosmetics() {
    customCosmeticEntries.clear();

    auto resourceManager = Ship::Context::GetRawInstance()->GetResourceManager();
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
            if (key.empty()) {
                continue;
            }
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
                entry.category = (cosmeticCategory != nullptr) ? cosmeticCategory : "";
                entry.baseCvar = std::string(CUSTOM_CVAR_PREFIX) + key;
                entry.valuesCvar = entry.baseCvar + ".Value";
                entry.rainbowCvar = entry.baseCvar + ".Rainbow";
                entry.lockedCvar = entry.baseCvar + ".Locked";
                entry.changedCvar = entry.baseCvar + ".Changed";
                const Color_RGBA8 defaultColor = { static_cast<uint8_t>(child->IntAttribute("R")),
                                                   static_cast<uint8_t>(child->IntAttribute("G")),
                                                   static_cast<uint8_t>(child->IntAttribute("B")),
                                                   static_cast<uint8_t>(child->IntAttribute("A")) };
                entry.option =
                    MakeCosmeticOption(entry.baseCvar.c_str(), entry.valuesCvar.c_str(), entry.rainbowCvar.c_str(),
                                       entry.lockedCvar.c_str(), entry.changedCvar.c_str(), cosmeticEntry,
                                       COSMETICS_GROUP_MAX, defaultColor, false, true, false);
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

                         return lhs.option.label < rhs.option.label;
                     });

    ApplyCustomCosmetics();
}

static void DrawCustomCosmeticRow(const CustomCosmeticEntry& entry) {
    const char* cvar = entry.option.cvar;

    DrawCustomCosmeticColorRow(
        entry.option.label.c_str(), cvar, entry.option.defaultColor, entry.option.rainbowCvar, entry.option.lockedCvar,
        entry.option.changedCvar,
        [&entry]() {
            CVarSetInteger(entry.option.changedCvar, 1);
            ShipInit::Init(entry.option.changedCvar);
            ApplyCustomCosmetics();
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        },
        [&entry]() { RandomizeCustomCosmeticColor(entry); },
        [&entry]() {
            CVarSetInteger(entry.option.changedCvar, 1);
            ShipInit::Init(entry.option.changedCvar);
            ApplyCustomCosmetics();
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        },
        [&entry]() { ResetCustomCosmeticColor(entry); });
}

static void DrawCustomCosmeticCategory(const char* label, const std::vector<const CustomCosmeticEntry*>& entries) {
    ImGui::Text("%s", label);
    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + std::string(label)).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        for (const auto* entry : entries) {
            RandomizeCustomCosmeticColor(*entry);
        }
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        ApplyCustomCosmetics();
    }
    ImGui::SameLine();
    if (UIWidgets::Button(("Reset##" + std::string(label)).c_str(),
                          UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
        for (const auto* entry : entries) {
            ResetCustomCosmeticColor(*entry);
        }
        ApplyCustomCosmetics();
    }
    UIWidgets::Spacer();
    for (const auto* entry : entries) {
        DrawCustomCosmeticRow(*entry);
    }
    UIWidgets::Separator(true, true, 2.0f, 2.0f);
}

bool HasCustomCosmetics() {
    return !customCosmeticEntries.empty();
}

void DrawCustomCosmetics() {
    if (customCosmeticEntries.empty()) {
        return;
    }

    std::vector<const CustomCosmeticEntry*> currentEntries;
    std::string currentCategory;

    auto flushCategory = [&]() {
        if (currentEntries.empty()) {
            return;
        }

        const char* label = currentCategory.empty() ? CUSTOM_COSMETIC_GROUP : currentCategory.c_str();
        DrawCustomCosmeticCategory(label, currentEntries);
        currentEntries.clear();
    };

    for (const auto& entry : customCosmeticEntries) {
        if (entry.category != currentCategory) {
            flushCategory();
            currentCategory = entry.category;
        }
        currentEntries.push_back(&entry);
    }

    flushCategory();
}

void UpdateCustomCosmeticsRainbow(int hue, float rainbowSpeed, int& index) {
    for (const auto& entry : customCosmeticEntries) {
        if (CVarGetInteger(entry.option.rainbowCvar, 0)) {
            double frequency = 2 * M_PI / (360 * rainbowSpeed);
            Color_RGBA8 newColor;
            newColor.r = static_cast<uint8_t>(sin(frequency * (hue + index) + 0) * 127) + 128;
            newColor.g = static_cast<uint8_t>(sin(frequency * (hue + index) + (2 * M_PI / 3)) * 127) + 128;
            newColor.b = static_cast<uint8_t>(sin(frequency * (hue + index) + (4 * M_PI / 3)) * 127) + 128;
            newColor.a = 255;
            CVarSetColor(entry.option.valuesCvar, newColor);
        }
        if (!CVarGetInteger(CVAR_COSMETIC("RainbowSync"), 0)) {
            index += static_cast<int>(60 * rainbowSpeed);
        }
    }
}

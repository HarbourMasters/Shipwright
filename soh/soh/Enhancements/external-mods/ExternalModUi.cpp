#include "ExternalModUi.h"

#include <string>

#include "ExternalModManager.h"
#include "soh/SohGui/UIWidgets.hpp"

namespace SOH {

void DrawExternalModControlsSection() {
    if (!ImGui::CollapsingHeader("External Mods (ZIP)", ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    const auto& packages = ExternalModManager::Instance().GetPackages();
    if (packages.empty()) {
        ImGui::TextDisabled("No external ZIP mods discovered in mods/.");
        return;
    }

    for (const auto& package : packages) {
        ImGui::Separator();

        const bool runtimeEnabled = package.runtime.enabled;
        const bool manifestValid = package.valid;
        const ImVec4 statusColor =
            runtimeEnabled ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f) : (manifestValid ? ImVec4(0.95f, 0.78f, 0.25f, 1.0f)
                                                                              : ImVec4(0.95f, 0.35f, 0.35f, 1.0f));

        ImGui::TextColored(statusColor, "%s", runtimeEnabled ? "Enabled" : "Disabled");
        ImGui::SameLine();
        ImGui::Text("%s (%s)", package.manifest.name.empty() ? "<unnamed>" : package.manifest.name.c_str(),
                    package.manifest.id.empty() ? "<unknown-id>" : package.manifest.id.c_str());

        if (!package.manifest.version.empty()) {
            ImGui::TextDisabled("Version: %s", package.manifest.version.c_str());
        }

        if (!package.error.empty()) {
            ImGui::TextWrapped("Reason: %s", package.error.c_str());
        }

        if (!runtimeEnabled) {
            continue;
        }

        if (!package.runtime.itemDefinitions.empty()) {
            ImGui::Text("Items:");
            for (const auto& item : package.runtime.itemDefinitions) {
                const char* slotName = item.slot == ExternalModItemSlot::Hookshot ? "SLOT_HOOKSHOT" : "UNKNOWN";
                ImGui::BulletText("%s (%s) slot=%s", item.displayName.c_str(), item.id.c_str(), slotName);
                if (!item.iconAsset.empty()) {
                    ImGui::SameLine();
                    ImGui::TextDisabled("icon: %s", item.iconAsset.c_str());
                }
            }
        }

        if (!package.runtime.inputBindings.empty()) {
            ImGui::Text("Bindings:");
            for (const auto& binding : package.runtime.inputBindings) {
                const auto cvarName = ExternalModManager::BuildBindingCVarName(package.manifest.id, binding.id);
                const auto label = std::string("Binding: ") + binding.id;
                UIWidgets::CVarBtnSelector(label.c_str(), cvarName.c_str(),
                                           UIWidgets::BtnSelectorOptions()
                                               .DefaultValue(binding.defaultMask)
                                               .Color(UIWidgets::Colors::LightBlue)
                                               .Tooltip("External mod action binding"));
            }
        } else {
            ImGui::TextDisabled("No input bindings for this mod.");
        }
    }
}

} // namespace SOH

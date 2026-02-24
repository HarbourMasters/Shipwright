#include "ExternalModUi.h"

#include <string>

#include <ship/Context.h>

#include "ExternalModManager.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/UIWidgets.hpp"

namespace SOH {

const char* GetExternalModItemSlotName(ExternalModItemSlot slot) {
    switch (slot) {
        case ExternalModItemSlot::Stick:
            return "SLOT_STICK";
        case ExternalModItemSlot::Nut:
            return "SLOT_NUT";
        case ExternalModItemSlot::Bomb:
            return "SLOT_BOMB";
        case ExternalModItemSlot::Bow:
            return "SLOT_BOW";
        case ExternalModItemSlot::FireArrow:
            return "SLOT_ARROW_FIRE";
        case ExternalModItemSlot::DinsFire:
            return "SLOT_DINS_FIRE";
        case ExternalModItemSlot::Slingshot:
            return "SLOT_SLINGSHOT";
        case ExternalModItemSlot::Ocarina:
            return "SLOT_OCARINA";
        case ExternalModItemSlot::Bombchu:
            return "SLOT_BOMBCHU";
        case ExternalModItemSlot::Hookshot:
            return "SLOT_HOOKSHOT";
        case ExternalModItemSlot::IceArrow:
            return "SLOT_ARROW_ICE";
        case ExternalModItemSlot::FaroresWind:
            return "SLOT_FARORES_WIND";
        case ExternalModItemSlot::Boomerang:
            return "SLOT_BOOMERANG";
        case ExternalModItemSlot::Lens:
            return "SLOT_LENS";
        case ExternalModItemSlot::Bean:
            return "SLOT_BEAN";
        case ExternalModItemSlot::LightArrow:
            return "SLOT_ARROW_LIGHT";
        case ExternalModItemSlot::Hammer:
            return "SLOT_HAMMER";
        case ExternalModItemSlot::NayrusLove:
            return "SLOT_NAYRUS_LOVE";
        case ExternalModItemSlot::Bottle1:
            return "SLOT_BOTTLE_1";
        case ExternalModItemSlot::Bottle2:
            return "SLOT_BOTTLE_2";
        case ExternalModItemSlot::Bottle3:
            return "SLOT_BOTTLE_3";
        case ExternalModItemSlot::Bottle4:
            return "SLOT_BOTTLE_4";
        case ExternalModItemSlot::TradeAdult:
            return "SLOT_TRADE_ADULT";
        case ExternalModItemSlot::TradeChild:
            return "SLOT_TRADE_CHILD";
        default:
            return "UNKNOWN";
    }
}
const char* GetExternalModItemAgePolicyName(ExternalModItemAgePolicy policy) {
    switch (policy) {
        case ExternalModItemAgePolicy::RespectVanilla:
            return "respectVanilla";
        case ExternalModItemAgePolicy::AllowChild:
            return "allowChild";
        case ExternalModItemAgePolicy::AllowAdult:
            return "allowAdult";
        default:
            return "unknown";
    }
}

const char* GetExternalModItemUseModeName(ExternalModItemUseMode mode) {
    switch (mode) {
        case ExternalModItemUseMode::Vanilla:
            return "vanilla";
        case ExternalModItemUseMode::Override:
            return "override";
        case ExternalModItemUseMode::Augment:
            return "augment";
        default:
            return "unknown";
    }
}

const char* GetExternalModActorArchetypeName(ExternalModActorArchetype archetype) {
    switch (archetype) {
        case ExternalModActorArchetype::Npc:
            return "npc";
        case ExternalModActorArchetype::Prop:
            return "prop";
        case ExternalModActorArchetype::Trigger:
            return "trigger";
        default:
            return "unknown";
    }
}

const char* GetExternalModHookTypeName(ExternalModHookType hookType) {
    switch (hookType) {
        case ExternalModHookType::OnLoadGame:
            return "OnLoadGame";
        case ExternalModHookType::OnExitGame:
            return "OnExitGame";
        case ExternalModHookType::OnSceneInit:
            return "OnSceneInit";
        case ExternalModHookType::AfterSceneCommands:
            return "AfterSceneCommands";
        case ExternalModHookType::OnTransitionEnd:
            return "OnTransitionEnd";
        case ExternalModHookType::OnFlagSet:
            return "OnFlagSet";
        case ExternalModHookType::OnFlagUnset:
            return "OnFlagUnset";
        case ExternalModHookType::OnSceneFlagSet:
            return "OnSceneFlagSet";
        case ExternalModHookType::OnSceneFlagUnset:
            return "OnSceneFlagUnset";
        case ExternalModHookType::OnPlayerUpdate:
            return "OnPlayerUpdate";
        case ExternalModHookType::OnPlayerUseItem:
            return "OnPlayerUseItem";
        case ExternalModHookType::OnPlayerHealthChange:
            return "OnPlayerHealthChange";
        case ExternalModHookType::OnItemReceive:
            return "OnItemReceive";
        case ExternalModHookType::OnActorInit:
            return "OnActorInit";
        case ExternalModHookType::OnActorSpawn:
            return "OnActorSpawn";
        case ExternalModHookType::OnActorUpdate:
            return "OnActorUpdate";
        case ExternalModHookType::OnActorKill:
            return "OnActorKill";
        case ExternalModHookType::OnActorDestroy:
            return "OnActorDestroy";
        case ExternalModHookType::OnEnemyDefeat:
            return "OnEnemyDefeat";
        case ExternalModHookType::OnBossDefeat:
            return "OnBossDefeat";
        case ExternalModHookType::OnPlayDestroy:
            return "OnPlayDestroy";
        case ExternalModHookType::OnGameFrameUpdate:
            return "OnGameFrameUpdate";
        default:
            return "UnknownHook";
    }
}

const char* GetExternalModHookDispatchName(ExternalModHookDispatchType dispatchType) {
    switch (dispatchType) {
        case ExternalModHookDispatchType::Actions:
            return "actions";
        case ExternalModHookDispatchType::WasmExport:
            return "wasmExport";
        default:
            return "unknown";
    }
}

const char* GetExternalModRuntimeModuleFormatName(ExternalModRuntimeModuleFormat moduleFormat) {
    switch (moduleFormat) {
        case ExternalModRuntimeModuleFormat::WasmBinary:
            return "wasm";
        case ExternalModRuntimeModuleFormat::WatText:
            return "wat";
        default:
            return "unknown";
    }
}

void DrawExternalModControlsSection() {
    if (!ImGui::CollapsingHeader("External Mods (ZIP)", ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    auto& packages = ExternalModManager::Instance().GetPackages();
    if (packages.empty()) {
        ImGui::TextDisabled("No external mods discovered in mods/.");
        return;
    }

    ImGui::TextDisabled("Toggle and bindings are saved. Use Reload External Mods after editing ZIP/pasta manifests or scripts.");

    if (ImGui::Button("Open Extra Inventory (I)")) {
        if (auto context = Ship::Context::GetInstance(); context != nullptr && context->GetWindow() != nullptr &&
                                                     context->GetWindow()->GetGui() != nullptr) {
            if (auto window = context->GetWindow()->GetGui()->GetGuiWindow("External Mod Inventory"); window != nullptr) {
                window->ToggleVisibility();
                context->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
        }
    }
    ImGui::SameLine();
    ImGui::TextDisabled("Grid livre para itens concedidos por mods e equip rapido nos slots comuns.");

    ImGui::SameLine();
    if (ImGui::Button("Reload External Mods")) {
        std::string reloadMessage;
        ExternalModManager::Instance().ReloadPackages(reloadMessage);
        Notification::Emit({
            .message = reloadMessage.empty() ? "[ExternalMods] Reload completed." : ("[ExternalMods] Reload completed with warnings: " + reloadMessage),
            .remainingTime = 8.0f,
        });
    }

    for (auto& package : packages) {
        ImGui::Separator();

        const bool runtimeEnabled = package.runtime.enabled;
        const bool manifestValid = package.valid;
        const ImVec4 statusColor =
            runtimeEnabled ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                         : (manifestValid ? ImVec4(0.95f, 0.78f, 0.25f, 1.0f) : ImVec4(0.95f, 0.35f, 0.35f, 1.0f));

        ImGui::TextColored(statusColor, "%s", runtimeEnabled ? "Enabled" : "Disabled");
        ImGui::SameLine();
        ImGui::Text("%s (%s)", package.manifest.name.empty() ? "<unnamed>" : package.manifest.name.c_str(),
                    package.manifest.id.empty() ? "<unknown-id>" : package.manifest.id.c_str());

        if (!package.manifest.version.empty()) {
            ImGui::TextDisabled("Version: %s", package.manifest.version.c_str());
        }

        if (!package.manifest.capabilities.empty()) {
            std::string capabilitiesLine;
            for (size_t i = 0; i < package.manifest.capabilities.size(); ++i) {
                if (!capabilitiesLine.empty()) {
                    capabilitiesLine += ", ";
                }
                capabilitiesLine += package.manifest.capabilities[i];
            }
            ImGui::TextDisabled("Capabilities: %s", capabilitiesLine.c_str());
        } else {
            ImGui::TextDisabled("Capabilities: none");
        }

        if (!package.runtime.moduleSourcePath.empty()) {
            ImGui::TextDisabled("Runtime module: %s (%s)", package.runtime.moduleSourcePath.c_str(),
                                GetExternalModRuntimeModuleFormatName(package.runtime.moduleFormat));
            ImGui::TextDisabled("Compiled wasm size: %zu bytes", package.runtime.compiledModuleSizeBytes);
            if (package.runtime.moduleFormat == ExternalModRuntimeModuleFormat::WatText) {
                ImGui::TextDisabled("WAT compile time: %d ms", package.runtime.moduleCompileTimeMs);
            }
            if (!package.runtime.moduleCompileDiagnostics.empty()) {
                ImGui::TextWrapped("Runtime diagnostics: %s", package.runtime.moduleCompileDiagnostics.c_str());
            }
        }

        const auto enabledCVar = ExternalModManager::BuildEnabledCVarName(package.manifest.id);
        bool modEnabled = CVarGetInteger(enabledCVar.c_str(), 1) != 0;

        if (!manifestValid) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Checkbox((std::string("Enable##") + package.manifest.id).c_str(), &modEnabled)) {
            CVarSetInteger(enabledCVar.c_str(), modEnabled ? 1 : 0);
            package.runtime.enabled = manifestValid && modEnabled;
            if (auto context = Ship::Context::GetInstance(); context != nullptr && context->GetWindow() != nullptr &&
                                                            context->GetWindow()->GetGui() != nullptr) {
                context->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
        }
        if (!manifestValid) {
            ImGui::EndDisabled();
        }

        if (!package.error.empty()) {
            ImGui::TextWrapped("Reason: %s", package.error.c_str());
        }

        if (!package.runtime.hookSubscriptions.empty()) {
            ImGui::Text("Hook subscriptions: %zu", package.runtime.hookSubscriptions.size());
            ImGui::TextDisabled("Hook budget/frame: %d used of %d", package.runtime.hookCallsThisFrame,
                                package.runtime.maxHookCallsPerFrame);
            for (const auto& subscription : package.runtime.hookSubscriptions) {
                ImGui::BulletText("%s hook=%s dispatch=%s cooldown=%d", subscription.id.c_str(),
                                  GetExternalModHookTypeName(subscription.hook),
                                  GetExternalModHookDispatchName(subscription.dispatch), subscription.cooldownFrames);
            }
        }

        if (!package.runtime.actorDefinitions.empty() || !package.runtime.actorInstances.empty()) {
            ImGui::Text("Actor VM: defs=%zu instances=%zu/%d", package.runtime.actorDefinitions.size(),
                        package.runtime.actorInstances.size(), package.runtime.maxActorInstances);
            for (const auto& actorDefinition : package.runtime.actorDefinitions) {
                ImGui::BulletText("%s archetype=%s scene=%d max=%d tick=%d", actorDefinition.id.c_str(),
                                  GetExternalModActorArchetypeName(actorDefinition.archetype), actorDefinition.sceneId,
                                  actorDefinition.maxInstances, actorDefinition.tickRate);
            }
        }

        if (!package.runtime.itemDefinitions.empty()) {
            ImGui::Text("Items:");
            for (const auto& item : package.runtime.itemDefinitions) {
                const char* slotName = GetExternalModItemSlotName(item.slot);
                ImGui::BulletText("%s (%s) slot=%s mode=%s age=%s", item.displayName.c_str(), item.id.c_str(), slotName,
                                  GetExternalModItemUseModeName(item.useMode),
                                  GetExternalModItemAgePolicyName(item.agePolicy));
                if (!item.description.empty()) {
                    ImGui::TextDisabled("desc: %s", item.description.c_str());
                }
                if (!item.iconAsset.empty()) {
                    ImGui::TextDisabled("icon: %s", item.iconAsset.c_str());
                }
                if (!item.modelAsset.empty()) {
                    ImGui::TextDisabled("model: %s (triangles=%zu)", item.modelAsset.c_str(),
                                        item.customModelTriangles.size());
                }
                if (!item.modelDisplayList.empty()) {
                    ImGui::TextDisabled("model dlist: %s", item.modelDisplayList.c_str());
                }
                if (!item.modelTextureAsset.empty()) {
                    ImGui::TextDisabled("model texture: %s", item.modelTextureAsset.c_str());
                }
                if (!item.hookshotMetalTextureAsset.empty() || !item.hookshotHandleTextureAsset.empty() ||
                    !item.hookshotDesignTextureAsset.empty() || !item.hookshotChainTextureAsset.empty() ||
                    !item.hookshotReticleTextureAsset.empty()) {
                    ImGui::TextDisabled("hookshot gameplay textures:");
                    if (!item.hookshotMetalTextureAsset.empty()) {
                        ImGui::TextDisabled("  metal: %s", item.hookshotMetalTextureAsset.c_str());
                    }
                    if (!item.hookshotHandleTextureAsset.empty()) {
                        ImGui::TextDisabled("  handle: %s", item.hookshotHandleTextureAsset.c_str());
                    }
                    if (!item.hookshotDesignTextureAsset.empty()) {
                        ImGui::TextDisabled("  design: %s", item.hookshotDesignTextureAsset.c_str());
                    }
                    if (!item.hookshotChainTextureAsset.empty()) {
                        ImGui::TextDisabled("  chain: %s", item.hookshotChainTextureAsset.c_str());
                    }
                    if (!item.hookshotReticleTextureAsset.empty()) {
                        ImGui::TextDisabled("  reticle: %s", item.hookshotReticleTextureAsset.c_str());
                    }
                }
                if (item.hasGrantItemId || item.hasGrantAmmo) {
                    const std::string grantItemText = item.hasGrantItemId ? std::to_string(item.grantItemId) : "<slot-default>";
                    const std::string grantAmmoText = item.hasGrantAmmo ? std::to_string(item.grantAmmo) : "<unchanged>";
                    ImGui::TextDisabled("grant: itemId=%s ammo=%s", grantItemText.c_str(), grantAmmoText.c_str());
                }
                ImGui::TextDisabled("state: granted=%s cooldown=%d/%d", item.granted ? "yes" : "no",
                                    item.cooldownRemaining, item.cooldownFrames);
            }
        }

        if (!package.runtime.inputBindings.empty()) {
            ImGui::Text("Bindings:");
            ImGui::TextDisabled("Tip: map to Mod Action buttons, then bind any keyboard/gamepad key in Settings > Controls > Modifier Buttons.");
            for (const auto& binding : package.runtime.inputBindings) {
                const auto cvarName = ExternalModManager::BuildBindingCVarName(package.manifest.id, binding.id);
                const auto label = std::string("Binding: ") + binding.id;
                UIWidgets::CVarBtnSelector(label.c_str(), cvarName.c_str(),
                                           UIWidgets::BtnSelectorOptions()
                                               .DefaultValue(binding.defaultMask)
                                               .Color(UIWidgets::Colors::LightBlue)
                                               .Tooltip("External mod action binding (supports combinations)"));
            }
        } else {
            ImGui::TextDisabled("No input bindings for this mod.");
        }
    }
}

} // namespace SOH

#ifdef ENABLE_REMOTE_CONTROL

#include "Sail.h"
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>
#include "soh/OTRGlobals.h"
#include "soh/util.h"

template <class DstType, class SrcType>
bool IsType(const SrcType* src) {
    return dynamic_cast<const DstType*>(src) != nullptr;
}

void Sail::Enable() {
    Network::Enable(CVarGetString(CVAR_REMOTE_SAIL("Host"), "127.0.0.1"), CVarGetInteger(CVAR_REMOTE_SAIL("Port"), 43384));
}

void Sail::OnConnected() {
    RegisterHooks();
}

void Sail::OnDisconnected() {
    RegisterHooks();
}

void Sail::OnIncomingJson(nlohmann::json payload) {
    SPDLOG_INFO("[Sail] Received payload: \n{}", payload.dump());

    nlohmann::json responsePayload;
    responsePayload["type"] = "result";
    responsePayload["status"] = "failure";

    try {
        if (!payload.contains("id")) {
            SPDLOG_ERROR("[Sail] Received payload without ID");
            SendJsonToRemote(responsePayload);
            return;
        }

        responsePayload["id"] = payload["id"];

        if (!payload.contains("type")) {
            SPDLOG_ERROR("[Sail] Received payload without type");
            SendJsonToRemote(responsePayload);
            return;
        }

        std::string payloadType = payload["type"].get<std::string>();

        if (payloadType == "command") {
            if (!payload.contains("command")) {
                SPDLOG_ERROR("[Sail] Received command payload without command");
                SendJsonToRemote(responsePayload);
                return;
            }

            std::string command = payload["command"].get<std::string>();
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch(command);
            responsePayload["status"] = "success";
            SendJsonToRemote(responsePayload);
            return;
        } else if (payloadType == "effect") {
            if (!payload.contains("effect") || !payload["effect"].contains("type")) {
                SPDLOG_ERROR("[Sail] Received effect payload without effect type");
                SendJsonToRemote(responsePayload);
                return;
            }

            std::string effectType = payload["effect"]["type"].get<std::string>();

            // Special case for "command" effect, so we can also run commands from the `simple_twitch_sail` script
            if (effectType == "command") {
                if (!payload["effect"].contains("command")) {
                    SPDLOG_ERROR("[Sail] Received command effect payload without command");
                    SendJsonToRemote(responsePayload);
                    return;
                }

                std::string command = payload["effect"]["command"].get<std::string>();
                std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch(command);
                responsePayload["status"] = "success";
                SendJsonToRemote(responsePayload);
                return;
            }

            if (effectType != "apply" && effectType != "remove") {
                SPDLOG_ERROR("[Sail] Received effect payload with unknown effect type: {}", effectType);
                SendJsonToRemote(responsePayload);
                return;
            }

            if (!GameInteractor::IsSaveLoaded()) {
                responsePayload["status"] = "try_again";
                SendJsonToRemote(responsePayload);
                return;
            }

            GameInteractionEffectBase* giEffect = EffectFromJson(payload["effect"]);
            if (giEffect) {
                GameInteractionEffectQueryResult result;
                if (effectType == "remove") {
                    if (IsType<RemovableGameInteractionEffect>(giEffect)) {
                        result = dynamic_cast<RemovableGameInteractionEffect*>(giEffect)->Remove();
                    } else {
                        result = GameInteractionEffectQueryResult::NotPossible;
                    }
                } else {
                    result = giEffect->Apply();
                }

                if (result == GameInteractionEffectQueryResult::Possible) {
                    responsePayload["status"] = "success";
                } else if (result == GameInteractionEffectQueryResult::TemporarilyNotPossible) {
                    responsePayload["status"] = "try_again";
                }
                SendJsonToRemote(responsePayload);
                return;
            }
        } else {
            SPDLOG_ERROR("[Sail] Unknown payload type: {}", payloadType);
            SendJsonToRemote(responsePayload);
            return;
        }

        // If we get here, something went wrong, send the failure response
        SPDLOG_ERROR("[Sail] Failed to handle remote JSON, sending failure response");
        SendJsonToRemote(responsePayload);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[Sail] Exception handling remote JSON: {}", e.what());
    } catch (...) {
        SPDLOG_ERROR("[Sail] Unknown exception handling remote JSON");
    }
}

GameInteractionEffectBase* Sail::EffectFromJson(nlohmann::json payload) {
    if (!payload.contains("name")) {
        return nullptr;
    }

    std::string name = payload["name"].get<std::string>();

    if (name == "SetSceneFlag") {
        auto effect = new GameInteractionEffect::SetSceneFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
            effect->parameters[2] = payload["parameters"][2].get<int32_t>();
        }
        return effect;
    } else if (name == "UnsetSceneFlag") {
        auto effect = new GameInteractionEffect::UnsetSceneFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
            effect->parameters[2] = payload["parameters"][2].get<int32_t>();
        }
        return effect;
    } else if (name == "SetFlag") {
        auto effect = new GameInteractionEffect::SetFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "UnsetFlag") {
        auto effect = new GameInteractionEffect::UnsetFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyHeartContainers") {
        auto effect = new GameInteractionEffect::ModifyHeartContainers();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "FillMagic") {
        return new GameInteractionEffect::FillMagic();
    } else if (name == "EmptyMagic") {
        return new GameInteractionEffect::EmptyMagic();
    } else if (name == "ModifyRupees") {
        auto effect = new GameInteractionEffect::ModifyRupees();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "NoUI") {
        return new GameInteractionEffect::NoUI();
    } else if (name == "ModifyGravity") {
        auto effect = new GameInteractionEffect::ModifyGravity();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyHealth") {
        auto effect = new GameInteractionEffect::ModifyHealth();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetPlayerHealth") {
        auto effect = new GameInteractionEffect::SetPlayerHealth();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "FreezePlayer") {
        return new GameInteractionEffect::FreezePlayer();
    } else if (name == "BurnPlayer") {
        return new GameInteractionEffect::BurnPlayer();
    } else if (name == "ElectrocutePlayer") {
        return new GameInteractionEffect::ElectrocutePlayer();
    } else if (name == "KnockbackPlayer") {
        auto effect = new GameInteractionEffect::KnockbackPlayer();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyLinkSize") {
        auto effect = new GameInteractionEffect::ModifyLinkSize();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "InvisibleLink") {
        return new GameInteractionEffect::InvisibleLink();
    } else if (name == "PacifistMode") {
        return new GameInteractionEffect::PacifistMode();
    } else if (name == "DisableZTargeting") {
        return new GameInteractionEffect::DisableZTargeting();
    } else if (name == "WeatherRainstorm") {
        return new GameInteractionEffect::WeatherRainstorm();
    } else if (name == "ReverseControls") {
        return new GameInteractionEffect::ReverseControls();
    } else if (name == "ForceEquipBoots") {
        auto effect = new GameInteractionEffect::ForceEquipBoots();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyRunSpeedModifier") {
        auto effect = new GameInteractionEffect::ModifyRunSpeedModifier();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "OneHitKO") {
        return new GameInteractionEffect::OneHitKO();
    } else if (name == "ModifyDefenseModifier") {
        auto effect = new GameInteractionEffect::ModifyDefenseModifier();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "GiveOrTakeShield") {
        auto effect = new GameInteractionEffect::GiveOrTakeShield();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "TeleportPlayer") {
        auto effect = new GameInteractionEffect::TeleportPlayer();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ClearAssignedButtons") {
        auto effect = new GameInteractionEffect::ClearAssignedButtons();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetTimeOfDay") {
        auto effect = new GameInteractionEffect::SetTimeOfDay();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetCollisionViewer") {
        return new GameInteractionEffect::SetCollisionViewer();
    } else if (name == "SetCosmeticsColor") {
        auto effect = new GameInteractionEffect::SetCosmeticsColor();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "RandomizeCosmetics") {
        return new GameInteractionEffect::RandomizeCosmetics();
    } else if (name == "PressButton") {
        auto effect = new GameInteractionEffect::PressButton();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "PressRandomButton") {
        auto effect = new GameInteractionEffect::PressRandomButton();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "AddOrTakeAmmo") {
        auto effect = new GameInteractionEffect::AddOrTakeAmmo();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "RandomBombFuseTimer") {
        return new GameInteractionEffect::RandomBombFuseTimer();
    } else if (name == "DisableLedgeGrabs") {
        return new GameInteractionEffect::DisableLedgeGrabs();
    } else if (name == "RandomWind") {
        return new GameInteractionEffect::RandomWind();
    } else if (name == "RandomBonks") {
        return new GameInteractionEffect::RandomBonks();
    } else if (name == "PlayerInvincibility") {
        return new GameInteractionEffect::PlayerInvincibility();
    } else if (name == "SlipperyFloor") {
        return new GameInteractionEffect::SlipperyFloor();
    } else {
        SPDLOG_INFO("[Sail] Unknown effect name: {}", name);
        return nullptr;
    }
}

void Sail::RegisterHooks() {
    static HOOK_ID onTransitionEndHook = 0;
    static HOOK_ID onLoadGameHook = 0;
    static HOOK_ID onExitGameHook = 0;
    static HOOK_ID onItemReceiveHook = 0;
    static HOOK_ID onEnemyDefeatHook = 0;
    static HOOK_ID onActorInitHook = 0;
    static HOOK_ID onFlagSetHook = 0;
    static HOOK_ID onFlagUnsetHook = 0;
    static HOOK_ID onSceneFlagSetHook = 0;
    static HOOK_ID onSceneFlagUnsetHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnTransitionEnd>(onTransitionEndHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnLoadGame>(onLoadGameHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnExitGame>(onExitGameHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(onItemReceiveHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnEnemyDefeat>(onEnemyDefeatHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagUnset>(onFlagUnsetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagUnset>(onSceneFlagUnsetHook);

    onTransitionEndHook = 0;
    onLoadGameHook = 0;
    onExitGameHook = 0;
    onItemReceiveHook = 0;
    onEnemyDefeatHook = 0;
    onActorInitHook = 0;
    onFlagSetHook = 0;
    onFlagUnsetHook = 0;
    onSceneFlagSetHook = 0;
    onSceneFlagUnsetHook = 0;

    if (!isConnected) {
        return;
    }

    onTransitionEndHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>([&](int32_t sceneNum) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnTransitionEnd";
        payload["hook"]["sceneNum"] = sceneNum;

        SendJsonToRemote(payload);
    });
    onLoadGameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([&](int32_t fileNum) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnLoadGame";
        payload["hook"]["fileNum"] = fileNum;

        SendJsonToRemote(payload);
    });
    onExitGameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([&](int32_t fileNum) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnExitGame";
        payload["hook"]["fileNum"] = fileNum;

        SendJsonToRemote(payload);
    });
    onItemReceiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([&](GetItemEntry itemEntry) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnItemReceive";
        payload["hook"]["tableId"] = itemEntry.tableId;
        payload["hook"]["getItemId"] = itemEntry.getItemId;

        SendJsonToRemote(payload);
    });
    onEnemyDefeatHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnEnemyDefeat>([&](void* refActor) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        Actor* actor = (Actor*)refActor;
        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnEnemyDefeat";
        payload["hook"]["actorId"] = actor->id;
        payload["hook"]["params"] = actor->params;

        SendJsonToRemote(payload);
    });
    onActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([&](void* refActor) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        Actor* actor = (Actor*)refActor;
        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnActorInit";
        payload["hook"]["actorId"] = actor->id;
        payload["hook"]["params"] = actor->params;

        SendJsonToRemote(payload);
    });
    onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([&](int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnFlagSet";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;

        SendJsonToRemote(payload);
    });
    onFlagUnsetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([&](int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnFlagUnset";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;

        SendJsonToRemote(payload);
    });
    onSceneFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>([&](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnSceneFlagSet";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;
        payload["hook"]["sceneNum"] = sceneNum;

        SendJsonToRemote(payload);
    });
    onSceneFlagUnsetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>([&](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnSceneFlagUnset";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;
        payload["hook"]["sceneNum"] = sceneNum;

        SendJsonToRemote(payload);
    });
}

void Sail::DrawMenu() {
    ImGui::PushID("Sail");

    static std::string host = CVarGetString(CVAR_REMOTE_SAIL("Host"), "127.0.0.1");
    static uint16_t port = CVarGetInteger(CVAR_REMOTE_SAIL("Port"), 43384);
    bool isFormValid = !SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535;

    ImGui::SeparatorText("Sail");
    UIWidgets::Tooltip(
        "Sail is a networking protocol designed to facilitate remote "
        "control of the Ship of Harkinian client. It is intended to "
        "be utilized alongside a Sail server, for which we provide a "
        "few straightforward implementations on our GitHub. The current "
        "implementations available allow integration with Twitch chat "
        "and SAMMI Bot, feel free to contribute your own!\n"
        "\n"
        "Click the question mark to copy the link to the Sail Github "
        "page to your clipboard."
    );
    if (ImGui::IsItemClicked()) {
        ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
    }

    ImGui::BeginDisabled(isEnabled);
    ImGui::Text("Host & Port");
    if (UIWidgets::InputString("##Host", &host)) {
        CVarSetString(CVAR_REMOTE_SAIL("Host"), host.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetFontSize() * 5);
    if (ImGui::InputScalar("##Port", ImGuiDataType_U16, &port)) {
        CVarSetInteger(CVAR_REMOTE_SAIL("Port"), port);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    ImGui::PopItemWidth();
    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::BeginDisabled(!isFormValid);
    const char* buttonLabel = isEnabled ? "Disable" : "Enable";
    if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
        if (isEnabled) {
            CVarClear(CVAR_REMOTE_SAIL("Enabled"));
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            Disable();
        } else {
            CVarSetInteger(CVAR_REMOTE_SAIL("Enabled"), 1);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            Enable();
        }
    }
    ImGui::EndDisabled();

    if (isEnabled) {
        ImGui::Spacing();
        if (isConnected) {
            ImGui::Text("Connected");
        } else {
            ImGui::Text("Connecting...");
        }
    }

    ImGui::PopID();
}

#endif // ENABLE_REMOTE_CONTROL

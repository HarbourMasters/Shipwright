#include "Sail.h"
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>
#include "soh/ShipUtils.h"

template <class DstType, class SrcType> bool IsType(const SrcType* src) {
    return dynamic_cast<const DstType*>(src) != nullptr;
}

void Sail::Enable() {
    Network::Enable(CVarGetString(CVAR_REMOTE_SAIL("Host"), "127.0.0.1"),
                    CVarGetInteger(CVAR_REMOTE_SAIL("Port"), 43384));
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
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                ->Dispatch(command);
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
                std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                    ->Dispatch(command);
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

            auto giEffect = EffectFromJson(payload["effect"]);
            if (giEffect) {
                GameInteractionEffectQueryResult result;
                if (effectType == "remove") {
                    if (IsType<RemovableGameInteractionEffect>(giEffect.get())) {
                        result = dynamic_cast<RemovableGameInteractionEffect*>(giEffect.get())->Remove();
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
    } catch (...) { SPDLOG_ERROR("[Sail] Unknown exception handling remote JSON"); }
}

std::unique_ptr<GameInteractionEffectBase> Sail::EffectFromJson(nlohmann::json payload) {
    if (!payload.contains("name")) {
        return nullptr;
    }

    std::string name = payload["name"].get<std::string>();

    if (name == "SetSceneFlag") {
        auto effect = std::make_unique<GameInteractionEffect::SetSceneFlag>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
            effect->parameters[2] = payload["parameters"][2].get<int32_t>();
        }
        return effect;
    } else if (name == "UnsetSceneFlag") {
        auto effect = std::make_unique<GameInteractionEffect::UnsetSceneFlag>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
            effect->parameters[2] = payload["parameters"][2].get<int32_t>();
        }
        return effect;
    } else if (name == "SetFlag") {
        auto effect = std::make_unique<GameInteractionEffect::SetFlag>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "UnsetFlag") {
        auto effect = std::make_unique<GameInteractionEffect::UnsetFlag>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyHeartContainers") {
        auto effect = std::make_unique<GameInteractionEffect::ModifyHeartContainers>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "FillMagic") {
        return std::make_unique<GameInteractionEffect::FillMagic>();
    } else if (name == "EmptyMagic") {
        return std::make_unique<GameInteractionEffect::EmptyMagic>();
    } else if (name == "ModifyRupees") {
        auto effect = std::make_unique<GameInteractionEffect::ModifyRupees>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "NoUI") {
        return std::make_unique<GameInteractionEffect::NoUI>();
    } else if (name == "ModifyGravity") {
        auto effect = std::make_unique<GameInteractionEffect::ModifyGravity>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyHealth") {
        auto effect = std::make_unique<GameInteractionEffect::ModifyHealth>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetPlayerHealth") {
        auto effect = std::make_unique<GameInteractionEffect::SetPlayerHealth>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "FreezePlayer") {
        return std::make_unique<GameInteractionEffect::FreezePlayer>();
    } else if (name == "BurnPlayer") {
        return std::make_unique<GameInteractionEffect::BurnPlayer>();
    } else if (name == "ElectrocutePlayer") {
        return std::make_unique<GameInteractionEffect::ElectrocutePlayer>();
    } else if (name == "KnockbackPlayer") {
        auto effect = std::make_unique<GameInteractionEffect::KnockbackPlayer>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyLinkSize") {
        auto effect = std::make_unique<GameInteractionEffect::ModifyLinkSize>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "InvisibleLink") {
        return std::make_unique<GameInteractionEffect::InvisibleLink>();
    } else if (name == "PacifistMode") {
        return std::make_unique<GameInteractionEffect::PacifistMode>();
    } else if (name == "DisableZTargeting") {
        return std::make_unique<GameInteractionEffect::DisableZTargeting>();
    } else if (name == "WeatherRainstorm") {
        return std::make_unique<GameInteractionEffect::WeatherRainstorm>();
    } else if (name == "ReverseControls") {
        return std::make_unique<GameInteractionEffect::ReverseControls>();
    } else if (name == "ForceEquipBoots") {
        auto effect = std::make_unique<GameInteractionEffect::ForceEquipBoots>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyMovementSpeedMultiplier") {
        auto effect = std::make_unique<GameInteractionEffect::ModifyMovementSpeedMultiplier>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "OneHitKO") {
        return std::make_unique<GameInteractionEffect::OneHitKO>();
    } else if (name == "ModifyDefenseModifier") {
        auto effect = std::make_unique<GameInteractionEffect::ModifyDefenseModifier>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "GiveOrTakeShield") {
        auto effect = std::make_unique<GameInteractionEffect::GiveOrTakeShield>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "TeleportPlayer") {
        auto effect = std::make_unique<GameInteractionEffect::TeleportPlayer>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ClearAssignedButtons") {
        auto effect = std::make_unique<GameInteractionEffect::ClearAssignedButtons>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetTimeOfDay") {
        auto effect = std::make_unique<GameInteractionEffect::SetTimeOfDay>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetCollisionViewer") {
        return std::make_unique<GameInteractionEffect::SetCollisionViewer>();
    } else if (name == "RandomizeCosmetics") {
        return std::make_unique<GameInteractionEffect::RandomizeCosmetics>();
    } else if (name == "PressButton") {
        auto effect = std::make_unique<GameInteractionEffect::PressButton>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "PressRandomButton") {
        auto effect = std::make_unique<GameInteractionEffect::PressRandomButton>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "AddOrTakeAmmo") {
        auto effect = std::make_unique<GameInteractionEffect::AddOrTakeAmmo>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "RandomBombFuseTimer") {
        return std::make_unique<GameInteractionEffect::RandomBombFuseTimer>();
    } else if (name == "DisableLedgeGrabs") {
        return std::make_unique<GameInteractionEffect::DisableLedgeGrabs>();
    } else if (name == "RandomWind") {
        return std::make_unique<GameInteractionEffect::RandomWind>();
    } else if (name == "RandomBonks") {
        return std::make_unique<GameInteractionEffect::RandomBonks>();
    } else if (name == "PlayerInvincibility") {
        return std::make_unique<GameInteractionEffect::PlayerInvincibility>();
    } else if (name == "SlipperyFloor") {
        return std::make_unique<GameInteractionEffect::SlipperyFloor>();
    } else if (name == "SpawnEnemyWithOffset") {
        auto effect = std::make_unique<GameInteractionEffect::SpawnEnemyWithOffset>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "SpawnActor") {
        auto effect = std::make_unique<GameInteractionEffect::SpawnActor>();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else {
        SPDLOG_INFO("[Sail] Unknown effect name: {}", name);
        return nullptr;
    }
}

void Sail::RegisterHooks() {
    COND_HOOK(OnTransitionEnd, isConnected, [&](int32_t sceneNum) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;

        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnTransitionEnd";
        payload["hook"]["sceneNum"] = sceneNum;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnLoadGame, isConnected, [&](int32_t fileNum) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;

        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnLoadGame";
        payload["hook"]["fileNum"] = fileNum;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnExitGame, isConnected, [&](int32_t fileNum) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;

        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnExitGame";
        payload["hook"]["fileNum"] = fileNum;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnItemReceive, isConnected, [&](GetItemEntry itemEntry) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;
        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnItemReceive";
        payload["hook"]["tableId"] = itemEntry.tableId;
        payload["hook"]["getItemId"] = itemEntry.getItemId;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnEnemyDefeat, isConnected, [&](void* refActor) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;

        Actor* actor = (Actor*)refActor;
        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnEnemyDefeat";
        payload["hook"]["actorId"] = actor->id;
        payload["hook"]["params"] = actor->params;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnActorInit, isConnected, [&](void* refActor) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;

        Actor* actor = (Actor*)refActor;
        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnActorInit";
        payload["hook"]["actorId"] = actor->id;
        payload["hook"]["params"] = actor->params;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnFlagSet, isConnected, [&](int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;
        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnFlagSet";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnFlagUnset, isConnected, [&](int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;
        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnFlagUnset";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnSceneFlagSet, isConnected, [&](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;
        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnSceneFlagSet";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;
        payload["hook"]["sceneNum"] = sceneNum;

        SendJsonToRemote(payload);
    });

    COND_HOOK(OnSceneFlagUnset, isConnected, [&](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!isConnected || !GameInteractor::IsSaveLoaded())
            return;
        nlohmann::json payload;
        payload["id"] = ShipUtils::Random(0, UINT32_MAX);
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnSceneFlagUnset";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;
        payload["hook"]["sceneNum"] = sceneNum;

        SendJsonToRemote(payload);
    });
}

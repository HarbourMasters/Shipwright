#ifdef ENABLE_REMOTE_CONTROL

#include "GameInteractor_Sail.h"
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>

template <class DstType, class SrcType>
bool IsType(const SrcType* src) {
  return dynamic_cast<const DstType*>(src) != nullptr;
}

void GameInteractorSail::Enable() {
    if (isEnabled) {
        return;
    }

    isEnabled = true;
    GameInteractor::Instance->EnableRemoteInteractor();
    GameInteractor::Instance->RegisterRemoteJsonHandler([&](nlohmann::json payload) {
        HandleRemoteJson(payload);
    });
    GameInteractor::Instance->RegisterRemoteConnectedHandler([&]() {
        RegisterHooks();
    });
}

void GameInteractorSail::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    GameInteractor::Instance->DisableRemoteInteractor();
}

void GameInteractorSail::HandleRemoteJson(nlohmann::json payload) {
    SPDLOG_INFO("[GameInteractorSail] Received payload: \n{}", payload.dump());

    nlohmann::json responsePayload;
    responsePayload["type"] = "result";
    responsePayload["status"] = "failure";

    try {
        if (!payload.contains("id")) {
            SPDLOG_ERROR("[GameInteractorSail] Received payload without ID");
            GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
            return;
        }

        responsePayload["id"] = payload["id"];

        if (!payload.contains("type")) {
            SPDLOG_ERROR("[GameInteractorSail] Received payload without type");
            GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
            return;
        }

        std::string payloadType = payload["type"].get<std::string>();

        if (payloadType == "command") {
            if (!payload.contains("command")) {
                SPDLOG_ERROR("[GameInteractorSail] Received command payload without command");
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
                return;
            }

            std::string command = payload["command"].get<std::string>();
            std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch(command);
            responsePayload["status"] = "success";
            GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
            return;
        } else if (payloadType == "effect") {
            if (!payload.contains("effect") || !payload["effect"].contains("type")) {
                SPDLOG_ERROR("[GameInteractorSail] Received effect payload without effect type");
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
                return;
            }

            std::string effectType = payload["effect"]["type"].get<std::string>();

            // Special case for "command" effect, so we can also run commands from the `simple_twitch_sail` script
            if (effectType == "command") {
                if (!payload["effect"].contains("command")) {
                    SPDLOG_ERROR("[GameInteractorSail] Received command effect payload without command");
                    GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
                    return;
                }

                std::string command = payload["effect"]["command"].get<std::string>();
                std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch(command);
                responsePayload["status"] = "success";
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
                return;
            }

            if (effectType != "apply" && effectType != "remove") {
                SPDLOG_ERROR("[GameInteractorSail] Received effect payload with unknown effect type: {}", effectType);
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
                return;
            }

            if (!GameInteractor::IsSaveLoaded()) {
                responsePayload["status"] = "try_again";
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
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
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
                return;
            }
        } else {
            SPDLOG_ERROR("[GameInteractorSail] Unknown payload type: {}", payloadType);
            GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
            return;
        }

        // If we get here, something went wrong, send the failure response
        SPDLOG_ERROR("[GameInteractorSail] Failed to handle remote JSON, sending failure response");
        GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[GameInteractorSail] Exception handling remote JSON: {}", e.what());
    } catch (...) {
        SPDLOG_ERROR("[GameInteractorSail] Unknown exception handling remote JSON");
    }
}

GameInteractionEffectBase* GameInteractorSail::EffectFromJson(nlohmann::json payload) {
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
        SPDLOG_INFO("[GameInteractorSail] Unknown effect name: {}", name);
        return nullptr;
    }
}

// Workaround until we have a way to unregister hooks
static bool hasRegisteredHooks = false;

void GameInteractorSail::RegisterHooks() {
    if (hasRegisteredHooks) {
        return;
    }
    hasRegisteredHooks = true;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>([](int32_t sceneNum) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnTransitionEnd";
        payload["hook"]["sceneNum"] = sceneNum;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnLoadGame";
        payload["hook"]["fileNum"] = fileNum;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](int32_t fileNum) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnExitGame";
        payload["hook"]["fileNum"] = fileNum;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnItemReceive";
        payload["hook"]["tableId"] = itemEntry.tableId;
        payload["hook"]["getItemId"] = itemEntry.getItemId;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnEnemyDefeat>([](void* refActor) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        Actor* actor = (Actor*)refActor;
        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnEnemyDefeat";
        payload["hook"]["actorId"] = actor->id;
        payload["hook"]["params"] = actor->params;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* refActor) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        Actor* actor = (Actor*)refActor;
        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnActorInit";
        payload["hook"]["actorId"] = actor->id;
        payload["hook"]["params"] = actor->params;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([](int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnFlagSet";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([](int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnFlagUnset";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>([](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnSceneFlagSet";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;
        payload["hook"]["sceneNum"] = sceneNum;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>([](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::IsSaveLoaded()) return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnSceneFlagUnset";
        payload["hook"]["flagType"] = flagType;
        payload["hook"]["flag"] = flag;
        payload["hook"]["sceneNum"] = sceneNum;

        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
}

#endif

#ifdef ENABLE_REMOTE_CONTROL

#include "GameInteractor.h"
#include <spdlog/spdlog.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <unordered_map>
#include <tuple>
#include <type_traits>

// MARK: - Declarations

template <class DstType, class SrcType>
bool IsType(const SrcType* src) {
  return dynamic_cast<const DstType*>(src) != nullptr;
}

template <class TypeA>
bool TakesParameter() {
    return std::is_base_of_v<ParameterizedGameInteractionEffect, TypeA>;
}

template <class TypeA>
bool IsRemovable() {
    return std::is_base_of_v<RemovableGameInteractionEffect, TypeA>;
}

/// Map of string name to enum value and flag whether it takes in a param or not
std::unordered_map<std::string, std::tuple<GameInteractionEffect::Values, bool, bool>> nameToEnum = {
    {"modify_heart_container", {
        GameInteractionEffect::Values::modifyHeartContainers,
        IsRemovable<GameInteractionEffect::ModifyHeartContainers>(),
        TakesParameter<GameInteractionEffect::ModifyHeartContainers>()
    }},
    { "fill_magic", {
        GameInteractionEffect::Values::fillMagic,
        IsRemovable<GameInteractionEffect::ModifyHeartContainers>(),
        TakesParameter<GameInteractionEffect::FillMagic>()
    }},
    { "empty_magic", {
        GameInteractionEffect::Values::emptyMagic,
        IsRemovable<GameInteractionEffect::EmptyMagic>(),
        TakesParameter<GameInteractionEffect::EmptyMagic>()
    }},
    { "modify_rupees", {
        GameInteractionEffect::Values::modifyRupees,
        IsRemovable<GameInteractionEffect::ModifyRupees>(),
        TakesParameter<GameInteractionEffect::ModifyRupees>()
    }},
    { "no_ui", {
        GameInteractionEffect::Values::noUI,
        IsRemovable<GameInteractionEffect::NoUI>(),
        TakesParameter<GameInteractionEffect::NoUI>()
    }},
    { "modify_gravity", {
        GameInteractionEffect::Values::modifyGravity,
        IsRemovable<GameInteractionEffect::ModifyGravity>(),
        TakesParameter<GameInteractionEffect::ModifyGravity>()
    }},
    { "modify_health", {
        GameInteractionEffect::Values::modifyHealth,
        IsRemovable<GameInteractionEffect::ModifyHealth>(),
        TakesParameter<GameInteractionEffect::ModifyHealth>()
    }},
    { "set_player_health", {
        GameInteractionEffect::Values::setPlayerHealth,
        IsRemovable<GameInteractionEffect::SetPlayerHealth>(),
        TakesParameter<GameInteractionEffect::SetPlayerHealth>()
    }},
    { "freeze_player", {
        GameInteractionEffect::Values::freezePlayer,
        IsRemovable<GameInteractionEffect::FreezePlayer>(),
        TakesParameter<GameInteractionEffect::FreezePlayer>()
    }},
    { "burn_player", {
        GameInteractionEffect::Values::burnPlayer,
        IsRemovable<GameInteractionEffect::BurnPlayer>(),
        TakesParameter<GameInteractionEffect::BurnPlayer>()
    }},
    { "electrocute_player", {
        GameInteractionEffect::Values::electrocutePlayer,
        IsRemovable<GameInteractionEffect::ElectrocutePlayer>(),
        TakesParameter<GameInteractionEffect::ElectrocutePlayer>()
    }},
    { "knockback_player", {
        GameInteractionEffect::Values::knockbackPlayer,
        IsRemovable<GameInteractionEffect::KnockbackPlayer>(),
        TakesParameter<GameInteractionEffect::KnockbackPlayer>()
    }},
    { "modify_link_size", {
        GameInteractionEffect::Values::modifyLinkSize,
        IsRemovable<GameInteractionEffect::ModifyLinkSize>(),
        TakesParameter<GameInteractionEffect::ModifyLinkSize>()
    }},
    { "invisible_link", {
        GameInteractionEffect::Values::invisibleLink,
        IsRemovable<GameInteractionEffect::InvisibleLink>(),
        TakesParameter<GameInteractionEffect::InvisibleLink>()
    }},
    { "pacifist_mode", {
        GameInteractionEffect::Values::pacifistMode,
        IsRemovable<GameInteractionEffect::PacifistMode>(),
        TakesParameter<GameInteractionEffect::PacifistMode>()
    }},
    { "disable_z_targeting", {
        GameInteractionEffect::Values::disableZTargeting,
        IsRemovable<GameInteractionEffect::DisableZTargeting>(),
        TakesParameter<GameInteractionEffect::DisableZTargeting>()
    }},
    { "weather_rainstorm", {
        GameInteractionEffect::Values::weatherRainstorm,
        IsRemovable<GameInteractionEffect::WeatherRainstorm>(),
        TakesParameter<GameInteractionEffect::WeatherRainstorm>()
    }},
    { "reverse_controls", {
        GameInteractionEffect::Values::reverseControls,
        IsRemovable<GameInteractionEffect::ReverseControls>(),
        TakesParameter<GameInteractionEffect::ReverseControls>()
    }},
    { "force_equip_boots", {
        GameInteractionEffect::Values::forceEquipBoots,
        IsRemovable<GameInteractionEffect::ForceEquipBoots>(),
        TakesParameter<GameInteractionEffect::ForceEquipBoots>()
    }},
    { "modify_run_speed_modifier", {
        GameInteractionEffect::Values::modifyRunSpeedModifier,
        IsRemovable<GameInteractionEffect::ModifyRunSpeedModifier>(),
        TakesParameter<GameInteractionEffect::ModifyRunSpeedModifier>()
    }},
    { "one_hit_ko", {
        GameInteractionEffect::Values::oneHitKO,
        IsRemovable<GameInteractionEffect::OneHitKO>(),
        TakesParameter<GameInteractionEffect::OneHitKO>()
    }},
    { "modify_defense_modifier", {
        GameInteractionEffect::Values::modifyDefenseModifier,
        IsRemovable<GameInteractionEffect::ModifyDefenseModifier>(),
        TakesParameter<GameInteractionEffect::ModifyDefenseModifier>()
    }},
    { "give_or_take_shield", {
        GameInteractionEffect::Values::giveOrTakeShield,
        IsRemovable<GameInteractionEffect::GiveOrTakeShield>(),
        TakesParameter<GameInteractionEffect::GiveOrTakeShield>()
    }},
};

// MARK: - Remote

void GameInteractor::EnableRemoteInteractor() {
    if (isRemoteInteractorEnabled) {
        return;
    }

    Uint16 port;
    ImGui::DataTypeApplyFromText(GameInteractor::Instance->remotePortStr, ImGuiDataType_U16, &port, "%u");

    if (SDLNet_ResolveHost(&remoteIP, remoteIPStr, port) == -1) {
        SPDLOG_ERROR("[GameInteractor] SDLNet_ResolveHost: {}", SDLNet_GetError());
    }

    isRemoteInteractorEnabled = true;
    remoteThreadReceive = std::thread(&GameInteractor::ReceiveFromServer, this);
}

void GameInteractor::RegisterRemoteForwarder(std::function<void(nlohmann::json)> method) {
    remoteForwarder = method;
}

void GameInteractor::DisableRemoteInteractor() {
    if (!isRemoteInteractorEnabled) {
        return;
    }

    isRemoteInteractorEnabled = false;
    remoteThreadReceive.join();
    remoteForwarder = nullptr;
}

void GameInteractor::TransmitMessageToRemote(nlohmann::json payload) {
    std::string jsonPayload = payload.dump();
    SDLNet_TCP_Send(remoteSocket, jsonPayload.c_str(), jsonPayload.size());
}

// MARK: - Private

void GameInteractor::ReceiveFromServer() {
    while (isRemoteInteractorEnabled) {
        while (!isRemoteInteractorConnected && isRemoteInteractorEnabled) {
            SPDLOG_TRACE("[GameInteractor] Attempting to make connection to server...");
            remoteSocket = SDLNet_TCP_Open(&remoteIP);

            if (remoteSocket) {
                isRemoteInteractorConnected = true;
                SPDLOG_TRACE("[GameInteractor] Connection to server established!");

                // transmit supported events to remote
                nlohmann::json payload;
                payload["action"] = "identify";
                payload["supported_events"] = nlohmann::json::array();
                for (auto& [key, value] : nameToEnum) {
                    nlohmann::json entry;
                    entry["event"] = key;
                    entry["is_removable"] = std::get<1>(value);
                    entry["takes_param"] = std::get<2>(value);
                    payload["supported_events"].push_back(entry);
                }
                TransmitMessageToRemote(payload);

                break;
            }
        }

        SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
        if (remoteSocket) {
            SDLNet_TCP_AddSocket(socketSet, remoteSocket);
        }

        // Listen to socket messages
        while (isRemoteInteractorConnected && remoteSocket && isRemoteInteractorEnabled) {
            // we check first if socket has data, to not block in the TCP_Recv
            int socketsReady = SDLNet_CheckSockets(socketSet, 0);

            if (socketsReady == -1) {
                SPDLOG_ERROR("[GameInteractor] SDLNet_CheckSockets: {}", SDLNet_GetError());
                break;
            }

            if (socketsReady == 0) {
                continue;
            }

            char remoteDataReceived[512];
            memset(remoteDataReceived, 0, sizeof(remoteDataReceived));
            int len = SDLNet_TCP_Recv(remoteSocket, &remoteDataReceived, sizeof(remoteDataReceived));
            if (!len || !remoteSocket || len == -1) {
                SPDLOG_ERROR("[GameInteractor] SDLNet_TCP_Recv: {}", SDLNet_GetError());
                break;
            }

            HandleRemoteMessage(remoteDataReceived);
        }

        if (isRemoteInteractorConnected) {
            SDLNet_TCP_Close(remoteSocket);
            isRemoteInteractorConnected = false;
            SPDLOG_TRACE("[GameInteractor] Ending receiving thread...");
        }
    }
}

// making it available as it's defined below
GameInteractionEffectBase* EffectFromJson(std::string name, nlohmann::json payload);

void GameInteractor::HandleRemoteMessage(char message[512]) {
    nlohmann::json payload = nlohmann::json::parse(message);

    if (remoteForwarder) {
        remoteForwarder(payload);
        return;
    }

    // { action: "apply_effect, effect: { "name: "value", "payload": { "parameter": "value" } }
    // { action: "remove_effect, effect: { "name: "value" }
    // if action contains effect then it's an effect
    if (payload["action"] == "apply_effect" || payload["action"] == "remove_effect") {
        nlohmann::json effect = payload["effect"];
        GameInteractionEffectBase* giEffect = EffectFromJson(effect["name"].get<std::string>(), effect["payload"]);
        if (giEffect) {
            if (payload["action"] == "apply_effect") {
                giEffect->Apply();
            } else if (IsType<RemovableGameInteractionEffect>(giEffect)) {
                dynamic_cast<RemovableGameInteractionEffect*>(giEffect)->Remove();
            }
        }
    }
}

// MARK: - Effect Helpers

GameInteractionEffectBase* EffectFromJson(std::string name, nlohmann::json payload) {
    if (nameToEnum.find(name) == nameToEnum.end()) {
        return nullptr;
    }

    switch (std::get<0>(nameToEnum[name])) {
        case GameInteractionEffect::Values::modifyHeartContainers: {
            auto effect = new GameInteractionEffect::ModifyHeartContainers();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::fillMagic:
            return new GameInteractionEffect::FillMagic();
        case GameInteractionEffect::Values::emptyMagic:
            return new GameInteractionEffect::EmptyMagic();
        case GameInteractionEffect::Values::modifyRupees: {
            auto effect = new GameInteractionEffect::ModifyRupees();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::noUI:
            return new GameInteractionEffect::NoUI();
        case GameInteractionEffect::Values::modifyGravity: {
            auto effect = new GameInteractionEffect::ModifyGravity();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::modifyHealth: {
            auto effect = new GameInteractionEffect::ModifyHealth();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::setPlayerHealth: {
            auto effect = new GameInteractionEffect::SetPlayerHealth();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::freezePlayer:
            return new GameInteractionEffect::FreezePlayer();
        case GameInteractionEffect::Values::burnPlayer:
            return new GameInteractionEffect::BurnPlayer();
        case GameInteractionEffect::Values::electrocutePlayer:
            return new GameInteractionEffect::ElectrocutePlayer();
        case GameInteractionEffect::Values::knockbackPlayer: {
            auto effect = new GameInteractionEffect::KnockbackPlayer();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::modifyLinkSize: {
            auto effect = new GameInteractionEffect::ModifyLinkSize();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::invisibleLink:
            return new GameInteractionEffect::InvisibleLink();
        case GameInteractionEffect::Values::pacifistMode:
            return new GameInteractionEffect::PacifistMode();
        case GameInteractionEffect::Values::disableZTargeting:
            return new GameInteractionEffect::DisableZTargeting();
        case GameInteractionEffect::Values::weatherRainstorm:
            return new GameInteractionEffect::WeatherRainstorm();
        case GameInteractionEffect::Values::reverseControls:
            return new GameInteractionEffect::ReverseControls();
        case GameInteractionEffect::Values::forceEquipBoots: {
            auto effect = new GameInteractionEffect::ForceEquipBoots();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::modifyRunSpeedModifier: {
            auto effect = new GameInteractionEffect::ModifyRunSpeedModifier();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::oneHitKO:
            return new GameInteractionEffect::OneHitKO();
        case GameInteractionEffect::Values::modifyDefenseModifier: {
            auto effect = new GameInteractionEffect::ModifyDefenseModifier();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::giveOrTakeShield: {
            auto effect = new GameInteractionEffect::GiveOrTakeShield();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        default:
            return nullptr;
    }
}

#endif
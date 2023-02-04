#ifdef ENABLE_REMOTE_CONTROL

#include "GameInteractor.h"
#include <spdlog/spdlog.h>
#include <unordered_map>

// MARK: - Remote

void GameInteractor::EnableRemoteInteractor(const char *host, Uint16 port) {
    if (isRemoteInteractorEnabled) {
        return;
    }

    if (SDLNet_ResolveHost(&remoteIP, host, port) == -1) {
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
    SDLNet_TCP_Send(remoteSocket, jsonPayload.c_str(), jsonPayload.size() + 1);
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
    nlohmann::json payload = nlohmann::json::parse(message, nullptr, false);

    if (remoteForwarder) {
        remoteForwarder(payload);
        return;
    }

    // { action: "apply_effect, effect: { "name: "value", "payload": {} }
    // { action: "remove_effect, effect: { "name: "value" }
    // if action contains effect then it's an effect
    if (payload["action"] == "apply_effect" || payload["action"] == "remove_effect") {
        nlohmann::json effect = payload["effect"];
        GameInteractionEffectBase* giEffect = EffectFromJson(effect["name"].get<std::string>(), effect["payload"]);
        if (giEffect) {
            if (payload["action"] == "apply_effect") {
                giEffect->Apply();
            } else {
                giEffect->Remove();
            }
        }
    }
}

// MARK: - Effect Helpers

std::unordered_map<std::string, GameInteractionEffect::Values> nameToEnum = {
    { "modify_heart_container", GameInteractionEffect::Values::modifyHeartContainers },
    { "fill_magic", GameInteractionEffect::Values::fillMagic },
    { "empty_magic", GameInteractionEffect::Values::emptyMagic },
    { "modify_rupees", GameInteractionEffect::Values::modifyRupees },
    { "no_ui", GameInteractionEffect::Values::noUI },
    { "modify_gravity", GameInteractionEffect::Values::modifyGravity },
    { "modify_health", GameInteractionEffect::Values::modifyHealth },
    { "set_player_health", GameInteractionEffect::Values::setPlayerHealth },
    { "freeze_player", GameInteractionEffect::Values::freezePlayer },
    { "burn_player", GameInteractionEffect::Values::burnPlayer },
    { "electrocute_player", GameInteractionEffect::Values::electrocutePlayer },
    { "knockback_player", GameInteractionEffect::Values::knockbackPlayer },
    { "modify_link_size", GameInteractionEffect::Values::modifyLinkSize },
    { "invisible_link", GameInteractionEffect::Values::invisibleLink },
    { "pacifist_mode", GameInteractionEffect::Values::pacifistMode },
    { "disable_z_targeting", GameInteractionEffect::Values::disableZTargeting },
    { "weather_rainstorm", GameInteractionEffect::Values::weatherRainstorm },
    { "reverse_controls", GameInteractionEffect::Values::reverseControls },
    { "force_equip_boots", GameInteractionEffect::Values::forceEquipBoots },
    { "modify_run_speed_modifier", GameInteractionEffect::Values::modifyRunSpeedModifier },
    { "one_hit_ko", GameInteractionEffect::Values::oneHitKO },
    { "modify_defense_modifier", GameInteractionEffect::Values::modifyDefenseModifier },
    { "give_deku_shield", GameInteractionEffect::Values::giveDekuShield },
    { "spawn_cucco_storm", GameInteractionEffect::Values::spawnCuccoStorm }
};

GameInteractionEffectBase* EffectFromJson(std::string name, nlohmann::json payload) {
    if (nameToEnum.find(name) == nameToEnum.end()) {
        return nullptr;
    }

    switch (nameToEnum[name]) {
        case GameInteractionEffect::Values::modifyHeartContainers: {
            auto effect = new GameInteractionEffect::ModifyHeartContainers();
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::fillMagic:
            return new GameInteractionEffect::FillMagic();
        case GameInteractionEffect::Values::emptyMagic:
            return new GameInteractionEffect::EmptyMagic();
        case GameInteractionEffect::Values::modifyRupees: {
            auto effect = new GameInteractionEffect::ModifyRupees();
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::noUI:
            return new GameInteractionEffect::NoUI();
        case GameInteractionEffect::Values::modifyGravity: {
            auto effect = new GameInteractionEffect::ModifyGravity();
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::modifyHealth: {
            auto effect = new GameInteractionEffect::ModifyHealth();
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::setPlayerHealth: {
            auto effect = new GameInteractionEffect::SetPlayerHealth();
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::freezePlayer:
            return new GameInteractionEffect::FreezePlayer();
        case GameInteractionEffect::Values::burnPlayer:
            return new GameInteractionEffect::BurnPlayer();
        case GameInteractionEffect::Values::electrocutePlayer:
            return new GameInteractionEffect::ElectrocutePlayer();
        case GameInteractionEffect::Values::knockbackPlayer:
            return new GameInteractionEffect::KnockbackPlayer();
        case GameInteractionEffect::Values::modifyLinkSize: {
            auto effect = new GameInteractionEffect::ModifyLinkSize();
            effect->parameter = payload["parameter"].get<int32_t>();
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
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::modifyRunSpeedModifier: {
            auto effect = new GameInteractionEffect::ModifyRunSpeedModifier();
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::oneHitKO:
            return new GameInteractionEffect::OneHitKO();
        case GameInteractionEffect::Values::modifyDefenseModifier: {
            auto effect = new GameInteractionEffect::ModifyDefenseModifier();
            effect->parameter = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractionEffect::Values::giveDekuShield:
            return new GameInteractionEffect::GiveDekuShield();
        case GameInteractionEffect::Values::spawnCuccoStorm:
            return new GameInteractionEffect::SpawnCuccoStorm();
        default:
            return nullptr;
    }
}

#endif

#ifdef ENABLE_REMOTE_CONTROL

#include "GameInteractor_BuiltIn.h"
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>

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
std::unordered_map<std::string, std::tuple<GameInteractorBuiltInEffects, bool, bool>> nameToEnum = {
    {"modify_heart_container", {
        GameInteractorBuiltInEffects::modifyHeartContainers,
        IsRemovable<GameInteractionEffect::ModifyHeartContainers>(),
        TakesParameter<GameInteractionEffect::ModifyHeartContainers>()
    }},
    { "fill_magic", {
        GameInteractorBuiltInEffects::fillMagic,
        IsRemovable<GameInteractionEffect::ModifyHeartContainers>(),
        TakesParameter<GameInteractionEffect::FillMagic>()
    }},
    { "empty_magic", {
        GameInteractorBuiltInEffects::emptyMagic,
        IsRemovable<GameInteractionEffect::EmptyMagic>(),
        TakesParameter<GameInteractionEffect::EmptyMagic>()
    }},
    { "modify_rupees", {
        GameInteractorBuiltInEffects::modifyRupees,
        IsRemovable<GameInteractionEffect::ModifyRupees>(),
        TakesParameter<GameInteractionEffect::ModifyRupees>()
    }},
    { "no_ui", {
        GameInteractorBuiltInEffects::noUI,
        IsRemovable<GameInteractionEffect::NoUI>(),
        TakesParameter<GameInteractionEffect::NoUI>()
    }},
    { "modify_gravity", {
        GameInteractorBuiltInEffects::modifyGravity,
        IsRemovable<GameInteractionEffect::ModifyGravity>(),
        TakesParameter<GameInteractionEffect::ModifyGravity>()
    }},
    { "modify_health", {
        GameInteractorBuiltInEffects::modifyHealth,
        IsRemovable<GameInteractionEffect::ModifyHealth>(),
        TakesParameter<GameInteractionEffect::ModifyHealth>()
    }},
    { "set_player_health", {
        GameInteractorBuiltInEffects::setPlayerHealth,
        IsRemovable<GameInteractionEffect::SetPlayerHealth>(),
        TakesParameter<GameInteractionEffect::SetPlayerHealth>()
    }},
    { "freeze_player", {
        GameInteractorBuiltInEffects::freezePlayer,
        IsRemovable<GameInteractionEffect::FreezePlayer>(),
        TakesParameter<GameInteractionEffect::FreezePlayer>()
    }},
    { "burn_player", {
        GameInteractorBuiltInEffects::burnPlayer,
        IsRemovable<GameInteractionEffect::BurnPlayer>(),
        TakesParameter<GameInteractionEffect::BurnPlayer>()
    }},
    { "electrocute_player", {
        GameInteractorBuiltInEffects::electrocutePlayer,
        IsRemovable<GameInteractionEffect::ElectrocutePlayer>(),
        TakesParameter<GameInteractionEffect::ElectrocutePlayer>()
    }},
    { "knockback_player", {
        GameInteractorBuiltInEffects::knockbackPlayer,
        IsRemovable<GameInteractionEffect::KnockbackPlayer>(),
        TakesParameter<GameInteractionEffect::KnockbackPlayer>()
    }},
    { "modify_link_size", {
        GameInteractorBuiltInEffects::modifyLinkSize,
        IsRemovable<GameInteractionEffect::ModifyLinkSize>(),
        TakesParameter<GameInteractionEffect::ModifyLinkSize>()
    }},
    { "invisible_link", {
        GameInteractorBuiltInEffects::invisibleLink,
        IsRemovable<GameInteractionEffect::InvisibleLink>(),
        TakesParameter<GameInteractionEffect::InvisibleLink>()
    }},
    { "pacifist_mode", {
        GameInteractorBuiltInEffects::pacifistMode,
        IsRemovable<GameInteractionEffect::PacifistMode>(),
        TakesParameter<GameInteractionEffect::PacifistMode>()
    }},
    { "disable_z_targeting", {
        GameInteractorBuiltInEffects::disableZTargeting,
        IsRemovable<GameInteractionEffect::DisableZTargeting>(),
        TakesParameter<GameInteractionEffect::DisableZTargeting>()
    }},
    { "weather_rainstorm", {
        GameInteractorBuiltInEffects::weatherRainstorm,
        IsRemovable<GameInteractionEffect::WeatherRainstorm>(),
        TakesParameter<GameInteractionEffect::WeatherRainstorm>()
    }},
    { "reverse_controls", {
        GameInteractorBuiltInEffects::reverseControls,
        IsRemovable<GameInteractionEffect::ReverseControls>(),
        TakesParameter<GameInteractionEffect::ReverseControls>()
    }},
    { "force_equip_boots", {
        GameInteractorBuiltInEffects::forceEquipBoots,
        IsRemovable<GameInteractionEffect::ForceEquipBoots>(),
        TakesParameter<GameInteractionEffect::ForceEquipBoots>()
    }},
    { "modify_run_speed_modifier", {
        GameInteractorBuiltInEffects::modifyRunSpeedModifier,
        IsRemovable<GameInteractionEffect::ModifyRunSpeedModifier>(),
        TakesParameter<GameInteractionEffect::ModifyRunSpeedModifier>()
    }},
    { "one_hit_ko", {
        GameInteractorBuiltInEffects::oneHitKO,
        IsRemovable<GameInteractionEffect::OneHitKO>(),
        TakesParameter<GameInteractionEffect::OneHitKO>()
    }},
    { "modify_defense_modifier", {
        GameInteractorBuiltInEffects::modifyDefenseModifier,
        IsRemovable<GameInteractionEffect::ModifyDefenseModifier>(),
        TakesParameter<GameInteractionEffect::ModifyDefenseModifier>()
    }},
    { "give_or_take_shield", {
        GameInteractorBuiltInEffects::giveOrTakeShield,
        IsRemovable<GameInteractionEffect::GiveOrTakeShield>(),
        TakesParameter<GameInteractionEffect::GiveOrTakeShield>()
    }},
};

void GameInteractorBuiltIn::Enable() {
    if (isEnabled) {
        return;
    }

    isEnabled = true;
    GameInteractor::Instance->EnableRemoteInteractor();
    GameInteractor::Instance->RegisterRemoteJsonHandler([&](nlohmann::json payload) {
        HandleRemoteJson(payload);
    });
    GameInteractor::Instance->RegisterRemoteConnectedHandler([&]() {
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
        GameInteractor::Instance->TransmitJsonToRemote(payload);
    });
}

void GameInteractorBuiltIn::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    GameInteractor::Instance->DisableRemoteInteractor();
}

void GameInteractorBuiltIn::HandleRemoteJson(nlohmann::json payload) {
    SPDLOG_INFO("[GameInteractorBuiltIn] Received payload: \n{}\n", payload.dump());

    if (!payload.contains("action")) {
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

GameInteractionEffectBase* GameInteractorBuiltIn::EffectFromJson(std::string name, nlohmann::json payload) {
    if (nameToEnum.find(name) == nameToEnum.end()) {
        return nullptr;
    }

    switch (std::get<0>(nameToEnum[name])) {
        case GameInteractorBuiltInEffects::modifyHeartContainers: {
            auto effect = new GameInteractionEffect::ModifyHeartContainers();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::fillMagic:
            return new GameInteractionEffect::FillMagic();
        case GameInteractorBuiltInEffects::emptyMagic:
            return new GameInteractionEffect::EmptyMagic();
        case GameInteractorBuiltInEffects::modifyRupees: {
            auto effect = new GameInteractionEffect::ModifyRupees();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::noUI:
            return new GameInteractionEffect::NoUI();
        case GameInteractorBuiltInEffects::modifyGravity: {
            auto effect = new GameInteractionEffect::ModifyGravity();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::modifyHealth: {
            auto effect = new GameInteractionEffect::ModifyHealth();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::setPlayerHealth: {
            auto effect = new GameInteractionEffect::SetPlayerHealth();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::freezePlayer:
            return new GameInteractionEffect::FreezePlayer();
        case GameInteractorBuiltInEffects::burnPlayer:
            return new GameInteractionEffect::BurnPlayer();
        case GameInteractorBuiltInEffects::electrocutePlayer:
            return new GameInteractionEffect::ElectrocutePlayer();
        case GameInteractorBuiltInEffects::knockbackPlayer: {
            auto effect = new GameInteractionEffect::KnockbackPlayer();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::modifyLinkSize: {
            auto effect = new GameInteractionEffect::ModifyLinkSize();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::invisibleLink:
            return new GameInteractionEffect::InvisibleLink();
        case GameInteractorBuiltInEffects::pacifistMode:
            return new GameInteractionEffect::PacifistMode();
        case GameInteractorBuiltInEffects::disableZTargeting:
            return new GameInteractionEffect::DisableZTargeting();
        case GameInteractorBuiltInEffects::weatherRainstorm:
            return new GameInteractionEffect::WeatherRainstorm();
        case GameInteractorBuiltInEffects::reverseControls:
            return new GameInteractionEffect::ReverseControls();
        case GameInteractorBuiltInEffects::forceEquipBoots: {
            auto effect = new GameInteractionEffect::ForceEquipBoots();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::modifyRunSpeedModifier: {
            auto effect = new GameInteractionEffect::ModifyRunSpeedModifier();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::oneHitKO:
            return new GameInteractionEffect::OneHitKO();
        case GameInteractorBuiltInEffects::modifyDefenseModifier: {
            auto effect = new GameInteractionEffect::ModifyDefenseModifier();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        case GameInteractorBuiltInEffects::giveOrTakeShield: {
            auto effect = new GameInteractionEffect::GiveOrTakeShield();
            effect->parameters[0] = payload["parameter"].get<int32_t>();
            return effect;
        }
        default:
            return nullptr;
    }
}

#endif

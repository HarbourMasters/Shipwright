#ifdef ENABLE_CROWD_CONTROL

#include "CrowdControl.h"
#include <libultraship/bridge.h>
#include <Console.h>
#include <ImGuiImpl.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <regex>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

typedef enum { 
    spawnCuccoStorm,
    spawnLitBomb,
    spawnExplosion,
    spawnArwing,
    spawnDarkLink,
    spawnIronKnuckle,
    spawnStalfos,
    spawnFreezard,
    spawnLikeLike,
    spawnKeese,
    spawnIceKeese,
    spawnFireKeese,
    spawnWolfos,
    spawnWallmaster,

} CCStringValues;

std::unordered_map<std::string, CCStringValues> effectStringToEnum = {
    { "spawn_cuccostorm", spawnCuccoStorm },
    { "spawn_litbomb", spawnLitBomb },
    { "spawn_explosion", spawnExplosion },
    { "spawn_arwing", spawnArwing },
    { "spawn_darklink", spawnDarkLink },
    { "spawn_ironknuckle", spawnIronKnuckle },
    { "spawn_stalfos", spawnStalfos },
    { "spawn_freezard", spawnFreezard },
    { "spawn_likelike", spawnLikeLike },
    { "spawn_keese", spawnKeese },
};

#define EFFECT_SPAWN_ARWING "spawn_arwing"
#define EFFECT_SPAWN_DARK_LINK "spawn_darklink"
#define EFFECT_SPAWN_IRON_KNUCKLE "spawn_ironknuckle"
#define EFFECT_SPAWN_STALFOS "spawn_stalfos"
#define EFFECT_SPAWN_FREEZARD "spawn_freezard"
#define EFFECT_SPAWN_LIKE_LIKE "spawn_likelike"
#define EFFECT_SPAWN_KEESE "spawn_keese"
#define EFFECT_SPAWN_ICE_KEESE "spawn_icekeese"
#define EFFECT_SPAWN_FIRE_KEESE "spawn_firekeese"
#define EFFECT_SPAWN_WOLFOS "spawn_wolfos"
#define EFFECT_SPAWN_WALLMASTER "spawn_wallmaster"

// Link Modifiers
#define EFFECT_HALF_DAMAGE_TAKEN "half_damage_taken"
#define EFFECT_DOUBLE_DAMAGE_TAKEN "double_damage_taken"
#define EFFECT_OHKO "ohko"
#define EFFECT_INVINCIBLE "invincible"
#define EFFECT_INCREASE_SPEED "increase_speed"
#define EFFECT_DECREASE_SPEED "decrease_speed"
#define EFFECT_LOW_GRAVITY "low_gravity"
#define EFFECT_HIGH_GRAVITY "high_gravity"
#define EFFECT_IRON_BOOTS "iron_boots"
#define EFFECT_HOVER_BOOTS "hover_boots"
#define EFFECT_SLIPPERY_FLOOR "slippery_floor"
#define EFFECT_NO_LEDGE_GRABS "no_ledge_grabs"
#define EFFECT_RANDOM_BONKS "random_bonks"

// Hurt or heal Link
#define EFFECT_DAMAGE "damage"
#define EFFECT_HEAL "heal"
#define EFFECT_KNOCKBACK "knockback"
#define EFFECT_BURN "burn"
#define EFFECT_FREEZE "freeze"
#define EFFECT_ELECTROCUTE "electrocute"
#define EFFECT_KILL "kill"

// Give Items or Consumables
#define EFFECT_ADD_HEART_CONTAINER "add_heart_container"
#define EFFECT_FILL_MAGIC "fill_magic"
#define EFFECT_ADD_RUPEES "add_rupees"
#define EFFECT_GIVE_DEKU_SHIELD "give_dekushield"
#define EFFECT_GIVE_HYLIAN_SHIELD "give_hylianshield"
#define EFFECT_REFILL_STICKS "refill_sticks"
#define EFFECT_REFILL_NUTS "refill_nuts"
#define EFFECT_REFILL_BOMBS "refill_bombs"
#define EFFECT_REFILL_SEEDS "refill_seeds"
#define EFFECT_REFILL_ARROWS "refill_arrows"
#define EFFECT_REFILL_BOMBCHUS "refill_bombchus"

// Take Items or Consumables
#define EFFECT_REMOVE_HEART_CONTAINER "remove_heart_container"
#define EFFECT_EMPTY_MAGIC "empty_magic"
#define EFFECT_REMOVE_RUPEES "remove_rupees"
#define EFFECT_TAKE_DEKU_SHIELD "take_dekushield"
#define EFFECT_TAKE_HYLIAN_SHIELD "take_hylianshield"
#define EFFECT_TAKE_STICKS "take_sticks"
#define EFFECT_TAKE_NUTS "take_nuts"
#define EFFECT_TAKE_BOMBS "take_bombs"
#define EFFECT_TAKE_SEEDS "take_seeds"
#define EFFECT_TAKE_ARROWS "take_arrows"
#define EFFECT_TAKE_BOMBCHUS "take_bombchus"

// Link Size Modifiers
#define EFFECT_GIANT_LINK "giant_link"
#define EFFECT_MINISH_LINK "minish_link"
#define EFFECT_PAPER_LINK "paper_link"
#define EFFECT_SQUISHED_LINK "squished_link"
#define EFFECT_INVISIBLE_LINK "invisible_link"

// Generic Effects
#define EFFECT_RANDOM_BOMB_TIMER "random_bomb_timer"
#define EFFECT_TIME_DAWN "time_dawn"
#define EFFECT_TIME_DUSK "time_dusk"

// Visual Effects
#define EFFECT_NO_UI "no_ui"
#define EFFECT_RAINSTORM "rainstorm"
#define EFFECT_DEBUG_MODE "debug_mode"
#define EFFECT_RAINBOW_MODE "rainbow_mode"
#define EFFECT_RANDOM_COSMETICS "random_cosmetics"

// Controls
#define EFFECT_NO_Z_TARGETING "no_z"
#define EFFECT_REVERSE_CONTROLS "reverse_controls"
#define EFFECT_PACIFIST "pacifist"
#define EFFECT_RANDOM_BUTTONS "random_buttons"
#define EFFECT_CLEAR_CBUTTONS "clear_cbuttons"
#define EFFECT_CLEAR_DPAD "clear_dpad"

// Teleport Player
#define EFFECT_TP_LINKSHOUSE "tp_linkshouse"
#define EFFECT_TP_MINUET "tp_minuet"
#define EFFECT_TP_BOLERO "tp_bolero"
#define EFFECT_TP_SERENADE "tp_serenade"
#define EFFECT_TP_REQUIEM "tp_requiem"
#define EFFECT_TP_NOCTURNE "tp_nocturne"
#define EFFECT_TP_PRELUDE "tp_prelude"

// Tunic Colors (bidding war)
#define EFFECT_TUNIC_RED "tunic_red"
#define EFFECT_TUNIC_GREEN "tunic_green"
#define EFFECT_TUNIC_BLUE "tunic_blue"
#define EFFECT_TUNIC_ORANGE "tunic_orange"
#define EFFECT_TUNIC_YELLOW "tunic_yellow"
#define EFFECT_TUNIC_PURPLE "tunic_purple"
#define EFFECT_TUNIC_PINK "tunic_pink"
#define EFFECT_TUNIC_BROWN "tunic_brown"
#define EFFECT_TUNIC_BLACK "tunic_black"

// Navi Colors (bidding war)
#define EFFECT_NAVI_RED "navi_red"
#define EFFECT_NAVI_GREEN "navi_green"
#define EFFECT_NAVI_BLUE "navi_blue"
#define EFFECT_NAVI_ORANGE "navi_orange"
#define EFFECT_NAVI_YELLOW "navi_yellow"
#define EFFECT_NAVI_PURPLE "navi_purple"
#define EFFECT_NAVI_PINK "navi_pink"
#define EFFECT_NAVI_BROWN "navi_brown"
#define EFFECT_NAVI_BLACK "navi_black"

// Link's hair Colors (bidding war)
#define EFFECT_HAIR_RED "hair_red"
#define EFFECT_HAIR_GREEN "hair_green"
#define EFFECT_HAIR_BLUE "hair_blue"
#define EFFECT_HAIR_ORANGE "hair_orange"
#define EFFECT_HAIR_YELLOW "hair_yellow"
#define EFFECT_HAIR_PURPLE "hair_purple"
#define EFFECT_HAIR_PINK "hair_pink"
#define EFFECT_HAIR_BROWN "hair_brown"
#define EFFECT_HAIR_BLACK "hair_black"

// Categories
#define EFFECT_CAT_UI "ui"
#define EFFECT_CAT_GRAVITY "gravity"
#define EFFECT_CAT_LINK_SIZE "link_size"
#define EFFECT_CAT_PACIFIST "pacifist"
#define EFFECT_CAT_NO_Z "no_z"
#define EFFECT_CAT_WEATHER "weather"
#define EFFECT_CAT_REVERSE_CONTROLS "reverse_controls"
#define EFFECT_CAT_BOOTS "boots"
#define EFFECT_CAT_SPEED "speed"
#define EFFECT_CAT_DAMAGE_TAKEN "damage_taken"
#define EFFECT_CAT_SPAWN_ENEMY "spawn_enemy"
#define EFFECT_CAT_NONE "none"

void CrowdControl::Init() {
    SDLNet_Init();
}

void CrowdControl::Shutdown() {
    SDLNet_Quit();
}

void CrowdControl::Enable() {
    if (isEnabled) {
        return;
    }

    if (SDLNet_ResolveHost(&ip, "127.0.0.1", 43384) == -1) {
        SPDLOG_ERROR("[CrowdControl] SDLNet_ResolveHost: {}", SDLNet_GetError());
    }

    isEnabled = true;
    ccThreadReceive = std::thread(&CrowdControl::ListenToServer, this);
    ccThreadProcess = std::thread(&CrowdControl::ProcessActiveEffects, this);
}

void CrowdControl::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    ccThreadReceive.join();
    ccThreadProcess.join();
}

void CrowdControl::ListenToServer() {
    while (isEnabled) {
        while (!connected && isEnabled) {
            SPDLOG_TRACE("[CrowdControl] Attempting to make connection to server...");
            tcpsock = SDLNet_TCP_Open(&ip);

            if (tcpsock) {
                connected = true;
                SPDLOG_TRACE("[CrowdControl] Connection to server established!");
                break;
            }
        }

        SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
        if (tcpsock) {
            SDLNet_TCP_AddSocket(socketSet, tcpsock);
        }

        // Listen to socket messages
        while (connected && tcpsock && isEnabled) {
            // we check first if socket has data, to not block in the TCP_Recv
            int socketsReady = SDLNet_CheckSockets(socketSet, 0);

            if (socketsReady == -1) {
                SPDLOG_ERROR("[CrowdControl] SDLNet_CheckSockets: {}", SDLNet_GetError());
                break;
            }

            if (socketsReady == 0) {
                continue;
            }

            int len = SDLNet_TCP_Recv(tcpsock, &received, sizeof(received));
            if (!len || !tcpsock || len == -1) {
                SPDLOG_ERROR("[CrowdControl] SDLNet_TCP_Recv: {}", SDLNet_GetError());
                break;
            }

            Effect* incomingEffect = ParseMessage(received);
            if (!incomingEffect) {
                continue;
            }

            // If effect is not a timed effect, execute and return result.
            if (!incomingEffect->timeRemaining) {
                EffectResult result = CrowdControl::ExecuteEffect(incomingEffect);
                EmitMessage(tcpsock, incomingEffect->id, incomingEffect->timeRemaining, result);
            } else {
                // If another timed effect is already active that conflicts with the incoming effect.
                bool isConflictingEffectActive = false;
                for (Effect* effect : activeEffects) {
                    if (effect != incomingEffect && effect->category == incomingEffect->category && effect->id < incomingEffect->id) {
                        isConflictingEffectActive = true;
                        EmitMessage(tcpsock, incomingEffect->id, incomingEffect->timeRemaining, EffectResult::Retry);
                        break;
                    }
                }

                if (!isConflictingEffectActive) {
                    // Check if effect can be applied, if it can't, let CC know.
                    EffectResult result = CrowdControl::CanApplyEffect(incomingEffect);
                    if (result == EffectResult::Retry || result == EffectResult::Failure) {
                        EmitMessage(tcpsock, incomingEffect->id, incomingEffect->timeRemaining, result);
                        continue;
                    }

                    activeEffectsMutex.lock();
                    activeEffects.push_back(incomingEffect);
                    activeEffectsMutex.unlock();
                }
            }
        }

        if (connected) {
            SDLNet_TCP_Close(tcpsock);
            connected = false;
            SPDLOG_TRACE("[CrowdControl] Ending Listen thread...");
        }
    }
}

void CrowdControl::ProcessActiveEffects() {
    while (isEnabled) {
        // We only want to send events when status changes, on start we send Success.
        // If it fails at some point, we send Pause, and when it starts to succeed again we send Success.
        // CC uses this to pause the timer on the overlay.
        activeEffectsMutex.lock();
        auto it = activeEffects.begin();

        while (it != activeEffects.end()) {
            Effect *effect = *it;
            EffectResult result = CrowdControl::ExecuteEffect(effect);

            if (result == EffectResult::Success) {
                // If time remaining has reached 0, we have finished the effect.
                if (effect->timeRemaining <= 0) {
                    it = activeEffects.erase(std::remove(activeEffects.begin(), activeEffects.end(), effect),
                                        activeEffects.end());
                    GameInteractor::RemoveEffect(effect->giEffect);
                    delete effect;
                } else {
                    // If we have a success after previously being paused, tell CC to resume timer.
                    if (effect->isPaused) {
                        effect->isPaused = false;
                        EmitMessage(tcpsock, effect->id, effect->timeRemaining, EffectResult::Resumed);
                    // If not paused before, subtract time from the timer and send a Success event if
                    // the result is different from the last time this was ran.
                    // Timed events are put on a thread that runs once per second.
                    } else {
                        effect->timeRemaining -= 1000;
                        if (result != effect->lastExecutionResult) {
                            effect->lastExecutionResult = result;
                            EmitMessage(tcpsock, effect->id, effect->timeRemaining, EffectResult::Success);
                        }
                    }
                    it++;
                }
            } else { // Timed effects only do Success or Retry
                if (!effect->isPaused && effect->timeRemaining > 0) {
                    effect->isPaused = true;
                    EmitMessage(tcpsock, effect->id, effect->timeRemaining, EffectResult::Paused);
                }
                it++;
            }
        }

        activeEffectsMutex.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    SPDLOG_TRACE("[CrowdControl] Ending Process thread...");
}

// MARK: - Helpers

void CrowdControl::EmitMessage(TCPsocket socket, uint32_t eventId, long timeRemaining, EffectResult status) {
    nlohmann::json payload;

    payload["id"] = eventId;
    payload["type"] = 0;
    payload["timeRemaining"] = timeRemaining;
    payload["status"] = status;

    std::string jsonPayload = payload.dump();
    SDLNet_TCP_Send(socket, jsonPayload.c_str(), jsonPayload.size() + 1);
}

CrowdControl::Effect* CrowdControl::ParseMessage(char payload[512]) {
    nlohmann::json dataReceived = nlohmann::json::parse(payload, nullptr, false);
    if (dataReceived.is_discarded()) {
        SPDLOG_ERROR("Error parsing JSON");
        return nullptr;
    }

    Effect* effect = new Effect();
    effect->lastExecutionResult = EffectResult::Initiate;
    effect->id = dataReceived["id"];
    auto parameters = dataReceived["parameters"];
    auto effectName = dataReceived["code"].get<std::string>();

    if (parameters.size() > 0) {
        effect->value[0] = dataReceived["parameters"][0];
    }

    // Assign GameInteractionEffect + values to CC effect.
    // Categories are mostly used for checking for conflicting timed effects.
    switch (effectStringToEnum[effectName]) {
        case spawnCuccoStorm:
            effect->giEffect = new GameInteractionEffect::SpawnCuccoStorm();
            break;
        case spawnLitBomb:
            effect->giEffect = new GameInteractionEffect::SpawnCuccoStorm();
            break;
        case spawnExplosion:
            effect->giEffect = new GameInteractionEffect::SpawnCuccoStorm();
            break;
    }

    // Spawn Enemies and Objects
    if (effectName == EFFECT_ADD_HEART_CONTAINER) {
        effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
        effect->giEffect->parameter = 1;
    } else if (effectName == EFFECT_REMOVE_HEART_CONTAINER) {
        effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
        effect->giEffect->parameter = -1;
    } else if (effectName == EFFECT_FILL_MAGIC) {
        effect->giEffect = new GameInteractionEffect::FillMagic();
    } else if (effectName == EFFECT_EMPTY_MAGIC) {
        effect->giEffect = new GameInteractionEffect::EmptyMagic();
    } else if (effectName == EFFECT_ADD_RUPEES) {
        effect->giEffect = new GameInteractionEffect::ModifyRupees();
    } else if (effectName == EFFECT_REMOVE_RUPEES) {
        effect->giEffect = new GameInteractionEffect::ModifyRupees();
        effect->paramMultiplier = -1;
    } else if (effectName == EFFECT_NO_UI) {
        effect->category = EFFECT_CAT_UI;
        effect->timeRemaining = 60000;
        effect->giEffect = new GameInteractionEffect::NoUI();
    } else if (effectName == EFFECT_HIGH_GRAVITY) {
        effect->category = EFFECT_CAT_GRAVITY;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyGravity();
        effect->giEffect->parameter = GI_GRAVITY_LEVEL_HEAVY;
    } else if (effectName == EFFECT_LOW_GRAVITY) {
        effect->category = EFFECT_CAT_GRAVITY;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyGravity();
        effect->giEffect->parameter = GI_GRAVITY_LEVEL_LIGHT;
    } else if (effectName == EFFECT_KILL) {
        effect->giEffect = new GameInteractionEffect::SetPlayerHealth();
        effect->value[0] = 0;
    } else if (effectName == EFFECT_FREEZE) {
        effect->giEffect = new GameInteractionEffect::FreezePlayer();
    } else if (effectName == EFFECT_BURN) {
        effect->giEffect = new GameInteractionEffect::BurnPlayer();
    } else if (effectName == EFFECT_ELECTROCUTE) {
        effect->giEffect = new GameInteractionEffect::ElectrocutePlayer();
    } else if (effectName == EFFECT_KNOCKBACK) {
        effect->giEffect = new GameInteractionEffect::KnockbackPlayer();
    } else if (effectName == EFFECT_HEAL) {
        effect->giEffect = new GameInteractionEffect::ModifyHealth();
    } else if (effectName == EFFECT_DAMAGE) {
        effect->giEffect = new GameInteractionEffect::ModifyHealth();
        effect->paramMultiplier = -1;
    } else if (effectName == EFFECT_GIANT_LINK) {
        effect->category = EFFECT_CAT_LINK_SIZE;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
        effect->giEffect->parameter = GI_LINK_SIZE_GIANT;
    } else if (effectName == EFFECT_MINISH_LINK) {
        effect->category = EFFECT_CAT_LINK_SIZE;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
        effect->giEffect->parameter = GI_LINK_SIZE_MINISH;
    } else if (effectName == EFFECT_PAPER_LINK) {
        effect->category = EFFECT_CAT_LINK_SIZE;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
        effect->giEffect->parameter = GI_LINK_SIZE_PAPER;
    } else if (effectName == EFFECT_INVISIBLE_LINK) {
        effect->category = EFFECT_CAT_LINK_SIZE;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::InvisibleLink();
    } else if (effectName == EFFECT_PACIFIST) {
        effect->category = EFFECT_CAT_PACIFIST;
        effect->timeRemaining = 15000;
        effect->giEffect = new GameInteractionEffect::PacifistMode();
    } else if (effectName == EFFECT_NO_Z_TARGETING) {
        effect->category = EFFECT_CAT_NO_Z;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::DisableZTargeting();
    } else if (effectName == EFFECT_RAINSTORM) {
        effect->category = EFFECT_CAT_WEATHER;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::WeatherRainstorm();
    } else if (effectName == EFFECT_REVERSE_CONTROLS) {
        effect->category = EFFECT_CAT_REVERSE_CONTROLS;
        effect->timeRemaining = 60000;
        effect->giEffect = new GameInteractionEffect::ReverseControls();
    } else if (effectName == EFFECT_IRON_BOOTS) {
        effect->category = EFFECT_CAT_BOOTS;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
        effect->giEffect->parameter = PLAYER_BOOTS_IRON;
    } else if (effectName == EFFECT_HOVER_BOOTS) {
        effect->category = EFFECT_CAT_BOOTS;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
        effect->giEffect->parameter = PLAYER_BOOTS_HOVER;;
    } else if (effectName == EFFECT_GIVE_DEKU_SHIELD) {
        effect->giEffect = new GameInteractionEffect::GiveDekuShield();
    } else if (effectName == EFFECT_INCREASE_SPEED) {
        effect->category = EFFECT_CAT_SPEED;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
        effect->giEffect->parameter = 2;
    } else if (effectName == EFFECT_DECREASE_SPEED) {
        effect->category = EFFECT_CAT_SPEED;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
        effect->giEffect->parameter = -2;
    } else if (effectName == EFFECT_OHKO) {
        effect->category = EFFECT_CAT_DAMAGE_TAKEN;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::OneHitKO();
    } else if (effectName == EFFECT_DOUBLE_DAMAGE_TAKEN) {
        effect->category = EFFECT_CAT_DAMAGE_TAKEN;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
        effect->paramMultiplier = -1;
    } else if (effectName == EFFECT_HALF_DAMAGE_TAKEN) {
        effect->category = EFFECT_CAT_DAMAGE_TAKEN;
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
    /*} else if (effectName == EFFECT_SPAWN_CUCCO_STORM) {
        effect->giEffect = new GameInteractionEffect::SpawnCuccoStorm();*/
    } else if (effectName == EFFECT_SPAWN_WALLMASTER) {
        effect->value[0] = ACTOR_EN_WALLMAS;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_ARWING) {
        effect->value[0] = ACTOR_EN_CLEAR_TAG;
        // Parameter for no cutscene Arwing
        effect->value[1] = 1;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_DARK_LINK) {
        effect->value[0] = ACTOR_EN_TORCH2;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_STALFOS) {
        effect->value[0] = ACTOR_EN_TEST;
        // Parameter for gravity-obeying Stalfos
        effect->value[1] = 2;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_WOLFOS) {
        effect->value[0] = ACTOR_EN_WF;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_FREEZARD) {
        effect->value[0] = ACTOR_EN_FZ;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_KEESE) {
        effect->value[0] = ACTOR_EN_FIREFLY;
        // Parameter for normal keese
        effect->value[1] = 2;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_ICE_KEESE) {
        effect->value[0] = ACTOR_EN_FIREFLY;
        // Parameter for ice keese
        effect->value[1] = 4;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_FIRE_KEESE) {
        effect->value[0] = ACTOR_EN_FIREFLY;
        // Parameter for fire keese
        effect->value[1] = 1;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    } else if (effectName == EFFECT_SPAWN_LIKE_LIKE) {
        effect->value[0] = ACTOR_EN_RR;
        effect->category = EFFECT_CAT_SPAWN_ENEMY;
    }

    // If no value is specifically set, default to using whatever CC sends us.
    // Values are used for various things depending on the effect, but they 
    // usually represent the "amount" of an effect. Amount of hearts healed,
    // strength of knockback, etc.
    if (effect->giEffect != NULL) {
        if (!effect->giEffect->parameter && effect->value[0]) {
            effect->giEffect->parameter = effect->value[0] * effect->paramMultiplier;
        }
    }

    if (effect->category == "") {
        effect->category = EFFECT_CAT_NONE;
    }

    return effect;
}

CrowdControl::EffectResult CrowdControl::ExecuteEffect(Effect* effect) {
    GameInteractionEffectQueryResult giResult;
    if (effect->category == EFFECT_CAT_SPAWN_ENEMY) {
        giResult = GameInteractor::RawAction::SpawnEnemyWithOffset(effect->value[0], effect->value[1]);
    } else {
        giResult = GameInteractor::ApplyEffect(effect->giEffect);
    }

    return TranslateGiEnum(giResult);
}

/// Checks if effect can be applied -- should not be used to check for spawn enemy effects.
CrowdControl::EffectResult CrowdControl::CanApplyEffect(Effect* effect) {
    assert(effect->category != EFFECT_CAT_SPAWN_ENEMY);
    GameInteractionEffectQueryResult giResult = GameInteractor::CanApplyEffect(effect->giEffect);

    return TranslateGiEnum(giResult);
}

CrowdControl::EffectResult CrowdControl::TranslateGiEnum(GameInteractionEffectQueryResult giResult) {
    // Translate GameInteractor result into CC's own enums.
    EffectResult result;
    if (giResult == GameInteractionEffectQueryResult::Possible) {
        result = EffectResult::Success;
    } else if (giResult == GameInteractionEffectQueryResult::TemporarilyNotPossible) {
        result = EffectResult::Retry;
    } else {
        result = EffectResult::Failure;
    }

    return result;
}

#endif

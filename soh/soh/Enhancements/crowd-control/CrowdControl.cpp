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

#define EFFECT_HIGH_GRAVITY "high_gravity"
#define EFFECT_LOW_GRAVITY "low_gravity"
#define EFFECT_DAMAGE_MULTIPLIER "damage_multiplier"
#define EFFECT_DEFENSE_MULTIPLIER "defense_multiplier"
#define EFFECT_GIANT_LINK "giant_link"
#define EFFECT_MINISH_LINK "minish_link"
#define EFFECT_INVISIBLE_LINK "invisible"
#define EFFECT_PAPER_LINK "paper_link"
#define EFFECT_FREEZE "freeze"
#define EFFECT_DAMAGE "damage"
#define EFFECT_HEAL "heal"
#define EFFECT_KNOCKBACK "knockback"
#define EFFECT_ELECTROCUTE "electrocute"
#define EFFECT_BURN "burn"
#define EFFECT_KILL "kill"
#define EFFECT_HOVER_BOOTS "hover_boots"
#define EFFECT_IRON_BOOTS "iron_boots"
#define EFFECT_ADD_HEART_CONTAINER "add_heart_container"
#define EFFECT_REMOVE_HEART_CONTAINER "remove_heart_container"
#define EFFECT_NO_UI "no_ui"
#define EFFECT_FILL_MAGIC "fill_magic"
#define EFFECT_EMPTY_MAGIC "empty_magic"
#define EFFECT_OHKO "ohko"
#define EFFECT_PACIFIST "pacifist"
#define EFFECT_RAINSTORM "rainstorm"
#define EFFECT_REVERSE_CONTROLS "reverse"
#define EFFECT_ADD_RUPEES "add_rupees"
#define EFFECT_REMOVE_RUPEES "remove_rupees"
#define EFFECT_INCREASE_SPEED "increase_speed"
#define EFFECT_DECREASE_SPEED "decrease_speed"
#define EFFECT_NO_Z_TARGETING "no_z"
#define EFFECT_GIVE_DEKU_SHIELD "give_dekushield"

#define EFFECT_SPAWN_WALLMASTER "spawn_wallmaster"
#define EFFECT_SPAWN_ARWING "spawn_arwing"
#define EFFECT_SPAWN_DARK_LINK "spawn_darklink"
#define EFFECT_SPAWN_STALFOS "spawn_stalfos"
#define EFFECT_SPAWN_WOLFOS "spawn_wolfos"
#define EFFECT_SPAWN_FREEZARD "spawn_freezard"
#define EFFECT_SPAWN_KEESE "spawn_keese"
#define EFFECT_SPAWN_ICE_KEESE "spawn_icekeese"
#define EFFECT_SPAWN_FIRE_KEESE "spawn_firekeese"
#define EFFECT_SPAWN_TEKTITE "spawn_tektite"
#define EFFECT_SPAWN_LIKE_LIKE "spawn_likelike"
#define EFFECT_SPAWN_CUCCO_STORM "cucco_storm"


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
                // If another timed effect is already active that conflicts with the
                // incoming effect, let CC retry later.
                bool isConflictingEffectActive = false;
                for (Effect* pack : activeEffects) {
                    if (pack != incomingEffect && pack->category == incomingEffect->category &&
                        pack->id < incomingEffect->id) {
                        isConflictingEffectActive = true;
                        EmitMessage(tcpsock, incomingEffect->id, incomingEffect->timeRemaining,
                                    EffectResult::Retry);
                        break;
                    }
                }

                // Execute effect. If it can't, let CC know.
                EffectResult result = CrowdControl::ExecuteEffect(incomingEffect);
                if (result == EffectResult::Retry || result == EffectResult::Failure) {
                    EmitMessage(tcpsock, incomingEffect->id, incomingEffect->timeRemaining, result);
                    continue;
                }

                if (!isConflictingEffectActive) {
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
    if (effectName == EFFECT_ADD_HEART_CONTAINER) {
        effect->giEffect = new GameInteractionEffect::AddHeartContainer();
    } else if (effectName == EFFECT_REMOVE_HEART_CONTAINER) {
        effect->giEffect = new GameInteractionEffect::RemoveHeartContainer();
    } else if (effectName == EFFECT_FILL_MAGIC) {
        effect->giEffect = new GameInteractionEffect::FillMagic();
    } else if (effectName == EFFECT_EMPTY_MAGIC) {
        effect->giEffect = new GameInteractionEffect::EmptyMagic();
    } else if (effectName == EFFECT_ADD_RUPEES) {
        effect->giEffect = new GameInteractionEffect::GiveRupees();
    } else if (effectName == EFFECT_REMOVE_RUPEES) {
        effect->giEffect = new GameInteractionEffect::TakeRupees();
    } else if (effectName == EFFECT_NO_UI) {
        effect->category = "ui";
        effect->timeRemaining = 60000;
        effect->giEffect = new GameInteractionEffect::NoUI();
    } else if (effectName == EFFECT_HIGH_GRAVITY) {
        effect->category = "gravity";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::HighGravity();
    } else if (effectName == EFFECT_LOW_GRAVITY) {
        effect->category = "gravity";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::LowGravity();
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
        effect->giEffect = new GameInteractionEffect::GiveHealth();
    } else if (effectName == EFFECT_DAMAGE) {
        effect->giEffect = new GameInteractionEffect::TakeHealth();
    } else if (effectName == EFFECT_GIANT_LINK) {
        effect->category = "link_size";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::GiantLink();
    } else if (effectName == EFFECT_MINISH_LINK) {
        effect->category = "link_size";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::MinishLink();
    } else if (effectName == EFFECT_PAPER_LINK) {
        effect->category = "link_size";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::PaperLink();
    } else if (effectName == EFFECT_INVISIBLE_LINK) {
        effect->category = "link_size";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::InvisibleLink();
    } else if (effectName == EFFECT_PACIFIST) {
        effect->category = "pacifist";
        effect->timeRemaining = 15000;
        effect->giEffect = new GameInteractionEffect::PacifistMode();
    } else if (effectName == EFFECT_NO_Z_TARGETING) {
        effect->category = "no_z";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::DisableZTargeting();
    } else if (effectName == EFFECT_RAINSTORM) {
        effect->category = "weather";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::WeatherRainstorm();
    } else if (effectName == EFFECT_REVERSE_CONTROLS) {
        effect->category = "reverse_controls";
        effect->timeRemaining = 60000;
        effect->giEffect = new GameInteractionEffect::ReverseControls();
    } else if (effectName == EFFECT_IRON_BOOTS) {
        effect->category = "boots";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ForceIronBoots();
    } else if (effectName == EFFECT_HOVER_BOOTS) {
        effect->category = "boots";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::ForceHoverBoots();
    } else if (effectName == EFFECT_GIVE_DEKU_SHIELD) {
        effect->giEffect = new GameInteractionEffect::GiveDekuShield();
    } else if (effectName == EFFECT_INCREASE_SPEED) {
        effect->category = "speed";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::IncreaseRunSpeed();
    } else if (effectName == EFFECT_DECREASE_SPEED) {
        effect->category = "speed";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::DecreaseRunSpeed();
    } else if (effectName == EFFECT_OHKO) {
        effect->category = "damage_taken";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::OneHitKO();
    } else if (effectName == EFFECT_DAMAGE_MULTIPLIER) {
        effect->category = "damage_taken";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::IncreaseDamageTaken();
    } else if (effectName == EFFECT_DEFENSE_MULTIPLIER) {
        effect->category = "damage_taken";
        effect->timeRemaining = 30000;
        effect->giEffect = new GameInteractionEffect::DecreaseDamageTaken();
    } else if (effectName == EFFECT_SPAWN_CUCCO_STORM) {
        effect->giEffect = new GameInteractionEffect::SpawnCuccoStorm();
    } else if (effectName == EFFECT_SPAWN_WALLMASTER) {
        effect->value[0] = ACTOR_EN_WALLMAS;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_ARWING) {
        effect->value[0] = ACTOR_EN_CLEAR_TAG;
        // Parameter for no cutscene Arwing
        effect->value[1] = 1;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_DARK_LINK) {
        effect->value[0] = ACTOR_EN_TORCH2;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_STALFOS) {
        effect->value[0] = ACTOR_EN_TEST;
        // Parameter for gravity-obeying Stalfos
        effect->value[1] = 2;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_WOLFOS) {
        effect->value[0] = ACTOR_EN_WF;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_FREEZARD) {
        effect->value[0] = ACTOR_EN_FZ;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_KEESE) {
        effect->value[0] = ACTOR_EN_FIREFLY;
        // Parameter for normal keese
        effect->value[1] = 2;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_ICE_KEESE) {
        effect->value[0] = ACTOR_EN_FIREFLY;
        // Parameter for ice keese
        effect->value[1] = 4;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_FIRE_KEESE) {
        effect->value[0] = ACTOR_EN_FIREFLY;
        // Parameter for fire keese
        effect->value[1] = 1;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_TEKTITE) {
        effect->value[0] = ACTOR_EN_TITE;
        effect->category = "spawn_enemy";
    } else if (effectName == EFFECT_SPAWN_LIKE_LIKE) {
        effect->value[0] = ACTOR_EN_RR;
        effect->category = "spawn_enemy";
    }

    // If no value is specifically set, default to using whatever CC sends us.
    // Values are used for various things depending on the effect, but they 
    // usually represent the "amount" of an effect. Amount of hearts healed,
    // strength of knockback, etc.
    if (effect->giEffect != NULL) {
        if (!effect->giEffect->parameter && effect->value[0]) {
            effect->giEffect->parameter = effect->value[0];
        }
    }

    if (effect->category == "") {
        effect->category = "none";
    }

    return effect;
}

CrowdControl::EffectResult CrowdControl::ExecuteEffect(Effect* effect) {

    GameInteractionEffectQueryResult giResult;
    if (effect->category == "spawn_enemy") {
        giResult = GameInteractor::RawAction::SpawnEnemyWithOffset(effect->value[0], effect->value[1]);
    } else {
        giResult = GameInteractor::ApplyEffect(effect->giEffect);
    }

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

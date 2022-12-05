#ifdef ENABLE_CROWD_CONTROL

#include "CrowdControl.h"
#include <Cvar.h>
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

#include "../debugconsole.h"

#define CMD_EXECUTE SohImGui::GetConsole()->Dispatch

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
        while (!connected) {
            SPDLOG_TRACE("[CrowdControl] Attempting to make connection to server...");
            tcpsock = SDLNet_TCP_Open(&ip);

            if (tcpsock) {
                connected = true;
                SPDLOG_TRACE("[CrowdControl] Connection to server established!");
                break;
            }
        }

        auto socketSet = SDLNet_AllocSocketSet(1);
        SDLNet_TCP_AddSocket(socketSet, tcpsock);

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

            // If effect is a one off run, let's execute
            if (!incomingEffect->timeRemaining) {
                EffectResult result =
                ExecuteEffect(incomingEffect->type.c_str(), incomingEffect->value, false);
                EmitMessage(tcpsock, incomingEffect->id, incomingEffect->timeRemaining, result);
            } else {
                // check if a conflicting event is already active
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

                // check if effect can be executed
                EffectResult result =
                ExecuteEffect(incomingEffect->type.c_str(), incomingEffect->value, true);
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
        // we only want to send events when status changes, on start we send Success,
        // if it fails at some point, we send Pause, and when it starts to succeed again we send Success.
        activeEffectsMutex.lock();
        auto it = activeEffects.begin();

        while (it != activeEffects.end()) {
            Effect *effect = *it;
            EffectResult result = ExecuteEffect(effect->type.c_str(), effect->value, false);
            if (result == EffectResult::Success) {
                // If time remaining has reached 0, we have finished the effect
                if (effect->timeRemaining <= 0) {
                    it = activeEffects.erase(std::remove(activeEffects.begin(), activeEffects.end(), effect),
                                        activeEffects.end());
                    RemoveEffect(effect->type.c_str());

                    delete effect;
                } else {
                    // If we have a success after previously being paused, fire Resume event
                    if (effect->isPaused) {
                        effect->isPaused = false;
                        EmitMessage(tcpsock, effect->id, effect->timeRemaining, EffectResult::Resumed);
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
    if (parameters.size() > 0) {
        effect->value = dataReceived["parameters"][0];
    }
    effect->type = dataReceived["code"].get<std::string>();

    if (effect->type == EFFECT_HIGH_GRAVITY || effect->type == EFFECT_LOW_GRAVITY) {
        effect->category = "gravity";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_DAMAGE_MULTIPLIER || effect->type == EFFECT_DEFENSE_MULTIPLIER) {
        effect->category = "defense";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_GIANT_LINK || effect->type == EFFECT_MINISH_LINK ||
               effect->type == EFFECT_INVISIBLE_LINK || effect->type == EFFECT_PAPER_LINK) {
        effect->category = "link_size";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_FREEZE || effect->type == EFFECT_DAMAGE || effect->type == EFFECT_HEAL ||
               effect->type == EFFECT_KNOCKBACK || effect->type == EFFECT_ELECTROCUTE ||
               effect->type == EFFECT_BURN || effect->type == EFFECT_KILL) {
        effect->category = "link_damage";
    } else if (effect->type == EFFECT_HOVER_BOOTS || effect->type == EFFECT_IRON_BOOTS) {
        effect->category = "boots";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_ADD_HEART_CONTAINER || effect->type == EFFECT_REMOVE_HEART_CONTAINER) {
        effect->category = "heart_container";
    } else if (effect->type == EFFECT_NO_UI) {
        effect->category = "ui";
        effect->timeRemaining = 60000;
    } else if (effect->type == EFFECT_FILL_MAGIC || effect->type == EFFECT_EMPTY_MAGIC) {
        effect->category = "magic";
    } else if (effect->type == EFFECT_OHKO) {
        effect->category = "ohko";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_PACIFIST) {
        effect->category = "pacifist";
        effect->timeRemaining = 15000;
    } else if (effect->type == EFFECT_RAINSTORM) {
        effect->category = "weather";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_REVERSE_CONTROLS) {
        effect->category = "controls";
        effect->timeRemaining = 60000;
    } else if (effect->type == EFFECT_ADD_RUPEES || effect->type == EFFECT_REMOVE_RUPEES) {
        effect->category = "rupees";
    } else if (effect->type == EFFECT_INCREASE_SPEED || effect->type == EFFECT_DECREASE_SPEED) {
        effect->category = "speed";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_NO_Z_TARGETING) {
        effect->category = "no_z";
        effect->timeRemaining = 30000;
    } else if (effect->type == EFFECT_SPAWN_WALLMASTER || effect->type == EFFECT_SPAWN_ARWING ||
               effect->type == EFFECT_SPAWN_DARK_LINK || effect->type == EFFECT_SPAWN_STALFOS ||
               effect->type == EFFECT_SPAWN_WOLFOS || effect->type == EFFECT_SPAWN_FREEZARD ||
               effect->type == EFFECT_SPAWN_KEESE || effect->type == EFFECT_SPAWN_ICE_KEESE ||
               effect->type == EFFECT_SPAWN_FIRE_KEESE || effect->type == EFFECT_SPAWN_TEKTITE ||
               effect->type == EFFECT_SPAWN_LIKE_LIKE || effect->type == EFFECT_SPAWN_CUCCO_STORM) {
        effect->category = "spawn";
    } else {
        effect->category = "none";
        effect->timeRemaining = 0;
    }

    return effect;
}

CrowdControl::EffectResult CrowdControl::ExecuteEffect(std::string effectId, uint32_t value, bool dryRun) {
    // Don't execute effect and don't advance timer when the player is not in a proper loaded savefile
    // and when they're busy dying.
    if (gPlayState == NULL || gPlayState->gameOverCtx.state > 0 || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) {
        return EffectResult::Retry;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (player != NULL) {
        if (effectId == EFFECT_ADD_HEART_CONTAINER) {
            if (gSaveContext.healthCapacity >= 0x140) {
                return EffectResult::Failure;
            }
            
            if (dryRun == 0) CMD_EXECUTE(EFFECT_ADD_HEART_CONTAINER);
            return EffectResult::Success;
        } else if (effectId == EFFECT_REMOVE_HEART_CONTAINER) {
            if ((gSaveContext.healthCapacity - 0x10) <= 0) {
                return EffectResult::Failure;
            }
            
            if (dryRun == 0) CMD_EXECUTE(EFFECT_REMOVE_HEART_CONTAINER);
            return EffectResult::Success;
        } else if (effectId == EFFECT_FILL_MAGIC) {
            if (!gSaveContext.isMagicAcquired) {
                return EffectResult::Failure;
            }

            if (gSaveContext.magic >= (gSaveContext.isDoubleMagicAcquired + 1) + 0x30) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE(EFFECT_FILL_MAGIC);
            return EffectResult::Success;
        } else if (effectId == EFFECT_EMPTY_MAGIC) {
            if (!gSaveContext.isMagicAcquired || gSaveContext.magic <= 0) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE(EFFECT_EMPTY_MAGIC);
            return EffectResult::Success;
        } else if (effectId == EFFECT_ADD_RUPEES) {
            if (dryRun == 0) CMD_EXECUTE(fmt::format("update_rupees {}", value));
            return EffectResult::Success;
        } else if (effectId == EFFECT_REMOVE_RUPEES) {
            if (gSaveContext.rupees - value < 0) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE(fmt::format("update_rupees -{}", value));
            return EffectResult::Success;
        }
    }

    if (player != NULL && !Player_InBlockingCsMode(gPlayState, player) && gPlayState->pauseCtx.state == 0
                                                                       && gPlayState->msgCtx.msgMode == 0) {
        if (effectId == EFFECT_HIGH_GRAVITY) {
            if (dryRun == 0) CMD_EXECUTE("gravity 2");
            return EffectResult::Success;
        } else if (effectId == EFFECT_LOW_GRAVITY) {
            if (dryRun == 0) CMD_EXECUTE("gravity 0");
            return EffectResult::Success;
        } else if (effectId == EFFECT_KILL
                    || effectId == EFFECT_FREEZE
                    || effectId == EFFECT_BURN
                    || effectId == EFFECT_ELECTROCUTE
                    || effectId == EFFECT_SPAWN_CUCCO_STORM
        ) {
            if (PlayerGrounded(player)) {
                if (dryRun == 0) CMD_EXECUTE(fmt::format("{}", effectId));
                return EffectResult::Success;
            }
            return EffectResult::Failure;
        } else if (effectId == EFFECT_HEAL
                    || effectId == EFFECT_KNOCKBACK
        ) {
            if (dryRun == 0) CMD_EXECUTE(fmt::format("{} {}", effectId, value));
            return EffectResult::Success;
        } else if (effectId == EFFECT_GIANT_LINK
                    || effectId == EFFECT_MINISH_LINK
                    || effectId == EFFECT_NO_UI
                    || effectId == EFFECT_INVISIBLE_LINK
                    || effectId == EFFECT_PAPER_LINK
                    || effectId == EFFECT_NO_Z_TARGETING
                    || effectId == EFFECT_OHKO
                    || effectId == EFFECT_PACIFIST
                    || effectId == EFFECT_RAINSTORM
        ) {
            if (dryRun == 0) CMD_EXECUTE(fmt::format("{} 1", effectId));
            return EffectResult::Success;
        } else if (effectId == EFFECT_REVERSE_CONTROLS) {
            if (dryRun == 0) CMD_EXECUTE("reverse_controls 1"); 
            return EffectResult::Success;
        } else if (effectId == EFFECT_IRON_BOOTS) {
            if (dryRun == 0) CMD_EXECUTE("boots iron");
            return EffectResult::Success;
        } else if (effectId == EFFECT_HOVER_BOOTS) {
            if (dryRun == 0) CMD_EXECUTE("boots hover");
            return EffectResult::Success;
        } else if (effectId == "give_dekushield") {
            if (dryRun == 0) CMD_EXECUTE("givedekushield");
            return EffectResult::Success;
        } else if (effectId == EFFECT_SPAWN_WALLMASTER 
            || effectId == EFFECT_SPAWN_ARWING 
            || effectId == EFFECT_SPAWN_DARK_LINK 
            || effectId == EFFECT_SPAWN_STALFOS 
            || effectId == EFFECT_SPAWN_WOLFOS 
            || effectId == EFFECT_SPAWN_FREEZARD 
            || effectId == EFFECT_SPAWN_KEESE 
            || effectId == EFFECT_SPAWN_ICE_KEESE
            || effectId == EFFECT_SPAWN_FIRE_KEESE
            || effectId == EFFECT_SPAWN_TEKTITE
            || effectId == EFFECT_SPAWN_LIKE_LIKE
        ) {
            if (dryRun == 0) {
                if (CrowdControl::SpawnEnemy(effectId)) {
                    return EffectResult::Success;
                } else {
                    return EffectResult::Failure;
                }
            }
            return EffectResult::Success;
        } else if (effectId == EFFECT_INCREASE_SPEED) {
           if (dryRun == 0) CMD_EXECUTE("speed_modifier 2");
            return EffectResult::Success;
        } else if (effectId == EFFECT_DECREASE_SPEED) {
           if (dryRun == 0) CMD_EXECUTE("speed_modifier -2");
            return EffectResult::Success;
        } else if (effectId == EFFECT_DAMAGE_MULTIPLIER) {
            if (dryRun == 0) CMD_EXECUTE(fmt::format("defense_modifier -{}", value));
            return EffectResult::Success;
        } else if (effectId == EFFECT_DEFENSE_MULTIPLIER) {
            if (dryRun == 0) CMD_EXECUTE(fmt::format("defense_modifier {}", value));
            return EffectResult::Success;
        } else if (effectId == EFFECT_DAMAGE) {
            if ((gSaveContext.healthCapacity - 0x10) <= 0) {
                return EffectResult::Failure;
            }
            
            if (dryRun == 0) CMD_EXECUTE(fmt::format("{} {}", effectId, value));
            return EffectResult::Success;
        }
    }

    return EffectResult::Retry;
}

bool CrowdControl::SpawnEnemy(std::string effectId) {
    Player* player = GET_PLAYER(gPlayState);

    int enemyId = 0;
    int enemyParams = 0;
    float posXOffset = 0;
    float posYOffset = 0;
    float posZOffset = 0;

    if (effectId == EFFECT_SPAWN_WALLMASTER) {
        enemyId = 17;
    } else if (effectId == EFFECT_SPAWN_ARWING) {
        enemyId = 315;
        enemyParams = 1;
        posYOffset = 100;
    } else if (effectId == EFFECT_SPAWN_DARK_LINK) {
        enemyId = 51;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_STALFOS) {
        enemyId = 2;
        enemyParams = 2;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_WOLFOS) {
        enemyId = 431;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_FREEZARD) {
        enemyId = 289;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_KEESE) {
        enemyId = 19;
        enemyParams = 2;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_ICE_KEESE) {
        enemyId = 19;
        enemyParams = 4;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_FIRE_KEESE) {
        enemyId = 19;
        enemyParams = 1;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_TEKTITE) {
        enemyId = 27;
        posXOffset = 75;
        posYOffset = 50;
    } else if (effectId == EFFECT_SPAWN_LIKE_LIKE) {
        enemyId = 221;
        posXOffset = 75;
        posYOffset = 50;
    }

    return Actor_Spawn(&gPlayState->actorCtx, gPlayState, enemyId, player->actor.world.pos.x + posXOffset,
        player->actor.world.pos.y + posYOffset, player->actor.world.pos.z + posZOffset, 0, 0, 0, enemyParams);

}

void CrowdControl::RemoveEffect(std::string effectId) {
    if (gPlayState == NULL) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (player != NULL) {
        if (effectId == EFFECT_GIANT_LINK
                || effectId == EFFECT_MINISH_LINK
                || effectId == EFFECT_NO_UI
                || effectId == EFFECT_INVISIBLE_LINK
                || effectId == EFFECT_PAPER_LINK
                || effectId == EFFECT_NO_Z_TARGETING
                || effectId == EFFECT_OHKO
                || effectId == EFFECT_PACIFIST
                || effectId == EFFECT_RAINSTORM
        ) {
            CMD_EXECUTE(fmt::format("{} 0", effectId));
            return;
        } else if (effectId == EFFECT_IRON_BOOTS || effectId == EFFECT_HOVER_BOOTS) {
            CMD_EXECUTE("boots kokiri");
            return;
        } else if (effectId == EFFECT_HIGH_GRAVITY || effectId == EFFECT_LOW_GRAVITY) {
            CMD_EXECUTE("gravity 1");
            return;
        } else if (effectId == EFFECT_REVERSE_CONTROLS) {
            CMD_EXECUTE("reverse_controls 0");
            return;
        } else if (effectId == EFFECT_INCREASE_SPEED
                    || effectId == EFFECT_DECREASE_SPEED
        ) {
            CMD_EXECUTE("speed_modifier 0");
            return;
        } else if (effectId == EFFECT_DAMAGE_MULTIPLIER
                    || effectId == EFFECT_DEFENSE_MULTIPLIER
        ) {
            CMD_EXECUTE("defense_modifier 0");
            return;
        }
    }
}
#endif

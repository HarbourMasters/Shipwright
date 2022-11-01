#ifdef ENABLE_CROWD_CONTROL

#include "CrowdControl.h"
#include <libultraship/Cvar.h>
#include <libultraship/Console.h>
#include <libultraship/ImGuiImpl.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <regex>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;
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

        // Listen to socket messages
        while (connected && tcpsock && isEnabled) {
            auto socketSet = SDLNet_AllocSocketSet(1);
            SDLNet_TCP_AddSocket(socketSet, tcpsock);

            // we check first if socket has data, to not block in the TCP_Recv
            int socketsReady = SDLNet_CheckSockets(socketSet, 0);

            if (socketsReady == -1) {
                SPDLOG_ERROR("[CrowdControl] SDLNet_CheckSockets: {}", SDLNet_GetError());
                break;
            }

            if (socketsReady == 0) {
                SPDLOG_TRACE("[CrowdControl] No sockets ready to be read");
                continue;
            }

            int len = SDLNet_TCP_Recv(tcpsock, &received, sizeof(received));
            if (!len || !tcpsock || len == -1) {
                SPDLOG_ERROR("[CrowdControl] SDLNet_TCP_Recv: {}", SDLNet_GetError());
                break;
            }

            try {
                CCPacket* incomingEffect = ParseMessage(received);

                // If effect is a one off run, let's execute
                if (!incomingEffect->timeRemaining) {
                    EffectResult result =
                        ExecuteEffect(incomingEffect->effectType.c_str(), incomingEffect->effectValue, false);
                    EmitMessage(tcpsock, incomingEffect->packetId, incomingEffect->timeRemaining, result);
                } else {
                    // check if a conflicting event is already active
                    bool isConflictingEffectActive = false;
                    for (CCPacket* pack : activeEffects) {
                        if (pack != incomingEffect && pack->effectCategory == incomingEffect->effectCategory &&
                            pack->packetId < incomingEffect->packetId) {
                            isConflictingEffectActive = true;
                            EmitMessage(tcpsock, incomingEffect->packetId, incomingEffect->timeRemaining,
                                        EffectResult::Retry);

                            break;
                        }
                    }

                    // check if effect can be executed
                    EffectResult result =
                        ExecuteEffect(incomingEffect->effectType.c_str(), incomingEffect->effectValue, true);
                    if (result == EffectResult::Retry || result == EffectResult::Failure) {
                        EmitMessage(tcpsock, incomingEffect->packetId, incomingEffect->timeRemaining, result);
                        continue;
                    }

                    if (!isConflictingEffectActive) {
                        activeEffectsMutex.lock();
                        activeEffects.push_back(incomingEffect);
                        SPDLOG_TRACE("[CrowdControl] {} effects active now", activeEffects.size());
                        activeEffectsMutex.unlock();
                    }
                }
            } catch (nlohmann::json::parse_error& e) { SPDLOG_ERROR("Error parsing JSON: {}", e.what()); }
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
            CCPacket *effect = *it;
            EffectResult result = ExecuteEffect(effect->effectType.c_str(), effect->effectValue, false);
            if (result == EffectResult::Success) {
                // If time remaining has reached 0, we have finished the effect
                if (effect->timeRemaining <= 0) {
                    activeEffectsMutex.lock();
                    it = activeEffects.erase(std::remove(activeEffects.begin(), activeEffects.end(), effect),
                                        activeEffects.end());
                    activeEffectsMutex.unlock();
                    RemoveEffect(effect->effectType.c_str());

                    delete effect;
                } else {
                    // If we have a success after previously being paused, fire Resume event
                    if (effect->isPaused) {
                        effect->isPaused = false;
                        EmitMessage(tcpsock, effect->packetId, effect->timeRemaining, EffectResult::Resumed);
                    } else {
                        effect->timeRemaining -= 1000;
                        if (result != effect->lastExecutionResult) {
                            effect->lastExecutionResult = result;
                            EmitMessage(tcpsock, effect->packetId, effect->timeRemaining, EffectResult::Success);
                        }
                    }

                    it++;
                }
            } else { // Timed effects only do Success or Retry
                if (!effect->isPaused && effect->timeRemaining > 0) {
                    effect->isPaused = true;
                    EmitMessage(tcpsock, effect->packetId, effect->timeRemaining, EffectResult::Paused);
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

CrowdControl::CCPacket* CrowdControl::ParseMessage(char payload[512]) {
    nlohmann::json dataReceived = nlohmann::json::parse(payload);
    CCPacket* packet = new CCPacket();

    packet->packetId = dataReceived["id"];
    auto parameters = dataReceived["parameters"];
    if (parameters.size() > 0) {
        packet->effectValue = dataReceived["parameters"][0];
    }
    packet->effectType = dataReceived["code"].get<std::string>();


    if (packet->effectType == EFFECT_HIGH_GRAVITY || packet->effectType == EFFECT_LOW_GRAVITY) {
        packet->effectCategory = "gravity";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_DAMAGE_MULTIPLIER || packet->effectType == EFFECT_DEFENSE_MULTIPLIER) {
        packet->effectCategory = "defense";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_GIANT_LINK || packet->effectType == EFFECT_MINISH_LINK ||
               packet->effectType == EFFECT_INVISIBLE_LINK || packet->effectType == EFFECT_PAPER_LINK) {
        packet->effectCategory = "link_size";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_FREEZE || packet->effectType == EFFECT_DAMAGE || packet->effectType == EFFECT_HEAL ||
               packet->effectType == EFFECT_KNOCKBACK || packet->effectType == EFFECT_ELECTROCUTE ||
               packet->effectType == EFFECT_BURN || packet->effectType == EFFECT_KILL) {
        packet->effectCategory = "link_damage";
    } else if (packet->effectType == EFFECT_HOVER_BOOTS || packet->effectType == EFFECT_IRON_BOOTS) {
        packet->effectCategory = "boots";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_ADD_HEART_CONTAINER || packet->effectType == EFFECT_REMOVE_HEART_CONTAINER) {
        packet->effectCategory = "heart_container";
    } else if (packet->effectType == EFFECT_NO_UI) {
        packet->effectCategory = "ui";
        packet->timeRemaining = 60000;
    } else if (packet->effectType == EFFECT_FILL_MAGIC || packet->effectType == EFFECT_EMPTY_MAGIC) {
        packet->effectCategory = "magic";
    } else if (packet->effectType == EFFECT_OHKO) {
        packet->effectCategory = "ohko";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_PACIFIST) {
        packet->effectCategory = "pacifist";
        packet->timeRemaining = 15000;
    } else if (packet->effectType == EFFECT_RAINSTORM) {
        packet->effectCategory = "weather";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_REVERSE_CONTROLS) {
        packet->effectCategory = "controls";
        packet->timeRemaining = 60000;
    } else if (packet->effectType == EFFECT_ADD_RUPEES || packet->effectType == EFFECT_REMOVE_RUPEES) {
        packet->effectCategory = "rupees";
    } else if (packet->effectType == EFFECT_INCREASE_SPEED || packet->effectType == EFFECT_DECREASE_SPEED) {
        packet->effectCategory = "speed";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_NO_Z_TARGETING) {
        packet->effectCategory = "no_z";
        packet->timeRemaining = 30000;
    } else if (packet->effectType == EFFECT_SPAWN_WALLMASTER || packet->effectType == EFFECT_SPAWN_ARWING ||
               packet->effectType == EFFECT_SPAWN_DARK_LINK || packet->effectType == EFFECT_SPAWN_STALFOS ||
               packet->effectType == EFFECT_SPAWN_WOLFOS || packet->effectType == EFFECT_SPAWN_FREEZARD ||
               packet->effectType == EFFECT_SPAWN_KEESE || packet->effectType == EFFECT_SPAWN_ICE_KEESE ||
               packet->effectType == EFFECT_SPAWN_FIRE_KEESE || packet->effectType == EFFECT_SPAWN_TEKTITE ||
               packet->effectType == EFFECT_SPAWN_LIKE_LIKE || packet->effectType == EFFECT_SPAWN_CUCCO_STORM) {
        packet->effectCategory = "spawn";
    } else {
        packet->effectCategory = "none";
        packet->timeRemaining = 0;
    }

    return packet;
}

CrowdControl::EffectResult CrowdControl::ExecuteEffect(std::string effectId, uint32_t value, bool dryRun) {
    // Don't execute effect and don't advance timer when the player is not in a proper loaded savefile
    // and when they're busy dying.
    if (gGlobalCtx == NULL || gGlobalCtx->gameOverCtx.state > 0 || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) {
        return EffectResult::Retry;
    }

    Player* player = GET_PLAYER(gGlobalCtx);

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
            if (!gSaveContext.magicAcquired) {
                return EffectResult::Failure;
            }

            if (gSaveContext.magic >= (gSaveContext.doubleMagic + 1) + 0x30) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE(EFFECT_FILL_MAGIC);
            return EffectResult::Success;
        } else if (effectId == EFFECT_EMPTY_MAGIC) {
            if (!gSaveContext.magicAcquired || gSaveContext.magic <= 0) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE(EFFECT_EMPTY_MAGIC);
            return EffectResult::Success;
        } else if (effectId == EFFECT_ADD_RUPEES) {
            if (dryRun == 0) CMD_EXECUTE(std::format("update_rupees {}", value));
            return EffectResult::Success;
        } else if (effectId == EFFECT_REMOVE_RUPEES) {
            if (gSaveContext.rupees - value < 0) {
                return EffectResult::Failure;
            }

            if (dryRun == 0) CMD_EXECUTE(std::format("update_rupees -{}", value));
            return EffectResult::Success;
        }
    }

    if (player != NULL && !Player_InBlockingCsMode(gGlobalCtx, player) && gGlobalCtx->pauseCtx.state == 0
                                                                       && gGlobalCtx->msgCtx.msgMode == 0) {
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
                if (dryRun == 0) CMD_EXECUTE(std::format("{}", effectId));
                return EffectResult::Success;
            }
            return EffectResult::Failure;
        } else if (effectId == EFFECT_HEAL
                    || effectId == EFFECT_KNOCKBACK
        ) {
            if (dryRun == 0) CMD_EXECUTE(std::format("{} {}", effectId, value));
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
            if (dryRun == 0) CMD_EXECUTE(std::format("{} 1", effectId));
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
            if (dryRun == 0) CMD_EXECUTE(std::format("defense_modifier -{}", value));
            return EffectResult::Success;
        } else if (effectId == EFFECT_DEFENSE_MULTIPLIER) {
            if (dryRun == 0) CMD_EXECUTE(std::format("defense_modifier {}", value));
            return EffectResult::Success;
        } else if (effectId == EFFECT_DAMAGE) {
            if ((gSaveContext.healthCapacity - 0x10) <= 0) {
                return EffectResult::Failure;
            }
            
            if (dryRun == 0) CMD_EXECUTE(std::format("{} {}", effectId, value));
            return EffectResult::Success;
        }
    }

    return EffectResult::Retry;
}

bool CrowdControl::SpawnEnemy(std::string effectId) {
    Player* player = GET_PLAYER(gGlobalCtx);

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

    return Actor_Spawn(&gGlobalCtx->actorCtx, gGlobalCtx, enemyId, player->actor.world.pos.x + posXOffset,
        player->actor.world.pos.y + posYOffset, player->actor.world.pos.z + posZOffset, 0, 0, 0, enemyParams);

}

void CrowdControl::RemoveEffect(std::string effectId) {
    if (gGlobalCtx == NULL) {
        return;
    }

    Player* player = GET_PLAYER(gGlobalCtx);

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
            CMD_EXECUTE(std::format("{} 0", effectId));
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

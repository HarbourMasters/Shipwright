#ifdef ENABLE_CROWD_CONTROL

#include "CrowdControl.h"
#include "CrowdControlTypes.h"
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

void CrowdControl::EmitMessage(TCPsocket socket, uint32_t eventId, long timeRemaining, EffectResult status) {
    nlohmann::json payload;

    payload["id"] = eventId;
    payload["type"] = 0;
    payload["timeRemaining"] = timeRemaining;
    payload["status"] = status;

    std::string jsonPayload = payload.dump();
    SDLNet_TCP_Send(socket, jsonPayload.c_str(), jsonPayload.size() + 1);
}

CrowdControl::EffectResult CrowdControl::ExecuteEffect(Effect* effect) {
    GameInteractionEffectQueryResult giResult;
    if (effectStringToEnum[effect->category] == effectCatSpawnEnemy) {
        giResult = GameInteractor::RawAction::SpawnEnemyWithOffset(effect->value[0], effect->value[1]);
    } else {
        giResult = GameInteractor::ApplyEffect(effect->giEffect);
    }

    return TranslateGiEnum(giResult);
}

/// Checks if effect can be applied -- should not be used to check for spawn enemy effects.
CrowdControl::EffectResult CrowdControl::CanApplyEffect(Effect* effect) {
    assert(effectStringToEnum[effect->category] != effectCatSpawnEnemy);
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

        // Spawn Enemies and Objects
        case effectSpawnCuccoStorm:
            effect->giEffect = new GameInteractionEffect::SpawnCuccoStorm();
            break;
        case effectSpawnLitBomb:
            break;
        case effectSpawnExplosion:
            break;
        case effectSpawnArwing:
            break;
        case effectSpawnDarklink:
            break;
        case effectSpawnIronknuckle:
            break;
        case effectSpawnStalfos:
            break;
        case effectSpawnFreezard:
            break;
        case effectSpawnLikelike:
            break;
        case effectSpawnKeese:
            break;
        case effectSpawnIcekeese:
            break;
        case effectSpawnFirekeese:
            break;
        case effectSpawnWolfos:
            break;
        case effectSpawnWallmaster:
            break;

        // Link Modifiers
        case effectHalfDamageTaken:
            break;
        case effectDoubleDamageTaken:
            break;
        case effectOhko:
            break;
        case effectInvincible:
            break;
        case effectIncreaseSpeed:
            break;
        case effectDecreaseSpeed:
            break;
        case effectLowGravity:
            break;
        case effectHighGravity:
            break;
        case effectIronBoots:
            break;
        case effectHoverBoots:
            break;
        case effectSlipperyFloor:
            break;
        case effectNoLedgeGrabs:
            break;
        case effectRandomBonks:
            break;

        // Hurt or Heal Link
        case effectDamage:
            break;
        case effectHeal:
            break;
        case effectKnockback:
            break;
        case effectBurn:
            break;
        case effectFreeze:
            break;
        case effectElectrocute:
            break;
        case effectKill:
            break;

        // Give Items and Consumables
        case effectAddHeartContainer:
            break;
        case effectFillMagic:
            break;
        case effectAddRupees:
            break;
        case effectGiveDekushield:
            break;
        case effectGiveHylianshield:
            break;
        case effectRefillSticks:
            break;
        case effectRefillNuts:
            break;
        case effectRefillBombs:
            break;
        case effectRefillSeeds:
            break;
        case effectRefillArrows:
            break;
        case effectRefillBombchus:
            break;

        // Take Items and Consumables
        case effectRemoveHeartContainer:
            break;
        case effectEmptyMagic:
            break;
        case effectRemoveRupees:
            break;
        case effectTakeDekushield:
            break;
        case effectTakeHylianshield:
            break;
        case effectTakeSticks:
            break;
        case effectTakeNuts:
            break;
        case effectTakeBombs:
            break;
        case effectTakeSeeds:
            break;
        case effectTakeArrows:
            break;
        case effectTakeBombchus:
            break;

        // Link Size Modifiers
        case effectGiantLink:
            break;
        case effectMinishLink:
            break;
        case effectPaperLink:
            break;
        case effectSquishedLink:
            break;
        case effectInvisibleLink:
            break;

        // Generic Effects
        case effectRandomBombTimer:
            break;
        case effectTimeDawn:
            break;
        case effectTimeDusk:
            break;

        // Visual Effects
        case effectNoUi:
            break;
        case effectRainstorm:
            break;
        case effectDebugMode:
            break;
        case effectRainbowMode:
            break;
        case effectRandomCosmetics:
            break;

        // Controls
        case effectNoZ:
            break;
        case effectReverseControls:
            break;
        case effectPacifist:
            break;
        case effectRandomButtons:
            break;
        case effectClearCbuttons:
            break;
        case effectClearDpad:
            break;

        // Teleport Player
        case effectTpLinkshouse:
            break;
        case effectTpMinuet:
            break;
        case effectTpBolero:
            break;
        case effectTpSerenade:
            break;
        case effectTpRequiem:
            break;
        case effectTpNocturne:
            break;
        case effectTpPrelude:
            break;

        // Tunic Color (Bidding War)
        case effectTunicRed:
            break;
        case effectTunicGreen:
            break;
        case effectTunicBlue:
            break;
        case effectTunicOrange:
            break;
        case effectTunicYellow:
            break;
        case effectTunicPurple:
            break;
        case effectTunicPink:
            break;
        case effectTunicBrown:
            break;
        case effectTunicBlack:
            break;

        // Navi Color (Bidding War)
        case effectNaviRed:
            break;
        case effectNaviGreen:
            break;
        case effectNaviBlue:
            break;
        case effectNaviOrange:
            break;
        case effectNaviYellow:
            break;
        case effectNaviPurple:
            break;
        case effectNaviPink:
            break;
        case effectNaviBrown:
            break;
        case effectNaviBlack:
            break;

        // Link's Hair Color (Bidding War)
        case effectHairRed:
            break;
        case effectHairGreen:
            break;
        case effectHairBlue:
            break;
        case effectHairOrange:
            break;
        case effectHairYellow:
            break;
        case effectHairPurple:
            break;
        case effectHairPink:
            break;
        case effectHairBrown:
            break;
        case effectHairBlack:
            break;

        // Categories
        case effectCatUi:
            break;
        case effectCatGravity:
            break;
        case effectCatLinkSize:
            break;
        case effectCatPacifist:
            break;
        case effectCatNoZ:
            break;
        case effectCatWeather:
            break;
        case effectCatReverseControls:
            break;
        case effectCatBoots:
            break;
        case effectCatSpeed:
            break;
        case effectCatDamageTaken:
            break;
        case effectCatSpawnEnemy:
            break;
        case effectCatNone:
            break;
        default:
            break;
    }

    // Spawn Enemies and Objects
    //if (effectName == EFFECT_ADD_HEART_CONTAINER) {
    //    effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
    //    effect->giEffect->parameter = 1;
    //} else if (effectName == EFFECT_REMOVE_HEART_CONTAINER) {
    //    effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
    //    effect->giEffect->parameter = -1;
    //} else if (effectName == EFFECT_FILL_MAGIC) {
    //    effect->giEffect = new GameInteractionEffect::FillMagic();
    //} else if (effectName == EFFECT_EMPTY_MAGIC) {
    //    effect->giEffect = new GameInteractionEffect::EmptyMagic();
    //} else if (effectName == EFFECT_ADD_RUPEES) {
    //    effect->giEffect = new GameInteractionEffect::ModifyRupees();
    //} else if (effectName == EFFECT_REMOVE_RUPEES) {
    //    effect->giEffect = new GameInteractionEffect::ModifyRupees();
    //    effect->paramMultiplier = -1;
    //} else if (effectName == EFFECT_NO_UI) {
    //    effect->category = EFFECT_CAT_UI;
    //    effect->timeRemaining = 60000;
    //    effect->giEffect = new GameInteractionEffect::NoUI();
    //} else if (effectName == EFFECT_HIGH_GRAVITY) {
    //    effect->category = EFFECT_CAT_GRAVITY;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyGravity();
    //    effect->giEffect->parameter = GI_GRAVITY_LEVEL_HEAVY;
    //} else if (effectName == EFFECT_LOW_GRAVITY) {
    //    effect->category = EFFECT_CAT_GRAVITY;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyGravity();
    //    effect->giEffect->parameter = GI_GRAVITY_LEVEL_LIGHT;
    //} else if (effectName == EFFECT_KILL) {
    //    effect->giEffect = new GameInteractionEffect::SetPlayerHealth();
    //    effect->value[0] = 0;
    //} else if (effectName == EFFECT_FREEZE) {
    //    effect->giEffect = new GameInteractionEffect::FreezePlayer();
    //} else if (effectName == EFFECT_BURN) {
    //    effect->giEffect = new GameInteractionEffect::BurnPlayer();
    //} else if (effectName == EFFECT_ELECTROCUTE) {
    //    effect->giEffect = new GameInteractionEffect::ElectrocutePlayer();
    //} else if (effectName == EFFECT_KNOCKBACK) {
    //    effect->giEffect = new GameInteractionEffect::KnockbackPlayer();
    //} else if (effectName == EFFECT_HEAL) {
    //    effect->giEffect = new GameInteractionEffect::ModifyHealth();
    //} else if (effectName == EFFECT_DAMAGE) {
    //    effect->giEffect = new GameInteractionEffect::ModifyHealth();
    //    effect->paramMultiplier = -1;
    //} else if (effectName == EFFECT_GIANT_LINK) {
    //    effect->category = EFFECT_CAT_LINK_SIZE;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
    //    effect->giEffect->parameter = GI_LINK_SIZE_GIANT;
    //} else if (effectName == EFFECT_MINISH_LINK) {
    //    effect->category = EFFECT_CAT_LINK_SIZE;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
    //    effect->giEffect->parameter = GI_LINK_SIZE_MINISH;
    //} else if (effectName == EFFECT_PAPER_LINK) {
    //    effect->category = EFFECT_CAT_LINK_SIZE;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
    //    effect->giEffect->parameter = GI_LINK_SIZE_PAPER;
    //} else if (effectName == EFFECT_INVISIBLE_LINK) {
    //    effect->category = EFFECT_CAT_LINK_SIZE;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::InvisibleLink();
    //} else if (effectName == EFFECT_PACIFIST) {
    //    effect->category = EFFECT_CAT_PACIFIST;
    //    effect->timeRemaining = 15000;
    //    effect->giEffect = new GameInteractionEffect::PacifistMode();
    //} else if (effectName == EFFECT_NO_Z_TARGETING) {
    //    effect->category = EFFECT_CAT_NO_Z;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::DisableZTargeting();
    //} else if (effectName == EFFECT_RAINSTORM) {
    //    effect->category = EFFECT_CAT_WEATHER;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::WeatherRainstorm();
    //} else if (effectName == EFFECT_REVERSE_CONTROLS) {
    //    effect->category = EFFECT_CAT_REVERSE_CONTROLS;
    //    effect->timeRemaining = 60000;
    //    effect->giEffect = new GameInteractionEffect::ReverseControls();
    //} else if (effectName == EFFECT_IRON_BOOTS) {
    //    effect->category = EFFECT_CAT_BOOTS;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
    //    effect->giEffect->parameter = PLAYER_BOOTS_IRON;
    //} else if (effectName == EFFECT_HOVER_BOOTS) {
    //    effect->category = EFFECT_CAT_BOOTS;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
    //    effect->giEffect->parameter = PLAYER_BOOTS_HOVER;;
    //} else if (effectName == EFFECT_GIVE_DEKU_SHIELD) {
    //    effect->giEffect = new GameInteractionEffect::GiveDekuShield();
    //} else if (effectName == EFFECT_INCREASE_SPEED) {
    //    effect->category = EFFECT_CAT_SPEED;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
    //    effect->giEffect->parameter = 2;
    //} else if (effectName == EFFECT_DECREASE_SPEED) {
    //    effect->category = EFFECT_CAT_SPEED;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
    //    effect->giEffect->parameter = -2;
    //} else if (effectName == EFFECT_OHKO) {
    //    effect->category = EFFECT_CAT_DAMAGE_TAKEN;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::OneHitKO();
    //} else if (effectName == EFFECT_DOUBLE_DAMAGE_TAKEN) {
    //    effect->category = EFFECT_CAT_DAMAGE_TAKEN;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
    //    effect->paramMultiplier = -1;
    //} else if (effectName == EFFECT_HALF_DAMAGE_TAKEN) {
    //    effect->category = EFFECT_CAT_DAMAGE_TAKEN;
    //    effect->timeRemaining = 30000;
    //    effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
    //} else if (effectName == EFFECT_SPAWN_CUCCO_STORM) {
    //    effect->giEffect = new GameInteractionEffect::SpawnCuccoStorm();
    //} else if (effectName == EFFECT_SPAWN_WALLMASTER) {
    //    effect->value[0] = ACTOR_EN_WALLMAS;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_ARWING) {
    //    effect->value[0] = ACTOR_EN_CLEAR_TAG;
    //    // Parameter for no cutscene Arwing
    //    effect->value[1] = 1;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_DARK_LINK) {
    //    effect->value[0] = ACTOR_EN_TORCH2;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_STALFOS) {
    //    effect->value[0] = ACTOR_EN_TEST;
    //    // Parameter for gravity-obeying Stalfos
    //    effect->value[1] = 2;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_WOLFOS) {
    //    effect->value[0] = ACTOR_EN_WF;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_FREEZARD) {
    //    effect->value[0] = ACTOR_EN_FZ;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_KEESE) {
    //    effect->value[0] = ACTOR_EN_FIREFLY;
    //    // Parameter for normal keese
    //    effect->value[1] = 2;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_ICE_KEESE) {
    //    effect->value[0] = ACTOR_EN_FIREFLY;
    //    // Parameter for ice keese
    //    effect->value[1] = 4;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_FIRE_KEESE) {
    //    effect->value[0] = ACTOR_EN_FIREFLY;
    //    // Parameter for fire keese
    //    effect->value[1] = 1;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //} else if (effectName == EFFECT_SPAWN_LIKE_LIKE) {
    //    effect->value[0] = ACTOR_EN_RR;
    //    effect->category = EFFECT_CAT_SPAWN_ENEMY;
    //}

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
        effect->category = effectCatNone;
    }

    return effect;
}

#endif

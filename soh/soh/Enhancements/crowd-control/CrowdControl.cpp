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
    if (effect->category == effectCatSpawnEnemy) {
        giResult = GameInteractor::RawAction::SpawnEnemyWithOffset(effect->value[0], effect->value[1]);
    } else if (effect->category == effectCatSpawnActor) {
        giResult = GameInteractor::RawAction::SpawnActor(effect->value[0], effect->value[1]);
    } else {
        giResult = GameInteractor::ApplyEffect(effect->giEffect);
    }

    return TranslateGiEnum(giResult);
}

/// Checks if effect can be applied -- should not be used to check for spawn enemy effects.
CrowdControl::EffectResult CrowdControl::CanApplyEffect(Effect* effect) {
    assert(effect->category != effectCatSpawnEnemy || effect->category != effectCatSpawnActor);
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
            effect->value[0] = ACTOR_EN_NIW;
            effect->category = effectCatSpawnActor;
            break;
        case effectSpawnLitBomb:
            effect->value[0] = ACTOR_EN_BOM;
            effect->category = effectCatSpawnActor;
            break;
        case effectSpawnExplosion:
            effect->value[0] = ACTOR_EN_BOM;
            effect->value[1] = 1;
            effect->category = effectCatSpawnActor;
            break;
        case effectSpawnArwing:
            effect->value[0] = ACTOR_EN_CLEAR_TAG;
            // Parameter for no cutscene Arwing
            effect->value[1] = 1;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnDarklink:
            effect->value[0] = ACTOR_EN_TORCH2;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnIronknuckle:
            effect->value[0] = ACTOR_EN_IK;
            // Parameter for black standing Iron Knuckle
            effect->value[1] = 2;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnStalfos:
            effect->value[0] = ACTOR_EN_TEST;
            // Parameter for gravity-obeying Stalfos
            effect->value[1] = 2;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnFreezard:
            effect->value[0] = ACTOR_EN_FZ;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnLikelike:
            effect->value[0] = ACTOR_EN_RR;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnGibdo:
            effect->value[0] = ACTOR_EN_RD;
            // Parameter for Gibdo
            effect->value[1] = 32766;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnKeese:
            effect->value[0] = ACTOR_EN_FIREFLY;
            // Parameter for normal keese
            effect->value[1] = 2;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnIcekeese:
            effect->value[0] = ACTOR_EN_FIREFLY;
            // Parameter for ice keese
            effect->value[1] = 4;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnFirekeese:
            effect->value[0] = ACTOR_EN_FIREFLY;
            // Parameter for fire keese
            effect->value[1] = 1;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnWolfos:
            effect->value[0] = ACTOR_EN_WF;
            effect->category = effectCatSpawnEnemy;
            break;
        case effectSpawnWallmaster:
            effect->value[0] = ACTOR_EN_WALLMAS;
            effect->category = effectCatSpawnEnemy;
            break;

        // Link Modifiers
        case effectHalfDamageTaken:
            effect->category = effectCatDamageTaken;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
            effect->giEffect->parameters[0] = 2;
            break;
        case effectDoubleDamageTaken:
            effect->category = effectCatDamageTaken;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
            effect->giEffect->parameters[0] = -2;
            break;
        case effectOhko:
            effect->category = effectCatOhko;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::OneHitKO();
            break;
        case effectInvincible:
            break;
        case effectIncreaseSpeed:
            effect->category = effectCatSpeed;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
            effect->giEffect->parameters[0] = 2;
            break;
        case effectDecreaseSpeed:
            effect->category = effectCatSpeed;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
            effect->giEffect->parameters[0] = -2;
            break;
        case effectLowGravity:
            effect->category = effectCatGravity;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyGravity();
            effect->giEffect->parameters[0] = GI_GRAVITY_LEVEL_LIGHT;
            break;
        case effectHighGravity:
            effect->category = effectCatGravity;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyGravity();
            effect->giEffect->parameters[0] = GI_GRAVITY_LEVEL_HEAVY;
            break;
        case effectIronBoots:
            effect->category = effectCatBoots;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
            effect->giEffect->parameters[0] = PLAYER_BOOTS_IRON;
            break;
        case effectHoverBoots:
            effect->category = effectCatBoots;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
            effect->giEffect->parameters[0] = PLAYER_BOOTS_HOVER;
            break;
        case effectSlipperyFloor:
            break;
        case effectNoLedgeGrabs:
            effect->category = effectCatNoLedgeGrabs;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::DisableLedgeGrabs();
            break;
        case effectRandomWind:
            effect->category = effectCatRandomWind;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::RandomWind();
            break;
        case effectRandomBonks:
            effect->category = effectCatRandomBonks;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::RandomBonks();
            break;

        // Hurt or Heal Link
        case effectDamage:
            effect->giEffect = new GameInteractionEffect::ModifyHealth();
            effect->paramMultiplier = -1;
            break;
        case effectHeal:
            effect->giEffect = new GameInteractionEffect::ModifyHealth();
            break;
        case effectKnockback:
            effect->giEffect = new GameInteractionEffect::KnockbackPlayer();
            break;
        case effectBurn:
            effect->giEffect = new GameInteractionEffect::BurnPlayer();
            break;
        case effectFreeze:
            effect->giEffect = new GameInteractionEffect::FreezePlayer();
            break;
        case effectElectrocute:
            effect->giEffect = new GameInteractionEffect::ElectrocutePlayer();
            break;
        case effectKill:
            effect->giEffect = new GameInteractionEffect::SetPlayerHealth();
            effect->value[0] = 0;
            break;

        // Give Items and Consumables
        case effectAddHeartContainer:
            effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
            effect->giEffect->parameters[0] = 1;
            break;
        case effectFillMagic:
            effect->giEffect = new GameInteractionEffect::FillMagic();
            break;
        case effectAddRupees:
            effect->giEffect = new GameInteractionEffect::ModifyRupees();
            break;
        case effectGiveDekushield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = ITEM_SHIELD_DEKU;
            break;
        case effectGiveHylianshield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = ITEM_SHIELD_HYLIAN;
            break;
        case effectRefillSticks:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_STICK;
            break;
        case effectRefillNuts:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_NUT;
            break;
        case effectRefillBombs:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOMB;
            break;
        case effectRefillSeeds:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_SLINGSHOT;
            break;
        case effectRefillArrows:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOW;
            break;
        case effectRefillBombchus:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOMBCHU;
            break;

        // Take Items and Consumables
        case effectRemoveHeartContainer:
            effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
            effect->giEffect->parameters[0] = -1;
            break;
        case effectEmptyMagic:
            effect->giEffect = new GameInteractionEffect::EmptyMagic();
            break;
        case effectRemoveRupees:
            effect->giEffect = new GameInteractionEffect::ModifyRupees();
            effect->paramMultiplier = -1;
            break;
        case effectTakeDekushield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = -ITEM_SHIELD_DEKU;
            break;
        case effectTakeHylianshield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = -ITEM_SHIELD_HYLIAN;
            break;
        case effectTakeSticks:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_STICK;
            effect->paramMultiplier = -1;
            break;
        case effectTakeNuts:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_NUT;
            effect->paramMultiplier = -1;
            break;
        case effectTakeBombs:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOMB;
            effect->paramMultiplier = -1;
            break;
        case effectTakeSeeds:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_SLINGSHOT;
            effect->paramMultiplier = -1;
            break;
        case effectTakeArrows:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOW;
            effect->paramMultiplier = -1;
            break;
        case effectTakeBombchus:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOMBCHU;
            effect->paramMultiplier = -1;
            break;

        // Link Size Modifiers
        case effectGiantLink:
            effect->category = effectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_GIANT;
            break;
        case effectMinishLink:
            effect->category = effectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_MINISH;
            break;
        case effectPaperLink:
            effect->category = effectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_PAPER;
            break;
        case effectSquishedLink:
            effect->category = effectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_SQUISHED;
            break;
        case effectInvisibleLink:
            effect->category = effectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::InvisibleLink();
            break;

        // Generic Effects
        case effectRandomBombTimer:
            effect->category = effectCatRandomBombFuseTimer;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::RandomBombFuseTimer();
            break;
        case effectTimeDawn:
            effect->giEffect = new GameInteractionEffect::SetTimeOfDay();
            effect->value[0] = GI_TIMEOFDAY_DAWN;
            break;
        case effectTimeDusk:
            effect->giEffect = new GameInteractionEffect::SetTimeOfDay();
            effect->value[0] = GI_TIMEOFDAY_DUSK;
            break;

        // Visual Effects
        case effectNoUi:
            effect->category = effectCatUi;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::NoUI();
            break;
        case effectRainstorm:
            effect->category = effectCatWeather;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::WeatherRainstorm();
            break;
        case effectDebugMode:
            effect->category = effectCatDebugMode;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::SetCollisionViewer();
            break;
        case effectRandomCosmetics:
            effect->giEffect = new GameInteractionEffect::RandomizeCosmetics();
            break;

        // Controls
        case effectNoZ:
            effect->category = effectCatNoZ;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::DisableZTargeting();
            break;
        case effectReverseControls:
            effect->category = effectCatReverseControls;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::ReverseControls();
            break;
        case effectPacifist:
            effect->category = effectCatPacifist;
            effect->timeRemaining = 15000;
            effect->giEffect = new GameInteractionEffect::PacifistMode();
            break;
        case effectRandomButtons:
            effect->category = effectCatRandomButtons;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::PressRandomButton();
            effect->giEffect->parameters[0] = 30;
            break;
        case effectClearCbuttons:
            effect->giEffect = new GameInteractionEffect::ClearAssignedButtons();
            effect->giEffect->parameters[0] = GI_BUTTONS_CBUTTONS;
            break;
        case effectClearDpad:
            effect->giEffect = new GameInteractionEffect::ClearAssignedButtons();
            effect->giEffect->parameters[0] = GI_BUTTONS_DPAD;
            break;

        // Teleport Player
        case effectTpLinkshouse:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_LINKSHOUSE;
            break;
        case effectTpMinuet:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_MINUET;
            break;
        case effectTpBolero:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_BOLERO;
            break;
        case effectTpSerenade:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_SERENADE;
            break;
        case effectTpRequiem:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_REQUIEM;
            break;
        case effectTpNocturne:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_NOCTURNE;
            break;
        case effectTpPrelude:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_PRELUDE;
            break;

        // Tunic Color (Bidding War)
        case effectTunicRed:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_RED;
            break;
        case effectTunicGreen:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_GREEN;
            break;
        case effectTunicBlue:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_BLUE;
            break;
        case effectTunicOrange:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_ORANGE;
            break;
        case effectTunicYellow:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_YELLOW;
            break;
        case effectTunicPurple:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_PURPLE;
            break;
        case effectTunicPink:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_PINK;
            break;
        case effectTunicBrown:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_BROWN;
            break;
        case effectTunicBlack:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_BLACK;
            break;

        // Navi Color (Bidding War)
        case effectNaviRed:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_RED;
            break;
        case effectNaviGreen:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_GREEN;
            break;
        case effectNaviBlue:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_BLUE;
            break;
        case effectNaviOrange:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_ORANGE;
            break;
        case effectNaviYellow:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_YELLOW;
            break;
        case effectNaviPurple:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_PURPLE;
            break;
        case effectNaviPink:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_PINK;
            break;
        case effectNaviBrown:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_BROWN;
            break;
        case effectNaviBlack:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_BLACK;
            break;

        // Link's Hair Color (Bidding War)
        case effectHairRed:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_RED;
            break;
        case effectHairGreen:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_GREEN;
            break;
        case effectHairBlue:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_BLUE;
            break;
        case effectHairOrange:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_ORANGE;
            break;
        case effectHairYellow:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_YELLOW;
            break;
        case effectHairPurple:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_PURPLE;
            break;
        case effectHairPink:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_PINK;
            break;
        case effectHairBrown:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_BROWN;
            break;
        case effectHairBlack:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_BLACK;
            break;

        default:
            break;
    }

    // If no value is specifically set, default to using whatever CC sends us.
    // Values are used for various things depending on the effect, but they 
    // usually represent the "amount" of an effect. Amount of hearts healed,
    // strength of knockback, etc.
    if (effect->giEffect != NULL) {
        if (!effect->giEffect->parameters[0] && effect->value[0]) {
            effect->giEffect->parameters[0] = effect->value[0] * effect->paramMultiplier;
        }
    }

    if (!effect->category) {
        effect->category = effectCatNone;
    }

    return effect;
}

#endif

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
    if (effect->category == EffectCatSpawnEnemy) {
        giResult = GameInteractor::RawAction::SpawnEnemyWithOffset(effect->spawnParams[0], effect->spawnParams[1]);
    } else if (effect->category == EffectCatSpawnActor) {
        giResult = GameInteractor::RawAction::SpawnActor(effect->spawnParams[0], effect->spawnParams[1]);
    } else {
        giResult = GameInteractor::ApplyEffect(effect->giEffect);
    }

    return TranslateGiEnum(giResult);
}

/// Checks if effect can be applied -- should not be used to check for spawn enemy effects.
CrowdControl::EffectResult CrowdControl::CanApplyEffect(Effect* effect) {
    assert(effect->category != EffectCatSpawnEnemy || effect->category != EffectCatSpawnActor);
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
    uint32_t receivedParameter = 0;
    auto effectName = dataReceived["code"].get<std::string>();

    if (parameters.size() > 0) {
        receivedParameter = dataReceived["parameters"][0];
    }

    // Assign GameInteractionEffect + values to CC effect.
    // Categories are mostly used for checking for conflicting timed effects.
    switch (effectStringToEnum[effectName]) {

        // Spawn Enemies and Objects
        case EffectSpawnCuccoStorm:
            effect->spawnParams[0] = ACTOR_EN_NIW;
            effect->category = EffectCatSpawnActor;
            break;
        case EffectSpawnLitBomb:
            effect->spawnParams[0] = ACTOR_EN_BOM;
            effect->category = EffectCatSpawnActor;
            break;
        case EffectSpawnExplosion:
            effect->spawnParams[0] = ACTOR_EN_BOM;
            effect->spawnParams[1] = 1;
            effect->category = EffectCatSpawnActor;
            break;
        case EffectSpawnArwing:
            effect->spawnParams[0] = ACTOR_EN_CLEAR_TAG;
            // Parameter for no cutscene Arwing
            effect->spawnParams[1] = 1;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnDarklink:
            effect->spawnParams[0] = ACTOR_EN_TORCH2;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnIronKnuckle:
            effect->spawnParams[0] = ACTOR_EN_IK;
            // Parameter for black standing Iron Knuckle
            effect->spawnParams[1] = 2;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnStalfos:
            effect->spawnParams[0] = ACTOR_EN_TEST;
            // Parameter for gravity-obeying Stalfos
            effect->spawnParams[1] = 2;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnFreezard:
            effect->spawnParams[0] = ACTOR_EN_FZ;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnLikeLike:
            effect->spawnParams[0] = ACTOR_EN_RR;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnGibdo:
            effect->spawnParams[0] = ACTOR_EN_RD;
            // Parameter for Gibdo
            effect->spawnParams[1] = 32766;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnKeese:
            effect->spawnParams[0] = ACTOR_EN_FIREFLY;
            // Parameter for normal keese
            effect->spawnParams[1] = 2;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnIceKeese:
            effect->spawnParams[0] = ACTOR_EN_FIREFLY;
            // Parameter for ice keese
            effect->spawnParams[1] = 4;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnFireKeese:
            effect->spawnParams[0] = ACTOR_EN_FIREFLY;
            // Parameter for fire keese
            effect->spawnParams[1] = 1;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnWolfos:
            effect->spawnParams[0] = ACTOR_EN_WF;
            effect->category = EffectCatSpawnEnemy;
            break;
        case EffectSpawnWallmaster:
            effect->spawnParams[0] = ACTOR_EN_WALLMAS;
            effect->category = EffectCatSpawnEnemy;
            break;

        // Link Modifiers
        case EffectTakeHalfDamage:
            effect->category = EffectCatDamageTaken;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
            effect->giEffect->parameters[0] = 2;
            break;
        case EffectTakeDoubleDamage:
            effect->category = EffectCatDamageTaken;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyDefenseModifier();
            effect->giEffect->parameters[0] = -2;
            break;
        case EffectOneHitKo:
            effect->category = EffectCatOhko;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::OneHitKO();
            break;
        case EffectInvincibility:
            effect->category = EffectCatInvincible;
            effect->timeRemaining = 15000;
            effect->giEffect = new GameInteractionEffect::PlayerInvincibility();
            break;
            break;
        case EffectIncreaseSpeed:
            effect->category = EffectCatSpeed;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
            effect->giEffect->parameters[0] = 2;
            break;
        case EffectDecreaseSpeed:
            effect->category = EffectCatSpeed;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyRunSpeedModifier();
            effect->giEffect->parameters[0] = -2;
            break;
        case EffectLowGravity:
            effect->category = EffectCatGravity;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyGravity();
            effect->giEffect->parameters[0] = GI_GRAVITY_LEVEL_LIGHT;
            break;
        case EffectHighGravity:
            effect->category = EffectCatGravity;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyGravity();
            effect->giEffect->parameters[0] = GI_GRAVITY_LEVEL_HEAVY;
            break;
        case EffectForceIronBoots:
            effect->category = EffectCatBoots;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
            effect->giEffect->parameters[0] = PLAYER_BOOTS_IRON;
            break;
        case EffectForceHoverBoots:
            effect->category = EffectCatBoots;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ForceEquipBoots();
            effect->giEffect->parameters[0] = PLAYER_BOOTS_HOVER;
            break;
        case EffectSlipperyFloor:
            effect->category = EffectCatSlipperyFloor;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::SlipperyFloor();
            break;
        case EffectNoLedgeGrabs:
            effect->category = EffectCatNoLedgeGrabs;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::DisableLedgeGrabs();
            break;
        case EffectRandomWind:
            effect->category = EffectCatRandomWind;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::RandomWind();
            break;
        case EffectRandomBonks:
            effect->category = EffectCatRandomBonks;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::RandomBonks();
            break;

        // Hurt or Heal Link
        case EffectEmptyHeart:
            effect->giEffect = new GameInteractionEffect::ModifyHealth();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            break;
        case EffectFillHeart:
            effect->giEffect = new GameInteractionEffect::ModifyHealth();
            break;
        case EffectKnockbackLink:
            effect->giEffect = new GameInteractionEffect::KnockbackPlayer();
            break;
        case EffectBurnLink:
            effect->giEffect = new GameInteractionEffect::BurnPlayer();
            break;
        case EffectFreezeLink:
            effect->giEffect = new GameInteractionEffect::FreezePlayer();
            break;
        case EffectElectrocuteLink:
            effect->giEffect = new GameInteractionEffect::ElectrocutePlayer();
            break;
        case EffectKillLink:
            effect->giEffect = new GameInteractionEffect::SetPlayerHealth();
            effect->giEffect->parameters[0] = 0;
            break;

        // Give Items and Consumables
        case EffectAddHeartContainer:
            effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
            effect->giEffect->parameters[0] = 1;
            break;
        case EffectFillMagic:
            effect->giEffect = new GameInteractionEffect::FillMagic();
            break;
        case EffectAddRupees:
            effect->giEffect = new GameInteractionEffect::ModifyRupees();
            break;
        case EffectGiveDekuShield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = ITEM_SHIELD_DEKU;
            break;
        case EffectGiveHylianShield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = ITEM_SHIELD_HYLIAN;
            break;
        case EffectRefillSticks:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_STICK;
            break;
        case EffectRefillNuts:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_NUT;
            break;
        case EffectRefillBombs:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOMB;
            break;
        case EffectRefillSeeds:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_SLINGSHOT;
            break;
        case EffectRefillArrows:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOW;
            break;
        case EffectRefillBombchus:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[1] = ITEM_BOMBCHU;
            break;

        // Take Items and Consumables
        case EffectRemoveHeartContainer:
            effect->giEffect = new GameInteractionEffect::ModifyHeartContainers();
            effect->giEffect->parameters[0] = -1;
            break;
        case EffectEmptyMagic:
            effect->giEffect = new GameInteractionEffect::EmptyMagic();
            break;
        case EffectRemoveRupees:
            effect->giEffect = new GameInteractionEffect::ModifyRupees();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            break;
        case EffectTakeDekuShield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = -ITEM_SHIELD_DEKU;
            break;
        case EffectTakeHylianShield:
            effect->giEffect = new GameInteractionEffect::GiveOrTakeShield();
            effect->giEffect->parameters[0] = -ITEM_SHIELD_HYLIAN;
            break;
        case EffectTakeSticks:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            effect->giEffect->parameters[1] = ITEM_STICK;
            break;
        case EffectTakeNuts:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            effect->giEffect->parameters[1] = ITEM_NUT;
            break;
        case EffectTakeBombs:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            effect->giEffect->parameters[1] = ITEM_BOMB;
            break;
        case EffectTakeSeeds:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            effect->giEffect->parameters[1] = ITEM_SLINGSHOT;
            break;
        case EffectTakeArrows:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            effect->giEffect->parameters[1] = ITEM_BOW;
            break;
        case EffectTakeBombchus:
            effect->giEffect = new GameInteractionEffect::AddOrTakeAmmo();
            effect->giEffect->parameters[0] = receivedParameter * -1;
            effect->giEffect->parameters[1] = ITEM_BOMBCHU;
            break;

        // Link Size Modifiers
        case EffectGiantLink:
            effect->category = EffectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_GIANT;
            break;
        case EffectMinishLink:
            effect->category = EffectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_MINISH;
            break;
        case EffectPaperLink:
            effect->category = EffectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_PAPER;
            break;
        case EffectSquishedLink:
            effect->category = EffectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::ModifyLinkSize();
            effect->giEffect->parameters[0] = GI_LINK_SIZE_SQUISHED;
            break;
        case EffectInvisibleLink:
            effect->category = EffectCatLinkSize;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::InvisibleLink();
            break;

        // Generic Effects
        case EffectRandomBombTimer:
            effect->category = EffectCatRandomBombFuseTimer;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::RandomBombFuseTimer();
            break;
        case EffectSetTimeToDawn:
            effect->giEffect = new GameInteractionEffect::SetTimeOfDay();
            effect->giEffect->parameters[0] = GI_TIMEOFDAY_DAWN;
            break;
        case EffectSetTimeToDusk:
            effect->giEffect = new GameInteractionEffect::SetTimeOfDay();
            effect->giEffect->parameters[0] = GI_TIMEOFDAY_DUSK;
            break;

        // Visual Effects
        case EffectNoUi:
            effect->category = EffectCatUi;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::NoUI();
            break;
        case EffectRainstorm:
            effect->category = EffectCatWeather;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::WeatherRainstorm();
            break;
        case EffectDebugMode:
            effect->category = EffectCatDebugMode;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::SetCollisionViewer();
            break;
        case EffectRandomCosmetics:
            effect->giEffect = new GameInteractionEffect::RandomizeCosmetics();
            break;

        // Controls
        case EffectNoZButton:
            effect->category = EffectCatNoZ;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::DisableZTargeting();
            break;
        case EffectReverseControls:
            effect->category = EffectCatReverseControls;
            effect->timeRemaining = 60000;
            effect->giEffect = new GameInteractionEffect::ReverseControls();
            break;
        case EffectPacifistMode:
            effect->category = EffectCatPacifist;
            effect->timeRemaining = 15000;
            effect->giEffect = new GameInteractionEffect::PacifistMode();
            break;
        case EffectPressRandomButtons:
            effect->category = EffectCatRandomButtons;
            effect->timeRemaining = 30000;
            effect->giEffect = new GameInteractionEffect::PressRandomButton();
            effect->giEffect->parameters[0] = 30;
            break;
        case EffectClearCbuttons:
            effect->giEffect = new GameInteractionEffect::ClearAssignedButtons();
            effect->giEffect->parameters[0] = GI_BUTTONS_CBUTTONS;
            break;
        case EffectClearDpad:
            effect->giEffect = new GameInteractionEffect::ClearAssignedButtons();
            effect->giEffect->parameters[0] = GI_BUTTONS_DPAD;
            break;

        // Teleport Player
        case EffectTpLinksHouse:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_LINKSHOUSE;
            break;
        case EffectTpMinuet:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_MINUET;
            break;
        case EffectTpBolero:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_BOLERO;
            break;
        case EffectTpSerenade:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_SERENADE;
            break;
        case EffectTpRequiem:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_REQUIEM;
            break;
        case EffectTpNocturne:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_NOCTURNE;
            break;
        case EffectTpPrelude:
            effect->giEffect = new GameInteractionEffect::TeleportPlayer();
            effect->giEffect->parameters[0] = GI_TP_DEST_PRELUDE;
            break;

        // Tunic Color (Bidding War)
        case EffectTunicRed:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_RED;
            break;
        case EffectTunicGreen:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_GREEN;
            break;
        case EffectTunicBlue:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_BLUE;
            break;
        case EffectTunicOrange:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_ORANGE;
            break;
        case EffectTunicYellow:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_YELLOW;
            break;
        case EffectTunicPurple:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_PURPLE;
            break;
        case EffectTunicPink:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_PINK;
            break;
        case EffectTunicBrown:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_BROWN;
            break;
        case EffectTunicBlack:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_TUNICS;
            effect->giEffect->parameters[1] = GI_COLOR_BLACK;
            break;

        // Navi Color (Bidding War)
        case EffectNaviRed:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_RED;
            break;
        case EffectNaviGreen:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_GREEN;
            break;
        case EffectNaviBlue:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_BLUE;
            break;
        case EffectNaviOrange:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_ORANGE;
            break;
        case EffectNaviYellow:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_YELLOW;
            break;
        case EffectNaviPurple:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_PURPLE;
            break;
        case EffectNaviPink:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_PINK;
            break;
        case EffectNaviBrown:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_BROWN;
            break;
        case EffectNaviBlack:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_NAVI;
            effect->giEffect->parameters[1] = GI_COLOR_BLACK;
            break;

        // Link's Hair Color (Bidding War)
        case EffectHairRed:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_RED;
            break;
        case EffectHairGreen:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_GREEN;
            break;
        case EffectHairBlue:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_BLUE;
            break;
        case EffectHairOrange:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_ORANGE;
            break;
        case EffectHairYellow:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_YELLOW;
            break;
        case EffectHairPurple:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_PURPLE;
            break;
        case EffectHairPink:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_PINK;
            break;
        case EffectHairBrown:
            effect->giEffect = new GameInteractionEffect::SetCosmeticsColor();
            effect->giEffect->parameters[0] = GI_COSMETICS_HAIR;
            effect->giEffect->parameters[1] = GI_COLOR_BROWN;
            break;
        case EffectHairBlack:
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
        if (!effect->giEffect->parameters[0]) {
            effect->giEffect->parameters[0] = receivedParameter;
        }
    }

    return effect;
}

#endif

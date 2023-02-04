#include "GameInteractor.h"
#include <libultraship/bridge.h>

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
}

#include "overlays/actors/ovl_En_Niw/z_en_niw.h"

void GameInteractor::RawAction::AddOrRemoveHealthContainers(int16_t amount) {
    gSaveContext.healthCapacity += amount * 0x10;
}

void GameInteractor::RawAction::AddOrRemoveMagic(int8_t amount) {
    // Full single magic = 48
    // Full double magic = 96
    int8_t currentMagicCapacity = (gSaveContext.isDoubleMagicAcquired + 1) * 48;

    if (gSaveContext.isMagicAcquired) {
        gSaveContext.prevMagicState = gSaveContext.magicState;
        if (amount > 0) {
            if (gSaveContext.magic + amount > currentMagicCapacity) {
                gSaveContext.magicFillTarget = currentMagicCapacity;
            } else {
                gSaveContext.magicFillTarget = gSaveContext.magic + amount;
            }
            gSaveContext.magicState = MAGIC_STATE_FILL;
        } else if (amount < 0) {
            if (gSaveContext.magic + amount < 0) {
                gSaveContext.magic = 0;
            } else {
                gSaveContext.magic += amount;
            }
        }
    }
}

void GameInteractor::RawAction::HealOrDamagePlayer(int16_t hearts) {
    if (hearts > 0) {
        Health_ChangeBy(gPlayState, hearts * 0x10);
    } else if (hearts < 0) {
        Player* player = GET_PLAYER(gPlayState);
        Health_ChangeBy(gPlayState, hearts * 0x10);
        func_80837C0C(gPlayState, player, 0, 0, 0, 0, 0);
        player->invincibilityTimer = 28;
    }
}

void GameInteractor::RawAction::SetPlayerHealth(int16_t hearts) {
    gSaveContext.health = hearts * 0x10;
}

void GameInteractor::RawAction::SetLinkInvisibility(bool active) {
    GameInteractor::State::InvisibleLinkActive = active;
    if (!active) {
        Player* player = GET_PLAYER(gPlayState);
        player->actor.shape.shadowDraw = ActorShadow_DrawFeet;
    }
}

void GameInteractor::RawAction::SetWeatherStorm(bool active) {
    if (active) {
        gPlayState->envCtx.unk_F2[0] = 20;    // rain intensity target
        gPlayState->envCtx.gloomySkyMode = 1; // start gloomy sky
        if ((gWeatherMode != 0) || gPlayState->envCtx.unk_17 != 0) {
            gPlayState->envCtx.unk_DE = 1;
        }
        gPlayState->envCtx.lightningMode = LIGHTNING_MODE_ON;
        Environment_PlayStormNatureAmbience(gPlayState);
    } else {
        gPlayState->envCtx.unk_F2[0] = 0;
        if (gPlayState->csCtx.state == CS_STATE_IDLE) {
            Environment_StopStormNatureAmbience(gPlayState);
        } else if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) == NA_BGM_NATURE_AMBIENCE) {
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
        }
        osSyncPrintf("\n\n\nE_wether_flg=[%d]", gWeatherMode);
        osSyncPrintf("\nrain_evt_trg=[%d]\n\n", gPlayState->envCtx.gloomySkyMode);
        if (gWeatherMode == 0 && (gPlayState->envCtx.gloomySkyMode == 1)) {
            gPlayState->envCtx.gloomySkyMode = 2; // end gloomy sky
        } else {
            gPlayState->envCtx.gloomySkyMode = 0;
            gPlayState->envCtx.unk_DE = 0;
        }
        gPlayState->envCtx.lightningMode = LIGHTNING_MODE_LAST;
    }
}

void GameInteractor::RawAction::ForceEquipBoots(int8_t boots) {
    Player* player = GET_PLAYER(gPlayState);
    player->currentBoots = boots;
    Inventory_ChangeEquipment(EQUIP_BOOTS, boots + 1);
    Player_SetBootData(gPlayState, player);
}

void GameInteractor::RawAction::FreezePlayer() {
    gSaveContext.pendingIceTrapCount++;
}

void GameInteractor::RawAction::BurnPlayer() {
    Player* player = GET_PLAYER(gPlayState);
    for (int i = 0; i < 18; i++) {
        player->flameTimers[i] = Rand_S16Offset(0, 200);
    }
    player->isBurning = true;
    func_80837C0C(gPlayState, player, 0, 0, 0, 0, 0);
}

void GameInteractor::RawAction::ElectrocutePlayer() {
    Player* player = GET_PLAYER(gPlayState);
    func_80837C0C(gPlayState, player, 4, 0, 0, 0, 0);
}

void GameInteractor::RawAction::KnockbackPlayer(float strength) {
    Player* player = GET_PLAYER(gPlayState);
    func_8002F71C(gPlayState, &player->actor, strength * 5, player->actor.world.rot.y + 0x8000, strength * 5);
}

void GameInteractor::RawAction::GiveDekuShield() {
    // Give Deku Shield to the player, and automatically equip it when they're child and have no shield currently equiped.
    Player* player = GET_PLAYER(gPlayState);
    Item_Give(gPlayState, ITEM_SHIELD_DEKU);
    if (LINK_IS_CHILD && player->currentShield == PLAYER_SHIELD_NONE) {
        player->currentShield = PLAYER_SHIELD_DEKU;
        Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_DEKU);
    }
}

void GameInteractor::RawAction::SpawnCuccoStorm() {
    Player* player = GET_PLAYER(gPlayState);
    EnNiw* cucco = (EnNiw*)Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_NIW, player->actor.world.pos.x,
                                       player->actor.world.pos.y + 2200, player->actor.world.pos.z, 0, 0, 0, 0, 0);
    cucco->actionFunc = func_80AB70A0_nocutscene;
}

void GameInteractor::RawAction::ForceInterfaceUpdate() {
    gSaveContext.unk_13E8 = 50;
    Interface_Update(gPlayState);
}

void GameInteractor::RawAction::TeleportPlayer(int32_t nextEntrance) {
    gPlayState->nextEntranceIndex = nextEntrance;
    gPlayState->sceneLoadFlag = 0x14;
    gPlayState->fadeTransition = 2;
    gSaveContext.nextTransitionType = 2;
}

void GameInteractor::RawAction::ClearAssignedButtons(uint8_t buttonSet) {
    switch (buttonSet) {
        case GI_BUTTONS_CBUTTONS:
            gSaveContext.equips.buttonItems[1] = gSaveContext.equips.buttonItems[2] =
                gSaveContext.equips.buttonItems[3] = ITEM_NONE;
            break;
        case GI_BUTTONS_DPAD:
            gSaveContext.equips.buttonItems[4] = gSaveContext.equips.buttonItems[5] =
                gSaveContext.equips.buttonItems[6] = gSaveContext.equips.buttonItems[7] = ITEM_NONE;
            break;
    }
}

void GameInteractor::RawAction::SetTimeOfDay(uint32_t time) {
    if (time > 0xFFFF) {
        gSaveContext.dayTime = 0xFFFF;
    } else {
        gSaveContext.dayTime = time;
    }
}

void GameInteractor::RawAction::SetCollisionViewer(bool active) {
    CVarSetInteger("gColViewerEnabled", active);
    CVarSetInteger("gColViewerDecal", active);
    
    if (active) {
        CVarSetInteger("gColViewerScene", 2);
        CVarSetInteger("gColViewerBgActors", 2);
        CVarSetInteger("gColViewerColCheck", 2);
        CVarSetInteger("gColViewerWaterbox", 2);
    } else {
        CVarSetInteger("gColViewerScene", 0);
        CVarSetInteger("gColViewerBgActors", 0);
        CVarSetInteger("gColViewerColCheck", 0);
        CVarSetInteger("gColViewerWaterbox", 0);
    }
}

GameInteractionEffectQueryResult GameInteractor::RawAction::SpawnEnemyWithOffset(uint32_t enemyId, int32_t enemyParams) {

    if (!GameInteractor::CanSpawnEnemy()) {
        return GameInteractionEffectQueryResult::TemporarilyNotPossible;
    }

    Player* player = GET_PLAYER(gPlayState);

    float posXOffset = 75;
    float posYOffset = 50;
    float posZOffset = 0;

    if (enemyId == ACTOR_EN_WALLMAS) {
        posXOffset = 0;
        posYOffset = 0;
    } else if (enemyId == ACTOR_EN_CLEAR_TAG) {
        // Don't allow Arwings in certain areas because they cause issues.
        // Locations: King dodongo room, Morpha room, Twinrova room, Ganondorf room, Fishing pond, Ganon's room
        // TODO: Swap this to disabling the option in CC options menu instead.
        if (gPlayState->sceneNum == SCENE_DDAN_BOSS || gPlayState->sceneNum == SCENE_MIZUSIN_BS ||
            gPlayState->sceneNum == SCENE_JYASINBOSS || gPlayState->sceneNum == SCENE_GANON_BOSS ||
            gPlayState->sceneNum == SCENE_TURIBORI || gPlayState->sceneNum == SCENE_GANON_DEMO) {
            return GameInteractionEffectQueryResult::NotPossible;
        }
        posXOffset = 0;
        posYOffset = 100;
    }

    if (Actor_Spawn(&gPlayState->actorCtx, gPlayState, enemyId, player->actor.world.pos.x + posXOffset,
        player->actor.world.pos.y + posYOffset, player->actor.world.pos.z + posZOffset, 0, 0, 0,
        enemyParams, 0) != NULL) {
        return GameInteractionEffectQueryResult::Possible;
    }

    return GameInteractionEffectQueryResult::TemporarilyNotPossible;
}

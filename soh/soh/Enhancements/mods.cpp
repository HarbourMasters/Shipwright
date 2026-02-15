#include "mods.h"
#include <libultraship/bridge.h>
#include "game-interactor/GameInteractor.h"
#include "soh/Enhancements/boss-rush/BossRush.h"
#include "soh/Enhancements/enhancementTypes.h"
#include <soh/Enhancements/item-tables/ItemTableManager.h>

extern "C" {
#include <z64.h>
#include "macros.h"
#include "soh/cvar_prefixes.h"
#include "variables.h"
#include "functions.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

/// Switches Link's age and respawns him at the last entrance he entered.
void SwitchAge() {
    if (gPlayState == NULL)
        return;

    Player* player = GET_PLAYER(gPlayState);

    // Hyrule Castle: Very likely to fall through floor, so we force a specific entrance
    if (gPlayState->sceneNum == SCENE_HYRULE_CASTLE || gPlayState->sceneNum == SCENE_OUTSIDE_GANONS_CASTLE) {
        gPlayState->nextEntranceIndex = ENTR_CASTLE_GROUNDS_SOUTH_EXIT;
    } else {
        gSaveContext.respawnFlag = 1;
        gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;

        // Preserve the player's position and orientation
        gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = gPlayState->nextEntranceIndex;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = gPlayState->roomCtx.curRoom.num;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = player->actor.world.pos;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = player->actor.shape.rot.y;

        if (gPlayState->roomCtx.curRoom.behaviorType2 < 4) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0x0DFF;
        } else {
            // Scenes with static backgrounds use a special camera we need to preserve
            Camera* camera = GET_ACTIVE_CAM(gPlayState);
            s16 camId = camera->camDataIdx;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0x0D00 | camId;
        }
    }

    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_INSTANT;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
    gPlayState->linkAgeOnLoad ^= 1;

    // Discover adult/child spawns
    if (gPlayState->linkAgeOnLoad == LINK_AGE_ADULT) {
        Entrance_SetEntranceDiscovered(ENTR_HYRULE_FIELD_10, false);
    } else {
        Entrance_SetEntranceDiscovered(ENTR_LINKS_HOUSE_CHILD_SPAWN, false);
    }

    static HOOK_ID hookId = 0;
    hookId = REGISTER_VB_SHOULD(VB_INFLICT_VOID_DAMAGE, {
        *should = false;
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnVanillaBehavior>(hookId);
    });
}

/// Switches Link's age and respawns him at the last entrance he entered.
void RegisterOcarinaTimeTravel() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnOcarinaSongAction>([]() {
        if (!GameInteractor::IsSaveLoaded(true) || !CVarGetInteger(CVAR_ENHANCEMENT("TimeTravel"), 0)) {
            return;
        }

        Actor* player = &GET_PLAYER(gPlayState)->actor;
        Actor* nearbyTimeBlockEmpty =
            Actor_FindNearby(gPlayState, player, ACTOR_OBJ_WARP2BLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyTimeBlock = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_TIMEBLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyOcarinaSpot = Actor_FindNearby(gPlayState, player, ACTOR_EN_OKARINA_TAG, ACTORCAT_PROP, 120.0f);
        Actor* nearbyDoorOfTime = Actor_FindNearby(gPlayState, player, ACTOR_DOOR_TOKI, ACTORCAT_BG, 500.0f);
        Actor* nearbyFrogs = Actor_FindNearby(gPlayState, player, ACTOR_EN_FR, ACTORCAT_NPC, 300.0f);
        Actor* nearbyGossipStone = Actor_FindNearby(gPlayState, player, ACTOR_EN_GS, ACTORCAT_NPC, 300.0f);
        bool justPlayedSoT = gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_TIME;
        bool notNearAnySource = !nearbyTimeBlockEmpty && !nearbyTimeBlock && !nearbyOcarinaSpot && !nearbyDoorOfTime &&
                                !nearbyFrogs && !nearbyGossipStone;
        bool hasOcarinaOfTime = (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME);
        bool hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
        int timeTravelSetting = CVarGetInteger(CVAR_ENHANCEMENT("TimeTravel"), 0);
        bool meetsTimeTravelRequirements = false;

        switch (timeTravelSetting) {
            case TIME_TRAVEL_ANY:
                meetsTimeTravelRequirements = true;
                break;
            case TIME_TRAVEL_ANY_MS:
                meetsTimeTravelRequirements = hasMasterSword;
                break;
            case TIME_TRAVEL_OOT_MS:
                meetsTimeTravelRequirements = hasMasterSword && hasOcarinaOfTime;
                break;
            case TIME_TRAVEL_OOT:
            default:
                meetsTimeTravelRequirements = hasOcarinaOfTime;
                break;
        }

        if (justPlayedSoT && notNearAnySource && meetsTimeTravelRequirements) {
            SwitchAge();
        }
    });
}

bool IsHyperBossesActive() {
    return CVarGetInteger(CVAR_ENHANCEMENT("HyperBosses"), 0) ||
           (IS_BOSS_RUSH &&
            gSaveContext.ship.quest.data.bossRush.options[BR_OPTIONS_HYPERBOSSES] == BR_CHOICE_HYPERBOSSES_YES);
}

void UpdateHyperBossesState() {
    static uint32_t actorUpdateHookId = 0;
    if (actorUpdateHookId != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(actorUpdateHookId);
        actorUpdateHookId = 0;
    }

    if (IsHyperBossesActive()) {
        actorUpdateHookId =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* refActor) {
                // Run the update function a second time to make bosses move and act twice as fast.

                Player* player = GET_PLAYER(gPlayState);
                Actor* actor = static_cast<Actor*>(refActor);

                uint8_t isBossActor = actor->id == ACTOR_BOSS_GOMA ||      // Gohma
                                      actor->id == ACTOR_BOSS_DODONGO ||   // King Dodongo
                                      actor->id == ACTOR_EN_BDFIRE ||      // King Dodongo Fire Breath
                                      actor->id == ACTOR_BOSS_VA ||        // Barinade
                                      actor->id == ACTOR_BOSS_GANONDROF || // Phantom Ganon
                                      actor->id == ACTOR_EN_FHG_FIRE || // Phantom Ganon/Ganondorf Energy Ball/Thunder
                                      actor->id == ACTOR_EN_FHG ||      // Phantom Ganon's Horse
                                      actor->id == ACTOR_BOSS_FD ||
                                      actor->id == ACTOR_BOSS_FD2 ||   // Volvagia (grounded/flying)
                                      actor->id == ACTOR_EN_VB_BALL || // Volvagia Rocks
                                      actor->id == ACTOR_BOSS_MO ||    // Morpha
                                      actor->id == ACTOR_BOSS_SST ||   // Bongo Bongo
                                      actor->id == ACTOR_BOSS_TW ||    // Twinrova
                                      actor->id == ACTOR_BOSS_GANON || // Ganondorf
                                      actor->id == ACTOR_BOSS_GANON2;  // Ganon

                // Don't apply during cutscenes because it causes weird behaviour and/or crashes on some bosses.
                if (IsHyperBossesActive() && isBossActor && !Player_InBlockingCsMode(gPlayState, player)) {
                    // Barinade needs to be updated in sequence to avoid unintended behaviour.
                    if (actor->id == ACTOR_BOSS_VA) {
                        // params -1 is BOSSVA_BODY
                        if (actor->params == -1) {
                            Actor* actorList = gPlayState->actorCtx.actorLists[ACTORCAT_BOSS].head;
                            while (actorList != NULL) {
                                GameInteractor::RawAction::UpdateActor(actorList);
                                actorList = actorList->next;
                            }
                        }
                    } else {
                        GameInteractor::RawAction::UpdateActor(actor);
                    }
                }
            });
    }
}

void RegisterHyperBosses() {
    UpdateHyperBossesState();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(
        [](int16_t fileNum) { UpdateHyperBossesState(); });
}

void InitMods() {
    RegisterOcarinaTimeTravel();
    RegisterHyperBosses();
}

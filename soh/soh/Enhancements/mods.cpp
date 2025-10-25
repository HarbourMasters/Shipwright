#include "mods.h"
#include <libultraship/bridge.h>
#include "game-interactor/GameInteractor.h"
#include "tts/tts.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/Enhancements/boss-rush/BossRush.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include "soh/Enhancements/timesaver_hook_handlers.h"
#include "soh/Enhancements/randomizer/hook_handlers.h"

#include "src/overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "src/overlays/actors/ovl_Door_Gerudo/z_door_gerudo.h"
#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "soh_assets.h"
#include "kaleido.h"

extern "C" {
#include <z64.h>
#include "align_asset_macro.h"
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

static bool hasAffectedHealth = false;
void UpdatePermanentHeartLossState() {
    if (!GameInteractor::IsSaveLoaded())
        return;

    if (!CVarGetInteger(CVAR_ENHANCEMENT("PermanentHeartLoss"), 0) && hasAffectedHealth) {
        uint8_t heartContainers = gSaveContext.ship.stats.heartContainers; // each worth 16 health
        uint8_t heartPieces = gSaveContext.ship.stats.heartPieces; // each worth 4 health, but only in groups of 4
        uint8_t startingHealth =
            16 * (IS_RANDO ? (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_STARTING_HEARTS) + 1) : 3);

        uint8_t newCapacity = startingHealth + (heartContainers * 16) + ((heartPieces - (heartPieces % 4)) * 4);
        gSaveContext.healthCapacity = MAX(newCapacity, gSaveContext.healthCapacity);
        gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
        hasAffectedHealth = false;
    }
}

void RegisterPermanentHeartLoss() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int16_t fileNum) {
        hasAffectedHealth = false;
        UpdatePermanentHeartLossState();
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!CVarGetInteger(CVAR_ENHANCEMENT("PermanentHeartLoss"), 0) || !GameInteractor::IsSaveLoaded())
            return;

        if (gSaveContext.healthCapacity > 16 && gSaveContext.healthCapacity - gSaveContext.health >= 16) {
            gSaveContext.healthCapacity -= 16;
            gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
            hasAffectedHealth = true;
        }
    });
};

void RegisterDeleteFileOnDeath() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!CVarGetInteger(CVAR_ENHANCEMENT("DeleteFileOnDeath"), 0) || !GameInteractor::IsSaveLoaded() ||
            gPlayState == NULL)
            return;

        if (gPlayState->gameOverCtx.state == GAMEOVER_DEATH_MENU && gPlayState->pauseCtx.state == 9) {
            SaveManager::Instance->DeleteZeldaFile(gSaveContext.fileNum);
            hasAffectedHealth = false;
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                ->Dispatch("reset");
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

void UpdateHyperEnemiesState() {
    static uint32_t actorUpdateHookId = 0;
    if (actorUpdateHookId != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(actorUpdateHookId);
        actorUpdateHookId = 0;
    }

    if (CVarGetInteger(CVAR_ENHANCEMENT("HyperEnemies"), 0)) {
        actorUpdateHookId =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* refActor) {
                // Run the update function a second time to make enemies and minibosses move and act twice as fast.

                Player* player = GET_PLAYER(gPlayState);
                Actor* actor = static_cast<Actor*>(refActor);

                // Some enemies are not in the ACTORCAT_ENEMY category, and some are that aren't really enemies.
                bool isEnemy = actor->category == ACTORCAT_ENEMY || actor->id == ACTOR_EN_TORCH2;
                bool isExcludedEnemy = actor->id == ACTOR_EN_FIRE_ROCK || actor->id == ACTOR_EN_ENCOUNT2;

                // Don't apply during cutscenes because it causes weird behaviour and/or crashes on some cutscenes.
                if (CVarGetInteger(CVAR_ENHANCEMENT("HyperEnemies"), 0) && isEnemy && !isExcludedEnemy &&
                    !Player_InBlockingCsMode(gPlayState, player)) {
                    GameInteractor::RawAction::UpdateActor(actor);
                }
            });
    }
}

void UpdateDirtPathFixState(int32_t sceneNum) {
    switch (sceneNum) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KOKIRI_FOREST:
        case SCENE_HYRULE_CASTLE:
            CVarSetInteger(CVAR_Z_FIGHTING_MODE,
                           CVarGetInteger(CVAR_ENHANCEMENT("SceneSpecificDirtPathFix"), ZFIGHT_FIX_DISABLED));
            return;
        default:
            CVarClear(CVAR_Z_FIGHTING_MODE);
    }
}

void RegisterMenuPathFix() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>(
        [](int32_t sceneNum) { UpdateDirtPathFixState(sceneNum); });
}

void UpdateMirrorModeState(int32_t sceneNum) {
    static bool prevMirroredWorld = false;
    bool nextMirroredWorld = false;

    int16_t mirroredMode = CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorldMode"), MIRRORED_WORLD_OFF);
    int16_t inDungeon = (sceneNum >= SCENE_DEKU_TREE && sceneNum <= SCENE_INSIDE_GANONS_CASTLE_COLLAPSE &&
                         sceneNum != SCENE_THIEVES_HIDEOUT) ||
                        (sceneNum >= SCENE_DEKU_TREE_BOSS && sceneNum <= SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                        (sceneNum == SCENE_GANON_BOSS);

    if (mirroredMode == MIRRORED_WORLD_RANDOM_SEEDED || mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED) {
        uint32_t seed =
            sceneNum + (IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : gSaveContext.ship.stats.fileCreatedAt);
        Random_Init(seed);
    }

    bool randomMirror = Random(0, 2) == 1;

    if (mirroredMode == MIRRORED_WORLD_ALWAYS ||
        ((mirroredMode == MIRRORED_WORLD_RANDOM || mirroredMode == MIRRORED_WORLD_RANDOM_SEEDED) && randomMirror) ||
        // Dungeon modes
        (inDungeon &&
         (mirroredMode == MIRRORED_WORLD_DUNGEONS_ALL ||
          (mirroredMode == MIRRORED_WORLD_DUNGEONS_VANILLA && !ResourceMgr_IsSceneMasterQuest(sceneNum)) ||
          (mirroredMode == MIRRORED_WORLD_DUNGEONS_MQ && ResourceMgr_IsSceneMasterQuest(sceneNum)) ||
          ((mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM || mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED) &&
           randomMirror)))) {
        nextMirroredWorld = true;
        CVarSetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 1);
    } else {
        nextMirroredWorld = false;
        CVarClear(CVAR_ENHANCEMENT("MirroredWorld"));
    }

    if (prevMirroredWorld != nextMirroredWorld) {
        prevMirroredWorld = nextMirroredWorld;
        ApplyMirrorWorldGfxPatches();
    }
}

void RegisterMirrorModeHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(
        [](int32_t sceneNum) { UpdateMirrorModeState(sceneNum); });
}

void UpdatePatchHand() {
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) && LINK_IS_CHILD) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer1", 92,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer2", 93, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot1", 84,
                                   gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot2", 85,
                                   gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow1", 51,
                                   gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow2", 52, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword1", 104,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword2", 105,
                                   gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword1", 79,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword2", 80, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife1", 76,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife2", 77,
                                   gsSPEndDisplayList());

    } else {
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife2");
    }
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword", 13,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot", 13,
                                   gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang", 50,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield", 49,
                                   gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot");
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield");
    }
    if (CVarGetInteger("gEnhancements.FixHammerHand", 0) && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1", 92,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2", 93, gsSPEndDisplayList());
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2");
    }
}

void RegisterPatchHandHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(
        [](int32_t sceneNum) { UpdatePatchHand(); });
}

void RegisterResetNaviTimer() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int32_t sceneNum) {
        if (CVarGetInteger(CVAR_ENHANCEMENT("ResetNaviTimer"), 0)) {
            gSaveContext.naviTimer = 0;
        }
    });
}

void RegisterBossDefeatTimestamps() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnBossDefeat>([](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        switch (actor->id) {
            case ACTOR_BOSS_DODONGO:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_KING_DODONGO] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_FD2:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_VOLVAGIA] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_GANON:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_GANONDORF] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_GANON2:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_GANON] = GAMEPLAYSTAT_TOTAL_TIME;
                gSaveContext.ship.stats.gameComplete = true;
                break;
            case ACTOR_BOSS_GANONDROF:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_PHANTOM_GANON] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_GOMA:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_GOHMA] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_MO:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_MORPHA] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_SST:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_BONGO_BONGO] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_TW:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_TWINROVA] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
            case ACTOR_BOSS_VA:
                gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_DEFEAT_BARINADE] = GAMEPLAYSTAT_TOTAL_TIME;
                break;
        }
    });
}

void UpdateHurtContainerModeState(bool newState) {
    static bool hurtEnabled = false;
    if (hurtEnabled == newState) {
        return;
    }

    hurtEnabled = newState;
    uint16_t getHeartPieces = gSaveContext.ship.stats.heartPieces / 4;
    uint16_t getHeartContainers = gSaveContext.ship.stats.heartContainers;

    if (hurtEnabled) {
        gSaveContext.healthCapacity = 320 - ((getHeartPieces + getHeartContainers) * 16);
    } else {
        gSaveContext.healthCapacity = 48 + ((getHeartPieces + getHeartContainers) * 16);
    }
}

void RegisterHurtContainerModeHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(
        [](int32_t fileNum) { UpdateHurtContainerModeState(CVarGetInteger(CVAR_ENHANCEMENT("HurtContainer"), 0)); });
}

void RegisterRandomizedEnemySizes() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* refActor) {
        // Randomized Enemy Sizes
        Player* player = GET_PLAYER(gPlayState);
        Actor* actor = static_cast<Actor*>(refActor);

        // Exclude wobbly platforms in Jabu because they need to act like platforms.
        // Exclude demo effect for Zora sapphire being re-categorized as a "boss".
        // Exclude Dead Hand hands and Bongo Bongo main body because they make the fights (near) impossible.
        uint8_t excludedEnemy = actor->id == ACTOR_EN_BROB || actor->id == ACTOR_EN_DHA ||
                                actor->id == ACTOR_DEMO_EFFECT || (actor->id == ACTOR_BOSS_SST && actor->params == -1);

        // Dodongo, Volvagia and Dead Hand are always smaller because they're impossible when bigger.
        uint8_t smallOnlyEnemy = actor->id == ACTOR_BOSS_DODONGO || actor->id == ACTOR_BOSS_FD ||
                                 actor->id == ACTOR_BOSS_FD2 || actor->id == ACTOR_EN_DH;

        // Only apply to enemies and bosses.
        if (!CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemySizes"), 0) ||
            (actor->category != ACTORCAT_ENEMY && actor->category != ACTORCAT_BOSS) || excludedEnemy) {
            return;
        }

        float randomNumber;
        float randomScale;

        uint8_t bigActor = rand() % 2;

        // Big actor
        if (bigActor && !smallOnlyEnemy) {
            randomNumber = rand() % 200;
            // Between 100% and 300% size.
            randomScale = 1.0f + (randomNumber / 100);
        } else {
            // Small actor
            randomNumber = rand() % 90;
            // Between 10% and 100% size.
            randomScale = 0.1f + (randomNumber / 100);
        }

        Actor_SetScale(actor, actor->scale.z * randomScale);

        if (CVarGetInteger(CVAR_ENHANCEMENT("EnemySizeScalesHealth"), 0) && (actor->category == ACTORCAT_ENEMY)) {
            // Scale the health based on a smaller factor than randomScale
            float healthScalingFactor = 0.8f; // Adjust this factor as needed
            float scaledHealth = actor->colChkInfo.health * (randomScale * healthScalingFactor);

            // Ensure the scaled health doesn't go below zero
            actor->colChkInfo.health = fmax(scaledHealth, 1.0f);
        } else {
            return;
        }
    });
}

void RegisterFloorSwitchesHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        if (actor->id != ACTOR_OBJ_SWITCH || !CVarGetInteger(CVAR_ENHANCEMENT("FixFloorSwitches"), 0)) {
            return;
        }

        ObjSwitch* switchActor = reinterpret_cast<ObjSwitch*>(actor);
        s32 type = (switchActor->dyna.actor.params & 7);

        if (switchActor->dyna.actor.params == 0x1200 || switchActor->dyna.actor.params == 0x3A00) {
            switchActor->dyna.actor.world.pos.y -= 1;
        }
    });
}

void RegisterCustomSkeletons() {
    static int8_t previousTunic = -1;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded() || gPlayState == NULL) {
            return;
        }

        if (CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC) != previousTunic) {
            SOH::SkeletonPatcher::UpdateCustomSkeletons();
        }
        previousTunic = CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC);
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnAssetAltChange>([]() {
        if (!GameInteractor::IsSaveLoaded() || gPlayState == NULL) {
            return;
        }

        SOH::SkeletonPatcher::UpdateCustomSkeletons();
    });
}

void InitMods() {
    RandomizerRegisterHooks();
    TimeSaverRegisterHooks();
    RegisterTTS();
    RegisterOcarinaTimeTravel();
    RegisterPermanentHeartLoss();
    RegisterDeleteFileOnDeath();
    RegisterHyperBosses();
    UpdateHyperEnemiesState();
    RegisterMenuPathFix();
    RegisterMirrorModeHandler();
    RegisterResetNaviTimer();
    RegisterBossDefeatTimestamps();
    RegisterRandomizedEnemySizes();
    RegisterFloorSwitchesHook();
    RegisterPatchHandHandler();
    RegisterHurtContainerModeHandler();
    RandoKaleido_RegisterHooks();
    RegisterCustomSkeletons();
}

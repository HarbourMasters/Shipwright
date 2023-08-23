#include "mods.h"
#include <libultraship/bridge.h>
#include "game-interactor/GameInteractor.h"
#include "tts/tts.h"
#include "soh/Enhancements/boss-rush/BossRushTypes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#include "soh/Enhancements/nametag.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "functions.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;

uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);
}
bool performDelayedSave = false;
bool performSave = false;

// TODO: When there's more uses of something like this, create a new GI::RawAction?
void ReloadSceneTogglingLinkAge() {
    gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;
    gPlayState->sceneLoadFlag = 0x14;
    gPlayState->fadeTransition = 11;
    gSaveContext.nextTransitionType = 11;
    gPlayState->linkAgeOnLoad ^= 1; // toggle linkAgeOnLoad
}

void RegisterInfiniteMoney() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (CVarGetInteger("gInfiniteMoney", 0) != 0) {
            if (gSaveContext.rupees < CUR_CAPACITY(UPG_WALLET)) {
                gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
            }
        }
    });
}

void RegisterInfiniteHealth() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (CVarGetInteger("gInfiniteHealth", 0) != 0) {
            if (gSaveContext.health < gSaveContext.healthCapacity) {
                gSaveContext.health = gSaveContext.healthCapacity;
            }
        }
    });
}

void RegisterInfiniteAmmo() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (CVarGetInteger("gInfiniteAmmo", 0) != 0) {
            // Deku Sticks
            if (AMMO(ITEM_STICK) < CUR_CAPACITY(UPG_STICKS)) {
                AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
            }

            // Deku Nuts
            if (AMMO(ITEM_NUT) < CUR_CAPACITY(UPG_NUTS)) {
                AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
            }

            // Bombs
            if (AMMO(ITEM_BOMB) < CUR_CAPACITY(UPG_BOMB_BAG)) {
                AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
            }

            // Fairy Bow (Ammo)
            if (AMMO(ITEM_BOW) < CUR_CAPACITY(UPG_QUIVER)) {
                AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
            }

            // Fairy Slingshot (Ammo)
            if (AMMO(ITEM_SLINGSHOT) < CUR_CAPACITY(UPG_BULLET_BAG)) {
                AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
            }

            // Bombchus (max: 50, no upgrades)
            if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU && AMMO(ITEM_BOMBCHU) < 50) {
                AMMO(ITEM_BOMBCHU) = 50;
            }
        }
    });
}

void RegisterInfiniteMagic() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (CVarGetInteger("gInfiniteMagic", 0) != 0) {
            if (gSaveContext.isMagicAcquired && gSaveContext.magic != (gSaveContext.isDoubleMagicAcquired + 1) * 0x30) {
                gSaveContext.magic = (gSaveContext.isDoubleMagicAcquired + 1) * 0x30;
            }
        }
    });
}

void RegisterInfiniteNayrusLove() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (CVarGetInteger("gInfiniteNayru", 0) != 0) {
            gSaveContext.nayrusLoveTimer = 0x44B;
        }
    });
}

void RegisterMoonJumpOnL() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!gPlayState) return;
        
        if (CVarGetInteger("gMoonJumpOnL", 0) != 0) {
            Player* player = GET_PLAYER(gPlayState);

            if (CHECK_BTN_ANY(gPlayState->state.input[0].cur.button, BTN_L)) {
                player->actor.velocity.y = 6.34375f;
            }
        }
    });
}


void RegisterInfiniteISG() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!gPlayState) return;

        if (CVarGetInteger("gEzISG", 0) != 0) {
            Player* player = GET_PLAYER(gPlayState);
            player->swordState = 1;
        }
    });
}

void RegisterUnrestrictedItems() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!gPlayState) return;

        if (CVarGetInteger("gNoRestrictItems", 0) != 0) {
            u8 sunsBackup = gPlayState->interfaceCtx.restrictions.sunsSong;
            memset(&gPlayState->interfaceCtx.restrictions, 0, sizeof(gPlayState->interfaceCtx.restrictions));
            gPlayState->interfaceCtx.restrictions.sunsSong = sunsBackup;
        }
    });
}

void RegisterFreezeTime() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (CVarGetInteger("gFreezeTime", 0) != 0) {
            if (CVarGetInteger("gPrevTime", -1) == -1) {
                CVarSetInteger("gPrevTime", gSaveContext.dayTime);
            }

            int32_t prevTime = CVarGetInteger("gPrevTime", gSaveContext.dayTime);
            gSaveContext.dayTime = prevTime;
        } else {
            CVarClear("gPrevTime");
        }
    });
}

/// Switches Link's age and respawns him at the last entrance he entered.
void RegisterSwitchAge() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        static bool warped = false;
        static Vec3f playerPos;
        static int16_t playerYaw;
        static RoomContext* roomCtx;
        static s32 roomNum;

        if (!gPlayState) return;

        if (CVarGetInteger("gSwitchAge", 0) && !warped) {
            playerPos = GET_PLAYER(gPlayState)->actor.world.pos;
            playerYaw = GET_PLAYER(gPlayState)->actor.shape.rot.y;
            roomCtx = &gPlayState->roomCtx;
            roomNum = roomCtx->curRoom.num;
            ReloadSceneTogglingLinkAge();
            warped = true;
        }

        if (warped && gPlayState->sceneLoadFlag != 0x0014 &&
            gSaveContext.nextTransitionType == 255) {
            GET_PLAYER(gPlayState)->actor.shape.rot.y = playerYaw;
            GET_PLAYER(gPlayState)->actor.world.pos = playerPos;
            if (roomNum != roomCtx->curRoom.num) {
                func_8009728C(gPlayState, roomCtx, roomNum); //load original room
                //func_800973FC(gPlayState, &gPlayState->roomCtx); // commit to room load?
                func_80097534(gPlayState, roomCtx);  // load map for new room (unloading the previous room)
            }
            warped = false;
            CVarSetInteger("gSwitchAge", 0);
        }
    });
}

/// Switches Link's age and respawns him at the last entrance he entered.
void RegisterOcarinaTimeTravel() {

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnOcarinaSongAction>([]() {
        if (!gPlayState) {
            return;
        }

        Actor* player = &GET_PLAYER(gPlayState)->actor;
        Actor* nearbyTimeBlockEmpty = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_WARP2BLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyTimeBlock = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_TIMEBLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyOcarinaSpot = Actor_FindNearby(gPlayState, player, ACTOR_EN_OKARINA_TAG, ACTORCAT_PROP, 120.0f);
        Actor* nearbyDoorOfTime = Actor_FindNearby(gPlayState, player, ACTOR_DOOR_TOKI, ACTORCAT_BG, 500.0f);
        Actor* nearbyFrogs = Actor_FindNearby(gPlayState, player, ACTOR_EN_FR, ACTORCAT_NPC, 300.0f);
        uint8_t hasMasterSword = (gBitFlags[ITEM_SWORD_MASTER - ITEM_SWORD_KOKIRI] << gEquipShifts[EQUIP_SWORD]) & gSaveContext.inventory.equipment;
        uint8_t hasOcarinaOfTime = (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME);
        // If TimeTravel + Player have the Ocarina of Time + Have Master Sword + is in proper range
        // TODO: Once Swordless Adult is fixed: Remove the Master Sword check
        if (((CVarGetInteger("gTimeTravel", 0) == 1 && hasOcarinaOfTime) || CVarGetInteger("gTimeTravel", 0) == 2) && hasMasterSword &&
            gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_TIME && !nearbyTimeBlockEmpty && !nearbyTimeBlock &&
            !nearbyOcarinaSpot && !nearbyFrogs) {

            if (gSaveContext.n64ddFlag) {
                CVarSetInteger("gSwitchTimeline", 1);
            } else if (!gSaveContext.n64ddFlag && !nearbyDoorOfTime) {
                // This check is made for when Link is learning the Song Of Time in a vanilla save file that load a
                // Temple of Time scene where the only object present is the Door of Time
                CVarSetInteger("gSwitchTimeline", 1);
            }
            ReloadSceneTogglingLinkAge();
        }
    });
}

void AutoSave(GetItemEntry itemEntry) {
    u8 item = itemEntry.itemId;
    // Don't autosave immediately after buying items from shops to prevent getting them for free!
    // Don't autosave in the Chamber of Sages since resuming from that map breaks the game
    // Don't autosave during the Ganon fight when picking up the Master Sword
    // Don't autosave in grottos since resuming from grottos breaks the game.
    if ((CVarGetInteger("gAutosave", AUTOSAVE_OFF) != AUTOSAVE_OFF) && (gPlayState != NULL) && (gSaveContext.pendingSale == ITEM_NONE) &&
        (gPlayState->gameplayFrames > 60 && gSaveContext.cutsceneIndex < 0xFFF0) && (gPlayState->sceneNum != SCENE_GANON_DEMO)) {
        if (((CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_ALL_ITEMS) || (CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_ALL_ITEMS)) && (item != ITEM_NONE)) {
            // Autosave for all items
            performSave = true;

        } else if (((CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_MAJOR_ITEMS) || (CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_MAJOR_ITEMS)) && (item != ITEM_NONE)) {
            // Autosave for major items
            if (itemEntry.modIndex == 0) {
                switch (item) {
                    case ITEM_STICK:
                    case ITEM_NUT:
                    case ITEM_BOMB:
                    case ITEM_BOW:
                    case ITEM_SEEDS:
                    case ITEM_FISHING_POLE:
                    case ITEM_MAGIC_SMALL:
                    case ITEM_MAGIC_LARGE:
                    case ITEM_INVALID_4:
                    case ITEM_INVALID_5:
                    case ITEM_INVALID_6:
                    case ITEM_INVALID_7:
                    case ITEM_HEART:
                    case ITEM_RUPEE_GREEN:
                    case ITEM_RUPEE_BLUE:
                    case ITEM_RUPEE_RED:
                    case ITEM_RUPEE_PURPLE:
                    case ITEM_RUPEE_GOLD:
                    case ITEM_INVALID_8:
                    case ITEM_STICKS_5:
                    case ITEM_STICKS_10:
                    case ITEM_NUTS_5:
                    case ITEM_NUTS_10:
                    case ITEM_BOMBS_5:
                    case ITEM_BOMBS_10:
                    case ITEM_BOMBS_20:
                    case ITEM_BOMBS_30:
                    case ITEM_ARROWS_SMALL:
                    case ITEM_ARROWS_MEDIUM:
                    case ITEM_ARROWS_LARGE:
                    case ITEM_SEEDS_30:
                    case ITEM_NONE:
                        break;
                    case ITEM_BOMBCHU:
                    case ITEM_BOMBCHUS_5:
                    case ITEM_BOMBCHUS_20:
                        if (!CVarGetInteger("gBombchuDrops", 0)) {
                            performSave = true;
                        }
                        break;
                    default:
                        performSave = true;
                        break;
                }
            } else if (itemEntry.modIndex == 1 && item != RG_ICE_TRAP) {
                performSave = true;
            }
        } else if (CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_MAJOR_ITEMS ||
                   CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_ALL_ITEMS ||
                   CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION) {
            performSave = true;
        }
        if ((gPlayState->sceneNum == SCENE_YOUSEI_IZUMI_TATE) || (gPlayState->sceneNum == SCENE_KAKUSIANA) ||
                (gPlayState->sceneNum == SCENE_KENJYANOMA)) {
            if (CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_MAJOR_ITEMS ||
                CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_ALL_ITEMS ||
                CVarGetInteger("gAutosave", AUTOSAVE_OFF) == AUTOSAVE_LOCATION) {
                performSave = false;
                return;
            }
            if (performSave) {
                performSave = false;
                performDelayedSave = true;
            }
            return;
        }
        if (performSave || performDelayedSave) {
            Play_PerformSave(gPlayState);
            performSave = false;
            performDelayedSave = false;
        }
    }
}

void RegisterAutoSave() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) { AutoSave(itemEntry); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaleEnd>([](GetItemEntry itemEntry) { AutoSave(itemEntry); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>([](int32_t sceneNum) { AutoSave(GET_ITEM_NONE); });
}

void RegisterRupeeDash() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!CVarGetInteger("gRupeeDash", 0)) {
            return;
        }
        
        // Initialize Timer
        static uint16_t rupeeDashTimer = 0;
        uint16_t rdmTime = CVarGetInteger("gDashInterval", 5) * 20;
        
        // Did time change by DashInterval?
        if (rupeeDashTimer >= rdmTime) {
            rupeeDashTimer = 0;
            if (gSaveContext.rupees > 0) {
                Rupees_ChangeBy(-1);
            } else {
                Health_ChangeBy(gPlayState, -16);
            }
        } else {
            rupeeDashTimer++;
        }
    });
}

void RegisterShadowTag() {
    static bool shouldSpawn = false;
    static uint16_t delayTimer = 60;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!CVarGetInteger("gShadowTag", 0)) {
            return;
        }
        if (shouldSpawn && (delayTimer <= 0)) {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_WALLMAS, 0, 0, 0, 0, 0, 0, 3, false);
            shouldSpawn = false;
        } else {
            delayTimer--;
        }
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() {
        shouldSpawn = true;
        delayTimer = 60;
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
        shouldSpawn = true;
        delayTimer = 60;
    });
}

struct DayTimeGoldSkulltulas {
    uint16_t scene;
    uint16_t room;
    bool forChild;
    std::vector<ActorEntry> actorEntries;
};

using DayTimeGoldSkulltulasList = std::vector<DayTimeGoldSkulltulas>;

void RegisterDaytimeGoldSkultullas() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() {
        if (!CVarGetInteger("gNightGSAlwaysSpawn", 0)) {
            return;
        }

        // Gold Skulltulas that are not part of the scene actor list during the day
        // Actor values copied from the night time scene actor list
        static const DayTimeGoldSkulltulasList dayTimeGoldSkulltulas = {
            // Graveyard
            { SCENE_SPOT02, 1, true, { { ACTOR_EN_SW, { 156, 315, 795 }, { 16384, -32768, 0 }, -20096 } } },
            // ZF
            { SCENE_SPOT08, 0, true, { { ACTOR_EN_SW, { -1891, 187, 1911 }, { 16384, 18022, 0 }, -19964 } } },
            // GF
            { SCENE_SPOT12, 0, false, { { ACTOR_EN_SW, { 1598, 999, -2008 }, { 16384, -16384, 0 }, -19198 } } },
            { SCENE_SPOT12, 1, false, { { ACTOR_EN_SW, { 3377, 1734, -4935 }, { 16384, 0, 0 }, -19199 } } },
            // Kak
            { SCENE_SPOT01, 0, false, { { ACTOR_EN_SW, { -18, 540, 1800 }, { 0, -32768, 0 }, -20160 } } },
            { SCENE_SPOT01,
              0,
              true,
              { { ACTOR_EN_SW, { -465, 377, -888 }, { 0, 28217, 0 }, -20222 },
                { ACTOR_EN_SW, { 5, 686, -171 }, { 0, -32768, 0 }, -20220 },
                { ACTOR_EN_SW, { 324, 270, 905 }, { 16384, 0, 0 }, -20216 },
                { ACTOR_EN_SW, { -602, 120, 1120 }, { 16384, 0, 0 }, -20208 } } },
            // LLR
            { SCENE_SPOT20,
              0,
              true,
              { { ACTOR_EN_SW, { -2344, 180, 672 }, { 16384, 22938, 0 }, -29695 },
                { ACTOR_EN_SW, { 808, 48, 326 }, { 16384, 0, 0 }, -29694 },
                { ACTOR_EN_SW, { 997, 286, -2698 }, { 16384, -16384, 0 }, -29692 } } },
        };

        for (const auto& dayTimeGS : dayTimeGoldSkulltulas) {
            if (IS_DAY && dayTimeGS.forChild == LINK_IS_CHILD && dayTimeGS.scene == gPlayState->sceneNum &&
                dayTimeGS.room == gPlayState->roomCtx.curRoom.num) {
                for (const auto& actorEntry : dayTimeGS.actorEntries) {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, actorEntry.id, actorEntry.pos.x, actorEntry.pos.y,
                                actorEntry.pos.z, actorEntry.rot.x, actorEntry.rot.y, actorEntry.rot.z,
                                actorEntry.params, false);
                }
            }
        }
    });
}

void RegisterHyperBosses() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* refActor) {
        // Run the update function a second time to make bosses move and act twice as fast.

        Player* player = GET_PLAYER(gPlayState);
        Actor* actor = static_cast<Actor*>(refActor);

        uint8_t isBossActor =
            actor->id == ACTOR_BOSS_GOMA ||                              // Gohma
            actor->id == ACTOR_BOSS_DODONGO ||                           // King Dodongo
            actor->id == ACTOR_EN_BDFIRE ||                              // King Dodongo Fire Breath
            actor->id == ACTOR_BOSS_VA ||                                // Barinade
            actor->id == ACTOR_BOSS_GANONDROF ||                         // Phantom Ganon
            actor->id == ACTOR_EN_FHG_FIRE ||                            // Phantom Ganon/Ganondorf Energy Ball/Thunder
            actor->id == ACTOR_EN_FHG ||                                 // Phantom Ganon's Horse
            actor->id == ACTOR_BOSS_FD || actor->id == ACTOR_BOSS_FD2 || // Volvagia (grounded/flying)
            actor->id == ACTOR_EN_VB_BALL ||                             // Volvagia Rocks
            actor->id == ACTOR_BOSS_MO ||                                // Morpha
            actor->id == ACTOR_BOSS_SST ||                               // Bongo Bongo
            actor->id == ACTOR_BOSS_TW ||                                // Twinrova
            actor->id == ACTOR_BOSS_GANON ||                             // Ganondorf
            actor->id == ACTOR_BOSS_GANON2;                              // Ganon

        uint8_t hyperBossesActive =
            CVarGetInteger("gHyperBosses", 0) ||
            (gSaveContext.isBossRush &&
             gSaveContext.bossRushOptions[BR_OPTIONS_HYPERBOSSES] == BR_CHOICE_HYPERBOSSES_YES);

        // Don't apply during cutscenes because it causes weird behaviour and/or crashes on some bosses.
        if (hyperBossesActive && isBossActor && !Player_InBlockingCsMode(gPlayState, player)) {
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

void RegisterHyperEnemies() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* refActor) {
        // Run the update function a second time to make enemies and minibosses move and act twice as fast.

        Player* player = GET_PLAYER(gPlayState);
        Actor* actor = static_cast<Actor*>(refActor);

        // Some enemies are not in the ACTORCAT_ENEMY category, and some are that aren't really enemies.
        bool isEnemy = actor->category == ACTORCAT_ENEMY || actor->id == ACTOR_EN_TORCH2;
        bool isExcludedEnemy = actor->id == ACTOR_EN_FIRE_ROCK || actor->id == ACTOR_EN_ENCOUNT2;

        // Don't apply during cutscenes because it causes weird behaviour and/or crashes on some cutscenes.
        if (CVarGetInteger("gHyperEnemies", 0) && isEnemy && !isExcludedEnemy &&
            !Player_InBlockingCsMode(gPlayState, player)) {
            GameInteractor::RawAction::UpdateActor(actor);
        }
    });
}

void RegisterBonkDamage() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerBonk>([]() {
        uint8_t bonkOption = CVarGetInteger("gBonkDamageMul", BONK_DAMAGE_NONE);
        if (bonkOption == BONK_DAMAGE_NONE) {
            return;
        }

        if (bonkOption == BONK_DAMAGE_OHKO) {
            gSaveContext.health = 0;
            return;
        }

        uint16_t bonkDamage = 0;
        switch (bonkOption) {
            case BONK_DAMAGE_QUARTER_HEART:
                bonkDamage = 4;
                break;
            case BONK_DAMAGE_HALF_HEART:
                bonkDamage = 8;
                break;
            case BONK_DAMAGE_1_HEART:
                bonkDamage = 16;
                break;
            case BONK_DAMAGE_2_HEARTS:
                bonkDamage = 32;
                break;
            case BONK_DAMAGE_4_HEARTS:
                bonkDamage = 64;
                break;
            case BONK_DAMAGE_8_HEARTS:
                bonkDamage = 128;
                break;
            default:
                break;
        }
        
        Health_ChangeBy(gPlayState, -bonkDamage);
        // Set invincibility to make Link flash red as a visual damage indicator.
        Player* player = GET_PLAYER(gPlayState);
        player->invincibilityTimer = 28;
    });
}

void UpdateDirtPathFixState(int32_t sceneNum) {
    switch (sceneNum) {
        case SCENE_SPOT00:
        case SCENE_SPOT04:
        case SCENE_SPOT15:
            CVarSetInteger("gZFightingMode", CVarGetInteger("gSceneSpecificDirtPathFix", ZFIGHT_FIX_DISABLED));
            return;
        default:
            CVarClear("gZFightingMode");
    }
}

void RegisterMenuPathFix() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>([](int32_t sceneNum) {
        UpdateDirtPathFixState(sceneNum);
    });
}

void UpdateMirrorModeState(int32_t sceneNum) {
    static bool prevMirroredWorld = false;
    bool nextMirroredWorld = false;

    int16_t mirroredMode = CVarGetInteger("gMirroredWorldMode", MIRRORED_WORLD_OFF);
    int16_t inDungeon = (sceneNum >= SCENE_YDAN && sceneNum <= SCENE_GANONTIKA_SONOGO && sceneNum != SCENE_GERUDOWAY) ||
                        (sceneNum >= SCENE_YDAN_BOSS && sceneNum <= SCENE_GANON_FINAL) ||
                        (sceneNum == SCENE_GANON_DEMO);

    if (mirroredMode == MIRRORED_WORLD_RANDOM_SEEDED || mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED) {
        uint32_t seed = sceneNum + (gSaveContext.n64ddFlag ? (gSaveContext.seedIcons[0] + gSaveContext.seedIcons[1] +
                        gSaveContext.seedIcons[2] + gSaveContext.seedIcons[3] + gSaveContext.seedIcons[4]) : gSaveContext.sohStats.fileCreatedAt);
        Random_Init(seed);
    }

    bool randomMirror = Random(0, 2) == 1;

    if (
        mirroredMode == MIRRORED_WORLD_ALWAYS ||
        ((mirroredMode == MIRRORED_WORLD_RANDOM || mirroredMode == MIRRORED_WORLD_RANDOM_SEEDED) && randomMirror) ||
        // Dungeon modes
        (inDungeon && (mirroredMode == MIRRORED_WORLD_DUNGEONS_All ||
         (mirroredMode == MIRRORED_WORLD_DUNGEONS_VANILLA && !ResourceMgr_IsSceneMasterQuest(sceneNum)) ||
         (mirroredMode == MIRRORED_WORLD_DUNGEONS_MQ && ResourceMgr_IsSceneMasterQuest(sceneNum)) ||
         ((mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM || mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED) && randomMirror)))
    ) {
        nextMirroredWorld = true;
        CVarSetInteger("gMirroredWorld", 1);
    } else {
        nextMirroredWorld = false;
        CVarClear("gMirroredWorld");
    }

    if (prevMirroredWorld != nextMirroredWorld) {
        prevMirroredWorld = nextMirroredWorld;
        ApplyMirrorWorldGfxPatches();
    }
}

void RegisterMirrorModeHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int32_t sceneNum) {
        UpdateMirrorModeState(sceneNum);
    });
}

void InitMods() {
    RegisterTTS();
    RegisterInfiniteMoney();
    RegisterInfiniteHealth();
    RegisterInfiniteAmmo();
    RegisterInfiniteMagic();
    RegisterInfiniteNayrusLove();
    RegisterMoonJumpOnL();
    RegisterInfiniteISG();
    RegisterUnrestrictedItems();
    RegisterFreezeTime();
    RegisterSwitchAge();
    RegisterOcarinaTimeTravel();
    RegisterAutoSave();
    RegisterDaytimeGoldSkultullas();
    RegisterRupeeDash();
    RegisterShadowTag();
    RegisterHyperBosses();
    RegisterHyperEnemies();
    RegisterBonkDamage();
    RegisterMenuPathFix();
    RegisterMirrorModeHandler();
    NameTag_RegisterHooks();
}

#include "mods.h"
#include <libultraship/bridge.h>
#include "game-interactor/GameInteractor.h"
#include "tts/tts.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "functions.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Play_PerformSave(PlayState* play);
extern s32 Health_ChangeBy(PlayState* play, s16 healthChange);
extern void Rupees_ChangeBy(s16 rupeeChange);
extern Actor* Actor_Spawn(ActorContext* actorCtx, PlayState* play, s16 actorId, f32 posX, f32 posY, f32 posZ,
                            s16 rotX, s16 rotY, s16 rotZ, s16 params, s16 canRandomize);
extern void Inventory_ChangeEquipment(s16 equipment, u16 value);
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
            CVarSetInteger("gPrevTime", -1);
        }
    });
}

/// Switches Link's age and respawns him at the last entrance he entered.
void RegisterSwitchAge() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        static bool warped = false;
        static Vec3f playerPos;
        static int16_t playerYaw;

        if (!gPlayState) return;

        if (CVarGetInteger("gSwitchAge", 0) != 0) {
            CVarSetInteger("gSwitchAge", 0);
            playerPos = GET_PLAYER(gPlayState)->actor.world.pos;
            playerYaw = GET_PLAYER(gPlayState)->actor.shape.rot.y;

            ReloadSceneTogglingLinkAge();

            warped = true;
        }

        if (warped && gPlayState->sceneLoadFlag != 0x0014 && gSaveContext.nextTransitionType == 255) {
            GET_PLAYER(gPlayState)->actor.shape.rot.y = playerYaw;
            GET_PLAYER(gPlayState)->actor.world.pos = playerPos;
            warped = false;
        }
    });
}

/// Switches Link's age and respawns him at the last entrance he entered.
void RegisterOcarinaTimeTravel() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!gPlayState) return;

        // For the gTimeTravel: Don't give child Link a Kokiri Sword if we don't have one
        if (LINK_AGE_IN_YEARS == 5 && CVarGetInteger("gTimeTravel", 0)) {
            uint32_t kokiriSwordBitMask = 1 << 0;
            if (!(gSaveContext.inventory.equipment & kokiriSwordBitMask)) {
                Player* player = GET_PLAYER(gPlayState);
                player->currentSwordItemId = ITEM_NONE;
                gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_NONE);
            }
        }

        // Switches Link's age and respawns him at the last entrance he entered.
        if (CVarGetInteger("gTimeTravel", 0) && CVarGetInteger("gSwitchTimeline", 0)) {
            CVarSetInteger("gSwitchTimeline", 0);
            ReloadSceneTogglingLinkAge();
        }
    });

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
        if (CVarGetInteger("gTimeTravel", 0) && hasOcarinaOfTime && hasMasterSword &&
            gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_TIME && !nearbyTimeBlockEmpty && !nearbyTimeBlock &&
            !nearbyOcarinaSpot && !nearbyFrogs) {
            if (gSaveContext.n64ddFlag) {
                CVarSetInteger("gSwitchTimeline", 1);
            } else if (!gSaveContext.n64ddFlag && !nearbyDoorOfTime) {
                // This check is made for when Link is learning the Song Of Time in a vanilla save file that load a
                // Temple of Time scene where the only object present is the Door of Time
                CVarSetInteger("gSwitchTimeline", 1);
            }
        }
    });
}

void AutoSave(GetItemEntry itemEntry) {
    u8 item = itemEntry.itemId;
    // Don't autosave immediately after buying items from shops to prevent getting them for free!
    // Don't autosave in the Chamber of Sages since resuming from that map breaks the game
    // Don't autosave during the Ganon fight when picking up the Master Sword
    // Don't autosave in grottos since resuming from grottos breaks the game.
    if ((CVarGetInteger("gAutosave", 0) > 0) && (gPlayState != NULL) && (gSaveContext.pendingSale == ITEM_NONE) &&
        (gPlayState->gameplayFrames > 60 && gSaveContext.cutsceneIndex < 0xFFF0) && (gPlayState->sceneNum != SCENE_GANON_DEMO)) {
        if (((CVarGetInteger("gAutosave", 0) == 2) || (CVarGetInteger("gAutosave", 0) == 5)) && (item != ITEM_NONE)) {
            // Autosave for all items
            performSave = true;

        } else if (((CVarGetInteger("gAutosave", 0) == 1) || (CVarGetInteger("gAutosave", 0) == 4)) && (item != ITEM_NONE)) {
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
        } else if ((CVarGetInteger("gAutosave", 0) > 0 && (CVarGetInteger("gAutosave", 0) < 4))) {
            performSave = true;
        }
        if ((gPlayState->sceneNum == SCENE_YOUSEI_IZUMI_TATE) || (gPlayState->sceneNum == SCENE_KAKUSIANA) ||
                (gPlayState->sceneNum == SCENE_KENJYANOMA)) {
            if ((CVarGetInteger("gAutosave", 0) > 0 && (CVarGetInteger("gAutosave", 0) < 4))) {
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
            actor->id == ACTOR_BOSS_VA ||                                // Barinade
            actor->id == ACTOR_BOSS_GANONDROF ||                         // Phantom Ganon
            (actor->id == 0 && actor->category == ACTORCAT_BOSS) ||      // Phantom Ganon/Ganondorf Energy Ball/Thunder
            actor->id == ACTOR_EN_FHG ||                                 // Phantom Ganon's Horse
            actor->id == ACTOR_BOSS_FD || actor->id == ACTOR_BOSS_FD2 || // Volvagia (grounded/flying)
            actor->id == ACTOR_BOSS_MO ||                                // Morpha
            actor->id == ACTOR_BOSS_SST ||                               // Bongo Bongo
            actor->id == ACTOR_BOSS_TW ||                                // Twinrova
            actor->id == ACTOR_BOSS_GANON ||                             // Ganondorf
            actor->id == ACTOR_BOSS_GANON2;                              // Ganon

        // Don't apply during cutscenes because it causes weird behaviour and/or crashes on some bosses.
        if (CVarGetInteger("gHyperBosses", 0) && isBossActor && !Player_InBlockingCsMode(gPlayState, player)) {
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

void RegisterBonkDamage() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerBonk>([]() {
        uint8_t bonkOption = CVarGetInteger("gBonkDamageMul", 0);
        uint16_t bonkDamage = 0;
        switch (bonkOption) {
            // Quarter heart
            case 1:
                bonkDamage = 4;
                break;
            // Half a heart
            case 2:
                bonkDamage = 8;
                break;
            // Full heart
            case 3:
                bonkDamage = 16;
                break;
            // 2 hearts
            case 4:
                bonkDamage = 32;
                break;
            // 4 hearts
            case 5:
                bonkDamage = 64;
                break;
            // 8 hearts
            case 6:
                bonkDamage = 128;
                break;
            case 0:
            case 7:
            default:
                break;
        }
        // OHKO
        if (bonkOption == 7) {
            gSaveContext.health = 0;
        } else if (bonkDamage) {
            Health_ChangeBy(gPlayState, -bonkDamage);
            // Set invincibility to make Link flash red as a visual damage indicator.
            Player* player = GET_PLAYER(gPlayState);
            player->invincibilityTimer = 28;
        }
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
    RegisterHyperBosses();
    RegisterBonkDamage();
}

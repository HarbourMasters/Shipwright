#include "mods.h"
#include <libultraship/bridge.h>
#include "game-interactor/GameInteractor.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "variables.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Play_PerformSave(PlayState* play);
extern s32 Health_ChangeBy(PlayState* play, s16 healthChange);
extern void Rupees_ChangeBy(s16 rupeeChange);
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

            gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;
            gPlayState->sceneLoadFlag = 0x14;
            gPlayState->fadeTransition = 11;
            gSaveContext.nextTransitionType = 11;
            gPlayState->linkAgeOnLoad ^= 1;

            warped = true;
        }

        if (warped && gPlayState->sceneLoadFlag != 0x0014 && gSaveContext.nextTransitionType == 255) {
            GET_PLAYER(gPlayState)->actor.shape.rot.y = playerYaw;
            GET_PLAYER(gPlayState)->actor.world.pos = playerPos;
            warped = false;
        }
    });
}

void RegisterAutoSave() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnReceiveItem>([](u8 item) {
        // Don't autosave immediately after buying items from shops to prevent getting them for free!
        // Don't autosave in the Chamber of Sages since resuming from that map breaks the game
        // Don't autosave during the Ganon fight when picking up the Master Sword
        if ((CVarGetInteger("gAutosave", 0) > 0) && (gPlayState != NULL) && (gSaveContext.pendingSale == ITEM_NONE) &&
            (gPlayState->sceneNum != SCENE_KENJYANOMA) && (gPlayState->sceneNum != SCENE_GANON_DEMO)) {
            if ((CVarGetInteger("gAutosave", 0) == 2) || (CVarGetInteger("gAutosave", 0) == 5)) {
                // Autosave for all items
                Play_PerformSave(gPlayState);

            } else if ((CVarGetInteger("gAutosave", 0) == 1) || (CVarGetInteger("gAutosave", 0) == 4)) {
                // Autosave for major items
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
                        break;
                    case ITEM_BOMBCHU:
                    case ITEM_BOMBCHUS_5:
                    case ITEM_BOMBCHUS_20:
                        if (!CVarGetInteger("gBombchuDrops", 0)) {
                            Play_PerformSave(gPlayState);
                        }
                        break;
                    default:
                        Play_PerformSave(gPlayState);
                        break;
                }
            }
        }
    });
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

void InitMods() {
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
    RegisterRupeeDash();
    RegisterAutoSave();
}

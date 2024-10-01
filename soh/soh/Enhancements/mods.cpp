#include "mods.h"
#include <libultraship/bridge.h>
#include "game-interactor/GameInteractor.h"
#include "tts/tts.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/boss-rush/BossRushTypes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include "soh/Enhancements/nametag.h"

#include "src/overlays/actors/ovl_En_Bb/z_en_bb.h"
#include "src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.h"
#include "src/overlays/actors/ovl_En_Mb/z_en_mb.h"
#include "src/overlays/actors/ovl_En_Tite/z_en_tite.h"
#include "src/overlays/actors/ovl_En_Zf/z_en_zf.h"
#include "src/overlays/actors/ovl_En_Wf/z_en_wf.h"
#include "src/overlays/actors/ovl_En_Reeba/z_en_reeba.h"
#include "src/overlays/actors/ovl_En_Peehat/z_en_peehat.h"
#include "src/overlays/actors/ovl_En_Po_Field/z_en_po_field.h"
#include "src/overlays/actors/ovl_En_Poh/z_en_poh.h"
#include "src/overlays/actors/ovl_En_Tp/z_en_tp.h"
#include "src/overlays/actors/ovl_En_Firefly/z_en_firefly.h"
#include "src/overlays/actors/ovl_En_Xc/z_en_xc.h"
#include "src/overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"

extern "C" {
#include <z64.h>
#include "align_asset_macro.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Overlay_DisplayText(float duration, const char* text);
uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);
}

// GreyScaleEndDlist
#define dgEndGrayscaleAndEndDlistDL "__OTR__helpers/cosmetics/gEndGrayscaleAndEndDlistDL"
static const ALIGN_ASSET(2) char gEndGrayscaleAndEndDlistDL[] = dgEndGrayscaleAndEndDlistDL;

// This is used for the Temple of Time Medalions' color
#define dtokinoma_room_0DL_007A70 "__OTR__scenes/shared/tokinoma_scene/tokinoma_room_0DL_007A70"
static const ALIGN_ASSET(2) char tokinoma_room_0DL_007A70[] = dtokinoma_room_0DL_007A70;
#define dtokinoma_room_0DL_007FD0 "__OTR__scenes/shared/tokinoma_scene/tokinoma_room_0DL_007FD0"
static const ALIGN_ASSET(2) char tokinoma_room_0DL_007FD0[] = dtokinoma_room_0DL_007FD0;

// TODO: When there's more uses of something like this, create a new GI::RawAction?
void ReloadSceneTogglingLinkAge() {
    gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;
    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_CIRCLE(TCA_WAVE, TCC_WHITE, TCS_FAST); // Fade Out
    gSaveContext.nextTransitionType = TRANS_TYPE_CIRCLE(TCA_WAVE, TCC_WHITE, TCS_FAST);
    gPlayState->linkAgeOnLoad ^= 1; // toggle linkAgeOnLoad
}

void RegisterInfiniteMoney() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;
        if (CVarGetInteger(CVAR_CHEAT("InfiniteMoney"), 0) != 0) {
            if (gSaveContext.rupees < CUR_CAPACITY(UPG_WALLET)) {
                gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
            }
        }
    });
}

void RegisterInfiniteHealth() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;
        if (CVarGetInteger(CVAR_CHEAT("InfiniteHealth"), 0) != 0) {
            if (gSaveContext.health < gSaveContext.healthCapacity) {
                gSaveContext.health = gSaveContext.healthCapacity;
            }
        }
    });
}

void RegisterInfiniteAmmo() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;
        if (CVarGetInteger(CVAR_CHEAT("InfiniteAmmo"), 0) != 0) {
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
        if (!GameInteractor::IsSaveLoaded(true)) return;
        if (CVarGetInteger(CVAR_CHEAT("InfiniteMagic"), 0) != 0) {
            if (gSaveContext.isMagicAcquired && gSaveContext.magic != (gSaveContext.isDoubleMagicAcquired + 1) * 0x30) {
                gSaveContext.magic = (gSaveContext.isDoubleMagicAcquired + 1) * 0x30;
            }
        }
    });
}

void RegisterInfiniteNayrusLove() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;
        if (CVarGetInteger(CVAR_CHEAT("InfiniteNayru"), 0) != 0) {
            gSaveContext.nayrusLoveTimer = 0x44B;
        }
    });
}

void RegisterMoonJumpOnL() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;
        
        if (CVarGetInteger(CVAR_CHEAT("MoonJumpOnL"), 0) != 0) {
            Player* player = GET_PLAYER(gPlayState);

            if (CHECK_BTN_ANY(gPlayState->state.input[0].cur.button, BTN_L)) {
                player->actor.velocity.y = 6.34375f;
            }
        }
    });
}


void RegisterInfiniteISG() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;

        if (CVarGetInteger(CVAR_CHEAT("EasyISG"), 0) != 0) {
            Player* player = GET_PLAYER(gPlayState);
            player->meleeWeaponState = 1;
        }
    });
}

//Permanent quick put away (QPA) glitched damage value
void RegisterEzQPA() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;

        if (CVarGetInteger(CVAR_CHEAT("EasyQPA"), 0) != 0) {
            Player* player = GET_PLAYER(gPlayState);
            player->meleeWeaponQuads[0].info.toucher.dmgFlags = 0x16171617;
            player->meleeWeaponQuads[1].info.toucher.dmgFlags = 0x16171617;
        }
    });
}

void RegisterUnrestrictedItems() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) return;

        if (CVarGetInteger(CVAR_CHEAT("NoRestrictItems"), 0) != 0) {
            u8 sunsBackup = gPlayState->interfaceCtx.restrictions.sunsSong;
            memset(&gPlayState->interfaceCtx.restrictions, 0, sizeof(gPlayState->interfaceCtx.restrictions));
            gPlayState->interfaceCtx.restrictions.sunsSong = sunsBackup;
        }
    });
}

void RegisterFreezeTime() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (CVarGetInteger(CVAR_CHEAT("FreezeTime"), 0) != 0) {
            if (CVarGetInteger(CVAR_GENERAL("PrevTime"), -1) == -1) {
                CVarSetInteger(CVAR_GENERAL("PrevTime"), gSaveContext.dayTime);
            }

            int32_t prevTime = CVarGetInteger(CVAR_GENERAL("PrevTime"), gSaveContext.dayTime);
            gSaveContext.dayTime = prevTime;
        } else {
            CVarClear(CVAR_GENERAL("PrevTime"));
        }
    });
}

/// Switches Link's age and respawns him at the last entrance he entered.
void RegisterSwitchAge() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        static bool warped = false;

        if (!GameInteractor::IsSaveLoaded(true)) {
            CVarClear(CVAR_GENERAL("SwitchAge"));
            warped = false;
            return;
        }

        static Vec3f playerPos;
        static int16_t playerYaw;
        static RoomContext* roomCtx;
        static s32 roomNum;

        if (CVarGetInteger(CVAR_GENERAL("SwitchAge"), 0) && !warped) {
            playerPos = GET_PLAYER(gPlayState)->actor.world.pos;
            playerYaw = GET_PLAYER(gPlayState)->actor.shape.rot.y;
            roomCtx = &gPlayState->roomCtx;
            roomNum = roomCtx->curRoom.num;
            ReloadSceneTogglingLinkAge();
            warped = true;
        }

        if (warped && gPlayState->transitionTrigger != TRANS_TRIGGER_START &&
            gSaveContext.nextTransitionType == TRANS_NEXT_TYPE_DEFAULT) {
            GET_PLAYER(gPlayState)->actor.shape.rot.y = playerYaw;
            GET_PLAYER(gPlayState)->actor.world.pos = playerPos;
            if (roomNum != roomCtx->curRoom.num) {
                func_8009728C(gPlayState, roomCtx, roomNum); //load original room
                //func_800973FC(gPlayState, &gPlayState->roomCtx); // commit to room load?
                func_80097534(gPlayState, roomCtx);  // load map for new room (unloading the previous room)
            }
            warped = false;
            CVarClear(CVAR_GENERAL("SwitchAge"));
        }
    });
}

/// Switches Link's age and respawns him at the last entrance he entered.
void RegisterOcarinaTimeTravel() {

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnOcarinaSongAction>([]() {
        if (!GameInteractor::IsSaveLoaded(true)) {
            CVarClear(CVAR_ENHANCEMENT("TimeTravel"));
            return;
        }

        Actor* player = &GET_PLAYER(gPlayState)->actor;
        Actor* nearbyTimeBlockEmpty = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_WARP2BLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyTimeBlock = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_TIMEBLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyOcarinaSpot = Actor_FindNearby(gPlayState, player, ACTOR_EN_OKARINA_TAG, ACTORCAT_PROP, 120.0f);
        Actor* nearbyDoorOfTime = Actor_FindNearby(gPlayState, player, ACTOR_DOOR_TOKI, ACTORCAT_BG, 500.0f);
        Actor* nearbyFrogs = Actor_FindNearby(gPlayState, player, ACTOR_EN_FR, ACTORCAT_NPC, 300.0f);
        uint8_t hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
        uint8_t hasOcarinaOfTime = (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME);
        // If TimeTravel + Player have the Ocarina of Time + Have Master Sword + is in proper range
        // TODO: Once Swordless Adult is fixed: Remove the Master Sword check
        if (((CVarGetInteger(CVAR_ENHANCEMENT("TimeTravel"), 0) == 1 && hasOcarinaOfTime) || CVarGetInteger(CVAR_ENHANCEMENT("TimeTravel"), 0) == 2) && hasMasterSword &&
            gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_TIME && !nearbyTimeBlockEmpty && !nearbyTimeBlock &&
            !nearbyOcarinaSpot && !nearbyFrogs) {

            if (IS_RANDO) {
                CVarSetInteger(CVAR_GENERAL("SwitchTimeline"), 1);
            } else if (!IS_RANDO && !nearbyDoorOfTime) {
                // This check is made for when Link is learning the Song Of Time in a vanilla save file that load a
                // Temple of Time scene where the only object present is the Door of Time
                CVarSetInteger(CVAR_GENERAL("SwitchTimeline"), 1);
            }
            ReloadSceneTogglingLinkAge();
        }
    });
}

void AutoSave(GetItemEntry itemEntry) {
    u8 item = itemEntry.itemId;
    bool performSave = false;
    // Don't autosave immediately after buying items from shops to prevent getting them for free!
    // Don't autosave in the Chamber of Sages since resuming from that map breaks the game
    // Don't autosave during the Ganon fight when picking up the Master Sword
    if ((CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) != AUTOSAVE_OFF) && (gPlayState != NULL) && (gSaveContext.pendingSale == ITEM_NONE) &&
        (gPlayState->gameplayFrames > 60 && gSaveContext.cutsceneIndex < 0xFFF0) && (gPlayState->sceneNum != SCENE_GANON_BOSS) && (gPlayState->sceneNum != SCENE_CHAMBER_OF_THE_SAGES)) {
        if (((CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_ALL_ITEMS) || (CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) == AUTOSAVE_ALL_ITEMS)) && (item != ITEM_NONE)) {
            // Autosave for all items
            performSave = true;

        } else if (((CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_MAJOR_ITEMS) || (CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) == AUTOSAVE_MAJOR_ITEMS)) && (item != ITEM_NONE)) {
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
                        if (!CVarGetInteger(CVAR_ENHANCEMENT("BombchuDrops"), 0)) {
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
        } else if (CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_MAJOR_ITEMS ||
                   CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) == AUTOSAVE_LOCATION_AND_ALL_ITEMS ||
                   CVarGetInteger(CVAR_ENHANCEMENT("Autosave"), AUTOSAVE_OFF) == AUTOSAVE_LOCATION) {
            performSave = true;
        }
        if (performSave) {
            Play_PerformSave(gPlayState);
            performSave = false;
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
        if (!CVarGetInteger(CVAR_ENHANCEMENT("RupeeDash"), 0)) {
            return;
        }
        
        // Initialize Timer
        static uint16_t rupeeDashTimer = 0;
        uint16_t rdmTime = CVarGetInteger(CVAR_ENHANCEMENT("RupeeDashInterval"), 5) * 20;
        
        // Did time change by DashInterval?
        if (rupeeDashTimer >= rdmTime) {
            rupeeDashTimer = 0;
            if (gSaveContext.rupees > 0) {
                uint16_t walletSize = (CUR_UPG_VALUE(UPG_WALLET) + 1) * -1;
                Rupees_ChangeBy(walletSize);
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
        if (!CVarGetInteger(CVAR_ENHANCEMENT("ShadowTag"), 0)) {
            return;
        }
        if (gPlayState->sceneNum == SCENE_FOREST_TEMPLE &&  // Forest Temple Scene
            gPlayState->roomCtx.curRoom.num == 16 ||        // Green Poe Room
            gPlayState->roomCtx.curRoom.num == 13 ||        // Blue Poe Room
            gPlayState->roomCtx.curRoom.num == 12) {        // Red Poe Room
            return;
        } else {
            if (shouldSpawn && (delayTimer <= 0)) {
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_WALLMAS, 0, 0, 0, 0, 0, 0, 3, false);
                shouldSpawn = false;
            } else {
                delayTimer--;
            }
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

static bool hasAffectedHealth = false;
void UpdatePermanentHeartLossState() {
    if (!GameInteractor::IsSaveLoaded()) return;

    if (!CVarGetInteger(CVAR_ENHANCEMENT("PermanentHeartLoss"), 0) && hasAffectedHealth) {
        uint8_t heartContainers = gSaveContext.sohStats.heartContainers; // each worth 16 health
        uint8_t heartPieces = gSaveContext.sohStats.heartPieces; // each worth 4 health, but only in groups of 4
        uint8_t startingHealth = 16 * 3;


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
        if (!CVarGetInteger(CVAR_ENHANCEMENT("PermanentHeartLoss"), 0) || !GameInteractor::IsSaveLoaded()) return;

        if (gSaveContext.healthCapacity > 16 && gSaveContext.healthCapacity - gSaveContext.health >= 16) {
            gSaveContext.healthCapacity -= 16;
            gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
            hasAffectedHealth = true;
        }
    });
};

void RegisterDeleteFileOnDeath() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!CVarGetInteger(CVAR_ENHANCEMENT("DeleteFileOnDeath"), 0) || !GameInteractor::IsSaveLoaded() || &gPlayState->gameOverCtx == NULL || &gPlayState->pauseCtx == NULL) return;

        if (gPlayState->gameOverCtx.state == GAMEOVER_DEATH_MENU && gPlayState->pauseCtx.state == 9) {
            SaveManager::Instance->DeleteZeldaFile(gSaveContext.fileNum);
            hasAffectedHealth = false;
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
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
        if (!CVarGetInteger(CVAR_ENHANCEMENT("NightGSAlwaysSpawn"), 0)) {
            return;
        }

        // Gold Skulltulas that are not part of the scene actor list during the day
        // Actor values copied from the night time scene actor list
        static const DayTimeGoldSkulltulasList dayTimeGoldSkulltulas = {
            // Graveyard
            { SCENE_GRAVEYARD, 1, true, { { ACTOR_EN_SW, { 156, 315, 795 }, { 16384, -32768, 0 }, -20096 } } },
            // ZF
            { SCENE_ZORAS_FOUNTAIN, 0, true, { { ACTOR_EN_SW, { -1891, 187, 1911 }, { 16384, 18022, 0 }, -19964 } } },
            // GF
            { SCENE_GERUDOS_FORTRESS, 0, false, { { ACTOR_EN_SW, { 1598, 999, -2008 }, { 16384, -16384, 0 }, -19198 } } },
            { SCENE_GERUDOS_FORTRESS, 1, false, { { ACTOR_EN_SW, { 3377, 1734, -4935 }, { 16384, 0, 0 }, -19199 } } },
            // Kak
            { SCENE_KAKARIKO_VILLAGE, 0, false, { { ACTOR_EN_SW, { -18, 540, 1800 }, { 0, -32768, 0 }, -20160 } } },
            { SCENE_KAKARIKO_VILLAGE,
              0,
              true,
              { { ACTOR_EN_SW, { -465, 377, -888 }, { 0, 28217, 0 }, -20222 },
                { ACTOR_EN_SW, { 5, 686, -171 }, { 0, -32768, 0 }, -20220 },
                { ACTOR_EN_SW, { 324, 270, 905 }, { 16384, 0, 0 }, -20216 },
                { ACTOR_EN_SW, { -602, 120, 1120 }, { 16384, 0, 0 }, -20208 } } },
            // LLR
            { SCENE_LON_LON_RANCH,
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

bool IsHyperBossesActive() {
    return CVarGetInteger(CVAR_ENHANCEMENT("HyperBosses"), 0) ||
           (IS_BOSS_RUSH && gSaveContext.bossRushOptions[BR_OPTIONS_HYPERBOSSES] == BR_CHOICE_HYPERBOSSES_YES);
}

void UpdateHyperBossesState() {
    static uint32_t actorUpdateHookId = 0;
    if (actorUpdateHookId != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(actorUpdateHookId);
        actorUpdateHookId = 0;
    }

    if (IsHyperBossesActive()) {
        actorUpdateHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* refActor) {
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
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int16_t fileNum) {
        UpdateHyperBossesState();
    });
}

void UpdateHyperEnemiesState() {
    static uint32_t actorUpdateHookId = 0;
    if (actorUpdateHookId != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(actorUpdateHookId);
        actorUpdateHookId = 0;
    }

    if (CVarGetInteger(CVAR_ENHANCEMENT("HyperEnemies"), 0)) {
        actorUpdateHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* refActor) {
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

void RegisterBonkDamage() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerBonk>([]() {
        uint8_t bonkOption = CVarGetInteger(CVAR_ENHANCEMENT("BonkDamageMult"), BONK_DAMAGE_NONE);
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
        case SCENE_HYRULE_FIELD:
        case SCENE_KOKIRI_FOREST:
        case SCENE_HYRULE_CASTLE:
            CVarSetInteger(CVAR_Z_FIGHTING_MODE, CVarGetInteger(CVAR_ENHANCEMENT("SceneSpecificDirtPathFix"), ZFIGHT_FIX_DISABLED));
            return;
        default:
            CVarClear(CVAR_Z_FIGHTING_MODE);
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

    int16_t mirroredMode = CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorldMode"), MIRRORED_WORLD_OFF);
    int16_t inDungeon = (sceneNum >= SCENE_DEKU_TREE && sceneNum <= SCENE_INSIDE_GANONS_CASTLE_COLLAPSE && sceneNum != SCENE_THIEVES_HIDEOUT) ||
                        (sceneNum >= SCENE_DEKU_TREE_BOSS && sceneNum <= SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                        (sceneNum == SCENE_GANON_BOSS);

    if (mirroredMode == MIRRORED_WORLD_RANDOM_SEEDED || mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED) {
        uint32_t seed = sceneNum + (IS_RANDO ? gSaveContext.finalSeed : gSaveContext.sohStats.fileCreatedAt);
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
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int32_t sceneNum) {
        UpdateMirrorModeState(sceneNum);
    });
}

void RegisterPatchNoMSHandler() {
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() { 
        if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MASTER_SWORD) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER &&
             gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS &&
             gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BROKEN)) {
            for (int i = 43; i <= 46; i++) {
                std::string patchName = "adultNoMS." + std::to_string(1) + "." + std::to_string(i);
                ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, patchName.c_str(), i, gsDPNoOp());
            }
            for (int i = 62; i <= 150; i++) {
                std::string patchName = "adultNoMS." + std::to_string(2) + "." + std::to_string(i);
                ResourceMgr_PatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathNearDL, patchName.c_str(), i, gsDPNoOp());
            }
            for (int i = 61; i <= 118; i++) {
                std::string patchName = "adultNoMS." + std::to_string(3) + "." + std::to_string(i);
                ResourceMgr_PatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathFarDL,  patchName.c_str(), i, gsDPNoOp());
            }
            ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "adultNoMSHylianShield1", 75, gsSPEndDisplayList());
            ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, "adultNoMSHylianShield2", 71, gsSPEndDisplayList());
            ResourceMgr_PatchGfxByName(gLinkAdultMasterSwordAndSheathNearDL, "adultNoMasterSword1", 2, gsSPEndDisplayList());
            ResourceMgr_PatchGfxByName(gLinkAdultMasterSwordAndSheathFarDL, "adultNoMasterSword2", 2, gsSPEndDisplayList());
        }
        else {
            for (int i = 43; i <= 46; i++) {
                std::string patchName = "adultNoMS." + std::to_string(1) + "." + std::to_string(i);
                ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, patchName.c_str());
            }
            for (int i = 62; i <= 150; i++) {
                std::string patchName = "adultNoMS." + std::to_string(2) + "." + std::to_string(i);
                ResourceMgr_UnpatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathNearDL, patchName.c_str());
            }
            for (int i = 61; i <= 118; i++) {
                std::string patchName = "adultNoMS." + std::to_string(3) + "." + std::to_string(i);
                ResourceMgr_UnpatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathFarDL,  patchName.c_str());
            }
            ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "adultNoMSHylianShield1");
            ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, "adultNoMSHylianShield2");
            ResourceMgr_UnpatchGfxByName(gLinkAdultMasterSwordAndSheathNearDL, "adultNoMasterSword1");
            ResourceMgr_UnpatchGfxByName(gLinkAdultMasterSwordAndSheathFarDL, "adultNoMasterSword2");
        }
    });
}

void UpdatePatchHand() {
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquimentAlwaysVisible"), 0)) && LINK_IS_CHILD) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer1", 92, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer2", 93, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot1", 84, gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot2", 85, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow1", 51, gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow2", 52, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword1", 104, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword2", 105, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword1", 79, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword2", 80, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife1", 76, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife2", 77, gsSPEndDisplayList());

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
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquimentAlwaysVisible"), 0)) && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword", 13, gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot", 13, gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang", 50, gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield", 49, gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot");
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield");
    }
}

void RegisterPatchHandHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int32_t sceneNum) { 
        UpdatePatchHand(); 
    });
}

void RegisterResetNaviTimer() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int32_t sceneNum) {
		if (CVarGetInteger(CVAR_ENHANCEMENT("ResetNaviTimer"), 0)) {
			gSaveContext.naviTimer = 0;
		}
	});
}

f32 triforcePieceScale;

void RegisterTriforceHunt() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!GameInteractor::IsGameplayPaused() &&
            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT)) {

            // Warp to credits
            if (GameInteractor::State::TriforceHuntCreditsWarpActive) {
                gPlayState->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                gSaveContext.nextCutsceneIndex = 0xFFF2;
                gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
                GameInteractor::State::TriforceHuntCreditsWarpActive = 0;
            }

            // Reset Triforce Piece scale for GI animation. Triforce Hunt allows for multiple triforce models,
            // and cycles through them based on the amount of triforce pieces collected. It takes a little while
            // for the count to increase during the GI animation, so the model is entirely hidden until that piece
            // has been added. That scale has to be reset after the textbox is closed, and this is the best way
            // to ensure it's done at that point in time specifically.
            if (GameInteractor::State::TriforceHuntPieceGiven) {
                triforcePieceScale = 0.0f;
                GameInteractor::State::TriforceHuntPieceGiven = 0;
            }
        }
    });
}

void RegisterGrantGanonsBossKey() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        // Triforce Hunt needs the check if the player isn't being teleported to the credits scene.
        if (!GameInteractor::IsGameplayPaused() && IS_RANDO &&
            Flags_GetRandomizerInf(RAND_INF_GRANT_GANONS_BOSSKEY) && gPlayState->transitionTrigger != TRANS_TRIGGER_START &&
            (1 << 0 & gSaveContext.inventory.dungeonItems[SCENE_GANONS_TOWER]) == 0) {
                GetItemEntry getItemEntry =
                    ItemTableManager::Instance->RetrieveItemEntry(MOD_RANDOMIZER, RG_GANONS_CASTLE_BOSS_KEY);
                GiveItemEntryWithoutActor(gPlayState, getItemEntry);
        }
    });
}

//this map is used for enemies that can be uniquely identified by their id
//and that are always counted
//enemies that can't be uniquely identified by their id
//or only sometimes count (like ACTOR_EN_TP)
//have to be manually handled in RegisterEnemyDefeatCounts
static std::unordered_map<u16, u16> uniqueEnemyIdToStatCount = {
    { ACTOR_EN_ANUBICE,    COUNT_ENEMIES_DEFEATED_ANUBIS },
    { ACTOR_EN_AM,         COUNT_ENEMIES_DEFEATED_ARMOS },
    { ACTOR_EN_CLEAR_TAG,  COUNT_ENEMIES_DEFEATED_ARWING },
    { ACTOR_EN_VALI,       COUNT_ENEMIES_DEFEATED_BARI },
    { ACTOR_EN_VM,         COUNT_ENEMIES_DEFEATED_BEAMOS },
    { ACTOR_EN_BIGOKUTA,   COUNT_ENEMIES_DEFEATED_BIG_OCTO },
    { ACTOR_EN_BILI,       COUNT_ENEMIES_DEFEATED_BIRI },
    { ACTOR_EN_DNS,        COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB },
    { ACTOR_EN_TORCH,      COUNT_ENEMIES_DEFEATED_DARK_LINK },
    { ACTOR_EN_DH,         COUNT_ENEMIES_DEFEATED_DEAD_HAND },
    { ACTOR_EN_HINTNUTS,   COUNT_ENEMIES_DEFEATED_DEKU_SCRUB },
    { ACTOR_EN_DODONGO,    COUNT_ENEMIES_DEFEATED_DODONGO },
    { ACTOR_EN_DODOJR,     COUNT_ENEMIES_DEFEATED_DODONGO_BABY },
    { ACTOR_DOOR_KILLER,   COUNT_ENEMIES_DEFEATED_DOOR_TRAP },
    { ACTOR_EN_FD,         COUNT_ENEMIES_DEFEATED_FLARE_DANCER },
    { ACTOR_EN_FLOORMAS,   COUNT_ENEMIES_DEFEATED_FLOORMASTER },
    { ACTOR_EN_TUBO_TRAP,  COUNT_ENEMIES_DEFEATED_FLYING_POT },
    { ACTOR_EN_YUKABYUN,   COUNT_ENEMIES_DEFEATED_FLOOR_TILE },
    { ACTOR_EN_FZ,         COUNT_ENEMIES_DEFEATED_FREEZARD },
    { ACTOR_EN_GELDB,      COUNT_ENEMIES_DEFEATED_GERUDO_THIEF },
    { ACTOR_EN_GOMA,       COUNT_ENEMIES_DEFEATED_GOHMA_LARVA },
    { ACTOR_EN_CROW,       COUNT_ENEMIES_DEFEATED_GUAY },
    { ACTOR_EN_RR,         COUNT_ENEMIES_DEFEATED_LIKE_LIKE },
    { ACTOR_EN_DEKUNUTS,   COUNT_ENEMIES_DEFEATED_MAD_SCRUB },
    { ACTOR_EN_OKUTA,      COUNT_ENEMIES_DEFEATED_OCTOROK },
    { ACTOR_EN_BA,         COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE },
    { ACTOR_EN_PO_SISTERS, COUNT_ENEMIES_DEFEATED_POE_SISTERS },
    { ACTOR_EN_BUBBLE,     COUNT_ENEMIES_DEFEATED_SHABOM },
    { ACTOR_EN_SB,         COUNT_ENEMIES_DEFEATED_SHELLBLADE },
    { ACTOR_EN_SKJ,        COUNT_ENEMIES_DEFEATED_SKULL_KID },
    { ACTOR_EN_NY,         COUNT_ENEMIES_DEFEATED_SPIKE },
    { ACTOR_EN_SKB,        COUNT_ENEMIES_DEFEATED_STALCHILD },
    { ACTOR_EN_TEST,       COUNT_ENEMIES_DEFEATED_STALFOS },
    { ACTOR_EN_WEIYER,     COUNT_ENEMIES_DEFEATED_STINGER },
    { ACTOR_EN_BW,         COUNT_ENEMIES_DEFEATED_TORCH_SLUG },
    { ACTOR_EN_WALLMAS,    COUNT_ENEMIES_DEFEATED_WALLMASTER },
    { ACTOR_EN_KAREBABA,   COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA },
};

void RegisterEnemyDefeatCounts() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnEnemyDefeat>([](void* refActor) {
        Actor* actor = (Actor*)refActor;
        if (uniqueEnemyIdToStatCount.contains(actor->id)) {
            gSaveContext.sohStats.count[uniqueEnemyIdToStatCount[actor->id]]++;
        } else {
            switch (actor->id) {
                case ACTOR_EN_BB:
                    if (actor->params == ENBB_GREEN || actor->params == ENBB_GREEN_BIG) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN]++;
                    } else if (actor->params == ENBB_BLUE) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE]++;
                    } else if (actor->params == ENBB_WHITE) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE]++;
                    } else if (actor->params == ENBB_RED) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_RED]++;
                    }
                    break;

                case ACTOR_EN_DEKUBABA:
                    if (actor->params == DEKUBABA_BIG) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA]++;
                    }
                    break;

                case ACTOR_EN_ZF:
                    if (actor->params == ENZF_TYPE_DINOLFOS) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DINOLFOS]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LIZALFOS]++;
                    }
                    break;

                case ACTOR_EN_RD:
                    if (actor->params >= -1) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_REDEAD]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_GIBDO]++;
                    }
                    break;

                case ACTOR_EN_IK:
                    if (actor->params == 0) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE]++;
                    }
                    break;

                case ACTOR_EN_FIREFLY:
                    if (actor->params == KEESE_NORMAL_FLY || actor->params == KEESE_NORMAL_PERCH) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_KEESE]++;
                    } else if (actor->params == KEESE_FIRE_FLY || actor->params == KEESE_FIRE_PERCH) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_KEESE_FIRE]++;
                    } else if (actor->params == KEESE_ICE_FLY) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_KEESE_ICE]++;
                    }
                    break;

                case ACTOR_EN_REEBA:
                    {
                        EnReeba* reeba = (EnReeba*)actor;
                        if (reeba->isBig) {
                            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LEEVER_BIG]++;
                        } else {
                            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LEEVER]++;
                        }
                    }
                    break;

                case ACTOR_EN_MB:
                    if (actor->params == 0) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN]++;
                    }
                    break;

                case ACTOR_EN_PEEHAT:
                    if (actor->params == PEAHAT_TYPE_LARVA) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT]++;
                    }
                    break;
                
                case ACTOR_EN_POH:
                    if (actor->params == EN_POH_FLAT || actor->params == EN_POH_SHARP) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE_COMPOSER]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE]++;
                    }
                    break;

                case ACTOR_EN_PO_FIELD:
                    if (actor->params == EN_PO_FIELD_BIG) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE_BIG]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE]++;
                    }
                    break;

                case ACTOR_EN_ST:
                    if (actor->params == 1) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA]++;
                    }
                    break;

                case ACTOR_EN_SW:
                    if (((actor->params & 0xE000) >> 0xD) != 0) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLWALLTULA]++;
                    }
                    break;

                case ACTOR_EN_TP:
                    if (actor->params == TAILPASARAN_HEAD) {  // Only count the head, otherwise each body segment will increment
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TAILPASARAN]++;
                    }
                    break;

                case ACTOR_EN_TITE:
                    if (actor->params == TEKTITE_BLUE) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_RED]++;
                    }
                    break;

                case ACTOR_EN_WF:
                    if (actor->params == WOLFOS_WHITE) {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE]++;
                    } else {
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_WOLFOS]++;
                    }
                    break;
            }
        }
    });
}

typedef enum {
    ADD_ICE_TRAP,
    ADD_BURN_TRAP,
    ADD_SHOCK_TRAP,
    ADD_KNOCK_TRAP,
    ADD_SPEED_TRAP,
    ADD_BOMB_TRAP,
    ADD_VOID_TRAP,
    ADD_AMMO_TRAP,
    ADD_KILL_TRAP,
    ADD_TELEPORT_TRAP,
    ADD_TRAP_MAX
} AltTrapType;

const char* altTrapTypeCvars[] = {
    CVAR_ENHANCEMENT("ExtraTraps.Ice"),
    CVAR_ENHANCEMENT("ExtraTraps.Burn"),
    CVAR_ENHANCEMENT("ExtraTraps.Shock"),
    CVAR_ENHANCEMENT("ExtraTraps.Knockback"),
    CVAR_ENHANCEMENT("ExtraTraps.Speed"),
    CVAR_ENHANCEMENT("ExtraTraps.Bomb"),
    CVAR_ENHANCEMENT("ExtraTraps.Void"),
    CVAR_ENHANCEMENT("ExtraTraps.Ammo"),
    CVAR_ENHANCEMENT("ExtraTraps.Kill"),
    CVAR_ENHANCEMENT("ExtraTraps.Teleport")
};

std::vector<AltTrapType> getEnabledAddTraps () {
    std::vector<AltTrapType> enabledAddTraps;
    for (int i = 0; i < ADD_TRAP_MAX; i++) {
        if (CVarGetInteger(altTrapTypeCvars[i], 0)) {
            enabledAddTraps.push_back(static_cast<AltTrapType>(i));
        }
    }
    if (enabledAddTraps.size() == 0) {
        enabledAddTraps.push_back(ADD_ICE_TRAP);
    }
    return enabledAddTraps;
};

void RegisterAltTrapTypes() {
    static AltTrapType roll = ADD_TRAP_MAX;
    static int statusTimer = -1;
    static int eventTimer = -1;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        if (!CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) || itemEntry.modIndex != MOD_RANDOMIZER || itemEntry.getItemId != RG_ICE_TRAP) {
            return;
        }
        roll = RandomElement(getEnabledAddTraps());
        switch (roll) {
            case ADD_ICE_TRAP:
                GameInteractor::RawAction::FreezePlayer();
                break;
            case ADD_BURN_TRAP:
                GameInteractor::RawAction::BurnPlayer();
                break;
            case ADD_SHOCK_TRAP:
                GameInteractor::RawAction::ElectrocutePlayer();
                break;
            case ADD_KNOCK_TRAP:
                eventTimer = 3;
                break;
            case ADD_SPEED_TRAP:
                Audio_PlaySoundGeneral(NA_SE_VO_KZ_MOVE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                GameInteractor::State::RunSpeedModifier = -2;
                statusTimer = 200;
                Overlay_DisplayText(10, "Speed Decreased!");
                break;
            case ADD_BOMB_TRAP:
                eventTimer = 3;
                break;
            case ADD_VOID_TRAP:
                Audio_PlaySoundGeneral(NA_SE_EN_GANON_LAUGH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                eventTimer = 3;                    
                break;
            case ADD_AMMO_TRAP:
                eventTimer = 3;
                Overlay_DisplayText(5, "Ammo Halved!");
                break;
            case ADD_KILL_TRAP:
                GameInteractor::RawAction::SetPlayerHealth(0);
                break;
            case ADD_TELEPORT_TRAP:
                eventTimer = 3;
                break;
        }
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        Player* player = GET_PLAYER(gPlayState);
        if (statusTimer == 0) {
            GameInteractor::State::RunSpeedModifier = 0;
        }
        if (eventTimer == 0) {
            switch (roll) {
                case ADD_KNOCK_TRAP:
                    GameInteractor::RawAction::KnockbackPlayer(1);
                    break;
                case ADD_BOMB_TRAP:
                    GameInteractor::RawAction::SpawnActor(ACTOR_EN_BOM, 1);
                    break;
                case ADD_VOID_TRAP:
                    Play_TriggerRespawn(gPlayState);
                    break;
                case ADD_AMMO_TRAP:
                    AMMO(ITEM_STICK) = AMMO(ITEM_STICK) * 0.5;
                    AMMO(ITEM_NUT) = AMMO(ITEM_NUT) * 0.5;
                    AMMO(ITEM_SLINGSHOT) = AMMO(ITEM_SLINGSHOT) * 0.5;
                    AMMO(ITEM_BOW) = AMMO(ITEM_BOW) * 0.5;
                    AMMO(ITEM_BOMB) = AMMO(ITEM_BOMB) * 0.5;
                    AMMO(ITEM_BOMBCHU) = AMMO(ITEM_BOMBCHU) * 0.5;
                    Audio_PlaySoundGeneral(NA_SE_VO_FR_SMILE_0, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    break;
                case ADD_TELEPORT_TRAP:
                    int entrance;
                    int index = 1 + rand() % 10;
                    switch (index) {
                        case 1:
                            entrance = GI_TP_DEST_SERENADE;
                            break;
                        case 2:
                            entrance = GI_TP_DEST_REQUIEM;
                            break;
                        case 3:
                            entrance = GI_TP_DEST_BOLERO;
                            break;
                        case 4:
                            entrance = GI_TP_DEST_MINUET;
                            break;
                        case 5:
                            entrance = GI_TP_DEST_NOCTURNE;
                            break;
                        case 6:
                            entrance = GI_TP_DEST_PRELUDE;
                            break;
                        default:
                            entrance = GI_TP_DEST_LINKSHOUSE;
                            break;
                    }
                    GameInteractor::RawAction::TeleportPlayer(entrance);
                    break;
            }
        }
        statusTimer--;
        eventTimer--;
    });
}

void RegisterRandomizerSheikSpawn() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() {
        if (!gPlayState) return;
        bool canSheik = (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIAL_COUNT) != RO_GANONS_TRIALS_SKIP && 
          OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LIGHT_ARROWS_HINT));
        if (!IS_RANDO || !LINK_IS_ADULT || !canSheik) return;
        switch (gPlayState->sceneNum) {
            case SCENE_TEMPLE_OF_TIME:
                if (gPlayState->roomCtx.curRoom.num == 1) {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_XC, -104, -40, 2382, 0, 0x8000, 0, SHEIK_TYPE_RANDO, false);
                }
                break;
            case SCENE_INSIDE_GANONS_CASTLE:
                if (gPlayState->roomCtx.curRoom.num == 1){
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_XC, 101, 150, 137, 0, 0, 0, SHEIK_TYPE_RANDO, false);
                    }
                break;
            default: break;
        }
    });
}

void UpdateHurtContainerModeState(bool newState) {
        static bool hurtEnabled = false;
        if (hurtEnabled == newState) {
            return;
        }

        hurtEnabled = newState;
        uint16_t getHeartPieces = gSaveContext.sohStats.heartPieces / 4;
        uint16_t getHeartContainers = gSaveContext.sohStats.heartContainers;
        
        if (hurtEnabled) {        
            gSaveContext.healthCapacity = 320 - ((getHeartPieces + getHeartContainers) * 16);
        } else {
            gSaveContext.healthCapacity = 48 + ((getHeartPieces + getHeartContainers) * 16);
        }
}

void RegisterHurtContainerModeHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        UpdateHurtContainerModeState(CVarGetInteger(CVAR_ENHANCEMENT("HurtContainer"), 0));
    });
}

void RegisterRandomizedEnemySizes() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* refActor) {
        // Randomized Enemy Sizes
        Player* player = GET_PLAYER(gPlayState);
        Actor* actor = static_cast<Actor*>(refActor);

        // Exclude wobbly platforms in Jabu because they need to act like platforms.
        // Exclude Dead Hand hands and Bongo Bongo main body because they make the fights (near) impossible.
        uint8_t excludedEnemy = actor->id == ACTOR_EN_BROB || actor->id == ACTOR_EN_DHA || (actor->id == ACTOR_BOSS_SST && actor->params == -1);

        // Dodongo, Volvagia and Dead Hand are always smaller because they're impossible when bigger.
        uint8_t smallOnlyEnemy = actor->id == ACTOR_BOSS_DODONGO || actor->id == ACTOR_BOSS_FD ||
                                 actor->id == ACTOR_BOSS_FD2 || actor->id == ACTOR_EN_DH;

        // Only apply to enemies and bosses.
        if (!CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemySizes"), 0) || (actor->category != ACTORCAT_ENEMY && actor->category != ACTORCAT_BOSS) || excludedEnemy) {
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
        // Small actor
        } else {
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

void RegisterOpenAllHours() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);

        if (CVarGetInteger(CVAR_ENHANCEMENT("OpenAllHours"), 0) && (actor->id == ACTOR_EN_DOOR)) {
            switch (actor->params) {
                case 4753: // Night Market Bazaar
                case 1678: // Night Potion Shop
                case 2689: // Day Bombchu Shop
                case 2703: // Night Slingshot Game
                case 653:  // Day Chest Game
                case 6801: // Night Kak Bazaar
                case 7822: // Night Kak Potion Shop
                case 4751: // Night Kak Archery Game
                case 3728: // Night Mask Shop
                {
                    actor->params = (actor->params & 0xFC00) | (DOOR_SCENEEXIT << 7) | 0x3F;
                    EnDoor* enDoor = static_cast<EnDoor*>(refActor);
                    EnDoor_SetupType(enDoor, gPlayState);
                    break;
                }
                default:
                    break;
            }
        }
    });
}

void PatchToTMedallions() {
    // TODO: Refactor the DemoEffect_UpdateJewelAdult and DemoEffect_UpdateJewelChild from z_demo_effect
    // effects to take effect in there
    if (CVarGetInteger(CVAR_ENHANCEMENT("ToTMedallionsColors"), 0)) {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_StartGrayscale", 7, gsSPGrayscale(true));
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_StartGrayscale", 7, gsSPGrayscale(true));

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER)) {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeBlue", 16, gsDPSetGrayscaleColor(0, 161, 255, 255));
        } else {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeBlue", 16, gsDPSetGrayscaleColor(255, 255, 255, 255));
        }

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeOrange", 45, gsDPSetGrayscaleColor(255, 135, 0, 255));
        } else {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeOrange", 45, gsDPSetGrayscaleColor(255, 255, 255, 255));
        }

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_LIGHT)) {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeYellow", 69, gsDPSetGrayscaleColor(255, 255, 0, 255));
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakeYellow", 16, gsDPSetGrayscaleColor(255, 255, 0, 255));
        } else {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeYellow", 69, gsDPSetGrayscaleColor(255, 255, 255, 255));
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakeYellow", 16, gsDPSetGrayscaleColor(255, 255, 255, 255));
        }

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeGreen", 94, gsDPSetGrayscaleColor(0, 255, 0, 255));
        } else {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeGreen", 94, gsDPSetGrayscaleColor(255, 255, 255, 255));
        }

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeRed", 118, gsDPSetGrayscaleColor(255, 0, 0, 255));
        } else {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeRed", 118, gsDPSetGrayscaleColor(255, 255, 255, 255));
        }

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakePurple", 142, gsDPSetGrayscaleColor(212, 0, 255, 255));
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakePurple", 27, gsDPSetGrayscaleColor(212, 0, 255, 255));
        } else {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakePurple", 142, gsDPSetGrayscaleColor(255, 255, 255, 255));
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakePurple", 27, gsDPSetGrayscaleColor(255, 255, 255, 255));
        }

        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_EndGrayscaleAndEndDlist", 160, gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL));
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_EndGrayscaleAndEndDlist", 51, gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL));
    } else {
        // Unpatch everything
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_StartGrayscale");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_StartGrayscale");

        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeBlue");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeOrange");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeYellow");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakeYellow");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeRed");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakePurple");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakePurple");

        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_EndGrayscaleAndEndDlist");
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_EndGrayscaleAndEndDlist");
    }
}

void RegisterToTMedallions() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry _unused) {
        if (!CVarGetInteger(CVAR_ENHANCEMENT("ToTMedallionsColors"), 0) || !gPlayState || gPlayState->sceneNum != SCENE_TEMPLE_OF_TIME) {
            return;
        }
        PatchToTMedallions();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
        if (!CVarGetInteger(CVAR_ENHANCEMENT("ToTMedallionsColors"), 0) || sceneNum != SCENE_TEMPLE_OF_TIME) {
            return;
        }
        PatchToTMedallions();
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

void RegisterPauseMenuHooks() {
    static bool pauseWarpHooksRegistered = false;
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([&]() {
        if (!GameInteractor::IsSaveLoaded() || !CVarGetInteger(CVAR_ENHANCEMENT("PauseWarp"), 0)) {
            pauseWarpHooksRegistered = false;
            return;
        }
        if (!pauseWarpHooksRegistered) {
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnKaleidoUpdate>([]() {PauseWarp_HandleSelection();});
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
                    PauseWarp_Execute();
            });
            pauseWarpHooksRegistered = true;
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
    RegisterEzQPA();
    RegisterUnrestrictedItems();
    RegisterFreezeTime();
    RegisterSwitchAge();
    RegisterOcarinaTimeTravel();
    RegisterAutoSave();
    RegisterDaytimeGoldSkultullas();
    RegisterRupeeDash();
    RegisterShadowTag();
    RegisterPermanentHeartLoss();
    RegisterDeleteFileOnDeath();
    RegisterHyperBosses();
    UpdateHyperEnemiesState();
    RegisterBonkDamage();
    RegisterMenuPathFix();
    RegisterMirrorModeHandler();
    RegisterResetNaviTimer();
    RegisterTriforceHunt();
    RegisterGrantGanonsBossKey();
    RegisterEnemyDefeatCounts();
    RegisterAltTrapTypes();
    RegisterRandomizerSheikSpawn();
    RegisterRandomizedEnemySizes();
    RegisterOpenAllHours();
    RegisterToTMedallions();
    NameTag_RegisterHooks();
    RegisterPatchNoMSHandler();
    RegisterFloorSwitchesHook();
    RegisterPatchHandHandler();
    RegisterHurtContainerModeHandler();
    RegisterPauseMenuHooks();
}

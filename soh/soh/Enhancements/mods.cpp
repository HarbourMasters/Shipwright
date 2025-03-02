#include "mods.h"

#include <libultraship/bridge.h>
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/SaveManager.h"
#include "soh/OTRGlobals.h"

#include "soh_assets.h"

extern "C" {
#include "z64.h"
#include "align_asset_macro.h"
#include "macros.h"
#include "soh/cvar_prefixes.h"
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_Door_Gerudo/z_door_gerudo.h"
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "src/overlays/actors/ovl_En_Bb/z_en_bb.h"
#include "src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.h"
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "src/overlays/actors/ovl_En_Firefly/z_en_firefly.h"
#include "src/overlays/actors/ovl_En_Mb/z_en_mb.h"
#include "src/overlays/actors/ovl_En_Peehat/z_en_peehat.h"
#include "src/overlays/actors/ovl_En_Po_Field/z_en_po_field.h"
#include "src/overlays/actors/ovl_En_Poh/z_en_poh.h"
#include "src/overlays/actors/ovl_En_Reeba/z_en_reeba.h"
#include "src/overlays/actors/ovl_En_Tite/z_en_tite.h"
#include "src/overlays/actors/ovl_En_Tp/z_en_tp.h"
#include "src/overlays/actors/ovl_En_Wf/z_en_wf.h"
#include "src/overlays/actors/ovl_En_Zf/z_en_zf.h"
#include "src/overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
#include "objects/object_gi_compass/object_gi_compass.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
extern PlayState* gPlayState;
}

// GreyScaleEndDlist
#define dgEndGrayscaleAndEndDlistDL "__OTR__helpers/cosmetics/gEndGrayscaleAndEndDlistDL"
static const ALIGN_ASSET(2) char gEndGrayscaleAndEndDlistDL[] = dgEndGrayscaleAndEndDlistDL;

// This is used for the Temple of Time Medalions' color
#define dtokinoma_room_0DL_007A70 "__OTR__scenes/shared/tokinoma_scene/tokinoma_room_0DL_007A70"
static const ALIGN_ASSET(2) char tokinoma_room_0DL_007A70[] = dtokinoma_room_0DL_007A70;
#define dtokinoma_room_0DL_007FD0 "__OTR__scenes/shared/tokinoma_scene/tokinoma_room_0DL_007FD0"
static const ALIGN_ASSET(2) char tokinoma_room_0DL_007FD0[] = dtokinoma_room_0DL_007FD0;

/// Switches Link's age and respawns him at the last entrance he entered.
void SwitchAge() {
    if (gPlayState == NULL) return;

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
        Actor* nearbyTimeBlockEmpty = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_WARP2BLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyTimeBlock = Actor_FindNearby(gPlayState, player, ACTOR_OBJ_TIMEBLOCK, ACTORCAT_ITEMACTION, 300.0f);
        Actor* nearbyOcarinaSpot = Actor_FindNearby(gPlayState, player, ACTOR_EN_OKARINA_TAG, ACTORCAT_PROP, 120.0f);
        Actor* nearbyDoorOfTime = Actor_FindNearby(gPlayState, player, ACTOR_DOOR_TOKI, ACTORCAT_BG, 500.0f);
        Actor* nearbyFrogs = Actor_FindNearby(gPlayState, player, ACTOR_EN_FR, ACTORCAT_NPC, 300.0f);
        Actor* nearbyGossipStone = Actor_FindNearby(gPlayState, player, ACTOR_EN_GS, ACTORCAT_NPC, 300.0f);
        bool justPlayedSoT = gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_TIME;
        bool notNearAnySource = !nearbyTimeBlockEmpty && !nearbyTimeBlock && !nearbyOcarinaSpot && !nearbyDoorOfTime && !nearbyFrogs && !nearbyGossipStone;
        bool hasOcarinaOfTime = (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME);
        bool doesntNeedOcarinaOfTime = CVarGetInteger(CVAR_ENHANCEMENT("TimeTravel"), 0) == 2;
        bool hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
        // TODO: Once Swordless Adult is fixed: Remove the Master Sword check
        if (justPlayedSoT && notNearAnySource && (hasOcarinaOfTime || doesntNeedOcarinaOfTime) && hasMasterSword) {
            SwitchAge();
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

void UpdatePatchHand() {
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) && LINK_IS_CHILD) {
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
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) && LINK_IS_ADULT) {
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
    if (CVarGetInteger("gEnhancements.FixHammerHand", 0) && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1", 92, gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2", 93, gsSPEndDisplayList());
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2");
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
        Actor* actor = static_cast<Actor*>(refActor);
        if (uniqueEnemyIdToStatCount.contains(actor->id)) {
            gSaveContext.ship.stats.count[uniqueEnemyIdToStatCount[actor->id]]++;
        } else {
            switch (actor->id) {
                case ACTOR_EN_BB:
                    if (actor->params == ENBB_GREEN || actor->params == ENBB_GREEN_BIG) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN]++;
                    } else if (actor->params == ENBB_BLUE) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE]++;
                    } else if (actor->params == ENBB_WHITE) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE]++;
                    } else if (actor->params == ENBB_RED) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_BUBBLE_RED]++;
                    }
                    break;

                case ACTOR_EN_DEKUBABA:
                    if (actor->params == DEKUBABA_BIG) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA]++;
                    }
                    break;

                case ACTOR_EN_ZF:
                    if (actor->params == ENZF_TYPE_DINOLFOS) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_DINOLFOS]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_LIZALFOS]++;
                    }
                    break;

                case ACTOR_EN_RD:
                    if (actor->params >= -1) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_REDEAD]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_GIBDO]++;
                    }
                    break;

                case ACTOR_EN_IK:
                    if (actor->params == 0) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE]++;
                    }
                    break;

                case ACTOR_EN_FIREFLY:
                    if (actor->params == KEESE_NORMAL_FLY || actor->params == KEESE_NORMAL_PERCH) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_KEESE]++;
                    } else if (actor->params == KEESE_FIRE_FLY || actor->params == KEESE_FIRE_PERCH) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_KEESE_FIRE]++;
                    } else if (actor->params == KEESE_ICE_FLY) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_KEESE_ICE]++;
                    }
                    break;

                case ACTOR_EN_REEBA:
                    {
                        EnReeba* reeba = (EnReeba*)actor;
                        if (reeba->isBig) {
                            gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_LEEVER_BIG]++;
                        } else {
                            gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_LEEVER]++;
                        }
                    }
                    break;

                case ACTOR_EN_MB:
                    if (actor->params == 0) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_MOBLIN]++;
                    }
                    break;

                case ACTOR_EN_PEEHAT:
                    if (actor->params == PEAHAT_TYPE_LARVA) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_PEAHAT]++;
                    }
                    break;

                case ACTOR_EN_POH:
                    if (actor->params == EN_POH_FLAT || actor->params == EN_POH_SHARP) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_POE_COMPOSER]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_POE]++;
                    }
                    break;

                case ACTOR_EN_PO_FIELD:
                    if (actor->params == EN_PO_FIELD_BIG) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_POE_BIG]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_POE]++;
                    }
                    break;

                case ACTOR_EN_ST:
                    if (actor->params == 1) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA]++;
                    }
                    break;

                case ACTOR_EN_SW:
                    if (((actor->params & 0xE000) >> 0xD) != 0) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_SKULLWALLTULA]++;
                    }
                    break;

                case ACTOR_EN_TP:
                    if (actor->params == TAILPASARAN_HEAD) {  // Only count the head, otherwise each body segment will increment
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_TAILPASARAN]++;
                    }
                    break;

                case ACTOR_EN_TITE:
                    if (actor->params == TEKTITE_BLUE) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_TEKTITE_RED]++;
                    }
                    break;

                case ACTOR_EN_WF:
                    if (actor->params == WOLFOS_WHITE) {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE]++;
                    } else {
                        gSaveContext.ship.stats.count[COUNT_ENEMIES_DEFEATED_WOLFOS]++;
                    }
                    break;
            }
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

        if (CVarGetInteger(CVAR_ENHANCEMENT("OpenAllHours"), 0) && (actor->id == ACTOR_EN_DOOR) && (!IS_RANDO || !OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LOCK_OVERWORLD_DOORS))) {
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

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);

void PatchCompasses() {
    s8 compassesCanBeOutsideDungeon = IS_RANDO && DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS);
    s8 isColoredCompassesEnabled = compassesCanBeOutsideDungeon && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MatchCompassColors"), 1);
    if (isColoredCompassesEnabled) {
        ResourceMgr_PatchGfxByName(gGiCompassDL, "Compass_PrimColor", 5, gsDPNoOp());
        ResourceMgr_PatchGfxByName(gGiCompassDL, "Compass_EnvColor", 6, gsDPNoOp());
    } else {
        ResourceMgr_UnpatchGfxByName(gGiCompassDL, "Compass_PrimColor");
        ResourceMgr_UnpatchGfxByName(gGiCompassDL, "Compass_EnvColor");
    }
}

void RegisterRandomizerCompasses() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadFile>([](int32_t _unused) {
        PatchCompasses();
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
    RegisterOcarinaTimeTravel();
    RegisterDaytimeGoldSkultullas();
    RegisterResetNaviTimer();
    RegisterEnemyDefeatCounts();
    RegisterBossDefeatTimestamps();
    RegisterRandomizedEnemySizes();
    RegisterOpenAllHours();
    RegisterToTMedallions();
    RegisterRandomizerCompasses();
    RegisterFloorSwitchesHook();
    RegisterPatchHandHandler();
    RegisterPauseMenuHooks();
    RegisterCustomSkeletons();
}

static RegisterShipInitFunc initFunc(InitMods);

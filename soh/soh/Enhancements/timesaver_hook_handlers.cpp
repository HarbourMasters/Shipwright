#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "src/overlays/actors/ovl_En_Wonder_Talk2/z_en_wonder_talk2.h"
#include "src/overlays/actors/ovl_Elf_Msg/z_elf_msg.h"
#include "src/overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
#include "src/overlays/actors/ovl_Bg_Bdan_Switch/z_bg_bdan_switch.h"
#include "src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth.h"
#include "src/overlays/actors/ovl_En_Owl/z_en_owl.h"
#include "src/overlays/actors/ovl_En_Ko/z_en_ko.h"
#include "src/overlays/actors/ovl_En_Ma1/z_en_ma1.h"
#include "src/overlays/actors/ovl_En_Zl4/z_en_zl4.h"
#include "src/overlays/actors/ovl_Demo_Im/z_demo_im.h"
#include "src/overlays/actors/ovl_En_Sa/z_en_sa.h"
#include "src/overlays/actors/ovl_Bg_Ddan_Kd/z_bg_ddan_kd.h"
#include "src/overlays/actors/ovl_En_Tk/z_en_tk.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).GetSelectedOptionIndex()

void EnKo_MoveWhenReady(EnKo* enKo, PlayState* play) {
    func_80A995CC(enKo, play);

    if ((enKo->actor.params & 0xFF) == ENKO_TYPE_CHILD_3) {
        // Typically this doesn't get get live updated in vanilla, but we need to
        // live update it if we're skipping a certain cutscene or in randomizer
        if (GameInteractor_Should(GI_VB_OPEN_KOKIRI_FOREST, CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD), NULL)) {
            enKo->collider.dim.height -= 200;
            Path_CopyLastPoint(enKo->path, &enKo->actor.world.pos);
            enKo->actionFunc = func_80A99384;
        }
    }
}

void EnMa1_EndTeachSong(EnMa1* enMa1, PlayState* play) {
    if (Message_GetState(&gPlayState->msgCtx) == TEXT_STATE_CLOSING) {
        Flags_SetRandomizerInf(RAND_INF_LEARNED_EPONA_SONG);
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        enMa1->actor.flags &= ~ACTOR_FLAG_WILL_TALK;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        enMa1->actionFunc = func_80AA0D88;
        enMa1->unk_1E0 = 1;
        enMa1->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        return;
    }
}

u16 EnZl4_GiveItemTextId(PlayState* play, Actor* actor) {
    return 0x207D;
}

void EnZl4_SkipToGivingZeldasLetter(EnZl4* enZl4, PlayState* play) {
    if (enZl4->csState == 0 && enZl4->actor.xzDistToPlayer < 600.0f && EnZl4_SetNextAnim(enZl4, 3)) {
        Audio_PlayFanfare(NA_BGM_APPEAR);
        enZl4->csState = 8; // ZL4_CS_PLAN
    } else {
        Npc_UpdateTalking(play, &enZl4->actor, &enZl4->interactInfo.talkState, enZl4->collider.dim.radius + 60.0f, EnZl4_GiveItemTextId, func_80B5B9B0);
        func_80B5BB78(enZl4, play);

        if (enZl4->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
            enZl4->talkState = 6;
            enZl4->actionFunc = EnZl4_Cutscene;
        }
    }
}

static int successChimeCooldown = 0;
void RateLimitedSuccessChime() {
    if (successChimeCooldown == 0) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        successChimeCooldown = 120;
    }
}

void TimeSaverOnGameFrameUpdateHandler() {
    if (successChimeCooldown > 0) {
        successChimeCooldown--;
    }
}

void TimeSaverOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* opt) {
    switch (id) {
        case GI_VB_PLAY_TRANSITION_CS:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.Intro", 0) && gSaveContext.entranceIndex == ENTR_LINKS_HOUSE_0 && gSaveContext.cutsceneIndex == 0xFFF1) {
                gSaveContext.cutsceneIndex = 0;
                *should = false;
            }

            if (CVarGetInteger("gTimeSavers.SkipCutscene.LearnSong", 0) || IS_RANDO) {
                // Song of Time
                if (gSaveContext.entranceIndex == ENTR_TEMPLE_OF_TIME_0 && gSaveContext.cutsceneIndex == 0xFFF7) {
                    gSaveContext.entranceIndex = ENTR_HYRULE_FIELD_16;
                    gSaveContext.cutsceneIndex = 0;
                    gSaveContext.nextTransitionType = 3;
                    *should = false;
                }

                // Requiem of Spirit
                if ((gSaveContext.entranceIndex == ENTR_DESERT_COLOSSUS_1) && !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_REQUIEM_OF_SPIRIT)) {
                    Flags_SetEventChkInf(EVENTCHKINF_LEARNED_REQUIEM_OF_SPIRIT);
                    // Normally happens in the cutscene
                    if (GameInteractor_Should(GI_VB_GIVE_ITEM_REQUIEM_OF_SPIRIT, true, NULL)) {
                        Item_Give(gPlayState, ITEM_SONG_REQUIEM);
                    }
                    *should = false;
                }
                
                u8 meetsBurningKakRequirements = 
                    LINK_IS_ADULT &&
                    gSaveContext.entranceIndex == ENTR_KAKARIKO_VILLAGE_0 &&
                    Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP) &&
                    Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP) &&
                    Flags_GetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP) &&
                    !Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL);
                if (GameInteractor_Should(GI_VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW, meetsBurningKakRequirements, NULL)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL);
                    // Normally happens in the cutscene
                    Flags_SetEventChkInf(EVENTCHKINF_LEARNED_NOCTURNE_OF_SHADOW);
                    if (GameInteractor_Should(GI_VB_GIVE_ITEM_NOCTURNE_OF_SHADOW, true, NULL)) {
                        Item_Give(gPlayState, ITEM_SONG_NOCTURNE);
                    }
                    *should = false;
                }
            }

            if (CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
                uint8_t isBlueWarp = 0;
                // Deku Tree Blue warp
                if (gSaveContext.entranceIndex == ENTR_KOKIRI_FOREST_0 && gSaveContext.cutsceneIndex == 0xFFF1) {
                    gSaveContext.dayTime = gSaveContext.skyboxTime = 0x8000;

                    gSaveContext.entranceIndex = ENTR_KOKIRI_FOREST_11;
                    isBlueWarp = 1;
                // Dodongo's Cavern Blue warp
                } else if (gSaveContext.entranceIndex == ENTR_DEATH_MOUNTAIN_TRAIL_0 && gSaveContext.cutsceneIndex == 0xFFF1) {
                    gSaveContext.entranceIndex = ENTR_DEATH_MOUNTAIN_TRAIL_5;
                    isBlueWarp = 1;
                // Jabu Jabu's Blue warp
                } else if (gSaveContext.entranceIndex == ENTR_ZORAS_FOUNTAIN_0 && gSaveContext.cutsceneIndex == 0xFFF0) {
                    gSaveContext.entranceIndex = ENTR_ZORAS_FOUNTAIN_0;
                    isBlueWarp = 1;
                // Forest Temple Blue warp
                } else if (gSaveContext.entranceIndex == ENTR_CHAMBER_OF_THE_SAGES_0 && gSaveContext.cutsceneIndex == 0x0 && gSaveContext.chamberCutsceneNum == CHAMBER_CS_FOREST) {
                    // Normally set in the blue warp cutscene
                    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_DEKU_TREE_SPROUT);
                    gSaveContext.dayTime = gSaveContext.skyboxTime = 0x8000;

                    gSaveContext.entranceIndex = ENTR_SACRED_FOREST_MEADOW_3;
                    isBlueWarp = 1;
                // Fire Temple Blue warp
                } else if (gSaveContext.entranceIndex == ENTR_KAKARIKO_VILLAGE_0 && gSaveContext.cutsceneIndex == 0xFFF3) {
                    gSaveContext.entranceIndex = ENTR_DEATH_MOUNTAIN_CRATER_5;
                    isBlueWarp = 1;
                // Water Temple Blue warp
                } else if (gSaveContext.entranceIndex == ENTR_CHAMBER_OF_THE_SAGES_0 && gSaveContext.cutsceneIndex == 0x0 && gSaveContext.chamberCutsceneNum == CHAMBER_CS_WATER) {
                    gSaveContext.entranceIndex = ENTR_LAKE_HYLIA_9;
                    isBlueWarp = 1;
                // Spirit Temple Blue warp
                } else if (gSaveContext.entranceIndex == ENTR_CHAMBER_OF_THE_SAGES_0 && gSaveContext.cutsceneIndex == 0x0 && gSaveContext.chamberCutsceneNum == CHAMBER_CS_SPIRIT) {
                    gSaveContext.entranceIndex = ENTR_DESERT_COLOSSUS_8;
                    isBlueWarp = 1;
                // Shadow Temple Blue warp
                } else if (gSaveContext.entranceIndex == ENTR_CHAMBER_OF_THE_SAGES_0 && gSaveContext.cutsceneIndex == 0x0 && gSaveContext.chamberCutsceneNum == CHAMBER_CS_SHADOW) {
                    gSaveContext.entranceIndex = ENTR_GRAVEYARD_8;
                    isBlueWarp = 1;
                }

                if (isBlueWarp) {
                    *should = false;
                    gSaveContext.cutsceneIndex = 0;

                    if (IS_RANDO && (RAND_GET_OPTION(RSK_SHUFFLE_DUNGEON_ENTRANCES) != RO_DUNGEON_ENTRANCE_SHUFFLE_OFF || RAND_GET_OPTION(RSK_SHUFFLE_BOSS_ENTRANCES) != RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF)) {
                        Entrance_OverrideBlueWarp();
                    }
                }

                // Flee hyrule castle cutscene
                if (gSaveContext.entranceIndex == ENTR_HYRULE_FIELD_0 && gSaveContext.cutsceneIndex == 0xFFF1) {
                    gSaveContext.cutsceneIndex = 0;
                    *should = false;
                }

                // Lost Woods Bridge
                if ((gSaveContext.entranceIndex == ENTR_LOST_WOODS_9) && !Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_SARIA_ON_BRIDGE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_SARIA_ON_BRIDGE);
                    if (GameInteractor_Should(GI_VB_GIVE_ITEM_FAIRY_OCARINA, true, NULL)) {
                        Item_Give(gPlayState, ITEM_OCARINA_FAIRY);
                    }
                    *should = false;
                }

                // LACS
                u8 meetsLACSRequirements =
                    LINK_IS_ADULT &&
                    (gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_TEMPLE_OF_TIME) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
                    !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
                if (GameInteractor_Should(GI_VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS, meetsLACSRequirements, NULL)) {
                    Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
                    if (GameInteractor_Should(GI_VB_GIVE_ITEM_LIGHT_ARROW, true, NULL)) {
                        Item_Give(gPlayState, ITEM_ARROW_LIGHT);
                    }
                    *should = false;
                }
            }
            break;
        case GI_VB_PLAY_ENTRANCE_CS: {
            s32* entranceFlag = static_cast<s32*>(opt);
            if (CVarGetInteger("gTimeSavers.SkipCutscene.Entrances", 0) && (*entranceFlag != EVENTCHKINF_EPONA_OBTAINED)) {
                *should = false;
            }
            break;
        }
        case GI_VB_PLAY_ONEPOINT_CS: {
            if (CVarGetInteger("gTimeSavers.SkipCutscene.OnePoint", 0)) {
                s16* csId = static_cast<s16*>(opt);
                switch (*csId) {
                    case 4180:
                    case 4100:
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                    default:
                        SPDLOG_INFO("GI_VB_PLAY_ONEPOINT_CS {}", *csId);
                        break;
                }
            }
            break;
        }
        case GI_VB_PLAY_ONEPOINT_ACTOR_CS: {
            if (CVarGetInteger("gTimeSavers.SkipCutscene.OnePoint", 0)) {
                Actor* actor = static_cast<Actor*>(opt);
                switch (actor->category) {
                    case ACTORCAT_BG:
                        if (actor->id == ACTOR_BG_DDAN_KD) {
                            BgDdanKd* ddanKd = static_cast<BgDdanKd*>(opt);
                            Flags_SetSwitch(gPlayState, ddanKd->dyna.actor.params);
                        }
                        if (actor->id == ACTOR_BG_MORI_HINERI) {
                            break;
                        }

                        RateLimitedSuccessChime();
                        *should = false;
                        break;
                }
                switch (actor->id) {
                    case ACTOR_OBJ_SWITCH: {
                        ObjSwitch *switchActor = static_cast<ObjSwitch*>(opt);
                        switchActor->cooldownTimer = 0;
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                    }
                    case ACTOR_BG_BDAN_SWITCH: {
                        BgBdanSwitch* switchActor = static_cast<BgBdanSwitch*>(opt);
                        switchActor->unk_1D8 = 0;
                        switchActor->unk_1DA = 0;
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                    }
                    // case ACTOR_PLAYER: // This might cause issues
                    case ACTOR_EN_TA:
                    case ACTOR_DOOR_SHUTTER:
                    case ACTOR_EN_BOX:
                    case ACTOR_OBJ_SYOKUDAI:
                    case ACTOR_OBJ_TIMEBLOCK:
                    case ACTOR_EN_PO_SISTERS:
                    // Prop
                    case ACTOR_OBJ_ICE_POLY:
                    case ACTOR_BG_YDAN_MARUTA:
                    case ACTOR_BG_SPOT18_SHUTTER:
                    case ACTOR_BG_SPOT05_SOKO:
                    case ACTOR_BG_SPOT18_BASKET:
                    // BG
                    // case ACTOR_BG_YDAN_SP:
                    // case ACTOR_BG_YDAN_HASI:
                    // case ACTOR_BG_DODOAGO:
                    // case ACTOR_BG_DDAN_KD:
                    // case ACTOR_BG_DDAN_JD:
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                }
                if (*should) {
                    SPDLOG_INFO("GI_VB_PLAY_ONEPOINT_ACTOR_CS ID:{} Cat:{}", actor->id, actor->category);
                }
            }
            break;
        }
        case GI_VB_SHOW_TITLE_CARD:
            if (CVarGetInteger("gTimeSavers.DisableTitleCard", 0)) {
                *should = false;
            }
            break;
        case GI_VB_WONDER_TALK: {
            if (CVarGetInteger("gTimeSavers.NoForcedDialog", 0)) {
                *should = false;
            }
            break;
        }
        case GI_VB_NAVI_TALK: {
            if (CVarGetInteger("gTimeSavers.NoForcedDialog", 0)) {
                ElfMsg* naviTalk = static_cast<ElfMsg*>(opt);
                Flags_SetSwitch(gPlayState, (naviTalk->actor.params >> 8) & 0x3F);
                Actor_Kill(&naviTalk->actor);
                *should = false;
            }
            break;
        }
        case GI_VB_NOT_BE_GREETED_BY_SARIA:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.Entrances", 0) && !Flags_GetInfTable(INFTABLE_GREETED_BY_SARIA)) {
                Flags_SetInfTable(INFTABLE_GREETED_BY_SARIA);
                *should = true;
            }
            break;
        case GI_VB_MOVE_MIDO_IN_KOKIRI_FOREST:
            if (
                CVarGetInteger("gTimeSavers.SkipMiscInteractions", 0) &&
                !Flags_GetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD) &&
                (CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_DEKU) && 
                (CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_KOKIRI)
            ) {
                Flags_SetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD);
                *should = true;
            }
            break;
        case GI_VB_PLAY_DEKU_TREE_INTRO_CS: {
            if (CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
                BgTreemouth* treeMouth = static_cast<BgTreemouth*>(opt);
                Flags_SetEventChkInf(EVENTCHKINF_DEKU_TREE_OPENED_MOUTH);
                Audio_PlaySoundGeneral(NA_SE_EV_WOODDOOR_OPEN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                BgTreemouth_SetupAction(treeMouth, func_808BC6F8);
                *should = false;
            }
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_BLUE_WARP:
        case GI_VB_PLAY_SHIEK_BLOCK_MASTER_SWORD_CS:
        case GI_VB_GIVE_ITEM_FAIRY_OCARINA:
        case GI_VB_GIVE_ITEM_LIGHT_ARROW:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
                *should = false;
            }
            break;
        case GI_VB_PLAY_NABOORU_CAPTURED_CS:
            if (*should == true && CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
                Flags_SetEventChkInf(EVENTCHKINF_NABOORU_CAPTURED_BY_TWINROVA);
                *should = false;
            }
            break;
        case GI_VB_PLAY_PULL_MASTER_SWORD_CS:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
                    // Normally, these would be done in the cutscene, but we're skipping it
                    Flags_SetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL);
                    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_MASTER_SWORD_CHAMBER);
                    Flags_SetEventChkInf(EVENTCHKINF_SHEIK_SPAWNED_AT_MASTER_SWORD_PEDESTAL);
                    Flags_SetEventChkInf(EVENTCHKINF_TIME_TRAVELED_TO_ADULT);
                    if (GameInteractor_Should(GI_VB_GIVE_ITEM_LIGHT_MEDALLION, true, NULL)) {
                        Item_Give(gPlayState, ITEM_MEDALLION_LIGHT);
                    }
                }
                *should = false;
            }
            break;
        case GI_VB_OWL_INTERACTION: {
            if (CVarGetInteger("gTimeSavers.SkipOwlInteractions", 0) && *should) {
                EnOwl* enOwl = static_cast<EnOwl*>(opt);
                s32 owlType = (enOwl->actor.params & 0xFC0) >> 6;

                if (((enOwl->actor.params & 0xFC0) >> 6) == 1) {
                    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_KAEPORA_BY_LOST_WOODS);
                }

                func_80ACA62C(enOwl, gPlayState);
                *should = false;
            }
            break;
        }
        case GI_VB_PLAY_EYEDROP_CREATION_ANIM:
        case GI_VB_PLAY_EYEDROPS_CS:
        case GI_VB_PLAY_DROP_FISH_FOR_JABU_CS:
        case GI_VB_PLAY_DARUNIAS_JOY_CS:
            if (CVarGetInteger("gTimeSavers.SkipMiscInteractions", 0)) {
                *should = false;
            }
            break;
        case GI_VB_PLAY_ZELDAS_LULLABY_CS: {
            if (CVarGetInteger("gTimeSavers.SkipCutscene.LearnSong", 0) || IS_RANDO) {
                DemoIm* demoIm = static_cast<DemoIm*>(opt);
                Player* player = GET_PLAYER(gPlayState);
                player->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                player->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM;
                func_80986794(demoIm);

                static uint32_t demoImUpdateHook = 0;
                static uint32_t demoImKillHook = 0;
                demoImUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* actorRef) mutable {
                    Actor* actor = static_cast<Actor*>(actorRef);
                    if (actor->id == ACTOR_DEMO_IM && (CVarGetInteger("gTimeSavers.SkipCutscene.LearnSong", 0) || IS_RANDO)) {
                        DemoIm* demoIm = static_cast<DemoIm*>(actorRef);
                        Player* player = GET_PLAYER(gPlayState);
                        player->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                        player->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM;

                        if (Animation_OnFrame(&demoIm->skelAnime, 25.0f)) {
                            Audio_PlaySoundGeneral(NA_SE_IT_DEKU, &demoIm->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(demoImUpdateHook);
                            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(demoImKillHook);
                            demoImUpdateHook = 0;
                            demoImKillHook = 0;
                        } else if (Animation_OnFrame(&demoIm->skelAnime, 15.0f)) {
                            Player* player = GET_PLAYER(gPlayState);
                            // SOH [Randomizer] In entrance rando have impa bring link back to the front of castle grounds
                            if (IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_OVERWORLD_ENTRANCES)) {
                                gPlayState->nextEntranceIndex = ENTR_HYRULE_CASTLE_0;
                            } else {
                                gPlayState->nextEntranceIndex = ENTR_HYRULE_FIELD_17;
                            }
                            gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
                            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                            gSaveContext.nextTransitionType = 2;
                            func_8002DF54(gPlayState, &player->actor, 8);
                        }
                    }
                });
                demoImKillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) mutable {
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(demoImUpdateHook);
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(demoImKillHook);
                    demoImUpdateHook = 0;
                    demoImKillHook = 0;
                });
                *should = false;
            }
            break;
        }
        case GI_VB_PLAY_SARIAS_SONG_CS: {
            if (CVarGetInteger("gTimeSavers.SkipCutscene.LearnSong", 0) || IS_RANDO) {
                EnSa* enSa = static_cast<EnSa*>(opt);
                enSa->actionFunc = func_80AF6B20;
                *should = false;
            }
            break;
        }
        case GI_VB_DESPAWN_HORSE_RACE_COW: {
            if (Flags_GetEventChkInf(EVENTCHKINF_WON_COW_IN_MALONS_RACE) && CVarGetInteger("gCowOfTime", 0)) {
                *should = false;
            }
            break;
        }
        case GI_VB_GIVE_ITEM_MINUET_OF_FOREST:
        case GI_VB_GIVE_ITEM_BOLERO_OF_FIRE:
        case GI_VB_GIVE_ITEM_SERENADE_OF_WATER:
        case GI_VB_GIVE_ITEM_REQUIEM_OF_SPIRIT:
        case GI_VB_GIVE_ITEM_NOCTURNE_OF_SHADOW:
        case GI_VB_GIVE_ITEM_PRELUDE_OF_LIGHT:
        case GI_VB_GIVE_ITEM_ZELDAS_LULLABY:
        case GI_VB_GIVE_ITEM_EPONAS_SONG:
        case GI_VB_GIVE_ITEM_SARIAS_SONG:
        case GI_VB_GIVE_ITEM_SUNS_SONG:
        case GI_VB_GIVE_ITEM_SONG_OF_TIME:
        case GI_VB_GIVE_ITEM_SONG_OF_STORMS:
        case GI_VB_PLAY_MINUET_OF_FOREST_CS:
        case GI_VB_PLAY_BOLERO_OF_FIRE_CS:
        case GI_VB_PLAY_SERENADE_OF_WATER_CS:
        case GI_VB_PLAY_PRELUDE_OF_LIGHT_CS:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.LearnSong", 0) || IS_RANDO) {
                *should = false;
            }
            break;
        case GI_VB_DAMPE_IN_GRAVEYARD_DESPAWN:
            if (CVarGetInteger("gDampeAllNight", 0)) {
                *should = LINK_IS_ADULT || gPlayState->sceneNum != SCENE_GRAVEYARD;
            }
            break;
        case GI_VB_BE_VALID_GRAVEDIGGING_SPOT:
            if (CVarGetInteger("gDampeWin", 0)) {
                EnTk *enTk = static_cast<EnTk*>(opt);
                enTk->validDigHere = true;
                *should = true;
            }
            break;
        case GI_VB_BE_DAMPE_GRAVEDIGGING_GRAND_PRIZE:
            if (CVarGetInteger("gDampeWin", 0)) {
                EnTk *enTk = static_cast<EnTk*>(opt);
                enTk->currentReward = 3;
                *should = true;
            }
            break;
        case GI_VB_DAMPE_GRAVEDIGGING_GRAND_PRIZE_BE_HEART_PIECE:
            if (CVarGetInteger("gGravediggingTourFix", 0) || IS_RANDO) {
                *should = !Flags_GetCollectible(gPlayState, COLLECTFLAG_GRAVEDIGGING_HEART_PIECE);
            }
            break;
        case GI_VB_FIX_SAW_SOFTLOCK:
            // Animation Count should be no more than 1 to guarantee putaway is complete after giving the saw
            // As this is vanilla behavior, it only applies with the Fix toggle or Skip Text enabled.
            *should = (CVarGetInteger("gFixSawSoftlock", 0) != 0 || CVarGetInteger("gSkipText", 0) != 0) ? gPlayState->animationCtx.animationCount > 1 : *should;
            break;
        case GI_VB_BIGGORON_CONSIDER_SWORD_FORGED:
            *should = Environment_GetBgsDayCount() >= CVarGetInteger("gForgeTime", 3);
            break;
    }
}

static uint32_t enKoUpdateHook = 0;
static uint32_t enKoKillHook = 0;
static uint32_t itemOcarinaUpdateHook = 0;
static uint32_t itemOcarinaframesSinceSpawn = 0;
static uint32_t enMa1UpdateHook = 0;
static uint32_t enMa1KillHook = 0;
void TimeSaverOnActorInitHandler(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (actor->id == ACTOR_EN_KO && (actor->params & 0xFF) == ENKO_TYPE_CHILD_3) {
        enKoUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) mutable {
            Actor* innerActor = static_cast<Actor*>(innerActorRef);
            if (innerActor->id == ACTOR_EN_KO && (innerActor->params & 0xFF) == ENKO_TYPE_CHILD_3 && (CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0) || IS_RANDO)) {
                EnKo* enKo = static_cast<EnKo*>(innerActorRef);
                // They haven't moved yet, wrap their update function so we check every frame
                if (enKo->actionFunc == func_80A995CC) {
                    enKo->actionFunc = EnKo_MoveWhenReady;
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enKoUpdateHook);
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enKoKillHook);
                    enKoUpdateHook = 0;
                    enKoKillHook = 0;
                // They have already moved
                } else if (enKo->actionFunc == func_80A99384) {
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enKoUpdateHook);
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enKoKillHook);
                    enKoUpdateHook = 0;
                    enKoKillHook = 0;
                }
            }
        });
        enKoKillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) mutable {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enKoUpdateHook);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enKoKillHook);
            enKoUpdateHook = 0;
            enKoKillHook = 0;
        });
    }

    if (actor->id == ACTOR_ITEM_OCARINA && actor->params == 3 && CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
        itemOcarinaframesSinceSpawn = 0;
        itemOcarinaUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) mutable {
            Actor* innerActor = static_cast<Actor*>(innerActorRef);
            if (innerActor->id != ACTOR_ITEM_OCARINA || innerActor->params != 3) return;
            itemOcarinaframesSinceSpawn++;
            if (itemOcarinaframesSinceSpawn > 20) {
                Audio_PlayActorSound2(innerActor, NA_SE_EV_BOMB_DROP_WATER);

                GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(itemOcarinaUpdateHook);
                itemOcarinaUpdateHook = 0;
            }
        });
    }

    if (actor->id == ACTOR_EN_MA1 && gPlayState->sceneNum == SCENE_LON_LON_RANCH) {
        enMa1UpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) mutable {
            Actor* innerActor = static_cast<Actor*>(innerActorRef);
            if (innerActor->id == ACTOR_EN_MA1 && (CVarGetInteger("gTimeSavers.SkipCutscene.LearnSong", 0) || IS_RANDO)) {
                EnMa1* enMa1 = static_cast<EnMa1*>(innerActorRef);
                if (enMa1->actionFunc == func_80AA106C) {
                    enMa1->actionFunc = EnMa1_EndTeachSong;
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enMa1UpdateHook);
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enMa1KillHook);
                    enMa1UpdateHook = 0;
                    enMa1KillHook = 0;
                // They've already learned the song
                } else if (enMa1->actionFunc == func_80AA0D88) {
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enMa1UpdateHook);
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enMa1KillHook);
                    enMa1UpdateHook = 0;
                    enMa1KillHook = 0;
                }
            }
        });
        enMa1KillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) mutable {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enMa1UpdateHook);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enMa1KillHook);
            enMa1UpdateHook = 0;
            enMa1KillHook = 0;
        });
    }

    if (actor->id == ACTOR_EN_ZL4 && CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
        EnZl4* enZl4 = static_cast<EnZl4*>(actorRef);
        if (enZl4->actionFunc != EnZl4_Cutscene || enZl4->csState != 0) return;

        enZl4->actionFunc = EnZl4_SkipToGivingZeldasLetter;
    }
}

void TimeSaverOnSceneInitHandler(int16_t sceneNum) {
    switch (sceneNum) {
        case SCENE_HYRULE_CASTLE:
            if (CVarGetInteger("gTimeSavers.SkipMiscInteractions", 0) && !Flags_GetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE)) {
                Flags_SetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE);
                Flags_SetInfTable(INFTABLE_MET_CHILD_MALON_AT_CASTLE_OR_MARKET);
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET);
            }
            break;
        case SCENE_LON_LON_RANCH:
            if (CVarGetInteger("gTimeSavers.SkipMiscInteractions", 0) && GameInteractor_Should(GI_VB_MALON_RETURN_FROM_CASTLE, Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE), NULL)) {
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH);
                Flags_SetInfTable(INFTABLE_CHILD_MALON_SAID_EPONA_WAS_AFRAID_OF_YOU);
                Flags_SetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
            }
            break;
        case SCENE_DEKU_TREE_BOSS:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.BossIntro", 0)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_GOHMA_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_GOHMA_BATTLE);
                }
            }
            break;
        case SCENE_DODONGOS_CAVERN_BOSS:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.BossIntro", 0)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE);
                }
            }
            break;
        case SCENE_JABU_JABU_BOSS:
            if (CVarGetInteger("gTimeSavers.SkipCutscene.BossIntro", 0)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE);
                }
            }
            break;
    }
}

static GetItemEntry vanillaQueuedItemEntry = GET_ITEM_NONE;

void TimeSaverOnFlagSetHandler(int16_t flagType, int16_t flag) {
    if (!CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) return;

    switch (flagType) {
        case FLAG_EVENT_CHECK_INF:
            switch (flag) {
                case EVENTCHKINF_SPOKE_TO_SARIA_ON_BRIDGE:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_FAIRY_OCARINA).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_ZELDAS_LULLABY:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_ZELDAS_LULLABY).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_MINUET_OF_FOREST:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_MINUET_OF_FOREST).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_BOLERO_OF_FIRE:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_BOLERO_OF_FIRE).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_SERENADE_OF_WATER:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_SERENADE_OF_WATER).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_REQUIEM_OF_SPIRIT:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_REQUIEM_OF_SPIRIT).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_NOCTURNE_OF_SHADOW).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_PRELUDE_OF_LIGHT).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_SARIAS_SONG:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_SARIAS_SONG).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_KOKIRI_EMERALD).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_GORON_RUBY).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_ZORA_SAPPHIRE).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_FOREST_MEDALLION).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_FIRE_MEDALLION).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_WATER_MEDALLION).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_LIGHT_ARROWS).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_TIME_TRAVELED_TO_ADULT:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_LIGHT_MEDALLION).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_SONG_OF_TIME:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_SONG_OF_TIME).GetGIEntry_Copy();
                    break;
            }
            break;
        case FLAG_RANDOMIZER_INF:
            switch (flag) {
                case RAND_INF_LEARNED_EPONA_SONG:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_EPONAS_SONG).GetGIEntry_Copy();
                    break;
                case RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_SHADOW_MEDALLION).GetGIEntry_Copy();
                    break;
                case RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_SPIRIT_MEDALLION).GetGIEntry_Copy();
                    break;
            }
            break;
    }
}

void TimeSaverOnPlayerUpdateHandler() {
    if (vanillaQueuedItemEntry.itemId == ITEM_NONE) return;

    Player* player = GET_PLAYER(gPlayState);
    if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_ITEM_OVER_HEAD) {
        return;
    }

    SPDLOG_INFO("Attempting to give Item: mod {} item {}", vanillaQueuedItemEntry.modIndex, vanillaQueuedItemEntry.itemId);
    GiveItemEntryWithoutActor(gPlayState, vanillaQueuedItemEntry);
    if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        // Allow the player to receive the item while swimming
        player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        func_8083E5A8(player, gPlayState);
    }
}

void TimeSaverOnItemReceiveHandler(GetItemEntry receivedItemEntry) {
    if (vanillaQueuedItemEntry.itemId == ITEM_NONE) return;

    if (vanillaQueuedItemEntry.modIndex == receivedItemEntry.modIndex && vanillaQueuedItemEntry.itemId == receivedItemEntry.itemId) {
        SPDLOG_INFO("Item received: mod {} item {}", receivedItemEntry.modIndex, receivedItemEntry.itemId);
        vanillaQueuedItemEntry = GET_ITEM_NONE;
    }
}

static uint32_t onSceneInitHook = 0;
static uint32_t onVanillaBehaviorHook = 0;
static uint32_t onActorInitHook = 0;
static uint32_t onGameFrameUpdate = 0;
static uint32_t onFlagSetHook = 0;
static uint32_t onPlayerUpdateHook = 0;
static uint32_t onItemReceiveHook = 0;
void TimeSaverRegisterHooks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) mutable {
        vanillaQueuedItemEntry = GET_ITEM_NONE;

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(onGameFrameUpdate);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(onItemReceiveHook);

        onSceneInitHook = 0;
        onVanillaBehaviorHook = 0;
        onActorInitHook = 0;
        onGameFrameUpdate = 0;
        onFlagSetHook = 0;
        onPlayerUpdateHook = 0;
        onItemReceiveHook = 0;

        onSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(TimeSaverOnSceneInitHandler);
        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(TimeSaverOnVanillaBehaviorHandler);
        onActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(TimeSaverOnActorInitHandler);
        onGameFrameUpdate = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(TimeSaverOnGameFrameUpdateHandler);

        if (IS_RANDO) return;

        onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(TimeSaverOnFlagSetHandler);
        onPlayerUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(TimeSaverOnPlayerUpdateHandler);
        onItemReceiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(TimeSaverOnItemReceiveHandler);
    });
}
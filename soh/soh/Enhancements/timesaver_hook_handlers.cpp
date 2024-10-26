#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/enhancementTypes.h"

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
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"
#include "src/overlays/actors/ovl_Demo_Im/z_demo_im.h"
#include "src/overlays/actors/ovl_En_Sa/z_en_sa.h"
#include "src/overlays/actors/ovl_Bg_Ddan_Kd/z_bg_ddan_kd.h"
#include "src/overlays/actors/ovl_En_Tk/z_en_tk.h"
#include "src/overlays/actors/ovl_En_Fu/z_en_fu.h"
#include "src/overlays/actors/ovl_Bg_Spot02_Objects/z_bg_spot02_objects.h"
#include "src/overlays/actors/ovl_Bg_Spot03_Taki/z_bg_spot03_taki.h"
#include "src/overlays/actors/ovl_Bg_Hidan_Kousi/z_bg_hidan_kousi.h"
#include "src/overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.h"
#include "src/overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.h"
#include "src/overlays/actors/ovl_En_Po_Sisters/z_en_po_sisters.h"
#include <overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h>
#include <overlays/actors/ovl_En_Ik/z_en_ik.h>
#include <objects/object_gnd/object_gnd.h>
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern int32_t D_8011D3AC;
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).GetSelectedOptionIndex()

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

void EnFu_EndTeachSong(EnFu* enFu, PlayState* play) {
    if (Message_GetState(&gPlayState->msgCtx) == TEXT_STATE_CLOSING) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        enFu->actionFunc = EnFu_WaitAdult;
        enFu->actor.flags &= ~ACTOR_FLAG_WILL_TALK;

        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        Flags_SetEventChkInf(EVENTCHKINF_PLAYED_SONG_OF_STORMS_IN_WINDMILL);
        Flags_SetEventChkInf(EVENTCHKINF_LEARNED_SONG_OF_STORMS);
        return;
    }
}

void EnDntDemo_JudgeSkipToReward(EnDntDemo* enDntDemo, PlayState* play) {
    // todo: figure out a better way to handle toggling so we don't
    //       need to double check cvars like this
    if(!(IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO))) {
        EnDntDemo_Judge(enDntDemo, play);
        return;
    }

    if (enDntDemo->actor.xzDistToPlayer > 30.0f) {
        EnDntDemo_Judge(enDntDemo, play);
        return;
    }

    Player* player = GET_PLAYER(play);
    switch (Player_GetMask(play)) {
        case PLAYER_MASK_SKULL: {
            Flags_SetItemGetInf(ITEMGETINF_OBTAINED_STICK_UPGRADE_FROM_STAGE);
            return;
        }
        case PLAYER_MASK_TRUTH: {
            Flags_SetItemGetInf(ITEMGETINF_OBTAINED_NUT_UPGRADE_FROM_STAGE);
            return;
        }
        default: {
            EnDntDemo_Judge(enDntDemo, play);
            return;
        }
    }
}

static int successChimeCooldown = 0;
void RateLimitedSuccessChime() {
    if (successChimeCooldown == 0) {
        // Currently disabled, need to find a better way to do this, while being consistent with vanilla
        // func_80078884(NA_SE_SY_CORRECT_CHIME);
        successChimeCooldown = 120;
    }
}

bool ForcedDialogIsDisabled(ForcedDialogMode type) {
    return (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipForcedDialog"),
                           IS_RANDO ? FORCED_DIALOG_SKIP_ALL : FORCED_DIALOG_SKIP_NONE) &
            type) != 0;
}

void TimeSaverOnGameFrameUpdateHandler() {
    if (successChimeCooldown > 0) {
        successChimeCooldown--;
    }
}

void TimeSaverOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    switch (id) {
        case VB_PLAY_TRANSITION_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO) {
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
                    gSaveContext.dayTime = gSaveContext.skyboxTime = 0xAC60;
                    if (GameInteractor_Should(VB_GIVE_ITEM_REQUIEM_OF_SPIRIT, true)) {
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
                if (GameInteractor_Should(VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW, meetsBurningKakRequirements)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL);
                    // Normally happens in the cutscene
                    Flags_SetEventChkInf(EVENTCHKINF_LEARNED_NOCTURNE_OF_SHADOW);
                    if (GameInteractor_Should(VB_GIVE_ITEM_NOCTURNE_OF_SHADOW, true)) {
                        Item_Give(gPlayState, ITEM_SONG_NOCTURNE);
                    }
                    *should = false;
                }
            }

            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
                // LACS
                u8 meetsLACSRequirements =
                    LINK_IS_ADULT &&
                    (gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_TEMPLE_OF_TIME) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
                    !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
                if (GameInteractor_Should(VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS, meetsLACSRequirements)) {
                    Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
                    if (GameInteractor_Should(VB_GIVE_ITEM_LIGHT_ARROW, true)) {
                        Item_Give(gPlayState, ITEM_ARROW_LIGHT);
                    }
                    *should = false;
                }
            }

            if (gSaveContext.entranceIndex == ENTR_GANONS_TOWER_COLLAPSE_EXTERIOR_0) {
                if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipTowerEscape"), false) || IS_BOSS_RUSH) {
                    Flags_SetEventChkInf(EVENTCHKINF_WATCHED_GANONS_CASTLE_COLLAPSE_CAUGHT_BY_GERUDO);
                    gSaveContext.entranceIndex = ENTR_GANON_BOSS_0;
                    *should = false;
                }
            }

            if (gSaveContext.entranceIndex == ENTR_CASTLE_COURTYARD_GUARDS_DAY_0) {
                if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipChildStealth"), false)) {
                    gSaveContext.entranceIndex = ENTR_CASTLE_COURTYARD_ZELDA_0;
                    *should = false;
                }
            }
            break;
        }
        case VB_PLAY_ENTRANCE_CS: {
            s32* entranceFlag = va_arg(args, s32*);
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Entrances"), IS_RANDO) && (*entranceFlag != EVENTCHKINF_EPONA_OBTAINED)) {
                *should = false;
            }
            break;
        }
        case VB_PLAY_ONEPOINT_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), IS_RANDO)) {
                s16* csId = va_arg(args, s16*);
                BgSpot03Taki* taki = NULL;
                switch (*csId) {
                    case 4180:
                    case 4100:
                        *should = false;
                        RateLimitedSuccessChime();
                        taki = (BgSpot03Taki*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor,
                                                               ACTOR_BG_SPOT03_TAKI, ACTORCAT_BG, 999.0f);
                        if (taki != NULL) {
                            func_8003EBF8(gPlayState, &gPlayState->colCtx.dyna, taki->dyna.bgId);
                        }
                        break;
                    default:
                        SPDLOG_INFO("VB_PLAY_ONEPOINT_CS {}", *csId);
                        break;
                }
            }
            break;
        }
        case VB_PLAY_ONEPOINT_ACTOR_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), IS_RANDO)) {
                Actor* actor = va_arg(args, Actor*);

                // there are a few checks throughout the game (such as chest spawns) that rely on this
                // the checks are for func_8005B198() == this->dyna.actor.category
                // func_8005B198 just returns D_8011D3AC
                // D_8011D3AC is set to camera->target->category in Camera_Demo5
                D_8011D3AC = actor->category;

                switch (actor->category) {
                    case ACTORCAT_BG:
                        if (actor->id == ACTOR_BG_DDAN_KD) {
                            BgDdanKd* ddanKd = (BgDdanKd*)actor;
                            Flags_SetSwitch(gPlayState, ddanKd->dyna.actor.params);
                        }
                        if (actor->id == ACTOR_BG_MORI_HINERI) {
                            break;
                        }
                        // This is handled in the FasterHeavyBlockLift enhancement
                        if (actor->id == ACTOR_BG_HEAVY_BLOCK) {
                            break;
                        }

                        RateLimitedSuccessChime();
                        *should = false;
                        break;
                }
                switch (actor->id) {
                    case ACTOR_OBJ_SWITCH: {
                        ObjSwitch *switchActor = (ObjSwitch*)actor;
                        switchActor->cooldownTimer = 0;
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                    }
                    case ACTOR_BG_BDAN_SWITCH: {
                        // The switch in jabu that you are intended to press with a box to reach barrinade
                        // can be skipped by either a frame perfect roll open or with OI
                        // The One Point for that switch is used in common setups for the former and is required for the latter to work
                        if (actor->params == 14848 && gPlayState->sceneNum == SCENE_JABU_JABU && !CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.GlitchAiding"), 0)){
                            break;
                        }
                        BgBdanSwitch* switchActor = (BgBdanSwitch*)actor;
                        switchActor->unk_1D8 = 0;
                        switchActor->unk_1DA = 0;
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                    }
                    case ACTOR_BG_HIDAN_KOUSI: {
                        BgHidanKousi* switchActor = (BgHidanKousi*)actor;
                        BgHidanKousi_SetupAction(switchActor, func_80889C18);
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                    }
                    case ACTOR_BG_HIDAN_FWBIG: {
                        *should = false;
                        break;
                    }
                    case ACTOR_EN_EX_ITEM: {
                        *should = false;
                        break;
                    }
                    case ACTOR_EN_DNT_NOMAL: {
                        *should = false;
                        break;
                    }
                    case ACTOR_EN_DNT_DEMO: {
                        *should = false;
                        break;
                    }
                    case ACTOR_EN_TA:
                    case ACTOR_DOOR_SHUTTER:
                    case ACTOR_BG_ICE_SHUTTER:
                    case ACTOR_OBJ_LIGHTSWITCH:
                    case ACTOR_EN_BOX:
                    case ACTOR_OBJ_SYOKUDAI:
                    case ACTOR_OBJ_TIMEBLOCK:
                    case ACTOR_EN_PO_SISTERS:
                    case ACTOR_OBJ_ICE_POLY:
                    case ACTOR_BG_YDAN_MARUTA:
                    case ACTOR_BG_SPOT18_SHUTTER:
                    case ACTOR_BG_SPOT05_SOKO:
                    case ACTOR_BG_SPOT18_BASKET:
                    case ACTOR_BG_HIDAN_CURTAIN:
                        *should = false;
                        RateLimitedSuccessChime();
                        break;
                }
                if (*should) {
                    SPDLOG_INFO("VB_PLAY_ONEPOINT_ACTOR_CS ID:{} Cat:{}", actor->id, actor->category);
                }
            }
            break;
        }
        case VB_SHOW_TITLE_CARD:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.DisableTitleCard"), IS_RANDO)) {
                *should = false;
            }
            break;
        case VB_WONDER_TALK: {
            //We want to show the frog hint if it is on, regardless of cutscene settings
            if (ForcedDialogIsDisabled(FORCED_DIALOG_SKIP_NPC) &&
                !(gPlayState->sceneNum == SCENE_ZORAS_RIVER && IS_RANDO && RAND_GET_OPTION(RSK_FROGS_HINT))) {
                *should = false;
            }
            break;
        }
        case VB_NAVI_TALK: {
            if (ForcedDialogIsDisabled(FORCED_DIALOG_SKIP_NAVI)) {
                ElfMsg* naviTalk = va_arg(args, ElfMsg*);
                if (((naviTalk->actor.params >> 8) & 0x3F) != 0x3F) {
                    Flags_SetSwitch(gPlayState, (naviTalk->actor.params >> 8) & 0x3F);
                    Actor_Kill(&naviTalk->actor);
                    *should = false;
                }
            }
            break;
        }
        case VB_NOT_BE_GREETED_BY_SARIA:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Entrances"), IS_RANDO) && !Flags_GetInfTable(INFTABLE_GREETED_BY_SARIA)) {
                Flags_SetInfTable(INFTABLE_GREETED_BY_SARIA);
                *should = true;
            }
            break;
        case VB_PLAY_SHIEK_BLOCK_MASTER_SWORD_CS:
        case VB_GIVE_ITEM_LIGHT_ARROW:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
                *should = false;
            }
            break;
        case VB_PLAY_NABOORU_CAPTURED_CS:
            if (*should == true && CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
                Flags_SetEventChkInf(EVENTCHKINF_NABOORU_CAPTURED_BY_TWINROVA);
                *should = false;
            }
            break;
        case VB_PLAY_PULL_MASTER_SWORD_CS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
                    // Normally, these would be done in the cutscene, but we're skipping it
                    Flags_SetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL);
                    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_MASTER_SWORD_CHAMBER);
                    Flags_SetEventChkInf(EVENTCHKINF_SHEIK_SPAWNED_AT_MASTER_SWORD_PEDESTAL);
                    Flags_SetEventChkInf(EVENTCHKINF_TIME_TRAVELED_TO_ADULT);
                    if (GameInteractor_Should(VB_GIVE_ITEM_LIGHT_MEDALLION, true)) {
                        Item_Give(gPlayState, ITEM_MEDALLION_LIGHT);
                    }
                }
                *should = false;
            }
            break;
        case VB_OWL_INTERACTION: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipOwlInteractions"), IS_RANDO) && *should) {
                EnOwl* enOwl = va_arg(args, EnOwl*);
                s32 owlType = (enOwl->actor.params & 0xFC0) >> 6;

                if (((enOwl->actor.params & 0xFC0) >> 6) == 1) {
                    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_KAEPORA_BY_LOST_WOODS);
                }

                func_80ACA62C(enOwl, gPlayState);
                *should = false;
            }
            break;
        }
        case VB_OVERRIDE_LINK_THE_GORON_DIALOGUE: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
                u16* textId = va_arg(args, u16*);
                // If the doors are not open yet, prioritize opening them
                if (!Flags_GetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED)) {
                    *textId = 0x3036;
                    *should = true;
                }
            }
            break;
        }
        case VB_PLAY_EYEDROP_CREATION_ANIM:
        case VB_PLAY_EYEDROPS_CS:
        case VB_PLAY_DROP_FISH_FOR_JABU_CS:
        case VB_PLAY_DARUNIAS_JOY_CS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
                *should = false;
            }
            break;
        case VB_PLAY_ZELDAS_LULLABY_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO) {
                DemoIm* demoIm = va_arg(args, DemoIm*);
                Player* player = GET_PLAYER(gPlayState);
                player->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                player->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM;
                func_80986794(demoIm);

                static uint32_t demoImUpdateHook = 0;
                static uint32_t demoImKillHook = 0;
                demoImUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* actorRef) mutable {
                    Actor* actor = static_cast<Actor*>(actorRef);
                    if (actor->id == ACTOR_DEMO_IM && (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO)) {
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
                            gSaveContext.dayTime = gSaveContext.skyboxTime = 0x8000;
                            gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
                            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                            gSaveContext.nextTransitionType = 2;
                            Player_SetCsActionWithHaltedActors(gPlayState, &player->actor, 8);
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
        case VB_PLAY_SARIAS_SONG_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO) {
                EnSa* enSa = va_arg(args, EnSa*);
                enSa->actionFunc = func_80AF6B20;
                *should = false;
            }
            break;
        }
        case VB_DESPAWN_HORSE_RACE_COW: {
            if (Flags_GetEventChkInf(EVENTCHKINF_WON_COW_IN_MALONS_RACE) && CVarGetInteger(CVAR_ENHANCEMENT("CowOfTime"), 0)) {
                *should = false;
            }
            break;
        }
        case VB_PLAY_DRAIN_WELL_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
                *should = false;
                Flags_SetSwitch(gPlayState, 0x2);
                Flags_SetEventChkInf(EVENTCHKINF_PLAYED_SONG_OF_STORMS_IN_WINDMILL);
                Flags_SetEventChkInf(EVENTCHKINF_DRAINED_WELL_IN_KAKARIKO);
            }
            break;
        }
        case VB_PLAY_SUNS_SONG_CS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO) {
                *should = false;
                Flags_SetEventChkInf(EVENTCHKINF_LEARNED_SUNS_SONG);
                // SoH [Randomizer] TODO: Increment time X amount (find out X)
                // When time is 0, it's changed to 0x46A7
                // When it's 0x8000, it's changed to 0xC090
            }
            break;
        case VB_PLAY_ROYAL_FAMILY_TOMB_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
                *should = false;
            }
            break;
        }
        case VB_PLAY_ROYAL_FAMILY_TOMB_EXPLODE: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
                *should = Flags_GetEventChkInf(EVENTCHKINF_DESTROYED_ROYAL_FAMILY_TOMB);
            }
            break;
        }
        case VB_PLAY_DOOR_OF_TIME_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
                *should = false;
                Flags_SetEnv(gPlayState, 2);
                func_80078884(NA_SE_SY_CORRECT_CHIME);
            }
            break;
        }
        case VB_GIVE_ITEM_MINUET_OF_FOREST:
        case VB_GIVE_ITEM_BOLERO_OF_FIRE:
        case VB_GIVE_ITEM_SERENADE_OF_WATER:
        case VB_GIVE_ITEM_REQUIEM_OF_SPIRIT:
        case VB_GIVE_ITEM_NOCTURNE_OF_SHADOW:
        case VB_GIVE_ITEM_PRELUDE_OF_LIGHT:
        case VB_GIVE_ITEM_ZELDAS_LULLABY:
        case VB_GIVE_ITEM_EPONAS_SONG:
        case VB_GIVE_ITEM_SARIAS_SONG:
        case VB_GIVE_ITEM_SUNS_SONG:
        case VB_GIVE_ITEM_SONG_OF_TIME:
        case VB_GIVE_ITEM_SONG_OF_STORMS:
        case VB_PLAY_MINUET_OF_FOREST_CS:
        case VB_PLAY_BOLERO_OF_FIRE_CS:
        case VB_PLAY_SERENADE_OF_WATER_CS:
        case VB_PLAY_PRELUDE_OF_LIGHT_CS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO) {
                *should = false;
            }
            break;
        case VB_FREEZE_ON_SKULL_TOKEN:
            if (CVarGetInteger(CVAR_ENHANCEMENT("SkulltulaFreeze"), 0)) {
                *should = false;
            }
            break;
        case VB_DAMPE_IN_GRAVEYARD_DESPAWN:
            if (CVarGetInteger(CVAR_ENHANCEMENT("DampeAllNight"), 0)) {
                *should = LINK_IS_ADULT || gPlayState->sceneNum != SCENE_GRAVEYARD;
            }
            break;
        case VB_BE_VALID_GRAVEDIGGING_SPOT:
            if (CVarGetInteger(CVAR_ENHANCEMENT("DampeWin"), 0)) {
                EnTk *enTk = va_arg(args, EnTk*);
                enTk->validDigHere = true;
                *should = true;
            }
            break;
        case VB_BE_DAMPE_GRAVEDIGGING_GRAND_PRIZE:
            if (CVarGetInteger(CVAR_ENHANCEMENT("DampeWin"), 0)) {
                EnTk *enTk = va_arg(args, EnTk*);
                enTk->currentReward = 3;
                *should = true;
            }
            break;
        case VB_DAMPE_GRAVEDIGGING_GRAND_PRIZE_BE_HEART_PIECE:
            if (CVarGetInteger(CVAR_ENHANCEMENT("GravediggingTourFix"), 0) || IS_RANDO) {
                *should = !Flags_GetCollectible(gPlayState, COLLECTFLAG_GRAVEDIGGING_HEART_PIECE);
            }
            break;
        case VB_FIX_SAW_SOFTLOCK:
            // Animation Count should be no more than 1 to guarantee putaway is complete after giving the saw
            // As this is vanilla behavior, it only applies with the Fix toggle or Skip Text enabled.
            *should = (CVarGetInteger(CVAR_ENHANCEMENT("FixSawSoftlock"), 0) != 0 || CVarGetInteger(CVAR_ENHANCEMENT("SkipText"), 0) != 0) ? gPlayState->animationCtx.animationCount > 1 : *should;
            break;
        case VB_BIGGORON_CONSIDER_SWORD_FORGED:
            *should = Environment_GetBgsDayCount() >= CVarGetInteger(CVAR_ENHANCEMENT("ForgeTime"), 3);
            break;
        case VB_BE_ELIGIBLE_FOR_GREAT_FAIRY_REWARD: {
            BgDyYoseizo* bgDyYoseizo = va_arg(args, BgDyYoseizo*);
            RandomizerInf flag = RAND_INF_MAX;

            if (gPlayState->sceneNum == SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS) {
                switch (bgDyYoseizo->fountainType) {
                    case 0:
                        flag = RAND_INF_ZF_GREAT_FAIRY_REWARD;
                        break;
                    case 1:
                        flag = RAND_INF_HC_GREAT_FAIRY_REWARD;
                        break;
                    case 2:
                        flag = RAND_INF_COLOSSUS_GREAT_FAIRY_REWARD;
                        break;
                }
            } else {
                switch (bgDyYoseizo->fountainType) {
                    case 0:
                        flag = RAND_INF_DMT_GREAT_FAIRY_REWARD;
                        break;
                    case 1:
                        flag = RAND_INF_DMC_GREAT_FAIRY_REWARD;
                        break;
                    case 2:
                        flag = RAND_INF_OGC_GREAT_FAIRY_REWARD;
                        break;
                }
            }

            if (flag != RAND_INF_MAX && (IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO))) {
                if (IS_RANDO || *should) {
                    Flags_SetRandomizerInf(flag);
                    gSaveContext.healthAccumulator = 0x140;
                    Magic_Fill(gPlayState);
                }
                *should = false;
            }

            break;
        }
        case VB_PLAY_RAINBOW_BRIDGE_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
                *should = false;
                if (!Flags_GetEventChkInf(EVENTCHKINF_RAINBOW_BRIDGE_BUILT)) {
                    func_800F595C(NA_BGM_BRIDGE_TO_GANONS);
                    // This would have been set 2 frames later, but we're skipping now so the sound doesn't play twice
                    Flags_SetEventChkInf(EVENTCHKINF_RAINBOW_BRIDGE_BUILT);
                }
            }
            break;
        }
        case VB_PHANTOM_GANON_DEATH_SCENE: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.QuickBossDeaths"), IS_RANDO || IS_BOSS_RUSH)) {
                *should = false;
                BossGanondrof* pg = va_arg(args, BossGanondrof*);
                Player* player = GET_PLAYER(gPlayState);
                if (pg->work[GND_ACTION_STATE] == DEATH_SPASM) {
                    // Skip to death scream animation and move ganondrof to middle
                    pg->deathState = DEATH_SCREAM;
                    pg->timers[0] = 50;
                    AnimationHeader* screamAnim = (AnimationHeader*)gPhantomGanonScreamAnim;
                    Animation_MorphToLoop(&pg->skelAnime, screamAnim, -10.0f);
                    pg->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
                    pg->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
                    pg->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                    pg->actor.shape.rot.y = 0;
                    pg->work[GND_BODY_DECAY_INDEX] = 0;
                    Audio_PlayActorSound2(&pg->actor, NA_SE_EN_FANTOM_LAST);

                    // Move Player out of the center of the room
                    player->actor.world.pos.x = GND_BOSSROOM_CENTER_X - 200.0f;
                    player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                }
            }
            break;
        }
        case VB_NABOORU_KNUCKLE_DEATH_SCENE: {
            EnIk* ik = va_arg(args, EnIk*);
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.QuickBossDeaths"), IS_RANDO)) {
                // Because no CS in rando, we hide the death of the knuckle by spawning flames and kill the actor
                if ((ik->actor.colChkInfo.health <= 10)) {
                    s32 i;
                    Vec3f pos;
                    Vec3f sp7C = { 0.0f, 0.5f, 0.0f };
                    int flameAmount = 100;

                    for (i = flameAmount; i >= 0; i--) {
                        pos.x = ik->actor.world.pos.x + Rand_CenteredFloat(120.0f);
                        pos.z = ik->actor.world.pos.z + Rand_CenteredFloat(120.0f);
                        pos.y = ik->actor.world.pos.y + 20.0f + Rand_CenteredFloat(120.0f);
                        EffectSsDeadDb_Spawn(gPlayState, &pos, &sp7C, &sp7C, 100, 0, 255, 255, 255, 255, 0, 0, 255, 1, 9,
                                             true);
                    }
                    Actor_Kill(&ik->actor);
                }
                *should = false;
            }
            break;
        }
        case VB_PLAY_SLOW_CHEST_CS: {
            if (CVarGetInteger(CVAR_ENHANCEMENT("FastChests"), 0)) {
                *should = false;
            } else if (CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_DISABLED) && *should) {
                EnBox* enBox = va_arg(args, EnBox*);
                *should = enBox->dyna.actor.scale.x != 0.005f;
            }
            break;
        }
        case VB_SKIP_SCARECROWS_SONG: {
            if (gPlayState->msgCtx.msgMode == MSGMODE_OCARINA_PLAYING && CVarGetInteger(CVAR_ENHANCEMENT("InstantScarecrow"), 0) && gSaveContext.scarecrowSpawnSongSet) {
                *should = true;
            }
            break;
        }
    }

    va_end(args);
}

static uint32_t enMa1UpdateHook = 0;
static uint32_t enMa1KillHook = 0;
static uint32_t enFuUpdateHook = 0;
static uint32_t enFuKillHook = 0;
static uint32_t bgSpot02UpdateHook = 0;
static uint32_t bgSpot02KillHook = 0;
static uint32_t enPoSistersUpdateHook = 0;
static uint32_t enPoSistersKillHook = 0;
void TimeSaverOnActorInitHandler(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (actor->id == ACTOR_EN_MA1 && gPlayState->sceneNum == SCENE_LON_LON_RANCH) {
        enMa1UpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) mutable {
            Actor* innerActor = static_cast<Actor*>(innerActorRef);
            if (innerActor->id == ACTOR_EN_MA1 && (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO)) {
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

    if (actor->id == ACTOR_EN_FU) {
        enFuUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) mutable {
            Actor* innerActor = static_cast<Actor*>(innerActorRef);
            if (innerActor->id == ACTOR_EN_FU && (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) || IS_RANDO)) {
                EnFu* enFu = static_cast<EnFu*>(innerActorRef);
                if (enFu->actionFunc == EnFu_TeachSong) {
                    enFu->actionFunc = EnFu_EndTeachSong;
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enFuUpdateHook);
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enFuKillHook);
                    enFuUpdateHook = 0;
                    enFuKillHook = 0;
                }
            }
        });
        enFuKillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) mutable {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enFuUpdateHook);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enFuKillHook);
            enFuUpdateHook = 0;
            enFuKillHook = 0;
        });
    }

    if (actor->id == ACTOR_BG_SPOT02_OBJECTS && actor->params == 2) {
        bgSpot02UpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) mutable {
            Actor* innerActor = static_cast<Actor*>(innerActorRef);
            if (innerActor->id == ACTOR_BG_SPOT02_OBJECTS && innerActor->params == 2 && (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO))) {
                BgSpot02Objects* bgSpot02 = static_cast<BgSpot02Objects*>(innerActorRef);
                if (bgSpot02->actionFunc == func_808ACC34) {
                    bgSpot02->actionFunc = func_808AC908;
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(bgSpot02UpdateHook);
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(bgSpot02KillHook);
                    bgSpot02UpdateHook = 0;
                    bgSpot02KillHook = 0;
                }
            }
        });
        bgSpot02KillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) mutable {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(bgSpot02UpdateHook);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(bgSpot02KillHook);
            bgSpot02UpdateHook = 0;
            bgSpot02KillHook = 0;
        });
    }

    if (actor->id == ACTOR_EN_DNT_DEMO && (IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO))) {
        EnDntDemo* enDntDemo = static_cast<EnDntDemo*>(actorRef);
        enDntDemo->actionFunc = EnDntDemo_JudgeSkipToReward;
    }

    // Forest Temple entrance cutscene
    // This is a bit of a hack, we can't effectively override the behavior of the torches 
    // or poes from which the cutscene is triggered until we can have a "BeforeActorInit" hook.
    // So for now we're just going to set the flag before they get to the room the cutscene is in
    if (gPlayState->sceneNum == SCENE_FOREST_TEMPLE && actor->id == ACTOR_EN_ST && !Flags_GetSwitch(gPlayState, 0x1B)) {
        if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.GlitchAiding"), 0)) {
            Flags_SetSwitch(gPlayState, 0x1B);
        }
    }

    // Forest Temple purple poe fight speedup
    if (actor->id == ACTOR_EN_PO_SISTERS && actor->params == 28) {
        enPoSistersUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) mutable {
            Actor* innerActor = static_cast<Actor*>(innerActorRef);
            if (innerActor->id == ACTOR_EN_PO_SISTERS && innerActor->params == 28 && (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO))) {
                EnPoSisters* enPoSisters = static_cast<EnPoSisters*>(innerActorRef);
                if (enPoSisters->actionFunc == func_80ADB338) {
                    enPoSisters->unk_19C = 0;
                }
            }
        });
        enPoSistersKillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) mutable {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enPoSistersUpdateHook);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enPoSistersKillHook);
            enPoSistersUpdateHook = 0;
            enPoSistersKillHook = 0;
        });
    }

    // Fire Temple Darunia cutscene
    if (actor->id == ACTOR_EN_DU && gPlayState->sceneNum == SCENE_FIRE_TEMPLE) {
        if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.GlitchAiding"), 0)) {
            Flags_SetInfTable(INFTABLE_SPOKE_TO_DARUNIA_IN_FIRE_TEMPLE);
            Actor_Kill(actor);
        }
    }
}

void TimeSaverOnSceneInitHandler(int16_t sceneNum) {
    switch (sceneNum) {
        case SCENE_HYRULE_CASTLE:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO) && !Flags_GetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE)) {
                Flags_SetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE);
                Flags_SetInfTable(INFTABLE_MET_CHILD_MALON_AT_CASTLE_OR_MARKET);
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET);
            }
            break;
        case SCENE_LON_LON_RANCH:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO) && GameInteractor_Should(VB_MALON_RETURN_FROM_CASTLE, Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE))) {
                Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH);
                Flags_SetInfTable(INFTABLE_CHILD_MALON_SAID_EPONA_WAS_AFRAID_OF_YOU);
                Flags_SetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
            }
            break;
        case SCENE_DEKU_TREE_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_GOHMA_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_GOHMA_BATTLE);
                }
            }
            break;
        case SCENE_DODONGOS_CAVERN_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE);
                }
            }
            break;
        case SCENE_JABU_JABU_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE);
                }
            }
            break;
        case SCENE_FOREST_TEMPLE_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_PHANTOM_GANON_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_PHANTOM_GANON_BATTLE);
                }
            }
            break;
        case SCENE_FIRE_TEMPLE_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE);
                }
            }
            break;
        case SCENE_WATER_TEMPLE_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_MORPHA_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_MORPHA_BATTLE);
                }
            }
            break;
        case SCENE_SPIRIT_TEMPLE_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_TWINROVA_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_TWINROVA_BATTLE);
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_NABOORU_BATTLE);
                    Flags_SetEventChkInf(EVENTCHKINF_NABOORU_ORDERED_TO_FIGHT_BY_TWINROVA);
                }
            }
            break;
        case SCENE_SHADOW_TEMPLE_BOSS:
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)) {
                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE)) {
                    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE);
                }
            }
            break;
    }
}

static GetItemEntry vanillaQueuedItemEntry = GET_ITEM_NONE;

void TimeSaverOnFlagSetHandler(int16_t flagType, int16_t flag) {
    if (!CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) return;

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
                case EVENTCHKINF_LEARNED_SONG_OF_STORMS:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_SONG_OF_STORMS).GetGIEntry_Copy();
                    break;
                case EVENTCHKINF_LEARNED_SUNS_SONG:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_SUNS_SONG).GetGIEntry_Copy();
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
                case RAND_INF_ZF_GREAT_FAIRY_REWARD:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_FARORES_WIND).GetGIEntry_Copy();
                    break;
                case RAND_INF_HC_GREAT_FAIRY_REWARD:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_DINS_FIRE).GetGIEntry_Copy();
                    break;
                case RAND_INF_COLOSSUS_GREAT_FAIRY_REWARD:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_NAYRUS_LOVE).GetGIEntry_Copy();
                    break;
                case RAND_INF_DMT_GREAT_FAIRY_REWARD:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_MAGIC_SINGLE).GetGIEntry_Copy();
                    break;
                case RAND_INF_DMC_GREAT_FAIRY_REWARD:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_MAGIC_DOUBLE).GetGIEntry_Copy();
                    break;
                case RAND_INF_OGC_GREAT_FAIRY_REWARD:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_DOUBLE_DEFENSE).GetGIEntry_Copy();
                    break;
            }
            break;
        case FLAG_ITEM_GET_INF:
            switch (flag) {
                case ITEMGETINF_OBTAINED_STICK_UPGRADE_FROM_STAGE:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_DEKU_STICK_CAPACITY_30).GetGIEntry_Copy();
                    break;
                case ITEMGETINF_OBTAINED_NUT_UPGRADE_FROM_STAGE:
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_DEKU_NUT_CAPACITY_40).GetGIEntry_Copy();
                    break;
            }
            break;
    }
}

void TimeSaverOnPlayerUpdateHandler() {
    if (vanillaQueuedItemEntry.itemId == ITEM_NONE) return;

    Player* player = GET_PLAYER(gPlayState);
    if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        return;
    }

    SPDLOG_INFO("Attempting to give Item: mod {} item {}", vanillaQueuedItemEntry.modIndex, vanillaQueuedItemEntry.itemId);
    GiveItemEntryWithoutActor(gPlayState, vanillaQueuedItemEntry);
    if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        // Allow the player to receive the item while swimming
        player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        Player_ActionHandler_2(player, gPlayState);
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

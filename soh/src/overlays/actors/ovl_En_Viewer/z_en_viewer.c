/*
 * File: z_en_viewer.c
 * Overlay: ovl_En_Viewer
 * Description: Cutscene Actors
 */

#include "z_en_viewer.h"
#include "overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.h"
#include "objects/object_zl4/object_zl4.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_horse_zelda/object_horse_zelda.h"
#include "objects/object_horse_ganon/object_horse_ganon.h"
#include "objects/object_im/object_im.h"
#include "objects/object_gndd/object_gndd.h"
#include "objects/object_ganon/object_ganon.h"
#include "objects/object_opening_demo1/object_opening_demo1.h"
#include "soh/frame_interpolation.h"

#define FLAGS ACTOR_FLAG_4

void EnViewer_Init(Actor* thisx, PlayState* play);
void EnViewer_Destroy(Actor* thisx, PlayState* play);
void EnViewer_Update(Actor* thisx, PlayState* play);
void EnViewer_Draw(Actor* thisx, PlayState* play);

void EnViewer_UpdatePosition(EnViewer* this, PlayState* play);
void EnViewer_DrawFireEffects(EnViewer* this2, PlayState* play);
void EnViewer_UpdateGanondorfCape(PlayState* play, EnViewer* this);
void EnViewer_InitImpl(EnViewer* this, PlayState* play);
void EnViewer_UpdateImpl(EnViewer* this, PlayState* play);

static u8 sHorseSfxPlayed = false;

const ActorInit En_Viewer_InitVars = {
    ACTOR_EN_VIEWER,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnViewer),
    (ActorFunc)EnViewer_Init,
    (ActorFunc)EnViewer_Destroy,
    (ActorFunc)EnViewer_Update,
    (ActorFunc)EnViewer_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_STOP),
};

static EnViewerInitData sInitData[] = {
    /* ENVIEWER_TYPE_0_HORSE_ZELDA */
    { OBJECT_HORSE_ZELDA, OBJECT_HORSE_ZELDA, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseZeldaSkel,
      &gHorseZeldaGallopingAnim },
    /* ENVIEWER_TYPE_1_IMPA */
    { OBJECT_IM, OBJECT_OPENING_DEMO1, 1, 0, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_IMPA, &gImpaSkel,
      &object_opening_demo1_Anim_0029CC },
    /* ENVIEWER_TYPE_2_ZELDA */
    { OBJECT_ZL4, OBJECT_OPENING_DEMO1, 1, 0, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_ZELDA, &gChildZeldaSkel,
      &object_opening_demo1_Anim_000450 },
    /* ENVIEWER_TYPE_3_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfHorsebackIdleAnim },
    /* ENVIEWER_TYPE_4_HORSE_GANONDORF */
    { OBJECT_HORSE_GANON, OBJECT_HORSE_GANON, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseGanonSkel,
      &gHorseGanonRearingAnim },
    /* ENVIEWER_TYPE_5_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfHorsebackRideAnim },
    /* ENVIEWER_TYPE_6_HORSE_GANONDORF */
    { OBJECT_HORSE_GANON, OBJECT_HORSE_GANON, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseGanonSkel,
      &gHorseGanonGallopingAnim },
    /* ENVIEWER_TYPE_7_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfArmsCrossedAnim },
    /* ENVIEWER_TYPE_8_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfWalkAnim },
    /* ENVIEWER_TYPE_9_GANONDORF */
    { OBJECT_GANON, OBJECT_GANON, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gGanondorfSkel,
      &gGanondorfEndingFloatAnim },
};

static EnGanonMant* sGanondorfCape;

static Vec3f sGanondorfNeckWorldPos;

void EnViewer_SetupAction(EnViewer* this, EnViewerActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnViewer_Init(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;
    u8 type;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnViewer_SetupAction(this, EnViewer_InitImpl);
    sHorseSfxPlayed = false;
    type = this->actor.params >> 8;
    this->unused = 0;
    this->state = 0;
    this->isVisible = false;
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF ||
        type == ENVIEWER_TYPE_8_GANONDORF || type == ENVIEWER_TYPE_9_GANONDORF) {
        sGanondorfCape = (EnGanonMant*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play,
                                                          ACTOR_EN_GANON_MANT, 0.0f, 0.0f, 0.0f, 0, 0, 0, 35);
    }
}

void EnViewer_Destroy(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;

    Skin_Free(play, &this->skin);
}

void EnViewer_InitAnimGanondorfOrZelda(EnViewer* this, PlayState* play, void* skeletonHeaderSeg,
                                       AnimationHeader* anim) {
    s16 type = this->actor.params >> 8;

    if (type == ENVIEWER_TYPE_2_ZELDA || type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF ||
        type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF || type == ENVIEWER_TYPE_9_GANONDORF) {
        SkelAnime_InitFlex(play, &this->skin.skelAnime, skeletonHeaderSeg, NULL, NULL, NULL, 0);
    } else {
        SkelAnime_Init(play, &this->skin.skelAnime, skeletonHeaderSeg, NULL, NULL, NULL, 0);
    }

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->animObjBankIndex].segment);
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF ||
        type == ENVIEWER_TYPE_9_GANONDORF) {
        Animation_PlayLoopSetSpeed(&this->skin.skelAnime, anim, 1.0f);
    } else {
        Animation_PlayLoopSetSpeed(&this->skin.skelAnime, anim, 3.0f);
    }
}

void EnViewer_InitAnimImpa(EnViewer* this, PlayState* play, void* skeletonHeaderSeg, AnimationHeader* anim) {
    SkelAnime_InitFlex(play, &this->skin.skelAnime, skeletonHeaderSeg, NULL, NULL, NULL, 0);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->animObjBankIndex].segment);
    Animation_PlayLoopSetSpeed(&this->skin.skelAnime, anim, 3.0f);
}

void EnViewer_InitAnimHorse(EnViewer* this, PlayState* play, void* skeletonHeaderSeg, AnimationHeader* anim) {
    u8 type;

    Skin_Init(play, &this->skin, skeletonHeaderSeg, anim);
    type = this->actor.params >> 8;
    if (!(type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_4_HORSE_GANONDORF ||
          type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF ||
          type == ENVIEWER_TYPE_9_GANONDORF)) {
        Animation_PlayLoopSetSpeed(&this->skin.skelAnime, anim, 3.0f);
    } else {
        Animation_PlayOnceSetSpeed(&this->skin.skelAnime, anim, 1.0f);
    }
}

static EnViewerInitAnimFunc sInitAnimFuncs[] = {
    EnViewer_InitAnimGanondorfOrZelda,
    EnViewer_InitAnimHorse,
    EnViewer_InitAnimGanondorfOrZelda,
    EnViewer_InitAnimImpa,
};

static ActorShadowFunc sShadowDrawFuncs[] = {
    NULL,
    ActorShadow_DrawCircle,
    ActorShadow_DrawHorse,
};

void EnViewer_InitImpl(EnViewer* this, PlayState* play) {
    EnViewerInitData* initData = &sInitData[this->actor.params >> 8];
    s32 skelObjBankIndex = Object_GetIndex(&play->objectCtx, initData->skeletonObject);

    ASSERT(skelObjBankIndex >= 0);

    this->animObjBankIndex = Object_GetIndex(&play->objectCtx, initData->animObject);
    ASSERT(this->animObjBankIndex >= 0);

    if (!Object_IsLoaded(&play->objectCtx, skelObjBankIndex) ||
        !Object_IsLoaded(&play->objectCtx, this->animObjBankIndex)) {
        this->actor.flags &= ~ACTOR_FLAG_6;
        return;
    }

    this->isVisible = true;
    this->actor.objBankIndex = skelObjBankIndex;
    Actor_SetObjectDependency(play, &this->actor);
    Actor_SetScale(&this->actor, initData->scale / 100.0f);
    ActorShape_Init(&this->actor.shape, initData->yOffset * 100, sShadowDrawFuncs[initData->shadowType],
                    initData->shadowScale);
    this->drawFuncIndex = initData->drawType;
    sInitAnimFuncs[this->drawFuncIndex](this, play, initData->skeletonHeaderSeg, initData->anim);
    EnViewer_SetupAction(this, EnViewer_UpdateImpl);
}

static s16 sTimer = 0;

void EnViewer_UpdateImpl(EnViewer* this, PlayState* play) {
    u8 type = this->actor.params >> 8;
    u16 csFrames;
    s32 animationEnded;

    if (type == ENVIEWER_TYPE_2_ZELDA) {
        if (gSaveContext.sceneSetupIndex == 5) {
            csFrames = play->csCtx.frames;
            if (csFrames == 792) {
                Audio_PlayActorSound2(&this->actor, NA_SE_VO_Z0_SURPRISE);
            } else if (csFrames == 845) {
                Audio_PlayActorSound2(&this->actor, NA_SE_VO_Z0_THROW);
            }
        }
    } else if (type == ENVIEWER_TYPE_7_GANONDORF) {
        Actor_SetScale(&this->actor, 0.3f);
        this->actor.uncullZoneForward = 10000.0f;
        this->actor.uncullZoneScale = 10000.0f;
        this->actor.uncullZoneDownward = 10000.0f;
    } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
        if (gSaveContext.sceneSetupIndex == 4) {
            switch (play->csCtx.frames) {
                case 20:
                case 59:
                case 71:
                case 129:
                case 140:
                case 219:
                case 280:
                case 320:
                case 380:
                case 409:
                case 438:
                    Audio_PlaySoundGeneral(NA_SE_SY_DEMO_CUT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    break;
            }
        }
        if (gSaveContext.sceneSetupIndex == 5) {
            if (play->csCtx.frames == 1508) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_ST_LAUGH);
            }
            if (play->csCtx.frames == 1545) {
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, 32.0f, 101.0f, 1226.0f,
                                   0, 0, 0, 0xC);
            }
        }
        if (play->csCtx.frames == 1020) {
            Audio_QueueSeqCmd(SEQ_PLAYER_FANFARE << 24 | NA_BGM_OPENING_GANON);
        }
        if (play->csCtx.frames == 960) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    } else if (type == ENVIEWER_TYPE_6_HORSE_GANONDORF) {
        if (gSaveContext.sceneSetupIndex == 5 || gSaveContext.sceneSetupIndex == 10) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
        }
    } else if (type == ENVIEWER_TYPE_4_HORSE_GANONDORF) {
        s16 curFrame = this->skin.skelAnime.curFrame;

        if (this->skin.skelAnime.animation == &gHorseGanonRearingAnim) {
            if (curFrame == 8) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_GANON_HORSE_NEIGH);
            }
            if (curFrame == 30) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_HORSE_LAND2);
            }
        } else if (this->skin.skelAnime.animation == &gHorseGanonIdleAnim) {
            if (curFrame == 25) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_HORSE_SANDDUST);
            }
        } else if (this->skin.skelAnime.animation == &gHorseGanonGallopingAnim) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
        }
    }

    if (sTimer != 0) {
        sTimer--;
    }

    EnViewer_UpdatePosition(this, play);
    Actor_MoveForward(&this->actor); // has no effect, speed/velocity and gravity are 0

    animationEnded = SkelAnime_Update(&this->skin.skelAnime);
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_4_HORSE_GANONDORF) {
        if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[1] != NULL) {
            if (play->csCtx.npcActions[1]->action == 2 && sTimer == 0) {
                if (type == ENVIEWER_TYPE_3_GANONDORF) {
                    if (this->skin.skelAnime.animation != &gYoungGanondorfHorsebackIdleAnim) {
                        Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &gYoungGanondorfHorsebackIdleAnim, 1.0f);
                    }
                } else if (this->skin.skelAnime.animation != &gHorseGanonIdleAnim) {
                    Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &gHorseGanonIdleAnim, 1.0f);
                }
            } else if (play->csCtx.npcActions[1]->action == 1) {
                sTimer = 100;
                if (type == ENVIEWER_TYPE_3_GANONDORF) {
                    if (this->skin.skelAnime.animation != &gYoungGanondorfHorsebackRearAnim) {
                        Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &gYoungGanondorfHorsebackRearAnim, 1.0f);
                    }
                } else if (this->skin.skelAnime.animation != &gHorseGanonRearingAnim) {
                    Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &gHorseGanonRearingAnim, 1.0f);
                }
            } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
                switch (this->state) {
                    case 0:
                        if (play->csCtx.npcActions[1]->action == 4) {
                            Animation_MorphToPlayOnce(&this->skin.skelAnime, &gYoungGanondorfHorsebackLookSidewaysStartAnim, -5.0f);
                            this->state++;
                        }
                        break;
                    case 1:
                        if (animationEnded) {
                            Animation_MorphToLoop(&this->skin.skelAnime, &gYoungGanondorfHorsebackLookSidewaysLoopAnim, -5.0f);
                            this->state++;
                        }
                        break;
                    case 2:
                        if (play->csCtx.npcActions[1]->action == 5) {
                            Animation_MorphToPlayOnce(&this->skin.skelAnime, &gYoungGanondorfHorsebackMagicChargeUpStartAnim, -5.0f);
                            this->state++;
                        }
                        break;
                    case 3:
                        if (animationEnded) {
                            Animation_MorphToLoop(&this->skin.skelAnime, &gYoungGanondorfHorsebackMagicChargeUpLoopAnim, -5.0f);
                            this->state++;
                        }
                        break;
                    case 4:
                        if (play->csCtx.npcActions[1]->action == 11) {
                            Animation_MorphToLoop(&this->skin.skelAnime, &gYoungGanondorfHorsebackLookSidewaysLoopAnim, -20.0f);
                            this->state++;
                        }
                        break;
                    case 5:
                        if (play->csCtx.npcActions[1]->action == 8) {
                            Animation_MorphToLoop(&this->skin.skelAnime, &gYoungGanondorfHorsebackIdleAnim, -15.0f);
                            this->state++;
                        }
                        break;
                    case 6:
                        if (play->csCtx.npcActions[1]->action == 12) {
                            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_VOICE_DEMO);
                            Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &gYoungGanondorfHorsebackRideAnim, 3.0f);
                            this->state++;
                        }
                        break;
                    case 7:
                        this->state = 0;
                        break;
                }
            } else if (this->skin.skelAnime.animation != &gHorseGanonGallopingAnim &&
                       play->csCtx.npcActions[1]->action == 12) {
                Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &gHorseGanonGallopingAnim, 3.0f);
            }
        }
    } else if (type == ENVIEWER_TYPE_1_IMPA) {
        if (gSaveContext.sceneSetupIndex == 5) {
            if (play->csCtx.frames == 845) {
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_ITEM_OCARINA, 4.0f, 81.0f,
                                   2600.0f, 0, 0, 0, 0);
            }
        } else {
            if (play->csCtx.frames == 195) {
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_ITEM_OCARINA, 4.0f, 81.0f,
                                   2035.0f, 0, 0, 0, 1);
            }
        }
        switch (this->state) {
            case 0:
                if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[0] != NULL &&
                    play->csCtx.npcActions[0]->action == 6 &&
                    this->skin.skelAnime.animation != &object_opening_demo1_Anim_002574) {
                    Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &object_opening_demo1_Anim_002574, 1.5f);
                    this->state++;
                }
                break;
            case 1:
                if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[0] != NULL &&
                    play->csCtx.npcActions[0]->action == 2 &&
                    this->skin.skelAnime.animation != &object_opening_demo1_Anim_0029CC) {
                    Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &object_opening_demo1_Anim_0029CC, 3.0f);
                    this->state++;
                }
                break;
        }
    } else if (type == ENVIEWER_TYPE_2_ZELDA) {
        if (play->sceneNum == SCENE_SPOT00) { // Hyrule Field
            switch (this->state) {
                case 0:
                    if (play->csCtx.state != CS_STATE_IDLE) {
                        if (play->csCtx.npcActions[0] != NULL && play->csCtx.npcActions[0]->action == 6 &&
                            this->skin.skelAnime.animation != &object_opening_demo1_Anim_001410) {
                            Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &object_opening_demo1_Anim_001410, 1.5f);
                            this->state++;
                        }
                    }
                    break;
                case 1:
                    if (play->csCtx.state != CS_STATE_IDLE) {
                        if (play->csCtx.npcActions[0] != NULL && play->csCtx.npcActions[0]->action == 2 &&
                            this->skin.skelAnime.animation != &object_opening_demo1_Anim_000450) {
                            Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &object_opening_demo1_Anim_000450, 3.0f);
                            this->state++;
                        }
                    }
                    break;
            }
        } else {
            Audio_SetBaseFilter(0);
            switch (this->state) {
                case 0:
                    Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &object_opening_demo1_Anim_00504C, 1.0f);
                    this->state++;
                    break;
                case 1:
                    if (play->csCtx.npcActions[0]->action == 11) {
                        Animation_MorphToPlayOnce(&this->skin.skelAnime, &object_opening_demo1_Anim_00420C, -5.0f);
                        this->state++;
                    }
                    break;
                case 2:
                    if (animationEnded) {
                        Animation_MorphToLoop(&this->skin.skelAnime, &object_opening_demo1_Anim_0048FC, -5.0f);
                        this->state++;
                    }
                    break;
                case 3:
                    break;
            }
        }
    } else if (type == ENVIEWER_TYPE_7_GANONDORF) {
        switch (this->state) {
            case 0:
                if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[1] != NULL &&
                    play->csCtx.npcActions[1]->action == 7) {
                    Audio_PlaySoundGeneral(NA_SE_EN_GANON_LAUGH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    Animation_MorphToPlayOnce(&this->skin.skelAnime, &gYoungGanondorfLaughStartAnim, -5.0f);
                    this->state++;
                }
                break;
            case 1:
                if (animationEnded) {
                    Animation_MorphToLoop(&this->skin.skelAnime, &gYoungGanondorfLaughLoopAnim, -5.0f);
                    this->state++;
                }
                break;
        }
    } else if (type == ENVIEWER_TYPE_8_GANONDORF) {
        switch (this->state) {
            case 0:
                if (play->csCtx.state != CS_STATE_IDLE) {
                    if (play->csCtx.npcActions[1] != NULL && play->csCtx.npcActions[1]->action == 9) {
                        Animation_PlayLoopSetSpeed(&this->skin.skelAnime, &gYoungGanondorfWalkAnim, 1.0f);
                        this->state++;
                    }
                }
                break;
            case 1:
                if (play->csCtx.npcActions[1]->action == 10) {
                    Animation_MorphToPlayOnce(&this->skin.skelAnime, &gYoungGanondorfKneelStartAnim, -10.0f);
                    this->state++;
                }
                break;
            case 2:
                if (animationEnded) {
                    Animation_MorphToLoop(&this->skin.skelAnime, &gYoungGanondorfKneelLoopAnim, -5.0f);
                    this->state++;
                }
                break;
            case 3:
                if (play->csCtx.npcActions[1]->action == 4) {
                    Animation_MorphToPlayOnce(&this->skin.skelAnime, &gYoungGanondorfKneelLookSidewaysAnim, -5.0f);
                    this->state++;
                }
                break;
            default:
                this->state = 0;
                break;
        }
    }
}

void EnViewer_Update(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->animObjBankIndex].segment);
    this->actionFunc(this, play);
}

s32 EnViewer_Ganondorf3OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        void* thisx) {
    if (gSaveContext.sceneSetupIndex == 4) {
        if (play->csCtx.frames >= 400) {
            if (limbIndex == 5) {
                *dList = gYoungGanondorfOpenLeftHandDL;
            }
        }
    } else {
        if (play->csCtx.frames >= 1510 && play->csCtx.frames <= 1650) {
            if (limbIndex == 5) {
                *dList = gYoungGanondorfOpenLeftHandDL;
            }
        }
    }
    return false;
}

void EnViewer_Ganondorf9PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 11) {
        OPEN_DISPS(play->state.gfxCtx);
        func_80093D84(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfEyesDL));
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnViewer_GanondorfPostLimbDrawUpdateCapeVec(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot,
                                                 void* thisx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 15) {
        Matrix_MultVec3f(&zeroVec, &sGanondorfNeckWorldPos);
    }
}

void EnViewer_DrawGanondorf(EnViewer* this, PlayState* play) {
    s16 frames = 0;
    s16 type;

    OPEN_DISPS(play->state.gfxCtx);
    type = this->actor.params >> 8;
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF ||
        type == ENVIEWER_TYPE_8_GANONDORF) {
        if (gSaveContext.sceneSetupIndex != 4) {
            frames = 149;
        }

        if (frames + 1127 >= play->csCtx.frames) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeOpenTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeOpenTex));

        } else if (frames + 1128 >= play->csCtx.frames) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeHalfTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeHalfTex));

        } else if (frames + 1129 >= play->csCtx.frames) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeClosedTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeClosedTex));

        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeLookingDownTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&gYoungGanondorfEyeLookingDownTex));
        }
    } else if (type == ENVIEWER_TYPE_9_GANONDORF) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&gGanondorfCrazedEyeTex));
    }

    if (type == ENVIEWER_TYPE_9_GANONDORF) {
        SkelAnime_DrawFlexOpa(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                              this->skin.skelAnime.dListCount, NULL, EnViewer_Ganondorf9PostLimbDraw, this);
    } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
        SkelAnime_DrawFlexOpa(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                              this->skin.skelAnime.dListCount, EnViewer_Ganondorf3OverrideLimbDraw,
                              EnViewer_GanondorfPostLimbDrawUpdateCapeVec, this);
        EnViewer_UpdateGanondorfCape(play, this);
    } else if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF ||
               type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF) {
        if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.npcActions[1] != NULL)) {
            SkelAnime_DrawFlexOpa(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                                  this->skin.skelAnime.dListCount, NULL, EnViewer_GanondorfPostLimbDrawUpdateCapeVec,
                                  this);
            EnViewer_UpdateGanondorfCape(play, this);
        }
    } else {
        SkelAnime_DrawOpa(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable, NULL, NULL, this);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnViewer_DrawHorse(EnViewer* this, PlayState* play) {
    func_800A6330(&this->actor, play, &this->skin, NULL, true);
}

s32 EnViewer_ZeldaOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                   void* thisx) {
    if (play->sceneNum == SCENE_SPOT00) { // Hyrule Field
        if (limbIndex == 2) {
            *dList = gChildZeldaCutsceneDressDL;
        }
        if (limbIndex == 7) {
            *dList = NULL;
        }
        if (limbIndex == 8) {
            *dList = NULL;
        }
        if (limbIndex == 9) {
            *dList = NULL;
        }
        if (limbIndex == 3) {
            *dList = NULL;
        }
        if (limbIndex == 5) {
            *dList = NULL;
        }
    }
    return false;
}

void EnViewer_ZeldaPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s32 pad;

    if (play->sceneNum == SCENE_TOKINOMA) {
        if (limbIndex == 16) {
            OPEN_DISPS(play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gChildZeldaOcarinaOfTimeDL);
            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void EnViewer_DrawZelda(EnViewer* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    if (play->sceneNum == SCENE_SPOT00) { // Hyrule Field
        if (play->csCtx.frames < 771) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeInTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeOutTex));
        } else if (play->csCtx.frames < 772) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
        } else if (play->csCtx.frames < 773) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        } else if (play->csCtx.frames < 791) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeWideTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeWideTex));
        } else if (play->csCtx.frames < 792) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
        } else if (play->csCtx.frames < 793) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeInTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeOutTex));
        }

        if (gSaveContext.sceneSetupIndex == 6) {
            gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthSurprisedTex));
        } else {
            if (play->csCtx.frames < 758) {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
            } else if (play->csCtx.frames < 848) {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthSurprisedTex));
            } else {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
            }
        }
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
    }
    SkelAnime_DrawFlexOpa(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                          this->skin.skelAnime.dListCount, EnViewer_ZeldaOverrideLimbDraw, EnViewer_ZeldaPostLimbDraw,
                          this);
    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnViewer_ImpaOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    if (limbIndex == 16) {
        *dList = gImpaHeadMaskedDL;
    }
    return false;
}

void EnViewer_DrawImpa(EnViewer* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gImpaEyeOpenTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gImpaEyeOpenTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                          this->skin.skelAnime.dListCount, EnViewer_ImpaOverrideLimbDraw, NULL, this);
    CLOSE_DISPS(play->state.gfxCtx);
}

static EnViewerDrawFunc sDrawFuncs[] = {
    EnViewer_DrawGanondorf,
    EnViewer_DrawHorse,
    EnViewer_DrawZelda,
    EnViewer_DrawImpa,
};

void EnViewer_Draw(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;
    s32 pad;
    s16 type;

    OPEN_DISPS(play->state.gfxCtx);
    if (this->isVisible) {
        type = this->actor.params >> 8;
        if (type <= ENVIEWER_TYPE_2_ZELDA) { // zelda's horse, impa and zelda
            if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[0] != NULL) {
                func_80093D18(play->state.gfxCtx);
                sDrawFuncs[this->drawFuncIndex](this, play);
            }
        } else if ((play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[1] != NULL) ||
                   type == ENVIEWER_TYPE_9_GANONDORF) {
            func_80093D18(play->state.gfxCtx);
            sDrawFuncs[this->drawFuncIndex](this, play);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnViewer_UpdatePosition(EnViewer* this, PlayState* play) {
    Vec3f startPos;
    Vec3f endPos;
    f32 lerpFactor;
    s16 type = this->actor.params >> 8;

    if (type <= ENVIEWER_TYPE_2_ZELDA) { // zelda's horse, impa and zelda
        if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[0] != NULL &&
            play->csCtx.frames < play->csCtx.npcActions[0]->endFrame) {
            if (type == ENVIEWER_TYPE_0_HORSE_ZELDA) {
                if (!sHorseSfxPlayed) {
                    sHorseSfxPlayed = true;
                    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                }
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
            }

            startPos.x = play->csCtx.npcActions[0]->startPos.x;
            startPos.y = play->csCtx.npcActions[0]->startPos.y;
            startPos.z = play->csCtx.npcActions[0]->startPos.z;
            endPos.x = play->csCtx.npcActions[0]->endPos.x;
            endPos.y = play->csCtx.npcActions[0]->endPos.y;
            endPos.z = play->csCtx.npcActions[0]->endPos.z;
            lerpFactor = Environment_LerpWeight(play->csCtx.npcActions[0]->endFrame,
                                                play->csCtx.npcActions[0]->startFrame, play->csCtx.frames);
            this->actor.world.pos.x = (endPos.x - startPos.x) * lerpFactor + startPos.x;
            this->actor.world.pos.y = (endPos.y - startPos.y) * lerpFactor + startPos.y;
            this->actor.world.pos.z = (endPos.z - startPos.z) * lerpFactor + startPos.z;
        }
    } else { // ganondorf and ganondorf's horse
        if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.npcActions[1] != NULL &&
            play->csCtx.frames < play->csCtx.npcActions[1]->endFrame) {
            startPos.x = play->csCtx.npcActions[1]->startPos.x;
            startPos.y = play->csCtx.npcActions[1]->startPos.y;
            startPos.z = play->csCtx.npcActions[1]->startPos.z;
            endPos.x = play->csCtx.npcActions[1]->endPos.x;
            endPos.y = play->csCtx.npcActions[1]->endPos.y;
            endPos.z = play->csCtx.npcActions[1]->endPos.z;
            lerpFactor = Environment_LerpWeight(play->csCtx.npcActions[1]->endFrame,
                                                play->csCtx.npcActions[1]->startFrame, play->csCtx.frames);
            this->actor.world.pos.x = (endPos.x - startPos.x) * lerpFactor + startPos.x;
            this->actor.world.pos.y = (endPos.y - startPos.y) * lerpFactor + startPos.y;
            this->actor.world.pos.z = (endPos.z - startPos.z) * lerpFactor + startPos.z;

            if (play->csCtx.npcActions[1]->action == 12) {
                s16 yaw = Math_Vec3f_Yaw(&startPos, &endPos);

                Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 0xA, 0x3E8, 1);
                Math_SmoothStepToS(&this->actor.shape.rot.y, yaw, 0xA, 0x3E8, 1);
            }

            if (type == ENVIEWER_TYPE_9_GANONDORF) {
                this->actor.world.rot.x = play->csCtx.npcActions[1]->urot.x;
                this->actor.world.rot.y = play->csCtx.npcActions[1]->urot.y;
                this->actor.world.rot.z = play->csCtx.npcActions[1]->urot.z;
                this->actor.shape.rot.x = play->csCtx.npcActions[1]->urot.x;
                this->actor.shape.rot.y = play->csCtx.npcActions[1]->urot.y;
                this->actor.shape.rot.z = play->csCtx.npcActions[1]->urot.z;
            }
        }
        if (type == ENVIEWER_TYPE_5_GANONDORF) {
            Audio_PlaySoundGeneral(NA_SE_EV_BURNING - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            EnViewer_DrawFireEffects(this, play);
        }
    }
}

void EnViewer_InitFireEffect(EnViewer* this, PlayState* play, s16 i) {
    EnViewerFireEffect* eff;

    if ((i % 2) == 0) {
        eff = &this->fireEffects[i];
        eff->startPos.x = 100.0f;
        eff->startPos.y = -420.0f;
        eff->startPos.z = 400.0f;
        eff->endPos.x = 100.0f;
        eff->endPos.y = -420.0f;
        eff->endPos.z = -400.0f;
        eff->scale = (Rand_ZeroOne() * 5.0f + 12.0f) * 0.001f;
        eff->epoch++;
    } else {
        eff = &this->fireEffects[i];
        eff->startPos.x = -100.0f;
        eff->startPos.y = -420.0f;
        eff->startPos.z = 400.0f;
        eff->endPos.x = -100.0f;
        eff->endPos.y = -420.0f;
        eff->endPos.z = -400.0f;
        eff->scale = (Rand_ZeroOne() * 5.0f + 12.0f) * 0.001f;
        eff->epoch++;
    }
    if (this) {}
}

void EnViewer_DrawFireEffects(EnViewer* this2, PlayState* play) {
    EnViewer* this = this2;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->fireEffects); i++) {
        switch (this->fireEffects[i].state) {
            case 0:
                EnViewer_InitFireEffect(this, play, i);
                this->fireEffects[i].lerpFactor = (i >> 1) * 0.1f;
                this->fireEffects[i].lerpFactorSpeed = 0.01f;
                this->fireEffects[i].state++;
                break;
            case 1:
                Math_SmoothStepToF(&this->fireEffects[i].lerpFactor, 1.0f, 1.0f, this->fireEffects[i].lerpFactorSpeed,
                                   this->fireEffects[i].lerpFactorSpeed);
                this->fireEffects[i].pos.x =
                    this->fireEffects[i].startPos.x +
                    (this->fireEffects[i].endPos.x - this->fireEffects[i].startPos.x) * this->fireEffects[i].lerpFactor;
                this->fireEffects[i].pos.y =
                    this->fireEffects[i].startPos.y +
                    (this->fireEffects[i].endPos.y - this->fireEffects[i].startPos.y) * this->fireEffects[i].lerpFactor;
                this->fireEffects[i].pos.z =
                    this->fireEffects[i].startPos.z +
                    (this->fireEffects[i].endPos.z - this->fireEffects[i].startPos.z) * this->fireEffects[i].lerpFactor;
                if (this->fireEffects[i].lerpFactor >= 1.0f) {
                    this->fireEffects[i].state++;
                }
                break;
            case 2:
                EnViewer_InitFireEffect(this, play, i);
                this->fireEffects[i].lerpFactor = 0.0f;
                this->fireEffects[i].lerpFactorSpeed = 0.01f;
                this->fireEffects[i].state--;
                break;
        }

        FrameInterpolation_RecordOpenChild(&this->fireEffects[i], this->fireEffects[i].epoch);
        func_80093D84(play->state.gfxCtx);
        Matrix_Translate(this->fireEffects[i].pos.x, this->fireEffects[i].pos.y, this->fireEffects[i].pos.z,
                         MTXMODE_NEW);
        Matrix_Scale(this->fireEffects[i].scale, this->fireEffects[i].scale, this->fireEffects[i].scale, MTXMODE_APPLY);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                    (10 * i - 20 * play->state.frames) % 512, 32, 128));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 00, 255);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        FrameInterpolation_RecordCloseChild();
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnViewer_UpdateGanondorfCape(PlayState* play, EnViewer* this) {
    static s16 yOscillationPhase = 0;
    Vec3f forearmModelOffset;
    Vec3f forearmWorldOffset;

    if ((this->actor.params >> 8) == ENVIEWER_TYPE_5_GANONDORF) {
        sGanondorfCape->backPush = BREG(54) / 10.0f;
        sGanondorfCape->backSwayMagnitude = (BREG(60) + 25) / 100.0f;
        sGanondorfCape->sideSwayMagnitude = (BREG(55) - 45) / 10.0f;
        sGanondorfCape->minY = -10000.0f;
        sGanondorfCape->minDist = 0.0f;
        sGanondorfCape->gravity = (BREG(67) - 10) / 10.0f;
        forearmModelOffset.x = KREG(16) - 13.0f;
        forearmModelOffset.y = KREG(17) + 3.0f + Math_SinS(yOscillationPhase) * KREG(20);
        forearmModelOffset.z = KREG(18) - 10.0f;
        yOscillationPhase += KREG(19) * 0x1000 + 0x2000;

        Matrix_RotateY((this->actor.shape.rot.y / (f32)0x8000) * M_PI, MTXMODE_NEW);
        Matrix_MultVec3f(&forearmModelOffset, &forearmWorldOffset);
        sGanondorfCape->rightForearmPos.x = sGanondorfNeckWorldPos.x + forearmWorldOffset.x;
        sGanondorfCape->rightForearmPos.y = sGanondorfNeckWorldPos.y + forearmWorldOffset.y;
        sGanondorfCape->rightForearmPos.z = sGanondorfNeckWorldPos.z + forearmWorldOffset.z;
        forearmModelOffset.x = -(KREG(16) - 13.0f);
        Matrix_MultVec3f(&forearmModelOffset, &forearmWorldOffset);
        sGanondorfCape->leftForearmPos.x = sGanondorfNeckWorldPos.x + forearmWorldOffset.x;
        sGanondorfCape->leftForearmPos.y = sGanondorfNeckWorldPos.y + forearmWorldOffset.y;
        sGanondorfCape->leftForearmPos.z = sGanondorfNeckWorldPos.z + forearmWorldOffset.z;
    }
}

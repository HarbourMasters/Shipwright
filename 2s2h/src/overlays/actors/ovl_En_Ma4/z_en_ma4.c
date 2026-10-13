/*
 * File: z_en_ma4.c
 * Overlay: ovl_En_Ma4
 * Description: Romani
 */

#include "z_en_ma4.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnMa4_Init(Actor* thisx, PlayState* play);
void EnMa4_Destroy(Actor* thisx, PlayState* play);
void EnMa4_Update(Actor* thisx, PlayState* play);
void EnMa4_Draw(Actor* thisx, PlayState* play);

void EnMa4_SetupWait(EnMa4* this);
void EnMa4_Wait(EnMa4* this, PlayState* play);
void EnMa4_HandlePlayerChoice(EnMa4* this, PlayState* play);
void EnMa4_ChooseNextDialogue(EnMa4* this, PlayState* play);
void EnMa4_SetupDialogueHandler(EnMa4* this);
void EnMa4_DialogueHandler(EnMa4* this, PlayState* play);
void EnMa4_SetupBeginHorsebackGame(EnMa4* this);
void EnMa4_BeginHorsebackGame(EnMa4* this, PlayState* play);
void EnMa4_HorsebackGameTalking(EnMa4* this, PlayState* play);
void EnMa4_InitHorsebackGame(EnMa4* this, PlayState* play);
void EnMa4_SetupHorsebackGameWait(EnMa4* this, PlayState* play);
void EnMa4_HorsebackGameWait(EnMa4* this, PlayState* play);
void EnMa4_SetupHorsebackGameEnd(EnMa4* this, PlayState* play);
void EnMa4_HorsebackGameEnd(EnMa4* this, PlayState* play);
void EnMa4_SetupBeginEponasSongCs(EnMa4* this);
void EnMa4_BeginEponasSongCs(EnMa4* this, PlayState* play);
void EnMa4_SetupEponasSongCs(EnMa4* this);
void EnMa4_EponasSongCs(EnMa4* this, PlayState* play);
void EnMa4_SetupEndEponasSongCs(EnMa4* this);
void EnMa4_EndEponasSongCs(EnMa4* this, PlayState* play);
void EnMa4_SetupBeginDescribeThemCs(EnMa4* this);
void EnMa4_BeginDescribeThemCs(EnMa4* this, PlayState* play);
void EnMa4_StartDialogue(EnMa4* this, PlayState* play);
void EnMa4_SetFaceExpression(EnMa4* this, s16 overrideEyeTexIndex, s16 mouthTexIndex);
void EnMa4_InitFaceExpression(EnMa4* this);

typedef enum {
    /* 0 */ MA4_TYPE_DAY1,
    /* 1 */ MA4_TYPE_ALIENS_DEFEATED,
    /* 2 */ MA4_TYPE_ALIENS_WON
} EnMa4Type;

typedef enum {
    /* 0 */ MA4_STATE_DEFAULT,
    /* 1 */ MA4_STATE_HORSEBACKGAME,
    /* 2 */ MA4_STATE_AFTERHORSEBACKGAME,
    /* 3 */ MA4_STATE_AFTERDESCRIBETHEMCS
} EnMa4State;

ActorProfile En_Ma4_Profile = {
    /**/ ACTOR_EN_MA4,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MA1,
    /**/ sizeof(EnMa4),
    /**/ EnMa4_Init,
    /**/ EnMa4_Destroy,
    /**/ EnMa4_Update,
    /**/ EnMa4_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static TexturePtr sEyeTextures[] = {
    gRomaniEyeOpenTex, gRomaniEyeHalfTex, gRomaniEyeClosedTex, gRomaniEyeHappyTex, gRomaniEyeSadTex,
};

static TexturePtr sMouthTextures[] = {
    gRomaniMouthHappyTex,
    gRomaniMouthFrownTex,
    gRomaniMouthHangingOpenTex,
    gRomaniMouthSmileTex,
};

void EnMa4_UpdateEyes(EnMa4* this) {
    if (this->overrideEyeTexIndex != 0) {
        this->eyeTexIndex = this->overrideEyeTexIndex;
    } else if (DECR(this->blinkTimer) == 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeTexIndex = 0;
        }
    }
}

typedef enum EnMa4Animation {
    /* -1 */ ENMA4_ANIM_NONE = -1,
    /*  0 */ ENMA4_ANIM_0,
    /*  1 */ ENMA4_ANIM_1,
    /*  2 */ ENMA4_ANIM_2,
    /*  3 */ ENMA4_ANIM_3,
    /*  4 */ ENMA4_ANIM_4,
    /*  5 */ ENMA4_ANIM_5,
    /*  6 */ ENMA4_ANIM_6,
    /*  7 */ ENMA4_ANIM_7,
    /*  8 */ ENMA4_ANIM_8,
    /*  9 */ ENMA4_ANIM_9,
    /* 10 */ ENMA4_ANIM_10,
    /* 11 */ ENMA4_ANIM_11,
    /* 12 */ ENMA4_ANIM_12,
    /* 13 */ ENMA4_ANIM_13,
    /* 14 */ ENMA4_ANIM_14,
    /* 15 */ ENMA4_ANIM_15,
    /* 16 */ ENMA4_ANIM_16,
    /* 17 */ ENMA4_ANIM_17,
    /* 18 */ ENMA4_ANIM_18,
    /* 19 */ ENMA4_ANIM_19,
    /* 20 */ ENMA4_ANIM_MAX
} EnMa4Animation;

static AnimationSpeedInfo sAnimationSpeedInfo[ENMA4_ANIM_MAX] = {
    { &gRomaniIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },              // ENMA4_ANIM_0
    { &gRomaniIdleAnim, 1.0f, ANIMMODE_LOOP, -6.0f },             // ENMA4_ANIM_1
    { &gRomaniLookAroundAnim, 1.0f, ANIMMODE_LOOP, 0.0f },        // ENMA4_ANIM_2
    { &gRomaniLookAroundAnim, 1.0f, ANIMMODE_LOOP, -6.0f },       // ENMA4_ANIM_3
    { &gRomaniSingStartAnim, 1.0f, ANIMMODE_ONCE, 0.0f },         // ENMA4_ANIM_4
    { &gRomaniSingStartAnim, 1.0f, ANIMMODE_ONCE, -6.0f },        // ENMA4_ANIM_5
    { &gRomaniSingLoopAnim, 1.0f, ANIMMODE_LOOP, 0.0f },          // ENMA4_ANIM_6
    { &gRomaniSingLoopAnim, 1.0f, ANIMMODE_LOOP, -6.0f },         // ENMA4_ANIM_7
    { &gRomaniRunAnim, 1.0f, ANIMMODE_LOOP, 0.0f },               // ENMA4_ANIM_8
    { &gRomaniRunAnim, 1.0f, ANIMMODE_LOOP, -6.0f },              // ENMA4_ANIM_9
    { &gRomaniWalkAnim, 1.0f, ANIMMODE_LOOP, 0.0f },              // ENMA4_ANIM_10
    { &gRomaniWalkAnim, 1.0f, ANIMMODE_LOOP, -6.0f },             // ENMA4_ANIM_11
    { &gRomaniShootBowAnim, 1.0f, ANIMMODE_ONCE, 0.0f },          // ENMA4_ANIM_12
    { &gRomaniShootBowAnim, 1.0f, ANIMMODE_ONCE, -6.0f },         // ENMA4_ANIM_13
    { &gRomaniSittingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },           // ENMA4_ANIM_14
    { &gRomaniSittingAnim, 1.0f, ANIMMODE_LOOP, -6.0f },          // ENMA4_ANIM_15
    { &gRomaniSittingHeadShakeAnim, 1.0f, ANIMMODE_LOOP, 0.0f },  // ENMA4_ANIM_16
    { &gRomaniSittingHeadShakeAnim, 1.0f, ANIMMODE_LOOP, -6.0f }, // ENMA4_ANIM_17
    { &gRomaniSittingLookDownAnim, 1.0f, ANIMMODE_LOOP, 0.0f },   // ENMA4_ANIM_18
    { &gRomaniSittingLookDownAnim, 1.0f, ANIMMODE_LOOP, -6.0f },  // ENMA4_ANIM_19
};

void EnMa4_ChangeAnim(EnMa4* this, s32 animIndex) {
    Animation_Change(&this->skelAnime, sAnimationSpeedInfo[animIndex].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationSpeedInfo[animIndex].animation),
                     sAnimationSpeedInfo[animIndex].mode, sAnimationSpeedInfo[animIndex].morphFrames);
}

void func_80ABDD9C(EnMa4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    if ((this->interactInfo.talkState == NPC_TALK_STATE_IDLE) &&
        ((this->skelAnime.animation == &gRomaniRunAnim) || (this->skelAnime.animation == &gRomaniLookAroundAnim) ||
         (this->skelAnime.animation == &gRomaniShootBowAnim))) {
        trackingMode = NPC_TRACKING_NONE;
    } else {
        trackingMode = ((this->type == MA4_TYPE_ALIENS_WON) && (this->actionFunc != EnMa4_DialogueHandler))
                           ? NPC_TRACKING_NONE
                           : NPC_TRACKING_PLAYER_AUTO_TURN;
    }

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.trackPos.y -= -10.0f;
    Npc_TrackPoint(&this->actor, &this->interactInfo, 0, trackingMode);
}

void EnMa4_InitPath(EnMa4* this, PlayState* play) {
    Path* path;
    Vec3f nextPoint;

    path = &play->setupPathList[ENMA_GET_PATH_INDEX(&this->actor)];
    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    this->waypointIndex = 0;
    this->pathPointsCount = path->count;

    this->actor.home.pos.x = this->pathPoints[0].x;
    this->actor.home.pos.y = this->pathPoints[0].y;
    this->actor.home.pos.z = this->pathPoints[0].z;
    nextPoint.x = this->pathPoints[1].x;
    nextPoint.y = this->pathPoints[1].y;
    nextPoint.z = this->pathPoints[1].z;

    this->actor.world.pos = this->actor.home.pos;
    this->actor.shape.rot.y = this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &nextPoint);
}

void EnMa4_Init(Actor* thisx, PlayState* play) {
    EnMa4* this = (EnMa4*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, NULL, this->jointTable, this->morphTable, ROMANI_LIMB_MAX);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    Actor_SetScale(&this->actor, 0.01f);

    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    this->unk_334 = 0;
    this->hasBow = true;
    this->mouthTexIndex = 0;
    this->animTimer = 0;

    if (CURRENT_DAY == 1) {
        this->type = MA4_TYPE_DAY1;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        this->type = MA4_TYPE_ALIENS_DEFEATED;
    } else {
        this->type = MA4_TYPE_ALIENS_WON;
        this->hasBow = false;
    }

    if (Cutscene_GetSceneLayer(play) != 0) {
        EnMa4_ChangeAnim(this, ENMA4_ANIM_0);
        this->state = MA4_STATE_HORSEBACKGAME;
        EnMa4_InitHorsebackGame(this, play);
    } else {
        EnMa4_InitPath(this, play);

        if (gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 1)) {
            EnMa4_ChangeAnim(this, ENMA4_ANIM_0);
            this->state = MA4_STATE_AFTERHORSEBACKGAME;
        } else if (gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 10)) {
            EnMa4_ChangeAnim(this, ENMA4_ANIM_0);
            this->state = MA4_STATE_AFTERDESCRIBETHEMCS;
        } else {
            if (this->type == MA4_TYPE_ALIENS_WON) {
                EnMa4_ChangeAnim(this, ENMA4_ANIM_14);
                this->actor.shape.shadowScale = 0.0f;
            } else {
                EnMa4_ChangeAnim(this, ENMA4_ANIM_8);
            }
            this->state = MA4_STATE_DEFAULT;
        }

        EnMa4_SetupWait(this);
    }
}

void EnMa4_Destroy(Actor* thisx, PlayState* play) {
    EnMa4* this = (EnMa4*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_08_01);
}

// Running in circles in the ranch
void EnMa4_RunInCircles(EnMa4* this, PlayState* play) {
    static s32 sAnimIndex = ENMA4_ANIM_9;
    static s32 D_80AC0250 = 0;
    static s16 D_80AC0254 = 10;
    Vec3f sp34;
    s32 pad;
    s16 sp2E;

    if ((sAnimIndex != ENMA4_ANIM_9) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (sAnimIndex == ENMA4_ANIM_3) {
            if (D_80AC0250 < 3) {
                D_80AC0250++;
            } else {
                D_80AC0250 = 0;
                EnMa4_ChangeAnim(this, ENMA4_ANIM_13);
                sAnimIndex = ENMA4_ANIM_13;
            }
        } else {
            this->actor.speed = 2.7f;
            EnMa4_ChangeAnim(this, ENMA4_ANIM_9);
            sAnimIndex = ENMA4_ANIM_9;
        }
    }

    if ((sAnimIndex == ENMA4_ANIM_13) && Animation_OnFrame(&this->skelAnime, 37.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ROMANI_BOW_FLICK);
    }

    sp34.x = this->pathPoints[this->waypointIndex].x;
    sp34.y = this->pathPoints[this->waypointIndex].y;
    sp34.z = this->pathPoints[this->waypointIndex].z;
    sp2E = Math_Vec3f_Yaw(&this->actor.world.pos, &sp34);
    if (Math_Vec3f_DistXZ(&this->actor.world.pos, &sp34) > 50.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, sp2E, 10, 0x3000, 0x100);
        Math_SmoothStepToS(&this->actor.shape.rot.y, sp2E, 5, 0x3000, 0x100);
    } else {
        if ((D_80AC0254 == 0) && ((Rand_Next() % 4) == 0)) {
            this->actor.speed = 0.0f;
            D_80AC0254 = 2;
            EnMa4_ChangeAnim(this, ENMA4_ANIM_3);
            sAnimIndex = ENMA4_ANIM_3;
        } else {
            if (D_80AC0254 > 0) {
                D_80AC0254--;
            }
        }

        if (this->waypointIndex < (this->pathPointsCount - 1)) {
            this->waypointIndex++;
        } else {
            this->waypointIndex = 0;
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    Actor_MoveWithGravity(&this->actor);
    if (this->skelAnime.animation == &gRomaniRunAnim) {
        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 4.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_ROMANI_WALK);
        }
    }
}

void EnMa4_SetupWait(EnMa4* this) {
    if ((this->state != MA4_STATE_AFTERHORSEBACKGAME) && (this->state != MA4_STATE_AFTERDESCRIBETHEMCS)) {
        if (this->type != MA4_TYPE_ALIENS_WON) {
            EnMa4_ChangeAnim(this, ENMA4_ANIM_9);
            this->actor.speed = 2.7f;
        } else {
            EnMa4_ChangeAnim(this, ENMA4_ANIM_15);
            this->actor.speed = 0.0f;
        }
    } else {
        EnMa4_ChangeAnim(this, ENMA4_ANIM_1);
        this->actor.speed = 0.0f;
    }

    this->actor.gravity = -0.2f;
    this->animTimer = 0;
    EnMa4_InitFaceExpression(this);
    this->actionFunc = EnMa4_Wait;
}

void EnMa4_Wait(EnMa4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;

    if ((this->state == MA4_STATE_AFTERHORSEBACKGAME) || (this->state == MA4_STATE_AFTERDESCRIBETHEMCS)) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else if (this->type != MA4_TYPE_ALIENS_WON) {
        EnMa4_RunInCircles(this, play);
    } else if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->animTimer++;
        if (this->animTimer == 5) {
            EnMa4_ChangeAnim(this, ENMA4_ANIM_17); // Traumatized anim
        } else if (this->animTimer == 8) {
            this->animTimer = 0;
            EnMa4_ChangeAnim(this, ENMA4_ANIM_15); // Sitting anim
        }
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnMa4_StartDialogue(this, play);
        EnMa4_SetupDialogueHandler(this);
    } else if (this->type != MA4_TYPE_ALIENS_WON || ABS_ALT(yaw) < 0x4000) {
        if (!(player->stateFlags1 & PLAYER_STATE1_800000)) {
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }
    }
}

// Chooses the next dialogue based on player's selection
void EnMa4_HandlePlayerChoice(EnMa4* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x3339:
                if (play->msgCtx.choiceIndex == 0) {
                    Audio_PlaySfx_MessageDecide();
                    Message_StartTextbox(play, 0x333A, &this->actor);
                    this->textId = 0x333A;
                } else {
                    Audio_PlaySfx_MessageDecide();
                    Message_StartTextbox(play, 0x333B, &this->actor);
                    this->textId = 0x333B;
                }
                break;

            case 0x3341:
                if (play->msgCtx.choiceIndex == 0) {
                    Audio_PlaySfx_MessageDecide();
                    SET_WEEKEVENTREG(WEEKEVENTREG_PROMISED_TO_HELP_WITH_ALIENS);
                    Message_StartTextbox(play, 0x3343, &this->actor);
                    this->textId = 0x3343;
                } else {
                    Audio_PlaySfx_MessageCancel();
                    EnMa4_SetFaceExpression(this, 0, 1);
                    Message_StartTextbox(play, 0x3342, &this->actor);
                    this->textId = 0x3342;
                    this->state = MA4_STATE_DEFAULT;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                }
                break;

            case 0x3346:
                if (play->msgCtx.choiceIndex == 0) {
                    Audio_PlaySfx_MessageDecide();
                    SET_WEEKEVENTREG(WEEKEVENTREG_PROMISED_TO_HELP_WITH_ALIENS);
                    Message_StartTextbox(play, 0x3343, &this->actor);
                    this->textId = 0x3343;
                } else {
                    Audio_PlaySfx_MessageCancel();
                    EnMa4_SetFaceExpression(this, 0, 1);
                    Message_StartTextbox(play, 0x3342, &this->actor);
                    this->textId = 0x3342;
                }
                break;

            case 0x3347:
                if (play->msgCtx.choiceIndex == 0) {
                    Audio_PlaySfx_MessageDecide();
                    Message_StartTextbox(play, 0x3349, &this->actor);
                    this->textId = 0x3349;
                } else {
                    Audio_PlaySfx_MessageCancel();
                    Message_StartTextbox(play, 0x3348, &this->actor);
                    this->textId = 0x3348;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                }
                break;

            case 0x334D:                             // Asks if you understood her explanation.
                if (play->msgCtx.choiceIndex == 0) { // Yes
                    s32 aux;

                    Audio_PlaySfx_MessageDecide();
                    Message_StartTextbox(play, 0x334E, &this->actor);
                    this->textId = 0x334E;
                    if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
                        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_PROMISED_TO_HELP_WITH_ALIENS);
                    }
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                } else { // No.
                    Audio_PlaySfx_MessageCancel();
                    EnMa4_SetFaceExpression(this, 0, 0);
                    Message_StartTextbox(play, 0x334C, &this->actor);
                    this->textId = 0x334C;
                }
                break;

            case 0x3354:
                if (play->msgCtx.choiceIndex == 0) {
                    Audio_PlaySfx_MessageDecide();
                    Message_StartTextbox(play, 0x3349, &this->actor);
                    this->textId = 0x3349;
                } else {
                    Audio_PlaySfx_MessageCancel();
                    EnMa4_SetFaceExpression(this, 1, 0);
                    Message_StartTextbox(play, 0x3355, &this->actor);
                    this->textId = 0x3355;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                }
                break;

            case 0x3356:
                // "Try again?"
                if (play->msgCtx.choiceIndex == 0) { // Yes
                    Audio_PlaySfx_MessageDecide();
                    Message_CloseTextbox(play);
                    EnMa4_SetupBeginHorsebackGame(this);
                } else { // No
                    if (this->type == MA4_TYPE_ALIENS_DEFEATED) {
                        Audio_PlaySfx_MessageCancel();
                        EnMa4_SetFaceExpression(this, 3, 3);
                        Message_StartTextbox(play, 0x3357, &this->actor);
                        this->textId = 0x3357;
                        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                    } else {
                        Audio_PlaySfx_MessageCancel();
                        EnMa4_SetFaceExpression(this, 4, 2);
                        Message_StartTextbox(play, 0x335B, &this->actor);
                        this->textId = 0x335B;
                        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                    }
                }
                break;

            case 0x3359:
                if (play->msgCtx.choiceIndex == 0) {
                    Audio_PlaySfx_MessageDecide();
                    Message_StartTextbox(play, 0x3349, &this->actor);
                    this->textId = 0x3349;
                } else {
                    Audio_PlaySfx_MessageCancel();
                    EnMa4_SetFaceExpression(this, 4, 2);
                    Message_StartTextbox(play, 0x335A, &this->actor);
                    this->textId = 0x335A;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                }
                break;

            default:
                break;
        }
    }
}

void EnMa4_ChooseNextDialogue(EnMa4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 aux;

    if (Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x2390:
                Message_CloseTextbox(play);
                EnMa4_SetupBeginHorsebackGame(this);
                break;

            case 0x3335:
                EnMa4_SetFaceExpression(this, 0, 3);
                Message_StartTextbox(play, 0x3336, &this->actor);
                this->textId = 0x3336;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                break;

            case 0x3338:
                Message_StartTextbox(play, 0x3339, &this->actor);
                this->textId = 0x3339;
                break;

            case 0x333A:
            case 0x333B:
                EnMa4_SetFaceExpression(this, 0, 3);
                Message_StartTextbox(play, 0x333C, &this->actor);
                this->textId = 0x333C;
                break;

            case 0x333C:
                EnMa4_SetFaceExpression(this, 0, 2);
                Message_StartTextbox(play, 0x333D, &this->actor);
                this->textId = 0x333D;
                break;

            case 0x333D:
                Message_StartTextbox(play, 0x333E, &this->actor);
                this->textId = 0x333E;
                break;

            case 0x333E:
                Message_CloseTextbox(play);
                EnMa4_SetupBeginDescribeThemCs(this);
                break;

            case 0x333F:
                Message_StartTextbox(play, 0x3340, &this->actor);
                this->textId = 0x3340;
                break;

            case 0x3340:
                EnMa4_SetFaceExpression(this, 0, 3);
                Message_StartTextbox(play, 0x3341, &this->actor);
                this->textId = 0x3341;
                break;

            case 0x3343:
                EnMa4_SetFaceExpression(this, 0, 0);
                Message_StartTextbox(play, 0x3344, &this->actor);
                this->textId = 0x3344;
                break;

            case 0x3344:
                Message_StartTextbox(play, 0x3345, &this->actor);
                this->textId = 0x3345;
                break;

            case 0x3345:
                Message_StartTextbox(play, 0x3349, &this->actor);
                this->textId = 0x3349;
                break;

            case 0x3349:
                Message_StartTextbox(play, 0x334A, &this->actor);
                this->textId = 0x334A;
                break;

            case 0x334A:
                Message_CloseTextbox(play);
                EnMa4_SetupBeginHorsebackGame(this);
                break;

            case 0x334C:
                Message_StartTextbox(play, 0x334D, &this->actor);
                this->textId = 0x334D;
                break;

            case 0x3350:
            case 0x3351:
            case 0x335D:
            case 0x335E:
                if (this->type == MA4_TYPE_DAY1) {
                    Message_StartTextbox(play, 0x3352, &this->actor);
                    this->textId = 0x3352;
                } else {
                    Message_StartTextbox(play, 0x3356, &this->actor);
                    this->textId = 0x3356;
                }
                break;

            case 0x3352: // Win minigame
            case 0x336D: // Lose minigame
                EnMa4_SetFaceExpression(this, 0, 0);

                // Check if player has Epona's song
                if (GameInteractor_Should(VB_ROMANI_CONSIDER_EPONA_SONG_GIVEN, CHECK_QUEST_ITEM(QUEST_SONG_EPONA),
                                          true)) {
                    Message_StartTextbox(play, 0x334C, &this->actor);
                    this->textId = 0x334C;
                } else {
                    Message_CloseTextbox(play);
                    player->stateFlags1 |= PLAYER_STATE1_20;
                    EnMa4_SetupBeginEponasSongCs(this);
                    EnMa4_BeginEponasSongCs(this, play);
                }
                break;

            case 0x3358:
                if ((GET_PLAYER_FORM != PLAYER_FORM_HUMAN) || !CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
                    Message_StartTextbox(play, 0x335C, &this->actor);
                    this->textId = 0x335C;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                } else {
                    Message_StartTextbox(play, 0x3359, &this->actor);
                    this->textId = 0x3359;
                }
                break;

            default:
                break;
        }
    }
}

void EnMa4_SetupDialogueHandler(EnMa4* this) {
    if (this->type != MA4_TYPE_ALIENS_WON) {
        EnMa4_ChangeAnim(this, ENMA4_ANIM_1);
    } else {
        EnMa4_ChangeAnim(this, ENMA4_ANIM_15);
    }
    this->actionFunc = EnMa4_DialogueHandler;
}

void EnMa4_DialogueHandler(EnMa4* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE: // Player answered a question
            EnMa4_HandlePlayerChoice(this, play);
            break;

        case TEXT_STATE_EVENT: // End message block
            EnMa4_ChooseNextDialogue(this, play);
            break;

        case TEXT_STATE_DONE: // End conversation
            if (Message_ShouldAdvance(play)) {
                if ((play->msgCtx.bombersNotebookEventQueueCount == 0) || !CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
                    EnMa4_SetupWait(this);
                }
            }

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            break;

        default:
            break;
    }

    if (this->type != MA4_TYPE_ALIENS_WON) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x7D0, 0x3E8);
    }
}

void EnMa4_SetupBeginHorsebackGame(EnMa4* this) {
    this->actionFunc = EnMa4_BeginHorsebackGame;
}

void EnMa4_BeginHorsebackGame(EnMa4* this, PlayState* play) {
    play->nextEntrance = ENTRANCE(ROMANI_RANCH, 0);
    gSaveContext.nextCutsceneIndex = 0xFFF0;
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_80;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
}

void EnMa4_HorsebackGameCheckPlayerInteractions(EnMa4* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        // "You're feeling confident"
        Message_StartTextbox(play, 0x336E, &this->actor);
        this->actionFunc = EnMa4_HorsebackGameTalking;
    } else if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] < SECONDS_TO_TIMER(115)) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void EnMa4_HorsebackGameTalking(EnMa4* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = EnMa4_HorsebackGameWait;
    }
}

void EnMa4_InitHorsebackGame(EnMa4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    play->interfaceCtx.minigameState = MINIGAME_STATE_COUNTDOWN_SETUP_3;
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 0);
    SET_WEEKEVENTREG(WEEKEVENTREG_08_01);
    Interface_InitMinigame(play);
    player->stateFlags1 |= PLAYER_STATE1_20;
    this->actionFunc = EnMa4_SetupHorsebackGameWait;
}

void EnMa4_SetupHorsebackGameWait(EnMa4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->interfaceCtx.minigameState == MINIGAME_STATE_COUNTDOWN_GO) {
        this->actionFunc = EnMa4_HorsebackGameWait;
        player->stateFlags1 &= ~PLAYER_STATE1_20;
    }
}

void EnMa4_HorsebackGameWait(EnMa4* this, PlayState* play) {
    static s16 D_80AC0258 = 0;
    Player* player = GET_PLAYER(play);

    player->stateFlags3 |= PLAYER_STATE3_400;
    EnMa4_HorsebackGameCheckPlayerInteractions(this, play);

    if (D_80AC0258 != this->poppedBalloonCounter) {
        D_80AC0258 = this->poppedBalloonCounter;
        play->interfaceCtx.minigamePoints = 1;
    }

    if ((gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] >= SECONDS_TO_TIMER(120)) ||
        GameInteractor_Should(VB_WIN_ROMANI_PRACTICE, this->poppedBalloonCounter == 10, this)) {
        gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_6;
        EnMa4_SetupHorsebackGameEnd(this, play);
        D_80AC0258 = 0;
    }
}

void EnMa4_SetupHorsebackGameEnd(EnMa4* this, PlayState* play) {
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_08_01);
    this->actionFunc = EnMa4_HorsebackGameEnd;
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_HORSE_GOAL | SEQ_FLAG_ASYNC);
}

void EnMa4_HorsebackGameEnd(EnMa4* this, PlayState* play) {
    static s32 sFrameCounter = 0;
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_100000) {
        play->actorCtx.isOverrideInputOn = true;
        play->actorCtx.overrideInput.press.button = BTN_A;
    } else {
        play->actorCtx.isOverrideInputOn = true;
    }

    if (sFrameCounter == 25) {
        if (this->poppedBalloonCounter == 10) {
            Message_StartTextbox(play, 0x334F, &this->actor);
            this->textId = 0x334F;
        } else {
            Message_StartTextbox(play, 0x334B, &this->actor);
            this->textId = 0x334B;
        }
    } else if (sFrameCounter == 50) {
        play->actorCtx.isOverrideInputOn = false;
        play->nextEntrance = ENTRANCE(ROMANI_RANCH, 1);
        gSaveContext.nextCutsceneIndex = 0;
        sFrameCounter = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;

        if (this->poppedBalloonCounter == 10) {
            play->transitionType = TRANS_TYPE_80;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        } else {
            play->transitionType = TRANS_TYPE_64;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
        }

        this->poppedBalloonCounter = 0;
    }

    sFrameCounter++;
}

void EnMa4_SetupBeginEponasSongCs(EnMa4* this) {
    this->actionFunc = EnMa4_BeginEponasSongCs;
}

// Epona's Song cutscene is an ActorCutscene
void EnMa4_BeginEponasSongCs(EnMa4* this, PlayState* play) {
    s16 csId = this->actor.csId;

    if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_Start(csId, &this->actor);
        EnMa4_SetupEponasSongCs(this);
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(csId);
    }
}

void EnMa4_SetupEponasSongCs(EnMa4* this) {
    this->animTimer = 0;
    this->actionFunc = EnMa4_EponasSongCs;
}

static u16 sCueId = 99;

void EnMa4_EponasSongCs(EnMa4* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_120)) {
        s32 cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_120);

        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            if (sCueId != play->csCtx.actorCues[cueChannel]->id) {
                sCueId = play->csCtx.actorCues[cueChannel]->id;
                this->animTimer = 0;

                switch (play->csCtx.actorCues[cueChannel]->id) {
                    case 1:
                        this->hasBow = true;
                        EnMa4_ChangeAnim(this, ENMA4_ANIM_1);
                        break;

                    case 2:
                        this->hasBow = false;
                        EnMa4_ChangeAnim(this, ENMA4_ANIM_4);
                        break;

                    default:
                        break;
                }
            }
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        if ((sCueId == 2) && (this->animTimer == 0) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            EnMa4_ChangeAnim(this, ENMA4_ANIM_7);
        }
    } else {
        Player* player = GET_PLAYER(play);

        player->stateFlags1 |= PLAYER_STATE1_20;
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
        sCueId = 99;
        this->hasBow = true;
        EnMa4_SetupEndEponasSongCs(this);
    }
}

void EnMa4_SetupEndEponasSongCs(EnMa4* this) {
    this->actionFunc = EnMa4_EndEponasSongCs;
}

void EnMa4_EndEponasSongCs(EnMa4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        Message_StartTextbox(play, 0x334C, &this->actor);
        this->textId = 0x334C;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        EnMa4_SetupDialogueHandler(this);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void EnMa4_SetupBeginDescribeThemCs(EnMa4* this) {
    this->actionFunc = EnMa4_BeginDescribeThemCs;
}

void EnMa4_BeginDescribeThemCs(EnMa4* this, PlayState* play) {
    play->nextEntrance = ENTRANCE(ROMANI_RANCH, 0);
    gSaveContext.nextCutsceneIndex = 0xFFF5;
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_64;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
}

void EnMa4_StartDialogue(EnMa4* this, PlayState* play) {
    s32 pad;
    OSTime time;

    switch (this->type) {
        case MA4_TYPE_DAY1:
            if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_21_80)) {
                    EnMa4_SetFaceExpression(this, 3, 3);
                    Message_StartTextbox(play, 0x3337, &this->actor);
                    this->textId = 0x3337;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                } else {
                    Message_StartTextbox(play, 0x3335, &this->actor);
                    this->textId = 0x3335;
                    SET_WEEKEVENTREG(WEEKEVENTREG_21_80);
                }
            } else if (this->state == MA4_STATE_DEFAULT) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_21_40)) {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_PROMISED_TO_HELP_WITH_ALIENS)) {
                        Message_StartTextbox(play, 0x3346, &this->actor);
                        this->textId = 0x3346;
                    } else {
                        Message_StartTextbox(play, 0x3347, &this->actor);
                        this->textId = 0x3347;
                    }
                } else {
                    Message_StartTextbox(play, 0x3338, &this->actor);
                    this->textId = 0x3338;
                    SET_WEEKEVENTREG(WEEKEVENTREG_21_40);
                }
            } else if (this->state == MA4_STATE_AFTERHORSEBACKGAME) {
                if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] >= SECONDS_TO_TIMER(120)) {
                    // "Too bad Grasshopper"
                    EnMa4_SetFaceExpression(this, 0, 0);
                    Message_StartTextbox(play, 0x336D, &this->actor);
                    this->textId = 0x336D;
                } else {
                    time = gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2];
                    if ((s32)time < HS_GET_HORSE_BACK_BALLOON_TIME()) {
                        // [Score] New record!
                        HS_SET_HORSE_BACK_BALLOON_TIME(time);
                        EnMa4_SetFaceExpression(this, 0, 3);
                        Message_StartTextbox(play, 0x3350, &this->actor);
                        this->textId = 0x3350;
                    } else {
                        // [Score] Great.
                        Message_StartTextbox(play, 0x3351, &this->actor);
                        this->textId = 0x3351;
                    }
                }
                this->state = MA4_STATE_DEFAULT;
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            } else if (this->state == MA4_STATE_AFTERDESCRIBETHEMCS) {
                // "Cremia doesn't believe me..."
                Message_StartTextbox(play, 0x3340, &this->actor);
                this->textId = 0x3340;
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            }
            break;

        case MA4_TYPE_ALIENS_DEFEATED:
            if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_21_80)) {
                    EnMa4_SetFaceExpression(this, 3, 3);
                    Message_StartTextbox(play, 0x3337, &this->actor);
                    this->textId = 0x3337;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                } else {
                    Message_StartTextbox(play, 0x3335, &this->actor);
                    this->textId = 0x3335;
                    SET_WEEKEVENTREG(WEEKEVENTREG_21_80);
                }
            } else if (this->state == MA4_STATE_DEFAULT) {
                Message_StartTextbox(play, 0x3354, &this->actor);
                this->textId = 0x3354;
            } else if (this->state == MA4_STATE_AFTERHORSEBACKGAME) {
                if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] >= SECONDS_TO_TIMER(120)) {
                    // "Try again?"
                    Message_StartTextbox(play, 0x3356, &this->actor);
                    this->textId = 0x3356;
                } else {
                    time = gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2];
                    if ((s32)time < HS_GET_HORSE_BACK_BALLOON_TIME()) {
                        HS_SET_HORSE_BACK_BALLOON_TIME(time);
                        EnMa4_SetFaceExpression(this, 0, 3);
                        Message_StartTextbox(play, 0x3350, &this->actor);
                        this->textId = 0x3350;
                    } else {
                        Message_StartTextbox(play, 0x3351, &this->actor);
                        this->textId = 0x3351;
                    }
                }
                this->state = MA4_STATE_DEFAULT;
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            }
            break;

        case MA4_TYPE_ALIENS_WON:
            if (this->state == MA4_STATE_DEFAULT) {
                // "Huh... You?"
                Message_StartTextbox(play, 0x3358, &this->actor);
                this->textId = 0x3358;
            } else if (this->state == MA4_STATE_AFTERHORSEBACKGAME) {
                if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] >= SECONDS_TO_TIMER(120)) {
                    // "Try again?"
                    Message_StartTextbox(play, 0x3356, &this->actor);
                    this->textId = 0x3356;
                } else {
                    time = gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2];
                    if ((s32)time < HS_GET_HORSE_BACK_BALLOON_TIME()) {
                        // New record
                        HS_SET_HORSE_BACK_BALLOON_TIME(time);
                        Message_StartTextbox(play, 0x335D, &this->actor);
                        this->textId = 0x335D;
                    } else {
                        // "Old record was: [record]"
                        Message_StartTextbox(play, 0x335E, &this->actor);
                        this->textId = 0x335E;
                    }
                }
                this->state = MA4_STATE_DEFAULT;
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            }
            break;

        default:
            // Dead code
            Message_StartTextbox(play, 0x3335, &this->actor);
            this->textId = 0x3335;
            break;
    }
}

void EnMa4_SetFaceExpression(EnMa4* this, s16 overrideEyeTexIndex, s16 mouthTexIndex) {
    this->overrideEyeTexIndex = overrideEyeTexIndex;
    this->mouthTexIndex = mouthTexIndex;
}

void EnMa4_InitFaceExpression(EnMa4* this) {
    if (this->type != MA4_TYPE_ALIENS_WON) {
        EnMa4_SetFaceExpression(this, 0, 0);
    } else {
        EnMa4_SetFaceExpression(this, 1, 2);
    }
}

void EnMa4_Update(Actor* thisx, PlayState* play) {
    EnMa4* this = (EnMa4*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);
    EnMa4_UpdateEyes(this);
    this->actionFunc(this, play);
    func_80ABDD9C(this, play);
}

s32 EnMa4_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnMa4* this = (EnMa4*)thisx;
    Vec3s limbRot;

    if (limbIndex == ROMANI_LIMB_HEAD) {
        limbRot = this->interactInfo.headRot;
        rot->x += limbRot.y;
        rot->z += limbRot.x;
    }
    if (limbIndex == ROMANI_LIMB_TORSO) {
        limbRot = this->interactInfo.torsoRot;
        rot->x -= limbRot.y;
        rot->z -= limbRot.x;
    }

    return false;
}

void EnMa4_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnMa4* this = (EnMa4*)thisx;
    Vec3f sp28 = { 800.0f, 0.0f, 0.0f };

    if (limbIndex == ROMANI_LIMB_HEAD) {
        Matrix_MultVec3f(&sp28, &this->actor.focus.pos);
    } else if (limbIndex == ROMANI_LIMB_LEFT_HAND) {
        if (this->hasBow == true) {
            OPEN_DISPS(play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, gRomaniBowDL);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void EnMa4_Draw(Actor* thisx, PlayState* play) {
    EnMa4* this = (EnMa4*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->type == MA4_TYPE_ALIENS_WON) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gRomaniWoodenBoxDL);
    }

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_K0(sEyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_K0(sMouthTextures[this->mouthTexIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMa4_OverrideLimbDraw, EnMa4_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

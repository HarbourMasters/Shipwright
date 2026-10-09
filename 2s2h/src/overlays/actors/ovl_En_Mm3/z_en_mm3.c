/*
 * File: z_en_mm3.c
 * Overlay: ovl_En_Mm3
 * Description: Counting Game Postman
 */

#include "z_en_mm3.h"

#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMm3_Init(Actor* thisx, PlayState* play);
void EnMm3_Destroy(Actor* thisx, PlayState* play);
void EnMm3_Update(Actor* thisx, PlayState* play);
void EnMm3_Draw(Actor* thisx, PlayState* play);

void func_80A6F270(EnMm3* this);
void func_80A6F2C8(EnMm3* this, PlayState* play);
void func_80A6F9C8(EnMm3* this);
void func_80A6F9DC(EnMm3* this, PlayState* play);
void func_80A6FBA0(EnMm3* this);
void func_80A6FBFC(EnMm3* this, PlayState* play);
void func_80A6FE1C(EnMm3* this);
void func_80A6FE30(EnMm3* this, PlayState* play);
void func_80A6FED8(EnMm3* this);
void func_80A6FEEC(EnMm3* this, PlayState* play);
s32 func_80A6FFAC(EnMm3* this, PlayState* play);
void func_80A70084(EnMm3* this, PlayState* play);

ActorProfile En_Mm3_Profile = {
    /**/ ACTOR_EN_MM3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MM,
    /**/ sizeof(EnMm3),
    /**/ EnMm3_Init,
    /**/ EnMm3_Destroy,
    /**/ EnMm3_Update,
    /**/ EnMm3_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnMm3Animation {
    /* 0 */ ENMM3_ANIM_0,
    /* 1 */ ENMM3_ANIM_1,
    /* 2 */ ENMM3_ANIM_2,
    /* 3 */ ENMM3_ANIM_3,
    /* 4 */ ENMM3_ANIM_4,
    /* 5 */ ENMM3_ANIM_5,
    /* 6 */ ENMM3_ANIM_6,
    /* 7 */ ENMM3_ANIM_7,
    /* 8 */ ENMM3_ANIM_MAX
} EnMm3Animation;

static AnimationInfo sAnimationInfo[ENMM3_ANIM_MAX] = {
    { &object_mm_Anim_002238, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -7.0f },   // ENMM3_ANIM_0
    { &object_mm_Anim_00A4E0, -1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -7.0f },  // ENMM3_ANIM_1
    { &object_mm_Anim_00C640, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -7.0f },   // ENMM3_ANIM_2
    { &object_mm_Anim_00A4E0, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -7.0f },   // ENMM3_ANIM_3
    { &object_mm_Anim_000468, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -7.0f },   // ENMM3_ANIM_4
    { &object_mm_Anim_00CD90, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -12.0f },  // ENMM3_ANIM_5
    { &object_mm_Anim_00DA50, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -12.0f },  // ENMM3_ANIM_6
    { &object_mm_Anim_00DA50, 1.0f, 0.0f, 10.0f, ANIMMODE_ONCE, -10.0f }, // ENMM3_ANIM_7
};

#include "overlays/ovl_En_Mm3/ovl_En_Mm3.h"

Vec3f D_80A704F0 = { 0.0f, 0.0f, 0.0f };

TexturePtr D_80A704FC[] = { object_mm_Tex_002950, object_mm_Tex_002750 };

void EnMm3_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm3* this = (EnMm3*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 21.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_mm_Skel_0096E8, &object_mm_Anim_00A4E0, this->jointTable,
                       this->morphTable, OBJECT_MM_LIMB_MAX);
    Animation_Change(&this->skelAnime, &object_mm_Anim_00A4E0, -1.0f, Animation_GetLastFrame(&object_mm_Anim_00A4E0),
                     0.0f, ANIMMODE_ONCE, 0.0f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    this->actor.parent = NULL;
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->unk_1DC = 1;
    this->unk_2B4 = 0;
    this->unk_2AE = 0;
    this->unk_2B0 &= ~1;
    this->actor.world.pos.y = this->actor.floorHeight;
    func_80A6F270(this);
}

void EnMm3_Destroy(Actor* thisx, PlayState* play) {
    EnMm3* this = (EnMm3*)thisx;

    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80A6F22C(EnMm3* this) {
    s16 abs = BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y);

    abs = ABS_ALT(abs);
    if (abs < 0x4000) {
        return true;
    }
    return false;
}

void func_80A6F270(EnMm3* this) {
    this->unk_1DC = 1;
    this->unk_2B0 &= ~1;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENMM3_ANIM_5);
    this->actionFunc = func_80A6F2C8;
}

void func_80A6F2C8(EnMm3* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, 0x278A, &this->actor);
        this->unk_2B4 = 0x278A;
        func_80A6F9C8(this);
    } else if (func_80A6F22C(this)) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }

    Math_SmoothStepToS(&this->headRot.x, 0, 5, 0x1000, 0x100);
    Math_SmoothStepToS(&this->headRot.y, 0, 5, 0x1000, 0x100);
    Math_SmoothStepToS(&this->torsoRot.x, 0, 5, 0x1000, 0x100);
    Math_SmoothStepToS(&this->torsoRot.y, 0, 5, 0x1000, 0x100);
}

void func_80A6F3B4(EnMm3* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->unk_2B4) {
            case 0x278E:
                if (play->msgCtx.choiceIndex == 0) {
                    if (this->unk_2B2 & 0x20) {
                        if (gSaveContext.save.saveInfo.playerData.rupees >= play->msgCtx.unk1206C) {
                            Audio_PlaySfx_MessageDecide();
                            Message_StartTextbox(play, 0x2790, &this->actor);
                            this->unk_2B4 = 0x2790;
                            Rupees_ChangeBy(-play->msgCtx.unk1206C);
                        } else {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            Message_StartTextbox(play, 0x279C, &this->actor);
                            this->unk_2B4 = 0x279C;
                            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN);
                        }
                    } else {
                        Audio_PlaySfx_MessageDecide();
                        Message_StartTextbox(play, 0x2790, &this->actor);
                        this->unk_2B4 = 0x2790;
                    }
                } else {
                    Audio_PlaySfx_MessageCancel();
                    Message_StartTextbox(play, 0x278F, &this->actor);
                    this->unk_2B4 = 0x278F;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN);
                }
                break;

            case 0x279A:
                if (play->msgCtx.choiceIndex == 0) {
                    if (gSaveContext.save.saveInfo.playerData.rupees >= play->msgCtx.unk1206C) {
                        Audio_PlaySfx_MessageDecide();
                        Message_StartTextbox(play, 0x2790, &this->actor);
                        this->unk_2B4 = 0x2790;
                        Rupees_ChangeBy(-play->msgCtx.unk1206C);
                    } else {
                        Audio_PlaySfx(NA_SE_SY_ERROR);
                        Message_StartTextbox(play, 0x279C, &this->actor);
                        this->unk_2B4 = 0x279C;
                        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN);
                    }
                } else {
                    Audio_PlaySfx_MessageCancel();
                    Message_StartTextbox(play, 0x279B, &this->actor);
                    this->unk_2B4 = 0x279B;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN);
                }
                break;

            default:
                break;
        }
    }
}

void func_80A6F5E4(EnMm3* this, PlayState* play) {
    if (((this->unk_2B4 != 0x2791) || (this->unk_2AC == 0)) && Message_ShouldAdvance(play)) {
        switch (this->unk_2B4) {
            case 0x278A:
                if (func_80A6FFAC(this, play)) {
                    Message_StartTextbox(play, 0x279D, &this->actor);
                    this->unk_2B4 = 0x279D;
                } else {
                    func_80A70084(this, play);
                    if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED,
                                              Player_GetMask(play) == PLAYER_MASK_BUNNY, GET_PLAYER(play))) {
                        Message_StartTextbox(play, 0x27A0, &this->actor);
                        this->unk_2B4 = 0x27A0;
                    } else {
                        Message_StartTextbox(play, 0x278B, &this->actor);
                        this->unk_2B4 = 0x278B;
                    }
                }
                this->unk_1DC = 0;
                this->unk_2B0 |= 1;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENMM3_ANIM_7);
                break;

            case 0x278B:
            case 0x27A0:
                this->unk_2AE = 0;
                Message_StartTextbox(play, 0x278C, &this->actor);
                this->unk_2B4 = 0x278C;
                break;

            case 0x278C:
                Message_StartTextbox(play, 0x278D, &this->actor);
                this->unk_2B4 = 0x278D;
                break;

            case 0x279D:
                this->unk_2AE = 0;
                Message_StartTextbox(play, 0x279E, &this->actor);
                this->unk_2B4 = 0x279E;
                break;

            case 0x279E:
                this->unk_2B2 |= 0x20;
                Message_StartTextbox(play, 0x279F, &this->actor);
                this->unk_2B4 = 0x279F;
                break;

            case 0x278D:
            case 0x279F:
                Message_StartTextbox(play, 0x278E, &this->actor);
                this->unk_2B4 = 0x278E;
                break;

            case 0x2791:
                if (gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] == SECONDS_TO_TIMER(10)) {
                    Message_StartTextbox(play, 0x2792, &this->actor);
                    this->unk_2B4 = 0x2792;
                } else if ((gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] >= SECONDS_TO_TIMER(15))) {
                    Message_StartTextbox(play, 0x2797, &this->actor);
                    this->unk_2B4 = 0x2797;
                } else if ((gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] <= SECONDS_TO_TIMER_PRECISE(10, 50)) &&
                           (gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] >= SECONDS_TO_TIMER_PRECISE(9, 50))) {
                    Message_StartTextbox(play, 0x2795, &this->actor);
                    this->unk_2B4 = 0x2795;
                } else {
                    Message_StartTextbox(play, 0x2796, &this->actor);
                    this->unk_2B4 = 0x2796;
                }
                break;

            case 0x2792:
                Message_StartTextbox(play, 0x2793, &this->actor);
                this->unk_2B4 = 0x2793;
                break;

            case 0x2793:
                Message_CloseTextbox(play);
                func_80A6FE1C(this);
                func_80A6FE30(this, play);
                break;

            case 0x2795:
            case 0x2796:
            case 0x2797:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                    Message_StartTextbox(play, 0x279B, &this->actor);
                    this->unk_2B4 = 0x279B;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN);
                } else {
                    Message_StartTextbox(play, 0x2798, &this->actor);
                    this->unk_2B4 = 0x2798;
                }
                break;

            case 0x2798:
                Message_StartTextbox(play, 0x2799, &this->actor);
                this->unk_2B4 = 0x2799;
                break;

            case 0x2799:
                Message_StartTextbox(play, 0x279A, &this->actor);
                this->unk_2B4 = 0x279A;
                break;

            default:
                break;
        }
    } else if ((this->unk_2AC > 0) && (this->unk_2B4 == 0x2791)) {
        this->unk_2AC--;
        if (this->unk_2AC == 0) {
            if (gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] == SECONDS_TO_TIMER(10)) {
                // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to
                // anything bad.
                Audio_PlayFanfare(NA_BGM_GET_ITEM);
            } else {
                Audio_PlaySfx(NA_SE_SY_ERROR);
            }
        }
    }
}

void func_80A6F9C8(EnMm3* this) {
    this->actionFunc = func_80A6F9DC;
}

void func_80A6F9DC(EnMm3* this, PlayState* play) {
    this->unk_2B0 &= ~2;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
            break;

        case TEXT_STATE_FADING:
            this->unk_2B0 |= 2;
            break;

        case TEXT_STATE_CHOICE:
            func_80A6F3B4(this, play);
            break;

        case TEXT_STATE_EVENT:
            func_80A6F5E4(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                if (this->unk_2B4 == 0x2790) {
                    Player* player = GET_PLAYER(play);

                    player->stateFlags1 |= PLAYER_STATE1_20;
                    if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED,
                                              Player_GetMask(play) == PLAYER_MASK_BUNNY, player)) {
                        Interface_StartPostmanTimer(0, POSTMAN_MINIGAME_BUNNY_HOOD_ON);
                    } else {
                        Interface_StartPostmanTimer(0, POSTMAN_MINIGAME_BUNNY_HOOD_OFF);
                    }
                    Message_CloseTextbox(play);
                    Audio_PlaySfx(NA_SE_SY_START_SHOT);
                    func_80A6FBA0(this);
                } else {
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                    func_80A6F270(this);
                }
            }
            break;

        default:
            break;
    }

    if ((this->skelAnime.mode == ANIMMODE_ONCE) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENMM3_ANIM_2);
    }

    if (((this->unk_2B4 == 0x279D) || (this->unk_2B4 == 0x27A0) || (this->unk_2B4 == 0x278B)) &&
        Animation_OnFrame(&this->skelAnime, 8.0f) && (this->unk_2AE == 0)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_LIE_DOWN_ON_BED);
        this->unk_2AE = 1;
    }
}

void func_80A6FBA0(EnMm3* this) {
    AudioSfx_MuteBanks((1 << BANK_PLAYER) | (1 << BANK_ITEM) | (1 << BANK_ENV) | (1 << BANK_ENEMY) |
                       (1 << BANK_OCARINA) | (1 << BANK_VOICE));
    Audio_SetMainBgmVolume(0, 5);
    SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
    this->actionFunc = func_80A6FBFC;
}

void func_80A6FBFC(EnMm3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (gSaveContext.timerStates[TIMER_ID_POSTMAN] == TIMER_STATE_POSTMAN_END) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        if (gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] > SECONDS_TO_TIMER(15)) {
            gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] = SECONDS_TO_TIMER(15);
        } else if ((((void)0, gSaveContext.timerCurTimes[TIMER_ID_POSTMAN]) >=
                    (OSTime)(SECONDS_TO_TIMER_PRECISE(10, -5) - XREG(16))) &&
                   (((void)0, gSaveContext.timerCurTimes[TIMER_ID_POSTMAN]) <=
                    (OSTime)(SECONDS_TO_TIMER_PRECISE(10, 5) + XREG(17)))) {
            gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] = SECONDS_TO_TIMER(10);
        }
    } else if (gSaveContext.timerCurTimes[TIMER_ID_POSTMAN] > SECONDS_TO_TIMER(15)) {
        gSaveContext.timerStates[TIMER_ID_POSTMAN] = TIMER_STATE_POSTMAN_STOP;
        gSaveContext.postmanTimerStopOsTime = osGetTime();
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        AudioSfx_MuteBanks(0);
        Audio_SetMainBgmVolume(0x7F, 5);
        Message_StartTextbox(play, 0x2791, &this->actor);
        this->unk_2B4 = 0x2791;
        this->unk_2AC = 7;
        gSaveContext.timerStates[TIMER_ID_POSTMAN] = TIMER_STATE_OFF;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Audio_PlaySfx(NA_SE_SY_START_SHOT);
        func_80A6F9C8(this);
    } else {
        Actor_OfferTalk(&this->actor, play, this->actor.xzDistToPlayer + 10.0f);
        Player_SetAutoLockOnActor(play, &this->actor);
        if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, Player_GetMask(play) == PLAYER_MASK_BUNNY, player)) {
            Audio_PlaySfx(NA_SE_SY_STOPWATCH_TIMER_INF - SFX_FLAG);
        } else {
            Audio_PlaySfx(NA_SE_SY_STOPWATCH_TIMER_3 - SFX_FLAG);
        }
    }
}

void func_80A6FE1C(EnMm3* this) {
    this->actionFunc = func_80A6FE30;
}

void func_80A6FE30(EnMm3* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_POSTMAN_COUNTING_GAME_HEART_PIECE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_POSTMAN_COUNTING_GAME_HEART_PIECE);
        }
        this->actor.parent = NULL;
        func_80A6FED8(this);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_POSTMAN_COUNTING_GAME_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
    }
}

void func_80A6FED8(EnMm3* this) {
    this->actionFunc = func_80A6FEEC;
}

void func_80A6FEEC(EnMm3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        Message_StartTextbox(play, 0x2794, &this->actor);
        this->unk_2B4 = 0x2794;
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMAN_HP);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        func_80A6F9C8(this);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

s32 func_80A6FFAC(EnMm3* this, PlayState* play) {
    switch (GET_PLAYER_FORM) {
        case PLAYER_FORM_HUMAN:
            if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, Player_GetMask(play) == PLAYER_MASK_BUNNY,
                                      GET_PLAYER(play))) {
                if (this->unk_2B2 & 0x10) {
                    return true;
                }
            } else if (this->unk_2B2 & 1) {
                return true;
            }
            break;

        case PLAYER_FORM_DEKU:
            if (this->unk_2B2 & 2) {
                return true;
            }
            break;

        case PLAYER_FORM_ZORA:
            if (this->unk_2B2 & 4) {
                return true;
            }
            break;

        case PLAYER_FORM_GORON:
            if (this->unk_2B2 & 8) {
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

void func_80A70084(EnMm3* this, PlayState* play) {
    switch (GET_PLAYER_FORM) {
        case PLAYER_FORM_HUMAN:
            if (GameInteractor_Should(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, Player_GetMask(play) == PLAYER_MASK_BUNNY,
                                      GET_PLAYER(play))) {
                this->unk_2B2 |= 0x10;
                this->unk_2B2 |= 1;
            } else {
                this->unk_2B2 |= 1;
            }
            break;

        case PLAYER_FORM_DEKU:
            this->unk_2B2 |= 2;
            break;

        case PLAYER_FORM_ZORA:
            this->unk_2B2 |= 4;
            break;

        case PLAYER_FORM_GORON:
            this->unk_2B2 |= 8;
            break;

        default:
            break;
    }
}

void EnMm3_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm3* this = (EnMm3*)thisx;

    this->actionFunc(this, play);

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_2B0 & 1) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnMm3_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnMm3* this = (EnMm3*)thisx;

    if (limbIndex == OBJECT_MM_LIMB_08) {
        rot->x += this->torsoRot.y;
        rot->y -= this->torsoRot.x;
    } else if (limbIndex == OBJECT_MM_LIMB_0F) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
        if ((this->unk_2B0 & 2) && ((play->gameplayFrames % 3) == 0)) {
            Matrix_Translate(40.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        }
    }
    return false;
}

void EnMm3_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnMm3* this = (EnMm3*)thisx;

    if (limbIndex == OBJECT_MM_LIMB_0F) {
        Matrix_MultVec3f(&D_80A704F0, &this->actor.focus.pos);
    }
}

void EnMm3_Draw(Actor* thisx, PlayState* play) {
    EnMm3* this = (EnMm3*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80A704FC[this->unk_1DC]));
    gSPSegment(POLY_OPA_DISP++, 0x0C, sEnMm3DL);

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMm3_OverrideLimbDraw, EnMm3_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

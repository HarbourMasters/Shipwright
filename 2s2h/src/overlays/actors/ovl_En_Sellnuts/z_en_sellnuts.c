/*
 * File: z_en_sellnuts.c
 * Overlay: ovl_En_Sellnuts
 * Description: Business Scrub Carrying Bags
 */

#include "z_en_sellnuts.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnSellnuts_Init(Actor* thisx, PlayState* play);
void EnSellnuts_Destroy(Actor* thisx, PlayState* play);
void EnSellnuts_Update(Actor* thisx, PlayState* play);
void EnSellnuts_Draw(Actor* thisx, PlayState* play);

void func_80ADB4F4(EnSellnuts* this, PlayState* play);
void func_80ADB544(EnSellnuts* this, PlayState* play);
void func_80ADB924(EnSellnuts* this, PlayState* play);
void func_80ADBAB8(EnSellnuts* this, PlayState* play);
void func_80ADBBEC(EnSellnuts* this, PlayState* play);
void func_80ADBCE4(EnSellnuts* this, PlayState* play);
void func_80ADBD64(EnSellnuts* this, PlayState* play);
void func_80ADBE80(EnSellnuts* this, PlayState* play);
void func_80ADBFA0(EnSellnuts* this, PlayState* play);
void func_80ADC2CC(EnSellnuts* this, PlayState* play);
void func_80ADC37C(EnSellnuts* this, PlayState* play);
void func_80ADC580(EnSellnuts* this, PlayState* play);
void func_80ADC6D0(EnSellnuts* this, PlayState* play);
void func_80ADC7B4(EnSellnuts* this, PlayState* play);
void func_80ADC8C4(EnSellnuts* this, PlayState* play);
void func_80ADCA64(EnSellnuts* this, PlayState* play);
s32 EnSellnuts_HasReachedPoint(EnSellnuts* this, Path* path, s32 pointIndex);
f32 func_80ADCFE8(Path* path, s32 arg1, Vec3f* pos, Vec3s* arg3);

static u16 D_80ADD910[] = { 0x0614, 0x060E, 0x0628 };

static u16 D_80ADD918[] = { 0x0616, 0x0610, 0x0629 };

static u16 D_80ADD920[] = { 0x0618, 0x0611, 0x062A };

static u16 D_80ADD928[] = { 0x0617, 0x0612, 0x0612 };

static u16 D_80ADD930[] = { 0x0619, 0x0613, 0x0613 };

static u16 D_80ADD938[] = { 0x0615, 0x060F, 0x060F };

static u8 D_80ADD940 = 0;

ActorProfile En_Sellnuts_Profile = {
    /**/ ACTOR_EN_SELLNUTS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNT,
    /**/ sizeof(EnSellnuts),
    /**/ EnSellnuts_Init,
    /**/ EnSellnuts_Destroy,
    /**/ EnSellnuts_Update,
    /**/ EnSellnuts_Draw,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 27, 32, 0, { 0, 0, 0 } },
};

typedef enum EnSellnutsAnimation {
    /* -1 */ ENSELLNUTS_ANIM_NONE = -1,
    /*  0 */ ENSELLNUTS_ANIM_0,
    /*  1 */ ENSELLNUTS_ANIM_1,
    /*  2 */ ENSELLNUTS_ANIM_2,
    /*  3 */ ENSELLNUTS_ANIM_3,
    /*  4 */ ENSELLNUTS_ANIM_4,
    /*  5 */ ENSELLNUTS_ANIM_5,
    /*  6 */ ENSELLNUTS_ANIM_6,
    /*  7 */ ENSELLNUTS_ANIM_7,
    /*  8 */ ENSELLNUTS_ANIM_8,
    /*  9 */ ENSELLNUTS_ANIM_9,
    /* 10 */ ENSELLNUTS_ANIM_10,
    /* 11 */ ENSELLNUTS_ANIM_11,
    /* 12 */ ENSELLNUTS_ANIM_12,
    /* 13 */ ENSELLNUTS_ANIM_13,
    /* 14 */ ENSELLNUTS_ANIM_14,
    /* 15 */ ENSELLNUTS_ANIM_15,
    /* 16 */ ENSELLNUTS_ANIM_16,
    /* 17 */ ENSELLNUTS_ANIM_17,
    /* 18 */ ENSELLNUTS_ANIM_18,
    /* 19 */ ENSELLNUTS_ANIM_19,
    /* 20 */ ENSELLNUTS_ANIM_20,
    /* 21 */ ENSELLNUTS_ANIM_21,
    /* 22 */ ENSELLNUTS_ANIM_22,
    /* 23 */ ENSELLNUTS_ANIM_MAX
} EnSellnutsAnimation;

static AnimationInfoS sAnimationInfo[ENSELLNUTS_ANIM_MAX] = {
    { &gBusinessScrubStandingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // ENSELLNUTS_ANIM_0
    { &gBusinessScrubWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },             // ENSELLNUTS_ANIM_1
    { &gBusinessScrubRiseUpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSELLNUTS_ANIM_2
    { &gBusinessScrubJumpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },             // ENSELLNUTS_ANIM_3
    { &gBusinessScrubBurrowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSELLNUTS_ANIM_4
    { &gBusinessScrubExcitedStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },     // ENSELLNUTS_ANIM_5
    { &gBusinessScrubExcitedLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },      // ENSELLNUTS_ANIM_6
    { &gBusinessScrubExcitedEndAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },       // ENSELLNUTS_ANIM_7
    { &gBusinessScrubTakeOffHatAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },       // ENSELLNUTS_ANIM_8
    { &gBusinessScrubFlyStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // ENSELLNUTS_ANIM_9
    { &gBusinessScrubFlyLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // ENSELLNUTS_ANIM_10
    { &gBusinessScrubShockedStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },     // ENSELLNUTS_ANIM_11
    { &gBusinessScrubShockedShakeHeadAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // ENSELLNUTS_ANIM_12
    { &gBusinessScrubShockedPoundAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },     // ENSELLNUTS_ANIM_13
    { &gBusinessScrubShockedEndAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },       // ENSELLNUTS_ANIM_14
    { &gBusinessScrubThinkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // ENSELLNUTS_ANIM_15
    { &gBusinessScrubBobAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },             // ENSELLNUTS_ANIM_16
    { &gBusinessScrubBurrowAnim, 1.0f, 8, -1, ANIMMODE_ONCE, 0 },           // ENSELLNUTS_ANIM_17
    { &gBusinessScrubBurrowAnim, 1.0f, 4, -1, ANIMMODE_ONCE, -4 },          // ENSELLNUTS_ANIM_18
    { &gBusinessScrubBurrowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSELLNUTS_ANIM_19
    { &gBusinessScrubFlyLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // ENSELLNUTS_ANIM_20
    { &gBusinessScrubTakeOffHatAnim, -1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // ENSELLNUTS_ANIM_21
    { &gBusinessScrubFlyEndAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSELLNUTS_ANIM_22
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void func_80ADADD0(EnSellnuts* this, PlayState* play) {
    if (this->unk_338 & 2) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->unk_338 & 1) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }
}

void func_80ADAE64(EnSellnuts* this) {
    s16 curFrame = this->skelAnime.curFrame;

    switch (curFrame) {
        case 10:
            this->unk_350 = 1;
            this->unk_354 = 0.1f;
            this->unk_358 = 0.1f;
            this->unk_35C = 0.1f;
            break;

        case 11:
        case 12:
            this->unk_354 += 0.15f;
            this->unk_358 += 0.32f;
            this->unk_35C += 0.15f;
            break;

        case 13:
            this->unk_354 = 0.55f;
            this->unk_358 = 1.05f;
            this->unk_35C = 0.55f;
            break;

        case 14:
            this->unk_354 = 1.0f;
            this->unk_358 = 2.0f;
            this->unk_35C = 1.0f;
            break;

        case 15:
        case 16:
            this->unk_358 -= 0.33f;
            break;

        case 17:
            this->unk_358 = 1.0f;
            break;

        case 18:
        case 19:
            this->unk_358 += 0.27f;
            break;

        case 20:
            this->unk_350 = 3;
            this->unk_358 = 1.8f;
            break;

        case 21:
        case 22:
        case 23:
            this->unk_358 -= 0.2f;
            break;

        case 24:
            this->unk_358 = 1.0f;
            break;

        default:
            break;
    }
}

void func_80ADAFC0(EnSellnuts* this) {
    s16 curFrame = this->skelAnime.curFrame;

    switch (curFrame) {
        case 10:
            this->unk_350 = 0;
            break;

        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            this->unk_350 = 1;
            this->unk_354 -= 0.1f;
            this->unk_358 -= 0.1f;
            this->unk_35C -= 0.1f;
            break;

        default:
            break;
    }
}

Actor* func_80ADB040(PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actor != NULL) {
        if ((actor->id == ACTOR_EN_SELLNUTS) && !ENSELLNUTS_GET_1(actor)) {
            return actor;
        }
        actor = actor->next;
    }

    return NULL;
}

f32 func_80ADB08C(PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Actor* sp18 = func_80ADB040(play);

    if (sp18 != NULL) {
        return Math_Vec3f_DistXZ(&player->actor.world.pos, &sp18->home.pos);
    }

    return 80.0f;
}

void func_80ADB0D8(EnSellnuts* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == endFrame) {
        switch (this->unk_340) {
            case 0x611:
            case 0x62A:
                this->animIndex = ENSELLNUTS_ANIM_0;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_0);
                this->actionFunc = func_80ADBFA0;
                break;

            case 0x618:
                if (this->animIndex == ENSELLNUTS_ANIM_1) {
                    this->animIndex = ENSELLNUTS_ANIM_0;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_0);
                    this->actionFunc = func_80ADB544;
                } else {
                    this->animIndex = ENSELLNUTS_ANIM_1;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_1);
                    this->actionFunc = func_80ADBFA0;
                }
                break;

            case 0xFF:
            case 0x60E:
            case 0x60F:
            case 0x614:
            case 0x628:
                this->animIndex = ENSELLNUTS_ANIM_0;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_0);
                this->actionFunc = func_80ADB924;
                break;

            case 0x610:
            case 0x615:
            case 0x616:
            case 0x629:
                this->animIndex = ENSELLNUTS_ANIM_1;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_1);
                this->actionFunc = func_80ADB924;
                break;

            case 0x613:
            case 0x619:
                this->animIndex = ENSELLNUTS_ANIM_7;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_7);
                this->actionFunc = func_80ADBD64;
                break;

            default:
                break;
        }
    }
}

void func_80ADB254(EnSellnuts* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f) ? true : false) &&
        ((this->actor.xzDistToPlayer < 200.0f) ? true : false)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->actionFunc = func_80ADB4F4;
        this->animIndex = ENSELLNUTS_ANIM_3;
        this->collider.dim.height = 64;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_3);
    } else if (curFrame == endFrame) {
        if ((this->animIndex == ENSELLNUTS_ANIM_4) || (this->animIndex == ENSELLNUTS_ANIM_18)) {
            this->animIndex = ENSELLNUTS_ANIM_17;
            this->collider.dim.height = 0;
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_17);
        } else if (this->animIndex == ENSELLNUTS_ANIM_2) {
            this->animIndex = ENSELLNUTS_ANIM_16;
            this->collider.dim.height = 32;
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_16);
        } else if (this->animIndex == ENSELLNUTS_ANIM_17) {
            if (DECR(this->unk_34E) == 0) {
                this->unk_34E = Rand_ZeroOne() * 10.0f;
                this->animIndex = ENSELLNUTS_ANIM_2;
                this->collider.dim.height = 32;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_2);
            }
        } else if (this->animIndex == ENSELLNUTS_ANIM_16) {
            if (DECR(this->unk_34E) == 0) {
                this->unk_34E = Rand_S16Offset(40, 40);
                this->animIndex = ENSELLNUTS_ANIM_18;
                this->collider.dim.height = 32;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_18);
            }
        }
    }
}

void func_80ADB4F4(EnSellnuts* this, PlayState* play) {
    if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
        this->actionFunc = func_80ADB544;
        // Does not set `animIndex`
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_0);
    }
}

void func_80ADB544(EnSellnuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x7D0, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (Player_GetExchangeItemAction(play) == PLAYER_IA_MOONS_TEAR) {
            player->actor.textId = D_80ADD928[this->unk_33A];
            this->unk_340 = player->actor.textId;
            this->actionFunc = func_80ADBAB8;
        } else {
            switch (this->unk_340) {
                case 0x60E:
                    SET_WEEKEVENTREG(WEEKEVENTREG_17_20);
                    SET_WEEKEVENTREG(WEEKEVENTREG_86_04);
                    Message_StartTextbox(play, this->unk_340, &this->actor);
                    this->actionFunc = func_80ADB0D8;
                    break;

                case 0x628:
                    SET_WEEKEVENTREG(WEEKEVENTREG_77_40);
                    SET_WEEKEVENTREG(WEEKEVENTREG_86_04);
                    Message_StartTextbox(play, this->unk_340, &this->actor);
                    this->actionFunc = func_80ADB0D8;
                    break;

                case 0x614:
                    SET_WEEKEVENTREG(WEEKEVENTREG_17_40);
                    Message_StartTextbox(play, this->unk_340, &this->actor);
                    this->actionFunc = func_80ADB0D8;
                    break;

                case 0x610:
                case 0x616:
                case 0x629:
                    Message_StartTextbox(play, this->unk_340, &this->actor);
                    this->actionFunc = func_80ADB0D8;
                    break;

                default:
                    this->actionFunc = func_80ADBAB8;
                    break;
            }
        }
    } else if (((this->actor.xzDistToPlayer < 80.0f) &&
                (((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) ? true : false)) ||
               this->actor.isLockedOn) {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 80.0f, PLAYER_IA_MOONS_TEAR);
        if (player->transformation == PLAYER_FORM_DEKU) {
            if (gSaveContext.save.day == 3) {
                this->unk_33A = 2;
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_40)) {
                    this->unk_340 = D_80ADD918[this->unk_33A];
                } else {
                    this->unk_340 = D_80ADD910[this->unk_33A];
                }
            } else {
                this->unk_33A = 1;
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_17_20)) {
                    this->unk_340 = D_80ADD918[this->unk_33A];
                } else {
                    this->unk_340 = D_80ADD910[this->unk_33A];
                }
            }
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_17_40)) {
            this->unk_340 = D_80ADD918[this->unk_33A];
        } else {
            this->unk_340 = D_80ADD910[this->unk_33A];
        }
    }

    if (!(((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) ? true : false) ||
        !((this->actor.xzDistToPlayer < 200.0f) ? true : false)) {
        this->animIndex = ENSELLNUTS_ANIM_4;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_4);
        this->actionFunc = func_80ADB254;
    }
}

void func_80ADB924(EnSellnuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u8 talkState = Message_GetState(&play->msgCtx);
    PlayerItemAction itemAction;

    if (talkState == TEXT_STATE_PAUSE_MENU) {
        itemAction = func_80123810(play);

        if (itemAction > PLAYER_IA_NONE) {
            if (itemAction == PLAYER_IA_MOONS_TEAR) {
                player->actor.textId = D_80ADD928[this->unk_33A];
                this->unk_340 = player->actor.textId;
                player->exchangeItemAction = itemAction;
                this->actionFunc = func_80ADBAB8;
            } else {
                player->actor.textId = D_80ADD920[this->unk_33A];
                this->unk_340 = player->actor.textId;
                this->actionFunc = func_80ADB0D8;
            }
            Message_CloseTextbox(play);
        } else if (itemAction <= PLAYER_IA_MINUS1) {
            this->unk_340 = D_80ADD920[this->unk_33A];
            Message_ContinueTextbox(play, this->unk_340);
            this->actionFunc = func_80ADB0D8;
        }
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_340 == D_80ADD910[this->unk_33A]) {
            this->unk_340 = D_80ADD938[this->unk_33A];
            Message_ContinueTextbox(play, this->unk_340);
            this->actionFunc = func_80ADB0D8;
        } else {
            this->unk_340 = 0xFF;
            Message_ContinueTextbox(play, this->unk_340);
            this->actionFunc = func_80ADB0D8;
        }
    }
}

void func_80ADBAB8(EnSellnuts* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->unk_368 == 0x28) {
        this->animIndex = ENSELLNUTS_ANIM_5;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_5);
    }

    this->unk_368++;

    if ((curFrame == endFrame) && (this->animIndex == ENSELLNUTS_ANIM_5)) {
        this->animIndex = ENSELLNUTS_ANIM_6;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_6);
    }

    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->actionFunc = func_80ADBBEC;
        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_19);
    }
}

void func_80ADBBEC(EnSellnuts* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_LAND_TITLE_DEED);
        this->actionFunc = func_80ADBCE4;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_DEED_LAND, 300.0f, 300.0f);
    }
}

void func_80ADBC60(EnSellnuts* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, this->unk_340, &this->actor);
        this->actionFunc = func_80ADB0D8;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
        this->unk_340 = D_80ADD930[this->unk_33A];
    }
}

void func_80ADBCE4(EnSellnuts* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
        this->unk_340 = D_80ADD930[this->unk_33A];
        this->actionFunc = func_80ADBC60;
    }
}

void func_80ADBD64(EnSellnuts* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((curFrame == endFrame) && (this->animIndex == ENSELLNUTS_ANIM_7)) {
        this->animIndex = ENSELLNUTS_ANIM_0;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_0);
    }

    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->unk_338 &= ~2;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->animIndex = ENSELLNUTS_ANIM_8;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_8);
        this->actionFunc = func_80ADBE80;
    }
}

void func_80ADBE80(EnSellnuts* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    func_80ADAE64(this);
    if (this->unk_366 == 0) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_366 = 1;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->csId);
            return;
        }
    }

    if (curFrame == endFrame) {
        this->unk_350 = 4;
        this->animIndex = ENSELLNUTS_ANIM_19;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_19);
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
        this->unk_338 &= ~1;
        this->unk_338 |= 8;
        this->unk_32C = this->actor.world.pos.y;
        this->actionFunc = func_80ADC2CC;
    }
}

void func_80ADBFA0(EnSellnuts* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        if (this->animIndex == ENSELLNUTS_ANIM_0) {
            this->actionFunc = func_80ADB544;
        } else {
            this->actionFunc = func_80ADB0D8;
        }
    }
}

void func_80ADC034(EnSellnuts* this, PlayState* play) {
    this->unk_344 += this->unk_364;
    if (this->unk_360 >= 1.0f) {
        this->unk_360 = 1.0f;
        this->actor.velocity.y = 5.0f;
    } else {
        this->actor.velocity.y = 5.0f;
        this->unk_360 += 0.1f;
    }

    if ((this->actor.home.pos.y + 200.0f) < this->actor.world.pos.y) {
        Math_ApproachF(&this->actor.velocity.y, 0.0f, 0.2f, 1.0f);
        this->animIndex = ENSELLNUTS_ANIM_10;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_10);
        this->unk_368 = 0;
        this->actionFunc = func_80ADC37C;
    }
}

void func_80ADC118(EnSellnuts* this, PlayState* play) {
    Vec3f sp34;
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == endFrame) {
        Math_SmoothStepToS(&this->unk_364, 0x1C71, 3, 0x100, 0);
        this->unk_344 += this->unk_364;
        this->actor.shape.yOffset = 1500.0f;
        Math_SmoothStepToS(&this->actor.shape.rot.y, 0x4000, 3, 0x7D0, 0);
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (DECR(this->unk_34A) == 0) {
        if (!(this->unk_328 & 3)) {
            sp34 = this->actor.world.pos;
            func_800B14D4(play, 20.0f, &sp34);
        }
        this->actor.velocity.y = 5.0f;
    } else if (!(this->unk_328 & 3)) {
        sp34.x = this->actor.world.pos.x;
        sp34.y = this->unk_32C;
        sp34.z = this->actor.world.pos.z;
        func_800B14D4(play, 20.0f, &sp34);
    }

    if ((this->actor.home.pos.y + 22.5f) < this->actor.world.pos.y) {
        this->animIndex = ENSELLNUTS_ANIM_9;
        this->unk_360 = 0.3f;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_9);
        Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->actionFunc = func_80ADC034;
    }
}

void func_80ADC2CC(EnSellnuts* this, PlayState* play) {
    f32 sp24 = this->unk_32C - this->actor.world.pos.y;
    Vec3f sp18;

    this->unk_344 += this->unk_364;
    if (!(this->unk_328 & 3)) {
        sp18.x = this->actor.world.pos.x;
        sp18.y = this->unk_32C;
        sp18.z = this->actor.world.pos.z;
        func_800B14D4(play, 20.0f, &sp18);
    }

    if (sp24 > 5.0f) {
        this->actionFunc = func_80ADC118;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
    }
}

void func_80ADC37C(EnSellnuts* this, PlayState* play) {
    Vec3s sp30;
    f32 sp2C;

    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
    if (this->path != NULL) {
        sp2C = func_80ADCFE8(this->path, this->unk_334, &this->actor.world.pos, &sp30);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp30.y = this->actor.wallYaw;
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, sp30.y, 0xA, 0x12C, 0);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->unk_342 = 0x1000;
        this->unk_344 += this->unk_364;
        this->actor.world.rot.x = -sp30.x;
        if (EnSellnuts_HasReachedPoint(this, this->path, this->unk_334) && (sp2C < 500.0f)) {
            if (this->unk_334 >= (this->path->count - 1)) {
                CutsceneManager_Stop(this->csId);
                this->actionFunc = func_80ADC580;
            } else {
                this->unk_334++;
            }
        }
    } else if (this->actor.playerHeightRel > 500.0f) {
        CutsceneManager_Stop(this->csId);
        this->actionFunc = func_80ADC580;
    }

    Math_ApproachF(&this->actor.speed, 2.0f, 0.2f, 1.0f);
    Actor_MoveWithoutGravity(&this->actor);
    if (this->unk_366 == 2) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_366 = 3;
        } else {
            CutsceneManager_Queue(this->csId);
            return;
        }
    } else if ((this->unk_366 == 1) && (this->unk_368 == 20)) {
        CutsceneManager_Stop(this->csId);
        this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        CutsceneManager_Queue(this->csId);
        this->unk_366 = 2;
    }

    this->unk_368++;
}

void func_80ADC580(EnSellnuts* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

void func_80ADC5A4(EnSellnuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        player->speedXZ = 0.0f;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Message_StartTextbox(play, this->unk_340, &this->actor);
        if (this->unk_340 == 0x625) {
            this->unk_338 |= 1;
            this->actor.draw = EnSellnuts_Draw;
            D_80ADD940 = 0;
            player->stateFlags1 |= PLAYER_STATE1_20;
            this->actionFunc = func_80ADC7B4;
        } else {
            player->stateFlags1 &= ~PLAYER_STATE1_20;
            this->actionFunc = func_80ADC6D0;
        }
    } else if (func_80ADB08C(play) < 80.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, this->actor.xzDistToPlayer);
    }
}

void func_80ADC6D0(EnSellnuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        if (player->transformation == PLAYER_FORM_DEKU) {
            if (gSaveContext.save.day == 3) {
                this->unk_33A = 2;
            } else {
                this->unk_33A = 1;
            }
        } else {
            this->unk_33A = 0;
        }
        this->unk_340 = D_80ADD910[this->unk_33A];
        this->actionFunc = func_80ADB544;
    }
}

void func_80ADC7B4(EnSellnuts* this, PlayState* play) {
    s32 talkState = Message_GetState(&play->msgCtx);

    if (this->unk_366 == 0) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_366 = 1;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->csId);
        }
    } else if ((this->unk_366 == 1) && (talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->unk_366 = 0;
        CutsceneManager_Stop(this->csId);
        this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        CutsceneManager_Queue(this->csId);
        this->unk_338 |= 8;
        this->actionFunc = func_80ADC8C4;
    }
}

void func_80ADC8C4(EnSellnuts* this, PlayState* play) {
    Vec3s sp30;

    if (this->unk_366 == 0) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_366 = 1;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->csId);
            return;
        }
    }

    if (this->path != NULL) {
        func_80ADCFE8(this->path, this->unk_334, &this->actor.world.pos, &sp30);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp30.y = this->actor.wallYaw;
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, sp30.y, 0xA, 0x12C, 0);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->unk_342 = 0x1000;
        this->unk_344 += 0x1C71;
        this->actor.world.rot.x = -sp30.x;
        if (EnSellnuts_HasReachedPoint(this, this->path, this->unk_334)) {
            if (this->unk_334 >= (this->path->count - 1)) {
                this->animIndex = ENSELLNUTS_ANIM_22;
                this->actor.gravity = -1.0f;
                this->actor.velocity.y = -1.0f;
                this->actor.speed = 0.0f;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
                this->unk_338 &= ~1;
                this->unk_338 &= ~2;
                this->actionFunc = func_80ADCA64;
                return;
            }
            this->unk_334++;
        }
        Math_ApproachF(&this->actor.speed, 2.0f, 0.2f, 1.0f);
        Actor_MoveWithoutGravity(&this->actor);
    }
}

void func_80ADCA64(EnSellnuts* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex == ENSELLNUTS_ANIM_22) {
        Math_SmoothStepToS(&this->unk_364, 0, 3, 0x100, 0);
        this->actor.shape.rot.y += 0x4000;
        this->unk_360 *= 0.93f;
        if (this->actor.world.pos.y < -50.0f) {
            CutsceneManager_Stop(this->csId);
            this->unk_338 &= ~8;
            this->unk_34E = 20;
            this->unk_350 = 4;
            this->animIndex = ENSELLNUTS_ANIM_19;
            this->actor.velocity.y = 0.0f;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
        }
        return;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (curFrame == endFrame) {
        if (this->animIndex == ENSELLNUTS_ANIM_19) {
            this->animIndex = ENSELLNUTS_ANIM_17;
            this->collider.dim.height = 0;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSELLNUTS_ANIM_17);
        } else if (this->animIndex == ENSELLNUTS_ANIM_17) {
            CutsceneManager_Stop(this->csId);
            SET_WEEKEVENTREG(WEEKEVENTREG_73_04);
            Actor_Kill(&this->actor);
        }
    }
}

void func_80ADCC04(EnSellnuts* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex == ENSELLNUTS_ANIM_3) {
        if (curFrame == endFrame) {
            this->unk_350 = 4;
            this->animIndex = ENSELLNUTS_ANIM_21;
            this->unk_354 = 1.0f;
            this->unk_358 = 1.0f;
            this->unk_35C = 1.0f;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
    } else if (this->animIndex == ENSELLNUTS_ANIM_21) {
        func_80ADAFC0(this);
        if (curFrame == 0) {
            if (func_80ADB08C(play) < 9999.0f) {
                this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Actor_OfferTalk(&this->actor, play, 9999.0f);
            }
            this->unk_340 = 0x626;
            this->animIndex = ENSELLNUTS_ANIM_0;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = func_80ADC5A4;
        }
    }
}

void func_80ADCD3C(EnSellnuts* this, PlayState* play) {
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_04)) {
        this->unk_338 |= 2;
        this->unk_338 |= 1;
        this->unk_340 = 0x626;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.gravity = -1.0f;
        this->actor.draw = EnSellnuts_Draw;
        this->unk_34A = 50;
        this->unk_34E = Rand_ZeroOne() * 10.0f;
        this->collider.dim.height = 64;
        this->animIndex = ENSELLNUTS_ANIM_3;
        this->unk_350 = 4;
        Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = func_80ADCC04;
    } else if (D_80ADD940 != 0) {
        this->collider.dim.height = 64;
    }
}

s32 EnSellnuts_HasReachedPoint(EnSellnuts* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

f32 func_80ADCFE8(Path* path, s32 arg1, Vec3f* pos, Vec3s* arg3) {
    s32 pad;
    Vec3f sp20;
    Vec3s* points;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[arg1];
        sp20.x = points->x;
        sp20.y = points->y;
        sp20.z = points->z;
    }

    arg3->y = Math_Vec3f_Yaw(pos, &sp20);
    arg3->x = Math_Vec3f_Pitch(pos, &sp20);
    return sp20.y - pos->y;
}

void EnSellnuts_Init(Actor* thisx, PlayState* play) {
    EnSellnuts* this = (EnSellnuts*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad2;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_LAND_TITLE_DEED) || CHECK_WEEKEVENTREG(WEEKEVENTREG_61_10)) {
        Actor_Kill(&this->actor);
    }

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gBusinessScrubSkel, &gBusinessScrubStandingAnim, this->jointTable,
                       this->morphTable, BUSINESS_SCRUB_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    this->path = SubS_GetPathByIndex(play, ENSELLNUTS_GET_PATH_INDEX(&this->actor), ENSELLNUTS_PATH_INDEX_NONE);
    this->csId = this->actor.csId;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.colChkInfo.cylRadius = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_350 = 0;
    this->unk_366 = 0;
    this->unk_374 = 0.01f;
    this->unk_370 = 0.01f;
    this->unk_36C = 0.01f;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_04)) {
        if (ENSELLNUTS_GET_1(&this->actor)) {
            Actor_Kill(&this->actor);
            return;
        }
        this->unk_338 |= 2;
        this->unk_338 |= 1;
        if (player->transformation == PLAYER_FORM_DEKU) {
            if (gSaveContext.save.day == 3) {
                this->unk_33A = 2;
            } else {
                this->unk_33A = 1;
            }
        } else {
            this->unk_33A = 0;
        }
        this->unk_340 = D_80ADD910[this->unk_33A];
        this->actor.gravity = -1.0f;
        this->unk_34A = 50;
        this->animIndex = ENSELLNUTS_ANIM_4;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = func_80ADB254;
        return;
    }

    D_80ADD940 = 1;
    if (ENSELLNUTS_GET_1(&this->actor)) {
        this->unk_338 |= 2;
        this->unk_338 &= ~1;
        this->unk_340 = 0x625;
        this->actor.gravity = 0.0f;
        this->actor.draw = NULL;
        this->animIndex = ENSELLNUTS_ANIM_20;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->unk_35C = 1.0f;
        this->unk_358 = 1.0f;
        this->unk_354 = 1.0f;
        this->unk_360 = 1.0f;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = func_80ADC5A4;
    } else {
        this->unk_338 |= 2;
        this->unk_338 &= ~1;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.gravity = 0.0f;
        this->actor.draw = NULL;
        this->animIndex = ENSELLNUTS_ANIM_4;
        this->unk_34E = 20;
        this->collider.dim.height = 64;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = func_80ADCD3C;
    }
}

void EnSellnuts_Destroy(Actor* thisx, PlayState* play) {
    EnSellnuts* this = (EnSellnuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnSellnuts_Update(Actor* thisx, PlayState* play) {
    EnSellnuts* this = (EnSellnuts*)thisx;
    Player* player = GET_PLAYER(play);

    this->unk_328++;
    if (player->transformation == PLAYER_FORM_DEKU) {
        if (gSaveContext.save.day == 3) {
            this->unk_33A = 2;
        } else {
            this->unk_33A = 1;
        }
    } else {
        this->unk_33A = 0;
    }

    Actor_SetFocus(&this->actor, 60.0f);
    Actor_SetScale(&this->actor, 0.01f);
    SkelAnime_Update(&this->skelAnime);
    Actor_MoveWithGravity(&this->actor);
    this->actionFunc(this, play);
    if (this->unk_338 & 8) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_AKINDO_FLY - SFX_FLAG);
    }
    func_80ADADD0(this, play);
}

s32 EnSellnuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnSellnuts* this = (EnSellnuts*)thisx;

    if (((this->animIndex == ENSELLNUTS_ANIM_4) && (this->unk_350 == 0)) ||
        ((this->animIndex == ENSELLNUTS_ANIM_8) && (this->unk_350 == 0)) ||
        ((this->animIndex == ENSELLNUTS_ANIM_21) && (this->unk_350 == 0)) || (this->animIndex == ENSELLNUTS_ANIM_18) ||
        (this->animIndex == ENSELLNUTS_ANIM_2) || ((this->animIndex == ENSELLNUTS_ANIM_3) && (this->unk_350 == 0)) ||
        ((this->animIndex == ENSELLNUTS_ANIM_17) && (this->unk_350 == 0)) || (this->animIndex == ENSELLNUTS_ANIM_5) ||
        (this->animIndex == ENSELLNUTS_ANIM_6) || (this->animIndex == ENSELLNUTS_ANIM_7) ||
        (this->animIndex == ENSELLNUTS_ANIM_1) || (this->animIndex == ENSELLNUTS_ANIM_0) ||
        (this->animIndex == ENSELLNUTS_ANIM_16)) {
        if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
            (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_SCALP) ||
            (limbIndex == BUSINESS_SCRUB_LIMB_HAIR) || (limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT)) {
            *dList = NULL;
        }
    } else if ((this->animIndex == ENSELLNUTS_ANIM_8) || (this->animIndex == ENSELLNUTS_ANIM_21) ||
               (this->animIndex == ENSELLNUTS_ANIM_19) ||
               ((this->animIndex == ENSELLNUTS_ANIM_3) && (this->unk_350 == 4)) ||
               ((this->animIndex == ENSELLNUTS_ANIM_17) && (this->unk_350 == 4))) {
        switch (this->unk_350) {
            case 1:
                if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT)) {
                    *dList = NULL;
                }
                break;

            case 3:
            case 4:
                if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT)) {
                    *dList = NULL;
                }
                break;

            case 5:
                if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT)) {
                    *dList = NULL;
                }
                break;

            default:
                break;
        }
    } else if ((this->animIndex == ENSELLNUTS_ANIM_9) || (this->animIndex == ENSELLNUTS_ANIM_10)) {
        if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT)) {
            *dList = NULL;
        }
    } else if (((this->animIndex == ENSELLNUTS_ANIM_22) || (this->animIndex == ENSELLNUTS_ANIM_20)) &&
               ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT) ||
                (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG))) {
        *dList = NULL;
    }

    if (limbIndex == BUSINESS_SCRUB_LIMB_HAIR) {
        Matrix_RotateXS(this->unk_342, MTXMODE_APPLY);
    }

    if (limbIndex == BUSINESS_SCRUB_LIMB_EYES) {
        if ((this->animIndex == ENSELLNUTS_ANIM_6) || (this->animIndex == ENSELLNUTS_ANIM_5) ||
            (this->animIndex == ENSELLNUTS_ANIM_7)) {
            *dList = gBusinessScrubEyesWideDL;
        } else {
            *dList = gBusinessScrubEyesDL;
        }
    }

    return false;
}

void EnSellnuts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnSellnuts_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnSellnuts* this = (EnSellnuts*)thisx;

    if (((this->unk_350 == 1) || (this->unk_350 == 3)) &&
        ((limbIndex == BUSINESS_SCRUB_LIMB_SCALP) || (limbIndex == BUSINESS_SCRUB_LIMB_HAIR))) {
        Matrix_Scale(this->unk_354, this->unk_358, this->unk_35C, MTXMODE_APPLY);
    }

    if (((this->animIndex == ENSELLNUTS_ANIM_9) || (this->animIndex == ENSELLNUTS_ANIM_20) ||
         (this->animIndex == ENSELLNUTS_ANIM_22)) &&
        ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
         (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG))) {
        Matrix_Scale(this->unk_360, this->unk_360, this->unk_360, MTXMODE_APPLY);
    }

    if (limbIndex == BUSINESS_SCRUB_LIMB_HAIR) {
        Matrix_RotateYS(this->unk_344, MTXMODE_APPLY);
    }
}

void EnSellnuts_Draw(Actor* thisx, PlayState* play) {
    EnSellnuts* this = (EnSellnuts*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnSellnuts_OverrideLimbDraw, EnSellnuts_PostLimbDraw,
                                   EnSellnuts_TransformLimbDraw, &this->actor);
}

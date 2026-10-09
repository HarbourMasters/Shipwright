/*
 * File: z_en_scopenuts.c
 * Overlay: ovl_En_Scopenuts
 * Description: Business Scrub that sells you a Heart Piece
 */

#include "z_en_scopenuts.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnScopenuts_Init(Actor* thisx, PlayState* play);
void EnScopenuts_Destroy(Actor* thisx, PlayState* play);
void EnScopenuts_Update(Actor* thisx, PlayState* play);
void EnScopenuts_Draw(Actor* thisx, PlayState* play);

void func_80BCB078(EnScopenuts* this, PlayState* play);
void func_80BCB1C8(EnScopenuts* this, PlayState* play);
void func_80BCB4DC(EnScopenuts* this, PlayState* play);
void func_80BCB52C(EnScopenuts* this, PlayState* play);
void func_80BCB6D0(EnScopenuts* this, PlayState* play);
void func_80BCB90C(EnScopenuts* this, PlayState* play);
void func_80BCB980(EnScopenuts* this, PlayState* play);
void func_80BCBA00(EnScopenuts* this, PlayState* play);
void func_80BCBC60(EnScopenuts* this, PlayState* play);
void func_80BCBD28(EnScopenuts* this, PlayState* play);
void func_80BCBF0C(EnScopenuts* this, PlayState* play);
void func_80BCBFFC(EnScopenuts* this, PlayState* play);
void func_80BCC288(EnScopenuts* this, PlayState* play);
s32 EnScopenuts_HasReachedPoint(EnScopenuts* this, Path* path, s32 pointIndex);
f32 func_80BCC448(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3);

ActorProfile En_Scopenuts_Profile = {
    /**/ ACTOR_EN_SCOPENUTS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNT,
    /**/ sizeof(EnScopenuts),
    /**/ EnScopenuts_Init,
    /**/ EnScopenuts_Destroy,
    /**/ EnScopenuts_Update,
    /**/ EnScopenuts_Draw,
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

typedef enum EnScopenutsAnimation {
    /* -1 */ ENSCOPENUTS_ANIM_NONE = -1,
    /*  0 */ ENSCOPENUTS_ANIM_0,
    /*  1 */ ENSCOPENUTS_ANIM_1,
    /*  2 */ ENSCOPENUTS_ANIM_2,
    /*  3 */ ENSCOPENUTS_ANIM_3,
    /*  4 */ ENSCOPENUTS_ANIM_4,
    /*  5 */ ENSCOPENUTS_ANIM_5,
    /*  6 */ ENSCOPENUTS_ANIM_6,
    /*  7 */ ENSCOPENUTS_ANIM_7,
    /*  8 */ ENSCOPENUTS_ANIM_8,
    /*  9 */ ENSCOPENUTS_ANIM_9,
    /* 10 */ ENSCOPENUTS_ANIM_10,
    /* 11 */ ENSCOPENUTS_ANIM_11,
    /* 12 */ ENSCOPENUTS_ANIM_12,
    /* 13 */ ENSCOPENUTS_ANIM_13,
    /* 14 */ ENSCOPENUTS_ANIM_14,
    /* 15 */ ENSCOPENUTS_ANIM_15,
    /* 16 */ ENSCOPENUTS_ANIM_16,
    /* 17 */ ENSCOPENUTS_ANIM_17,
    /* 18 */ ENSCOPENUTS_ANIM_18,
    /* 19 */ ENSCOPENUTS_ANIM_19,
    /* 20 */ ENSCOPENUTS_ANIM_20,
    /* 21 */ ENSCOPENUTS_ANIM_21,
    /* 22 */ ENSCOPENUTS_ANIM_22,
    /* 23 */ ENSCOPENUTS_ANIM_MAX
} EnScopenutsAnimation;

static AnimationInfoS sAnimationInfo[ENSCOPENUTS_ANIM_MAX] = {
    { &gBusinessScrubStandingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // ENSCOPENUTS_ANIM_0
    { &gBusinessScrubWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },             // ENSCOPENUTS_ANIM_1
    { &gBusinessScrubRiseUpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSCOPENUTS_ANIM_2
    { &gBusinessScrubJumpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },             // ENSCOPENUTS_ANIM_3
    { &gBusinessScrubBurrowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSCOPENUTS_ANIM_4
    { &gBusinessScrubExcitedStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },     // ENSCOPENUTS_ANIM_5
    { &gBusinessScrubExcitedLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },      // ENSCOPENUTS_ANIM_6
    { &gBusinessScrubExcitedEndAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },       // ENSCOPENUTS_ANIM_7
    { &gBusinessScrubTakeOffHatAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },       // ENSCOPENUTS_ANIM_8
    { &gBusinessScrubFlyStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // ENSCOPENUTS_ANIM_9
    { &gBusinessScrubFlyLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // ENSCOPENUTS_ANIM_10
    { &gBusinessScrubShockedStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },     // ENSCOPENUTS_ANIM_11
    { &gBusinessScrubShockedShakeHeadAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // ENSCOPENUTS_ANIM_12
    { &gBusinessScrubShockedPoundAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },     // ENSCOPENUTS_ANIM_13
    { &gBusinessScrubShockedEndAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },       // ENSCOPENUTS_ANIM_14
    { &gBusinessScrubThinkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // ENSCOPENUTS_ANIM_15
    { &gBusinessScrubBobAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },             // ENSCOPENUTS_ANIM_16
    { &gBusinessScrubBurrowAnim, 1.0f, 8, -1, ANIMMODE_ONCE, 0 },           // ENSCOPENUTS_ANIM_17
    { &gBusinessScrubBurrowAnim, 1.0f, 4, -1, ANIMMODE_ONCE, -4 },          // ENSCOPENUTS_ANIM_18
    { &gBusinessScrubBurrowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSCOPENUTS_ANIM_19
    { &gBusinessScrubFlyLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // ENSCOPENUTS_ANIM_20
    { &gBusinessScrubTakeOffHatAnim, -1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // ENSCOPENUTS_ANIM_21
    { &gBusinessScrubFlyEndAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // ENSCOPENUTS_ANIM_22
};

Gfx* D_80BCCCDC[] = { gKakeraLeafMiddleDL, gKakeraLeafTipDL };

Vec3f D_80BCCCE4 = { 0.0f, -0.5f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

s16 func_80BCABF0(Path* path) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    Vec3f sp28;
    Vec3f sp1C;

    Math_Vec3s_ToVec3f(&sp28, &points[0]);
    Math_Vec3s_ToVec3f(&sp1C, &points[1]);
    return Math_Vec3f_Yaw(&sp28, &sp1C);
}

void func_80BCAC40(EnScopenuts* this, PlayState* play) {
    s16 i;
    Vec3f sp60;
    Vec3f sp54;

    sp60 = this->actor.world.pos;
    sp60.y += 100.0f;

    for (i = 0; i < 36; i++) {
        sp54.x = Rand_Centered() * 10.0f;
        sp54.y = 2.0f * Rand_Centered();
        sp54.z = Rand_Centered() * 10.0f;
        EffectSsHahen_Spawn(play, &sp60, &sp54, &D_80BCCCE4, 0, 150, GAMEPLAY_KEEP, 16, D_80BCCCDC[i & 1]);
    }
}

void func_80BCAD64(EnScopenuts* this, s16 arg1) {
    f32 sp24 = Math_CosS(this->actor.world.rot.x) * this->actor.speed;

    switch (arg1) {
        case 1:
            this->actor.velocity.y = this->actor.speed;
            this->actor.velocity.x = 0.0f;
            this->actor.velocity.z = 0.0f;
            break;

        case 2:
            this->actor.velocity.x = Math_SinS(this->actor.world.rot.y) * this->actor.speed;
            this->actor.velocity.y = 0.0f;
            this->actor.velocity.z = Math_CosS(this->actor.world.rot.y) * this->actor.speed;
            break;

        case 3:
            this->actor.velocity.x = 0.0f;
            this->actor.velocity.y = 0.0f;
            this->actor.velocity.z = 0.0f;
            break;

        default:
            this->actor.velocity.x = Math_SinS(this->actor.world.rot.y) * sp24;
            this->actor.velocity.y = Math_SinS(this->actor.world.rot.x) * this->actor.speed;
            this->actor.velocity.z = Math_CosS(this->actor.world.rot.y) * sp24;
            break;
    }
    Actor_UpdatePos(&this->actor);
}

void func_80BCAE78(EnScopenuts* this, PlayState* play) {
    if (this->unk_328 & 4) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->unk_328 & 2) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }
}

s16 func_80BCAF0C(EnScopenuts* this) {
    switch (this->unk_33C) {
        case 0x0:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BUSINESS_SCRUB_HEART_PIECE)) {
                this->unk_328 |= 1;
                return 0x1638;
            }
            return 0x162F;

        case 0x162F:
            return 0x1630;

        case 0x1630:
            return 0x1631;

        case 0x1631:
            this->unk_358 = 150;
            return 0x1632;

        case 0x1633:
            this->unk_358 = 100;
            return 0x1634;

        default:
            return 0;
    }
}

void func_80BCAFA8(EnScopenuts* this, PlayState* play) {
    Vec3f screenPos;

    Play_GetScreenPos(play, &this->actor.world.pos, &screenPos);
    if ((screenPos.x >= 130.0f) && (screenPos.x < (SCREEN_WIDTH - 130.0f)) && (screenPos.y >= 90.0f)) {
        if (screenPos.y < (SCREEN_HEIGHT - 90.0f)) {
            this->actor.draw = EnScopenuts_Draw;
            this->animIndex = ENSCOPENUTS_ANIM_10;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_10);
            this->actionFunc = func_80BCB078;
        }
    }
}

void func_80BCB078(EnScopenuts* this, PlayState* play) {
    Vec3s sp30;

    if (this->path != NULL) {
        func_80BCC448(this->path, this->unk_334, &this->actor.world.pos, &sp30);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp30.y = this->actor.wallYaw;
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, sp30.y, 10, 0x12C, 0);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->unk_33E = 0x1000;
        this->unk_340 += 0x1C71;
        this->actor.world.rot.x = -sp30.x;
        if (EnScopenuts_HasReachedPoint(this, this->path, this->unk_334)) {
            if (this->unk_334 >= (this->path->count - 1)) {
                this->actionFunc = func_80BCB1C8;
                this->actor.speed = 0.0f;
                this->actor.gravity = -1.0f;
                return;
            }
            this->unk_334++;
        }
    }

    if (this->unk_334 >= (this->path->count - 2)) {
        Math_ApproachF(&this->actor.speed, 1.5f, 0.2f, 1.0f);
    } else {
        Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 1.0f);
    }
    Actor_MoveWithoutGravity(&this->actor);
}

void func_80BCB1C8(EnScopenuts* this, PlayState* play) {
    this->unk_350 *= 0.92f;
    Actor_SetScale(&this->actor, this->unk_350);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        SET_WEEKEVENTREG(WEEKEVENTREG_52_40);
        Actor_Kill(&this->actor);
    }
}

void func_80BCB230(EnScopenuts* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);

    if ((((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) ? true : false) &&
        ((this->actor.xzDistToPlayer < 200.0f) ? true : false)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->actionFunc = func_80BCB4DC;
        this->animIndex = ENSCOPENUTS_ANIM_3;
        this->collider.dim.height = 64;
        func_80BCAC40(this, play);
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_3);
    } else if (curFrame == endFrame) {
        if ((this->animIndex == ENSCOPENUTS_ANIM_4) || (this->animIndex == ENSCOPENUTS_ANIM_18)) {
            this->animIndex = ENSCOPENUTS_ANIM_17;
            this->collider.dim.height = 0;
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_17);
        } else if (this->animIndex == ENSCOPENUTS_ANIM_2) {
            this->animIndex = ENSCOPENUTS_ANIM_16;
            this->collider.dim.height = 32;
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_16);
        } else if (this->animIndex == ENSCOPENUTS_ANIM_17) {
            if (DECR(this->unk_34E) == 0) {
                this->unk_34E = Rand_ZeroOne() * 10.0f;
                this->animIndex = ENSCOPENUTS_ANIM_2;
                this->collider.dim.height = 32;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_2);
            }
        } else if ((this->animIndex == ENSCOPENUTS_ANIM_16) && (DECR(this->unk_34E) == 0)) {
            this->unk_34E = Rand_S16Offset(40, 40);
            this->animIndex = ENSCOPENUTS_ANIM_18;
            this->collider.dim.height = 32;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_18);
        }
    }
}

void func_80BCB4DC(EnScopenuts* this, PlayState* play) {
    if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
        this->actionFunc = func_80BCB52C;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_0);
    }
}

void func_80BCB52C(EnScopenuts* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x7D0, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_33C = func_80BCAF0C(this);
        Message_StartTextbox(play, this->unk_33C, &this->actor);
        this->actionFunc = func_80BCB6D0;
    } else if (((this->actor.xzDistToPlayer < 100.0f) &&
                (((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) ? true : false)) ||
               this->actor.isLockedOn) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    } else if (!(((this->actor.playerHeightRel < 50.0f) && (this->actor.playerHeightRel > -50.0f)) ? true : false) ||
               !((this->actor.xzDistToPlayer < 200.0f) ? true : false)) {
        this->animIndex = ENSCOPENUTS_ANIM_4;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_4);
        this->actionFunc = func_80BCB230;
    }
}

void func_80BCB6D0(EnScopenuts* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            if (this->unk_328 & 1) {
                this->unk_328 &= ~1;
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->unk_328 &= ~4;
                this->animIndex = ENSCOPENUTS_ANIM_8;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_8);
                this->actionFunc = func_80BCBA00;
            } else {
                this->unk_33C = func_80BCAF0C(this);
                Message_StartTextbox(play, this->unk_33C, &this->actor);
            }
        }
    } else if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    if (gSaveContext.save.saveInfo.playerData.rupees < this->unk_358) {
                        Audio_PlaySfx(NA_SE_SY_ERROR);
                        this->unk_33C = 0x1636;
                        this->unk_328 |= 1;
                        Message_StartTextbox(play, this->unk_33C, &this->actor);
                    } else {
                        Audio_PlaySfx_MessageDecide();
                        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                        play->msgCtx.stateTimer = 4;
                        Rupees_ChangeBy(-this->unk_358);
                        this->actionFunc = func_80BCB90C;
                    }
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    if (this->unk_358 == 150) {
                        this->unk_33C = 0x1633;
                    } else {
                        this->unk_33C = 0x1635;
                        this->unk_328 |= 1;
                    }
                    Message_StartTextbox(play, this->unk_33C, &this->actor);
                    break;

                default:
                    break;
            }
        }
    } else if (talkState == TEXT_STATE_DONE) {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
        this->actionFunc = func_80BCB980;
    }
}

void func_80BCB90C(EnScopenuts* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BUSINESS_SCRUB_HEART_PIECE);
        this->actionFunc = func_80BCB6D0;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 300.0f, 300.0f);
    }
}

void func_80BCB980(EnScopenuts* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_33C = 0x1637;
        this->unk_328 |= 1;
        Message_StartTextbox(play, this->unk_33C, &this->actor);
        this->actionFunc = func_80BCB6D0;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void func_80BCBA00(EnScopenuts* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    switch (curFrame) {
        case 10:
            this->unk_35A = 1;
            this->unk_35C = 0.1f;
            this->unk_360 = 0.1f;
            this->unk_364 = 0.1f;
            break;

        case 11:
        case 12:
            this->unk_35C += 0.15f;
            this->unk_360 += 0.32f;
            this->unk_364 += 0.15f;
            break;

        case 13:
            this->unk_35C = 0.55f;
            this->unk_360 = 1.05f;
            this->unk_364 = 0.55f;
            break;

        case 14:
            this->unk_35C = 1.0f;
            this->unk_360 = 2.0f;
            this->unk_364 = 1.0f;
            break;

        case 15:
        case 16:
            this->unk_360 -= 0.33f;
            break;

        case 17:
            this->unk_360 = 1.0f;
            break;

        case 18:
        case 19:
            this->unk_360 += 0.27f;
            break;

        case 20:
            this->unk_35A = 2;
            this->unk_360 = 1.8f;
            break;

        case 21:
        case 22:
        case 23:
            this->unk_360 -= 0.2f;
            break;

        case 24:
            this->unk_360 = 1.0f;
            break;

        default:
            break;
    }

    if (this->unk_36C == 0) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_36C = 1;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->csId);
            return;
        }
    }

    if (curFrame == endFrame) {
        this->unk_35A = 3;
        this->animIndex = ENSCOPENUTS_ANIM_19;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_19);
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
        this->unk_328 &= ~2;
        this->unk_34E = 50;
        this->unk_328 |= 8;
        this->unk_32C = this->actor.world.pos.y;
        this->actionFunc = func_80BCBC60;
    }
}

void func_80BCBC60(EnScopenuts* this, PlayState* play) {
    f32 sp24;
    Vec3f sp18;

    this->unk_340 += this->unk_34C;
    sp24 = this->unk_32C - this->actor.world.pos.y;

    if (!(this->unk_370 & 3)) {
        sp18.x = this->actor.world.pos.x;
        sp18.y = this->unk_32C;
        sp18.z = this->actor.world.pos.z;
        func_800B14D4(play, 20.0f, &sp18);
    }

    if (sp24 > 5.0f) {
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        this->unk_372 = func_80BCABF0(this->path);
        this->actionFunc = func_80BCBD28;
    }
}

void func_80BCBD28(EnScopenuts* this, PlayState* play) {
    Vec3f sp44;
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);
    Vec3s sp38;

    func_80BCC448(this->path, this->unk_334, &this->actor.world.pos, &sp38);
    if (curFrame == endFrame) {
        Math_SmoothStepToS(&this->unk_34C, 0x1C71, 3, 0x100, 0);
        this->unk_340 += this->unk_34C;
        this->actor.shape.yOffset = 1500.0f;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->unk_372, 3, 0x7D0, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (DECR(this->unk_34E) == 0) {
        if (!(this->unk_370 & 3)) {
            sp44 = this->actor.world.pos;
            func_800B14D4(play, 20.0f, &sp44);
        }
        this->actor.velocity.y = 5.0f;
    } else if (!(this->unk_370 & 3)) {
        sp44.x = this->actor.world.pos.x;
        sp44.y = this->unk_32C;
        sp44.z = this->actor.world.pos.z;
        func_800B14D4(play, 20.0f, &sp44);
    }

    if ((this->actor.home.pos.y + 22.5f) < this->actor.world.pos.y) {
        this->unk_368 = 0.3f;
        this->animIndex = ENSCOPENUTS_ANIM_9;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_9);
        Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        func_80BCAC40(this, play);
        this->actionFunc = func_80BCBF0C;
    }
}

void func_80BCBF0C(EnScopenuts* this, PlayState* play) {
    this->unk_340 += this->unk_34C;
    if (this->unk_368 >= 1.0f) {
        this->unk_368 = 1.0f;
        this->actor.velocity.y = 5.0f;
    } else {
        this->actor.velocity.y = 5.0f;
        this->unk_368 += 0.1f;
    }

    if ((this->actor.home.pos.y + 50.0f) < this->actor.world.pos.y) {
        Math_ApproachF(&this->actor.velocity.y, 0.0f, 0.2f, 1.0f);
        this->animIndex = ENSCOPENUTS_ANIM_10;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_10);
        this->unk_328 |= 2;
        this->unk_36E = 0;
        this->actionFunc = func_80BCBFFC;
    }
}

void func_80BCBFFC(EnScopenuts* this, PlayState* play) {
    Vec3s sp38;
    f32 sp34;
    s16 sp32 = 0;

    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;

    if (this->path != NULL) {
        sp34 = func_80BCC448(this->path, this->unk_334, &this->actor.world.pos, &sp38);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp38.y = this->actor.wallYaw;
        }

        if (this->unk_334 < 6) {
            Math_SmoothStepToS(&this->actor.world.rot.y, sp38.y, 10, 0x12C, 0);
        } else {
            Math_SmoothStepToS(&this->actor.world.rot.y, sp38.y, 10, 0x5DC, 0);
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->unk_33E = 0x1000;
        this->unk_340 += this->unk_34C;
        this->actor.world.rot.x = -sp38.x;

        if (EnScopenuts_HasReachedPoint(this, this->path, this->unk_334)) {
            sp32 = 1;
        }

        if (sp34 < 10.0f) {
            if (sp32 == 1) {
                sp32 = 3;
            } else {
                sp32 = 2;
            }
        }

        if (sp32 == 3) {
            if (this->unk_334 >= (this->path->count - 1)) {
                CutsceneManager_Stop(this->csId);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_52_40);
                this->actionFunc = func_80BCC288;
            } else {
                this->unk_334++;
            }
        }
    } else if (this->actor.playerHeightRel > 500.0f) {
        CutsceneManager_Stop(this->csId);
        this->actionFunc = func_80BCC288;
    }

    Math_ApproachF(&this->actor.speed, 1.0f, 0.2f, 1.0f);
    func_80BCAD64(this, sp32);

    if (this->unk_36C == 2) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            this->unk_36C = 3;
        } else {
            CutsceneManager_Queue(this->csId);
            return;
        }
    } else if ((this->unk_36C == 1) && (this->unk_36E == 20)) {
        CutsceneManager_Stop(this->csId);
        this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        CutsceneManager_Queue(this->csId);
        this->unk_36C = 2;
    }
    this->unk_36E++;
}

void func_80BCC288(EnScopenuts* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

s32 EnScopenuts_HasReachedPoint(EnScopenuts* this, Path* path, s32 pointIndex) {
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

f32 func_80BCC448(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3) {
    Vec3s* points;
    Vec3f sp20;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[arg1];
        sp20.x = points[0].x;
        sp20.y = points[0].y;
        sp20.z = points[0].z;
    }

    arg3->y = Math_Vec3f_Yaw(arg2, &sp20);
    arg3->x = Math_Vec3f_Pitch(arg2, &sp20);

    return sp20.y - arg2->y;
}

void EnScopenuts_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnScopenuts* this = (EnScopenuts*)thisx;

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_74_40) &&
        GameInteractor_Should(VB_SCOPENUTS_CONSIDER_FIRST_CYCLE,
                              gSaveContext.save.saveInfo.inventory.items[ITEM_OCARINA_OF_TIME] == ITEM_NONE)) {
        Actor_Kill(&this->actor);
        return;
    }

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gBusinessScrubSkel, &gBusinessScrubStandingAnim, this->jointTable,
                       this->morphTable, BUSINESS_SCRUB_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    this->unk_350 = 0.01f;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.colChkInfo.cylRadius = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_328 |= 2;
    this->unk_328 |= 4;
    this->actor.speed = 0.0f;

    if (ENSCOPENUTS_GET_3E0(&this->actor) == ENSCOPENUTS_3E0_0) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_52_40)) {
            Actor_Kill(&this->actor);
        } else if (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) {
            this->path =
                SubS_GetPathByIndex(play, ENSCOPENUTS_GET_PATH_INDEX(&this->actor), ENSCOPENUTS_PATH_INDEX_NONE);
            this->actor.draw = NULL;
            this->actionFunc = func_80BCAFA8;
            this->actor.gravity = 0.0f;
        } else {
            Actor_Kill(&this->actor);
        }
    } else if (ENSCOPENUTS_GET_3E0(&this->actor) == ENSCOPENUTS_3E0_1) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_52_40)) {
            this->path =
                SubS_GetPathByIndex(play, ENSCOPENUTS_GET_PATH_INDEX(&this->actor), ENSCOPENUTS_PATH_INDEX_NONE);
            if (this->path == NULL) {
                Actor_Kill(&this->actor);
            } else {
                this->actor.gravity = -1.0f;
                this->actor.draw = EnScopenuts_Draw;
                this->csId = this->actor.csId;
                this->unk_33C = 0;
                this->unk_358 = 150;
                this->animIndex = ENSCOPENUTS_ANIM_4;
                this->unk_35A = 0;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSCOPENUTS_ANIM_4);
                this->actionFunc = func_80BCB230;
            }
        } else {
            Actor_Kill(&this->actor);
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnScopenuts_Destroy(Actor* thisx, PlayState* play) {
    EnScopenuts* this = (EnScopenuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnScopenuts_Update(Actor* thisx, PlayState* play) {
    EnScopenuts* this = (EnScopenuts*)thisx;

    Actor_SetFocus(&this->actor, 60.0f);
    SkelAnime_Update(&this->skelAnime);
    Actor_MoveWithGravity(&this->actor);

    this->actionFunc(this, play);

    if (this->unk_328 & 8) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_AKINDO_FLY - SFX_FLAG);
    }
    func_80BCAE78(this, play);
}

s32 EnScopenuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnScopenuts* this = (EnScopenuts*)thisx;

    if (((this->animIndex == ENSCOPENUTS_ANIM_4) && (this->unk_35A == 0)) ||
        ((this->animIndex == ENSCOPENUTS_ANIM_8) && (this->unk_35A == 0)) || (this->animIndex == ENSCOPENUTS_ANIM_18) ||
        (this->animIndex == ENSCOPENUTS_ANIM_2) || (this->animIndex == ENSCOPENUTS_ANIM_3) ||
        (this->animIndex == ENSCOPENUTS_ANIM_17) || (this->animIndex == ENSCOPENUTS_ANIM_16)) {
        if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
            (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_SCALP) ||
            (limbIndex == BUSINESS_SCRUB_LIMB_HAIR) || (limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT)) {
            *dList = NULL;
        }
    } else if ((this->animIndex == ENSCOPENUTS_ANIM_8) || (this->animIndex == ENSCOPENUTS_ANIM_19)) {
        switch (this->unk_35A) {
            case 1:
                if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT)) {
                    *dList = NULL;
                }
                break;

            case 2:
            case 3:
                if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG) ||
                    (limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT)) {
                    *dList = NULL;
                }
                break;

            default:
                break;
        }
    } else if (((this->animIndex == ENSCOPENUTS_ANIM_9) || (this->animIndex == ENSCOPENUTS_ANIM_10)) &&
               ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT) || (limbIndex == BUSINESS_SCRUB_LIMB_HAT))) {
        *dList = NULL;
    }

    if (limbIndex == BUSINESS_SCRUB_LIMB_EYES) {
        if ((this->unk_33C == 0x162F) || (this->unk_33C == 0x1630)) {
            *dList = gBusinessScrubEyesSquintDL;
        } else {
            *dList = gBusinessScrubEyesDL;
        }
    }

    return false;
}

void EnScopenuts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnScopenuts_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnScopenuts* this = (EnScopenuts*)thisx;

    if (((this->unk_35A == 1) || (this->unk_35A == 2)) &&
        ((limbIndex == BUSINESS_SCRUB_LIMB_SCALP) || (limbIndex == BUSINESS_SCRUB_LIMB_HAIR))) {
        Matrix_Scale(this->unk_35C, this->unk_360, this->unk_364, MTXMODE_APPLY);
    }

    if ((this->animIndex == ENSCOPENUTS_ANIM_9) &&
        ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
         (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG))) {
        Matrix_Scale(this->unk_368, this->unk_368, this->unk_368, MTXMODE_APPLY);
    }

    if (limbIndex == BUSINESS_SCRUB_LIMB_HAIR) {
        Matrix_RotateYS(this->unk_340, MTXMODE_APPLY);
    }
}

void EnScopenuts_Draw(Actor* thisx, PlayState* play) {
    EnScopenuts* this = (EnScopenuts*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnScopenuts_OverrideLimbDraw, EnScopenuts_PostLimbDraw,
                                   EnScopenuts_TransformLimbDraw, &this->actor);
}

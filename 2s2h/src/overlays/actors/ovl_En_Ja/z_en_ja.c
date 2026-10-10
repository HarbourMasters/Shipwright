/*
 * File: z_en_ja.c
 * Overlay: ovl_En_Ja
 * Description: Juggler
 */

#include "z_en_ja.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnJa_Init(Actor* thisx, PlayState* play);
void EnJa_Destroy(Actor* thisx, PlayState* play);
void EnJa_Update(Actor* thisx, PlayState* play);
void EnJa_Draw(Actor* thisx, PlayState* play);

void func_80BC21A8(EnJa* this, PlayState* play);
void func_80BC22F4(EnJa* this, PlayState* play);
void func_80BC2EA4(EnJa* this);
void func_80BC32D8(EnJa* this, PlayState* play);
void func_80BC3594(EnJa* this, PlayState* play);

static u8 D_80BC35F0[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x05 - 0x04),
    /* 0x04 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x05 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 0, 18, 0, 0x13 - 0x0B),
    /* 0x0B */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_YADOYA, 0x12 - 0x0F),
    /* 0x0F */ SCHEDULE_CMD_RET_VAL_L(2),
    /* 0x12 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x13 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_TOWN, 0x1A - 0x17),
    /* 0x17 */ SCHEDULE_CMD_RET_VAL_L(1),
    /* 0x1A */ SCHEDULE_CMD_RET_NONE(),
};

MsgScript D_80BC360C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2937),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2938),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2939),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x000F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293A),
    /* 0x0012 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0013 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0016 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293B),
    /* 0x001A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BC362C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2940),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2941),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2942),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x000F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2943),
    /* 0x0012 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0013 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0016 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293B),
    /* 0x001A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BC364C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293C),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293D),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293E),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x000F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293F),
    /* 0x0012 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0013 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0016 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293B),
    /* 0x001A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BC366C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2944),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2945),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2946),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x000F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2947),
    /* 0x0012 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0013 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0016 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x293B),
    /* 0x001A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Ja_Profile = {
    /**/ ACTOR_EN_JA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BOJ,
    /**/ sizeof(EnJa),
    /**/ EnJa_Init,
    /**/ EnJa_Destroy,
    /**/ EnJa_Update,
    /**/ EnJa_Draw,
};

Vec3f D_80BC36AC = { -10.0f, 56.0f, 25.0f };
Vec3f D_80BC36B8 = { 0.0f, 60.0f, 20.0f };
Vec3f D_80BC36C4 = { 10.0f, 60.0f, 25.0f };
Vec3f D_80BC36D0 = { 0.0f, 58.0f, 20.0f };

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 12, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnJaAnimation {
    /* -1 */ ENJA_ANIM_NONE = -1,
    /*  0 */ ENJA_ANIM_0,
    /*  1 */ ENJA_ANIM_1,
    /*  2 */ ENJA_ANIM_2,
    /*  3 */ ENJA_ANIM_3,
    /*  4 */ ENJA_ANIM_4,
    /*  5 */ ENJA_ANIM_5,
    /*  6 */ ENJA_ANIM_MAX
} EnJaAnimation;

static AnimationInfoS sAnimationInfo[ENJA_ANIM_MAX] = {
    { &object_boj_Anim_002734, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENJA_ANIM_0
    { &object_boj_Anim_0033B0, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENJA_ANIM_1
    { &object_boj_Anim_002734, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENJA_ANIM_2
    { &object_boj_Anim_0033B0, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENJA_ANIM_3
    { &object_boj_Anim_004078, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENJA_ANIM_4
    { &object_boj_Anim_005CE4, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENJA_ANIM_5
};

void EnJa_UpdateSkelAnime(EnJa* this) {
    this->skelAnime.playSpeed = this->animPlaySpeed;
    SkelAnime_Update(&this->skelAnime);
}

s32 EnJa_ChangeAnim(EnJa* this, s32 animIndex) {
    s32 didAnimChange = false;

    if (this->animIndex != animIndex) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        this->animPlaySpeed = this->skelAnime.playSpeed;
    }

    return didAnimChange;
}

void func_80BC1984(EnJa* this, PlayState* play) {
    s32 pad[2];

    Collider_UpdateCylinder(&this->actor, &this->collider);
    this->collider.dim.height = TRUNCF_BINANG(fabsf(this->actor.focus.pos.y - this->actor.world.pos.y)) + 5;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 func_80BC19FC(EnJa* this, PlayState* play) {
    s32 ret = false;

    if (((this->unk_340 & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        SubS_SetOfferMode(&this->unk_340, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->actionFunc = func_80BC22F4;
        ret = true;
    }
    return ret;
}

void func_80BC1A68(EnJa* this) {
    if (DECR(this->unk_360) == 0) {
        this->unk_362++;
        if (this->unk_362 >= 4) {
            this->unk_360 = Rand_S16Offset(30, 30);
            this->unk_362 = 0;
        }
    }
}

Actor* func_80BC1AE0(EnJa* this, PlayState* play) {
    Actor* ja = SubS_FindNearestActor(&this->actor, play, ACTORCAT_NPC, ACTOR_EN_JA);
    Vec3f sp30;
    Vec3f sp24;

    if (ja != NULL) {
        this->actor.child = ja;
        Math_Vec3f_Copy(&sp24, &ja->world.pos);
        Math_Vec3f_Copy(&sp30, &this->actor.world.pos);
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp30, &sp24);
    } else {
        this->actor.child = NULL;
    }
    return this->actor.child;
}

Actor* func_80BC1B50(EnJa* this, PlayState* play) {
    return &GET_PLAYER(play)->actor;
}

s32 func_80BC1B60(EnJa* this, PlayState* play) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    s16 sp32;

    Math_Vec3f_Copy(&sp40, &this->unk_1E0->world.pos);
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
    sp32 = Math_Vec3f_Yaw(&sp34, &sp40);
    Math_ApproachS(&this->unk_356, (sp32 - this->unk_35A) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_356 = CLAMP(this->unk_356, -0x1FFE, 0x1FFE);

    Math_ApproachS(&this->unk_35A, (sp32 - this->unk_356) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_35A = CLAMP(this->unk_35A, -0x1C70, 0x1C70);

    if (this->unk_1E0->id == ACTOR_PLAYER) {
        sp40.y = ((Player*)this->unk_1E0)->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&sp40, &this->unk_1E0->focus.pos);
    }

    Math_Vec3f_Copy(&sp34, &this->actor.focus.pos);
    Math_ApproachS(&this->unk_354, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_358, 4, 0x2AA8);
    this->unk_354 = CLAMP(this->unk_354, -0x1C70, 0x1C70);

    Math_ApproachS(&this->unk_358, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_354, 4, 0x2AA8);
    this->unk_358 = CLAMP(this->unk_358, -0x1C70, 0x1C70);

    return true;
}

s32 func_80BC1D70(EnJa* this, PlayState* play) {
    if (this->unk_340 & 8) {
        this->unk_1E0 = func_80BC1B50(this, play);
        if (this->unk_1E0 != NULL) {
            func_80BC1B60(this, play);
        }
        this->unk_340 &= ~0x10;
        this->unk_340 |= 0x20;
    } else if (this->unk_340 & 0x20) {
        this->unk_340 &= ~0x20;
        this->unk_354 = 0;
        this->unk_356 = 0;
        this->unk_358 = 0;
        this->unk_35A = 0;
        this->unk_35E = 20;
    } else if (DECR(this->unk_35E) == 0) {
        this->unk_340 |= 0x10;
        this->unk_35E = 20;
    }
    return true;
}

void func_80BC1E40(EnJa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 talkState = Message_GetState(&play->msgCtx);
    f32 phi_f0;

    if (((play->msgCtx.currentTextId < 0xFF) || (play->msgCtx.currentTextId > 0x200)) &&
        (talkState == TEXT_STATE_FADING) && (this->prevTalkState == TEXT_STATE_FADING) &&
        (&this->actor == player->talkActor)) {
        if ((play->state.frames % 2) == 0) {
            if (this->unk_348 != 0.0f) {
                this->unk_348 = 0.0f;
            } else {
                this->unk_348 = 1.0f;
            }
        }
    } else {
        this->unk_348 = 0.0f;
    }

    this->unk_34C += (this->unk_348 != 0.0f) ? 60.0f : -60.0f;
    this->unk_34C = CLAMP(this->unk_34C, 0.0f, 120.0f);

    Matrix_Translate(this->unk_34C, 0.0f, 0.0f, MTXMODE_APPLY);
    this->prevTalkState = talkState;
}

s32 func_80BC1FC8(EnJa* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;

    if (func_80BC1AE0(this, play) != NULL) {
        SubS_SetOfferMode(&this->unk_340, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_340 |= 0x10;
        EnJa_ChangeAnim(this, ENJA_ANIM_5);
        func_80BC2EA4(this);
        ret = true;
    }
    return ret;
}

s32 func_80BC203C(EnJa* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;

    if (func_80BC1AE0(this, play) != NULL) {
        if (ENJA_GET_3(&this->actor) == 0) {
            EnJa_ChangeAnim(this, ENJA_ANIM_1);
        } else {
            EnJa_ChangeAnim(this, ENJA_ANIM_4);
        }
        SubS_SetOfferMode(&this->unk_340, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->actor.shape.shadowDraw = NULL;
        this->unk_340 |= 0x50;
        ret = true;
    }
    return ret;
}

s32 func_80BC20D0(EnJa* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;

    this->unk_340 = 0;

    switch (scheduleOutput->result) {
        case 1:
            ret = func_80BC1FC8(this, play, scheduleOutput);
            if (ret == true) {}
            break;

        case 2:
            ret = func_80BC203C(this, play, scheduleOutput);
            break;

        default:
            break;
    }
    return ret;
}

s32 func_80BC213C(EnJa* this, PlayState* play) {
    return true;
}

void func_80BC2150(EnJa* this, PlayState* play) {
    if ((this->scheduleResult == 1) || (this->scheduleResult == 2)) {
        func_80BC213C(this, play);
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 4, 0x1554);
}

void func_80BC21A8(EnJa* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    this->unk_35C = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);
    if (!Schedule_RunScript(play, D_80BC35F0, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) && !func_80BC20D0(this, play, &scheduleOutput))) {
        this->actor.shape.shadowDraw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = 0;
    } else {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->scheduleResult = scheduleOutput.result;
    func_80BC2150(this, play);
}

MsgScript* EnJa_GetMsgScript(EnJa* this, PlayState* play) {
    switch (this->scheduleResult) {
        case 1:
            if (ENJA_GET_3(&this->actor) == 0) {
                return D_80BC360C;
            }
            return D_80BC364C;

        case 2:
            if (ENJA_GET_3(&this->actor) == 0) {
                return D_80BC362C;
            }
            return D_80BC366C;

        default:
            return D_80BC360C;
    }
}

void func_80BC22F4(EnJa* this, PlayState* play) {
    if (MsgEvent_RunScript(&this->actor, play, EnJa_GetMsgScript(this, play), this->msgScriptCallback,
                           &this->msgScriptPos)) {
        this->unk_340 &= ~8;
        SubS_SetOfferMode(&this->unk_340, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->msgScriptPos = 0;
        this->unk_340 |= 0x10;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actionFunc = func_80BC21A8;
    }
}

void EnJa_Init(Actor* thisx, PlayState* play) {
    EnJa* this = (EnJa*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_boj_Skel_00C240, NULL, this->jointTable, this->morphTable,
                       OBJECT_BOJ_LIMB_MAX);
    this->animIndex = ENJA_ANIM_NONE;
    EnJa_ChangeAnim(this, ENJA_ANIM_0);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.cullingVolumeDistance = 800.0f;
    this->actor.gravity = 0.0f;
    SubS_SetOfferMode(&this->unk_340, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
    this->unk_340 |= 0x10;
    this->scheduleResult = 0;
    this->msgScriptCallback = NULL;
    this->actionFunc = func_80BC21A8;
}

void EnJa_Destroy(Actor* thisx, PlayState* play) {
    EnJa* this = (EnJa*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnJa_Update(Actor* thisx, PlayState* play) {
    EnJa* this = (EnJa*)thisx;
    f32 height;
    f32 radius;

    func_80BC19FC(this, play);

    this->actionFunc(this, play);

    if (this->scheduleResult != 0) {
        EnJa_UpdateSkelAnime(this);
        func_80BC1A68(this);
        func_80BC1D70(this, play);

        radius = this->collider.dim.radius + 30;
        height = this->collider.dim.height + 10;
        SubS_Offer(&this->actor, play, radius, height, PLAYER_IA_NONE, this->unk_340 & SUBS_OFFER_MODE_MASK);

        if (this->scheduleResult != 2) {
            Actor_MoveWithGravity(&this->actor);
            Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
        }
        func_80BC1984(this, play);
    }

    if (this->scheduleResult == 1) {
        func_80BC32D8(this, play);
    }
}

s32 EnJa_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnJa* this = (EnJa*)thisx;

    if (limbIndex == OBJECT_BOJ_LIMB_0F) {
        func_80BC1E40(this, play);
    }
    return false;
}

void EnJa_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80BC3774 = { -87.0f, 444.0f, -49.0f };
    static Vec3f D_80BC3780 = { 600.0f, 0.0f, 0.0f };
    static Vec3f D_80BC378C = { 400.0f, 0.0f, -400.0f };
    static Vec3f D_80BC3798 = { 400.0f, 0.0f, 400.0f };
    static Vec3s D_80BC37A4 = { 0x7770, -0x4BC, -0x251C };
    s32 pad;
    EnJa* this = (EnJa*)thisx;
    s32 pad2;

    if (limbIndex == OBJECT_BOJ_LIMB_0F) {
        Matrix_MultVec3f(&D_80BC3780, &this->actor.focus.pos);
        Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
    } else if ((this->unk_340 & 0x40) && (limbIndex == OBJECT_BOJ_LIMB_0B)) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        Matrix_TranslateRotateZYX(&D_80BC3774, &D_80BC37A4);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_boj_DL_00BA30);

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (this->scheduleResult == 1) {
        if ((limbIndex == OBJECT_BOJ_LIMB_0B) &&
            (((this->skelAnime.curFrame >= 0.0f) && (this->skelAnime.curFrame <= 6.0f)) ||
             ((this->skelAnime.curFrame >= 35.0f) && (this->skelAnime.curFrame <= 47.0f)))) {
            OPEN_DISPS(play->state.gfxCtx);

            Matrix_Push();
            Matrix_Translate(D_80BC378C.x, D_80BC378C.y, D_80BC378C.z, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);

            gDPPipeSync(POLY_OPA_DISP++);

            switch (this->unk_364) {
                case 0:
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 255, 255);
                    break;

                case 1:
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 0, 0, 255);
                    break;

                case 2:
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 255, 255);
                    break;

                case 3:
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);
                    break;

                default:
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                    break;
            }

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, object_boj_DL_00BCC8);

            Matrix_Pop();

            CLOSE_DISPS(play->state.gfxCtx);
        } else if (limbIndex == OBJECT_BOJ_LIMB_0E) {
            if ((this->skelAnime.curFrame >= 0.0f) && (this->skelAnime.curFrame <= 18.0f)) {
                OPEN_DISPS(play->state.gfxCtx);

                Matrix_Push();

                Matrix_Translate(D_80BC3798.x, D_80BC3798.y, D_80BC3798.z, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&play->billboardMtxF);

                gDPPipeSync(POLY_OPA_DISP++);

                switch (this->unk_366) {
                    case 0:
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 255, 255);
                        break;

                    case 1:
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 0, 0, 255);
                        break;

                    case 2:
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 255, 255);
                        break;

                    case 3:
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);
                        break;

                    default:
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                        break;
                }

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, object_boj_DL_00BCC8);

                Matrix_Pop();

                CLOSE_DISPS(play->state.gfxCtx);
            }
        }
    }
}

void EnJa_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnJa* this = (EnJa*)thisx;
    s32 stepRot;
    s32 overrideRot;

    if (this->unk_340 & 0x10) {
        stepRot = false;
    } else {
        stepRot = true;
    }

    if (this->unk_340 & 0x20) {
        overrideRot = true;
    } else {
        overrideRot = false;
    }

    if (!stepRot) {
        overrideRot = false;
    }

    switch (limbIndex) {
        case OBJECT_BOJ_LIMB_0F:
            SubS_UpdateLimb(this->unk_354 + this->unk_358 + 0x4000,
                            this->unk_356 + this->unk_35A + this->actor.shape.rot.y + 0x4000, &this->unk_1EC,
                            &this->unk_274, stepRot, overrideRot);
            Matrix_Pop();
            Matrix_Translate(this->unk_1EC.x, this->unk_1EC.y, this->unk_1EC.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->unk_274.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk_274.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->unk_274.z, MTXMODE_APPLY);
            Matrix_Push();
            break;

        case OBJECT_BOJ_LIMB_08:
            SubS_UpdateLimb(this->unk_358 + 0x4000, this->unk_35A + this->actor.shape.rot.y + 0x4000, &this->unk_1F8,
                            &this->unk_27A, stepRot, overrideRot);
            Matrix_Pop();
            Matrix_Translate(this->unk_1F8.x, this->unk_1F8.y, this->unk_1F8.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->unk_27A.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk_27A.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->unk_27A.z, MTXMODE_APPLY);
            Matrix_Push();
            break;

        default:
            break;
    }
}

void EnJa_Draw(Actor* thisx, PlayState* play) {
    static Color_RGBA8 D_80BC37AC[] = {
        { 200, 0, 80, 0 },
        { 0, 130, 220, 0 },
    };
    static Color_RGBA8 D_80BC37B4[] = {
        { 255, 255, 255, 0 },
        { 255, 255, 255, 0 },
    };
    static TexturePtr D_80BC37BC[] = {
        object_boj_Tex_0062B0,
        object_boj_Tex_0063B0,
        object_boj_Tex_0064B0,
        object_boj_Tex_0063B0,
    };
    s32 pad;
    EnJa* this = (EnJa*)thisx;
    s32 phi_t2;

    if (ENJA_GET_3(&this->actor) == 0) {
        phi_t2 = 0;
    } else {
        phi_t2 = 1;
    }

    if (this->scheduleResult != 0) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08,
                   Gfx_EnvColor(play->state.gfxCtx, D_80BC37AC[phi_t2].r, D_80BC37AC[phi_t2].g, D_80BC37AC[phi_t2].b,
                                D_80BC37AC[phi_t2].a));
        gSPSegment(POLY_OPA_DISP++, 0x09,
                   Gfx_EnvColor(play->state.gfxCtx, D_80BC37B4[phi_t2].r, D_80BC37B4[phi_t2].g, D_80BC37B4[phi_t2].b,
                                D_80BC37B4[phi_t2].a));
        gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(D_80BC37BC[this->unk_362]));
        gDPPipeSync(POLY_OPA_DISP++);

        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnJa_OverrideLimbDraw, EnJa_PostLimbDraw,
                                       EnJa_TransformLimbDraw, &this->actor);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (this->scheduleResult == 1) {
        func_80BC3594(this, play);
    }
}

void func_80BC2EA4(EnJa* this) {
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    f32 sp38;
    s32 i;
    Vec3f sp28;

    sp44 = Math_SinS(this->actor.shape.rot.y);
    sp40 = Math_CosS(this->actor.shape.rot.y);
    sp3C = Math_SinS(this->actor.child->shape.rot.y);
    sp38 = Math_CosS(this->actor.child->shape.rot.y);

    for (i = 0; i < ARRAY_COUNT(this->unk_234); i++) {
        this->unk_234[i].unk_0E = i * 0x30;
        this->unk_234[i].unk_0C = i;
    }

    sp28 = this->actor.world.pos;
    this->unk_210 = sp28;
    this->unk_204 = sp28;

    sp28 = this->actor.child->world.pos;
    this->unk_228 = sp28;
    this->unk_21C = sp28;

    this->unk_204.x += (D_80BC36AC.z * sp44) + (D_80BC36AC.x * sp40);
    this->unk_204.y += D_80BC36AC.y;
    this->unk_204.z += (D_80BC36AC.z * sp40) - (D_80BC36AC.x * sp44);

    this->unk_210.x += (D_80BC36B8.z * sp44) + (D_80BC36B8.x * sp40);
    this->unk_210.y += D_80BC36B8.y;
    this->unk_210.z += (D_80BC36B8.z * sp40) - (D_80BC36B8.x * sp44);

    this->unk_21C.x += (D_80BC36C4.z * sp3C) + (D_80BC36C4.x * sp38);
    this->unk_21C.y += D_80BC36C4.y;
    this->unk_21C.z += (D_80BC36C4.z * sp38) - (D_80BC36C4.x * sp3C);

    this->unk_228.x += (D_80BC36D0.z * sp3C) + (D_80BC36D0.x * sp38);
    this->unk_228.y += D_80BC36D0.y;
    this->unk_228.z += (D_80BC36D0.z * sp38) - (D_80BC36D0.x * sp3C);

    this->unk_364 = 2;
    this->unk_366 = 0;
}

void func_80BC3154(EnJa* this, EnJaStruct* ptr) {
    if ((ptr->unk_0E >= 18) && (ptr->unk_0E < 83)) {
        f32 temp_f0 = ptr->unk_0E - 18;

        ptr->unk_00.x = (((65.0f - temp_f0) * this->unk_204.x) + (temp_f0 * this->unk_228.x)) / 65.0f;
        ptr->unk_00.y = (((65.0f - temp_f0) * this->unk_204.y) + (temp_f0 * this->unk_228.y)) / 65.0f;
        ptr->unk_00.z = (((65.0f - temp_f0) * this->unk_204.z) + (temp_f0 * this->unk_228.z)) / 65.0f;

        temp_f0 -= 32.5f;
        temp_f0 = SQ(temp_f0);

        ptr->unk_00.y += -0.11360947f * temp_f0 + 120.0f;
    } else if ((ptr->unk_0E >= 102) && (ptr->unk_0E < 191)) {
        f32 temp_f0 = ptr->unk_0E - 102;

        ptr->unk_00.x = (((89.0f - temp_f0) * this->unk_21C.x) + (temp_f0 * this->unk_210.x)) / 89.0f;
        ptr->unk_00.y = (((89.0f - temp_f0) * this->unk_21C.y) + (temp_f0 * this->unk_210.y)) / 89.0f;
        ptr->unk_00.z = (((89.0f - temp_f0) * this->unk_21C.z) + (temp_f0 * this->unk_210.z)) / 89.0f;

        temp_f0 -= 44.5f;
        temp_f0 = SQ(temp_f0);

        ptr->unk_00.y += -0.08079788f * temp_f0 + 160.0f;
    }
}

void func_80BC32D8(EnJa* this, PlayState* play) {
    EnJaStruct* phi_s0 = &this->unk_234[0];
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_234); i++, phi_s0++) {
        func_80BC3154(this, phi_s0);
        if (phi_s0->unk_0E != 191) {
            phi_s0->unk_0E++;
        } else {
            phi_s0->unk_0E = 0;
        }
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (this->unk_366 > 0) {
            this->unk_366--;
        } else {
            this->unk_366 = 3;
        }
    } else if (Animation_OnFrame(&this->skelAnime, 34.0f)) {
        if (this->unk_364 > 0) {
            this->unk_364--;
        } else {
            this->unk_364 = 3;
        }
    }
}

void func_80BC33C0(EnJaStruct* ptr, PlayState* play) {
    s32 pad;

    if (((ptr->unk_0E < 0) || (ptr->unk_0E >= 19)) && ((ptr->unk_0E < 83) || (ptr->unk_0E >= 103))) {
        OPEN_DISPS(play->state.gfxCtx);

        FrameInterpolation_RecordOpenChild(ptr, ptr->unk_0C);
        Matrix_Translate(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, MTXMODE_NEW);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);

        switch (ptr->unk_0C) {
            case 0:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 255, 255);
                break;

            case 1:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 0, 0, 255);
                break;

            case 2:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 255, 255);
                break;

            case 3:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);
                break;

            default:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                break;
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_boj_DL_00BCC8);
        FrameInterpolation_RecordCloseChild();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_80BC3594(EnJa* this, PlayState* play) {
    EnJaStruct* phi_s0 = this->unk_234;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_234); i++, phi_s0++) {
        func_80BC33C0(phi_s0, play);
    }
}

/*
 * File: z_en_dnh.c
 * Overlay: ovl_En_Dnh
 * Description: Koume in boat house
 */

#include "z_en_dnh.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnDnh_Init(Actor* thisx, PlayState* play);
void EnDnh_Destroy(Actor* thisx, PlayState* play);
void EnDnh_Update(Actor* thisx, PlayState* play);
void EnDnh_Draw(Actor* thisx, PlayState* play);

void func_80A50F38(EnDnh* this, PlayState* play);
void EnDnh_DoNothing(EnDnh* this, PlayState* play);

MsgScript D_80A51250[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_SAVED_KOUME, 0x000D - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0862),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_TALKED_KOUME_KIOSK_EMPTY),
    /* 0x000C 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x000D 0x03 */ MSCRIPT_CMD_CHECK_HUMAN(0x0015 - 0x0010),
    /* 0x0010 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0867),
    /* 0x0013 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0014 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0015 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE, 0x0094 - 0x001A),
    /* 0x001A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_25_10, 0x004F - 0x001F),
    /* 0x001F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0859),
    /* 0x0022 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0023 0x05 */ MSCRIPT_CMD_CHECK_ITEM(ITEM_PICTOGRAPH_BOX, 0x0040 - 0x0028),
    /* 0x0028 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0863),
    /* 0x002B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002C 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x002D 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_PICTOGRAPH_BOX, 0x0),
    /* 0x0032 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0043),
    /* 0x0035 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0036 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0039 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x088D),
    /* 0x003C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003D 0x03 */ MSCRIPT_CMD_JUMP(0x0047 - 0x0040),
    /* 0x0040 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x088D),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_JUMP(0x0),
    /* 0x0047 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0048 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_25_10),
    /* 0x004B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x004E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x004F 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_26_10, 0x006A - 0x0054),
    /* 0x0054 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x085A),
    /* 0x0057 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_26_10),
    /* 0x005A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005B 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x085B),
    /* 0x005E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x085C),
    /* 0x0062 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0063 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0070 - 0x006A, 0x008E - 0x006A, 0x0070 - 0x006A),
    /* 0x006A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x085B),
    /* 0x006D 0x03 */ MSCRIPT_CMD_JUMP(0x005E - 0x0070),

    /* 0x0070 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x0071 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x085E),
    /* 0x0074 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0075 0x05 */ MSCRIPT_CMD_CHECK_RUPEES(10, 0x007F - 0x007A),
    /* 0x007A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0875),
    /* 0x007D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x007E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x007F 0x03 */ MSCRIPT_CMD_CHANGE_RUPEES(-10),
    /* 0x0082 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0860),
    /* 0x0085 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0086 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0087 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_25_10),
    /* 0x008A 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x008D 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x008E 0x01 */ MSCRIPT_CMD_PLAY_CANCEL(),
    /* 0x008F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x085F),
    /* 0x0092 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0093 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0094 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_25_20, 0x00E2 - 0x0099),
    /* 0x0099 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_26_10, 0x00DB - 0x009E),
    /* 0x009E 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0868),
    /* 0x00A1 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_26_10),
    /* 0x00A4 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A5 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0869),
    /* 0x00A8 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A9 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x086A),
    /* 0x00AC 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00AD 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x086B),
    /* 0x00B0 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00B1 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x086C),
    /* 0x00B4 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00B5 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0, 0x00D5 - 0x00BC, 0x0),
    /* 0x00BC 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x00BD 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x086E),
    /* 0x00C0 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_25_20),
    /* 0x00C3 0x03 */ MSCRIPT_CMD_SET_EVENT_INF(EVENTINF_35),
    /* 0x00C6 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_36),
    /* 0x00C9 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_37),
    /* 0x00CC 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_40),
    /* 0x00CF 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00D0 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00D1 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x00D4 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00D5 0x01 */ MSCRIPT_CMD_PLAY_CANCEL(),
    /* 0x00D6 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x086D),
    /* 0x00D9 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00DA 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00DB 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0870),
    /* 0x00DE 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00DF 0x03 */ MSCRIPT_CMD_JUMP(0x00AD - 0x00E2),

    /* 0x00E2 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_25_20, 0x00EE - 0x00E7),
    /* 0x00E7 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0871),
    /* 0x00EA 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00EB 0x03 */ MSCRIPT_CMD_JUMP(0x00A9 - 0x00EE),

    /* 0x00EE 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0872),
    /* 0x00F1 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00F2 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0873),
    /* 0x00F5 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00F6 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0, 0x0114 - 0x00FD, 0x0),
    /* 0x00FD 0x05 */ MSCRIPT_CMD_CHECK_RUPEES(10, 0x0108 - 0x0102),
    /* 0x0102 0x01 */ MSCRIPT_CMD_PLAY_ERROR(),
    /* 0x0103 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0875),
    /* 0x0106 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0107 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0108 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x0109 0x03 */ MSCRIPT_CMD_CHANGE_RUPEES(-10),
    /* 0x010C 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_26_40, 0x011A - 0x0111),
    /* 0x0111 0x03 */ MSCRIPT_CMD_JUMP(0x00BC - 0x0114),

    /* 0x0114 0x01 */ MSCRIPT_CMD_PLAY_CANCEL(),
    /* 0x0115 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0874),
    /* 0x0118 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0119 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x011A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0876),
    /* 0x011D 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_25_20),
    /* 0x0120 0x03 */ MSCRIPT_CMD_SET_EVENT_INF(EVENTINF_35),
    /* 0x0123 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_36),
    /* 0x0126 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_37),
    /* 0x0129 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_40),
    /* 0x012C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x012D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x012E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0131 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80A51384[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0861),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0005 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80A5138C[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_EVENT_INF(EVENTINF_36, 0x006C - 0x0005),
    /* 0x0005 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_26_40, 0x0044 - 0x000A),
    /* 0x000A 0x05 */ MSCRIPT_CMD_CHECK_EVENT_INF(EVENTINF_37, 0x0029 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x087C),
    /* 0x0012 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0013 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_91_20, 0x0022 - 0x0018),
    /* 0x0018 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x087E),
    /* 0x001B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_91_20),
    /* 0x001F 0x03 */ MSCRIPT_CMD_JUMP(0x0070 - 0x0022),
    /* 0x0022 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x087B),
    /* 0x0025 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0026 0x03 */ MSCRIPT_CMD_JUMP(0x0070 - 0x0029),
    /* 0x0029 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x087D),
    /* 0x002C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x002E 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_HEART_PIECE, 0x0),
    /* 0x0033 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x000C),
    /* 0x0036 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0037 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x003A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x087E),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_26_40),
    /* 0x0041 0x03 */ MSCRIPT_CMD_JUMP(0x0070 - 0x0044),
    /* 0x0044 0x05 */ MSCRIPT_CMD_CHECK_EVENT_INF(EVENTINF_37, 0x0054 - 0x0049),
    /* 0x0049 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x087A),
    /* 0x004C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x087B),
    /* 0x0050 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0051 0x03 */ MSCRIPT_CMD_JUMP(0x0070 - 0x0054),
    /* 0x0054 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0878),
    /* 0x0057 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0058 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0059 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_PURPLE, 0x0),
    /* 0x005E 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0005),
    /* 0x0061 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0062 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0065 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0879),
    /* 0x0068 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0069 0x03 */ MSCRIPT_CMD_JUMP(0x0070 - 0x006C),
    /* 0x006C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0877),
    /* 0x006F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0070 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_35),
    /* 0x0073 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_36),
    /* 0x0076 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_37),
    /* 0x0079 0x03 */ MSCRIPT_CMD_UNSET_EVENT_INF(EVENTINF_40),
    /* 0x007C 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x007D 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Dnh_Profile = {
    /**/ ACTOR_EN_DNH,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TRO,
    /**/ sizeof(EnDnh),
    /**/ EnDnh_Init,
    /**/ EnDnh_Destroy,
    /**/ EnDnh_Update,
    /**/ EnDnh_Draw,
};

typedef enum {
    /* 0 */ ENDNH_ANIM_HEAD_MOVING,
    /* 1 */ ENDNH_ANIM_MAX
} EnDnhAnimation;

static AnimationInfoS sAnimationInfo[ENDNH_ANIM_MAX] = {
    { &gKoumeKioskHeadMovingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // ENDNH_ANIM_HEAD_MOVING
};

static TexturePtr sEyeTextures[] = {
    gKoumeKioskEyeOpenTex,
    gKoumeKioskEyeHalfTex,
    gKoumeKioskEyeClosedTex,
    gKoumeKioskEyeHalfTex,
};

s32 func_80A50D40(Actor* actor, PlayState* play) {
    Player_SetCsActionWithHaltedActors(play, actor, PLAYER_CSACTION_WAIT);
    if (CHECK_EVENTINF(EVENTINF_35)) {
        play->nextEntrance = ENTRANCE(SOUTHERN_SWAMP_CLEARED, 6);
    } else {
        play->nextEntrance = ENTRANCE(SOUTHERN_SWAMP_POISONED, 6);
    }
    gSaveContext.nextCutsceneIndex = 0;
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_FADE_WHITE;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE_SLOW;
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_90_40);
    CLEAR_EVENTINF(EVENTINF_50);
    return 1;
}

MsgScript* EnDnh_GetMsgScript(EnDnh* this, PlayState* play) {
    switch (this->unk198) {
        case 1:
            return D_80A51384;

        case 2:
            return D_80A5138C;

        default:
            return D_80A51250;
    }
}

s32 func_80A50E40(EnDnh* this, PlayState* play) {
    if (((this->unk18C & SUBS_OFFER_MODE_MASK) == SUBS_OFFER_MODE_NONE) ||
        !Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        return 0;
    }
    SubS_SetOfferMode(&this->unk18C, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
    this->msgScript = EnDnh_GetMsgScript(this, play);
    this->actionFunc = func_80A50F38;
    return 1;
}

void func_80A50EC0(EnDnh* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= ARRAY_COUNT(sEyeTextures)) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeTexIndex = 0;
        }
    }
}

void func_80A50F38(EnDnh* this, PlayState* play) {
    if (MsgEvent_RunScript(&this->actor, play, this->msgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk18C, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->msgScriptPos = 0;
        this->unk198 = 0;
        this->actionFunc = EnDnh_DoNothing;
    }
}

void EnDnh_DoNothing(EnDnh* this, PlayState* play) {
}

void EnDnh_Init(Actor* thisx, PlayState* play) {
    EnDnh* this = (EnDnh*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_Init(play, &this->skelAnime, &gKoumeKioskSkel, NULL, this->jointTable, this->morphTable,
                   KOUME_KIOSK_LIMB_MAX);
    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENDNH_ANIM_HEAD_MOVING);
    this->actor.shape.yOffset = 1100.0f;

    if (gSaveContext.save.entrance != ENTRANCE(TOURIST_INFORMATION, 1)) {
        SubS_SetOfferMode(&this->unk18C, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk198 = 0;
    } else {
        SubS_SetOfferMode(&this->unk18C, SUBS_OFFER_MODE_AUTO, SUBS_OFFER_MODE_MASK);
        this->unk198 = CHECK_EVENTINF(EVENTINF_35) ? 2 : 1;
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME)) {
        this->actor.draw = NULL;
    }

    this->msgScriptCallback = func_80A50D40;
    this->msgScriptPos = 0;
    this->actionFunc = EnDnh_DoNothing;
}

void EnDnh_Destroy(Actor* thisx, PlayState* play) {
}

void EnDnh_Update(Actor* thisx, PlayState* play) {
    EnDnh* this = (EnDnh*)thisx;

    func_80A50E40(this, play);
    this->actionFunc(this, play);
    func_80A50EC0(this);
    SkelAnime_Update(&this->skelAnime);
    SubS_Offer(&this->actor, play, 60.0f, 30.0f, PLAYER_IA_NONE, this->unk18C & SUBS_OFFER_MODE_MASK);
    Actor_SetFocus(&this->actor, 26.0f);
}

s32 EnDnh_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDnh* this = (EnDnh*)thisx;

    if (limbIndex == KOUME_KIOSK_LIMB_HEAD) {
        Matrix_Translate(0.0f, this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    }
    return false;
}

void EnDnh_Draw(Actor* thisx, PlayState* play) {
    EnDnh* this = (EnDnh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDnh_OverrideLimbDraw, NULL,
                      &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

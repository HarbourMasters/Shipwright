/*
 * File: z_en_dnq.c
 * Overlay: ovl_En_Dnq
 * Description: Deku King
 *
 * This actor's name is probably short for "Dekunuts Queen". This is likely a misnamed actor,
 * since it uses assets from object_dno (probably short for "Dekunuts Ou (king)").
 */

#include "z_en_dnq.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnDnq_Init(Actor* thisx, PlayState* play);
void EnDnq_Destroy(Actor* thisx, PlayState* play);
void EnDnq_Update(Actor* thisx, PlayState* play);
void EnDnq_Draw(Actor* thisx, PlayState* play);

void func_80A52FB8(EnDnq* this, PlayState* play);

static MsgScript D_80A53400[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_18_01, 0x001A - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0899),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x000A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x089A),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x089B),
    /* 0x0011 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x089C),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_18_01),
    /* 0x0019 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0898),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80A53420[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_18_02, 0x0011 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x089D),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x089E),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_18_02),
    /* 0x0010 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0011 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x089F),
    /* 0x0014 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0015 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80A53438[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x08A1),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x08A2),
    /* 0x0007 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Dnq_Profile = {
    /**/ ACTOR_EN_DNQ,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNO,
    /**/ sizeof(EnDnq),
    /**/ EnDnq_Init,
    /**/ EnDnq_Destroy,
    /**/ EnDnq_Update,
    /**/ EnDnq_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 34, 80, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* -1 */ DEKU_KING_ANIM_NONE = -1,
    /*  0 */ DEKU_KING_ANIM_IDLE,
    /*  1 */ DEKU_KING_ANIM_IDLE_MORPH,
    /*  2 */ DEKU_KING_ANIM_SURPRISE,
    /*  3 */ DEKU_KING_ANIM_JUMPED_ON_START,
    /*  4 */ DEKU_KING_ANIM_JUMPED_ON_LOOP,
    /*  5 */ DEKU_KING_ANIM_JUMPED_ON_END,
    /*  6 */ DEKU_KING_ANIM_JUMPED_ON_END_MORPH,
    /*  7 */ DEKU_KING_ANIM_LYING_DOWN_TWITCH,
    /*  8 */ DEKU_KING_ANIM_WAIL,
    /*  9 */ DEKU_KING_ANIM_FOOT_STAMP_START,
    /* 10 */ DEKU_KING_ANIM_FOOT_STAMP_ONCE,
    /* 11 */ DEKU_KING_ANIM_WAIL_START,
    /* 12 */ DEKU_KING_ANIM_MARCH,
    /* 13 */ DEKU_KING_ANIM_SPIN_SCEPTER,
    /* 14 */ DEKU_KING_ANIM_INTIMIDATE,
    /* 15 */ DEKU_KING_ANIM_POINT_SCEPTER,
    /* 16 */ DEKU_KING_ANIM_JUMP,
    /* 17 */ DEKU_KING_ANIM_LAUGH_ONCE,
    /* 18 */ DEKU_KING_ANIM_FOOT_STAMP_LOOP,
    /* 19 */ DEKU_KING_ANIM_LAUGH_LOOP,
    /* 36 */ ENDNQ_ANIM_MAX
} EnDnkAnimation;

static AnimationInfoS sAnimationInfo[ENDNQ_ANIM_MAX] = {
    { &gDekuKingIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },             // DEKU_KING_ANIM_IDLE
    { &gDekuKingIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },            // DEKU_KING_ANIM_IDLE_MORPH
    { &gDekuKingSurpriseAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },        // DEKU_KING_ANIM_SURPRISE
    { &gDekuKingJumpedOnStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },    // DEKU_KING_ANIM_JUMPED_ON_START
    { &gDekuKingJumpedOnLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },    // DEKU_KING_ANIM_JUMPED_ON_LOOP
    { &gDekuKingJumpedOnEndAnim, 0.0f, 0, -1, ANIMMODE_ONCE, 0 },      // DEKU_KING_ANIM_JUMPED_ON_END
    { &gDekuKingJumpedOnEndAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },     // DEKU_KING_ANIM_JUMPED_ON_END_MORPH
    { &gDekuKingLyingDownTwitchAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // DEKU_KING_ANIM_LYING_DOWN_TWITCH
    { &gDekuKingWailLoopAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },        // DEKU_KING_ANIM_WAIL
    { &gDekuKingFootStampStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },  // DEKU_KING_ANIM_FOOT_STAMP_START
    { &gDekuKingFootStampLoopAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },   // DEKU_KING_ANIM_FOOT_STAMP_ONCE
    { &gDekuKingWailStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },       // DEKU_KING_ANIM_WAIL_START
    { &gDekuKingMarchAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },            // DEKU_KING_ANIM_MARCH
    { &gDekuKingSpinScepterAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // DEKU_KING_ANIM_SPIN_SCEPTER
    { &gDekuKingIntimidateAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },      // DEKU_KING_ANIM_INTIMIDATE
    { &gDekuKingPointScepterAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },    // DEKU_KING_ANIM_POINT_SCEPTER
    { &gDekuKingJumpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },            // DEKU_KING_ANIM_JUMP
    { &gDekuKingLaughAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },           // DEKU_KING_ANIM_LAUGH_ONCE
    { &gDekuKingFootStampLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },   // DEKU_KING_ANIM_FOOT_STAMP_LOOP
    { &gDekuKingLaughAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },           // DEKU_KING_ANIM_LAUGH_LOOP
};

s32 EnDnq_ValidatePictograph(PlayState* play, Actor* thisx) {
    return Snap_ValidatePictograph(play, thisx, PICTO_VALID_DEKU_KING, &thisx->focus.pos, &thisx->world.rot, 120.0f,
                                   480.0f, 0x38E3);
}

s32 EnDnq_ChangeAnim(EnDnq* this, s32 animIndex) {
    s32 changeAnim = false;
    s32 didAnimChange = false;

    if ((animIndex == DEKU_KING_ANIM_IDLE) || (animIndex == DEKU_KING_ANIM_IDLE_MORPH)) {
        switch (this->animIndex) {
            case DEKU_KING_ANIM_IDLE:
            case DEKU_KING_ANIM_IDLE_MORPH:
                break;

            default:
                changeAnim = true;
                break;
        }
    } else if (this->animIndex != animIndex) {
        changeAnim = true;
    }

    if (changeAnim) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
    }

    return didAnimChange;
}

void func_80A52604(EnDnq* this, PlayState* play) {
    Collider_UpdateCylinder(&this->picto.actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 func_80A52648(EnDnq* this, PlayState* play) {
    s32 ret = false;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (!(this->unk_37C & 0x20)) {
            this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->cueId = 255;
            this->unk_37C |= 0x20;
        }
        SubS_SetOfferMode(&this->unk_37C, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        ret = true;
    } else {
        if (this->unk_37C & 0x20) {
            this->picto.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->cueId = 255;
            this->unk_37C &= ~0x20;
            SubS_SetOfferMode(&this->unk_37C, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        }
    }

    return ret;
}

s32 func_80A526F8(EnDnq* this) {
    if (this->unk_386 == 0) {
        EnDnq_ChangeAnim(this, DEKU_KING_ANIM_WAIL);
        this->unk_38C = ((s32)(Rand_ZeroOne() * 100.0f) % 4) + 3;
        this->unk_388 = 0;
        this->unk_386 = 1;
    } else if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        switch (this->animIndex) {
            case DEKU_KING_ANIM_WAIL:
                if (DECR(this->unk_38C) == 0) {
                    EnDnq_ChangeAnim(this, DEKU_KING_ANIM_FOOT_STAMP_START);
                }
                break;

            case DEKU_KING_ANIM_FOOT_STAMP_START:
                EnDnq_ChangeAnim(this, DEKU_KING_ANIM_FOOT_STAMP_ONCE);
                this->unk_38C = ((s32)(Rand_ZeroOne() * 100.0f) % 3) + 2;
                break;

            case DEKU_KING_ANIM_FOOT_STAMP_ONCE:
                if (DECR(this->unk_38C) == 0) {
                    this->unk_388++;
                    if ((this->unk_38C != 0) || (this->unk_388 < 2)) {
                        EnDnq_ChangeAnim(this, DEKU_KING_ANIM_WAIL_START);
                    } else {
                        EnDnq_ChangeAnim(this, DEKU_KING_ANIM_LAUGH_ONCE);
                        this->unk_38C = 4;
                    }
                }
                break;

            case DEKU_KING_ANIM_WAIL_START:
                EnDnq_ChangeAnim(this, DEKU_KING_ANIM_WAIL);
                this->unk_38C = ((s32)(Rand_ZeroOne() * 100.0f) % 4) + 3;
                break;

            case DEKU_KING_ANIM_LAUGH_ONCE:
                if (DECR(this->unk_38C) == 0) {
                    this->unk_386 = 0;
                }
                break;

            default:
                break;
        }

        if (this->unk_386 != 0) {
            this->skelAnime.curFrame = 0.0f;
        }
    }

    this->unk_38A = 0;
    return 0;
}

s32 func_80A52944(EnDnq* this) {
    s16 phi_v1 = 0;

    if (this->unk_386 == 0) {
        EnDnq_ChangeAnim(this, DEKU_KING_ANIM_MARCH);
        this->unk_38C = 1;
        this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y;
        this->unk_386 = 1;
        phi_v1 = this->unk_38C * 0x2000;
        this->picto.actor.shape.rot.y += phi_v1;
    } else if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->unk_386++;
        if (this->unk_386 >= 6) {
            EnDnq_ChangeAnim(this, DEKU_KING_ANIM_MARCH);
            this->unk_386 = 1;
            phi_v1 = this->unk_38C * 0x2000;
        } else if (this->unk_386 <= 3) {
            this->unk_38C = -this->unk_38C;
            this->skelAnime.curFrame = 0.0f;
            phi_v1 = this->unk_38C * 0x2000;
        } else {
            EnDnq_ChangeAnim(this, DEKU_KING_ANIM_SPIN_SCEPTER);
            this->skelAnime.curFrame = 0.0f;
        }
        this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y;
        this->picto.actor.shape.rot.y += phi_v1;
    }

    this->unk_38A = 0;
    return 0;
}

s32 func_80A52A78(EnDnq* this, PlayState* play) {
    static s32 D_80A535DC[] = {
        DEKU_KING_ANIM_LAUGH_ONCE,   DEKU_KING_ANIM_MARCH, DEKU_KING_ANIM_INTIMIDATE,
        DEKU_KING_ANIM_SPIN_SCEPTER, DEKU_KING_ANIM_JUMP,
    };

    if (this->unk_38A == 0) {
        this->unk_38C = 0;
        this->unk_38A = 1;
        this->unk_388 = play->state.frames % 5;
    }

    this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y;
    if (this->unk_38C != 0) {
        this->unk_38C--;
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (this->unk_38C == 0) {
            this->unk_38C = Rand_S16Offset(20, 20);
            this->unk_388 += 3;
            this->unk_388 %= 5;
        }
        EnDnq_ChangeAnim(this, D_80A535DC[this->unk_388]);
        this->skelAnime.curFrame = 0.0f;
    }

    this->unk_386 = 0;
    return 0;
}

s32 func_80A52B68(EnDnq* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 textId = play->msgCtx.currentTextId;

    if ((player->stateFlags1 & PLAYER_STATE1_TALKING) && (player->talkActor == &this->picto.actor)) {
        switch (textId) {
            case 0x89B:
                EnDnq_ChangeAnim(this, DEKU_KING_ANIM_FOOT_STAMP_LOOP);
                break;

            case 0x89E:
                EnDnq_ChangeAnim(this, DEKU_KING_ANIM_LAUGH_LOOP);
                break;

            case 0x898:
            case 0x89F:
                EnDnq_ChangeAnim(this, DEKU_KING_ANIM_JUMP);
                break;

            case 0x899:
            case 0x89D:
                EnDnq_ChangeAnim(this, DEKU_KING_ANIM_INTIMIDATE);
                break;

            case 0x89A:
            case 0x89C:
                EnDnq_ChangeAnim(this, DEKU_KING_ANIM_POINT_SCEPTER);
                break;

            default:
                break;
        }
        this->unk_39C = 1;
    } else if (this->unk_39C != 0) {
        EnDnq_ChangeAnim(this, DEKU_KING_ANIM_IDLE);
        this->unk_39C = 0;
        this->unk_37E = 0;
        this->unk_386 = 0;
    }

    return 0;
}

void func_80A52C6C(EnDnq* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    Vec3f sp34 = { 0.0f, 0.0f, 110.0f };
    Vec3f sp28;
    Vec3f sp1C;

    Math_Vec3f_Copy(&sp1C, &player->actor.world.pos);
    Lib_Vec3f_TranslateAndRotateY(&this->picto.actor.world.pos, this->picto.actor.world.rot.y, &sp34, &sp28);
    Math_Vec3f_Copy(&this->unk_370, &sp28);
    this->picto.actor.xzDistToPlayer = Math_Vec3f_DistXZ(&sp28, &sp1C);
}

MsgScript* EnDnq_GetMsgScript(EnDnq* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
        return D_80A53438;
    }

    if (this->unk_3A4 != 0) {
        return D_80A53420;
    }

    return D_80A53400;
}

s32 func_80A52D44(EnDnq* this, PlayState* play) {
    s32 ret = false;

    if (((this->unk_37C & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        SubS_SetOfferMode(&this->unk_37C, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->msgScript = EnDnq_GetMsgScript(this, play);
        this->actionFunc = func_80A52FB8;
        ret = true;
    }

    return ret;
}

void func_80A52DC8(EnDnq* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw = this->picto.actor.yawTowardsPlayer - this->picto.actor.world.rot.y;

    if (yaw <= 0x3800) {
        SubS_SetOfferMode(&this->unk_37C, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    } else {
        SubS_SetOfferMode(&this->unk_37C, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
        this->unk_390 = 70.0f;
        if (Inventory_HasItemInBottle(ITEM_DEKU_PRINCESS) && !Play_InCsMode(play) &&
            (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) && (CutsceneManager_GetCurrentCsId() == CS_ID_NONE)) {
            if ((DECR(this->unk_384) == 0) && CHECK_WEEKEVENTREG(WEEKEVENTREG_29_40)) {
                Message_StartTextbox(play, 0x969, NULL);
                this->unk_384 = 200;
            }
        }

        this->unk_394 = this->picto.actor.xzDistToPlayer;
        func_80A52C6C(this, play);
        if (this->picto.actor.xzDistToPlayer < this->unk_390) {
            player->unk_B2B = 0x1A;
        }

        this->picto.actor.xzDistToPlayer = this->unk_394;

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_83_08)) {
            func_80A52A78(this, play);
        } else if (this->unk_3A4 == 0) {
            Math_ApproachS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 3, 0x2AA8);
            func_80A526F8(this);
        } else {
            func_80A52944(this);
        }
    } else {
        Math_ApproachS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 3, 0x2AA8);
        this->unk_390 = 70.0f;
    }
}

void func_80A52FB8(EnDnq* this, PlayState* play) {
    s16 sp2E = this->picto.actor.yawTowardsPlayer;

    if (MsgEvent_RunScript(&this->picto.actor, play, this->msgScript, NULL, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk_37C, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_386 = 0;
        this->actionFunc = func_80A52DC8;
    } else {
        Math_ApproachS(&this->picto.actor.shape.rot.y, sp2E, 3, 0x2AA8);
    }
}

void EnDnq_HandleCutscene(EnDnq* this, PlayState* play) {
    //! @bug The credits cutscene accesses this array OOB with a cueId of 6, which ends up giving 0
    static s32 sCsAnimIndex[] = {
        DEKU_KING_ANIM_IDLE,          DEKU_KING_ANIM_IDLE_MORPH,
        DEKU_KING_ANIM_SURPRISE,      DEKU_KING_ANIM_JUMPED_ON_START,
        DEKU_KING_ANIM_JUMPED_ON_END, DEKU_KING_ANIM_JUMPED_ON_END_MORPH,
        DEKU_KING_ANIM_IDLE, // 2S2H [Port] Added to prevent a crash with garbage data for cueId 6
    };
    s32 cueChannel;
    u32 cueId;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_105)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_105);
        cueId = play->csCtx.actorCues[cueChannel]->id;
        if (this->cueId != (u8)cueId) {
            EnDnq_ChangeAnim(this, sCsAnimIndex[cueId]);
            this->cueId = cueId;
        }

        if ((this->animIndex == DEKU_KING_ANIM_JUMPED_ON_LOOP) && Animation_OnFrame(&this->skelAnime, 2.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EN_KINGNUTS_DAMAGE);
        }

        if (((this->animIndex == DEKU_KING_ANIM_JUMPED_ON_START) ||
             (this->animIndex == DEKU_KING_ANIM_JUMPED_ON_END_MORPH)) &&
            Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            EnDnq_ChangeAnim(this, this->animIndex + 1);
        }

        Cutscene_ActorTranslateAndYaw(&this->picto.actor, play, cueChannel);
    }
}

void EnDnq_Init(Actor* thisx, PlayState* play) {
    EnDnq* this = (EnDnq*)thisx;

    ActorShape_Init(&this->picto.actor.shape, 0.0f, NULL, 14.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDekuKingSkel, NULL, this->jointTable, this->morphTable,
                       DEKU_KING_LIMB_MAX);
    this->animIndex = DEKU_KING_ANIM_NONE;
    EnDnq_ChangeAnim(this, DEKU_KING_ANIM_IDLE);
    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->picto.actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_SetScale(&this->picto.actor, 0.02f);
    this->picto.actor.attentionRangeType = ATTENTION_RANGE_1;
    this->unk_386 = 0;
    this->unk_37C = 0;
    SubS_SetOfferMode(&this->unk_37C, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80)) {
        this->unk_3A4 = 1;
    } else {
        this->unk_3A4 = 0;
    }
    this->picto.validationFunc = EnDnq_ValidatePictograph;
    this->actionFunc = func_80A52DC8;
}

void EnDnq_Destroy(Actor* thisx, PlayState* play) {
    EnDnq* this = (EnDnq*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDnq_Update(Actor* thisx, PlayState* play) {
    EnDnq* this = (EnDnq*)thisx;

    if (!func_80A52D44(this, play) && func_80A52648(this, play)) {
        EnDnq_HandleCutscene(this, play);
        SkelAnime_Update(&this->skelAnime);
    } else {
        this->actionFunc(this, play);
        func_80A52B68(this, play);
        SkelAnime_Update(&this->skelAnime);
        Actor_UpdateBgCheckInfo(play, &this->picto.actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
        this->unk_394 = this->picto.actor.xzDistToPlayer;
        func_80A52C6C(this, play);
        SubS_Offer(&this->picto.actor, play, this->unk_390, fabsf(this->picto.actor.playerHeightRel) + 1.0f,
                   PLAYER_IA_NONE, this->unk_37C & SUBS_OFFER_MODE_MASK);
        this->picto.actor.xzDistToPlayer = this->unk_394;
        Actor_SetFocus(&this->picto.actor, 46.0f);
        func_80A52604(this, play);
    }
}

void EnDnq_Draw(Actor* thisx, PlayState* play) {
    EnDnq* this = (EnDnq*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->picto.actor);
}

/*
 * File: z_en_dns.c
 * Overlay: ovl_En_Dns
 * Description: Deku Palace - King's Chamber Deku Guard
 */

#include "z_en_dns.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnDns_Init(Actor* thisx, PlayState* play);
void EnDns_Destroy(Actor* thisx, PlayState* play);
void EnDns_Update(Actor* thisx, PlayState* play);
void EnDns_Draw(Actor* thisx, PlayState* play);

void func_8092D330(EnDns* this, PlayState* play);
void EnDns_DoNothing(EnDns* this, PlayState* play);
void func_8092D4D8(EnDns* this, PlayState* play);

static MsgScript D_8092DCB0[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_23_20, 0x000A - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x082F),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0820),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_8092DCC0[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_23_20, 0x000A - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0830),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0821),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_8092DCD0[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_23_20, 0x000A - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0831),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0822),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_8092DCE0[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_23_20, 0x000A - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0832),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x000A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0823),
    /* 0x000D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000E 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_8092DCF0[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0833),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0007 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x0008 0x03 */ MSCRIPT_CMD_SET_EVENT_INF(EVENTINF_16),
    /* 0x000B 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x000C 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_8092DD00[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x082E),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Dns_Profile = {
    /**/ ACTOR_EN_DNS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNS,
    /**/ sizeof(EnDns),
    /**/ EnDns_Init,
    /**/ EnDns_Destroy,
    /**/ EnDns_Update,
    /**/ EnDns_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
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
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* -1 */ EN_DNS_ANIM_NONE = -1,
    /*  0 */ EN_DNS_ANIM_IDLE,
    /*  1 */ EN_DNS_ANIM_IDLE_MORPH,
    /*  2 */ EN_DNS_ANIM_WALK,
    /*  3 */ EN_DNS_ANIM_WALK_MORPH,
    /*  4 */ EN_DNS_ANIM_SURPRISE_START,
    /*  5 */ EN_DNS_ANIM_SURPRISE_LOOP,
    /*  6 */ EN_DNS_ANIM_RUN_START,
    /*  7 */ EN_DNS_ANIM_RUN_LOOP,
    /*  8 */ EN_DNS_ANIM_DANCE,
    /*  9 */ EN_DNS_ANIM_FLIP,
    /* 10 */ EN_DNS_ANIM_MAX
} EnDnsAnimation;

static AnimationInfoS sAnimationInfo[EN_DNS_ANIM_MAX] = {
    { &gKingsChamberDekuGuardIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // EN_DNS_ANIM_IDLE
    { &gKingsChamberDekuGuardIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },         // EN_DNS_ANIM_IDLE_MORPH
    { &gKingsChamberDekuGuardWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // EN_DNS_ANIM_WALK
    { &gKingsChamberDekuGuardWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },         // EN_DNS_ANIM_WALK_MORPH
    { &gKingsChamberDekuGuardSurpriseStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 }, // EN_DNS_ANIM_SURPRISE_START
    { &gKingsChamberDekuGuardSurpriseLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // EN_DNS_ANIM_SURPRISE_LOOP
    { &gKingsChamberDekuGuardRunStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // EN_DNS_ANIM_RUN_START
    { &gKingsChamberDekuGuardRunLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },       // EN_DNS_ANIM_RUN_LOOP
    { &gKingsChamberDekuGuardDanceAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // EN_DNS_ANIM_DANCE
    { &gKingsChamberDekuGuardFlipAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },          // EN_DNS_ANIM_FLIP
};

void func_8092C5C0(EnDns* this) {
    s32 pad;

    if (((this->animIndex == EN_DNS_ANIM_WALK) || (this->animIndex == EN_DNS_ANIM_WALK_MORPH) ||
         (this->animIndex == EN_DNS_ANIM_RUN_START) || (this->animIndex == EN_DNS_ANIM_RUN_LOOP)) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 3.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
    }
}

s32 EnDns_ChangeAnim(EnDns* this, s32 animIndex) {
    s32 changeAnim = false;
    s32 didAnimChange = false;

    switch (animIndex) {
        case EN_DNS_ANIM_IDLE:
        case EN_DNS_ANIM_IDLE_MORPH:
            if ((this->animIndex != EN_DNS_ANIM_IDLE) && (this->animIndex != EN_DNS_ANIM_IDLE_MORPH)) {
                changeAnim = true;
            }
            break;

        case EN_DNS_ANIM_WALK:
        case EN_DNS_ANIM_WALK_MORPH:
            if ((this->animIndex != EN_DNS_ANIM_WALK) && (this->animIndex != EN_DNS_ANIM_WALK_MORPH)) {
                changeAnim = true;
            }
            break;
        default:
            if (this->animIndex != animIndex) {
                changeAnim = true;
            }
    }

    if (changeAnim) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
    }

    return didAnimChange;
}

void func_8092C6FC(EnDns* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_8092C740(EnDns* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 temp = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    Vec3f sp34;
    Vec3f sp28;

    Math_ApproachS(&this->unk_2CE, temp, 4, 0x2AA8);
    this->unk_2CE = CLAMP(this->unk_2CE, -0x3FFC, 0x3FFC);

    Math_Vec3f_Copy(&sp28, &player->actor.world.pos);
    sp28.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
    sp34.y += 10.0f;
    temp = Math_Vec3f_Pitch(&sp34, &sp28);
    Math_ApproachS(&this->unk_2CC, temp, 4, 0x2AA8);
    this->unk_2CC = CLAMP(this->unk_2CC, -0x16C0, 0xE38);
}

void func_8092C86C(EnDns* this, PlayState* play) {
    if ((this->unk_2C6 & 8) && (DECR(this->unk_2DC) == 0)) {
        func_8092C740(this, play);
        this->unk_2C6 &= ~0x10;
        this->unk_2C6 |= 0x20;
    } else if (this->unk_2C6 & 0x20) {
        this->unk_2C6 &= ~0x20;
        this->unk_2CC = 0;
        this->unk_2CE = 0;
        this->unk_2DC = 20;
    } else if (DECR(this->unk_2DC) == 0) {
        this->unk_2C6 |= 0x10;
    }
}

void func_8092C934(EnDns* this) {
    if ((this->unk_2C6 & 0x40) && (DECR(this->blinkTimer) == 0)) {
        this->eyeIndex++;
        if (this->eyeIndex >= 4) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeIndex = 0;
        }
    }
}

MsgScript* EnDns_GetMsgScript(EnDns* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
        if (player->transformation != PLAYER_FORM_DEKU) {
            return D_8092DCF0;
        } else if (this->unk_2FC != 0) {
            return D_8092DD00;
        }
    }

    switch (ENDNS_GET_7(&this->actor)) {
        case ENDNS_GET_7_0:
            return D_8092DCB0;

        case ENDNS_GET_7_1:
            return D_8092DCC0;

        case ENDNS_GET_7_2:
            return D_8092DCD0;

        case ENDNS_GET_7_3:
            return D_8092DCE0;

        default:
            return NULL;
    }
}

s32 EnDns_GetCueType(EnDns* this) {
    switch (ENDNS_GET_7(&this->actor)) {
        case ENDNS_GET_7_0:
            return CS_CMD_ACTOR_CUE_465;

        case ENDNS_GET_7_1:
            return CS_CMD_ACTOR_CUE_466;

        case ENDNS_GET_7_2:
            return CS_CMD_ACTOR_CUE_467;

        case ENDNS_GET_7_3:
            return CS_CMD_ACTOR_CUE_468;

        default:
            return 0;
    }
}

s32 func_8092CAD0(EnDns* this, PlayState* play) {
    s32 ret = false;

    if (((this->unk_2C6 & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        SubS_SetOfferMode(&this->unk_2C6, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->unk_2C6 &= ~0x10;
        if (ENDNS_GET_4000(&this->actor)) {
            this->animCurFrame = 0.0f;
            if (this->unk_2D2 != 0) {
                this->animCurFrame = this->skelAnime.curFrame;
                EnDns_ChangeAnim(this, EN_DNS_ANIM_WALK);
            }
            this->unk_2DA = this->actor.world.rot.y;
        }
        this->msgScript = EnDns_GetMsgScript(this, play);
        this->actionFunc = func_8092D4D8;
        ret = true;
    }
    return ret;
}

s32 func_8092CB98(EnDns* this, PlayState* play) {
    s32 phi_v1 = 0;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (!(this->unk_2C6 & 0x80)) {
            this->cueType = EnDns_GetCueType(this);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            SubS_SetOfferMode(&this->unk_2C6, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
            this->unk_2C6 |= 0x80;
            this->cueId = 255;
        }
        phi_v1 = 1;
    } else if (this->unk_2C6 & 0x80) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        SubS_SetOfferMode(&this->unk_2C6, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_2C6 &= ~0x80;
    }
    return phi_v1;
}

s32 func_8092CC68(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad[2];
    s32 ret = false;
    s16 bgId;

    if (!Play_InCsMode(play) && (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        (player->transformation != PLAYER_FORM_DEKU)) {
        bgId = player->actor.floorBgId;
        if (SurfaceType_GetSceneExitIndex(&play->colCtx, player->actor.floorPoly, bgId) != 4) {
            ret = true;
        }
    }

    return ret;
}

s32 func_8092CCEC(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnDns* this = (EnDns*)thisx;
    Vec3f sp3C = player->actor.world.pos;
    Vec3f sp30 = this->actor.world.pos;
    s16 sp2E;

    Math_Vec3f_Copy(&sp30, &this->actor.world.pos);
    Math_Vec3f_Copy(&sp3C, &player->actor.world.pos);
    this->unk_2D6 = Math_Vec3f_Yaw(&gZeroVec3f, &sp3C);
    this->unk_2D4 = Math_Vec3f_Yaw(&gZeroVec3f, &sp30);
    this->unk_2EC = Math_Vec3f_DistXZ(&sp30, &gZeroVec3f);
    sp2E = Math_Vec3f_Yaw(&gZeroVec3f, &sp3C);
    sp2E -= Math_Vec3f_Yaw(&gZeroVec3f, &sp30);
    this->unk_2D8 = (Rand_ZeroOne() * 182.0f) + 182.0f;
    this->unk_2D8 = (sp2E > 0) ? this->unk_2D8 : -this->unk_2D8;
    this->unk_2D0 = 0x28;
    this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
    return 1;
}

s32 func_8092CE38(EnDns* this) {
    static s32 D_8092DE00[] = { EN_DNS_ANIM_DANCE, EN_DNS_ANIM_DANCE, EN_DNS_ANIM_FLIP };
    s16 angularVelocity;
    s32 pad;
    Vec3f sp2C;
    s32 ret = false;

    if ((this->unk_2C6 & 0x200) || Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnDns_ChangeAnim(this, D_8092DE00[this->unk_2D2]);
        this->unk_2C6 &= ~0x200;
        this->skelAnime.curFrame = 0.0f;
        if (this->unk_2D2 == 2) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_JUMP);
        }
        this->unk_2D2++;
        if (this->unk_2D2 >= 3) {
            this->unk_2D2 = 0;
        }
        ret = true;
    } else {
        if (this->unk_2D2 == 0) {
            if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
                this->actor.world.rot.y = BINANG_ROT180(this->actor.world.rot.y);
                this->unk_2E4 = 0.0f;
                this->actor.shape.rot.y = this->actor.world.rot.y;
                Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_JUMP);
            } else if (this->skelAnime.curFrame < 13.0f) {
                angularVelocity = this->skelAnime.curFrame;
                this->actor.shape.rot.y = this->actor.world.rot.y;
                angularVelocity *= 0x9D8;
                this->actor.shape.rot.y += angularVelocity;
                this->unk_2E4 -= -(40.0f / 13.0f);
            }
        } else {
            if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 6.0f) ||
                Animation_OnFrame(&this->skelAnime, 13.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
            }

            if (this->skelAnime.curFrame > 7.0f) {
                this->unk_2E4 += -(20.0f / 13.0f);
            }
        }
        sp2C.x = this->unk_2E4;
        sp2C.y = 0.0f;
        sp2C.z = 0.0f;
        Lib_Vec3f_TranslateAndRotateY(&this->actor.home.pos, this->actor.world.rot.y, &sp2C, &this->actor.world.pos);
    }

    return ret;
}

s32 func_8092D068(EnDns* this) {
    s32 ret = false;

    if (ENDNS_GET_8000(&this->actor)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
            ret = true;
        }
    } else if (ENDNS_GET_4000(&this->actor)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
            ret = true;
        }
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
        ret = true;
    }

    return ret;
}

void func_8092D108(EnDns* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    Matrix_SetTranslateRotateYXZ(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                                 &this->actor.home.rot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gKingsChamberDekuGuardDekuFlowerDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_8092D1B8(EnDns* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 sp22 = this->actor.world.rot.y;

    if (ENDNS_GET_4000(&this->actor)) {
        func_8092CE38(this);
    }

    if (!ENDNS_GET_4000(&this->actor) || (this->unk_2D2 != 0)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20) && !CHECK_EVENTINF(EVENTINF_15) && func_8092CC68(play)) {
            player->stateFlags1 |= PLAYER_STATE1_20;
            this->unk_2C6 |= 0x100;
            SubS_SetOfferMode(&this->unk_2C6, SUBS_OFFER_MODE_AUTO, SUBS_OFFER_MODE_MASK);
            Audio_PlaySfx(NA_SE_SY_FOUND);
            SET_EVENTINF(EVENTINF_15);
            this->msgScriptCallback = func_8092CCEC;
            EnDns_ChangeAnim(this, EN_DNS_ANIM_WALK);
            this->actionFunc = EnDns_DoNothing;
        } else if (CHECK_EVENTINF(EVENTINF_16)) {
            func_8092CCEC(&this->actor, play);
            EnDns_ChangeAnim(this, EN_DNS_ANIM_WALK);
            this->actionFunc = func_8092D330;
        }
        Math_ApproachS(&this->actor.shape.rot.y, sp22, 3, 0x2AA8);
    }
}

void EnDns_DoNothing(EnDns* this, PlayState* play) {
}

void func_8092D330(EnDns* this, PlayState* play) {
    s32 pad;
    Vec3f sp30 = gZeroVec3f;
    s16 temp = this->unk_2D6 - this->unk_2D4;

    if (ABS_ALT(temp) < 0xC16) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x2AA8);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else {
        this->actor.world.pos.x = Math_SinS(this->unk_2D4) * this->unk_2EC;
        this->actor.world.pos.z = Math_CosS(this->unk_2D4) * this->unk_2EC;
        this->unk_2D4 += this->unk_2D8;
        sp30.x = Math_SinS(this->unk_2D4) * this->unk_2EC;
        sp30.z = Math_CosS(this->unk_2D4) * this->unk_2EC;
        Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &sp30), 3, 0x2AA8);
        Actor_MoveWithGravity(&this->actor);
    }
    if ((this->unk_2C6 & 0x100) && (DECR(this->unk_2D0) == 0)) {
        this->unk_2C6 &= ~0x100;
        play->nextEntrance = ENTRANCE(DEKU_PALACE, 1);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_FADE_WHITE;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
    }
}

void func_8092D4D8(EnDns* this, PlayState* play) {
    s16 sp2E = this->actor.yawTowardsPlayer;

    if (ENDNS_GET_4000(&this->actor) && (this->unk_2D2 == 0)) {
        if (func_8092CE38(this)) {
            EnDns_ChangeAnim(this, EN_DNS_ANIM_WALK);
        }
    } else if (MsgEvent_RunScript(&this->actor, play, this->msgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk_2C6, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->msgScriptCallback = NULL;
        if (ENDNS_GET_4000(&this->actor)) {
            if (!CHECK_EVENTINF(EVENTINF_15)) {
                this->skelAnime.curFrame = this->animCurFrame;
                this->actor.world.rot.y = this->unk_2DA;
                EnDns_ChangeAnim(this, EN_DNS_ANIM_DANCE);
            }
            this->unk_2CC = 0;
            this->unk_2CE = 0;
            this->unk_2C6 |= 0x10;
        }
        this->actionFunc = func_8092D1B8;
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, sp2E, 3, 0x2AA8);
    }
}

void EnDns_HandleCutscene(EnDns* this, PlayState* play) {
    static s32 sCsAnimIndex[] = {
        EN_DNS_ANIM_IDLE,
        EN_DNS_ANIM_IDLE,
        EN_DNS_ANIM_SURPRISE_START,
        EN_DNS_ANIM_RUN_START,
    };
    s32 cueChannel;
    u32 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        cueChannel = Cutscene_GetCueChannel(play, this->cueType);
        cueId = play->csCtx.actorCues[cueChannel]->id;
        if (this->cueId != (u8)cueId) {
            EnDns_ChangeAnim(this, sCsAnimIndex[cueId]);
            this->cueId = cueId;
        }

        if (((this->animIndex == EN_DNS_ANIM_SURPRISE_START) || (this->animIndex == EN_DNS_ANIM_RUN_START)) &&
            Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            EnDns_ChangeAnim(this, this->animIndex + 1);
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    }
}

void EnDns_Init(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    if (!func_8092D068(this)) {
        Actor_Kill(&this->actor);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 18.0f);
    SkelAnime_Init(play, &this->skelAnime, &gKingsChamberDekuGuardSkel, NULL, this->jointTable, this->morphTable,
                   KINGS_CHAMBER_DEKU_GUARD_LIMB_MAX);
    this->animIndex = EN_DNS_ANIM_NONE;
    EnDns_ChangeAnim(this, EN_DNS_ANIM_WALK);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.gravity = -0.8f;
    this->unk_2D2 = 0;
    this->unk_2C6 = 0;
    SubS_SetOfferMode(&this->unk_2C6, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->unk_2C6 |= (0x40 | 0x10);
    this->unk_2C6 |= 0x200;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80)) {
        this->unk_2FC = 1;
    } else {
        this->unk_2FC = 0;
    }
    this->actionFunc = func_8092D1B8;
    CLEAR_EVENTINF(EVENTINF_15);
    CLEAR_EVENTINF(EVENTINF_16);
}

void EnDns_Destroy(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDns_Update(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    if (!func_8092CAD0(this, play) && func_8092CB98(this, play)) {
        EnDns_HandleCutscene(this, play);
        SkelAnime_Update(&this->skelAnime);
        func_8092C5C0(this);
    } else {
        this->actionFunc(this, play);
        SkelAnime_Update(&this->skelAnime);
        func_8092C934(this);
        func_8092C86C(this, play);
        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
        SubS_Offer(&this->actor, play, 80.0f, 40.0f, PLAYER_IA_NONE, this->unk_2C6 & SUBS_OFFER_MODE_MASK);
        Actor_SetFocus(&this->actor, 34.0f);
        func_8092C6FC(this, play);
        func_8092C5C0(this);
    }
}

s32 func_8092D954(s16 arg0, s16 arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4, s32 arg5) {
    Vec3f sp74;
    Vec3s sp6C;
    MtxF sp2C;

    Matrix_MultVec3f(&gZeroVec3f, &sp74);
    Matrix_Get(&sp2C);
    Matrix_MtxFToYXZRot(&sp2C, &sp6C, false);
    *arg2 = sp74;

    if (!arg4) {
        if (arg5) {
            sp6C.z = arg0;
            sp6C.y = arg1;
        }
        Math_SmoothStepToS(&arg3->x, sp6C.x, 3, 0x2AA8, 0xB6);
        Math_SmoothStepToS(&arg3->y, sp6C.y, 3, 0x2AA8, 0xB6);
        Math_SmoothStepToS(&arg3->z, sp6C.z, 3, 0x2AA8, 0xB6);
    } else {
        arg3->x = sp6C.x;
        arg3->y = sp6C.y;
        arg3->z = sp6C.z;
    }

    return 1;
}

s32 EnDns_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDns* this = (EnDns*)thisx;

    this->unk_1E4[limbIndex] = *dList;
    *dList = NULL;
    return false;
}

void EnDns_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDns* this = (EnDns*)thisx;
    s32 pad;
    s32 phi_v1;
    s32 phi_v0;

    if (this->unk_2C6 & 0x10) {
        phi_v1 = true;
        phi_v0 = false;
    } else {
        phi_v1 = false;
        if (this->unk_2C6 & 0x20) {
            phi_v0 = true;
        } else {
            phi_v0 = false;
        }
    }

    if (limbIndex == KINGS_CHAMBER_DEKU_GUARD_LIMB_HEAD) {
        func_8092D954(this->unk_2CC, this->unk_2CE + this->actor.shape.rot.y, &this->unk_218, &this->unk_224, phi_v1,
                      phi_v0);
        Matrix_Translate(this->unk_218.x, this->unk_218.y, this->unk_218.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_224.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_224.z, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_224.x, MTXMODE_APPLY);
    }

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->unk_1E4[limbIndex]);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDns_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gKingsChamberDekuGuardEyeOpenTex,
        gKingsChamberDekuGuardEyeHalfTex,
        gKingsChamberDekuGuardEyeClosedTex,
        gKingsChamberDekuGuardEyeHalfTex,
    };
    EnDns* this = (EnDns*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    gDPPipeSync(POLY_OPA_DISP++);

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDns_OverrideLimbDraw,
                      EnDns_PostLimbDraw, &this->actor);
    func_8092D108(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

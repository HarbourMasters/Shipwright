/*
 * File: z_en_ig.c
 * Overlay: ovl_En_Ig
 * Description: The Goron named the same as the player
 */

#include "z_en_ig.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnIg_Init(Actor* thisx, PlayState* play);
void EnIg_Destroy(Actor* thisx, PlayState* play);
void EnIg_Update(Actor* thisx, PlayState* play);
void EnIg_Draw(Actor* thisx, PlayState* play);

void func_80BF2AF8(EnIg* this, PlayState* play);
void func_80BF2BD4(EnIg* this, PlayState* play);

static u8 D_80BF3260[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_L(1, 0xB6 - 0x05),
    /* 0x05 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_TOWN, 0x57 - 0x09),
    /* 0x09 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 25, 15, 45, 0x51 - 0x0F),
    /* 0x0F */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 45, 15, 50, 0x4B - 0x15),
    /* 0x15 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_RECEIVED_ROOM_KEY, 0x1A - 0x19),
    /* 0x19 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1A */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 50, 16, 55, 0x45 - 0x20),
    /* 0x20 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 55, 17, 15, 0x3F - 0x26),
    /* 0x26 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(17, 15, 18, 0, 0x39 - 0x2C),
    /* 0x2C */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(18, 0, 6, 0, 0x33 - 0x32),
    /* 0x32 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x33 */ SCHEDULE_CMD_RET_TIME(18, 0, 6, 0, 4),
    /* 0x39 */ SCHEDULE_CMD_RET_TIME(17, 15, 6, 0, 2),
    /* 0x3F */ SCHEDULE_CMD_RET_TIME(16, 55, 17, 15, 11),
    /* 0x45 */ SCHEDULE_CMD_RET_TIME(16, 50, 16, 55, 6),
    /* 0x4B */ SCHEDULE_CMD_RET_TIME(15, 45, 15, 50, 5),
    /* 0x51 */ SCHEDULE_CMD_RET_TIME(15, 25, 15, 45, 10),
    /* 0x57 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_YADOYA, 0xB5 - 0x5B),
    /* 0x5B */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 45, 15, 50, 0xAF - 0x61),
    /* 0x61 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 50, 16, 10, 0xA9 - 0x67),
    /* 0x67 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 10, 16, 30, 0xA3 - 0x6D),
    /* 0x6D */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_RECEIVED_ROOM_KEY, 0x8A - 0x71),
    /* 0x71 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 30, 16, 50, 0x84 - 0x77),
    /* 0x77 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 50, 16, 55, 0x7E - 0x7D),
    /* 0x7D */ SCHEDULE_CMD_RET_NONE(),
    /* 0x7E */ SCHEDULE_CMD_RET_TIME(16, 50, 16, 55, 8),
    /* 0x84 */ SCHEDULE_CMD_RET_TIME(16, 30, 16, 50, 13),
    /* 0x8A */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 30, 16, 50, 0x9D - 0x90),
    /* 0x90 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 50, 16, 55, 0x97 - 0x96),
    /* 0x96 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x97 */ SCHEDULE_CMD_RET_TIME(16, 50, 16, 55, 9),
    /* 0x9D */ SCHEDULE_CMD_RET_TIME(16, 30, 16, 50, 14),
    /* 0xA3 */ SCHEDULE_CMD_RET_TIME(16, 10, 16, 30, 3),
    /* 0xA9 */ SCHEDULE_CMD_RET_TIME(15, 50, 16, 10, 12),
    /* 0xAF */ SCHEDULE_CMD_RET_TIME(15, 45, 15, 50, 7),
    /* 0xB5 */ SCHEDULE_CMD_RET_NONE(),
    /* 0xB6 */ SCHEDULE_CMD_RET_NONE(),
};

static s32 D_80BF3318[] = { -1, -1, 3, 1, 3, 1, 2, 0, 3, 5, 0, 3, 1, 2, 4 };

static MsgScript D_80BF3354[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B0),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80BF335C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0003 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_RECEIVED_ROOM_KEY, 0x0027 - 0x0008),
    /* 0x0008 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0009 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B1),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0010 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0011 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B2),
    /* 0x0014 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0015 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0018 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0019 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B3),
    /* 0x001C 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_ANJU),
    /* 0x001F 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_55_02),
    /* 0x0022 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0023 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0026 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0027 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_86_10, 0x005B - 0x002C),
    /* 0x002C 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x002D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B1),
    /* 0x0030 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0031 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0034 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0035 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B5),
    /* 0x0038 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0039 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x003C 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x003D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B6),
    /* 0x0040 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0041 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0044 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0045 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B7),
    /* 0x0048 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0049 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x004C 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x004D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B8),
    /* 0x0050 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_ANJU),
    /* 0x0053 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0054 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0057 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_86_10),
    /* 0x005A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x005B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x005E 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x005F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2956),
    /* 0x0062 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0063 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0066 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0067 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2957),
    /* 0x006A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x006B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x006E 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x006F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2958),
    /* 0x0072 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0073 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0076 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0077 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2959),
    /* 0x007A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_ANJU),
    /* 0x007D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x007E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0081 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0082 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80BF33E0[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B9),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80BF33E8[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x28B4),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80BF33F0[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x295B),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Ig_Profile = {
    /**/ ACTOR_EN_IG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DAI,
    /**/ sizeof(EnIg),
    /**/ EnIg_Init,
    /**/ EnIg_Destroy,
    /**/ EnIg_Update,
    /**/ EnIg_Draw,
};

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
    { 28, 62, 0, { 0, 0, 0 } },
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 20 }, 100 },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnIgAnimation {
    /* -1 */ ENIG_ANIM_NONE = -1,
    /*  0 */ ENIG_ANIM_0,
    /*  1 */ ENIG_ANIM_1,
    /*  2 */ ENIG_ANIM_2,
    /*  3 */ ENIG_ANIM_3,
    /*  4 */ ENIG_ANIM_4,
    /*  5 */ ENIG_ANIM_5,
    /*  6 */ ENIG_ANIM_6,
    /*  7 */ ENIG_ANIM_7,
    /*  8 */ ENIG_ANIM_8,
    /*  9 */ ENIG_ANIM_9,
    /* 10 */ ENIG_ANIM_MAX
} EnIgAnimation;

static AnimationInfoS sAnimationInfo[ENIG_ANIM_MAX] = {
    { &object_dai_Anim_0048B4, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIG_ANIM_0
    { &object_dai_Anim_0048B4, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIG_ANIM_1
    { &object_dai_Anim_005100, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIG_ANIM_2
    { &object_dai_Anim_005100, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIG_ANIM_3
    { &object_dai_Anim_0010F8, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // ENIG_ANIM_4
    { &object_dai_Anim_001E44, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIG_ANIM_5
    { &object_dai_Anim_0014BC, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // ENIG_ANIM_6
    { &object_dai_Anim_003CAC, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENIG_ANIM_7
    { &object_dai_Anim_0040E0, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIG_ANIM_8
    { &object_dai_Anim_0040E0, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIG_ANIM_9
};

Actor* EnIg_FindActor(EnIg* this, PlayState* play, u8 actorCategory, s16 actorId) {
    Actor* actorIter = NULL;

    while (true) {
        actorIter = SubS_FindActor(play, actorIter, actorCategory, actorId);

        if (actorIter == NULL) {
            break;
        }

        if ((this != (EnIg*)actorIter) && (actorIter->update != NULL)) {
            break;
        }

        if (actorIter->next == NULL) {
            actorIter = NULL;
            break;
        }
        actorIter = actorIter->next;
    }

    return actorIter;
}

EnDoor* EnIg_FindScheduleDoor(PlayState* play, s32 scheduleOutputResult) {
    EnDoorScheduleType schType;

    switch (scheduleOutputResult) {
        case 5:
        case 6:
        case 7:
        case 9:
            schType = ENDOOR_SCH_TYPE_INN_MAIN_ENTRANCE;
            break;

        case 8:
            schType = ENDOOR_SCH_TYPE_INN_KNIFE_CHAMBER;
            break;

        default:
            schType = -1;
            break;
    }

    return SubS_FindScheduleDoor(play, schType);
}

void EnIg_UpdateSkelAnime(EnIg* this) {
    this->skelAnime.playSpeed = this->animPlaySpeed;
    SkelAnime_Update(&this->skelAnime);
}

s32 EnIg_ChangeAnim(EnIg* this, s32 animIndex) {
    s32 changeAnim = false;
    s32 didAnimChange = false;

    switch (animIndex) {
        case ENIG_ANIM_0:
        case ENIG_ANIM_1:
            if ((this->animIndex != ENIG_ANIM_0) && (this->animIndex != ENIG_ANIM_1)) {
                changeAnim = true;
            }
            break;

        case ENIG_ANIM_2:
        case ENIG_ANIM_3:
            if ((this->animIndex != ENIG_ANIM_2) && (this->animIndex != ENIG_ANIM_3)) {
                changeAnim = true;
            }
            break;

        default:
            if (this->animIndex != animIndex) {
                changeAnim = true;
            }
            break;
    }

    if (changeAnim) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        this->animPlaySpeed = this->skelAnime.playSpeed;
    }

    return didAnimChange;
}

void func_80BF1354(EnIg* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider1);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
    this->collider2.dim.worldSphere.radius = this->collider2.dim.modelSphere.radius * this->collider2.dim.scale;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider2.base);
}

void func_80BF13E4(EnIg* this) {
    if ((this->unk_3D0 & 0x100) && (DECR(this->unk_3F0) == 0)) {
        this->unk_3F2++;
        if (this->unk_3F2 >= 4) {
            this->unk_3F0 = Rand_S16Offset(30, 30);
            this->unk_3F2 = 0;
        }
    }
}

Actor* func_80BF146C(EnIg* this, PlayState* play) {
    Actor* retActor;

    if (this->scheduleResult == 3) {
        retActor = EnIg_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_AN);
    } else {
        retActor = &GET_PLAYER(play)->actor;
    }
    return retActor;
}

void func_80BF14B0(EnIg* this) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    s32 pad2;

    Math_Vec3f_Copy(&sp40, &this->unk_2A8->world.pos);
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
    Math_ApproachS(&this->unk_3EA, Math_Vec3f_Yaw(&sp34, &sp40) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_3EA = CLAMP(this->unk_3EA, -0x1C70, 0x1C70);

    Math_Vec3f_Copy(&sp34, &this->actor.focus.pos);

    if (this->unk_2A8->id == ACTOR_PLAYER) {
        sp40.y = ((Player*)this->unk_2A8)->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&sp40, &this->unk_2A8->focus.pos);
    }

    Math_ApproachS(&this->unk_3E8, Math_Vec3f_Pitch(&sp34, &sp40), 4, 0x2AA8);
    this->unk_3E8 = CLAMP(this->unk_3E8, -0xE38, 0xE38);
}

void func_80BF15EC(EnIg* this) {
    if ((this->unk_3D0 & 0x20) && (this->unk_2A8 != 0)) {
        if (DECR(this->unk_3EE) == 0) {
            func_80BF14B0(this);
            this->unk_3D0 &= ~0x200;
            this->unk_3D0 |= 0x80;
            return;
        }
    }

    if (this->unk_3D0 & 0x80) {
        this->unk_3D0 &= ~0x80;
        this->unk_3E4 = 0;
        this->unk_3E6 = 0;
        this->unk_3E8 = 0;
        this->unk_3EA = 0;
        this->unk_3EE = 20;
        return;
    }

    if (DECR(this->unk_3EE) == 0) {
        this->unk_3D0 |= 0x200;
    }
}

s32 func_80BF16C8(EnIg* this, s16 csId) {
    s32 ret = false;

    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(csId);
    } else if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_StartWithPlayerCs(csId, &this->actor);
        ret = true;
    } else {
        CutsceneManager_Queue(csId);
    }
    return ret;
}

s16 func_80BF1744(EnIg* this, s32 numCutscenes) {
    s16 csId = CS_ID_NONE;
    s32 i;

    if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
        csId = this->actor.child->csId;
        for (i = 0; i < numCutscenes; i++) {
            csId = CutsceneManager_GetAdditionalCsId(csId);
        }
    }
    return csId;
}

s32 func_80BF17BC(Actor* thisx, PlayState* play) {
    EnIg* this = (EnIg*)thisx;
    s16 csId;
    s32 ret;

    csId = func_80BF1744(this, 0);
    ret = false;

    switch (this->unk_3F6) {
        case 0:
            if (!func_80BF16C8(this, csId)) {
                break;
            }
            // fallthrough
        case 2:
        case 4:
            if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
                Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(csId)),
                                      this->actor.child);
            }
            this->unk_3F6++;
            ret = true;
            break;

        case 1:
        case 3:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ROOM_KEY) && (this->unk_3F6 == 3)) {
                CutsceneManager_Stop(csId);
                this->unk_3F6 = 5;
            } else {
                Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(csId)), &this->actor);
            }
            this->unk_3F6++;
            ret = true;
            break;

        case 5:
            CutsceneManager_Stop(csId);
            this->unk_3F6++;
            ret = true;
            break;

        default:
            break;
    }
    return ret;
}

MsgScript* EnIg_GetMsgScript(EnIg* this, PlayState* play) {
    switch (this->scheduleResult) {
        case 3:
            this->msgScriptCallback = func_80BF17BC;
            return D_80BF335C;

        case 4:
            return D_80BF33F0;

        case 2:
        case 11:
        case 14:
            return D_80BF33E0;

        case 10:
        case 12:
            return D_80BF3354;

        case 13:
            return D_80BF33E8;

        default:
            return NULL;
    }
}

s32 func_80BF19A0(EnIg* this, PlayState* play) {
    s32 ret = false;

    if (((this->unk_3D0 & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        SubS_SetOfferMode(&this->unk_3D0, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->unk_3F6 = 0;
        this->msgScriptCallback = NULL;
        this->actor.child = this->unk_2A8;
        this->msgScript = EnIg_GetMsgScript(this, play);
        if ((this->scheduleResult != 2) && (this->scheduleResult != 3) && (this->scheduleResult != 4)) {
            this->unk_3D0 |= 0x20;
        }
        this->actionFunc = func_80BF2BD4;
        ret = true;
    }
    return ret;
}

void func_80BF1A60(EnIg* this, PlayState* play) {
    if (this->unk_3F4 == 0) {
        EnIg_ChangeAnim(this, ENIG_ANIM_4);
        this->unk_3F4++;
    } else if ((this->unk_3F4 == 1) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnIg_ChangeAnim(this, ENIG_ANIM_5);
        this->unk_3F4++;
    }
}

s32 func_80BF1AE0(EnIg* this, PlayState* play) {
    switch (this->scheduleResult) {
        case 3:
            EnIg_ChangeAnim(this, ENIG_ANIM_0);
            break;

        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            EnIg_ChangeAnim(this, ENIG_ANIM_2);
            break;

        default:
            break;
    }
    return true;
}

s32 func_80BF1B40(EnIg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 temp = play->msgCtx.currentTextId;
    s32 pad;

    if (player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_400 | PLAYER_STATE1_CARRYING_ACTOR)) {
        this->unk_3D0 |= 0x400;
        if (this->unk_3D2 != temp) {
            if ((this->animIndex == ENIG_ANIM_2) || (this->animIndex == ENIG_ANIM_3)) {
                EnIg_ChangeAnim(this, ENIG_ANIM_0);
            }

            if ((temp == 0x28B0) || (temp == 0x28B7)) {
                this->unk_18C = func_80BF1A60;
                this->unk_3F4 = 0;
            }
        }
        this->unk_3D2 = temp;
    } else if (this->unk_3D0 & 0x400) {
        this->unk_3D2 = 0;
        this->unk_3D0 &= ~0x400;

        //! FAKE:
        if (1) {}

        func_80BF1AE0(this, play);
    }

    if (this->unk_18C != NULL) {
        this->unk_18C(this, play);
    }

    return false;
}

s32 func_80BF1C44(EnIg* this, PlayState* play, ScheduleOutput* scheduleOutput, s32 actorCategory, s32 actorId) {
    u8 pathIndex = ENIG_GET_PATH_INDEX(&this->actor);
    Vec3s* sp48;
    Vec3f sp3C;
    Vec3f sp30;
    Actor* sp2C;
    s32 pad;
    s32 sp24 = false;

    sp2C = EnIg_FindActor(this, play, actorCategory, actorId);
    this->timePath = NULL;

    if (D_80BF3318[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80BF3318[scheduleOutput->result]);
    }

    if ((sp2C != NULL) && (sp2C->update != NULL)) {
        if (this->timePath != NULL) {
            sp48 = Lib_SegmentedToVirtual(this->timePath->points);
            Math_Vec3s_ToVec3f(&sp3C, &sp48[this->timePath->count - 2]);
            Math_Vec3s_ToVec3f(&sp30, &sp48[this->timePath->count - 1]);
            this->actor.shape.shadowDraw = NULL;
            this->actor.world.rot.y = Math_Vec3f_Yaw(&sp3C, &sp30);
            Math_Vec3f_Copy(&this->actor.world.pos, &sp30);
            sp24 = true;
        }
    }
    return sp24;
}

s32 func_80BF1D78(EnIg* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 sp2C = 0;

    if (func_80BF1C44(this, play, scheduleOutput, ACTORCAT_NPC, ACTOR_EN_AN)) {
        EnIg_ChangeAnim(this, ENIG_ANIM_0);
        SubS_SetOfferMode(&this->unk_3D0, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_3D0 |= 0x20;
        this->unk_3D0 |= 0x100;
        sp2C = true;
    }
    return sp2C;
}

s32 func_80BF1DF4(EnIg* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    u16 sp56 = SCRIPT_TIME_NOW;
    u8 pathIndex = ENIG_GET_PATH_INDEX(&this->actor);
    EnDoor* door;
    Vec3s* sp4C;
    Vec3f sp40;
    Vec3f sp34;
    s32 pad;
    s32 ret = false;

    this->timePath = NULL;
    door = EnIg_FindScheduleDoor(play, scheduleOutput->result);

    if (D_80BF3318[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80BF3318[scheduleOutput->result]);
    }

    if ((door != NULL) && (door->knobDoor.dyna.actor.update != NULL)) {
        if (this->timePath != NULL) {
            sp4C = Lib_SegmentedToVirtual(this->timePath->points);
            Math_Vec3s_ToVec3f(&sp40, &sp4C[0]);
            Math_Vec3s_ToVec3f(&sp34, &sp4C[1]);
            Math_Vec3f_Copy(&this->unk_2B0, &sp40);
            Math_Vec3f_Copy(&this->unk_2BC, &sp34);
            this->actor.world.rot.y = Math_Vec3f_Yaw(&sp40, &sp34);
            Math_Vec3f_Copy(&this->actor.world.pos, &sp40);

            if (ABS_ALT(BINANG_SUB(this->actor.world.rot.y, door->knobDoor.dyna.actor.shape.rot.y)) <= 0x4000) {
                this->unk_2A4 = -75;
            } else {
                this->unk_2A4 = 75;
            }

            this->unk_3E0 = scheduleOutput->time1 - scheduleOutput->time0;
            this->unk_3E2 = sp56 - scheduleOutput->time0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->unk_3D0 |= 0x100;
            EnIg_ChangeAnim(this, ENIG_ANIM_3);
            this->actor.gravity = 0.0f;
            ret = true;
        }
    }
    return ret;
}

s32 func_80BF1FA8(EnIg* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    u16 sp2E = SCRIPT_TIME_NOW;
    u16 phi_v1;
    u8 pathIndex = ENIG_GET_PATH_INDEX(&this->actor);
    u16 tmp;
    s16 pad;
    s32 ret = false;

    this->timePath = NULL;

    if (D_80BF3318[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80BF3318[scheduleOutput->result]);
    }

    if ((this->timePath != NULL) && (this->timePath->count < 3)) {
        this->timePath = NULL;
    }

    if (this->timePath != NULL) {
        if ((this->scheduleResult < 10) && (this->scheduleResult != 0) && (this->timePathTimeSpeed >= 0)) {
            phi_v1 = sp2E;
        } else {
            phi_v1 = scheduleOutput->time0;
        }

        if (scheduleOutput->time1 < phi_v1) {
            this->timePathTotalTime = (phi_v1 - scheduleOutput->time1) + 0xFFFF;
        } else {
            this->timePathTotalTime = scheduleOutput->time1 - phi_v1;
        }

        this->timePathElapsedTime = sp2E - phi_v1;

        tmp = phi_v1 = this->timePath->count - (SUBS_TIME_PATHING_ORDER - 1);

        this->timePathWaypointTime = this->timePathTotalTime / tmp;
        this->timePathWaypoint =
            (this->timePathElapsedTime / this->timePathWaypointTime) + (SUBS_TIME_PATHING_ORDER - 1);

        this->unk_3D0 &= ~0x8;
        this->unk_3D0 &= ~0x10;
        SubS_SetOfferMode(&this->unk_3D0, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_3D0 |= 0x100;
        EnIg_ChangeAnim(this, ENIG_ANIM_2);
        this->actor.gravity = -1.0f;
        ret = true;
    }
    return ret;
}

s32 func_80BF219C(EnIg* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    u8 pathIndex = ENIG_GET_PATH_INDEX(&this->actor);
    Vec3f sp40;
    Vec3f sp34;
    Vec3s* sp30;
    s32 pad;
    s32 ret = false;

    this->timePath = NULL;

    if (D_80BF3318[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80BF3318[scheduleOutput->result]);
    }

    if ((this->timePath != 0) && (this->timePath->count >= 2)) {
        sp30 = Lib_SegmentedToVirtual(this->timePath->points);
        Math_Vec3s_ToVec3f(&sp40, &sp30[this->timePath->count - 1]);
        Math_Vec3s_ToVec3f(&sp34, &sp30[this->timePath->count - 2]);
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp34, &sp40);
        Math_Vec3s_Copy(&this->actor.shape.rot, &this->actor.world.rot);
        Math_Vec3f_Copy(&this->actor.world.pos, &sp40);
        Math_Vec3f_Copy(&this->actor.prevPos, &sp40);

        switch (scheduleOutput->result) {
            case 2:
                this->actor.home.rot.y = this->actor.world.rot.y;
                this->actor.home.rot.y += 0x8000;
                SubS_SetOfferMode(&this->unk_3D0, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
                this->unk_3D0 |= 0x100;
                EnIg_ChangeAnim(this, ENIG_ANIM_1);
                break;

            case 4:
                this->actor.world.rot.y += 0x8000;
                this->actor.shape.rot.y = this->actor.world.rot.y;
                SubS_SetOfferMode(&this->unk_3D0, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
                this->unk_3D0 |= 0x100;
                EnIg_ChangeAnim(this, ENIG_ANIM_8);
                break;

            default:
                break;
        }
        ret = true;
    }
    return ret;
}

s32 func_80BF2368(EnIg* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;

    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->unk_3D0 = 0;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

    switch (scheduleOutput->result) {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            ret = func_80BF1DF4(this, play, scheduleOutput);
            break;

        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            ret = func_80BF1FA8(this, play, scheduleOutput);
            break;

        case 2:
        case 4:
            ret = func_80BF219C(this, play, scheduleOutput);
            break;

        case 3:
            ret = func_80BF1D78(this, play, scheduleOutput);
            break;

        default:
            break;
    }
    return ret;
}

s32 func_80BF2400(EnIg* this, PlayState* play) {
    Vec3f sp2C;
    Vec3f sp20;

    if ((this->unk_2A8 != NULL) && (this->unk_2A8->update != NULL)) {
        Math_Vec3f_Copy(&sp2C, &this->unk_2A8->world.pos);
        Math_Vec3f_Copy(&sp20, &this->actor.world.pos);
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp20, &sp2C);
    }

    return true;
}

s32 func_80BF2470(EnIg* this, PlayState* play) {
    EnDoor* door = EnIg_FindScheduleDoor(play, this->scheduleResult);
    Vec3f sp38;
    f32 temp;
    s32 pad;

    if (!SubS_InCsMode(play) && (this->timePathTimeSpeed != 0)) {
        if ((door != NULL) && (door->knobDoor.dyna.actor.update != NULL)) {
            if (((f32)this->unk_3E2 / this->unk_3E0) <= 0.9f) {
                door->openTimer = this->unk_2A4;
            } else {
                door->openTimer = 0;
            }
        }
        this->unk_3E2 = CLAMP(this->unk_3E2, 0, this->unk_3E0);
        temp = Math_Vec3f_DistXZ(&this->unk_2B0, &this->unk_2BC) / this->unk_3E0;
        sp38.x = 0.0f;
        sp38.y = 0.0f;
        sp38.z = this->unk_3E2 * temp;
        Lib_Vec3f_TranslateAndRotateY(&this->unk_2B0, this->actor.world.rot.y, &sp38, &this->actor.world.pos);
        this->unk_3E2 += this->timePathTimeSpeed;
        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 13.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_PIRATE_WALK);
        }
    }
    return false;
}

s32 func_80BF25E8(EnIg* this, PlayState* play) {
    f32 knots[265];
    Vec3f sp70;
    Vec3f sp64;
    Vec3f timePathTargetPos;
    s32 sp54 = 0;
    s32 sp50 = 0;
    s32 pad;

    SubS_TimePathing_FillKnots(knots, SUBS_TIME_PATHING_ORDER, this->timePath->count + SUBS_TIME_PATHING_ORDER);

    if (!(this->unk_3D0 & 8)) {
        timePathTargetPos = gZeroVec3f;
        SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint, knots,
                                &timePathTargetPos, this->timePathTimeSpeed);
        SubS_TimePathing_ComputeInitialY(play, this->timePath, this->timePathWaypoint, &timePathTargetPos);
        this->actor.world.pos.y = timePathTargetPos.y;
        this->unk_3D0 |= 8;
    } else {
        timePathTargetPos = this->timePathTargetPos;
    }

    this->actor.world.pos.x = timePathTargetPos.x;
    this->actor.world.pos.z = timePathTargetPos.z;

    if (SubS_InCsMode(play)) {
        sp54 = this->timePathElapsedTime;
        sp50 = this->timePathWaypoint;
        timePathTargetPos = this->actor.world.pos;
    }

    this->timePathTargetPos = gZeroVec3f;

    if (SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint, knots,
                                &this->timePathTargetPos, this->timePathTimeSpeed)) {
        this->unk_3D0 |= 0x10;
    } else {
        sp70 = this->actor.world.pos;
        sp64 = this->timePathTargetPos;
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp70, &sp64);
    }

    if (SubS_InCsMode(play)) {
        this->timePathElapsedTime = sp54;
        this->timePathWaypoint = sp50;
        this->timePathTargetPos = timePathTargetPos;
    } else if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 13.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_PIRATE_WALK);
    }
    return false;
}

s32 func_80BF2890(EnIg* this, PlayState* play) {
    if ((this->unk_3D0 & 0x100) && (this->unk_3F2 == 2)) {
        this->unk_3D0 &= ~0x100;
    }

    if (!(this->unk_3D0 & 0x100) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (this->unk_408 != 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_SNORE1);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_SNORE2);
        }
        this->unk_408 ^= 1;
    }
    return true;
}

s32 func_80BF293C(EnIg* this, PlayState* play) {
    if (this->actor.world.rot.y != this->actor.home.rot.y) {
        if ((this->actor.world.rot.y / 182) != (this->actor.home.rot.y / 182)) {
            Math_ApproachS(&this->actor.world.rot.y, this->actor.home.rot.y, 3, 0x2AA8);
        } else {
            this->actor.world.rot.y = this->actor.home.rot.y;
            EnIg_ChangeAnim(this, ENIG_ANIM_7);
        }
    } else if ((this->animIndex == ENIG_ANIM_7) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        SubS_SetOfferMode(&this->unk_3D0, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        EnIg_ChangeAnim(this, ENIG_ANIM_9);
    }
    return true;
}

void func_80BF2A50(EnIg* this, PlayState* play) {
    switch (this->scheduleResult) {
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            func_80BF25E8(this, play);
            break;

        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            func_80BF2470(this, play);
            break;

        case 2:
            func_80BF293C(this, play);
            break;

        case 3:
            func_80BF2400(this, play);
            break;

        case 4:
            func_80BF2890(this, play);
            break;

        default:
            break;
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
}

void func_80BF2AF8(EnIg* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    this->timePathTimeSpeed = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);

    if (!Schedule_RunScript(play, D_80BF3260, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) && !func_80BF2368(this, play, &scheduleOutput))) {
        this->actor.shape.shadowDraw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = 0;
    } else {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->unk_2A8 = func_80BF146C(this, play);
    this->scheduleResult = scheduleOutput.result;
    func_80BF2A50(this, play);
}

void func_80BF2BD4(EnIg* this, PlayState* play) {
    s16 yaw;
    Vec3f sp38;
    Vec3f sp2C;

    if (MsgEvent_RunScript(&this->actor, play, this->msgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk_3D0, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_3D0 &= ~0x20;
        this->unk_3D0 |= 0x200;
        this->unk_3EE = 20;
        this->msgScriptPos = 0;
        this->actionFunc = func_80BF2AF8;
    } else if (((this->scheduleResult != 2) && (this->scheduleResult != 4)) &&
               ((this->unk_2A8 != NULL) && (this->unk_2A8->update != NULL))) {
        Math_Vec3f_Copy(&sp38, &this->unk_2A8->world.pos);
        Math_Vec3f_Copy(&sp2C, &this->actor.world.pos);
        yaw = Math_Vec3f_Yaw(&sp2C, &sp38);
        Math_ApproachS(&this->actor.shape.rot.y, yaw, 4, 0x2AA8);
    }
}

void EnIg_Init(Actor* thisx, PlayState* play) {
    EnIg* this = (EnIg*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 28.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_dai_Skel_0130D0, NULL, this->jointTable, this->morphTable,
                       OBJECT_DAI_LIMB_MAX);
    this->animIndex = ENIG_ANIM_NONE;
    EnIg_ChangeAnim(this, ENIG_ANIM_0);
    Collider_InitAndSetCylinder(play, &this->collider1, &this->actor, &sCylinderInit);
    Collider_InitAndSetSphere(play, &this->collider2, &this->actor, &sSphereInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01f);
    this->scheduleResult = 0;
    this->actor.gravity = 0.0f;
    this->actionFunc = func_80BF2AF8;
    this->actionFunc(this, play);
}

void EnIg_Destroy(Actor* thisx, PlayState* play) {
    EnIg* this = (EnIg*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroySphere(play, &this->collider2);
}

void EnIg_Update(Actor* thisx, PlayState* play) {
    EnIg* this = (EnIg*)thisx;

    func_80BF19A0(this, play);

    this->actionFunc(this, play);

    func_80BF1B40(this, play);

    if (this->scheduleResult != 0) {
        EnIg_UpdateSkelAnime(this);
        func_80BF13E4(this);
        func_80BF15EC(this);
        SubS_Offer(&this->actor, play, 60.0f, 30.0f, PLAYER_IA_NONE, this->unk_3D0 & SUBS_OFFER_MODE_MASK);
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
        func_80BF1354(this, play);
    }
}

s32 EnIg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                          Gfx** gfx) {
    EnIg* this = (EnIg*)thisx;

    if (limbIndex == OBJECT_DAI_LIMB_0A) {
        *dList = NULL;
    }
    return false;
}

void EnIg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    static Vec3f D_80BF351C = { 1800.0f, -2000.0f, 0.0f };
    static Vec3f D_80BF3528 = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    EnIg* this = (EnIg*)thisx;
    Vec3f sp2C;

    if (limbIndex == OBJECT_DAI_LIMB_0B) {
        Matrix_MultVec3f(&D_80BF3528, &this->actor.focus.pos);
        Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);

        gSPDisplayList((*gfx)++, object_dai_DL_008710);
        gSPDisplayList((*gfx)++, object_dai_DL_0087B8);
    }

    if (limbIndex == OBJECT_DAI_LIMB_0C) {
        gSPDisplayList((*gfx)++, object_dai_DL_0089D8);
    }

    if (limbIndex == OBJECT_DAI_LIMB_09) {
        gSPDisplayList((*gfx)++, object_dai_DL_008B00);
        Matrix_MultVec3f(&D_80BF351C, &sp2C);
        Math_Vec3f_ToVec3s(&this->collider2.dim.worldSphere.center, &sp2C);
    }

    if (limbIndex == OBJECT_DAI_LIMB_0A) {
        Matrix_Get(&this->unk_190);
    }
}

void EnIg_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx, Gfx** gfx) {
    EnIg* this = (EnIg*)thisx;
    s32 stepRot;
    s32 overrideRot;

    if (!(this->unk_3D0 & 0x200)) {
        if (this->unk_3D0 & 0x80) {
            overrideRot = true;
        } else {
            overrideRot = false;
        }
        stepRot = true;
    } else {
        overrideRot = false;
        stepRot = false;
    }

    if (limbIndex == OBJECT_DAI_LIMB_09) {
        SubS_UpdateLimb(this->unk_3E8 + 0x4000, this->unk_3EA + this->actor.shape.rot.y + 0x4000, &this->unk_2D4,
                        &this->unk_2E6, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->unk_2D4.x, this->unk_2D4.y, this->unk_2D4.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_2E6.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_2E6.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_2E6.z, MTXMODE_APPLY);
        Matrix_Push();
    }
}

void EnIg_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr D_80BF3534[] = {
        object_dai_Tex_0107B0, object_dai_Tex_010FB0, object_dai_Tex_0117B0,
        object_dai_Tex_011FB0, object_dai_Tex_0127B0,
    };
    s32 pad;
    EnIg* this = (EnIg*)thisx;

    if (this->scheduleResult != 0) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        OPEN_DISPS(play->state.gfxCtx);

        Scene_SetRenderModeXlu(play, 0, 1);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80BF3534[this->unk_3F2]));

        POLY_OPA_DISP = SubS_DrawTransformFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                               this->skelAnime.dListCount, EnIg_OverrideLimbDraw, EnIg_PostLimbDraw,
                                               EnIg_TransformLimbDraw, &this->actor, POLY_OPA_DISP);
        Matrix_Put(&this->unk_190);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_dai_DL_00C538);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

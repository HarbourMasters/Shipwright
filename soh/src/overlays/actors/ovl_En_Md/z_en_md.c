/*
 * File: z_en_md.c
 * Overlay: ovl_En_Md
 * Description: Mido
 */

#include "z_en_md.h"
#include "objects/object_md/object_md.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnMd_Init(Actor* thisx, PlayState* play);
void EnMd_Destroy(Actor* thisx, PlayState* play);
void EnMd_Update(Actor* thisx, PlayState* play);
void EnMd_Draw(Actor* thisx, PlayState* play);

void func_80AAB874(EnMd* this, PlayState* play);
void func_80AAB8F8(EnMd* this, PlayState* play);
void func_80AAB948(EnMd* this, PlayState* play);
void func_80AABC10(EnMd* this, PlayState* play);
void func_80AABD0C(EnMd* this, PlayState* play);

const ActorInit En_Md_InitVars = {
    ACTOR_EN_MD,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MD,
    sizeof(EnMd),
    (ActorFunc)EnMd_Init,
    (ActorFunc)EnMd_Destroy,
    (ActorFunc)EnMd_Update,
    (ActorFunc)EnMd_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 36, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /*  0 */ ENMD_ANIM_0,
    /*  1 */ ENMD_ANIM_1,
    /*  2 */ ENMD_ANIM_2,
    /*  3 */ ENMD_ANIM_3,
    /*  4 */ ENMD_ANIM_4,
    /*  5 */ ENMD_ANIM_5,
    /*  6 */ ENMD_ANIM_6,
    /*  7 */ ENMD_ANIM_7,
    /*  8 */ ENMD_ANIM_8,
    /*  9 */ ENMD_ANIM_9,
    /* 10 */ ENMD_ANIM_10,
    /* 11 */ ENMD_ANIM_11,
    /* 12 */ ENMD_ANIM_12,
    /* 13 */ ENMD_ANIM_13,
} EnMdAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gMidoHandsOnHipsIdleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMidoHandsOnHipsIdleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMidoRaiseHand1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoHaltAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoPutHandDownAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoAnnoyedPointedHeadIdle1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoAnnoyedPointedHeadIdle2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoAnim_92B0, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoWalkingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoHandsOnHipsTransitionAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoHandsOnHipsIdleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gMidoSlamAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoRaiseHand2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoAngryHeadTurnAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
};

void func_80AAA250(EnMd* this) {
    f32 startFrame;

    startFrame = this->skelAnime.startFrame;
    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.curFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = startFrame;
    this->skelAnime.playSpeed = -1.0f;
}

void func_80AAA274(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_2);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_3);
                this->unk_20A++;
            }
    }
}

void func_80AAA308(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_4);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_5);
                this->unk_20A++;
            }
    }
}

void func_80AAA39C(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_2);
            func_80AAA250(this);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_7);
                this->unk_20A++;
            } else {
                break;
            }
        case 2:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_8);
                this->unk_20A++;
            }
    }
}

void func_80AAA474(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_7);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                this->unk_20A++;
            }
    }
}

void func_80AAA508(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_2);
            func_80AAA250(this);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                this->unk_20A++;
            }
    }
}

void func_80AAA5A4(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_9);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_6);
                this->unk_20A++;
            }
    }
}

void func_80AAA638(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_9);
            func_80AAA250(this);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                this->unk_20A++;
            }
    }
}

void func_80AAA6D4(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_11);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_6);
                this->unk_20A++;
            }
    }
}

void func_80AAA768(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_12);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_3);
                this->unk_20A++;
            }
    }
}

void func_80AAA7FC(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_13);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_6);
                this->unk_20A++;
            }
    }
}

void func_80AAA890(EnMd* this) {
    switch (this->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_7);
            func_80AAA250(this);
            this->unk_20A++;
        case 1:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                this->unk_20A++;
            }
    }
}

void func_80AAA92C(EnMd* this, u8 arg1) {
    this->unk_20B = arg1;
    this->unk_20A = 0;
}

void func_80AAA93C(EnMd* this) {
    switch (this->unk_20B) {
        case 1:
            func_80AAA274(this);
            break;
        case 2:
            func_80AAA308(this);
            break;
        case 3:
            func_80AAA39C(this);
            break;
        case 4:
            func_80AAA474(this);
            break;
        case 5:
            func_80AAA508(this);
            break;
        case 6:
            func_80AAA5A4(this);
            break;
        case 7:
            func_80AAA638(this);
            break;
        case 8:
            func_80AAA6D4(this);
            break;
        case 9:
            func_80AAA768(this);
            break;
        case 10:
            func_80AAA7FC(this);
            break;
        case 11:
            func_80AAA890(this);
    }
}

void func_80AAAA24(EnMd* this) {
    if (this->unk_1E0.unk_00 != 0) {
        switch (this->actor.textId) {
            case 0x102F:
                if ((this->unk_208 == 0) && (this->unk_20B != 1)) {
                    func_80AAA92C(this, 1);
                }
                if ((this->unk_208 == 2) && (this->unk_20B != 2)) {
                    func_80AAA92C(this, 2);
                }
                if ((this->unk_208 == 5) && (this->unk_20B != 8)) {
                    func_80AAA92C(this, 8);
                }
                if ((this->unk_208 == 11) && (this->unk_20B != 9)) {
                    func_80AAA92C(this, 9);
                }
                break;
            case 0x1033:
                if ((this->unk_208 == 0) && (this->unk_20B != 1)) {
                    func_80AAA92C(this, 1);
                }
                if ((this->unk_208 == 1) && (this->unk_20B != 2)) {
                    func_80AAA92C(this, 2);
                }
                if ((this->unk_208 == 5) && (this->unk_20B != 10)) {
                    func_80AAA92C(this, 10);
                }
                if ((this->unk_208 == 7) && (this->unk_20B != 9)) {
                    func_80AAA92C(this, 9);
                }
                break;
            case 0x1030:
            case 0x1034:
            case 0x1045:
                if ((this->unk_208 == 0) && (this->unk_20B != 1)) {
                    func_80AAA92C(this, 1);
                }
                break;
            case 0x1046:
                if ((this->unk_208 == 0) && (this->unk_20B != 6)) {
                    func_80AAA92C(this, 6);
                }
                break;
        }
    } else if (this->skelAnime.animation != &gMidoHandsOnHipsIdleAnim) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_10);
        func_80AAA92C(this, 0);
    }

    func_80AAA93C(this);
}

s16 func_80AAAC78(EnMd* this, PlayState* play) {
    s16 dialogState = Message_GetState(&play->msgCtx);

    if ((this->unk_209 == TEXT_STATE_AWAITING_NEXT) || (this->unk_209 == TEXT_STATE_EVENT) ||
        (this->unk_209 == TEXT_STATE_CLOSING) || (this->unk_209 == TEXT_STATE_DONE_HAS_NEXT)) {
        if (this->unk_209 != dialogState) {
            this->unk_208++;
        }
    }

    this->unk_209 = dialogState;
    return dialogState;
}

u16 EnMd_GetTextKokiriForest(PlayState* play, EnMd* this) {
    u16 reactionText = Text_GetFaceReaction(play, 0x11);

    if (reactionText != 0) {
        return reactionText;
    }

    this->unk_208 = 0;
    this->unk_209 = TEXT_STATE_NONE;

    if ((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) ||
        (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DEKU_TREE))) {
        return 0x1045;
    }

    if (gSaveContext.eventChkInf[0] & 0x10) {
        return 0x1034;
    }

    if ((CUR_EQUIP_VALUE(EQUIP_SHIELD) == 1) && (CUR_EQUIP_VALUE(EQUIP_SWORD) == 1)) {
        return 0x1033;
    }

    if (gSaveContext.infTable[0] & 0x1000) {
        return 0x1030;
    }

    return 0x102F;
}

u16 EnMd_GetTextKokiriHome(PlayState* play, EnMd* this) {
    this->unk_208 = 0;
    this->unk_209 = TEXT_STATE_NONE;

    if (gSaveContext.eventChkInf[4] & 1) {
        return 0x1028;
    }

    return 0x1046;
}

u16 EnMd_GetTextLostWoods(PlayState* play, EnMd* this) {
    this->unk_208 = 0;
    this->unk_209 = TEXT_STATE_NONE;

    if (gSaveContext.eventChkInf[4] & 0x100) {
        if (gSaveContext.infTable[1] & 0x200) {
            return 0x1071;
        }
        return 0x1070;
    }

    if (gSaveContext.eventChkInf[0] & 0x400) {
        return 0x1068;
    }

    if (gSaveContext.infTable[1] & 0x20) {
        return 0x1061;
    }

    return 0x1060;
}

u16 EnMd_GetText(PlayState* play, Actor* thisx) {
    EnMd* this = (EnMd*)thisx;

    switch (play->sceneNum) {
        case SCENE_SPOT04:
            return EnMd_GetTextKokiriForest(play, this);
        case SCENE_KOKIRI_HOME4:
            return EnMd_GetTextKokiriHome(play, this);
        case SCENE_SPOT10:
            return EnMd_GetTextLostWoods(play, this);
        default:
            return 0;
    }
}

s16 func_80AAAF04(PlayState* play, Actor* thisx) {
    EnMd* this = (EnMd*)thisx;
    switch (func_80AAAC78(this, play)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return 1;
        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x1028:
                    gSaveContext.eventChkInf[0] |= 0x8000;
                    break;
                case 0x102F:
                    gSaveContext.eventChkInf[0] |= 4;
                    gSaveContext.infTable[0] |= 0x1000;
                    break;
                case 0x1060:
                    gSaveContext.infTable[1] |= 0x20;
                    break;
                case 0x1070:
                    gSaveContext.infTable[1] |= 0x200;
                    break;
                case 0x1033:
                case 0x1067:
                    return 2;
            }
            return 0;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                return 2;
            }
        default:
            return 1;
    }
}

u8 EnMd_ShouldSpawn(EnMd* this, PlayState* play) {
    if (play->sceneNum == SCENE_SPOT04) {
        if (gSaveContext.n64ddFlag) {
            // if we have beaten deku tree or have open forest turned on
            // or have already shown mido we have an equipped sword/shield
            if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DEKU_TREE) ||
                gSaveContext.eventChkInf[0] & 0x10) {
                return 0;
            }
            return 1;
        }

        if (!(gSaveContext.eventChkInf[1] & 0x1000) && !(gSaveContext.eventChkInf[4] & 1)) {
            return 1;
        }
    }

    if (play->sceneNum == SCENE_KOKIRI_HOME4) {
        if (((gSaveContext.eventChkInf[1] & 0x1000) != 0) || ((gSaveContext.eventChkInf[4] & 1) != 0)) {
            if (!LINK_IS_ADULT) {
                return 1;
            }
        }
    }

    if (play->sceneNum == SCENE_SPOT10) {
        return 1;
    }

    return 0;
}

void EnMd_UpdateEyes(EnMd* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIdx++;
        if (this->eyeIdx > 2) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeIdx = 0;
        }
    }
}

void func_80AAB158(EnMd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 absYawDiff;
    s16 temp;
    s16 temp2;
    s16 yawDiff;

    if (this->actor.xzDistToPlayer < 170.0f) {
        yawDiff = (f32)this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        absYawDiff = ABS(yawDiff);

        temp = (absYawDiff <= func_800347E8(2)) ? 2 : 1;
        temp2 = 1;
    } else {
        temp = 1;
        temp2 = 0;
    }

    if (this->unk_1E0.unk_00 != 0) {
        temp = 4;
    }

    if (this->actionFunc == func_80AABD0C) {
        temp = 1;
        temp2 = 0;
    }
    if (this->actionFunc == func_80AAB8F8) {
        temp = 4;
        temp2 = 1;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || gDbgCamEnabled) {
        this->unk_1E0.unk_18 = play->view.eye;
        this->unk_1E0.unk_14 = 40.0f;
        temp = 2;
    } else {
        this->unk_1E0.unk_18 = player->actor.world.pos;
        this->unk_1E0.unk_14 = (gSaveContext.linkAge > 0) ? 0.0f : -18.0f;
    }

    func_80034A14(&this->actor, &this->unk_1E0, 2, temp);
    if (this->actionFunc != func_80AABC10) {
        if (temp2) {
            func_800343CC(play, &this->actor, &this->unk_1E0.unk_00, this->collider.dim.radius + 30.0f,
                          EnMd_GetText, func_80AAAF04);
        }
    }
}

u8 EnMd_FollowPath(EnMd* this, PlayState* play) {
    Path* path;
    Vec3s* pointPos;
    f32 pathDiffX;
    f32 pathDiffZ;

    if ((this->actor.params & 0xFF00) == 0xFF00) {
        return 0;
    }

    path = &play->setupPathList[(this->actor.params & 0xFF00) >> 8];
    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos += this->waypoint;

    pathDiffX = pointPos->x - this->actor.world.pos.x;
    pathDiffZ = pointPos->z - this->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_FAtan2F(pathDiffX, pathDiffZ) * (65536.0f / (2 * M_PI)), 4, 4000,
                       1);

    if ((SQ(pathDiffX) + SQ(pathDiffZ)) < 100.0f) {
        this->waypoint++;
        if (this->waypoint >= path->count) {
            this->waypoint = 0;
        }
        return 1;
    }
    return 0;
}

u8 EnMd_SetMovedPos(EnMd* this, PlayState* play) {
    Path* path;
    Vec3s* lastPointPos;

    if ((this->actor.params & 0xFF00) == 0xFF00) {
        return 0;
    }

    path = &play->setupPathList[(this->actor.params & 0xFF00) >> 8];
    lastPointPos = SEGMENTED_TO_VIRTUAL(path->points);
    lastPointPos += path->count - 1;

    this->actor.world.pos.x = lastPointPos->x;
    this->actor.world.pos.y = lastPointPos->y;
    this->actor.world.pos.z = lastPointPos->z;

    return 1;
}

void func_80AAB5A4(EnMd* this, PlayState* play) {
    f32 temp;

    if (play->sceneNum != SCENE_KOKIRI_HOME4) {
        if (CVar_GetS32("gDisableKokiriDrawDistance", 0) != 0) {
            temp = (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && !(gSaveContext.eventChkInf[1] & 0x1000) &&
                    (play->sceneNum == SCENE_SPOT04))
                       ? 100.0f
                       : 32767.0f;
        } else {
            temp = (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && !(gSaveContext.eventChkInf[1] & 0x1000) &&
                    (play->sceneNum == SCENE_SPOT04))
                       ? 100.0f
                       : 400.0f;
        }
        
        this->alpha = func_80034DD4(&this->actor, play, this->alpha, temp);
        this->actor.shape.shadowAlpha = this->alpha;
    } else {
        this->alpha = 255;
        this->actor.shape.shadowAlpha = this->alpha;
    }
}

void EnMd_Init(Actor* thisx, PlayState* play) {
    EnMd* this = (EnMd*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gMidoSkel, NULL, this->jointTable, this->morphTable, 17);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    if (!EnMd_ShouldSpawn(this, play)) {
        Actor_Kill(&this->actor);
        return;
    }

    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENMD_ANIM_0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.targetMode = 6;
    this->alpha = 255;
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, FAIRY_KOKIRI);

    if (((play->sceneNum == SCENE_SPOT04) && !(gSaveContext.eventChkInf[0] & 0x10)) ||
        ((play->sceneNum == SCENE_SPOT04) && (gSaveContext.eventChkInf[0] & 0x10) &&
         CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) ||
        ((play->sceneNum == SCENE_SPOT10) && !(gSaveContext.eventChkInf[0] & 0x400))) {
        this->actor.home.pos = this->actor.world.pos;
        this->actionFunc = func_80AAB948;
        return;
    }

    if (play->sceneNum != SCENE_KOKIRI_HOME4) {
        EnMd_SetMovedPos(this, play);
    }

    this->actionFunc = func_80AAB874;
}

void EnMd_Destroy(Actor* thisx, PlayState* play) {
    EnMd* this = (EnMd*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AAB874(EnMd* this, PlayState* play) {
    if (this->skelAnime.animation == &gMidoHandsOnHipsIdleAnim) {
        func_80034F54(play, this->unk_214, this->unk_236, 17);
    } else if ((this->unk_1E0.unk_00 == 0) && (this->unk_20B != 7)) {
        func_80AAA92C(this, 7);
    }

    func_80AAAA24(this);
}

void func_80AAB8F8(EnMd* this, PlayState* play) {
    if (this->skelAnime.animation == &gMidoHandsOnHipsIdleAnim) {
        func_80034F54(play, this->unk_214, this->unk_236, 17);
    }
    func_80AAA93C(this);
}

void func_80AAB948(EnMd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp;
    Actor* actorToBlock = &GET_PLAYER(play)->actor;
    s16 yaw;

    func_80AAAA24(this);

    if (this->unk_1E0.unk_00 == 0) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;

        yaw = Math_Vec3f_Yaw(&this->actor.home.pos, &actorToBlock->world.pos);

        this->actor.world.pos.x = this->actor.home.pos.x;
        this->actor.world.pos.x += 60.0f * Math_SinS(yaw);

        this->actor.world.pos.z = this->actor.home.pos.z;
        this->actor.world.pos.z += 60.0f * Math_CosS(yaw);

        temp = fabsf((f32)this->actor.yawTowardsPlayer - yaw) * 0.001f * 3.0f;
        this->skelAnime.playSpeed = CLAMP(temp, 1.0f, 3.0f);
    }

    if (this->unk_1E0.unk_00 == 2) {
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && !(gSaveContext.eventChkInf[1] & 0x1000) &&
            (play->sceneNum == SCENE_SPOT04)) {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
        }

        if (play->sceneNum == SCENE_SPOT04) {
            gSaveContext.eventChkInf[0] |= 0x10;
        }
        if (play->sceneNum == SCENE_SPOT10) {
            gSaveContext.eventChkInf[0] |= 0x400;
        }

        func_80AAA92C(this, 3);
        func_80AAA93C(this);
        this->waypoint = 1;
        this->unk_1E0.unk_00 = 0;
        this->actionFunc = func_80AABD0C;
        this->actor.speedXZ = 1.5f;
        return;
    }

    if (this->skelAnime.animation == &gMidoHandsOnHipsIdleAnim) {
        func_80034F54(play, this->unk_214, this->unk_236, 17);
    }

    if ((this->unk_1E0.unk_00 == 0) && (play->sceneNum == SCENE_SPOT10)) {
        if (player->stateFlags2 & 0x1000000) {
            player->stateFlags2 |= 0x2000000;
            player->unk_6A8 = &this->actor;
            func_8010BD58(play, OCARINA_ACTION_CHECK_SARIA);
            this->actionFunc = func_80AABC10;
            return;
        }

        if (this->actor.xzDistToPlayer < (30.0f + this->collider.dim.radius)) {
            player->stateFlags2 |= 0x800000;
        }
    }
}

void func_80AABC10(EnMd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode >= OCARINA_MODE_04) {
        this->actionFunc = func_80AAB948;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->actor.textId = 0x1067;
        func_8002F2CC(&this->actor, play, this->collider.dim.radius + 30.0f);

        this->actionFunc = func_80AAB948;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else {
        player->stateFlags2 |= 0x800000;
    }
}

void func_80AABD0C(EnMd* this, PlayState* play) {
    func_80034F54(play, this->unk_214, this->unk_236, 17);
    func_80AAA93C(this);

    if (!(EnMd_FollowPath(this, play)) || (this->waypoint != 0)) {
        this->actor.shape.rot = this->actor.world.rot;
        return;
    }

    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && !(gSaveContext.eventChkInf[1] & 0x1000) &&
        (play->sceneNum == SCENE_SPOT04)) {
        Message_CloseTextbox(play);
        gSaveContext.eventChkInf[1] |= 0x1000;
        Actor_Kill(&this->actor);
        return;
    }

    func_80AAA92C(this, 11);

    this->skelAnime.playSpeed = 0.0f;
    this->actor.speedXZ = 0.0f;
    this->actor.home.pos = this->actor.world.pos;
    this->actionFunc = func_80AAB8F8;
}

void EnMd_Update(Actor* thisx, PlayState* play) {
    EnMd* this = (EnMd*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);
    EnMd_UpdateEyes(this);
    func_80AAB5A4(this, play);
    Actor_MoveForward(&this->actor);
    func_80AAB158(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    this->actionFunc(this, play);
}

s32 EnMd_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnMd* this = (EnMd*)thisx;
    Vec3s vec;

    if (limbIndex == 16) {
        Matrix_Translate(1200.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec = this->unk_1E0.unk_08;
        Matrix_RotateX((vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_Translate(-1200.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 9) {
        vec = this->unk_1E0.unk_0E;
        Matrix_RotateX((vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateY((vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
    }

    if (((limbIndex == 9) || (limbIndex == 10)) || (limbIndex == 13)) {
        rot->y += Math_SinS(this->unk_214[limbIndex]) * 200.0f;
        rot->z += Math_CosS(this->unk_236[limbIndex]) * 200.0f;
    }

    return false;
}

void EnMd_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnMd* this = (EnMd*)thisx;
    Vec3f vec = { 400.0f, 0.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_MultVec3f(&vec, &this->actor.focus.pos);
    }
}

void EnMd_Draw(Actor* thisx, PlayState* play) {
    static void* sEyeTextures[] = {
        gMidoEyeOpenTex,
        gMidoEyeHalfTex,
        gMidoEyeClosedTex,
    };
    EnMd* this = (EnMd*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->alpha == 255) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeIdx]));
        func_80034BA0(play, &this->skelAnime, EnMd_OverrideLimbDraw, EnMd_PostLimbDraw, &this->actor, this->alpha);
    } else if (this->alpha != 0) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeIdx]));
        func_80034CC4(play, &this->skelAnime, EnMd_OverrideLimbDraw, EnMd_PostLimbDraw, &this->actor, this->alpha);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

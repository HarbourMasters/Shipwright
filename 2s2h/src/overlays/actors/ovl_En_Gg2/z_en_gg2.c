/*
 * File: z_en_gg2.c
 * Overlay: ovl_En_Gg2
 * Description: Darmani's Ghost (2)
 */

#include "z_en_gg2.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_REACT_TO_LENS)

void EnGg2_Init(Actor* thisx, PlayState* play2);
void EnGg2_Destroy(Actor* thisx, PlayState* play);
void EnGg2_Update(Actor* thisx, PlayState* play);
void EnGg2_Draw(Actor* thisx, PlayState* play);

void func_80B3AE60(EnGg2* this, PlayState* play);
void func_80B3AFB0(EnGg2* this, PlayState* play);
void func_80B3B05C(EnGg2* this, PlayState* play);
void func_80B3B0A0(EnGg2* this, PlayState* play);
void func_80B3B120(EnGg2* this, PlayState* play);
void func_80B3B21C(EnGg2* this, PlayState* play);
void func_80B3B294(EnGg2* this, PlayState* play);
void func_80B3B5D4(EnGg2* this, PlayState* play);
s32 EnGg2_HasReachedPoint(EnGg2* this, Path* path, s32 pointIndex);
f32 func_80B3B7E4(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3);

ActorProfile En_Gg2_Profile = {
    /**/ ACTOR_EN_GG2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GG,
    /**/ sizeof(EnGg2),
    /**/ EnGg2_Init,
    /**/ EnGg2_Destroy,
    /**/ EnGg2_Update,
    /**/ EnGg2_Draw,
};

typedef enum {
    /*   -1 */ ENGG2_ANIM_NONE = -1,
    /* 0x00 */ ENGG2_ANIM_0,
    /* 0x01 */ ENGG2_ANIM_1,
    /* 0x02 */ ENGG2_ANIM_2,
    /* 0x03 */ ENGG2_ANIM_3,
    /* 0x04 */ ENGG2_ANIM_4,
    /* 0x05 */ ENGG2_ANIM_5,
    /* 0x06 */ ENGG2_ANIM_6,
    /* 0x07 */ ENGG2_ANIM_7,
    /* 0x08 */ ENGG2_ANIM_8,
    /* 0x09 */ ENGG2_ANIM_9,
    /* 0x0A */ ENGG2_ANIM_10,
    /* 0x0B */ ENGG2_ANIM_11,
    /* 0x0C */ ENGG2_ANIM_12,
    /* 0x0D */ ENGG2_ANIM_13,
    /* 0x0E */ ENGG2_ANIM_14,
    /* 0x0F */ ENGG2_ANIM_15,
    /* 0x10 */ ENGG2_ANIM_MAX
} EnGg2Animation;

static AnimationInfo sAnimationInfo[ENGG2_ANIM_MAX] = {
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG2_ANIM_0
    { &object_gg_Anim_00D528, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -10.0f }, // ENGG2_ANIM_1
    { &object_gg_Anim_00D174, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -10.0f }, // ENGG2_ANIM_2
    { &object_gg_Anim_00ECC0, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -10.0f }, // ENGG2_ANIM_3
    { &object_gg_Anim_00BAF0, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG2_ANIM_4
    { &object_gg_Anim_00AF40, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG2_ANIM_5
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG2_ANIM_6
    { &object_gg_Anim_00AF40, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG2_ANIM_7
    { &object_gg_Anim_00F578, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -10.0f }, // ENGG2_ANIM_8
    { &object_gg_Anim_0100C8, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG2_ANIM_9
    { &object_gg_Anim_00CDA0, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG2_ANIM_10
    { &object_gg_Anim_00B560, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG2_ANIM_11
    { &object_gg_Anim_00A4B4, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },   // ENGG2_ANIM_12
    { &object_gg_Anim_00E86C, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },   // ENGG2_ANIM_13
    { &object_gg_Anim_00D99C, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },   // ENGG2_ANIM_14
    { &object_gg_Anim_00E2A4, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // ENGG2_ANIM_15
};

Color_RGBA8 D_80B3C080 = { 255, 255, 255, 255 };
Color_RGBA8 D_80B3C084 = { 255, 150, 0, 255 };

Vec3f D_80B3C088 = { 0.0f, 0.0f, 0.0f };
Vec3f D_80B3C094 = { 0.0f, 0.0f, 0.0f };
Vec3f D_80B3C0A0 = { 1000.0f, 0.0f, 0.0f };

TexturePtr D_80B3C0AC[] = {
    object_gg_Tex_009260,
    object_gg_Tex_009660,
    object_gg_Tex_009A60,
    object_gg_Tex_009E60,
};

void func_80B3AC50(EnGg2* this) {
    this->actor.focus.pos.x = this->actor.world.pos.x;
    this->actor.focus.pos.y = this->actor.world.pos.y + 80.0f;
    this->actor.focus.pos.z = this->actor.world.pos.z;

    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
}

s32 func_80B3AC94(EnGg2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;
    s16 pitch;

    sp40 = player->actor.world.pos;
    sp40.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;

    sp34 = this->actor.world.pos;
    sp34.y += 70.0f;

    pitch = Math_Vec3f_Pitch(&sp34, &sp40);

    if ((this->actor.xzDistToPlayer < 250.0f) && (this->actor.xzDistToPlayer > 50.0f) &&
        CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        Math_SmoothStepToS(&this->unk_2F4, pitch, 4, 0x2AA8, 1);
    } else {
        Math_SmoothStepToS(&this->unk_2F4, 0, 4, 0x2AA8, 1);
    }

    this->unk_2F4 = CLAMP(this->unk_2F4, 0, 0x1C70);
    return true;
}

void func_80B3ADD8(EnGg2* this) {
    s16 temp_v0 = this->unk_2EC - 1;

    if (temp_v0 >= 3) {
        this->unk_2EA = 0;
        this->unk_2EC = temp_v0;
    } else if (temp_v0 == 0) {
        this->unk_2EA = 2;
        this->unk_2EC = (s32)(Rand_ZeroOne() * 60.0f) + 20;
    } else {
        this->unk_2EA = 1;
        this->unk_2EC = temp_v0;
    }
}

void func_80B3AE60(EnGg2* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == endFrame) {
        switch (this->animIndex) {
            case ENGG2_ANIM_0:
                this->animIndex = ENGG2_ANIM_1;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_1);
                this->actionFunc = func_80B3B0A0;
                break;

            case ENGG2_ANIM_1:
            case ENGG2_ANIM_8:
                this->animIndex = ENGG2_ANIM_5;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_5);
                this->actionFunc = func_80B3B120;
                break;

            case ENGG2_ANIM_5:
                this->animIndex = ENGG2_ANIM_6;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_0);
                this->actionFunc = func_80B3B21C;
                break;

            case ENGG2_ANIM_6:
                this->animIndex = ENGG2_ANIM_7;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_5);
                this->actionFunc = func_80B3B294;
                break;

            default:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_0);
                this->actionFunc = func_80B3AFB0;
                break;
        }
    }
}

void func_80B3AFB0(EnGg2* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_2F0 = 1;
        this->actionFunc = func_80B3AE60;
    } else if ((this->actor.xzDistToPlayer < 100.0f) && (this->actor.xzDistToPlayer > 50.0f) &&
               CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        this->actor.textId = 0xCE4;
    }
}

void func_80B3B05C(EnGg2* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 100.0f) && CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        this->actionFunc = func_80B3B5D4;
    }
}

void func_80B3B0A0(EnGg2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->unk_2F0 = 0;
        this->actionFunc = func_80B3B5D4;
    }
}

void func_80B3B120(EnGg2* this, PlayState* play) {
    Vec3s sp30;

    if (this->path != NULL) {
        func_80B3B7E4(this->path, this->unk_1DC, &this->actor.world.pos, &sp30);
        Math_SmoothStepToS(&this->actor.world.rot.y, sp30.y, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Math_SmoothStepToS(&this->actor.shape.rot.x, sp30.x, 4, 0x3E8, 1);
        this->actor.world.rot.x = -this->actor.shape.rot.x;
        if (EnGg2_HasReachedPoint(this, this->path, this->unk_1DC)) {
            if (this->unk_1DC >= (this->path->count - 2)) {
                this->actionFunc = func_80B3AE60;
                this->actor.speed = 0.0f;
            } else {
                this->unk_1DC++;
            }
        }
        Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 1.0f);
    }
}

void func_80B3B21C(EnGg2* this, PlayState* play) {
    this->actor.speed = 0.0f;
    if ((this->actor.xzDistToPlayer < 100.0f) && CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        this->actionFunc = func_80B3B5D4;
    }
}

void func_80B3B294(EnGg2* this, PlayState* play) {
    Vec3s sp30;

    if (this->unk_2F1 == 0) {
        if (play->sceneId == SCENE_11GORONNOSATO) {
            SET_WEEKEVENTREG(WEEKEVENTREG_20_04);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_10);
        } else if (play->sceneId == SCENE_17SETUGEN) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
            SET_WEEKEVENTREG(WEEKEVENTREG_20_08);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_10);
        } else if (play->sceneId == SCENE_10YUKIYAMANOMURA) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
            SET_WEEKEVENTREG(WEEKEVENTREG_20_10);
        }

        if (this->path != NULL) {
            func_80B3B7E4(this->path, this->unk_1DC, &this->actor.world.pos, &sp30);
            Math_SmoothStepToS(&this->actor.world.rot.y, sp30.y, 4, 0x3E8, 1);
            this->actor.shape.rot.y = this->actor.world.rot.y;
            Math_SmoothStepToS(&this->actor.shape.rot.x, sp30.x, 4, 0x3E8, 1);
            this->actor.world.rot.x = -this->actor.shape.rot.x;

            if (EnGg2_HasReachedPoint(this, this->path, this->unk_1DC)) {
                if (this->unk_1DC < (this->path->count - 1)) {
                    this->unk_1DC++;
                } else {
                    this->unk_2F1 = 1;
                    if (play->sceneId == SCENE_11GORONNOSATO) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_20_04);
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_10);
                    } else if (play->sceneId == SCENE_17SETUGEN) {
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
                        SET_WEEKEVENTREG(WEEKEVENTREG_20_08);
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_10);
                    } else if (play->sceneId == SCENE_10YUKIYAMANOMURA) {
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
                        SET_WEEKEVENTREG(WEEKEVENTREG_20_10);
                    }
                }
            }
        }
    }
    Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 1.0f);
}

void func_80B3B4B0(EnGg2* this, PlayState* play) {
    Vec3f sp64;
    Color_RGBA8 sp60 = D_80B3C080;
    Color_RGBA8 sp5C = D_80B3C084;
    s32 i;

    for (i = 0; i < 5; i++) {
        sp64.x = (Rand_Centered() * 50.0f) + this->unk_304.x;
        sp64.y = this->unk_304.y;
        sp64.z = (Rand_Centered() * 50.0f) + this->unk_304.z;
        EffectSsKirakira_SpawnDispersed(play, &sp64, &D_80B3C088, &D_80B3C094, &sp60, &sp5C, 3000, 40);
    }
}

void func_80B3B5D4(EnGg2* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
        this->actionFunc = func_80B3AE60;
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csId);
    }
}

s32 EnGg2_HasReachedPoint(EnGg2* this, Path* path, s32 pointIndex) {
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

f32 func_80B3B7E4(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3) {
    Vec3s* points;
    Vec3f sp20;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[arg1];
        sp20.x = points->x;
        sp20.y = points->y;
        sp20.z = points->z;
    }

    arg3->y = Math_Vec3f_Yaw(arg2, &sp20);
    arg3->x = Math_Vec3f_Pitch(arg2, &sp20);
    return sp20.y - arg2->y;
}

void func_80B3B8A4(EnGg2* this) {
    f32 sp1C;

    if ((this->animIndex != ENGG2_ANIM_5) && (this->animIndex != ENGG2_ANIM_7)) {
        this->unk_2F2 += 0x62C;
        sp1C = 1100.0f;
    } else {
        this->unk_2F2 += 0x7BC;
        sp1C = 950.0f;
    }

    this->actor.shape.yOffset = Math_SinS(this->unk_2F2) * sp1C;
    this->actor.shape.shadowScale = 30.0f - (Math_SinS(this->unk_2F2) * 5.0f);
    this->unk_2F6 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
}

void EnGg2_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGg2* this = (EnGg2*)thisx;

    if (GameInteractor_Should(VB_CONSIDER_DARMANI_HEALED, INV_CONTENT(ITEM_MASK_GORON) == ITEM_MASK_GORON)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_91_10)) {
        Actor_Kill(&this->actor);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_400;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_gg_Skel_00F6C0, &object_gg_Anim_00F578, this->jointTable,
                       this->morphTable, OBJECT_GG_LIMB_MAX);
    this->path = SubS_GetPathByIndex(play, ENGG2_GET_PATH_INDEX(&this->actor), ENGG2_PATH_INDEX_NONE);
    this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    this->unk_2F0 = 0;
    this->unk_2F1 = 0;
    this->unk_2F2 = 0;
    this->unk_2F4 = 0;
    this->unk_2F6 = 0;
    this->csId = this->actor.csId;
    this->unk_2EC = 20;
    this->unk_2EA = 0;

    if (play->sceneId == SCENE_11GORONNOSATO) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_10);
        this->animIndex = ENGG2_ANIM_0;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_0);
        this->actionFunc = func_80B3AFB0;
    } else if (play->sceneId == SCENE_17SETUGEN) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_20_04) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_20_08) &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_20_10)) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
            this->animIndex = ENGG2_ANIM_8;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_0);
            this->actionFunc = func_80B3B05C;
        } else {
            Actor_Kill(&this->actor);
        }
    } else if (play->sceneId == SCENE_10YUKIYAMANOMURA) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_20_04) && CHECK_WEEKEVENTREG(WEEKEVENTREG_20_08) &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_20_10)) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
            this->animIndex = ENGG2_ANIM_8;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGG2_ANIM_0);
            this->actionFunc = func_80B3B05C;
        } else {
            Actor_Kill(&this->actor);
        }
    } else {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_04);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_08);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_20_10);
        Actor_Kill(&this->actor);
    }
}

void EnGg2_Destroy(Actor* thisx, PlayState* play) {
}

void EnGg2_Update(Actor* thisx, PlayState* play) {
    EnGg2* this = (EnGg2*)thisx;

    if (play->actorCtx.lensMaskSize == LENS_MASK_ACTIVE_SIZE) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if ((this->animIndex == ENGG2_ANIM_5) && (this->animIndex == ENGG2_ANIM_7)) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }

    this->actionFunc(this, play);

    func_80B3AC50(this);
    func_80B3AC94(this, play);
    Actor_MoveWithoutGravity(&this->actor);
    SkelAnime_Update(&this->skelAnime);
    func_80B3B8A4(this);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);

    if ((this->animIndex == ENGG2_ANIM_5) || (this->animIndex == ENGG2_ANIM_7)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_SHARP_FLOAT - SFX_FLAG);
        if ((play->actorCtx.lensMaskSize == LENS_MASK_ACTIVE_SIZE) && ((play->state.frames % 4) == 0)) {
            func_80B3B4B0(this, play);
        }
    }

    func_80B3ADD8(this);
}

s32 EnGg2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                           Gfx** gfx) {
    EnGg2* this = (EnGg2*)thisx;

    if ((this->animIndex != ENGG2_ANIM_5) && (this->animIndex != ENGG2_ANIM_7)) {
        if (limbIndex == OBJECT_GG_LIMB_01) {
            Matrix_RotateYS(this->unk_2F6, MTXMODE_APPLY);
        }

        if (limbIndex == OBJECT_GG_LIMB_02) {
            Matrix_RotateZS(this->unk_2F4, MTXMODE_APPLY);
        }
    }
    return false;
}

void EnGg2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnGg2* this = (EnGg2*)thisx;

    if (limbIndex == OBJECT_GG_LIMB_04) {
        Matrix_MultVec3f(&D_80B3C0A0, &this->unk_304);
    }
}

void EnGg2_Draw(Actor* thisx, PlayState* play) {
    EnGg2* this = (EnGg2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS) || (this->unk_2F0 == 1)) {
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B3C0AC[this->unk_2EA]));

        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnGg2_OverrideLimbDraw, EnGg2_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

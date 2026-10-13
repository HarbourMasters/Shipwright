/*
 * File: z_en_daiku2.c
 * Overlay: ovl_En_Daiku2
 * Description: Milk Road Carpenter
 */

#include "z_en_daiku2.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/object_daiku/object_daiku.h"
#include "objects/object_bombiwa/object_bombiwa.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnDaiku2_Init(Actor* thisx, PlayState* play);
void EnDaiku2_Destroy(Actor* thisx, PlayState* play);
void EnDaiku2_Update(Actor* thisx, PlayState* play);
void EnDaiku2_Draw(Actor* thisx, PlayState* play);

void func_80BE65B4(EnDaiku2* this, PlayState* play);
void func_80BE66E4(EnDaiku2* this, PlayState* play);
void func_80BE6B40(EnDaiku2* this, PlayState* play);
void func_80BE6BC0(EnDaiku2* this, PlayState* play);
void func_80BE6CFC(EnDaiku2* this);
void func_80BE6D40(EnDaiku2* this, PlayState* play);
void func_80BE6EB0(EnDaiku2* this);
void func_80BE6EF0(EnDaiku2* this, PlayState* play);
void func_80BE7600(EnDaiku2* this, PlayState* play);
void func_80BE71A0(EnDaiku2* this);
void func_80BE71D8(EnDaiku2* this, PlayState* play);
void func_80BE7504(EnDaiku2* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4, s16 arg5);
void func_80BE7718(EnDaiku2* this, PlayState* play);

ActorProfile En_Daiku2_Profile = {
    /**/ ACTOR_EN_DAIKU2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DAIKU,
    /**/ sizeof(EnDaiku2),
    /**/ EnDaiku2_Init,
    /**/ EnDaiku2_Destroy,
    /**/ EnDaiku2_Update,
    /**/ EnDaiku2_Draw,
};

static u16 sTextIds[] = {
    0x32CB, 0x32CC, 0x32CD, 0x32CE, 0x32CF, 0x32D0, 0x32D1, 0x32D2,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

void func_80BE61D0(EnDaiku2* this) {
    if ((this->pathIndex != PATH_INDEX_NONE) && (this->path != NULL)) {
        if (!SubS_CopyPointFromPath(this->path, this->unk_25C, &this->unk_268)) {
            Actor_Kill(&this->actor);
        }
    }
}

void EnDaiku2_Init(Actor* thisx, PlayState* play) {
    EnDaiku2* this = (EnDaiku2*)thisx;
    s32 day = gSaveContext.save.day;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_daiku_Skel_00A850, &object_daiku_Anim_002FA0, this->jointTable,
                       this->morphTable, OBJECT_DAIKU_LIMB_MAX);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->switchFlag = ENDAIKU2_GET_SWITCH_FLAG(&this->actor);
    this->pathIndex = ENDAIKU2_GET_PATH_INDEX(&this->actor);
    this->path = SubS_GetPathByIndex(play, this->pathIndex, ENDAIKU2_PATH_INDEX_NONE);
    this->unk_280 = ENDAIKU2_GET_8000(&this->actor);
    Actor_SetScale(&this->actor, 0.01f);
    if (!this->unk_280) {
        if (day == 3) {
            Actor_Kill(&this->actor);
            return;
        }

        if (this->switchFlag == ENDAIKU2_SWITCH_FLAG_NONE) {
            this->switchFlag = SWITCH_FLAG_NONE;
        } else if (Flags_GetSwitch(play, this->switchFlag)) {
            this->unk_25C = this->path->count - 1;
            func_80BE61D0(this);
            Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_268);
        }
    } else if (day != 3) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.gravity = -3.0f;
    Math_Vec3f_Copy(&this->unk_268, &this->actor.world.pos);
    func_80BE65B4(this, play);
}

void EnDaiku2_Destroy(Actor* thisx, PlayState* play) {
    EnDaiku2* this = (EnDaiku2*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

typedef enum {
    /* -1 */ ENDAIKU2_ANIM_NONE = -1,
    /*  0 */ ENDAIKU2_ANIM_0,
    /*  1 */ ENDAIKU2_ANIM_1,
    /*  2 */ ENDAIKU2_ANIM_2,
    /*  3 */ ENDAIKU2_ANIM_3,
    /*  4 */ ENDAIKU2_ANIM_4,
    /*  5 */ ENDAIKU2_ANIM_5,
    /*  6 */ ENDAIKU2_ANIM_6,
    /*  7 */ ENDAIKU2_ANIM_7,
    /*  8 */ ENDAIKU2_ANIM_8,
    /*  9 */ ENDAIKU2_ANIM_9,
    /* 10 */ ENDAIKU2_ANIM_10,
    /* 11 */ ENDAIKU2_ANIM_MAX,
    /* 11 */ ENDAIKU2_ANIM_11 = ENDAIKU2_ANIM_MAX // for object_daiku_Anim_002134 set external to `EnDaiku2_ChangeAnim`
} EnDaiAnimation;

static AnimationHeader* sAnimations[ENDAIKU2_ANIM_MAX] = {
    &object_daiku_Anim_002FA0, // ENDAIKU2_ANIM_0
    &object_daiku_Anim_00ACD0, // ENDAIKU2_ANIM_1
    &object_daiku_Anim_00C92C, // ENDAIKU2_ANIM_2
    &object_daiku_Anim_000C44, // ENDAIKU2_ANIM_3
    &object_daiku_Anim_00C234, // ENDAIKU2_ANIM_4
    &object_daiku_Anim_000600, // ENDAIKU2_ANIM_5
    &object_daiku_Anim_001114, // ENDAIKU2_ANIM_6
    &object_daiku_Anim_00B690, // ENDAIKU2_ANIM_7
    &object_daiku_Anim_001A24, // ENDAIKU2_ANIM_8
    &object_daiku_Anim_002134, // ENDAIKU2_ANIM_9
    &object_daiku_Anim_00D328, // ENDAIKU2_ANIM_10
};

static u8 sAnimationModes[ENDAIKU2_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_0
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_1
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_2
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_3
    ANIMMODE_ONCE, // ENDAIKU2_ANIM_4
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_5
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_6
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_7
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_8
    ANIMMODE_ONCE, // ENDAIKU2_ANIM_9
    ANIMMODE_LOOP, // ENDAIKU2_ANIM_10
};

void EnDaiku2_ChangeAnim(EnDaiku2* this, s32 animIndex) {
    f32 playSpeed = 1.0f;

    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);
    if (this->animIndex == ENDAIKU2_ANIM_3) {
        playSpeed = 2.0f;
    }
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -4.0f);
}

s32 func_80BE64C0(EnDaiku2* this, PlayState* play) {
    EnBom* bomb;
    Vec3f sp30;

    Math_Vec3f_Copy(&sp30, &this->actor.world.pos);
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    bomb = (EnBom*)Actor_FindNearby(play, &this->actor, -1, ACTORCAT_EXPLOSIVES, BREG(7) + 240.0f);
    Math_Vec3f_Copy(&this->actor.world.pos, &sp30);
    if ((this->switchFlag > SWITCH_FLAG_NONE) && !Flags_GetSwitch(play, this->switchFlag) && (bomb != NULL) &&
        (bomb->actor.id == ACTOR_EN_BOM)) {
        if (!bomb->isPowderKeg) {
            this->actor.textId = 0x32D3;
        } else {
            this->actor.textId = 0x32D4;
        }

        return true;
    }

    return false;
}

void func_80BE65B4(EnDaiku2* this, PlayState* play) {
    switch (gSaveContext.save.day - 1) {
        case 0:
            this->unk_28A = 0;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_64_02)) {
                this->unk_28A = 6;
            }
            EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_8);
            break;

        case 1:
            this->unk_28A = 2;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_64_04)) {
                this->unk_28A = 7;
            }
            EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_8);
            break;

        case 2:
            this->unk_28A = 4;
            this->unk_264 = 1.0f;
            EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_10);
            break;

        default:
            break;
    }

    this->unk_264 = 1.0f;
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        this->unk_28A = 5;
        if (this->animIndex != ENDAIKU2_ANIM_10) {
            EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_10);
        }
    }

    this->unk_288 = 0;
    this->actionFunc = func_80BE66E4;
}

void func_80BE66E4(EnDaiku2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 sp98 = gSaveContext.save.day - 1;
    s32 i;
    Vec3f sp88;
    Vec3f sp7C;
    Vec3f sp70;
    s32 pad[2];
    s16 temp_v0;

    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.home.rot.y, 1, 0xBB8, 0);
    if (sp98 != 2) {
        if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
            this->unk_28A = 5;
            if (this->animIndex != ENDAIKU2_ANIM_10) {
                EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_10);
            }
        }
    }

    this->actor.textId = sTextIds[this->unk_28A];

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80BE6B40(this, play);
        return;
    }

    if ((this->unk_28A != 5) && (sp98 != 2) && func_80BE64C0(this, play)) {
        func_80BE6CFC(this);
        return;
    }

    Actor_OfferTalk(&this->actor, play, 80.0f);
    if ((this->animIndex == ENDAIKU2_ANIM_8) && Animation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ROCK_BROKEN);

        for (i = 0; i < 10; i++) {
            Math_Vec3f_Copy(&sp70, &this->actor.world.pos);

            sp70.x += Math_SinS(this->actor.world.rot.y) * 70.0f;
            sp70.y += 40.0f;
            sp70.z += Math_CosS(this->actor.world.rot.y) * 70.0f;

            sp70.x += Rand_CenteredFloat(5.0f);
            sp70.y += Rand_CenteredFloat(5.0f);
            sp70.z += Rand_CenteredFloat(5.0f);

            sp88.y = -1.0f;

            sp7C.x = (Rand_ZeroOne() - 0.5f) * 4.0f;
            sp7C.y = (Rand_ZeroOne() * 10.0f) + 4.0f;
            sp7C.z = (Rand_ZeroOne() - 0.5f) * 4.0f;

            func_80BE7504(this, &sp70, &sp7C, &sp88, 0.01f - (Rand_ZeroFloat(1.0f) * 0.005f), 40);
        }
    }

    temp_v0 = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
    if (temp_v0 > 0x2890) {
        if (sp98 != 2) {
            if ((this->animIndex == ENDAIKU2_ANIM_5) || (this->animIndex == ENDAIKU2_ANIM_9)) {
                EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_10);
            } else if ((this->animIndex == ENDAIKU2_ANIM_10) && (curFrame >= this->animEndFrame)) {
                this->animEndFrame = Animation_GetLastFrame(&object_daiku_Anim_002134);
                Animation_Change(&this->skelAnime, &object_daiku_Anim_002134, -1.0f, this->animEndFrame, 0.0f,
                                 ANIMMODE_ONCE, -4.0f);
                this->animIndex = ENDAIKU2_ANIM_11;
            } else if ((this->animIndex == ENDAIKU2_ANIM_11) && (curFrame <= 0.0f)) {
                EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_8);
            }
        }
    } else if (sp98 != 2) {
        if ((this->animIndex != ENDAIKU2_ANIM_9) && (this->animIndex != ENDAIKU2_ANIM_10)) {
            EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_9);
        } else if ((this->animIndex == ENDAIKU2_ANIM_9) && (curFrame >= this->animEndFrame)) {
            EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_10);
        }
    }
}

void func_80BE6B40(EnDaiku2* this, PlayState* play) {
    s32 day = gSaveContext.save.day;

    this->unk_288 = 1;
    if ((day != 3) && Flags_GetSwitch(play, this->switchFlag)) {
        this->actionFunc = func_80BE6BC0;
    } else {
        EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_5);
        this->actionFunc = func_80BE6BC0;
    }
}

void func_80BE6BC0(EnDaiku2* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        s32 day = gSaveContext.save.day - 1;

        Message_CloseTextbox(play);

        if (this->unk_288 == 2) {
            this->actionFunc = func_80BE6D40;
        } else if (this->unk_288 == 3) {
            this->actionFunc = func_80BE6EF0;
        } else if ((this->unk_28A == 0) || (this->unk_28A == 2)) {
            this->unk_28A++;
            Message_ContinueTextbox(play, sTextIds[this->unk_28A]);
        } else {
            switch (day) {
                case 0:
                    SET_WEEKEVENTREG(WEEKEVENTREG_64_02);
                    break;

                case 1:
                    SET_WEEKEVENTREG(WEEKEVENTREG_64_04);
                    break;

                default:
                    break;
            }
            func_80BE65B4(this, play);
        }
    }
}

void func_80BE6CFC(EnDaiku2* this) {
    EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_3);
    this->unk_288 = 2;
    this->actionFunc = func_80BE6D40;
    this->unk_264 = 0.0f;
}

void func_80BE6D40(EnDaiku2* this, PlayState* play) {
    s32 pad[3];
    s16 sp3A = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_268);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BE6BC0;
        return;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, sp3A, 1, 0x7D0, 0xA);
    Math_ApproachF(&this->actor.world.pos.x, this->unk_268.x, 0.5f, fabsf(Math_SinS(this->actor.world.rot.y) * 6.0f));
    Math_ApproachF(&this->actor.world.pos.z, this->unk_268.z, 0.5f, fabsf(Math_CosS(this->actor.world.rot.y) * 6.0f));

    if ((sqrtf(SQ(this->actor.world.pos.x - this->unk_268.x) + SQ(this->actor.world.pos.z - this->unk_268.z)) < 4.0f) &&
        (this->path != NULL)) {
        this->unk_25C++;
        if (this->unk_25C >= this->path->count) {
            func_80BE6EB0(this);
            return;
        }
        func_80BE61D0(this);
    }
    Actor_OfferTalk(&this->actor, play, 80.0f);
}

void func_80BE6EB0(EnDaiku2* this) {
    EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_2);
    this->unk_274 = 0;
    this->unk_288 = 3;
    this->actionFunc = func_80BE6EF0;
}

void func_80BE6EF0(EnDaiku2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 pad[4];
    Vec3f sp40;
    s16 var;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BE6BC0;
        return;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.home.rot.y, 1, 0xBB8, 0);
    if (curFrame >= this->animEndFrame) {
        this->unk_274 = 1;
    }

    if (this->unk_274 != 0) {
        Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_268), 1, 0x7D0,
                           0xA);
        Math_ApproachF(&this->actor.world.pos.x, this->unk_268.x, 0.5f,
                       fabsf(Math_SinS(this->actor.world.rot.y) * 4.0f));
        Math_ApproachF(&this->actor.world.pos.z, this->unk_268.z, 0.5f,
                       fabsf(Math_CosS(this->actor.world.rot.y) * 4.0f));
        if ((sqrtf(SQ(this->actor.world.pos.x - this->unk_268.x) + SQ(this->actor.world.pos.z - this->unk_268.z)) <
             4.0f) &&
            (this->path != NULL)) {
            if (!func_80BE64C0(this, play)) {
                if (this->animIndex != ENDAIKU2_ANIM_3) {
                    EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_3);
                }

                if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
                    this->unk_28A = 5;
                    if (this->animIndex != ENDAIKU2_ANIM_10) {
                        EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_10);
                    }
                    func_80BE65B4(this, play);
                    return;
                }

                this->unk_25C--;
                if (this->unk_25C < 0) {
                    this->unk_25C = 0;
                    func_80BE71A0(this);
                    return;
                }
                func_80BE61D0(this);
            }
        } else if (func_80BE64C0(this, play)) {
            Math_Vec3f_Copy(&sp40, &this->unk_268);
            var = this->unk_25C;
            this->unk_25C++;
            if (this->unk_25C < this->path->count) {
                func_80BE61D0(this);
                func_80BE6CFC(this);
                return;
            }
            this->unk_25C = var;
            Math_Vec3f_Copy(&this->unk_268, &sp40);
        }
    }
    Actor_OfferTalk(&this->actor, play, 80.0f);
}

void func_80BE71A0(EnDaiku2* this) {
    EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_9);
    this->actionFunc = func_80BE71D8;
}

void func_80BE71D8(EnDaiku2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (func_80BE64C0(this, play)) {
        func_80BE6CFC(this);
    } else if ((this->animIndex == ENDAIKU2_ANIM_9) && (curFrame >= this->animEndFrame)) {
        EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_0);
    } else if ((this->animIndex == ENDAIKU2_ANIM_0) && (curFrame >= this->animEndFrame)) {
        EnDaiku2_ChangeAnim(this, ENDAIKU2_ANIM_5);
    } else if ((this->animIndex == ENDAIKU2_ANIM_5) && (curFrame >= this->animEndFrame)) {
        func_80BE65B4(this, play);
    }
}

void EnDaiku2_Update(Actor* thisx, PlayState* play) {
    EnDaiku2* this = (EnDaiku2*)thisx;
    s32 pad;

    SkelAnime_Update(&this->skelAnime);
    this->actionFunc(this, play);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Actor_SetFocus(&this->actor, 65.0f);
    Actor_MoveWithGravity(&this->actor);
    Math_ApproachF(&this->unk_260, this->unk_264, 0.3f, 2.0f);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    func_80BE7600(this, play);
}

void EnDaiku2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDaiku2* this = (EnDaiku2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (limbIndex == OBJECT_DAIKU_LIMB_0E) {
        Matrix_Scale(this->unk_260, this->unk_260, this->unk_260, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_daiku_DL_009638);
    }

    if (limbIndex == OBJECT_DAIKU_LIMB_0F) {
        gSPDisplayList(POLY_OPA_DISP++, object_daiku_DL_00A390);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDaiku2_Draw(Actor* thisx, PlayState* play) {
    EnDaiku2* this = (EnDaiku2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 245, 155, 0, 255);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnDaiku2_PostLimbDraw, &this->actor);
    func_80BE7718(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BE7504(EnDaiku2* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4, s16 arg5) {
    s16 i;
    EnDaiku2Effect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isEnabled) {
            effect->isEnabled = true;
            effect->unk_04 = *arg1;
            effect->unk_10 = *arg2;
            effect->unk_1C = *arg3;
            effect->unk_30 = arg4;
            effect->unk_34 = arg5;
            effect->unk_28.x = Rand_CenteredFloat(30000.0f);
            effect->unk_28.y = Rand_CenteredFloat(30000.0f);
            effect->unk_28.z = Rand_CenteredFloat(30000.0f);
            break;
        }
    }
}

void func_80BE7600(EnDaiku2* this, PlayState* play) {
    s32 i;
    EnDaiku2Effect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            effect->unk_04.x += effect->unk_10.x;
            effect->unk_28.x += 0x100;
            effect->unk_28.z += 0x130;
            effect->unk_04.y += effect->unk_10.y;
            effect->unk_04.z += effect->unk_10.z;
            effect->unk_10.y += effect->unk_1C.y;
            if (effect->unk_34 != 0) {
                effect->unk_34 -= 1;
            } else {
                effect->isEnabled = false;
            }
        }
    }
}

void func_80BE7718(EnDaiku2* this, PlayState* play) {
    s32 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    EnDaiku2Effect* effect = this->effects;
    s32 objectSlot;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_BOMBIWA);
    if ((objectSlot > OBJECT_SLOT_NONE) && Object_IsLoaded(&play->objectCtx, objectSlot)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);

        for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
            if (effect->isEnabled) {
                Matrix_Push();
                Matrix_Translate(effect->unk_04.x, effect->unk_04.y, effect->unk_04.z, MTXMODE_NEW);
                Matrix_RotateXS(effect->unk_28.x, MTXMODE_APPLY);
                Matrix_RotateYS(effect->unk_28.y, MTXMODE_APPLY);
                Matrix_RotateZS(effect->unk_28.z, MTXMODE_APPLY);
                Matrix_Scale(effect->unk_30, effect->unk_30, effect->unk_30, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, object_bombiwa_DL_0009E0);

                Matrix_Pop();
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

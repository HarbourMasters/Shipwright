/*
 * File: z_en_daiku.c
 * Overlay: ovl_En_Daiku
 * Description: Carpenter
 */

#include "z_en_daiku.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnDaiku_Init(Actor* thisx, PlayState* play);
void EnDaiku_Destroy(Actor* thisx, PlayState* play);
void EnDaiku_Update(Actor* thisx, PlayState* play);
void EnDaiku_Draw(Actor* thisx, PlayState* play);

void func_80943820(EnDaiku* this);
void func_80943BC0(EnDaiku* this);
void func_80943BDC(EnDaiku* this, PlayState* play);
void func_809438F8(EnDaiku* this, PlayState* play);

ActorProfile En_Daiku_Profile = {
    /**/ ACTOR_EN_DAIKU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DAIKU,
    /**/ sizeof(EnDaiku),
    /**/ EnDaiku_Init,
    /**/ EnDaiku_Destroy,
    /**/ EnDaiku_Update,
    /**/ EnDaiku_Draw,
};

static u16 sTextIds[] = {
    0x061C, 0x061A, 0x061B, 0x061C, 0x061C, 0x061D, 0x061E, 0x061F, 0x061C, 0x0620, 0x0621, 0x0622,
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

typedef enum {
    /* -1 */ ENDAIKU_ANIM_NONE = -1,
    /*  0 */ ENDAIKU_ANIM_0,
    /*  1 */ ENDAIKU_ANIM_1,
    /*  2 */ ENDAIKU_ANIM_2,
    /*  3 */ ENDAIKU_ANIM_3,
    /*  4 */ ENDAIKU_ANIM_4,
    /*  5 */ ENDAIKU_ANIM_5,
    /*  6 */ ENDAIKU_ANIM_6,
    /*  7 */ ENDAIKU_ANIM_7,
    /*  8 */ ENDAIKU_ANIM_8,
    /*  9 */ ENDAIKU_ANIM_MAX
} EnDaiAnimation;

static AnimationHeader* sAnimations[ENDAIKU_ANIM_MAX] = {
    &object_daiku_Anim_002FA0, // ENDAIKU_ANIM_0
    &object_daiku_Anim_00ACD0, // ENDAIKU_ANIM_1
    &object_daiku_Anim_00C92C, // ENDAIKU_ANIM_2
    &object_daiku_Anim_000C44, // ENDAIKU_ANIM_3
    &object_daiku_Anim_00C234, // ENDAIKU_ANIM_4
    &object_daiku_Anim_000600, // ENDAIKU_ANIM_5
    &object_daiku_Anim_001114, // ENDAIKU_ANIM_6
    &object_daiku_Anim_00B690, // ENDAIKU_ANIM_7
    &object_daiku_Anim_00BEAC, // ENDAIKU_ANIM_8
};

static u8 sAnimationModes[ENDAIKU_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENDAIKU_ANIM_0
    ANIMMODE_LOOP, // ENDAIKU_ANIM_1
    ANIMMODE_LOOP, // ENDAIKU_ANIM_2
    ANIMMODE_LOOP, // ENDAIKU_ANIM_3
    ANIMMODE_ONCE, // ENDAIKU_ANIM_4
    ANIMMODE_LOOP, // ENDAIKU_ANIM_5
    ANIMMODE_LOOP, // ENDAIKU_ANIM_6
    ANIMMODE_ONCE, // ENDAIKU_ANIM_7
    ANIMMODE_ONCE, // ENDAIKU_ANIM_8
};

void EnDaiku_Init(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->unk_278 = ENDAIKU_GET_FF(&this->actor);
    if (this->unk_278 == ENDAIKU_PARAM_FF_3) {
        this->pathIndex = ENDAIKU_GET_PATH_INDEX(&this->actor);
        this->path = SubS_GetPathByIndex(play, this->pathIndex, ENDAIKU_PATH_INDEX_NONE);
    } else if (this->unk_278 == ENDAIKU_PARAM_FF_2) {
        this->unk_264 = -2000;
    }

    if (this->unk_278 == ENDAIKU_PARAM_FF_0) {
        this->collider.dim.radius = 30;
        this->collider.dim.height = 60;
        this->collider.dim.yShift = 0;
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) ||
            ((gSaveContext.save.day == 3) && gSaveContext.save.isNight)) {
            Actor_Kill(&this->actor);
        }
    } else if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
        Actor_Kill(&this->actor);
    }

    Math_Vec3f_Copy(&this->unk_26C, &this->actor.world.pos);
    this->unk_280 = this->actor.world.rot.y;
    this->actor.gravity = -3.0f;

    switch (this->unk_278) {
        case ENDAIKU_PARAM_FF_0:
            this->unk_27E = this->unk_278 * 4 + 4;

        case ENDAIKU_PARAM_FF_1:
            SkelAnime_InitFlex(play, &this->skelAnime, &object_daiku_Skel_00A850, &object_daiku_Anim_002FA0,
                               this->jointTable, this->morphTable, OBJECT_DAIKU_LIMB_MAX);
            break;

        case ENDAIKU_PARAM_FF_2:
            SkelAnime_InitFlex(play, &this->skelAnime, &object_daiku_Skel_00A850, &object_daiku_Anim_00B690,
                               this->jointTable, this->morphTable, OBJECT_DAIKU_LIMB_MAX);
            break;

        case ENDAIKU_PARAM_FF_3:
            SkelAnime_InitFlex(play, &this->skelAnime, &object_daiku_Skel_00A850, &object_daiku_Anim_001114,
                               this->jointTable, this->morphTable, OBJECT_DAIKU_LIMB_MAX);
            break;

        default:
            break;
    }

    func_80943820(this);
}

void EnDaiku_Destroy(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDaiku_ChangeAnim(EnDaiku* this, s32 animIndex) {
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[animIndex], -4.0f);
}

void func_809437C8(EnDaiku* this) {
    if ((this->pathIndex != PATH_INDEX_NONE) && (this->path != NULL)) {
        if (!SubS_CopyPointFromPath(this->path, this->unk_25C, &this->unk_26C)) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80943820(EnDaiku* this) {
    s32 day = gSaveContext.save.day - 1;

    switch (this->unk_278) {
        case 0:
        case 1:
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_0);
            break;

        case 2:
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_7);
            break;

        case 3:
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_6);
            break;

        default:
            break;
    }

    this->unk_28C = (day * 4) + this->unk_278;
    this->actor.textId = sTextIds[this->unk_28C];
    this->unk_280 = this->actor.shape.rot.y;
    this->unk_28A = 0;
    this->actionFunc = func_809438F8;
}

void func_809438F8(EnDaiku* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 pad;
    s32 day = gSaveContext.save.day - 1;
    s32 pad2;

    if (Player_GetMask(play) == PLAYER_MASK_KAFEIS_MASK) {
        if (this->unk_278 == ENDAIKU_PARAM_FF_1) {
            this->actor.textId = 0x2365;
        } else {
            this->actor.textId = 0x2366;
        }
    } else {
        this->unk_28C = (day * 4) + this->unk_278;
        this->actor.textId = sTextIds[this->unk_28C];
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80943BC0(this);
        return;
    }

    if ((this->unk_278 == ENDAIKU_PARAM_FF_2) && (curFrame >= this->animEndFrame)) {
        if (Rand_ZeroOne() < 0.5f) {
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_7);
        } else {
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_8);
        }
    }

    if (this->unk_278 == ENDAIKU_PARAM_FF_3) {
        f32 xzDist;
        f32 yawDiff;

        Math_ApproachF(&this->actor.world.pos.x, this->unk_26C.x, 0.5f,
                       fabsf(2.0f * Math_SinS(this->actor.world.rot.y)));
        Math_ApproachF(&this->actor.world.pos.z, this->unk_26C.z, 0.5f,
                       fabsf(2.0f * Math_CosS(this->actor.world.rot.y)));
        Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_282, 1, 0x7D0, 0xA);

        xzDist = sqrtf(SQ(this->actor.world.pos.x - this->unk_26C.x) + SQ(this->actor.world.pos.z - this->unk_26C.z));
        yawDiff = fabsf(this->actor.world.rot.y - this->unk_282);
        if ((xzDist < 4.0f) && (this->path != NULL) && (yawDiff < 10.0f)) {
            this->unk_25C++;
            if (this->unk_25C >= this->path->count) {
                this->unk_25C = 0;
            }

            func_809437C8(this);
            this->unk_282 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_26C);
        }
    }

    if (this->unk_278 != ENDAIKU_PARAM_FF_0) {
        s16 angle = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y));

        this->unk_280 = this->actor.yawTowardsPlayer;
        if ((this->unk_278 == ENDAIKU_PARAM_FF_1) || (this->unk_278 == ENDAIKU_PARAM_FF_2) || (angle <= 0x2890)) {
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }
    }
}

void func_80943BC0(EnDaiku* this) {
    this->unk_28A = 1;
    this->actionFunc = func_80943BDC;
}

void func_80943BDC(EnDaiku* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->unk_278 == ENDAIKU_PARAM_FF_2) && (curFrame >= this->animEndFrame)) {
        if (Rand_ZeroOne() < 0.5f) {
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_7);
        } else {
            EnDaiku_ChangeAnim(this, ENDAIKU_ANIM_8);
        }
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_80943820(this);
    }
}

void EnDaiku_Update(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;
    s32 pad;

    if (this->unk_27E == 0) {
        SkelAnime_Update(&this->skelAnime);
    }

    if ((this->unk_278 == ENDAIKU_PARAM_FF_0) && (gSaveContext.save.day == 3) && (gSaveContext.save.isNight)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc(this, play);

    if (this->unk_27C != 0) {
        this->unk_27C--;
    }

    if (this->unk_27E != 0) {
        this->unk_27E--;
    }

    Actor_SetScale(&this->actor, 0.01f);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Actor_SetFocus(&this->actor, 65.0f);
    Actor_MoveWithGravity(&this->actor);
    Math_SmoothStepToS(&this->unk_260, this->unk_266, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->unk_25E, this->unk_264, 1, 0xBB8, 0);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    this->actor.cullingVolumeDistance = 650.0f;
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnDaiku_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDaiku* this = (EnDaiku*)thisx;

    if (limbIndex == OBJECT_DAIKU_LIMB_0F) {
        rot->x += this->unk_260;
        rot->z += this->unk_25E;
    }

    return false;
}

void EnDaiku_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Gfx* D_809440D4[] = {
        object_daiku_DL_0070C0,
        object_daiku_DL_006FB0,
        object_daiku_DL_006E80,
        object_daiku_DL_006D70,
    };
    EnDaiku* this = (EnDaiku*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == OBJECT_DAIKU_LIMB_0F) {
        gSPDisplayList(POLY_OPA_DISP++, D_809440D4[this->unk_278]);
    }

    if ((this->unk_278 == ENDAIKU_PARAM_FF_3) && (limbIndex == OBJECT_DAIKU_LIMB_08)) {
        gSPDisplayList(POLY_OPA_DISP++, object_daiku_DL_008EC8);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDaiku_Draw(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    switch (this->unk_278) {
        case 0:
            gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
            break;

        case 1:
            gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
            break;

        case 2:
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
            break;

        case 3:
            gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
            break;

        case 4:
            gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 0, 255);
            break;
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDaiku_OverrideLimbDraw, EnDaiku_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

/*
 * File: z_en_baisen.c
 * Overlay: ovl_En_Baisen
 * Description: Viscen (leader of the guards in Clock Town)
 */

#include "z_en_baisen.h"
#include "objects/object_bai/object_bai.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnBaisen_Init(Actor* thisx, PlayState* play);
void EnBaisen_Destroy(Actor* thisx, PlayState* play);
void EnBaisen_Update(Actor* thisx, PlayState* play);
void EnBaisen_Draw(Actor* thisx, PlayState* play);

void func_80BE87B0(EnBaisen* this, PlayState* play);
void func_80BE87FC(EnBaisen* this);
void func_80BE887C(EnBaisen* this, PlayState* play);
void func_80BE895C(EnBaisen* this, PlayState* play);
void func_80BE8AAC(EnBaisen* this, PlayState* play);
void func_80BE89D8(EnBaisen* this, PlayState* play);

ActorProfile En_Baisen_Profile = {
    /**/ ACTOR_EN_BAISEN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BAI,
    /**/ sizeof(EnBaisen),
    /**/ EnBaisen_Init,
    /**/ EnBaisen_Destroy,
    /**/ EnBaisen_Update,
    /**/ EnBaisen_Draw,
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

static u16 sTextIds[] = { 0x2ABD, 0x2ABB, 0x2AD5, 0x2AD6, 0x2AD7, 0x2AD8, 0x2AC6 };

typedef enum {
    /* 0 */ ENBAISEN_ANIM_0,
    /* 1 */ ENBAISEN_ANIM_1,
    /* 2 */ ENBAISEN_ANIM_2,
    /* 3 */ ENBAISEN_ANIM_MAX
} EnBaisenAnimation;

static AnimationHeader* sAnimations[ENBAISEN_ANIM_MAX] = {
    &object_bai_Anim_0011C0, // ENBAISEN_ANIM_0
    &object_bai_Anim_0008B4, // ENBAISEN_ANIM_1
    &object_bai_Anim_008198, // ENBAISEN_ANIM_2
};

static u8 sAnimationModes[ENBAISEN_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENBAISEN_ANIM_0
    ANIMMODE_LOOP, // ENBAISEN_ANIM_1
    ANIMMODE_LOOP, // ENBAISEN_ANIM_2
};

void EnBaisen_Init(Actor* thisx, PlayState* play) {
    EnBaisen* this = (EnBaisen*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_bai_Skel_007908, &object_bai_Anim_0011C0, this->jointTable,
                       this->morphTable, OBJECT_BAI_LIMB_MAX);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->paramCopy = this->actor.params;
    if (this->actor.params == 0) {
        this->unk290 = true;
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) &&
            ((gSaveContext.save.day != 3) || !gSaveContext.save.isNight)) {
            Actor_Kill(&this->actor);
        }
    } else {
        this->collider.dim.radius = 30;
        this->collider.dim.height = 60;
        this->collider.dim.yShift = 0;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) ||
            ((gSaveContext.save.day == 3) && gSaveContext.save.isNight)) {
            Actor_Kill(&this->actor);
        }
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    if (this->paramCopy == 0) {
        this->actionFunc = func_80BE87B0;
    } else {
        func_80BE87FC(this);
    }
}

void EnBaisen_Destroy(Actor* thisx, PlayState* play) {
    EnBaisen* this = (EnBaisen*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnBaisen_ChangeAnim(EnBaisen* this, s32 animIndex) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -10.0f);
}

void func_80BE871C(EnBaisen* this) {
    s16 yawTemp = this->unk29E - this->actor.world.rot.y;
    s32 yaw = ABS_ALT(yawTemp);

    this->headRotXTarget = 0;
    if ((this->actor.xzDistToPlayer < 200.0f) && (yaw < 0x4E20)) {
        this->headRotXTarget = this->unk29E - this->actor.world.rot.y;
        if (this->headRotXTarget > 0x2710) {
            this->headRotXTarget = 0x2710;
        } else if (this->headRotXTarget < -0x2710) {
            this->headRotXTarget = -0x2710;
        }
    }
}

void func_80BE87B0(EnBaisen* this, PlayState* play) {
    Actor* actorIterator = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actorIterator != NULL) {
        if (actorIterator->id == ACTOR_EN_HEISHI) {
            this->heishiPointer = actorIterator;
        }
        actorIterator = actorIterator->next;
    }

    func_80BE87FC(this);
}

void func_80BE87FC(EnBaisen* this) {
    if (this->paramCopy == 0) {
        this->textIdIndex = 2;
        EnBaisen_ChangeAnim(this, ENBAISEN_ANIM_2);
        this->unk29E = this->actor.world.rot.y;
    } else {
        EnBaisen_ChangeAnim(this, ENBAISEN_ANIM_0);
    }

    this->actor.textId = sTextIds[this->textIdIndex];
    this->unk29C = 0;
    this->actionFunc = func_80BE887C;
}

void func_80BE887C(EnBaisen* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80BE895C(this, play);
    } else {
        if (this->paramCopy != 0) {
            this->textIdIndex = 0;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ATTENDED_MAYOR_MEETING)) {
                this->textIdIndex = 1;
            }
            if (Player_GetMask(play) == PLAYER_MASK_COUPLE) {
                this->textIdIndex = 6;
            }
            if (this->cutsceneState == 1) {
                func_80BE895C(this, play);
                return;
            }
        }
        this->actor.textId = sTextIds[this->textIdIndex];
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80BE895C(EnBaisen* this, PlayState* play) {
    if (this->targetActor != NULL) {
        this->unk290 = true;
        this->cutsceneState = 1;
        Actor_ChangeFocus(this->targetActor, play, this->targetActor);
    }
    this->unk29C = 1;
    if (this->paramCopy == 0) {
        this->targetActor = this->heishiPointer;
        this->actionFunc = func_80BE8AAC;
    } else {
        this->actionFunc = func_80BE89D8;
    }
}

void func_80BE89D8(EnBaisen* this, PlayState* play) {
    if (&this->actor == this->targetActor) {
        this->unk29E = this->actor.world.rot.y;
        if (this->animIndex == ENBAISEN_ANIM_0) {
            EnBaisen_ChangeAnim(this, ENBAISEN_ANIM_1);
        }
    } else {
        this->unk29E = Math_Vec3f_Yaw(&this->actor.world.pos, &this->targetActor->world.pos);
        if (this->animIndex != ENBAISEN_ANIM_0) {
            EnBaisen_ChangeAnim(this, ENBAISEN_ANIM_0);
        }
    }
    if ((play->msgCtx.currentTextId == 0x2AC6) || (play->msgCtx.currentTextId == 0x2AC7) ||
        (play->msgCtx.currentTextId == 0x2AC8)) {
        this->skelAnime.playSpeed = 0.0f;
        this->unk29E = this->actor.yawTowardsPlayer;
    }
    if (this->cutsceneState == 2) { // Note: This variable is also set by EnDt.
        func_80BE87FC(this);
    }
}

void func_80BE8AAC(EnBaisen* this, PlayState* play) {
    if ((this->textIdIndex % 2) != 0) {
        this->unk29E = this->actor.world.rot.y;
        if (this->animIndex == ENBAISEN_ANIM_0) {
            EnBaisen_ChangeAnim(this, ENBAISEN_ANIM_1);
        }
    } else {
        if (this->targetActor != NULL) {
            this->unk29E = Math_Vec3f_Yaw(&this->actor.world.pos, &this->targetActor->world.pos);
        }
        if (this->animIndex != ENBAISEN_ANIM_0) {
            EnBaisen_ChangeAnim(this, ENBAISEN_ANIM_0);
        }
    }
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->textIdIndex++;
        if (this->textIdIndex < 6) {
            Message_ContinueTextbox(play, sTextIds[this->textIdIndex]);
            if ((this->textIdIndex % 2) == 0) {
                this->targetActor = this->heishiPointer;
            } else {
                this->targetActor = &this->actor;
            }
            Actor_ChangeFocus(this->targetActor, play, this->targetActor);
        } else {
            func_80BE87FC(this);
        }
    }
}

void EnBaisen_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBaisen* this = (EnBaisen*)thisx;

    SkelAnime_Update(&this->skelAnime);
    if (this->unusedCounter != 0) {
        this->unusedCounter--;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if ((this->paramCopy != 0) && (gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Actor_SetScale(&this->actor, 0.01f);
    if (this->unk290) {
        func_80BE871C(this);
    }
    Actor_SetFocus(&this->actor, 60.0f);
    Math_SmoothStepToS(&this->headRotX, this->headRotXTarget, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->headRotY, this->headRotYTarget, 1, 0x3E8, 0);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnBaisen_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBaisen* this = (EnBaisen*)thisx;

    if (limbIndex == OBJECT_BAI_LIMB_09) {
        rot->x += this->headRotX;
        rot->y += this->headRotY;
        rot->z += this->headRotZ;
    }

    return false;
}

void EnBaisen_Draw(Actor* thisx, PlayState* play) {
    EnBaisen* this = (EnBaisen*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBaisen_OverrideLimbDraw, NULL, &this->actor);
}

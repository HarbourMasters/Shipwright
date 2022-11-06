/*
 * File: z_en_ani.c
 * Overlay: ovl_En_Ani
 * Description: Kakariko Roof Guy
 */

#include "z_en_ani.h"
#include "objects/object_ani/object_ani.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnAni_Init(Actor* thisx, PlayState* play);
void EnAni_Destroy(Actor* thisx, PlayState* play);
void EnAni_Update(Actor* thisx, PlayState* play);
void EnAni_Draw(Actor* thisx, PlayState* play);

s32 EnAni_SetText(EnAni* this, PlayState* play, u16 textId);
void func_809B04F0(EnAni* this, PlayState* play);
void func_809B0524(EnAni* this, PlayState* play);
void func_809B0558(EnAni* this, PlayState* play);
void func_809B05F0(EnAni* this, PlayState* play);
void func_809B064C(EnAni* this, PlayState* play);
void func_809B07F8(EnAni* this, PlayState* play);
void func_809B0988(EnAni* this, PlayState* play);
void func_809B0994(EnAni* this, PlayState* play);
void func_809B0A28(EnAni* this, PlayState* play);
void func_809B0A6C(EnAni* this, PlayState* play);

const ActorInit En_Ani_InitVars = {
    ACTOR_EN_ANI,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ANI,
    sizeof(EnAni),
    (ActorFunc)EnAni_Init,
    (ActorFunc)EnAni_Destroy,
    (ActorFunc)EnAni_Update,
    (ActorFunc)EnAni_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0 } },
};

void EnAni_SetupAction(EnAni* this, EnAniActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 850, ICHAIN_STOP),
};

void EnAni_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAni* this = (EnAni*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, -2800.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gRoofManSkel, &gRoofManIdleAnim, this->jointTable,
                       this->morphTable, 0x10);
    Animation_PlayOnce(&this->skelAnime, &gRoofManIdleAnim);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    if (!LINK_IS_ADULT) {
        EnAni_SetupAction(this, func_809B064C);
    } else {
        EnAni_SetupAction(this, func_809B07F8);
    }
    this->unk_2AA = 0;
    this->unk_2A8 = 0;
    this->actor.minVelocityY = -1.0f;
    this->actor.velocity.y = -1.0f;
}

void EnAni_Destroy(Actor* thisx, PlayState* play) {
    EnAni* this = (EnAni*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 EnAni_SetText(EnAni* this, PlayState* play, u16 textId) {
    this->actor.textId = textId;
    this->unk_2A8 |= 1;
    func_8002F2CC(&this->actor, play, 100.0f);
    return 0;
}

void func_809B04F0(EnAni* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnAni_SetupAction(this, func_809B064C);
    }
}

void func_809B0524(EnAni* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnAni_SetupAction(this, func_809B07F8);
    }
}

void func_809B0558(EnAni* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        if (!LINK_IS_ADULT) {
            EnAni_SetupAction(this, func_809B04F0);
        } else {
            EnAni_SetupAction(this, func_809B0524);
        }
        gSaveContext.itemGetInf[1] |= 0x20;
    } else {
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_HEART_PIECE, 10000.0f, 200.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_KAK_MAN_ON_ROOF, GI_HEART_PIECE);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 200.0f);
        }
    }
}

void func_809B05F0(EnAni* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnAni_SetupAction(this, func_809B0558);
    }

    if (!gSaveContext.n64ddFlag) {
        func_8002F434(&this->actor, play, GI_HEART_PIECE, 10000.0f, 200.0f);
    } else {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_KAK_MAN_ON_ROOF, GI_HEART_PIECE);
        GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 200.0f);
    }
}

void func_809B064C(EnAni* this, PlayState* play) {
    u16 textId;
    s16 yawDiff;
    u16 textId2;

    textId2 = Text_GetFaceReaction(play, 0xA);
    textId = textId2 & 0xFFFF;

    if (!textId) {}

    if (textId2 == 0) {
        textId = !IS_DAY ? 0x5051 : 0x5050;
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (this->actor.textId == 0x5056) {
            EnAni_SetupAction(this, func_809B04F0);
        } else if (this->actor.textId == 0x5055) {
            EnAni_SetupAction(this, func_809B05F0);
        } else {
            EnAni_SetupAction(this, func_809B04F0);
        }
    } else if (yawDiff >= -0x36AF && yawDiff < 0 && this->actor.xzDistToPlayer < 150.0f &&
               -80.0f < this->actor.yDistToPlayer) {
        if (gSaveContext.itemGetInf[1] & 0x20) {
            EnAni_SetText(this, play, 0x5056);
        } else {
            EnAni_SetText(this, play, 0x5055);
        }
    } else if (yawDiff >= -0x3E7 && yawDiff < 0x36B0 && this->actor.xzDistToPlayer < 350.0f) {
        EnAni_SetText(this, play, textId);
    }
}

void func_809B07F8(EnAni* this, PlayState* play) {
    s16 pad;
    s16 yawDiff;
    u16 textId;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (this->actor.textId == 0x5056) {
            EnAni_SetupAction(this, func_809B0524);
        } else if (this->actor.textId == 0x5055) {
            EnAni_SetupAction(this, func_809B05F0);
        } else {
            EnAni_SetupAction(this, func_809B0524);
        }
    } else if (yawDiff > -0x36B0 && yawDiff < 0 && this->actor.xzDistToPlayer < 150.0f &&
               -80.0f < this->actor.yDistToPlayer) {
        if (gSaveContext.itemGetInf[1] & 0x20) {
            EnAni_SetText(this, play, 0x5056);
        } else {
            EnAni_SetText(this, play, 0x5055);
        }
    } else if (yawDiff > -0x3E8 && yawDiff < 0x36B0 && this->actor.xzDistToPlayer < 350.0f) {
        if (!(gSaveContext.eventChkInf[2] & 0x8000)) {
            textId = 0x5052;
        } else {
            textId = (gSaveContext.itemGetInf[1] & 0x20) ? 0x5054 : 0x5053;
        }
        EnAni_SetText(this, play, textId);
    }
}

void func_809B0988(EnAni* this, PlayState* play) {
}

void func_809B0994(EnAni* this, PlayState* play) {
    if (play->csCtx.npcActions[0]->action == 4) {
        Animation_Change(&this->skelAnime, &gRoofManGettingUpAfterKnockbackAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gRoofManGettingUpAfterKnockbackAnim), ANIMMODE_ONCE, -4.0f);
        this->unk_2AA++;
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
    }
}

void func_809B0A28(EnAni* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->unk_2AA++;
    }
}

void func_809B0A6C(EnAni* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    if (play->csCtx.npcActions[0]->action == 2) {
        Animation_Change(&this->skelAnime, &gRoofManKnockbackAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gRoofManKnockbackAnim), ANIMMODE_ONCE, 0.0f);
        this->actor.shape.shadowDraw = NULL;
        this->unk_2AA++;
    }
}

void EnAni_Update(Actor* thisx, PlayState* play) {
    EnAni* this = (EnAni*)thisx;
    s32 pad[2];

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.npcActions[0] != NULL)) {
        switch (this->unk_2AA) {
            case 0:
                func_809B0A6C(this, play);
                break;
            case 1:
                func_809B0A28(this, play);
                break;
            case 2:
                func_809B0994(this, play);
                break;
            case 3:
                func_809B0A28(this, play);
                break;
            case 4:
                func_809B0988(this, play);
                break;
        }

        if (play->csCtx.frames == 100) {
            func_800788CC(NA_SE_IT_EARTHQUAKE);
        }
    } else {
        if (SkelAnime_Update(&this->skelAnime) != 0) {
            this->skelAnime.curFrame = 0.0f;
        }
        this->actionFunc(this, play);
    }

    if (this->unk_2A8 & 1) {
        func_80038290(play, &this->actor, &this->unk_29C, &this->unk_2A2, this->actor.focus.pos);
        this->unk_2A2.z = 0;
        this->unk_2A2.y = this->unk_2A2.z;
        this->unk_2A2.x = this->unk_2A2.z;
    } else {
        Math_SmoothStepToS(&this->unk_29C.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_29C.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.y, 0, 6, 6200, 100);
    }

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }
    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

s32 EnAni_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnAni* this = (EnAni*)thisx;

    if (limbIndex == 15) {
        rot->x += this->unk_29C.y;
        rot->z += this->unk_29C.x;
    }
    return false;
}

void EnAni_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f sMultVec = { 800.0f, 500.0f, 0.0f };
    EnAni* this = (EnAni*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&sMultVec, &this->actor.focus.pos);
    }
}

void EnAni_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = {
        gRoofManEyeOpenTex,
        gRoofManEyeHalfTex,
        gRoofManEyeClosedTex,
    };
    EnAni* this = (EnAni*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    func_800943C8(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnAni_OverrideLimbDraw, EnAni_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}

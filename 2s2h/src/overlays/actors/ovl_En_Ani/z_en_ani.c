/*
 * File: z_en_ani.c
 * Overlay: ovl_En_Ani
 * Description: Man in Tree in South Termina Field
 *    version in shop at night is EnOssan actor using object_ani
 */

#include "z_en_ani.h"
#include "z64quake.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

// clang-format off
#define ANI_STATE_STANDING  (0)
#define ANI_STATE_UNK       (1 << 0)
#define ANI_STATE_WRITHING (1 << 1)
#define ANI_STATE_CLIMBING  (1 << 2)
#define ANI_STATE_FALLING   (1 << 3)
// clang-format on

void EnAni_Init(Actor* thisx, PlayState* play);
void EnAni_Destroy(Actor* thisx, PlayState* play);
void EnAni_Update(Actor* thisx, PlayState* play);
void EnAni_Draw(Actor* thisx, PlayState* play);

void EnAni_DefaultBlink(EnAni* this);
void EnAni_WaitForEyeClose(EnAni* this);
void EnAni_WaitForEyeOpen(EnAni* this);

void EnAni_SetText(EnAni* this, PlayState* play, u16 textId);

void EnAni_HangInTree(EnAni* this, PlayState* play);
void EnAni_LoseBalance(EnAni* this, PlayState* play);
void EnAni_FallToGround(EnAni* this, PlayState* play);
void EnAni_LandOnFoot(EnAni* this, PlayState* play);
void EnAni_FallOverInPain(EnAni* this, PlayState* play);
void EnAni_IdleInPain(EnAni* this, PlayState* play);
void EnAni_Talk(EnAni* this, PlayState* play);
void EnAni_IdleStanding(EnAni* this, PlayState* play);

ActorProfile En_Ani_Profile = {
    /**/ ACTOR_EN_ANI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ANI,
    /**/ sizeof(EnAni),
    /**/ EnAni_Init,
    /**/ EnAni_Destroy,
    /**/ EnAni_Update,
    /**/ EnAni_Draw,
};

// two different colliders, but only one init for both
static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 850, ICHAIN_STOP),
};

void EnAni_DefaultBlink(EnAni* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeState = this->blinkTimer;
    if (this->eyeState >= 3) {
        this->eyeState = 0;
    }
}

void EnAni_WaitForEyeClose(EnAni* this) {
    if (this->blinkTimer > 0) {
        this->blinkTimer--;
    } else if (this->eyeState < 2) {
        this->eyeState++;
    } else {
        this->blinkFunc = EnAni_WaitForEyeOpen;
        this->blinkTimer = Rand_S16Offset(20, 20);
    }
}

void EnAni_WaitForEyeOpen(EnAni* this) {
    if (this->blinkTimer > 0) {
        this->blinkTimer--;
    } else if (this->eyeState > 0) {
        this->eyeState--;
    } else {
        this->blinkFunc = EnAni_WaitForEyeClose;
        this->blinkTimer = Rand_S16Offset(10, 10);
    }
}

void EnAni_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAni* this = (EnAni*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gAniSkel, &gAniStandingNormalAnim, this->jointTable, this->morphTable,
                       ANI_LIMB_MAX);
    Animation_PlayOnce(&this->skelAnime, &gAniStandingNormalAnim);
    Collider_InitAndSetCylinder(play, &this->collider1, &this->actor, &sCylinderInit);
    Collider_InitAndSetCylinder(play, &this->collider2, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider2);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->stateFlags = ANI_STATE_STANDING;
    this->unk2EE = 0;
    this->treeReachTimer = 0;
    this->blinkFunc = EnAni_DefaultBlink;

    if (ANI_GET_TYPE(thisx) == ANI_TYPE_TREE_HANGING) {
        Animation_Change(&this->skelAnime, &gAniTreeHangingAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gAniTreeHangingAnim), ANIMMODE_ONCE, 0.0f);
        this->actionFunc = EnAni_HangInTree;
        this->actor.velocity.y = 0.0f;
        this->actor.terminalVelocity = 0.0f;
        this->actor.gravity = 0.0f;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->stateFlags |= ANI_STATE_CLIMBING;
        CLEAR_EVENTINF(EVENTINF_14);
    } else { // ANI_TYPE_STANDING
        // ( unused code )
        // for some reason standing he has a large collider
        // possibly he was meant to be a blocking npc like bomber
        this->collider2.dim.radius = 60;
        this->actionFunc = EnAni_IdleStanding;
        this->actor.velocity.y = -25.0f;
        this->actor.terminalVelocity = -25.0f;
        this->actor.gravity = -5.0f;
    }
}

void EnAni_Destroy(Actor* thisx, PlayState* play) {
    EnAni* this = (EnAni*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
}

void EnAni_SetText(EnAni* this, PlayState* play, u16 textId) {
    s16 diffAngle = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    this->actor.textId = textId;
    if ((this->stateFlags & ANI_STATE_WRITHING) || ABS_ALT(diffAngle) <= 0x4300) {
        if (this->actor.xzDistToPlayer < 100.0f) {
            Actor_OfferTalk(&this->actor, play, 120.0f);
        }
    }
}

void EnAni_IdleStanding(EnAni* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnAni_Talk(EnAni* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) && play->msgCtx.currentTextId == 0x6DE) {
        this->actionFunc = EnAni_IdleInPain;
    }
}

void EnAni_IdleInPain(EnAni* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = EnAni_Talk;
    } else {
        // telling you not to take his rupees you knocked from the tree
        EnAni_SetText(this, play, 0x6DE);
    }
}

void EnAni_FallOverInPain(EnAni* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->blinkFunc = EnAni_WaitForEyeOpen;
        this->actionFunc = EnAni_IdleInPain;
        Animation_Change(&this->skelAnime, &gAniHoldingFootWrithingInPainAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gAniHoldingFootWrithingInPainAnim), ANIMMODE_LOOP, 0.0f);
    }
}

void EnAni_LandOnFoot(EnAni* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->actionFunc = EnAni_FallOverInPain;
        Animation_Change(&this->skelAnime, &gAniFallOverHoldingFootAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gAniFallOverHoldingFootAnim), ANIMMODE_ONCE, 0.0f);
    }
}

void EnAni_FallToGround(EnAni* this, PlayState* play) {
    s32 pad;
    s16 quakeIndex;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actionFunc = EnAni_LandOnFoot;
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.z = 0.0f;
        // the animation gets cut short, (first 16 frames only) only the landing part is seen
        Animation_Change(&this->skelAnime, &gAniLandingThenStandingUpAnim, 1.0f, 0.0f, 16.0f, ANIMMODE_ONCE, 0.0f);
        this->stateFlags |= ANI_STATE_WRITHING;

        quakeIndex = Quake_Request(play->cameraPtrs[CAM_ID_MAIN], QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, 27000);
        Quake_SetPerturbations(quakeIndex, 7, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 20);

        Actor_PlaySfx(&this->actor, NA_SE_IT_HAMMER_HIT);
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x2, 0x7D0, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnAni_LoseBalance(EnAni* this, PlayState* play) {
    s32 pad;

    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.terminalVelocity = -20.0f;
        this->actor.gravity = -5.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = -4.0f;
        // frame count : 0.0f, only first frame, rest is handled in next action func
        Animation_Change(&this->skelAnime, &gAniLandingThenStandingUpAnim, 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 5.0f);
        this->actionFunc = EnAni_FallToGround;
        SET_EVENTINF(EVENTINF_14);
    }
}

void EnAni_HangInTree(EnAni* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->treeReachTimer > 0) {
            this->treeReachTimer--;
            if (this->treeReachTimer > 0) {
                Animation_PlayOnce(&this->skelAnime, &gAniTreeHangingReachAnim);
            } else {
                Animation_PlayOnce(&this->skelAnime, &gAniTreeHangingAnim);
            }
        } else if (Rand_ZeroFloat(1.0f) < 0.4f) {
            Animation_PlayOnce(&this->skelAnime, &gAniTreeHangingReachAnim);
            this->treeReachTimer = 2;
        } else {
            Animation_PlayOnce(&this->skelAnime, &gAniTreeHangingAnim);
        }
    }

    // his bonk detection is based on his tilt, neat
    if (this->actor.home.rot.x != 0) {
        this->stateFlags |= ANI_STATE_FALLING;
        this->actionFunc = EnAni_LoseBalance;
        Animation_Change(&this->skelAnime, &gAniTreeHangLosingBalanceAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gAniTreeHangLosingBalanceAnim), ANIMMODE_ONCE, -5.0f);
    }
}

void EnAni_Update(Actor* thisx, PlayState* play) {
    EnAni* this = (EnAni*)thisx;
    f32 minVelocity;

    Collider_UpdateCylinder(&this->actor, &this->collider1);
    Collider_UpdateCylinder(&this->actor, &this->collider2);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
    if (!(this->stateFlags & ANI_STATE_UNK)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider2.base);
    }

    this->actor.velocity.y += this->actor.gravity;
    minVelocity = this->actor.terminalVelocity;
    if (this->actor.velocity.y < minVelocity) {
        this->actor.velocity.y = minVelocity;
    }

    Actor_UpdatePos(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    this->actionFunc(this, play);
    if ((this->actor.xzDistToPlayer < 100.0f) && !(this->stateFlags & ANI_STATE_CLIMBING)) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
        this->torsoRot.x = this->torsoRot.y = this->torsoRot.z = 0;
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 0x6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 0x6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 0x6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 0x6, 0x1838, 0x64);
    }

    this->blinkFunc(this);
    if (this->stateFlags & ANI_STATE_FALLING) {
        if (this->actor.csId == CS_ID_NONE) {
            this->stateFlags &= ~ANI_STATE_FALLING;
        } else if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
            this->actor.csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
            Camera_SetFocalActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->actor.csId)),
                                 &this->actor);
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }
}

s32 EnAni_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnAni* this = (EnAni*)thisx;

    if (limbIndex == ANI_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }

    return false;
}

void EnAni_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_809686A4 = { 800.0f, 500.0f, 0.0f };

    if (limbIndex == ANI_LIMB_HEAD) {
        Matrix_MultVec3f(&D_809686A4, &thisx->focus.pos);
    }
}

void EnAni_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gAniOpenEyeTex, gAniClosingEyeTex, gAniClosedEyeTex };
    s32 pad;
    EnAni* this = (EnAni*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(0.0f, 0.0f, -1000.0f, MTXMODE_APPLY);
    Gfx_SetupDL37_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeState]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnAni_OverrideLimbDraw, EnAni_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

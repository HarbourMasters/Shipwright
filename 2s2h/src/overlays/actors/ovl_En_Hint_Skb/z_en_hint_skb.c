/*
 * File: z_en_hint_skb.c
 * Overlay: ovl_En_Hint_Skb
 * Description: Stalchild that gives hints in Oceanside Spider House
 */

#include "z_en_hint_skb.h"
#include "overlays/actors/ovl_En_Part/z_en_part.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnHintSkb_Init(Actor* thisx, PlayState* play);
void EnHintSkb_Destroy(Actor* thisx, PlayState* play);
void EnHintSkb_Update(Actor* thisx, PlayState* play);
void EnHintSkb_Draw(Actor* thisx, PlayState* play);

void func_80C1FE0C(EnHintSkb* this);
void func_80C1FE20(EnHintSkb* this, PlayState* play);
void func_80C1FE80(EnHintSkb* this, PlayState* play);
void func_80C1FF30(EnHintSkb* this);
void func_80C1FF88(EnHintSkb* this, PlayState* play);
void func_80C2003C(EnHintSkb* this);
void func_80C200B8(EnHintSkb* this, PlayState* play);
void func_80C2016C(EnHintSkb* this, PlayState* play);
void func_80C20334(EnHintSkb* this, PlayState* play);
void func_80C20484(EnHintSkb* this);
void func_80C204F0(EnHintSkb* this, PlayState* play);
void func_80C20590(EnHintSkb* this, PlayState* play);
void func_80C2069C(EnHintSkb* this, PlayState* play);
void func_80C2077C(EnHintSkb* this, PlayState* play);
void func_80C208BC(EnHintSkb* this);
void func_80C208D0(EnHintSkb* this, PlayState* play);
void func_80C20B88(EnHintSkb* this, PlayState* play);
void func_80C20C24(EnHintSkb* this, PlayState* play);
s32 func_80C21414(EnHintSkb* this);
void func_80C21468(EnHintSkb* this, PlayState* play);
void func_80C215E4(PlayState* play, EnHintSkb* this, Vec3f* arg2);

ActorProfile En_Hint_Skb_Profile = {
    /**/ ACTOR_EN_HINT_SKB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SKB,
    /**/ sizeof(EnHintSkb),
    /**/ EnHintSkb_Init,
    /**/ EnHintSkb_Destroy,
    /**/ EnHintSkb_Update,
    /**/ EnHintSkb_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 15, { { 0, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 2, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0xD),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(3, 0xC),
    /* Normal arrow   */ DMG_ENTRY(3, 0xC),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0xF),
    /* Hookshot       */ DMG_ENTRY(1, 0x1),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(3, 0x2),
    /* Ice arrow      */ DMG_ENTRY(3, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0xD),
    /* Deku launch    */ DMG_ENTRY(2, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(0, 0xB),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(2, 0x0),
    /* Normal Roll    */ DMG_ENTRY(4, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

typedef enum StalchildHintAnimation {
    /*  0 */ STALCHILD_HINT_ANIM_WALK,
    /*  1 */ STALCHILD_HINT_ANIM_STAND_UP_MORPH_1,
    /*  2 */ STALCHILD_HINT_ANIM_ATTACK,
    /*  3 */ STALCHILD_HINT_ANIM_STAGGER_MORPH_4,
    /*  4 */ STALCHILD_HINT_ANIM_COLLAPSE,
    /*  5 */ STALCHILD_HINT_ANIM_SIT_LAUGH,
    /*  6 */ STALCHILD_HINT_ANIM_SIT_TAP_TOES,
    /*  7 */ STALCHILD_HINT_ANIM_SWING_ON_BRANCH,
    /*  8 */ STALCHILD_HINT_ANIM_STAND_UP_MORPH_8,
    /*  9 */ STALCHILD_HINT_ANIM_STAND_UP_MORPH_16,
    /* 10 */ STALCHILD_HINT_ANIM_STAGGER_MORPH_8,
    /* 11 */ STALCHILD_HINT_ANIM_SALUTE,
    /* 12 */ STALCHILD_HINT_ANIM_IDLE,
    /* 13 */ STALCHILD_HINT_ANIM_MAX
} StalchildHintAnimation;

static AnimationInfo sAnimationInfo[STALCHILD_HINT_ANIM_MAX] = {
    { &gStalchildWalkAnim, 0.96f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },         // STALCHILD_HINT_ANIM_WALK
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },       // STALCHILD_HINT_ANIM_STAND_UP_MORPH_1
    { &gStalchildAttackAnim, 0.6f, 0.0f, 0.0f, ANIMMODE_ONCE_INTERP, 4.0f },  // STALCHILD_HINT_ANIM_ATTACK
    { &gStalchildStaggerAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },       // STALCHILD_HINT_ANIM_STAGGER_MORPH_4
    { &gStalchildCollapseAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },      // STALCHILD_HINT_ANIM_COLLAPSE
    { &gStalchildSitLaughAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },      // STALCHILD_HINT_ANIM_SIT_LAUGH
    { &gStalchildSitTapToesAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },    // STALCHILD_HINT_ANIM_SIT_TAP_TOES
    { &gStalchildSwingOnBranchAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f }, // STALCHILD_HINT_ANIM_SWING_ON_BRANCH
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },       // STALCHILD_HINT_ANIM_STAND_UP_MORPH_8
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -16.0f },      // STALCHILD_HINT_ANIM_STAND_UP_MORPH_16
    { &gStalchildStaggerAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },       // STALCHILD_HINT_ANIM_STAGGER_MORPH_8
    { &gStalchildSaluteAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },        // STALCHILD_HINT_ANIM_SALUTE
    { &gStalchildIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },          // STALCHILD_HINT_ANIM_IDLE
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void EnHintSkb_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHintSkb* this = (EnHintSkb*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_Init(play, &this->skelAnime, &gStalchildSkel, &gStalchildSitLaughAnim, this->jointTable, this->morphTable,
                   STALCHILD_LIMB_MAX);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElement);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.hintId = TATL_HINT_ID_STALCHILD;
    this->unk_3E4 = 0;
    this->unk_3DE = 0;
    this->unk_3E0 = 0;
    this->unk_3DC = 0;
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    func_80C1FE0C(this);
}

void EnHintSkb_Destroy(Actor* thisx, PlayState* play) {
    EnHintSkb* this = (EnHintSkb*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void func_80C1FE0C(EnHintSkb* this) {
    this->actionFunc = func_80C1FE20;
}

void func_80C1FE20(EnHintSkb* this, PlayState* play) {
}

void func_80C1FE30(EnHintSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_WALK);
    this->actor.speed = 1.6f;
    this->actionFunc = func_80C1FE80;
}

void func_80C1FE80(EnHintSkb* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 8.0f) || Animation_OnFrame(&this->skelAnime, 15.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_WALK);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x2EE, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_IsFacingPlayer(&this->actor, 0x11C7) && (this->actor.xzDistToPlayer < 60.0f)) {
        func_80C1FF30(this);
    }
}

void func_80C1FF30(EnHintSkb* this) {
    this->collider.base.atFlags &= ~AT_BOUNCED;
    this->actor.speed = 0.0f;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_ATTACK);
    this->actionFunc = func_80C1FF88;
}

void func_80C1FF88(EnHintSkb* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
        func_80C2003C(this);
    }

    if (Animation_OnFrame(&this->skelAnime, 3.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (func_80C21414(this) && (Player_GetMask(play) != PLAYER_MASK_CAPTAIN)) {
            func_80C1FF30(this);
        } else {
            func_80C1FE30(this);
        }
    }
}

void func_80C2003C(EnHintSkb* this) {
    Animation_Change(&this->skelAnime, &gStalchildAttackAnim, -0.4f, this->skelAnime.curFrame - 1.0f, 0.0f,
                     ANIMMODE_ONCE_INTERP, 0.0f);
    this->collider.base.atFlags &= ~AT_BOUNCED;
    this->actionFunc = func_80C200B8;
}

void func_80C200B8(EnHintSkb* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (func_80C21414(this)) {
            func_80C1FF30(this);
        } else {
            func_80C1FE30(this);
        }
    }
}

void func_80C2011C(EnHintSkb* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = func_80C2016C;
}

void func_80C2016C(EnHintSkb* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
    }

    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->actor.colChkInfo.health == 0) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
            func_80C20484(this);
        } else if (this->skelAnime.animation == &gStalchildSitLaughAnim) {
            func_80C1FE0C(this);
        } else if (func_80C21414(this)) {
            func_80C1FF30(this);
        } else {
            func_80C1FE30(this);
        }
    }
}

void func_80C20274(EnHintSkb* this) {
    if (this->skelAnime.animation == &gStalchildSitLaughAnim) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_STAND_UP_MORPH_8);
        this->actor.gravity = -1.0f;
        this->actor.speed = 1.0f;
    } else {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_STAGGER_MORPH_4);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.speed = -4.0f;
        }
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
    this->actionFunc = func_80C20334;
}

void func_80C20334(EnHintSkb* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        s32 i;

        for (i = 0; i < 10; i++) {
            func_80C215E4(play, this, &this->actor.world.pos);
        }
        this->actor.speed = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        } else if (this->actor.speed != 0.0f) {
            this->actor.speed -= 0.05f;
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x9C4, 0);
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 16, 0x7D0, 0x64);

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.world.rot = this->actor.shape.rot;
        func_80C1FE30(this);
    }
}

void func_80C20484(EnHintSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_COLLAPSE);
    this->unk_3E8 |= 4;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -6.0f;
    }
    this->actionFunc = func_80C204F0;
}

void func_80C204F0(EnHintSkb* this, PlayState* play) {
    if (!(this->unk_3E8 & 8)) {
        return;
    }

    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x10);
    Actor_Kill(&this->actor);
}

void func_80C20540(EnHintSkb* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = func_80C20590;
}

void func_80C20590(EnHintSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer == 0) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffScale = 0.0f;
        this->drawDmgEffAlpha = 0.0f;
        if (this->actor.colChkInfo.health != 0) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_STAGGER_MORPH_4);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
            this->unk_3E8 |= 1;
            func_80C20274(this);
        } else {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
            func_80C20484(this);
        }
    } else if (this->drawDmgEffTimer == 1) {
        func_80C21468(this, play);
    }
}

void func_80C2066C(EnHintSkb* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }
    this->actionFunc = func_80C2069C;
}

void func_80C2069C(EnHintSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer == 0) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffScale = 0.0f;
        this->drawDmgEffAlpha = 0.0f;
        if (this->actor.colChkInfo.health != 0) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_STAGGER_MORPH_4);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
            this->unk_3E8 |= 1;
            func_80C20274(this);
        } else {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
            func_80C20484(this);
        }
    }
}

void func_80C2075C(EnHintSkb* this) {
    this->unk_3E0 = 0;
    this->actor.speed = 0.0f;
    this->actionFunc = func_80C2077C;
}

void func_80C2077C(EnHintSkb* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_3E0 = 1;
        if (this->unk_3DC == 0) {
            this->unk_3DC = 1;
            Message_StartTextbox(play, 0x1147, &this->actor);
            this->unk_3E6 = 0x1147;
            if (this->skelAnime.animation == &gStalchildSitLaughAnim) {
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                this->actor.speed = 2.4f;
                this->actor.gravity = -1.0f;
                this->actor.velocity.y = 3.0f;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_STAND_UP_MORPH_8);
            }
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_SALUTE);
            Message_StartTextbox(play, 0x1148, &this->actor);
            this->unk_3E6 = 0x1148;
        }
        func_80C208BC(this);
    } else if ((this->actor.xzDistToPlayer < 100.0f) && !(this->collider.base.acFlags & AC_HIT)) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void func_80C208BC(EnHintSkb* this) {
    this->actionFunc = func_80C208D0;
}

void func_80C208D0(EnHintSkb* this, PlayState* play) {
    this->unk_3DE = 0;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_FADING:
            if ((play->gameplayFrames % 2) != 0) {
                this->unk_3DE = 1;
            }
            break;

        case TEXT_STATE_CHOICE:
            func_80C20B88(this, play);
            break;

        case TEXT_STATE_EVENT:
            func_80C20C24(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                func_80C2075C(this);
            }
            break;

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        default:
            break;
    }

    if (this->unk_3E6 == 0x1147) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            s32 i;

            this->actor.speed = 0.0f;

            for (i = 0; i < 10; i++) {
                func_80C215E4(play, this, &this->actor.world.pos);
            }
        }

        if ((this->skelAnime.animation == &gStalchildStandUpAnim) &&
            Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            Message_StartTextbox(play, 0x1147, &this->actor);
            this->unk_3E6 = 0x1147;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_WALK);
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, 0x71C, 0xB6);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
}

void func_80C20A74(EnHintSkb* this, PlayState* play) {
    s32 params = ENHINTSKB_GET_FF(&this->actor);

    switch (params) {
        case 1:
            Message_StartTextbox(play, 0x1149, &this->actor);
            this->unk_3E6 = 0x1149;
            break;

        case 2:
            Message_StartTextbox(play, 0x114A, &this->actor);
            this->unk_3E6 = 0x114A;
            break;

        case 3:
            Message_StartTextbox(play, 0x114B, &this->actor);
            this->unk_3E6 = 0x114B;
            break;

        case 4:
            Message_StartTextbox(play, 0x114C, &this->actor);
            this->unk_3E6 = 0x114C;
            break;

        case 5:
            Message_StartTextbox(play, 0x114D, &this->actor);
            this->unk_3E6 = 0x114D;
            break;

        case 6:
            Message_StartTextbox(play, 0x114E, &this->actor);
            this->unk_3E6 = 0x114E;
            break;

        default:
            Message_StartTextbox(play, 0x1149, &this->actor);
            this->unk_3E6 = 0x1149;
            break;
    }
}

void func_80C20B88(EnHintSkb* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) {
            Audio_PlaySfx_MessageDecide();
            this->unk_3E8 |= 0x10;
            Message_StartTextbox(play, 0x1150, &this->actor);
            this->unk_3E6 = 0x1150;
        } else {
            Audio_PlaySfx_MessageCancel();
            Message_StartTextbox(play, 0x1152, &this->actor);
            this->unk_3E6 = 0x1152;
        }
    }
}

void func_80C20C24(EnHintSkb* this, PlayState* play) {
    if (!Message_ShouldAdvance(play)) {
        return;
    }

    switch (this->unk_3E6) {
        case 0x1147:
            Message_StartTextbox(play, 0x1148, &this->actor);
            this->unk_3E6 = 0x1148;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_SALUTE);
            break;

        case 0x1148:
            func_80C20A74(this, play);
            break;

        case 0x1149:
        case 0x114A:
        case 0x114B:
        case 0x114C:
        case 0x114D:
        case 0x114E:
            Message_StartTextbox(play, 0x114F, &this->actor);
            this->unk_3E6 = 0x114F;
            break;

        case 0x1152:
            Message_StartTextbox(play, 0x1151, &this->actor);
            this->unk_3E6 = 0x1151;
            break;

        case 0x1150:
            Message_StartTextbox(play, 0x1151, &this->actor);
            this->unk_3E6 = 0x1151;
            break;

        case 0x1151:
            Message_CloseTextbox(play);
            if (this->unk_3E8 & 0x10) {
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
                func_80C20484(this);
            } else {
                func_80C2075C(this);
            }
            break;

        default:
            break;
    }
}

void func_80C20D64(EnHintSkb* this, PlayState* play) {
    if ((this->actionFunc == func_80C1FE20) || (this->actionFunc == func_80C2077C) ||
        (this->actionFunc == func_80C1FE80)) {
        if (this->actionFunc != func_80C2077C) {
            if (Player_GetMask(play) == PLAYER_MASK_CAPTAIN) {
                this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
                this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                this->actor.hintId = TATL_HINT_ID_NONE;
                this->actor.textId = 0;
                if (this->actionFunc == func_80C1FE80) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_IDLE);
                }
                func_80C2075C(this);
            }
        } else if (Player_GetMask(play) != PLAYER_MASK_CAPTAIN) {
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
            this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            this->actor.hintId = TATL_HINT_ID_STALCHILD;
            this->actor.textId = 0;
            if (this->skelAnime.animation == &gStalchildSitLaughAnim) {
                func_80C1FE0C(this);
            } else {
                func_80C1FE30(this);
            }
        }
    }
}

void func_80C20E90(EnHintSkb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH)) &&
        (this->actor.depthInWater >= 40.0f)) {
        this->actor.colChkInfo.health = 0;
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
        func_80C20484(this);
        return;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actionFunc == func_80C20590) {
            switch (this->actor.colChkInfo.damageEffect) {
                case 1:
                case 3:
                case 4:
                case 12:
                case 13:
                    return;

                default:
                case 2:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                    if (this->drawDmgEffTimer > 1) {
                        func_80C21468(this, play);
                    }
                    this->drawDmgEffTimer = 0;
                    break;
            }
        }

        if (!Actor_ApplyDamage(&this->actor)) {
            if ((this->actor.colChkInfo.damageEffect != 3) && (this->actor.colChkInfo.damageEffect != 4)) {
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
                this->unk_3E8 |= 4;
                func_80C20484(this);
                return;
            }
        }

        switch (this->actor.colChkInfo.damageEffect) {
            case 11:
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                this->drawDmgEffTimer = 40;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.0f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                func_80C2011C(this);
                break;

            case 1:
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                func_80C2011C(this);
                break;

            case 2:
                this->drawDmgEffTimer = 80;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.0f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                this->unk_3E8 |= 1;
                func_80C20274(this);
                break;

            case 3:
                if (this->actor.colChkInfo.health != 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                    this->drawDmgEffTimer = 80;
                } else {
                    this->drawDmgEffTimer = 3;
                }
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.5f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                func_80C20540(this);
                break;

            case 4:
                this->drawDmgEffTimer = 40;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.5f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_STAGGER_MORPH_4);
                func_80C2066C(this);
                break;

            case 12:
            case 14:
                this->unk_3E8 |= 1;
                // fallthrough
            case 15:
                if ((player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) ||
                    (player->meleeWeaponAnimation == PLAYER_MWA_LEFT_COMBO_2H) ||
                    (player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_RIGHT) ||
                    (player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_LEFT)) {
                    this->unk_3E8 |= 1;
                }
                // fallthrough
            case 13:
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_HINT_ANIM_STAGGER_MORPH_4);
                func_80C20274(this);
                break;

            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            default:
                break;
        }
    }
}

void func_80C21250(EnHintSkb* this, PlayState* play) {
    if (this->unk_3E0 != 1) {
        func_80C20E90(this, play);
        if (this->actionFunc == func_80C1FF88) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if ((this->actionFunc != func_80C200B8) && (this->actionFunc != func_80C20334) &&
            (this->actionFunc != func_80C204F0)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_80C21320(EnHintSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer > 0) {
        this->drawDmgEffTimer--;
    }

    if (this->actionFunc != func_80C20590) {
        if (this->drawDmgEffTimer < 20) {
            if (this->actionFunc == func_80C2069C) {
                Math_SmoothStepToF(&this->drawDmgEffScale, 0.0f, 0.5f, 0.03f, 0.0f);
            } else {
                Math_SmoothStepToF(&this->drawDmgEffScale, 0.0f, 0.5f, 0.01f, 0.0f);
            }
            this->drawDmgEffAlpha = this->drawDmgEffTimer * 0.05f;
        } else {
            Math_SmoothStepToF(&this->drawDmgEffScale, 0.5f, 0.1f, 0.02f, 0.0f);
        }
    }
}

s32 func_80C21414(EnHintSkb* this) {
    if (Actor_IsFacingPlayer(&this->actor, 0x11C7) && (this->actor.xzDistToPlayer < 60.0f)) {
        return true;
    }
    return false;
}

void func_80C21468(EnHintSkb* this, PlayState* play) {
    static Color_RGBA8 D_80C21E44 = { 170, 255, 255, 255 };
    static Color_RGBA8 D_80C21E48 = { 200, 200, 255, 255 };
    static Vec3f D_80C21E4C = { 0.0f, -1.0f, 0.0f };
    Vec3f sp84;
    s32 bodyPartsCount;
    s16 temp_s1;
    s32 i;

    if (this->unk_3E8 & 2) {
        bodyPartsCount = ENHINTSKB_BODYPART_MAX - 1;
    } else {
        bodyPartsCount = ENHINTSKB_BODYPART_MAX;
    }

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < bodyPartsCount; i++) {
        temp_s1 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->bodyPartsPos[i]);

        sp84.x = Math_SinS(temp_s1) * 3.0f;
        sp84.z = Math_CosS(temp_s1) * 3.0f;
        sp84.y = (Rand_ZeroOne() * 4.0f) + 4.0f;

        EffectSsEnIce_Spawn(play, &this->bodyPartsPos[i], 0.6f, &sp84, &D_80C21E4C, &D_80C21E44, &D_80C21E48, 30);
    }
}

void func_80C215E4(PlayState* play, EnHintSkb* this, Vec3f* arg2) {
    Vec3f sp5C;
    Vec3f sp50 = { 0.0f, 8.0f, 0.0f };
    Vec3f sp44 = { 0.0f, -1.5f, 0.0f };
    f32 sp40 = (Rand_ZeroOne() - 0.5f) * 6.28f;
    s32 pad;

    sp5C.y = this->actor.floorHeight;
    sp5C.x = (Math_SinF(sp40) * 15.0f) + arg2->x;
    sp5C.z = (Math_CosF(sp40) * 15.0f) + arg2->z;

    sp44.x = Rand_CenteredFloat(1.0f);
    sp44.z = Rand_CenteredFloat(1.0f);

    sp50.y += (Rand_ZeroOne() - 0.5f) * 4.0f;

    EffectSsHahen_Spawn(play, &sp5C, &sp50, &sp44, 0, ((Rand_ZeroOne() * 5.0f) + 12.0f) * 0.8f, HAHEN_OBJECT_DEFAULT,
                        10, NULL);
    func_800BBFB0(play, &sp5C, 10.0f, 1, 150, 0, 1);
}

void EnHintSkb_Update(Actor* thisx, PlayState* play) {
    EnHintSkb* this = (EnHintSkb*)thisx;

    this->actionFunc(this, play);

    if ((this->actionFunc != func_80C2016C) && (this->actionFunc != func_80C20590) &&
        (this->actionFunc != func_80C2069C)) {
        SkelAnime_Update(&this->skelAnime);
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    func_80C21250(this, play);
    func_80C20D64(this, play);
    func_80C21320(this, play);
}

s32 EnHintSkb_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnHintSkb* this = (EnHintSkb*)thisx;
    f32 temp_f10;

    if (limbIndex == STALCHILD_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        temp_f10 = fabsf(Math_SinS(play->state.frames * 6000) * 95.0f) + 160.0f;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, TRUNCF_BINANG(temp_f10), TRUNCF_BINANG(temp_f10), TRUNCF_BINANG(temp_f10), 255);

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (limbIndex == STALCHILD_LIMB_RIBCAGE) {
        Matrix_MultZero(&this->actor.focus.pos);
    } else if ((limbIndex == STALCHILD_LIMB_LOWER_JAW) && (this->unk_3DE == 1)) {
        Matrix_RotateZS(0x71C, MTXMODE_APPLY);
    }

    if (((limbIndex == STALCHILD_LIMB_HEAD) || (limbIndex == STALCHILD_LIMB_LOWER_JAW)) && (this->unk_3E8 & 2)) {
        *dList = NULL;
    }

    return false;
}

void EnHintSkb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80C21E70 = { 800.0f, 1200.0f, 0.0f };
    EnHintSkb* this = (EnHintSkb*)thisx;

    if (!(this->unk_3E8 & 8)) {
        Collider_UpdateSpheres(limbIndex, &this->collider);

        if ((limbIndex == STALCHILD_LIMB_HEAD) && (this->unk_3E8 & 1) && !(this->unk_3E8 & 2)) {
            Actor_SpawnBodyParts(&this->actor, play, ENPART_PARAMS(ENPART_TYPE_1), dList);
            this->unk_3E8 |= 2;
        } else if ((this->unk_3E8 & 4) && !(this->unk_3E8 & 8) &&
                   ((limbIndex != STALCHILD_LIMB_HEAD) || !(this->unk_3E8 & 1)) &&
                   (limbIndex != STALCHILD_LIMB_LOWER_JAW)) {
            Actor_SpawnBodyParts(&this->actor, play, ENPART_PARAMS(ENPART_TYPE_1), dList);
        }

        if (this->drawDmgEffTimer != 0) {
            if ((limbIndex == STALCHILD_LIMB_PELVIS) || (limbIndex == STALCHILD_LIMB_RIGHT_THIGH) ||
                (limbIndex == STALCHILD_LIMB_RIGHT_SHIN) || (limbIndex == STALCHILD_LIMB_RIGHT_FOOT) ||
                (limbIndex == STALCHILD_LIMB_LEFT_THIGH) || (limbIndex == STALCHILD_LIMB_LEFT_SHIN) ||
                (limbIndex == STALCHILD_LIMB_LEFT_FOOT) || (limbIndex == STALCHILD_LIMB_RIGHT_UPPER_ARM) ||
                (limbIndex == STALCHILD_LIMB_RIGHT_LOWER_ARM) || (limbIndex == STALCHILD_LIMB_RIGHT_HAND) ||
                (limbIndex == STALCHILD_LIMB_LEFT_UPPER_ARM) || (limbIndex == STALCHILD_LIMB_LEFT_LOWER_ARM) ||
                (limbIndex == STALCHILD_LIMB_LEFT_HAND)) {
                Matrix_MultZero(&this->bodyPartsPos[this->bodyPartsCount]);
                this->bodyPartsCount++;
            } else if ((limbIndex == STALCHILD_LIMB_HEAD) && !(this->unk_3E8 & 2)) {
                Matrix_MultVec3f(&D_80C21E70, &this->bodyPartsPos[this->bodyPartsCount]);
                this->bodyPartsCount++;
            }
        }
    }
}

void EnHintSkb_Draw(Actor* thisx, PlayState* play) {
    EnHintSkb* this = (EnHintSkb*)thisx;

    this->bodyPartsCount = 0;
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnHintSkb_OverrideLimbDraw,
                      EnHintSkb_PostLimbDraw, &this->actor);
    if (this->drawDmgEffTimer > 0) {
        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, this->bodyPartsCount, this->drawDmgEffScale,
                                0.5f, this->drawDmgEffAlpha, this->drawDmgEffType);
    }

    if (this->unk_3E8 & 4) {
        this->unk_3E8 |= 8;
    }
}

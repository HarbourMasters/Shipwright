/*
 * File: z_en_skb.c
 * Overlay: ovl_En_Skb
 * Description: Stalchild
 */

#include "z_en_skb.h"
#include "overlays/actors/ovl_En_Encount4/z_en_encount4.h"
#include "overlays/actors/ovl_En_Part/z_en_part.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void EnSkb_Init(Actor* thisx, PlayState* play);
void EnSkb_Destroy(Actor* thisx, PlayState* play);
void EnSkb_Update(Actor* thisx, PlayState* play);
void EnSkb_Draw(Actor* thisx, PlayState* play);

void func_80994E2C(EnSkb* this);
void func_80994E94(EnSkb* this, PlayState* play);
void func_80995068(EnSkb* this, PlayState* play);
void func_80995190(EnSkb* this, PlayState* play);
void func_80995244(EnSkb* this, PlayState* play);
void func_809952D8(EnSkb* this);
void func_8099533C(EnSkb* this, PlayState* play);
void func_809953E8(EnSkb* this);
void func_8099544C(EnSkb* this, PlayState* play);
void func_809954F8(EnSkb* this);
void func_8099556C(EnSkb* this, PlayState* play);
void func_8099571C(EnSkb* this);
void func_80995818(EnSkb* this, PlayState* play);
void func_8099599C(EnSkb* this, PlayState* play);
void func_80995A30(EnSkb* this);
void func_80995A8C(EnSkb* this, PlayState* play);
void func_80995C24(EnSkb* this);
void func_80995C84(EnSkb* this, PlayState* play);
void func_80995D3C(EnSkb* this);
void func_80995DC4(EnSkb* this, PlayState* play);
void func_80995E64(EnSkb* this, PlayState* play);
void func_809960AC(EnSkb* this, PlayState* play);
void func_809961E4(EnSkb* this, PlayState* play);
void func_80996284(EnSkb* this, PlayState* play);
void func_8099630C(EnSkb* this, PlayState* play);
void func_809963D8(EnSkb* this, PlayState* play);
void func_80996474(EnSkb* this);
void func_809964A0(EnSkb* this, PlayState* play);
s32 func_80996594(EnSkb* this, PlayState* play);
void func_80996BEC(EnSkb* this, PlayState* play);

typedef enum StalchildAnimation {
    /*  0 */ STALCHILD_ANIM_0,
    /*  1 */ STALCHILD_ANIM_1,
    /*  2 */ STALCHILD_ANIM_2,
    /*  3 */ STALCHILD_ANIM_3,
    /*  4 */ STALCHILD_ANIM_4,
    /*  5 */ STALCHILD_ANIM_5,
    /*  6 */ STALCHILD_ANIM_6,
    /*  7 */ STALCHILD_ANIM_7,
    /*  8 */ STALCHILD_ANIM_8,
    /*  9 */ STALCHILD_ANIM_9,
    /* 10 */ STALCHILD_ANIM_10,
    /* 11 */ STALCHILD_ANIM_11,
    /* 12 */ STALCHILD_ANIM_12,
    /* 13 */ STALCHILD_ANIM_MAX
} StalchildAnimation;

static AnimationInfo sAnimationInfo[STALCHILD_ANIM_MAX] = {
    { &gStalchildWalkAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0 },          // STALCHILD_ANIM_0
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },      // STALCHILD_ANIM_1
    { &gStalchildAttackAnim, 0.6f, 0.0f, 0.0f, ANIMMODE_ONCE_INTERP, 4.0f }, // STALCHILD_ANIM_2
    { &gStalchildStaggerAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0 },       // STALCHILD_ANIM_3
    { &gStalchildCollapseAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0 },      // STALCHILD_ANIM_4
    { &gStalchildSitLaughAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0 },      // STALCHILD_ANIM_5
    { &gStalchildSitTapToesAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0 },    // STALCHILD_ANIM_6
    { &gStalchildSwingOnBranchAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0 }, // STALCHILD_ANIM_7
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },      // STALCHILD_ANIM_8
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -16.0f },     // STALCHILD_ANIM_9
    { &gStalchildStaggerAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },      // STALCHILD_ANIM_10
    { &gStalchildSaluteAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0 },        // STALCHILD_ANIM_11
    { &gStalchildIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0 },          // STALCHILD_ANIM_12
};

static Vec3f D_80997468[] = {
    { -534.0f, 80.0f, -1277.0f },
    { 113.0f, 80.0f, -1359.0f },
    { 100.0f, 80.0f, -770.0f },
    { -334.0f, 80.0f, -720.0f },
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

ActorProfile En_Skb_Profile = {
    /**/ ACTOR_EN_SKB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SKB,
    /**/ sizeof(EnSkb),
    /**/ EnSkb_Init,
    /**/ EnSkb_Destroy,
    /**/ EnSkb_Update,
    /**/ EnSkb_Draw,
};

void func_809947B0(PlayState* play, EnSkb* this, Vec3f* inPos) {
    Vec3f pos;
    Vec3f velocity = { 0.0f, 8.0f, 0.0f };
    Vec3f accel = { 0.0f, -1.5f, 0.0f };
    f32 sp40 = (Rand_ZeroOne() - 0.5f) * 6.28f;
    s32 pad;

    pos.y = this->actor.floorHeight;
    pos.x = (Math_SinF(sp40) * 15.0f) + inPos->x;
    pos.z = (Math_CosF(sp40) * 15.0f) + inPos->z;
    accel.x = Rand_CenteredFloat(1.0f);
    accel.z = Rand_CenteredFloat(1.0f);
    velocity.y += (Rand_ZeroOne() - 0.5f) * 4.0f;
    EffectSsHahen_Spawn(play, &pos, &velocity, &accel, 0, ((Rand_ZeroOne() * 5.0f) + 12.0f) * 0.8f,
                        HAHEN_OBJECT_DEFAULT, 10, NULL);
    func_800BBFB0(play, &pos, 10.0f, 1, 150, 0, 1);
}

void func_8099495C(EnSkb* this, PlayState* play) {
    SkelAnime_Init(play, &this->skelAnime, &gStalchildSkel, &gStalchildStandUpAnim, this->jointTable, this->morphTable,
                   STALCHILD_LIMB_MAX);
    this->unk_3DC = 0;
    func_80994E2C(this);
}

void func_809949C4(EnSkb* this, PlayState* play) {
    SkelAnime_Init(play, &this->skelAnime, &gStalchildSkel, &gStalchildSitLaughAnim, this->jointTable, this->morphTable,
                   STALCHILD_LIMB_MAX);
    this->unk_3DC = 1;
    func_809952D8(this);
}

void func_80994A30(EnSkb* this, PlayState* play) {
    SkelAnime_Init(play, &this->skelAnime, &gStalchildSkel, &gStalchildSitTapToesAnim, this->jointTable,
                   this->morphTable, STALCHILD_LIMB_MAX);
    this->unk_3DC = 1;
    func_809953E8(this);
}

void func_80994A9C(EnSkb* this, PlayState* play) {
    SkelAnime_Init(play, &this->skelAnime, &gStalchildSkel, &gStalchildSwingOnBranchAnim, this->jointTable,
                   this->morphTable, STALCHILD_LIMB_MAX);
    this->unk_3DC = 1;
    func_809954F8(this);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void EnSkb_Init(Actor* thisx, PlayState* play) {
    EnSkb* this = (EnSkb*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    this->unk_3E0 = 0;
    this->unk_3E2 = 0;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.colChkInfo.mass = 70;
    this->actor.colChkInfo.health = 2;
    this->actor.shape.yOffset = -8000.0f;
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    this->collider.elements[0].dim.modelSphere.radius = this->collider.elements[0].dim.worldSphere.radius = 10;
    this->collider.elements[1].dim.modelSphere.radius = this->collider.elements[1].dim.worldSphere.radius = 20;
    this->actor.home.pos = this->actor.world.pos;
    this->unk_3D6 = ENSKB_GET_F0(&this->actor);
    this->actor.floorHeight = this->actor.world.pos.y;

    if ((play->sceneId == SCENE_BOTI) && (gSaveContext.sceneLayer == 1) && (play->csCtx.scriptIndex == 0)) {
        this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
    }

    switch (this->unk_3D6) {
        case 2:
            func_809949C4(this, play);
            break;

        case 3:
            func_80994A30(this, play);
            break;

        case 4:
            func_80994A9C(this, play);
            break;

        default:
            this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
            this->actor.hintId = TATL_HINT_ID_STALCHILD;
            func_8099495C(this, play);
            break;
    }
}

void EnSkb_Destroy(Actor* thisx, PlayState* play) {
    EnSkb* this = (EnSkb*)thisx;

    if ((this->actor.parent != NULL) && (this->actor.parent->update != NULL) &&
        (this->actor.parent->id == ACTOR_EN_ENCOUNT4)) {
        EnEncount4* encount = (EnEncount4*)this->actor.parent;

        if (encount->unk_14C > 0) {
            encount->unk_14C--;
        }

        if (this->actor.colChkInfo.health == 0) {
            encount->unk_14E++;
        } else {
            encount->unk_14C = 0;
            encount->unk_14E = encount->unk_14C;
        }
    }

    Collider_DestroyJntSph(play, &this->collider);
}

void func_80994DA8(EnSkb* this, PlayState* play) {
    if (Actor_IsFacingPlayer(&this->actor, 0x11C7) && (this->actor.xzDistToPlayer < 60.0f) &&
        (Player_GetMask(play) != PLAYER_MASK_CAPTAIN)) {
        func_80995C24(this);
    } else {
        func_80995A30(this);
    }
}

void func_80994E2C(EnSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_1);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_APPEAR);
    this->unk_3D0 = 0;
    this->unk_3DE = 0;
    this->actionFunc = func_80994E94;
}

void func_80994E94(EnSkb* this, PlayState* play) {
    if (this->skelAnime.curFrame < 4.0f) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }

    Math_ApproachZeroF(&this->actor.shape.yOffset, 1.0f, 800.0f);
    Math_ApproachF(&this->actor.shape.shadowScale, 25.0f, 1.0f, 2.5f);
    if ((play->gameplayFrames % 2) == 0) {
        func_809947B0(play, this, &this->actor.world.pos);
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) && (this->actor.shape.yOffset == 0.0f)) {
        func_80995A30(this);
    }
    this->unk_3D0++;
}

void func_80994F7C(EnSkb* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_3E2 = 1;
        if (this->unk_3E0 == 1) {
            Message_StartTextbox(play, 0x13F8, &this->actor);
        } else {
            Message_StartTextbox(play, 0x13F6, &this->actor);
            this->unk_3E0 = 1;
        }
        this->actionFunc = func_80995190;
        this->actor.speed = 0.0f;
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2AAA) && !(this->collider.base.acFlags & AC_HIT)) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void func_8099504C(EnSkb* this) {
    this->actionFunc = func_80995068;
    this->actor.speed = 0.0f;
}

void func_80995068(EnSkb* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_3E2 = 1;
        if (this->unk_3E0 == 1) {
            Message_StartTextbox(play, 0x13F8, &this->actor);
            if (this->unk_3DE == 2) {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_11);
            }
        } else {
            Message_StartTextbox(play, 0x13F6, &this->actor);
            this->unk_3E0 = 1;
        }
        this->actionFunc = func_80995190;
        this->actor.speed = 0.0f;
    } else if (Player_GetMask(play) != PLAYER_MASK_CAPTAIN) {
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.hintId = TATL_HINT_ID_STALCHILD;
        this->actor.colChkInfo.mass = MASS_HEAVY;
        func_80995A30(this);
    } else if (!(this->collider.base.acFlags & AC_HIT)) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void func_80995190(EnSkb* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
        case TEXT_STATE_CHOICE:
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                Message_StartTextbox(play, 0x13F7, &this->actor);
                if (this->unk_3DE == 2) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_11);
                }
            }
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                this->actionFunc = func_80995244;
            }
            break;
    }
}

void func_80995244(EnSkb* this, PlayState* play) {
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->unk_3E2 = 0;

    switch (this->unk_3DE) {
        case 9:
            this->actionFunc = func_8099533C;
            break;

        case 10:
            this->actionFunc = func_8099544C;
            break;

        case 11:
            this->actionFunc = func_8099556C;
            break;

        default:
            func_8099504C(this);
            break;
    }
}

void func_809952D8(EnSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_5);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_3DE = 9;
    this->actionFunc = func_8099533C;
    this->actor.shape.shadowScale = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.shape.yOffset = 0.0f;
}

void func_8099533C(EnSkb* this, PlayState* play) {
    if (Player_GetMask(play) == PLAYER_MASK_CAPTAIN) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        func_80994F7C(this, play);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2AAA) && (this->actor.xzDistToPlayer < 200.0f)) {
        this->actor.hintId = TATL_HINT_ID_STALCHILD;
        this->actor.colChkInfo.mass = MASS_HEAVY;
        func_8099571C(this);
    }
}

void func_809953E8(EnSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_6);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_3DE = 10;
    this->actionFunc = func_8099544C;
    this->actor.shape.shadowScale = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.shape.yOffset = 0.0f;
}

void func_8099544C(EnSkb* this, PlayState* play) {
    if (Player_GetMask(play) == PLAYER_MASK_CAPTAIN) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        func_80994F7C(this, play);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2AAA) && (this->actor.xzDistToPlayer < 200.0f)) {
        this->actor.hintId = TATL_HINT_ID_STALCHILD;
        this->actor.colChkInfo.mass = MASS_HEAVY;
        func_8099571C(this);
    }
}

void func_809954F8(EnSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_7);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_3DE = 11;
    this->actionFunc = func_8099556C;
    this->actor.gravity = 0.0f;
    this->actor.shape.shadowScale = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actor.lockOnArrowOffset = 4000.0f;
}

void func_8099556C(EnSkb* this, PlayState* play) {
    s16 sp26;

    if (this->skelAnime.curFrame < 15.0f) {
        sp26 = this->skelAnime.curFrame - 7.5f;
    } else {
        sp26 = 22.5f - this->skelAnime.curFrame;
    }

    if (Animation_OnFrame(&this->skelAnime, 22.5f)) {
        this->unk_3D4 = Rand_Next() % 0x7D0;
    }

    this->actor.shape.rot.x = Math_SinS(this->unk_3D4 * sp26) * 0x4E20;
    if (Player_GetMask(play) == PLAYER_MASK_CAPTAIN) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        func_80994F7C(this, play);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2AAA) && (this->actor.xzDistToPlayer < 200.0f) &&
               (this->skelAnime.curFrame > 24.0f) && (this->skelAnime.curFrame < 28.0f)) {
        this->actor.hintId = TATL_HINT_ID_STALCHILD;
        this->actor.colChkInfo.mass = MASS_HEAVY;
        this->actor.lockOnArrowOffset = 2000.0f;
        func_8099571C(this);
    }
}

void func_8099571C(EnSkb* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
    this->unk_3DC = 0;
    this->actor.shape.shadowScale = 0.0f;
    if (this->unk_3DE == 9) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_8);
        this->actor.speed = 2.4f;
        this->actor.gravity = -1.0f;
        this->actor.velocity.y = 3.0f;
    } else if (this->unk_3DE == 0xA) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_8);
    } else if (this->unk_3DE == 0xB) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_9);
        this->actor.speed = 3.2f;
        this->actor.gravity = -1.0f;
        this->actor.velocity.y = 2.0f;
    }
    this->unk_3DE = 13;
    this->actionFunc = func_80995818;
}

void func_80995818(EnSkb* this, PlayState* play) {
    s32 i;

    Math_ApproachF(&this->actor.shape.shadowScale, 25.0f, 1.0f, 2.5f);
    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 0x10, 0x7D0, 0x64);
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->actor.shape.rot.x = 0;
        this->actor.gravity = -2.0f;
        func_80995A30(this);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
        for (i = 0; i < 10; i++) {
            func_809947B0(play, this, &this->actor.world.pos);
        }
    }
}

void func_809958F4(EnSkb* this) {
    Animation_Change(&this->skelAnime, &gStalchildStandUpAnim, -1.0f, Animation_GetLastFrame(&gStalchildStandUpAnim),
                     0.0f, ANIMMODE_ONCE, -4.0f);
    this->unk_3E4 = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
    this->unk_3DE = 1;
    this->actionFunc = func_8099599C;
}

void func_8099599C(EnSkb* this, PlayState* play) {
    Math_ApproachF(&this->actor.shape.yOffset, -8000.0f, 1.0f, 500.0f);

    if ((play->gameplayFrames % 2) != 0) {
        func_809947B0(play, this, &this->actor.world.pos);
    }

    Math_ApproachF(&this->actor.shape.shadowScale, 0.0f, 1.0f, 2.5f);

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_Kill(&this->actor);
    }
}

void func_80995A30(EnSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_0);
    this->actor.speed = 1.6f;
    this->unk_3DA = 0;
    this->unk_3DE = 2;
    this->actionFunc = func_80995A8C;
}

void func_80995A8C(EnSkb* this, PlayState* play) {
    if (Player_GetMask(play) == PLAYER_MASK_CAPTAIN) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->actor.hintId = TATL_HINT_ID_NONE;
        this->actor.colChkInfo.mass = MASS_HEAVY;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_12);
        func_8099504C(this);
        return;
    }

    if ((this->unk_3D8 != 0) && ((play->gameplayFrames % 16) == 0)) {
        this->unk_3DA = Rand_CenteredFloat(50000.0f);
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + this->unk_3DA, 1, 0x2EE, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Animation_OnFrame(&this->skelAnime, 8.0f) || Animation_OnFrame(&this->skelAnime, 15.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_WALK);
    }

    if ((this->actor.xzDistToPlayer > 800.0f) || func_80996594(this, play)) {
        func_809958F4(this);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x11C7) && (this->actor.xzDistToPlayer < 60.0f)) {
        func_80995C24(this);
    }
}

void func_80995C24(EnSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_2);
    this->collider.base.atFlags &= ~AT_BOUNCED;
    this->actor.speed = 0.0f;
    this->unk_3DE = 3;
    this->actionFunc = func_80995C84;
}

void func_80995C84(EnSkb* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 3.0f) && (this->unk_3E4 == 0)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
        this->unk_3E4 = 1;
    } else if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        this->unk_3E4 = 0;
    }

    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
        func_80995D3C(this);
    } else if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        func_80994DA8(this, play);
    }
}

void func_80995D3C(EnSkb* this) {
    Animation_Change(&this->skelAnime, &gStalchildAttackAnim, -0.4f, this->skelAnime.curFrame - 1.0f, 0.0f,
                     ANIMMODE_ONCE_INTERP, 0.0f);
    this->collider.base.atFlags &= ~AT_BOUNCED;
    this->unk_3DE = 4;
    this->unk_3E4 = 0;
    this->actionFunc = func_80995DC4;
}

void func_80995DC4(EnSkb* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        func_80994DA8(this, play);
    }
}

void func_80995E08(EnSkb* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->unk_3E4 = 0;
    this->unk_3DE = 5;
    this->actionFunc = func_80995E64;
}

void func_80995E64(EnSkb* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
    }

    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->unk_3DC == 0) {
            if (this->actor.colChkInfo.health == 0) {
                func_809961E4(this, play);
            } else {
                func_80994DA8(this, play);
            }
        } else {
            switch (this->unk_3D6) {
                case 2:
                    func_809952D8(this);
                    break;

                case 3:
                    func_809953E8(this);
                    break;
            }
        }
    } else if ((this->actor.colorFilterTimer == 0) && (this->unk_3DC == 1) && (this->unk_3D6 == 4)) {
        this->unk_3DE = 11;
        this->actionFunc = func_8099556C;
    }
}

void func_80995F98(EnSkb* this) {
    if ((this->unk_3DE == 9) || (this->unk_3DE == 0xA)) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_8);
        this->actor.gravity = -1.0f;
        this->actor.speed = 1.0f;
    } else if (this->unk_3DE == 0xB) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_9);
        this->actor.speed = 3.2f;
        this->actor.velocity.y = 2.0f;
        this->actor.gravity = -1.0f;
    } else {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_3);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.speed = -4.0f;
        }
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
    this->unk_3DE = 6;
    this->actionFunc = func_809960AC;
}

void func_809960AC(EnSkb* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        s32 i;

        for (i = 0; i < 10; i++) {
            func_809947B0(play, this, &this->actor.world.pos);
        }
        this->actor.speed = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x9C4, 0);
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 0x10, 0x7D0, 0x64);
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.shape.rot.x = 0;
        this->actor.world.rot = this->actor.shape.rot;
        func_80994DA8(this, play);
    }
}

void func_809961E4(EnSkb* this, PlayState* play) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_4);
    this->unk_3D8 |= 0x40;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -6.0f;
    }
    this->unk_3E4 = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
    this->unk_3DE = 7;
    this->actionFunc = func_80996284;
}

void func_80996284(EnSkb* this, PlayState* play) {
    if (!(this->unk_3D8 & 0x80)) {
        return;
    }
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x10);
    func_80996474(this);
}

void func_809962D4(EnSkb* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = func_8099630C;
}

void func_8099630C(EnSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer == 0) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffScale = 0.0f;
        this->drawDmgEffAlpha = 0.0f;
        if (this->actor.colChkInfo.health != 0) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_3);
            this->unk_3D8 |= 1;
            func_80995F98(this);
        } else {
            func_809961E4(this, play);
        }
    } else if (this->drawDmgEffTimer == 1) {
        func_80996BEC(this, play);
    }
}

void func_809963C4(EnSkb* this) {
    this->actionFunc = func_809963D8;
}

void func_809963D8(EnSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer == 0) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffScale = 0.0f;
        this->drawDmgEffAlpha = 0.0f;
        if (this->actor.colChkInfo.health != 0) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_3);
            this->unk_3D8 |= 1;
            func_80995F98(this);
        } else {
            func_809961E4(this, play);
        }
    }
}

void func_80996474(EnSkb* this) {
    this->unk_3D0 = 0;
    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_809964A0;
}

void func_809964A0(EnSkb* this, PlayState* play) {
    if (this->unk_3D0++ >= 19) {
        Actor_Kill(&this->actor);
    }
}

void func_809964DC(EnSkb* this, PlayState* play) {
    if (this->unk_3D6 == 0) {
        if ((this->actionFunc != func_80994E94) && (this->actionFunc != func_80996284) &&
            (this->actionFunc != func_809964A0) && CHECK_WEEKEVENTREG(WEEKEVENTREG_85_40)) {
            this->actor.colChkInfo.health = 0;
            func_809961E4(this, play);
        }
    }
}

s32 func_80996544(Vec2f arg0, Vec2f arg1) {
    s32 ret;

    if ((arg1.x * arg0.z) < (arg0.x * arg1.z)) {
        ret = 1;
    } else {
        ret = -1;
    }

    return ret;
}

s32 func_80996594(EnSkb* this, PlayState* play) {
    s32 temp_s2;
    s32 j;
    Vec2f sp60;
    Vec2f sp58;
    s32 sp54 = true;
    f32 worldZ;
    s32 i = 0;
    f32 worldX;

    worldZ = this->actor.world.pos.z;
    worldX = this->actor.world.pos.x;

    sp60.x = D_80997468[0].z - worldZ;
    sp60.z = D_80997468[0].x - worldX;
    sp58.x = D_80997468[1].z - worldZ;
    sp58.z = D_80997468[1].x - worldX;
    j = 1;
    temp_s2 = func_80996544(sp60, sp58);

    //! FAKE:
    if (1) {}

    do {
        i++;
        if (j < 3) {
            j++;
        } else {
            j = 0;
        }
        sp60.x = D_80997468[i].z - worldZ;
        sp60.z = D_80997468[i].x - worldX;
        sp58.x = D_80997468[j].z - worldZ;
        sp58.z = D_80997468[j].x - worldX;
        if (func_80996544(sp60, sp58) != temp_s2) {
            sp54 = false;
            break;
        }
    } while (j != 0);

    return sp54;
}

void func_8099672C(EnSkb* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if ((this->unk_3DE != 6) && (this->unk_3DE != 7)) {
        if ((this->actor.bgCheckFlags & (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH)) &&
            (this->actor.depthInWater >= 40.0f)) {
            this->actor.colChkInfo.health = 0;
            this->unk_3E4 = 0;
            func_809961E4(this, play);
            return;
        }

        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->actionFunc == func_8099630C) {
                switch (this->actor.colChkInfo.damageEffect) {
                    case 1:
                    case 3:
                    case 4:
                    case 12:
                    case 13:
                        return;

                    default:
                        if (this->drawDmgEffTimer > 1) {
                            func_80996BEC(this, play);
                        }
                        this->drawDmgEffTimer = 0;
                        break;
                }
            }

            if (!Actor_ApplyDamage(&this->actor) && (this->actor.colChkInfo.damageEffect != 3) &&
                (this->actor.colChkInfo.damageEffect != 4)) {
                this->unk_3D8 |= 0x40;
                func_809961E4(this, play);
                return;
            }

            switch (this->actor.colChkInfo.damageEffect) {
                case 11:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffAlpha = 1.0f;
                    this->drawDmgEffScale = 0.0f;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                    func_80995E08(this);
                    break;

                case 1:
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                    func_80995E08(this);
                    break;

                case 2:
                    this->drawDmgEffTimer = 80;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    this->drawDmgEffAlpha = 1.0f;
                    this->drawDmgEffScale = 0.0f;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                    this->unk_3D8 |= 1;
                    func_80995F98(this);
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
                    func_809962D4(this);
                    break;

                case 4:
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    this->drawDmgEffAlpha = 1.0f;
                    this->drawDmgEffScale = 0.5f;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_3);
                    func_809963C4(this);
                    break;

                case 12:
                case 14:
                    this->unk_3D8 |= 1;

                case 15:
                    if ((player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) ||
                        (player->meleeWeaponAnimation == PLAYER_MWA_LEFT_COMBO_2H) ||
                        (player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_RIGHT) ||
                        (player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_LEFT)) {
                        this->unk_3D8 |= 1;
                    }

                case 13:
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, STALCHILD_ANIM_3);
                    func_80995F98(this);
                    break;
            }
        }
    }
}

void func_80996AD0(EnSkb* this, PlayState* play) {
    if ((this->actionFunc == func_80996284) || (this->unk_3E2 == 1)) {
        return;
    }

    if (this->unk_3E4 != 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (((this->unk_3DE != 0) || (this->unk_3D0 >= 11)) && (this->unk_3DE != 1) && (this->unk_3DE != 4) &&
        (this->unk_3DE != 6) && (this->unk_3DE != 7) &&
        ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000))) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if ((this->actionFunc != func_80996284) && (this->actionFunc != func_809964A0)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_80996BEC(EnSkb* this, PlayState* play) {
    static Color_RGBA8 D_80997550 = { 170, 255, 255, 255 };
    static Color_RGBA8 D_80997554 = { 200, 200, 255, 255 };
    static Vec3f D_80997558 = { 0.0f, -1.0f, 0.0f };
    Vec3f sp84;
    s32 i;
    s16 yaw;
    s32 bodyPartsCount;

    if (this->unk_3D8 & 2) {
        bodyPartsCount = ENSKB_BODYPART_MAX - 1;
    } else {
        bodyPartsCount = ENSKB_BODYPART_MAX;
    }

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < bodyPartsCount; i++) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->bodyPartsPos[i]);
        sp84.x = Math_SinS(yaw) * 3.0f;
        sp84.z = Math_CosS(yaw) * 3.0f;
        sp84.y = (Rand_ZeroOne() * 4.0f) + 4.0f;
        EffectSsEnIce_Spawn(play, &this->bodyPartsPos[i], 0.6f, &sp84, &D_80997558, &D_80997550, &D_80997554, 30);
    }
}

void func_80996D68(EnSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer > 0) {
        this->drawDmgEffTimer--;
    }

    if (this->actionFunc != func_8099630C) {
        if (this->drawDmgEffTimer < 20) {
            if (this->actionFunc == func_809963D8) {
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

void EnSkb_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSkb* this = (EnSkb*)thisx;

    this->actionFunc(this, play);
    if ((this->actionFunc != func_80995E64) && (this->actionFunc != func_80996284) &&
        (this->actionFunc != func_8099630C) && (this->actionFunc != func_809963D8)) {
        SkelAnime_Update(&this->skelAnime);
    }

    func_8099672C(this, play);
    if ((this->actionFunc != func_8099630C) && (this->actionFunc != func_809963D8) &&
        (this->actionFunc != func_80995E64)) {
        Actor_MoveWithGravity(&this->actor);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    func_809964DC(this, play);
    func_80996AD0(this, play);
    func_80996D68(this, play);
}

s32 EnSkb_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnSkb* this = (EnSkb*)thisx;
    s32 pad;
    s16 sins;

    if (limbIndex == STALCHILD_LIMB_HEAD) {
        if (!(this->unk_3D8 & 2)) {
            OPEN_DISPS(play->state.gfxCtx);

            sins = fabsf(Math_SinS(play->gameplayFrames * 6000) * 95.0f) + 160.0f;

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, sins, sins, sins, 255);

            CLOSE_DISPS(play->state.gfxCtx);
        } else {
            *dList = NULL;
        }
    } else if ((limbIndex == STALCHILD_LIMB_LOWER_JAW) && (this->unk_3D8 & 2)) {
        *dList = NULL;
    }

    if (limbIndex == STALCHILD_LIMB_RIBCAGE) {
        Matrix_MultZero(&this->actor.focus.pos);
    }

    return false;
}

void EnSkb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80997564 = { 800.0f, 1200.0f, 0.0f };
    EnSkb* this = (EnSkb*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->collider);
    if ((this->unk_3D8 & 1) && !(this->unk_3D8 & 2)) {
        if (limbIndex == STALCHILD_LIMB_HEAD) {
            Actor_SpawnBodyParts(&this->actor, play, ENPART_PARAMS(ENPART_TYPE_1), dList);
            this->unk_3D8 |= 2;
        }
    } else if ((this->unk_3D8 & 0x40) && !(this->unk_3D8 & 0x80) &&
               ((limbIndex != STALCHILD_LIMB_HEAD) || !(this->unk_3D8 & 1)) &&
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
        } else if ((limbIndex == STALCHILD_LIMB_HEAD) && !(this->unk_3D8 & 2)) {
            Matrix_MultVec3f(&D_80997564, &this->bodyPartsPos[this->bodyPartsCount]);
            this->bodyPartsCount++;
        }
    }
}

void EnSkb_Draw(Actor* thisx, PlayState* play) {
    EnSkb* this = (EnSkb*)thisx;

    this->bodyPartsCount = 0;
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnSkb_OverrideLimbDraw,
                      EnSkb_PostLimbDraw, &this->actor);
    if (this->drawDmgEffTimer > 0) {
        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, this->bodyPartsCount, this->drawDmgEffScale,
                                0.5f, this->drawDmgEffAlpha, this->drawDmgEffType);
    }

    if (this->unk_3D8 & 0x40) {
        this->unk_3D8 |= 0x80;
    }
}

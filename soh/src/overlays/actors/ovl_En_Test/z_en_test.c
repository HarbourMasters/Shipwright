/*
 * File: z_en_test.c
 * Overlay: ovl_En_Test
 * Description: Stalfos
 */

#include "z_en_test.h"
#include "objects/object_sk2/object_sk2.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnTest_Init(Actor* thisx, PlayState* play);
void EnTest_Destroy(Actor* thisx, PlayState* play);
void EnTest_Update(Actor* thisx, PlayState* play);
void EnTest_Draw(Actor* thisx, PlayState* play);

void EnTest_SetupWaitGround(EnTest* this);
void EnTest_SetupWaitAbove(EnTest* this);
void EnTest_SetupJumpBack(EnTest* this);
void EnTest_SetupSlashDownEnd(EnTest* this);
void EnTest_SetupSlashUp(EnTest* this);
void EnTest_SetupJumpslash(EnTest* this);
void EnTest_SetupWalkAndBlock(EnTest* this);
void func_80860EC0(EnTest* this);
void EnTest_SetupSlashDown(EnTest* this);
void func_80860BDC(EnTest* this);
void EnTest_SetupIdleFromBlock(EnTest* this);
void EnTest_SetupRecoil(EnTest* this);
void func_80862398(EnTest* this);
void func_80862154(EnTest* this);
void EnTest_SetupStopAndBlock(EnTest* this);
void func_808627C4(EnTest* this, PlayState* play);

void EnTest_WaitGround(EnTest* this, PlayState* play);
void EnTest_WaitAbove(EnTest* this, PlayState* play);
void EnTest_Fall(EnTest* this, PlayState* play);
void EnTest_Land(EnTest* this, PlayState* play);
void EnTest_Rise(EnTest* this, PlayState* play);
void EnTest_Idle(EnTest* this, PlayState* play);
void EnTest_WalkAndBlock(EnTest* this, PlayState* play);
void func_80860C24(EnTest* this, PlayState* play);
void func_80860F84(EnTest* this, PlayState* play);
void EnTest_SlashDown(EnTest* this, PlayState* play);
void EnTest_SlashDownEnd(EnTest* this, PlayState* play);
void EnTest_SlashUp(EnTest* this, PlayState* play);
void EnTest_JumpBack(EnTest* this, PlayState* play);
void EnTest_Jumpslash(EnTest* this, PlayState* play);
void EnTest_JumpUp(EnTest* this, PlayState* play);
void EnTest_StopAndBlock(EnTest* this, PlayState* play);
void EnTest_IdleFromBlock(EnTest* this, PlayState* play);
void func_808621D4(EnTest* this, PlayState* play);
void func_80862418(EnTest* this, PlayState* play);
void EnTest_Stunned(EnTest* this, PlayState* play);
void func_808628C8(EnTest* this, PlayState* play);
void func_80862E6C(EnTest* this, PlayState* play);
void func_80863044(EnTest* this, PlayState* play);
void func_8086318C(EnTest* this, PlayState* play);
void EnTest_Recoil(EnTest* this, PlayState* play);
void func_808633E8(EnTest* this, PlayState* play);
void func_80862FA8(EnTest* this, PlayState* play);

s32 EnTest_ReactToProjectile(PlayState* play, EnTest* this);

static u8 sJointCopyFlags[] = {
    false, // STALFOS_LIMB_NONE
    false, // STALFOS_LIMB_ROOT
    false, // STALFOS_LIMB_UPPERBODY_ROOT
    false, // STALFOS_LIMB_CORE_LOWER_ROOT
    true,  // STALFOS_LIMB_CORE_UPPER_ROOT
    true,  // STALFOS_LIMB_NECK_ROOT
    true,  // STALFOS_LIMB_HEAD_ROOT
    true,  // STALFOS_LIMB_7
    true,  // STALFOS_LIMB_8
    true,  // STALFOS_LIMB_JAW_ROOT
    true,  // STALFOS_LIMB_JAW
    true,  // STALFOS_LIMB_HEAD
    true,  // STALFOS_LIMB_NECK_UPPER
    true,  // STALFOS_LIMB_NECK_LOWER
    true,  // STALFOS_LIMB_CORE_UPPER
    true,  // STALFOS_LIMB_CHEST
    true,  // STALFOS_LIMB_SHOULDER_R_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_R_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_R
    true,  // STALFOS_LIMB_SHOULDER_L_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_L_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_L
    true,  // STALFOS_LIMB_ARM_L_ROOT
    true,  // STALFOS_LIMB_UPPERARM_L_ROOT
    true,  // STALFOS_LIMB_FOREARM_L_ROOT
    true,  // STALFOS_LIMB_HAND_L_ROOT
    true,  // STALFOS_LIMB_HAND_L
    true,  // STALFOS_LIMB_SHIELD
    true,  // STALFOS_LIMB_FOREARM_L
    true,  // STALFOS_LIMB_UPPERARM_L
    true,  // STALFOS_LIMB_ARM_R_ROOT
    true,  // STALFOS_LIMB_UPPERARM_R_ROOT
    true,  // STALFOS_LIMB_FOREARM_R_ROOT
    true,  // STALFOS_LIMB_HAND_R_ROOT
    true,  // STALFOS_LIMB_SWORD
    true,  // STALFOS_LIMB_HAND_R
    true,  // STALFOS_LIMB_FOREARM_R
    true,  // STALFOS_LIMB_UPPERARM_R
    true,  // STALFOS_LIMB_CORE_LOWER
    false, // STALFOS_LIMB_LOWERBODY_ROOT
    false, // STALFOS_LIMB_WAIST_ROOT
    false, // STALFOS_LIMB_LEGS_ROOT
    false, // STALFOS_LIMB_LEG_L_ROOT
    false, // STALFOS_LIMB_THIGH_L_ROOT
    false, // STALFOS_LIMB_LOWERLEG_L_ROOT
    false, // STALFOS_LIMB_ANKLE_L_ROOT
    false, // STALFOS_LIMB_ANKLE_L
    false, // STALFOS_LIMB_FOOT_L_ROOT
    false, // STALFOS_LIMB_FOOT_L
    false, // STALFOS_LIMB_LOWERLEG_L
    false, // STALFOS_LIMB_THIGH_L
    false, // STALFOS_LIMB_LEG_R_ROOT
    false, // STALFOS_LIMB_THIGH_R_ROOT
    false, // STALFOS_LIMB_LOWERLEG_R_ROOT
    false, // STALFOS_LIMB_ANKLE_R_ROOT
    false, // STALFOS_LIMB_ANKLE_R
    false, // STALFOS_LIMB_FOOT_R_ROOT
    false, // STALFOS_LIMB_FOOT_R
    false, // STALFOS_LIMB_LOWERLEG_R
    false, // STALFOS_LIMB_THIGH_R
    false, // STALFOS_LIMB_WAIST
};

const ActorInit En_Test_InitVars = {
    ACTOR_EN_TEST,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SK2,
    sizeof(EnTest),
    (ActorFunc)EnTest_Init,
    (ActorFunc)EnTest_Destroy,
    (ActorFunc)EnTest_Update,
    (ActorFunc)EnTest_Draw,
    NULL,
};

static ColliderCylinderInit sBodyColliderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
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
    { 25, 65, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sShieldColliderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFC1FFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 20, 70, -50, { 0, 0, 0 } },
};

static ColliderQuadInit sSwordColliderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ STALFOS_DMGEFF_NORMAL,
    /* 0x1 */ STALFOS_DMGEFF_STUN,
    /* 0x6 */ STALFOS_DMGEFF_FIREMAGIC = 6,
    /* 0xD */ STALFOS_DMGEFF_SLING = 0xD,
    /* 0xE */ STALFOS_DMGEFF_LIGHT,
    /* 0xF */ STALFOS_DMGEFF_FREEZE
} StalfosDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, STALFOS_DMGEFF_SLING),
    /* Explosive     */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, STALFOS_DMGEFF_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Ice arrow     */ DMG_ENTRY(4, STALFOS_DMGEFF_FREEZE),
    /* Light arrow   */ DMG_ENTRY(2, STALFOS_DMGEFF_LIGHT),
    /* Unk arrow 1   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Unk arrow 2   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Unk arrow 3   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Fire magic    */ DMG_ENTRY(0, STALFOS_DMGEFF_FIREMAGIC),
    /* Ice magic     */ DMG_ENTRY(3, STALFOS_DMGEFF_FREEZE),
    /* Light magic   */ DMG_ENTRY(0, STALFOS_DMGEFF_LIGHT),
    /* Shield        */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Mirror Ray    */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Kokiri spin   */ DMG_ENTRY(1, STALFOS_DMGEFF_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, STALFOS_DMGEFF_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Unblockable   */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Hammer jump   */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Unknown 2     */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x1B, ICHAIN_CONTINUE),    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_CONTINUE), ICHAIN_F32(scale.y, 0, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1500, ICHAIN_STOP),
};

void EnTest_SetupAction(EnTest* this, EnTestActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnTest_Init(Actor* thisx, PlayState* play) {
    EffectBlureInit1 slashBlure;
    EnTest* this = (EnTest*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    SkelAnime_Init(play, &this->skelAnime, &gStalfosSkel, &gStalfosMiddleGuardAnim, this->jointTable,
                   this->morphTable, STALFOS_LIMB_MAX);
    SkelAnime_Init(play, &this->upperSkelanime, &gStalfosSkel, &gStalfosMiddleGuardAnim, this->upperJointTable,
                   this->upperMorphTable, STALFOS_LIMB_MAX);

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, 90.0f);

    this->actor.colChkInfo.cylRadius = 40;
    this->actor.colChkInfo.cylHeight = 100;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 45.0f;
    this->actor.colChkInfo.damageTable = &sDamageTable;

    Collider_InitCylinder(play, &this->bodyCollider);
    Collider_SetCylinder(play, &this->bodyCollider, &this->actor, &sBodyColliderInit);

    Collider_InitCylinder(play, &this->shieldCollider);
    Collider_SetCylinder(play, &this->shieldCollider, &this->actor, &sShieldColliderInit);

    Collider_InitQuad(play, &this->swordCollider);
    Collider_SetQuad(play, &this->swordCollider, &this->actor, &sSwordColliderInit);

    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 10;

    slashBlure.p1StartColor[0] = slashBlure.p1StartColor[1] = slashBlure.p1StartColor[2] = slashBlure.p1StartColor[3] =
        slashBlure.p2StartColor[0] = slashBlure.p2StartColor[1] = slashBlure.p2StartColor[2] =
            slashBlure.p1EndColor[0] = slashBlure.p1EndColor[1] = slashBlure.p1EndColor[2] = slashBlure.p2EndColor[0] =
                slashBlure.p2EndColor[1] = slashBlure.p2EndColor[2] = 255;

    slashBlure.p1EndColor[3] = 0;
    slashBlure.p2EndColor[3] = 0;
    slashBlure.p2StartColor[3] = 64;

    slashBlure.elemDuration = 4;
    slashBlure.unkFlag = 0;
    slashBlure.calcMode = 2;

    Effect_Add(play, &this->effectIndex, EFFECT_BLURE1, 0, 0, &slashBlure);

    if (this->actor.params != STALFOS_TYPE_CEILING) {
        EnTest_SetupWaitGround(this);
    } else {
        EnTest_SetupWaitAbove(this);
    }

    if (this->actor.params == STALFOS_TYPE_INVISIBLE) {
        this->actor.flags |= ACTOR_FLAG_7;
    }
}

void EnTest_Destroy(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;

    if ((this->actor.params != STALFOS_TYPE_2) &&
        !Actor_FindNearby(play, &this->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f)) {
        func_800F5B58();
    }

    Effect_Delete(play, this->effectIndex);
    Collider_DestroyCylinder(play, &this->shieldCollider);
    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyQuad(play, &this->swordCollider);
}

/**
 * If EnTest_ChooseAction failed to pick a new action, this function will unconditionally pick
 * a new action as a last resort
 */
void EnTest_ChooseRandomAction(EnTest* this, PlayState* play) {
    switch ((u32)(Rand_ZeroOne() * 10.0f)) {
        case 0:
        case 1:
        case 5:
        case 6:
            if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 170.0f) &&
                Actor_IsFacingPlayer(&this->actor, 0x71C) && Actor_IsTargeted(play, &this->actor)) {
                EnTest_SetupJumpslash(this);
                break;
            }
            // fallthrough
        case 8:
            EnTest_SetupWalkAndBlock(this);
            break;

        case 3:
        case 4:
        case 7:
            func_808627C4(this, play);
            break;

        case 2:
        case 9:
        case 10:
            EnTest_SetupStopAndBlock(this);
            break;
    }
}

void EnTest_ChooseAction(EnTest* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

    yawDiff = ABS(yawDiff);

    if (yawDiff >= 0x61A8) {
        switch ((u32)(Rand_ZeroOne() * 10.0f)) {
            case 0:
            case 3:
            case 7:
                EnTest_SetupStopAndBlock(this);
                break;

            case 1:
            case 5:
            case 6:
            case 8:
                func_808627C4(this, play);
                break;

            case 2:
            case 4:
            case 9:
                if (this->actor.params != STALFOS_TYPE_CEILING) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    EnTest_SetupJumpBack(this);
                }
                break;
        }
    } else if (yawDiff <= 0x3E80) {
        if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) > 0x3E80) {
            if (((play->gameplayFrames % 2) != 0) && (this->actor.params != STALFOS_TYPE_CEILING)) {
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                EnTest_SetupJumpBack(this);
            } else if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 170.0f)) {
                if (Actor_IsFacingPlayer(&this->actor, 0x71C) && !Actor_IsTargeted(play, &this->actor)) {
                    EnTest_SetupJumpslash(this);
                }
            } else {
                EnTest_SetupWalkAndBlock(this);
            }
        } else {
            if (this->actor.xzDistToPlayer < 110.0f) {
                if (Rand_ZeroOne() > 0.2f) {
                    if (player->stateFlags1 & 0x10) {
                        if (this->actor.isTargeted) {
                            EnTest_SetupSlashDown(this);
                        } else {
                            func_808627C4(this, play);
                        }
                    } else {
                        EnTest_SetupSlashDown(this);
                    }
                }
            } else {
                EnTest_ChooseRandomAction(this, play);
            }
        }
    } else {
        EnTest_ChooseRandomAction(this, play);
    }
}

void EnTest_SetupWaitGround(EnTest* this) {
    Animation_PlayLoop(&this->skelAnime, &gStalfosMiddleGuardAnim);
    this->unk_7C8 = 0;
    this->timer = 15;
    this->actor.scale.y = 0.0f;
    this->actor.world.pos.y = this->actor.home.pos.y - 3.5f;
    this->actor.flags &= ~ACTOR_FLAG_0;
    EnTest_SetupAction(this, EnTest_WaitGround);
}

void EnTest_WaitGround(EnTest* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((this->timer == 0) && (ABS(this->actor.yDistToPlayer) < 150.0f)) {
        this->unk_7C8 = 3;
        EnTest_SetupAction(this, EnTest_Rise);
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;

        if (this->actor.params != STALFOS_TYPE_2) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        this->actor.world.pos.y = this->actor.home.pos.y - 3.5f;
    }
}

void EnTest_SetupWaitAbove(EnTest* this) {
    Animation_PlayLoop(&this->skelAnime, &gStalfosMiddleGuardAnim);
    this->unk_7C8 = 0;
    this->actor.world.pos.y = this->actor.home.pos.y + 150.0f;
    Actor_SetScale(&this->actor, 0.0f);
    this->actor.flags &= ~ACTOR_FLAG_0;
    EnTest_SetupAction(this, EnTest_WaitAbove);
}

void EnTest_WaitAbove(EnTest* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.world.pos.y = this->actor.home.pos.y + 150.0f;

    if ((this->actor.xzDistToPlayer < 200.0f) && (ABS(this->actor.yDistToPlayer) < 450.0f)) {
        EnTest_SetupAction(this, EnTest_Fall);
        this->actor.flags |= ACTOR_FLAG_0;
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        Actor_SetScale(&this->actor, 0.015f);
    }
}

void EnTest_SetupIdle(EnTest* this) {
    Animation_PlayLoop(&this->skelAnime, &gStalfosMiddleGuardAnim);
    this->unk_7C8 = 0xA;
    this->timer = (Rand_ZeroOne() * 10.0f) + 5.0f;
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnTest_SetupAction(this, EnTest_Idle);
}

void EnTest_Idle(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);

    if (!EnTest_ReactToProjectile(play, this)) {
        yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

        if (this->actor.xzDistToPlayer < 100.0f) {
            if ((player->swordState != 0) && (ABS(yawDiff) >= 0x1F40)) {
                this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

                if (Rand_ZeroOne() > 0.7f && player->meleeWeaponAnimation != 0x11) {
                    EnTest_SetupJumpBack(this);
                } else {
                    func_808627C4(this, play);
                }
                return;
            }
        }

        if (this->timer != 0) {
            this->timer--;
        } else {
            if (Actor_IsFacingPlayer(&this->actor, 0x1555)) {
                if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 160.0f) &&
                    (Rand_ZeroOne() < 0.3f)) {
                    if (Actor_IsTargeted(play, &this->actor)) {
                        EnTest_SetupJumpslash(this);
                    } else {
                        func_808627C4(this, play);
                    }
                } else {
                    if (Rand_ZeroOne() > 0.3f) {
                        EnTest_SetupWalkAndBlock(this);
                    } else {
                        func_808627C4(this, play);
                    }
                }
            } else {
                if (Rand_ZeroOne() > 0.7f) {
                    func_80860BDC(this);
                } else {
                    EnTest_ChooseAction(this, play);
                }
            }
        }
    }
}

void EnTest_Fall(EnTest* this, PlayState* play) {
    Animation_PlayOnceSetSpeed(&this->skelAnime, &gStalfosLandFromLeapAnim, 0.0f);
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.world.pos.y <= this->actor.floorHeight) {
        this->skelAnime.playSpeed = 1.0f;
        this->unk_7C8 = 0xC;
        this->timer = this->unk_7E4 * 0.15f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
        EnTest_SetupAction(this, EnTest_Land);
    }
}

void EnTest_Land(EnTest* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnTest_SetupIdle(this);
        this->timer = (Rand_ZeroOne() * 10.0f) + 5.0f;
    }
}

void EnTest_SetupWalkAndBlock(EnTest* this) {
    Animation_Change(&this->upperSkelanime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                     Animation_GetLastFrame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
    Animation_PlayLoop(&this->skelAnime, &gStalfosSlowAdvanceAnim);
    this->timer = (s16)(Rand_ZeroOne() * 5.0f);
    this->unk_7C8 = 0xD;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnTest_SetupAction(this, EnTest_WalkAndBlock);
}

void EnTest_WalkAndBlock(EnTest* this, PlayState* play) {
    s32 pad;
    f32 checkDist = 0.0f;
    s32 pad1;
    s32 prevFrame;
    s32 temp_f16;
    f32 playSpeed;
    Player* player = GET_PLAYER(play);
    s32 absPlaySpeed;
    s16 yawDiff;

    if (!EnTest_ReactToProjectile(play, this)) {
        this->timer++;

        if (Actor_OtherIsTargeted(play, &this->actor)) {
            checkDist = 150.0f;
        }

        if (this->actor.xzDistToPlayer <= (80.0f + checkDist)) {
            Math_SmoothStepToF(&this->actor.speedXZ, -5.0f, 1.0f, 0.8f, 0.0f);
        } else if (this->actor.xzDistToPlayer > (110.0f + checkDist)) {
            Math_SmoothStepToF(&this->actor.speedXZ, 5.0f, 1.0f, 0.8f, 0.0f);
        }

        if (this->actor.speedXZ >= 5.0f) {
            this->actor.speedXZ = 5.0f;
        } else if (this->actor.speedXZ < -5.0f) {
            this->actor.speedXZ = -5.0f;
        }

        if ((this->actor.params == STALFOS_TYPE_CEILING) &&
            !Actor_TestFloorInDirection(&this->actor, play, this->actor.speedXZ, this->actor.world.rot.y)) {
            this->actor.speedXZ *= -1.0f;
        }

        if (ABS(this->actor.speedXZ) < 3.0f) {
            Animation_Change(&this->skelAnime, &gStalfosSlowAdvanceAnim, 0.0f, this->skelAnime.curFrame,
                             Animation_GetLastFrame(&gStalfosSlowAdvanceAnim), 0, -6.0f);
            playSpeed = this->actor.speedXZ * 10.0f;
        } else {
            Animation_Change(&this->skelAnime, &gStalfosFastAdvanceAnim, 0.0f, this->skelAnime.curFrame,
                             Animation_GetLastFrame(&gStalfosFastAdvanceAnim), 0, -4.0f);
            playSpeed = this->actor.speedXZ * 10.0f * 0.02f;
        }

        if (this->actor.speedXZ >= 0.0f) {
            if (this->unk_7DE == 0) {
                this->unk_7DE++;
            }

            playSpeed = CLAMP_MAX(playSpeed, 2.5f);
            this->skelAnime.playSpeed = playSpeed;
        } else {
            playSpeed = CLAMP_MIN(playSpeed, -2.5f);
            this->skelAnime.playSpeed = playSpeed;
        }

        yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

        if ((this->actor.xzDistToPlayer < 100.0f) && (player->swordState != 0)) {
            if (ABS(yawDiff) >= 0x1F40) {
                this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

                if ((Rand_ZeroOne() > 0.7f) && (player->meleeWeaponAnimation != 0x11)) {
                    EnTest_SetupJumpBack(this);
                } else {
                    EnTest_SetupStopAndBlock(this);
                }

                return;
            }
        }

        prevFrame = this->skelAnime.curFrame;
        SkelAnime_Update(&this->skelAnime);

        temp_f16 = this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed);
        absPlaySpeed = (f32)ABS(this->skelAnime.playSpeed);

        if ((s32)this->skelAnime.curFrame != prevFrame) {
            s32 temp_v0_2 = absPlaySpeed + prevFrame;

            if (((temp_v0_2 > 1) && (temp_f16 <= 0)) || ((temp_f16 < 7) && (temp_v0_2 >= 8))) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if ((this->timer % 32) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WARAU);
            this->timer += (s16)(Rand_ZeroOne() * 5.0f);
        }

        if ((this->actor.xzDistToPlayer < 220.0f) && (this->actor.xzDistToPlayer > 160.0f) &&
            (Actor_IsFacingPlayer(&this->actor, 0x71C))) {
            if (Actor_IsTargeted(play, &this->actor)) {
                if (Rand_ZeroOne() < 0.1f) {
                    EnTest_SetupJumpslash(this);
                    return;
                }
            } else if (player->heldItemAction != PLAYER_IA_NONE) {
                if (this->actor.isTargeted) {
                    if ((play->gameplayFrames % 2) != 0) {
                        func_808627C4(this, play);
                        return;
                    }

                    EnTest_ChooseAction(this, play);
                } else {
                    func_80860EC0(this);
                }
            }
        }

        if (Rand_ZeroOne() < 0.4f) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        }

        if (!Actor_IsFacingPlayer(&this->actor, 0x11C7)) {
            EnTest_SetupIdle(this);
            this->timer = (Rand_ZeroOne() * 10.0f) + 10.0f;
            return;
        }

        if (this->actor.xzDistToPlayer < 110.0f) {
            if (Rand_ZeroOne() > 0.2f) {
                if (player->stateFlags1 & 0x10) {
                    if (this->actor.isTargeted) {
                        EnTest_SetupSlashDown(this);
                    } else {
                        func_808627C4(this, play);
                    }
                } else {
                    EnTest_SetupSlashDown(this);
                }
            } else {
                EnTest_SetupStopAndBlock(this);
            }
        } else if (Rand_ZeroOne() < 0.1f) {
            this->actor.speedXZ = 5.0f;
        }
    }
}

// a variation of sidestep
void func_80860BDC(EnTest* this) {
    Animation_PlayLoop(&this->skelAnime, &gStalfosSidestepAnim);
    this->unk_7C8 = 0xE;
    EnTest_SetupAction(this, func_80860C24);
}

// a variation of sidestep
void func_80860C24(EnTest* this, PlayState* play) {
    s16 yawDiff;
    s16 yawChange;
    f32 playSpeed;
    s32 prevFrame;
    s32 temp1;
    s32 temp2;
    s32 absPlaySpeed;

    if (!EnTest_ReactToProjectile(play, this)) {
        yawDiff = this->actor.yawTowardsPlayer;
        yawDiff -= this->actor.shape.rot.y;

        if (yawDiff > 0) {
            yawChange = (yawDiff / 42.0f) + 300.0f;
            this->actor.shape.rot.y += yawChange * 2;
        } else {
            yawChange = (yawDiff / 42.0f) - 300.0f;
            this->actor.shape.rot.y += yawChange * 2;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y;

        if (yawDiff > 0) {
            playSpeed = yawChange * 0.02f;
            playSpeed = CLAMP_MAX(playSpeed, 1.0f);
            this->skelAnime.playSpeed = playSpeed;
        } else {
            playSpeed = yawChange * 0.02f;
            playSpeed = CLAMP_MIN(playSpeed, -1.0f);
            this->skelAnime.playSpeed = playSpeed;
        }

        prevFrame = this->skelAnime.curFrame;
        SkelAnime_Update(&this->skelAnime);
        temp1 = this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed);
        absPlaySpeed = (f32)ABS(this->skelAnime.playSpeed);

        if ((s32)this->skelAnime.curFrame != prevFrame) {
            temp2 = absPlaySpeed + prevFrame;

            if (((temp2 > 2) && (temp1 <= 0)) || ((temp1 < 7) && (temp2 >= 9))) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if (Actor_IsFacingPlayer(&this->actor, 0x71C)) {
            if (Rand_ZeroOne() > 0.8f) {
                if ((Rand_ZeroOne() > 0.7f)) {
                    func_80860EC0(this);
                } else {
                    EnTest_ChooseAction(this, play);
                }
            } else {
                EnTest_SetupWalkAndBlock(this);
            }
        }
    }
}

// a variation of sidestep
void func_80860EC0(EnTest* this) {
    Animation_PlayLoop(&this->skelAnime, &gStalfosSidestepAnim);
    this->unk_7C8 = 0xF;
    this->actor.speedXZ = (Rand_ZeroOne() > 0.5f) ? -0.5f : 0.5f;
    this->timer = (s16)((Rand_ZeroOne() * 15.0f) + 25.0f);
    this->unk_7EC = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnTest_SetupAction(this, func_80860F84);
}

// a variation of sidestep
void func_80860F84(EnTest* this, PlayState* play) {
    s16 playerYaw180;
    s32 pad;
    s32 prevFrame;
    s32 temp_f16;
    s16 yawDiff;
    Player* player = GET_PLAYER(play);
    f32 checkDist = 0.0f;
    s16 newYaw;
    s32 absPlaySpeed;

    if (!EnTest_ReactToProjectile(play, this)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3E80;
        playerYaw180 = player->actor.shape.rot.y + 0x8000;

        if (this->actor.speedXZ >= 0.0f) {
            if (this->actor.speedXZ < 6.0f) {
                this->actor.speedXZ += 0.5f;
            } else {
                this->actor.speedXZ = 6.0f;
            }
        } else {
            if (this->actor.speedXZ > -6.0f) {
                this->actor.speedXZ -= 0.5f;
            } else {
                this->actor.speedXZ = -6.0f;
            }
        }

        if ((this->actor.bgCheckFlags & 8) ||
            ((this->actor.params == STALFOS_TYPE_CEILING) &&
             !Actor_TestFloorInDirection(&this->actor, play, this->actor.speedXZ, this->actor.world.rot.y))) {
            if (this->actor.bgCheckFlags & 8) {
                if (this->actor.speedXZ >= 0.0f) {
                    newYaw = this->actor.shape.rot.y + 0x3FFF;
                } else {
                    newYaw = this->actor.shape.rot.y - 0x3FFF;
                }

                newYaw = this->actor.wallYaw - newYaw;
            } else {
                this->actor.speedXZ *= -0.8f;
                newYaw = 0;
            }

            if (ABS(newYaw) > 0x4000) {
                this->actor.speedXZ *= -0.8f;

                if (this->actor.speedXZ < 0.0f) {
                    this->actor.speedXZ -= 0.5f;
                } else {
                    this->actor.speedXZ += 0.5f;
                }
            }
        }

        if (Actor_OtherIsTargeted(play, &this->actor)) {
            checkDist = 200.0f;
        }

        if (this->actor.xzDistToPlayer <= (80.0f + checkDist)) {
            Math_SmoothStepToF(&this->unk_7EC, -2.5f, 1.0f, 0.8f, 0.0f);
        } else if (this->actor.xzDistToPlayer > (110.0f + checkDist)) {
            Math_SmoothStepToF(&this->unk_7EC, 2.5f, 1.0f, 0.8f, 0.0f);
        } else {
            Math_SmoothStepToF(&this->unk_7EC, 0.0f, 1.0f, 6.65f, 0.0f);
        }

        if (this->unk_7EC != 0.0f) {
            this->actor.world.pos.x += Math_SinS(this->actor.shape.rot.y) * this->unk_7EC;
            this->actor.world.pos.z += Math_CosS(this->actor.shape.rot.y) * this->unk_7EC;
        }

        this->skelAnime.playSpeed = this->actor.speedXZ * 0.5f;

        prevFrame = this->skelAnime.curFrame;
        SkelAnime_Update(&this->skelAnime);
        temp_f16 = this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed);
        absPlaySpeed = (f32)ABS(this->skelAnime.playSpeed);

        if ((s32)this->skelAnime.curFrame != prevFrame) {
            s32 temp_v0_2 = absPlaySpeed + prevFrame;

            if (((temp_v0_2 > 1) && (temp_f16 <= 0)) || ((temp_f16 < 7) && (temp_v0_2 >= 8))) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if ((play->gameplayFrames & 95) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WARAU);
        }

        yawDiff = playerYaw180 - this->actor.shape.rot.y;
        yawDiff = ABS(yawDiff);

        if ((yawDiff > 0x6800) || (this->timer == 0)) {
            EnTest_ChooseAction(this, play);
        } else if (this->timer != 0) {
            this->timer--;
        }
    }
}

void EnTest_SetupSlashDown(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosDownSlashAnim);
    Audio_StopSfxByPosAndId(&this->actor.projectedPos, NA_SE_EN_STAL_WARAU);
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->unk_7C8 = 0x10;
    this->actor.speedXZ = 0.0f;
    EnTest_SetupAction(this, EnTest_SlashDown);
    this->swordCollider.info.toucher.damage = 16;

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }
}

void EnTest_SlashDown(EnTest* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;

    if ((s32)this->skelAnime.curFrame < 4) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    }

    if ((s32)this->skelAnime.curFrame == 7) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_SAKEBI);
    }

    if ((this->skelAnime.curFrame > 7.0f) && (this->skelAnime.curFrame < 11.0f)) {
        this->swordState = 1;
    } else {
        this->swordState = 0;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if ((play->gameplayFrames % 2) != 0) {
            EnTest_SetupSlashDownEnd(this);
        } else {
            EnTest_SetupSlashUp(this);
        }
    }
}

void EnTest_SetupSlashDownEnd(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosRecoverFromDownSlashAnim);
    this->unk_7C8 = 0x12;
    this->actor.speedXZ = 0.0f;
    EnTest_SetupAction(this, EnTest_SlashDownEnd);
}

void EnTest_SlashDownEnd(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->swordCollider.base.atFlags & AT_HIT) {
            this->swordCollider.base.atFlags &= ~AT_HIT;
            if (this->actor.params != STALFOS_TYPE_CEILING) {
                EnTest_SetupJumpBack(this);
                return;
            }
        }

        if (Rand_ZeroOne() > 0.7f) {
            EnTest_SetupIdle(this);
            this->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
            return;
        }

        this->actor.world.rot.y = Actor_WorldYawTowardActor(&this->actor, &player->actor);

        if (Rand_ZeroOne() > 0.7f) {
            if (this->actor.params != STALFOS_TYPE_CEILING) {
                EnTest_SetupJumpBack(this);
                return;
            }
        }

        yawDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;

        if (ABS(yawDiff) <= 0x2710) {
            yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if ((ABS(yawDiff) > 0x3E80) && (this->actor.params != STALFOS_TYPE_CEILING)) {
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                EnTest_SetupJumpBack(this);
            } else if (player->stateFlags1 & 0x10) {
                if (this->actor.isTargeted) {
                    EnTest_SetupSlashDown(this);
                } else if ((play->gameplayFrames % 2) != 0) {
                    func_808627C4(this, play);
                } else {
                    EnTest_SetupJumpBack(this);
                }
            } else {
                EnTest_SetupSlashDown(this);
            }
        } else {
            func_808627C4(this, play);
        }
    }
}

void EnTest_SetupSlashUp(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosUpSlashAnim);
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->unk_7C8 = 0x11;
    this->swordCollider.info.toucher.damage = 16;
    this->actor.speedXZ = 0.0f;
    EnTest_SetupAction(this, EnTest_SlashUp);

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }
}

void EnTest_SlashUp(EnTest* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;

    if ((s32)this->skelAnime.curFrame == 2) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_SAKEBI);
    }

    if ((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 8.0f)) {
        this->swordState = 1;
    } else {
        this->swordState = 0;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        EnTest_SetupSlashDown(this);
    }
}

void EnTest_SetupJumpBack(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosJumpBackwardsAnim);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
    this->unk_7C8 = 0x14;
    this->timer = 5;
    EnTest_SetupAction(this, EnTest_JumpBack);

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }

    if (this->actor.params != STALFOS_TYPE_CEILING) {
        this->actor.speedXZ = -11.0f;
    } else {
        this->actor.speedXZ = -7.0f;
    }
}

void EnTest_JumpBack(EnTest* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 1);

    if (this->timer == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WARAU);
    } else {
        this->timer--;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (!EnTest_ReactToProjectile(play, this)) {
            if (this->actor.xzDistToPlayer <= 100.0f) {
                if (Actor_IsFacingPlayer(&this->actor, 0x1555)) {
                    EnTest_SetupSlashDown(this);
                } else {
                    EnTest_SetupIdle(this);
                    this->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
                }
            } else {
                if ((this->actor.xzDistToPlayer <= 220.0f) && Actor_IsFacingPlayer(&this->actor, 0xE38)) {
                    EnTest_SetupJumpslash(this);
                } else {
                    EnTest_SetupIdle(this);
                    this->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
                }
            }
            this->actor.flags |= ACTOR_FLAG_0;
        }
    } else if (this->skelAnime.curFrame == (this->skelAnime.endFrame - 4.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
    }
}

void EnTest_SetupJumpslash(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosJumpAnim);
    Audio_StopSfxByPosAndId(&this->actor.projectedPos, NA_SE_EN_STAL_WARAU);
    this->timer = 0;
    this->unk_7C8 = 0x17;
    this->actor.velocity.y = 10.0f;
    this->actor.speedXZ = 8.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    EnTest_SetupAction(this, EnTest_Jumpslash);
    this->swordCollider.info.toucher.damage = 32;

    if (this->unk_7DE != 0) {
        this->unk_7DE = 3;
    }
}

void EnTest_Jumpslash(EnTest* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer == 0) {
            Animation_PlayOnce(&this->skelAnime, &gStalfosJumpslashAnim);
            this->timer = 1;
            this->swordState = 1;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_SAKEBI);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
        } else {
            this->actor.speedXZ = 0.0f;
            EnTest_SetupIdle(this);
        }
    }

    if ((this->timer != 0) && (this->skelAnime.curFrame >= 5.0f)) {
        this->swordState = 0;
    }

    if (this->actor.world.pos.y <= this->actor.floorHeight) {
        if (this->actor.speedXZ != 0.0f) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        }

        this->actor.world.pos.y = this->actor.floorHeight;
        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
    }
}

void EnTest_SetupJumpUp(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosJumpAnim);
    this->timer = 0;
    this->unk_7C8 = 4;
    this->actor.velocity.y = 14.0f;
    this->actor.speedXZ = 6.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnTest_SetupAction(this, EnTest_JumpUp);
}

void EnTest_JumpUp(EnTest* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.world.pos.y <= this->actor.floorHeight) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.world.pos.y = this->actor.floorHeight;
        this->unk_7E4 = -(s32)this->actor.velocity.y;

        if (this->unk_7E4 == 0) {
            this->unk_7E4 = 1;
        }

        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
        this->unk_7C8 = 0xC;
        this->timer = 4;
        Animation_Change(&this->skelAnime, &gStalfosLandFromLeapAnim, 0.0f, 0.0f, 0.0f, 2, 0.0f);
        EnTest_SetupAction(this, EnTest_Land);
    }
}

void EnTest_SetupStopAndBlock(EnTest* this) {
    Animation_Change(&this->skelAnime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                     Animation_GetLastFrame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
    this->unk_7C8 = 0x15;
    this->actor.speedXZ = 0.0f;
    this->timer = (Rand_ZeroOne() * 10.0f) + 11.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_7DE = 5;
    EnTest_SetupAction(this, EnTest_StopAndBlock);
}

void EnTest_StopAndBlock(EnTest* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    SkelAnime_Update(&this->skelAnime);

    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) > 0x3E80) &&
        (this->actor.params != STALFOS_TYPE_CEILING) && ((play->gameplayFrames % 2) != 0)) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        EnTest_SetupJumpBack(this);
    }

    if (this->timer == 0) {
        EnTest_SetupIdleFromBlock(this);
    } else {
        this->timer--;
    }
}

void EnTest_SetupIdleFromBlock(EnTest* this) {
    Animation_MorphToLoop(&this->skelAnime, &gStalfosMiddleGuardAnim, -4.0f);
    this->unk_7C8 = 0x16;
    EnTest_SetupAction(this, EnTest_IdleFromBlock);
}

void EnTest_IdleFromBlock(EnTest* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 1.5f, 0.0f);
    SkelAnime_Update(&this->skelAnime);

    if (this->skelAnime.morphWeight == 0.0f) {
        this->actor.speedXZ = 0.0f;
        this->unk_7DE = 0;

        if (!EnTest_ReactToProjectile(play, this)) {
            if (this->actor.xzDistToPlayer < 500.0f) {
                EnTest_ChooseAction(this, play);
            } else {
                func_808627C4(this, play);
            }
        }
    }
}

void func_80862154(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosFlinchFromHitFrontAnim);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_DAMAGE);
    this->unk_7C8 = 8;
    this->actor.speedXZ = -2.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
    EnTest_SetupAction(this, func_808621D4);
}

void func_808621D4(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.1f, 0.0f);

    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.speedXZ = 0.0f;

        if ((this->actor.bgCheckFlags & 8) && ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
                                               (this->actor.xzDistToPlayer < 80.0f))) {
            EnTest_SetupJumpUp(this);
        } else if (!EnTest_ReactToProjectile(play, this)) {
            EnTest_ChooseAction(this, play);
        } else {
            return;
        }
    }

    if (player->swordState != 0) {
        if ((this->actor.bgCheckFlags & 8) && ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
                                               (this->actor.xzDistToPlayer < 80.0f))) {
            EnTest_SetupJumpUp(this);
        } else if ((Rand_ZeroOne() > 0.7f) && (this->actor.params != STALFOS_TYPE_CEILING) &&
                   (player->meleeWeaponAnimation != 0x11)) {
            EnTest_SetupJumpBack(this);
        } else {
            EnTest_SetupStopAndBlock(this);
        }

        this->unk_7C8 = 8;
    }
}

void func_80862398(EnTest* this) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosFlinchFromHitBehindAnim);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_DAMAGE);
    this->unk_7C8 = 9;
    this->actor.speedXZ = -2.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
    EnTest_SetupAction(this, func_80862418);
}

void func_80862418(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.1f, 0.0f);

    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.speedXZ = 0.0f;

        if (!EnTest_ReactToProjectile(play, this)) {
            EnTest_ChooseAction(this, play);
        } else {
            return;
        }
    }

    if (player->swordState != 0) {
        if ((this->actor.bgCheckFlags & 8) && ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
                                               (this->actor.xzDistToPlayer < 80.0f))) {
            EnTest_SetupJumpUp(this);
        } else if ((Rand_ZeroOne() > 0.7f) && (this->actor.params != STALFOS_TYPE_CEILING) &&
                   (player->meleeWeaponAnimation != 0x11)) {
            EnTest_SetupJumpBack(this);
        } else {
            EnTest_SetupStopAndBlock(this);
        }

        this->unk_7C8 = 8;
    }
}

void EnTest_SetupStunned(EnTest* this) {
    this->unk_7C8 = 0xB;
    this->unk_7DE = 0;
    this->swordState = 0;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.speedXZ = -4.0f;

    if (this->lastDamageEffect == STALFOS_DMGEFF_LIGHT) {
        Actor_SetColorFilter(&this->actor, -0x8000, 0x78, 0, 0x50);
    } else {
        Actor_SetColorFilter(&this->actor, 0, 0x78, 0, 0x50);

        if (this->lastDamageEffect == STALFOS_DMGEFF_FREEZE) {
            this->iceTimer = 36;
        } else {
            Animation_PlayOnceSetSpeed(&this->skelAnime, &gStalfosFlinchFromHitFrontAnim, 0.0f);
        }
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnTest_SetupAction(this, EnTest_Stunned);
}

void EnTest_Stunned(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);

    if (this->actor.colorFilterTimer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            func_80862FA8(this, play);
        } else if (player->swordState != 0) {
            if ((this->actor.bgCheckFlags & 8) &&
                ((ABS((s16)(this->actor.wallYaw - this->actor.shape.rot.y)) < 0x38A4) &&
                 (this->actor.xzDistToPlayer < 80.0f))) {
                EnTest_SetupJumpUp(this);
            } else if ((Rand_ZeroOne() > 0.7f) && (player->meleeWeaponAnimation != 0x11)) {
                EnTest_SetupJumpBack(this);
            } else {
                EnTest_SetupStopAndBlock(this);
            }

            this->unk_7C8 = 8;
        } else {
            this->actor.speedXZ = 0.0f;
            if (!EnTest_ReactToProjectile(play, this)) {
                EnTest_ChooseAction(this, play);
            }
        }
    }
}

// a variation of sidestep
void func_808627C4(EnTest* this, PlayState* play) {
    if (Actor_OtherIsTargeted(play, &this->actor)) {
        func_80860EC0(this);
        return;
    }

    Animation_MorphToLoop(&this->skelAnime, &gStalfosSidestepAnim, -2.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    this->actor.speedXZ = ((play->gameplayFrames % 2) != 0) ? -4.0f : 4.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
    this->timer = (Rand_ZeroOne() * 20.0f) + 20.0f;
    this->unk_7C8 = 0x18;
    EnTest_SetupAction(this, func_808628C8);
    this->unk_7EC = 0.0f;
}

// a variation of sidestep
void func_808628C8(EnTest* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad1;
    s32 prevFrame;
    s32 temp_f16;
    s32 pad2;
    f32 checkDist = 0.0f;
    s16 newYaw;
    f32 absPlaySpeed;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 1);

    if (this->unk_7DE == 0) {
        this->unk_7DE++;
    }

    if (this->actor.speedXZ >= 0.0f) {
        if (this->actor.speedXZ < 6.0f) {
            this->actor.speedXZ += 0.125f;
        } else {
            this->actor.speedXZ = 6.0f;
        }
    } else {
        if (this->actor.speedXZ > -6.0f) {
            this->actor.speedXZ -= 0.125f;
        } else {
            this->actor.speedXZ = -6.0f;
        }
    }

    if ((this->actor.bgCheckFlags & 8) ||
        ((this->actor.params == STALFOS_TYPE_CEILING) &&
         !Actor_TestFloorInDirection(&this->actor, play, this->actor.speedXZ, this->actor.shape.rot.y + 0x3FFF))) {
        if (this->actor.bgCheckFlags & 8) {
            if (this->actor.speedXZ >= 0.0f) {
                newYaw = (this->actor.shape.rot.y + 0x3FFF);
            } else {
                newYaw = (this->actor.shape.rot.y - 0x3FFF);
            }

            newYaw = this->actor.wallYaw - newYaw;
        } else {
            this->actor.speedXZ *= -0.8f;
            newYaw = 0;
        }

        if (ABS(newYaw) > 0x4000) {
            this->actor.speedXZ *= -0.8f;

            if (this->actor.speedXZ < 0.0f) {
                this->actor.speedXZ -= 0.5f;
            } else {
                this->actor.speedXZ += 0.5f;
            }
        }
    }

    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;

    if (Actor_OtherIsTargeted(play, &this->actor)) {
        checkDist = 200.0f;
    }

    if (this->actor.xzDistToPlayer <= (80.0f + checkDist)) {
        Math_SmoothStepToF(&this->unk_7EC, -2.5f, 1.0f, 0.8f, 0.0f);
    } else if (this->actor.xzDistToPlayer > (110.0f + checkDist)) {
        Math_SmoothStepToF(&this->unk_7EC, 2.5f, 1.0f, 0.8f, 0.0f);
    } else {
        Math_SmoothStepToF(&this->unk_7EC, 0.0f, 1.0f, 6.65f, 0.0f);
    }

    if (this->unk_7EC != 0.0f) {
        this->actor.world.pos.x += (Math_SinS(this->actor.shape.rot.y) * this->unk_7EC);
        this->actor.world.pos.z += (Math_CosS(this->actor.shape.rot.y) * this->unk_7EC);
    }

    this->skelAnime.playSpeed = this->actor.speedXZ * 0.5f;
    prevFrame = this->skelAnime.curFrame;
    SkelAnime_Update(&this->skelAnime);

    temp_f16 = this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed);
    absPlaySpeed = ABS(this->skelAnime.playSpeed);

    if ((this->timer % 32) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WARAU);
    }
    if ((s32)this->skelAnime.curFrame != prevFrame) {
        s32 temp_v0_2 = (s32)absPlaySpeed + prevFrame;

        if (((temp_v0_2 > 1) && (temp_f16 <= 0)) || ((temp_f16 < 7) && (temp_v0_2 >= 8))) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_WALK);
        }
    }

    if (this->timer == 0) {
        if (Actor_OtherIsTargeted(play, &this->actor)) {
            EnTest_SetupIdle(this);
        } else if (Actor_IsTargeted(play, &this->actor)) {
            if (!EnTest_ReactToProjectile(play, this)) {
                EnTest_ChooseAction(this, play);
            }
        } else if (player->heldItemAction != PLAYER_IA_NONE) {
            if ((play->gameplayFrames % 2) != 0) {
                EnTest_SetupIdle(this);
            } else {
                EnTest_SetupWalkAndBlock(this);
            }
        } else {
            EnTest_SetupWalkAndBlock(this);
        }

    } else {
        this->timer--;
    }
}

void func_80862DBC(EnTest* this, PlayState* play) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_DAMAGE);
    this->unk_7C8 = 2;
    BodyBreak_Alloc(&this->bodyBreak, 60, play);
    this->actor.home.rot.x = 0;

    if (this->swordState >= 0) {
        EffectBlure_AddSpace(Effect_GetByIndex(this->effectIndex));
        this->swordState = -1;
    }

    this->actor.flags &= ~ACTOR_FLAG_0;

    if (this->actor.params == STALFOS_TYPE_5) {
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    }

    EnTest_SetupAction(this, func_80862E6C);
}

void func_80862E6C(EnTest* this, PlayState* play) {
    if (this->actor.child == NULL) {
        if (this->actor.home.rot.x == 0) {
            this->actor.home.rot.x = this->bodyBreak.count;
        }

        if (BodyBreak_SpawnParts(&this->actor, &this->bodyBreak, play, this->actor.params + 8)) {
            this->actor.child = &this->actor;
        }
    } else {
        if (this->actor.home.rot.x == 0) {
            this->actor.colChkInfo.health = 10;

            if (this->actor.params == STALFOS_TYPE_4) {
                this->actor.params = -1;
            } else {
                Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            }

            this->actor.child = NULL;
            this->actor.flags |= ACTOR_FLAG_0;
            EnTest_SetupJumpBack(this);
        } else if ((this->actor.params == STALFOS_TYPE_5) &&
                   !Actor_FindNearby(play, &this->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f)) {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xD0);

            if (this->actor.parent != NULL) {
                this->actor.parent->home.rot.z--;
            }

            Actor_Kill(&this->actor);
            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_STALFOS]++;
        }
    }
}

void func_80862FA8(EnTest* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosFallOverBackwardsAnim);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_DEAD);
    this->unk_7DE = 0;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.colorFilterTimer = 0;
    this->actor.speedXZ = 0.0f;

    if (this->actor.params <= STALFOS_TYPE_CEILING) {
        this->unk_7C8 = 5;
        EnTest_SetupAction(this, func_80863044);
    } else {
        func_80862DBC(this, play);
    }
}

void func_80863044(EnTest* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->timer = (Rand_ZeroOne() * 10.0f) + 10.0f;
        this->unk_7C8 = 7;
        EnTest_SetupAction(this, func_808633E8);
        BodyBreak_Alloc(&this->bodyBreak, 60, play);
    }

    if ((s32)this->skelAnime.curFrame == 15) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void func_808630F0(EnTest* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gStalfosFallOverForwardsAnim);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_DEAD);
    this->unk_7C8 = 6;
    this->actor.colorFilterTimer = 0;
    this->unk_7DE = 0;
    this->actor.speedXZ = 0.0f;

    if (this->actor.params <= STALFOS_TYPE_CEILING) {
        this->actor.flags &= ~ACTOR_FLAG_0;
        EnTest_SetupAction(this, func_8086318C);
    } else {
        func_80862DBC(this, play);
    }
}

void func_8086318C(EnTest* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->timer = (Rand_ZeroOne() * 10.0f) + 10.0f;
        this->unk_7C8 = 7;
        EnTest_SetupAction(this, func_808633E8);
        BodyBreak_Alloc(&this->bodyBreak, 60, play);
    }

    if (((s32)this->skelAnime.curFrame == 10) || ((s32)this->skelAnime.curFrame == 25)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void EnTest_SetupRecoil(EnTest* this) {
    this->swordState = 0;
    this->skelAnime.moveFlags = 2;
    this->unk_7C8 = 0x13;
    this->skelAnime.playSpeed = -1.0f;
    this->skelAnime.startFrame = this->skelAnime.curFrame;
    this->skelAnime.endFrame = 0.0f;
    EnTest_SetupAction(this, EnTest_Recoil);
}

void EnTest_Recoil(EnTest* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (Rand_ZeroOne() > 0.7f) {
            EnTest_SetupIdle(this);
            this->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
        } else if (((play->gameplayFrames % 2) != 0) && (this->actor.params != STALFOS_TYPE_CEILING)) {
            EnTest_SetupJumpBack(this);
        } else {
            func_808627C4(this, play);
        }
    }
}

void EnTest_Rise(EnTest* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.scale.y < 0.015f) {
        this->actor.scale.y += 0.002f;
        this->actor.world.pos.y = this->actor.home.pos.y - 3.5f;
    } else {
        this->actor.world.pos.y = this->actor.home.pos.y;
        EnTest_SetupJumpBack(this);
    }
}

void func_808633E8(EnTest* this, PlayState* play) {
    this->actor.params = STALFOS_TYPE_1;

    if (BodyBreak_SpawnParts(&this->actor, &this->bodyBreak, play, this->actor.params)) {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xD0);

        if (this->actor.parent != NULL) {
            this->actor.parent->home.rot.z--;
        }

        Actor_Kill(&this->actor);
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_STALFOS]++;
    }
}

void EnTest_UpdateHeadRot(EnTest* this, PlayState* play) {
    s16 lookAngle = this->actor.yawTowardsPlayer;

    lookAngle -= (s16)(this->headRot.y + this->actor.shape.rot.y);

    this->headRotOffset.y = CLAMP(lookAngle, -0x7D0, 0x7D0);
    this->headRot.y += this->headRotOffset.y;
    this->headRot.y = CLAMP(this->headRot.y, -0x382F, 0x382F);
}

void EnTest_UpdateDamage(EnTest* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->shieldCollider.base.acFlags & AC_BOUNCED) {
        this->shieldCollider.base.acFlags &= ~AC_BOUNCED;
        this->bodyCollider.base.acFlags &= ~AC_HIT;

        if (this->unk_7C8 >= 0xA) {
            this->actor.speedXZ = -4.0f;
        }
    } else if (this->bodyCollider.base.acFlags & AC_HIT) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;

        if ((this->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_SLING) &&
            (this->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_FIREMAGIC)) {
            this->lastDamageEffect = this->actor.colChkInfo.damageEffect;
            if (this->swordState >= 1) {
                this->swordState = 0;
            }
            this->unk_7DC = player->unk_845;
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            Actor_SetDropFlag(&this->actor, &this->bodyCollider.info, false);
            Audio_StopSfxByPosAndId(&this->actor.projectedPos, NA_SE_EN_STAL_WARAU);

            if ((this->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_STUN) ||
                (this->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_FREEZE) ||
                (this->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_LIGHT)) {
                if (this->unk_7C8 != 0xB) {
                    Actor_ApplyDamage(&this->actor);
                    EnTest_SetupStunned(this);
                }
            } else {
                if (Actor_IsFacingPlayer(&this->actor, 0x4000)) {
                    if (Actor_ApplyDamage(&this->actor) == 0) {
                        Enemy_StartFinishingBlow(play, &this->actor);
                        func_80862FA8(this, play);
                    } else {
                        func_80862154(this);
                    }
                } else if (Actor_ApplyDamage(&this->actor) == 0) {
                    func_808630F0(this, play);
                    Enemy_StartFinishingBlow(play, &this->actor);
                } else {
                    func_80862398(this);
                }
            }
        }
    }
}

void EnTest_Update(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;
    f32 oldWeight;
    u32 floorProperty;
    s32 pad;

    EnTest_UpdateDamage(this, play);

    if (this->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_FIREMAGIC) {
        Actor_MoveForward(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, 0x1D);

        if (this->actor.params == STALFOS_TYPE_1) {
            if (this->actor.world.pos.y <= this->actor.home.pos.y) {
                this->actor.world.pos.y = this->actor.home.pos.y;
                this->actor.velocity.y = 0.0f;
            }

            if (this->actor.floorHeight <= this->actor.home.pos.y) {
                this->actor.floorHeight = this->actor.home.pos.y;
            }
        } else if (this->actor.bgCheckFlags & 2) {
            floorProperty = func_80041EA4(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

            if ((floorProperty == 5) || (floorProperty == 0xC) ||
                func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) == 9) {
                Actor_Kill(&this->actor);
                gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_STALFOS]++;
                return;
            }
        }

        this->actionFunc(this, play);

        switch (this->unk_7DE) {
            case 0:
                break;

            case 1:
                Animation_Change(&this->upperSkelanime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                                 Animation_GetLastFrame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
                AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                             this->upperSkelanime.jointTable, sJointCopyFlags);
                this->unk_7DE++;
                break;

            case 2:
                SkelAnime_Update(&this->upperSkelanime);
                SkelAnime_CopyFrameTableTrue(&this->skelAnime, this->skelAnime.jointTable,
                                             this->upperSkelanime.jointTable, sJointCopyFlags);
                break;

            case 3:
                this->unk_7DE++;
                this->upperSkelanime.morphWeight = 4.0f;
                // fallthrough
            case 4:
                oldWeight = this->upperSkelanime.morphWeight;
                this->upperSkelanime.morphWeight -= 1.0f;

                if (this->upperSkelanime.morphWeight <= 0.0f) {
                    this->unk_7DE = 0;
                }

                SkelAnime_InterpFrameTable(this->skelAnime.limbCount, this->upperSkelanime.jointTable,
                                           this->upperSkelanime.jointTable, this->skelAnime.jointTable,
                                           1.0f - (this->upperSkelanime.morphWeight / oldWeight));
                SkelAnime_CopyFrameTableTrue(&this->skelAnime, this->skelAnime.jointTable,
                                             this->upperSkelanime.jointTable, sJointCopyFlags);
                break;
        }

        if ((this->actor.colorFilterTimer == 0) && (this->actor.colChkInfo.health != 0)) {
            if ((this->unk_7C8 != 0x10) && (this->unk_7C8 != 0x17)) {
                EnTest_UpdateHeadRot(this, play);
            } else {
                Math_SmoothStepToS(&this->headRot.y, 0, 1, 0x3E8, 0);
            }
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->bodyCollider);

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 45.0f;

    if ((this->actor.colChkInfo.health > 0) || (this->actor.colorFilterTimer != 0)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);

        if ((this->unk_7C8 >= 0xA) &&
            ((this->actor.colorFilterTimer == 0) || (!(this->actor.colorFilterParams & 0x4000)))) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if (this->unk_7DE != 0) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->shieldCollider.base);
        }
    }

    if (this->swordState >= 1) {
        if (!(this->swordCollider.base.atFlags & AT_BOUNCED)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->swordCollider.base);
        } else {
            this->swordCollider.base.atFlags &= ~AT_BOUNCED;
            EnTest_SetupRecoil(this);
        }
    }

    if (this->actor.params == STALFOS_TYPE_INVISIBLE) {
        if (play->actorCtx.lensActive) {
            this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_7;
            this->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        } else {
            this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_7);
            this->actor.shape.shadowDraw = NULL;
        }
    }
}

s32 EnTest_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTest* this = (EnTest*)thisx;
    s32 pad;

    if (limbIndex == STALFOS_LIMB_HEAD_ROOT) {
        rot->x += this->headRot.y;
        rot->y -= this->headRot.x;
        rot->z += this->headRot.z;
    } else if (limbIndex == STALFOS_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 80 + ABS((s16)(Math_SinS(play->gameplayFrames * 2000) * 175.0f)), 0, 0,
                       255);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if ((this->actor.params == STALFOS_TYPE_INVISIBLE) && !CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_7)) {
        *dList = NULL;
    }

    return false;
}

void EnTest_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f unused1 = { 1100.0f, -700.0f, 0.0f };
    static Vec3f D_80864658 = { 300.0f, 0.0f, 0.0f };
    static Vec3f D_80864664 = { 3400.0f, 0.0f, 0.0f };
    static Vec3f D_80864670 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_8086467C = { 7000.0f, 1000.0f, 0.0f };
    static Vec3f D_80864688 = { 3000.0f, -2000.0f, -1000.0f };
    static Vec3f D_80864694 = { 3000.0f, -2000.0f, 1000.0f };
    static Vec3f D_808646A0 = { -1300.0f, 1100.0f, 0.0f };
    static Vec3f unused2 = { -3000.0f, 1900.0f, 800.0f };
    static Vec3f unused3 = { -3000.0f, -1100.0f, 800.0f };
    static Vec3f unused4 = { 1900.0f, 1900.0f, 800.0f };
    static Vec3f unused5 = { -3000.0f, -1100.0f, 800.0f };
    static Vec3f unused6 = { 1900.0f, -1100.0f, 800.0f };
    static Vec3f unused7 = { 1900.0f, 1900.0f, 800.0f };
    s32 bodyPart = -1;
    Vec3f sp70;
    Vec3f sp64;
    EnTest* this = (EnTest*)thisx;
    s32 pad;
    Vec3f sp50;

    BodyBreak_SetInfo(&this->bodyBreak, limbIndex, 0, 60, 60, dList, BODYBREAK_OBJECT_DEFAULT);

    if (limbIndex == STALFOS_LIMB_SWORD) {
        Matrix_MultVec3f(&D_8086467C, &this->swordCollider.dim.quad[1]);
        Matrix_MultVec3f(&D_80864688, &this->swordCollider.dim.quad[0]);
        Matrix_MultVec3f(&D_80864694, &this->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&D_808646A0, &this->swordCollider.dim.quad[2]);

        Collider_SetQuadVertices(&this->swordCollider, &this->swordCollider.dim.quad[0],
                                 &this->swordCollider.dim.quad[1], &this->swordCollider.dim.quad[2],
                                 &this->swordCollider.dim.quad[3]);

        Matrix_MultVec3f(&D_80864664, &sp70);
        Matrix_MultVec3f(&D_80864670, &sp64);

        if ((this->swordState >= 1) &&
            ((this->actor.params != STALFOS_TYPE_INVISIBLE) || play->actorCtx.lensActive)) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->effectIndex), &sp70, &sp64);
        } else if (this->swordState >= 0) {
            EffectBlure_AddSpace(Effect_GetByIndex(this->effectIndex));
            this->swordState = -1;
        }

    } else if ((limbIndex == STALFOS_LIMB_SHIELD) && (this->unk_7DE != 0)) {
        Matrix_MultVec3f(&D_80864670, &sp64);

        this->shieldCollider.dim.pos.x = sp64.x;
        this->shieldCollider.dim.pos.y = sp64.y;
        this->shieldCollider.dim.pos.z = sp64.z;
    } else {
        Actor_SetFeetPos(&this->actor, limbIndex, STALFOS_LIMB_FOOT_L, &D_80864658, STALFOS_LIMB_ANKLE_R, &D_80864658);

        if ((limbIndex == STALFOS_LIMB_FOOT_L) || (limbIndex == STALFOS_LIMB_ANKLE_R)) {
            if ((this->unk_7C8 == 0x15) || (this->unk_7C8 == 0x16)) {
                if (this->actor.speedXZ != 0.0f) {
                    Matrix_MultVec3f(&D_80864658, &sp64);
                    Actor_SpawnFloorDustRing(play, &this->actor, &sp64, 10.0f, 1, 8.0f, 100, 15, false);
                }
            }
        }
    }

    if (this->iceTimer != 0) {
        switch (limbIndex) {
            case STALFOS_LIMB_HEAD:
                bodyPart = 0;
                break;
            case STALFOS_LIMB_CHEST:
                bodyPart = 1;
                break;
            case STALFOS_LIMB_SWORD:
                bodyPart = 2;
                break;
            case STALFOS_LIMB_SHIELD:
                bodyPart = 3;
                break;
            case STALFOS_LIMB_UPPERARM_R:
                bodyPart = 4;
                break;
            case STALFOS_LIMB_UPPERARM_L:
                bodyPart = 5;
                break;
            case STALFOS_LIMB_WAIST:
                bodyPart = 6;
                break;
            case STALFOS_LIMB_FOOT_L:
                bodyPart = 7;
                break;
            case STALFOS_LIMB_FOOT_R:
                bodyPart = 8;
                break;
        }

        if (bodyPart >= 0) {
            Matrix_MultVec3f(&D_80864670, &sp50);

            this->bodyPartsPos[bodyPart].x = sp50.x;
            this->bodyPartsPos[bodyPart].y = sp50.y;
            this->bodyPartsPos[bodyPart].z = sp50.z;
        }
    }
}

void EnTest_Draw(Actor* thisx, PlayState* play) {
    EnTest* this = (EnTest*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    func_8002EBCC(&this->actor, play, 1);

    if ((thisx->params <= STALFOS_TYPE_CEILING) || (thisx->child == NULL)) {
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnTest_OverrideLimbDraw,
                          EnTest_PostLimbDraw, this);
    }

    if (this->iceTimer != 0) {
        thisx->colorFilterTimer++;
        this->iceTimer--;

        if ((this->iceTimer % 4) == 0) {
            s32 iceIndex = this->iceTimer >> 2;

            EffectSsEnIce_SpawnFlyingVec3s(play, thisx, &this->bodyPartsPos[iceIndex], 150, 150, 150, 250, 235,
                                           245, 255, 1.5f);
        }
    }
}

// a variation of sidestep
void func_80864158(EnTest* this, f32 xzSpeed) {
    Animation_MorphToLoop(&this->skelAnime, &gStalfosSidestepAnim, -2.0f);
    this->actor.speedXZ = xzSpeed;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
    this->timer = (Rand_ZeroOne() * 20.0f) + 15.0f;
    this->unk_7C8 = 0x18;
    EnTest_SetupAction(this, func_808628C8);
}

/**
 * Check if a projectile actor is within 300 units and react accordingly.
 * Returns true if the projectile test passes and a new action is performed.
 */
s32 EnTest_ReactToProjectile(PlayState* play, EnTest* this) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 wallYawDiff;
    s16 touchingWall;
    s16 directionFlag;

    projectileActor = Actor_GetProjectileActor(play, &this->actor, 300.0f);

    if (projectileActor != NULL) {
        yawToProjectile = Actor_WorldYawTowardActor(&this->actor, projectileActor) - (u16)this->actor.shape.rot.y;

        if ((u8)(this->actor.bgCheckFlags & 8)) {
            wallYawDiff = ((u16)this->actor.wallYaw - (u16)this->actor.shape.rot.y);
            touchingWall = true;
        } else {
            touchingWall = false;
        }

        if (Math_Vec3f_DistXYZ(&this->actor.world.pos, &projectileActor->world.pos) < 200.0f) {
            if (Actor_IsTargeted(play, &this->actor) && (projectileActor->id == ACTOR_ARMS_HOOK)) {
                EnTest_SetupJumpUp(this);
            } else if (ABS(yawToProjectile) < 0x2000) {
                EnTest_SetupStopAndBlock(this);
            } else if (ABS(yawToProjectile) < 0x6000) {
                EnTest_SetupJumpBack(this);
            } else {
                EnTest_SetupJumpUp(this);
            }

            return true;
        }

        if (Actor_IsTargeted(play, &this->actor) && (projectileActor->id == ACTOR_ARMS_HOOK)) {
            EnTest_SetupJumpUp(this);
            return true;
        }

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) > 0x6000)) {
            directionFlag = play->gameplayFrames % 2;

            if (touchingWall && (wallYawDiff > 0x2000) && (wallYawDiff < 0x6000)) {
                directionFlag = false;
            } else if (touchingWall && (wallYawDiff < -0x2000) && (wallYawDiff > -0x6000)) {
                directionFlag = true;
            }

            if (directionFlag) {
                func_80864158(this, 4.0f);
            } else {
                func_80864158(this, -4.0f);
            }
        } else if (ABS(yawToProjectile) < 0x6000) {
            directionFlag = play->gameplayFrames % 2;

            if (touchingWall && (ABS(wallYawDiff) > 0x6000)) {
                directionFlag = false;
            } else if (touchingWall && (ABS(wallYawDiff) < 0x2000)) {
                directionFlag = true;
            }

            if (directionFlag) {
                EnTest_SetupJumpBack(this);
            } else {
                EnTest_SetupJumpUp(this);
            }
        }

        return true;
    }

    return false;
}

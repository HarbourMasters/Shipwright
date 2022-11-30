#include "z_en_peehat.h"
#include "objects/object_peehat/object_peehat.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_24)

#define GROUND_HOVER_HEIGHT 75.0f
#define MAX_LARVA 3

void EnPeehat_Init(Actor* thisx, PlayState* play);
void EnPeehat_Destroy(Actor* thisx, PlayState* play);
void EnPeehat_Update(Actor* thisx, PlayState* play);
void EnPeehat_Draw(Actor* thisx, PlayState* play);

void EnPeehat_Ground_SetStateGround(EnPeehat* this);
void EnPeehat_Flying_SetStateGround(EnPeehat* this);
void EnPeehat_Larva_SetStateSeekPlayer(EnPeehat* this);
void EnPeehat_Ground_StateGround(EnPeehat* this, PlayState* play);
void EnPeehat_Ground_SetStateRise(EnPeehat* this);
void EnPeehat_Flying_StateGrounded(EnPeehat* this, PlayState* play);
void EnPeehat_Flying_SetStateRise(EnPeehat* this);
void EnPeehat_Flying_StateFly(EnPeehat* this, PlayState* play);
void EnPeehat_Flying_SetStateLanding(EnPeehat* this);
void EnPeehat_Ground_StateRise(EnPeehat* this, PlayState* play);
void EnPeehat_Ground_SetStateHover(EnPeehat* this);
void EnPeehat_Flying_StateRise(EnPeehat* this, PlayState* play);
void EnPeehat_Ground_StateSeekPlayer(EnPeehat* this, PlayState* play);
void EnPeehat_Ground_SetStateReturnHome(EnPeehat* this);
void EnPeehat_Ground_SetStateLanding(EnPeehat* this);
void EnPeehat_Larva_StateSeekPlayer(EnPeehat* this, PlayState* play);
void EnPeehat_SetStateAttackRecoil(EnPeehat* this);
void EnPeehat_Ground_StateLanding(EnPeehat* this, PlayState* play);
void EnPeehat_Flying_StateLanding(EnPeehat* this, PlayState* play);
void EnPeehat_Ground_StateHover(EnPeehat* this, PlayState* play);
void EnPeehat_Ground_StateReturnHome(EnPeehat* this, PlayState* play);
void EnPeehat_StateAttackRecoil(EnPeehat* this, PlayState* play);
void EnPeehat_StateBoomerangStunned(EnPeehat* this, PlayState* play);
void EnPeehat_Adult_StateDie(EnPeehat* this, PlayState* play);
void EnPeehat_SetStateExplode(EnPeehat* this);
void EnPeehat_StateExplode(EnPeehat* this, PlayState* play);

const ActorInit En_Peehat_InitVars = {
    ACTOR_EN_PEEHAT,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_PEEHAT,
    sizeof(EnPeehat),
    (ActorFunc)EnPeehat_Init,
    (ActorFunc)EnPeehat_Destroy,
    (ActorFunc)EnPeehat_Update,
    (ActorFunc)EnPeehat_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_WOOD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 50, 160, -70, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElemInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElemInit,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 00 */ PEAHAT_DMG_EFF_ATTACK = 0,
    /* 06 */ PEAHAT_DMG_EFF_LIGHT_ICE_ARROW = 6,
    /* 12 */ PEAHAT_DMG_EFF_FIRE = 12,
    /* 13 */ PEAHAT_DMG_EFF_HOOKSHOT = 13,
    /* 14 */ PEAHAT_DMG_EFF_BOOMERANG = 14,
    /* 15 */ PEAHAT_DMG_EFF_NUT = 15
} DamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, PEAHAT_DMG_EFF_NUT),
    /* Deku stick    */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Slingshot     */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Explosive     */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Boomerang     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Hammer swing  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Hookshot      */ DMG_ENTRY(2, PEAHAT_DMG_EFF_HOOKSHOT),
    /* Kokiri sword  */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Master sword  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Giant's Knife */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Fire arrow    */ DMG_ENTRY(4, PEAHAT_DMG_EFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Light arrow   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 1   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 2   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 3   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Fire magic    */ DMG_ENTRY(3, PEAHAT_DMG_EFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_LIGHT_ICE_ARROW),
    /* Light magic   */ DMG_ENTRY(0, PEAHAT_DMG_EFF_LIGHT_ICE_ARROW),
    /* Shield        */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Mirror Ray    */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Kokiri spin   */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Giant spin    */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Master spin   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Kokiri jump   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Giant jump    */ DMG_ENTRY(8, PEAHAT_DMG_EFF_ATTACK),
    /* Master jump   */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Unknown 1     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Unblockable   */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Hammer jump   */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Unknown 2     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
};

typedef enum {
    /* 00 */ PEAHAT_STATE_DYING,
    /* 01 */ PEAHAT_STATE_EXPLODE,
    /* 03 */ PEAHAT_STATE_3 = 3,
    /* 04 */ PEAHAT_STATE_4,
    /* 05 */ PEAHAT_STATE_FLY,
    /* 07 */ PEAHAT_STATE_ATTACK_RECOIL = 7,
    /* 08 */ PEAHAT_STATE_8,
    /* 09 */ PEAHAT_STATE_9,
    /* 10 */ PEAHAT_STATE_LANDING,
    /* 12 */ PEAHAT_STATE_RETURN_HOME = 12,
    /* 13 */ PEAHAT_STATE_STUNNED,
    /* 14 */ PEAHAT_STATE_SEEK_PLAYER,
    /* 15 */ PEAHAT_STATE_15
} PeahatState;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 700, ICHAIN_STOP),
};

void EnPeehat_SetupAction(EnPeehat* this, EnPeehatActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnPeehat_Init(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, 36.0f * 0.001f);
    SkelAnime_Init(play, &this->skelAnime, &gPeehatSkel, &gPeehatRisingAnim, this->jointTable, this->morphTable,
                   24);
    ActorShape_Init(&this->actor.shape, 100.0f, ActorShadow_DrawCircle, 27.0f);
    this->actor.focus.pos = this->actor.world.pos;
    this->unk_2D4 = 0;
    this->actor.world.rot.y = 0;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 6;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.floorHeight = this->actor.world.pos.y;
    Collider_InitCylinder(play, &this->colCylinder);
    Collider_SetCylinder(play, &this->colCylinder, &this->actor, &sCylinderInit);
    Collider_InitQuad(play, &this->colQuad);
    Collider_SetQuad(play, &this->colQuad, &this->actor, &sQuadInit);
    Collider_InitJntSph(play, &this->colJntSph);
    Collider_SetJntSph(play, &this->colJntSph, &this->actor, &sJntSphInit, this->colJntSphItemList);

    this->actor.naviEnemyId = 0x48;
    this->xzDistToRise = 740.0f;
    this->xzDistMax = 1200.0f;
    this->actor.uncullZoneForward = 4000.0f;
    this->actor.uncullZoneScale = 800.0f;
    this->actor.uncullZoneDownward = 1800.0f;
    switch (this->actor.params) {
        case PEAHAT_TYPE_GROUNDED:
            EnPeehat_Ground_SetStateGround(this);
            break;
        case PEAHAT_TYPE_FLYING:
            this->actor.uncullZoneForward = 4200.0f;
            this->xzDistToRise = 2800.0f;
            this->xzDistMax = 1400.0f;
            EnPeehat_Flying_SetStateGround(this);
            this->actor.flags &= ~ACTOR_FLAG_0;
            break;
        case PEAHAT_TYPE_LARVA:
            this->actor.scale.x = this->actor.scale.z = 0.006f;
            this->actor.scale.y = 0.003f;
            this->colCylinder.dim.radius = 25;
            this->colCylinder.dim.height = 15;
            this->colCylinder.dim.yShift = -5;
            this->colCylinder.info.bumper.dmgFlags = 0x1F824;
            this->colQuad.base.atFlags = AT_ON | AT_TYPE_ENEMY;
            this->colQuad.base.acFlags = AC_ON | AC_TYPE_PLAYER;
            this->actor.naviEnemyId = 0x49; // Larva
            EnPeehat_Larva_SetStateSeekPlayer(this);
            break;
    }
}

void EnPeehat_Destroy(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;
    EnPeehat* parent;

    Collider_DestroyCylinder(play, &this->colCylinder);
    Collider_DestroyJntSph(play, &this->colJntSph);

    // If PEAHAT_TYPE_LARVA, decrement total larva spawned
    if (this->actor.params > 0) {
        parent = (EnPeehat*)this->actor.parent;
        if (parent != NULL && parent->actor.update != NULL) {
            parent->unk_2FA--;
        }
    }
}

void EnPeehat_SpawnDust(PlayState* play, EnPeehat* this, Vec3f* pos, f32 arg3, s32 arg4, f32 arg5, f32 arg6) {
    Vec3f dustPos;
    Vec3f dustVel = { 0.0f, 8.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, -1.5f, 0.0f };
    f32 rot; // radians
    s32 pScale;

    rot = (Rand_ZeroOne() - 0.5f) * 6.28f;
    dustPos.y = this->actor.floorHeight;
    dustPos.x = Math_SinF(rot) * arg3 + pos->x;
    dustPos.z = Math_CosF(rot) * arg3 + pos->z;
    dustAccel.x = (Rand_ZeroOne() - 0.5f) * arg5;
    dustAccel.z = (Rand_ZeroOne() - 0.5f) * arg5;
    dustVel.y += (Rand_ZeroOne() - 0.5f) * 4.0f;
    pScale = (Rand_ZeroOne() * 5 + 12) * arg6;
    EffectSsHahen_Spawn(play, &dustPos, &dustVel, &dustAccel, arg4, pScale, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

/**
 * Handles being hit when on the ground
 */
void EnPeehat_HitWhenGrounded(EnPeehat* this, PlayState* play) {
    this->colCylinder.base.acFlags &= ~AC_HIT;
    if ((play->gameplayFrames & 0xF) == 0) {
        Vec3f itemDropPos = this->actor.world.pos;

        itemDropPos.y += 70.0f;
        Item_DropCollectibleRandom(play, &this->actor, &itemDropPos, 0x40);
        Item_DropCollectibleRandom(play, &this->actor, &itemDropPos, 0x40);
        Item_DropCollectibleRandom(play, &this->actor, &itemDropPos, 0x40);
        this->unk_2D4 = 240;
    } else {
        s32 i;

        this->colCylinder.base.acFlags &= ~AC_HIT;
        for (i = MAX_LARVA - this->unk_2FA; i > 0; i--) {
            Actor* larva =
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PEEHAT,
                                   Rand_CenteredFloat(25.0f) + this->actor.world.pos.x,
                                   Rand_CenteredFloat(25.0f) + (this->actor.world.pos.y + 50.0f),
                                   Rand_CenteredFloat(25.0f) + this->actor.world.pos.z, 0, 0, 0, PEAHAT_TYPE_LARVA);

            if (larva != NULL) {
                larva->velocity.y = 6.0f;
                larva->shape.rot.y = larva->world.rot.y = Rand_CenteredFloat(0xFFFF);
                this->unk_2FA++;
            }
        }
        this->unk_2D4 = 8;
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_DAMAGE);
}

void EnPeehat_Ground_SetStateGround(EnPeehat* this) {
    Animation_Change(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, Animation_GetLastFrame(&gPeehatRisingAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->seekPlayerTimer = 600;
    this->unk_2D4 = 0;
    this->unk_2FA = 0;
    this->state = PEAHAT_STATE_3;
    this->colCylinder.base.acFlags &= ~AC_HIT;
    EnPeehat_SetupAction(this, EnPeehat_Ground_StateGround);
}

void EnPeehat_Ground_StateGround(EnPeehat* this, PlayState* play) {
    if (IS_DAY) {
        this->actor.flags |= ACTOR_FLAG_0;
        if (this->riseDelayTimer == 0) {
            if (this->actor.xzDistToPlayer < this->xzDistToRise) {
                EnPeehat_Ground_SetStateRise(this);
            }
        } else {
            Math_SmoothStepToF(&this->actor.shape.yOffset, -1000.0f, 1.0f, 10.0f, 0.0f);
            this->riseDelayTimer--;
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_0;
        Math_SmoothStepToF(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
        if (this->unk_2D4 != 0) {
            this->unk_2D4--;
            if (this->unk_2D4 & 4) {
                Math_SmoothStepToF(&this->scaleShift, 0.205f, 1.0f, 0.235f, 0.0f);
            } else {
                Math_SmoothStepToF(&this->scaleShift, 0.0f, 1.0f, 0.005f, 0.0f);
            }
        } else if (this->colCylinder.base.acFlags & AC_HIT) {
            EnPeehat_HitWhenGrounded(this, play);
        }
    }
}

void EnPeehat_Flying_SetStateGround(EnPeehat* this) {
    Animation_Change(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, Animation_GetLastFrame(&gPeehatRisingAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->seekPlayerTimer = 400;
    this->unk_2D4 = 0;
    this->unk_2FA = 0; //! @bug: overwrites number of child larva spawned, allowing for more than MAX_LARVA spawns
    this->state = PEAHAT_STATE_4;
    EnPeehat_SetupAction(this, EnPeehat_Flying_StateGrounded);
}

void EnPeehat_Flying_StateGrounded(EnPeehat* this, PlayState* play) {
    if (IS_DAY) {
        if (this->actor.xzDistToPlayer < this->xzDistToRise) {
            EnPeehat_Flying_SetStateRise(this);
        }
    } else {
        Math_SmoothStepToF(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
        if (this->unk_2D4 != 0) {
            this->unk_2D4--;
            if (this->unk_2D4 & 4) {
                Math_SmoothStepToF(&this->scaleShift, 0.205f, 1.0f, 0.235f, 0.0f);
            } else {
                Math_SmoothStepToF(&this->scaleShift, 0.0f, 1.0f, 0.005f, 0.0f);
            }
        } else if (this->colCylinder.base.acFlags & AC_HIT) {
            EnPeehat_HitWhenGrounded(this, play);
        }
    }
}

void EnPeehat_Flying_SetStateFly(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_FLY;
    EnPeehat_SetupAction(this, EnPeehat_Flying_StateFly);
}

void EnPeehat_Flying_StateFly(EnPeehat* this, PlayState* play) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
    SkelAnime_Update(&this->skelAnime);
    if (!IS_DAY || this->xzDistToRise < this->actor.xzDistToPlayer) {
        EnPeehat_Flying_SetStateLanding(this);
    } else if (this->actor.xzDistToPlayer < this->xzDistMax) {
        if (this->unk_2FA < MAX_LARVA && (play->gameplayFrames & 7) == 0) {
            Actor* larva = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PEEHAT,
                                              Rand_CenteredFloat(25.0f) + this->actor.world.pos.x,
                                              Rand_CenteredFloat(5.0f) + this->actor.world.pos.y,
                                              Rand_CenteredFloat(25.0f) + this->actor.world.pos.z, 0, 0, 0, 1);
            if (larva != NULL) {
                larva->shape.rot.y = larva->world.rot.y = Rand_CenteredFloat(0xFFFF);
                this->unk_2FA++;
            }
        }
    }
    this->bladeRot += this->bladeRotVel;
}

void EnPeehat_Ground_SetStateRise(EnPeehat* this) {
    f32 lastFrame = Animation_GetLastFrame(&gPeehatRisingAnim);

    if (this->state != PEAHAT_STATE_STUNNED) {
        Animation_Change(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    }
    this->state = PEAHAT_STATE_8;
    this->animTimer = lastFrame;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_UP);
    EnPeehat_SetupAction(this, EnPeehat_Ground_StateRise);
}

void EnPeehat_Ground_StateRise(EnPeehat* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.shape.yOffset, 0.0f, 1.0f, 50.0f, 0.0f);
    if (Math_SmoothStepToS(&this->bladeRotVel, 4000, 1, 800, 0) == 0) {
        if (this->animTimer != 0) {
            this->animTimer--;
            if (this->skelAnime.playSpeed == 0.0f) {
                if (this->animTimer == 0) {
                    this->animTimer = 40;
                    this->skelAnime.playSpeed = 1.0f;
                }
            }
        }
        if (SkelAnime_Update(&this->skelAnime) || this->animTimer == 0) {
            EnPeehat_Ground_SetStateHover(this);
        } else {
            this->actor.world.pos.y += 6.5f;
        }
        if (this->actor.world.pos.y - this->actor.floorHeight < 80.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            func_80033480(play, &pos, 90.0f, 1, 0x96, 100, 1);
        }
    }
    EnPeehat_SpawnDust(play, this, &this->actor.world.pos, 75.0f, 2, 1.05f, 2.0f);
    Math_SmoothStepToF(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    this->bladeRot += this->bladeRotVel;
}

void EnPeehat_Flying_SetStateRise(EnPeehat* this) {
    f32 lastFrame;

    lastFrame = Animation_GetLastFrame(&gPeehatRisingAnim);
    if (this->state != PEAHAT_STATE_STUNNED) {
        Animation_Change(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    }
    this->state = PEAHAT_STATE_9;
    this->animTimer = lastFrame;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_UP);
    EnPeehat_SetupAction(this, EnPeehat_Flying_StateRise);
}

void EnPeehat_Flying_StateRise(EnPeehat* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.shape.yOffset, 0.0f, 1.0f, 50.0f, 0.0f);
    if (Math_SmoothStepToS(&this->bladeRotVel, 4000, 1, 800, 0) == 0) {
        if (this->animTimer != 0) {
            this->animTimer--;
            if (this->skelAnime.playSpeed == 0.0f) {
                if (this->animTimer == 0) {
                    this->animTimer = 40;
                    this->skelAnime.playSpeed = 1.0f;
                }
            }
        }
        if (SkelAnime_Update(&this->skelAnime) || this->animTimer == 0) {
            //! @bug: overwrites number of child larva spawned, allowing for more than MAX_LARVA spawns
            this->unk_2FA = 0;
            EnPeehat_Flying_SetStateFly(this);
        } else {
            this->actor.world.pos.y += 18.0f;
        }
        if (this->actor.world.pos.y - this->actor.floorHeight < 80.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            func_80033480(play, &pos, 90.0f, 1, 0x96, 100, 1);
        }
    }
    EnPeehat_SpawnDust(play, this, &this->actor.world.pos, 75.0f, 2, 1.05f, 2.0f);
    Math_SmoothStepToF(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    this->bladeRot += this->bladeRotVel;
}

void EnPeehat_Ground_SetStateSeekPlayer(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_SEEK_PLAYER;
    this->unk_2E0 = 0.0f;
    EnPeehat_SetupAction(this, EnPeehat_Ground_StateSeekPlayer);
}

void EnPeehat_Ground_StateSeekPlayer(EnPeehat* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->actor.speedXZ, 3.0f, 1.0f, 0.25f, 0.0f);
    Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight + 80.0f, 1.0f, 3.0f, 0.0f);
    if (this->seekPlayerTimer <= 0) {
        EnPeehat_Ground_SetStateLanding(this);
        this->riseDelayTimer = 40;
    } else {
        this->seekPlayerTimer--;
    }
    if (IS_DAY && (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < this->xzDistMax)) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 1000, 0);
        if (this->unk_2FA != 0) {
            this->actor.shape.rot.y += 0x1C2;
        } else {
            this->actor.shape.rot.y -= 0x1C2;
        }
    } else {
        EnPeehat_Ground_SetStateReturnHome(this);
    }
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->bladeRotVel, 4000, 1, 500, 0);
    this->bladeRot += this->bladeRotVel;
    Math_SmoothStepToF(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_Larva_SetStateSeekPlayer(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_SEEK_PLAYER;
    this->unk_2D4 = 0;
    EnPeehat_SetupAction(this, EnPeehat_Larva_StateSeekPlayer);
}

void EnPeehat_Larva_StateSeekPlayer(EnPeehat* this, PlayState* play) {
    f32 speedXZ = 5.3f;

    if (this->actor.xzDistToPlayer <= 5.3f) {
        speedXZ = this->actor.xzDistToPlayer + 0.0005f;
    }
    if (this->actor.parent != NULL && this->actor.parent->update == NULL) {
        this->actor.parent = NULL;
    }
    this->actor.speedXZ = speedXZ;
    if (this->actor.world.pos.y - this->actor.floorHeight >= 70.0f) {
        Math_SmoothStepToF(&this->actor.velocity.y, -1.3f, 1.0f, 0.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&this->actor.velocity.y, -0.135f, 1.0f, 0.05f, 0.0f);
    }
    if (this->unk_2D4 == 0) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 830, 0);
    } else {
        this->unk_2D4--;
    }
    this->actor.shape.rot.y += 0x15E;
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->bladeRotVel, 4000, 1, 500, 0);
    this->bladeRot += this->bladeRotVel;
    Math_SmoothStepToF(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_SM_FLY - SFX_FLAG);
    if (this->colQuad.base.atFlags & AT_BOUNCED) {
        this->actor.colChkInfo.health = 0;
        this->colQuad.base.acFlags = this->colQuad.base.acFlags & ~AC_BOUNCED;
        EnPeehat_SetStateAttackRecoil(this);
    } else if ((this->colQuad.base.atFlags & AT_HIT) || (this->colCylinder.base.acFlags & AC_HIT) ||
               (this->actor.bgCheckFlags & 1)) {
        Player* player = GET_PLAYER(play);
        this->colQuad.base.atFlags &= ~AT_HIT;
        if (!(this->colCylinder.base.acFlags & AC_HIT) && &player->actor == this->colQuad.base.at) {
            if (Rand_ZeroOne() > 0.5f) {
                this->actor.world.rot.y += 0x2000;
            } else {
                this->actor.world.rot.y -= 0x2000;
            }
            this->unk_2D4 = 40;
        } else if (this->colCylinder.base.acFlags & AC_HIT || this->actor.bgCheckFlags & 1) {
            Vec3f zeroVec = { 0, 0, 0 };
            s32 i;
            for (i = 4; i >= 0; i--) {
                Vec3f pos;
                pos.x = Rand_CenteredFloat(20.0f) + this->actor.world.pos.x;
                pos.y = Rand_CenteredFloat(10.0f) + this->actor.world.pos.y;
                pos.z = Rand_CenteredFloat(20.0f) + this->actor.world.pos.z;
                EffectSsDeadDb_Spawn(play, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                                     1);
            }
        }
        if (&player->actor != this->colQuad.base.at || this->colCylinder.base.acFlags & AC_HIT) {
            if (!(this->actor.bgCheckFlags & 1)) {
                EffectSsDeadSound_SpawnStationary(play, &this->actor.projectedPos, NA_SE_EN_PIHAT_SM_DEAD, 1, 1,
                                                  40);
            }
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x20);
            Actor_Kill(&this->actor);
            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA]++;
        }
    }
}

void EnPeehat_Ground_SetStateLanding(EnPeehat* this) {
    this->state = PEAHAT_STATE_LANDING;
    Animation_PlayOnce(&this->skelAnime, &gPeehatLandingAnim);
    EnPeehat_SetupAction(this, EnPeehat_Ground_StateLanding);
}

void EnPeehat_Ground_StateLanding(EnPeehat* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 50, 0);
    if (SkelAnime_Update(&this->skelAnime)) {
        EnPeehat_Ground_SetStateGround(this);
        this->actor.world.pos.y = this->actor.floorHeight;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_LAND);
    } else if (this->actor.floorHeight < this->actor.world.pos.y) {
        Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight, 0.3f, 3.5f, 0.25f);
        if (this->actor.world.pos.y - this->actor.floorHeight < 60.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            func_80033480(play, &pos, 80.0f, 1, 150, 100, 1);
            EnPeehat_SpawnDust(play, this, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
    }
    Math_SmoothStepToS(&this->bladeRotVel, 0, 1, 100, 0);
    this->bladeRot += this->bladeRotVel;
}

void EnPeehat_Flying_SetStateLanding(EnPeehat* this) {
    Animation_PlayOnce(&this->skelAnime, &gPeehatLandingAnim);
    this->state = PEAHAT_STATE_LANDING;
    EnPeehat_SetupAction(this, EnPeehat_Flying_StateLanding);
}

void EnPeehat_Flying_StateLanding(EnPeehat* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 50, 0);
    if (SkelAnime_Update(&this->skelAnime)) {
        EnPeehat_Flying_SetStateGround(this);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_LAND);
        this->actor.world.pos.y = this->actor.floorHeight;
    } else if (this->actor.floorHeight < this->actor.world.pos.y) {
        Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight, 0.3f, 13.5f, 0.25f);
        if (this->actor.world.pos.y - this->actor.floorHeight < 60.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            func_80033480(play, &pos, 80.0f, 1, 150, 100, 1);
            EnPeehat_SpawnDust(play, this, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
    }
    Math_SmoothStepToS(&this->bladeRotVel, 0, 1, 100, 0);
    this->bladeRot += this->bladeRotVel;
}

void EnPeehat_Ground_SetStateHover(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &gPeehatFlyingAnim);
    this->actor.speedXZ = Rand_ZeroOne() * 0.5f + 2.5f;
    this->unk_2D4 = Rand_ZeroOne() * 10 + 10;
    this->state = PEAHAT_STATE_15;
    EnPeehat_SetupAction(this, EnPeehat_Ground_StateHover);
}

void EnPeehat_Ground_StateHover(EnPeehat* this, PlayState* play) {
    f32 cos;
    Player* player = GET_PLAYER(play);

    // hover but don't gain altitude
    if (this->actor.world.pos.y - this->actor.floorHeight > 75.0f) {
        this->actor.world.pos.y -= 1.0f;
    }
    this->actor.world.pos.y += Math_CosF(this->unk_2E0) * 1.4f;
    cos = Math_CosF(this->unk_2E0) * 0.18f;
    this->unk_2E0 += ((0.0f <= cos) ? cos : -cos) + 0.07f;
    this->unk_2D4--;
    if (this->unk_2D4 <= 0) {
        this->actor.speedXZ = Rand_ZeroOne() * 0.5f + 2.5f;
        this->unk_2D4 = Rand_ZeroOne() * 10.0f + 10.0f;
        this->unk_2F4 = (Rand_ZeroOne() - 0.5f) * 1000.0f;
    }
    SkelAnime_Update(&this->skelAnime);
    this->actor.world.rot.y += this->unk_2F4;
    if (this->seekPlayerTimer <= 0) {
        EnPeehat_Ground_SetStateLanding(this);
        this->riseDelayTimer = 40;
    } else {
        this->seekPlayerTimer--;
    }
    this->actor.shape.rot.y += 0x15E;
    // if daytime, and the player is close to the initial spawn position
    if (IS_DAY && Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < this->xzDistMax) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        EnPeehat_Ground_SetStateSeekPlayer(this);
        this->unk_2FA = play->gameplayFrames & 1;
    } else {
        EnPeehat_Ground_SetStateReturnHome(this);
    }
    Math_SmoothStepToS(&this->bladeRotVel, 4000, 1, 500, 0);
    this->bladeRot += this->bladeRotVel;
    Math_SmoothStepToF(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_Ground_SetStateReturnHome(EnPeehat* this) {
    this->state = PEAHAT_STATE_RETURN_HOME;
    this->actor.speedXZ = 2.5f;
    EnPeehat_SetupAction(this, EnPeehat_Ground_StateReturnHome);
}

void EnPeehat_Ground_StateReturnHome(EnPeehat* this, PlayState* play) {
    f32 cos;
    s16 yRot;
    Player* player;

    player = GET_PLAYER(play);
    if (this->actor.world.pos.y - this->actor.floorHeight > 75.0f) {
        this->actor.world.pos.y -= 1.0f;
    } else {
        this->actor.world.pos.y += 1.0f;
    }
    this->actor.world.pos.y += Math_CosF(this->unk_2E0) * 1.4f;
    cos = Math_CosF(this->unk_2E0) * 0.18f;
    this->unk_2E0 += ((0.0f <= cos) ? cos : -cos) + 0.07f;
    yRot = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
    Math_SmoothStepToS(&this->actor.world.rot.y, yRot, 1, 600, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.x, 4500, 1, 600, 0);
    this->actor.shape.rot.y += 0x15E;
    this->bladeRot += this->bladeRotVel;
    if (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) < 2.0f) {
        EnPeehat_Ground_SetStateLanding(this);
        this->riseDelayTimer = 60;
    }
    if (IS_DAY && Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < this->xzDistMax) {
        this->seekPlayerTimer = 400;
        EnPeehat_Ground_SetStateSeekPlayer(this);
        this->unk_2FA = (play->gameplayFrames & 1);
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_SetStateAttackRecoil(EnPeehat* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gPeehatRecoilAnim, -4.0f);
    this->state = PEAHAT_STATE_ATTACK_RECOIL;
    this->actor.speedXZ = -9.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    EnPeehat_SetupAction(this, EnPeehat_StateAttackRecoil);
}

void EnPeehat_StateAttackRecoil(EnPeehat* this, PlayState* play) {
    this->bladeRot += this->bladeRotVel;
    SkelAnime_Update(&this->skelAnime);
    this->actor.speedXZ += 0.5f;
    if (this->actor.speedXZ == 0.0f) {
        // Is PEAHAT_TYPE_LARVA
        if (this->actor.params > 0) {
            Vec3f zeroVec = { 0, 0, 0 };
            s32 i;
            for (i = 4; i >= 0; i--) {
                Vec3f pos;
                pos.x = Rand_CenteredFloat(20.0f) + this->actor.world.pos.x;
                pos.y = Rand_CenteredFloat(10.0f) + this->actor.world.pos.y;
                pos.z = Rand_CenteredFloat(20.0f) + this->actor.world.pos.z;
                EffectSsDeadDb_Spawn(play, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                                     1);
            }
            Actor_Kill(&this->actor);
            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA]++;
        } else {
            EnPeehat_Ground_SetStateSeekPlayer(this);
            // Is PEAHAT_TYPE_GROUNDED
            if (this->actor.params < 0) {
                this->unk_2FA = (this->unk_2FA != 0) ? 0 : 1;
            }
        }
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_SetStateBoomerangStunned(EnPeehat* this) {
    this->state = PEAHAT_STATE_STUNNED;
    if (this->actor.floorHeight < this->actor.world.pos.y) {
        this->actor.speedXZ = -9.0f;
    }
    this->bladeRotVel = 0;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_SetColorFilter(&this->actor, 0, 200, 0, 80);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnPeehat_SetupAction(this, EnPeehat_StateBoomerangStunned);
}

void EnPeehat_StateBoomerangStunned(EnPeehat* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight, 1.0f, 8.0f, 0.0f);
    if (this->actor.colorFilterTimer == 0) {
        EnPeehat_Ground_SetStateRise(this);
    }
}

void EnPeehat_Adult_SetStateDie(EnPeehat* this) {
    this->bladeRotVel = 0;
    this->isStateDieFirstUpdate = 1;
    this->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 8);
    this->state = PEAHAT_STATE_DYING;
    this->scaleShift = 0.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    EnPeehat_SetupAction(this, EnPeehat_Adult_StateDie);
}

void EnPeehat_Adult_StateDie(EnPeehat* this, PlayState* play) {
    if (this->isStateDieFirstUpdate) {
        this->unk_2D4--;
        if (this->unk_2D4 <= 0 || this->actor.colChkInfo.health == 0) {
            Animation_MorphToPlayOnce(&this->skelAnime, &gPeehatRecoilAnim, -4.0f);
            this->bladeRotVel = 4000;
            this->unk_2D4 = 14;
            this->actor.speedXZ = 0;
            this->actor.velocity.y = 6;
            this->isStateDieFirstUpdate = 0;
            this->actor.shape.rot.z = this->actor.shape.rot.x = 0;
        } else if (this->actor.colorFilterTimer & 4) {
            Math_SmoothStepToF(&this->scaleShift, 0.205f, 1.0f, 0.235f, 0);
        } else {
            Math_SmoothStepToF(&this->scaleShift, 0, 1.0f, 0.005f, 0);
        }
    } else {
        SkelAnime_Update(&this->skelAnime);
        this->bladeRot += this->bladeRotVel;
        Math_SmoothStepToS(&this->bladeRotVel, 4000, 1, 250, 0);
        if (this->actor.colChkInfo.health == 0) {
            this->actor.scale.x -= 0.0015f;
            Actor_SetScale(&this->actor, this->actor.scale.x);
        }
        if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight + 88.5f, 1.0f, 3.0f, 0.0f) == 0.0f &&
            this->actor.world.pos.y - this->actor.floorHeight < 59.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            func_80033480(play, &pos, 80.0f, 1, 150, 100, 1);
            EnPeehat_SpawnDust(play, this, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
        if (this->actor.speedXZ < 0) {
            this->actor.speedXZ += 0.25f;
        }
        this->unk_2D4--;
        if (this->unk_2D4 <= 0) {
            if (this->actor.colChkInfo.health == 0) {
                EnPeehat_SetStateExplode(this);
                // if PEAHAT_TYPE_GROUNDED
            } else if (this->actor.params < 0) {
                EnPeehat_Ground_SetStateHover(this);
                this->riseDelayTimer = 60;
            } else {
                EnPeehat_Flying_SetStateFly(this);
            }
        }
    }
}

void EnPeehat_SetStateExplode(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_EXPLODE;
    this->animTimer = 5;
    this->unk_2E0 = 0.0f;
    EnPeehat_SetupAction(this, EnPeehat_StateExplode);
}

void EnPeehat_StateExplode(EnPeehat* this, PlayState* play) {
    EnBom* bomb;
    s32 pad[2];

    if (this->animTimer == 5) {
        bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0x602, 0);
        if (bomb != NULL) {
            bomb->timer = 0;
        }
    }
    this->animTimer--;
    if (this->animTimer == 0) {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x40);
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x40);
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x40);
        Actor_Kill(&this->actor);
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_PEAHAT]++;
    }
}

void EnPeehat_Adult_CollisionCheck(EnPeehat* this, PlayState* play) {
    if ((this->colCylinder.base.acFlags & AC_BOUNCED) || (this->colQuad.base.acFlags & AC_BOUNCED)) {
        this->colQuad.base.acFlags &= ~AC_BOUNCED;
        this->colCylinder.base.acFlags &= ~AC_BOUNCED;
        this->colJntSph.base.acFlags &= ~AC_HIT;
    } else if (this->colJntSph.base.acFlags & AC_HIT) {
        this->colJntSph.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&this->actor, &this->colJntSph, 1);
        if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_NUT ||
            this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_LIGHT_ICE_ARROW) {
            return;
        }
        if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_HOOKSHOT) {
            this->actor.colChkInfo.health = 0;
        } else if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_BOOMERANG) {
            if (this->state != PEAHAT_STATE_STUNNED) {
                EnPeehat_SetStateBoomerangStunned(this);
            }
            return;
        } else {
            Actor_ApplyDamage(&this->actor);
            Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 8);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_PIHAT_DAMAGE);
        }

        if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_FIRE) {
            Vec3f pos;
            s32 i;
            for (i = 4; i >= 0; i--) {
                pos.x = Rand_CenteredFloat(20.0f) + this->actor.world.pos.x;
                pos.y = Rand_ZeroOne() * 25.0f + this->actor.world.pos.y;
                pos.z = Rand_CenteredFloat(20.0f) + this->actor.world.pos.z;
                EffectSsEnFire_SpawnVec3f(play, &this->actor, &pos, 70, 0, 0, -1);
            }
            Actor_SetColorFilter(&this->actor, 0x4000, 200, 0, 100);
        }
        if (this->actor.colChkInfo.health == 0) {
            EnPeehat_Adult_SetStateDie(this);
        }
    }
}

void EnPeehat_Update(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;
    s32 i;
    Player* player = GET_PLAYER(play);

    // If Adult Peahat
    if (thisx->params <= 0) {
        EnPeehat_Adult_CollisionCheck(this, play);
    }
    if (thisx->colChkInfo.damageEffect != PEAHAT_DMG_EFF_LIGHT_ICE_ARROW) {
        if (thisx->speedXZ != 0.0f || thisx->velocity.y != 0.0f) {
            Actor_MoveForward(thisx);
            Actor_UpdateBgCheckInfo(play, thisx, 25.0f, 30.0f, 30.0f, 5);
        }

        this->actionFunc(this, play);
        if ((play->gameplayFrames & 0x7F) == 0) {
            this->jiggleRotInc = (Rand_ZeroOne() * 0.25f) + 0.5f;
        }
        this->jiggleRot += this->jiggleRotInc;
    }
    // if PEAHAT_TYPE_GROUNDED
    if (thisx->params < 0) {
        // Set the Z-Target point on the Peahat's weak point
        thisx->focus.pos.x = this->colJntSph.elements[0].dim.worldSphere.center.x;
        thisx->focus.pos.y = this->colJntSph.elements[0].dim.worldSphere.center.y;
        thisx->focus.pos.z = this->colJntSph.elements[0].dim.worldSphere.center.z;
        if (this->state == PEAHAT_STATE_SEEK_PLAYER) {
            Math_SmoothStepToS(&thisx->shape.rot.x, 6000, 1, 300, 0);
        } else {
            Math_SmoothStepToS(&thisx->shape.rot.x, 0, 1, 300, 0);
        }
    } else {
        thisx->focus.pos = thisx->world.pos;
    }
    Collider_UpdateCylinder(thisx, &this->colCylinder);
    if (thisx->colChkInfo.health > 0) {
        // If Adult Peahat
        if (thisx->params <= 0) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colCylinder.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colJntSph.base);
            if (thisx->colorFilterTimer == 0 || !(thisx->colorFilterParams & 0x4000)) {
                if (this->state != PEAHAT_STATE_EXPLODE) {
                    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colJntSph.base);
                }
            }
        }
        if (thisx->params != PEAHAT_TYPE_FLYING && this->colQuad.base.atFlags & AT_HIT) {
            this->colQuad.base.atFlags &= ~AT_HIT;
            if (&player->actor == this->colQuad.base.at) {
                EnPeehat_SetStateAttackRecoil(this);
            }
        }
    }
    if (this->state == PEAHAT_STATE_15 || this->state == PEAHAT_STATE_SEEK_PLAYER || this->state == PEAHAT_STATE_FLY ||
        this->state == PEAHAT_STATE_RETURN_HOME || this->state == PEAHAT_STATE_EXPLODE) {
        if (thisx->params != PEAHAT_TYPE_FLYING) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colQuad.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colQuad.base);
        }
        // if PEAHAT_TYPE_GROUNDED
        if (thisx->params < 0 && (thisx->flags & ACTOR_FLAG_6)) {
            for (i = 1; i >= 0; i--) {
                Vec3f posResult;
                CollisionPoly* poly = NULL;
                s32 bgId;
                Vec3f* posB = &this->bladeTip[i];

                if (BgCheck_EntityLineTest1(&play->colCtx, &thisx->world.pos, posB, &posResult, &poly, true, true,
                                            false, true, &bgId) == true) {
                    func_80033480(play, &posResult, 0.0f, 1, 300, 150, 1);
                    EnPeehat_SpawnDust(play, this, &posResult, 0.0f, 3, 1.05f, 1.5f);
                }
            }
        } else if (thisx->params != PEAHAT_TYPE_FLYING) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder.base);
        }
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder.base);
    }
    Math_SmoothStepToF(&this->scaleShift, 0.0f, 1.0f, 0.001f, 0.0f);
}

s32 EnPeehat_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    EnPeehat* this = (EnPeehat*)thisx;

    if (limbIndex == 4) {
        rot->x = -this->bladeRot;
    }
    if (limbIndex == 3 || (limbIndex == 23 && (this->state == PEAHAT_STATE_DYING || this->state == PEAHAT_STATE_3 ||
                                               this->state == PEAHAT_STATE_4))) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateX(this->jiggleRot * 0.115f, MTXMODE_APPLY);
        Matrix_RotateY(this->jiggleRot * 0.13f, MTXMODE_APPLY);
        Matrix_RotateZ(this->jiggleRot * 0.1f, MTXMODE_APPLY);
        Matrix_Scale(1.0f - this->scaleShift, this->scaleShift + 1.0f, 1.0f - this->scaleShift, MTXMODE_APPLY);
        Matrix_RotateZ(-(this->jiggleRot * 0.1f), MTXMODE_APPLY);
        Matrix_RotateY(-(this->jiggleRot * 0.13f), MTXMODE_APPLY);
        Matrix_RotateX(-(this->jiggleRot * 0.115f), MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
        return true;
    }
    return false;
}

void EnPeehat_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f peahatBladeTip[] = { { 0.0f, 0.0f, 5500.0f }, { 0.0f, 0.0f, -5500.0f } };

    EnPeehat* this = (EnPeehat*)thisx;
    f32 damageYRot;

    if (limbIndex == 4) {
        Matrix_MultVec3f(&peahatBladeTip[0], &this->bladeTip[0]);
        Matrix_MultVec3f(&peahatBladeTip[1], &this->bladeTip[1]);
        return;
    }
    // is Adult Peahat
    if (limbIndex == 3 && this->actor.params <= 0) {
        damageYRot = 0.0f;
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        Matrix_Translate(-1000.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Collider_UpdateSpheres(0, &this->colJntSph);
        Matrix_Translate(500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        if (this->actor.colorFilterTimer != 0 && (this->actor.colorFilterParams & 0x4000)) {
            damageYRot = Math_SinS(this->actor.colorFilterTimer * 0x4E20) * 0.35f;
        }
        Matrix_RotateY(3.2f + damageYRot, MTXMODE_APPLY);
        Matrix_Scale(0.3f, 0.2f, 0.2f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnPeehat_Draw(Actor* thisx, PlayState* play) {
    static Vec3f D_80AD285C[] = {
        { 0.0f, 0.0f, -4500.0f }, { -4500.0f, 0.0f, 0.0f }, { 4500.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 4500.0f }
    };
    EnPeehat* this = (EnPeehat*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnPeehat_OverrideLimbDraw,
                      EnPeehat_PostLimbDraw, this);
    if (this->actor.speedXZ != 0.0f || this->actor.velocity.y != 0.0f) {
        Matrix_MultVec3f(&D_80AD285C[0], &this->colQuad.dim.quad[1]);
        Matrix_MultVec3f(&D_80AD285C[1], &this->colQuad.dim.quad[0]);
        Matrix_MultVec3f(&D_80AD285C[2], &this->colQuad.dim.quad[3]);
        Matrix_MultVec3f(&D_80AD285C[3], &this->colQuad.dim.quad[2]);
        Collider_SetQuadVertices(&this->colQuad, &this->colQuad.dim.quad[0], &this->colQuad.dim.quad[1],
                                 &this->colQuad.dim.quad[2], &this->colQuad.dim.quad[3]);
    }
}

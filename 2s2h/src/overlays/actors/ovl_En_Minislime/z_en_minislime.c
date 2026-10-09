/*
 * File: z_en_minislime.c
 * Overlay: En_Minislime
 * Description: Mad Jelly & Gekko Miniboss: Mini Jelly Droplets
 */

#include "z_en_minislime.h"
#include "overlays/actors/ovl_En_Bigslime/z_en_bigslime.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void EnMinislime_Init(Actor* thisx, PlayState* play);
void EnMinislime_Destroy(Actor* thisx, PlayState* play);
void EnMinislime_Update(Actor* thisx, PlayState* play);

void EnMinislime_SetupDisappear(EnMinislime* this);
void EnMinislime_Disappear(EnMinislime* this, PlayState* play);
void EnMinislime_SetupFall(EnMinislime* this, PlayState* play);
void EnMinislime_Fall(EnMinislime* this, PlayState* play);
void EnMinislime_SetupBreakFromBigslime(EnMinislime* this);
void EnMinislime_BreakFromBigslime(EnMinislime* this, PlayState* play);
void EnMinislime_IceArrowDamage(EnMinislime* this, PlayState* play);
void EnMinislime_FireArrowDamage(EnMinislime* this, PlayState* play);
void EnMinislime_SetupGrowAndShrink(EnMinislime* this);
void EnMinislime_GrowAndShrink(EnMinislime* this, PlayState* play);
void EnMinislime_SetupIdle(EnMinislime* this);
void EnMinislime_SetupBounce(EnMinislime* this);
void EnMinislime_Idle(EnMinislime* this, PlayState* play);
void EnMinislime_Bounce(EnMinislime* this, PlayState* play);
void EnMinislime_SetupDespawn(EnMinislime* this);
void EnMinislime_Despawn(EnMinislime* this, PlayState* play);
void EnMinislime_MoveToBigslime(EnMinislime* this, PlayState* play);
void EnMinislime_Knockback(EnMinislime* this, PlayState* play);
void EnMinislime_DefeatIdle(EnMinislime* this, PlayState* play);
void EnMinislime_SetupDefeatMelt(EnMinislime* this, PlayState* play);
void EnMinislime_DefeatMelt(EnMinislime* this, PlayState* play);
void EnMinislime_SetupMoveToGekko(EnMinislime* this);
void EnMinislime_MoveToGekko(EnMinislime* this, PlayState* play);
void EnMinislime_SetupGekkoThrow(EnMinislime* this);
void EnMinislime_GekkoThrow(EnMinislime* this, PlayState* play);

ActorProfile En_Minislime_Profile = {
    /**/ ACTOR_EN_MINISLIME,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BIGSLIME,
    /**/ sizeof(EnMinislime),
    /**/ EnMinislime_Init,
    /**/ EnMinislime_Destroy,
    /**/ EnMinislime_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE | AT_TYPE_ENEMY,
        AC_NONE | AC_TYPE_PLAYER,
        OC1_NONE | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 54, 60, -30, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 4, 40, 40, 30 };

typedef enum {
    /* 0x0 */ MINISLIME_DMGEFF_NONE,
    /* 0x2 */ MINISLIME_DMGEFF_FIRE = 0x2,
    /* 0x3 */ MINISLIME_DMGEFF_ICE,
    /* 0xE */ MINISLIME_DMGEFF_HOOKSHOT = 0xE,
    /* 0xF */ MINISLIME_DMGEFF_BREAK_ICE
} MinislimeDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, MINISLIME_DMGEFF_BREAK_ICE),
    /* Zora boomerang */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, MINISLIME_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, MINISLIME_DMGEFF_BREAK_ICE),
    /* Sword          */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, MINISLIME_DMGEFF_BREAK_ICE),
    /* Fire arrow     */ DMG_ENTRY(1, MINISLIME_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, MINISLIME_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Goron spikes   */ DMG_ENTRY(1, MINISLIME_DMGEFF_BREAK_ICE),
    /* Deku spin      */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Normal shield  */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, MINISLIME_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, MINISLIME_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, MINISLIME_DMGEFF_BREAK_ICE),
};

void EnMinislime_Init(Actor* thisx, PlayState* play) {
    EnMinislime* this = (EnMinislime*)thisx;

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->id = this->actor.params;
    this->actor.shape.shadowAlpha = 255;
    EnMinislime_SetupDisappear(this);
}

void EnMinislime_Destroy(Actor* thisx, PlayState* play) {
    EnMinislime* this = (EnMinislime*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnMinislime_CheckBackgroundCollision(EnMinislime* this) {
    f32 scaleX = this->actor.scale.x * 400.0f;
    f32 scaleY = this->actor.scale.y * 400.0f;
    f32 scaleZ = this->actor.scale.z * 400.0f;

    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL | BGCHECKFLAG_CEILING);

    if ((this->actor.world.pos.y + scaleY) > GBT_ROOM_5_MAX_Y) {
        this->actor.bgCheckFlags |= BGCHECKFLAG_CEILING;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        this->actor.world.pos.y = GBT_ROOM_5_MAX_Y - scaleY;
    } else if ((this->actor.world.pos.y - scaleY) < GBT_ROOM_5_MIN_Y) {
        this->actor.world.pos.y = GBT_ROOM_5_MIN_Y + scaleY;
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND_TOUCH;
        }
        this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
    } else {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    }

    if ((this->actor.world.pos.x + scaleX) > GBT_ROOM_5_MAX_X) {
        this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
        this->actor.world.pos.x = GBT_ROOM_5_MAX_X - scaleX;
    } else if ((this->actor.world.pos.x - scaleX) < GBT_ROOM_5_MIN_X) {
        this->actor.world.pos.x = GBT_ROOM_5_MIN_X + scaleX;
        this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
    }

    if ((this->actor.world.pos.z + scaleZ) > GBT_ROOM_5_MAX_Z) {
        this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
        this->actor.world.pos.z = GBT_ROOM_5_MAX_Z - scaleZ;
    } else if ((this->actor.world.pos.z - scaleZ) < GBT_ROOM_5_MIN_Z) {
        this->actor.world.pos.z = GBT_ROOM_5_MIN_Z + scaleZ;
        this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
    }
}

void EnMinislime_AddIceShardEffect(EnMinislime* this) {
    s32 pad;
    EnBigslime* bigslime = (EnBigslime*)this->actor.parent;
    EnBigslimeIceShardEffect* iceShardEffect;
    s32 i = 10 * this->id + BIGSLIME_NUM_VTX;
    s32 i_end = i + 10;
    s16 pitch;
    s16 yaw = 0;

    for (; i < i_end; i++) {
        iceShardEffect = &bigslime->iceShardEffect[i];
        pitch = Rand_S16Offset(0x1000, 0x3000);
        iceShardEffect->velocity.x = Math_CosS(pitch) * Math_SinS(yaw);
        iceShardEffect->velocity.y = Math_SinS(pitch);
        iceShardEffect->velocity.z = Math_CosS(pitch) * Math_CosS(yaw);
        iceShardEffect->pos.x = this->actor.world.pos.x + (400.0f * this->actor.scale.x) * iceShardEffect->velocity.x;
        iceShardEffect->pos.y =
            this->actor.world.pos.y + (((iceShardEffect->velocity.y * 2.0f) - 1.0f) * 400.0f * this->actor.scale.y);
        iceShardEffect->pos.z = this->actor.world.pos.z + (400.0f * this->actor.scale.z) * iceShardEffect->velocity.z;
        iceShardEffect->rot.x = (s32)Rand_Next() >> 0x10;
        iceShardEffect->rot.y = (s32)Rand_Next() >> 0x10;
        iceShardEffect->rot.z = (s32)Rand_Next() >> 0x10;
        iceShardEffect->isEnabled = true;
        Math_Vec3f_ScaleAndStore(&iceShardEffect->velocity, Rand_ZeroFloat(3.0f) + 7.0f, &iceShardEffect->velocity);
        iceShardEffect->scale = (Rand_ZeroFloat(6.0f) + 2.0f) * 0.001f;
        yaw += 0x1999;
    }

    this->frozenAlpha = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
}

void EnMinislime_AddIceSmokeEffect(EnMinislime* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;

    pos.x = (Rand_CenteredFloat(200.0f) * this->actor.scale.x) + this->actor.world.pos.x;
    pos.y = CLAMP_MIN(this->actor.world.pos.y, GBT_ROOM_5_MIN_Y + 30.0f);
    pos.z = (Rand_CenteredFloat(200.0f) * this->actor.scale.z) + this->actor.world.pos.z;
    velocity.x = Rand_CenteredFloat(1.5f);
    velocity.z = Rand_CenteredFloat(1.5f);
    velocity.y = 2.0f;
    EffectSsIceSmoke_Spawn(play, &pos, &velocity, &gZeroVec3f, 500);
}

void EnMinislime_SetupDisappear(EnMinislime* this) {
    this->actor.params = MINISLIME_DISAPPEAR;
    this->actionFunc = EnMinislime_Disappear;
}

void EnMinislime_Disappear(EnMinislime* this, PlayState* play) {
    if (this->actor.params == MINISLIME_BREAK_BIGSLIME) {
        EnMinislime_SetupBreakFromBigslime(this);
    } else if (this->actor.params == MINISLIME_INIT_FALL) {
        EnMinislime_SetupFall(this, play);
    } else if (this->actor.params == MINISLIME_DESPAWN) {
        EnMinislime_SetupDespawn(this);
    }
}

void EnMinislime_SetupFall(EnMinislime* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw;

    this->collider.base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->collider.base.ocFlags1 |= OC1_ON;
    this->actor.speed = 0.0f;
    this->actor.gravity = -2.0f;
    if (this->actionFunc != EnMinislime_GekkoThrow) {
        this->actor.scale.x = 0.095f;
        this->actor.scale.z = 0.095f;
        this->actor.scale.y = 0.10700001f;
        if (Actor_WorldDistXZToActor(&this->actor, &player->actor) < 225.0f) {
            yaw = Actor_WorldYawTowardActor(&player->actor, &this->actor);
            this->actor.world.pos.x = Math_SinS(yaw) * 225.0f + player->actor.world.pos.x;
            this->actor.world.pos.z = Math_CosS(yaw) * 225.0f + player->actor.world.pos.z;
        }
    }
    this->actionFunc = EnMinislime_Fall;
}

void EnMinislime_Fall(EnMinislime* this, PlayState* play) {
    Math_StepToF(&this->actor.scale.x, 0.17999999f, 0.003f);
    Math_StepToF(&this->actor.scale.y, 0.05f, 0.003f);
    this->actor.scale.z = this->actor.scale.x;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnMinislime_SetupGrowAndShrink(this);
    }
}

void EnMinislime_SetupBreakFromBigslime(EnMinislime* this) {
    f32 velY;

    this->actor.world.rot.y = Actor_WorldYawTowardActor(this->actor.parent, &this->actor);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.speed = Math_CosS(this->actor.world.rot.x) * 15.0f;
    velY = Math_SinS(this->actor.world.rot.x) * 15.0f;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.velocity.y = velY + 2.0f;
    this->actor.gravity = -1.0f;
    this->frozenScale = 0.1f;
    this->actor.world.rot.x = Rand_S16Offset(0x800, 0x800);
    this->actor.shape.rot.x = (s32)Rand_Next() >> 0x10;
    this->actor.shape.rot.z = (s32)Rand_Next() >> 0x10;
    this->actor.scale.x = 0.15f;
    this->actor.scale.y = 0.075f;
    this->actor.scale.z = 0.15f;
    this->frozenAlpha = 200;
    this->actionFunc = EnMinislime_BreakFromBigslime;
}

void EnMinislime_BreakFromBigslime(EnMinislime* this, PlayState* play) {
    this->actor.shape.rot.x += this->actor.world.rot.x;
    if (this->actor.velocity.y < 0.0f) {
        this->collider.base.ocFlags1 |= OC1_ON;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            EnMinislime_AddIceShardEffect(this);
            this->attackTimer = 40;
            EnMinislime_SetupGrowAndShrink(this);
        }
    }
}

void EnMinislime_SetupIceArrowDamage(EnMinislime* this) {
    this->collider.base.atFlags &= ~AT_ON;
    this->frozenTimer = 80;
    this->actor.speed = 0.0f;
    this->frozenScale = 0.1f;
    this->actionFunc = EnMinislime_IceArrowDamage;
}

void EnMinislime_IceArrowDamage(EnMinislime* this, PlayState* play) {
    if (this->frozenTimer == 80) {
        this->frozenAlpha += 10;
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        if (this->frozenAlpha >= 200) {
            this->frozenAlpha = 200;
            this->frozenTimer--;
        }
    } else if (this->frozenTimer > 0) {
        this->frozenTimer--;
        if (this->frozenTimer == 40) {
            Math_Vec3f_Copy(&this->shakeRefPos, &this->actor.world.pos);
        } else if (this->frozenTimer > 0) {
            if ((this->frozenTimer < 20) || ((this->frozenTimer < 40) && ((this->frozenTimer % 2) != 0))) {
                f32 invFrozenTimer = 1.0f / this->frozenTimer;
                s32 pad;
                f32 randFloat = Rand_ZeroFloat(invFrozenTimer);
                s32 randSign = Rand_ZeroOne() < 0.5f ? -1 : 1;

                this->actor.world.pos.x = randSign * (invFrozenTimer + randFloat) + this->shakeRefPos.x;
                randFloat = Rand_ZeroFloat(invFrozenTimer);
                randSign = Rand_ZeroOne() < 0.5f ? -1 : 1;
                this->actor.world.pos.z = randSign * (invFrozenTimer + randFloat) + this->shakeRefPos.z;
            }
        }
    } else {
        EnMinislime_AddIceShardEffect(this);
        EnMinislime_SetupIdle(this);
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        EnMinislime_AddIceShardEffect(this);
        EnMinislime_SetupGrowAndShrink(this);
    }
}

void EnMinislime_SetupFireArrowDamage(EnMinislime* this) {
    this->meltTimer = 40;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.y = 0;
    this->actor.shape.rot.z = 0;
    this->actor.world.rot.x = 0;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.speed = 0.0f;
    this->actionFunc = EnMinislime_FireArrowDamage;
}

void EnMinislime_FireArrowDamage(EnMinislime* this, PlayState* play) {
    this->meltTimer--;
    if ((this->meltTimer % 25) == 0) {
        EnMinislime_AddIceSmokeEffect(this, play);
    }

    this->frozenScale = this->meltTimer * 0.0025f;
    if ((10 * this->meltTimer) > 200) {
        this->frozenAlpha = 200;
    } else {
        this->frozenAlpha = 10 * this->meltTimer;
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);
    if (this->meltTimer == 0) {
        EnMinislime_SetupIdle(this);
    }
}

void EnMinislime_SetupGrowAndShrink(EnMinislime* this) {
    this->collider.base.ocFlags1 |= OC1_ON;
    this->collider.base.acFlags |= AC_ON;
    this->collider.base.atFlags |= AT_ON;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.y = 0;
    this->actor.shape.rot.z = 0;
    this->actor.world.rot.x = 0;
    this->actor.speed = 0.0f;
    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    this->growShrinkTimer = 42;
    this->actor.scale.x = 0.19f;
    this->actor.scale.y = 0.044999998f;
    this->actor.scale.z = 0.19f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_SLIME_JUMP2);
    this->actionFunc = EnMinislime_GrowAndShrink;
}

void EnMinislime_GrowAndShrink(EnMinislime* this, PlayState* play) {
    f32 scaleFactor;

    this->growShrinkTimer--;
    scaleFactor = (this->growShrinkTimer / 6) + 1.0f;
    this->actor.scale.z = this->actor.scale.x =
        ((Math_CosF(this->growShrinkTimer * (M_PIf / 3)) * (scaleFactor * (2.0f / 30.0f))) + 1.5f) * 0.1f;
    this->actor.scale.y = ((Math_SinF(this->growShrinkTimer * (M_PIf / 3)) * (scaleFactor * 0.05f)) + 0.75f) * 0.1f;
    if (this->actor.params == MINISLIME_SETUP_GEKKO_THROW) {
        EnMinislime_SetupMoveToGekko(this);
    } else if ((this->actor.xzDistToPlayer < 150.0f) && (this->growShrinkTimer < 38)) {
        EnMinislime_SetupBounce(this);
    } else if (this->growShrinkTimer == 0) {
        EnMinislime_SetupIdle(this);
    }
}

void EnMinislime_SetupIdle(EnMinislime* this) {
    this->idleTimer = 20;
    this->collider.base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->actor.params = MINISLIME_IDLE;
    this->actionFunc = EnMinislime_Idle;
}

void EnMinislime_Idle(EnMinislime* this, PlayState* play) {
    f32 speedXZ;

    this->idleTimer--;
    speedXZ = Math_SinF(this->idleTimer * (M_PIf / 10));
    this->actor.speed = speedXZ * 1.5f;
    this->actor.speed = CLAMP_MIN(this->actor.speed, 0.0f);
    Math_StepToF(&this->actor.scale.x, ((0.14f * speedXZ) + 1.5f) * 0.1f, 0.010000001f);
    Math_StepToF(&this->actor.scale.y, ((Math_CosF(this->idleTimer * (M_PIf / 10)) * 0.07f) + 0.75f) * 0.1f,
                 0.010000001f);
    Math_StepToF(&this->actor.scale.z, 0.3f - this->actor.scale.x, 0.010000001f);
    if (this->idleTimer == 0) {
        if (this->actor.xzDistToPlayer < 300.0f) {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < 200.0f) {
            this->actor.world.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
        } else {
            this->actor.world.rot.y += (s16)((s32)Rand_Next() >> 0x13);
        }
        this->idleTimer = 20;
    }

    if (this->actor.params == MINISLIME_SETUP_GEKKO_THROW) {
        EnMinislime_SetupMoveToGekko(this);
    } else if (this->actor.xzDistToPlayer < 150.0f) {
        EnMinislime_SetupBounce(this);
    }
}

void EnMinislime_SetupBounce(EnMinislime* this) {
    this->actor.speed = 0.0f;
    this->bounceTimer = (this->actionFunc == EnMinislime_GrowAndShrink) ? 1 : 4;
    Actor_PlaySfx(&this->actor, NA_SE_EN_SLIME_JUMP1);
    this->actionFunc = EnMinislime_Bounce;
}

void EnMinislime_Bounce(EnMinislime* this, PlayState* play) {
    if (this->actor.params == MINISLIME_SETUP_GEKKO_THROW) {
        EnMinislime_SetupMoveToGekko(this);
    } else {
        if (this->bounceTimer > 0) {
            this->bounceTimer--;
            if (this->bounceTimer == 0) {
                this->actor.gravity = -2.0f;
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                this->actor.speed = 1.0f;
                this->actor.velocity.y = 12.0f;
                this->actor.shape.rot.y = this->actor.world.rot.y;
            }
            Math_StepToF(&this->actor.scale.x, 0.17999999f, 0.010000001f);
            Math_StepToF(&this->actor.scale.y, 0.05f, 0.010000001f);
        } else if (this->actor.velocity.y > 0.0f) {
            Math_StepToF(&this->actor.scale.x, 0.095f, 0.020000001f);
            Math_StepToF(&this->actor.scale.y, 0.10700001f, 0.020000001f);
        } else {
            Math_StepToF(&this->actor.scale.x, 0.17999999f, 0.003f);
            Math_StepToF(&this->actor.scale.y, 0.05f, 0.003f);
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                EnMinislime_SetupGrowAndShrink(this);
                return;
            }
        }

        this->actor.scale.z = this->actor.scale.x;
    }
}

void EnMinislime_SetupMoveToBigslime(EnMinislime* this) {
    this->actor.gravity = 0.0f;
    this->actor.speed = 15.0f;
    this->actor.shape.rot.x = Actor_WorldPitchTowardPoint(&this->actor, &this->actor.parent->home.pos);
    this->actor.shape.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->actor.parent->home.pos);
    this->actor.world.rot.x = -this->actor.shape.rot.x;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->actor.scale.x = 0.095f;
    this->actor.scale.y = 0.095f;
    this->actor.scale.z = 0.15f;
    if (this->frozenAlpha > 20) {
        EnMinislime_AddIceShardEffect(this);
    }
    this->frozenAlpha = 0;

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
    }
    this->actionFunc = EnMinislime_MoveToBigslime;
}

void EnMinislime_MoveToBigslime(EnMinislime* this, PlayState* play) {
    if (this->actor.params == MINISLIME_DISAPPEAR) {
        EnMinislime_SetupDisappear(this);
    } else if ((this->actor.scale.x > 0.0f) && (this->actor.world.pos.y > (GBT_ROOM_5_MAX_Y - 100.0f))) {
        this->actor.params = MINISLIME_SETUP_DISAPPEAR;
        this->actor.speed = 0.0f;
        Actor_SetScale(&this->actor, 0.0f);
    }
}

void EnMinislime_SetupKnockback(EnMinislime* this) {
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 |= OC1_ON;
    this->knockbackTimer = 30;
    this->actor.speed = 20.0f;
    func_800BE504(&this->actor, &this->collider);
    this->actionFunc = EnMinislime_Knockback;
}

void EnMinislime_Knockback(EnMinislime* this, PlayState* play) {
    f32 sqrtFrozenTimer;

    this->knockbackTimer--;
    Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    sqrtFrozenTimer = sqrtf(this->knockbackTimer);
    this->actor.scale.x = ((Math_CosF(this->knockbackTimer * (M_PIf / 3)) * (0.05f * sqrtFrozenTimer)) + 1.0f) * 0.15f;
    this->actor.scale.z = this->actor.scale.x;
    if (this->knockbackTimer == 15) {
        this->collider.base.acFlags |= AC_ON;
    }

    this->actor.scale.y = ((Math_SinF(this->knockbackTimer * (M_PIf / 3)) * (0.05f * sqrtFrozenTimer)) + 1.0f) * 0.075f;
    if (this->actor.params == MINISLIME_SETUP_GEKKO_THROW) {
        EnMinislime_SetupMoveToGekko(this);
    } else if (this->knockbackTimer == 0) {
        EnMinislime_SetupIdle(this);
    }
}

void EnMinislime_SetupDefeatIdle(EnMinislime* this) {
    this->actor.params = MINISLIME_DISAPPEAR;
    this->idleTimer = 20;
    this->collider.base.atFlags &= ~(AT_ON | AT_HIT);
    this->collider.base.acFlags &= ~(AC_ON | AC_HIT);
    this->actor.speed = 0.0f;
    if (this->frozenAlpha > 20) {
        EnMinislime_AddIceShardEffect(this);
    }

    this->frozenAlpha = 0;
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
    }

    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->actionFunc = EnMinislime_DefeatIdle;
}

void EnMinislime_DefeatIdle(EnMinislime* this, PlayState* play) {
    f32 xzScale;

    this->idleTimer--;
    xzScale = Math_SinF(this->idleTimer * (M_PIf / 10));
    Math_StepToF(&this->actor.scale.x, ((0.14f * xzScale) + 1.5f) * 0.1f, 0.010000001f);
    Math_StepToF(&this->actor.scale.y, ((0.07f * Math_CosF(this->idleTimer * (M_PIf / 10))) + 0.75f) * 0.1f,
                 0.010000001f);
    Math_StepToF(&this->actor.scale.z, 0.3f - this->actor.scale.x, 0.010000001f);
    if (this->idleTimer == 0) {
        this->idleTimer = 20;
    }

    if (this->actor.params == MINISLIME_DEFEAT_MELT) {
        EnMinislime_SetupDefeatMelt(this, play);
    }
}

void EnMinislime_SetupDefeatMelt(EnMinislime* this, PlayState* play) {
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = -50.0f;
    this->meltTimer = Rand_ZeroFloat(25.0f);
    EnMinislime_AddIceSmokeEffect(this, play);
    this->actor.params = MINISLIME_DISAPPEAR;
    this->actionFunc = EnMinislime_DefeatMelt;
}

void EnMinislime_DefeatMelt(EnMinislime* this, PlayState* play) {
    this->meltTimer++;
    if (((this->meltTimer % 25) == 0) && (this->actor.shape.shadowAlpha > 25)) {
        EnMinislime_AddIceSmokeEffect(this, play);
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);
    if (Math_StepToF(&this->actor.scale.y, 0.001f, 0.00075f)) {
        if ((this->actor.shape.shadowAlpha - 4) <= 0) {
            this->actor.shape.shadowAlpha = 0;
            EnMinislime_SetupDisappear(this);
        } else {
            this->actor.shape.shadowAlpha -= 4;
        }
    }
    Math_StepToF(&this->actor.scale.x, 0.25f, 0.001f);
    this->actor.scale.z = this->actor.scale.x;
}

void EnMinislime_SetupDespawn(EnMinislime* this) {
    this->actionFunc = EnMinislime_Despawn;
}

void EnMinislime_Despawn(EnMinislime* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

void EnMinislime_SetupMoveToGekko(EnMinislime* this) {
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
    }

    this->actionFunc = EnMinislime_MoveToGekko;
}

void EnMinislime_MoveToGekko(EnMinislime* this, PlayState* play) {
    Math_StepToF(&this->actor.world.pos.x, this->actor.parent->world.pos.x, 10.0f);
    Math_StepToF(&this->actor.world.pos.z, this->actor.parent->world.pos.z, 10.0f);
    Math_StepToF(&this->actor.world.pos.y, this->actor.parent->world.pos.y + 80.0f, 10.0f);
    Math_StepToF(&this->actor.scale.x, 0.15f, 0.0075000003f);
    Math_StepToF(&this->actor.scale.y, 0.075f, 0.0037500001f);
    Math_StepToF(&this->actor.scale.z, 0.15f, 0.0075000003f);
    if (this->actor.params == MINISLIME_GEKKO_THROW) {
        EnMinislime_SetupGekkoThrow(this);
    } else if (this->actor.params == MINISLIME_IDLE) {
        EnMinislime_SetupFall(this, play);
    }
}

void EnMinislime_SetupGekkoThrow(EnMinislime* this) {
    f32 xzDistToPlayer;

    this->collider.base.acFlags |= AC_ON;
    this->collider.base.ocFlags1 |= OC1_ON;
    xzDistToPlayer = CLAMP_MIN(this->actor.xzDistToPlayer, 200.0f);
    this->actor.speed = 17.5f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.gravity = -1.0f;
    this->actor.velocity.y = ((xzDistToPlayer - 200.0f) * 0.01f) + 3.0f;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->throwTimer = 10;
    this->actionFunc = EnMinislime_GekkoThrow;
}

void EnMinislime_GekkoThrow(EnMinislime* this, PlayState* play) {
    f32 xzScale;

    this->throwTimer--;
    xzScale = Math_SinF(this->throwTimer * (M_PIf / 5));
    this->actor.scale.x = ((0.3f * xzScale) + 1.5f) * 0.1f;
    this->actor.scale.y = ((Math_CosF(this->throwTimer * (M_PIf / 5)) * 0.2f) + 0.75f) * 0.1f;
    this->actor.scale.z = 0.3f - this->actor.scale.x;
    if (this->throwTimer == 0) {
        this->throwTimer = 10;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnMinislime_SetupGrowAndShrink(this);
    }
}

void EnMinislime_ApplyDamage(EnMinislime* this) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actionFunc == EnMinislime_IceArrowDamage) {
            if (this->actor.colChkInfo.damageEffect != MINISLIME_DMGEFF_HOOKSHOT) {
                if (this->actor.colChkInfo.damageEffect == MINISLIME_DMGEFF_BREAK_ICE) {
                    EnMinislime_AddIceShardEffect(this);
                    EnMinislime_SetupIdle(this);
                } else if (this->actor.colChkInfo.damageEffect == MINISLIME_DMGEFF_FIRE) {
                    EnMinislime_SetupFireArrowDamage(this);
                }
            }
        } else {
            if (this->actor.colChkInfo.damageEffect != MINISLIME_DMGEFF_HOOKSHOT) {
                if (this->actor.colChkInfo.damageEffect == MINISLIME_DMGEFF_ICE) {
                    EnMinislime_SetupIceArrowDamage(this);
                } else {
                    EnMinislime_SetupKnockback(this);
                }
            }
        }
    }
}

void EnMinislime_Update(Actor* thisx, PlayState* play) {
    EnMinislime* this = (EnMinislime*)thisx;
    Player* player;
    s32 pad;
    Vec3f vec1;

    if ((this->actor.params == MINISLIME_DEFEAT_IDLE) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        EnMinislime_SetupDefeatIdle(this);
    } else if (this->actor.params == MINISLIME_DEFEAT_MELT) {
        EnMinislime_SetupDefeatMelt(this, play);
    } else if ((this->actor.params == MINISLIME_FORM_BIGSLIME) && (this->actionFunc != EnMinislime_MoveToBigslime)) {
        EnMinislime_SetupMoveToBigslime(this);
    } else {
        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
            this->collider.base.acFlags &= ~AC_HIT;
            return;
        }
        EnMinislime_ApplyDamage(this);
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->attackTimer = 10;
        this->collider.base.atFlags &= ~AT_HIT;
    }

    this->actionFunc(this, play);

    if ((this->actionFunc != EnMinislime_Disappear) && (this->actionFunc != EnMinislime_Despawn)) {
        if (this->actionFunc == EnMinislime_MoveToBigslime) {
            Actor_MoveWithoutGravity(&this->actor);
        } else {
            Actor_MoveWithGravity(&this->actor);
        }

        EnMinislime_CheckBackgroundCollision(this);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        this->collider.dim.radius = this->actor.scale.x * 360.0f;
        this->collider.dim.height = this->actor.scale.y * 800.0f;
        this->collider.dim.yShift = ((EnMinislime*)thisx)->actor.scale.y * -400.0f;

        if ((this->attackTimer == 0) && (this->collider.base.atFlags & AT_ON)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->collider.base.ocFlags1 & OC1_ON) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->attackTimer != 0) {
            this->attackTimer--;
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            player = GET_PLAYER(play);
            vec1.x = this->actor.world.pos.x;
            vec1.z = this->actor.world.pos.z;
            vec1.y = player->actor.world.pos.y + player->actor.depthInWater;
            EffectSsGRipple_Spawn(play, &vec1, 500, 720, 0);
        }
    }
}

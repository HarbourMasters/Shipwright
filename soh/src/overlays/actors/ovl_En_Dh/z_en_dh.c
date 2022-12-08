#include "z_en_dh.h"
#include "objects/object_dh/object_dh.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_10)

typedef enum {
    /* 0 */ DH_WAIT,
    /* 1 */ DH_RETREAT,
    /* 2 */ DH_BURROW,
    /* 3 */ DH_WALK,
    /* 4 */ DH_ATTACK,
    /* 5 */ DH_DEATH,
    /* 6 */ DH_DAMAGE
} EnDhAction;

void EnDh_Init(Actor* this, PlayState* play);
void EnDh_Destroy(Actor* thisx, PlayState* play);
void EnDh_Update(Actor* thisx, PlayState* play);
void EnDh_Draw(Actor* thisx, PlayState* play);

void EnDh_SetupWait(EnDh* this);
void EnDh_SetupWalk(EnDh* this);
void EnDh_SetupAttack(EnDh* this);
void EnDh_SetupBurrow(EnDh* this);

void EnDh_Wait(EnDh* this, PlayState* play);
void EnDh_Walk(EnDh* this, PlayState* play);
void EnDh_Retreat(EnDh* this, PlayState* play);
void EnDh_Attack(EnDh* this, PlayState* play);
void EnDh_Burrow(EnDh* this, PlayState* play);
void EnDh_Damage(EnDh* this, PlayState* play);
void EnDh_Death(EnDh* this, PlayState* play);

const ActorInit En_Dh_InitVars = {
    ACTOR_EN_DH,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DH,
    sizeof(EnDh),
    (ActorFunc)EnDh_Init,
    (ActorFunc)EnDh_Destroy,
    (ActorFunc)EnDh_Update,
    (ActorFunc)EnDh_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 35, 70, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON | OCELEM_UNK3,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
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
    sJntSphElementsInit,
};

static DamageTable D_809EC620 = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(2, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(2, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(4, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x2F, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

void EnDh_SetupAction(EnDh* this, EnDhActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnDh_Init(Actor* thisx, PlayState* play) {
    EnDh* this = (EnDh*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.colChkInfo.damageTable = &D_809EC620;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_dh_Skel_007E88, &object_dh_Anim_005880, this->jointTable,
                       this->limbRotTable, 16);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 64.0f);
    this->actor.params = ENDH_WAIT_UNDERGROUND;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = LINK_IS_ADULT ? 14 : 20;
    this->alpha = this->unk_258 = 255;
    this->actor.flags &= ~ACTOR_FLAG_0;
    Collider_InitCylinder(play, &this->collider1);
    Collider_SetCylinder(play, &this->collider1, &this->actor, &sCylinderInit);
    Collider_InitJntSph(play, &this->collider2);
    Collider_SetJntSph(play, &this->collider2, &this->actor, &sJntSphInit, this->elements);
    EnDh_SetupWait(this);
}

void EnDh_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDh* this = (EnDh*)thisx;

    func_800F5B58();
    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyJntSph(play, &this->collider2);
}

void EnDh_SpawnDebris(PlayState* play, EnDh* this, Vec3f* spawnPos, f32 spread, s32 arg4, f32 accelXZ,
                      f32 scale) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 8.0f, 0.0f };
    Vec3f accel = { 0.0f, -1.5f, 0.0f };
    f32 spreadAngle;
    f32 scaleMod;

    spreadAngle = (Rand_ZeroOne() - 0.5f) * 6.28f;
    pos.y = this->actor.floorHeight;
    pos.x = (Math_SinF(spreadAngle) * spread) + spawnPos->x;
    pos.z = (Math_CosF(spreadAngle) * spread) + spawnPos->z;
    accel.x = (Rand_ZeroOne() - 0.5f) * accelXZ;
    accel.z = (Rand_ZeroOne() - 0.5f) * accelXZ;
    vel.y += (Rand_ZeroOne() - 0.5f) * 4.0f;
    scaleMod = (Rand_ZeroOne() * 5.0f) + 12.0f;
    EffectSsHahen_Spawn(play, &pos, &vel, &accel, arg4, scaleMod * scale, -1, 10, NULL);
}

void EnDh_SetupWait(EnDh* this) {
    Animation_PlayLoop(&this->skelAnime, &object_dh_Anim_003A8C);
    this->curAction = DH_WAIT;
    this->actor.world.pos.x = Rand_CenteredFloat(600.0f) + this->actor.home.pos.x;
    this->actor.world.pos.z = Rand_CenteredFloat(600.0f) + this->actor.home.pos.z;
    this->actor.shape.yOffset = -15000.0f;
    this->dirtWaveSpread = this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.flags |= ACTOR_FLAG_7;
    this->dirtWavePhase = this->actionState = this->actor.params = ENDH_WAIT_UNDERGROUND;
    EnDh_SetupAction(this, EnDh_Wait);
}

void EnDh_Wait(EnDh* this, PlayState* play) {
    if ((s32)this->skelAnime.curFrame == 5) {
        func_800F5ACC(NA_BGM_MINI_BOSS);
    }
    if (Actor_GetCollidedExplosive(play, &this->collider1.base)) {
        this->actor.params = ENDH_START_ATTACK_BOMB;
    }
    if ((this->actor.params >= ENDH_START_ATTACK_GRAB) || (this->actor.params <= ENDH_HANDS_KILLED_4)) {
        switch (this->actionState) {
            case 0:
                this->actor.flags |= ACTOR_FLAG_0;
                this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                this->actor.flags &= ~ACTOR_FLAG_7;
                this->actionState++;
                this->drawDirtWave++;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_HIDE);
            case 1:
                this->dirtWavePhase += 0x3A7;
                Math_SmoothStepToF(&this->dirtWaveSpread, 300.0f, 1.0f, 5.0f, 0.0f);
                this->dirtWaveHeight = Math_SinS(this->dirtWavePhase) * 55.0f;
                this->dirtWaveAlpha = (s16)(Math_SinS(this->dirtWavePhase) * 255.0f);
                EnDh_SpawnDebris(play, this, &this->actor.world.pos, this->dirtWaveSpread, 4, 2.05f, 1.2f);
                if (this->actor.shape.yOffset == 0.0f) {
                    this->drawDirtWave = false;
                    this->actionState++;
                } else if (this->dirtWavePhase > 0x12C0) {
                    this->actor.shape.yOffset += 500.0f;
                }
                break;
            case 2:
                EnDh_SetupWalk(this);
                break;
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x7D0, 0);
        SkelAnime_Update(&this->skelAnime);
        if (this->actor.params != ENDH_START_ATTACK_BOMB) {
            func_8008EEAC(play, &this->actor);
        }
    }
}

void EnDh_SetupWalk(EnDh* this) {
    Animation_Change(&this->skelAnime, &object_dh_Anim_003A8C, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_dh_Anim_003A8C) - 3.0f, ANIMMODE_LOOP, -6.0f);
    this->curAction = DH_WALK;
    this->timer = 300;
    this->actor.speedXZ = 1.0f;
    EnDh_SetupAction(this, EnDh_Walk);
}

void EnDh_Walk(EnDh* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    SkelAnime_Update(&this->skelAnime);
    if (((s32)this->skelAnime.curFrame % 8) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_WALK);
    }
    if ((play->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_LAUGH);
    }
    if (this->actor.xzDistToPlayer <= 100.0f) {
        this->actor.speedXZ = 0.0f;
        if (Actor_IsFacingPlayer(&this->actor, 60 * 0x10000 / 360)) {
            EnDh_SetupAttack(this);
        }
    } else if (--this->timer == 0) {
        EnDh_SetupBurrow(this);
    }
}

void EnDh_SetupRetreat(EnDh* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &object_dh_Anim_005880, -4.0f);
    this->curAction = DH_RETREAT;
    this->timer = 70;
    this->actor.speedXZ = 1.0f;
    EnDh_SetupAction(this, EnDh_Retreat);
}

void EnDh_Retreat(EnDh* this, PlayState* play) {
    this->timer--;
    if (this->timer == 0) {
        this->retreat = false;
        EnDh_SetupBurrow(this);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, (s16)(this->actor.yawTowardsPlayer + 0x8000), 1, 0xBB8, 0);
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    SkelAnime_Update(&this->skelAnime);
}

void EnDh_SetupAttack(EnDh* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_dh_Anim_004658, -6.0f);
    this->timer = this->actionState = 0;
    this->curAction = DH_ATTACK;
    this->actor.speedXZ = 0.0f;
    EnDh_SetupAction(this, EnDh_Attack);
}

void EnDh_Attack(EnDh* this, PlayState* play) {
    s32 pad;

    if (SkelAnime_Update(&this->skelAnime)) {
        this->actionState++;
    } else if ((this->actor.xzDistToPlayer > 100.0f) || !Actor_IsFacingPlayer(&this->actor, 60 * 0x10000 / 360)) {
        Animation_Change(&this->skelAnime, &object_dh_Anim_004658, -1.0f, this->skelAnime.curFrame, 0.0f, ANIMMODE_ONCE,
                         -4.0f);
        this->actionState = 4;
        this->collider2.base.atFlags = this->collider2.elements[0].info.toucherFlags = AT_NONE; // also TOUCH_NONE
        this->collider2.elements[0].info.toucher.dmgFlags = this->collider2.elements[0].info.toucher.damage = 0;
    }
    switch (this->actionState) {
        case 1:
            Animation_PlayOnce(&this->skelAnime, &object_dh_Anim_001A3C);
            this->actionState++;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_BITE);
        case 0:
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x5DC, 0);
            break;
        case 2:
            if (this->skelAnime.curFrame >= 4.0f) {
                this->collider2.base.atFlags = this->collider2.elements[0].info.toucherFlags =
                    AT_ON | AT_TYPE_ENEMY; // also TOUCH_ON | TOUCH_SFX_WOOD
                this->collider2.elements[0].info.toucher.dmgFlags = 0xFFCFFFFF;
                this->collider2.elements[0].info.toucher.damage = 8;
            }
            if (this->collider2.base.atFlags & AT_BOUNCED) {
                this->collider2.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
                this->collider2.base.atFlags = this->collider2.elements[0].info.toucherFlags =
                    AT_NONE; // also TOUCH_NONE
                this->collider2.elements[0].info.toucher.dmgFlags = this->collider2.elements[0].info.toucher.damage = 0;
                this->actionState++;
            } else if (this->collider2.base.atFlags & AT_HIT) {
                this->collider2.base.atFlags &= ~AT_HIT;
                func_8002F71C(play, &this->actor, 8.0f, this->actor.shape.rot.y, 8.0f);
            }
            break;
        case 3:
            if ((this->actor.xzDistToPlayer <= 100.0f) && (Actor_IsFacingPlayer(&this->actor, 60 * 0x10000 / 360))) {
                Animation_Change(&this->skelAnime, &object_dh_Anim_004658, 1.0f, 20.0f,
                                 Animation_GetLastFrame(&object_dh_Anim_004658), ANIMMODE_ONCE, -6.0f);
                this->actionState = 0;
            } else {
                Animation_Change(&this->skelAnime, &object_dh_Anim_004658, -1.0f,
                                 Animation_GetLastFrame(&object_dh_Anim_004658), 0.0f, ANIMMODE_ONCE, -4.0f);
                this->actionState++;
                this->collider2.base.atFlags = this->collider2.elements[0].info.toucherFlags =
                    AT_NONE; // also TOUCH_NONE
                this->collider2.elements[0].info.toucher.dmgFlags = this->collider2.elements[0].info.toucher.damage = 0;
            }
            break;
        case 5:
            EnDh_SetupWalk(this);
            break;
        case 4:
            break;
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnDh_SetupBurrow(EnDh* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_dh_Anim_002148, -6.0f);
    this->curAction = DH_BURROW;
    this->dirtWaveSpread = this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->dirtWavePhase = 0;
    this->actionState = 0;
    this->actor.flags &= ~ACTOR_FLAG_0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_HIDE);
    EnDh_SetupAction(this, EnDh_Burrow);
}

void EnDh_Burrow(EnDh* this, PlayState* play) {
    switch (this->actionState) {
        case 0:
            this->actionState++;
            this->drawDirtWave++;
            this->collider1.base.atFlags = this->collider1.info.toucherFlags =
                AT_ON | AT_TYPE_ENEMY; // also TOUCH_ON | TOUCH_SFX_WOOD
            this->collider1.info.toucher.dmgFlags = 0xFFCFFFFF;
            this->collider1.info.toucher.damage = 4;
        case 1:
            this->dirtWavePhase += 0x47E;
            Math_SmoothStepToF(&this->dirtWaveSpread, 300.0f, 1.0f, 8.0f, 0.0f);
            this->dirtWaveHeight = Math_SinS(this->dirtWavePhase) * 55.0f;
            this->dirtWaveAlpha = (s16)(Math_SinS(this->dirtWavePhase) * 255.0f);
            EnDh_SpawnDebris(play, this, &this->actor.world.pos, this->dirtWaveSpread, 4, 2.05f, 1.2f);
            this->collider1.dim.radius = this->dirtWaveSpread * 0.6f;
            if (SkelAnime_Update(&this->skelAnime)) {
                this->actionState++;
            }
            break;
        case 2:
            this->drawDirtWave = false;
            this->collider1.dim.radius = 35;
            this->collider1.base.atFlags = this->collider1.info.toucherFlags = AT_NONE; // Also TOUCH_NONE
            this->collider1.info.toucher.dmgFlags = this->collider1.info.toucher.damage = 0;
            EnDh_SetupWait(this);
            break;
    }
}

void EnDh_SetupDamage(EnDh* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_dh_Anim_003D6C, -6.0f);
    if (this->actor.bgCheckFlags & 1) {
        this->actor.speedXZ = -1.0f;
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_DAMAGE);
    this->curAction = DH_DAMAGE;
    EnDh_SetupAction(this, EnDh_Damage);
}

void EnDh_Damage(EnDh* this, PlayState* play) {
    if (this->actor.speedXZ < 0.0f) {
        this->actor.speedXZ += 0.15f;
    }
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->retreat) {
            EnDh_SetupRetreat(this, play);
        } else if ((this->actor.xzDistToPlayer <= 105.0f) && Actor_IsFacingPlayer(&this->actor, 60 * 0x10000 / 360)) {
            f32 frames = Animation_GetLastFrame(&object_dh_Anim_004658);

            EnDh_SetupAttack(this);
            Animation_Change(&this->skelAnime, &object_dh_Anim_004658, 1.0f, 20.0f, frames, ANIMMODE_ONCE, -6.0f);
        } else {
            EnDh_SetupWalk(this);
        }
        this->unk_258 = 255;
    }
}

void EnDh_SetupDeath(EnDh* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_dh_Anim_0032BC, -1.0f);
    this->curAction = DH_DEATH;
    this->timer = 300;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.speedXZ = 0.0f;
    func_800F5B58();
    this->actor.params = ENDH_DEATH;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_DEAD);
    EnDh_SetupAction(this, EnDh_Death);
    gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEAD_HAND]++;
}

void EnDh_Death(EnDh* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) || (this->timer != 300)) {
        if (this->timer == 300) {
            Animation_PlayLoop(&this->skelAnime, &object_dh_Anim_00375C);
        }
        this->timer--;
        if (this->timer < 150) {
            if (this->alpha != 0) {
                this->actor.scale.y -= 0.000075f;
                this->actor.shape.shadowAlpha = this->alpha -= 5;
            } else {
                Actor_Kill(&this->actor);
                return;
            }
        }
    } else {
        if (((s32)this->skelAnime.curFrame == 53) || ((s32)this->skelAnime.curFrame == 56) ||
            ((s32)this->skelAnime.curFrame == 61)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
        }
        if ((s32)this->skelAnime.curFrame == 61) {
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        }
    }
}

void EnDh_CollisionCheck(EnDh* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 lastHealth;

    if ((this->collider2.base.acFlags & AC_HIT) && !this->retreat) {
        this->collider2.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect != 0) && (this->actor.colChkInfo.damageEffect != 6)) {
            this->collider2.base.atFlags = this->collider2.elements[0].info.toucherFlags = AT_NONE; // also TOUCH_NONE
            this->collider2.elements[0].info.toucher.dmgFlags = this->collider2.elements[0].info.toucher.damage = 0;
            if (player->unk_844 != 0) {
                this->unk_258 = player->unk_845;
            }
            Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
            lastHealth = this->actor.colChkInfo.health;
            if (Actor_ApplyDamage(&this->actor) == 0) {
                EnDh_SetupDeath(this);
                Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
            } else {
                if (((lastHealth >= 15) && (this->actor.colChkInfo.health < 15)) ||
                    ((lastHealth >= 9) && (this->actor.colChkInfo.health < 9)) ||
                    ((lastHealth >= 3) && (this->actor.colChkInfo.health < 3))) {

                    this->retreat++;
                }
                EnDh_SetupDamage(this);
            }
        }
    }
}

void EnDh_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDh* this = (EnDh*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad40;

    EnDh_CollisionCheck(this, play);
    this->actionFunc(this, play);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 45.0f, 45.0f, 0x1D);
    this->actor.focus.pos = this->headPos;
    Collider_UpdateCylinder(&this->actor, &this->collider1);
    if (this->actor.colChkInfo.health > 0) {
        if (this->curAction == DH_WAIT) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
        } else {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        }
        if (((this->curAction != DH_DAMAGE) && (this->actor.shape.yOffset == 0.0f)) ||
            ((player->unk_844 != 0) && (player->unk_845 != this->unk_258))) {

            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
        }
    } else {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider2.base);
    }
}

void EnDh_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    Vec3f headOffset = { 2000.0f, 1000.0f, 0.0f };
    EnDh* this = (EnDh*)thisx;

    if (limbIndex == 13) {
        Matrix_MultVec3f(&headOffset, &this->headPos);
        Matrix_Push();
        Matrix_Translate(headOffset.x, headOffset.y, headOffset.z, MTXMODE_APPLY);
        Collider_UpdateSpheres(1, &this->collider2);
        Matrix_Pop();
    }
}

void EnDh_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDh* this = (EnDh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    if (this->alpha == 255) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                               this->skelAnime.dListCount, NULL, EnDh_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_XLU_DISP++, 0x08, &D_80116280[0]);
        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);
    }
    if (this->drawDirtWave) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 85, 55, 0, 130);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, (play->state.frames * -3) % 0x80, 0, 0x20, 0x40, 1,
                                    (play->state.frames * -10) % 0x80, (play->state.frames * -20) % 0x100,
                                    0x20, 0x40));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 0, 0, this->dirtWaveAlpha);

        Matrix_Translate(0.0f, -this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(this->dirtWaveSpread * 0.01f, this->dirtWaveHeight * 0.01f, this->dirtWaveSpread * 0.01f,
                     MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, object_dh_DL_007FC0);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

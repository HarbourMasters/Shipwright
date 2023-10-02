/*
 * File: z_en_sb.c
 * Overlay: ovl_En_Sb
 * Description: Shellblade
 */

#include "z_en_sb.h"
#include "vt.h"
#include "objects/object_sb/object_sb.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)

void EnSb_Init(Actor* thisx, PlayState* play);
void EnSb_Destroy(Actor* thisx, PlayState* play);
void EnSb_Update(Actor* thisx, PlayState* play);
void EnSb_Draw(Actor* thisx, PlayState* play);

void EnSb_SetupWaitClosed(EnSb* this);

void EnSb_WaitClosed(EnSb* this, PlayState* play);
void EnSb_Open(EnSb* this, PlayState* play);
void EnSb_WaitOpen(EnSb* this, PlayState* play);
void EnSb_TurnAround(EnSb* this, PlayState* play);
void EnSb_Lunge(EnSb* this, PlayState* play);
void EnSb_Bounce(EnSb* this, PlayState* play);
void EnSb_Cooldown(EnSb* this, PlayState* play);

const ActorInit En_Sb_InitVars = {
    ACTOR_EN_SB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SB,
    sizeof(EnSb),
    (ActorFunc)EnSb_Init,
    (ActorFunc)EnSb_Destroy,
    (ActorFunc)EnSb_Update,
    (ActorFunc)EnSb_Draw,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0xFFCFFFFF, 0x04, 0x08 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x01, 0x01, 0x01 },
    { 30, 40, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xD),
    /* Master sword  */ DMG_ENTRY(2, 0xD),
    /* Giant's Knife */ DMG_ENTRY(4, 0xD),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xF),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xD),
    /* Giant spin    */ DMG_ENTRY(4, 0xD),
    /* Master spin   */ DMG_ENTRY(2, 0xD),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xD),
    /* Giant jump    */ DMG_ENTRY(8, 0xD),
    /* Master jump   */ DMG_ENTRY(4, 0xD),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x27, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

static Vec3f sFlamePosOffsets[] = {
    { 5.0f, 0.0f, 0.0f },
    { -5.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 5.0f },
    { 0.0f, 0.0f, -5.0f },
};

typedef enum {
    /* 0x00 */ SHELLBLADE_OPEN,
    /* 0x01 */ SHELLBLADE_WAIT_CLOSED,
    /* 0x02 */ SHELLBLADE_WAIT_OPEN,
    /* 0x03 */ SHELLBLADE_LUNGE,
    /* 0x04 */ SHELLBLADE_BOUNCE
} ShellbladeBehavior;

void EnSb_Init(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.colChkInfo.damageTable = sDamageTable;
    this->actor.colChkInfo.health = 2;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_sb_Skel_002BF0, &object_sb_Anim_000194, NULL, NULL, 0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    this->isDead = false;
    this->actor.colChkInfo.mass = 0;
    Actor_SetScale(&this->actor, 0.006f);
    this->actor.shape.rot.y = 0;
    this->actor.speedXZ = 0.0f;
    this->actor.gravity = -0.35f;
    this->fire = 0;
    this->hitByWindArrow = false;
    this->actor.velocity.y = -1.0f;
    EnSb_SetupWaitClosed(this);
}

void EnSb_Destroy(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;
    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->collider);
}

void EnSb_SpawnBubbles(PlayState* play, EnSb* this) {
    s32 i;

    if (this->actor.yDistToWater > 0) {
        for (i = 0; i < 10; i++) {
            EffectSsBubble_Spawn(play, &this->actor.world.pos, 10.0f, 10.0f, 30.0f, 0.25f);
        }
    }
}

void EnSb_SetupWaitClosed(EnSb* this) {
    Animation_Change(&this->skelAnime, &object_sb_Anim_00004C, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_00004C),
                     ANIMMODE_ONCE, 0.0f);
    this->behavior = SHELLBLADE_WAIT_CLOSED;
    this->actionFunc = EnSb_WaitClosed;
}

void EnSb_SetupOpen(EnSb* this) {
    Animation_Change(&this->skelAnime, &object_sb_Anim_000194, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_000194),
                     ANIMMODE_ONCE, 0.0f);
    this->behavior = SHELLBLADE_OPEN;
    this->actionFunc = EnSb_Open;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHELL_MOUTH);
}

void EnSb_SetupWaitOpen(EnSb* this) {
    Animation_Change(&this->skelAnime, &object_sb_Anim_002C8C, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_002C8C),
                     ANIMMODE_LOOP, 0.0f);
    this->behavior = SHELLBLADE_WAIT_OPEN;
    this->actionFunc = EnSb_WaitOpen;
}

void EnSb_SetupLunge(EnSb* this) {
    f32 frameCount = Animation_GetLastFrame(&object_sb_Anim_000124);
    f32 playbackSpeed = this->actor.yDistToWater > 0.0f ? 1.0f : 0.0f;

    Animation_Change(&this->skelAnime, &object_sb_Anim_000124, playbackSpeed, 0.0f, frameCount, ANIMMODE_ONCE, 0);
    this->behavior = SHELLBLADE_LUNGE;
    this->actionFunc = EnSb_Lunge;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_SHELL_MOUTH);
}

void EnSb_SetupBounce(EnSb* this) {
    Animation_Change(&this->skelAnime, &object_sb_Anim_0000B4, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_0000B4),
                     ANIMMODE_ONCE, 0.0f);
    this->behavior = SHELLBLADE_BOUNCE;
    this->actionFunc = EnSb_Bounce;
}

void EnSb_SetupCooldown(EnSb* this, s32 changeSpeed) {
    f32 frameCount = Animation_GetLastFrame(&object_sb_Anim_00004C);

    if (this->behavior != SHELLBLADE_WAIT_CLOSED) {
        Animation_Change(&this->skelAnime, &object_sb_Anim_00004C, 1.0f, 0, frameCount, ANIMMODE_ONCE, 0.0f);
    }
    this->behavior = SHELLBLADE_WAIT_CLOSED;
    if (changeSpeed) {
        if (this->actor.yDistToWater > 0.0f) {
            this->actor.speedXZ = -5.0f;
            if (this->actor.velocity.y < 0.0f) {
                this->actor.velocity.y = 2.1f;
            }
        } else {
            this->actor.speedXZ = -6.0f;
            if (this->actor.velocity.y < 0.0f) {
                this->actor.velocity.y = 1.4f;
            }
        }
    }
    this->timer = 60;
    this->actionFunc = EnSb_Cooldown;
}

void EnSb_WaitClosed(EnSb* this, PlayState* play) {
    // always face toward link
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);

    if ((this->actor.xzDistToPlayer <= 160.0f) && (this->actor.xzDistToPlayer > 40.0f)) {
        EnSb_SetupOpen(this);
    }
}

void EnSb_Open(EnSb* this, PlayState* play) {
    f32 currentFrame = this->skelAnime.curFrame;

    if (Animation_GetLastFrame(&object_sb_Anim_000194) <= currentFrame) {
        this->timer = 15;
        EnSb_SetupWaitOpen(this);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);
        if ((this->actor.xzDistToPlayer > 160.0f) || (this->actor.xzDistToPlayer <= 40.0f)) {
            EnSb_SetupWaitClosed(this);
        }
    }
}

void EnSb_WaitOpen(EnSb* this, PlayState* play) {
    s16 timer = this->timer;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);

    if ((this->actor.xzDistToPlayer > 160.0f) || (this->actor.xzDistToPlayer <= 40.0f)) {
        EnSb_SetupWaitClosed(this);
    }

    if (timer != 0) {
        this->timer = timer - 1;
    } else {
        this->timer = 0;
        this->attackYaw = this->actor.yawTowardsPlayer;
        this->actionFunc = EnSb_TurnAround;
    }
}

void EnSb_TurnAround(EnSb* this, PlayState* play) {
    s16 invertedYaw;

    invertedYaw = this->attackYaw + 0x8000;
    Math_SmoothStepToS(&this->actor.shape.rot.y, invertedYaw, 0x1, 0x1F40, 0xA);

    if (this->actor.shape.rot.y == invertedYaw) {
        this->actor.world.rot.y = this->attackYaw;
        if (this->actor.yDistToWater > 0.0f) {
            this->actor.velocity.y = 3.0f;
            this->actor.speedXZ = 5.0f;
            this->actor.gravity = -0.35f;
        } else {
            this->actor.velocity.y = 2.0f;
            this->actor.speedXZ = 6.0f;
            this->actor.gravity = -2.0f;
        }
        EnSb_SpawnBubbles(play, this);
        this->bouncesLeft = 3;
        EnSb_SetupLunge(this);
        // "Attack!!"
        osSyncPrintf("アタァ〜ック！！\n");
    }
}

void EnSb_Lunge(EnSb* this, PlayState* play) {
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.2f);
    if ((this->actor.velocity.y <= -0.1f) || ((this->actor.bgCheckFlags & 2))) {
        if (!(this->actor.yDistToWater > 0.0f)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        }
        this->actor.bgCheckFlags = this->actor.bgCheckFlags & ~2;
        EnSb_SetupBounce(this);
    }
}

void EnSb_Bounce(EnSb* this, PlayState* play) {
    s32 pad;
    f32 currentFrame;
    f32 frameCount;

    currentFrame = this->skelAnime.curFrame;
    frameCount = Animation_GetLastFrame(&object_sb_Anim_0000B4);
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.2f);

    if (currentFrame == frameCount) {
        if (this->bouncesLeft != 0) {
            this->bouncesLeft--;
            this->timer = 1;
            if (this->actor.yDistToWater > 0.0f) {
                this->actor.velocity.y = 3.0f;
                this->actor.speedXZ = 5.0f;
                this->actor.gravity = -0.35f;
            } else {
                this->actor.velocity.y = 2.0f;
                this->actor.speedXZ = 6.0f;
                this->actor.gravity = -2.0f;
            }
            EnSb_SpawnBubbles(play, this);
            EnSb_SetupLunge(this);
        } else if (this->actor.bgCheckFlags & 1) {
            this->actor.bgCheckFlags &= ~2;
            this->actor.speedXZ = 0.0f;
            this->timer = 1;
            EnSb_SetupWaitClosed(this);
            osSyncPrintf(VT_FGCOL(RED) "攻撃終了！！" VT_RST "\n"); // "Attack Complete!"
        }
    }
}

void EnSb_Cooldown(EnSb* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
        if (this->actor.bgCheckFlags & 1) {
            this->actor.bgCheckFlags &= ~1;
            this->actor.speedXZ = 0.0f;
        }
    } else {
        if (this->actor.bgCheckFlags & 1) {
            this->actor.bgCheckFlags &= ~1;
            this->actionFunc = EnSb_WaitClosed;
            this->actor.speedXZ = 0.0f;
        }
    }
}

s32 EnSb_IsVulnerable(EnSb* this) {
    switch (this->behavior) {
        case SHELLBLADE_OPEN:
            if ((this->skelAnime.curFrame >= 2.0f) && (this->skelAnime.curFrame <= 5.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_WAIT_CLOSED:
            if ((this->skelAnime.curFrame >= 0.0f) && (this->skelAnime.curFrame <= 1.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_WAIT_OPEN:
            if ((this->skelAnime.curFrame >= 0.0f) && (this->skelAnime.curFrame <= 19.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_LUNGE:
            if (this->skelAnime.curFrame == 0.0f) {
                return true;
            }
            break;
        case SHELLBLADE_BOUNCE:
            if ((this->skelAnime.curFrame >= 3.0f) && (this->skelAnime.curFrame <= 5.0f)) {
                return true;
            }
            break;
    }
    return false;
}

s32 EnSb_UpdateDamage(EnSb* this, PlayState* play) {
    Vec3f hitPoint;
    f32 hitY;
    s16 yawDiff;
    s32 tookDamage;
    u8 hitByWindArrow;

    // hit box collided, switch to cool down
    if ((this->collider.base.atFlags & AT_HIT)) {
        EnSb_SetupCooldown(this, 1);
        return 1;
    }

    // hurt box collided, take damage if appropriate
    if ((this->collider.base.acFlags & AC_HIT)) {
        hitByWindArrow = false;
        tookDamage = false;
        this->collider.base.acFlags &= ~AC_HIT;

        switch (this->actor.colChkInfo.damageEffect) {
            case 14: // wind arrow
                hitByWindArrow = true;
            case 15: // explosions, arrow, hammer, ice arrow, light arrow, spirit arrow, shadow arrow
                if (EnSb_IsVulnerable(this)) {
                    hitY = this->collider.info.bumper.hitPos.y - this->actor.world.pos.y;
                    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    if ((hitY < 30.0f) && (hitY > 10.0f) && (yawDiff >= -0x1FFF) && (yawDiff < 0x2000)) {
                        Actor_ApplyDamage(&this->actor);
                        Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0x2000, 0x50);
                        tookDamage = true;
                    }
                }
                break;
            case 2: // fire arrow, dins fire
                this->fire = 4;
                Actor_ApplyDamage(&this->actor);
                Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0x2000, 0x50);
                tookDamage = true;
                break;
            case 1:  // hookshot/longshot
            case 13: // all sword damage
                if (EnSb_IsVulnerable(this)) {
                    hitY = this->collider.info.bumper.hitPos.y - this->actor.world.pos.y;
                    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    if ((hitY < 30.0f) && (hitY > 10.0f) && (yawDiff >= -0x1FFF) && (yawDiff < 0x2000)) {
                        Actor_ApplyDamage(&this->actor);
                        Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0x2000, 0x50);
                        tookDamage = true;
                        EnSb_SetupCooldown(this, 0);
                    }
                }
                break;
            default:
                break;
        }
        if (this->actor.colChkInfo.health == 0) {
            this->hitByWindArrow = hitByWindArrow;
            BodyBreak_Alloc(&this->bodyBreak, 8, play);
            this->isDead = true;
            Enemy_StartFinishingBlow(play, &this->actor);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_SHELL_DEAD);
            return 1;
        }

        // if player attack didn't do damage, play recoil sound and spawn sparks
        if (!tookDamage) {
            hitPoint.x = this->collider.info.bumper.hitPos.x;
            hitPoint.y = this->collider.info.bumper.hitPos.y;
            hitPoint.z = this->collider.info.bumper.hitPos.z;
            CollisionCheck_SpawnShieldParticlesMetal2(play, &hitPoint);
        }
    }

    return 0;
}

void EnSb_Update(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;
    s32 pad;

    if (this->isDead) {
        if (this->actor.yDistToWater > 0.0f) {
            this->actor.params = 4;
        } else {
            this->actor.params = 1;
        }
        if (BodyBreak_SpawnParts(&this->actor, &this->bodyBreak, play, this->actor.params)) {
            if (!this->hitByWindArrow) {
                Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x80);
            } else {
                Item_DropCollectible(play, &this->actor.world.pos, 8);
            }
            GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
            Actor_Kill(&this->actor);
        }
    } else {
        Actor_SetFocus(&this->actor, 20.0f);
        Actor_SetScale(&this->actor, 0.006f);
        Actor_MoveForward(&this->actor);
        this->actionFunc(this, play);
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, 5);
        EnSb_UpdateDamage(this, play);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        SkelAnime_Update(&this->skelAnime);
    }
}

void EnSb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnSb* this = (EnSb*)thisx;

    BodyBreak_SetInfo(&this->bodyBreak, limbIndex, 0, 6, 8, dList, BODYBREAK_OBJECT_DEFAULT);
}

void EnSb_Draw(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;
    Vec3f flamePos;
    Vec3f* offset;
    s16 fireDecr;

    func_8002EBCC(&this->actor, play, 1);
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, NULL, EnSb_PostLimbDraw, this);
    if (this->fire != 0) {
        this->actor.colorFilterTimer++;
        fireDecr = this->fire - 1;
        // this is intended to draw flames after being burned, but the condition is never met to run this code
        // fire gets set to 4 when burned, decrements to 3 and fails the "& 1" check and never stores the decrement
        if ((fireDecr & 1) == 0) {
            offset = &sFlamePosOffsets[(fireDecr & 3)];
            flamePos.x = Rand_CenteredFloat(5.0f) + (this->actor.world.pos.x + offset->x);
            flamePos.y = Rand_CenteredFloat(5.0f) + (this->actor.world.pos.y + offset->y);
            flamePos.z = Rand_CenteredFloat(5.0f) + (this->actor.world.pos.z + offset->z);
            EffectSsEnFire_SpawnVec3f(play, &this->actor, &flamePos, 100, 0, 0, -1);
        }
    }
}

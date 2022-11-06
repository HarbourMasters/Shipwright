/*
 * File: z_en_tp.c
 * Overlay: ovl_En_Tp
 * Description: Electric Tailpasaran
 */

#include "z_en_tp.h"
#include "objects/object_tp/object_tp.h"

#define FLAGS 0

void EnTp_Init(Actor* thisx, PlayState* play);
void EnTp_Destroy(Actor* thisx, PlayState* play);
void EnTp_Update(Actor* thisx, PlayState* play);
void EnTp_Draw(Actor* thisx, PlayState* play);

void EnTp_Tail_SetupFollowHead(EnTp* this);
void EnTp_Tail_FollowHead(EnTp* this, PlayState* play);
void EnTp_Head_SetupApproachPlayer(EnTp* this);
void EnTp_Head_ApproachPlayer(EnTp* this, PlayState* play);
void EnTp_SetupDie(EnTp* this);
void EnTp_Die(EnTp* this, PlayState* play);
void EnTp_Fragment_SetupFade(EnTp* this);
void EnTp_Fragment_Fade(EnTp* this, PlayState* play);
void EnTp_Head_SetupTakeOff(EnTp* this);
void EnTp_Head_TakeOff(EnTp* this, PlayState* play);
void EnTp_Head_SetupWait(EnTp* this);
void EnTp_Head_Wait(EnTp* this, PlayState* play);
void EnTp_Head_SetupBurrowReturnHome(EnTp* this);
void EnTp_Head_BurrowReturnHome(EnTp* this, PlayState* play);

typedef enum {
    /* 0 */ TAILPASARAN_ACTION_FRAGMENT_FADE,
    /* 1 */ TAILPASARAN_ACTION_DIE,
    /* 2 */ TAILPASARAN_ACTION_TAIL_FOLLOWHEAD,
    /* 4 */ TAILPASARAN_ACTION_HEAD_WAIT = 4,
    /* 7 */ TAILPASARAN_ACTION_HEAD_APPROACHPLAYER = 7,
    /* 8 */ TAILPASARAN_ACTION_HEAD_TAKEOFF,
    /* 9 */ TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME
} TailpasaranAction;

const ActorInit En_Tp_InitVars = {
    ACTOR_EN_TP,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_TP,
    sizeof(EnTp),
    (ActorFunc)EnTp_Init,
    (ActorFunc)EnTp_Destroy,
    (ActorFunc)EnTp_Update,
    (ActorFunc)EnTp_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x03, 0x08 },
            { 0xFFCFFFFF, 0x01, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 4 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT1,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

typedef enum {
    /* 00 */ TAILPASARAN_DMGEFF_NONE,
    /* 01 */ TAILPASARAN_DMGEFF_DEKUNUT,
    /* 14 */ TAILPASARAN_DMGEFF_SHOCKING = 14, // Kills the Tailpasaran but shocks Player
    /* 15 */ TAILPASARAN_DMGEFF_INSULATING     // Kills the Tailpasaran and does not shock Player
} TailpasaranDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_INSULATING),
    /* Slingshot     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_INSULATING),
    /* Normal arrow  */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Hookshot      */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master sword  */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant's Knife */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Fire arrow    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Ice arrow     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Ice magic     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant spin    */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master spin   */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Kokiri jump   */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant jump    */ DMG_ENTRY(8, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master jump   */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Unknown 1     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Unknown 2     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 10, ICHAIN_STOP),
};

void EnTp_SetupAction(EnTp* this, EnTpActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnTp_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTp* this = (EnTp*)thisx;
    EnTp* now;
    EnTp* next;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.targetMode = 3;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.14f);
    this->unk_150 = 0;
    this->actor.colChkInfo.health = 1;
    now = this;
    this->alpha = 255;
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);

    if (this->actor.params <= TAILPASARAN_HEAD) {
        this->actor.naviEnemyId = 0x06;
        this->timer = 0;
        this->collider.base.acFlags |= AC_HARD;
        this->collider.elements->dim.modelSphere.radius = this->collider.elements->dim.worldSphere.radius = 8;
        EnTp_Head_SetupWait(this);
        this->actor.focus.pos = this->actor.world.pos;
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4;
        Actor_SetScale(&this->actor, 1.5f);

        for (i = 0; i <= 6; i++) {
            next = (EnTp*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TP, this->actor.world.pos.x,
                                      this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0 * i);

            if (0 * i) {} // Very fake, but needed to get the s registers right
            if (next != NULL) {
                now->actor.child = &next->actor;
                next->actor.parent = &now->actor;
                next->kiraSpawnTimer = i + 1;
                next->head = this;
                Actor_SetScale(&next->actor, 0.3f);

                if (i == 2) {
                    next->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4;
                    next->unk_150 = 1; // Why?
                }

                next->timer = next->unk_15C = i * -5;
                next->horizontalVariation = 6.0f - (i * 0.75f);
                now = next;
                if (0 * i) {}
            }
        }
    } else if (this->actor.params == TAILPASARAN_TAIL) {
        EnTp_Tail_SetupFollowHead(this);
    } else {
        EnTp_Fragment_SetupFade(this);
    }
}

void EnTp_Destroy(Actor* thisx, PlayState* play) {
    EnTp* this = (EnTp*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnTp_Tail_SetupFollowHead(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_TAIL_FOLLOWHEAD;
    EnTp_SetupAction(this, EnTp_Tail_FollowHead);
}

void EnTp_Tail_FollowHead(EnTp* this, PlayState* play) {
    s16 angle;
    s16 phase;

    if (this->actor.params == TAILPASARAN_TAIL_DYING) {
        this->actionIndex = TAILPASARAN_ACTION_DIE;

        if (this->actor.parent == NULL) {
            EnTp_SetupDie(this);
        }
    } else {
        if (this->unk_150 != 0) {
            this->actor.flags |= ACTOR_FLAG_0;
        }

        if (this->head->unk_150 != 0) {
            this->actor.speedXZ = this->red = this->actor.velocity.y = this->heightPhase = 0.0f;
            if (this->actor.world.pos.y < this->head->actor.home.pos.y) {
                this->actor.flags &= ~ACTOR_FLAG_0;
            }

            this->actor.world.pos = this->actor.parent->prevPos;
        } else {
            Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.parent->world.pos.y - 4.0f, 1.0f, 1.0f, 0.0f);
            angle = this->head->actor.shape.rot.y + 0x4000;
            phase = 2000 * (this->head->unk_15C + this->timer);
            this->actor.world.pos.x =
                this->actor.home.pos.x + Math_SinS(phase) * (Math_SinS(angle) * this->horizontalVariation);
            this->actor.world.pos.z =
                this->actor.home.pos.z + Math_SinS(phase) * (Math_CosS(angle) * this->horizontalVariation);
        }
    }
}

void EnTp_Head_SetupApproachPlayer(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_HEAD_APPROACHPLAYER;
    this->timer = 200;
    EnTp_SetupAction(this, EnTp_Head_ApproachPlayer);
}

void EnTp_Head_ApproachPlayer(EnTp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->actor.world.pos.y, player->actor.world.pos.y + 30.0f, 1.0f, 0.5f, 0.0f);
    Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (&player->actor == this->collider.base.at) {
            this->timer = 1;
        }
    }

    if (this->red < 255) {
        this->red += 15;
    }

    if (Math_CosF(this->heightPhase) == 0.0f) {
        this->extraHeightVariation = 2.0f * Rand_ZeroOne();
    }

    this->actor.world.pos.y += Math_CosF(this->heightPhase) * (2.0f + this->extraHeightVariation);
    this->heightPhase += 0.2f;
    Math_SmoothStepToF(&this->actor.speedXZ, 2.5f, 0.1f, 0.2f, 0.0f);
    this->timer--;

    if (this->timer != 0) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 750, 0);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    } else {
        EnTp_Head_SetupBurrowReturnHome(this);
    }
}

void EnTp_SetupDie(EnTp* this) {
    Actor* now;

    this->timer = 2;

    if (this->actor.params <= TAILPASARAN_HEAD) {
        for (now = this->actor.child; now != NULL; now = now->child) {
            now->params = TAILPASARAN_TAIL_DYING;
            now->colChkInfo.health = 0;
        }

        this->timer = 13;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_TAIL_DEAD);
    }
    this->actionIndex = TAILPASARAN_ACTION_DIE;
    EnTp_SetupAction(this, EnTp_Die);
}

/**
 * Spawns effects and smaller tail segment-like fragments
 */
void EnTp_Die(EnTp* this, PlayState* play) {
    EnTp* now;
    s16 i;
    s32 pad;
    Vec3f effectVelAccel = { 0.0f, 0.5f, 0.0f };
    Vec3f effectPos = { 0.0f, 0.0f, 0.0f };

    this->timer--;

    if (this->timer <= 0) {
        if (this->actor.params == TAILPASARAN_HEAD_DYING) {
            effectPos.x = ((Rand_ZeroOne() - 0.5f) * 15.0f) + this->actor.world.pos.x;
            effectPos.z = ((Rand_ZeroOne() - 0.5f) * 15.0f) + this->actor.world.pos.z;
            effectPos.y = ((Rand_ZeroOne() - 0.5f) * 5.0f) + this->actor.world.pos.y;
            EffectSsDeadDb_Spawn(play, &effectPos, &effectVelAccel, &effectVelAccel, 100, 0, 255, 255, 255, 255, 0,
                                 0, 255, 1, 9, 1);

            effectPos.x = ((Rand_ZeroOne() - 0.5f) * 15.0f) + this->actor.world.pos.x;
            effectPos.z = ((Rand_ZeroOne() - 0.5f) * 15.0f) + this->actor.world.pos.z;
            effectPos.y = ((Rand_ZeroOne() - 0.5f) * 5.0f) + this->actor.world.pos.y;
            EffectSsDeadDb_Spawn(play, &effectPos, &effectVelAccel, &effectVelAccel, 100, 0, 255, 255, 255, 255, 0,
                                 0, 255, 1, 9, 1);
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x50);
        } else {
            for (i = 0; i < 1; i++) {
                now =
                    (EnTp*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TP, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, TAILPASARAN_FRAGMENT);

                if (now != NULL) {
                    Actor_SetScale(&now->actor, this->actor.scale.z * 0.5f);
                    now->red = this->red;
                }
            }
        }

        if (this->actor.child != NULL) {
            this->actor.child->parent = NULL;
            this->actor.child->params = TAILPASARAN_TAIL_DYING;
            this->actor.child->colChkInfo.health = 0;
        }

        this->unk_150 = 2;
        Actor_Kill(&this->actor);
    }
}

void EnTp_Fragment_SetupFade(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_FRAGMENT_FADE;
    this->actor.world.pos.x += ((Rand_ZeroOne() - 0.5f) * 5.0f);
    this->actor.world.pos.y += ((Rand_ZeroOne() - 0.5f) * 5.0f);
    this->actor.world.pos.z += ((Rand_ZeroOne() - 0.5f) * 5.0f);
    this->actor.velocity.x = (Rand_ZeroOne() - 0.5f) * 1.5f;
    this->actor.velocity.y = (Rand_ZeroOne() - 0.5f) * 1.5f;
    this->actor.velocity.z = (Rand_ZeroOne() - 0.5f) * 1.5f;
    this->actor.flags &= ~ACTOR_FLAG_0;
    EnTp_SetupAction(this, EnTp_Fragment_Fade);
}

void EnTp_Fragment_Fade(EnTp* this, PlayState* play) {
    func_8002D7EC(&this->actor);
    this->alpha -= 20;

    if (this->alpha < 20) {
        this->alpha = 0;
        Actor_Kill(&this->actor);
    }
}

void EnTp_Head_SetupTakeOff(EnTp* this) {
    this->timer = (Rand_ZeroOne() * 15.0f) + 40.0f;
    this->actionIndex = TAILPASARAN_ACTION_HEAD_TAKEOFF;
    EnTp_SetupAction(this, EnTp_Head_TakeOff);
}

/**
 * Flies up and loops around until it makes for Player
 */
void EnTp_Head_TakeOff(EnTp* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->actor.speedXZ, 2.5f, 0.1f, 0.2f, 0.0f);
    Math_SmoothStepToF(&this->actor.world.pos.y, player->actor.world.pos.y + 85.0f + this->horizontalVariation, 1.0f,
                       this->actor.speedXZ * 0.25f, 0.0f);
    Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (&player->actor == this->collider.base.at) {
            this->unk_15C = 1;
        }
    }

    if (this->red != 0) {
        this->red -= 15;
    }

    if (Math_CosF(this->heightPhase) == 0.0f) {
        this->extraHeightVariation = Rand_ZeroOne() * 4.0f;
    }

    this->actor.world.pos.y +=
        Math_CosF(this->heightPhase) * ((this->actor.speedXZ * 0.25f) + this->extraHeightVariation);
    this->actor.world.rot.y += this->unk_164;
    this->heightPhase += 0.2f;

    if (this->timer != 0) {
        this->timer--;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos), 1, 750,
                       0);

    if (this->timer == 0) {
        EnTp_Head_SetupApproachPlayer(this);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnTp_Head_SetupWait(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_HEAD_WAIT;
    this->unk_150 = 0;
    this->actor.shape.rot.x = -0x4000;
    this->timer = 60;
    this->unk_15C = 0;
    this->actor.speedXZ = 0.0f;
    EnTp_SetupAction(this, EnTp_Head_Wait);
}

/**
 * Awaken and rise from the ground when Player is closer than 200
 */
void EnTp_Head_Wait(EnTp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw;

    this->unk_15C--;

    if (this->actor.xzDistToPlayer < 200.0f) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            if (&player->actor == this->collider.base.at) {
                this->timer = 0;
            }
        }

        if (this->timer != 0) {
            this->timer--;

            Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 500, 0);
            Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 1500, 0);

            yaw = Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos) + 0x4000;
            Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y + 30.0f, 0.3f, 1.0f, 0.3f);
            this->actor.world.pos.x = this->actor.home.pos.x +
                                      (Math_SinS(2000 * this->unk_15C) * (Math_SinS(yaw) * this->horizontalVariation));
            this->actor.world.pos.z = this->actor.home.pos.z +
                                      (Math_SinS(2000 * this->unk_15C) * (Math_CosS(yaw) * this->horizontalVariation));
        } else {
            this->actor.shape.rot.x = 0;
            this->unk_150 = 1;
            EnTp_Head_SetupTakeOff(this);
        }
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.x, -0x4000, 1, 500, 0);

        if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.3f, 1.5f, 0.3f) == 0.0f) {
            this->timer = 60;
        } else {
            yaw = Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos);
            this->actor.world.pos.x =
                this->actor.home.pos.x + (Math_SinS(2000 * this->unk_15C) * (Math_SinS(yaw) * 6.0f));
            this->actor.world.pos.z =
                this->actor.home.pos.z + (Math_SinS(2000 * this->unk_15C) * (Math_CosS(yaw) * 6.0f));
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.world.pos.y != this->actor.home.pos.y) {
        Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void EnTp_Head_SetupBurrowReturnHome(EnTp* this) {
    this->actionIndex = TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME;
    this->timer = 0;
    EnTp_SetupAction(this, EnTp_Head_BurrowReturnHome);
}

void EnTp_Head_BurrowReturnHome(EnTp* this, PlayState* play) {
    static Vec3f bubbleAccel = { 0.0f, -0.5f, 0.0f };
    static Color_RGBA8 bubblePrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 bubbleEnvColor = { 150, 150, 150, 0 };
    Vec3f bubbleVelocity;
    Vec3f bubblePos;
    s32 closeToFloor;
    EnTp* now;
    s16 temp_v0; // Required to match, usage can maybe be improved

    closeToFloor = false;
    temp_v0 = this->timer;
    this->unk_15C--;

    if ((temp_v0 != 0) || ((this->actor.home.pos.y - this->actor.world.pos.y) > 60.0f)) {
        this->timer = temp_v0 - 1;
        temp_v0 = this->timer;

        if (temp_v0 == 0) {
            EnTp_Head_SetupWait(this);

            for (now = (EnTp*)this->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                now->unk_15C = now->timer;
            }
        } else {
            if (this->actor.shape.rot.x != -0x4000) {
                this->timer = 80;
                this->actor.velocity.y = 0.0f;
                this->actor.speedXZ = 0.0f;
                this->actor.world.pos = this->actor.home.pos;
                this->actor.shape.rot.x = -0x4000;

                for (now = (EnTp*)this->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                    now->actor.velocity.y = 0.0f;
                    now->actor.speedXZ = 0.0f;
                    now->actor.world.pos = this->actor.home.pos;
                    now->actor.world.pos.y = this->actor.home.pos.y - 80.0f;
                }
            }

            this->actor.world.pos.y = this->actor.home.pos.y - this->timer;
        }
    } else {
        if (this->actor.shape.rot.x != 0x4000) {
            this->actor.shape.rot.x -= 0x400;
        }

        if (this->red != 0) {
            this->red -= 15;
        }

        this->actor.speedXZ = 2.0f * Math_CosS(this->actor.shape.rot.x);
        this->actor.velocity.y = Math_SinS(this->actor.shape.rot.x) * -2.0f;

        if ((this->actor.world.pos.y - this->actor.floorHeight) < 20.0f) {
            closeToFloor = true;
        }

        if (this->actor.world.pos.y != this->actor.home.pos.y) {
            Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }

        if (closeToFloor && ((play->gameplayFrames & 1) != 0)) {
            bubblePos = this->actor.world.pos;
            bubblePos.y = this->actor.floorHeight;

            bubbleVelocity.x = Rand_CenteredFloat(5.0f);
            bubbleVelocity.y = (Rand_ZeroOne() * 3.5f) + 1.5f;
            bubbleVelocity.z = Rand_CenteredFloat(5.0f);

            EffectSsDtBubble_SpawnCustomColor(play, &bubblePos, &bubbleVelocity, &bubbleAccel, &bubblePrimColor,
                                              &bubbleEnvColor, Rand_S16Offset(100, 50), 20, 0);
        }
    }
}

void EnTp_UpdateDamage(EnTp* this, PlayState* play) {
    s32 phi_s2;
    s32 phi_s4;
    EnTp* head; // Can eliminate this and just use now, but they're used differently
    EnTp* now;

    if ((this->collider.base.acFlags & AC_HIT) && (this->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD)) {
        phi_s4 = phi_s2 = 0;

        if (this->actor.params <= TAILPASARAN_HEAD) {
            phi_s2 = 1;
        }

        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&this->actor, &this->collider, 1);
        this->damageEffect = this->actor.colChkInfo.damageEffect;

        if (this->actor.colChkInfo.damageEffect != TAILPASARAN_DMGEFF_NONE) {
            if (this->actor.colChkInfo.damageEffect == TAILPASARAN_DMGEFF_DEKUNUT) {
                phi_s4 = 1;
            }

            // Head is invincible
            if (phi_s2 == 0) {
                Actor_ApplyDamage(&this->actor);
            }

            if (this->actor.colChkInfo.health == 0) {
                this->actor.flags &= ~ACTOR_FLAG_0;
                head = this->head;

                if (head->actor.params <= TAILPASARAN_HEAD) {
                    EnTp_SetupDie(head);
                    head->damageEffect = this->actor.colChkInfo.damageEffect;
                    head->actor.params = TAILPASARAN_HEAD_DYING;
                }
            } else {
                if (phi_s4 != 0) {
                    this->actor.freezeTimer = 80;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                    if (phi_s2 != 0) {
                        Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 0x50);
                    } else {
                        Actor_SetColorFilter(&this->actor, 0, 0xFF, 0x2000, 0x50);
                    }
                }

                for (now = (EnTp*)this->actor.parent; now != NULL; now = (EnTp*)now->actor.parent) {
                    now->collider.base.acFlags &= ~AC_HIT;

                    if (phi_s4 != 0) {
                        now->actor.freezeTimer = 80;
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);

                        if (phi_s2 != 0) {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0, 0x50);
                        } else {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0x2000, 0x50);
                        }
                    }
                }

                for (now = (EnTp*)this->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                    now->collider.base.acFlags &= ~AC_HIT;
                    if (phi_s4 != 0) {
                        now->actor.freezeTimer = 80;

                        if (phi_s2 != 0) {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0, 0x50);
                        } else {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0x2000, 0x50);
                        }
                    }
                }
            }
        }
    }
}

void EnTp_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTp* this = (EnTp*)thisx;
    Vec3f kiraVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f kiraAccel = { 0.0f, -0.6f, 0.0f };
    Vec3f kiraPos;
    Color_RGBA8 kiraPrimColor = { 0, 0, 255, 255 };
    Color_RGBA8 kiraEnvColor = { 0, 0, 0, 0 };
    Player* player = GET_PLAYER(play);
    s16 yawToWall;

    if (player->stateFlags1 & 0x4000000) { // Shielding
        this->damageEffect = TAILPASARAN_DMGEFF_NONE;
    }

    if (this->actor.colChkInfo.health != 0) {
        EnTp_UpdateDamage(this, play);
    }

    this->actionFunc(this, play);

    if (this->actor.params <= TAILPASARAN_HEAD) {
        Actor_MoveForward(&this->actor);

        if (this->actionIndex != TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 15.0f, 10.0f, 5);
        }

        // Turn away from wall
        if ((this->actor.speedXZ != 0.0f) && (this->actor.bgCheckFlags & 8)) {
            yawToWall = this->actor.wallYaw - this->actor.world.rot.y;

            if (ABS(yawToWall) > 0x4000) {
                if (yawToWall >= 0) {
                    this->actor.world.rot.y -= 500;
                } else {
                    this->actor.world.rot.y += 500;
                }

                this->actor.shape.rot.y = this->actor.world.rot.y;
            }
        }

        this->actor.shape.rot.z += 0x800;

        if (this->actor.shape.rot.z == 0) {
            Audio_PlaySoundGeneral(NA_SE_EN_TAIL_CRY, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }

        if (this->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
    }

    if (this->actor.params != TAILPASARAN_TAIL_DYING) {
        this->kiraSpawnTimer--;
        this->kiraSpawnTimer &= 7;
    }

    this->actor.focus.pos = this->actor.world.pos;

    if (this->damageEffect == TAILPASARAN_DMGEFF_SHOCKING) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if ((this->kiraSpawnTimer & 7) == 0) {
        kiraPrimColor.r = this->red;
        kiraAccel.x = -this->actor.velocity.x * 0.25f;
        kiraAccel.y = -this->actor.velocity.y * 0.25f;
        kiraAccel.z = -this->actor.velocity.z * 0.25f;
        kiraPos.x = ((Rand_ZeroOne() - 0.5f) * 25.0f) + this->actor.world.pos.x;
        kiraPos.y = ((Rand_ZeroOne() - 0.5f) * 20.0f) + this->actor.world.pos.y;
        kiraPos.z = ((Rand_ZeroOne() - 0.5f) * 25.0f) + this->actor.world.pos.z;
        EffectSsKiraKira_SpawnSmall(play, &kiraPos, &kiraVelocity, &kiraAccel, &kiraPrimColor, &kiraEnvColor);
    }

    if ((this->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD) && (this->actor.colChkInfo.health != 0)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnTp_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTp* this = (EnTp*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_150 != 2) {
        if ((thisx->params <= TAILPASARAN_HEAD) || (thisx->params == TAILPASARAN_HEAD_DYING)) {
            func_80093D18(play->state.gfxCtx);

            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gTailpasaranHeadDL);

            Matrix_Translate(0.0f, 0.0f, 8.0f, MTXMODE_APPLY);
        } else {
            func_80093D84(play->state.gfxCtx);
            Matrix_ReplaceRotation(&play->billboardMtxF);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->red, 0, 255, this->alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT,
                              TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT,
                              TEXEL0, ENVIRONMENT);
            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gTailpasaranTailSegmentTex));
            gDPPipeSync(POLY_XLU_DISP++);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gTailpasaranTailSegmentDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);

    if ((thisx->params <= TAILPASARAN_TAIL) || (thisx->params == TAILPASARAN_TAIL_DYING)) {
        Collider_UpdateSpheres(0, &this->collider);
    }
}

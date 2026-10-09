/*
 * File: z_en_firefly.c
 * Overlay: ovl_En_Firefly
 * Description: Keese (Normal, Fire, Ice)
 */

#include "z_en_firefly.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void EnFirefly_Init(Actor* thisx, PlayState* play);
void EnFirefly_Destroy(Actor* thisx, PlayState* play);
void EnFirefly_Update(Actor* thisx, PlayState* play2);
void EnFirefly_Draw(Actor* thisx, PlayState* play);

void EnFirefly_FlyIdle(EnFirefly* this, PlayState* play);
void EnFirefly_Fall(EnFirefly* this, PlayState* play);
void EnFirefly_SetupDie(EnFirefly* this);
void EnFirefly_Die(EnFirefly* this, PlayState* play);
void EnFirefly_SetupDiveAttack(EnFirefly* this);
void EnFirefly_DiveAttack(EnFirefly* this, PlayState* play);
void EnFirefly_Rebound(EnFirefly* this, PlayState* play);
void EnFirefly_SetupFlyAway(EnFirefly* this);
void EnFirefly_FlyAway(EnFirefly* this, PlayState* play);
void EnFirefly_Stunned(EnFirefly* this, PlayState* play);
void EnFirefly_SetupPerch(EnFirefly* this);
void EnFirefly_Perch(EnFirefly* this, PlayState* play);
void EnFirefly_SetupDisturbDiveAttack(EnFirefly* this);
void EnFirefly_DisturbDiveAttack(EnFirefly* this, PlayState* play);

typedef enum {
    /* 0 */ KEESE_FIRE,
    /* 3 */ KEESE_NORMAL = 3,
    /* 4 */ KEESE_ICE
} KeeseCurrentType;

typedef enum {
    /* 0 */ KEESE_AURA_NONE,
    /* 1 */ KEESE_AURA_FIRE,
    /* 2 */ KEESE_AURA_ICE
} KeeseAuraType;

ActorProfile En_Firefly_Profile = {
    /**/ ACTOR_EN_FIREFLY,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FIREFLY,
    /**/ sizeof(EnFirefly),
    /**/ EnFirefly_Init,
    /**/ EnFirefly_Destroy,
    /**/ EnFirefly_Update,
    /**/ EnFirefly_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x01, 0x08 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 1, { { 0, 1000, 0 }, 15 }, 100 },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 10, 10, 10 };

typedef enum {
    /* 0 */ KEESE_DMGEFF_NONE,
    /* 1 */ KEESE_DMGEFF_STUN,
    /* 2 */ KEESE_DMGEFF_FIRE,
    /* 3 */ KEESE_DMGEFF_ICE,
    /* 4 */ KEESE_DMGEFF_LIGHT,
    /* 5 */ KEESE_DMGEFF_ELECTRIC
} BatDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, KEESE_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, KEESE_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, KEESE_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, KEESE_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, KEESE_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, KEESE_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, KEESE_DMGEFF_ELECTRIC),
    /* Normal shield  */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, KEESE_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, KEESE_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 5, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 4000, ICHAIN_STOP),
};

void EnFirefly_Init(Actor* thisx, PlayState* play) {
    EnFirefly* this = (EnFirefly*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_Init(play, &this->skelAnime, &gFireKeeseSkel, &gFireKeeseFlyAnim, this->jointTable, this->morphTable,
                   FIRE_KEESE_LIMB_MAX);
    Collider_InitAndSetSphere(play, &this->collider, &this->actor, &sSphereInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (this->actor.params & KEESE_INVISIBLE) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        this->actor.params = KEESE_GET_MAIN_TYPE(thisx);
        this->isInvisible = true;
    }

    if (this->actor.params == KEESE_FIRE_FLY) {
        this->auraType = KEESE_AURA_FIRE;
        this->timer = Rand_S16Offset(20, 60);
        this->actor.shape.rot.x = 0x1554;
        this->actor.hintId = TATL_HINT_ID_FIRE_KEESE;
        this->maxAltitude = this->actor.home.pos.y;
        this->actionFunc = EnFirefly_FlyIdle;
    } else if (this->actor.params == KEESE_ICE_FLY) {
        this->auraType = KEESE_AURA_ICE;
        this->collider.elem.atDmgInfo.effect = 2; // Freeze
        this->actor.hintId = TATL_HINT_ID_ICE_KEESE;
        this->maxAltitude = this->actor.home.pos.y + 100.0f;
        this->actionFunc = EnFirefly_FlyIdle;
    } else {
        this->auraType = KEESE_AURA_NONE;
        this->collider.elem.atDmgInfo.effect = 0; // Nothing
        this->actor.hintId = TATL_HINT_ID_KEESE;
        this->maxAltitude = this->actor.home.pos.y + 100.0f;
        this->actionFunc = EnFirefly_Perch;
    }

    this->currentType = this->actor.params;
    this->collider.dim.worldSphere.radius = sSphereInit.dim.modelSphere.radius;
}

void EnFirefly_Destroy(Actor* thisx, PlayState* play) {
    EnFirefly* this = (EnFirefly*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

void EnFirefly_SpawnIceEffects(EnFirefly* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, KEESE_BODYPART_MAX, 2, 0.2f, 0.2f);
    }
}

void EnFirefly_Extinguish(EnFirefly* this) {
    this->currentType = KEESE_NORMAL;
    this->collider.elem.atDmgInfo.effect = 0; // Nothing
    this->auraType = KEESE_AURA_NONE;
    this->actor.hintId = TATL_HINT_ID_KEESE;
}

void EnFirefly_Ignite(EnFirefly* this) {
    if (this->actor.params == KEESE_FIRE_FLY) {
        this->currentType = KEESE_FIRE;
        this->collider.elem.atDmgInfo.effect = 1; // Fire
        this->auraType = KEESE_AURA_FIRE;
        this->actor.hintId = TATL_HINT_ID_FIRE_KEESE;
    }
}

s32 EnFirefly_ReturnToPerch(EnFirefly* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distFromHome;

    if (this->actor.params != KEESE_NORMAL_PERCH) {
        return false;
    }

    if (Actor_WorldDistXZToPoint(&player->actor, &this->actor.home.pos) > 300.0f) {
        distFromHome = Actor_WorldDistXYZToPoint(&this->actor, &this->actor.home.pos);

        if (distFromHome < 5.0f) {
            EnFirefly_SetupPerch(this);
        } else {
            if (distFromHome * 0.05f < 1.0f) {
                this->actor.speed *= distFromHome * 0.05f;
            }

            Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                               0x300);
            Math_ScaledStepToS(&this->actor.shape.rot.x,
                               Actor_WorldPitchTowardPoint(&this->actor, &this->actor.home.pos) + 0x1554, 0x100);
        }

        return true;
    }

    return false;
}

s32 EnFirefly_SeekTorch(EnFirefly* this, PlayState* play) {
    ObjSyokudai* findTorch = (ObjSyokudai*)play->actorCtx.actorLists[ACTORCAT_PROP].first;
    ObjSyokudai* closestTorch = NULL;
    f32 currentMinDist = 35000.0f;
    f32 currentDist;
    Vec3f flamePos;

    if ((this->actor.params != KEESE_FIRE_FLY) || (this->currentType != KEESE_NORMAL)) {
        return false;
    }

    while (findTorch != NULL) {
        if ((findTorch->actor.id == ACTOR_OBJ_SYOKUDAI) && (findTorch->snuffTimer != OBJ_SYOKUDAI_SNUFF_OUT)) {
            currentDist = Actor_WorldDistXYZToActor(&this->actor, &findTorch->actor);
            if (currentDist < currentMinDist) {
                currentMinDist = currentDist;
                closestTorch = findTorch;
            }
        }
        findTorch = (ObjSyokudai*)findTorch->actor.next;
    }

    if (closestTorch != NULL) {
        flamePos.x = closestTorch->actor.world.pos.x;
        flamePos.y = closestTorch->actor.world.pos.y + 52.0f + 30.0f;
        flamePos.z = closestTorch->actor.world.pos.z;

        if (Actor_WorldDistXYZToPoint(&this->actor, &flamePos) < 15.0f) {
            EnFirefly_Ignite(this);
        } else {
            Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, &closestTorch->actor),
                               0x300);
            Math_ScaledStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &flamePos) + 0x1554,
                               0x100);
        }

        return true;
    }

    return false;
}

void EnFirefly_SetupFlyIdle(EnFirefly* this) {
    this->timer = Rand_S16Offset(70, 100);
    this->actor.speed = (Rand_ZeroOne() * 1.5f) + 1.5f;
    Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), 0x300);
    this->pitchTarget = ((this->maxAltitude < this->actor.world.pos.y) ? 0xC00 : -0xC00) + 0x1554;
    this->skelAnime.playSpeed = 1.0f;
    this->actionFunc = EnFirefly_FlyIdle;
}

void EnFirefly_FlyIdle(EnFirefly* this, PlayState* play) {
    s32 onAnimFirstFrame;
    f32 rand;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    onAnimFirstFrame = Animation_OnFrame(&this->skelAnime, 0.0f);
    this->actor.speed = (Rand_ZeroOne() * 1.5f) + 1.5f;

    if (!EnFirefly_ReturnToPerch(this, play) && !EnFirefly_SeekTorch(this, play)) {
        if (onAnimFirstFrame) {
            rand = Rand_ZeroOne();

            if (rand < 0.5f) {
                Math_ScaledStepToS(&this->actor.shape.rot.y,
                                   Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), 0x300);
            } else if (rand < 0.8f) {
                this->actor.shape.rot.y += TRUNCF_BINANG(Rand_CenteredFloat(0x600));
            }

            // Climb if too close to ground
            if (this->actor.world.pos.y < this->actor.floorHeight + 20.0f) {
                this->pitchTarget = 0x954;
                // Descend if above maxAltitude
            } else if (this->maxAltitude < this->actor.world.pos.y) {
                this->pitchTarget = 0x2154;
                // Otherwise ascend or descend at random, biased towards ascending
            } else if (Rand_ZeroOne() > 0.35f) {
                this->pitchTarget = 0x954;
            } else {
                this->pitchTarget = 0x2154;
            }
        } else {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->pitchTarget = 0x954;
            } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) ||
                       (this->maxAltitude < this->actor.world.pos.y)) {
                this->pitchTarget = 0x2154;
            }
        }

        Math_ScaledStepToS(&this->actor.shape.rot.x, this->pitchTarget, 0x100);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
    }

    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 200.0f) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
        EnFirefly_SetupDiveAttack(this);
    }
}

void EnFirefly_SetupFall(EnFirefly* this, PlayState* play) {
    this->timer = 40;
    this->actor.velocity.y = 0.0f;
    Animation_Change(&this->skelAnime, &gFireKeeseFlyAnim, 0.0f, 6.0f, 6.0f, ANIMMODE_ONCE, 0.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_DEAD);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

    if (this->isInvisible) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 40);
    } else {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    }

    if (this->actor.colChkInfo.damageEffect == KEESE_DMGEFF_ICE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        this->drawDmgEffAlpha = 1.0f;
        this->drawDmgEffScale = 0.55f;
        this->drawDmgEffFrozenSteamScale = 0.55f * 1.5f;
    } else if (this->actor.colChkInfo.damageEffect == KEESE_DMGEFF_LIGHT) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffScale = 0.55f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                    this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
    } else if (this->actor.colChkInfo.damageEffect == KEESE_DMGEFF_FIRE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffScale = 0.55f;
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        this->auraType = KEESE_AURA_NONE;
    }

    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        this->actor.speed = 0.0f;
    }

    this->actionFunc = EnFirefly_Fall;
}

// Fall to the ground after being hit
void EnFirefly_Fall(EnFirefly* this, PlayState* play) {
    this->actor.colorFilterTimer = 40;
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_ScaledStepToS(&this->actor.shape.rot.x, 0x6800, 0x200);
            this->actor.shape.rot.y -= 0x300;
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
            EnFirefly_SpawnIceEffects(this, play);
            EnFirefly_SetupDie(this);
        }
    }
}

void EnFirefly_SetupDie(EnFirefly* this) {
    this->timer = 15;
    this->actor.speed = 0.0f;
    this->actionFunc = EnFirefly_Die;
}

// Hit the ground or burn up, spawn drops
void EnFirefly_Die(EnFirefly* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    Math_StepToF(&this->actor.scale.x, 0.0f, 0.00034f);
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;

    if (this->timer == 0) {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xA0);
        Actor_Kill(&this->actor);
    }
}

void EnFirefly_SetupDiveAttack(EnFirefly* this) {
    this->timer = Rand_S16Offset(70, 100);
    this->skelAnime.playSpeed = 1.0f;
    this->pitchTarget = ((this->actor.playerHeightRel > 0.0f) ? -0xC00 : 0xC00) + 0x1554;
    this->actionFunc = EnFirefly_DiveAttack;
}

void EnFirefly_DiveAttack(EnFirefly* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f preyPos;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    Math_StepToF(&this->actor.speed, 4.0f, 0.5f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->pitchTarget, 0x100);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2800)) {
        if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
            this->skelAnime.playSpeed = 0.0f;
            this->skelAnime.curFrame = 4.0f;
        }

        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xC00, 0x300);
        preyPos.x = player->actor.world.pos.x;
        preyPos.y = player->actor.world.pos.y + 20.0f;
        preyPos.z = player->actor.world.pos.z;
        Math_SmoothStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &preyPos) + 0x1554, 2,
                           0x400, 0x100);
    } else {
        this->skelAnime.playSpeed = 1.5f;
        if (this->actor.xzDistToPlayer > 80.0f) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xC00, 0x300);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->pitchTarget = 0x954;
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) || (this->maxAltitude < this->actor.world.pos.y)) {
            this->pitchTarget = 0x2154;
        } else {
            this->pitchTarget = 0x954;
        }

        Math_ScaledStepToS(&this->actor.shape.rot.x, this->pitchTarget, 0x100);
    }

    if ((this->timer == 0) || (Player_GetMask(play) == PLAYER_MASK_STONE) || (player->stateFlags2 & PLAYER_STATE2_80) ||
        (player->actor.freezeTimer > 0)) {
        EnFirefly_SetupFlyAway(this);
    }
}

void EnFirefly_SetupRebound(EnFirefly* this) {
    this->actor.world.rot.x = 0x7000;
    this->timer = 18;
    this->skelAnime.playSpeed = 1.0f;
    this->actor.speed = 2.5f;
    this->actionFunc = EnFirefly_Rebound;
}

// Knockback after hitting player
void EnFirefly_Rebound(EnFirefly* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x100);
    Math_StepToF(&this->actor.velocity.y, 0.0f, 0.4f);
    if (Math_StepToF(&this->actor.speed, 0.0f, 0.15f)) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            EnFirefly_SetupFlyAway(this);
        }
    }
}

void EnFirefly_SetupFlyAway(EnFirefly* this) {
    this->timer = 150;
    this->pitchTarget = 0x954;
    this->actionFunc = EnFirefly_FlyAway;
    this->skelAnime.playSpeed = 1.0f;
}

void EnFirefly_FlyAway(EnFirefly* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (((fabsf(this->actor.world.pos.y - this->maxAltitude) < 10.0f) &&
         (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) < 20.0f)) ||
        (this->timer == 0)) {
        EnFirefly_SetupFlyIdle(this);
        return;
    }

    Math_StepToF(&this->actor.speed, 3.0f, 0.3f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->pitchTarget = 0x954;
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) || (this->maxAltitude < this->actor.world.pos.y)) {
        this->pitchTarget = 0x2154;
    } else {
        this->pitchTarget = 0x954;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.wallYaw, 2, 0xC00, 0x300);
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           0x300);
    }

    Math_ScaledStepToS(&this->actor.shape.rot.x, this->pitchTarget, 0x100);
}

void EnFirefly_SetupStunned(EnFirefly* this) {
    if (this->isInvisible) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU, this->timer);
    } else {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, this->timer);
    }

    if (this->actionFunc != EnFirefly_Stunned) {
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
    }

    this->auraType = KEESE_AURA_NONE;
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = EnFirefly_Stunned;
}

void EnFirefly_Stunned(EnFirefly* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1554, 0x100);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            if (this->currentType == KEESE_FIRE) {
                this->auraType = KEESE_AURA_FIRE;
            } else if (this->currentType == KEESE_ICE) {
                this->auraType = KEESE_AURA_ICE;
            }

            EnFirefly_SetupFlyIdle(this);
        }
    } else {
        this->actor.colorFilterTimer = 40;

        if (this->drawDmgEffAlpha > 0.0f) {
            this->drawDmgEffAlpha = 2.0f;
        }
    }
}

void EnFirefly_SetupPerch(EnFirefly* this) {
    this->timer = 1;
    this->actionFunc = EnFirefly_Perch;
    this->actor.speed = 0.0f;
}

// When perching, sit on collision and flap at random intervals
void EnFirefly_Perch(EnFirefly* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x100);
    if (this->timer != 0) {
        SkelAnime_Update(&this->skelAnime);
        if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
            this->timer--;
        }
    } else if (Rand_ZeroOne() < 0.02f) {
        this->timer = 1;
    }

    if (this->actor.xzDistToPlayer < 120.0f) {
        EnFirefly_SetupDisturbDiveAttack(this);
    }
}

void EnFirefly_SetupDisturbDiveAttack(EnFirefly* this) {
    this->skelAnime.playSpeed = 3.0f;
    this->actor.shape.rot.x = 0x1554;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->timer = 50;
    this->actor.speed = 3.0f;
    this->actionFunc = EnFirefly_DisturbDiveAttack;
}

void EnFirefly_DisturbDiveAttack(EnFirefly* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f preyPos;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer < 40) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0xAAC, 0x100);
    } else {
        preyPos.x = player->actor.world.pos.x;
        preyPos.y = player->actor.world.pos.y + 20.0f;
        preyPos.z = player->actor.world.pos.z;
        Math_ScaledStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &preyPos) + 0x1554,
                           0x100);
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x300);
    }

    if (this->timer == 0) {
        EnFirefly_SetupFlyIdle(this);
    }
}

void EnFirefly_UpdateDamage(EnFirefly* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elem);

        if (this->actor.colChkInfo.damageEffect == KEESE_DMGEFF_STUN) {
            this->timer = 40;
            EnFirefly_SetupStunned(this);
        } else if (this->actor.colChkInfo.damageEffect == KEESE_DMGEFF_ELECTRIC) {
            this->timer = 40;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM;
            this->drawDmgEffAlpha = 2.0f;
            this->drawDmgEffScale = 0.55f;
            EnFirefly_SetupStunned(this);
        } else {
            Enemy_StartFinishingBlow(play, &this->actor);
            this->actor.colChkInfo.health = 0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

            // Negate effects of fire on Fire Keese and Ice on Ice Keese
            if (((this->currentType == KEESE_FIRE) && (this->actor.colChkInfo.damageEffect == KEESE_DMGEFF_FIRE)) ||
                ((this->currentType == KEESE_ICE) && (this->actor.colChkInfo.damageEffect == KEESE_DMGEFF_ICE))) {
                this->actor.colChkInfo.damageEffect = KEESE_DMGEFF_NONE;
            }

            EnFirefly_SetupFall(this, play);
        }
    }
}

void EnFirefly_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnFirefly* this = (EnFirefly*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_ATTACK);

        if (this->currentType != KEESE_NORMAL) {
            EnFirefly_Extinguish(this);
        }

        if (this->actionFunc != EnFirefly_DisturbDiveAttack) {
            EnFirefly_SetupRebound(this);
        }
    }

    EnFirefly_UpdateDamage(this, play);
    this->actionFunc(this, play);

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if ((this->actor.colChkInfo.health == 0) || (this->actionFunc == EnFirefly_Stunned)) {
            Actor_MoveWithGravity(&this->actor);
        } else {
            if (this->actionFunc != EnFirefly_Rebound) {
                this->actor.world.rot.x = 0x1554 - this->actor.shape.rot.x;
            }

            Actor_MoveWithoutGravity(&this->actor);
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 15.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
    this->collider.dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.dim.worldSphere.center.y = (s32)this->actor.world.pos.y + 10;
    this->collider.dim.worldSphere.center.z = this->actor.world.pos.z;

    if ((this->actionFunc == EnFirefly_DiveAttack) || (this->actionFunc == EnFirefly_DisturbDiveAttack)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actor.colChkInfo.health != 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        this->actor.world.rot.y = this->actor.shape.rot.y;

        if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_FLY);
        }
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.275f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.55f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.55f, 0.55f / 40.0f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }

    this->actor.focus.pos.x =
        10.0f * Math_SinS(this->actor.shape.rot.x) * Math_SinS(this->actor.shape.rot.y) + this->actor.world.pos.x;
    this->actor.focus.pos.y = 10.0f * Math_CosS(this->actor.shape.rot.x) + this->actor.world.pos.y;
    this->actor.focus.pos.z =
        10.0f * Math_SinS(this->actor.shape.rot.x) * Math_CosS(this->actor.shape.rot.y) + this->actor.world.pos.z;
}

s32 EnFirefly_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                               Gfx** gfx) {
    EnFirefly* this = (EnFirefly*)thisx;

    if (this->isInvisible && (play->actorCtx.lensMaskSize != LENS_MASK_ACTIVE_SIZE)) {
        *dList = NULL;
    } else if (limbIndex == FIRE_KEESE_LIMB_ROOT) {
        pos->y += 2300.0f;
    }
    return false;
}

void EnFirefly_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    static Color_RGBA8 sFireAuraPrimColor = { 255, 255, 100, 255 };
    static Color_RGBA8 sFireAuraEnvColor = { 255, 50, 0, 0 };
    static Color_RGBA8 sIceAuraPrimColor = { 100, 200, 255, 255 };
    static Color_RGBA8 sIceAuraEnvColor = { 0, 0, 255, 0 };
    static Vec3f sAuraVelocity = { 0.0f, 0.5f, 0.0f };
    static Vec3f sAuraAccel = { 0.0f, 0.5f, 0.0f };
    Vec3f auraPos;
    Color_RGBA8* auraPrimColor;
    Color_RGBA8* auraEnvColor;
    s16 auraScaleStep;
    s16 auraLife;
    s32 pad;
    EnFirefly* this = (EnFirefly*)thisx;

    if ((this->currentType != KEESE_FIRE) && (limbIndex == FIRE_KEESE_LIMB_HEAD)) {
        gSPDisplayList((*gfx)++, gKeeseRedEyesDL);
    } else if ((this->lastDrawnFrame != play->gameplayFrames) &&
               ((this->auraType == KEESE_AURA_FIRE) || (this->auraType == KEESE_AURA_ICE)) &&
               ((limbIndex == FIRE_KEESE_LIMB_LEFT_WING_END) || (limbIndex == FIRE_KEESE_LIMB_RIGHT_WING_END_ROOT))) {
        if (this->actionFunc != EnFirefly_Die) {
            Matrix_MultZero(&auraPos);
            auraPos.x += Rand_ZeroFloat(5.0f);
            auraPos.y += Rand_ZeroFloat(5.0f);
            auraPos.z += Rand_ZeroFloat(5.0f);
            auraScaleStep = -40;
            auraLife = 3;
        } else {
            if (limbIndex == FIRE_KEESE_LIMB_LEFT_WING_END) {
                auraPos.x = Math_SinS(9100 * this->timer) * this->timer + this->actor.world.pos.x;
                auraPos.z = Math_CosS(9100 * this->timer) * this->timer + this->actor.world.pos.z;
            } else {
                auraPos.x = this->actor.world.pos.x - Math_SinS(9100 * this->timer) * this->timer;
                auraPos.z = this->actor.world.pos.z - Math_CosS(9100 * this->timer) * this->timer;
            }

            auraPos.y = this->actor.world.pos.y + (15 - this->timer) * 1.5f;
            auraScaleStep = -5;
            auraLife = 10;
        }

        if (this->auraType == KEESE_AURA_FIRE) {
            auraPrimColor = &sFireAuraPrimColor;
            auraEnvColor = &sFireAuraEnvColor;
        } else {
            auraPrimColor = &sIceAuraPrimColor;
            auraEnvColor = &sIceAuraEnvColor;
        }

        func_800B0F80(play, &auraPos, &sAuraVelocity, &sAuraAccel, auraPrimColor, auraEnvColor, 250, auraScaleStep,
                      auraLife);
    }

    if (limbIndex == FIRE_KEESE_LIMB_LEFT_WING_END) {
        Matrix_MultZero(&this->bodyPartsPos[KEESE_BODYPART_LEFT_WING_END]);
    } else if (limbIndex == FIRE_KEESE_LIMB_RIGHT_WING_END_ROOT) {
        Matrix_MultZero(&this->bodyPartsPos[KEESE_BODYPART_RIGHT_WING_END_ROOT]);
    } else if (limbIndex == FIRE_KEESE_LIMB_BODY) {
        Matrix_MultZero(&this->bodyPartsPos[KEESE_BODYPART_BODY]);
    }
}

void EnFirefly_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFirefly* this = (EnFirefly*)thisx;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->isInvisible) {
        gfx = POLY_XLU_DISP;
    } else {
        gfx = POLY_OPA_DISP;
    }

    gSPDisplayList(gfx, gSetupDLs[SETUPDL_25]);

    if (this->currentType == KEESE_FIRE) {
        gDPSetEnvColor(&gfx[1], 0, 0, 0, 0);
    } else {
        gDPSetEnvColor(&gfx[1], 0, 0, 0, 255);
    }

    gfx = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnFirefly_OverrideLimbDraw,
                         EnFirefly_PostLimbDraw, &this->actor, &gfx[2]);
    if (this->isInvisible) {
        POLY_XLU_DISP = gfx;
    } else {
        POLY_OPA_DISP = gfx;
    }

    if (GameInteractor_Should(VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS, true, this, this->bodyPartsPos,
                              KEESE_BODYPART_MAX)) {
        Actor_DrawDamageEffects(play, NULL, this->bodyPartsPos, KEESE_BODYPART_MAX,
                                this->drawDmgEffScale * this->actor.scale.y * 200.0f, this->drawDmgEffFrozenSteamScale,
                                this->drawDmgEffAlpha, this->drawDmgEffType);
    }
    this->lastDrawnFrame = play->gameplayFrames;

    CLOSE_DISPS(play->state.gfxCtx);
}

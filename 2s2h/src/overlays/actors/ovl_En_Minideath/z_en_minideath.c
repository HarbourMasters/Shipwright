/*
 * File: z_en_minideath.c
 * Overlay: ovl_En_Minideath
 * Description: Gomess's bats
 */

#include "z_en_minideath.h"
#include "overlays/actors/ovl_En_Death/z_en_death.h"
#include "objects/object_death/object_death.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMinideath_Init(Actor* thisx, PlayState* play);
void EnMinideath_Destroy(Actor* thisx, PlayState* play);
void EnMinideath_Update(Actor* thisx, PlayState* play);

void EnMinideath_PreBattle(EnMinideath* this, PlayState* play);
void EnMinideath_Intro1(EnMinideath* this, PlayState* play);
void EnMinideath_Intro2(EnMinideath* this, PlayState* play);
void EnMinideath_CrowdParent(EnMinideath* this, PlayState* play);
void EnMinideath_Scatter(EnMinideath* this, PlayState* play);
void EnMinideath_Return(EnMinideath* this, PlayState* play);
void EnMinideath_Intro3(EnMinideath* this, PlayState* play);
void EnMinideath_StartSwarm(EnMinideath* this, PlayState* play);
void EnMinideath_SwarmFollower(EnMinideath* this, PlayState* play);
void EnMinideath_SwarmLeader(EnMinideath* this, PlayState* play);
void EnMinideath_Death1(EnMinideath* this, PlayState* play);
void EnMinideath_Die(EnMinideath* this, PlayState* play);
void EnMinideath_Dead(EnMinideath* this, PlayState* play);

void EnMinideath_RandomizeEffects(EnMinideath* this);
void EnMinideath_SetupPreBattle(EnMinideath* this);
void EnMinideath_SetupIntro1(EnMinideath* this);
void EnMinideath_SetupIntro2(EnMinideath* this);
void EnMinideath_SetupCrowdParent(EnMinideath* this);
void EnMinideath_SetupReturn(EnMinideath* this);
void EnMinideath_SetupIntro3(EnMinideath* this);
void EnMinideath_SetupDead(EnMinideath* this);

ActorProfile En_Minideath_Profile = {
    /**/ ACTOR_EN_MINIDEATH,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DEATH,
    /**/ sizeof(EnMinideath),
    /**/ EnMinideath_Init,
    /**/ EnMinideath_Destroy,
    /**/ EnMinideath_Update,
    /**/ NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[3] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

typedef enum {
    /* 0 */ DMGEFF_NONE,
    /* 1 */ DMGEFF_STUN,
    /* 2 */ DMGEFF_FIRE_ARROW,
    /* 3 */ DMGEFF_ICE_ARROW,
    /* 4 */ DMGEFF_LIGHT_ARROW,
    /* 5 */ DMGEFF_ZORA_BARRIER
} EnMinideathDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(0, DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, DMGEFF_FIRE_ARROW),
    /* Ice arrow      */ DMG_ENTRY(1, DMGEFF_ICE_ARROW),
    /* Light arrow    */ DMG_ENTRY(2, DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(1, DMGEFF_ZORA_BARRIER),
    /* Normal shield  */ DMG_ENTRY(0, DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, DMGEFF_NONE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 30, 10 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

static s32 sItemDropTimer;
static s32 sNumSwarmHits;
static s32 sScatterTimer;
static s32 sPlayedDeathSfx;

void EnMinideath_Init(Actor* thisx, PlayState* play) {
    EnMinideath* this = (EnMinideath*)thisx;
    s32 i;

    Actor_ProcessInitChain(thisx, sInitChain);

    this->initialHeightDiffFromParent = thisx->world.pos.y - thisx->parent->world.pos.y;
    this->spawnShapeYaw = thisx->shape.rot.y;

    thisx->shape.rot.y = thisx->parent->shape.rot.y;
    thisx->world.rot.y = thisx->parent->shape.rot.y;
    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    Collider_InitAndSetJntSph(play, &this->collider, thisx, &sJntSphInit, this->colliderElements);
    CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);

    for (i = 0; i < MINIDEATH_NUM_EFFECTS; i++) {
        this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
    }

    EnMinideath_RandomizeEffects(this);

    this->number = thisx->params;
    thisx->params = MINIDEATH_ACTION_CONTINUE;
    this->crowdState = false;

    if (CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_GOMESS)) {
        this->collider.base.atFlags |= AT_ON;
        this->crowdState = true;

        EnMinideath_CrowdParent(this, play);

        for (i = 0; i < MINIDEATH_NUM_EFFECTS; i++) {
            Math_Vec3f_Sum(&thisx->world.pos, &this->effects[i].vel, &this->effects[i].pos);
        }

        EnMinideath_SetupCrowdParent(this);
    } else {
        EnMinideath_SetupPreBattle(this);
    }
}

void EnMinideath_Destroy(Actor* thisx, PlayState* play) {
    EnMinideath* this = (EnMinideath*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnMinideath_DropCollectible(Vec3f* spawnPos, PlayState* play) {
    if (sItemDropTimer == 0 && Rand_ZeroOne() < 0.3f) {
        if (Rand_ZeroOne() < 0.5f) {
            Item_DropCollectible(play, spawnPos, ITEM00_ARROWS_10);
        } else {
            Item_DropCollectible(play, spawnPos, ITEM00_MAGIC_JAR_BIG);
        }
        sItemDropTimer = 800; // wait 40 seconds before next drop
    }
}

void EnMinideath_UpdateCrowdState(EnMinideath* this, s32 crowdParent) {
    // If the state is changed to the same as before, don't update the count
    if (this->crowdState != crowdParent) {
        this->crowdState = crowdParent;

        // Gomess params stores the number of crowding bats
        if (crowdParent == true) {
            this->actor.parent->params++;
        } else {
            this->actor.parent->params--;
        }
    }
}

void EnMinideath_RandomizeEffects(EnMinideath* this) {
    MiniDeathEffect* effect;
    s32 i;

    for (effect = this->effects, i = 0; i < MINIDEATH_NUM_EFFECTS; i++, effect++) {
        effect->vel.x = Rand_CenteredFloat(80.0f);
        effect->vel.y = Rand_ZeroFloat(40.0f);
        effect->vel.z = Rand_CenteredFloat(80.0f);
        effect->animFrame = (s32)Rand_ZeroFloat(9.0f) % MINIDEATH_ANIM_LENGTH;
        effect->angle.x = (s32)Rand_Next() >> 17;
        effect->angle.y = (s32)Rand_Next() >> 16;
        effect->state = 0;
        effect->timer = 0;
    }
}

void EnMinideath_UpdateEffects(EnMinideath* this, PlayState* play) {
    s16 angle;
    MiniDeathEffect* effect;
    f32 phi_fv0;
    f32 phi_fa0;
    s32 i;
    s32 phi_s7;
    s32 phi_s3;

    phi_s3 = 0;
    phi_s7 = 0;

    effect = this->effects;
    for (i = 0; i < MINIDEATH_NUM_EFFECTS; i++, effect++) {
        if (effect->state == 0) {
            effect->vel.x += 3.0f * Math_CosS(effect->angle.x) * Math_SinS(effect->angle.y);
            effect->vel.y += 3.0f * Math_SinS(effect->angle.x);
            effect->vel.z += 3.0f * Math_CosS(effect->angle.x) * Math_CosS(effect->angle.y);

            if (this->actionFunc == EnMinideath_PreBattle) {
                phi_fv0 = 100.0f;
                phi_fa0 = SQ(phi_fv0);
            } else if (this->actionFunc == EnMinideath_StartSwarm || this->actionFunc == EnMinideath_SwarmLeader) {
                phi_fv0 = 20.0f;
                phi_fa0 = SQ(20.0f);
            } else {
                phi_fv0 = 40.0f;
                phi_fa0 = SQ(40.0f);
            }

            if (effect->vel.y < 0.0f || phi_fv0 < effect->vel.y || phi_fa0 < SQ(effect->vel.x) + SQ(effect->vel.z)) {
                angle = Math_Atan2S_XY(sqrtf(SQ(effect->vel.x) + SQ(effect->vel.z)), effect->vel.y);
                effect->angle.x = -1 * angle + ((s32)Rand_Next() >> 19);
                angle = Math_Atan2S_XY(effect->vel.z, effect->vel.x);
                effect->angle.y = 0x8000 + angle + ((s32)Rand_Next() >> 19);
            }

            phi_s3++;
        } else if (effect->state == 1) {
            effect->pos.y += effect->vel.y;
            effect->angle.y += 0x1800;
            effect->vel.y -= 1.0f;

            if (effect->pos.y < this->actor.parent->home.pos.y) {
                effect->pos.y = this->actor.parent->home.pos.y;
                func_800B3030(play, &effect->pos, &gZeroVec3f, &gZeroVec3f, 100, 0, 0);
                SoundSource_PlaySfxAtFixedWorldPos(play, &effect->pos, 11, NA_SE_EN_EXTINCT);
                EnMinideath_DropCollectible(&effect->pos, play);
                effect->state = 2;
            }
        } else if (effect->state == 2) {
            if (effect->timer > 0) {
                effect->timer--;
            } else if (this->actionFunc == EnMinideath_CrowdParent) {
                Math_Vec3f_Diff(&this->actor.parent->focus.pos, &this->actor.world.pos, &effect->vel);
                effect->state = 0;
                this->collider.elements[i].base.acElemFlags |= ACELEM_ON;
                this->collider.elements[i].base.atElemFlags |= ATELEM_ON;
                phi_s7 = 1;
                phi_s3++;
            }
        }
    }

    if (phi_s7 && phi_s3 > 1) {
        EnMinideath_UpdateCrowdState(this, true);
    }
}

void EnMinideath_SetupPreBattle(EnMinideath* this) {
    this->actor.speed = 6.0f;
    this->actor.world.pos.x = Rand_CenteredFloat(600.0f) + this->actor.parent->world.pos.x;
    this->actor.world.pos.z = Rand_CenteredFloat(600.0f) + this->actor.parent->world.pos.z;
    this->actor.shape.rot.y = (s32)Rand_Next() >> 16;
    this->actor.shape.rot.x = 0;
    this->moveDirection.y = this->actor.shape.rot.y;
    this->actionFunc = EnMinideath_PreBattle;
}

void EnMinideath_PreBattle(EnMinideath* this, PlayState* play) {
    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->moveDirection.y, 0x480) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        // Fly away from walls
        this->moveDirection.y = ((s32)Rand_Next() >> 18) + this->actor.wallYaw;
    }
    if (this->actor.params == MINIDEATH_ACTION_INTRO_1) {
        EnMinideath_SetupIntro1(this);
    }
}

void EnMinideath_SetupIntro1(EnMinideath* this) {
    this->actionFunc = EnMinideath_Intro1;
}

void EnMinideath_Intro1(EnMinideath* this, PlayState* play) {
    EnDeath* parent;
    Vec3f sp38;
    s32 phi_v0;
    s16 angle;

    Math_StepToF(&this->actor.speed, 15.0f, 0.5f);

    if (this->actor.child == NULL) {
        phi_v0 = 0;
        angle = 0;
    } else if (this->number == 6) {
        phi_v0 = 1;
        angle = 0x5555;
    } else if (this->number == 12) {
        phi_v0 = 2;
        angle = -0x5556;
    } else {
        phi_v0 = -1;
        Math_Vec3f_Copy(&sp38, &this->actor.child->world.pos);
        angle = 0;
    }

    if (phi_v0 != -1) {
        parent = (EnDeath*)this->actor.parent;

        angle += parent->coreRotation;
        sp38.x = this->actor.parent->world.pos.x + Math_SinS(angle) * parent->coreVelocity;
        sp38.z = this->actor.parent->world.pos.z + Math_CosS(angle) * parent->coreVelocity;
        sp38.y = this->actor.parent->world.pos.y + phi_v0 * 20;
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &sp38), 2, 0x1000, 0x100);
    if (this->actor.parent->scale.z > 0.0f) {
        sp38.y = this->actor.parent->world.pos.y + this->initialHeightDiffFromParent;
    }
    Math_SmoothStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &sp38), 2, 0x1000, 0x100);
    if (this->actor.params == MINIDEATH_ACTION_INTRO_2) {
        EnMinideath_SetupIntro2(this);
    }
}

void EnMinideath_SetupIntro2(EnMinideath* this) {
    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    this->actionFunc = EnMinideath_Intro2;
    this->actor.speed = 5.0f;
}

void EnMinideath_Intro2(EnMinideath* this, PlayState* play) {
    Actor_WorldDistXYZToPoint(&this->actor, &this->actor.parent->focus.pos);
    Math_ScaledStepToS(&this->actor.shape.rot.y,
                       Actor_WorldYawTowardPoint(&this->actor, &this->actor.parent->focus.pos), 0x800);
    Math_ScaledStepToS(&this->actor.shape.rot.x,
                       Actor_WorldPitchTowardPoint(&this->actor, &this->actor.parent->focus.pos), 0x800);
    if (this->actor.params == MINIDEATH_ACTION_INTRO_3) {
        this->collider.base.atFlags |= AT_ON;
        EnMinideath_SetupIntro3(this);
    }
}

void EnMinideath_SetupCrowdParent(EnMinideath* this) {
    this->actor.shape.rot.x = 0;
    this->actionFunc = EnMinideath_CrowdParent;
    this->actor.speed = 0.0f;
}

void EnMinideath_CrowdParent(EnMinideath* this, PlayState* play) {
    s16 yaw = this->spawnShapeYaw + this->actor.parent->shape.rot.y;

    this->actor.world.pos.x = this->actor.parent->world.pos.x + Math_SinS(yaw) * 60.0f;
    this->actor.world.pos.z = this->actor.parent->world.pos.z + Math_CosS(yaw) * 60.0f;
    this->actor.world.pos.y = this->actor.parent->world.pos.y + this->initialHeightDiffFromParent;
    this->actor.shape.rot.y = this->actor.parent->shape.rot.y;
}

void EnMinideath_SetupScatter(EnMinideath* this) {
    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    this->actor.speed = 8.0f;
    func_800BE33C(&this->actor.parent->world.pos, &this->actor.world.pos, &this->moveDirection, false);
    if (sScatterTimer != 0) {
        this->timer = sScatterTimer;
    } else {
        this->timer = 200;
    }
    EnMinideath_UpdateCrowdState(this, false);
    this->actionFunc = EnMinideath_Scatter;
}

void EnMinideath_Scatter(EnMinideath* this, PlayState* play) {
    f32 y;
    f32 parentY;

    Math_StepToF(&this->actor.speed, 6.0f, 0.5f);

    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->moveDirection.y, 0x480) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        // Fly away from walls
        this->moveDirection.y = ((s32)Rand_Next() >> 18) + this->actor.wallYaw;
    }

    if (Math_ScaledStepToS(&this->actor.shape.rot.x, this->moveDirection.x, 0x480)) {
        y = this->actor.world.pos.y;
        parentY = this->actor.parent->home.pos.y;

        if (y < parentY + 50.0f) {
            this->moveDirection.x = -0x800 - (Rand_Next() >> 20);
        } else if (y > parentY + 200.0f) {
            this->moveDirection.x = 0x800 + (Rand_Next() >> 20);
        }
    }

    if (this->timer == 0) {
        EnMinideath_SetupReturn(this);
    } else {
        this->timer--;
    }
}

void EnMinideath_SetupReturn(EnMinideath* this) {
    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    this->actionFunc = EnMinideath_Return;
}

void EnMinideath_Return(EnMinideath* this, PlayState* play) {
    s32 pad;
    f32 distToParent = Actor_WorldDistXYZToPoint(&this->actor, &this->actor.parent->focus.pos);
    f32 velocityFactor = distToParent * (1.0f / 60.0f);

    if (velocityFactor > 1.0f) {
        velocityFactor = 1.0f;
    }
    Math_StepToF(&this->actor.speed, 6.0f * velocityFactor, 0.5f);
    Math_ScaledStepToS(&this->actor.shape.rot.y,
                       Actor_WorldYawTowardPoint(&this->actor, &this->actor.parent->focus.pos), 0x800);
    Math_ScaledStepToS(&this->actor.shape.rot.x,
                       Actor_WorldPitchTowardPoint(&this->actor, &this->actor.parent->focus.pos), 0x800);
    if (distToParent < 30.0f) {
        EnMinideath_SetupIntro3(this);
    }
}

void EnMinideath_SetupIntro3(EnMinideath* this) {
    s32 i;

    this->actor.speed = 0.0f;
    EnMinideath_UpdateCrowdState(this, true);

    for (i = 0; i < MINIDEATH_NUM_EFFECTS; i++) {
        this->effects[i].timer = 0;
    }
    this->actionFunc = EnMinideath_Intro3;
}

void EnMinideath_Intro3(EnMinideath* this, PlayState* play) {
    Vec3f target;
    f32 dist;
    s16 sp26;
    s32 stepDone;

    stepDone =
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->spawnShapeYaw + this->actor.parent->shape.rot.y, 0x800);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x800);
    sp26 = this->spawnShapeYaw + this->actor.parent->shape.rot.y;
    target.x = this->actor.parent->world.pos.x + Math_SinS(sp26) * 60.0f;
    target.z = this->actor.parent->world.pos.z + Math_CosS(sp26) * 60.0f;
    target.y = this->actor.parent->world.pos.y + this->initialHeightDiffFromParent;

    dist = Math_Vec3f_StepTo(&this->actor.world.pos, &target, 10.0f);
    if (stepDone && dist < 20.0f) {
        EnMinideath_SetupCrowdParent(this);
    }
}

void EnMinideath_SetupStartSwarm(EnMinideath* this) {
    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    this->actionFunc = EnMinideath_StartSwarm;
}

void EnMinideath_StartSwarm(EnMinideath* this, PlayState* play) {
    Math_Vec3f_StepTo(&this->actor.world.pos, &this->actor.parent->focus.pos, 5.0f);
}

void EnMinideath_SetupSwarmFollower(EnMinideath* this, PlayState* play) {
    s32 phi_a2;
    s32 phi_v1;
    s32 phi_a1;

    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    this->actor.speed = 8.0f;

    if (this->number >= 10) {
        phi_v1 = 19 - this->number;
        phi_a1 = 1;
        phi_a2 = 0;
    } else {
        phi_v1 = 9 - this->number;
        phi_a1 = -1;
        phi_a2 = 0x200;
    }
    this->actor.shape.rot.y = this->actor.parent->shape.rot.y + 0x4000 * phi_a1 + 0x2000 * phi_v1 * phi_a1;
    this->actor.shape.rot.x = 0xC00 + phi_a2 - 0x400 * phi_v1;

    this->actor.world.pos.x = this->actor.parent->focus.pos.x + Math_SinS(this->actor.shape.rot.y) * 60.0f;
    this->actor.world.pos.z = this->actor.parent->focus.pos.z + Math_CosS(this->actor.shape.rot.y) * 60.0f;
    this->actor.world.pos.y = this->actor.parent->focus.pos.y - Math_SinS(this->actor.shape.rot.x) * 60.0f;

    EnMinideath_RandomizeEffects(this);
    this->timer = 100;
    EnMinideath_UpdateCrowdState(this, false);

    sNumSwarmHits = 0;

    if (this->actor.child == NULL || this->number == 9) {
        this->targetActor = &GET_PLAYER(play)->actor;
    } else {
        this->targetActor = this->actor.child;
    }
    this->actionFunc = EnMinideath_SwarmFollower;
}

void EnMinideath_SwarmFollower(EnMinideath* this, PlayState* play) {
    Vec3f targetFocus;
    s16 yawOffset = 0;

    Math_StepToF(&this->actor.speed, 6.0f, 0.2f);
    if (this->timer > 0) {
        this->timer--;
    }

    if (this->targetActor->id != ACTOR_EN_MINIDEATH) {
        if (this->timer == 0) {
            this->targetActor = this->actor.parent;
        }
        Math_Vec3f_Copy(&targetFocus, &this->targetActor->focus.pos);
        if (Actor_WorldDistXYZToPoint(&this->actor, &targetFocus) > 200.0f) {
            if (this->number < 10) {
                yawOffset = -0x1C00;
            } else {
                yawOffset = 0x1C00;
            }
            targetFocus.y += 50.0f;
        }
    } else {
        if (((EnMinideath*)this->targetActor)->actionFunc != EnMinideath_SwarmFollower) {
            Math_Vec3f_Copy(&targetFocus, &this->actor.parent->focus.pos);
        } else {
            Math_Vec3f_Copy(&targetFocus, &this->targetActor->focus.pos);
        }
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &targetFocus) + yawOffset, 5,
                       0xC00, 0x80);
    Math_SmoothStepToS(&this->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&this->actor, &targetFocus), 5, 0xC00,
                       0x80);
    if ((this->collider.base.atFlags & AT_HIT) && this->timer > 0) {
        sNumSwarmHits++;
    }
    if (this->timer == 0 && Actor_WorldDistXYZToPoint(&this->actor, &this->actor.parent->focus.pos) < 50.0f) {
        EnMinideath_SetupIntro3(this);
    }
}

void EnMinideath_SetupSwarmLeader(EnMinideath* this) {
    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    this->timer = 10;
    EnMinideath_RandomizeEffects(this);
    this->actionFunc = EnMinideath_SwarmLeader;
}

void EnMinideath_SwarmLeader(EnMinideath* this, PlayState* play) {
    Vec3f target;

    if (this->timer > 0) {
        this->timer--;
    }
    target.x = (Math_SinS(this->actor.parent->shape.rot.y) * 20.0f) + this->actor.parent->focus.pos.x;
    target.z = (Math_CosS(this->actor.parent->shape.rot.y) * 20.0f) + this->actor.parent->focus.pos.z;
    target.y = this->actor.parent->focus.pos.y;
    Math_Vec3f_StepTo(&this->actor.world.pos, &target, 5.0f);
    if (this->timer == 0 && ((EnMinideath*)this->actor.child)->actionFunc != EnMinideath_SwarmFollower) {
        EnMinideath_SetupCrowdParent(this);
    }
}

void EnMinideath_SetupDeath1(EnMinideath* this) {
    s32 i;

    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    EnMinideath_UpdateCrowdState(this, false);
    this->collider.base.atFlags &= ~AT_ON;

    for (i = 0; i < MINIDEATH_NUM_EFFECTS; i++) {
        this->effects[i].state = 0;
    }

    this->moveDirection.y = this->actor.shape.rot.y;
    this->moveDirection.x = this->actor.shape.rot.x;
    this->actionFunc = EnMinideath_Death1;
}

void EnMinideath_Death1(EnMinideath* this, PlayState* play) {
    f32 y;
    f32 parentY;

    Math_StepToF(&this->actor.speed, 5.0f, 0.2f);
    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->moveDirection.y, 0x480) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        // Fly away from walls
        this->moveDirection.y = ((s32)Rand_Next() >> 18) + this->actor.wallYaw;
    }

    if (Math_ScaledStepToS(&this->actor.shape.rot.x, this->moveDirection.x, 0x480)) {
        y = this->actor.world.pos.y;
        parentY = this->actor.parent->home.pos.y;
        if (y < parentY + 50.0f) {
            this->moveDirection.x = -0x800 - (Rand_Next() >> 20);
        } else if (y > parentY + 200.0f) {
            this->moveDirection.x = 0x800 + (Rand_Next() >> 20);
        }
    }
}

void EnMinideath_SetupDie(EnMinideath* this) {
    this->actor.params = MINIDEATH_ACTION_CONTINUE;
    this->moveDirection.x = 0x4000;
    this->actionFunc = EnMinideath_Die;
    this->actor.gravity = -0.5f;
}

void EnMinideath_Die(EnMinideath* this, PlayState* play) {
    MiniDeathEffect* effect;
    s32 i;
    s32 stepsComplete = 0;

    Math_StepToF(&this->actor.speed, 0.0f, 0.2f);
    Math_ScaledStepToS(&this->actor.shape.rot.x, this->moveDirection.x, 0x480);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;

        for (effect = this->effects, i = 0; i < MINIDEATH_NUM_EFFECTS; i++, effect++) {
            if (Math_StepToF(&effect->vel.y, 0.0f, 7.0f)) {
                stepsComplete++;
            }
        }
        if (stepsComplete == MINIDEATH_NUM_EFFECTS) {
            EnMinideath_SetupDead(this);
        }
    }
}

void EnMinideath_SetupDead(EnMinideath* this) {
    this->actionFunc = EnMinideath_Dead;
}

void EnMinideath_Dead(EnMinideath* this, PlayState* play) {
    if (this->actor.parent->update == NULL) {
        Actor_Kill(&this->actor);
    } else {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
    }
}

void EnMinideath_SetNextAction(EnMinideath* this, PlayState* play) {
    s16 action = this->actor.params;

    if (action == MINIDEATH_ACTION_DEATH_1) {
        EnMinideath_SetupDeath1(this);
    } else if (action == MINIDEATH_ACTION_RETURN) {
        EnMinideath_SetupReturn(this);
    } else if (action == MINIDEATH_ACTION_START_SWARM) {
        EnMinideath_SetupStartSwarm(this);
    } else if (action == MINIDEATH_ACTION_SWARM) {
        if (this->number == 0 || this->number == 10) {
            // Bats 0 and 10 set the general motion trajectory
            EnMinideath_SetupSwarmLeader(this);
        } else {
            // These ones follow around 0 and 10
            EnMinideath_SetupSwarmFollower(this, play);
        }
    } else if (action == MINIDEATH_ACTION_SCATTER) {
        EnMinideath_SetupScatter(this);
    } else if (action == MINIDEATH_ACTION_DEATH_2) {
        EnMinideath_SetupDie(this);
    }
}

void EnMinideath_UpdateDamage(EnMinideath* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actor.colChkInfo.damageEffect == DMGEFF_STUN) {
            if (this->actionFunc == EnMinideath_CrowdParent || this->actionFunc == EnMinideath_Intro3) {
                sScatterTimer = 5;
            } else if (this->actionFunc == EnMinideath_SwarmFollower) {
                EnMinideath** miniDeaths = ((EnDeath*)this->actor.parent)->miniDeaths;
                s32 i;

                for (i = this->number - 1; i >= 0; i--) {
                    EnMinideath* other = miniDeaths[i];

                    if (other->targetActor == &this->actor) {
                        other->targetActor = this->targetActor;
                        break;
                    }
                }

                this->timer = 0;
                this->targetActor = &GET_PLAYER(play)->actor;
            }
        } else {
            s32 i;
            s32 j;
            s32 phi_a0;

            for (i = 0; i < MINIDEATH_NUM_EFFECTS; i++) {
                if (this->collider.elements[i].base.acElemFlags & ACELEM_HIT) {
                    this->collider.elements[i].base.acElemFlags &= ~(ACELEM_ON | ACELEM_HIT);
                    this->collider.elements[i].base.atElemFlags &= ~(ATELEM_ON | ATELEM_HIT);
                    this->effects[i].vel.y = -1.0f;
                    this->effects[i].state = 1;
                    this->effects[i].angle.y = this->actor.shape.rot.y;
                    this->effects[i].timer = 60;

                    if ((this->actionFunc == EnMinideath_CrowdParent || this->actionFunc == EnMinideath_Intro3) &&
                        this->actor.colChkInfo.damageEffect == DMGEFF_LIGHT_ARROW) {
                        sScatterTimer = 200;
                    }
                }
            }

            phi_a0 = 0;
            for (j = 0; j < MINIDEATH_NUM_EFFECTS; j++) {
                if (this->effects[j].state != 0) {
                    phi_a0++;
                }
            }

            if (phi_a0 > 1) {
                EnMinideath_UpdateCrowdState(this, false);
            }

            if (!sPlayedDeathSfx) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_DEAD);
                sPlayedDeathSfx = true;
            }
        }
    }
}

void EnMinideath_Update(Actor* thisx, PlayState* play) {
    EnMinideath* this = (EnMinideath*)thisx;
    s32 pad;
    ColliderJntSphElement* jntSphElem;
    s32 temp;
    s32 i;
    MiniDeathEffect* effect;

    if (sItemDropTimer > 0) {
        sItemDropTimer--;
    }

    if (this->actionFunc != EnMinideath_Dead) {
        for (i = 0; i < MINIDEATH_NUM_EFFECTS; i++) {
            if (this->effects[i].state == 0) {
                Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_BAT_FLY - SFX_FLAG);
                break;
            }
        }
    }

    EnMinideath_UpdateDamage(this, play);

    EnMinideath_SetNextAction(this, play);
    this->actionFunc(this, play);

    if (this->actionFunc != EnMinideath_Dead) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->actor.world.rot.x = -this->actor.shape.rot.x;

        if (this->actionFunc == EnMinideath_Die) {
            Actor_MoveWithGravity(&this->actor);
        } else {
            Actor_MoveWithoutGravity(&this->actor);
        }

        if (this->actionFunc != EnMinideath_CrowdParent && this->actionFunc != EnMinideath_Intro3) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 50.0f, 80.0f, 7);
        }

        if (this->actionFunc != EnMinideath_Die) {
            EnMinideath_UpdateEffects(this, play);
        }

        Actor_SetFocus(&this->actor, 0.0f);

        effect = &this->effects[0];
        jntSphElem = &this->collider.elements[0];
        for (i = 0; i != MINIDEATH_NUM_EFFECTS; i++) {
            if (effect->state == 0) {
                Math_Vec3f_Sum(&this->actor.world.pos, &effect->vel, &effect->pos);

                if (this->actionFunc != EnMinideath_Die) {
                    effect->animFrame++;
                    if (effect->animFrame == MINIDEATH_ANIM_LENGTH) {
                        effect->animFrame = 0;
                    }
                }

                jntSphElem->dim.worldSphere.center.x = effect->pos.x;
                jntSphElem->dim.worldSphere.center.y = effect->pos.y;
                jntSphElem->dim.worldSphere.center.z = effect->pos.z;
            }
            effect++;
            jntSphElem++;
        }

        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            this->attackTimer = 40;
            Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_ATTACK);
        }
        if (this->collider.base.atFlags & AT_ON) {
            temp = (this->actionFunc == EnMinideath_CrowdParent) ? (play->gameplayFrames % 4) : 4;

            if (temp == 4 || temp == (this->number & 3)) {
                if (this->attackTimer == 0 && this->actor.parent->shape.rot.x == 0) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
                }
                if (this->actionFunc != EnMinideath_StartSwarm) {
                    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
                }
            }
        }
        if (this->attackTimer > 0) {
            this->attackTimer--;
        }
        if (this->number == 19) {
            EnMinideath* other = NULL;
            sPlayedDeathSfx = false;

            if (sScatterTimer != 0) {
                do {
                    other = (EnMinideath*)SubS_FindActor(play, &other->actor, ACTORCAT_ENEMY, ACTOR_EN_MINIDEATH);
                    if (other != NULL) {
                        EnMinideath_SetupScatter(other);
                        other = (EnMinideath*)other->actor.next;
                    }
                } while (other != NULL);

                play->envCtx.lightSettingOverride = 28;

                if (sScatterTimer == 5) {
                    ((EnDeath*)this->actor.parent)->lightArrowDamageTimer = 5;
                } else {
                    ((EnDeath*)this->actor.parent)->lightArrowDamageTimer = 20;
                }
                sScatterTimer = 0;
            } else if (sNumSwarmHits >= 5) {
                do {
                    other = (EnMinideath*)SubS_FindActor(play, &other->actor, ACTORCAT_ENEMY, ACTOR_EN_MINIDEATH);
                    if (other != NULL) {
                        other->timer = 0;
                        other = (EnMinideath*)other->actor.next;
                    }
                } while (other != NULL);
                sNumSwarmHits = 0;
            }
        }
    }
}

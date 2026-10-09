/*
 * File: z_en_famos.c
 * Overlay: ovl_En_Famos
 * Description: Death Armos (floating statue enemy in Inverted Stone Tower Temple)
 */

#include "z_en_famos.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void EnFamos_Init(Actor* thisx, PlayState* play);
void EnFamos_Destroy(Actor* thisx, PlayState* play);
void EnFamos_Update(Actor* thisx, PlayState* play);
void EnFamos_Draw(Actor* thisx, PlayState* play);

void EnFamos_SetupStillIdle(EnFamos* this);
void EnFamos_StillIdle(EnFamos* this, PlayState* play);
void EnFamos_SetupPathingIdle(EnFamos* this);
void EnFamos_PathingIdle(EnFamos* this, PlayState* play);
void EnFamos_SetupTurnHome(EnFamos* this);
void EnFamos_TurnHome(EnFamos* this, PlayState* play);
void EnFamos_SetupReturnHome(EnFamos* this);
void EnFamos_ReturnHome(EnFamos* this, PlayState* play);
void EnFamos_SetupAlert(EnFamos* this);
void EnFamos_Alert(EnFamos* this, PlayState* play);
void EnFamos_SetupChase(EnFamos* this);
void EnFamos_Chase(EnFamos* this, PlayState* play);
void EnFamos_SetupAttackAim(EnFamos* this);
void EnFamos_AttackAim(EnFamos* this, PlayState* play);
void EnFamos_SetupAttack(EnFamos* this);
void EnFamos_Attack(EnFamos* this, PlayState* play);
void EnFamos_SetupFinishAttack(EnFamos* this);
void EnFamos_FinishAttack(EnFamos* this, PlayState* play);
void EnFamos_SetupAttackRebound(EnFamos* this);
void EnFamos_AttackRebound(EnFamos* this, PlayState* play);
void EnFamos_SetupScanForPlayer(EnFamos* this);
void EnFamos_ScanForPlayer(EnFamos* this, PlayState* play);
void EnFamos_SetupDeathSlam(EnFamos* this);
void EnFamos_DeathSlam(EnFamos* this, PlayState* play);
void EnFamos_SetupDeathExplosion(EnFamos* this);
void EnFamos_DeathExplosion(EnFamos* this, PlayState* play);
void EnFamos_SetupDeathFade(EnFamos* this);
void EnFamos_DeathFade(EnFamos* this, PlayState* play);

ActorProfile En_Famos_Profile = {
    /**/ ACTOR_EN_FAMOS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FAMOS,
    /**/ sizeof(EnFamos),
    /**/ EnFamos_Init,
    /**/ EnFamos_Destroy,
    /**/ EnFamos_Update,
    /**/ EnFamos_Draw,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_METAL,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x20000000, 0x04, 0x10 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x08 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 70, 10, 0, { 0, 0, 0 } },
};

// the emblem at the from and the back is one limb running front to back through the torso
// this is both sides of the emblem, front and back
static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { FAMOS_LIMB_EMBLEM, { { 2500, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { FAMOS_LIMB_EMBLEM, { { -1500, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

typedef enum {
    /* 0 */ FAMOS_ANIMATED_MAT_NORMAL, // normal is greenish
    /* 1 */ FAMOS_ANIMATED_MAT_FLIPPED // flipped is orange/yellowish
} FamosAnimatedMatArrayIndexes;

static AnimatedMaterial* sEmblemAnimatedMats[] = {
    gFamosNormalGlowingEmblemTexAnim,
    gFamosFlippedGlowingEmblemTexAnim,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_DEATH_ARMOS, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 3500, ICHAIN_STOP),
};

static s32 sAnimatedMaterialsDesgmented = false;

void EnFamos_Init(Actor* thisx, PlayState* play) {
    EnFamos* this = (EnFamos*)thisx;
    Path* path;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (FAMOS_GET_PATH_INDEX(&this->actor) != FAMOS_PATH_INDEX_NONE) {
        path = &play->setupPathList[FAMOS_GET_PATH_INDEX(&this->actor)];
        this->pathPoints = Lib_SegmentedToVirtual(path->points);
        this->pathCount = path->count;
        if (this->pathCount == 1) {
            this->pathPoints = NULL;
            this->pathCount = 0;
        }
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawSquare, 30.0f);
    SkelAnime_Init(play, &this->skelAnime, &gFamosSkel, &gFamosIdleAnim, this->jointTable, this->morphTable,
                   FAMOS_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_InitAndSetCylinder(play, &this->collider2, &this->actor, &sCylinderInit2);
    Collider_InitAndSetJntSph(play, &this->emblemCollider, &this->actor, &sJntSphInit, this->emblemColliderElements);

    if (!sAnimatedMaterialsDesgmented) { // init animated materials
        for (i = 0; i < ARRAY_COUNT(sEmblemAnimatedMats); i++) {
            sEmblemAnimatedMats[i] = Lib_SegmentedToVirtual(sEmblemAnimatedMats[i]);
        }
        sAnimatedMaterialsDesgmented = true;
    }

    this->actor.colChkInfo.mass = 250;
    this->baseHeight = this->actor.world.pos.y;
    // params: [this->actor.shape.rot.x] is used to set aggro distance
    this->aggroDistance = (FAMOS_GET_AGGRO_DISTANCE(thisx) <= 0) ? (200.0f) : (this->actor.shape.rot.x * 40.0f * 0.1f);
    this->actor.shape.rot.x = 0;
    this->actor.world.rot.x = 0;
    this->hasFinishedRotating = true;
    this->isCalm = true;
    if (this->pathPoints != NULL) {
        EnFamos_SetupPathingIdle(this);
    } else {
        EnFamos_SetupStillIdle(this);
    }
}

void EnFamos_Destroy(Actor* thisx, PlayState* play) {
    EnFamos* this = (EnFamos*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
    Collider_DestroyJntSph(play, &this->emblemCollider);
}

/**
 * Sets 20 rocks to draw from an explosion on the ground (slam attack).
 */
void EnFamos_SetupAttackDebris(EnFamos* this) {
    EnFamosRock* rock;
    f32 randFloat;
    s16 randOffset;
    s16 randVelDirection;
    s32 i;

    this->debrisTimer = 40;
    rock = &this->rocks[0];
    for (i = 0; i < ARRAY_COUNT(this->rocks); i++, rock++) {
        randVelDirection = (s32)Rand_Next() >> 0x10;
        randOffset = Rand_S16Offset(0x1800, 0x2800);
        randFloat = Rand_ZeroFloat(5.0f) + 5.0f;
        rock->velocity.x = randFloat * Math_CosS(randOffset) * Math_SinS(randVelDirection);
        rock->velocity.y = Math_SinS(randOffset) * randFloat + 3.0f;
        rock->velocity.z = randFloat * Math_CosS(randOffset) * Math_CosS(randVelDirection);
        rock->rot.x = (s32)Rand_Next() >> 0x10;
        rock->rot.y = (s32)Rand_Next() >> 0x10;
        rock->rot.z = (s32)Rand_Next() >> 0x10;
        rock->pos.x = (Math_SinS(randVelDirection) * 20.0f) + this->actor.world.pos.x;
        rock->pos.y = this->actor.floorHeight;
        rock->pos.z = (Math_CosS(randVelDirection) * 20.0f) + this->actor.world.pos.z;
        rock->scale = Rand_ZeroFloat(0.0015f) + (2.0f * 0.001f);
    }
}

/**
 * Sets 20 rocks to draw from an explosion on death.
 */
void EnFamos_SetupDeathDebris(EnFamos* this) {
    f32 randFloat;
    s16 randSmaller;
    s16 randVelDirection;
    EnFamosRock* rock;
    s32 i;

    this->debrisTimer = 40;
    rock = &this->rocks[0];
    for (i = 0; i < ARRAY_COUNT(this->rocks); i++, rock++) {
        randVelDirection = (s32)Rand_Next() >> 0x10;
        randSmaller = Rand_Next() >> 0x12;
        randFloat = Rand_ZeroFloat(6.0f) + 7.0f;
        rock->velocity.x = randFloat * Math_CosS(randSmaller) * Math_SinS(randVelDirection);
        rock->velocity.y = Math_SinS(randSmaller) * randFloat + 4.5f;
        rock->velocity.z = randFloat * Math_CosS(randSmaller) * Math_CosS(randVelDirection);
        rock->rot.x = (s32)Rand_Next() >> 0x10;
        rock->rot.y = (s32)Rand_Next() >> 0x10;
        rock->rot.z = (s32)Rand_Next() >> 0x10;
        rock->pos.x = Math_SinS(randVelDirection) * 20.0f + this->actor.world.pos.x;
        rock->pos.y = Rand_CenteredFloat(60.0f) + (this->actor.world.pos.y + 40.0f);
        rock->pos.z = Math_CosS(randVelDirection) * 20.0f + this->actor.world.pos.z;
        rock->scale = Rand_ZeroFloat(0.002f) + (2.5f * 0.001f);
    }
}

s32 EnFamos_IsPlayerSeen(EnFamos* this, PlayState* play) {
    if ((Player_GetMask(play) != PLAYER_MASK_STONE) &&
        (Actor_WorldDistXZToPoint(&GET_PLAYER(play)->actor, &this->calmPos) < this->aggroDistance) &&
        Actor_IsFacingPlayer(&this->actor, 0x5000)) {
        return true;
    } else {
        return false;
    }
}

void EnFamos_UpdateBobbingHeight(EnFamos* this) {
    if (this->hoverTimer == 0) {
        this->hoverTimer = 30;
    }

    this->hoverTimer--;
    this->actor.world.pos.y = (Math_SinS(this->hoverTimer * 0x888) * 10.0f) + this->baseHeight;

    if (ABS_ALT(this->flipRot) > 0x4000) { // is famos upside down
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FAMOS_FLOAT_REVERSE - SFX_FLAG);
    } else {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FAMOS_FLOAT - SFX_FLAG);
    }
}

/**
 * Checks if emblem has been hit with light arrow.
 *          Also checks if previously flipped; handles flip status.
 */
void EnFamos_UpdateFlipStatus(EnFamos* this) {
    u8 hasFinishedRotating;

    if (this->emblemCollider.base.acFlags & AC_HIT) { // light arrow collision, flip
        this->emblemCollider.base.acFlags &= ~AC_HIT;
        if (this->hasFinishedRotating == true) {
            if (this->animatedMaterialIndex != FAMOS_ANIMATED_MAT_NORMAL) {
                this->animatedMaterialIndex = FAMOS_ANIMATED_MAT_NORMAL;
                Actor_PlaySfx(&this->actor, NA_SE_EN_FAMOS_REVERSE2);
            } else {
                this->animatedMaterialIndex = FAMOS_ANIMATED_MAT_FLIPPED;
                this->flippedTimer = 100;
                Actor_PlaySfx(&this->actor, NA_SE_EN_FAMOS_REVERSE1);
                Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_DAMAGE);
            }
            this->hasFinishedRotating = false;
        }

    } else if (this->flippedTimer > 0) {
        this->flippedTimer--;
        if (this->flippedTimer == 0) {
            if (this->animatedMaterialIndex != FAMOS_ANIMATED_MAT_NORMAL) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_FAMOS_REVERSE2);
            }

            this->animatedMaterialIndex = FAMOS_ANIMATED_MAT_NORMAL;
            this->hasFinishedRotating = false;
        }
    }

    if (this->hasFinishedRotating == false) {
        if (this->animatedMaterialIndex != FAMOS_ANIMATED_MAT_NORMAL) {
            hasFinishedRotating = Math_ScaledStepToS(&this->flipRot, -0x8000, 0x1000);
        } else {
            hasFinishedRotating = Math_ScaledStepToS(&this->flipRot, 0, 0x1000);
        }

        this->hasFinishedRotating = hasFinishedRotating;
    }
}

/**
 * If Famos path is 0xFF, famos hovers stationary in the air
 *          facing forward, only bobbing up and down, without a path to follow.
 */
void EnFamos_SetupStillIdle(EnFamos* this) {
    this->actionFunc = EnFamos_StillIdle;
    this->actor.speed = 0.0f;
}

void EnFamos_StillIdle(EnFamos* this, PlayState* play) {
    EnFamos_UpdateBobbingHeight(this);
    if (this->isCalm) {
        Math_Vec3f_Copy(&this->calmPos, &this->actor.world.pos);
    }
    if (EnFamos_IsPlayerSeen(this, play)) {
        EnFamos_SetupAlert(this);
    }
}

/**
 * Regular Famos follows a path until seeing the player.
 */
void EnFamos_SetupPathingIdle(EnFamos* this) {
    if (this->isCalm) {
        this->waypointIndex++;
        if (this->waypointIndex == this->pathCount) {
            this->waypointIndex = 0;
        }
    } else {
        this->isCalm = true;
    }

    Math_Vec3s_ToVec3f(&this->targetDest, &this->pathPoints[this->waypointIndex]);
    this->targetYaw = Actor_WorldYawTowardPoint(&this->actor, &this->targetDest);
    this->actionFunc = EnFamos_PathingIdle;
    this->actor.speed = 0.0f;
}

void EnFamos_PathingIdle(EnFamos* this, PlayState* play) {
    EnFamos_UpdateBobbingHeight(this);
    if (this->isCalm) {
        Math_Vec3f_Copy(&this->calmPos, &this->actor.world.pos);
    }

    if (EnFamos_IsPlayerSeen(this, play)) {
        EnFamos_SetupAlert(this);
    } else if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 0x200)) {
        EnFamos_SetupReturnHome(this);
    }
}

/**
 * Famos lost player; Turning to face back toward home.
 */
void EnFamos_SetupTurnHome(EnFamos* this) {
    this->targetYaw = Actor_WorldYawTowardPoint(&this->actor, &this->calmPos);
    Math_Vec3f_Copy(&this->targetDest, &this->calmPos);
    this->actionFunc = EnFamos_TurnHome;
    this->actor.speed = 0.0f;
}

void EnFamos_TurnHome(EnFamos* this, PlayState* play) {
    EnFamos_UpdateBobbingHeight(this);
    if (EnFamos_IsPlayerSeen(this, play)) {
        EnFamos_SetupAlert(this);
    } else if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 0x200)) {
        EnFamos_SetupReturnHome(this);
    }
}

/**
 * Famos has finished rotating toward home; Take off flying in straight line.
 */
void EnFamos_SetupReturnHome(EnFamos* this) {
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.world.rot.x = -Actor_WorldPitchTowardPoint(&this->actor, &this->targetDest);
    this->actionFunc = EnFamos_ReturnHome;
}

void EnFamos_ReturnHome(EnFamos* this, PlayState* play) {
    f32 distanceToHome = Actor_WorldDistXZToPoint(&this->actor, &this->targetDest);

    this->actor.shape.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->targetDest);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnFamos_UpdateBobbingHeight(this);
    if (this->isCalm) {
        Math_Vec3f_Copy(&this->calmPos, &this->actor.world.pos);
    }

    if (EnFamos_IsPlayerSeen(this, play)) {
        EnFamos_SetupAlert(this);
    } else if (distanceToHome < 20.0f) {
        if (this->pathPoints != NULL) {
            EnFamos_SetupPathingIdle(this);
        } else {
            EnFamos_SetupStillIdle(this);
        }
    } else if (distanceToHome < 40.0f) {
        Math_StepToF(&this->actor.speed, 0.5f, 0.3f);
    } else {
        Math_StepToF(&this->actor.speed, 3.0f, 0.3f);
    }
}

/**
 * Famos has spotted the player; delayed by surprise.
 */
void EnFamos_SetupAlert(EnFamos* this) {
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->stateTimer = 8;
    this->actor.speed = 0.0f;

    if (this->isCalm == true) {
        this->isCalm = false;
        Math_Vec3f_Copy(&this->calmPos, &this->actor.world.pos);
    }

    this->actionFunc = EnFamos_Alert;
}

void EnFamos_Alert(EnFamos* this, PlayState* play) {
    if (ABS_ALT(this->flipRot) > 0x4000) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FAMOS_FLOAT_REVERSE - SFX_FLAG);
    } else {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FAMOS_FLOAT - SFX_FLAG);
    }

    this->stateTimer--;
    if (this->stateTimer == 0) {
        this->actor.world.pos.y = this->baseHeight;
        EnFamos_SetupChase(this);
    } else {
        this->actor.world.pos.y = Math_SinS(this->stateTimer * 0x1000) * 30.0f + this->baseHeight;
    }
}

/**
 * Famos has spotted the player; Begin chasing to attack.
 */
void EnFamos_SetupChase(EnFamos* this) {
    this->hoverTimer = 0;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnFamos_Chase;
}

void EnFamos_Chase(EnFamos* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f abovePlayerPos;
    FloorProperty surfaceType;

    EnFamos_UpdateBobbingHeight(this);
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x800);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    abovePlayerPos.x = player->actor.world.pos.x;
    abovePlayerPos.y = player->actor.world.pos.y + 100.0f;
    abovePlayerPos.z = player->actor.world.pos.z;
    this->actor.world.rot.x = -Actor_WorldPitchTowardPoint(&this->actor, &abovePlayerPos);
    Math_StepToF(&this->actor.speed, 6.0f, 0.5f);

    surfaceType = SurfaceType_GetFloorProperty2(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
    if ((this->actor.xzDistToPlayer < 30.0f) && (this->actor.floorHeight > BGCHECK_Y_MIN) &&
        ((surfaceType != FLOOR_PROPERTY_12) && (surfaceType != FLOOR_PROPERTY_13))) {
        EnFamos_SetupAttackAim(this);

    } else if ((Player_GetMask(play) == PLAYER_MASK_STONE) ||
               (this->aggroDistance < Actor_WorldDistXZToPoint(&GET_PLAYER(play)->actor, &this->calmPos)) ||
               !Actor_IsFacingPlayer(&this->actor, 0x6000)) {
        EnFamos_SetupScanForPlayer(this);
    }
}

void EnFamos_SetupAttackAim(EnFamos* this) {
    Animation_PlayOnce(&this->skelAnime, &gFamosShakeAnim);
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_AMOS_VOICE);
    this->actionFunc = EnFamos_AttackAim;
}

void EnFamos_AttackAim(EnFamos* this, PlayState* play) {
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_LAST1_FALL_OLD - SFX_FLAG);
    if (SkelAnime_Update(&this->skelAnime)) {
        EnFamos_SetupAttack(this);
    }
}

void EnFamos_SetupAttack(EnFamos* this) {
    this->actor.world.rot.x = -0x4000;
    this->collider1.base.atFlags |= AT_ON;
    this->stateTimer = 4;
    this->actionFunc = EnFamos_Attack;
}

void EnFamos_Attack(EnFamos* this, PlayState* play) {
    s32 hitFloor;
    u32 surfaceType;

    Math_StepToF(&this->actor.speed, 20.0f, 2.0f);
    this->stateTimer--;
    if (this->stateTimer == 0) {
        this->emblemCollider.base.acFlags &= ~AC_ON;
    }

    surfaceType = SurfaceType_GetFloorProperty2(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
    hitFloor = this->actor.bgCheckFlags & BGCHECKFLAG_GROUND;
    if (hitFloor || (this->actor.floorHeight == BGCHECK_Y_MIN) || (surfaceType == FLOOR_PROPERTY_12) ||
        (surfaceType == FLOOR_PROPERTY_13)) {
        this->collider1.base.atFlags &= ~AT_ON;
        this->collider2.base.atFlags |= AT_ON;
        if (hitFloor) {
            Camera_AddQuake(GET_ACTIVE_CAM(play), 2, 15, 10);
            Rumble_Request(this->actor.xyzDistToPlayerSq, 180, 20, 100);
            EnFamos_SetupAttackDebris(this);

            // spawn crator on floor
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_TEST, this->actor.world.pos.x,
                               this->actor.floorHeight, this->actor.world.pos.z, 0, 0, 0, 0x0);

            if (this->actor.child != NULL) {
                Actor_SetScale(this->actor.child, 0.015f);
            }

            if (this->animatedMaterialIndex != FAMOS_ANIMATED_MAT_NORMAL) {
                this->cratorDespawnTimer = 70;
                EnFamos_SetupDeathSlam(this);
            } else {
                this->cratorDespawnTimer = 20;
                EnFamos_SetupFinishAttack(this);
            }
        } else {
            this->emblemCollider.base.acFlags |= AC_ON;
            EnFamos_SetupAttackRebound(this);
        }
    } else {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_LAST1_FALL_OLD - SFX_FLAG);
    }
}

/**
 * Attack has hit; Wait for animation to finish before recovering from attack.
 */
void EnFamos_SetupFinishAttack(EnFamos* this) {
    Animation_PlayOnce(&this->skelAnime, &gFamosIdleAnim);
    SkelAnime_Update(&this->skelAnime);
    this->emblemCollider.base.acFlags |= AC_ON;
    this->stateTimer = 3;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EV_EXPLOSION);
    this->actionFunc = EnFamos_FinishAttack;
}

void EnFamos_FinishAttack(EnFamos* this, PlayState* play) {
    if (this->stateTimer == 0) {
        this->collider2.base.atFlags &= ~AT_ON;
    }

    this->stateTimer--;
    if (SkelAnime_Update(&this->skelAnime)) {
        EnFamos_SetupAttackRebound(this);
    }
}

void EnFamos_SetupAttackRebound(EnFamos* this) {
    this->actor.world.rot.x = 0x4000;
    this->actionFunc = EnFamos_AttackRebound;
    this->actor.speed = 0.0f;
}

void EnFamos_AttackRebound(EnFamos* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 5.0f, 0.3f);
    if (this->actor.speed > 1.0f) {
        if (ABS_ALT(this->flipRot) > 0x4000) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FAMOS_FLOAT_REVERSE - SFX_FLAG);
        } else {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FAMOS_FLOAT - SFX_FLAG);
        }
    }

    if ((this->baseHeight < this->actor.world.pos.y) || (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING)) {
        this->actor.speed = 0.0f;
        EnFamos_SetupChase(this);
    }
}

/**
 * Looking around for player.
 */
void EnFamos_SetupScanForPlayer(EnFamos* this) {
    this->stateTimer = 60;
    this->actionFunc = EnFamos_ScanForPlayer;
    this->actor.speed = 0.0f;
}

void EnFamos_ScanForPlayer(EnFamos* this, PlayState* play) {
    EnFamos_UpdateBobbingHeight(this);
    this->stateTimer--;

    if (EnFamos_IsPlayerSeen(this, play)) {
        EnFamos_SetupAlert(this);
    } else if (this->stateTimer == 0) {
        EnFamos_SetupTurnHome(this);
    } else {
        this->actor.shape.rot.y = (s32)(Math_SinS(this->stateTimer * 0x888) * 8192.0f) + this->actor.world.rot.y;
    }
}

/**
 * Famos was upside down and hit the ground headfirst. This kills the Famos.
 */
void EnFamos_SetupDeathSlam(EnFamos* this) {
    this->emblemCollider.base.acFlags &= ~AC_ON;
    this->stateTimer = 20;
    this->actor.speed = 0.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
    this->flippedTimer = -1;
    this->actor.world.pos.y = this->actor.floorHeight - 60.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_DEAD);
    this->actionFunc = EnFamos_DeathSlam;
}

void EnFamos_DeathSlam(EnFamos* this, PlayState* play) {
    if (this->stateTimer == 17) {
        this->collider2.base.atFlags &= ~AT_ON;
    }

    if (this->stateTimer == 0) {
        EnFamos_SetupDeathExplosion(this);
    } else {
        this->stateTimer--;
    }
}

void EnFamos_SetupDeathExplosion(EnFamos* this) {
    this->actor.world.rot.x = 0x4000;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 4);
    this->stateTimer = 25;
    Math_Vec3f_Copy(&this->targetDest, &this->actor.world.pos);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = EnFamos_DeathExplosion;
}

void EnFamos_DeathExplosion(EnFamos* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 3.0f, 0.3f);
    if (this->actor.colorFilterTimer == 0) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 4);
    }

    this->actor.world.pos.x = Rand_CenteredFloat(5.0f) + this->targetDest.x;
    this->actor.world.pos.z = Rand_CenteredFloat(5.0f) + this->targetDest.z;
    if (this->stateTimer == 1) {
        EnBom* explosion = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                               this->actor.world.pos.y + 40.0f, this->actor.world.pos.z,
                                               BOMB_EXPLOSIVE_TYPE_BOMB, 0, 0, BOMB_TYPE_BODY);
        if (explosion != NULL) {
            explosion->timer = 0; // instant explosion
        }
        this->stateTimer--;

    } else if (this->stateTimer == 0) {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos,
                                   (0xD << 4)); // random item from droptable 0xD
        EnFamos_SetupDeathFade(this);

    } else {
        this->stateTimer--;
    }
}

void EnFamos_SetupDeathFade(EnFamos* this) {
    EnFamos_SetupDeathDebris(this);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.shape.shadowDraw = NULL;
    this->actionFunc = EnFamos_DeathFade;
    this->actor.speed = 0.0f;
}

void EnFamos_DeathFade(EnFamos* this, PlayState* play) {
    Actor* enBom;

    if (this->debrisTimer == 0) {
        enBom = this->actor.child;
        if (enBom != NULL) {
            enBom->parent = NULL;
        }
        Actor_Kill(&this->actor);
    }
}

void EnFamos_UpdateDebrisPosRot(EnFamos* this) {
    EnFamosRock* rock;
    s32 i;

    rock = &this->rocks[0];
    for (i = 0; i < ARRAY_COUNT(this->rocks); i++, rock++) {
        rock->velocity.y -= 1.0f;
        Math_Vec3f_Sum(&rock->pos, &rock->velocity, &rock->pos);
        rock->rot.x += (s16)((Rand_Next() >> 0x17) + 0x700);
        rock->rot.y += (s16)((Rand_Next() >> 0x17) + 0x900);
        rock->rot.z += (s16)((Rand_Next() >> 0x17) + 0xB00);
    }
}

void EnFamos_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFamos* this = (EnFamos*)thisx;
    f32 oldHeight;
    s32 oldHoverTimer; // save old value to test if changed

    if ((this->debrisTimer <= 0) ||
        (this->debrisTimer--, EnFamos_UpdateDebrisPosRot(this), (this->actionFunc != EnFamos_DeathFade))) {
        oldHoverTimer = this->hoverTimer;
        EnFamos_UpdateFlipStatus(this);
        if (this->cratorDespawnTimer > 0) {
            this->cratorDespawnTimer--;
            if (this->cratorDespawnTimer == 0) {
                this->actor.child->parent = NULL; // child is EnTest (Crator after stomp)
            }
        }

        this->actionFunc(this, play);
        oldHeight = this->actor.world.pos.y;
        Actor_MoveWithoutGravity(&this->actor);
        if (oldHoverTimer != this->hoverTimer) { // test if updated in actionFunc
            this->baseHeight += this->actor.world.pos.y - oldHeight;
        }

        if (this->flippedTimer >= 0) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 30.0f, 80.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                        UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
            if ((this->actionFunc == EnFamos_Attack) && (this->animatedMaterialIndex != FAMOS_ANIMATED_MAT_NORMAL) &&
                (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                this->actor.world.pos.y -= 60.0f;
            }
        }

        this->actor.focus.rot.y = this->actor.shape.rot.y;
        Collider_UpdateCylinder(&this->actor, &this->collider1);
        if (this->collider1.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
        }

        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        if (this->emblemCollider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->emblemCollider.base);
        }

        if (this->collider2.base.atFlags & AT_ON) {
            Collider_UpdateCylinder(&this->actor, &this->collider2);
            this->collider2.dim.pos.y = this->actor.floorHeight;
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
        }
    }
}

s32 EnFamos_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFamos* this = (EnFamos*)thisx;

    if (limbIndex == FAMOS_LIMB_BODY) {
        Matrix_Translate(0.0f, 4000.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateZS(this->flipRot, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -4000.0f, 0.0f, MTXMODE_APPLY);

    } else if (this->flippedTimer < 0) { // if set to -1, famos is dying
        if ((limbIndex == FAMOS_LIMB_SWORD) || (limbIndex == FAMOS_LIMB_SHIELD) || (limbIndex == FAMOS_LIMB_HEAD)) {
            *dList = NULL;
        }
    }

    return false;
}

void EnFamos_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnFamos* this = (EnFamos*)thisx;

    if (limbIndex == FAMOS_LIMB_EMBLEM) {
        Matrix_MultZero(&this->actor.focus.pos);
        Collider_UpdateSpheres(limbIndex, &this->emblemCollider);
    }
}

void EnFamos_DrawDebris(EnFamos* this, PlayState* play) {
    s32 i;

    if (this->debrisTimer > 0) {
        Gfx* dispOpa;
        EnFamosRock* rock;

        OPEN_DISPS(play->state.gfxCtx);

        dispOpa = POLY_OPA_DISP;

        gSPDisplayList(&dispOpa[0], gSetupDLs[SETUPDL_25]);

        gDPSetPrimColor(&dispOpa[1], 0, 0x80, 255, 255, 255, 255);

        gDPSetEnvColor(&dispOpa[2], 255, 255, 255, 255);

        rock = &this->rocks[0];
        for (i = 0; i < ARRAY_COUNT(this->rocks); i++, rock++) {

            Matrix_SetTranslateRotateYXZ(rock->pos.x, rock->pos.y, rock->pos.z, &rock->rot);
            Matrix_Scale(rock->scale, rock->scale, rock->scale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(&dispOpa[3 + i * 2], play->state.gfxCtx);

            gSPDisplayList(&dispOpa[4 + i * 2], &gameplay_keep_DL_06AB30); // greenish brown rock DL
        }

        POLY_OPA_DISP = &dispOpa[3 + (ARRAY_COUNT(this->rocks) * 2)];

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnFamos_Draw(Actor* thisx, PlayState* play) {
    EnFamos* this = (EnFamos*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if (this->actionFunc != EnFamos_DeathFade) {
        AnimatedMat_Draw(play, sEmblemAnimatedMats[this->animatedMaterialIndex]);
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnFamos_OverrideLimbDraw,
                          EnFamos_PostLimbDraw, &this->actor);
        if (this->actor.colorFilterTimer != 0) {
            func_800AE5A0(play);
        }
    }
    EnFamos_DrawDebris(this, play);
}

/*
 * File: z_en_mb.c
 * Overlay: ovl_En_Mb
 * Description: Moblins
 */

#include "z_en_mb.h"
#include "objects/object_mb/object_mb.h"

/*
 * This actor can have three behaviors:
 * - "Spear Guard" (variable -1): uses a spear, walks around home point, charges player if too close
 * - "Club" (variable 0): uses a club, stands still and smashes its club on the ground when the player approaches
 * - "Spear Patrol" (variable 0xPP00 PP=pathId): uses a spear, patrols following a path, charges
 */

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

typedef enum {
    /* -1 */ ENMB_TYPE_SPEAR_GUARD = -1,
    /*  0 */ ENMB_TYPE_CLUB,
    /*  1 */ ENMB_TYPE_SPEAR_PATROL
} EnMbType;

#define ENMB_ATTACK_NONE 0
#define ENMB_ATTACK_SPEAR 1
#define ENMB_ATTACK_CLUB_RIGHT 1
#define ENMB_ATTACK_CLUB_MIDDLE 2
#define ENMB_ATTACK_CLUB_LEFT 3

/* Spear and Club moblins use a different skeleton but the limbs are organized the same */
typedef enum {
    /*  1 */ ENMB_LIMB_ROOT = 1,
    /*  3 */ ENMB_LIMB_WAIST = 3,
    /*  6 */ ENMB_LIMB_CHEST = 6,
    /*  7 */ ENMB_LIMB_HEAD,
    /*  9 */ ENMB_LIMB_LSHOULDER = 9,
    /* 11 */ ENMB_LIMB_LFOREARM = 11,
    /* 12 */ ENMB_LIMB_LHAND,
    /* 14 */ ENMB_LIMB_RSHOULDER = 14,
    /* 16 */ ENMB_LIMB_RFOREARM = 16,
    /* 17 */ ENMB_LIMB_RHAND,
    /* 20 */ ENMB_LIMB_LTHIGH = 20,
    /* 21 */ ENMB_LIMB_LSHIN,
    /* 22 */ ENMB_LIMB_LFOOT,
    /* 25 */ ENMB_LIMB_RTHIGH = 25,
    /* 26 */ ENMB_LIMB_RSHIN,
    /* 27 */ ENMB_LIMB_RFOOT
} EnMbLimb;

void EnMb_Init(Actor* thisx, PlayState* play);
void EnMb_Destroy(Actor* thisx, PlayState* play);
void EnMb_Update(Actor* thisx, PlayState* play);
void EnMb_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Mb_InitVars = {
    ACTOR_EN_MB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_MB,
    sizeof(EnMb),
    (ActorFunc)EnMb_Init,
    (ActorFunc)EnMb_Destroy,
    (ActorFunc)EnMb_Update,
    (ActorFunc)EnMb_Draw,
    NULL,
};

void EnMb_SetupSpearPatrolTurnTowardsWaypoint(EnMb* this, PlayState* play);
void EnMb_SetupClubWaitPlayerNear(EnMb* this);
void EnMb_SpearGuardLookAround(EnMb* this, PlayState* play);
void EnMb_SetupSpearGuardLookAround(EnMb* this);
void EnMb_SetupSpearDamaged(EnMb* this);
void EnMb_SpearGuardWalk(EnMb* this, PlayState* play);
void EnMb_SpearGuardPrepareAndCharge(EnMb* this, PlayState* play);
void EnMb_SpearPatrolPrepareAndCharge(EnMb* this, PlayState* play);
void EnMb_SpearEndChargeQuick(EnMb* this, PlayState* play);
void EnMb_Stunned(EnMb* this, PlayState* play);
void EnMb_ClubDead(EnMb* this, PlayState* play);
void EnMb_ClubDamagedWhileKneeling(EnMb* this, PlayState* play);
void EnMb_ClubWaitPlayerNear(EnMb* this, PlayState* play);
void EnMb_ClubAttack(EnMb* this, PlayState* play);
void EnMb_SpearDead(EnMb* this, PlayState* play);
void EnMb_SpearDamaged(EnMb* this, PlayState* play);
void EnMb_SetupSpearDead(EnMb* this);
void EnMb_SpearPatrolTurnTowardsWaypoint(EnMb* this, PlayState* play);
void EnMb_SpearPatrolWalkTowardsWaypoint(EnMb* this, PlayState* play);
void EnMb_SpearPatrolEndCharge(EnMb* this, PlayState* play);
void EnMb_SpearPatrolImmediateCharge(EnMb* this, PlayState* play);
void EnMb_ClubWaitAfterAttack(EnMb* this, PlayState* play);
void EnMb_ClubDamaged(EnMb* this, PlayState* play);

static ColliderCylinderInit sHitboxInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit sFrontShieldingTrisInit[2] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit sFrontShieldingInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    sFrontShieldingTrisInit,
};

static ColliderQuadInit sAttackColliderInit = {
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
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ ENMB_DMGEFF_IGNORE,
    /* 0x1 */ ENMB_DMGEFF_STUN,
    /* 0x5 */ ENMB_DMGEFF_FREEZE = 0x5,
    /* 0x6 */ ENMB_DMGEFF_STUN_ICE,
    /* 0xF */ ENMB_DMGEFF_DEFAULT = 0xF
} EnMbDamageEffect;

static DamageTable sSpearMoblinDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Deku stick    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Slingshot     */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Explosive     */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Boomerang     */ DMG_ENTRY(0, ENMB_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hammer swing  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hookshot      */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Kokiri sword  */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Master sword  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant's Knife */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Fire arrow    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Ice arrow     */ DMG_ENTRY(4, ENMB_DMGEFF_STUN_ICE),
    /* Light arrow   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 1   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Fire magic    */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Ice magic     */ DMG_ENTRY(3, ENMB_DMGEFF_STUN_ICE),
    /* Light magic   */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Shield        */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Giant spin    */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Master spin   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Kokiri jump   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant jump    */ DMG_ENTRY(8, ENMB_DMGEFF_DEFAULT),
    /* Master jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 1     */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Unblockable   */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Hammer jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 2     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
};

static DamageTable sClubMoblinDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Deku stick    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Slingshot     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Explosive     */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Boomerang     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Normal arrow  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hammer swing  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hookshot      */ DMG_ENTRY(0, ENMB_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Master sword  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant's Knife */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Fire arrow    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Ice arrow     */ DMG_ENTRY(4, ENMB_DMGEFF_STUN_ICE),
    /* Light arrow   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 1   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Fire magic    */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Ice magic     */ DMG_ENTRY(3, ENMB_DMGEFF_STUN_ICE),
    /* Light magic   */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Shield        */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Giant spin    */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Master spin   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Kokiri jump   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant jump    */ DMG_ENTRY(8, ENMB_DMGEFF_DEFAULT),
    /* Master jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 1     */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Unblockable   */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Hammer jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 2     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4A, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5300, ICHAIN_STOP),
};

void EnMb_SetupAction(EnMb* this, EnMbActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnMb_Init(Actor* thisx, PlayState* play) {
    EnMb* this = (EnMb*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 relYawFromPlayer;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 46.0f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.colChkInfo.damageTable = &sSpearMoblinDamageTable;
    Collider_InitCylinder(play, &this->hitbox);
    Collider_SetCylinder(play, &this->hitbox, &this->actor, &sHitboxInit);
    Collider_InitTris(play, &this->frontShielding);
    Collider_SetTris(play, &this->frontShielding, &this->actor, &sFrontShieldingInit, this->frontShieldingTris);
    Collider_InitQuad(play, &this->attackCollider);
    Collider_SetQuad(play, &this->attackCollider, &this->actor, &sAttackColliderInit);

    switch (this->actor.params) {
        case ENMB_TYPE_SPEAR_GUARD:
            SkelAnime_InitFlex(play, &this->skelAnime, &gEnMbSpearSkel, &gEnMbSpearStandStillAnim,
                               this->jointTable, this->morphTable, 28);
            this->actor.colChkInfo.health = 2;
            this->actor.colChkInfo.mass = MASS_HEAVY;
            this->maxHomeDist = 1000.0f;
            this->playerDetectionRange = 1750.0f;
            EnMb_SetupSpearGuardLookAround(this);
            break;
        case ENMB_TYPE_CLUB:
            SkelAnime_InitFlex(play, &this->skelAnime, &gEnMbClubSkel, &gEnMbClubStandStillClubDownAnim,
                               this->jointTable, this->morphTable, 28);

            this->actor.colChkInfo.health = 6;
            this->actor.colChkInfo.mass = MASS_IMMOVABLE;
            this->actor.colChkInfo.damageTable = &sClubMoblinDamageTable;
            Actor_SetScale(&this->actor, 0.02f);
            this->hitbox.dim.height = 170;
            this->hitbox.dim.radius = 45;
            this->actor.uncullZoneForward = 4000.0f;
            this->actor.uncullZoneScale = 800.0f;
            this->actor.uncullZoneDownward = 1800.0f;
            this->playerDetectionRange = 710.0f;
            this->attackCollider.info.toucher.dmgFlags = 0x20000000;

            relYawFromPlayer =
                this->actor.world.rot.y - Math_Vec3f_Yaw(&this->actor.world.pos, &player->actor.world.pos);
            if (ABS(relYawFromPlayer) > 0x4000) {
                this->actor.world.rot.y = thisx->world.rot.y + 0x8000;
                this->actor.shape.rot.y = thisx->world.rot.y;
                this->actor.world.pos.z = thisx->world.pos.z + 600.0f;
            }

            ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, 90.0f);
            this->actor.flags &= ~ACTOR_FLAG_0;
            this->actor.naviEnemyId += 1;
            EnMb_SetupClubWaitPlayerNear(this);
            break;
        default: /* Spear Patrol */
            SkelAnime_InitFlex(play, &this->skelAnime, &gEnMbSpearSkel, &gEnMbSpearStandStillAnim,
                               this->jointTable, this->morphTable, 28);

            Actor_SetScale(&this->actor, 0.014f);
            this->path = (thisx->params & 0xFF00) >> 8;
            this->actor.params = ENMB_TYPE_SPEAR_PATROL;
            this->waypoint = 0;
            this->actor.colChkInfo.health = 1;
            this->actor.colChkInfo.mass = MASS_HEAVY;
            this->maxHomeDist = 350.0f;
            this->playerDetectionRange = 1750.0f;
            this->actor.flags &= ~ACTOR_FLAG_0;
            EnMb_SetupSpearPatrolTurnTowardsWaypoint(this, play);
            break;
    }
}

void EnMb_Destroy(Actor* thisx, PlayState* play) {
    EnMb* this = (EnMb*)thisx;

    Collider_DestroyTris(play, &this->frontShielding);
    Collider_DestroyCylinder(play, &this->hitbox);
    Collider_DestroyQuad(play, &this->attackCollider);
}

void EnMb_FaceWaypoint(EnMb* this, PlayState* play) {
    s16 yawToWaypoint = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);

    this->actor.shape.rot.y = yawToWaypoint;
    this->actor.world.rot.y = yawToWaypoint;
}

void EnMb_NextWaypoint(EnMb* this, PlayState* play) {
    Path* path;
    Vec3s* waypointPos;

    path = &play->setupPathList[this->path];

    if (this->waypoint == 0) {
        this->direction = 1;
    } else if (this->waypoint == (s8)(path->count - 1)) {
        this->direction = -1;
    }

    this->waypoint += this->direction;
    waypointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->waypoint;
    this->waypointPos.x = waypointPos->x;
    this->waypointPos.y = waypointPos->y;
    this->waypointPos.z = waypointPos->z;
}

/**
 * Checks if the player is in a 800*74 units XZ area centered on this actor,
 * the area being directed along its line of sight snapped to a cardinal angle.
 * Note: the longest corridor in Sacred Forest Meadows is 800 units long,
 *       and they all are 100 units wide.
 */
s32 EnMb_IsPlayerInCorridor(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 xFromPlayer;
    f32 zFromPlayer;
    f32 cos;
    f32 sin;
    f32 xFromPlayerAbs;
    f32 zFromPlayerAbs;
    s16 alignedYaw = 0;

    if ((this->actor.world.rot.y < -0x62A2) || (this->actor.world.rot.y > 0x62A2)) {
        alignedYaw = -0x8000;
    } else if (this->actor.world.rot.y < -0x20E0) {
        alignedYaw = -0x4000;
    } else if (this->actor.world.rot.y > 0x20E0) {
        alignedYaw = 0x4000;
    }

    cos = Math_CosS(alignedYaw);
    sin = Math_SinS(alignedYaw);
    cos = ABS(cos);
    sin = ABS(sin);
    xFromPlayer = this->actor.world.pos.x - player->actor.world.pos.x;
    zFromPlayer = this->actor.world.pos.z - player->actor.world.pos.z;
    xFromPlayerAbs = ABS(xFromPlayer);
    if (xFromPlayerAbs < (cos * 37.0f + sin * 400.0f)) {
        zFromPlayerAbs = ABS(zFromPlayer);
        if (zFromPlayerAbs < (sin * 37.0f + cos * 400.0f)) {
            return true;
        }
    }
    return false;
}

void EnMb_FindWaypointTowardsPlayer(EnMb* this, PlayState* play) {
    Path* path = &play->setupPathList[this->path];
    s16 yawToWaypoint;
    Vec3f waypointPosF;
    Vec3s* waypointPosS;
    s16 yawPlayerToWaypoint;
    s32 i;
    s32 waypoint;

    for (waypoint = 0, i = path->count - 1; i >= 0; i--, waypoint++) {
        waypointPosS = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + waypoint;
        waypointPosF.x = waypointPosS->x;
        waypointPosF.y = waypointPosS->y;
        waypointPosF.z = waypointPosS->z;
        yawToWaypoint = Math_Vec3f_Yaw(&this->actor.world.pos, &waypointPosF);
        yawPlayerToWaypoint = yawToWaypoint - this->actor.yawTowardsPlayer;
        if (ABS(yawPlayerToWaypoint) <= 0x1770) {
            this->actor.world.rot.y = yawToWaypoint;
            if (waypoint == this->waypoint) {
                this->direction = -this->direction;
            }
            this->waypointPos = waypointPosF;
            this->waypoint = waypoint;
            break;
        }
    }
}

void EnMb_SetupSpearGuardLookAround(EnMb* this) {
    Animation_MorphToLoop(&this->skelAnime, &gEnMbSpearLookLeftAndRightAnim, -4.0f);
    this->actor.speedXZ = 0.0f;
    this->timer1 = Rand_S16Offset(30, 50);
    this->state = ENMB_STATE_IDLE;
    EnMb_SetupAction(this, EnMb_SpearGuardLookAround);
}

void EnMb_SetupClubWaitPlayerNear(EnMb* this) {
    Animation_PlayLoop(&this->skelAnime, &gEnMbClubStandStillClubDownAnim);
    this->actor.speedXZ = 0.0f;
    this->timer1 = Rand_S16Offset(30, 50);
    this->state = ENMB_STATE_IDLE;
    EnMb_SetupAction(this, EnMb_ClubWaitPlayerNear);
}

void EnMb_SetupSpearPatrolTurnTowardsWaypoint(EnMb* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gEnMbSpearLookLeftAndRightAnim, -4.0f);
    this->actor.speedXZ = 0.0f;
    this->timer1 = Rand_S16Offset(40, 80);
    this->state = ENMB_STATE_IDLE;
    EnMb_NextWaypoint(this, play);
    EnMb_SetupAction(this, EnMb_SpearPatrolTurnTowardsWaypoint);
}

void EnMb_SetupSpearGuardWalk(EnMb* this) {
    Animation_Change(&this->skelAnime, &gEnMbSpearWalkAnim, 0.0f, 0.0f, Animation_GetLastFrame(&gEnMbSpearWalkAnim),
                     ANIMMODE_LOOP, -4.0f);
    this->actor.speedXZ = 0.59999996f;
    this->timer1 = Rand_S16Offset(50, 70);
    this->unk_332 = 1;
    this->state = ENMB_STATE_WALK;
    EnMb_SetupAction(this, EnMb_SpearGuardWalk);
}

void EnMb_SetupSpearPatrolWalkTowardsWaypoint(EnMb* this) {
    f32 frameCount = Animation_GetLastFrame(&gEnMbSpearWalkAnim);

    this->actor.speedXZ = 0.59999996f;
    this->timer1 = Rand_S16Offset(50, 70);
    this->unk_332 = 1;
    this->state = ENMB_STATE_WALK;
    Animation_Change(&this->skelAnime, &gEnMbSpearWalkAnim, 0.0f, 0.0f, frameCount, ANIMMODE_LOOP_INTERP, -4.0f);
    EnMb_SetupAction(this, EnMb_SpearPatrolWalkTowardsWaypoint);
}

void EnMb_SetupSpearPrepareAndCharge(EnMb* this) {
    f32 frameCount = Animation_GetLastFrame(&gEnMbSpearPrepareChargeAnim);

    Animation_MorphToPlayOnce(&this->skelAnime, &gEnMbSpearPrepareChargeAnim, -4.0f);
    this->state = ENMB_STATE_ATTACK;
    this->actor.speedXZ = 0.0f;
    this->timer3 = (s16)frameCount + 6;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_SPEAR_AT);
    if (this->actor.params == ENMB_TYPE_SPEAR_GUARD) {
        EnMb_SetupAction(this, EnMb_SpearGuardPrepareAndCharge);
    } else {
        EnMb_SetupAction(this, EnMb_SpearPatrolPrepareAndCharge);
    }
}

void EnMb_SetupSpearPatrolImmediateCharge(EnMb* this) {
    Animation_PlayLoop(&this->skelAnime, &gEnMbSpearChargeAnim);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_ATTACK);
    this->attack = ENMB_ATTACK_SPEAR;
    this->state = ENMB_STATE_ATTACK;
    this->timer3 = 3;
    this->actor.speedXZ = 10.0f;
    EnMb_SetupAction(this, EnMb_SpearPatrolImmediateCharge);
}

void EnMb_SetupClubAttack(EnMb* this) {
    f32 frames = Animation_GetLastFrame(&gEnMbClubLiftClubAnim);
    s16 relYawFromPlayer;

    this->state = ENMB_STATE_ATTACK;
    Animation_Change(&this->skelAnime, &gEnMbClubLiftClubAnim, 3.0f, 0.0f, frames, ANIMMODE_ONCE_INTERP, 0.0f);
    this->timer3 = 1;
    relYawFromPlayer = this->actor.world.rot.y - this->actor.yawTowardsPlayer;

    if (ABS(relYawFromPlayer) <= 0x258) {
        this->attack = ENMB_ATTACK_CLUB_MIDDLE;
    } else if (relYawFromPlayer >= 0) {
        this->attack = ENMB_ATTACK_CLUB_RIGHT;
    } else {
        this->attack = ENMB_ATTACK_CLUB_LEFT;
    }

    EnMb_SetupAction(this, EnMb_ClubAttack);
}

void EnMb_SetupSpearEndChargeQuick(EnMb* this) {
    Animation_PlayOnce(&this->skelAnime, &gEnMbSpearSlowDownAnim);
    this->state = ENMB_STATE_ATTACK_END;
    this->timer1 = 0;
    this->timer3 = 5;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_SLIDE);
    EnMb_SetupAction(this, EnMb_SpearEndChargeQuick);
}

void EnMb_SetupSpearPatrolEndCharge(EnMb* this) {
    Animation_PlayOnce(&this->skelAnime, &gEnMbSpearSlowDownAnim);
    this->state = ENMB_STATE_ATTACK_END;
    this->actor.bgCheckFlags &= ~1;
    this->timer1 = 0;
    this->timer3 = 50;
    this->actor.speedXZ = -8.0f;
    this->actor.velocity.y = 6.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_SLIDE);
    EnMb_SetupAction(this, EnMb_SpearPatrolEndCharge);
}

void EnMb_SetupClubWaitAfterAttack(EnMb* this) {
    f32 frameCount = Animation_GetLastFrame(&gEnMbClubStandStillClubDownAnim);

    this->state = ENMB_STATE_ATTACK_END;
    Animation_Change(&this->skelAnime, &gEnMbClubStandStillClubDownAnim, 5.0f, 0.0f, frameCount, ANIMMODE_ONCE_INTERP,
                     0.0f);
    EnMb_SetupAction(this, EnMb_ClubWaitAfterAttack);
}

void EnMb_SetupClubDamaged(EnMb* this) {
    Animation_PlayOnce(&this->skelAnime, &gEnMbClubDamagedKneelAnim);
    this->state = ENMB_STATE_CLUB_KNEELING;
    this->timer1 = 0;
    this->timer3 = 20;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_DEAD);
    EnMb_SetupAction(this, EnMb_ClubDamaged);
}

void EnMb_SetupClubDamagedWhileKneeling(EnMb* this) {
    f32 frames = Animation_GetLastFrame(&gEnMbClubBeatenKneelingAnim);

    this->state = ENMB_STATE_CLUB_KNEELING_DAMAGED;
    this->timer1 = 0;
    this->timer3 = 6;
    Animation_Change(&this->skelAnime, &gEnMbClubBeatenKneelingAnim, 1.0f, 4.0f, frames, ANIMMODE_ONCE_INTERP, 0.0f);
    EnMb_SetupAction(this, EnMb_ClubDamagedWhileKneeling);
}

void EnMb_SetupClubDead(EnMb* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gEnMbClubFallOnItsBackAnim, -4.0f);
    this->state = ENMB_STATE_CLUB_DEAD;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->hitbox.dim.height = 80;
    this->hitbox.dim.radius = 95;
    this->timer1 = 30;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_DEAD);
    EnMb_SetupAction(this, EnMb_ClubDead);
}

void EnMb_SetupStunned(EnMb* this) {
    this->state = ENMB_STATE_STUNNED;
    this->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&this->actor, 0, 0x78, 0, 0x50);
    if (this->damageEffect == ENMB_DMGEFF_STUN_ICE) {
        this->iceEffectTimer = 40;
    } else {
        if (this->actor.params != ENMB_TYPE_CLUB) {
            Animation_PlayOnceSetSpeed(&this->skelAnime, &gEnMbSpearDamagedFromFrontAnim, 0.0f);
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    }
    EnMb_SetupAction(this, EnMb_Stunned);
}

void EnMb_Stunned(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &this->actor) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
        func_8002F71C(play, &this->actor, 4.0f, this->actor.world.rot.y, 4.0f);
        this->attack = ENMB_ATTACK_NONE;
    }

    if (this->actor.colorFilterTimer == 0) {
        if (this->actor.params == ENMB_TYPE_CLUB) {
            if (this->actor.colChkInfo.health == 0) {
                EnMb_SetupClubDead(this);
            } else if (this->state == ENMB_STATE_CLUB_KNEELING) {
                /* dead code: the setup for this action sets state to something else */
                EnMb_SetupClubDamagedWhileKneeling(this);
            } else {
                EnMb_SetupClubWaitPlayerNear(this);
            }
        } else {
            if (this->actor.colChkInfo.health == 0) {
                EnMb_SetupSpearDead(this);
            } else {
                EnMb_SetupSpearDamaged(this);
            }
        }
    }
}

void EnMb_SpearGuardLookAround(EnMb* this, PlayState* play) {
    s16 timer1;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer1 == 0) {
        timer1 = 0;
    } else {
        this->timer1--;
        timer1 = this->timer1;
    }
    if (timer1 == 0 && Animation_OnFrame(&this->skelAnime, 0.0f)) {
        EnMb_SetupSpearGuardWalk(this);
    }
}

void EnMb_SpearPatrolTurnTowardsWaypoint(EnMb* this, PlayState* play) {
    s16 relYawFromPlayer;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer1 == 0) {
        this->yawToWaypoint = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);
        if (Math_SmoothStepToS(&this->actor.shape.rot.y, this->yawToWaypoint, 1, 0x3E8, 0) == 0) {
            this->actor.world.rot.y = this->actor.shape.rot.y;
            EnMb_SetupSpearPatrolWalkTowardsWaypoint(this);
        }
    } else {
        this->timer1--;
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 1, 0x3E8, 0);
    }

    if (ABS(this->actor.yDistToPlayer) <= 20.0f && EnMb_IsPlayerInCorridor(this, play)) {
        relYawFromPlayer = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;
        if (ABS(relYawFromPlayer) <= 0x4000 || (func_8002DDE4(play) && this->actor.xzDistToPlayer < 160.0f)) {
            EnMb_FindWaypointTowardsPlayer(this, play);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_VOICE);
            EnMb_SetupSpearPrepareAndCharge(this);
        }
    }
}

/**
 * Slow down and resume walking.
 */
void EnMb_SpearEndChargeQuick(EnMb* this, PlayState* play) {
    s32 pad;

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.5f, 1.0f, 0.0f);
    if (this->actor.speedXZ > 1.0f) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
    }
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer1 == 0) {
            this->timer3--;
            if (this->timer3 == 0) {
                /* Play the charge animation in reverse: let go of the spear and stand normally */
                Animation_Change(&this->skelAnime, &gEnMbSpearPrepareChargeAnim, -1.0f,
                                 Animation_GetLastFrame(&gEnMbSpearPrepareChargeAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
                this->timer1 = 1;
                this->actor.speedXZ = 0.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_SPEAR_NORM);
            }
        } else {
            if (this->actor.params <= ENMB_TYPE_SPEAR_GUARD) {
                EnMb_SetupSpearGuardWalk(this);
                this->timer1 = this->timer2 = this->timer3 = 80;
            } else {
                EnMb_SetupSpearPatrolTurnTowardsWaypoint(this, play);
            }
        }
    }
}

void EnMb_ClubWaitAfterAttack(EnMb* this, PlayState* play) {
    this->attack = ENMB_ATTACK_NONE;
    if (SkelAnime_Update(&this->skelAnime)) {
        EnMb_SetupClubWaitPlayerNear(this);
    }
}

/**
 * Slow down, charge again if the player is near, or resume walking.
 */
void EnMb_SpearPatrolEndCharge(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 lastFrame;
    s16 relYawFromPlayer;
    s16 yawPlayerToWaypoint;

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &this->actor) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
        func_8002F71C(play, &this->actor, 4.0f, this->actor.world.rot.y, 4.0f);
    }

    if (this->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 1.5f, 0.0f);

        if (this->actor.speedXZ > 1.0f) {
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
        }

        if (this->timer1 != 0) {
            this->timer3--;
            if (this->timer3 == 0) {
                relYawFromPlayer = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;

                if (ABS(this->actor.yDistToPlayer) <= 20.0f && EnMb_IsPlayerInCorridor(this, play) &&
                    ABS(relYawFromPlayer) <= 0x4000 && this->actor.xzDistToPlayer <= 200.0f) {
                    EnMb_SetupSpearPrepareAndCharge(this);
                } else {
                    lastFrame = Animation_GetLastFrame(&gEnMbSpearPrepareChargeAnim);
                    /* Play the charge animation in reverse: let go of the spear and stand normally */
                    Animation_Change(&this->skelAnime, &gEnMbSpearPrepareChargeAnim, -1.0f, lastFrame, 0.0f,
                                     ANIMMODE_ONCE, 0.0f);
                    this->actor.speedXZ = 0.0f;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_SPEAR_NORM);
                }
            } else {
                if (this->actor.xzDistToPlayer <= 160.0f) {
                    this->actor.speedXZ = -5.0f;
                } else {
                    this->actor.speedXZ = 0.0f;
                }
            }
        }

        if (SkelAnime_Update(&this->skelAnime)) {
            if (this->timer1 == 0) {
                lastFrame = Animation_GetLastFrame(&gEnMbSpearChargeAnim);
                Animation_Change(&this->skelAnime, &gEnMbSpearChargeAnim, 0.5f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP,
                                 0.0f);
                this->timer1 = 1;
            } else {
                yawPlayerToWaypoint =
                    Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos) - this->actor.yawTowardsPlayer;

                if (ABS(yawPlayerToWaypoint) <= 0x4000) {
                    EnMb_SetupSpearPatrolTurnTowardsWaypoint(this, play);
                } else {
                    EnMb_SetupSpearPatrolWalkTowardsWaypoint(this);
                }
            }
        }
    }
}

/**
 * Prepare charge (animation), then charge until the player isn't in front.
 */
void EnMb_SpearGuardPrepareAndCharge(EnMb* this, PlayState* play) {
    s32 prevFrame;
    s16 relYawTowardsPlayerAbs = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (relYawTowardsPlayerAbs < 0) {
        relYawTowardsPlayerAbs = -relYawTowardsPlayerAbs;
    }

    prevFrame = this->skelAnime.curFrame;

    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayLoop(&this->skelAnime, &gEnMbSpearChargeAnim);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_ATTACK);
    }

    if (this->timer3 != 0) {
        this->timer3--;
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    } else {
        this->actor.speedXZ = 10.0f;
        this->attack = ENMB_ATTACK_SPEAR;
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
        if (prevFrame != (s32)this->skelAnime.curFrame &&
            ((s32)this->skelAnime.curFrame == 2 || (s32)this->skelAnime.curFrame == 6)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_DASH);
        }
    }

    if (relYawTowardsPlayerAbs > 0x1388) {
        this->attack = ENMB_ATTACK_NONE;
        EnMb_SetupSpearEndChargeQuick(this);
    }
}

void EnMb_ClubAttack(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    Vec3f effSpawnPos;
    Vec3f effWhiteShockwaveDynamics = { 0.0f, 0.0f, 0.0f };
    f32 flamesParams[] = { 18.0f, 18.0f, 0.0f };
    s16 flamesUnused[] = { 20, 40, 0 };
    s16 relYawTarget[] = { -0x9C4, 0, 0xDAC };

    Math_SmoothStepToS(&this->actor.shape.rot.y, relYawTarget[this->attack - 1] + this->actor.world.rot.y, 1, 0x2EE, 0);

    if (this->attackCollider.base.atFlags & AT_HIT) {
        this->attackCollider.base.atFlags &= ~AT_HIT;
        if (this->attackCollider.base.at == &player->actor) {
            u8 prevPlayerInvincibilityTimer = player->invincibilityTimer;

            if (player->invincibilityTimer < 0) {
                if (player->invincibilityTimer <= -40) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    play->damagePlayer(play, -8);
                }
            }

            func_8002F71C(play, &this->actor, (650.0f - this->actor.xzDistToPlayer) * 0.04f + 4.0f,
                          this->actor.world.rot.y, 8.0f);

            player->invincibilityTimer = prevPlayerInvincibilityTimer;
        }
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer3 != 0) {
            this->timer3--;
            if (this->timer3 == 0) {
                f32 lastAnimFrame = Animation_GetLastFrame(&gEnMbClubStrikeDownAnim);
                Animation_Change(&this->skelAnime, &gEnMbClubStrikeDownAnim, 1.5f, 0.0f, lastAnimFrame,
                                 ANIMMODE_ONCE_INTERP, 0.0f);
            }
        } else {
            effSpawnPos = this->effSpawnPos;
            effSpawnPos.y = this->actor.floorHeight;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MONBLIN_HAM_LAND);
            func_800AA000(this->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
            EffectSsBlast_SpawnWhiteShockwave(play, &effSpawnPos, &effWhiteShockwaveDynamics,
                                              &effWhiteShockwaveDynamics);
            func_80033480(play, &effSpawnPos, 2.0f, 3, 0x12C, 0xB4, 1);
            Camera_AddQuake(&play->mainCamera, 2, 0x19, 5);
            func_800358DC(&this->actor, &effSpawnPos, &this->actor.world.rot, flamesParams, 20, flamesUnused, play,
                          -1, 0);
            EnMb_SetupClubWaitAfterAttack(this);
        }
    } else {
        if (this->timer3 != 0 && this->skelAnime.curFrame == 6.0f) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MONBLIN_HAM_UP);

        } else if (this->timer3 == 0 && this->skelAnime.curFrame == 3.0f) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MONBLIN_HAM_DOWN);
        }
    }
}

/**
 * Prepare charge (animation), then charge to the end of the floor collision.
 */
void EnMb_SpearPatrolPrepareAndCharge(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 prevFrame;
    s32 hasHitPlayer = false;
    s32 endCharge = !Actor_TestFloorInDirection(&this->actor, play, 110.0f, this->actor.world.rot.y);

    prevFrame = (s32)this->skelAnime.curFrame;
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayLoop(&this->skelAnime, &gEnMbSpearChargeAnim);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_ATTACK);
    }

    if (this->timer3 != 0) {
        this->timer3--;
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x1F40, 0);
        endCharge = false;
    } else {
        this->actor.speedXZ = 10.0f;
        this->attack = ENMB_ATTACK_SPEAR;
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
        if (prevFrame != (s32)this->skelAnime.curFrame &&
            ((s32)this->skelAnime.curFrame == 2 || (s32)this->skelAnime.curFrame == 6)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_DASH);
        }
    }

    if (this->attackCollider.base.atFlags & AT_HIT) {
        if (this->attackCollider.base.at == &player->actor) {
            if (!endCharge && !(player->stateFlags2 & 0x80)) {
                if (player->invincibilityTimer < 0) {
                    if (player->invincibilityTimer < -39) {
                        player->invincibilityTimer = 0;
                    } else {
                        player->invincibilityTimer = 0;
                        play->damagePlayer(play, -8);
                    }
                }
                if (!(this->attackCollider.base.atFlags & AT_BOUNCED)) {
                    Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                }
                if (play->grabPlayer(play, player)) {
                    player->actor.parent = &this->actor;
                }
            }
            hasHitPlayer = true;
        } else {
            this->attackCollider.base.atFlags &= ~AT_HIT;
        }
    }

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &this->actor) {
        player->actor.world.pos.x = this->actor.world.pos.x + Math_CosS(this->actor.shape.rot.y) * 10.0f +
                                    Math_SinS(this->actor.shape.rot.y) * 89.0f;
        hasHitPlayer = true;
        player->actor.world.pos.z = this->actor.world.pos.z + Math_SinS(this->actor.shape.rot.y) * 10.0f +
                                    Math_CosS(this->actor.shape.rot.y) * 89.0f;
        player->unk_850 = 0;
        player->actor.speedXZ = 0.0f;
        player->actor.velocity.y = 0.0f;
    }

    if (endCharge) {
        if (hasHitPlayer || (player->stateFlags2 & 0x80)) {
            this->attackCollider.base.atFlags &= ~AT_HIT;
            if (player->stateFlags2 & 0x80) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
                func_8002F71C(play, &this->actor, 4.0f, this->actor.world.rot.y, 4.0f);
            }
        }
        this->attack = ENMB_ATTACK_NONE;
        this->actor.speedXZ = -10.0f;
        EnMb_SetupSpearPatrolEndCharge(this);
    }
}

/**
 * Charge and follow the path, until hitting the player or, after some time, reaching home.
 */
void EnMb_SpearPatrolImmediateCharge(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 prevFrame;
    s32 hasHitPlayer = false;
    s32 endCharge = !Actor_TestFloorInDirection(&this->actor, play, 110.0f, this->actor.world.rot.y);

    prevFrame = (s32)this->skelAnime.curFrame;
    SkelAnime_Update(&this->skelAnime);

    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
    if (prevFrame != (s32)this->skelAnime.curFrame &&
        ((s32)this->skelAnime.curFrame == 2 || (s32)this->skelAnime.curFrame == 6)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_DASH);
    }

    if (this->attackCollider.base.atFlags & AT_HIT) {
        if (this->attackCollider.base.at == &player->actor) {
            if (!endCharge && !(player->stateFlags2 & 0x80)) {
                if (player->invincibilityTimer < 0) {
                    if (player->invincibilityTimer <= -40) {
                        player->invincibilityTimer = 0;
                    } else {
                        player->invincibilityTimer = 0;
                        play->damagePlayer(play, -8);
                    }
                }
                if (!(this->attackCollider.base.atFlags & AT_BOUNCED)) {
                    Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                }
                if (play->grabPlayer(play, player)) {
                    player->actor.parent = &this->actor;
                }
            }
            hasHitPlayer = true;
        } else {
            this->attackCollider.base.atFlags &= ~AT_HIT;
        }
    }

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &this->actor) {
        player->actor.world.pos.x = this->actor.world.pos.x + Math_CosS(this->actor.shape.rot.y) * 10.0f +
                                    Math_SinS(this->actor.shape.rot.y) * 89.0f;
        hasHitPlayer = true;
        player->actor.world.pos.z = this->actor.world.pos.z + Math_SinS(this->actor.shape.rot.y) * 10.0f +
                                    Math_CosS(this->actor.shape.rot.y) * 89.0f;
        player->unk_850 = 0;
        player->actor.speedXZ = 0.0f;
        player->actor.velocity.y = 0.0f;
    }

    if (endCharge) {
        if (hasHitPlayer || (player->stateFlags2 & 0x80)) {
            this->attackCollider.base.atFlags &= ~AT_HIT;
            if (player->stateFlags2 & 0x80) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
                func_8002F71C(play, &this->actor, 4.0f, this->actor.world.rot.y, 4.0f);
            }
            this->attack = ENMB_ATTACK_NONE;
            this->actor.speedXZ = -10.0f;
            EnMb_SetupSpearPatrolEndCharge(this);
            this->timer3 = 1;
        } else {
            this->timer3--;
            EnMb_NextWaypoint(this, play);
        }
    }

    EnMb_FaceWaypoint(this, play);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->timer3 == 0 && Math_Vec3f_DistXZ(&this->actor.home.pos, &this->actor.world.pos) < 80.0f) {
        this->attack = ENMB_ATTACK_NONE;
        EnMb_SetupSpearEndChargeQuick(this);
    }
}

void EnMb_ClubDamaged(EnMb* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer3 != 0) {
            Animation_PlayOnce(&this->skelAnime, &gEnMbClubStandUpAnim);
            this->timer3 = 0;
            func_800AA000(this->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
            Camera_AddQuake(&play->mainCamera, 2, 25, 5);
        } else {
            EnMb_SetupClubWaitPlayerNear(this);
        }
    }
}

void EnMb_ClubDamagedWhileKneeling(EnMb* this, PlayState* play) {
    s32 pad;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer3 != 0) {
            this->timer3--;
            if (this->timer3 == 0) {
                if (this->timer1 == 0) {
                    Animation_Change(&this->skelAnime, &gEnMbClubStandUpAnim, 3.0f, 0.0f,
                                     Animation_GetLastFrame(&gEnMbClubStandUpAnim), ANIMMODE_ONCE_INTERP, 0.0f);
                    this->timer1 = 1;
                    this->timer3 = 6;
                } else {
                    Animation_Change(&this->skelAnime, &gEnMbClubStandUpAnim, 3.0f, 0.0f,
                                     Animation_GetLastFrame(&gEnMbClubStandUpAnim), ANIMMODE_ONCE_INTERP, 0.0f);
                }
            }
        } else {
            EnMb_SetupClubWaitPlayerNear(this);
        }
    }
}

void EnMb_ClubDead(EnMb* this, PlayState* play) {
    Vec3f effPos;
    Vec3f effPosBase;

    effPos = this->actor.world.pos;
    effPos.x += Math_SinS(this->actor.shape.rot.y) * -70.0f;
    effPos.z += Math_CosS(this->actor.shape.rot.y) * -70.0f;
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    effPosBase = effPos;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer1 > 0) {
            Vec3f effZeroVec = { 0.0f, 0.0f, 0.0f };
            s32 i;

            this->timer1--;
            for (i = 4; i >= 0; i--) {
                effPos.x = Rand_CenteredFloat(240.0f) + effPosBase.x;
                effPos.y = Rand_CenteredFloat(15.0f) + (effPosBase.y + 20.0f);
                effPos.z = Rand_CenteredFloat(240.0f) + effPosBase.z;
                EffectSsDeadDb_Spawn(play, &effPos, &effZeroVec, &effZeroVec, 230, 7, 255, 255, 255, 255, 0, 255,
                                     0, 1, 9, true);
            }
        } else {
            Item_DropCollectibleRandom(play, &this->actor, &effPos, 0xC0);
            Actor_Kill(&this->actor);
        }
    } else if ((s32)this->skelAnime.curFrame == 15 || (s32)this->skelAnime.curFrame == 22) {
        func_800AA000(this->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
        Actor_SpawnFloorDustRing(play, &this->actor, &effPos, 50.0f, 10, 3.0f, 400, 60, false);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
        Camera_AddQuake(&play->mainCamera, 2, 25, 5);
    }
}

/**
 * Walk around the home point, face and charge the player if close.
 */
void EnMb_SpearGuardWalk(EnMb* this, PlayState* play) {
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 pad1;
    s32 pad2;
    Player* player = GET_PLAYER(play);
    s16 relYawTowardsPlayer = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    s16 yawTowardsHome;
    f32 playSpeedAbs;

    relYawTowardsPlayer = ABS(relYawTowardsPlayer);
    Math_SmoothStepToF(&this->actor.speedXZ, 0.59999996f, 0.1f, 1.0f, 0.0f);
    this->skelAnime.playSpeed = this->actor.speedXZ;
    prevFrame = this->skelAnime.curFrame;
    SkelAnime_Update(&this->skelAnime);

    playSpeedAbs = ABS(this->skelAnime.playSpeed);
    beforeCurFrame = this->skelAnime.curFrame - playSpeedAbs;
    playSpeedAbs = ABS(this->skelAnime.playSpeed);
    if (this->timer3 == 0 &&
        Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < this->playerDetectionRange) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x2EE, 0);
        this->actor.flags |= ACTOR_FLAG_0;
        if (this->actor.xzDistToPlayer < 500.0f && relYawTowardsPlayer < 0x1388) {
            EnMb_SetupSpearPrepareAndCharge(this);
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_0;
        if (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) > this->maxHomeDist || this->timer2 != 0) {
            yawTowardsHome = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
            Math_SmoothStepToS(&this->actor.world.rot.y, yawTowardsHome, 1, 0x2EE, 0);
        }
        if (this->timer2 != 0) {
            this->timer2--;
        }
        if (this->timer3 != 0) {
            this->timer3--;
        }
        if (this->timer2 == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_VOICE);
        }
        this->timer1--;
        if (this->timer1 == 0) {
            if (Rand_ZeroOne() > 0.7f) {
                this->timer1 = Rand_S16Offset(50, 70);
                this->timer2 = Rand_S16Offset(15, 40);
            } else {
                EnMb_SetupSpearGuardLookAround(this);
            }
        }
    }

    if (prevFrame != (s32)this->skelAnime.curFrame) {
        if ((beforeCurFrame <= 1 && prevFrame + (s32)playSpeedAbs >= 1) ||
            (beforeCurFrame <= 20 && prevFrame + (s32)playSpeedAbs >= 20)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnMb_SpearPatrolWalkTowardsWaypoint(EnMb* this, PlayState* play) {
    s32 prevFrame;
    s32 beforeCurFrame;
    s16 relYawTowardsPlayer;
    f32 yDistToPlayerAbs;
    f32 playSpeedABS;

    if (Math_Vec3f_DistXZ(&this->waypointPos, &this->actor.world.pos) <= 8.0f ||
        (Rand_ZeroOne() < 0.1f && Math_Vec3f_DistXZ(&this->actor.home.pos, &this->actor.world.pos) <= 4.0f)) {
        EnMb_SetupSpearPatrolTurnTowardsWaypoint(this, play);
    } else {
        Math_SmoothStepToF(&this->actor.speedXZ, 0.59999996f, 0.1f, 1.0f, 0.0f);
        this->skelAnime.playSpeed = 2.0f * this->actor.speedXZ;
    }

    this->yawToWaypoint = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->yawToWaypoint, 1, 0x5DC, 0);

    yDistToPlayerAbs = (this->actor.yDistToPlayer >= 0.0f) ? this->actor.yDistToPlayer : -this->actor.yDistToPlayer;
    if (yDistToPlayerAbs <= 20.0f && EnMb_IsPlayerInCorridor(this, play)) {
        relYawTowardsPlayer = (this->actor.shape.rot.y - this->actor.yawTowardsPlayer);
        if (ABS(relYawTowardsPlayer) <= 0x4000 || (func_8002DDE4(play) && this->actor.xzDistToPlayer < 160.0f)) {
            EnMb_FindWaypointTowardsPlayer(this, play);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_VOICE);
            EnMb_SetupSpearPrepareAndCharge(this);
            return;
        }
    }

    if (this->timer2 != 0) {
        this->timer2--;
    }
    if (this->timer3 != 0) {
        this->timer3--;
    }
    if (this->timer2 == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_VOICE);
        this->timer2 = Rand_S16Offset(30, 70);
    }

    prevFrame = this->skelAnime.curFrame;
    SkelAnime_Update(&this->skelAnime);
    playSpeedABS = ABS(this->skelAnime.playSpeed);
    beforeCurFrame = this->skelAnime.curFrame - playSpeedABS;
    playSpeedABS = (this->skelAnime.playSpeed >= 0.0f) ? this->skelAnime.playSpeed : -this->skelAnime.playSpeed;
    if (prevFrame != (s32)this->skelAnime.curFrame) {
        if ((beforeCurFrame <= 1 && (s32)playSpeedABS + prevFrame >= 1) ||
            (beforeCurFrame <= 20 && (s32)playSpeedABS + prevFrame >= 20)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnMb_ClubWaitPlayerNear(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 relYawFromPlayer = this->actor.world.rot.y - this->actor.yawTowardsPlayer;

    SkelAnime_Update(&this->skelAnime);
    if (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < this->playerDetectionRange &&
        !(player->stateFlags1 & 0x4000000) && ABS(relYawFromPlayer) < 0x3E80) {
        EnMb_SetupClubAttack(this);
    }
}

void EnMb_SetupSpearDamaged(EnMb* this) {
    s16 relYawTowardsPlayer = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(relYawTowardsPlayer) <= 0x4000) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gEnMbSpearDamagedFromFrontAnim, -4.0f);
        this->actor.speedXZ = -8.0f;
    } else {
        Animation_MorphToPlayOnce(&this->skelAnime, &gEnMbSpearDamagedFromBehindAnim, -4.0f);
        this->actor.speedXZ = 8.0f;
    }

    this->timer1 = 30;
    this->state = ENMB_STATE_SPEAR_SPEARPATH_DAMAGED;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_DEAD);
    EnMb_SetupAction(this, EnMb_SpearDamaged);
}

void EnMb_SpearDamaged(EnMb* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.params <= ENMB_TYPE_SPEAR_GUARD) {
            EnMb_SetupSpearGuardLookAround(this);
        } else {
            EnMb_SetupSpearPatrolImmediateCharge(this);
        }
    }
}

void EnMb_SetupSpearDead(EnMb* this) {
    s16 relYawTowardsPlayer = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(relYawTowardsPlayer) <= 0x4000) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gEnMbSpearFallOnItsBackAnim, -4.0f);
        this->actor.speedXZ = -8.0f;
    } else {
        /* The gEnMbSpearFallFaceDownAnim animation was probably meant to be used here */
        Animation_MorphToPlayOnce(&this->skelAnime, &gEnMbSpearFallOnItsBackAnim, -4.0f);
        this->actor.speedXZ = 8.0f;
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->timer1 = 30;
    this->state = ENMB_STATE_SPEAR_SPEARPATH_DAMAGED;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_DEAD);
    this->actor.flags &= ~ACTOR_FLAG_0;
    EnMb_SetupAction(this, EnMb_SpearDead);
}

void EnMb_SpearDead(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &this->actor) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
        func_8002F71C(play, &this->actor, 4.0f, this->actor.world.rot.y, 4.0f);
        this->attack = ENMB_ATTACK_NONE;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer1 > 0) {
            Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
            s32 i;
            Vec3f effPos;

            this->actor.shape.shadowScale = 0.0f;
            this->timer1--;
            for (i = 4; i >= 0; i--) {
                effPos.x = Rand_CenteredFloat(110.0f) + this->actor.world.pos.x;
                effPos.y = Rand_CenteredFloat(15.0f) + (this->actor.world.pos.y + 20.0f);
                effPos.z = Rand_CenteredFloat(110.0f) + this->actor.world.pos.z;

                EffectSsDeadDb_Spawn(play, &effPos, &zeroVec, &zeroVec, 100, 7, 255, 255, 255, 255, 0, 255, 0, 1,
                                     9, true);
            }
        } else {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xE0);
            Actor_Kill(&this->actor);
        }
    }
}

void EnMb_SpearUpdateAttackCollider(Actor* thisx, PlayState* play) {
    Vec3f quadModel0 = { 1000.0f, 1500.0f, 0.0f };
    Vec3f quadModel1 = { -1000.0f, 1500.0f, 0.0f };
    Vec3f quadModel2 = { 1000.0f, 1500.0f, 4500.0f };
    Vec3f quadModel3 = { -1000.0f, 1500.0f, 4500.0f };
    EnMb* this = (EnMb*)thisx;

    if (this->actor.params >= ENMB_TYPE_SPEAR_PATROL) {
        quadModel0.x += 2000.0f;
        quadModel0.z = -4000.0f;
        quadModel1.z = -4000.0f;
        quadModel2.x += 2000.0f;
        quadModel1.x -= 2000.0f;
        quadModel3.x -= 2000.0f;
        quadModel2.z += 4000.0f;
        quadModel3.z += 4000.0f;
    }
    Matrix_MultVec3f(&quadModel0, &this->attackCollider.dim.quad[1]);
    Matrix_MultVec3f(&quadModel1, &this->attackCollider.dim.quad[0]);
    Matrix_MultVec3f(&quadModel2, &this->attackCollider.dim.quad[3]);
    Matrix_MultVec3f(&quadModel3, &this->attackCollider.dim.quad[2]);
    Collider_SetQuadVertices(&this->attackCollider, &this->attackCollider.dim.quad[0],
                             &this->attackCollider.dim.quad[1], &this->attackCollider.dim.quad[2],
                             &this->attackCollider.dim.quad[3]);
}

void EnMb_ClubUpdateAttackCollider(Actor* thisx, PlayState* play) {
    static Vec3f quadModel[] = { { 1000.0f, 0.0f, 0.0f },
                                 { 1000.0f, 0.0f, 0.0f },
                                 { 1000.0f, -8000.0f, -1500.0f },
                                 { 1000.0f, -9000.0f, 2000.0f } };
    EnMb* this = (EnMb*)thisx;

    Matrix_MultVec3f(&quadModel[0], &this->attackCollider.dim.quad[1]);
    Matrix_MultVec3f(&quadModel[1], &this->attackCollider.dim.quad[0]);
    Matrix_MultVec3f(&quadModel[2], &this->attackCollider.dim.quad[3]);
    Matrix_MultVec3f(&quadModel[3], &this->attackCollider.dim.quad[2]);
    Collider_SetQuadVertices(&this->attackCollider, &this->attackCollider.dim.quad[0],
                             &this->attackCollider.dim.quad[1], &this->attackCollider.dim.quad[2],
                             &this->attackCollider.dim.quad[3]);
}

void EnMb_CheckColliding(EnMb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->frontShielding.base.acFlags & AC_HIT) {
        this->frontShielding.base.acFlags &= ~(AC_HIT | AC_BOUNCED);
        this->hitbox.base.acFlags &= ~AC_HIT;
    } else if ((this->hitbox.base.acFlags & AC_HIT) && this->state >= ENMB_STATE_STUNNED) {
        this->hitbox.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != ENMB_DMGEFF_IGNORE &&
            this->actor.colChkInfo.damageEffect != ENMB_DMGEFF_FREEZE) {
            if ((player->stateFlags2 & 0x80) && player->actor.parent == &this->actor) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
                func_8002F71C(play, &this->actor, 6.0f, this->actor.world.rot.y, 6.0f);
            }
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            this->attack = ENMB_ATTACK_NONE;
            Actor_SetDropFlag(&this->actor, &this->hitbox.info, false);
            if (this->actor.colChkInfo.damageEffect == ENMB_DMGEFF_STUN ||
                this->actor.colChkInfo.damageEffect == ENMB_DMGEFF_STUN_ICE) {
                if (this->state != ENMB_STATE_STUNNED) {
                    Actor_ApplyDamage(&this->actor);
                    EnMb_SetupStunned(this);
                }
            } else {
                Actor_ApplyDamage(&this->actor);
                Actor_SetColorFilter(&this->actor, 0x4000, 0xFA, 0, 0xC);
                if (this->actor.params == ENMB_TYPE_CLUB) {
                    if (this->actor.colChkInfo.health == 0) {
                        EnMb_SetupClubDead(this);
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB]++;
                    } else if (this->state != ENMB_STATE_CLUB_KNEELING) {
                        EnMb_SetupClubDamaged(this);
                    }
                } else {
                    if (this->actor.colChkInfo.health == 0) {
                        EnMb_SetupSpearDead(this);
                        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_MOBLIN]++;
                    } else {
                        EnMb_SetupSpearDamaged(this);
                    }
                }
            }
        }
    }
}

void EnMb_Update(Actor* thisx, PlayState* play) {
    EnMb* this = (EnMb*)thisx;
    s32 pad;

    EnMb_CheckColliding(this, play);
    if (thisx->colChkInfo.damageEffect != ENMB_DMGEFF_FREEZE) {
        this->actionFunc(this, play);
        Actor_MoveForward(thisx);
        Actor_UpdateBgCheckInfo(play, thisx, 40.0f, 40.0f, 70.0f, 0x1D);
        Actor_SetFocus(thisx, thisx->scale.x * 4500.0f);
        Collider_UpdateCylinder(thisx, &this->hitbox);
        if (thisx->colChkInfo.health <= 0) {
            this->hitbox.dim.pos.x += Math_SinS(thisx->shape.rot.y) * (-4400.0f * thisx->scale.y);
            this->hitbox.dim.pos.z += Math_CosS(thisx->shape.rot.y) * (-4400.0f * thisx->scale.y);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->hitbox.base);
        if (this->state >= ENMB_STATE_STUNNED &&
            (thisx->params == ENMB_TYPE_CLUB || this->state != ENMB_STATE_ATTACK)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->hitbox.base);
        }
        if (this->state >= ENMB_STATE_IDLE) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->frontShielding.base);
        }
        if (this->attack > ENMB_ATTACK_NONE) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->attackCollider.base);
        }
    }
}

void EnMb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f unused = { 1100.0f, -700.0f, 0.0f };
    static Vec3f feetPos = { 0.0f, 0.0f, 0.0f };
    static Vec3f effSpawnPosModel = { 0.0f, -8000.0f, 0.0f };
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 bodyPart = -1;
    EnMb* this = (EnMb*)thisx;
    Vec3f bodyPartPos;

    if (this->actor.params == ENMB_TYPE_CLUB) {
        if (limbIndex == ENMB_LIMB_LHAND) {
            Matrix_MultVec3f(&effSpawnPosModel, &this->effSpawnPos);
            if (this->attack > ENMB_ATTACK_NONE) {
                EnMb_ClubUpdateAttackCollider(&this->actor, play);
            }
        }
        Actor_SetFeetPos(&this->actor, limbIndex, ENMB_LIMB_LFOOT, &feetPos, ENMB_LIMB_RFOOT, &feetPos);
    }

    if (this->iceEffectTimer != 0) {
        switch (limbIndex) {
            case ENMB_LIMB_HEAD:
                bodyPart = 0;
                break;
            case ENMB_LIMB_LHAND:
                bodyPart = 1;
                break;
            case ENMB_LIMB_RHAND:
                bodyPart = 2;
                break;
            case ENMB_LIMB_LSHOULDER:
                bodyPart = 3;
                break;
            case ENMB_LIMB_RSHOULDER:
                bodyPart = 4;
                break;
            case ENMB_LIMB_CHEST:
                bodyPart = 5;
                break;
            case ENMB_LIMB_LTHIGH:
                bodyPart = 6;
                break;
            case ENMB_LIMB_RTHIGH:
                bodyPart = 7;
                break;
            case ENMB_LIMB_LFOOT:
                bodyPart = 8;
                break;
            case ENMB_LIMB_RFOOT:
                bodyPart = 9;
                break;
        }
        if (bodyPart >= 0) {
            Matrix_MultVec3f(&zeroVec, &bodyPartPos);
            this->bodyPartsPos[bodyPart].x = bodyPartPos.x;
            this->bodyPartsPos[bodyPart].y = bodyPartPos.y;
            this->bodyPartsPos[bodyPart].z = bodyPartPos.z;
        }
    }
}

void EnMb_Draw(Actor* thisx, PlayState* play) {
    static Vec3f frontShieldingTriModel0[] = {
        { 4000.0f, 7000.0f, 3500.0f },
        { 4000.0f, 0.0f, 3500.0f },
        { -4000.0f, 7000.0f, 3500.0f },
    };
    static Vec3f frontShieldingTriModel1[] = {
        { -4000.0f, 7000.0f, 3500.0f },
        { -4000.0f, 0.0f, 3500.0f },
        { 4000.0f, 0.0f, 3500.0f },
    };
    s32 i;
    f32 scale;
    Vec3f frontShieldingTri0[3];
    Vec3f frontShieldingTri1[3];
    s32 bodyPartIdx;
    EnMb* this = (EnMb*)thisx;

    func_80093D18(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, EnMb_PostLimbDraw, thisx);

    if (thisx->params != ENMB_TYPE_CLUB) {
        if (this->attack > ENMB_ATTACK_NONE) {
            EnMb_SpearUpdateAttackCollider(thisx, play);
        }
        for (i = 0; i < 3; i++) {
            Matrix_MultVec3f(&frontShieldingTriModel0[i], &frontShieldingTri0[i]);
            Matrix_MultVec3f(&frontShieldingTriModel1[i], &frontShieldingTri1[i]);
        }
        Collider_SetTrisVertices(&this->frontShielding, 0, &frontShieldingTri0[0], &frontShieldingTri0[1],
                                 &frontShieldingTri0[2]);
        Collider_SetTrisVertices(&this->frontShielding, 1, &frontShieldingTri1[0], &frontShieldingTri1[1],
                                 &frontShieldingTri1[2]);
    }

    if (this->iceEffectTimer != 0) {
        thisx->colorFilterTimer++;
        if (this->iceEffectTimer >= 0) {
            this->iceEffectTimer--;
        }
        if ((this->iceEffectTimer % 4) == 0) {
            scale = 2.5f;
            if (thisx->params == ENMB_TYPE_CLUB) {
                scale = 4.0f;
            }
            bodyPartIdx = this->iceEffectTimer >> 2;
            EffectSsEnIce_SpawnFlyingVec3s(play, thisx, &this->bodyPartsPos[bodyPartIdx], 150, 150, 150, 250, 235,
                                           245, 255, scale);
        }
    }
}

/*
 * File: z_en_zf.c
 * Overlay: ovl_En_Zf
 * Description: Lizalfos and Dinolfos
 */

#include "z_en_zf.h"
#include "objects/object_zf/object_zf.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnZf_Init(Actor* thisx, PlayState* play);
void EnZf_Destroy(Actor* thisx, PlayState* play);
void EnZf_Update(Actor* thisx, PlayState* play);
void EnZf_Draw(Actor* thisx, PlayState* play);
void EnZf_Reset(void);

s16 EnZf_FindPlatform(Vec3f* pos, s16 preferredIndex);
void EnZf_SetupDropIn(EnZf* this);
void EnZf_DropIn(EnZf* this, PlayState* play);
void func_80B45384(EnZf* this);
void func_80B4543C(EnZf* this, PlayState* play);
void EnZf_SetupApproachPlayer(EnZf* this, PlayState* play);
void EnZf_ApproachPlayer(EnZf* this, PlayState* play);
void EnZf_SetupJumpForward(EnZf* this);
void EnZf_JumpForward(EnZf* this, PlayState* play);
void func_80B4604C(EnZf* this);
void func_80B46098(EnZf* this, PlayState* play);
void func_80B462E4(EnZf* this, PlayState* play);
void func_80B463E4(EnZf* this, PlayState* play);
void EnZf_SetupSlash(EnZf* this);
void EnZf_Slash(EnZf* this, PlayState* play);
void EnZf_RecoilFromBlockedSlash(EnZf* this, PlayState* play);
void EnZf_SetupJumpBack(EnZf* this);
void EnZf_JumpBack(EnZf* this, PlayState* play);
void EnZf_Stunned(EnZf* this, PlayState* play);
void EnZf_SetupSheatheSword(EnZf* this, PlayState* play);
void EnZf_SheatheSword(EnZf* this, PlayState* play);
void EnZf_HopAndTaunt(EnZf* this, PlayState* play);
void EnZf_SetupHopAway(EnZf* this, PlayState* play);
void EnZf_HopAway(EnZf* this, PlayState* play);
void EnZf_DrawSword(EnZf* this, PlayState* play);
void EnZf_Damaged(EnZf* this, PlayState* play);
void EnZf_SetupJumpUp(EnZf* this);
void EnZf_JumpUp(EnZf* this, PlayState* play);
void func_80B483E4(EnZf* this, PlayState* play);
void EnZf_CircleAroundPlayer(EnZf* this, PlayState* play);
void EnZf_SetupDie(EnZf* this);
void EnZf_Die(EnZf* this, PlayState* play);
void EnZf_SetupCircleAroundPlayer(EnZf* this, f32 speed);
s32 EnZf_DodgeRangedEngaging(PlayState* play, EnZf* this);
s32 EnZf_DodgeRangedWaiting(PlayState* play, EnZf* this);

#define PLATFORM_INDEX_DOWNSTAIRS_MIN 0
#define PLATFORM_INDEX_DOWNSTAIRS_INNER_MAX 5
#define PLATFORM_INDEX_DOWNSTAIRS_MAX 7
#define PLATFORM_INDEX_UPSTAIRS_MIN (PLATFORM_INDEX_DOWNSTAIRS_MAX + 1)
#define PLATFORM_INDEX_UPSTAIRS_INNER_MAX 15

/**
 * Array of platform positions in Dodongo's Cavern miniboss room.
 *  0 -  7 : Downstairs
 *  8 - 15 : Upstairs inner platforms
 * 16 - 23 : Upstairs outer platforms (including several points on the long thin one)
 */
static Vec3f sPlatformPositions[] = {
    // Downstairs
    { 3560.0f, 100.0f, -1517.0f },
    { 3170.0f, 100.0f, -1767.0f },
    { 3165.0f, 100.0f, -2217.0f },
    { 3563.0f, 100.0f, -2437.0f },
    { 3946.0f, 100.0f, -2217.0f },
    { 3942.0f, 100.0f, -1765.0f },
    // Westmost downstairs two
    { 2861.0f, 100.0f, -2394.0f },
    { 2776.0f, 100.0f, -1987.0f },

    // Upstairs inner
    { 4527.0f, 531.0f, -1146.0f },
    { 4442.0f, 531.0f, -1405.0f },
    { 4170.0f, 531.0f, -1395.0f },
    { 4030.0f, 531.0f, -1162.0f },
    { 4010.0f, 531.0f, -883.0f },
    { 4270.0f, 531.0f, -810.0f },
    { 4520.0f, 531.0f, -880.0f },
    { 4260.0f, 531.0f, -1035.0f },

    // Upstairs outer
    { 4757.0f, 531.0f, -1146.0f },
    { 3850.0f, 531.0f, -883.0f },
    { 4380.0f, 531.0f, -690.0f },
    { 4197.0f, 531.0f, -646.0f },
    { 4070.0f, 531.0f, -1575.0f },
    { 3930.0f, 531.0f, -1705.0f },
    { 3780.0f, 531.0f, -1835.0f },
    { 3560.0f, 531.0f, -1985.0f },
};

// These seem to relate to the tagging in/out the minibosses do
s16 D_80B4A1B0 = 0;
s16 D_80B4A1B4 = 1;

const ActorInit En_Zf_InitVars = {
    ACTOR_EN_ZF,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_ZF,
    sizeof(EnZf),
    (ActorFunc)EnZf_Init,
    (ActorFunc)EnZf_Destroy,
    (ActorFunc)EnZf_Update,
    (ActorFunc)EnZf_Draw,
    (ActorResetFunc)EnZf_Reset,
};

static ColliderCylinderInit sBodyCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
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

static ColliderQuadInit sSwordQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ ENZF_DMGEFF_NONE,
    /* 0x1 */ ENZF_DMGEFF_STUN,
    /* 0x6 */ ENZF_DMGEFF_IMMUNE = 6,       // Skips damage code, but also skips the top half of Update
    /* 0xD */ ENZF_DMGEFF_PROJECTILE = 0xD, // Projectiles that don't have another damageeffect
    /* 0xF */ ENZF_DMGEFF_ICE = 0xF
} EnZfDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, ENZF_DMGEFF_PROJECTILE),
    /* Explosive     */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, ENZF_DMGEFF_NONE),
    /* Master sword  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Giant's Knife */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Fire arrow    */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Ice arrow     */ DMG_ENTRY(4, ENZF_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 1   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Fire magic    */ DMG_ENTRY(0, ENZF_DMGEFF_IMMUNE),
    /* Ice magic     */ DMG_ENTRY(3, ENZF_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, ENZF_DMGEFF_IMMUNE),
    /* Shield        */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENZF_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, ENZF_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

static AnimationHeader* sHoppingAnims[] = { &gZfHopCrouchingAnim, &gZfHopLeapingAnim, &gZfHopLandingAnim };

static s32 D_80B4AB30; // Set to 0 and incremented in EnZf_HopAway, but not actually used

void EnZf_SetupAction(EnZf* this, EnZfActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Tests if it will still be on a floor after moving forwards a distance determined by dist, in the shape forward
 * direction. If `dist` is 0, it defaults to a dist depending on speed direction, and params.
 */
s32 EnZf_PrimaryFloorCheck(EnZf* this, PlayState* play, f32 dist) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    if (dist == 0.0f) {
        dist = ((this->actor.speedXZ >= 0.0f) ? 1.0f : -1.0f);
        dist = ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) ? dist * 45.0f : dist * 30.0f);
    }

    // Save currents to restore later
    curPos = this->actor.world.pos;
    curBgCheckFlags = this->actor.bgCheckFlags;

    sin = Math_SinS(this->actor.world.rot.y) * dist;
    cos = Math_CosS(this->actor.world.rot.y) * dist;

    this->actor.world.pos.x += sin;
    this->actor.world.pos.z += cos;

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 0x1C);
    this->actor.world.pos = curPos;
    ret = !(this->actor.bgCheckFlags & 1);
    this->actor.bgCheckFlags = curBgCheckFlags;
    return ret;
}

/**
 * Supplementary floor test.
 */
s16 EnZf_SecondaryFloorCheck(EnZf* this, PlayState* play, f32 dist) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    if ((this->actor.speedXZ != 0.0f) && EnZf_PrimaryFloorCheck(this, play, this->actor.speedXZ)) {
        return true;
    }

    // Save currents to restore later
    curPos = this->actor.world.pos;
    curBgCheckFlags = this->actor.bgCheckFlags;

    sin = Math_SinS(this->actor.shape.rot.y) * dist;
    cos = Math_CosS(this->actor.shape.rot.y) * dist;

    this->actor.world.pos.x += sin;
    this->actor.world.pos.z += cos;

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 0x1C);
    this->actor.world.pos = curPos;
    ret = !(this->actor.bgCheckFlags & 1);
    this->actor.bgCheckFlags = curBgCheckFlags;
    return ret;
}

void EnZf_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZf* this = (EnZf*)thisx;
    Player* player = GET_PLAYER(play);
    EffectBlureInit1 blureInit;
    f32 posDiff;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->targetMode = 3;
    this->clearFlag = (thisx->params & 0xFF00) >> 8;
    /* Strip the top byte of params */
    thisx->params &= 0xFF;

    /* Return the params to their original value if they were originally negative, i.e. 0xFFFF or 0xFFFE */
    if (thisx->params & 0x80) {
        thisx->params |= 0xFF00;
    }

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawFeet, 90.0f);
    this->unk_3E0 = 0;
    thisx->colChkInfo.mass = MASS_HEAVY;
    thisx->colChkInfo.damageTable = &sDamageTable;

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;
    blureInit.p2StartColor[3] = 64;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    Effect_Add(play, &this->blureIndex, EFFECT_BLURE1, 0, 0, &blureInit);

    Actor_UpdateBgCheckInfo(play, thisx, 75.0f, 45.0f, 45.0f, 0x1D);

    this->alpha = 255;
    thisx->colChkInfo.cylRadius = 40;
    thisx->colChkInfo.cylHeight = 100;
    Collider_InitCylinder(play, &this->bodyCollider);
    Collider_SetCylinder(play, &this->bodyCollider, thisx, &sBodyCylinderInit);
    Collider_InitQuad(play, &this->swordCollider);
    Collider_SetQuad(play, &this->swordCollider, thisx, &sSwordQuadInit);

    if (thisx->params == ENZF_TYPE_DINOLFOS) {
        thisx->colChkInfo.health = 12;
        thisx->naviEnemyId = 0x10;
        SkelAnime_Init(play, &this->skelAnime, &gZfDinolfosSkel, &gZfCryingAnim, this->jointTable,
                       this->morphTable, ENZF_LIMB_MAX);
    } else { // Lizalfos
        thisx->colChkInfo.health = 6;
        thisx->naviEnemyId = 0x0F;
        SkelAnime_Init(play, &this->skelAnime, &gZfLizalfosSkel, &gZfCryingAnim, this->jointTable,
                       this->morphTable, ENZF_LIMB_MAX);
    }

    if (thisx->params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // Not minibosses
        this->homePlatform = this->curPlatform = -1;
        D_80B4A1B4 = -1;
        this->hopAnimIndex = 1;
        if (thisx->params == ENZF_TYPE_LIZALFOS_LONE) {
            EnZf_SetupDropIn(this);
        } else { // Dinolfos
            func_80B45384(this);
        }
    } else { // Minibosses
        posDiff = player->actor.world.pos.y - thisx->world.pos.y;

        if ((ABS(posDiff) <= 100.0f) && !Flags_GetSwitch(play, this->clearFlag)) {
            this->homePlatform = this->curPlatform = EnZf_FindPlatform(&thisx->world.pos, 0);
            EnZf_SetupDropIn(this);
            D_80B4A1B4 = 1;
        } else {
            Actor_Kill(thisx);
        }
    }
}

void EnZf_Destroy(Actor* thisx, PlayState* play) {
    EnZf* this = (EnZf*)thisx;

    if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ &&
        (Actor_FindNearby(play, &this->actor, ACTOR_EN_ZF, ACTORCAT_ENEMY, 10000.0f) == NULL)) {
        func_800F5B58();
    }

    Effect_Delete(play, this->blureIndex);
    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyQuad(play, &this->swordCollider);
}

/**
 * Finds the index of the platform position in `sPlatformPositions` that is sufficiently close to `pos`.
 * Returns `preferedIndex` if the associated position is close enough.
 */
s16 EnZf_FindPlatform(Vec3f* pos, s16 preferredIndex) {
    f32 rangeXZ;
    s16 i;

    rangeXZ = 210.0f;

    // Upstairs has a smaller range
    if (pos->y >= 420.0f) {
        rangeXZ = 110.0f;
    }

    if (preferredIndex != -1) {
        i = preferredIndex;
        if (((sPlatformPositions[i].y - 150.0f) <= pos->y) && (pos->y <= (sPlatformPositions[i].y + 150.0f)) &&
            ((sPlatformPositions[i].x - rangeXZ) <= pos->x) && (pos->x <= (sPlatformPositions[i].x + rangeXZ)) &&
            ((sPlatformPositions[i].z - rangeXZ) <= pos->z) && (pos->z <= (sPlatformPositions[i].z + rangeXZ))) {
            return preferredIndex;
        }
    }

    for (i = ARRAY_COUNT(sPlatformPositions) - 1; i > -1; i--) {
        if (((sPlatformPositions[i].y - 150.0f) <= pos->y) && (pos->y <= (sPlatformPositions[i].y + 150.0f)) &&
            ((sPlatformPositions[i].x - rangeXZ) <= pos->x) && (pos->x <= (sPlatformPositions[i].x + rangeXZ)) &&
            ((sPlatformPositions[i].z - rangeXZ) <= pos->z) && (pos->z <= (sPlatformPositions[i].z + rangeXZ))) {
            break;
        }
    }
    return i;
}

s16 EnZf_FindNextPlatformAwayFromPlayer(Vec3f* pos, s16 curPlatform, s16 arg2, PlayState* play) {
    f32 distToCurLoopPlatform;
    f32 platformMinDist = 585.0f;
    Player* player = GET_PLAYER(play);
    s16 initialPlatform = curPlatform;
    f32 playerMaxDist = 400.0f;
    f32 smallMaxRange = 99998.0f;
    s16 curLoopPlatform = PLATFORM_INDEX_DOWNSTAIRS_INNER_MAX; // Will never retreat to the last two
    s16 minIndex = PLATFORM_INDEX_DOWNSTAIRS_MIN;
    f32 largeMaxRange = 99999.0f;
    s16 altNextPlatform = -1;
    s16 nextPlatform = -1;
    s16 playerPlatform = EnZf_FindPlatform(&player->actor.world.pos, initialPlatform);

    // Set up search constraints
    // Upstairs
    if (pos->y > 420.0f) {
        minIndex = PLATFORM_INDEX_UPSTAIRS_MIN;
        playerMaxDist = 50.0f;

        // Upstairs outer
        if (initialPlatform >= PLATFORM_INDEX_UPSTAIRS_INNER_MAX) {
            curLoopPlatform = ARRAY_COUNT(sPlatformPositions) - 1;
            platformMinDist = 400.0f;
        } else { // upstairs inner
            curLoopPlatform = PLATFORM_INDEX_UPSTAIRS_INNER_MAX - 1;
            platformMinDist = 380.0f;
        }
    }

    for (; curLoopPlatform >= minIndex; curLoopPlatform--) {
        if ((curLoopPlatform == initialPlatform) || (curLoopPlatform == playerPlatform)) {
            continue;
        }
        if ((playerPlatform == -1) &&
            (Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[curLoopPlatform]) < playerMaxDist)) {
            continue;
        }
        distToCurLoopPlatform = Math_Vec3f_DistXYZ(pos, &sPlatformPositions[curLoopPlatform]);

        if (platformMinDist < distToCurLoopPlatform) {
            continue;
        }
        if (distToCurLoopPlatform < smallMaxRange) {
            largeMaxRange = smallMaxRange;
            altNextPlatform = nextPlatform;
            smallMaxRange = distToCurLoopPlatform;
            nextPlatform = curLoopPlatform;
        } else if (distToCurLoopPlatform < largeMaxRange) {
            largeMaxRange = distToCurLoopPlatform;
            altNextPlatform = curLoopPlatform;
        }
    }

    // These functions have no side effects, so these two calls do nothing
    Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[nextPlatform]);
    Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[altNextPlatform]);

    if (altNextPlatform > 0) {
        s16 nextPlatformToPlayerYaw =
            Math_Vec3f_Yaw(pos, &sPlatformPositions[nextPlatform]) - Math_Vec3f_Yaw(pos, &player->actor.world.pos);

        if (ABS(nextPlatformToPlayerYaw) < 0x36B0) {
            nextPlatform = altNextPlatform;
        }
    }

    if (nextPlatform < 0) {
        nextPlatform = arg2;
    }
    return nextPlatform;
}

s16 EnZf_FindNextPlatformTowardsPlayer(Vec3f* pos, s16 curPlatform, s16 arg2, PlayState* play) {
    s16 curLoopPlatform = PLATFORM_INDEX_DOWNSTAIRS_MAX;
    s16 minIndex = PLATFORM_INDEX_DOWNSTAIRS_MIN;
    Player* player = GET_PLAYER(play);
    s16 nextPlatform = EnZf_FindPlatform(&player->actor.world.pos, -1);
    f32 minRange = 500.0f;
    f32 smallMaxRange = 99998.0f;
    f32 largeMaxRange = 99999.0f;
    s16 phi_s2 = curPlatform;
    s16 phi_s3 = arg2;

    // Upstairs
    if (pos->y > 200.0f) {
        curLoopPlatform = ARRAY_COUNT(sPlatformPositions) - 1;
        minIndex = PLATFORM_INDEX_UPSTAIRS_MIN;
        minRange = 290.0f;
    }

    for (; curLoopPlatform >= minIndex; curLoopPlatform--) {
        if (minRange < Math_Vec3f_DistXYZ(pos, &sPlatformPositions[curLoopPlatform])) {
            continue;
        }
        if (curLoopPlatform != nextPlatform) {
            f32 curPlatformDistToPlayer =
                Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[curLoopPlatform]);

            if (curPlatformDistToPlayer < smallMaxRange) {
                largeMaxRange = smallMaxRange;
                phi_s3 = phi_s2;
                smallMaxRange = curPlatformDistToPlayer;
                phi_s2 = curLoopPlatform;
            } else if (curPlatformDistToPlayer < largeMaxRange) {
                largeMaxRange = curPlatformDistToPlayer;
                phi_s3 = curLoopPlatform;
            }
        } else {
            phi_s2 = nextPlatform;
            break;
        }
    }

    if (phi_s3 != nextPlatform) {
        nextPlatform = phi_s2;
    } else {
        nextPlatform = phi_s3;
    }

    return nextPlatform;
}

// Player not targeting this or another EnZf?
s32 EnZf_CanAttack(PlayState* play, EnZf* this) {
    Actor* targetedActor;
    Player* player = GET_PLAYER(play);

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        if (player->stateFlags1 & 0x6000) {                    // Hanging or climbing
            return false;
        } else {
            return true;
        }
    } else {
        if (!Actor_OtherIsTargeted(play, &this->actor)) {
            return true;
        }
        if (this->actor.params == ENZF_TYPE_DINOLFOS) {
            targetedActor = player->unk_664;
            if (targetedActor == NULL) {
                return false;
            } else {
                if (targetedActor->category != ACTORCAT_ENEMY) {
                    return true;
                }
                if (targetedActor->id != ACTOR_EN_ZF) {
                    return false;
                } else if (targetedActor->colorFilterTimer != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

void func_80B44DC4(EnZf* this, PlayState* play) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (angleDiff < 0) {
        angleDiff = -angleDiff;
    }

    if (angleDiff >= 0x1B58) {
        func_80B483E4(this, play);
    } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 8) != 0) &&
               EnZf_CanAttack(play, this)) {
        EnZf_SetupSlash(this);
    } else {
        func_80B45384(this);
    }
}

s32 EnZf_ChooseAction(PlayState* play, EnZf* this) {
    s16 angleToWall;
    Actor* explosive;

    angleToWall = this->actor.wallYaw - this->actor.shape.rot.y;
    angleToWall = ABS(angleToWall);

    if (func_800354B4(play, &this->actor, 100.0f, 0x5DC0, 0x2AA8, this->actor.shape.rot.y)) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

        if ((this->actor.bgCheckFlags & 8) && (ABS(angleToWall) < 0x2EE0) && (this->actor.xzDistToPlayer < 80.0f)) {
            EnZf_SetupJumpUp(this);
            return true;
        } else if ((this->actor.xzDistToPlayer < 90.0f) && ((play->gameplayFrames % 2) != 0)) {
            EnZf_SetupJumpUp(this);
            return true;
        } else {
            EnZf_SetupJumpBack(this);
            return true;
        }
    }

    explosive = Actor_FindNearby(play, &this->actor, -1, ACTORCAT_EXPLOSIVE, 80.0f);

    if (explosive != NULL) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if (((this->actor.bgCheckFlags & 8) && (angleToWall < 0x2EE0)) || (explosive->id == ACTOR_EN_BOM_CHU)) {
            if ((explosive->id == ACTOR_EN_BOM_CHU) && (Actor_WorldDistXYZToActor(&this->actor, explosive) < 80.0f) &&
                ((s16)((this->actor.shape.rot.y - explosive->world.rot.y) + 0x8000) < 0x3E80)) {
                EnZf_SetupJumpUp(this);
                return true;
            } else {
                EnZf_SetupCircleAroundPlayer(this, 4.0f);
                return true;
            }
        } else {
            EnZf_SetupJumpBack(this);
            return true;
        }
    }
    return false;
}

// Setup functions and action functions

/**
 * Set position 300 units above ground and invisible, fade in and drop to ground, fully solid when on ground
 */
void EnZf_SetupDropIn(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfJumpingAnim, 0.0f, 9.0f, Animation_GetLastFrame(&gZfJumpingAnim),
                     ANIMMODE_LOOP, 0.0f);

    this->actor.world.pos.y = this->actor.floorHeight + 300.0f;
    this->alpha = this->actor.shape.shadowAlpha = 0;
    this->unk_3F0 = 10;
    this->hopAnimIndex = 1;
    this->action = ENZF_ACTION_DROP_IN;
    this->actor.bgCheckFlags &= ~2;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    EnZf_SetupAction(this, EnZf_DropIn);
}

void EnZf_DropIn(EnZf* this, PlayState* play) {
    if (this->unk_3F0 == 1) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
        this->actor.flags |= ACTOR_FLAG_0;

        if (this->actor.params == ENZF_TYPE_LIZALFOS_MINIBOSS_A) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
    }

    if (this->unk_3F0 != 0) {
        if (this->actor.params != ENZF_TYPE_LIZALFOS_LONE) {
            this->unk_3F0--;
        } else if (this->actor.xzDistToPlayer <= 160.0f) {
            this->unk_3F0 = 0;
            this->actor.flags |= ACTOR_FLAG_0;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
        }

        this->actor.world.pos.y = this->actor.floorHeight + 300.0f;
    } else if (this->alpha < 255) {
        this->alpha += 255 / 5;
    }

    if ((this->actor.bgCheckFlags & 3) && (this->hopAnimIndex != 0)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_ONGND);
        Animation_Change(&this->skelAnime, &gZfLandingAnim, 1.0f, 0.0f, 17.0f, ANIMMODE_ONCE, 0.0f);
        this->hopAnimIndex = 0;
        this->actor.bgCheckFlags &= ~2;
        this->actor.world.pos.y = this->actor.floorHeight;
        this->actor.velocity.y = 0.0f;
        Actor_SpawnFloorDustRing(play, &this->actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        Actor_SpawnFloorDustRing(play, &this->actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        this->alpha = 255;
        if (this->actor.params > ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // Only miniboss B
            EnZf_SetupSheatheSword(this, play);
        } else {
            func_80B45384(this);
        }
    }
    this->actor.shape.shadowAlpha = this->alpha;
}

// stop? and choose an action
void func_80B45384(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfCryingAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gZfCryingAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    this->action = ENZF_ACTION_3;
    this->unk_3F0 = Rand_ZeroOne() * 10.0f + 5.0f;
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnZf_SetupAction(this, func_80B4543C);
}

void func_80B4543C(EnZf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 angleToPlayer = (this->actor.yawTowardsPlayer - this->headRot) - this->actor.shape.rot.y;

    angleToPlayer = ABS(angleToPlayer);
    SkelAnime_Update(&this->skelAnime);

    if (!EnZf_DodgeRangedEngaging(play, this)) {
        if (this->actor.params == ENZF_TYPE_DINOLFOS) {
            if (this->unk_3F4 != 0) {
                this->unk_3F4--;
                if (angleToPlayer >= 0x1FFE) {
                    return;
                }
                this->unk_3F4 = 0;

            } else if (EnZf_ChooseAction(play, this)) {
                return;
            }
        }
        angleToPlayer = player->actor.shape.rot.y - this->actor.shape.rot.y;
        angleToPlayer = ABS(angleToPlayer);

        if ((this->actor.xzDistToPlayer < 100.0f) && (player->swordState != 0) && (angleToPlayer >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            func_80B483E4(this, play);
        } else if (this->unk_3F0 != 0) {
            this->unk_3F0--;
        } else {
            if (Actor_IsFacingPlayer(&this->actor, 30 * 0x10000 / 360)) {
                if ((this->actor.xzDistToPlayer < 200.0f) && (this->actor.xzDistToPlayer > 100.0f) &&
                    (Rand_ZeroOne() < 0.3f)) {
                    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
                        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                        EnZf_SetupJumpForward(this);
                    } else {
                        func_80B483E4(this, play);
                    }
                } else if (Rand_ZeroOne() > 0.3f) {
                    EnZf_SetupApproachPlayer(this, play);
                } else {
                    func_80B483E4(this, play);
                }
            } else {
                func_80B4604C(this);
            }

            if ((play->gameplayFrames & 0x5F) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
            }
        }
    }
}

void EnZf_SetupApproachPlayer(EnZf* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gZfWalkingAnim, -4.0f);
    this->action = ENZF_ACTION_APPROACH_PLAYER;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, this->curPlatform);
        this->nextPlatform = EnZf_FindNextPlatformTowardsPlayer(&this->actor.world.pos, this->curPlatform,
                                                                this->homePlatform, play);
        this->hopAnimIndex = 0;
    }
    this->actor.speedXZ = 0.0f;
    EnZf_SetupAction(this, EnZf_ApproachPlayer);
}

void EnZf_ApproachPlayer(EnZf* this, PlayState* play) {
    s32 sp54;
    s32 sp50;
    s32 temp;
    s16 temp_v1;
    s16 sp48 = -1;
    f32 sp44 = 350.0f;
    f32 sp40 = 0.0f;
    Player* player = GET_PLAYER(play);
    s32 sp30;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        sp48 = EnZf_FindPlatform(&player->actor.world.pos, sp48);
        this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, sp48);

        if (this->actor.world.pos.y >= 420.0f) {
            sp44 = 270.0f;
        }
    }

    if (!EnZf_DodgeRangedEngaging(play, this)) {
        if (sp48 != this->curPlatform) {
            this->nextPlatform = EnZf_FindNextPlatformTowardsPlayer(&this->actor.world.pos, this->curPlatform,
                                                                    this->homePlatform, play);

            if ((sp48 < 0) && (this->nextPlatform == this->curPlatform)) {
                sp48 = this->curPlatform;
                this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            } else {
                this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer =
                    Actor_WorldYawTowardPoint(&this->actor, &sPlatformPositions[this->nextPlatform]);

                temp_v1 = this->actor.wallYaw - this->actor.shape.rot.y;
                temp_v1 = ABS(temp_v1);

                if ((this->unk_3F8 && (this->actor.speedXZ > 0.0f)) ||
                    ((this->actor.bgCheckFlags & 8) && (temp_v1 >= 0x5C19))) {
                    if ((Actor_WorldDistXZToPoint(&this->actor, &sPlatformPositions[this->nextPlatform]) < sp44) &&
                        !EnZf_PrimaryFloorCheck(this, play, 191.9956f)) {
                        EnZf_SetupJumpForward(this);

                        if (this->actor.bgCheckFlags & 8) {
                            this->actor.velocity.y = 20.0f;
                        }

                        return;
                    } else {
                        this->actor.world.rot.y =
                            Actor_WorldYawTowardPoint(&this->actor, &sPlatformPositions[this->curPlatform]);
                    }
                } else {
                    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    Math_SmoothStepToF(&this->actor.speedXZ, 8.0f, 1.0f, 1.5f, 0.0f);
                }
            }
        }

        if (Actor_OtherIsTargeted(play, &this->actor)) {
            sp40 = 100.0f;
        }

        if (this->actor.xzDistToPlayer <= (70.0f + sp40)) {
            Math_SmoothStepToF(&this->actor.speedXZ, -8.0f, 1.0f, 0.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&this->actor.speedXZ, 8.0f, 1.0f, 0.5f, 0.0f);
        }

        this->skelAnime.playSpeed = this->actor.speedXZ * 1.2f;

        temp_v1 = player->actor.shape.rot.y - this->actor.shape.rot.y;
        temp_v1 = ABS(temp_v1);

        if ((sp48 == this->curPlatform) && (this->actor.xzDistToPlayer < 150.0f) && (player->swordState != 0) &&
            (temp_v1 >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

            if (Rand_ZeroOne() > 0.7f) {
                func_80B483E4(this, play);
                return;
            }
        }

        sp54 = this->skelAnime.curFrame;
        SkelAnime_Update(&this->skelAnime);
        sp50 = this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed);
        sp30 = (f32)ABS(this->skelAnime.playSpeed);

        if (sp48 == this->curPlatform) {
            if (!Actor_IsFacingPlayer(&this->actor, 0x11C7)) {
                if (Rand_ZeroOne() > 0.5f) {
                    func_80B462E4(this, play);
                } else {
                    func_80B45384(this);
                }
            } else if (this->actor.xzDistToPlayer < 100.0f) {
                if ((Rand_ZeroOne() > 0.05f) && EnZf_CanAttack(play, this)) {
                    EnZf_SetupSlash(this);
                } else if (Rand_ZeroOne() > 0.5f) {
                    func_80B483E4(this, play);
                } else {
                    func_80B45384(this);
                }
            } else {
                if (this->unk_3F8) {
                    func_80B462E4(this, play);
                } else if (Rand_ZeroOne() < 0.1f) {
                    func_80B45384(this);
                }
            }
        }

        if (this->actor.params == ENZF_TYPE_DINOLFOS) {
            if (EnZf_ChooseAction(play, this)) {
                return;
            }

            if ((this->actor.xzDistToPlayer < 180.0f) && (this->actor.xzDistToPlayer > 160.0f) &&
                Actor_IsFacingPlayer(&this->actor, 0x71C)) {
                if (Actor_IsTargeted(play, &this->actor)) {
                    if (Rand_ZeroOne() < 0.1f) {
                        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                        EnZf_SetupJumpForward(this);
                        return;
                    }
                } else {
                    func_80B483E4(this, play);
                    return;
                }
            }
        }

        if ((play->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
        }

        if (sp54 != (s32)this->skelAnime.curFrame) {
            temp = sp30 + sp54;

            if (((sp50 < 2) && (temp >= 4)) || ((sp50 < 32) && (temp >= 34))) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_WALK);
            }
        }
    }
}

void EnZf_SetupJumpForward(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfJumpingAnim, 1.0f, 0.0f, 3.0f, ANIMMODE_ONCE, -3.0f);
    this->unk_3F0 = 0;
    this->hopAnimIndex = 1;
    this->actor.velocity.y = 15.0f;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        this->actor.speedXZ = 16.0f;
    } else {
        this->actor.speedXZ = 10.0f;
    }

    this->action = ENZF_ACTION_JUMP_FORWARD;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_JUMP);
    EnZf_SetupAction(this, EnZf_JumpForward);
}

void EnZf_JumpForward(EnZf* this, PlayState* play) {
    if ((this->unk_3F0 != 0) && (this->actor.world.pos.y <= this->actor.floorHeight)) {
        this->actor.world.pos.y = this->actor.floorHeight;
        this->hopAnimIndex = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_3F0 == 0) {
            Animation_Change(&this->skelAnime, &gZfLandingAnim, 3.0f, 0.0f, 17.0f, ANIMMODE_ONCE, -3.0f);
            this->unk_3F0 = 10;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_JUMP);
        } else {
            this->actor.speedXZ = 0.0f;
            this->hopAnimIndex = 0;
            EnZf_SetupApproachPlayer(this, play);
        }
    }
    if ((play->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
    }

    if ((this->actor.params == ENZF_TYPE_DINOLFOS) && (this->actor.bgCheckFlags & 3)) {
        if (EnZf_CanAttack(play, this)) {
            EnZf_SetupSlash(this);
        } else {
            func_80B483E4(this, play);
        }
    }
}

void func_80B4604C(EnZf* this) {
    Animation_MorphToLoop(&this->skelAnime, &gZfWalkingAnim, -4.0f);
    this->action = ENZF_ACTION_6;
    EnZf_SetupAction(this, func_80B46098);
}

void func_80B46098(EnZf* this, PlayState* play) {
    s32 pad;
    f32 phi_f2;
    Player* player = GET_PLAYER(play);
    s16 temp_v0;
    s16 phi_v1;

    if (!EnZf_DodgeRangedEngaging(play, this)) {
        if ((this->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(play, this)) {
            temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if (temp_v0 > 0) {
                phi_v1 = temp_v0 * 0.25f + 2000.0f;
            } else {
                phi_v1 = temp_v0 * 0.25f - 2000.0f;
            }

            this->actor.shape.rot.y += phi_v1;
            this->actor.world.rot.y = this->actor.shape.rot.y;

            if (temp_v0 > 0) {
                phi_f2 = phi_v1 * 1.5f;
                if (phi_f2 > 2.0f) {
                    phi_f2 = 2.0f;
                }
            } else {
                phi_f2 = phi_v1 * 1.5f;
                if (phi_f2 < -2.0f) {
                    phi_f2 = -2.0f;
                }
            }

            this->skelAnime.playSpeed = -phi_f2;
            SkelAnime_Update(&this->skelAnime);

            if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
                this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, this->curPlatform);
                if (this->curPlatform != EnZf_FindPlatform(&player->actor.world.pos, -1)) {
                    EnZf_SetupApproachPlayer(this, play);
                    return;
                }
            }

            if (Actor_IsFacingPlayer(&this->actor, 30 * 0x10000 / 360)) {
                if (Rand_ZeroOne() > 0.8f) {
                    func_80B462E4(this, play);
                } else {
                    EnZf_SetupApproachPlayer(this, play);
                }
            }
            if ((play->gameplayFrames & 0x5F) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
            }
        }
    }
}

// Conditional setup function
void func_80B462E4(EnZf* this, PlayState* play) {
    if ((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ ||
        Actor_TestFloorInDirection(&this->actor, play, 40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF)) ||
        Actor_TestFloorInDirection(&this->actor, play, -40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF))) {
        Animation_PlayLoop(&this->skelAnime, &gZfSidesteppingAnim);
        this->actor.speedXZ = Rand_CenteredFloat(12.0f);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->unk_3F0 = Rand_ZeroOne() * 10.0f + 20.0f;
        this->hopAnimIndex = 0;
        this->action = ENZF_ACTION_7;
        this->unk_408 = 0.0f;
        EnZf_SetupAction(this, func_80B463E4);
    } else {
        EnZf_SetupApproachPlayer(this, play);
    }
}

void func_80B463E4(EnZf* this, PlayState* play) {
    s16 angleBehindPlayer;
    s16 phi_v0_3;
    s32 pad;
    s32 curKeyFrame;
    s32 prevKeyFrame;
    s32 playSpeed;
    Player* player = GET_PLAYER(play);
    f32 baseRange = 0.0f;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4000, 1);

    if (!EnZf_DodgeRangedEngaging(play, this) &&
        ((this->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(play, this))) {
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3A98;
        angleBehindPlayer = player->actor.shape.rot.y + 0x8000;

        if (Math_SinS(angleBehindPlayer - this->actor.shape.rot.y) >= 0.0f) {
            this->actor.speedXZ -= 0.25f;
            if (this->actor.speedXZ < -8.0f) {
                this->actor.speedXZ = -8.0f;
            }
        } else if (Math_SinS(angleBehindPlayer - this->actor.shape.rot.y) < 0.0f) { // Superfluous check
            this->actor.speedXZ += 0.25f;
            if (this->actor.speedXZ > 8.0f) {
                this->actor.speedXZ = 8.0f;
            }
        }

        if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
            if (this->unk_3F8) {
                this->actor.speedXZ = -this->actor.speedXZ;
            }
        } else if ((this->actor.bgCheckFlags & 8) ||
                   !Actor_TestFloorInDirection(&this->actor, play, this->actor.speedXZ,
                                               this->actor.shape.rot.y + 0x3FFF)) {
            if (this->actor.bgCheckFlags & 8) {
                if (this->actor.speedXZ >= 0.0f) {
                    phi_v0_3 = this->actor.shape.rot.y + 0x3FFF;
                } else {
                    phi_v0_3 = this->actor.shape.rot.y - 0x3FFF;
                }
                phi_v0_3 = this->actor.wallYaw - phi_v0_3;
            } else {
                this->actor.speedXZ *= -0.8f;
                phi_v0_3 = 0;
            }

            if (ABS(phi_v0_3) > 0x4000) {
                this->actor.speedXZ *= -0.8f;
                if (this->actor.speedXZ < 0.0f) {
                    this->actor.speedXZ -= 0.5f;
                } else {
                    this->actor.speedXZ += 0.5f;
                }
            }
        }

        if (Actor_OtherIsTargeted(play, &this->actor)) {
            baseRange = 100.0f;
        }

        if (this->actor.xzDistToPlayer <= (70.0f + baseRange)) {
            Math_SmoothStepToF(&this->unk_408, -4.0f, 1.0f, 1.5f, 0.0f);
        } else if ((90.0f + baseRange) < this->actor.xzDistToPlayer) {
            Math_SmoothStepToF(&this->unk_408, 4.0f, 1.0f, 1.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&this->unk_408, 0.0f, 1.0f, 5.65f, 0.0f);
        }

        if ((this->unk_408 != 0.0f) && !EnZf_SecondaryFloorCheck(this, play, this->unk_408)) {
            this->actor.world.pos.x += Math_SinS(this->actor.shape.rot.y) * this->unk_408;
            this->actor.world.pos.z += Math_CosS(this->actor.shape.rot.y) * this->unk_408;
        }

        if (ABS(this->actor.speedXZ) >= ABS(this->unk_408)) {
            this->skelAnime.playSpeed = this->actor.speedXZ * 0.75f;
        } else if (this->skelAnime.playSpeed < 0.0f) {
            this->skelAnime.playSpeed = this->unk_408 * -0.75f;
        } else {
            this->skelAnime.playSpeed = this->unk_408 * 0.75f;
        }

        curKeyFrame = this->skelAnime.curFrame;
        SkelAnime_Update(&this->skelAnime);
        prevKeyFrame = this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed);
        playSpeed = (f32)ABS(this->skelAnime.playSpeed);

        if (curKeyFrame != (s32)this->skelAnime.curFrame) {
            s32 nextKeyFrame = playSpeed + curKeyFrame;

            if (((prevKeyFrame < 14) && (nextKeyFrame > 15)) || ((prevKeyFrame < 27) && (nextKeyFrame > 28))) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_WALK);
            }
        }

        if ((play->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
        }

        if ((Math_CosS(angleBehindPlayer - this->actor.shape.rot.y) < -0.85f) || (this->unk_3F0 == 0)) {
            this->actor.world.rot.y = this->actor.shape.rot.y;

            if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0) &&
                EnZf_CanAttack(play, this)) {
                EnZf_SetupSlash(this);
            } else {
                func_80B45384(this);
            }
        } else if (this->unk_3F0 != 0) {
            this->unk_3F0--;
        }
    }
}

void EnZf_SetupSlash(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfSlashAnim, 1.25f, 0.0f, Animation_GetLastFrame(&gZfSlashAnim), ANIMMODE_ONCE,
                     -4.0f);

    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
        this->skelAnime.playSpeed = 1.75f;
    }

    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->action = ENZF_ACTION_SLASH;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
    this->actor.speedXZ = 0.0f;
    EnZf_SetupAction(this, EnZf_Slash);
}

void EnZf_Slash(EnZf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 rotDiff;
    s16 yawDiff;

    this->actor.speedXZ = 0.0f;

    if ((s32)this->skelAnime.curFrame == 10) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_ATTACK);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        EffectBlure_AddSpace(Effect_GetByIndex(this->blureIndex));

        if ((this->actor.params == ENZF_TYPE_DINOLFOS) && !Actor_IsFacingPlayer(&this->actor, 5460)) {
            func_80B45384(this);
            this->unk_3F0 = Rand_ZeroOne() * 5.0f + 5.0f;
            this->unk_3F4 = Rand_ZeroOne() * 20.0f + 100.0f;
        } else if ((Rand_ZeroOne() > 0.7f) || (this->actor.xzDistToPlayer >= 120.0f)) {
            func_80B45384(this);
            this->unk_3F0 = Rand_ZeroOne() * 5.0f + 5.0f;
        } else {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;

            if (Rand_ZeroOne() > 0.7f) {
                func_80B483E4(this, play);
            } else {
                rotDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;
                rotDiff = ABS(rotDiff);

                if (rotDiff <= 10000) {
                    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    yawDiff = ABS(yawDiff);

                    if (yawDiff > 16000) {
                        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                        func_80B483E4(this, play);
                    } else if (player->stateFlags1 & 0x6010) {
                        if (this->actor.isTargeted) {
                            EnZf_SetupSlash(this);
                        } else {
                            func_80B483E4(this, play);
                        }
                    } else {
                        EnZf_SetupSlash(this);
                    }
                } else {
                    func_80B483E4(this, play);
                }
            }
        }
    }
}

void EnZf_SetupRecoilFromBlockedSlash(EnZf* this) {
    f32 frame = this->skelAnime.curFrame - 3.0f;

    Animation_Change(&this->skelAnime, &gZfSlashAnim, -1.0f, frame, 0.0f, ANIMMODE_ONCE, 0.0f);
    this->action = ENZF_ACTION_RECOIL_FROM_BLOCKED_SLASH;
    EnZf_SetupAction(this, EnZf_RecoilFromBlockedSlash);
}

void EnZf_RecoilFromBlockedSlash(EnZf* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (Rand_ZeroOne() > 0.7f) {
            func_80B45384(this);
        } else if ((Rand_ZeroOne() > 0.2f) && EnZf_CanAttack(play, this)) {
            EnZf_SetupSlash(this);
        } else {
            func_80B483E4(this, play);
        }
    }
}

void EnZf_SetupJumpBack(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfJumpingAnim, -1.0f, 3.0f, 0.0f, ANIMMODE_ONCE, -3.0f);
    this->unk_3F0 = 0;
    this->hopAnimIndex = 1;
    this->action = ENZF_ACTION_JUMP_BACK;
    this->actor.velocity.y = 15.0f;
    this->actor.speedXZ = -15.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_JUMP);
    EnZf_SetupAction(this, EnZf_JumpBack);
}

void EnZf_JumpBack(EnZf* this, PlayState* play) {
    if ((this->unk_3F0 != 0) && (this->actor.world.pos.y <= this->actor.floorHeight)) {
        this->actor.world.pos.y = this->actor.floorHeight;
        this->hopAnimIndex = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_3F0 == 0) {
            Animation_Change(&this->skelAnime, &gZfLandingAnim, 3.0f, 0.0f, 17.0f, ANIMMODE_ONCE, -3.0f);
            this->unk_3F0 = 10;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_JUMP);
        } else if ((play->gameplayFrames % 2) != 0) {
            func_80B483E4(this, play);
        } else {
            func_80B45384(this);
        }
    }

    if ((play->state.frames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
    }
}

void EnZf_SetupStunned(EnZf* this) {
    if ((this->actor.bgCheckFlags & 1) && ((this->actor.velocity.y == 0.0f) || (this->actor.velocity.y == -4.0f))) {
        this->actor.speedXZ = 0.0f;
        this->hopAnimIndex = 0;
    } else {
        this->hopAnimIndex = 1;
    }

    if (this->damageEffect == ENZF_DMGEFF_ICE) {
        this->iceTimer = 36;
    } else {
        Animation_PlayOnceSetSpeed(&this->skelAnime, &gZfKnockedBackAnim, 0.0f);
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->action = ENZF_ACTION_STUNNED;
    EnZf_SetupAction(this, EnZf_Stunned);
}

void EnZf_Stunned(EnZf* this, PlayState* play) {
    s16 angleToWall;

    if (this->actor.bgCheckFlags & 2) {
        this->actor.speedXZ = 0.0f;
    }

    if (this->actor.bgCheckFlags & 1) {
        if (this->actor.speedXZ < 0.0f) {
            this->actor.speedXZ += 0.05f;
        }
        this->hopAnimIndex = 0;
    }

    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & 1)) {
        if (this->actor.colChkInfo.health == 0) {
            EnZf_SetupDie(this);
        } else if ((this->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(play, this)) {
            if (D_80B4A1B4 != -1) {
                func_80B44DC4(this, play);
            } else {
                angleToWall = this->actor.wallYaw - this->actor.shape.rot.y;
                angleToWall = ABS(angleToWall);

                if ((this->actor.params == ENZF_TYPE_DINOLFOS) && (this->actor.bgCheckFlags & 8) &&
                    (ABS(angleToWall) < 0x2EE0) && (this->actor.xzDistToPlayer < 90.0f)) {
                    this->actor.world.rot.y = this->actor.shape.rot.y;
                    EnZf_SetupJumpUp(this);
                } else if (!EnZf_DodgeRangedEngaging(play, this)) {
                    if (this->actor.params != ENZF_TYPE_DINOLFOS) {
                        func_80B44DC4(this, play);
                    } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) != 0) &&
                               EnZf_CanAttack(play, this)) {
                        EnZf_SetupSlash(this);
                    } else {
                        func_80B44DC4(this, play);
                    }
                }
            }
        }
    }
}

void EnZf_SetupSheatheSword(EnZf* this, PlayState* play) {
    f32 morphFrames = 0.0f;
    f32 lastFrame = Animation_GetLastFrame(&gZfSheathingSwordAnim);

    if (this->action <= ENZF_ACTION_DAMAGED) {
        morphFrames = -4.0f;
    }

    Animation_Change(&this->skelAnime, &gZfSheathingSwordAnim, 2.0f, 0.0f, lastFrame, ANIMMODE_ONCE, morphFrames);
    this->action = ENZF_ACTION_SHEATHE_SWORD;
    this->actor.speedXZ = 0.0f;
    this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, this->curPlatform);
    this->nextPlatform =
        EnZf_FindNextPlatformAwayFromPlayer(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnZf_SetupAction(this, EnZf_SheatheSword);
}

void EnZf_SheatheSword(EnZf* this, PlayState* play) {
    s16 yaw = Actor_WorldYawTowardPoint(&this->actor, &sPlatformPositions[this->nextPlatform]) + 0x8000;

    Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 1, 1000, 0);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.world.rot.y = yaw - 0x8000;
        EnZf_SetupHopAway(this, play);
        this->swordSheathed = true;
    }
}

void EnZf_SetupHopAndTaunt(EnZf* this) {
    this->hopAnimIndex = 0;
    Animation_MorphToPlayOnce(&this->skelAnime, sHoppingAnims[0], -4.0f);
    this->action = ENZF_ACTION_HOP_AND_TAUNT;
    this->actor.speedXZ = 0.0f;
    this->unk_40C = 0.0f;
    this->unk_408 = 0.0f;
    EnZf_SetupAction(this, EnZf_HopAndTaunt);
}

void EnZf_HopAndTaunt(EnZf* this, PlayState* play) {
    f32 lastFrame;
    f32 maxDist = 400.0f;

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1, 4000, 0);

    // Upstairs
    if (this->actor.world.pos.y >= 420.0f) {
        maxDist = 250.0f;
    }

    // If player gets too close, run away
    if ((this->actor.xzDistToPlayer < maxDist) && (this->hopAnimIndex != 1)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
        EnZf_SetupSheatheSword(this, play);
    } else {
        if (this->hopAnimIndex != 1) {
            EnZf_DodgeRangedWaiting(play, this);
        }

        if (SkelAnime_Update(&this->skelAnime)) {
            this->hopAnimIndex++; // move on to next animation

            // Loop back to beginning
            if (this->hopAnimIndex >= ARRAY_COUNT(sHoppingAnims)) {
                this->hopAnimIndex = 0;
            }

            if ((this->unk_408 != 0.0f) || (this->unk_40C != 0.0f)) {
                this->hopAnimIndex = 1;
            }

            lastFrame = Animation_GetLastFrame(sHoppingAnims[this->hopAnimIndex]);

            switch (this->hopAnimIndex) {
                case 0:
                    this->actor.velocity.y = 0.0f;
                    this->actor.world.pos.y = this->actor.floorHeight;
                    break;

                case 1:
                    this->actor.velocity.y = this->unk_40C + 10.0f;
                    this->actor.speedXZ = this->unk_408;
                    this->unk_408 = 0.0f;
                    this->unk_40C = 0.0f;
                    break;

                case 2:
                    this->actor.world.pos.y = this->actor.floorHeight;
                    lastFrame = 3.0f;
                    break;

                default:
                    break;
            }

            Animation_Change(&this->skelAnime, sHoppingAnims[this->hopAnimIndex], 1.5f, 0.0f, lastFrame, ANIMMODE_ONCE,
                             0.0f);
        }

        if ((play->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
        }
    }
}

void EnZf_SetupHopAway(EnZf* this, PlayState* play) {
    this->hopAnimIndex = 0;
    Animation_PlayOnce(&this->skelAnime, sHoppingAnims[0]);
    this->action = ENZF_ACTION_HOP_AWAY;
    this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, this->curPlatform);
    this->nextPlatform =
        EnZf_FindNextPlatformAwayFromPlayer(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);
    EnZf_SetupAction(this, EnZf_HopAway);
}

void EnZf_HopAway(EnZf* this, PlayState* play) {
    f32 sp74;
    f32 sp70 = 1.0f;
    f32 phi_f20 = 550.0f;
    s32 pad;
    f32 phi_f20_2;
    f32 phi_f0;
    s32 pad2;
    s16 sp5A;
    s32 sp54;
    s32 temp_v1_2;
    s32 phi_v1;

    sp74 = Actor_WorldDistXZToPoint(&this->actor, &sPlatformPositions[this->nextPlatform]);
    sp54 = this->hopAnimIndex;

    if ((play->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
    }

    // Upstairs
    if (this->actor.world.pos.y >= 420.0f) {
        phi_f20 = 280.0f;
    }

    sp5A = Actor_WorldYawTowardPoint(&this->actor, &sPlatformPositions[this->nextPlatform]);

    switch (this->hopAnimIndex) {
        case 0:
            this->actor.world.rot.y = sp5A;
            this->actor.shape.rot.y = sp5A + 0x8000;
            D_80B4AB30 = 0;
            this->homePlatform = this->curPlatform;
            temp_v1_2 = !EnZf_PrimaryFloorCheck(this, play, 107.0f);
            temp_v1_2 |= !EnZf_PrimaryFloorCheck(this, play, 220.0f) << 1;
            this->hopAnimIndex++;

            switch (temp_v1_2) {
                case 1:
                case 1 | 2:
                    this->actor.velocity.y = 12.0f;
                    if (this->actor.bgCheckFlags & 8) {
                        this->actor.velocity.y += 8.0f;
                    }

                    this->actor.speedXZ = 8.0f;
                    break;

                case 2:
                    this->actor.velocity.y = 15.0f;
                    this->actor.speedXZ = 20.0f;
                    break;

                default: // 0
                    phi_f20_2 = 107.0f;
                    phi_f20_2 += 10.0f;
                    phi_f0 = 8.0f;
                    phi_f0 += 1.2f;

                    for (phi_v1 = 20; phi_v1 >= 0; phi_v1--, phi_f20_2 += 10.0f, phi_f0 += 1.2f) {

                        if (!EnZf_PrimaryFloorCheck(this, play, phi_f20_2)) {
                            this->actor.speedXZ = phi_f0;
                            this->actor.velocity.y = 12.0f;
                            break;
                        }
                    }
                    if (this->actor.speedXZ == 0.0f) {
                        EnZf_SetupHopAndTaunt(this);
                    }
            }
            break;

        case 1:
            if ((this->actor.bgCheckFlags & 2) || (this->actor.bgCheckFlags & 1)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_ONGND);
                this->actor.velocity.y = 0.0f;
                this->actor.world.pos.y = this->actor.floorHeight;
                this->actor.speedXZ = 0.0f;
                Actor_SpawnFloorDustRing(play, &this->actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
                Actor_SpawnFloorDustRing(play, &this->actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);

                if (phi_f20 <= this->actor.xzDistToPlayer) {
                    EnZf_SetupHopAndTaunt(this);
                } else if (sp74 < 80.0f) {
                    this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, this->curPlatform);
                    this->nextPlatform = EnZf_FindNextPlatformAwayFromPlayer(&this->actor.world.pos, this->curPlatform,
                                                                             this->homePlatform, play);
                }

                this->hopAnimIndex = 0;
                sp70 = 2.0f;
            } else {
                Math_SmoothStepToS(&this->actor.world.rot.y, sp5A, 1, 0xFA0, 0);
                this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
                D_80B4AB30++;
            }
            break;

        case 2:
            if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                this->hopAnimIndex = 0;
            }
            break;
    }

    if (sp54 != this->hopAnimIndex) {
        Animation_PlayOnceSetSpeed(&this->skelAnime, sHoppingAnims[this->hopAnimIndex], sp70);
    }

    SkelAnime_Update(&this->skelAnime);
}

void EnZf_SetupDrawSword(EnZf* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gZfDrawingSwordAnim);
    this->actor.world.rot.y += 0x8000;
    this->action = ENZF_ACTION_DRAW_SWORD;
    this->actor.speedXZ = 0.0f;
    this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, this->curPlatform);
    this->nextPlatform =
        EnZf_FindNextPlatformAwayFromPlayer(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);
    EnZf_SetupAction(this, EnZf_DrawSword);
}

void EnZf_DrawSword(EnZf* this, PlayState* play) {
    s16 yawTowardsPlayer = this->actor.yawTowardsPlayer;

    if (this->skelAnime.curFrame >= 26.0f) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, yawTowardsPlayer, 1, 6000, 0);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
        this->actor.world.rot.y = yawTowardsPlayer;
        this->hopAnimIndex = -1;
        func_80B45384(this);
    }

    if (this->skelAnime.curFrame == 22.0f) {
        this->swordSheathed = false;
    }
}

void EnZf_SetupDamaged(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfKnockedBackAnim, 1.5f, 0.0f, Animation_GetLastFrame(&gZfKnockedBackAnim),
                     ANIMMODE_ONCE, -4.0f);

    if ((this->actor.bgCheckFlags & 1) && ((this->actor.velocity.y == 0.0f) || (this->actor.velocity.y == -4.0f))) {
        this->actor.speedXZ = -4.0f;
        this->hopAnimIndex = 0;
    } else {
        this->hopAnimIndex = 1;
    }

    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
        this->skelAnime.playSpeed = 4.5f;
    }

    if (this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // not miniboss
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DAMAGE);
    this->action = ENZF_ACTION_DAMAGED;
    EnZf_SetupAction(this, EnZf_Damaged);
}

void EnZf_Damaged(EnZf* this, PlayState* play) {
    s16 wallYawDiff;

    if (this->actor.bgCheckFlags & 2) {
        this->actor.speedXZ = 0.0f;
    }

    if (this->actor.bgCheckFlags & 1) {
        if (this->actor.speedXZ < 0.0f) {
            this->actor.speedXZ += 0.05f;
        }
        this->hopAnimIndex = 0;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4500, 0);

    if (((this->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(play, this)) &&
        SkelAnime_Update(&this->skelAnime) && (this->actor.bgCheckFlags & 1)) {

        if (D_80B4A1B4 != -1) {
            if (this->damageEffect == ENZF_DMGEFF_PROJECTILE) {
                D_80B4A1B0++;
            } else {
                this->actor.world.rot.y = this->actor.shape.rot.y;

                if (!EnZf_PrimaryFloorCheck(this, play, 135.0f) && (this->actor.xzDistToPlayer < 90.0f)) {
                    EnZf_SetupJumpUp(this);
                } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0)) {
                    EnZf_SetupSlash(this);
                } else {
                    func_80B44DC4(this, play);
                }
            }
        } else {

            wallYawDiff = this->actor.wallYaw - this->actor.shape.rot.y;
            wallYawDiff = ABS(wallYawDiff);

            if ((this->actor.params == ENZF_TYPE_DINOLFOS) && (this->actor.bgCheckFlags & 8) &&
                (ABS(wallYawDiff) < 12000) && (this->actor.xzDistToPlayer < 90.0f)) {
                EnZf_SetupJumpUp(this);
            } else if (!EnZf_DodgeRangedEngaging(play, this)) {
                if (this->actor.params != ENZF_TYPE_DINOLFOS) {
                    this->actor.world.rot.y = this->actor.shape.rot.y;

                    if (!EnZf_PrimaryFloorCheck(this, play, 135.0f) && (this->actor.xzDistToPlayer < 90.0f)) {
                        EnZf_SetupJumpUp(this);
                    } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0)) {
                        EnZf_SetupSlash(this);
                    } else {
                        func_80B44DC4(this, play);
                    }
                } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0) &&
                           EnZf_CanAttack(play, this)) {
                    EnZf_SetupSlash(this);
                } else {
                    func_80B44DC4(this, play);
                }
            }
        }
    }
}

void EnZf_SetupJumpUp(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfJumpingAnim, 1.0f, 0.0f, 3.0f, ANIMMODE_ONCE, 0.0f);
    this->unk_3F0 = 0;
    this->hopAnimIndex = 1;
    this->action = ENZF_ACTION_JUMP_UP;
    this->actor.velocity.y = 22.0f;
    this->actor.speedXZ = 7.5f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnZf_SetupAction(this, EnZf_JumpUp);
}

void EnZf_JumpUp(EnZf* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4000, 1);
    if (this->actor.velocity.y >= 5.0f) {
        func_800355B8(play, &this->leftFootPos);
        func_800355B8(play, &this->rightFootPos);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_3F0 == 0) {
            Animation_Change(&this->skelAnime, &gZfSlashAnim, 3.0f, 0.0f, 13.0f, ANIMMODE_ONCE, -4.0f);
            this->unk_3F0 = 10;
        } else if (this->actor.bgCheckFlags & 3) {
            this->actor.velocity.y = 0.0f;
            this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
            this->actor.speedXZ = 0.0f;
            this->actor.world.pos.y = this->actor.floorHeight;
            EnZf_SetupSlash(this);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_ATTACK);
            this->skelAnime.curFrame = 13.0f;
        }
    }
}

// Conditional setup function
void func_80B483E4(EnZf* this, PlayState* play) {
    s16 playerRotY;
    Player* player;

    if ((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
        Actor_TestFloorInDirection(&this->actor, play, 40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF)) ||
        Actor_TestFloorInDirection(&this->actor, play, -40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF))) {
        Animation_PlayLoop(&this->skelAnime, &gZfSidesteppingAnim);
        player = GET_PLAYER(play);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4000, 1);
        playerRotY = player->actor.shape.rot.y;

        if (Math_SinS(playerRotY - this->actor.shape.rot.y) >= 0.0f) {
            this->actor.speedXZ = -6.0f;
        } else if (Math_SinS(playerRotY - this->actor.shape.rot.y) < 0.0f) { // Superfluous check
            this->actor.speedXZ = 6.0f;
        }

        this->unk_408 = 0.0f;
        this->hopAnimIndex = 0;
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
        this->unk_3F0 = Rand_ZeroOne() * 10.0f + 5.0f;
        this->action = ENZF_ACTION_CIRCLE_AROUND_PLAYER;
        EnZf_SetupAction(this, EnZf_CircleAroundPlayer);
    } else {
        EnZf_SetupApproachPlayer(this, play);
    }
}

void EnZf_CircleAroundPlayer(EnZf* this, PlayState* play) {
    s16 playerRot;
    s16 phi_v0_4;
    Player* player = GET_PLAYER(play);
    s32 curKeyFrame;
    s32 prevKeyFrame;
    s32 playSpeed;
    f32 baseRange = 0.0f;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 1);
    playerRot = player->actor.shape.rot.y;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        if (this->unk_3F8) {
            this->actor.speedXZ = -this->actor.speedXZ;
        }
    } else if ((this->actor.bgCheckFlags & 8) ||
               !Actor_TestFloorInDirection(&this->actor, play, this->actor.speedXZ,
                                           this->actor.shape.rot.y + 0x3FFF)) {
        if (this->actor.bgCheckFlags & 8) {
            if (this->actor.speedXZ >= 0.0f) {
                phi_v0_4 = this->actor.shape.rot.y + 0x3FFF;
            } else {
                phi_v0_4 = this->actor.shape.rot.y - 0x3FFF;
            }

            phi_v0_4 = this->actor.wallYaw - phi_v0_4;
        } else {
            this->actor.speedXZ *= -0.8f;
            phi_v0_4 = 0;
        }

        if (ABS(phi_v0_4) > 0x4000) {
            this->actor.speedXZ *= -0.8f;
            if (this->actor.speedXZ < 0.0f) {
                this->actor.speedXZ -= 0.5f;
            } else {
                this->actor.speedXZ += 0.5f;
            }
        }
    }

    if (Math_SinS(playerRot - this->actor.shape.rot.y) >= 0.0f) {
        this->actor.speedXZ += 0.125f;
    } else {
        this->actor.speedXZ -= 0.125f;
    }

    this->actor.world.rot.y = this->actor.shape.rot.y + 0x4000;

    if (Actor_OtherIsTargeted(play, &this->actor)) {
        baseRange = 100.0f;
    }

    if (this->actor.xzDistToPlayer <= (70.0f + baseRange)) {
        Math_SmoothStepToF(&this->unk_408, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if ((90.0f + baseRange) < this->actor.xzDistToPlayer) {
        Math_SmoothStepToF(&this->unk_408, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&this->unk_408, 0.0f, 1.0f, 5.65f, 0.0f);
    }

    if ((this->unk_408 != 0.0f) && !EnZf_SecondaryFloorCheck(this, play, this->unk_408)) {
        this->actor.world.pos.x += Math_SinS(this->actor.shape.rot.y) * this->unk_408;
        this->actor.world.pos.z += Math_CosS(this->actor.shape.rot.y) * this->unk_408;
    }

    if (ABS(this->actor.speedXZ) >= ABS(this->unk_408)) {
        this->skelAnime.playSpeed = -this->actor.speedXZ * 0.75f;
    } else if (this->skelAnime.playSpeed < 0.0f) {
        this->skelAnime.playSpeed = this->unk_408 * -0.75f;
    } else {
        this->skelAnime.playSpeed = this->unk_408 * 0.75f;
    }

    curKeyFrame = this->skelAnime.curFrame;
    SkelAnime_Update(&this->skelAnime);
    prevKeyFrame = this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed);
    playSpeed = (f32)ABS(this->skelAnime.playSpeed);

    this->curPlatform = EnZf_FindPlatform(&this->actor.world.pos, this->curPlatform);

    if (EnZf_FindPlatform(&player->actor.world.pos, -1) != this->curPlatform) {
        this->actor.speedXZ = 0.0f;

        if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ &&
            (D_80B4A1B4 == this->actor.params)) {
            EnZf_SetupHopAndTaunt(this);
        } else {
            EnZf_SetupApproachPlayer(this, play);
        }
    } else if ((this->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(play, this)) {
        if (this->unk_3F0 == 0) {
            phi_v0_4 = player->actor.shape.rot.y - this->actor.shape.rot.y;

            phi_v0_4 = ABS(phi_v0_4);

            if (phi_v0_4 >= 0x3A98) {
                if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) && (D_80B4A1B4 == this->actor.params)) {
                    EnZf_SetupHopAndTaunt(this);
                } else {
                    func_80B45384(this);
                    this->unk_3F0 = Rand_ZeroOne() * 5.0f + 1.0f;
                }
            } else if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) && (D_80B4A1B4 == this->actor.params)) {
                EnZf_SetupHopAndTaunt(this);
            } else {
                this->actor.world.rot.y = this->actor.shape.rot.y;

                if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0) &&
                    EnZf_CanAttack(play, this)) {
                    EnZf_SetupSlash(this);
                } else if ((this->actor.xzDistToPlayer < 280.0f) && (this->actor.xzDistToPlayer > 240.0f) &&
                           !EnZf_PrimaryFloorCheck(this, play, 191.9956f) &&
                           ((play->gameplayFrames % 2) == 0)) {
                    EnZf_SetupJumpForward(this);
                } else {
                    EnZf_SetupApproachPlayer(this, play);
                }
            }
        } else {
            this->unk_3F0--;
        }
        if (curKeyFrame != (s32)this->skelAnime.curFrame) {
            s32 nextKeyFrame = playSpeed + curKeyFrame;
            if (((prevKeyFrame < 14) && (nextKeyFrame >= 16)) || ((prevKeyFrame < 27) && (nextKeyFrame >= 29))) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_WALK);
            }
        }
        if ((play->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
        }
    }
}

void EnZf_SetupDie(EnZf* this) {
    Animation_Change(&this->skelAnime, &gZfDyingAnim, 1.5f, 0.0f, Animation_GetLastFrame(&gZfDyingAnim), ANIMMODE_ONCE,
                     -4.0f);

    if ((this->actor.bgCheckFlags & 1) && ((this->actor.velocity.y == 0.0f) || (this->actor.velocity.y == -4.0f))) {
        this->actor.speedXZ = 0.0f;
        this->hopAnimIndex = 0;
    } else {
        this->hopAnimIndex = 1;
    }

    this->action = ENZF_ACTION_DIE;
    this->actor.flags &= ~ACTOR_FLAG_0;

    if (D_80B4A1B4 != -1) {
        if (this->actor.prev != NULL) {
            ((EnZf*)this->actor.prev)->unk_3F4 = 90;

            if (this->actor.prev->colChkInfo.health < 3) {
                this->actor.prev->colChkInfo.health = 3;
            }
        } else {
            ((EnZf*)this->actor.next)->unk_3F4 = 90;

            if (this->actor.next->colChkInfo.health < 3) {
                this->actor.next->colChkInfo.health = 3;
            }
        }
    }

    D_80B4A1B0 = 0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DEAD);
    EnZf_SetupAction(this, EnZf_Die);
}

void EnZf_Die(EnZf* this, PlayState* play) {

    if (this->actor.bgCheckFlags & 2) {
        this->actor.speedXZ = 0.0f;
    }

    if (this->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.15f, 0.0f);
        this->hopAnimIndex = 0;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.category != ACTORCAT_PROP) {
            if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ && (D_80B4A1B4 == -1)) {
                Flags_SetSwitch(play, this->clearFlag);
                func_800F5B58();
            } else {
                D_80B4A1B4 = -1;
            }
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        }

        if (this->alpha != 0) {
            this->actor.shape.shadowAlpha = this->alpha -= 5;

        } else {
            Actor_Kill(&this->actor);
        }
    } else {
        s32 curFrame = this->skelAnime.curFrame;

        if ((curFrame == 10) || (curFrame == 18)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
        }
    }
}

void EnZf_UpdateHeadRotation(EnZf* this, PlayState* play) {
    s16 angleTemp;

    if ((this->actor.params == ENZF_TYPE_DINOLFOS) && (this->action == ENZF_ACTION_3) && (this->unk_3F4 != 0)) {
        this->headRot = Math_SinS(this->unk_3F4 * 1400) * 0x2AA8;
    } else {
        angleTemp = this->actor.yawTowardsPlayer;
        angleTemp -= (s16)(this->headRot + this->actor.shape.rot.y);
        this->headRotTemp = CLAMP(angleTemp, -0x7D0, 0x7D0);
        this->headRot += this->headRotTemp;
        this->headRot = CLAMP(this->headRot, -0x1CD7, 0x1CD7);
    }
}

void EnZf_UpdateDamage(EnZf* this, PlayState* play) {
    s32 pad;
    s16 dropParams;

    if ((this->bodyCollider.base.acFlags & AC_HIT) && (this->action <= ENZF_ACTION_STUNNED)) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;

        if (((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
             (D_80B4A1B4 != this->actor.params)) &&
            (this->actor.colChkInfo.damageEffect != ENZF_DMGEFF_IMMUNE)) {
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            Actor_SetDropFlag(&this->actor, &this->bodyCollider.info, 0);

            if ((this->actor.colChkInfo.damageEffect == ENZF_DMGEFF_STUN) ||
                (this->actor.colChkInfo.damageEffect == ENZF_DMGEFF_ICE)) {
                if (this->action != ENZF_ACTION_STUNNED) {
                    Actor_SetColorFilter(&this->actor, 0, 120, 0, 80);
                    Actor_ApplyDamage(&this->actor);
                    EnZf_SetupStunned(this);
                }
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_CRY);
                Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 8);

                if (Actor_ApplyDamage(&this->actor) == 0) {
                    dropParams = 0x40;
                    EnZf_SetupDie(this);

                    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
                        dropParams = 0xE0;
                    }

                    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, dropParams);
                    Enemy_StartFinishingBlow(play, &this->actor);
                } else {
                    if ((D_80B4A1B4 != -1) && ((this->actor.colChkInfo.health + this->actor.colChkInfo.damage) >= 4) &&
                        (this->actor.colChkInfo.health < 4)) {
                        this->damageEffect = ENZF_DMGEFF_PROJECTILE;
                    }

                    EnZf_SetupDamaged(this);
                }
            }
        }
    }
}

void EnZf_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZf* this = (EnZf*)thisx;
    s32 pad2;

    EnZf_UpdateDamage(this, play);
    if (this->actor.colChkInfo.damageEffect != ENZF_DMGEFF_IMMUNE) {
        this->unk_3F8 = false;
        if ((this->hopAnimIndex != 1) && (this->action != ENZF_ACTION_HOP_AWAY)) {
            if (this->actor.speedXZ != 0.0f) {
                this->unk_3F8 = EnZf_PrimaryFloorCheck(this, play, this->actor.speedXZ * 1.5f);
            }
            if (!this->unk_3F8) {
                this->unk_3F8 = EnZf_PrimaryFloorCheck(this, play, 0.0f);
            }
        }

        if (!this->unk_3F8) {
            Actor_MoveForward(&this->actor);
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 25.0f, 30.0f, 60.0f, 0x1D);

        if (!(this->actor.bgCheckFlags & 1)) {
            this->hopAnimIndex = 1;
        }

        this->actionFunc(this, play);
    }

    if (this->actor.colChkInfo.health > 0) {
        if ((this->action != ENZF_ACTION_SLASH) && (this->action != ENZF_ACTION_STUNNED)) {
            EnZf_UpdateHeadRotation(this, play);
        }

        if ((D_80B4A1B0 != 0) && (D_80B4A1B4 != this->actor.params)) {
            EnZf_SetupSheatheSword(this, play);
            D_80B4A1B4 = this->actor.params;
            D_80B4A1B0 = 0;

            if (this->actor.prev != NULL) {
                ((EnZf*)this->actor.prev)->unk_3F4 = 90;
            } else {
                ((EnZf*)this->actor.next)->unk_3F4 = 90;
            }
        }
    }

    if (this->action >= ENZF_ACTION_DIE) {
        Math_SmoothStepToS(&this->headRot, 0, 1, 2000, 0);

        if (this->action <= ENZF_ACTION_HOP_AND_TAUNT) {
            if ((this->unk_3F4 == 1) && (this->actor.bgCheckFlags & 1)) {
                if (this->actor.colChkInfo.health > 0) {
                    EnZf_SetupDrawSword(this, play);
                }
                this->unk_3F4--;
            }
        }

        if (this->unk_3F4 >= 2) {
            this->unk_3F4--;
        }
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 40.0f;

    if ((this->actor.colChkInfo.health > 0) && (this->alpha == 255)) {
        Collider_UpdateCylinder(&this->actor, &this->bodyCollider);

        if ((this->actor.world.pos.y == this->actor.floorHeight) && (this->action <= ENZF_ACTION_DAMAGED)) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if ((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
            (D_80B4A1B4 != this->actor.params)) {
            if ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000)) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
            }
        }
    }

    if ((this->action == ENZF_ACTION_SLASH) && (this->skelAnime.curFrame >= 14.0f) &&
        (this->skelAnime.curFrame <= 20.0f)) {
        if (!(this->swordCollider.base.atFlags & AT_BOUNCED) && !(this->swordCollider.base.acFlags & AC_HIT)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->swordCollider.base);
        } else {
            this->swordCollider.base.atFlags &= ~AT_BOUNCED;
            this->swordCollider.base.acFlags &= ~AC_HIT;
            EnZf_SetupRecoilFromBlockedSlash(this);
        }
    }
}

s32 EnZf_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnZf* this = (EnZf*)thisx;

    switch (limbIndex) {
        case ENZF_LIMB_HEAD_ROOT:
            rot->y -= this->headRot;
            break;
        case ENZF_LIMB_SWORD:
            if (this->swordSheathed) {
                *dList = gZfEmptyHandDL;
            }
            break;
        case ENZF_LIMB_SCABBARD:
            if (this->swordSheathed) {
                *dList = gZfSheathedSwordDL;
            }
            break;
        default:
            break;
    }

    return false;
}

void EnZf_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f sUnused = { 1100.0f, -700.0f, 0.0f };
    static Vec3f footOffset = { 300.0f, 0.0f, 0.0f };
    static Vec3f D_80B4A2A4 = { 300.0f, -1700.0f, 0.0f }; // Sword tip?
    static Vec3f D_80B4A2B0 = { -600.0f, 300.0f, 0.0f };  // Sword hilt?
    static Vec3f swordQuadOffset1 = { 0.0f, 1500.0f, 0.0f };
    static Vec3f swordQuadOffset0 = { -600.0f, -3000.0f, 1000.0f };
    static Vec3f swordQuadOffset3 = { -600.0f, -3000.0f, -1000.0f };
    static Vec3f swordQuadOffset2 = { 1500.0f, -3000.0f, 0.0f };
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f sp54;
    Vec3f sp48;
    EnZf* this = (EnZf*)thisx;
    s32 bodyPart = -1;

    if (limbIndex == ENZF_LIMB_SWORD) {
        Matrix_MultVec3f(&swordQuadOffset1, &this->swordCollider.dim.quad[1]);
        Matrix_MultVec3f(&swordQuadOffset0, &this->swordCollider.dim.quad[0]);
        Matrix_MultVec3f(&swordQuadOffset3, &this->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&swordQuadOffset2, &this->swordCollider.dim.quad[2]);
        Collider_SetQuadVertices(&this->swordCollider, &this->swordCollider.dim.quad[0],
                                 &this->swordCollider.dim.quad[1], &this->swordCollider.dim.quad[2],
                                 &this->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&D_80B4A2A4, &sp54);
        Matrix_MultVec3f(&D_80B4A2B0, &sp48);

        if (this->action == ENZF_ACTION_SLASH) {
            if (this->skelAnime.curFrame < 14.0f) {
                EffectBlure_AddSpace(Effect_GetByIndex(this->blureIndex));
            } else if (this->skelAnime.curFrame < 20.0f) {
                EffectBlure_AddVertex(Effect_GetByIndex(this->blureIndex), &sp54, &sp48);
            }
        }
    } else {
        Actor_SetFeetPos(&this->actor, limbIndex, ENZF_LIMB_LEFT_FOOT, &footOffset, ENZF_LIMB_RIGHT_FOOT, &footOffset);
    }

    switch (limbIndex) {
        case ENZF_LIMB_LEFT_FOOT:
            Matrix_MultVec3f(&footOffset, &this->leftFootPos);
            break;
        case ENZF_LIMB_RIGHT_FOOT:
            Matrix_MultVec3f(&footOffset, &this->rightFootPos);
            break;
    }

    if (this->iceTimer != 0) {
        switch (limbIndex) {
            case ENZF_LIMB_HEAD:
                bodyPart = 0;
                break;
            case ENZF_LIMB_NECK:
                bodyPart = 1;
                break;
            case ENZF_LIMB_CHEST_ARMOR:
                bodyPart = 2;
                break;
            case ENZF_LIMB_RIGHT_FOREARM:
                bodyPart = 3;
                break;
            case ENZF_LIMB_LEFT_FOREARM:
                bodyPart = 4;
                break;
            case ENZF_LIMB_TRUNK:
                bodyPart = 5;
                break;
            case ENZF_LIMB_SWORD_ROOT:
                bodyPart = 6;
                break;
            case ENZF_LIMB_RIGHT_SHIN:
                bodyPart = 7;
                break;
            case ENZF_LIMB_LEFT_SHIN_ROOT:
                bodyPart = 8;
                break;
            default:
                break;
        }
        if (bodyPart >= 0) {
            Matrix_MultVec3f(&zeroVec, &this->bodyPartsPos[bodyPart]);
        }
    }
}

static Gfx D_80B4A2F8[] = {
    gsSPTexture(0x0A00, 0x0A00, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

void EnZf_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZf* this = (EnZf*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    func_8002EBCC(thisx, play, 1);

    gSPTexture(D_80B4A2F8, IREG(0), IREG(1), 0, G_TX_RENDERTILE, G_ON);

    gSPSegment(POLY_OPA_DISP++, 0x08, D_80B4A2F8);

    if (this->alpha == 255) {
        func_80093D18(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_OPA_DISP++, 0x09, &D_80116280[2]);

        POLY_OPA_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnZf_OverrideLimbDraw, EnZf_PostLimbDraw, this, POLY_OPA_DISP);

        if (this->iceTimer != 0) {
            thisx->colorFilterTimer++;
            this->iceTimer--;

            if ((this->iceTimer % 4) == 0) {
                s32 icePosIndex = this->iceTimer >> 2;

                EffectSsEnIce_SpawnFlyingVec3f(play, thisx, &this->bodyPartsPos[icePosIndex], 150, 150, 150, 250,
                                               235, 245, 255, 1.4f);
            }
        }
    } else { // fades out when dead
        func_80093D84(play->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_XLU_DISP++, 0x09, &D_80116280[0]);
        POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnZf_OverrideLimbDraw, EnZf_PostLimbDraw, this, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZf_SetupCircleAroundPlayer(EnZf* this, f32 speed) {
    Animation_MorphToLoop(&this->skelAnime, &gZfSidesteppingAnim, -1.0f);
    this->unk_3F0 = Rand_ZeroOne() * 10.0f + 8.0f;

    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
        this->actor.speedXZ = 2.0f * speed;
        this->unk_3F0 /= 2;
    } else {
        this->actor.speedXZ = speed;
    }

    this->hopAnimIndex = 0;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
    this->action = ENZF_ACTION_CIRCLE_AROUND_PLAYER;
    EnZf_SetupAction(this, EnZf_CircleAroundPlayer);
}

s32 EnZf_DodgeRangedEngaging(PlayState* play, EnZf* this) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 phi_t0;
    s16 phi_v1;

    projectileActor = Actor_GetProjectileActor(play, &this->actor, 600.0f);

    if (projectileActor != NULL) {
        yawToProjectile =
            Actor_WorldYawTowardActor(&this->actor, projectileActor) - (s16)(u16)(this->actor.shape.rot.y);
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;

        phi_t0 = 0;

        if (EnZf_PrimaryFloorCheck(this, play, -8.0f)) {
            phi_t0 = 1;
        }

        if (EnZf_PrimaryFloorCheck(this, play, 8.0f)) {
            phi_t0 |= 2;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y;

        if ((((this->actor.xzDistToPlayer < 90.0f) || (phi_t0 == 3)) &&
             !EnZf_PrimaryFloorCheck(this, play, 135.0f)) ||
            (projectileActor->id == ACTOR_ARMS_HOOK)) {
            EnZf_SetupJumpUp(this);
            return true;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;

        if (phi_t0 == 0) {
            phi_v1 = play->gameplayFrames % 2;
        } else {
            phi_v1 = phi_t0;
        }

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) >= 0x6000)) {
            if (phi_v1 & 1) {
                EnZf_SetupCircleAroundPlayer(this, 8.0f);
                return true;
            }
            EnZf_SetupCircleAroundPlayer(this, -8.0f);
            return true;
        }
        if (ABS(yawToProjectile) < 0x5FFF) {
            if (phi_v1 & 1) {
                EnZf_SetupCircleAroundPlayer(this, 4.0f);
                return true;
            }
            EnZf_SetupCircleAroundPlayer(this, -4.0f);
        }
        return true;
    }
    return false;
}

s32 EnZf_DodgeRangedWaiting(PlayState* play, EnZf* this) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 phi_t0;
    s16 sp1E;
    s16 sp1C = 0;

    projectileActor = Actor_GetProjectileActor(play, &this->actor, 600.0f);
    if (projectileActor != NULL) {
        yawToProjectile = Actor_WorldYawTowardActor(&this->actor, projectileActor) - (s16)(u16)this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF; // Set to move sideways

        phi_t0 = 0;

        if (EnZf_PrimaryFloorCheck(this, play, -70.0f)) {
            phi_t0 = 1;
        }

        if (EnZf_PrimaryFloorCheck(this, play, 70.0f)) {
            phi_t0 |= 2;
        }

        this->actor.speedXZ = 0.0f;

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) >= 0x6000)) {
            if (phi_t0 == 0) {
                if ((play->gameplayFrames % 2) != 0) {
                    sp1E = 6;
                } else {
                    sp1E = -6;
                }
            } else {
                switch (phi_t0) {
                    case 1:
                        sp1E = 6;
                        break;
                    case 2:
                        sp1E = -6;
                        break;
                    case 1 | 2:
                        sp1C = 5;
                        sp1E = 0;
                        break;
                }
            }
        } else if (ABS(yawToProjectile) < 0x5FFF) {
            if (phi_t0 == 0) {
                if ((play->gameplayFrames % 2) != 0) {
                    sp1E = 6;
                } else {
                    sp1E = -6;
                }
            } else {
                switch (phi_t0) {
                    case 1:
                        sp1E = 6;
                        break;
                    case 2:
                        sp1E = -6;
                        break;
                    case 1 | 2:
                        sp1C = 10;
                        sp1E = 0;
                        break;
                }
            }
        }

        this->unk_408 = sp1E;
        this->unk_40C = sp1C;
        return true;
    }
    return false;
}

void EnZf_Reset(void) {
    D_80B4A1B0 = 0;
    D_80B4A1B4 = 1;
}
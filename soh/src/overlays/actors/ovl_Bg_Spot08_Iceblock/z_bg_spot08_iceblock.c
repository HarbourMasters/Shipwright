/*
 * File: z_bg_spot08_iceblock.c
 * Overlay: ovl_Bg_Spot08_Iceblock
 * Description: Floating ice platforms
 */

#include "z_bg_spot08_iceblock.h"
#include "objects/object_spot08_obj/object_spot08_obj.h"

#define FLAGS 0

void BgSpot08Iceblock_Init(Actor* thisx, PlayState* play);
void BgSpot08Iceblock_Destroy(Actor* thisx, PlayState* play);
void BgSpot08Iceblock_Update(Actor* thisx, PlayState* play);
void BgSpot08Iceblock_Draw(Actor* thisx, PlayState* play);

void BgSpot08Iceblock_SetupFloatNonrotating(BgSpot08Iceblock* this);
void BgSpot08Iceblock_FloatNonrotating(BgSpot08Iceblock* this, PlayState* play);
void BgSpot08Iceblock_SetupFloatRotating(BgSpot08Iceblock* this);
void BgSpot08Iceblock_FloatRotating(BgSpot08Iceblock* this, PlayState* play);
void BgSpot08Iceblock_SetupFloatOrbitingTwins(BgSpot08Iceblock* this);
void BgSpot08Iceblock_FloatOrbitingTwins(BgSpot08Iceblock* this, PlayState* play);
void BgSpot08Iceblock_SetupNoAction(BgSpot08Iceblock* this);

const ActorInit Bg_Spot08_Iceblock_InitVars = {
    ACTOR_BG_SPOT08_ICEBLOCK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT08_OBJ,
    sizeof(BgSpot08Iceblock),
    (ActorFunc)BgSpot08Iceblock_Init,
    (ActorFunc)BgSpot08Iceblock_Destroy,
    (ActorFunc)BgSpot08Iceblock_Update,
    (ActorFunc)BgSpot08Iceblock_Draw,
    NULL,
};

void BgSpot08Iceblock_SetupAction(BgSpot08Iceblock* this, BgSpot08IceblockActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgSpot08Iceblock_InitDynaPoly(BgSpot08Iceblock* this, PlayState* play, CollisionHeader* collision,
                                   s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning: move BG registration failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

// Sets params to 0x10 (medium, nonrotating) if not in the cases listed.
void BgSpot08Iceblock_CheckParams(BgSpot08Iceblock* this) {
    switch (this->dyna.actor.params & 0xFF) {
        case 0xFF:
            this->dyna.actor.params = 0x10;
            break;
        default:
            // "Error: arg_data setting error"
            osSyncPrintf("Error : arg_data 設定ミスです。(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                         this->dyna.actor.params);
            this->dyna.actor.params = 0x10;
            break;
        case 1:
        case 4:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x14:
        case 0x20:
        case 0x23:
        case 0x24:
            break;
    }
}

void BgSpot08Iceblock_Bobbing(BgSpot08Iceblock* this) {
    this->bobOffset = (Math_SinS(this->bobPhaseSlow) * 4.0f) + (Math_SinS(this->bobPhaseFast) * 3.0f);
}

void BgSpot08Iceblock_SinkUnderPlayer(BgSpot08Iceblock* this) {
    f32 target;
    f32 step;

    switch (this->dyna.actor.params & 0xF0) {
        case 0:
            step = 0.15f;
            break;
        case 0x10:
            step = 0.2f;
            break;
        case 0x20:
            step = 0.4f;
            break;
    }

    // Sink under Player's weight if standing on it
    target = (func_80043548(&this->dyna) ? -4.0f : 0.0f);

    Math_StepToF(&this->sinkOffset, target, step);
}

void BgSpot08Iceblock_SetWaterline(BgSpot08Iceblock* this) {
    this->dyna.actor.world.pos.y = this->sinkOffset + this->bobOffset + this->dyna.actor.home.pos.y;
}

void BgSpot08Iceblock_MultVectorScalar(Vec3f* dest, Vec3f* v, f32 scale) {
    dest->x = v->x * scale;
    dest->y = v->y * scale;
    dest->z = v->z * scale;
}

void BgSpot08Iceblock_CrossProduct(Vec3f* dest, Vec3f* v1, Vec3f* v2) {
    dest->x = (v1->y * v2->z) - (v1->z * v2->y);
    dest->y = (v1->z * v2->x) - (v1->x * v2->z);
    dest->z = (v1->x * v2->y) - (v1->y * v2->x);
}

s32 BgSpot08Iceblock_NormalizeVector(Vec3f* dest, Vec3f* v) {
    f32 magnitude;

    magnitude = Math3D_Vec3fMagnitude(v);
    if (magnitude < 0.001f) {
        dest->x = dest->y = 0.0f;
        dest->z = 1.0f;
        return false;
    } else {
        dest->x = v->x * (1.0f / magnitude);
        dest->y = v->y * (1.0f / magnitude);
        dest->z = v->z * (1.0f / magnitude);
        return true;
    }
}

static Vec3f sVerticalVector = { 0.0f, 1.0f, 0.0f };
static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };
static f32 sInertias[] = { 1.0f / 70000000, 1.0f / 175000000, 1.0f / 700000000 };
static f32 sDampingFactors[] = { 0.96f, 0.96f, 0.98f };

static f32 sRollSins[] = {
    0.22495104f, // sin(13 degrees)
    0.22495104f, // sin(13 degrees)
    0.03489947f, // sin(2 degrees)
};

static f32 sRollCoss[] = {
    0.97437006f, // cos(13 degrees)
    0.97437006f, // cos(13 degrees)
    0.99939084f, // cos(2 degrees)
};

/**
 *  Handles all the factors that influence rolling: inertia, random oscillations, and most significantly, player weight,
 * and combines them to produce a matrix that rotates the actor to match the surface normal
 */
void BgSpot08Iceblock_Roll(BgSpot08Iceblock* this, PlayState* play) {
    f32 deviationFromVertSq;
    f32 stabilityCorrection;
    Vec3f surfaceNormalHorizontal;
    Vec3f playerCentroidDiff;
    Vec3f playerMoment;
    Vec3f surfaceNormalHorizontalScaled;
    Vec3f randomNutation;
    Vec3f tempVec; // reused with different meanings
    Vec3f torqueDirection;
    f32 playerCentroidDist;
    s32 rollDataIndex;
    MtxF mtx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    switch (this->dyna.actor.params & 0xFF) {
        case 0x11: // Medium nonrotating
            rollDataIndex = 0;
            break;
        case 1:
            rollDataIndex = 1; // Large nonrotating
            break;
        default:
            rollDataIndex = 2;
            break;
    }

    Math_Vec3f_Diff(&player->actor.world.pos, &this->dyna.actor.world.pos, &playerCentroidDiff);
    playerCentroidDiff.y -= (150.0f * this->dyna.actor.scale.y);
    playerCentroidDist = Math3D_Vec3fMagnitude(&playerCentroidDiff);

    randomNutation.x = (Rand_ZeroOne() - 0.5f) * (1.0f / 625);
    randomNutation.y = 0.0f;
    randomNutation.z = (Rand_ZeroOne() - 0.5f) * (1.0f / 625);

    surfaceNormalHorizontal.x = this->surfaceNormal.x;
    surfaceNormalHorizontal.y = 0.0f;
    surfaceNormalHorizontal.z = this->surfaceNormal.z;

    // If player is standing on it or holding the edge
    if (func_8004356C(&this->dyna) && (playerCentroidDist > 3.0f)) {
        Math_Vec3f_Diff(&playerCentroidDiff, &surfaceNormalHorizontal, &playerMoment);
        BgSpot08Iceblock_MultVectorScalar(&playerMoment, &playerMoment,
                                          (sInertias[rollDataIndex] * playerCentroidDist) / this->dyna.actor.scale.x);
    } else {
        playerMoment = sZeroVector;
    }

    BgSpot08Iceblock_MultVectorScalar(&surfaceNormalHorizontalScaled, &surfaceNormalHorizontal, -0.01f);

    // Add all three deviations
    Math_Vec3f_Sum(&this->normalDelta, &playerMoment, &this->normalDelta);
    Math_Vec3f_Sum(&this->normalDelta, &surfaceNormalHorizontalScaled, &this->normalDelta);
    Math_Vec3f_Sum(&this->normalDelta, &randomNutation, &this->normalDelta);

    this->normalDelta.y = 0.0f;

    Math_Vec3f_Sum(&this->surfaceNormal, &this->normalDelta, &tempVec);

    tempVec.x *= sDampingFactors[rollDataIndex];
    tempVec.z *= sDampingFactors[rollDataIndex];

    // Set up roll axis and final new angle
    if (BgSpot08Iceblock_NormalizeVector(&this->surfaceNormal, &tempVec)) {
        deviationFromVertSq = Math3D_Dist1DSq(this->surfaceNormal.z, this->surfaceNormal.x);

        // Prevent overrolling
        if (sRollSins[rollDataIndex] < deviationFromVertSq) {
            stabilityCorrection = sRollSins[rollDataIndex] / deviationFromVertSq;

            this->surfaceNormal.x *= stabilityCorrection;
            this->surfaceNormal.y = sRollCoss[rollDataIndex];
            this->surfaceNormal.z *= stabilityCorrection;
        }

        BgSpot08Iceblock_CrossProduct(&tempVec, &sVerticalVector, &this->surfaceNormal);

        if (BgSpot08Iceblock_NormalizeVector(&torqueDirection, &tempVec)) {
            this->rotationAxis = torqueDirection;
        }
    } else {
        this->surfaceNormal = sVerticalVector;
    }

    // Rotation by the angle between surfaceNormal and the vertical about rotationAxis
    Matrix_RotateAxis(Math_FAcosF(Math3D_Cos(&sVerticalVector, &this->surfaceNormal)), &this->rotationAxis,
                      MTXMODE_NEW);
    Matrix_RotateY(this->dyna.actor.shape.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_Get(&mtx);
    Matrix_MtxFToYXZRotS(&mtx, &this->dyna.actor.shape.rot, 0);
}

void BgSpot08Iceblock_SpawnTwinFloe(BgSpot08Iceblock* this, PlayState* play) {
    s32 pad[2];
    f32 sin;
    f32 cos;

    sin = Math_SinS(this->dyna.actor.home.rot.y) * 100.0f;
    cos = Math_CosS(this->dyna.actor.home.rot.y) * 100.0f;

    if (!(this->dyna.actor.params & 0x100)) {
        Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_SPOT08_ICEBLOCK,
                           this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y, this->dyna.actor.home.pos.z,
                           this->dyna.actor.home.rot.x, this->dyna.actor.home.rot.y, this->dyna.actor.home.rot.z,
                           0x123);

        this->dyna.actor.world.pos.x += sin;
        this->dyna.actor.world.pos.z += cos;
    } else {
        this->dyna.actor.world.pos.x -= sin;
        this->dyna.actor.world.pos.z -= cos;
    }
    BgSpot08Iceblock_SetupFloatOrbitingTwins(this);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2200, ICHAIN_STOP),
};

void BgSpot08Iceblock_Init(Actor* thisx, PlayState* play) {
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;
    CollisionHeader* colHeader;

    // "spot08 ice floe"
    osSyncPrintf("(spot08 流氷)(arg_data 0x%04x)\n", this->dyna.actor.params);
    BgSpot08Iceblock_CheckParams(this);

    switch (this->dyna.actor.params & 0x200) {
        case 0:
            colHeader = &gZorasFountainIcebergCol;
            break;
        case 0x200:
            colHeader = &gZorasFountainIceRampCol;
            break;
    }

    switch (this->dyna.actor.params & 0xF) {
        case 2:
        case 3:
            BgSpot08Iceblock_InitDynaPoly(this, play, colHeader, DPM_UNK3);
            break;
        default:
            BgSpot08Iceblock_InitDynaPoly(this, play, colHeader, DPM_UNK);
            break;
    }

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    switch (this->dyna.actor.params & 0xF0) {
        case 0:
            Actor_SetScale(&this->dyna.actor, 0.2f);
            break;
        case 0x10:
            Actor_SetScale(&this->dyna.actor, 0.1f);
            break;
        case 0x20:
            Actor_SetScale(&this->dyna.actor, 0.05f);
            break;
    }

    this->bobPhaseSlow = (s32)(Rand_ZeroOne() * (0xFFFF + 0.5f));
    this->bobPhaseFast = (s32)(Rand_ZeroOne() * (0xFFFF + 0.5f));
    this->surfaceNormal.y = 1.0f;
    this->rotationAxis.x = 1.0f;

    switch (this->dyna.actor.params & 0xF) {
        case 0:
        case 1:
            BgSpot08Iceblock_SetupFloatNonrotating(this);
            break;
        case 2:
            BgSpot08Iceblock_SetupFloatRotating(this);
            break;
        case 3:
            BgSpot08Iceblock_SpawnTwinFloe(this, play);
            break;
        case 4:
            BgSpot08Iceblock_SetupNoAction(this);
            break;
    }
}

void BgSpot08Iceblock_Destroy(Actor* thisx, PlayState* play) {
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgSpot08Iceblock_SetupFloatNonrotating(BgSpot08Iceblock* this) {
    BgSpot08Iceblock_SetupAction(this, BgSpot08Iceblock_FloatNonrotating);
}

void BgSpot08Iceblock_FloatNonrotating(BgSpot08Iceblock* this, PlayState* play) {
    BgSpot08Iceblock_Bobbing(this);
    BgSpot08Iceblock_SinkUnderPlayer(this);
    BgSpot08Iceblock_SetWaterline(this);
    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
    BgSpot08Iceblock_Roll(this, play);
}

void BgSpot08Iceblock_SetupFloatRotating(BgSpot08Iceblock* this) {
    BgSpot08Iceblock_SetupAction(this, BgSpot08Iceblock_FloatRotating);
}

void BgSpot08Iceblock_FloatRotating(BgSpot08Iceblock* this, PlayState* play) {
    BgSpot08Iceblock_Bobbing(this);
    BgSpot08Iceblock_SinkUnderPlayer(this);
    BgSpot08Iceblock_SetWaterline(this);
    this->dyna.actor.world.rot.y = this->dyna.actor.world.rot.y + 0x190;
    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
    BgSpot08Iceblock_Roll(this, play);
}

void BgSpot08Iceblock_SetupFloatOrbitingTwins(BgSpot08Iceblock* this) {
    BgSpot08Iceblock_SetupAction(this, BgSpot08Iceblock_FloatOrbitingTwins);
}

void BgSpot08Iceblock_FloatOrbitingTwins(BgSpot08Iceblock* this, PlayState* play) {
    f32 cos;
    f32 sin;

    BgSpot08Iceblock_Bobbing(this);
    BgSpot08Iceblock_SinkUnderPlayer(this);
    BgSpot08Iceblock_SetWaterline(this);

    // parent handles rotations of both
    if (!(this->dyna.actor.params & 0x100)) {
        this->dyna.actor.world.rot.y += 0x190;
        sin = Math_SinS(this->dyna.actor.world.rot.y) * 100.0f;
        cos = Math_CosS(this->dyna.actor.world.rot.y) * 100.0f;

        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + sin;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + cos;

        if (this->dyna.actor.child != NULL) {
            this->dyna.actor.child->world.pos.x = this->dyna.actor.home.pos.x - sin;
            this->dyna.actor.child->world.pos.z = this->dyna.actor.home.pos.z - cos;
        }
    }

    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
    BgSpot08Iceblock_Roll(this, play);
}

void BgSpot08Iceblock_SetupNoAction(BgSpot08Iceblock* this) {
    BgSpot08Iceblock_SetupAction(this, NULL);
}

void BgSpot08Iceblock_Update(Actor* thisx, PlayState* play) {
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;

    if (Rand_ZeroOne() < 0.05f) {
        this->bobIncrSlow = Rand_S16Offset(300, 100);
        this->bobIncrFast = Rand_S16Offset(800, 400);
    }

    this->bobPhaseSlow += this->bobIncrSlow;
    this->bobPhaseFast += this->bobIncrFast;
    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void BgSpot08Iceblock_Draw(Actor* thisx, PlayState* play) {
    Gfx* dList;
    BgSpot08Iceblock* this = (BgSpot08Iceblock*)thisx;

    switch (this->dyna.actor.params & 0x200) {
        case 0:
            dList = gZorasFountainIcebergDL;
            break;
        case 0x200:
            dList = gZorasFountainIceRampDL;
            break;
    }

    Gfx_DrawDListOpa(play, dList);
}

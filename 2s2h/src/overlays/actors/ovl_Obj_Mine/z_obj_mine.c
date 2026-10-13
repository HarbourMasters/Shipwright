/*
 * File: z_obj_mine.c
 * Overlay: ovl_Obj_Mine
 * Description: Spike metal mine
 */

#include "z_obj_mine.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/object_ny/object_ny.h"

#define FLAGS 0x00000000

#define LINK_SIZE 12.0f
#define ATTACH_OFFSET 10.0f
#define PATH_RADIUS 32.0f
#define AIR_RADIUS 20.0f
#define WATER_RADIUS 36.0f
#define BOMB_SPAWN_OFFSET 15.0f
#define WATER_KNOCKBACK 7.0f
#define AIR_KNOCKBACK 0.04f

void ObjMine_Init(Actor* thisx, PlayState* play);
void ObjMine_Destroy(Actor* thisx, PlayState* play);
void ObjMine_Path_Update(Actor* thisx, PlayState* play);
void ObjMine_Path_Draw(Actor* thisx, PlayState* play);

void ObjMine_Path_SetupStationary(ObjMine* this);
void ObjMine_Path_Stationary(ObjMine* this, PlayState* play);
void ObjMine_Path_SetupMove(ObjMine* this);
void ObjMine_Path_Move(ObjMine* this, PlayState* play);
void ObjMine_Explode(ObjMine* this, PlayState* play);
void ObjMine_Air_SetupChained(ObjMine* this);
void ObjMine_Air_Chained(ObjMine* this, PlayState* play);
void ObjMine_Air_SetupStationary(ObjMine* this);
void ObjMine_Air_Stationary(ObjMine* this, PlayState* play);
void ObjMine_Water_SetupChained(ObjMine* this);
void ObjMine_Water_Chained(ObjMine* this, PlayState* play);
void ObjMine_Water_SetupStationary(ObjMine* this);
void ObjMine_Water_Stationary(ObjMine* this, PlayState* play);

void ObjMine_AirWater_Update(Actor* thisx, PlayState* play);
void ObjMine_DrawExplosion(Actor* thisx, PlayState* play);
void ObjMine_Air_Draw(Actor* thisx, PlayState* play);
void ObjMine_Water_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Mine_Profile = {
    /**/ ACTOR_OBJ_MINE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_NY,
    /**/ sizeof(ObjMine),
    /**/ ObjMine_Init,
    /**/ ObjMine_Destroy,
    /**/ ObjMine_Path_Update,
    /**/ ObjMine_Path_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0x01CBFFBE, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static f32 sPathSpeeds[] = {
    1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
};

static ObjMineMtxF3 sStandardBasis = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 1300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 150, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void ObjMine_Path_MoveToWaypoint(ObjMine* this, s32 index) {
    Math_Vec3s_ToVec3f(&this->actor.world.pos, &this->waypoints[index]);
}

s32 ObjMine_GetUnitVec3f(Vec3f* src, Vec3f* dst) {
    f32 magnitude = Math3D_Vec3fMagnitude(src);

    if (magnitude < 0.001f) {
        return false;
    }

    dst->x = src->x * (1.0f / magnitude);
    dst->y = src->y * (1.0f / magnitude);
    dst->z = src->z * (1.0f / magnitude);
    return true;
}

s32 ObjMine_GetUnitVec3fNorm(Vec3f* src, Vec3f* dst, f32* norm, f32* invNorm) {
    f32 magnitude = Math3D_Vec3fMagnitude(src);

    if (magnitude < 0.001f) {
        return false;
    }

    dst->x = src->x * (1.0f / magnitude);
    dst->y = src->y * (1.0f / magnitude);
    dst->z = src->z * (1.0f / magnitude);
    *norm = magnitude;
    *invNorm = 1.0f / magnitude;

    return true;
}

void ObjMine_Path_SpawnBomb(ObjMine* this, PlayState* play) {
    EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                      this->actor.world.pos.y - BOMB_SPAWN_OFFSET, this->actor.world.pos.z, 0, 0, 0,
                                      BOMB_TYPE_BODY);

    if (bomb != NULL) {
        bomb->timer = 0;
    }
}

void ObjMine_AirWater_SpawnBomb(ObjMine* this, PlayState* play) {
    f32 x = this->collider.elements[0].dim.worldSphere.center.x;
    f32 y = this->collider.elements[0].dim.worldSphere.center.y - BOMB_SPAWN_OFFSET;
    f32 z = this->collider.elements[0].dim.worldSphere.center.z;
    EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, x, y, z, 0, 0, 0, BOMB_TYPE_BODY);

    if (bomb != NULL) {
        bomb->timer = 0;
    }
}

s32 ObjMine_AirWater_CheckOC(ObjMine* this) {
    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        return true;
    }

    if (this->collider.base.ocFlags1 & OC1_HIT) {
        Actor* hitActor = this->collider.base.oc;

        if ((hitActor->id == ACTOR_OBJ_MINE) && (hitActor->room == this->actor.room)) {
            return true;
        }
    }

    return false;
}

void ObjMine_Air_CheckAC(ObjMine* this, s16* hitAngle, s16* torqueAngle) {
    s32 pad[2];
    Vec3s* centerPos3s = &this->collider.elements[0].dim.worldSphere.center;
    Vec3f centerPos;
    s16 yawToAttack;
    Actor* attackActor = this->collider.base.ac;

    Math_Vec3s_ToVec3f(&centerPos, centerPos3s);
    yawToAttack = Math_Vec3f_Yaw(&attackActor->world.pos, &centerPos);

    // dmgFlag check is (DMG_DEKU_BUBBLE | DMG_FIRE_ARROW | DMG_ICE_ARROW | DMG_FIRE_ARROW | DMG_NORMAL_ARROW)
    if (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & 0x13820) {
        *hitAngle = attackActor->shape.rot.y;
        *torqueAngle = attackActor->shape.rot.y - yawToAttack;
    } else {
        Vec3f hitPos;
        Vec3s* hitPos3s = &this->collider.elements[0].base.acDmgInfo.hitPos;

        Math_Vec3s_ToVec3f(&hitPos, hitPos3s);
        *hitAngle = Actor_WorldYawTowardActor(attackActor, &this->actor);
        *torqueAngle = Math_Vec3f_Yaw(&attackActor->world.pos, &hitPos) - yawToAttack;
    }
}

void ObjMine_Water_CheckAC(ObjMine* this, Vec3f* knockbackDir) {
    Actor* attackActor = this->collider.base.ac;

    // dmgFlag check is (DMG_DEKU_BUBBLE | DMG_LIGHT_ARROW | DMG_ICE_ARROW | DMG_FIRE_ARROW | DMG_NORMAL_ARROW)
    if (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & 0x13820) {
        Matrix_Push();
        Matrix_RotateYS(attackActor->shape.rot.y, MTXMODE_NEW);
        Matrix_RotateXS(attackActor->shape.rot.x, MTXMODE_APPLY);
        Matrix_MultVecZ(1.0f, knockbackDir);
        Matrix_Pop();
    } else {
        Vec3f posDiff;
        Sphere16* sphere = &this->collider.elements[0].dim.worldSphere;

        posDiff.x = sphere->center.x - attackActor->world.pos.x;
        posDiff.y = sphere->center.y - attackActor->world.pos.y;
        posDiff.z = sphere->center.z - attackActor->world.pos.z;

        if (!ObjMine_GetUnitVec3f(&posDiff, knockbackDir)) {
            Math_Vec3f_Copy(knockbackDir, &sStandardBasis.y);
        }
    }
}

void ObjMine_AirWater_Noop(ObjMine* this) {
}

void ObjMine_SetMatrixTranslation(Vec3f* translation) {
    MtxF* matrix = Matrix_GetCurrent();

    matrix->xw = translation->x;
    matrix->yw = translation->y;
    matrix->zw = translation->z;
}

void ObjMine_SetMatrixRotation(ObjMineMtxF3* basis) {
    MtxF* matrix = Matrix_GetCurrent();

    matrix->xx = basis->x.x;
    matrix->yx = basis->x.y;
    matrix->zx = basis->x.z;
    matrix->wx = 0.0f;

    matrix->xy = basis->y.x;
    matrix->yy = basis->y.y;
    matrix->zy = basis->y.z;
    matrix->wy = 0.0f;

    matrix->xz = basis->z.x;
    matrix->yz = basis->z.y;
    matrix->zz = basis->z.z;
    matrix->wz = 0.0f;

    matrix->xw = 0.0f;
    matrix->yw = 0.0f;
    matrix->zw = 0.0f;
    matrix->ww = 1.0f;
}

s32 ObjMine_StepUntilParallel(Vec3f* value, Vec3f* target, f32 angleStep) {
    Vec3f perpVec;
    Vec3f prevValue;
    Vec3f perpNormal;
    f32 cosAngle = Math3D_Cos(value, target);

    if (Math_CosF(angleStep) <= cosAngle) {
        Math_Vec3f_Copy(value, target);
        return true;
    }

    Matrix_Push();
    Math_Vec3f_Copy(&prevValue, value);
    Math3D_Vec3f_Cross(value, target, &perpVec);
    if (ObjMine_GetUnitVec3f(&perpVec, &perpNormal)) {
        Matrix_RotateAxisS(RAD_TO_BINANG(angleStep), &perpNormal, MTXMODE_NEW);
        Matrix_MultVec3f(&prevValue, value);
    } else {
        Matrix_RotateXFNew(angleStep);
        Matrix_MultVec3f(&prevValue, value);
    }

    Matrix_Pop();
    return false;
}

void ObjMine_UpdateCollider(ObjMine* this) {
    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;
}

void ObjMine_Air_InitChain(ObjMine* this, s32 linkCount) {
    f32 linkCountF = linkCount;
    ObjMineAirChain* airChain = &this->chain.air;
    ObjMineAirLink* airLink;
    s32 i;
    f32 wallCheckRadius = this->actor.home.rot.z * 5.0f;

    airChain->basis.x.x = 1.0f;
    airChain->basis.y.y = 1.0f;
    airChain->basis.z.z = 1.0f;

    airChain->translation.y = 1.0f;

    // Sets restoring force and drag. Longer chains have lower frequency and less drag.
    if (linkCount > 0) {
        airChain->restore = -sqrtf((0.124992f / OBJMINE_CHAIN_MAX) / linkCountF);  // constant is close to 1/8
        airChain->drag = 0.95f + ((0.02000004f / OBJMINE_CHAIN_MAX) * linkCountF); // constant is close to 1/50
    }

    airChain->swayMax = 0.0002f;

    // Consecutive chain links are offset 90 degrees.
    for (i = 0, airLink = airChain->links; i < linkCount; i++, airLink++) {
        airLink->twist = 0x4000;
    }

    if (wallCheckRadius < 0.0f) {
        airChain->wallCheckDistSq = -1.0f; // Negative value means skip wall collision checks
    } else if (wallCheckRadius <= AIR_RADIUS + 1.0f) {
        airChain->wallCheckDistSq = 0.0f;
    } else {
        airChain->wallCheckDistSq = SQ(wallCheckRadius - AIR_RADIUS);
    }
}

void ObjMine_Air_InitCollider(ObjMine* this, s32 linkCount) {
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Collider_UpdateSpheres(0, &this->collider);
}

void ObjMine_Air_SetBasis(ObjMine* this) {
    ObjMineAirChain* airChain = &this->chain.air;
    Vec3f tempVec;

    tempVec.x = -airChain->displacement.x;
    tempVec.y = 1.0f;
    tempVec.z = -airChain->displacement.z;

    ObjMine_GetUnitVec3f(&tempVec, &airChain->basis.y);

    Math3D_Vec3f_Cross(&sStandardBasis.x, &airChain->basis.y, &tempVec);
    ObjMine_GetUnitVec3f(&tempVec, &airChain->basis.z);

    Math3D_Vec3f_Cross(&airChain->basis.y, &airChain->basis.z, &tempVec);
    ObjMine_GetUnitVec3f(&tempVec, &airChain->basis.x);
}

void ObjMine_Air_SetWorld(ObjMine* this) {
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);
    f32 chainLength = ATTACH_OFFSET + (linkCount * LINK_SIZE);

    this->actor.world.pos.x = (this->chain.air.basis.y.x * -chainLength) + this->actor.home.pos.x;
    this->actor.world.pos.y = (this->chain.air.basis.y.y * -chainLength) + this->actor.home.pos.y;
    this->actor.world.pos.z = (this->chain.air.basis.y.z * -chainLength) + this->actor.home.pos.z;
}

void ObjMine_Air_SetChainXZ(ObjMine* this) {
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);
    f32 invLength = 1.0f / (ATTACH_OFFSET + (linkCount * LINK_SIZE));
    f32 dx = this->actor.world.pos.x - this->actor.home.pos.x;
    f32 dz = this->actor.world.pos.z - this->actor.home.pos.z;

    this->chain.air.displacement.x = dx * invLength;
    this->chain.air.displacement.z = dz * invLength;
}

void ObjMine_Water_InitChain(ObjMine* this, s32 linkCount) {
    ObjMineWaterChain* waterChain = &this->chain.water;
    ObjMineWaterLink* waterLink;
    s32 i;
    f32 wallCheckRadius = this->actor.home.rot.z * 5.0f;
    f32 linkY;
    f32 swayVel;

    waterChain->drag = 0.9f;
    waterChain->swayMax = 0.003f;

    waterChain->swayPhaseVel = Rand_Next() >> (0x20 - 13);
    waterChain->restoreXZ = -0.0002f;
    waterChain->restoreY = -0.0002f;

    swayVel = waterChain->swayMax * (LINK_SIZE / 2.0f);
    linkY = this->actor.home.pos.y;

    for (i = 0, waterLink = waterChain->links; i < linkCount; i++, waterLink++) {
        linkY += LINK_SIZE;
        waterLink->basis.x.x = 1.0f;
        waterLink->basis.y.y = 1.0f;
        waterLink->basis.z.z = 1.0f;
        waterLink->pos.x = this->actor.home.pos.x;
        waterLink->pos.y = linkY;
        waterLink->pos.z = this->actor.home.pos.z;
        waterLink->velocity.x = (Rand_ZeroOne() - 0.5f) * swayVel;
        waterLink->velocity.y = (Rand_ZeroOne() - 0.5f) * swayVel;
        waterLink->velocity.z = (Rand_ZeroOne() - 0.5f) * swayVel;
    }
    waterChain->maxY = linkY;

    // Rest position is slightly below max chain length to give slack.
    waterChain->restY = waterChain->maxY * 0.97f;
    linkY -= LINK_SIZE * 1.5f;
    if (waterChain->restY < linkY) { // This should only happen if linkCount > 50
        waterChain->restY = linkY;
    }

    if (wallCheckRadius < 0.0f) {
        waterChain->wallCheckDistSq = -1.0f; // Negative value means skip wall collision checks
    } else if (wallCheckRadius <= WATER_RADIUS + 1.0f) {
        waterChain->wallCheckDistSq = 0.0f;
    } else {
        waterChain->wallCheckDistSq = SQ(wallCheckRadius - WATER_RADIUS);
    }
}

void ObjMine_Water_InitCollider(ObjMine* this, s32 linkCount) {
    Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y + (linkCount * LINK_SIZE) + ATTACH_OFFSET,
                     this->actor.home.pos.z, MTXMODE_NEW);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Collider_UpdateSpheres(0, &this->collider);
}

void ObjMine_Water_SetWorld(ObjMine* this) {
    s32 pad; // Could be recast to thisx as in ObjMine_Path_Move
    ObjMineWaterChain* waterChain = &this->chain.water;
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);

    if (linkCount == 0) {
        this->actor.world.pos.y = this->actor.home.pos.y + LINK_SIZE + ATTACH_OFFSET;
    } else {
        ObjMineWaterLink* lastLink = &waterChain->links[linkCount - 1];
        Vec3f mineOffset;

        Math_Vec3f_ScaleAndStore(&lastLink->basis.y, ATTACH_OFFSET, &mineOffset);
        Math_Vec3f_Sum(&lastLink->pos, &mineOffset, &this->actor.world.pos);
    }
}

void ObjMine_Water_WallCheck(ObjMine* this, PlayState* play) {
    s32 pad; // Could be recast to thisx as in ObjMine_Path_Move
    ObjMineWaterChain* waterChain = &this->chain.water;

    waterChain->touchWall = false;
    if (waterChain->wallCheckDistSq > -1e-6f) {
        //  Checks for walls if mine is sufficiently far from home. If found, sets ejection force towards home.
        if (waterChain->wallCheckDistSq <= Math3D_Dist2DSq(this->actor.home.pos.x, this->actor.home.pos.z,
                                                           this->actor.world.pos.x, this->actor.world.pos.z)) {
            Vec3f centerPos;
            Vec3f offsetPos;
            Vec3f result; // not used
            Vec3f xzFromHome;
            Vec3f xzDirFromHome;
            CollisionPoly* poly; // not used
            s32 bgId;            // not used
            f32 norm;            // not used
            f32 invNorm;         // not used

            xzFromHome.x = this->actor.world.pos.x - this->actor.home.pos.x;
            xzFromHome.y = 0.0f;
            xzFromHome.z = this->actor.world.pos.z - this->actor.home.pos.z;

            if (ObjMine_GetUnitVec3fNorm(&xzFromHome, &xzDirFromHome, &norm, &invNorm)) {

                offsetPos.x = this->actor.world.pos.x + (xzDirFromHome.x * WATER_RADIUS);
                offsetPos.y = this->actor.world.pos.y;
                offsetPos.z = this->actor.world.pos.z + (xzDirFromHome.z * WATER_RADIUS);

                centerPos.x = this->actor.home.pos.x;
                centerPos.y = this->actor.world.pos.y;
                centerPos.z = this->actor.home.pos.z;

                if (BgCheck_EntityLineTest1(&play->colCtx, &centerPos, &offsetPos, &result, &poly, true, false, false,
                                            true, &bgId)) {
                    waterChain->touchWall = true;
                    waterChain->wallEject.x = xzDirFromHome.x * -0.2f;
                    waterChain->wallEject.z = xzDirFromHome.z * -0.2f;
                }
            }
        }
    }
}

Vec3f sLastLinkAccel[OBJMINE_CHAIN_MAX + 1];

void ObjMine_Water_ApplyForces(ObjMine* this) {
    ObjMineWaterChain* waterChain = &this->chain.water;
    ObjMineWaterLink* waterLink;
    s32 i;
    s16 swayPhase = 0;
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);
    f32 inverseCount = 1.0f / linkCount;
    f32 restoreY = (waterChain->links[linkCount - 1].pos.y - waterChain->restY) * waterChain->restoreY;
    Vec3f tension;
    f32 scaledKnockback;
    s32 index;
    s32 pad;
    Vec3f* lastLinkAccel;

    // Applies the buoyant force and sway from the water.
    for (i = 0, waterLink = waterChain->links, lastLinkAccel = sLastLinkAccel; i < linkCount;
         i++, waterLink++, swayPhase += waterChain->swayPhaseVel, lastLinkAccel++) {
        Math_Vec3f_Copy(lastLinkAccel, &waterLink->accel);

        if (waterLink->pos.y <= this->actor.home.pos.y) {
            waterLink->accel.y = waterChain->restoreY * -96.0f;
        } else if (restoreY > 0.0f) {
            waterLink->accel.y = (i + 1) * inverseCount * (restoreY / 0.3f);
        } else {
            waterLink->accel.y = restoreY;
        }

        waterLink->accel.x = (waterLink->pos.x - this->actor.home.pos.x) * waterChain->restoreXZ;
        waterLink->accel.z = (waterLink->pos.z - this->actor.home.pos.z) * waterChain->restoreXZ;
        waterLink->accel.x += waterChain->swayXZ * Math_SinS(swayPhase);
        waterLink->accel.y += waterChain->swayY;
        waterLink->accel.z += waterChain->swayXZ * Math_CosS(swayPhase);
    }

    // Applies knockback force. Knockback on the links is scaled quadratically, possibly to account for link rotations
    // being cumulative
    if (waterChain->knockback > 0.0001f) {
        for (i = 0, waterLink = waterChain->links; i < linkCount; i++, waterLink++) {
            scaledKnockback = SQ((linkCount - i) * inverseCount);

            waterLink->accel.x += waterChain->knockbackDir.x * scaledKnockback;
            waterLink->accel.y += waterChain->knockbackDir.y * scaledKnockback;
            waterLink->accel.z += waterChain->knockbackDir.z * scaledKnockback;
        }
    }

    // Moves chain away from wall if mine is intersecting one
    if (waterChain->touchWall) {
        for (i = 0, waterLink = waterChain->links; i < linkCount; i++, waterLink++) {
            waterLink->accel.x += waterChain->wallEject.x;
            waterLink->accel.z += waterChain->wallEject.z;
        }
    }

    // Forces on the links from other links. This is simulated by a triangle filter on the forces from the previous
    // frame.
    for (i = 0, waterLink = waterChain->links; i < linkCount; i++, waterLink++) {
        Math_Vec3f_Copy(&tension, &gZeroVec3f);

        if (i >= 2) {
            tension.x += sLastLinkAccel[i - 2].x * 0.075f;
            tension.y += sLastLinkAccel[i - 2].y * 0.075f;
            tension.z += sLastLinkAccel[i - 2].z * 0.075f;
        }

        if (i >= 1) {
            tension.x += sLastLinkAccel[i - 1].x * 0.15f;
            tension.y += sLastLinkAccel[i - 1].y * 0.15f;
            tension.z += sLastLinkAccel[i - 1].z * 0.15f;
        }

        tension.x += sLastLinkAccel[i].x * 0.3f;
        tension.y += sLastLinkAccel[i].y * 0.3f;
        tension.z += sLastLinkAccel[i].z * 0.3f;

        index = i + 1;
        if (index < linkCount) {
            tension.x += sLastLinkAccel[index].x * 0.15f;
            tension.y += sLastLinkAccel[index].y * 0.15f;
            tension.z += sLastLinkAccel[index].z * 0.15f;
        }

        index = i + 2;
        if (index < linkCount) {
            tension.x += sLastLinkAccel[index].x * 0.075f;
            tension.y += sLastLinkAccel[index].y * 0.075f;
            tension.z += sLastLinkAccel[index].z * 0.075f;
        }
        waterLink->accel.x += tension.x;
        waterLink->accel.y += tension.y;
        waterLink->accel.z += tension.z;
    }
}

void ObjMine_Water_UpdateLinks(ObjMine* this) {
    s32 pad; // Could be recast to thisx as in ObjMine_Path_Move
    ObjMineMtxF3 newBasis;
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);
    ObjMineWaterChain* waterChain = &this->chain.water;
    ObjMineWaterLink* waterLink;
    s32 i;
    Vec3f* prevBasisX;
    Vec3f tempVec;
    Vec3f diffDir;
    Vec3f unusedLinkPos;
    Vec3f jointPos;
    s32 changeBasis;
    f32 diffNorm;
    f32 invNorm; // not used
    Vec3f* tempBasisX;

    // The joint between the current link and next link is half the link's length from its center. The first link is
    // assumed to be vertical and centered on home.
    jointPos.x = this->actor.home.pos.x;
    jointPos.y = this->actor.home.pos.y + (LINK_SIZE / 2.0f);
    jointPos.z = this->actor.home.pos.z;

    for (i = 0, prevBasisX = NULL, waterLink = waterChain->links; i < linkCount;
         i++, prevBasisX = &waterLink->basis.x, waterLink++) {
        changeBasis = false;

        waterLink->velocity.x += waterLink->accel.x;
        waterLink->velocity.y += waterLink->accel.y;
        waterLink->velocity.z += waterLink->accel.z;

        Math_Vec3f_Scale(&waterLink->velocity, waterChain->drag);
        Math_Vec3f_Copy(&unusedLinkPos, &waterLink->pos);

        waterLink->pos.x += waterLink->velocity.x;
        waterLink->pos.y += waterLink->velocity.y;
        waterLink->pos.z += waterLink->velocity.z;

        // Heavily reduce velocity if link is below home and moving downward.
        if ((waterLink->pos.y <= this->actor.home.pos.y) && (waterLink->velocity.y < 0.0f)) {
            waterLink->velocity.y *= 0.1f;
        }

        // If the calculated position is less than 1/3 of the link's length from the joint, the chain is considered
        // slack and the link keeps the same basis. Otherwise the basis is updated with the new position relative to the
        // joint. The StepUntilParallel causes the chain to straighten over time.
        Math_Vec3f_Diff(&waterLink->pos, &jointPos, &tempVec);

        if (ObjMine_GetUnitVec3fNorm(&tempVec, &diffDir, &diffNorm, &invNorm) && (diffNorm > LINK_SIZE / 3.0f)) {
            Math_Vec3f_Copy(&newBasis.y, &waterLink->basis.y);
            ObjMine_StepUntilParallel(&newBasis.y, &diffDir, M_PIf / 30);

            tempBasisX = (prevBasisX == NULL) ? &sStandardBasis.x : prevBasisX;

            Math3D_Vec3f_Cross(tempBasisX, &newBasis.y, &tempVec);

            // Skips change of basis if any of the basis vectors would be zero.
            if (ObjMine_GetUnitVec3f(&tempVec, &newBasis.z)) {
                Math3D_Vec3f_Cross(&newBasis.y, &newBasis.z, &tempVec);
                if (ObjMine_GetUnitVec3f(&tempVec, &newBasis.x)) {
                    changeBasis = true;
                }
            }
        }

        if (changeBasis) {
            Math_Vec3f_Copy(&waterLink->basis.x, &newBasis.x);
            Math_Vec3f_Copy(&waterLink->basis.y, &newBasis.y);
            Math_Vec3f_Copy(&waterLink->basis.z, &newBasis.z);
        } else {
            diffNorm = LINK_SIZE / 3.0f;
        }

        // Sets new link position from calculated rotation basis. The check ensures displacement from the previous link
        // doesn't cause the links to separate
        if (diffNorm >= (LINK_SIZE / 2.0f)) {
            waterLink->pos.x = jointPos.x + (waterLink->basis.y.x * (LINK_SIZE / 2.0f));
            waterLink->pos.y = jointPos.y + (waterLink->basis.y.y * (LINK_SIZE / 2.0f));
            waterLink->pos.z = jointPos.z + (waterLink->basis.y.z * (LINK_SIZE / 2.0f));
        } else {
            waterLink->pos.x = jointPos.x + (waterLink->basis.y.x * diffNorm);
            waterLink->pos.y = jointPos.y + (waterLink->basis.y.y * diffNorm);
            waterLink->pos.z = jointPos.z + (waterLink->basis.y.z * diffNorm);
        }

        // Calculate the position of the joint with the next link.
        jointPos.x = waterLink->pos.x + (waterLink->basis.y.x * (LINK_SIZE / 2.0f));
        jointPos.y = waterLink->pos.y + (waterLink->basis.y.y * (LINK_SIZE / 2.0f));
        jointPos.z = waterLink->pos.z + (waterLink->basis.y.z * (LINK_SIZE / 2.0f));
    }
}

void ObjMine_Water_UpdateChain(ObjMine* this, PlayState* play) {
    ObjMine_Water_WallCheck(this, play);
    ObjMine_Water_ApplyForces(this);
    ObjMine_Water_UpdateLinks(this);
}

void ObjMine_Init(Actor* thisx, PlayState* play) {
    s32 pad; // Can be playstate recast. Must be gamestate recast.
    ObjMine* this = (ObjMine*)thisx;
    s32 pathIndex = OBJMINE_GET_PATH_INDEX(&this->actor);
    Path* path;
    s32 bgId; // not used
    s32 type = OBJMINE_GET_TYPE(&this->actor);

    Actor_ProcessInitChain(&this->actor, sInitChain);

    this->actor.shape.rot.z = 0;
    this->actor.world.rot.z = 0;

    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);

    if (type == OBJMINE_TYPE_PATH) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 45.0f);
        this->actor.shape.shadowAlpha = 140;
        this->pathSpeed = sPathSpeeds[OBJMINE_GET_PATH_SPEED(&this->actor)]; // sPathSpeeds[i] = i + 1.0f
        if (pathIndex == 0xFF) {
            ObjMine_Path_SetupStationary(this);
        } else {
            path = &play->setupPathList[pathIndex];

            this->waypointIndex = 0;
            this->waypointCount = path->count - 1;
            this->waypoints = (Vec3s*)Lib_SegmentedToVirtual(path->points);

            ObjMine_Path_MoveToWaypoint(this, this->waypointIndex);
            ObjMine_Path_SetupMove(this);
        }
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                     &this->actor.shape.rot);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Collider_UpdateSpheres(0, &this->collider);
        this->actor.floorHeight = BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                              &this->actor, &this->actor.world.pos);
    } else {
        s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);

        this->actor.update = ObjMine_AirWater_Update;
        this->actor.cullingVolumeScale = 150.0f + (linkCount * (LINK_SIZE * 1.75f));
        this->actor.cullingVolumeDownward = 150.0f + (linkCount * (LINK_SIZE * 1.75f));
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 45.0f);
        this->actor.shape.shadowAlpha = 140;

        if (type == OBJMINE_TYPE_AIR) {
            this->actor.draw = ObjMine_Air_Draw;
            ObjMine_Air_InitChain(this, linkCount);
            this->actor.world.pos.y = -ATTACH_OFFSET - (linkCount * LINK_SIZE) + this->actor.home.pos.y;
            ObjMine_Air_InitCollider(this, linkCount);
            func_800B4AEC(play, &this->actor, 0.0f);
            if (linkCount == 0) {
                ObjMine_Air_SetupStationary(this);
            } else {
                ObjMine_Air_SetupChained(this);
            }
        } else {
            this->actor.draw = ObjMine_Water_Draw;
            ObjMine_Water_InitChain(this, linkCount);
            this->actor.world.pos.y = ATTACH_OFFSET + (linkCount * LINK_SIZE) + this->actor.home.pos.y;
            ObjMine_Water_InitCollider(this, linkCount);
            if (linkCount == 0) {
                ObjMine_Water_SetupStationary(this);
            } else {
                ObjMine_Water_SetupChained(this);
            }
        }
    }
}

void ObjMine_Destroy(Actor* thisx, PlayState* play) {
    ObjMine* this = (ObjMine*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void ObjMine_Path_SetupStationary(ObjMine* this) {
    this->actionFunc = ObjMine_Path_Stationary;
}

void ObjMine_Path_Stationary(ObjMine* this, PlayState* play) {
}

void ObjMine_Path_SetupMove(ObjMine* this) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = ObjMine_Path_Move;
}

void ObjMine_Path_Move(ObjMine* this, PlayState* play) {
    Actor* thisx = &this->actor;
    Vec3f nextWaypoint;
    f32 distToWaypoint;
    f32 step;
    f32 target;
    s32 bgId; // not used

    // thisx->velocity is temporarily set to the vector difference to the next waypoint.
    Math_Vec3s_ToVec3f(&nextWaypoint, &this->waypoints[this->waypointIndex + 1]);
    Math_Vec3f_Diff(&nextWaypoint, &thisx->world.pos, &thisx->velocity);
    distToWaypoint = Math3D_Vec3fMagnitude(&thisx->velocity);

    // Mine slows to 2.0f speed when 8 frames away from the next waypoint
    if ((distToWaypoint < (this->pathSpeed * 8.0f)) && (this->pathSpeed > 2.0f)) {
        target = 2.0f + ((this->pathSpeed - 2.0f) * 0.1f);
        step = this->pathSpeed * 0.03f;
    } else {
        target = this->pathSpeed;
        step = this->pathSpeed * 0.16f;
    }
    Math_StepToF(&thisx->speed, target, step);

    // Checks if mine will reach the waypoint next frame
    if ((thisx->speed + 0.05f) < distToWaypoint) {
        // Rescales thisx->velocity to be equal in magnitude to speed
        Math_Vec3f_Scale(&thisx->velocity, thisx->speed / distToWaypoint);
        thisx->world.pos.x += thisx->velocity.x;
        thisx->world.pos.y += thisx->velocity.y;
        thisx->world.pos.z += thisx->velocity.z;
    } else {
        thisx->speed *= 0.4f;
        this->waypointIndex++;
        if (this->waypointIndex >= this->waypointCount) {
            this->waypointIndex = 0;
        }
        ObjMine_Path_MoveToWaypoint(this, this->waypointIndex);
    }
    thisx->floorHeight = BgCheck_EntityRaycastFloor5(&play->colCtx, &thisx->floorPoly, &bgId, thisx, &thisx->world.pos);
    if (thisx->flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
        Vec3f rotAxis;
        Vec3f yhatCrossV;
        MtxF rotMtxF;

        // Makes mines appear to roll while traversing the path
        Math3D_Vec3f_Cross(&sStandardBasis.y, &thisx->velocity, &yhatCrossV);
        if (ObjMine_GetUnitVec3f(&yhatCrossV, &rotAxis)) {
            Matrix_RotateAxisF(thisx->speed / PATH_RADIUS, &rotAxis, MTXMODE_NEW);
            Matrix_RotateYS(thisx->shape.rot.y, MTXMODE_APPLY);
            Matrix_RotateXS(thisx->shape.rot.x, MTXMODE_APPLY);
            Matrix_RotateZS(thisx->shape.rot.z, MTXMODE_APPLY);
            Matrix_Get(&rotMtxF);
            Matrix_MtxFToYXZRot(&rotMtxF, &thisx->shape.rot, false);
        }
    }
}

void ObjMine_SetupExplode(ObjMine* this) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.draw = ObjMine_DrawExplosion;
    this->actor.shape.shadowDraw = NULL;
    this->actor.scale.x = 0.02f;
    this->actor.scale.y = 0.02f;
    this->actor.scale.z = 0.02f;
    this->actionFunc = ObjMine_Explode;
}

void ObjMine_Explode(ObjMine* this, PlayState* play) {
    this->actor.scale.x *= 1.8f;
    if (this->actor.scale.x > (0.02f * 1.5f * 5.832f)) { // 5.832 = 1.8^3
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.scale.y = this->actor.scale.x;
    this->actor.scale.z = this->actor.scale.x;
}

void ObjMine_Air_SetupChained(ObjMine* this) {
    this->actionFunc = ObjMine_Air_Chained;
}

void ObjMine_Air_Chained(ObjMine* this, PlayState* play) {
    s32 pad; // Could be recast to thisx as in ObjMine_Path_Move
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);
    ObjMineAirChain* airChain = &this->chain.air;
    ObjMineAirLink* airLink;
    s32 i;
    f32 xAccel;
    f32 zAccel;
    f32 spin;
    s16 twistDiff;

    Math_Vec3f_Copy(&airChain->translation, &airChain->basis.y);

    // Explodes if collision with Player or another mine is detected.
    if (ObjMine_AirWater_CheckOC(this)) {
        ObjMine_AirWater_SpawnBomb(this, play);
        ObjMine_AirWater_Noop(this);
        ObjMine_SetupExplode(this);
        return;
    }

    // Calculates initial knockback and torque from hit scaled to chain length
    if (this->collider.base.acFlags & AC_HIT) {
        s16 torqueAngle;
        f32 torque;
        f32 torqueDiff;

        this->collider.base.acFlags &= ~AC_HIT;
        airChain->knockback = AIR_KNOCKBACK;
        ObjMine_Air_CheckAC(this, &airChain->knockbackAngle, &torqueAngle);
        torque = Math_SinS(torqueAngle) * 0x96;
        torqueDiff = -(torque / linkCount);

        for (i = 0, airLink = airChain->links; i < linkCount; i++, airLink++) {
            airLink->spin += (s16)torque;
            airLink->spin = CLAMP(airLink->spin, -0x320, 0x320);
            torque -= torqueDiff;
        }
    }

    // Applies knockback force. Knockback applies over two frames, with the second at half strength
    if (airChain->knockback > 0.0001f) {
        xAccel = Math_SinS(airChain->knockbackAngle) * airChain->knockback;
        zAccel = Math_CosS(airChain->knockbackAngle) * airChain->knockback;
        airChain->velocity.x += xAccel;
        airChain->velocity.z += zAccel;
        Math_StepToF(&airChain->knockback, 0.0f, AIR_KNOCKBACK * 0.5f);
    }

    // Applies chain sway, choosing a new sway randomly about every 32 frames
    if ((Rand_Next() >> (0x20 - 5)) == 0) {
        airChain->swaySize = Rand_ZeroOne() * airChain->swayMax;
        airChain->swayPhase = Rand_Next() >> (0x20 - 16);
    }
    xAccel = Math_SinS(airChain->swayPhase) * airChain->swaySize;
    zAccel = Math_CosS(airChain->swayPhase) * airChain->swaySize;
    airChain->velocity.x += xAccel;
    airChain->velocity.z += zAccel;

    // Applies the restoring force of gravity and chain tension using small-angle approximation
    airChain->velocity.x += airChain->displacement.x * airChain->restore;
    airChain->velocity.z += airChain->displacement.z * airChain->restore;

    // Applies linear drag
    airChain->velocity.x *= airChain->drag;
    airChain->velocity.z *= airChain->drag;

    // Updates scaled position and does a safety clamp to 5.0f.
    airChain->displacement.x += airChain->velocity.x;
    airChain->displacement.z += airChain->velocity.z;

    airChain->displacement.x = CLAMP(airChain->displacement.x, -5.0f, 5.0f);
    airChain->displacement.z = CLAMP(airChain->displacement.z, -5.0f, 5.0f);

    ObjMine_Air_SetBasis(this);
    ObjMine_Air_SetWorld(this);

    // Checks for wall collisions if sufficiently far from home. If collision detected, bounce off the wall at half
    // speed. If speed is close to zero when hitting wall, weakly eject it instead.
    if (airChain->wallCheckDistSq > -1e-6f) {
        if (airChain->wallCheckDistSq <= Math3D_Dist2DSq(this->actor.world.pos.x, this->actor.world.pos.z,
                                                         this->actor.home.pos.x, this->actor.home.pos.z)) {

            Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, AIR_RADIUS, 0.0f, UPDBGCHECKINFO_FLAG_1);

            if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (this->actor.wallPoly != NULL)) {
                Vec3f xzDir;
                Vec3f reflectedDir;
                Vec3f wallNormal;
                Vec3f xzVel;
                f32 xzSpeed;
                f32 invNorm; // not used

                xzVel.x = airChain->velocity.x;
                xzVel.y = 0.0f;
                xzVel.z = airChain->velocity.z;

                if (ObjMine_GetUnitVec3fNorm(&xzVel, &xzDir, &xzSpeed, &invNorm)) {
                    wallNormal.x = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
                    wallNormal.y = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.y);
                    wallNormal.z = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);

                    Math3D_Vec3fReflect(&xzDir, &wallNormal, &reflectedDir);

                    xzSpeed /= 2.0f;
                    airChain->velocity.x = reflectedDir.x * xzSpeed;
                    airChain->velocity.z = reflectedDir.z * xzSpeed;
                } else {
                    airChain->velocity.x *= -0.1f;
                    airChain->velocity.z *= -0.1f;
                }
                ObjMine_Air_SetChainXZ(this);
                ObjMine_Air_SetBasis(this);
                ObjMine_Air_SetWorld(this);
            }
        }
    }

    // Applies three torques to individual chain links:
    //  - restoring torque towards the default twist of 0x4000
    //  - random torque for variance
    //  - linear drag torque
    for (i = 0, airLink = airChain->links; i < linkCount; i++, airLink++) {
        twistDiff = airLink->twist - 0x4000;
        spin = airLink->spin + (twistDiff * -0.05f) + (30.0f * Rand_ZeroOne() - 15.0f);
        spin *= 0.995f;
        airLink->spin = spin;
        airLink->twist += airLink->spin;
    }

    func_800B4AEC(play, &this->actor, 0.0f);
    ObjMine_UpdateCollider(this);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void ObjMine_Air_SetupStationary(ObjMine* this) {
    this->actionFunc = ObjMine_Air_Stationary;
}

void ObjMine_Air_Stationary(ObjMine* this, PlayState* play) {
    // Explodes if collision with Player or another mine is detected.
    if (ObjMine_AirWater_CheckOC(this)) {
        ObjMine_AirWater_SpawnBomb(this, play);
        ObjMine_AirWater_Noop(this);
        ObjMine_SetupExplode(this);
        return;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void ObjMine_Water_SetupChained(ObjMine* this) {
    this->actionFunc = ObjMine_Water_Chained;
}

void ObjMine_Water_Chained(ObjMine* this, PlayState* play) {
    ObjMineWaterChain* waterChain = &this->chain.water;

    // Explodes if collision with Player or another mine is detected.
    if (ObjMine_AirWater_CheckOC(this)) {
        ObjMine_AirWater_SpawnBomb(this, play);
        ObjMine_AirWater_Noop(this);
        ObjMine_SetupExplode(this);
        return;
    }

    // Calculates knockback from AC hits
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        ObjMine_Water_CheckAC(this, &waterChain->knockbackDir);
        waterChain->knockback = WATER_KNOCKBACK;
        Math_Vec3f_Scale(&waterChain->knockbackDir, WATER_KNOCKBACK);
    }

    // Reduces knockback each frame. Effectively makes knockback 60% on the first frame and 20% on the second.
    if (waterChain->knockback > 0.0001f) {
        waterChain->knockback = Math_Vec3f_StepTo(&waterChain->knockbackDir, &gZeroVec3f, WATER_KNOCKBACK * 0.4f);
    }

    waterChain->drag = 0.9f;
    waterChain->restoreXZ = -0.0002f;
    waterChain->restoreY = -0.0002f;
    waterChain->swayMax = 0.003f;

    // Chooses a new sway randomly about every 32 frames.
    if (Rand_Next() >> (0x20 - 5) == 0) {
        s16 randAngle = Rand_Next() >> (0x20 - 16);

        waterChain->swayXZ = Math_SinS(randAngle) * 1.8f * waterChain->swayMax;
        waterChain->swayY = Math_CosS(randAngle) * 0.2f * waterChain->swayMax;
        waterChain->swayPhaseVel = Rand_Next() >> (0x20 - 13);
    }

    ObjMine_Water_UpdateChain(this, play);
    ObjMine_Water_SetWorld(this);
    ObjMine_UpdateCollider(this);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void ObjMine_Water_SetupStationary(ObjMine* this) {
    this->actionFunc = ObjMine_Water_Stationary;
}

void ObjMine_Water_Stationary(ObjMine* this, PlayState* play) {
    // Explodes if collision with Player or another mine is detected.
    if (ObjMine_AirWater_CheckOC(this)) {
        ObjMine_AirWater_SpawnBomb(this, play);
        ObjMine_AirWater_Noop(this);
        ObjMine_SetupExplode(this);
        return;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void ObjMine_Path_Update(Actor* thisx, PlayState* play) {
    s32 pad; // Can be playstate recast.
    ObjMine* this = (ObjMine*)thisx;

    if ((this->collider.base.ocFlags2 & OC2_HIT_PLAYER) || (this->collider.base.acFlags & AC_HIT)) {
        ObjMine_Path_SpawnBomb(this, play);
        ObjMine_SetupExplode(this);
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        this->collider.base.ocFlags1 &= ~OC1_HIT;
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        if ((this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) && (this->actionFunc != ObjMine_Explode)) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void ObjMine_AirWater_Update(Actor* thisx, PlayState* play) {
    ObjMine* this = (ObjMine*)thisx;

    this->actionFunc(this, play);
}

void ObjMine_Path_Draw(Actor* thisx, PlayState* play) {
    s32 pad; // Can be playstate recast
    ObjMine* this = (ObjMine*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actionFunc == ObjMine_Path_Move) {
        Collider_UpdateSpheres(0, &this->collider);
    }

    func_800B8050(&this->actor, play, true);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPDisplayList(POLY_OPA_DISP++, object_ny_DL_002068);
    gSPDisplayList(POLY_OPA_DISP++, object_ny_DL_002188);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjMine_DrawExplosion(Actor* thisx, PlayState* play) {
    s32 pad; // This could be thisx recast or playstate recast. Not enough room on stack for both.

    OPEN_DISPS(play->state.gfxCtx);

    func_800B8118(thisx, play, true);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 75);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_ny_DL_002068);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjMine_Air_Draw(Actor* thisx, PlayState* play) {
    s32 pad; // Can be playstate recast
    ObjMine* this = (ObjMine*)thisx;
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);
    ObjMineAirChain* airChain = &this->chain.air;
    ObjMineAirLink* airLink;
    s32 i;
    Vec3f linkPos;
    Vec3f linkOffset;
    Gfx* gfx;

    func_800B8050(&this->actor, play, true);

    OPEN_DISPS(play->state.gfxCtx);
    gfx = POLY_OPA_DISP;

    gSPDisplayList(gfx++, &gSetupDLs[SETUPDL_25]);
    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
    gSPDisplayList(gfx++, object_ny_DL_000030);

    ObjMine_SetMatrixRotation(&airChain->basis);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    if (linkCount != 0) {
        // Sets pivot point to be half a chain link length below home
        Math_Vec3f_ScaleAndStore(&airChain->basis.y, -(LINK_SIZE / 2.0f), &linkOffset);
        linkPos.x = this->actor.home.pos.x - linkOffset.x;
        linkPos.y = this->actor.home.pos.y - (LINK_SIZE / 2.0f) - linkOffset.y;
        linkPos.z = this->actor.home.pos.z - linkOffset.z;
        Math_Vec3f_ScaleAndStore(&airChain->basis.y, -LINK_SIZE, &linkOffset);

        for (i = 0, airLink = airChain->links; i < linkCount; i++, airLink++) {
            Matrix_RotateYS(airLink->twist, MTXMODE_APPLY);
            linkPos.x += linkOffset.x;
            linkPos.y += linkOffset.y;
            linkPos.z += linkOffset.z;
            ObjMine_SetMatrixTranslation(&linkPos);

            MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
            gSPDisplayList(gfx++, object_ny_DL_000030);
        }
    }

    Matrix_RotateXS(0x2000, MTXMODE_APPLY);
    ObjMine_SetMatrixTranslation(&this->actor.world.pos);

    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
    gDPPipeSync(gfx++);
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
    gDPSetEnvColor(gfx++, 0, 0, 0, 255);
    gSPDisplayList(gfx++, object_ny_DL_002068);
    gSPDisplayList(gfx++, object_ny_DL_002188);

    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjMine_Water_Draw(Actor* thisx, PlayState* play) {
    s32 pad; // Can be playstate recast
    ObjMine* this = (ObjMine*)thisx;
    s32 linkCount = OBJMINE_GET_LINK_COUNT(&this->actor);
    ObjMineWaterChain* waterChain = &this->chain.water;
    ObjMineWaterLink* waterLink;
    s32 i;
    Gfx* gfx;

    func_800B8050(&this->actor, play, true);

    OPEN_DISPS(play->state.gfxCtx);
    gfx = POLY_OPA_DISP;

    gSPDisplayList(gfx++, &gSetupDLs[SETUPDL_25]);
    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
    gSPDisplayList(gfx++, object_ny_DL_000030);

    for (i = 0, waterLink = waterChain->links; i < linkCount; i++, waterLink++) {
        ObjMine_SetMatrixRotation(&waterLink->basis);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        // Consecutive chain links are offset 90 degrees.
        if ((i % 2) == 0) {
            Matrix_RotateYS(0x4000, MTXMODE_APPLY);
        }
        ObjMine_SetMatrixTranslation(&waterLink->pos);

        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
        gSPDisplayList(gfx++, object_ny_DL_000030);
    }

    Matrix_RotateXS(0x2000, MTXMODE_APPLY);
    ObjMine_SetMatrixTranslation(&this->actor.world.pos);

    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
    gDPPipeSync(gfx++);
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
    gDPSetEnvColor(gfx++, 0, 0, 0, 255);
    gSPDisplayList(gfx++, object_ny_DL_002068);
    gSPDisplayList(gfx++, object_ny_DL_002188);

    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}

#include "ultra64.h"
#include "global.h"
#include "vt.h"

#include <string.h>

#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

#include "soh/frame_interpolation.h"

s16 Camera_ChangeSettingFlags(Camera* camera, s16 setting, s16 flags);
s32 Camera_ChangeModeFlags(Camera* camera, s16 mode, u8 flags);
s32 Camera_QRegInit(void);
s32 Camera_UpdateWater(Camera* camera);

#define RELOAD_PARAMS \
    (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14 || R_RELOAD_CAM_PARAMS)

#define PCT(x) ((x)*0.01f)
#define NEXTSETTING ((values++)->val)
#define NEXTPCT PCT(NEXTSETTING)

#define BGCAM_POS(v) ((v)[0])
#define BGCAM_ROT(v) ((v)[1])
#define BGCAM_FOV(v) ((v)[2].x)
#define BGCAM_JFIFID(v) ((v)[2].y)

#define FLG_ADJSLOPE (1 << 0)
#define FLG_OFFGROUND (1 << 7)

#define DISTORTION_HOT_ROOM (1 << 0)
#define DISTORTION_UNDERWATER_WEAK (1 << 1)
#define DISTORTION_UNDERWATER_MEDIUM (1 << 2)
#define DISTORTION_UNDERWATER_STRONG (1 << 3)
#define DISTORTION_UNDERWATER_FISHING (1 << 4)

#include "z_camera_data.inc"

/*===============================================================*/

/**
 * Interpolates along a curve between 0 and 1 with a period of
 * -a <= p <= a at time `b`
 */
f32 Camera_InterpolateCurve(f32 a, f32 b) {
    f32 ret;
    f32 absB;
    f32 t = 0.4f;
    f32 t2;
    f32 t3;
    f32 t4;

    absB = fabsf(b);
    if (a < absB) {
        ret = 1.0f;
    } else {
        t2 = 1.0f - t;
        if ((a * t2) > absB) {
            t3 = SQ(b) * (1.0f - t);
            t4 = SQ(a * t2);
            ret = t3 / t4;
        } else {
            t3 = SQ(a - absB) * t;
            t4 = SQ(0.4f * a);
            ret = 1.0f - (t3 / t4);
        }
    }
    return ret;
}

/*
 * Performs linear interpoloation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, The result is rounded up to `target`
 */
f32 Camera_LERPCeilF(f32 target, f32 cur, f32 stepScale, f32 minDiff) {
    f32 diff = target - cur;
    f32 step;
    f32 ret;

    if (fabsf(diff) >= minDiff) {
        step = diff * stepScale;
        ret = cur + step;
    } else {
        ret = target;
    }

    return ret;
}

/*
 * Performs linear interpoloation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, The result is rounded down to `cur`
 */
f32 Camera_LERPFloorF(f32 target, f32 cur, f32 stepScale, f32 minDiff) {
    f32 diff = target - cur;
    f32 step;
    f32 ret;

    if (fabsf(diff) >= minDiff) {
        step = diff * stepScale;
        ret = cur + step;
    } else {
        ret = cur;
    }

    return ret;
}

/*
 * Performs linear interpoloation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, The result is rounded up to `target`
 */
s16 Camera_LERPCeilS(s16 target, s16 cur, f32 stepScale, s16 minDiff) {
    s16 diff = target - cur;
    s16 step;
    s32 ret;

    if (ABS(diff) >= minDiff) {
        step = diff * stepScale + 0.5f;
        ret = cur + step;
    } else {
        ret = target;
    }

    return ret;
}

/*
 * Performs linear interpoloation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, The result is rounded down to `cur`
 */
s16 Camera_LERPFloorS(s16 target, s16 cur, f32 stepScale, s16 minDiff) {
    s16 diff = target - cur;
    s16 step;
    s32 ret;

    if (ABS(diff) >= minDiff) {
        step = diff * stepScale + 0.5f;
        ret = cur + step;
    } else {
        ret = cur;
    }

    return ret;
}

/*
 * Performs linear interpoloation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, The result is rounded up to `target`
 */
void Camera_LERPCeilVec3f(Vec3f* target, Vec3f* cur, f32 yStepScale, f32 xzStepScale, f32 minDiff) {
    cur->x = Camera_LERPCeilF(target->x, cur->x, xzStepScale, minDiff);
    cur->y = Camera_LERPCeilF(target->y, cur->y, yStepScale, minDiff);
    cur->z = Camera_LERPCeilF(target->z, cur->z, xzStepScale, minDiff);
}

void func_80043ABC(Camera* camera) {
    camera->yawUpdateRateInv = 100.0f;
    camera->pitchUpdateRateInv = R_CAM_DEFA_PHI_UPDRATE;
    camera->rUpdateRateInv = OREG(6);
    camera->xzOffsetUpdateRate = PCT(OREG(2));
    camera->yOffsetUpdateRate = PCT(OREG(3));
    camera->fovUpdateRate = PCT(OREG(4));
}

void func_80043B60(Camera* camera) {
    camera->rUpdateRateInv = OREG(27);
    camera->yawUpdateRateInv = OREG(27);
    camera->pitchUpdateRateInv = OREG(27);
    camera->xzOffsetUpdateRate = 0.001f;
    camera->yOffsetUpdateRate = 0.001f;
    camera->fovUpdateRate = 0.001f;
}

Vec3f* Camera_Vec3sToVec3f(Vec3f* dest, Vec3s* src) {
    Vec3f copy;

    copy.x = src->x;
    copy.y = src->y;
    copy.z = src->z;

    *dest = copy;
    return dest;
}

Vec3f* Camera_Vec3fVecSphGeoAdd(Vec3f* dest, Vec3f* a, VecSph* b) {
    Vec3f copy;
    Vec3f vecB;
    OLib_VecSphGeoToVec3f(&vecB, b);

    copy.x = a->x + vecB.x;
    copy.y = a->y + vecB.y;
    copy.z = a->z + vecB.z;

    *dest = copy;
    return dest;
}

Vec3f* Camera_Vec3fTranslateByUnitVector(Vec3f* dest, Vec3f* src, Vec3f* unitVector, f32 uvScale) {
    Vec3f copy;

    copy.x = src->x + (unitVector->x * uvScale);
    copy.y = src->y + (unitVector->y * uvScale);
    copy.z = src->z + (unitVector->z * uvScale);

    *dest = copy;
    return dest;
}

/**
 * Detects the collision poly between `from` and `to`, places collision info in `to`
 */
s32 Camera_BGCheckInfo(Camera* camera, Vec3f* from, CamColChk* to) {
    CollisionContext* colCtx = &camera->play->colCtx;
    Vec3f toNewPos;
    Vec3f toPoint;
    Vec3f fromToNorm;
    f32 floorPolyY;
    CollisionPoly* floorPoly;
    s32 floorBgId = 0;
    VecSph fromToOffset;

    OLib_Vec3fDiffToVecSphGeo(&fromToOffset, from, &to->pos);
    fromToOffset.r += 8.0f;
    Camera_Vec3fVecSphGeoAdd(&toPoint, from, &fromToOffset);

    if (!BgCheck_CameraLineTest1(colCtx, from, &toPoint, &toNewPos, &to->poly, 1, 1, 1, -1, &to->bgId)) {
        // no poly in path.
        OLib_Vec3fDistNormalize(&fromToNorm, from, &to->pos);

        to->norm.x = -fromToNorm.x;
        to->norm.y = -fromToNorm.y;
        to->norm.z = -fromToNorm.z;

        toNewPos = to->pos;
        toNewPos.y += 5.0f;
        floorPolyY = BgCheck_CameraRaycastFloor2(colCtx, &floorPoly, &floorBgId, &toNewPos);

        if ((to->pos.y - floorPolyY) > 5.0f) {
            // if the y distance from the check point to the floor is more than 5 units
            // the point is not colliding with any collision.
            to->pos.x += to->norm.x;
            to->pos.y += to->norm.y;
            to->pos.z += to->norm.z;
            return 0;
        }

        to->poly = floorPoly;
        toNewPos.y = floorPolyY + 1.0f;
        to->bgId = floorBgId;
    }

    to->norm.x = COLPOLY_GET_NORMAL(to->poly->normal.x);
    to->norm.y = COLPOLY_GET_NORMAL(to->poly->normal.y);
    to->norm.z = COLPOLY_GET_NORMAL(to->poly->normal.z);
    to->pos.x = to->norm.x + toNewPos.x;
    to->pos.y = to->norm.y + toNewPos.y;
    to->pos.z = to->norm.z + toNewPos.z;

    return floorBgId + 1;
}

/**
 * Detects if there is collision between `from` and `to`
 */
s32 Camera_BGCheck(Camera* camera, Vec3f* from, Vec3f* to) {
    CamColChk toCol;
    s32 bgId;

    toCol.pos = *to;
    bgId = Camera_BGCheckInfo(camera, from, &toCol);
    *to = toCol.pos;
    return bgId;
}

s32 func_80043F94(Camera* camera, Vec3f* from, CamColChk* to) {
    CollisionContext* colCtx = &camera->play->colCtx;
    Vec3f toNewPos;
    Vec3f toPos;
    Vec3f fromToNorm;
    Vec3f playerFloorNormF;
    f32 floorY;
    CollisionPoly* floorPoly;
    s32 bgId;
    VecSph fromToGeo;

    OLib_Vec3fDiffToVecSphGeo(&fromToGeo, from, &to->pos);
    fromToGeo.r += 8.0f;
    Camera_Vec3fVecSphGeoAdd(&toPos, from, &fromToGeo);
    if (!BgCheck_CameraLineTest1(colCtx, from, &toPos, &toNewPos, &to->poly, 1, 1, 1, -1, &to->bgId)) {
        OLib_Vec3fDistNormalize(&fromToNorm, from, &to->pos);
        to->norm.x = -fromToNorm.x;
        to->norm.y = -fromToNorm.y;
        to->norm.z = -fromToNorm.z;
        toNewPos = to->pos;
        toNewPos.y += 5.0f;
        floorY = BgCheck_CameraRaycastFloor2(colCtx, &floorPoly, &bgId, &toNewPos);
        if ((to->pos.y - floorY) > 5.0f) {
            // to is not on the ground or below it.
            to->pos.x += to->norm.x;
            to->pos.y += to->norm.y;
            to->pos.z += to->norm.z;
            return 0;
        }
        // to is touching the ground, move it up 1 unit.
        to->poly = floorPoly;
        toNewPos.y = floorY + 1.0f;
        to->bgId = bgId;
    }
    to->norm.x = COLPOLY_GET_NORMAL(to->poly->normal.x);
    to->norm.y = COLPOLY_GET_NORMAL(to->poly->normal.y);
    to->norm.z = COLPOLY_GET_NORMAL(to->poly->normal.z);
    if ((to->norm.y > 0.5f) || (to->norm.y < -0.8f)) {
        to->pos.x = to->norm.x + toNewPos.x;
        to->pos.y = to->norm.y + toNewPos.y;
        to->pos.z = to->norm.z + toNewPos.z;
    } else if (playerFloorPoly != NULL) {
        playerFloorNormF.x = COLPOLY_GET_NORMAL(playerFloorPoly->normal.x);
        playerFloorNormF.y = COLPOLY_GET_NORMAL(playerFloorPoly->normal.y);
        playerFloorNormF.z = COLPOLY_GET_NORMAL(playerFloorPoly->normal.z);
        if (Math3D_LineSegVsPlane(playerFloorNormF.x, playerFloorNormF.y, playerFloorNormF.z, playerFloorPoly->dist,
                                  from, &toPos, &toNewPos, 1)) {
            // line is from->to is touching the poly the player is on.
            to->norm = playerFloorNormF;
            to->poly = playerFloorPoly;
            to->bgId = camera->bgCheckId;
            to->pos.x = to->norm.x + toNewPos.x;
            to->pos.y = to->norm.y + toNewPos.y;
            to->pos.z = to->norm.z + toNewPos.z;
        } else {
            OLib_Vec3fDistNormalize(&fromToNorm, from, &to->pos);
            to->norm.x = -fromToNorm.x;
            to->norm.y = -fromToNorm.y;
            to->norm.z = -fromToNorm.z;
            to->pos.x += to->norm.x;
            to->pos.y += to->norm.y;
            to->pos.z += to->norm.z;
            return 0;
        }
    }
    return 1;
}

void func_80044340(Camera* camera, Vec3f* arg1, Vec3f* arg2) {
    CamColChk sp20;
    Vec3s unused;

    sp20.pos = *arg2;
    func_80043F94(camera, arg1, &sp20);
    *arg2 = sp20.pos;
}

/**
 * Checks if `from` to `to` is looking from the outside of a poly towards the front
 */
s32 Camera_CheckOOB(Camera* camera, Vec3f* from, Vec3f* to) {
    s32 pad;
    Vec3f intersect;
    s32 pad2;
    s32 bgId;
    CollisionPoly* poly;
    CollisionContext* colCtx = &camera->play->colCtx;

    poly = NULL;
    if (BgCheck_CameraLineTest1(colCtx, from, to, &intersect, &poly, 1, 1, 1, 0, &bgId) &&
        (CollisionPoly_GetPointDistanceFromPlane(poly, from) < 0.0f)) {
        // if there is a poly between `from` and `to` and the `from` is behind the poly.
        return true;
    }

    return false;
}

/**
 * Gets the floor position underneath `chkPos`, and returns the normal of the floor to `floorNorm`,
 * and bgId to `bgId`.  If no floor is found, then the normal is a flat surface pointing upwards.
 */
f32 Camera_GetFloorYNorm(Camera* camera, Vec3f* floorNorm, Vec3f* chkPos, s32* bgId) {
    s32 pad;
    CollisionPoly* floorPoly;
    f32 floorY = BgCheck_EntityRaycastFloor3(&camera->play->colCtx, &floorPoly, bgId, chkPos);

    if (floorY == BGCHECK_Y_MIN) {
        // no floor
        floorNorm->x = 0.0f;
        floorNorm->y = 1.0f;
        floorNorm->z = 0.0f;
    } else {
        floorNorm->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        floorNorm->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        floorNorm->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
    }

    return floorY;
}

/**
 * Gets the position of the floor from `pos`
 */
f32 Camera_GetFloorY(Camera* camera, Vec3f* pos) {
    Vec3f posCheck;
    Vec3f floorNorm;
    s32 bgId;

    posCheck = *pos;
    posCheck.y += 80.0f;

    return Camera_GetFloorYNorm(camera, &floorNorm, &posCheck, &bgId);
}

/**
 * Gets the position of the floor from `pos`, and if the floor is considered not solid,
 * it checks the next floor below that up to 3 times.  Returns the normal of the floor into `norm`
 */
f32 Camera_GetFloorYLayer(Camera* camera, Vec3f* norm, Vec3f* pos, s32* bgId) {
    CollisionPoly* floorPoly;
    CollisionContext* colCtx = &camera->play->colCtx;
    f32 floorY;
    s32 i;

    for (i = 3; i > 0; i--) {
        floorY = BgCheck_CameraRaycastFloor2(colCtx, &floorPoly, bgId, pos);
        if (floorY == BGCHECK_Y_MIN ||
            (camera->playerGroundY < floorY && !(COLPOLY_GET_NORMAL(floorPoly->normal.y) > 0.5f))) {
            // no floor, or player is below the floor and floor is not considered steep
            norm->x = 0.0f;
            norm->y = 1.0f;
            norm->z = 0.0f;
            floorY = BGCHECK_Y_MIN;
            break;
        } else if (func_80041D4C(colCtx, floorPoly, *bgId) == 1) {
            // floor is not solid, check below that floor.
            pos->y = floorY - 10.0f;
            continue;
        } else {
            norm->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
            norm->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
            norm->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
            break;
        }
    }
    if (i == 0) {
        osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: foward check: too many layer!\n" VT_RST);
    }
    return floorY;
}

/**
 * Returns the CameraSettingType of the camera at index `camDataIdx`
 */
s16 Camera_GetCamDataSetting(Camera* camera, s32 camDataIdx) {
    return func_80041A4C(&camera->play->colCtx, camDataIdx, BGCHECK_SCENE);
}

/**
 * Returns the scene camera info for the current camera data index
 */
Vec3s* Camera_GetCamBGData(Camera* camera) {
    return func_80041C10(&camera->play->colCtx, camera->camDataIdx, BGCHECK_SCENE);
}

/**
 * Gets the scene's camera index for the poly `poly`, returns -1 if
 * there is no camera data for that poly.
 */
s32 Camera_GetDataIdxForPoly(Camera* camera, s32* bgId, CollisionPoly* poly) {
    s32 camDataIdx;
    PosRot playerPosRot;
    s32 ret;

    Actor_GetWorldPosShapeRot(&playerPosRot, &camera->player->actor); // unused.
    camDataIdx = SurfaceType_GetCamDataIndex(&camera->play->colCtx, poly, *bgId);

    if (func_80041A4C(&camera->play->colCtx, camDataIdx, *bgId) == CAM_SET_NONE) {
        ret = -1;
    } else {
        ret = camDataIdx;
    }
    return ret;
}

/**
 * Returns the scene camera info for the floor under the player.
 * If there is no floor then there is no camera data, returns the number of
 * pieces of data there are in `dataCnt`
 */
Vec3s* Camera_GetCamBgDataUnderPlayer(Camera* camera, u16* dataCnt) {
    CollisionPoly* floorPoly;
    s32 pad;
    s32 bgId;
    PosRot playerPosShape;

    Actor_GetWorldPosShapeRot(&playerPosShape, &camera->player->actor);
    playerPosShape.pos.y += Player_GetHeight(camera->player);
    if (BgCheck_EntityRaycastFloor3(&camera->play->colCtx, &floorPoly, &bgId, &playerPosShape.pos) ==
        BGCHECK_Y_MIN) {
        // no floor
        return NULL;
    }
    *dataCnt = SurfaceType_GetNumCameras(&camera->play->colCtx, floorPoly, bgId);
    return SurfaceType_GetCamPosData(&camera->play->colCtx, floorPoly, bgId);
}

/**
 * Gets the Camera information for the water box the player is in.
 * Returns -1 if the player is not in a water box, or does not have a swimming state.
 * Returns -2 if there is no camera index for the water box.
 * Returns the camera data index otherwise.
 */
s32 Camera_GetWaterBoxDataIdx(Camera* camera, f32* waterY) {
    PosRot playerPosShape;
    WaterBox* waterBox;
    s32 ret;

    Actor_GetWorldPosShapeRot(&playerPosShape, &camera->player->actor);
    *waterY = playerPosShape.pos.y;

    if (!WaterBox_GetSurface1(camera->play, &camera->play->colCtx, playerPosShape.pos.x, playerPosShape.pos.z,
                              waterY, &waterBox)) {
        // player's position is not in a water box.
        *waterY = BGCHECK_Y_MIN;
        return -1;
    }

    if (!(camera->player->stateFlags1 & 0x8000000)) {
        // player is not swimming
        *waterY = BGCHECK_Y_MIN;
        return -1;
    }

    ret = WaterBox_GetCamDataIndex(&camera->play->colCtx, waterBox);
    if ((ret <= 0) || (WaterBox_GetCameraSType(&camera->play->colCtx, waterBox) <= 0)) {
        // no camera data idx, or no CameraSettingType
        return -2;
    }

    return ret;
}

/**
 * Checks if `chkPos` is inside a waterbox. If there is no water box below `chkPos`
 * or if `chkPos` is above the water surface, return BGCHECK_Y_MIN, output
 * environment properites to `envProp` if `chkPos` is inside the waterbox.
 */
f32 Camera_GetWaterSurface(Camera* camera, Vec3f* chkPos, s32* envProp) {
    PosRot playerPosRot;
    f32 waterY;
    WaterBox* waterBox;

    Actor_GetWorldPosShapeRot(&playerPosRot, &camera->player->actor);
    waterY = playerPosRot.pos.y;

    if (!WaterBox_GetSurface1(camera->play, &camera->play->colCtx, chkPos->x, chkPos->z, &waterY,
                              &waterBox)) {
        // chkPos is not within the x/z boundaries of a water box.
        return BGCHECK_Y_MIN;
    }

    if (waterY < chkPos->y) {
        // the water's y position is below the check position
        // meaning the position is NOT in the water.
        return BGCHECK_Y_MIN;
    }

    *envProp = WaterBox_GetLightSettingIndex(&camera->play->colCtx, waterBox);
    return waterY;
}

/**
 * Calculates the angle between points `from` and `to`
 */
s16 Camera_XZAngle(Vec3f* to, Vec3f* from) {
    return DEGF_TO_BINANG(RADF_TO_DEGF(Math_FAtan2F(from->x - to->x, from->z - to->z)));
}

 f32 D_8015CE50;
 f32 D_8015CE54;
 CamColChk D_8015CE58;
s16 func_80044ADC(Camera* camera, s16 yaw, s16 arg2) {
    Vec3f playerPos;
    Vec3f rotatedPos;
    Vec3f floorNorm;
    f32 temp_f2;
    s16 temp_s0;
    s16 temp_s1;
    f32 phi_f18;
    f32 sinYaw;
    f32 cosYaw;
    s32 bgId;
    f32 sp30;
    f32 sp2C;
    f32 phi_f16;
    f32 playerHeight;

    sinYaw = Math_SinS(yaw);
    cosYaw = Math_CosS(yaw);
    playerHeight = Player_GetHeight(camera->player);
    temp_f2 = PCT(OREG(19)) * playerHeight;
    sp30 = PCT(OREG(17)) * playerHeight;
    sp2C = PCT(OREG(18)) * playerHeight;
    playerPos.x = camera->playerPosRot.pos.x;
    playerPos.y = camera->playerGroundY + temp_f2;
    playerPos.z = camera->playerPosRot.pos.z;
    rotatedPos.x = playerPos.x + (sp30 * sinYaw);
    rotatedPos.y = playerPos.y;
    rotatedPos.z = playerPos.z + (sp30 * cosYaw);
    if (arg2 || (camera->play->state.frames % 2) == 0) {
        D_8015CE58.pos.x = playerPos.x + (sp2C * sinYaw);
        D_8015CE58.pos.y = playerPos.y;
        D_8015CE58.pos.z = playerPos.z + (sp2C * cosYaw);
        Camera_BGCheckInfo(camera, &playerPos, &D_8015CE58);
        if (arg2) {
            D_8015CE50 = D_8015CE54 = camera->playerGroundY;
        }
    } else {
        sp2C = OLib_Vec3fDistXZ(&playerPos, &D_8015CE58.pos);
        D_8015CE58.pos.x += D_8015CE58.norm.x * 5.0f;
        D_8015CE58.pos.y += D_8015CE58.norm.y * 5.0f;
        D_8015CE58.pos.z += D_8015CE58.norm.z * 5.0f;
        if (sp2C < sp30) {
            sp30 = sp2C;
            D_8015CE50 = D_8015CE54 = Camera_GetFloorYLayer(camera, &floorNorm, &D_8015CE58.pos, &bgId);
        } else {
            D_8015CE50 = Camera_GetFloorYLayer(camera, &floorNorm, &rotatedPos, &bgId);
            D_8015CE54 = Camera_GetFloorYLayer(camera, &floorNorm, &D_8015CE58.pos, &bgId);
        }

        if (D_8015CE50 == BGCHECK_Y_MIN) {
            D_8015CE50 = camera->playerGroundY;
        }

        if (D_8015CE54 == BGCHECK_Y_MIN) {
            D_8015CE54 = D_8015CE50;
        }
    }
    phi_f16 = PCT(OREG(20)) * (D_8015CE50 - camera->playerGroundY);
    phi_f18 = (1.0f - PCT(OREG(20))) * (D_8015CE54 - camera->playerGroundY);
    temp_s0 = DEGF_TO_BINANG(RADF_TO_DEGF(Math_FAtan2F(phi_f16, sp30)));
    temp_s1 = DEGF_TO_BINANG(RADF_TO_DEGF(Math_FAtan2F(phi_f18, sp2C)));
    return temp_s0 + temp_s1;
}

Vec3f* Camera_CalcUpFromPitchYawRoll(Vec3f* dest, s16 pitch, s16 yaw, s16 roll) {
    f32 sinPitch;
    f32 cosPitch;
    f32 sinYaw;
    f32 cosYaw;
    f32 sinNegRoll;
    f32 cosNegRoll;
    Vec3f spA4;
    f32 pad;
    f32 sp54;
    f32 sp4C;
    f32 cosPitchCosYawSinRoll;
    f32 negSinPitch;
    f32 temp_f10_2;
    f32 cosPitchcosYaw;
    f32 temp_f14;
    f32 negSinPitchSinYaw;
    f32 negSinPitchCosYaw;
    f32 cosPitchSinYaw;
    f32 temp_f4_2;
    f32 temp_f6;
    f32 temp_f8;
    f32 temp_f8_2;
    f32 temp_f8_3;

    sinPitch = Math_SinS(pitch);
    cosPitch = Math_CosS(pitch);
    sinYaw = Math_SinS(yaw);
    cosYaw = Math_CosS(yaw);
    negSinPitch = -sinPitch;
    sinNegRoll = Math_SinS(-roll);
    cosNegRoll = Math_CosS(-roll);
    negSinPitchSinYaw = negSinPitch * sinYaw;
    temp_f14 = 1.0f - cosNegRoll;
    cosPitchSinYaw = cosPitch * sinYaw;
    sp54 = SQ(cosPitchSinYaw);
    sp4C = (cosPitchSinYaw * sinPitch) * temp_f14;
    cosPitchcosYaw = cosPitch * cosYaw;
    temp_f4_2 = ((1.0f - sp54) * cosNegRoll) + sp54;
    cosPitchCosYawSinRoll = cosPitchcosYaw * sinNegRoll;
    negSinPitchCosYaw = negSinPitch * cosYaw;
    temp_f6 = (cosPitchcosYaw * cosPitchSinYaw) * temp_f14;
    temp_f10_2 = sinPitch * sinNegRoll;
    spA4.x = ((negSinPitchSinYaw * temp_f4_2) + (cosPitch * (sp4C - cosPitchCosYawSinRoll))) +
             (negSinPitchCosYaw * (temp_f6 + temp_f10_2));
    sp54 = SQ(sinPitch);
    temp_f4_2 = (sinPitch * cosPitchcosYaw) * temp_f14;
    temp_f8_3 = cosPitchSinYaw * sinNegRoll;
    temp_f8 = sp4C + cosPitchCosYawSinRoll;
    spA4.y = ((negSinPitchSinYaw * temp_f8) + (cosPitch * (((1.0f - sp54) * cosNegRoll) + sp54))) +
             (negSinPitchCosYaw * (temp_f4_2 - temp_f8_3));
    temp_f8_2 = temp_f6 - temp_f10_2;
    spA4.z = ((negSinPitchSinYaw * temp_f8_2) + (cosPitch * (temp_f4_2 + temp_f8_3))) +
             (negSinPitchCosYaw * (((1.0f - SQ(cosPitchcosYaw)) * cosNegRoll) + SQ(cosPitchcosYaw)));
    *dest = spA4;
    return dest;
}

f32 Camera_ClampLERPScale(Camera* camera, f32 maxLERPScale) {
    f32 ret;

    if (camera->atLERPStepScale < PCT(R_AT_LERP_MIN)) {
        ret = PCT(R_AT_LERP_MIN);
    } else if (camera->atLERPStepScale >= maxLERPScale) {
        ret = maxLERPScale;
    } else {
        ret = PCT(R_AT_LERP_SCALE) * camera->atLERPStepScale;
    }

    return ret;
}

void Camera_CopyDataToRegs(Camera* camera, s16 mode) {
    CameraModeValue* values;
    CameraModeValue* valueP;
    s32 i;

    if (PREG(82)) {
        osSyncPrintf("camera: res: stat (%d/%d/%d)\n", camera->thisIdx, camera->setting, mode);
    }

    values = sCameraSettings[camera->setting].cameraModes[mode].values;

    for (i = 0; i < sCameraSettings[camera->setting].cameraModes[mode].valueCnt; i++) {
        valueP = &values[i];
        PREG(valueP->dataType) = valueP->val;
        if (PREG(82)) {
            osSyncPrintf("camera: res: PREG(%02d) = %d\n", valueP->dataType, valueP->val);
        }
    }
    camera->animState = 0;
}

s32 Camera_CopyPREGToModeValues(Camera* camera) {
    CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
    CameraModeValue* valueP;
    s32 i;

    for (i = 0; i < sCameraSettings[camera->setting].cameraModes[camera->mode].valueCnt; i++) {
        valueP = &values[i];
        valueP->val = R_CAM_DATA(valueP->dataType);
        if (PREG(82)) {
            osSyncPrintf("camera: res: %d = PREG(%02d)\n", valueP->val, valueP->dataType);
        }
    }
    return true;
}

#define SHRINKWIN_MASK (0xF000)
#define SHRINKWINVAL_MASK (0x7000)
#define SHRINKWIN_CURVAL (0x8000)
#define IFACE_ALPHA_MASK (0x0F00)

void Camera_UpdateInterface(s16 flags) {
    s16 interfaceAlpha;

    if ((flags & SHRINKWIN_MASK) != SHRINKWIN_MASK) {
        switch (flags & SHRINKWINVAL_MASK) {
            case 0x1000:
                sCameraShrinkWindowVal = 0x1A;
                break;
            case 0x2000:
                sCameraShrinkWindowVal = 0x1B;
                break;
            case 0x3000:
                sCameraShrinkWindowVal = 0x20;
                break;
            default:
                sCameraShrinkWindowVal = 0;
                break;
        }

        if (flags & SHRINKWIN_CURVAL) {
            ShrinkWindow_SetCurrentVal(sCameraShrinkWindowVal);
        } else {
            ShrinkWindow_SetVal(sCameraShrinkWindowVal);
        }
    }

    if ((flags & IFACE_ALPHA_MASK) != IFACE_ALPHA_MASK) {
        interfaceAlpha = (flags & IFACE_ALPHA_MASK) >> 8;
        if (interfaceAlpha == 0) {
            interfaceAlpha = 0x32;
        }
        if (interfaceAlpha != sCameraInterfaceAlpha) {
            sCameraInterfaceAlpha = interfaceAlpha;
            Interface_ChangeAlpha(sCameraInterfaceAlpha);
        }
    }
}

Vec3f* Camera_BGCheckCorner(Vec3f* dst, Vec3f* linePointA, Vec3f* linePointB, CamColChk* pointAColChk,
                            CamColChk* pointBColChk) {
    Vec3f closestPoint;

    if (!func_800427B4(pointAColChk->poly, pointBColChk->poly, linePointA, linePointB, &closestPoint)) {
        osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: corner check no cross point %x %x\n" VT_RST, pointAColChk,
                     pointBColChk);
        *dst = pointAColChk->pos;
        return dst;
    }

    *dst = closestPoint;
    return dst;
}

/**
 * Checks collision between at and eyeNext, if `checkEye` is set, if there is no collsion between
 * eyeNext->at, then eye->at is also checked.
 * Returns:
 * 0 if no collsion is found between at->eyeNext
 * 2 if the angle between the polys is between 60 degrees and 120 degrees
 * 3 ?
 * 6 if the angle between the polys is greater than 120 degrees
 */
s32 func_80045508(Camera* camera, VecSph* diffSph, CamColChk* eyeChk, CamColChk* atChk, s16 checkEye) {
    Vec3f* at = &camera->at;
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f eyePos;
    s32 atEyeBgId;
    s32 eyeAtBgId;
    s32 ret;
    f32 cosEyeAt;

    eyeChk->pos = camera->eyeNext;

    ret = 0;

    atEyeBgId = Camera_BGCheckInfo(camera, at, eyeChk);
    if (atEyeBgId != 0) {
        // collision found between at->eye
        atChk->pos = camera->at;

        OLib_Vec3fToVecSphGeo(&eyeChk->sphNorm, &eyeChk->norm);

        if (eyeChk->sphNorm.pitch >= 0x2EE1) {
            eyeChk->sphNorm.yaw = diffSph->yaw;
        }

        eyeAtBgId = Camera_BGCheckInfo(camera, eyeNext, atChk);

        if (eyeAtBgId == 0) {
            // no collision from eyeNext->at
            if (checkEye & 1) {

                atChk->pos = *at;
                eyePos = *eye;

                if (Camera_BGCheckInfo(camera, &eyePos, atChk) == 0) {
                    // no collision from eye->at
                    return 3;
                } else if (eyeChk->poly == atChk->poly) {
                    // at->eye and eye->at is the same poly
                    return 3;
                }
            } else {
                return 3;
            }
        } else if (eyeChk->poly == atChk->poly) {
            // at->eyeNext and eyeNext->at is the same poly
            return 3;
        }

        OLib_Vec3fToVecSphGeo(&atChk->sphNorm, &atChk->norm);

        if (atChk->sphNorm.pitch >= 0x2EE1) {
            atChk->sphNorm.yaw = BINANG_ROT180(diffSph->yaw);
        }

        if (atEyeBgId != eyeAtBgId) {
            // different bgIds for at->eye[Next] and eye[Next]->at
            ret = 3;
        } else {
            cosEyeAt = Math3D_Cos(&eyeChk->norm, &atChk->norm);
            if (cosEyeAt < -0.5f) {
                ret = 6;
            } else if (cosEyeAt > 0.5f) {
                ret = 3;
            } else {
                ret = 2;
            }
        }
    }
    return ret;
}

/**
 * Calculates how much to adjust the camera at's y value when on a slope.
 */
f32 Camera_CalcSlopeYAdj(Vec3f* floorNorm, s16 playerYRot, s16 eyeAtYaw, f32 adjAmt) {
    f32 tmp;
    VecSph floorNormSph;

    OLib_Vec3fToVecSphGeo(&floorNormSph, floorNorm);

    tmp = Math_CosS(floorNormSph.pitch) * Math_CosS(playerYRot - floorNormSph.yaw);
    return (fabsf(tmp) * adjAmt) * Math_CosS(playerYRot - eyeAtYaw);
}

/**
 * Calculates new at vector for the camera pointing in `eyeAtDir`
 */
s32 Camera_CalcAtDefault(Camera* camera, VecSph* eyeAtDir, f32 extraYOffset, s16 calcSlope) {
    Vec3f* at = &camera->at;
    Vec3f posOffsetTarget;
    Vec3f atTarget;
    s32 pad2;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 yOffset;

    yOffset = Player_GetHeight(camera->player);

    posOffsetTarget.x = 0.f;
    posOffsetTarget.y = yOffset + extraYOffset;
    posOffsetTarget.z = 0.f;

    if (calcSlope) {
        posOffsetTarget.y -= OLib_ClampMaxDist(
            Camera_CalcSlopeYAdj(&camera->floorNorm, playerPosRot->rot.y, eyeAtDir->yaw, OREG(9)), yOffset);
    }

    Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, camera->yOffsetUpdateRate, camera->xzOffsetUpdateRate,
                         0.1f);

    atTarget.x = playerPosRot->pos.x + camera->posOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->posOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->posOffset.z;

    Camera_LERPCeilVec3f(&atTarget, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);

    return true;
}

s32 func_800458D4(Camera* camera, VecSph* eyeAtDir, f32 arg2, f32* arg3, s16 arg4) {
    f32 phi_f2;
    Vec3f posOffsetTarget;
    Vec3f atTarget;
    f32 eyeAtAngle;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 deltaY;
    s32 pad[2];

    posOffsetTarget.y = Player_GetHeight(camera->player) + arg2;
    posOffsetTarget.x = 0.0f;
    posOffsetTarget.z = 0.0f;

    if (arg4) {
        posOffsetTarget.y -= Camera_CalcSlopeYAdj(&camera->floorNorm, playerPosRot->rot.y, eyeAtDir->yaw, OREG(9));
    }

    deltaY = playerPosRot->pos.y - *arg3;
    eyeAtAngle = Math_FAtan2F(deltaY, OLib_Vec3fDistXZ(&camera->at, &camera->eye));

    if (eyeAtAngle > DEGF_TO_RADF(OREG(32))) {
        phi_f2 = 1.0f - sinf(DEGF_TO_RADF(eyeAtAngle - OREG(32)));
    } else if (eyeAtAngle < DEGF_TO_RADF(OREG(33))) {
        phi_f2 = 1.0f - sinf(DEGF_TO_RADF(OREG(33)) - eyeAtAngle);
    } else {
        phi_f2 = 1.0f;
    }

    posOffsetTarget.y -= deltaY * phi_f2;
    Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, PCT(OREG(29)), PCT(OREG(30)), 0.1f);

    atTarget.x = playerPosRot->pos.x + camera->posOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->posOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->posOffset.z;

    Camera_LERPCeilVec3f(&atTarget, &camera->at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);

    return 1;
}

s32 func_80045B08(Camera* camera, VecSph* eyeAtDir, f32 yExtra, s16 arg3) {
    f32 phi_f2;
    Vec3f posOffsetTarget;
    Vec3f atTarget;
    f32 pad;
    f32 temp_ret;
    PosRot* playerPosRot = &camera->playerPosRot;

    posOffsetTarget.y = Player_GetHeight(camera->player) + yExtra;
    posOffsetTarget.x = 0.0f;
    posOffsetTarget.z = 0.0f;

    temp_ret = Math_SinS(arg3);

    if (temp_ret < 0.0f) {
        phi_f2 = Math_CosS(playerPosRot->rot.y - eyeAtDir->yaw);
    } else {
        phi_f2 = -Math_CosS(playerPosRot->rot.y - eyeAtDir->yaw);
    }

    posOffsetTarget.y -= temp_ret * phi_f2 * OREG(9);
    Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, camera->yOffsetUpdateRate, camera->xzOffsetUpdateRate,
                         0.1f);

    atTarget.x = playerPosRot->pos.x + camera->posOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->posOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->posOffset.z;
    Camera_LERPCeilVec3f(&atTarget, &camera->at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);

    return 1;
}

/**
 * Adjusts the camera's at position for Camera_Parallel1
 */
s32 Camera_CalcAtForParallel(Camera* camera, VecSph* arg1, f32 yOffset, f32* arg3, s16 arg4) {
    Vec3f* at = &camera->at;
    Vec3f posOffsetTarget;
    Vec3f atTarget;
    Vec3f* eye = &camera->eye;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 temp_f2;
    f32 phi_f16;
    f32 sp54;
    f32 phi_f20;
    f32 temp_f0_4;

    temp_f0_4 = Player_GetHeight(camera->player);
    posOffsetTarget.x = 0.0f;
    posOffsetTarget.y = temp_f0_4 + yOffset;
    posOffsetTarget.z = 0.0f;

    if (PREG(76) && arg4) {
        posOffsetTarget.y -= Camera_CalcSlopeYAdj(&camera->floorNorm, playerPosRot->rot.y, arg1->yaw, OREG(9));
    }

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & 0x200000) {
        *arg3 = Camera_LERPCeilF(playerPosRot->pos.y, *arg3, PCT(OREG(43)), 0.1f);
        phi_f20 = playerPosRot->pos.y - *arg3;
        posOffsetTarget.y -= phi_f20;
        Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, camera->yOffsetUpdateRate,
                             camera->xzOffsetUpdateRate, 0.1f);
    } else {
        if (!PREG(75)) {
            phi_f20 = playerPosRot->pos.y - *arg3;
            sp54 = OLib_Vec3fDistXZ(at, &camera->eye);
            phi_f16 = sp54;
            Math_FAtan2F(phi_f20, sp54);
            temp_f2 = Math_FTanF(DEG_TO_RAD(camera->fov * 0.4f)) * phi_f16;
            if (temp_f2 < phi_f20) {
                *arg3 += phi_f20 - temp_f2;
                phi_f20 = temp_f2;
            } else if (phi_f20 < -temp_f2) {
                *arg3 += phi_f20 + temp_f2;
                phi_f20 = -temp_f2;
            }
            posOffsetTarget.y -= phi_f20;
        } else {
            phi_f20 = playerPosRot->pos.y - *arg3;
            temp_f2 = Math_FAtan2F(phi_f20, OLib_Vec3fDistXZ(at, eye));
            if (DEG_TO_RAD(OREG(32)) < temp_f2) {
                phi_f16 = 1 - sinf(temp_f2 - DEG_TO_RAD(OREG(32)));
            } else if (temp_f2 < DEG_TO_RAD(OREG(33))) {
                phi_f16 = 1 - sinf(DEG_TO_RAD(OREG(33)) - temp_f2);
            } else {
                phi_f16 = 1;
            }
            posOffsetTarget.y -= phi_f20 * phi_f16;
        }
        Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, PCT(OREG(29)), PCT(OREG(30)), 0.1f);
        camera->yOffsetUpdateRate = PCT(OREG(29));
        camera->xzOffsetUpdateRate = PCT(OREG(30));
    }
    atTarget.x = playerPosRot->pos.x + camera->posOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->posOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->posOffset.z;
    Camera_LERPCeilVec3f(&atTarget, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);
    return 1;
}

/**
 * Adjusts at position for Camera_Battle1 and Camera_KeepOn1
 */
s32 Camera_CalcAtForLockOn(Camera* camera, VecSph* eyeAtDir, Vec3f* targetPos, f32 yOffset, f32 distance,
                           f32* yPosOffset, VecSph* outPlayerToTargetDir, s16 flags) {
    Vec3f* at = &camera->at;
    Vec3f tmpPos0;
    Vec3f tmpPos1;
    Vec3f lookFromOffset;
    Vec3f* floorNorm = &camera->floorNorm;
    VecSph playerToTargetDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 yPosDelta;
    f32 phi_f16;
    f32 eyeAtDist;
    f32 temp_f0_2;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);
    tmpPos0.x = 0.0f;
    tmpPos0.y = playerHeight + yOffset;
    tmpPos0.z = 0.0f;
    if (PREG(76) && (flags & FLG_ADJSLOPE)) {
        tmpPos0.y -= Camera_CalcSlopeYAdj(floorNorm, playerPosRot->rot.y, eyeAtDir->yaw, OREG(9));
    }

    // tmpPos1 is player's head
    tmpPos1 = playerPosRot->pos;
    tmpPos1.y += playerHeight;
    OLib_Vec3fDiffToVecSphGeo(outPlayerToTargetDir, &tmpPos1, targetPos);
    playerToTargetDir = *outPlayerToTargetDir;
    if (distance < playerToTargetDir.r) {
        playerToTargetDir.r = playerToTargetDir.r * PCT(OREG(38));
    } else {
        // ratio of player's height off ground to player's height.
        temp_f0_2 = OLib_ClampMaxDist((playerPosRot->pos.y - camera->playerGroundY) / playerHeight, 1.0f);
        playerToTargetDir.r =
            (playerToTargetDir.r * PCT(OREG(39))) -
            (((PCT(OREG(39)) - PCT(OREG(38))) * playerToTargetDir.r) * (playerToTargetDir.r / distance));
        playerToTargetDir.r = playerToTargetDir.r - (playerToTargetDir.r * temp_f0_2) * temp_f0_2;
    }

    if (flags & FLG_OFFGROUND) {
        playerToTargetDir.r *= 0.2f;
        camera->xzOffsetUpdateRate = camera->yOffsetUpdateRate = .01f;
    }

    OLib_VecSphGeoToVec3f(&lookFromOffset, &playerToTargetDir);

    if (PREG(89)) {
        osSyncPrintf("%f (%f %f %f) %f\n", playerToTargetDir.r / distance, lookFromOffset.x, lookFromOffset.y,
                     lookFromOffset.z, camera->atLERPStepScale);
    }

    tmpPos0.x = tmpPos0.x + lookFromOffset.x;
    tmpPos0.y = tmpPos0.y + lookFromOffset.y;
    tmpPos0.z = tmpPos0.z + lookFromOffset.z;

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & 0x200000) {
        *yPosOffset = Camera_LERPCeilF(playerPosRot->pos.y, *yPosOffset, PCT(OREG(43)), 0.1f);
        yPosDelta = playerPosRot->pos.y - *yPosOffset;
        tmpPos0.y -= yPosDelta;
        Camera_LERPCeilVec3f(&tmpPos0, &camera->posOffset, camera->yOffsetUpdateRate, camera->xzOffsetUpdateRate, 0.1f);
    } else {
        if (!(flags & FLG_OFFGROUND)) {
            yPosDelta = playerPosRot->pos.y - *yPosOffset;
            eyeAtDist = OLib_Vec3fDistXZ(at, &camera->eye);
            phi_f16 = eyeAtDist;
            Math_FAtan2F(yPosDelta, eyeAtDist);
            temp_f0_2 = Math_FTanF(DEG_TO_RAD(camera->fov * 0.4f)) * phi_f16;
            if (temp_f0_2 < yPosDelta) {
                *yPosOffset = *yPosOffset + (yPosDelta - temp_f0_2);
                yPosDelta = temp_f0_2;
            } else if (yPosDelta < -temp_f0_2) {
                *yPosOffset = *yPosOffset + (yPosDelta + temp_f0_2);
                yPosDelta = -temp_f0_2;
            }
            tmpPos0.y = tmpPos0.y - yPosDelta;
        } else {
            yPosDelta = playerPosRot->pos.y - *yPosOffset;
            temp_f0_2 = Math_FAtan2F(yPosDelta, OLib_Vec3fDistXZ(at, &camera->eye));

            if (temp_f0_2 > DEG_TO_RAD(OREG(32))) {
                phi_f16 = 1.0f - sinf(temp_f0_2 - DEG_TO_RAD(OREG(32)));
            } else if (temp_f0_2 < DEG_TO_RAD(OREG(33))) {
                phi_f16 = 1.0f - sinf(DEG_TO_RAD(OREG(33)) - temp_f0_2);
            } else {
                phi_f16 = 1.0f;
            }
            tmpPos0.y -= (yPosDelta * phi_f16);
        }

        Camera_LERPCeilVec3f(&tmpPos0, &camera->posOffset, PCT(OREG(29)), PCT(OREG(30)), 0.1f);
        camera->yOffsetUpdateRate = PCT(OREG(29));
        camera->xzOffsetUpdateRate = PCT(OREG(30));
    }

    tmpPos1.x = playerPosRot->pos.x + camera->posOffset.x;
    tmpPos1.y = playerPosRot->pos.y + camera->posOffset.y;
    tmpPos1.z = playerPosRot->pos.z + camera->posOffset.z;
    Camera_LERPCeilVec3f(&tmpPos1, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);
    return 1;
}

s32 Camera_CalcAtForHorse(Camera* camera, VecSph* eyeAtDir, f32 yOffset, f32* yPosOffset, s16 calcSlope) {
    Vec3f* at = &camera->at;
    Vec3f posOffsetTarget;
    Vec3f atTarget;
    s32 pad;
    s32 pad2;
    f32 playerHeight;
    Player* player;
    PosRot horsePosRot;

    playerHeight = Player_GetHeight(camera->player);
    player = camera->player;
    Actor_GetWorldPosShapeRot(&horsePosRot, player->rideActor);

    if (EN_HORSE_CHECK_JUMPING((EnHorse*)player->rideActor)) {
        horsePosRot.pos.y -= 49.f;
        *yPosOffset = Camera_LERPCeilF(horsePosRot.pos.y, *yPosOffset, 0.1f, 0.2f);
        camera->atLERPStepScale = Camera_LERPCeilF(0.4f, camera->atLERPStepScale, 0.2f, 0.02f);
    } else {
        *yPosOffset = Camera_LERPCeilF(horsePosRot.pos.y, *yPosOffset, 0.5f, 0.2f);
    }

    posOffsetTarget.x = 0.0f;
    posOffsetTarget.y = playerHeight + yOffset;
    posOffsetTarget.z = 0.0f;

    if (calcSlope != 0) {
        posOffsetTarget.y -=
            Camera_CalcSlopeYAdj(&camera->floorNorm, camera->playerPosRot.rot.y, eyeAtDir->yaw, OREG(9));
    }

    Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, camera->yOffsetUpdateRate, camera->xzOffsetUpdateRate,
                         0.1f);

    atTarget.x = camera->posOffset.x + horsePosRot.pos.x;
    atTarget.y = camera->posOffset.y + horsePosRot.pos.y;
    atTarget.z = camera->posOffset.z + horsePosRot.pos.z;
    Camera_LERPCeilVec3f(&atTarget, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);

    return 1;
}

f32 Camera_LERPClampDist(Camera* camera, f32 dist, f32 min, f32 max) {
    f32 distTarget;
    f32 rUpdateRateInvTarget;

    if (dist < min) {
        distTarget = min;
        rUpdateRateInvTarget = OREG(6);
    } else if (dist > max) {
        distTarget = max;
        rUpdateRateInvTarget = OREG(6);
    } else {
        distTarget = dist;
        rUpdateRateInvTarget = 1.0f;
    }

    camera->rUpdateRateInv = Camera_LERPCeilF(rUpdateRateInvTarget, camera->rUpdateRateInv, PCT(OREG(25)), 0.1f);
    return Camera_LERPCeilF(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 0.2f);
}

f32 Camera_ClampDist(Camera* camera, f32 dist, f32 minDist, f32 maxDist, s16 timer) {
    f32 distTarget;
    f32 rUpdateRateInvTarget;

    if (dist < minDist) {
        distTarget = minDist;

        rUpdateRateInvTarget = timer != 0 ? OREG(6) * 0.5f : OREG(6);
    } else if (maxDist < dist) {
        distTarget = maxDist;

        rUpdateRateInvTarget = timer != 0 ? OREG(6) * 0.5f : OREG(6);
    } else {
        distTarget = dist;

        rUpdateRateInvTarget = timer != 0 ? OREG(6) : 1.0f;
    }

    camera->rUpdateRateInv = Camera_LERPCeilF(rUpdateRateInvTarget, camera->rUpdateRateInv, PCT(OREG(25)), 0.1f);
    return Camera_LERPCeilF(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 0.2f);
}

s16 Camera_CalcDefaultPitch(Camera* camera, s16 arg1, s16 arg2, s16 arg3) {
    f32 pad;
    f32 phi_a2;
    f32 t;
    s16 phi_v0;
    s16 phi_v1;
    s16 sp1C;

    phi_v1 = ABS(arg1);
    phi_v0 = arg3 > 0 ? (s16)(Math_CosS(arg3) * arg3) : arg3;
    sp1C = arg2 - phi_v0;

    if (ABS(sp1C) < phi_v1) {
        phi_a2 = (1.0f / camera->pitchUpdateRateInv) * 3.0f;
    } else {
        t = phi_v1 * (1.0f / R_CAM_MAX_PHI);
        pad = Camera_InterpolateCurve(0.8f, 1.0f - t);
        phi_a2 = (1.0f / camera->pitchUpdateRateInv) * pad;
    }
    return Camera_LERPCeilS(sp1C, arg1, phi_a2, 0xA);
}

s16 Camera_CalcDefaultYaw(Camera* camera, s16 cur, s16 target, f32 arg3, f32 accel) {
    f32 velocity;
    s16 angDelta;
    f32 updSpeed;
    f32 speedT;
    f32 velFactor;
    f32 yawUpdRate;

    if (camera->xzSpeed > 0.001f) {
        angDelta = target - BINANG_ROT180(cur);
        speedT = COLPOLY_GET_NORMAL(BINANG_ROT180(angDelta));
    } else {
        angDelta = target - BINANG_ROT180(cur);
        speedT = PCT(OREG(48));
    }

    updSpeed = Camera_InterpolateCurve(arg3, speedT);

    velocity = updSpeed + (1.0f - updSpeed) * accel;

    if (velocity < 0.0f) {
        velocity = 0.0f;
    }

    velFactor = Camera_InterpolateCurve(0.5f, camera->speedRatio);
    yawUpdRate = 1.0f / camera->yawUpdateRateInv;
    return cur + (s16)(angDelta * velocity * velFactor * yawUpdRate);
}

void func_80046E20(Camera* camera, VecSph* eyeAdjustment, f32 minDist, f32 arg3, f32* arg4, SwingAnimation* anim) {
    static CamColChk atEyeColChk;
    static CamColChk eyeAtColChk;
    static CamColChk newEyeColChk;
    Vec3f* eye = &camera->eye;
    s32 temp_v0;
    Vec3f* at = &camera->at;
    Vec3f peekAroundPoint;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 temp_f0;
    VecSph newEyeAdjustment;
    VecSph sp40;

    temp_v0 = func_80045508(camera, eyeAdjustment, &atEyeColChk, &eyeAtColChk, !anim->unk_18);

    switch (temp_v0) {
        case 1:
        case 2:
            // angle between polys is between 60 and 120 degrees.
            Camera_BGCheckCorner(&anim->collisionClosePoint, at, eyeNext, &atEyeColChk, &eyeAtColChk);
            peekAroundPoint.x = anim->collisionClosePoint.x + (atEyeColChk.norm.x + eyeAtColChk.norm.x);
            peekAroundPoint.y = anim->collisionClosePoint.y + (atEyeColChk.norm.y + eyeAtColChk.norm.y);
            peekAroundPoint.z = anim->collisionClosePoint.z + (atEyeColChk.norm.z + eyeAtColChk.norm.z);

            temp_f0 = OLib_Vec3fDist(at, &atEyeColChk.pos);
            *arg4 = temp_f0 > minDist ? 1.0f : temp_f0 / minDist;

            anim->swingUpdateRate = PCT(OREG(10));
            anim->unk_18 = 1;
            anim->atEyePoly = eyeAtColChk.poly;
            OLib_Vec3fDiffToVecSphGeo(&newEyeAdjustment, at, &peekAroundPoint);

            newEyeAdjustment.r = eyeAdjustment->r;
            Camera_Vec3fVecSphGeoAdd(eye, at, &newEyeAdjustment);
            newEyeColChk.pos = *eye;
            if (Camera_BGCheckInfo(camera, at, &newEyeColChk) == 0) {
                // no collision found between at->newEyePos
                newEyeAdjustment.yaw += BINANG_SUB(eyeAdjustment->yaw, newEyeAdjustment.yaw) >> 1;
                newEyeAdjustment.pitch += BINANG_SUB(eyeAdjustment->pitch, newEyeAdjustment.pitch) >> 1;
                Camera_Vec3fVecSphGeoAdd(eye, at, &newEyeAdjustment);
                if (atEyeColChk.sphNorm.pitch < 0x2AA8) {
                    // ~ 60 degrees
                    anim->unk_16 = newEyeAdjustment.yaw;
                    anim->unk_14 = newEyeAdjustment.pitch;
                } else {
                    anim->unk_16 = eyeAdjustment->yaw;
                    anim->unk_14 = eyeAdjustment->pitch;
                }
                peekAroundPoint.x = anim->collisionClosePoint.x - (atEyeColChk.norm.x + eyeAtColChk.norm.x);
                peekAroundPoint.y = anim->collisionClosePoint.y - (atEyeColChk.norm.y + eyeAtColChk.norm.y);
                peekAroundPoint.z = anim->collisionClosePoint.z - (atEyeColChk.norm.z + eyeAtColChk.norm.z);
                OLib_Vec3fDiffToVecSphGeo(&newEyeAdjustment, at, &peekAroundPoint);
                newEyeAdjustment.r = eyeAdjustment->r;
                Camera_Vec3fVecSphGeoAdd(eyeNext, at, &newEyeAdjustment);
                break;
            }

            camera->eye = newEyeColChk.pos;
            atEyeColChk = newEyeColChk;

        case 3:
        case 6:
            if (anim->unk_18 != 0) {
                anim->swingUpdateRateTimer = OREG(52);
                anim->unk_18 = 0;
                *eyeNext = *eye;
            }

            temp_f0 = OLib_Vec3fDist(at, &atEyeColChk.pos);
            *arg4 = temp_f0 > minDist ? 1.0f : temp_f0 / minDist;

            anim->swingUpdateRate = *arg4 * arg3;

            Camera_Vec3fTranslateByUnitVector(eye, &atEyeColChk.pos, &atEyeColChk.norm, 1.0f);
            anim->atEyePoly = NULL;
            if (temp_f0 < OREG(21)) {
                sp40.yaw = eyeAdjustment->yaw;
                sp40.pitch = Math_SinS(atEyeColChk.sphNorm.pitch + 0x3FFF) * 16380.0f;
                sp40.r = (OREG(21) - temp_f0) * PCT(OREG(22));
                Camera_Vec3fVecSphGeoAdd(eye, eye, &sp40);
            }
            break;
        default:
            if (anim->unk_18 != 0) {
                anim->swingUpdateRateTimer = OREG(52);
                *eyeNext = *eye;
                anim->unk_18 = 0;
            }
            anim->swingUpdateRate = arg3;
            anim->atEyePoly = NULL;
            eye->x = atEyeColChk.pos.x + atEyeColChk.norm.x;
            eye->y = atEyeColChk.pos.y + atEyeColChk.norm.y;
            eye->z = atEyeColChk.pos.z + atEyeColChk.norm.z;
            break;
    }
}

s32 Camera_Noop(Camera* camera) {
    return true;
}

s32 SetCameraManual(Camera* camera) {
    f32 newCamX = -D_8015BD7C->state.input[0].cur.right_stick_x * 10.0f;
    f32 newCamY = D_8015BD7C->state.input[0].cur.right_stick_y * 10.0f;

    if ((fabsf(newCamX) >= 15.0f || fabsf(newCamY) >= 15.0f) && camera->play->manualCamera == false) {
        camera->play->manualCamera = true;

        VecSph eyeAdjustment;
        OLib_Vec3fDiffToVecSphGeo(&eyeAdjustment, &camera->at, &camera->eye);

        camera->play->camX = eyeAdjustment.yaw;
        camera->play->camY = eyeAdjustment.pitch;
    }

    if (camera->play->manualCamera) {
        return 1;
    }

    return 0;
}

s32 Camera_Free(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecSph spA8;
    CamColChk sp6C;
    Parallel1* para1 = (Parallel1*)camera->paramData;
    f32 playerHeight;

    at->x = Camera_LERPCeilF(camera->player->actor.world.pos.x, camera->at.x, 0.5f, 1.0f);
    at->y = Camera_LERPCeilF(camera->player->actor.world.pos.y + (camera->player->rideActor != NULL
                                                                      ? Player_GetHeight(camera->player) / 2
                                                                      : Player_GetHeight(camera->player)) /
                                                                     1.2f,
                             camera->at.y, 0.5f, 1.0f);
    at->z = Camera_LERPCeilF(camera->player->actor.world.pos.z, camera->at.z, 0.5f, 1.0f);

    playerHeight = Player_GetHeight(camera->player);

    if (RELOAD_PARAMS) {
        OLib_Vec3fDiffToVecSphGeo(&spA8, &camera->at, &camera->eye);

        camera->play->camX = spA8.yaw;
        camera->play->camY = spA8.pitch;

        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(OREG(46))) - (PCT(OREG(46)) * (68.0f / playerHeight));

        para1->yOffset = NEXTPCT * playerHeight * yNormal;
        para1->distTarget = NEXTPCT * playerHeight * yNormal;
        para1->pitchTarget = DEGF_TO_BINANG(NEXTSETTING);
        para1->yawTarget = DEGF_TO_BINANG(NEXTSETTING);
        para1->unk_08 = NEXTSETTING;
        para1->unk_0C = NEXTSETTING;
        para1->fovTarget = NEXTSETTING;
        para1->unk_14 = NEXTPCT;
        para1->interfaceFlags = NEXTSETTING;
        para1->unk_18 = NEXTPCT * playerHeight * yNormal;
        para1->unk_1C = NEXTPCT;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = 1;

    camera->animState = 1;

    f32 newCamX = -D_8015BD7C->state.input[0].cur.right_stick_x * 10.0f * (CVar_GetFloat("gCameraSensitivity", 1.0f));
    f32 newCamY = D_8015BD7C->state.input[0].cur.right_stick_y * 10.0f * (CVar_GetFloat("gCameraSensitivity", 1.0f));

    camera->play->camX += newCamX * (CVar_GetS32("gInvertXAxis", 0) ? -1 : 1);
    camera->play->camY += newCamY * (CVar_GetS32("gInvertYAxis", 1) ? 1 : -1);

    if (camera->play->camY > 0x32A4) {
        camera->play->camY = 0x32A4;
    }
    if (camera->play->camY < -0x228C) {
        camera->play->camY = -0x228C;
    }

    camera->dist = Camera_LERPCeilF(para1->distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 0.0f);
    OLib_Vec3fDiffToVecSphGeo(&spA8, at, eyeNext);

    spA8.r = camera->dist;
    spA8.yaw = camera->play->camX;
    spA8.pitch = camera->play->camY;

    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &spA8);
    if (camera->status == CAM_STAT_ACTIVE) {
        sp6C.pos = *eyeNext;
        Camera_BGCheckInfo(camera, at, &sp6C);
        *eye = sp6C.pos;
    }

    camera->fov = Camera_LERPCeilF(65.0f, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5, 0xA);

    return 1;
}

s32 Camera_Normal1(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 spA0;
    f32 sp9C;
    f32 sp98;
    f32 sp94;
    Vec3f sp88;
    s16 wiggleAdj;
    s16 t;
    VecSph eyeAdjustment;
    VecSph atEyeGeo;
    VecSph atEyeNextGeo;
    PosRot* playerPosRot = &camera->playerPosRot;
    Normal1* norm1 = (Normal1*)camera->paramData;
    Normal1Anim* anim = &norm1->anim;
    f32 playerHeight;
    f32 rate = 0.1f;

    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM) - PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));
        sp94 = yNormal * PCT(playerHeight);

        norm1->yOffset = NEXTSETTING * sp94;
        norm1->distMin = NEXTSETTING * sp94;
        norm1->distMax = NEXTSETTING * sp94;
        norm1->pitchTarget = DEGF_TO_BINANG(NEXTSETTING);
        norm1->unk_0C = NEXTSETTING;
        norm1->unk_10 = NEXTSETTING;
        norm1->unk_14 = NEXTPCT;
        norm1->fovTarget = NEXTSETTING;
        norm1->atLERPScaleMax = NEXTPCT;
        norm1->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = norm1->interfaceFlags;

    OLib_Vec3fDiffToVecSphGeo(&atEyeGeo, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&atEyeNextGeo, at, eyeNext);

    switch (camera->animState) {
        case 0x14:
            camera->yawUpdateRateInv = OREG(27);
            camera->pitchUpdateRateInv = OREG(27);
        case 0:
        case 0xA:
        case 0x19:
            anim->swing.atEyePoly = NULL;
            anim->slopePitchAdj = 0;
            anim->unk_28 = 0xA;
            anim->swing.unk_16 = anim->swing.unk_14 = anim->swing.unk_18 = 0;
            anim->swing.swingUpdateRate = norm1->unk_0C;
            anim->yOffset = camera->playerPosRot.pos.y;
            anim->unk_20 = camera->xzSpeed;
            anim->swing.swingUpdateRateTimer = 0;
            anim->swingYawTarget = atEyeGeo.yaw;
            sUpdateCameraDirection = 0;
            anim->startSwingTimer = OREG(50) + OREG(51);
            break;
        default:
            break;
    }

    camera->animState = 1;
    sUpdateCameraDirection = 1;

    if (anim->unk_28 != 0) {
        anim->unk_28--;
    }

    if (camera->xzSpeed > 0.001f) {
        anim->startSwingTimer = OREG(50) + OREG(51);
    } else if (anim->startSwingTimer > 0) {
        if (anim->startSwingTimer > OREG(50)) {
            anim->swingYawTarget = atEyeGeo.yaw + (BINANG_SUB(BINANG_ROT180(camera->playerPosRot.rot.y), atEyeGeo.yaw) /
                                                   anim->startSwingTimer);
        }
        anim->startSwingTimer--;
    }

    spA0 = camera->speedRatio * PCT(OREG(25));
    sp9C = camera->speedRatio * PCT(OREG(26));
    sp98 = anim->swing.unk_18 != 0 ? PCT(OREG(25)) : spA0;

    sp94 = (camera->xzSpeed - anim->unk_20) * (0.333333f);
    if (sp94 > 1.0f) {
        sp94 = 1.0f;
    }
    if (sp94 > -1.0f) {
        sp94 = -1.0f;
    }

    anim->unk_20 = camera->xzSpeed;

    if (anim->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv =
            Camera_LERPCeilF(anim->swing.swingUpdateRate + (f32)(anim->swing.swingUpdateRateTimer * 2),
                             camera->yawUpdateRateInv, sp98, rate);
        camera->pitchUpdateRateInv =
            Camera_LERPCeilF((f32)R_CAM_DEFA_PHI_UPDRATE + (f32)(anim->swing.swingUpdateRateTimer * 2),
                             camera->pitchUpdateRateInv, sp9C, rate);
        anim->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv =
            Camera_LERPCeilF(anim->swing.swingUpdateRate - ((OREG(49) * 0.01f) * anim->swing.swingUpdateRate * sp94),
                             camera->yawUpdateRateInv, sp98, rate);
        camera->pitchUpdateRateInv = Camera_LERPCeilF(R_CAM_DEFA_PHI_UPDRATE, camera->pitchUpdateRateInv, sp9C, rate);
    }

    camera->pitchUpdateRateInv = Camera_LERPCeilF(R_CAM_DEFA_PHI_UPDRATE, camera->pitchUpdateRateInv, sp9C, rate);
    camera->xzOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(2)), camera->xzOffsetUpdateRate, spA0, rate);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, sp9C, rate);
    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, rate);

    if (norm1->interfaceFlags & 1) {
        t = func_80044ADC(camera, BINANG_ROT180(atEyeGeo.yaw), 0);
        sp9C = ((1.0f / norm1->unk_10) * 0.5f) * (1.0f - camera->speedRatio);
        anim->slopePitchAdj = Camera_LERPCeilS(t, anim->slopePitchAdj, ((1.0f / norm1->unk_10) * 0.5f) + sp9C, 0xF);
    } else {
        anim->slopePitchAdj = 0;
        if (camera->playerGroundY == camera->playerPosRot.pos.y) {
            anim->yOffset = camera->playerPosRot.pos.y;
        }
    }

    spA0 = ((anim->swing.unk_18 != 0) && (norm1->yOffset > -40.0f))
               ? (sp9C = Math_SinS(anim->swing.unk_14), ((-40.0f * sp9C) + (norm1->yOffset * (1.0f - sp9C))))
               : norm1->yOffset;

    if (norm1->interfaceFlags & 0x80) {
        func_800458D4(camera, &atEyeNextGeo, spA0, &anim->yOffset, norm1->interfaceFlags & 1);
    } else if (norm1->interfaceFlags & 0x20) {
        func_80045B08(camera, &atEyeNextGeo, spA0, anim->slopePitchAdj);
    } else {
        Camera_CalcAtDefault(camera, &atEyeNextGeo, spA0, norm1->interfaceFlags & 1);
    }

    OLib_Vec3fDiffToVecSphGeo(&eyeAdjustment, at, eyeNext);

    camera->dist = eyeAdjustment.r =
        Camera_ClampDist(camera, eyeAdjustment.r, norm1->distMin, norm1->distMax, anim->unk_28);

    if (anim->startSwingTimer <= 0) {
        eyeAdjustment.pitch = atEyeNextGeo.pitch;
        eyeAdjustment.yaw =
            Camera_LERPCeilS(anim->swingYawTarget, atEyeNextGeo.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else if (anim->swing.unk_18 != 0) {
        eyeAdjustment.yaw =
            Camera_LERPCeilS(anim->swing.unk_16, atEyeNextGeo.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
        eyeAdjustment.pitch =
            Camera_LERPCeilS(anim->swing.unk_14, atEyeNextGeo.pitch, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else {
        // rotate yaw to follow player.
        eyeAdjustment.yaw =
            Camera_CalcDefaultYaw(camera, atEyeNextGeo.yaw, camera->playerPosRot.rot.y, norm1->unk_14, sp94);
        eyeAdjustment.pitch =
            Camera_CalcDefaultPitch(camera, atEyeNextGeo.pitch, norm1->pitchTarget, anim->slopePitchAdj);
    }

    // set eyeAdjustment pitch from 79.65 degrees to -85 degrees
    if (eyeAdjustment.pitch > 0x38A4) {
        eyeAdjustment.pitch = 0x38A4;
    }
    if (eyeAdjustment.pitch < -0x3C8C) {
        eyeAdjustment.pitch = -0x3C8C;
    }

    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeAdjustment);
    if ((camera->status == CAM_STAT_ACTIVE) && (!(norm1->interfaceFlags & 0x10))) {
        anim->swingYawTarget = BINANG_ROT180(camera->playerPosRot.rot.y);
        if (anim->startSwingTimer > 0) {
            func_80046E20(camera, &eyeAdjustment, norm1->distMin, norm1->unk_0C, &sp98, &anim->swing);
        } else {
            sp88 = *eyeNext;
            anim->swing.swingUpdateRate = camera->yawUpdateRateInv = norm1->unk_0C * 2.0f;
            if (Camera_BGCheck(camera, at, &sp88)) {
                anim->swingYawTarget = atEyeNextGeo.yaw;
                anim->startSwingTimer = -1;
            } else {
                *eye = *eyeNext;
            }
            anim->swing.unk_18 = 0;
        }

        if (anim->swing.unk_18 != 0) {
            camera->inputDir.y =
                Camera_LERPCeilS(camera->inputDir.y + BINANG_SUB(BINANG_ROT180(anim->swing.unk_16), camera->inputDir.y),
                                 camera->inputDir.y, 1.0f - (0.99f * sp98), 0xA);
        }

        if (norm1->interfaceFlags & 4) {
            camera->inputDir.x = -atEyeGeo.pitch;
            camera->inputDir.y = BINANG_ROT180(atEyeGeo.yaw);
            camera->inputDir.z = 0;
        } else {
            OLib_Vec3fDiffToVecSphGeo(&eyeAdjustment, eye, at);
            camera->inputDir.x = eyeAdjustment.pitch;
            camera->inputDir.y = eyeAdjustment.yaw;
            camera->inputDir.z = 0;
        }

        // crit wiggle
        if(!CVar_GetS32("gDisableCritWiggle",0)) {
            if (gSaveContext.health <= 16 && ((camera->play->state.frames % 256) == 0)) {
                wiggleAdj = Rand_ZeroOne() * 10000.0f;
                camera->inputDir.y = wiggleAdj + camera->inputDir.y;
            }
        }
    } else {
        anim->swing.swingUpdateRate = norm1->unk_0C;
        anim->swing.unk_18 = 0;
        sUpdateCameraDirection = 0;
        *eye = *eyeNext;
    }

    spA0 = (gSaveContext.health <= 16 ? 0.8f : 1.0f);
    camera->fov = Camera_LERPCeilF(norm1->fovTarget * spA0, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, norm1->atLERPScaleMax);
    return 1;
}

s32 Camera_Normal2(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    CamColChk bgChk;
    s16 phi_a0;
    s16 phi_a1;
    f32 spA4;
    f32 spA0;
    VecSph adjSph;
    VecSph sp90;
    VecSph sp88;
    VecSph atToEyeDir;
    VecSph atToEyeNextDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    Normal2* norm2 = (Normal2*)camera->paramData;
    Normal2Anim* anim = &norm2->anim;
    s32 pad;
    Vec3s* bgData;
    f32 playerHeight;
    f32 yNormal;

    playerHeight = Player_GetHeight(camera->player);
    yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        norm2->unk_00 = NEXTPCT * playerHeight * yNormal;
        norm2->unk_04 = NEXTPCT * playerHeight * yNormal;
        norm2->unk_08 = NEXTPCT * playerHeight * yNormal;
        norm2->unk_1C = DEGF_TO_BINANG(NEXTSETTING);
        norm2->unk_0C = NEXTSETTING;
        norm2->unk_10 = NEXTPCT;
        norm2->unk_14 = NEXTSETTING;
        norm2->unk_18 = NEXTPCT;
        norm2->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = norm2->interfaceFlags;

    switch (camera->animState) {
        case 0:
        case 0xA:
        case 0x14:
        case 0x19:
            bgData = Camera_GetCamBGData(camera);
            Camera_Vec3sToVec3f(&anim->unk_00, &BGCAM_POS(bgData));
            anim->unk_20 = BGCAM_ROT(bgData).x;
            anim->unk_22 = BGCAM_ROT(bgData).y;
            anim->unk_24 = playerPosRot->pos.y;
            anim->unk_1C = BGCAM_FOV(bgData) == -1      ? norm2->unk_14
                           : BGCAM_FOV(bgData) >= 0x169 ? PCT(BGCAM_FOV(bgData))
                                                        : BGCAM_FOV(bgData);

            anim->unk_28 = BGCAM_JFIFID(bgData) == -1 ? 0 : BGCAM_JFIFID(bgData);

            anim->unk_18 = 0.0f;

            if (norm2->interfaceFlags & 4) {
                sp88.pitch = anim->unk_20;
                sp88.yaw = anim->unk_22 + 0x3FFF;
                sp88.r = 100.0f;
                OLib_VecSphGeoToVec3f(&anim->unk_0C, &sp88);
            }

            camera->animState = 1;
            camera->yawUpdateRateInv = 50.0f;
            break;
        default:
            if (camera->playerGroundY == playerPosRot->pos.y) {
                anim->unk_24 = playerPosRot->pos.y;
            }
            break;
    }

    OLib_Vec3fDiffToVecSphGeo(&atToEyeDir, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&atToEyeNextDir, at, eyeNext);

    camera->speedRatio *= 0.5f;
    spA4 = PCT(OREG(25)) * camera->speedRatio;
    spA0 = PCT(OREG(26)) * camera->speedRatio;

    camera->yawUpdateRateInv =
        Camera_LERPCeilF(norm2->unk_0C, camera->yawUpdateRateInv * camera->speedRatio, PCT(OREG(25)), 0.1f);
    camera->pitchUpdateRateInv = Camera_LERPCeilF(OREG(7), camera->pitchUpdateRateInv, spA0, 0.1f);
    camera->xzOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(2)), camera->xzOffsetUpdateRate, spA4, 0.1f);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, spA0, 0.1f);
    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, 0.1f);

    if (!(norm2->interfaceFlags & 0x80)) {
        Camera_CalcAtDefault(camera, &atToEyeNextDir, norm2->unk_00, norm2->interfaceFlags & 1);
    } else {
        func_800458D4(camera, &atToEyeNextDir, norm2->unk_00, &anim->unk_24, norm2->interfaceFlags & 1);
    }

    if (norm2->interfaceFlags & 4) {
        anim->unk_00.x = playerPosRot->pos.x + anim->unk_0C.x;
        anim->unk_00.z = playerPosRot->pos.z + anim->unk_0C.z;
    }

    anim->unk_00.y = playerPosRot->pos.y;

    OLib_Vec3fDiffToVecSphGeo(&sp88, &anim->unk_00, at);
    OLib_Vec3fDiffToVecSphGeo(&sp90, at, eyeNext);

    phi_a1 = (anim->unk_28 & 2 ? anim->unk_22 : norm2->unk_1C);
    phi_a0 = BINANG_SUB(sp90.yaw, sp88.yaw);
    if ((phi_a1 < 0x4000 && ABS(phi_a0) > phi_a1) || (phi_a1 >= 0x4000 && ABS(phi_a0) < phi_a1)) {

        phi_a0 = (phi_a0 < 0 ? -phi_a1 : phi_a1);
        phi_a0 += sp88.yaw;
        adjSph.yaw =
            Camera_LERPCeilS(phi_a0, atToEyeDir.yaw, (1.0f / camera->yawUpdateRateInv) * camera->speedRatio, 0xA);
        if (anim->unk_28 & 1) {
            adjSph.pitch = Camera_CalcDefaultPitch(camera, atToEyeNextDir.pitch, anim->unk_20, 0);
        } else {
            adjSph.pitch = atToEyeDir.pitch;
        }
    } else {
        adjSph = sp90;
    }

    camera->dist = adjSph.r = Camera_ClampDist(camera, sp90.r, norm2->unk_04, norm2->unk_08, 0);

    if (!(anim->unk_28 & 1)) {
        if (adjSph.pitch >= 0xE39) {
            adjSph.pitch += (BINANG_SUB(0xE38, adjSph.pitch) >> 2);
        }

        if (adjSph.pitch < 0) {
            adjSph.pitch += (BINANG_SUB(-0x38E, adjSph.pitch) >> 2);
        }
    }

    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &adjSph);

    if (camera->status == CAM_STAT_ACTIVE) {
        bgChk.pos = *eyeNext;
        if (!camera->play->envCtx.skyboxDisabled || norm2->interfaceFlags & 0x10) {
            Camera_BGCheckInfo(camera, at, &bgChk);
            *eye = bgChk.pos;
        } else {
            func_80043F94(camera, at, &bgChk);
            *eye = bgChk.pos;
            OLib_Vec3fDiffToVecSphGeo(&adjSph, eye, at);
            camera->inputDir.x = adjSph.pitch;
            camera->inputDir.y = adjSph.yaw;
            camera->inputDir.z = 0;
        }
    }

    camera->fov = Camera_LERPCeilF(anim->unk_1C, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, norm2->unk_18);
    return 1;
}

// riding epona
s32 Camera_Normal3(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 sp98;
    f32 sp94;
    f32 sp90;
    f32 sp8C;
    VecSph sp84;
    VecSph sp7C;
    VecSph sp74;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 temp_f0;
    f32 temp_f6;
    s16 phi_a0;
    s16 t2;
    Normal3* norm3 = (Normal3*)camera->paramData;
    Normal3Anim* anim = &norm3->anim;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        norm3->yOffset = NEXTSETTING * PCT(playerHeight);
        norm3->distMin = NEXTSETTING * PCT(playerHeight);
        norm3->distMax = NEXTSETTING * PCT(playerHeight);
        norm3->pitchTarget = DEGF_TO_BINANG(NEXTSETTING);
        norm3->yawUpdateSpeed = NEXTSETTING;
        norm3->unk_10 = NEXTSETTING;
        norm3->fovTarget = NEXTSETTING;
        norm3->maxAtLERPScale = NEXTPCT;
        norm3->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&sp7C, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&sp74, at, eyeNext);

    sUpdateCameraDirection = true;
    sCameraInterfaceFlags = norm3->interfaceFlags;
    switch (camera->animState) {
        case 0:
        case 0xA:
        case 0x14:
        case 0x19:
            anim->swing.atEyePoly = NULL;
            anim->curPitch = 0;
            anim->unk_1C = 0.0f;
            anim->unk_20 = camera->playerGroundY;
            anim->swing.unk_16 = anim->swing.unk_14 = anim->swing.unk_18 = 0;
            anim->swing.swingUpdateRate = norm3->yawUpdateSpeed;
            anim->yawUpdAmt = BINANG_SUB(BINANG_ROT180(playerPosRot->rot.y), sp7C.yaw) * (1.0f / OREG(23));
            anim->distTimer = 10;
            anim->yawTimer = OREG(23);
            camera->animState = 1;
            anim->swing.swingUpdateRateTimer = 0;
    }

    if (anim->distTimer != 0) {
        anim->distTimer--;
    }

    sp98 = PCT(OREG(25)) * camera->speedRatio;
    sp94 = PCT(OREG(26)) * camera->speedRatio;

    if (anim->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv = Camera_LERPCeilF(norm3->yawUpdateSpeed + (anim->swing.swingUpdateRateTimer * 2),
                                                    camera->yawUpdateRateInv, sp98, 0.1f);
        camera->pitchUpdateRateInv = Camera_LERPCeilF((f32)OREG(7) + (anim->swing.swingUpdateRateTimer * 2),
                                                      camera->pitchUpdateRateInv, sp94, 0.1f);
        anim->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv = Camera_LERPCeilF(norm3->yawUpdateSpeed, camera->yawUpdateRateInv, sp98, 0.1f);
        camera->pitchUpdateRateInv = Camera_LERPCeilF(OREG(7), camera->pitchUpdateRateInv, sp94, 0.1f);
    }

    camera->xzOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(2)), camera->xzOffsetUpdateRate, sp98, 0.1f);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, sp94, 0.1f);
    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->fovUpdateRate, sp94, 0.1f);

    t2 = func_80044ADC(camera, BINANG_ROT180(sp7C.yaw), 1);
    sp94 = ((1.0f / norm3->unk_10) * 0.5f);
    temp_f0 = (((1.0f / norm3->unk_10) * 0.5f) * (1.0f - camera->speedRatio));
    anim->curPitch = Camera_LERPCeilS(t2, anim->curPitch, sp94 + temp_f0, 0xF);

    Camera_CalcAtForHorse(camera, &sp74, norm3->yOffset, &anim->unk_20, 1);
    sp90 = (norm3->distMax + norm3->distMin) * 0.5f;
    OLib_Vec3fDiffToVecSphGeo(&sp84, at, eyeNext);
    camera->dist = sp84.r = Camera_ClampDist(camera, sp84.r, norm3->distMin, norm3->distMax, anim->distTimer);
    if (camera->xzSpeed > 0.001f) {
        sp84.r += (sp90 - sp84.r) * 0.002f;
    }
    phi_a0 = BINANG_SUB(norm3->pitchTarget, anim->curPitch);
    sp84.pitch = Camera_LERPCeilS(phi_a0, sp74.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);

    if (OREG(5) < sp84.pitch) {
        sp84.pitch = OREG(5);
    }
    if (sp84.pitch < OREG(34)) {
        sp84.pitch = OREG(34);
    }

    phi_a0 = BINANG_SUB(playerPosRot->rot.y, BINANG_ROT180(sp74.yaw));
    if (ABS(phi_a0) > 0x2AF8) {
        if (phi_a0 > 0) {
            phi_a0 = 0x2AF8;
        } else {
            phi_a0 = -0x2AF8;
        }
    }

    sp90 = 1.0f;
    sp98 = 0.5;
    sp94 = camera->speedRatio;
    sp90 -= sp98;
    sp98 = sp98 + (sp94 * sp90);
    sp98 = (sp98 * phi_a0) / camera->yawUpdateRateInv;

    sp84.yaw = fabsf(sp98) > (150.0f * (1.0f - camera->speedRatio)) ? (s16)(sp74.yaw + sp98) : sp74.yaw;

    if (anim->yawTimer > 0) {
        sp84.yaw += anim->yawUpdAmt;
        anim->yawTimer--;
    }

    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &sp84);

    if (camera->status == CAM_STAT_ACTIVE) {
        func_80046E20(camera, &sp84, norm3->distMin, norm3->yawUpdateSpeed, &sp8C, &anim->swing);
    } else {
        *eye = *eyeNext;
    }

    camera->fov = Camera_LERPCeilF(norm3->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, norm3->maxAtLERPScale);
    return 1;
}

s32 Camera_Normal4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Normal0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Parallel1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 spB8;
    f32 spB4;
    s16 tangle;
    VecSph spA8;
    VecSph atToEyeDir;
    VecSph atToEyeNextDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    CamColChk sp6C;
    s16 sp6A;
    s16 phi_a0;
    Parallel1* para1 = (Parallel1*)camera->paramData;
    Parallel1Anim* anim = &para1->anim;
    f32 pad2;
    f32 playerHeight;
    s32 pad3;

    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(OREG(46))) - (PCT(OREG(46)) * (68.0f / playerHeight));

        para1->yOffset = NEXTPCT * playerHeight * yNormal;
        para1->distTarget = NEXTPCT * playerHeight * yNormal;
        para1->pitchTarget = DEGF_TO_BINANG(NEXTSETTING);
        para1->yawTarget = DEGF_TO_BINANG(NEXTSETTING);
        para1->unk_08 = NEXTSETTING;
        para1->unk_0C = NEXTSETTING;
        para1->fovTarget = NEXTSETTING;
        para1->unk_14 = NEXTPCT;
        para1->interfaceFlags = NEXTSETTING;
        para1->unk_18 = NEXTPCT * playerHeight * yNormal;
        para1->unk_1C = NEXTPCT;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&atToEyeDir, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&atToEyeNextDir, at, eyeNext);

    camera->play->manualCamera = false;

    switch (camera->animState) {
        case 0:
        case 0xA:
        case 0x14:
        case 0x19:
            anim->unk_16 = 0;
            anim->unk_10 = 0;
            if (para1->interfaceFlags & 4) {
                anim->animTimer = 20;
            } else {
                anim->animTimer = OREG(23);
            }
            anim->unk_00.x = 0.0f;
            anim->yTarget = playerPosRot->pos.y - camera->playerPosDelta.y;
            camera->animState++;
    }

    if (anim->animTimer != 0) {
        if (para1->interfaceFlags & 2) {
            // Rotate para1->yawTarget degrees from behind the player.
            anim->yawTarget = BINANG_ROT180(playerPosRot->rot.y) + para1->yawTarget;
        } else if (para1->interfaceFlags & 4) {
            // rotate to para1->yawTarget
            anim->yawTarget = para1->yawTarget;
        } else {
            // leave the rotation alone.
            anim->yawTarget = atToEyeNextDir.yaw;
        }
    } else {
        if (para1->interfaceFlags & 0x20) {
            anim->yawTarget = BINANG_ROT180(playerPosRot->rot.y) + para1->yawTarget;
        }
        sCameraInterfaceFlags = para1->interfaceFlags;
    }

    anim->pitchTarget = para1->pitchTarget;

    if (camera->animState == 0x15) {
        anim->unk_16 = 1;
        camera->animState = 1;
    } else if (camera->animState == 0xB) {
        camera->animState = 1;
    }

    spB8 = PCT(OREG(25)) * camera->speedRatio;
    spB4 = PCT(OREG(26)) * camera->speedRatio;

    camera->rUpdateRateInv = Camera_LERPCeilF(OREG(6), camera->rUpdateRateInv, spB8, 0.1f);
    camera->yawUpdateRateInv = Camera_LERPCeilF(para1->unk_08, camera->yawUpdateRateInv, spB8, 0.1f);
    camera->pitchUpdateRateInv = Camera_LERPCeilF(2.0f, camera->pitchUpdateRateInv, spB4, 0.1f);
    camera->xzOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(2)), camera->xzOffsetUpdateRate, spB8, 0.1f);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, spB4, 0.1f);
    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.1f);

    if (para1->interfaceFlags & 1) {
        tangle = func_80044ADC(camera, BINANG_ROT180(atToEyeDir.yaw), 1);

        spB8 = ((1.0f / para1->unk_0C) * 0.3f);
        pad2 = (((1.0f / para1->unk_0C) * 0.7f) * (1.0f - camera->speedRatio));
        anim->unk_10 = Camera_LERPCeilS(tangle, anim->unk_10, spB8 + pad2, 0xF);
    } else {
        anim->unk_10 = 0;
    }

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & 0x200000) {
        anim->yTarget = playerPosRot->pos.y;
        sp6A = 0;
    } else {
        sp6A = 1;
    }

    if (!(para1->interfaceFlags & 0x80) && !sp6A) {
        Camera_CalcAtForParallel(camera, &atToEyeNextDir, para1->yOffset, &anim->yTarget, para1->interfaceFlags & 1);
    } else {
        func_800458D4(camera, &atToEyeNextDir, para1->unk_18, &anim->yTarget, para1->interfaceFlags & 1);
    }

    if (anim->animTimer != 0) {
        camera->unk_14C |= 0x20;
        tangle = (((anim->animTimer + 1) * anim->animTimer) >> 1);
        spA8.yaw = atToEyeDir.yaw + ((BINANG_SUB(anim->yawTarget, atToEyeDir.yaw) / tangle) * anim->animTimer);
        spA8.pitch = atToEyeDir.pitch;
        spA8.r = atToEyeDir.r;
        anim->animTimer--;
    } else {
        anim->unk_16 = 0;
        camera->dist = Camera_LERPCeilF(para1->distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 2.0f);
        OLib_Vec3fDiffToVecSphGeo(&spA8, at, eyeNext);
        spA8.r = camera->dist;

        if (para1->interfaceFlags & 0x40) {
            spA8.yaw = Camera_LERPCeilS(anim->yawTarget, atToEyeNextDir.yaw, 0.6f, 0xA);
        } else {
            spA8.yaw = Camera_LERPCeilS(anim->yawTarget, atToEyeNextDir.yaw, 0.8f, 0xA);
        }

        if (para1->interfaceFlags & 1) {
            phi_a0 = BINANG_SUB(anim->pitchTarget, anim->unk_10);
        } else {
            phi_a0 = anim->pitchTarget;
        }

        spA8.pitch = Camera_LERPCeilS(phi_a0, atToEyeNextDir.pitch, 1.0f / camera->pitchUpdateRateInv, 4);

        if (spA8.pitch > OREG(5)) {
            spA8.pitch = OREG(5);
        }

        if (spA8.pitch < OREG(34)) {
            spA8.pitch = OREG(34);
        }
    }
    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &spA8);
    if (camera->status == CAM_STAT_ACTIVE) {
        sp6C.pos = *eyeNext;
        if (!camera->play->envCtx.skyboxDisabled || para1->interfaceFlags & 0x10) {
            Camera_BGCheckInfo(camera, at, &sp6C);
            *eye = sp6C.pos;
        } else {
            func_80043F94(camera, at, &sp6C);
            *eye = sp6C.pos;
            OLib_Vec3fDiffToVecSphGeo(&spA8, eye, at);
            camera->inputDir.x = spA8.pitch;
            camera->inputDir.y = spA8.yaw;
            camera->inputDir.z = 0;
        }
    }
    camera->fov = Camera_LERPCeilF(para1->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, sp6A ? para1->unk_1C : para1->unk_14);
    //! @bug No return
}

s32 Camera_Parallel2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Parallel3(Camera* camera) {
    CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
    s16 val = NEXTSETTING;

    sCameraInterfaceFlags = val;

    if (val & 1) {
        camera->unk_14C |= 0x400;
    }
    if (val & 2) {
        camera->unk_14C |= 0x10;
    }
    //! @bug doesn't return
}

s32 Camera_Parallel4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Parallel0(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Generic jump, jumping off ledges
 */
s32 Camera_Jump1(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    s32 pad2;
    f32 spA4;
    Vec3f newEye;
    VecSph eyeAtOffset;
    VecSph eyeNextAtOffset;
    VecSph eyeDiffSph;
    VecSph eyeDiffTarget;
    PosRot* playerPosRot = &camera->playerPosRot;
    PosRot playerhead;
    s16 tangle;
    Jump1* jump1 = (Jump1*)camera->paramData;
    Jump1Anim* anim = &jump1->anim;
    s32 pad;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));

        jump1->atYOffset = PCT(NEXTSETTING) * playerHeight * yNormal;
        jump1->distMin = PCT(NEXTSETTING) * playerHeight * yNormal;
        jump1->distMax = PCT(NEXTSETTING) * playerHeight * yNormal;
        jump1->yawUpateRateTarget = NEXTSETTING;
        jump1->maxYawUpdate = PCT(NEXTSETTING);
        jump1->unk_14 = NEXTSETTING;
        jump1->atLERPScaleMax = PCT(NEXTSETTING);
        jump1->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    // playerhead never gets used.
    Actor_GetFocus(&playerhead, &camera->player->actor);

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&eyeNextAtOffset, at, eyeNext);

    sCameraInterfaceFlags = jump1->interfaceFlags;

    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        anim->swing.unk_16 = anim->swing.unk_18 = 0;
        anim->swing.atEyePoly = NULL;
        anim->unk_20.pitch = 0;
        anim->unk_20.yaw = 0xC8;
        anim->swing.swingUpdateRateTimer = 0;
        anim->swing.swingUpdateRate = jump1->yawUpateRateTarget;
        anim->unk_1C = playerPosRot->pos.y - camera->playerPosDelta.y;
        anim->unk_20.r = eyeAtOffset.r;
        camera->posOffset.y -= camera->playerPosDelta.y;
        camera->xzOffsetUpdateRate = (1.0f / 10000.0f);
        camera->animState++;
    }

    if (anim->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv = Camera_LERPCeilF(jump1->yawUpateRateTarget + anim->swing.swingUpdateRateTimer,
                                                    camera->yawUpdateRateInv, PCT(OREG(26)), 0.1f);
        camera->pitchUpdateRateInv = Camera_LERPCeilF((f32)R_CAM_DEFA_PHI_UPDRATE + anim->swing.swingUpdateRateTimer,
                                                      camera->pitchUpdateRateInv, PCT(OREG(26)), 0.1f);
        anim->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv =
            Camera_LERPCeilF(jump1->yawUpateRateTarget, camera->yawUpdateRateInv, PCT(OREG(26)), 0.1f);
        camera->pitchUpdateRateInv =
            Camera_LERPCeilF((f32)R_CAM_DEFA_PHI_UPDRATE, camera->pitchUpdateRateInv, PCT(OREG(26)), 0.1f);
    }

    camera->xzOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(2)), camera->xzOffsetUpdateRate, PCT(OREG(25)), 0.1f);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, PCT(OREG(26)), 0.1f);
    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->yOffsetUpdateRate, 0.05f, 0.1f);

    func_800458D4(camera, &eyeNextAtOffset, jump1->atYOffset, &anim->unk_1C, 0);

    eyeDiffSph = eyeAtOffset;

    OLib_Vec3fDiffToVecSphGeo(&eyeDiffTarget, at, eye);

    eyeDiffSph.r = Camera_LERPCeilF(eyeDiffTarget.r, eyeAtOffset.r, PCT(OREG(29)), 1.0f);
    eyeDiffSph.pitch = Camera_LERPCeilS(eyeDiffTarget.pitch, eyeAtOffset.pitch, PCT(OREG(29)), 0xA);

    if (anim->swing.unk_18) {
        eyeDiffSph.yaw =
            Camera_LERPCeilS(anim->swing.unk_16, eyeNextAtOffset.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
        eyeDiffSph.pitch =
            Camera_LERPCeilS(anim->swing.unk_14, eyeNextAtOffset.pitch, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else {
        eyeDiffSph.yaw =
            Camera_CalcDefaultYaw(camera, eyeNextAtOffset.yaw, camera->playerPosRot.rot.y, jump1->maxYawUpdate, 0.0f);
    }

    // Clamp the eye->at distance to jump1->distMin < eyeDiffSph.r < jump1->distMax
    if (eyeDiffSph.r < jump1->distMin) {
        eyeDiffSph.r = jump1->distMin;
    } else if (eyeDiffSph.r > jump1->distMax) {
        eyeDiffSph.r = jump1->distMax;
    }

    // Clamp the phi rotation at R_CAM_MAX_PHI AND R_CAM_MIN_PHI2
    if (eyeDiffSph.pitch > R_CAM_MAX_PHI) {
        eyeDiffSph.pitch = R_CAM_MAX_PHI;
    } else if (eyeDiffSph.pitch < R_CAM_MIN_PHI2) {
        eyeDiffSph.pitch = R_CAM_MIN_PHI2;
    }

    Camera_Vec3fVecSphGeoAdd(&newEye, at, &eyeDiffSph);
    eyeNext->x = newEye.x;
    eyeNext->z = newEye.z;
    eyeNext->y += (newEye.y - eyeNext->y) * PCT(OREG(31));
    if ((camera->status == CAM_STAT_ACTIVE) && !(jump1->interfaceFlags & 0x10)) {
        func_80046E20(camera, &eyeDiffSph, jump1->distMin, jump1->yawUpateRateTarget, &spA4, &anim->swing);
        if (jump1->interfaceFlags & 4) {
            camera->inputDir.x = -eyeAtOffset.pitch;
            camera->inputDir.y = BINANG_ROT180(eyeAtOffset.yaw);
            camera->inputDir.z = 0;
        } else {
            OLib_Vec3fDiffToVecSphGeo(&eyeDiffSph, eye, at);
            camera->inputDir.x = eyeDiffSph.pitch;
            camera->inputDir.y = eyeDiffSph.yaw;
            camera->inputDir.z = 0;
        }
        if (anim->swing.unk_18) {
            camera->inputDir.y =
                Camera_LERPCeilS(camera->inputDir.y + BINANG_SUB(BINANG_ROT180(anim->swing.unk_16), camera->inputDir.y),
                                 camera->inputDir.y, 1.0f - (0.99f * spA4), 0xA);
        }
    } else {
        anim->swing.swingUpdateRate = jump1->yawUpateRateTarget;
        anim->swing.unk_18 = 0;
        sUpdateCameraDirection = 0;
        *eye = *eyeNext;
    }

    camera->dist = OLib_Vec3fDist(at, eye);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, jump1->atLERPScaleMax);
    return true;
}

// Climbing ladders/vines
s32 Camera_Jump2(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f bgChkPos;
    Vec3f floorNorm;
    VecSph adjAtToEyeDir;
    VecSph bgChkPara;
    VecSph atToEyeNextDir;
    VecSph atToEyeDir;
    f32 temp_f14;
    f32 temp_f16;
    f32 sp90;
    f32 sp8C;
    s32 bgId;
    CamColChk camBgChk;
    PosRot* playerPosRot = &camera->playerPosRot;
    s16 yawDiff;
    s16 playerYawRot180;
    Jump2* jump2 = (Jump2*)camera->paramData;
    Jump2Anim* anim = &jump2->anim;
    CameraModeValue* values;
    f32 playerHeight;
    f32 yNormal;

    playerHeight = Player_GetHeight(camera->player);

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        yNormal = (1.0f + PCT(OREG(46))) - (PCT(OREG(46)) * (68.0f / playerHeight));
        jump2->atYOffset =
            PCT((camera->playerPosDelta.y > 0.0f ? -10.0f : 10.0f) + NEXTSETTING) * playerHeight * yNormal;
        jump2->minDist = NEXTPCT * playerHeight * yNormal;
        jump2->maxDist = NEXTPCT * playerHeight * yNormal;
        jump2->minMaxDistFactor = NEXTPCT;
        jump2->yawUpdRateTarget = NEXTSETTING;
        jump2->xzUpdRateTarget = NEXTPCT;
        jump2->fovTarget = NEXTSETTING;
        jump2->atLERPStepScale = NEXTPCT;
        jump2->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&atToEyeDir, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&atToEyeNextDir, at, eyeNext);

    sCameraInterfaceFlags = jump2->interfaceFlags;

    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        bgChkPos = playerPosRot->pos;
        anim->floorY = Camera_GetFloorY(camera, &bgChkPos);
        anim->yawTarget = atToEyeNextDir.yaw;
        anim->initYawDiff = 0;
        if (anim->floorY == BGCHECK_Y_MIN) {
            osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: climb: no floor \n" VT_RST);
            anim->onFloor = -1;
            anim->floorY = playerPosRot->pos.y - 1000.0f;
        } else if (playerPosRot->pos.y - anim->floorY < playerHeight) {
            // player's model is within the height of the floor.
            anim->onFloor = 1;
        } else {
            anim->onFloor = -1;
        }

        yawDiff = BINANG_SUB(BINANG_ROT180(playerPosRot->rot.y), atToEyeNextDir.yaw);
        anim->initYawDiff = ((yawDiff / OREG(23)) / 4) * 3;
        if (jump2->interfaceFlags & 2) {
            anim->yawAdj = 0xA;
        } else {
            anim->yawAdj = 0x2710;
        }

        playerPosRot->pos.x -= camera->playerPosDelta.x;
        playerPosRot->pos.y -= camera->playerPosDelta.y;
        playerPosRot->pos.z -= camera->playerPosDelta.z;
        anim->animTimer = OREG(23);
        camera->animState++;
        camera->atLERPStepScale = jump2->atLERPStepScale;
    }

    sp90 = PCT(OREG(25)) * camera->speedRatio;
    sp8C = PCT(OREG(26)) * camera->speedRatio;
    camera->yawUpdateRateInv = Camera_LERPCeilF(jump2->yawUpdRateTarget, camera->yawUpdateRateInv, sp90, 0.1f);
    camera->xzOffsetUpdateRate = Camera_LERPCeilF(jump2->xzUpdRateTarget, camera->xzOffsetUpdateRate, sp90, 0.1f);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, sp8C, 0.1f);

    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, 0.1f);
    camera->rUpdateRateInv = OREG(27);

    Camera_CalcAtDefault(camera, &atToEyeNextDir, jump2->atYOffset, 0);
    OLib_Vec3fDiffToVecSphGeo(&adjAtToEyeDir, at, eye);

    temp_f16 = jump2->minDist;
    sp90 = jump2->maxDist + (jump2->maxDist * jump2->minMaxDistFactor);
    temp_f14 = temp_f16 - (jump2->minDist * jump2->minMaxDistFactor);

    if (adjAtToEyeDir.r > sp90) {
        adjAtToEyeDir.r = sp90;
    } else if (adjAtToEyeDir.r < temp_f14) {
        adjAtToEyeDir.r = temp_f14;
    }

    yawDiff = BINANG_SUB(BINANG_ROT180(playerPosRot->rot.y), adjAtToEyeDir.yaw);
    if (anim->animTimer != 0) {
        anim->yawTarget = BINANG_ROT180(playerPosRot->rot.y);
        anim->animTimer--;
        adjAtToEyeDir.yaw = Camera_LERPCeilS(anim->yawTarget, atToEyeNextDir.yaw, 0.5f, 0xA);
    } else if (anim->yawAdj < ABS(yawDiff)) {
        playerYawRot180 = BINANG_ROT180(playerPosRot->rot.y);
        adjAtToEyeDir.yaw = Camera_LERPFloorS(
            ((yawDiff < 0) ? (s16)(playerYawRot180 + anim->yawAdj) : (s16)(playerYawRot180 - anim->yawAdj)),
            atToEyeNextDir.yaw, 0.1f, 0xA);
    } else {
        adjAtToEyeDir.yaw = Camera_LERPCeilS(adjAtToEyeDir.yaw, atToEyeNextDir.yaw, 0.25f, 0xA);
    }

    // Check the floor at the top of the climb
    bgChkPos.x = playerPosRot->pos.x + (Math_SinS(playerPosRot->rot.y) * 25.0f);
    bgChkPos.y = playerPosRot->pos.y + (playerHeight * 2.2f);
    bgChkPos.z = playerPosRot->pos.z + (Math_CosS(playerPosRot->rot.y) * 25.0f);

    sp90 = Camera_GetFloorYNorm(camera, &floorNorm, &bgChkPos, &bgId);
    if ((sp90 != BGCHECK_Y_MIN) && (playerPosRot->pos.y < sp90)) {
        // top of the climb is within 2.2x of the player's height.
        camera->pitchUpdateRateInv = Camera_LERPCeilF(20.0f, camera->pitchUpdateRateInv, PCT(OREG(26)), 0.1f);
        camera->rUpdateRateInv = Camera_LERPCeilF(20.0f, camera->rUpdateRateInv, PCT(OREG(26)), 0.1f);
        adjAtToEyeDir.pitch = Camera_LERPCeilS(0x1F4, atToEyeNextDir.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);
    } else if ((playerPosRot->pos.y - anim->floorY) < playerHeight) {
        // player is within his height of the ground.
        camera->pitchUpdateRateInv = Camera_LERPCeilF(20.0f, camera->pitchUpdateRateInv, PCT(OREG(26)), 0.1f);
        camera->rUpdateRateInv = Camera_LERPCeilF(20.0f, camera->rUpdateRateInv, PCT(OREG(26)), 0.1f);
        adjAtToEyeDir.pitch = Camera_LERPCeilS(0x1F4, atToEyeNextDir.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);
    } else {
        camera->pitchUpdateRateInv = 100.0f;
        camera->rUpdateRateInv = 100.0f;
    }

    // max pitch to +/- ~ 60 degrees
    if (adjAtToEyeDir.pitch > 0x2AF8) {
        adjAtToEyeDir.pitch = 0x2AF8;
    }

    if (adjAtToEyeDir.pitch < -0x2AF8) {
        adjAtToEyeDir.pitch = -0x2AF8;
    }

    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &adjAtToEyeDir);
    camBgChk.pos = *eyeNext;
    if (Camera_BGCheckInfo(camera, at, &camBgChk)) {
        // Collision detected between at->eyeNext, Check if collision between
        // at->eyeNext, but parallel to at (pitch = 0).
        bgChkPos = camBgChk.pos;
        bgChkPara.r = adjAtToEyeDir.r;
        bgChkPara.pitch = 0;
        bgChkPara.yaw = adjAtToEyeDir.yaw;
        Camera_Vec3fVecSphGeoAdd(&camBgChk.pos, at, &bgChkPara);
        if (Camera_BGCheckInfo(camera, at, &camBgChk)) {
            // Collision found between parallel at->eyeNext, set eye position to
            // first collsion point.
            *eye = bgChkPos;
        } else {
            // no collision found with the parallel at->eye, animate to be parallel
            adjAtToEyeDir.pitch = Camera_LERPCeilS(0, adjAtToEyeDir.pitch, 0.2f, 0xA);
            Camera_Vec3fVecSphGeoAdd(eye, at, &adjAtToEyeDir);
            // useless?
            Camera_BGCheck(camera, at, eye);
        }
    } else {
        // no collision detected.
        *eye = *eyeNext;
    }

    camera->dist = adjAtToEyeDir.r;
    camera->fov = Camera_LERPCeilF(jump2->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    return true;
}

// swimming
s32 Camera_Jump3(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    s32 prevMode;
    f32 spC4;
    f32 spC0;
    f32 spBC;
    Vec3f spB0; // unused
    VecSph eyeDiffSph;
    PosRot* playerPosRot = &camera->playerPosRot;
    Jump3* jump3 = (Jump3*)camera->paramData;
    VecSph eyeAtOffset;
    VecSph eyeNextAtOffset;
    s32 pad;
    s32 pad2;
    CameraModeValue* values;
    f32 t2;
    f32 phi_f0;
    f32 phi_f2;
    f32 playerHeight;
    PosRot playerhead;
    f32 yNormal;
    f32 temp_f18;
    s32 modeSwitch;
    f32 temp_f2_2;
    Jump3Anim* anim = &jump3->anim;

    playerHeight = Player_GetHeight(camera->player);
    Actor_GetFocus(&playerhead, &camera->player->actor);

    modeSwitch = false;
    if (((camera->waterYPos - eye->y) < OREG(44) || (camera->animState == 0))) {
        if (anim->mode != CAM_MODE_NORMAL) {
            anim->mode = CAM_MODE_NORMAL;
            modeSwitch = true;
        }
    } else if (((camera->waterYPos - eye->y) > OREG(45)) && (anim->mode != CAM_MODE_BOOMERANG)) {
        anim->mode = CAM_MODE_BOOMERANG;
        modeSwitch = true;
    }

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&eyeNextAtOffset, at, eyeNext);

    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14 || modeSwitch ||
        R_RELOAD_CAM_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[anim->mode].values;
        yNormal = ((1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight)));
        t2 = PCT(playerHeight) * yNormal;
        jump3->yOffset = NEXTSETTING * t2;
        jump3->distMin = NEXTSETTING * t2;
        jump3->distMax = NEXTSETTING * t2;
        jump3->pitchTarget = DEGF_TO_BINANG(NEXTSETTING);
        jump3->swingUpdateRate = NEXTSETTING;
        jump3->unk_10 = NEXTSETTING;
        jump3->unk_14 = NEXTPCT;
        jump3->fovTarget = NEXTSETTING;
        jump3->unk_1C = NEXTPCT;
        jump3->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        prevMode = camera->mode;
        camera->mode = anim->mode;
        Camera_CopyPREGToModeValues(camera);
        camera->mode = prevMode;
    }

    sCameraInterfaceFlags = jump3->interfaceFlags;

    switch (camera->animState) {
        case 0:
        case 0xA:
        case 0x14:
        case 0x19:
            anim->swing.atEyePoly = NULL;
            anim->unk_1C = camera->playerGroundY;
            anim->swing.unk_16 = anim->swing.unk_14 = anim->swing.unk_18 = 0;
            anim->animTimer = 0xA;
            anim->swing.swingUpdateRate = jump3->swingUpdateRate;
            camera->animState++;
            anim->swing.swingUpdateRateTimer = 0;
            break;
        default:
            if (anim->animTimer != 0) {
                anim->animTimer--;
            }
            break;
    }

    spB0 = *eye; // unused
    (void)spB0;  // suppresses set but unused warning

    spC4 = PCT(OREG(25)) * camera->speedRatio;
    spC0 = camera->speedRatio * PCT(OREG(26));
    spBC = anim->swing.unk_18 != 0 ? PCT(OREG(25)) : spC4;

    if (anim->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv = Camera_LERPCeilF(
            anim->swing.swingUpdateRate + (anim->swing.swingUpdateRateTimer * 2), camera->yawUpdateRateInv, spC4, 0.1f);
        camera->pitchUpdateRateInv =
            Camera_LERPCeilF((anim->swing.swingUpdateRateTimer * 2) + 40.0f, camera->pitchUpdateRateInv, spC0, 0.1f);
        anim->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv = Camera_LERPCeilF(anim->swing.swingUpdateRate, camera->yawUpdateRateInv, spBC, 0.1f);
        camera->pitchUpdateRateInv = Camera_LERPCeilF(40.0f, camera->pitchUpdateRateInv, spC0, 0.1f);
    }

    camera->xzOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(2)), camera->xzOffsetUpdateRate, spC4, 0.1f);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, spC0, 0.1f);
    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, 0.1f);

    Camera_CalcAtDefault(camera, &eyeNextAtOffset, jump3->yOffset, jump3->interfaceFlags);
    OLib_Vec3fDiffToVecSphGeo(&eyeDiffSph, at, eyeNext);

    camera->dist = eyeDiffSph.r =
        Camera_ClampDist(camera, eyeDiffSph.r, jump3->distMin, jump3->distMax, anim->animTimer);

    if (camera->playerGroundY <= playerPosRot->pos.y) {
        phi_f0 = playerPosRot->pos.y - camera->playerGroundY;
    } else {
        phi_f0 = -(playerPosRot->pos.y - camera->playerGroundY);
    }

    if (!(phi_f0 < 10.0f)) {
        if (camera->waterYPos <= playerhead.pos.y) {
            phi_f2 = playerhead.pos.y - camera->waterYPos;
        } else {
            phi_f2 = -(playerhead.pos.y - camera->waterYPos);
        }
        if (!(phi_f2 < 50.0f)) {
            camera->pitchUpdateRateInv = 100.0f;
        }
    }
    if (anim->swing.unk_18 != 0) {
        eyeDiffSph.yaw =
            Camera_LERPCeilS(anim->swing.unk_16, eyeNextAtOffset.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
        eyeDiffSph.pitch =
            Camera_LERPCeilS(anim->swing.unk_14, eyeNextAtOffset.pitch, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else {
        eyeDiffSph.yaw = Camera_CalcDefaultYaw(camera, eyeNextAtOffset.yaw, playerPosRot->rot.y, jump3->unk_14, 0.0f);
        eyeDiffSph.pitch = Camera_CalcDefaultPitch(camera, eyeNextAtOffset.pitch, jump3->pitchTarget, 0);
    }

    if (eyeDiffSph.pitch > OREG(5)) {
        eyeDiffSph.pitch = OREG(5);
    }

    if (eyeDiffSph.pitch < OREG(34)) {
        eyeDiffSph.pitch = OREG(34);
    }

    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeDiffSph);
    if ((camera->status == CAM_STAT_ACTIVE) && !(jump3->interfaceFlags & 0x10)) {
        func_80046E20(camera, &eyeDiffSph, jump3->distMin, jump3->swingUpdateRate, &spBC, &anim->swing);
        if (jump3->interfaceFlags & 4) {
            camera->inputDir.x = -eyeAtOffset.pitch;
            camera->inputDir.y = BINANG_ROT180(eyeAtOffset.yaw);
            camera->inputDir.z = 0;
        } else {
            OLib_Vec3fDiffToVecSphGeo(&eyeDiffSph, eye, at);
            camera->inputDir.x = eyeDiffSph.pitch;
            camera->inputDir.y = eyeDiffSph.yaw;
            camera->inputDir.z = 0;
        }

        if (anim->swing.unk_18 != 0) {
            camera->inputDir.y =
                Camera_LERPCeilS(camera->inputDir.y + BINANG_SUB(BINANG_ROT180(anim->swing.unk_16), camera->inputDir.y),
                                 camera->inputDir.y, 1.0f - (0.99f * spBC), 0xA);
        }
    } else {
        anim->swing.swingUpdateRate = jump3->swingUpdateRate;
        anim->swing.unk_18 = 0;
        sUpdateCameraDirection = 0;
        *eye = *eyeNext;
    }
    camera->fov = Camera_LERPCeilF(jump3->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, jump3->unk_1C);
    return true;
}

s32 Camera_Jump4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Jump0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Battle1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp128;
    Vec3f playerHead;
    Vec3f targetPos;
    f32 var3;
    f32 var2;
    f32 temp_f0_2;
    f32 temp_f12_2;
    f32 spFC;
    f32 spF8;
    f32 swingAngle;
    f32 temp_f2_2;
    f32 temp_f14;
    s32 skipEyeAtCalc;
    f32 distRatio;
    CamColChk spBC;
    VecSph spB4;
    VecSph atToTargetDir;
    VecSph playerToTargetDir;
    VecSph atToEyeDir;
    VecSph atToEyeNextDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    s16 tmpAng1;
    s16 tmpAng2;
    Player* player;
    s16 sp86;
    s16 isOffGround;
    f32 distance;
    f32 sp7C;
    f32 sp78;
    f32 fov;
    Battle1* batt1 = (Battle1*)camera->paramData;
    Battle1Anim* anim = &batt1->anim;
    s32 pad;
    f32 playerHeight;

    skipEyeAtCalc = false;
    player = camera->player;
    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(OREG(46))) - (PCT(OREG(46)) * (68.0f / playerHeight));

        batt1->yOffset = NEXTPCT * playerHeight * yNormal;
        batt1->distance = NEXTSETTING;
        batt1->swingYawInitial = NEXTSETTING;
        batt1->swingYawFinal = NEXTSETTING;
        batt1->swingPitchInitial = NEXTSETTING;
        batt1->swingPitchFinal = NEXTSETTING;
        batt1->swingPitchAdj = NEXTPCT;
        batt1->fov = NEXTSETTING;
        batt1->atLERPScaleOnGround = NEXTPCT;
        batt1->flags = NEXTSETTING;
        batt1->yOffsetOffGround = NEXTPCT * playerHeight * yNormal;
        batt1->atLERPScaleOffGround = NEXTPCT;
        anim->chargeTimer = 40;
        anim->unk_10 = PCT(OREG(12));
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    distance = batt1->distance;
    sp7C = batt1->swingPitchInitial;
    sp78 = batt1->swingPitchFinal;
    fov = batt1->fov;

    if (camera->player->stateFlags1 & 0x1000) {
        // charging sword.
        anim->unk_10 = Camera_LERPCeilF(PCT(OREG(12)) * 0.5f, anim->unk_10, PCT(OREG(25)), 0.1f);
        camera->xzOffsetUpdateRate = Camera_LERPCeilF(0.2f, camera->xzOffsetUpdateRate, PCT(OREG(25)), 0.1f);
        camera->yOffsetUpdateRate = Camera_LERPCeilF(0.2f, camera->yOffsetUpdateRate, PCT(OREG(25)), 0.1f);
        if (anim->chargeTimer >= -19) {
            anim->chargeTimer--;
        } else {
            distance = 250.0f;
            sp7C = 50.0f;
            sp78 = 40.0f;
            fov = 60.0f;
        }
    } else if (anim->chargeTimer < 0) {
        distance = 250.0f;
        sp7C = 50.0f;
        sp78 = 40.0f;
        fov = 60.0f;
        anim->chargeTimer++;
    } else {
        anim->chargeTimer = 40;
        anim->unk_10 = Camera_LERPCeilF(PCT(OREG(12)), anim->unk_10, PCT(OREG(25)), 0.1f);
        camera->xzOffsetUpdateRate =
            Camera_LERPCeilF(PCT(OREG(40)), camera->xzOffsetUpdateRate, PCT(OREG(25)) * camera->speedRatio, 0.1f);
        camera->yOffsetUpdateRate =
            Camera_LERPCeilF(PCT(OREG(40)), camera->yOffsetUpdateRate, PCT(OREG(26)) * camera->speedRatio, 0.1f);
    }
    camera->fovUpdateRate = Camera_LERPCeilF(PCT(OREG(4)), camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.1f);
    playerHeight += batt1->yOffset;
    OLib_Vec3fDiffToVecSphGeo(&atToEyeDir, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&atToEyeNextDir, at, eyeNext);
    if (camera->target == NULL || camera->target->update == NULL) {
        if (camera->target == NULL) {
            osSyncPrintf(
                VT_COL(YELLOW, BLACK) "camera: warning: battle: target is not valid, change parallel\n" VT_RST);
        }
        camera->target = NULL;
        Camera_ChangeMode(camera, CAM_MODE_TARGET);
        return true;
    }

    sCameraInterfaceFlags = batt1->flags;

    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        anim->unk_14 = 0;
        anim->roll = 0.0f;
        anim->target = camera->target;
        camera->animState++;
        if (anim->target->id > 0) {
            osSyncPrintf("camera: battle: target actor name " VT_FGCOL(BLUE) "%d" VT_RST "\n", anim->target->id);
        } else {
            osSyncPrintf("camera: battle: target actor name " VT_COL(RED, WHITE) "%d" VT_RST "\n", anim->target->id);
            camera->target = NULL;
            Camera_ChangeMode(camera, CAM_MODE_TARGET);
            return true;
        }
        anim->animTimer = OREG(23) + OREG(24);
        anim->initialEyeToAtYaw = atToEyeDir.yaw;
        anim->initialEyeToAtPitch = atToEyeDir.pitch;
        anim->initialEyeToAtDist = atToEyeDir.r;
        anim->yPosOffset = playerPosRot->pos.y - camera->playerPosDelta.y;
    }

    if (camera->status == CAM_STAT_ACTIVE) {
        sUpdateCameraDirection = 1;
        camera->inputDir.x = -atToEyeDir.pitch;
        camera->inputDir.y = BINANG_ROT180(atToEyeDir.yaw);
        camera->inputDir.z = 0;
    }

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & 0x200000) {
        isOffGround = false;
        anim->yPosOffset = playerPosRot->pos.y;
    } else {
        isOffGround = true;
    }

    if (anim->animTimer == 0) {
        camera->atLERPStepScale =
            Camera_ClampLERPScale(camera, isOffGround ? batt1->atLERPScaleOffGround : batt1->atLERPScaleOnGround);
    }
    Actor_GetFocus(&camera->targetPosRot, camera->target);
    if (anim->target != camera->target) {
        osSyncPrintf("camera: battle: change target %d -> " VT_FGCOL(BLUE) "%d" VT_RST "\n", anim->target->id,
                     camera->target->id);
        camera->animState = 0;
        return true;
    }

    Camera_CalcAtForLockOn(camera, &atToEyeNextDir, &camera->targetPosRot.pos,
                           isOffGround ? batt1->yOffsetOffGround : batt1->yOffset, distance, &anim->yPosOffset,
                           &playerToTargetDir, (isOffGround ? 0x81 : 1) | batt1->flags);
    tmpAng2 = playerToTargetDir.yaw;
    playerHead = playerPosRot->pos;
    playerHead.y += playerHeight;
    OLib_Vec3fDiffToVecSphGeo(&playerToTargetDir, &playerHead, &camera->targetPosRot.pos);
    distRatio = playerToTargetDir.r > distance ? 1 : playerToTargetDir.r / distance;
    targetPos = camera->targetPosRot.pos;
    OLib_Vec3fDiffToVecSphGeo(&atToTargetDir, at, &targetPos);
    atToTargetDir.r = distance - ((atToTargetDir.r <= distance ? atToTargetDir.r : distance) * 0.5f);
    swingAngle = batt1->swingYawInitial + ((batt1->swingYawFinal - batt1->swingYawInitial) * (1.1f - distRatio));
    spF8 = OREG(13) + swingAngle;

    spB4.r = camera->dist = Camera_LERPCeilF(distance, camera->dist, PCT(OREG(11)), 2.0f);
    spB4.yaw = atToEyeNextDir.yaw;
    tmpAng1 = BINANG_SUB(atToTargetDir.yaw, BINANG_ROT180(atToEyeNextDir.yaw));
    if (anim->animTimer != 0) {
        if (anim->animTimer >= OREG(24)) {
            sp86 = anim->animTimer - OREG(24);
            OLib_Vec3fDiffToVecSphGeo(&playerToTargetDir, at, eye);
            playerToTargetDir.yaw = BINANG_ROT180(tmpAng2);

            var2 = 1.0f / OREG(23);
            var3 = (anim->initialEyeToAtDist - playerToTargetDir.r) * var2;
            tmpAng1 = BINANG_SUB(anim->initialEyeToAtYaw, playerToTargetDir.yaw) * var2;
            tmpAng2 = BINANG_SUB(anim->initialEyeToAtPitch, playerToTargetDir.pitch) * var2;

            spB4.r = Camera_LERPCeilF(playerToTargetDir.r + (var3 * sp86), atToEyeDir.r, PCT(OREG(28)), 1.0f);
            spB4.yaw = Camera_LERPCeilS(playerToTargetDir.yaw + (tmpAng1 * sp86), atToEyeDir.yaw, PCT(OREG(28)), 0xA);
            spB4.pitch =
                Camera_LERPCeilS(playerToTargetDir.pitch + (tmpAng2 * sp86), atToEyeDir.pitch, PCT(OREG(28)), 0xA);
        } else {
            skipEyeAtCalc = true;
        }
        anim->animTimer--;
    } else if (ABS(tmpAng1) > DEGF_TO_BINANG(swingAngle)) {
        spFC = BINANG_TO_DEGF(tmpAng1);
        temp_f2_2 = swingAngle + (spF8 - swingAngle) * (OLib_ClampMaxDist(atToTargetDir.r, spB4.r) / spB4.r);
        temp_f12_2 = ((temp_f2_2 * temp_f2_2) - 2.0f) / (temp_f2_2 - 360.0f);
        var2 = ((temp_f12_2 * spFC) + (2.0f - (360.0f * temp_f12_2)));
        temp_f14 = SQ(spFC) / var2;
        tmpAng2 = tmpAng1 >= 0 ? DEGF_TO_BINANG(temp_f14) : (-DEGF_TO_BINANG(temp_f14));
        spB4.yaw = BINANG_ROT180((s16)(BINANG_ROT180(atToEyeNextDir.yaw) + tmpAng2));
    } else {
        spFC = 0.05f;
        spFC = (1 - camera->speedRatio) * spFC;
        tmpAng2 = tmpAng1 >= 0 ? DEGF_TO_BINANG(swingAngle) : -DEGF_TO_BINANG(swingAngle);
        spB4.yaw = atToEyeNextDir.yaw - (s16)((tmpAng2 - tmpAng1) * spFC);
    }

    if (!skipEyeAtCalc) {
        var3 = atToTargetDir.pitch * batt1->swingPitchAdj;
        var2 = F32_LERPIMP(sp7C, sp78, distRatio);
        tmpAng1 = DEGF_TO_BINANG(var2) - (s16)(playerToTargetDir.pitch * (0.5f + distRatio * (1.0f - 0.5f)));
        tmpAng1 += (s16)(var3);

        if (tmpAng1 < -0x2AA8) {
            tmpAng1 = -0x2AA8;
        } else if (tmpAng1 > 0x2AA8) {
            tmpAng1 = 0x2AA8;
        }

        spB4.pitch = Camera_LERPCeilS(tmpAng1, atToEyeNextDir.pitch, anim->unk_10, 0xA);
        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &spB4);
        spBC.pos = *eyeNext;
        if (camera->status == CAM_STAT_ACTIVE) {
            if (!camera->play->envCtx.skyboxDisabled || batt1->flags & 1) {
                Camera_BGCheckInfo(camera, at, &spBC);
            } else if (batt1->flags & 2) {
                func_80043F94(camera, at, &spBC);
            } else {
                OLib_Vec3fDistNormalize(&sp128, at, &spBC.pos);
                spBC.pos.x -= sp128.x;
                spBC.pos.y -= sp128.y;
                spBC.pos.z -= sp128.z;
            }
            *eye = spBC.pos;
        } else {
            *eye = *eyeNext;
        }
    }
    anim->roll += (((OREG(36) * camera->speedRatio) * (1.0f - distRatio)) - anim->roll) * PCT(OREG(37));
    camera->roll = DEGF_TO_BINANG(anim->roll);
    camera->fov = Camera_LERPCeilF((player->swordState != 0       ? 0.8f
                                    : gSaveContext.health <= 0x10 ? 0.8f
                                                                  : 1.0f) *
                                       (fov - ((fov * 0.05f) * distRatio)),
                                   camera->fov, camera->fovUpdateRate, 1.0f);
}

s32 Camera_Battle2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Battle3(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Charging spin attack
 * Camera zooms out slowly for 50 frames, then tilts up to a specified
 * setting value.
 */
s32 Camera_Battle4(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecSph eyeNextOffset;
    VecSph eyeAtOffset;
    VecSph eyeNextAtOffset;
    Battle4* batt4 = (Battle4*)camera->paramData;
    Battle4Anim* anim = &batt4->anim;
    s32 pad;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));

        batt4->yOffset = NEXTPCT * playerHeight * yNormal;
        batt4->rTarget = NEXTPCT * playerHeight * yNormal;
        batt4->pitchTarget = DEGF_TO_BINANG(NEXTSETTING);
        batt4->lerpUpdateRate = NEXTPCT;
        batt4->fovTarget = NEXTSETTING;
        batt4->atLERPTarget = NEXTPCT;
        batt4->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&eyeNextAtOffset, at, eyeNext);

    sCameraInterfaceFlags = batt4->interfaceFlags;

    switch (camera->animState) {
        case 0:
        case 0xA:
        case 0x14:
            anim->animTimer = 50;
            camera->animState++;
    }

    camera->yawUpdateRateInv =
        Camera_LERPCeilF(batt4->lerpUpdateRate, camera->yawUpdateRateInv, PCT(OREG(25)) * camera->speedRatio, 0.1f);
    camera->rUpdateRateInv = 1000.0f;
    camera->pitchUpdateRateInv = 1000.0f;
    camera->xzOffsetUpdateRate = Camera_LERPCeilF(0.025f, camera->xzOffsetUpdateRate, PCT(OREG(25)), 0.1f);
    camera->yOffsetUpdateRate =
        Camera_LERPCeilF(PCT(OREG(3)), camera->yOffsetUpdateRate, PCT(OREG(26)) * camera->speedRatio, 0.1f);
    camera->fovUpdateRate = 0.0001f;
    Camera_CalcAtDefault(camera, &eyeNextAtOffset, batt4->yOffset, 1);
    if (anim->animTimer != 0) {
        eyeNextOffset.yaw = eyeAtOffset.yaw;
        eyeNextOffset.pitch = eyeAtOffset.pitch;
        eyeNextOffset.r = eyeAtOffset.r;
        anim->animTimer--;
    } else {
        eyeNextOffset.yaw = eyeAtOffset.yaw;
        eyeNextOffset.pitch = Camera_LERPCeilS(batt4->pitchTarget, eyeAtOffset.pitch, batt4->lerpUpdateRate, 2);
        eyeNextOffset.r = Camera_LERPCeilF(batt4->rTarget, eyeAtOffset.r, batt4->lerpUpdateRate, 0.001f);
    }
    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeNextOffset);
    *eye = *eyeNext;
    camera->dist = eyeNextOffset.r;
    camera->fov = Camera_LERPCeilF(batt4->fovTarget, camera->fov, batt4->lerpUpdateRate, 1.0f);
    camera->roll = 0;
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, batt4->atLERPTarget);
    return true;
}

s32 Camera_Battle0(Camera* camera) {
    return Camera_Noop(camera);
}

// Targeting non-enemy
s32 Camera_KeepOn1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp120;
    Vec3f sp114;
    Vec3f sp108;
    f32 sp104;
    f32 temp_f12_2;
    f32 temp_f14;
    f32 t1;
    f32 spF4;
    f32 spF0;
    f32 spEC;
    f32 spE8;
    f32 t2;
    s16 spE2;
    s16 spE0;
    VecSph spD8;
    VecSph spD0;
    VecSph spC8;
    VecSph spC0;
    VecSph spB8;
    PosRot* playerPosRot = &camera->playerPosRot;
    CamColChk sp8C;
    s32 sp88;
    f32 sp84;
    s16 sp82;
    s16 sp80;
    KeepOn1* keep1 = (KeepOn1*)camera->paramData;
    Keep1Anim* anim = &keep1->anim;
    s16 t3;
    f32 playerHeight;

    sp88 = 0;
    playerHeight = Player_GetHeight(camera->player);
    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        if (camera->target == NULL) {
            osSyncPrintf(
                VT_COL(YELLOW, BLACK) "camera: warning: keepon: target is not valid, change parallel\n" VT_RST);
        }
        camera->target = NULL;
        Camera_ChangeMode(camera, CAM_MODE_TARGET);
        return 1;
    }

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(OREG(46))) - (PCT(OREG(46)) * (68.0f / playerHeight));

        keep1->unk_00 = NEXTPCT * playerHeight * yNormal;
        keep1->unk_04 = NEXTSETTING;
        keep1->unk_08 = NEXTSETTING;
        keep1->unk_0C = NEXTSETTING;
        keep1->unk_10 = NEXTSETTING;
        keep1->unk_14 = NEXTSETTING;
        keep1->unk_18 = NEXTSETTING;
        keep1->unk_1C = NEXTPCT;
        keep1->unk_20 = NEXTSETTING;
        keep1->unk_24 = NEXTPCT;
        keep1->interfaceFlags = NEXTSETTING;
        keep1->unk_28 = NEXTPCT * playerHeight * yNormal;
        keep1->unk_2C = NEXTPCT;
    }
    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    playerHeight += keep1->unk_00;
    OLib_Vec3fDiffToVecSphGeo(&spC0, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&spB8, at, eyeNext);
    sCameraInterfaceFlags = keep1->interfaceFlags;
    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        camera->animState++;
        anim->unk_10 = 0;
        anim->unk_04 = 0.0f;
        anim->unk_0C = camera->target;
        anim->unk_16 = OREG(23) + OREG(24);
        anim->unk_12 = spC0.yaw;
        anim->unk_14 = spC0.pitch;
        anim->unk_00 = spC0.r;
        anim->unk_08 = playerPosRot->pos.y - camera->playerPosDelta.y;
    }
    if (camera->status == 7) {
        sUpdateCameraDirection = 1;
        camera->inputDir.x = -spC0.pitch;
        camera->inputDir.y = BINANG_ROT180(spC0.yaw);
        camera->inputDir.z = 0;
    }

    sp104 = keep1->unk_04;
    sp84 = 1;

    switch (camera->paramFlags & 0x18) {
        case 8:
            if ((camera->player->actor.category == 2) && (camera->player->interactRangeActor == camera->target)) {
                PosRot sp54;
                Actor_GetFocus(&sp54, &camera->player->actor);
                spC8.r = 60.0f;
                spC8.yaw = camera->playerPosRot.rot.y;
                spC8.pitch = 0x2EE0;
                Camera_Vec3fVecSphGeoAdd(&camera->targetPosRot.pos, &sp54.pos, &spC8);
            } else {
                Actor_GetFocus(&camera->targetPosRot, camera->target);
            }
            Actor_GetFocus(&camera->targetPosRot, camera->target);
            if (anim->unk_0C != camera->target) {
                anim->unk_0C = camera->target;
                camera->atLERPStepScale = 0.0f;
            }
            camera->xzOffsetUpdateRate =
                Camera_LERPCeilF(1.0f, camera->xzOffsetUpdateRate, PCT(OREG(25)) * camera->speedRatio, 0.1f);
            camera->yOffsetUpdateRate =
                Camera_LERPCeilF(1.0f, camera->yOffsetUpdateRate, PCT(OREG(26)) * camera->speedRatio, 0.1f);
            camera->fovUpdateRate =
                Camera_LERPCeilF(PCT(OREG(4)), camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.1f);
            goto cont;
        case 0x10:
            anim->unk_0C = NULL;
        cont:
            if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
                camera->player->stateFlags1 & 0x200000) {
                anim->unk_08 = playerPosRot->pos.y;
                sp80 = 0;
            } else {
                sp80 = 1;
            }

            Camera_CalcAtForLockOn(camera, &spB8, &camera->targetPosRot.pos, sp80 ? keep1->unk_28 : keep1->unk_00,
                                   sp104, &anim->unk_08, &spC8, (sp80 ? 0x80 : 0) | keep1->interfaceFlags);
            sp114 = playerPosRot->pos;
            sp114.y += playerHeight;
            OLib_Vec3fDiffToVecSphGeo(&spC8, &sp114, &camera->targetPosRot.pos);
            sp84 = spC8.r > sp104 ? 1.0f : spC8.r / sp104;
            break;
        default:
            *at = playerPosRot->pos;
            at->y += playerHeight;
            anim->unk_0C = NULL;
            break;
    }
    OLib_Vec3fDiffToVecSphGeo(&spD8, at, eyeNext);
    if (spD8.r < keep1->unk_04) {
        sp104 = keep1->unk_04;
        spE8 = OREG(6);
    } else if (keep1->unk_08 < spD8.r) {
        sp104 = keep1->unk_08;
        spE8 = OREG(6);
    } else {
        sp104 = spD8.r;
        spE8 = 1.0f;
    }

    camera->rUpdateRateInv = Camera_LERPCeilF(spE8, camera->rUpdateRateInv, PCT(OREG(25)), 0.1f);
    spD8.r = spE8 = camera->dist = Camera_LERPCeilF(sp104, camera->dist, 1.0f / camera->rUpdateRateInv, 0.2f);
    sp108 = camera->targetPosRot.pos;
    OLib_Vec3fDiffToVecSphGeo(&spD0, at, &sp108);
    spD0.r = spE8 - ((spD0.r <= spE8 ? spD0.r : spE8) * 0.5f);
    spEC = keep1->unk_0C + ((keep1->unk_10 - keep1->unk_0C) * (1.1f - sp84));
    spF0 = OREG(13) + spEC;
    spD8.r = camera->dist = Camera_LERPCeilF(spE8, camera->dist, PCT(OREG(11)), 2.0f);
    spD8.yaw = spB8.yaw;
    spE2 = BINANG_SUB(spD0.yaw, BINANG_ROT180(spB8.yaw));
    if (anim->unk_16 != 0) {
        if (anim->unk_16 >= OREG(24)) {
            sp82 = anim->unk_16 - OREG(24);
            spE2 = spC8.yaw;
            OLib_Vec3fDiffToVecSphGeo(&spC8, at, eye);
            spC8.yaw = BINANG_ROT180(spE2);

            t2 = 1.0f / OREG(23);
            spE8 = (anim->unk_00 - spC8.r) * t2;
            spE2 = BINANG_SUB(anim->unk_12, spC8.yaw) * t2;
            spE0 = BINANG_SUB(anim->unk_14, spC8.pitch) * t2;

            spD8.r = Camera_LERPCeilF(spC8.r + (spE8 * sp82), spC0.r, PCT(OREG(28)), 1.0f);
            spD8.yaw = Camera_LERPCeilS(spC8.yaw + (spE2 * sp82), spC0.yaw, PCT(OREG(28)), 0xA);
            spD8.pitch = Camera_LERPCeilS(spC8.pitch + (spE0 * sp82), spC0.pitch, PCT(OREG(28)), 0xA);
        } else {
            sp88 = 1;
        }
        anim->unk_16--;
    } else if (ABS(spE2) > DEGF_TO_BINANG(spEC)) {
        spF4 = BINANG_TO_DEGF(spE2);
        t2 = spEC + (spF0 - spEC) * (OLib_ClampMaxDist(spD0.r, spD8.r) / spD8.r);
        temp_f12_2 = ((SQ(t2) - 2.0f) / (t2 - 360.0f));
        t1 = (temp_f12_2 * spF4) + (2.0f - (360.0f * temp_f12_2));
        temp_f14 = SQ(spF4) / t1;
        spE0 = spE2 >= 0 ? (DEGF_TO_BINANG(temp_f14)) : (-DEGF_TO_BINANG(temp_f14));
        spD8.yaw = BINANG_ROT180((s16)(BINANG_ROT180(spB8.yaw) + spE0));
    } else {
        spF4 = 0.02f;
        spF4 = (1.0f - camera->speedRatio) * spF4;
        spE0 = spE2 >= 0 ? DEGF_TO_BINANG(spEC) : -DEGF_TO_BINANG(spEC);
        spD8.yaw = spB8.yaw - (s16)((spE0 - spE2) * spF4);
    }

    if (sp88 == 0) {
        spE2 = DEGF_TO_BINANG((f32)(keep1->unk_14 + ((keep1->unk_18 - keep1->unk_14) * sp84)));
        spE2 -= (s16)(spC8.pitch * (0.5f + (sp84 * 0.5f)));

        spE8 = spD0.pitch * keep1->unk_1C;
        spE2 += (s16)spE8;
        if (spE2 < -0x3200) {
            spE2 = -0x3200;
        } else if (spE2 > 0x3200) {
            spE2 = 0x3200;
        }

        spD8.pitch = Camera_LERPCeilS(spE2, spB8.pitch, PCT(OREG(12)), 0xA);
        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &spD8);
        sp8C.pos = *eyeNext;
        if (camera->status == CAM_STAT_ACTIVE) {
            if (!camera->play->envCtx.skyboxDisabled || keep1->interfaceFlags & 1) {
                Camera_BGCheckInfo(camera, at, &sp8C);
            } else if (keep1->interfaceFlags & 2) {
                func_80043F94(camera, at, &sp8C);
            } else {
                OLib_Vec3fDistNormalize(&sp120, at, &sp8C.pos);
                sp8C.pos.x -= sp120.x;
                sp8C.pos.y -= sp120.y;
                sp8C.pos.z -= sp120.z;
            }
            *eye = sp8C.pos;
        } else {
            *eye = *eyeNext;
        }
        OLib_Vec3fDistNormalize(&sp120, eye, at);
        Camera_Vec3fTranslateByUnitVector(eye, eye, &sp120, OREG(1));
    }
    camera->fov = Camera_LERPCeilF(keep1->unk_20, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, sp80 ? keep1->unk_2C : keep1->unk_24);
    return 1;
}

s32 Camera_KeepOn2(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Talking to an NPC
 */
s32 Camera_KeepOn3(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerHeadPos;
    Vec3f lineChkPointB;
    f32 temp_f0;
    f32 spBC;
    f32 prevTargetPlayerDist;
    f32 swingAngle;
    Actor* colChkActors[2];
    VecSph targetToPlayerDir;
    VecSph atToEyeAdj;
    VecSph atToEyeDir;
    VecSph atToEyeNextDir;
    s32 i;
    s32 angleCnt;
    s16 sp82;
    s16 sp80;
    PosRot playerPosRot;
    PosRot* camPlayerPosRot = &camera->playerPosRot;
    KeepOn3* keep3 = (KeepOn3*)camera->paramData;
    Keep3Anim* anim = &keep3->anim;
    s32 pad;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);
    if (camera->target == NULL || camera->target->update == NULL) {
        if (camera->target == NULL) {
            osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: warning: talk: target is not valid, change parallel\n" VT_RST);
        }
        camera->target = NULL;
        Camera_ChangeMode(camera, CAM_MODE_TARGET);
        return 1;
    }
    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        if (camera->play->view.unk_124 == 0) {
            camera->unk_14C |= 0x20;
            camera->play->view.unk_124 = camera->thisIdx | 0x50;
            return 1;
        }
        camera->unk_14C &= ~0x20;
    }
    camera->unk_14C &= ~0x10;
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(OREG(46))) - (PCT(OREG(46)) * (68.0f / playerHeight));

        keep3->yOffset = NEXTPCT * playerHeight * yNormal;
        keep3->minDist = NEXTSETTING;
        keep3->maxDist = NEXTSETTING;
        keep3->swingYawInital = NEXTSETTING;
        keep3->swingYawFinal = NEXTSETTING;
        keep3->swingPitchInitial = NEXTSETTING;
        keep3->swingPitchFinal = NEXTSETTING;
        keep3->swingPitchAdj = NEXTPCT;
        keep3->fovTarget = NEXTSETTING;
        keep3->atLERPScaleMax = NEXTPCT;
        keep3->initTimer = NEXTSETTING;
        keep3->flags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    playerHeight += keep3->yOffset;
    OLib_Vec3fDiffToVecSphGeo(&atToEyeDir, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&atToEyeNextDir, at, eyeNext);
    Actor_GetFocus(&camera->targetPosRot, camera->target);
    Actor_GetFocus(&playerPosRot, &camera->player->actor);
    playerHeadPos = camPlayerPosRot->pos;
    playerHeadPos.y += playerHeight;
    OLib_Vec3fDiffToVecSphGeo(&targetToPlayerDir, &playerHeadPos, &camera->targetPosRot.pos);
    sCameraInterfaceFlags = keep3->flags;
    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        colChkActors[0] = camera->target;
        colChkActors[1] = &camera->player->actor;
        camera->animState++;
        anim->target = camera->target;
        temp_f0 = (keep3->maxDist < targetToPlayerDir.r ? 1.0f : targetToPlayerDir.r / keep3->maxDist);
        anim->animTimer = keep3->initTimer;
        spBC = ((1.0f - temp_f0) * targetToPlayerDir.r) / anim->animTimer;
        swingAngle = F32_LERPIMP(keep3->swingPitchInitial, keep3->swingPitchFinal, temp_f0);
        atToEyeAdj.pitch = DEGF_TO_BINANG(swingAngle) + ((s16)(-(targetToPlayerDir.pitch * keep3->swingPitchAdj)));
        swingAngle = F32_LERPIMP(keep3->swingYawInital, keep3->swingYawFinal, temp_f0);
        if (keep3->flags & 0x10) {
            if (BINANG_SUB(targetToPlayerDir.yaw, atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = targetToPlayerDir.yaw + DEGF_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = targetToPlayerDir.yaw - DEGF_TO_BINANG(swingAngle);
            }
        } else if (keep3->flags & 0x20) {
            if (BINANG_SUB(targetToPlayerDir.yaw, atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = BINANG_ROT180(targetToPlayerDir.yaw) - DEGF_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = BINANG_ROT180(targetToPlayerDir.yaw) + DEGF_TO_BINANG(swingAngle);
            }
        } else if (ABS(BINANG_SUB(targetToPlayerDir.yaw, atToEyeNextDir.yaw)) < 0x3FFF) {
            if (BINANG_SUB(targetToPlayerDir.yaw, atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = targetToPlayerDir.yaw + DEGF_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = targetToPlayerDir.yaw - DEGF_TO_BINANG(swingAngle);
            }
        } else {
            if (BINANG_SUB(targetToPlayerDir.yaw, atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = BINANG_ROT180(targetToPlayerDir.yaw) - DEGF_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = BINANG_ROT180(targetToPlayerDir.yaw) + DEGF_TO_BINANG(swingAngle);
            }
        }
        prevTargetPlayerDist = targetToPlayerDir.r;
        temp_f0 = 0.6f;
        targetToPlayerDir.r = (spBC * 0.6f) + (prevTargetPlayerDist * (1.0f - temp_f0));
        sp80 = atToEyeAdj.yaw;
        sp82 = atToEyeAdj.pitch;
        playerHeadPos = camPlayerPosRot->pos;
        playerHeadPos.y += playerHeight;
        Camera_Vec3fVecSphGeoAdd(&anim->atTarget, &playerHeadPos, &targetToPlayerDir);
        angleCnt = ARRAY_COUNT(D_8011D3B0);
        i = 0;
        targetToPlayerDir.r = prevTargetPlayerDist;
        atToEyeAdj.r = ((keep3->minDist + (targetToPlayerDir.r * (1 - 0.5f))) - atToEyeNextDir.r) + atToEyeNextDir.r;
        Camera_Vec3fVecSphGeoAdd(&lineChkPointB, &anim->atTarget, &atToEyeAdj);
        if (!(keep3->flags & 0x80)) {
            while (i < angleCnt) {
                if (!CollisionCheck_LineOCCheck(camera->play, &camera->play->colChkCtx, &anim->atTarget,
                                                &lineChkPointB, colChkActors, 2) &&
                    !Camera_BGCheck(camera, &anim->atTarget, &lineChkPointB)) {
                    break;
                }
                atToEyeAdj.yaw = sp80 + D_8011D3B0[i];
                atToEyeAdj.pitch = sp82 + D_8011D3CC[i];
                Camera_Vec3fVecSphGeoAdd(&lineChkPointB, &anim->atTarget, &atToEyeAdj);
                i++;
            }
        }
        osSyncPrintf("camera: talk: BG&collision check %d time(s)\n", i);
        camera->unk_14C &= ~0xC;
        pad = ((anim->animTimer + 1) * anim->animTimer) >> 1;
        anim->eyeToAtTarget.y = (f32)BINANG_SUB(atToEyeAdj.yaw, atToEyeNextDir.yaw) / pad;
        anim->eyeToAtTarget.z = (f32)BINANG_SUB(atToEyeAdj.pitch, atToEyeNextDir.pitch) / pad;
        anim->eyeToAtTarget.x = (atToEyeAdj.r - atToEyeNextDir.r) / pad;
        return 1;
    }

    if (anim->animTimer != 0) {
        at->x += (anim->atTarget.x - at->x) / anim->animTimer;
        at->y += (anim->atTarget.y - at->y) / anim->animTimer;
        at->z += (anim->atTarget.z - at->z) / anim->animTimer;
        // needed to match
        //if (!prevTargetPlayerDist) {}
        atToEyeAdj.r = ((anim->eyeToAtTarget.x * anim->animTimer) + atToEyeNextDir.r) + 1.0f;
        atToEyeAdj.yaw = atToEyeNextDir.yaw + (s16)(anim->eyeToAtTarget.y * anim->animTimer);
        atToEyeAdj.pitch = atToEyeNextDir.pitch + (s16)(anim->eyeToAtTarget.z * anim->animTimer);
        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &atToEyeAdj);
        *eye = *eyeNext;
        camera->fov = Camera_LERPCeilF(keep3->fovTarget, camera->fov, 0.5, 1.0f);
        camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5, 0xA);
        camera->atLERPStepScale = Camera_ClampLERPScale(camera, keep3->atLERPScaleMax);
        Camera_BGCheck(camera, at, eye);
        anim->animTimer--;
    } else {
        camera->unk_14C |= 0x410;
    }

    if (camera->unk_14C & 8) {
        sCameraInterfaceFlags = 0;
        func_80043B60(camera);
        camera->atLERPStepScale = 0.0f;

        if (camera->xzSpeed > 0.001f || CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_A) ||
            CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_B) ||
            CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CLEFT) ||
            CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CDOWN) ||
            CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CUP) ||
            CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CRIGHT) ||
            CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_R) ||
            CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_Z)) {
            camera->unk_14C |= 4;
            camera->unk_14C &= ~8;
        }
    }
    return 1;
}

s32 Camera_KeepOn4(Camera* camera) {
    static Vec3f D_8015BD50;
    static Vec3f D_8015BD60;
    static Vec3f D_8015BD70;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Actor* spCC[2];
    f32 t = -0.5f;
    f32 temp_f0_2;
    CollisionPoly* spC0;
    VecSph spB8;
    VecSph spB0;
    VecSph spA8;
    s16* temp_s0 = &camera->data2;
    s16 spA2;
    s16 spA0;
    s16 sp9E;
    s16 sp9C;
    PosRot* playerPosRot = &camera->playerPosRot;
    KeepOn4* keep4 = (KeepOn4*)camera->paramData;
    KeepOn4_Unk20* unk20 = &keep4->unk_20;
    s32 pad;
    f32 playerHeight;
    Player* player = GET_PLAYER(camera->play);
    s16 angleCnt;
    s32 i;

    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        if (camera->play->view.unk_124 == 0) {
            camera->unk_14C |= 0x20;
            camera->unk_14C &= ~(0x4 | 0x2);
            camera->play->view.unk_124 = camera->thisIdx | 0x50;
            return 1;
        }
        unk20->unk_14 = *temp_s0;
        camera->unk_14C &= ~0x20;
    }

    if (unk20->unk_14 != *temp_s0) {
        osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: item: item type changed %d -> %d\n" VT_RST, unk20->unk_14,
                     *temp_s0);
        camera->animState = 0x14;
        camera->unk_14C |= 0x20;
        camera->unk_14C &= ~(0x4 | 0x2);
        camera->play->view.unk_124 = camera->thisIdx | 0x50;
        return 1;
    }

    playerHeight = Player_GetHeight(camera->player);
    camera->unk_14C &= ~0x10;
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + t) - ((68.0f / playerHeight) * t);

        keep4->unk_00 = NEXTPCT * playerHeight * yNormal;
        keep4->unk_04 = NEXTPCT * playerHeight * yNormal;
        keep4->unk_08 = NEXTSETTING;
        keep4->unk_0C = NEXTSETTING;
        keep4->unk_10 = NEXTSETTING;
        keep4->unk_18 = NEXTSETTING;
        keep4->unk_1C = NEXTSETTING;
        keep4->unk_14 = NEXTPCT;
        keep4->unk_1E = NEXTSETTING;
        osSyncPrintf("camera: item: type %d\n", *temp_s0);
        switch (*temp_s0) {
            case 1:
                keep4->unk_00 = playerHeight * -0.6f * yNormal;
                keep4->unk_04 = playerHeight * 2.0f * yNormal;
                keep4->unk_08 = 10.0f;
                break;
            case 2:
            case 3:
                keep4->unk_08 = -20.0f;
                keep4->unk_18 = 80.0f;
                break;
            case 4:
                keep4->unk_00 = playerHeight * -0.2f * yNormal;
                keep4->unk_08 = 25.0f;
                break;
            case 8:
                keep4->unk_00 = playerHeight * -0.2f * yNormal;
                keep4->unk_04 = playerHeight * 0.8f * yNormal;
                keep4->unk_08 = 50.0f;
                keep4->unk_18 = 70.0f;
                break;
            case 9:
                keep4->unk_00 = playerHeight * 0.1f * yNormal;
                keep4->unk_04 = playerHeight * 0.5f * yNormal;
                keep4->unk_08 = -20.0f;
                keep4->unk_0C = 0.0f;
                keep4->unk_1C = 0x2540;
                break;
            case 5:
                keep4->unk_00 = playerHeight * -0.4f * yNormal;
                keep4->unk_08 = -10.0f;
                keep4->unk_0C = 45.0f;
                keep4->unk_1C = 0x2002;
                break;
            case 10:
                keep4->unk_00 = playerHeight * -0.5f * yNormal;
                keep4->unk_04 = playerHeight * 1.5f * yNormal;
                keep4->unk_08 = -15.0f;
                keep4->unk_0C = 175.0f;
                keep4->unk_18 = 70.0f;
                keep4->unk_1C = 0x2202;
                keep4->unk_1E = 0x3C;
                break;
            case 12:
                keep4->unk_00 = playerHeight * -0.6f * yNormal;
                keep4->unk_04 = playerHeight * 1.6f * yNormal;
                keep4->unk_08 = -2.0f;
                keep4->unk_0C = 120.0f;
                keep4->unk_10 = player->stateFlags1 & 0x8000000 ? 0.0f : 20.0f;
                keep4->unk_1C = 0x3212;
                keep4->unk_1E = 0x1E;
                keep4->unk_18 = 50.0f;
                break;
            case 0x5A:
                keep4->unk_00 = playerHeight * -0.3f * yNormal;
                keep4->unk_18 = 45.0f;
                keep4->unk_1C = 0x2F02;
                break;
            case 0x5B:
                keep4->unk_00 = playerHeight * -0.1f * yNormal;
                keep4->unk_04 = playerHeight * 1.5f * yNormal;
                keep4->unk_08 = -3.0f;
                keep4->unk_0C = 10.0f;
                keep4->unk_18 = 55.0f;
                keep4->unk_1C = 0x2F08;
                break;
            case 0x51:
                keep4->unk_00 = playerHeight * -0.3f * yNormal;
                keep4->unk_04 = playerHeight * 1.5f * yNormal;
                keep4->unk_08 = 2.0f;
                keep4->unk_0C = 20.0f;
                keep4->unk_10 = 20.0f;
                keep4->unk_1C = 0x2280;
                keep4->unk_1E = 0x1E;
                keep4->unk_18 = 45.0f;
                break;
            case 11:
                keep4->unk_00 = playerHeight * -0.19f * yNormal;
                keep4->unk_04 = playerHeight * 0.7f * yNormal;
                keep4->unk_0C = 130.0f;
                keep4->unk_10 = 10.0f;
                keep4->unk_1C = 0x2522;
                break;
            default:
                break;
        }
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sUpdateCameraDirection = 1;
    sCameraInterfaceFlags = keep4->unk_1C;
    OLib_Vec3fDiffToVecSphGeo(&spB0, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&spA8, at, eyeNext);
    D_8015BD50 = playerPosRot->pos;
    D_8015BD50.y += playerHeight;
    temp_f0_2 = BgCheck_CameraRaycastFloor2(&camera->play->colCtx, &spC0, &i, &D_8015BD50);
    if (temp_f0_2 > (keep4->unk_00 + D_8015BD50.y)) {
        D_8015BD50.y = temp_f0_2 + 10.0f;
    } else {
        D_8015BD50.y += keep4->unk_00;
    }

    sp9C = 0;
    switch (camera->animState) {
        case 0:
        case 0x14:
            spCC[sp9C] = &camera->player->actor;
            sp9C++;
            func_80043ABC(camera);
            camera->unk_14C &= ~(0x4 | 0x2);
            unk20->unk_10 = keep4->unk_1E;
            unk20->unk_08 = playerPosRot->pos.y - camera->playerPosDelta.y;
            if (keep4->unk_1C & 2) {
                spA2 = DEGF_TO_BINANG(keep4->unk_08);
                spA0 = BINANG_SUB(BINANG_ROT180(playerPosRot->rot.y), spA8.yaw) > 0
                           ? BINANG_ROT180(playerPosRot->rot.y) + DEGF_TO_BINANG(keep4->unk_0C)
                           : BINANG_ROT180(playerPosRot->rot.y) - DEGF_TO_BINANG(keep4->unk_0C);
            } else if (keep4->unk_1C & 4) {
                spA2 = DEGF_TO_BINANG(keep4->unk_08);
                spA0 = DEGF_TO_BINANG(keep4->unk_0C);
            } else if ((keep4->unk_1C & 8) && camera->target != NULL) {
                PosRot sp60;

                Actor_GetWorldPosShapeRot(&sp60, camera->target);
                spA2 = DEGF_TO_BINANG(keep4->unk_08) - sp60.rot.x;
                spA0 = BINANG_SUB(BINANG_ROT180(sp60.rot.y), spA8.yaw) > 0
                           ? BINANG_ROT180(sp60.rot.y) + DEGF_TO_BINANG(keep4->unk_0C)
                           : BINANG_ROT180(sp60.rot.y) - DEGF_TO_BINANG(keep4->unk_0C);
                spCC[1] = camera->target;
                sp9C++;
            } else if ((keep4->unk_1C & 0x80) && camera->target != NULL) {
                PosRot sp4C;

                Actor_GetWorld(&sp4C, camera->target);
                spA2 = DEGF_TO_BINANG(keep4->unk_08);
                sp9E = Camera_XZAngle(&sp4C.pos, &playerPosRot->pos);
                spA0 = (BINANG_SUB(sp9E, spA8.yaw) > 0) ? sp9E + DEGF_TO_BINANG(keep4->unk_0C)
                                                        : sp9E - DEGF_TO_BINANG(keep4->unk_0C);
                spCC[1] = camera->target;
                sp9C++;
            } else if (keep4->unk_1C & 0x40) {
                spA2 = DEGF_TO_BINANG(keep4->unk_08);
                spA0 = spA8.yaw;
            } else {
                spA2 = spA8.pitch;
                spA0 = spA8.yaw;
            }

            spB8.pitch = spA2;
            spB8.yaw = spA0;
            spB8.r = keep4->unk_04;
            Camera_Vec3fVecSphGeoAdd(&D_8015BD70, &D_8015BD50, &spB8);
            if (!(keep4->unk_1C & 1)) {
                angleCnt = ARRAY_COUNT(D_8011D3B0);
                for (i = 0; i < angleCnt; i++) {
                    if (!CollisionCheck_LineOCCheck(camera->play, &camera->play->colChkCtx, &D_8015BD50,
                                                    &D_8015BD70, spCC, sp9C) &&
                        !Camera_BGCheck(camera, &D_8015BD50, &D_8015BD70)) {
                        break;
                    }
                    spB8.yaw = D_8011D3B0[i] + spA0;
                    spB8.pitch = D_8011D3CC[i] + spA2;
                    Camera_Vec3fVecSphGeoAdd(&D_8015BD70, &D_8015BD50, &spB8);
                }
                osSyncPrintf("camera: item: BG&collision check %d time(s)\n", i);
            }
            unk20->unk_04 = BINANG_SUB(spB8.pitch, spA8.pitch) / (f32)unk20->unk_10;
            unk20->unk_00 = BINANG_SUB(spB8.yaw, spA8.yaw) / (f32)unk20->unk_10;
            unk20->unk_0C = spA8.yaw;
            unk20->unk_0E = spA8.pitch;
            camera->animState++;
            unk20->unk_12 = 1;
            break;
        case 0xA:
            unk20->unk_08 = playerPosRot->pos.y - camera->playerPosDelta.y;
        default:
            break;
    }
    camera->xzOffsetUpdateRate = 0.25f;
    camera->yOffsetUpdateRate = 0.25f;
    camera->atLERPStepScale = 0.75f;
    Camera_LERPCeilVec3f(&D_8015BD50, at, 0.5f, 0.5f, 0.2f);
    if (keep4->unk_10 != 0.0f) {
        spB8.r = keep4->unk_10;
        spB8.pitch = 0;
        spB8.yaw = playerPosRot->rot.y;
        Camera_Vec3fVecSphGeoAdd(at, at, &spB8);
    }
    camera->atLERPStepScale = 0.0f;
    camera->dist = Camera_LERPCeilF(keep4->unk_04, camera->dist, 0.25f, 2.0f);
    spB8.r = camera->dist;
    if (unk20->unk_10 != 0) {
        camera->unk_14C |= 0x20;
        unk20->unk_0C += (s16)unk20->unk_00;
        unk20->unk_0E += (s16)unk20->unk_04;
        unk20->unk_10--;
    } else if (keep4->unk_1C & 0x10) {
        camera->unk_14C |= (0x400 | 0x10);
        camera->unk_14C |= (0x4 | 0x2);
        camera->unk_14C &= ~8;
        if (camera->timer > 0) {
            camera->timer--;
        }
    } else {
        camera->unk_14C |= (0x400 | 0x10);
        if (camera->unk_14C & 8 || keep4->unk_1C & 0x80) {
            sCameraInterfaceFlags = 0;
            camera->unk_14C |= (0x4 | 0x2);
            camera->unk_14C &= ~8;
            if (camera->prevCamDataIdx < 0) {
                Camera_ChangeSettingFlags(camera, camera->prevSetting, 2);
            } else {
                Camera_ChangeDataIdx(camera, camera->prevCamDataIdx);
                camera->prevCamDataIdx = -1;
            }
        }
    }
    spB8.yaw = Camera_LERPCeilS(unk20->unk_0C, spA8.yaw, keep4->unk_14, 4);
    spB8.pitch = Camera_LERPCeilS(unk20->unk_0E, spA8.pitch, keep4->unk_14, 4);
    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &spB8);
    *eye = *eyeNext;
    Camera_BGCheck(camera, at, eye);
    camera->fov = Camera_LERPCeilF(keep4->unk_18, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
}

/**
 * Talking in a pre-rendered room
 */
s32 Camera_KeepOn0(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* at = &camera->at;
    VecSph eyeTargetPosOffset;
    VecSph eyeAtOffset;
    KeepOn0* keep0 = (KeepOn0*)camera->paramData;
    KeepOn0Anim* anim = &keep0->anim;
    s32 pad;
    Vec3s* sceneCamData;
    Vec3s sceneCamRot;
    s16 fov;

    camera->unk_14C &= ~0x10;
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        keep0->fovScale = NEXTPCT;
        keep0->yawScale = NEXTPCT;
        keep0->timerInit = NEXTSETTING;
        keep0->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }
    sceneCamData = Camera_GetCamBGData(camera);
    Camera_Vec3sToVec3f(eyeNext, &BGCAM_POS(sceneCamData));
    *eye = *eyeNext;

    sceneCamRot = BGCAM_ROT(sceneCamData); // unused
    (void)sceneCamRot;                     // suppresses set but unused warning

    fov = BGCAM_FOV(sceneCamData);
    if (fov == -1) {
        fov = 6000;
    }

    if (camera->target == NULL || camera->target->update == NULL) {
        if (camera->target == NULL) {
            osSyncPrintf(
                VT_COL(YELLOW, BLACK) "camera: warning: talk: target is not valid, change normal camera\n" VT_RST);
        }
        camera->target = NULL;
        Camera_ChangeMode(camera, CAM_MODE_NORMAL);
        return true;
    }

    Actor_GetFocus(&camera->targetPosRot, camera->target);

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, eye, at);
    OLib_Vec3fDiffToVecSphGeo(&eyeTargetPosOffset, eye, &camera->targetPosRot.pos);

    sCameraInterfaceFlags = keep0->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        camera->fov = PCT(fov);
        camera->roll = 0;
        camera->atLERPStepScale = 0.0f;
        anim->animTimer = keep0->timerInit;
        anim->fovTarget = camera->fov - (camera->fov * keep0->fovScale);
    }

    if (anim->animTimer != 0) {
        eyeAtOffset.yaw += (BINANG_SUB(eyeTargetPosOffset.yaw, eyeAtOffset.yaw) / anim->animTimer) * keep0->yawScale;
        Camera_Vec3fVecSphGeoAdd(at, eye, &eyeAtOffset);
        anim->animTimer--;
    } else {
        camera->unk_14C |= (0x400 | 0x10);
    }
    camera->fov = Camera_LERPCeilF(anim->fovTarget, camera->fov, 0.5f, 10.0f);
    return true;
}

s32 Camera_Fixed1(Camera* camera) {
    Fixed1* fixd1 = (Fixed1*)camera->paramData;
    Fixed1Anim* anim = &fixd1->anim;
    s32 pad;
    VecSph eyeOffset;
    VecSph eyeAtOffset;
    s32 pad2;
    Vec3f adjustedPos;
    Vec3s* scenePosData;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        scenePosData = Camera_GetCamBGData(camera);
        Camera_Vec3sToVec3f(&anim->eyePosRotTarget.pos, &BGCAM_POS(scenePosData));
        anim->eyePosRotTarget.rot = BGCAM_ROT(scenePosData);
        anim->fov = BGCAM_FOV(scenePosData);
        fixd1->unk_00 = NEXTPCT * playerHeight;
        fixd1->lerpStep = NEXTPCT;
        fixd1->fov = NEXTSETTING;
        fixd1->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }
    if (anim->fov == -1) {
        anim->fov = fixd1->fov * 100.0f;
    } else if (anim->fov < 361) {
        anim->fov *= 100;
    }

    sCameraInterfaceFlags = fixd1->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        func_80043B60(camera);
        if (anim->fov != -1) {
            fixd1->fov = PCT(anim->fov);
        }
    }

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, eye, at);

    Camera_LERPCeilVec3f(&anim->eyePosRotTarget.pos, eye, 0.1f, 0.1f, 0.2f);
    adjustedPos = playerPosRot->pos;
    adjustedPos.y += playerHeight;
    camera->dist = OLib_Vec3fDist(&adjustedPos, eye);

    eyeOffset.r = camera->dist;
    eyeOffset.pitch = Camera_LERPCeilS(-anim->eyePosRotTarget.rot.x, eyeAtOffset.pitch, fixd1->lerpStep, 5);
    eyeOffset.yaw = Camera_LERPCeilS(anim->eyePosRotTarget.rot.y, eyeAtOffset.yaw, fixd1->lerpStep, 5);

    Camera_Vec3fVecSphGeoAdd(at, eye, &eyeOffset);

    camera->eyeNext = *eye;

    camera->fov = Camera_LERPCeilF(fixd1->fov, camera->fov, fixd1->lerpStep, 0.01f);
    camera->roll = 0;
    camera->atLERPStepScale = 0.0f;

    camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->posOffset.z = camera->at.z - playerPosRot->pos.z;

    return true;
}

s32 Camera_Fixed2(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f atTarget;
    Vec3f posOffsetTarget;
    PosRot* playerPosRot = &camera->playerPosRot;
    Vec3s* scenePosData;
    Fixed2* fixd2 = (Fixed2*)camera->paramData;
    Fixed2InitParams* initParams = &fixd2->initParams;
    s32 pad;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));
        fixd2->yOffset = (NEXTPCT * playerHeight) * yNormal;
        fixd2->eyeStepScale = NEXTPCT;
        fixd2->posStepScale = NEXTPCT;
        fixd2->fov = NEXTSETTING;
        fixd2->interfaceFlags = NEXTSETTING;
        initParams->fov = fixd2->fov * 100.0f;
        scenePosData = Camera_GetCamBGData(camera);
        if (scenePosData != NULL) {
            Camera_Vec3sToVec3f(&initParams->eye, &BGCAM_POS(scenePosData));
            if (BGCAM_FOV(scenePosData) != -1) {
                initParams->fov = BGCAM_FOV(scenePosData);
            }
        } else {
            initParams->eye = *eye;
        }
        if (initParams->fov < 361) {
            initParams->fov *= 100;
        }
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = fixd2->interfaceFlags;

    posOffsetTarget.x = 0.0f;
    posOffsetTarget.y = fixd2->yOffset + playerHeight;
    posOffsetTarget.z = 0.0f;

    Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, fixd2->posStepScale, fixd2->posStepScale, 0.1f);
    atTarget.x = playerPosRot->pos.x + camera->posOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->posOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->posOffset.z;
    if (camera->animState == 0) {
        camera->animState++;
        func_80043B60(camera);
        if (!(fixd2->interfaceFlags & 1)) {
            *eye = *eyeNext = initParams->eye;
            camera->at = atTarget;
        }
    }

    Camera_LERPCeilVec3f(&atTarget, &camera->at, fixd2->posStepScale, fixd2->posStepScale, 10.0f);
    Camera_LERPCeilVec3f(&initParams->eye, eyeNext, fixd2->eyeStepScale, fixd2->eyeStepScale, 0.1f);

    *eye = *eyeNext;
    camera->dist = OLib_Vec3fDist(at, eye);
    camera->roll = 0;
    camera->xzSpeed = 0.0f;
    camera->fov = PCT(initParams->fov);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, 1.0f);
    camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->posOffset.z = camera->at.z - playerPosRot->pos.z;
    return true;
}

/**
 * Camera's position is fixed, does not move, or rotate
 */
s32 Camera_Fixed3(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecSph atSph;
    Vec3s* sceneCamData;
    VecSph eyeAtOffset;
    Fixed3* fixd3 = (Fixed3*)camera->paramData;
    Fixed3Anim* anim = &fixd3->anim;
    s32 pad;

    sceneCamData = Camera_GetCamBGData(camera);

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, eye, at);

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        fixd3->interfaceFlags = NEXTSETTING;
        Camera_Vec3sToVec3f(eyeNext, &BGCAM_POS(sceneCamData));
        *eye = *eyeNext;
        anim->rot = BGCAM_ROT(sceneCamData);
        anim->fov = BGCAM_FOV(sceneCamData);
        anim->jfifId = BGCAM_JFIFID(sceneCamData);
        if (anim->fov == -1) {
            anim->fov = 6000;
        }
        if (anim->fov <= 360) {
            anim->fov *= 100;
        }
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    if (camera->animState == 0) {
        anim->updDirTimer = 5;
        R_CAM_DATA(CAM_DATA_FOV) = anim->fov;
        camera->animState++;
    }

    if (BGCAM_JFIFID(sceneCamData) != anim->jfifId) {
        osSyncPrintf("camera: position change %d \n", anim->jfifId);
        anim->jfifId = BGCAM_JFIFID(sceneCamData);
        anim->updDirTimer = 5;
    }

    if (anim->updDirTimer > 0) {
        anim->updDirTimer--;
        sUpdateCameraDirection = true;
    } else {
        sUpdateCameraDirection = false;
    }

    atSph.r = 150.0f;
    atSph.yaw = anim->rot.y;
    atSph.pitch = -anim->rot.x;

    Camera_Vec3fVecSphGeoAdd(at, eye, &atSph);
    sCameraInterfaceFlags = fixd3->interfaceFlags;
    anim->fov = R_CAM_DATA(CAM_DATA_FOV);
    camera->roll = 0;
    camera->fov = anim->fov * 0.01f;
    camera->atLERPStepScale = 0.0f;
    return true;
}

/**
 * camera follow player, eye is in a fixed offset of the previous eye, and a value
 * specified in the scene.
 */
s32 Camera_Fixed4(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerPosWithCamOffset;
    Vec3f atTarget;
    Vec3f posOffsetTarget;
    VecSph atEyeNextOffset;
    VecSph atTargetEyeNextOffset;
    PosRot* playerPosRot = &camera->playerPosRot;
    Vec3s* camPosData;
    Vec3f* posOffset = &camera->posOffset;
    Fixed4* fixed4 = (Fixed4*)camera->paramData;
    Fixed4Anim* anim = &fixed4->anim;
    f32 playerYOffset;

    playerYOffset = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = ((1.0f + PCT(OREG(46))) - (PCT(OREG(46)) * (68.0f / playerYOffset)));
        fixed4->yOffset = NEXTPCT * playerYOffset * yNormal;
        fixed4->speedToEyePos = NEXTPCT;
        fixed4->followSpeed = NEXTPCT;
        fixed4->fov = NEXTSETTING;
        fixed4->interfaceFlags = NEXTSETTING;
        camPosData = Camera_GetCamBGData(camera);
        if (camPosData != NULL) {
            Camera_Vec3sToVec3f(&anim->eyeTarget, &BGCAM_POS(camPosData));
        } else {
            anim->eyeTarget = *eye;
        }
    }
    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }
    sCameraInterfaceFlags = fixed4->interfaceFlags;
    if (camera->animState == 0) {
        camera->animState++;
        if (!(fixed4->interfaceFlags & 4)) {
            func_80043B60(camera);
        }
        anim->followSpeed = fixed4->followSpeed;
    }

    VEC3F_LERPIMPDST(eyeNext, eyeNext, &anim->eyeTarget, fixed4->speedToEyePos);
    *eye = *eyeNext;

    posOffsetTarget.x = 0.0f;
    posOffsetTarget.y = fixed4->yOffset + playerYOffset;
    posOffsetTarget.z = 0.0f;
    Camera_LERPCeilVec3f(&posOffsetTarget, &camera->posOffset, 0.1f, 0.1f, 0.1f);

    playerPosWithCamOffset.x = playerPosRot->pos.x + camera->posOffset.x;
    playerPosWithCamOffset.y = playerPosRot->pos.y + camera->posOffset.y;
    playerPosWithCamOffset.z = playerPosRot->pos.z + camera->posOffset.z;
    VEC3F_LERPIMPDST(&atTarget, at, &playerPosWithCamOffset, 0.5f);

    OLib_Vec3fDiffToVecSphGeo(&atEyeNextOffset, eyeNext, at);
    OLib_Vec3fDiffToVecSphGeo(&atTargetEyeNextOffset, eyeNext, &atTarget);

    atEyeNextOffset.r += (atTargetEyeNextOffset.r - atEyeNextOffset.r) * anim->followSpeed;
    atEyeNextOffset.pitch = Camera_LERPCeilS(atTargetEyeNextOffset.pitch, atEyeNextOffset.pitch,
                                             anim->followSpeed * camera->speedRatio, 0xA);
    atEyeNextOffset.yaw =
        Camera_LERPCeilS(atTargetEyeNextOffset.yaw, atEyeNextOffset.yaw, anim->followSpeed * camera->speedRatio, 0xA);
    Camera_Vec3fVecSphGeoAdd(at, eyeNext, &atEyeNextOffset);
    camera->dist = OLib_Vec3fDist(at, eye);
    camera->roll = 0;
    camera->fov = fixed4->fov;
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, 1.0f);
    return true;
}

s32 Camera_Fixed0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Subj1(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Subj2(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * First person view
 */
s32 Camera_Subj3(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp98;
    Vec3f sp8C;
    VecSph sp84;
    VecSph sp7C;
    VecSph tsph;
    PosRot sp60;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 sp58;
    f32 temp_f0_3;
    s16 sp52;
    s16 sp50;
    Subj3* subj3 = (Subj3*)camera->paramData;
    Subj3Anim* anim = &subj3->anim;
    CameraModeValue* values;
    Vec3f* pad2;
    f32 playerHeight;

    Actor_GetFocus(&sp60, &camera->player->actor);
    playerHeight = Player_GetHeight(camera->player);

    if (camera->play->view.unk_124 == 0) {
        camera->play->view.unk_124 = camera->thisIdx | 0x50;
        return true;
    }

    func_80043ABC(camera);
    Camera_CopyPREGToModeValues(camera);
    values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
    subj3->eyeNextYOffset = NEXTPCT * playerHeight;
    subj3->eyeDist = NEXTSETTING;
    subj3->eyeNextDist = NEXTSETTING;
    subj3->unk_0C = NEXTSETTING;
    subj3->atOffset.x = NEXTSETTING * 0.1f;
    subj3->atOffset.y = NEXTSETTING * 0.1f;
    subj3->atOffset.z = NEXTSETTING * 0.1f;
    subj3->fovTarget = NEXTSETTING;
    subj3->interfaceFlags = NEXTSETTING;
    sp84.r = subj3->eyeNextDist;
    sp84.yaw = BINANG_ROT180(sp60.rot.y);
    sp84.pitch = sp60.rot.x;
    sp98 = sp60.pos;
    sp98.y += subj3->eyeNextYOffset;

    Camera_Vec3fVecSphGeoAdd(&sp8C, &sp98, &sp84);
    OLib_Vec3fDiffToVecSphGeo(&sp7C, at, eye);

    sCameraInterfaceFlags = subj3->interfaceFlags;
    if (camera->animState == 0 || camera->animState == 0xA || camera->animState == 0x14) {
        anim->r = sp7C.r;
        anim->yaw = sp7C.yaw;
        anim->pitch = sp7C.pitch;
        anim->animTimer = OREG(23);
        camera->dist = subj3->eyeNextDist;
        camera->animState++;
        camera->rUpdateRateInv = 1.0f;
        camera->dist = subj3->eyeNextDist;
    }

    tsph.r = anim->r;
    tsph.yaw = anim->yaw;
    tsph.pitch = anim->pitch;
    if (anim->animTimer != 0) {
        temp_f0_3 = (1.0f / anim->animTimer);
        pad2 = at;
        at->x = at->x + (sp98.x - pad2->x) * temp_f0_3;
        at->y = at->y + (sp98.y - pad2->y) * temp_f0_3;
        at->z = at->z + (sp98.z - pad2->z) * temp_f0_3;

        temp_f0_3 = (1.0f / OREG(23));
        sp58 = (tsph.r - sp84.r) * temp_f0_3;
        sp52 = BINANG_SUB(tsph.yaw, sp84.yaw) * temp_f0_3;
        sp50 = BINANG_SUB(tsph.pitch, sp84.pitch) * temp_f0_3;

        sp7C.r = Camera_LERPCeilF(sp84.r + (sp58 * anim->animTimer), sp7C.r, PCT(OREG(28)), 1.0f);
        sp7C.yaw = Camera_LERPCeilS(sp84.yaw + (sp52 * anim->animTimer), sp7C.yaw, PCT(OREG(28)), 0xA);
        sp7C.pitch = Camera_LERPCeilS(sp84.pitch + (sp50 * anim->animTimer), sp7C.pitch, PCT(OREG(28)), 0xA);
        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &sp7C);

        *eye = *eyeNext;
        anim->animTimer--;

        if (!camera->play->envCtx.skyboxDisabled) {
            Camera_BGCheck(camera, at, eye);
        } else {
            func_80044340(camera, at, eye);
        }
    } else {
        sp58 = Math_SinS(-sp60.rot.x);
        temp_f0_3 = Math_CosS(-sp60.rot.x);
        sp98.x = subj3->atOffset.x;
        sp98.y = (subj3->atOffset.y * temp_f0_3) - (subj3->atOffset.z * sp58);
        sp98.z = (subj3->atOffset.y * sp58) + (subj3->atOffset.z * temp_f0_3);
        sp58 = Math_SinS(BINANG_ROT180(sp60.rot.y));
        temp_f0_3 = Math_CosS(BINANG_ROT180(sp60.rot.y));
        subj3->atOffset.x = (sp98.z * sp58) + (sp98.x * temp_f0_3);
        subj3->atOffset.y = sp98.y;
        subj3->atOffset.z = (sp98.z * temp_f0_3) - (sp98.x * sp58);
        at->x = subj3->atOffset.x + sp60.pos.x;
        at->y = subj3->atOffset.y + sp60.pos.y;
        at->z = subj3->atOffset.z + sp60.pos.z;
        sp7C.r = subj3->eyeNextDist;
        sp7C.yaw = BINANG_ROT180(sp60.rot.y);
        sp7C.pitch = sp60.rot.x;
        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &sp7C);
        sp7C.r = subj3->eyeDist;
        Camera_Vec3fVecSphGeoAdd(eye, at, &sp7C);
    }

    camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->posOffset.z = camera->at.z - playerPosRot->pos.z;
    camera->fov = Camera_LERPCeilF(subj3->fovTarget, camera->fov, 0.25f, 1.0f);
    camera->roll = 0;
    camera->atLERPStepScale = 0.0f;
    return 1;
}

s32 Camera_Subj4(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* at = &camera->at;
    u16 spAA;
    Vec3s* spA4;
    Vec3f sp98;
    Vec3f sp8C;
    f32 sp88;
    s16 pad2;
    f32 temp_f16;
    PosRot sp6C;
    VecSph sp64;
    VecSph sp5C;
    s16 temp_a0;
    f32 tx;
    Player* player;
    PosRot* playerPosRot = &camera->playerPosRot;
    Subj4* subj4 = (Subj4*)camera->paramData;
    Subj4Anim* anim = &subj4->anim;

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        subj4->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    if (camera->play->view.unk_124 == 0) {
        camera->play->view.unk_124 = (camera->thisIdx | 0x50);
        anim->unk_24 = camera->xzSpeed;
        return true;
    }

    Actor_GetWorldPosShapeRot(&sp6C, &camera->player->actor);

    OLib_Vec3fDiffToVecSphGeo(&sp5C, at, eye);
    sCameraInterfaceFlags = subj4->interfaceFlags;
    if (camera->animState == 0) {
        spA4 = Camera_GetCamBgDataUnderPlayer(camera, &spAA);
        Camera_Vec3sToVec3f(&anim->unk_00.a, &spA4[1]);
        Camera_Vec3sToVec3f(&sp98, &spA4[spAA - 2]);

        sp64.r = 10.0f;
        // 0x238C ~ 50 degrees
        sp64.pitch = 0x238C;
        sp64.yaw = Camera_XZAngle(&sp98, &anim->unk_00.a);
        sp88 = OLib_Vec3fDist(&playerPosRot->pos, &anim->unk_00.a);
        if (OLib_Vec3fDist(&playerPosRot->pos, &sp98) < sp88) {
            anim->unk_00.b.x = anim->unk_00.a.x - sp98.x;
            anim->unk_00.b.y = anim->unk_00.a.y - sp98.y;
            anim->unk_00.b.z = anim->unk_00.a.z - sp98.z;
            anim->unk_00.a = sp98;
        } else {
            anim->unk_00.b.x = sp98.x - anim->unk_00.a.x;
            anim->unk_00.b.y = sp98.y - anim->unk_00.a.y;
            anim->unk_00.b.z = sp98.z - anim->unk_00.a.z;
            sp64.yaw = BINANG_ROT180(sp64.yaw);
        }
        anim->unk_30 = sp64.yaw;
        anim->unk_32 = 0xA;
        anim->unk_2C = 0;
        anim->unk_2E = false;
        anim->unk_28 = 0.0f;
        camera->animState++;
    }

    if (anim->unk_32 != 0) {
        sp64.r = 10.0f;
        sp64.pitch = 0x238C;
        sp64.yaw = anim->unk_30;
        Camera_Vec3fVecSphGeoAdd(&sp8C, &sp6C.pos, &sp64);
        sp88 = (anim->unk_32 + 1.0f);
        at->x += (sp8C.x - at->x) / sp88;
        at->y += (sp8C.y - at->y) / sp88;
        at->z += (sp8C.z - at->z) / sp88;
        sp5C.r -= (sp5C.r / sp88);
        sp5C.yaw = BINANG_LERPIMPINV(sp5C.yaw, BINANG_ROT180(sp6C.rot.y), anim->unk_32);
        sp5C.pitch = BINANG_LERPIMPINV(sp5C.pitch, sp6C.rot.x, anim->unk_32);
        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &sp5C);
        *eye = *eyeNext;
        anim->unk_32--;
        return false;
    } else if (anim->unk_24 < 0.5f) {
        return false;
    }

    Actor_GetWorldPosShapeRot(&sp6C, &camera->player->actor);
    Math3D_LineClosestToPoint(&anim->unk_00, &sp6C.pos, eyeNext);
    at->x = eyeNext->x + anim->unk_00.b.x;
    at->y = eyeNext->y + anim->unk_00.b.y;
    at->z = eyeNext->z + anim->unk_00.b.z;
    *eye = *eyeNext;
    sp64.yaw = anim->unk_30;
    sp64.r = 5.0f;
    sp64.pitch = 0x238C;
    Camera_Vec3fVecSphGeoAdd(&sp98, eyeNext, &sp64);
    anim->unk_2C += 0xBB8;
    temp_f16 = Math_CosS(anim->unk_2C);
    eye->x += (sp98.x - eye->x) * fabsf(temp_f16);
    eye->y += (sp98.y - eye->y) * fabsf(temp_f16);
    eye->z += (sp98.z - eye->z) * fabsf(temp_f16);

    if ((anim->unk_28 < temp_f16) && !anim->unk_2E) {
        player = camera->player;
        anim->unk_2E = true;
        func_800F4010(&player->actor.projectedPos, player->unk_89E + 0x8B0, 4.0f);
    } else if (anim->unk_28 > temp_f16) {
        anim->unk_2E = false;
    }

    anim->unk_28 = temp_f16;
    //camera->player->actor.world.pos = *eyeNext;
    //camera->player->actor.world.pos.y = camera->playerGroundY;
    //camera->player->actor.shape.rot.y = sp64.yaw;
    temp_f16 = ((240.0f * temp_f16) * (anim->unk_24 * 0.416667f));
    temp_a0 = temp_f16 + anim->unk_30;
    at->x = eye->x + (Math_SinS(temp_a0) * 10.0f);
    at->y = eye->y;
    at->z = eye->z + (Math_CosS(temp_a0) * 10.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    return 1;
}

s32 Camera_Subj0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data1(Camera* camera) {
    osSyncPrintf("chau!chau!\n");
    return Camera_Normal1(camera);
}

s32 Camera_Data2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data4(Camera* camera) {
    s32 pad2[2];
    Data4* data4 = (Data4*)camera->paramData;
    VecSph eyeAtOffset;
    VecSph atOffset;
    VecSph eyeNextAtOffset;
    f32 yNormal;
    s16 fov;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3s* sceneCamData;
    Vec3f lookAt;
    CameraModeValue* values;
    Data4InitParams* initParams = &data4->initParams;
    Vec3f* eye = &camera->eye;
    f32 playerHeight;
    Vec3f* at = &camera->at;
    s32 pad;

    playerHeight = Player_GetHeight(camera->player);

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));
        data4->yOffset = NEXTPCT * playerHeight * yNormal;
        data4->fov = NEXTSETTING;
        data4->interfaceFlags = NEXTSETTING;
        sceneCamData = Camera_GetCamBGData(camera);
        Camera_Vec3sToVec3f(&initParams->eyePosRot.pos, &BGCAM_POS(sceneCamData));
        initParams->eyePosRot.rot = BGCAM_ROT(sceneCamData);
        fov = BGCAM_FOV(sceneCamData);
        initParams->fov = fov;
        if (fov != -1) {
            data4->fov = initParams->fov < 361 ? initParams->fov : PCT(initParams->fov);
        }

        initParams->jfifId = BGCAM_JFIFID(sceneCamData);
        *eye = initParams->eyePosRot.pos;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = data4->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        func_80043B60(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&eyeNextAtOffset, at, eyeNext);
    Camera_CalcAtDefault(camera, &eyeNextAtOffset, data4->yOffset, false);
    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, eye, at);

    atOffset.r = eyeAtOffset.r;
    atOffset.yaw =
        (initParams->jfifId & 1) ? (DEGF_TO_BINANG(camera->data2) + initParams->eyePosRot.rot.y) : eyeAtOffset.yaw;
    atOffset.pitch =
        (initParams->jfifId & 2) ? (DEGF_TO_BINANG(camera->data3) + initParams->eyePosRot.rot.x) : eyeAtOffset.pitch;

    Camera_Vec3fVecSphGeoAdd(at, eye, &atOffset);

    lookAt = camera->playerPosRot.pos;
    lookAt.y += playerHeight;

    camera->dist = OLib_Vec3fDist(&lookAt, eye);
    camera->roll = 0;
    camera->xzSpeed = 0.0f;
    camera->fov = data4->fov;
    camera->atLERPStepScale = 0;
    return true;
}

/**
 * Hanging off of a ledge
 */
s32 Camera_Unique1(Camera* camera) {
    if (CVar_GetS32("gFreeCamera", 0) && SetCameraManual(camera) == 1) {
        Camera_Free(camera);
        return 1;
    }

    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerBodyPart0;
    s16 phiTarget;
    VecSph sp8C;
    VecSph unk908PlayerPosOffset;
    VecSph eyeAtOffset;
    VecSph eyeNextAtOffset;
    PosRot* playerPosRot = &camera->playerPosRot;
    PosRot playerhead;
    Unique1* uniq1 = (Unique1*)camera->paramData;
    Unique1Anim* anim = &uniq1->anim;
    s32 pad;
    f32 playerHeight;
    s32 pad2;

    playerHeight = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));
        uniq1->yOffset = NEXTPCT * playerHeight * yNormal;
        uniq1->distMin = NEXTPCT * playerHeight * yNormal;
        uniq1->distMax = NEXTPCT * playerHeight * yNormal;
        uniq1->pitchTarget = DEGF_TO_BINANG(NEXTSETTING);
        uniq1->fovTarget = NEXTSETTING;
        uniq1->atLERPScaleMax = NEXTPCT;
        uniq1->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS != 0) {
        Camera_CopyPREGToModeValues(camera);
    }

    sUpdateCameraDirection = 1;

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&eyeNextAtOffset, at, eyeNext);

    sCameraInterfaceFlags = uniq1->interfaceFlags;

    if (camera->animState == 0) {
        camera->posOffset.y = camera->posOffset.y - camera->playerPosDelta.y;
        anim->yawTarget = eyeNextAtOffset.yaw;
        anim->unk_00 = 0.0f;
        playerBodyPart0 = camera->player->bodyPartsPos[0];
        OLib_Vec3fDiffToVecSphGeo(&unk908PlayerPosOffset, &playerPosRot->pos, &playerBodyPart0);
        anim->timer = R_DEFA_CAM_ANIM_TIME;
        anim->yawTargetAdj = ABS(BINANG_SUB(unk908PlayerPosOffset.yaw, eyeAtOffset.yaw)) < 0x3A98
                                 ? 0
                                 : ((BINANG_SUB(unk908PlayerPosOffset.yaw, eyeAtOffset.yaw) / anim->timer) / 4) * 3;
        camera->animState++;
    }

    Actor_GetFocus(&playerhead, &camera->player->actor); // unused

    camera->yawUpdateRateInv = Camera_LERPCeilF(100.0f, camera->yawUpdateRateInv, OREG(25) * 0.01f, 0.1f);
    camera->pitchUpdateRateInv = Camera_LERPCeilF(100.0f, camera->pitchUpdateRateInv, OREG(25) * 0.01f, 0.1f);
    camera->xzOffsetUpdateRate = Camera_LERPCeilF(0.005f, camera->xzOffsetUpdateRate, OREG(25) * 0.01f, 0.01f);
    camera->yOffsetUpdateRate = Camera_LERPCeilF(0.01f, camera->yOffsetUpdateRate, OREG(26) * 0.01f, 0.01f);
    camera->fovUpdateRate = Camera_LERPCeilF(OREG(4) * 0.01f, camera->fovUpdateRate, 0.05f, 0.1f);

    Camera_CalcAtDefault(camera, &eyeNextAtOffset, uniq1->yOffset, 1);
    OLib_Vec3fDiffToVecSphGeo(&sp8C, at, eyeNext);

    camera->dist = Camera_LERPClampDist(camera, sp8C.r, uniq1->distMin, uniq1->distMax);

    phiTarget = uniq1->pitchTarget;
    sp8C.pitch = Camera_LERPCeilS(phiTarget, eyeNextAtOffset.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);

    if (sp8C.pitch > OREG(5)) {
        sp8C.pitch = OREG(5);
    }
    if (sp8C.pitch < -OREG(5)) {
        sp8C.pitch = -OREG(5);
    }

    if (anim->timer != 0) {
        anim->yawTarget += anim->yawTargetAdj;
        anim->timer--;
    }

    sp8C.yaw = Camera_LERPFloorS(anim->yawTarget, eyeNextAtOffset.yaw, 0.5f, 0x2710);
    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &sp8C);
    *eye = *eyeNext;
    Camera_BGCheck(camera, at, eye);
    camera->fov = Camera_LERPCeilF(uniq1->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = 0;
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, uniq1->atLERPScaleMax);
    return true;
}

s32 Camera_Unique2(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerPos;
    VecSph eyeOffset;
    VecSph eyeAtOffset;
    s32 pad;
    f32 lerpRateFactor;
    Unique2* uniq2 = (Unique2*)camera->paramData;
    Unique2Unk10* unk10 = &uniq2->unk_10;
    s32 pad2;
    f32 playerHeight;

    playerHeight = Player_GetHeight(camera->player);

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, at, eye);

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = ((1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight)));
        uniq2->yOffset = NEXTPCT * playerHeight * yNormal;
        uniq2->distTarget = NEXTSETTING;
        uniq2->fovTarget = NEXTSETTING;
        uniq2->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = uniq2->interfaceFlags;

    if ((camera->animState == 0) || (unk10->unk_04 != uniq2->interfaceFlags)) {
        unk10->unk_04 = uniq2->interfaceFlags;
    }

    if (camera->animState == 0) {
        camera->animState = 1;
        func_80043B60(camera);
        unk10->unk_00 = 200.0f;
        if (uniq2->interfaceFlags & 0x10) {
            camera->unk_14C &= ~4;
        }
    }

    playerPos = camera->playerPosRot.pos;
    lerpRateFactor = (uniq2->interfaceFlags & 1 ? 1.0f : camera->speedRatio);
    at->x = F32_LERPIMP(at->x, playerPos.x, lerpRateFactor * 0.6f);
    at->y = F32_LERPIMP(at->y, playerPos.y + playerHeight + uniq2->yOffset, 0.4f);
    at->z = F32_LERPIMP(at->z, playerPos.z, lerpRateFactor * 0.6f);
    unk10->unk_00 = F32_LERPIMP(unk10->unk_00, 2.0f, 0.05f); // unused.

    if (uniq2->interfaceFlags & 1) {
        OLib_Vec3fDiffToVecSphGeo(&eyeOffset, at, eyeNext);
        eyeOffset.r = uniq2->distTarget;
        Camera_Vec3fVecSphGeoAdd(&playerPos, at, &eyeOffset);
        Camera_LERPCeilVec3f(&playerPos, eye, 0.25f, 0.25f, 0.2f);
    } else if (uniq2->interfaceFlags & 2) {
        if (OLib_Vec3fDistXZ(at, eyeNext) < uniq2->distTarget) {
            OLib_Vec3fDiffToVecSphGeo(&eyeOffset, at, eyeNext);
            eyeOffset.yaw = Camera_LERPCeilS(eyeOffset.yaw, eyeAtOffset.yaw, 0.1f, 0xA);
            eyeOffset.r = uniq2->distTarget;
            eyeOffset.pitch = 0;
            Camera_Vec3fVecSphGeoAdd(eye, at, &eyeOffset);
            eye->y = eyeNext->y;
        } else {
            Camera_LERPCeilVec3f(eyeNext, eye, 0.25f, 0.25f, 0.2f);
        }
    }

    Camera_BGCheck(camera, at, eye);
    camera->dist = OLib_Vec3fDist(at, eye);
    camera->roll = 0;
    camera->fov = Camera_LERPCeilF(uniq2->fovTarget, camera->fov, 0.2f, 0.1f);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, 1.0f);
    return true;
}

s32 Camera_Unique3(Camera* camera) {
    VecSph sp60;
    f32 playerHeight;
    Unique3* uniq3 = (Unique3*)camera->paramData;
    Vec3s* temp_v0_2;
    Vec3s sp4C;
    Unique3Anim* anim = &uniq3->anim;
    Unique3Params* params = &uniq3->params;
    Vec3f* at = &camera->at;
    PosRot* cameraPlayerPosRot = &camera->playerPosRot;

    playerHeight = Player_GetHeight(camera->player);
    camera->unk_14C &= ~0x10;
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerHeight));

        params->yOffset = NEXTPCT * playerHeight * yNormal;
        params->fov = NEXTSETTING;
        params->interfaceFlags = NEXTSETTING;
    }
    if (R_RELOAD_CAM_PARAMS != 0) {
        Camera_CopyPREGToModeValues(camera);
    }
    sCameraInterfaceFlags = params->interfaceFlags;
    switch (camera->animState) {
        case 0:
            func_80043B60(camera);
            camera->unk_14C &= ~(0x8 | 0x4);
            anim->initialFov = camera->fov;
            anim->initialDist = OLib_Vec3fDist(at, &camera->eye);
            camera->animState++;
        case 1:
            if (uniq3->doorParams.timer1-- > 0) {
                break;
            }
            temp_v0_2 = Camera_GetCamBGData(camera);
            Camera_Vec3sToVec3f(&camera->eyeNext, &BGCAM_POS(temp_v0_2));
            camera->eye = camera->eyeNext;
            sp4C = BGCAM_ROT(temp_v0_2);
            sp60.r = 100.0f;
            sp60.yaw = sp4C.y;
            sp60.pitch = -sp4C.x;
            Camera_Vec3fVecSphGeoAdd(at, &camera->eye, &sp60);
            camera->animState++;

        case 2:
            if (params->interfaceFlags & 4) {
                camera->at = cameraPlayerPosRot->pos;
                camera->at.y += playerHeight + params->yOffset;
            }
            if (uniq3->doorParams.timer2-- > 0) {
                break;
            }
            camera->animState++;

        case 3:
            camera->unk_14C |= (0x400 | 0x10);
            if ((camera->unk_14C & 8) != 0) {
                camera->animState++;
            } else {
                break;
            }
        case 4:
            if (params->interfaceFlags & 2) {
                camera->unk_14C |= 4;
                camera->unk_14C &= ~8;
                Camera_ChangeSettingFlags(camera, CAM_SET_PIVOT_IN_FRONT, 2);
                break;
            }
            uniq3->doorParams.timer3 = 5;
            if (camera->xzSpeed > 0.001f || CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_A) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_B) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CLEFT) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CDOWN) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CUP) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CRIGHT) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_R) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_Z)) {
                camera->animState++;
            } else {
                break;
            }
        case 5:
            camera->fov = Camera_LERPCeilF(anim->initialFov, camera->fov, 0.4f, 0.1f);
            OLib_Vec3fDiffToVecSphGeo(&sp60, at, &camera->eye);
            sp60.r = Camera_LERPCeilF(100.0f, sp60.r, 0.4f, 0.1f);
            Camera_Vec3fVecSphGeoAdd(&camera->eyeNext, at, &sp60);
            camera->eye = camera->eyeNext;
            if (uniq3->doorParams.timer3-- > 0) {
                break;
            }
            camera->animState++;
        default:
            camera->unk_14C |= 4;
            camera->unk_14C &= ~8;
            camera->fov = params->fov;
            Camera_ChangeSettingFlags(camera, camera->prevSetting, 2);
            camera->atLERPStepScale = 0.0f;
            camera->posOffset.x = camera->at.x - cameraPlayerPosRot->pos.x;
            camera->posOffset.y = camera->at.y - cameraPlayerPosRot->pos.y;
            camera->posOffset.z = camera->at.z - cameraPlayerPosRot->pos.z;
            break;
    }

    return true;
}

/**
 * Camera's eye is specified by scene camera data, at point is generated at the intersection
 * of the eye to the player
 */
s32 Camera_Unique0(Camera* camera) {
    f32 yOffset;
    CameraModeValue* values;
    Player* player;
    Vec3f playerPosWithOffset;
    VecSph atPlayerOffset;
    Vec3s* sceneCamData;
    Vec3s sceneCamRot;
    PosRot* playerPosRot = &camera->playerPosRot;
    Unique0* uniq0 = (Unique0*)camera->paramData;
    Unique0Params* params = &uniq0->uniq0;
    Unique0Anim* anim = &params->anim;
    Vec3f* eye = &camera->eye;
    s16 fov;

    yOffset = Player_GetHeight(camera->player);
    player = camera->player;

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        params->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    playerPosWithOffset = playerPosRot->pos;
    playerPosWithOffset.y += yOffset;

    sCameraInterfaceFlags = params->interfaceFlags;

    if (camera->animState == 0) {
        func_80043B60(camera);
        camera->unk_14C &= ~4;
        sceneCamData = Camera_GetCamBGData(camera);
        Camera_Vec3sToVec3f(&anim->sceneCamPosPlayerLine.a, &BGCAM_POS(sceneCamData));

        *eye = camera->eyeNext = anim->sceneCamPosPlayerLine.a;
        sceneCamRot = BGCAM_ROT(sceneCamData);
        fov = BGCAM_FOV(sceneCamData);
        if (fov != -1) {
            camera->fov = fov < 361 ? fov : PCT(fov);
        }
        anim->animTimer = BGCAM_JFIFID(sceneCamData);
        if (anim->animTimer == -1) {
            anim->animTimer = uniq0->doorParams.timer1 + uniq0->doorParams.timer2;
        }
        atPlayerOffset.r = OLib_Vec3fDist(&playerPosWithOffset, eye);
        atPlayerOffset.yaw = sceneCamRot.y;
        atPlayerOffset.pitch = -sceneCamRot.x;
        OLib_VecSphGeoToVec3f(&anim->sceneCamPosPlayerLine.b, &atPlayerOffset);
        Math3D_LineClosestToPoint(&anim->sceneCamPosPlayerLine, &playerPosRot->pos, &camera->at);
        anim->initalPos = playerPosRot->pos;
        camera->animState++;
    }

    if (player->stateFlags1 & 0x20000000) {
        anim->initalPos = playerPosRot->pos;
    }

    if (params->interfaceFlags & 1) {
        if (anim->animTimer > 0) {
            anim->animTimer--;
            anim->initalPos = playerPosRot->pos;
        } else if ((!(player->stateFlags1 & 0x20000000)) &&
                   ((OLib_Vec3fDistXZ(&playerPosRot->pos, &anim->initalPos) >= 10.0f) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_A) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_B) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CLEFT) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CDOWN) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CUP) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CRIGHT) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_R) ||
                    CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_Z))) {
            camera->dist = OLib_Vec3fDist(&camera->at, eye);
            camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
            camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
            camera->posOffset.z = camera->at.z - playerPosRot->pos.z;
            camera->atLERPStepScale = 0.0f;
            camera->unk_14C |= 4;
            Camera_ChangeSettingFlags(camera, camera->prevSetting, 2);
        }
    } else {
        if (anim->animTimer > 0) {
            anim->animTimer--;
            if (anim->animTimer == 0) {
                sCameraInterfaceFlags = 0;
            }
        } else {
            anim->initalPos = playerPosRot->pos;
        }

        if ((!(player->stateFlags1 & 0x20000000)) &&
            ((0.001f < camera->xzSpeed) || CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_A) ||
             CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_B) ||
             CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CLEFT) ||
             CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CDOWN) ||
             CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CUP) ||
             CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CRIGHT) ||
             CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_R) ||
             CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_Z))) {
            camera->dist = OLib_Vec3fDist(&camera->at, &camera->eye);
            camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
            camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
            camera->posOffset.z = camera->at.z - playerPosRot->pos.z;
            camera->atLERPStepScale = 0.0f;
            Camera_ChangeSettingFlags(camera, camera->prevSetting, 2);
            camera->unk_14C |= 4;
        }
    }
    return true;
}

s32 Camera_Unique4(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Was setup to be used by the camera setting "FOREST_UNUSED"
 */
s32 Camera_Unique5(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * This function doesn't really update much.
 * Eye/at positions are updated via Camera_SetParam
 */
s32 Camera_Unique6(Camera* camera) {
    Unique6* uniq6 = (Unique6*)camera->paramData;
    CameraModeValue* values;
    Vec3f sp2C;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 offset;

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        uniq6->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = uniq6->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        func_80043ABC(camera);
    }

    if (camera->player != NULL) {
        offset = Player_GetHeight(camera->player);
        sp2C = playerPosRot->pos;
        sp2C.y += offset;
        camera->dist = OLib_Vec3fDist(&sp2C, &camera->eye);
        camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
        camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
        camera->posOffset.z = camera->at.z - playerPosRot->pos.z;
    } else {
        camera->dist = OLib_Vec3fDist(&camera->at, &camera->eye);
    }

    if (uniq6->interfaceFlags & 1 && camera->timer > 0) {
        camera->timer--;
    }

    return true;
}

/**
 * Camera is at a fixed point specified by the scene's camera data,
 * camera rotates to follow player
 */
s32 Camera_Unique7(Camera* camera) {
    s32 pad;
    Unique7* uniq7 = (Unique7*)camera->paramData;
    CameraModeValue* values;
    VecSph playerPosEyeOffset;
    s16 fov;
    Vec3s* sceneCamData;
    Vec3s sceneCamRot;
    Vec3f* at = &camera->at;
    PosRot* playerPosRot = &camera->playerPosRot;
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Unique7Unk8* unk08 = &uniq7->unk_08;

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        uniq7->fov = NEXTSETTING;
        uniq7->interfaceFlags = (s16)NEXTSETTING;
    }
    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sceneCamData = Camera_GetCamBGData(camera);

    Camera_Vec3sToVec3f(eyeNext, &BGCAM_POS(sceneCamData));
    *eye = *eyeNext;
    sceneCamRot = BGCAM_ROT(sceneCamData); // unused
    (void)sceneCamRot;                     // suppresses set but unused warning

    OLib_Vec3fDiffToVecSphGeo(&playerPosEyeOffset, eye, &playerPosRot->pos);

    // fov actually goes unused since it's hard set later on.
    fov = BGCAM_FOV(sceneCamData);
    if (fov == -1) {
        fov = uniq7->fov * 100.0f;
    }

    if (fov < 361) {
        fov *= 100;
    }

    sCameraInterfaceFlags = uniq7->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        camera->fov = PCT(fov);
        camera->atLERPStepScale = 0.0f;
        camera->roll = 0;
        unk08->unk_00.x = playerPosEyeOffset.yaw;
    }

    camera->fov = 60.0f;

    // 0x7D0 ~ 10.98 degres.
    unk08->unk_00.x = Camera_LERPFloorS(playerPosEyeOffset.yaw, unk08->unk_00.x, 0.4f, 0x7D0);
    playerPosEyeOffset.pitch =
        -BGCAM_ROT(sceneCamData).x * Math_CosS(playerPosEyeOffset.yaw - BGCAM_ROT(sceneCamData).y);
    Camera_Vec3fVecSphGeoAdd(at, eye, &playerPosEyeOffset);
    camera->unk_14C |= 0x400;
    return true;
}

s32 Camera_Unique8(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Unique9(Camera* camera) {
    Vec3f atTarget;
    Vec3f eyeTarget;
    Unique9* uniq9 = &ONEPOINT_CS_INFO(camera)->uniq9;
    Unique9Anim* anim = &uniq9->anim;
    f32 invKeyFrameTimer;
    VecSph eyeNextAtOffset;
    VecSph scratchSph;
    VecSph playerTargetOffset;
    s16 action;
    s16 atInitFlags;
    s16 eyeInitFlags;
    s16 pad2;
    PosRot targethead;
    PosRot playerhead;
    PosRot playerPosRot;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* at = &camera->at;
    Vec3f* eye = &camera->eye;
    Player* player = camera->player;
    Actor* focusActor;
    f32 spB4;
    PosRot atFocusPosRot;
    Vec3f eyeLookAtPos;
    CameraModeValue* values;
    PosRot eyeFocusPosRot;

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        uniq9->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = uniq9->interfaceFlags;

    Actor_GetWorld(&playerPosRot, &camera->player->actor);

    if (camera->animState == 0) {
        camera->animState++;
        anim->curKeyFrameIdx = -1;
        anim->keyFrameTimer = 1;
        anim->unk_38 = 0;
        anim->playerPos.x = playerPosRot.pos.x;
        anim->playerPos.y = playerPosRot.pos.y;
        anim->playerPos.z = playerPosRot.pos.z;
        camera->atLERPStepScale = 0.0f;
        func_80043B60(camera);
    }

    if (anim->unk_38 == 0 && anim->keyFrameTimer > 0) {
        anim->keyFrameTimer--;
    }

    if (anim->keyFrameTimer == 0) {
        anim->isNewKeyFrame = true;
        anim->curKeyFrameIdx++;
        if (anim->curKeyFrameIdx < ONEPOINT_CS_INFO(camera)->keyFrameCnt) {
            anim->curKeyFrame = &ONEPOINT_CS_INFO(camera)->keyFrames[anim->curKeyFrameIdx];
            anim->keyFrameTimer = anim->curKeyFrame->timerInit;

            if (anim->curKeyFrame->unk_01 != 0xFF) {
                if ((anim->curKeyFrame->unk_01 & 0xF0) == 0x80) {
                    D_8011D3AC = anim->curKeyFrame->unk_01 & 0xF;
                } else if ((anim->curKeyFrame->unk_01 & 0xF0) == 0xC0) {
                    Camera_UpdateInterface(0xF000 | ((anim->curKeyFrame->unk_01 & 0xF) << 8));
                } else if (camera->player->stateFlags1 & 0x8000000 && player->currentBoots != PLAYER_BOOTS_IRON) {
                    func_8002DF38(camera->play, camera->target, 8);
                    osSyncPrintf("camera: demo: player demo set WAIT\n");
                } else {
                    osSyncPrintf("camera: demo: player demo set %d\n", anim->curKeyFrame->unk_01);
                    func_8002DF38(camera->play, camera->target, anim->curKeyFrame->unk_01);
                }
            }
        } else {
            // We've gone through all the keyframes.
            if (camera->thisIdx != MAIN_CAM) {
                camera->timer = 0;
            }
            return true;
        }
    } else {
        anim->isNewKeyFrame = false;
    }

    atInitFlags = anim->curKeyFrame->initFlags & 0xFF;
    if (atInitFlags == 1) {
        anim->atTarget = anim->curKeyFrame->atTargetInit;
    } else if (atInitFlags == 2) {
        if (anim->isNewKeyFrame) {
            anim->atTarget.x = camera->play->view.lookAt.x + anim->curKeyFrame->atTargetInit.x;
            anim->atTarget.y = camera->play->view.lookAt.y + anim->curKeyFrame->atTargetInit.y;
            anim->atTarget.z = camera->play->view.lookAt.z + anim->curKeyFrame->atTargetInit.z;
        }
    } else if (atInitFlags == 3) {
        if (anim->isNewKeyFrame) {
            anim->atTarget.x = camera->at.x + anim->curKeyFrame->atTargetInit.x;
            anim->atTarget.y = camera->at.y + anim->curKeyFrame->atTargetInit.y;
            anim->atTarget.z = camera->at.z + anim->curKeyFrame->atTargetInit.z;
        }
    } else if (atInitFlags == 4 || atInitFlags == 0x84) {
        if (camera->target != NULL && camera->target->update != NULL) {
            Actor_GetFocus(&targethead, camera->target);
            Actor_GetFocus(&playerhead, &camera->player->actor);
            playerhead.pos.x = playerPosRot.pos.x;
            playerhead.pos.z = playerPosRot.pos.z;
            OLib_Vec3fDiffToVecSphGeo(&playerTargetOffset, &targethead.pos, &playerhead.pos);
            if (atInitFlags & (s16)0x8080) {
                scratchSph.pitch = DEGF_TO_BINANG(anim->curKeyFrame->atTargetInit.x);
                scratchSph.yaw = DEGF_TO_BINANG(anim->curKeyFrame->atTargetInit.y);
                scratchSph.r = anim->curKeyFrame->atTargetInit.z;
            } else {
                OLib_Vec3fToVecSphGeo(&scratchSph, &anim->curKeyFrame->atTargetInit);
            }
            scratchSph.yaw += playerTargetOffset.yaw;
            scratchSph.pitch += playerTargetOffset.pitch;
            Camera_Vec3fVecSphGeoAdd(&anim->atTarget, &targethead.pos, &scratchSph);
        } else {
            if (camera->target == NULL) {
                osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
            }

            camera->target = NULL;
            anim->atTarget = camera->at;
        }
    } else if (atInitFlags & 0x6060) {
        if (!(atInitFlags & 4) || anim->isNewKeyFrame) {
            if (atInitFlags & 0x2020) {
                focusActor = &camera->player->actor;
            } else if (camera->target != NULL && camera->target->update != NULL) {
                focusActor = camera->target;
            } else {
                camera->target = NULL;
                focusActor = NULL;
            }

            if (focusActor != NULL) {
                if ((atInitFlags & 0xF) == 1) {
                    Actor_GetFocus(&atFocusPosRot, focusActor);
                } else if ((atInitFlags & 0xF) == 2) {
                    Actor_GetWorld(&atFocusPosRot, focusActor);
                } else {
                    Actor_GetWorldPosShapeRot(&atFocusPosRot, focusActor);
                }

                if (atInitFlags & (s16)0x8080) {
                    scratchSph.pitch = DEGF_TO_BINANG(anim->curKeyFrame->atTargetInit.x);
                    scratchSph.yaw = DEGF_TO_BINANG(anim->curKeyFrame->atTargetInit.y);
                    scratchSph.r = anim->curKeyFrame->atTargetInit.z;
                } else {
                    OLib_Vec3fToVecSphGeo(&scratchSph, &anim->curKeyFrame->atTargetInit);
                }

                scratchSph.yaw += atFocusPosRot.rot.y;
                scratchSph.pitch -= atFocusPosRot.rot.x;
                Camera_Vec3fVecSphGeoAdd(&anim->atTarget, &atFocusPosRot.pos, &scratchSph);
            } else {
                if (camera->target == NULL) {
                    osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
                }
                anim->atTarget = *at;
            }
        }
    } else {
        anim->atTarget = *at;
    }

    eyeInitFlags = anim->curKeyFrame->initFlags & 0xFF00;
    if (eyeInitFlags == 0x100) {
        anim->eyeTarget = anim->curKeyFrame->eyeTargetInit;
    } else if (eyeInitFlags == 0x200) {
        if (anim->isNewKeyFrame) {
            anim->eyeTarget.x = camera->play->view.eye.x + anim->curKeyFrame->eyeTargetInit.x;
            anim->eyeTarget.y = camera->play->view.eye.y + anim->curKeyFrame->eyeTargetInit.y;
            anim->eyeTarget.z = camera->play->view.eye.z + anim->curKeyFrame->eyeTargetInit.z;
        }
    } else if (eyeInitFlags == 0x300) {
        if (anim->isNewKeyFrame) {
            anim->eyeTarget.x = camera->eyeNext.x + anim->curKeyFrame->eyeTargetInit.x;
            anim->eyeTarget.y = camera->eyeNext.y + anim->curKeyFrame->eyeTargetInit.y;
            anim->eyeTarget.z = camera->eyeNext.z + anim->curKeyFrame->eyeTargetInit.z;
        }
    } else if (eyeInitFlags == 0x400 || eyeInitFlags == (s16)0x8400 || eyeInitFlags == 0x500 ||
               eyeInitFlags == (s16)0x8500) {
        if (camera->target != NULL && camera->target->update != NULL) {
            Actor_GetFocus(&targethead, camera->target);
            Actor_GetFocus(&playerhead, &camera->player->actor);
            playerhead.pos.x = playerPosRot.pos.x;
            playerhead.pos.z = playerPosRot.pos.z;
            OLib_Vec3fDiffToVecSphGeo(&playerTargetOffset, &targethead.pos, &playerhead.pos);
            if (eyeInitFlags == 0x400 || eyeInitFlags == (s16)0x8400) {
                eyeLookAtPos = targethead.pos;
            } else {
                eyeLookAtPos = anim->atTarget;
            }

            if (eyeInitFlags & (s16)0x8080) {
                scratchSph.pitch = DEGF_TO_BINANG(anim->curKeyFrame->eyeTargetInit.x);
                scratchSph.yaw = DEGF_TO_BINANG(anim->curKeyFrame->eyeTargetInit.y);
                scratchSph.r = anim->curKeyFrame->eyeTargetInit.z;
            } else {
                OLib_Vec3fToVecSphGeo(&scratchSph, &anim->curKeyFrame->eyeTargetInit);
            }

            scratchSph.yaw += playerTargetOffset.yaw;
            scratchSph.pitch += playerTargetOffset.pitch;
            Camera_Vec3fVecSphGeoAdd(&anim->eyeTarget, &eyeLookAtPos, &scratchSph);
        } else {
            if (camera->target == NULL) {
                osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
            }
            camera->target = NULL;
            anim->eyeTarget = *eyeNext;
        }
    } else if (eyeInitFlags & 0x6060) {
        if (!(eyeInitFlags & 0x400) || anim->isNewKeyFrame) {
            if (eyeInitFlags & 0x2020) {
                focusActor = &camera->player->actor;
            } else if (camera->target != NULL && camera->target->update != NULL) {
                focusActor = camera->target;
            } else {
                camera->target = NULL;
                focusActor = NULL;
            }

            if (focusActor != NULL) {
                if ((eyeInitFlags & 0xF00) == 0x100) {
                    // head
                    Actor_GetFocus(&eyeFocusPosRot, focusActor);
                } else if ((eyeInitFlags & 0xF00) == 0x200) {
                    // world
                    Actor_GetWorld(&eyeFocusPosRot, focusActor);
                } else {
                    // world, shapeRot
                    Actor_GetWorldPosShapeRot(&eyeFocusPosRot, focusActor);
                }

                if (eyeInitFlags & (s16)0x8080) {
                    scratchSph.pitch = DEGF_TO_BINANG(anim->curKeyFrame->eyeTargetInit.x);
                    scratchSph.yaw = DEGF_TO_BINANG(anim->curKeyFrame->eyeTargetInit.y);
                    scratchSph.r = anim->curKeyFrame->eyeTargetInit.z;
                } else {
                    OLib_Vec3fToVecSphGeo(&scratchSph, &anim->curKeyFrame->eyeTargetInit);
                }

                scratchSph.yaw += eyeFocusPosRot.rot.y;
                scratchSph.pitch -= eyeFocusPosRot.rot.x;
                Camera_Vec3fVecSphGeoAdd(&anim->eyeTarget, &eyeFocusPosRot.pos, &scratchSph);
            } else {
                if (camera->target == NULL) {
                    osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
                }
                camera->target = NULL;
                anim->eyeTarget = *eyeNext;
            }
        }
    } else {
        anim->eyeTarget = *eyeNext;
    }

    if (anim->curKeyFrame->initFlags == 2) {
        anim->fovTarget = camera->play->view.fovy;
        anim->rollTarget = 0;
    } else if (anim->curKeyFrame->initFlags == 0) {
        anim->fovTarget = camera->fov;
        anim->rollTarget = camera->roll;
    } else {
        anim->fovTarget = anim->curKeyFrame->fovTargetInit;
        anim->rollTarget = DEGF_TO_BINANG(anim->curKeyFrame->rollTargetInit);
    }

    action = anim->curKeyFrame->actionFlags & 0x1F;
    switch (action) {
        case 15:
            // static copy to at/eye/fov/roll
            *at = anim->atTarget;
            *eyeNext = anim->eyeTarget;
            camera->fov = anim->fovTarget;
            camera->roll = anim->rollTarget;
            camera->unk_14C |= 0x400;
            break;
        case 21:
            // same as 15, but with unk_38 ?
            if (anim->unk_38 == 0) {
                anim->unk_38 = 1;
            } else if (camera->unk_14C & 8) {
                anim->unk_38 = 0;
                camera->unk_14C &= ~8;
            }
            *at = anim->atTarget;
            *eyeNext = anim->eyeTarget;
            camera->fov = anim->fovTarget;
            camera->roll = anim->rollTarget;
            break;
        case 16:
            // same as 16, but don't unset bit 0x8 on unk_14C
            if (anim->unk_38 == 0) {
                anim->unk_38 = 1;
            } else if (camera->unk_14C & 8) {
                anim->unk_38 = 0;
            }

            *at = anim->atTarget;
            *eyeNext = anim->eyeTarget;
            camera->fov = anim->fovTarget;
            camera->roll = anim->rollTarget;
            break;
        case 1:
            // linear interpolation of eye/at using the spherical coordinates
            OLib_Vec3fDiffToVecSphGeo(&eyeNextAtOffset, at, eyeNext);
            OLib_Vec3fDiffToVecSphGeo(&anim->atEyeOffsetTarget, &anim->atTarget, &anim->eyeTarget);
            invKeyFrameTimer = 1.0f / anim->keyFrameTimer;
            scratchSph.r = F32_LERPIMP(eyeNextAtOffset.r, anim->atEyeOffsetTarget.r, invKeyFrameTimer);
            scratchSph.pitch = eyeNextAtOffset.pitch +
                               (BINANG_SUB(anim->atEyeOffsetTarget.pitch, eyeNextAtOffset.pitch) * invKeyFrameTimer);
            scratchSph.yaw =
                eyeNextAtOffset.yaw + (BINANG_SUB(anim->atEyeOffsetTarget.yaw, eyeNextAtOffset.yaw) * invKeyFrameTimer);
            Camera_Vec3fVecSphGeoAdd(&eyeTarget, at, &scratchSph);
            goto setEyeNext;
        case 2:
            // linear interpolation of eye/at using the eyeTarget
            invKeyFrameTimer = 1.0f / anim->keyFrameTimer;
            eyeTarget.x = F32_LERPIMP(camera->eyeNext.x, anim->eyeTarget.x, invKeyFrameTimer);
            eyeTarget.y = F32_LERPIMP(camera->eyeNext.y, anim->eyeTarget.y, invKeyFrameTimer);
            eyeTarget.z = F32_LERPIMP(camera->eyeNext.z, anim->eyeTarget.z, invKeyFrameTimer);

        setEyeNext:
            camera->eyeNext.x =
                Camera_LERPFloorF(eyeTarget.x, camera->eyeNext.x, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.y =
                Camera_LERPFloorF(eyeTarget.y, camera->eyeNext.y, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.z =
                Camera_LERPFloorF(eyeTarget.z, camera->eyeNext.z, anim->curKeyFrame->lerpStepScale, 1.0f);
        case 9:
        case 10:
            // linear interpolation of at/fov/roll
            invKeyFrameTimer = 1.0f / anim->keyFrameTimer;
            atTarget.x = F32_LERPIMP(camera->at.x, anim->atTarget.x, invKeyFrameTimer);
            atTarget.y = F32_LERPIMP(camera->at.y, anim->atTarget.y, invKeyFrameTimer);
            atTarget.z = F32_LERPIMP(camera->at.z, anim->atTarget.z, invKeyFrameTimer);
            camera->at.x = Camera_LERPFloorF(atTarget.x, camera->at.x, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.y = Camera_LERPFloorF(atTarget.y, camera->at.y, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.z = Camera_LERPFloorF(atTarget.z, camera->at.z, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->fov = Camera_LERPFloorF(F32_LERPIMP(camera->fov, anim->fovTarget, invKeyFrameTimer), camera->fov,
                                            anim->curKeyFrame->lerpStepScale, 0.01f);
            camera->roll = Camera_LERPFloorS(BINANG_LERPIMPINV(camera->roll, anim->rollTarget, anim->keyFrameTimer),
                                             camera->roll, anim->curKeyFrame->lerpStepScale, 0xA);
            break;
        case 4:
            // linear interpolation of eye/at/fov/roll using the step scale, and spherical coordinates
            OLib_Vec3fDiffToVecSphGeo(&eyeNextAtOffset, at, eyeNext);
            OLib_Vec3fDiffToVecSphGeo(&anim->atEyeOffsetTarget, &anim->atTarget, &anim->eyeTarget);
            scratchSph.r =
                Camera_LERPCeilF(anim->atEyeOffsetTarget.r, eyeNextAtOffset.r, anim->curKeyFrame->lerpStepScale, 0.1f);
            scratchSph.pitch = Camera_LERPCeilS(anim->atEyeOffsetTarget.pitch, eyeNextAtOffset.pitch,
                                                anim->curKeyFrame->lerpStepScale, 1);
            scratchSph.yaw =
                Camera_LERPCeilS(anim->atEyeOffsetTarget.yaw, eyeNextAtOffset.yaw, anim->curKeyFrame->lerpStepScale, 1);
            Camera_Vec3fVecSphGeoAdd(eyeNext, at, &scratchSph);
            goto setAtFOVRoll;
        case 3:
            // linear interplation of eye/at/fov/roll using the step scale using eyeTarget
            camera->eyeNext.x =
                Camera_LERPCeilF(anim->eyeTarget.x, camera->eyeNext.x, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.y =
                Camera_LERPCeilF(anim->eyeTarget.y, camera->eyeNext.y, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.z =
                Camera_LERPCeilF(anim->eyeTarget.z, camera->eyeNext.z, anim->curKeyFrame->lerpStepScale, 1.0f);
        case 11:
        case 12:
        setAtFOVRoll:
            // linear interpolation of at/fov/roll using the step scale.
            camera->at.x = Camera_LERPCeilF(anim->atTarget.x, camera->at.x, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.y = Camera_LERPCeilF(anim->atTarget.y, camera->at.y, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.z = Camera_LERPCeilF(anim->atTarget.z, camera->at.z, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->fov = Camera_LERPCeilF(anim->fovTarget, camera->fov, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->roll = Camera_LERPCeilS(anim->rollTarget, camera->roll, anim->curKeyFrame->lerpStepScale, 1);
            break;
        case 13:
            // linear interpolation of at, with rotation around eyeTargetInit.y
            camera->at.x = Camera_LERPCeilF(anim->atTarget.x, camera->at.x, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.y += camera->playerPosDelta.y * anim->curKeyFrame->lerpStepScale;
            camera->at.z = Camera_LERPCeilF(anim->atTarget.z, camera->at.z, anim->curKeyFrame->lerpStepScale, 1.0f);
            OLib_Vec3fDiffToVecSphGeo(&scratchSph, at, eyeNext);
            scratchSph.yaw += DEGF_TO_BINANG(anim->curKeyFrame->eyeTargetInit.y);

            // 3A98 ~ 82.40 degrees
            if (scratchSph.pitch >= 0x3A99) {
                scratchSph.pitch = 0x3A98;
            }

            if (scratchSph.pitch < -0x3A98) {
                scratchSph.pitch = -0x3A98;
            }

            spB4 = scratchSph.r;
            scratchSph.r =
                !(spB4 < anim->curKeyFrame->eyeTargetInit.z)
                    ? Camera_LERPCeilF(anim->curKeyFrame->eyeTargetInit.z, spB4, anim->curKeyFrame->lerpStepScale, 1.0f)
                    : scratchSph.r;

            Camera_Vec3fVecSphGeoAdd(eyeNext, at, &scratchSph);
            camera->fov =
                Camera_LERPCeilF(F32_LERPIMPINV(camera->fov, anim->curKeyFrame->fovTargetInit, anim->keyFrameTimer),
                                 camera->fov, anim->curKeyFrame->lerpStepScale, 1.0f);
            camera->roll = Camera_LERPCeilS(anim->rollTarget, camera->roll, anim->curKeyFrame->lerpStepScale, 1);
            break;
        case 24:
            // Set current keyframe to the roll target?
            anim->curKeyFrameIdx = anim->rollTarget;
            break;
        case 19: {
            // Change the parent camera (or default)'s mode to normal
            s32 camIdx = camera->parentCamIdx <= SUBCAM_NONE ? MAIN_CAM : camera->parentCamIdx;

            Camera_ChangeModeFlags(camera->play->cameraPtrs[camIdx], CAM_MODE_NORMAL, 1);
        }
        case 18: {
            // copy the current camera to the parent (or default)'s camera.
            s32 camIdx = camera->parentCamIdx <= SUBCAM_NONE ? MAIN_CAM : camera->parentCamIdx;
            Camera* cam = camera->play->cameraPtrs[camIdx];

            *eye = *eyeNext;
            Camera_Copy(cam, camera);
        }
        default:
            if (camera->thisIdx != MAIN_CAM) {
                camera->timer = 0;
            }
    }

    *eye = *eyeNext;

    if (anim->curKeyFrame->actionFlags & 0x80) {
        Camera_BGCheck(camera, at, eye);
    }

    if (anim->curKeyFrame->actionFlags & 0x40) {
        // Set the player's position
        camera->player->actor.world.pos.x = anim->playerPos.x;
        camera->player->actor.world.pos.z = anim->playerPos.z;
        if (camera->player->stateFlags1 & 0x8000000 && player->currentBoots != PLAYER_BOOTS_IRON) {
            camera->player->actor.world.pos.y = anim->playerPos.y;
        }
    } else {
        anim->playerPos.x = playerPosRot.pos.x;
        anim->playerPos.y = playerPosRot.pos.y;
        anim->playerPos.z = playerPosRot.pos.z;
    }

    if (anim->unk_38 == 0 && camera->timer > 0) {
        camera->timer--;
    }

    if (camera->player != NULL) {
        camera->posOffset.x = camera->at.x - camera->playerPosRot.pos.x;
        camera->posOffset.y = camera->at.y - camera->playerPosRot.pos.y;
        camera->posOffset.z = camera->at.z - camera->playerPosRot.pos.z;
    }

    camera->dist = OLib_Vec3fDist(at, eye);
    return true;
}

void Camera_DebugPrintSplineArray(char* name, s16 length, CutsceneCameraPoint cameraPoints[]) {
    s32 i;

    osSyncPrintf("static SplinedatZ  %s[] = {\n", name);
    for (i = 0; i < length; i++) {
        osSyncPrintf("    /* key frame %2d */ {\n", i);
        osSyncPrintf("    /*     code     */ %d,\n", cameraPoints[i].continueFlag);
        osSyncPrintf("    /*     z        */ %d,\n", cameraPoints[i].cameraRoll);
        osSyncPrintf("    /*     T        */ %d,\n", cameraPoints[i].nextPointFrame);
        osSyncPrintf("    /*     zoom     */ %f,\n", cameraPoints[i].viewAngle);
        osSyncPrintf("    /*     pos      */ { %d, %d, %d }\n", cameraPoints[i].pos.x, cameraPoints[i].pos.y,
                     cameraPoints[i].pos.z);
        osSyncPrintf("    },\n");
    }
    osSyncPrintf("};\n\n");
}

/**
 * Copies `src` to `dst`, used in Camera_Demo1
 * Name from AC map: Camera2_SetPos_Demo
 */
void Camera_Vec3fCopy(Vec3f* src, Vec3f* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

/**
 * Calculates new position from `at` to `pos`, outputs to `dst
 * Name from AC map: Camera2_CalcPos_Demo
 */
void Camera_RotateAroundPoint(PosRot* at, Vec3f* pos, Vec3f* dst) {
    VecSph posSph;
    Vec3f posCopy;

    Camera_Vec3fCopy(pos, &posCopy);
    OLib_Vec3fToVecSphGeo(&posSph, &posCopy);
    posSph.yaw += at->rot.y;
    Camera_Vec3fVecSphGeoAdd(dst, &at->pos, &posSph);
}

/**
 * Camera follows points specified at pointers to CutsceneCameraPoints,
 * camera->data0 for camera at positions, and camera->data1 for camera eye positions
 * until all keyFrames have been exhausted.
 */
s32 Camera_Demo1(Camera* camera) {
    s32 pad;
    Demo1* demo1 = (Demo1*)camera->paramData;
    CameraModeValue* values;
    Vec3f* at = &camera->at;
    CutsceneCameraPoint* csAtPoints = (CutsceneCameraPoint*)camera->data0;
    CutsceneCameraPoint* csEyePoints = (CutsceneCameraPoint*)camera->data1;
    Vec3f* eye = &camera->eye;
    PosRot curPlayerPosRot;
    Vec3f csEyeUpdate;
    Vec3f csAtUpdate;
    f32 newRoll;
    Vec3f* eyeNext = &camera->eyeNext;
    f32* cameraFOV = &camera->fov;
    s16* relativeToPlayer = &camera->data2;
    Demo1Anim* anim = &demo1->anim;

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        demo1->interfaceFlags = NEXTSETTING;
    }

    sCameraInterfaceFlags = demo1->interfaceFlags;

    switch (camera->animState) {
        case 0:
            // initalize camera state
            anim->keyframe = 0;
            anim->curFrame = 0.0f;
            camera->animState++;
            // "absolute" : "relative"
            osSyncPrintf(VT_SGR("1") "%06u:" VT_RST " camera: spline demo: start %s \n",
                         camera->play->state.frames, *relativeToPlayer == 0 ? "絶対" : "相対");

            if (PREG(93)) {
                Camera_DebugPrintSplineArray("CENTER", 5, csAtPoints);
                Camera_DebugPrintSplineArray("   EYE", 5, csEyePoints);
            }
        case 1:
            // follow CutsceneCameraPoints.  function returns 1 if at the end.
            if (func_800BB2B4(&csEyeUpdate, &newRoll, cameraFOV, csEyePoints, &anim->keyframe, &anim->curFrame) ||
                func_800BB2B4(&csAtUpdate, &newRoll, cameraFOV, csAtPoints, &anim->keyframe, &anim->curFrame)) {
                camera->animState++;
            }
            if (*relativeToPlayer) {
                // if the camera is set to be relative to the player, move the interpolated points
                // relative to the player's position
                if (camera->player != NULL && camera->player->actor.update != NULL) {
                    Actor_GetWorld(&curPlayerPosRot, &camera->player->actor);
                    Camera_RotateAroundPoint(&curPlayerPosRot, &csEyeUpdate, eyeNext);
                    Camera_RotateAroundPoint(&curPlayerPosRot, &csAtUpdate, at);
                } else {
                    osSyncPrintf(VT_COL(RED, WHITE) "camera: spline demo: owner dead\n" VT_RST);
                }
            } else {
                // simply copy the interpolated values to the eye and at
                Camera_Vec3fCopy(&csEyeUpdate, eyeNext);
                Camera_Vec3fCopy(&csAtUpdate, at);
            }
            *eye = *eyeNext;
            camera->roll = newRoll * 256.0f;
            camera->dist = OLib_Vec3fDist(at, eye);
            break;
    }
    return true;
}

s32 Camera_Demo2(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Opening large chests.
 * The camera position will be at a fixed point, and rotate around at different intervals.
 * The direction, and initial position is dependent on when the camera was started.
 */
s32 Camera_Demo3(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* camPlayerPosRot = &camera->playerPosRot;
    VecSph eyeAtOffset;
    VecSph eyeOffset;
    VecSph atOffset;
    Vec3f sp74;
    Vec3f sp68;
    Vec3f sp5C;
    f32 temp_f0;
    s32 pad;
    u8 skipUpdateEye = false;
    f32 yOffset = Player_GetHeight(camera->player);
    s16 angle;
    Demo3* demo3 = (Demo3*)camera->paramData;
    Demo3Anim* anim = &demo3->anim;
    s32 pad2;

    camera->unk_14C &= ~0x10;

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        demo3->fov = NEXTSETTING;
        demo3->unk_04 = NEXTSETTING; // unused.
        demo3->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, at, eye);

    sCameraInterfaceFlags = demo3->interfaceFlags;

    switch (camera->animState) {
        case 0:
            camera->unk_14C &= ~(0x8 | 0x4);
            func_80043B60(camera);
            camera->fov = demo3->fov;
            camera->roll = anim->animFrame = 0;
            anim->initialAt = camPlayerPosRot->pos;
            if (camera->playerGroundY != BGCHECK_Y_MIN) {
                anim->initialAt.y = camera->playerGroundY;
            }
            angle = camPlayerPosRot->rot.y;
            sp68.x = anim->initialAt.x + (Math_SinS(angle) * 40.0f);
            sp68.y = anim->initialAt.y + 40.0f;
            sp68.z = anim->initialAt.z + (Math_CosS(angle) * 40.0f);
            if (camera->play->state.frames & 1) {
                angle -= 0x3FFF;
                anim->yawDir = 1;
            } else {
                angle += 0x3FFF;
                anim->yawDir = -1;
            }
            sp74.x = sp68.x + (D_8011D658[1].r * Math_SinS(angle));
            sp74.y = anim->initialAt.y + 5.0f;
            sp74.z = sp68.z + (D_8011D658[1].r * Math_CosS(angle));
            if (Camera_BGCheck(camera, &sp68, &sp74)) {
                anim->yawDir = -anim->yawDir;
            }
            OLib_Vec3fToVecSphGeo(&atOffset, &D_8011D678[0]);
            atOffset.yaw += camPlayerPosRot->rot.y;
            Camera_Vec3fVecSphGeoAdd(at, &anim->initialAt, &atOffset);
            eyeOffset.r = D_8011D658[0].r;
            eyeOffset.pitch = D_8011D658[0].pitch;
            eyeOffset.yaw = (D_8011D658[0].yaw * anim->yawDir) + camPlayerPosRot->rot.y;
            anim->unk_0C = 1.0f;
            break;
        case 1:
            temp_f0 = (anim->animFrame - 2) * (1.0f / 146.0f);

            sp5C.x = F32_LERPIMP(D_8011D678[0].x, D_8011D678[1].x, temp_f0);
            sp5C.y = F32_LERPIMP(D_8011D678[0].y, D_8011D678[1].y, temp_f0);
            sp5C.z = F32_LERPIMP(D_8011D678[0].z, D_8011D678[1].z, temp_f0);

            OLib_Vec3fToVecSphGeo(&atOffset, &sp5C);
            atOffset.yaw = (atOffset.yaw * anim->yawDir) + camPlayerPosRot->rot.y;
            Camera_Vec3fVecSphGeoAdd(at, &anim->initialAt, &atOffset);

            atOffset.r = F32_LERPIMP(D_8011D658[0].r, D_8011D658[1].r, temp_f0);
            atOffset.pitch = BINANG_LERPIMP(D_8011D658[0].pitch, D_8011D658[1].pitch, temp_f0);
            atOffset.yaw = BINANG_LERPIMP(D_8011D658[0].yaw, D_8011D658[1].yaw, temp_f0);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * anim->yawDir) + camPlayerPosRot->rot.y;

            anim->unk_0C -= (1.0f / 365.0f);
            break;
        case 2:
            temp_f0 = (anim->animFrame - 0x94) * 0.1f;

            sp5C.x = F32_LERPIMP(D_8011D678[1].x, D_8011D678[2].x, temp_f0);
            sp5C.y = F32_LERPIMP((D_8011D678[1].y - yOffset), D_8011D678[2].y, temp_f0);
            sp5C.y += yOffset;
            sp5C.z = F32_LERPIMP(D_8011D678[1].z, D_8011D678[2].z, temp_f0);

            OLib_Vec3fToVecSphGeo(&atOffset, &sp5C);
            atOffset.yaw = (atOffset.yaw * anim->yawDir) + camPlayerPosRot->rot.y;
            Camera_Vec3fVecSphGeoAdd(at, &anim->initialAt, &atOffset);

            atOffset.r = F32_LERPIMP(D_8011D658[1].r, D_8011D658[2].r, temp_f0);
            atOffset.pitch = BINANG_LERPIMP(D_8011D658[1].pitch, D_8011D658[2].pitch, temp_f0);
            atOffset.yaw = BINANG_LERPIMP(D_8011D658[1].yaw, D_8011D658[2].yaw, temp_f0);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * anim->yawDir) + camPlayerPosRot->rot.y;
            anim->unk_0C -= 0.04f;
            break;
        case 3:
            temp_f0 = (anim->animFrame - 0x9F) * (1.0f / 9.0f);

            sp5C.x = F32_LERPIMP(D_8011D678[2].x, D_8011D678[3].x, temp_f0);
            sp5C.y = F32_LERPIMP(D_8011D678[2].y, D_8011D678[3].y, temp_f0);
            sp5C.y += yOffset;
            sp5C.z = F32_LERPIMP(D_8011D678[2].z, D_8011D678[3].z, temp_f0);

            OLib_Vec3fToVecSphGeo(&atOffset, &sp5C);
            atOffset.yaw = (atOffset.yaw * anim->yawDir) + camPlayerPosRot->rot.y;
            Camera_Vec3fVecSphGeoAdd(at, &anim->initialAt, &atOffset);

            atOffset.r = F32_LERPIMP(D_8011D658[2].r, D_8011D658[3].r, temp_f0);
            atOffset.pitch = BINANG_LERPIMP(D_8011D658[2].pitch, D_8011D658[3].pitch, temp_f0);
            atOffset.yaw = BINANG_LERPIMP(D_8011D658[2].yaw, D_8011D658[3].yaw, temp_f0);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * anim->yawDir) + camPlayerPosRot->rot.y;
            anim->unk_0C += (4.0f / 45.0f);
            break;
        case 30:
            camera->unk_14C |= 0x400;
            if (camera->unk_14C & 8) {
                camera->animState = 4;
            }
        case 10:
        case 20:
            skipUpdateEye = true;
            break;
        case 4:
            eyeOffset.r = 80.0f;
            eyeOffset.pitch = 0;
            eyeOffset.yaw = eyeAtOffset.yaw;
            anim->unk_0C = 0.1f;
            sCameraInterfaceFlags = 0x3400;

            if (!((anim->animFrame < 0 || camera->xzSpeed > 0.001f ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_A) ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_B) ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CLEFT) ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CDOWN) ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CUP) ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CRIGHT) ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_R) ||
                   CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_Z)) &&
                  camera->unk_14C & 8)) {
                goto skipeyeUpdate;
            }

        default:
            camera->unk_14C |= 0x14;
            camera->unk_14C &= ~8;
            if (camera->prevCamDataIdx < 0) {
                Camera_ChangeSettingFlags(camera, camera->prevSetting, 2);
            } else {
                Camera_ChangeDataIdx(camera, camera->prevCamDataIdx);
                camera->prevCamDataIdx = -1;
            }
            sCameraInterfaceFlags = 0;
        skipeyeUpdate:
            skipUpdateEye = true;
            break;
    }

    anim->animFrame++;

    if (anim->animFrame == 1) {
        camera->animState = 0xA;
    } else if (anim->animFrame == 2) {
        camera->animState = 0x1;
    } else if (anim->animFrame == 0x94) {
        camera->animState = 2;
    } else if (anim->animFrame == 0x9E) {
        camera->animState = 0x14;
    } else if (anim->animFrame == 0x9F) {
        camera->animState = 3;
    } else if (anim->animFrame == 0xA8) {
        camera->animState = 0x1E;
    } else if (anim->animFrame == 0xE4) {
        camera->animState = 4;
    }

    if (!skipUpdateEye) {
        eyeOffset.r = Camera_LERPCeilF(eyeOffset.r, eyeAtOffset.r, anim->unk_0C, 2.0f);
        eyeOffset.pitch = Camera_LERPCeilS(eyeOffset.pitch, eyeAtOffset.pitch, anim->unk_0C, 0xA);
        eyeOffset.yaw = Camera_LERPCeilS(eyeOffset.yaw, eyeAtOffset.yaw, anim->unk_0C, 0xA);
        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeOffset);
        *eye = *eyeNext;
    }

    camera->dist = OLib_Vec3fDist(at, eye);
    camera->atLERPStepScale = 0.1f;
    camera->posOffset.x = camera->at.x - camPlayerPosRot->pos.x;
    camera->posOffset.y = camera->at.y - camPlayerPosRot->pos.y;
    camera->posOffset.z = camera->at.z - camPlayerPosRot->pos.z;
    return true;
}

s32 Camera_Demo4(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Sets up a cutscene for Camera_Uniq9
 */
s32 Camera_Demo5(Camera* camera) {
    f32 eyeTargetDist;
    f32 sp90;
    VecSph playerTargetGeo;
    VecSph eyePlayerGeo;
    VecSph sp78;
    PosRot playerhead;
    PosRot targethead;
    Player* player;
    s16 sp4A;
    s32 pad;
    s32 temp_v0;
    s16 t;
    s32 pad2;

    Actor_GetFocus(&playerhead, &camera->player->actor);
    player = camera->player;
    sCameraInterfaceFlags = 0x3200;
    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        if (camera->target == NULL) {
            osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: warning: attention: target is not valid, stop!\n" VT_RST);
        }
        camera->target = NULL;
        return true;
    }
    Actor_GetFocus(&camera->targetPosRot, camera->target);
    OLib_Vec3fDiffToVecSphGeo(&playerTargetGeo, &camera->targetPosRot.pos, &camera->playerPosRot.pos);
    D_8011D3AC = camera->target->category;
    Actor_GetScreenPos(camera->play, camera->target, &sp78.yaw, &sp78.pitch);
    eyeTargetDist = OLib_Vec3fDist(&camera->targetPosRot.pos, &camera->eye);
    OLib_Vec3fDiffToVecSphGeo(&eyePlayerGeo, &playerhead.pos, &camera->eyeNext);
    sp4A = eyePlayerGeo.yaw - playerTargetGeo.yaw;
    if (camera->target->category == ACTORCAT_PLAYER) {
        // camera is targeting a(the) player actor
        if (eyePlayerGeo.r > 30.0f) {
            D_8011D6AC[1].timerInit = camera->timer - 1;
            D_8011D6AC[1].atTargetInit.z = Rand_ZeroOne() * 10.0f;
            D_8011D6AC[1].eyeTargetInit.x = Rand_ZeroOne() * 10.0f;
            ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D6AC;
            ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D6AC);
            if (camera->parentCamIdx != MAIN_CAM) {
                ONEPOINT_CS_INFO(camera)->keyFrameCnt--;
            } else {
                camera->timer += D_8011D6AC[2].timerInit;
            }
        } else {
            D_8011D724[1].eyeTargetInit.x = Rand_ZeroOne() * 10.0f;
            D_8011D724[1].timerInit = camera->timer - 1;
            ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D724;
            ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D724);
            if (camera->parentCamIdx != MAIN_CAM) {
                ONEPOINT_CS_INFO(camera)->keyFrameCnt--;
            } else {
                camera->timer += D_8011D724[2].timerInit;
            }
        }
    } else if (playerTargetGeo.r < 30.0f) {
        // distance between player and target is less than 30 units.
        ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D79C;
        ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D79C);
        if ((sp78.yaw < 0x15) || (sp78.yaw >= 0x12C) || (sp78.pitch < 0x29) || (sp78.pitch >= 0xC8)) {
            D_8011D79C[0].actionFlags = 0x41;
            D_8011D79C[0].atTargetInit.y = -30.0f;
            D_8011D79C[0].atTargetInit.x = 0.0f;
            D_8011D79C[0].atTargetInit.z = 0.0f;
            D_8011D79C[0].eyeTargetInit.y = 0.0f;
            D_8011D79C[0].eyeTargetInit.x = 10.0f;
            D_8011D79C[0].eyeTargetInit.z = -50.0f;
        }

        D_8011D79C[1].timerInit = camera->timer - 1;

        if (camera->parentCamIdx != MAIN_CAM) {
            ONEPOINT_CS_INFO(camera)->keyFrameCnt -= 2;
        } else {
            camera->timer += D_8011D79C[2].timerInit + D_8011D79C[3].timerInit;
        }
    } else if (eyeTargetDist < 300.0f && eyePlayerGeo.r < 30.0f) {
        // distance from the camera's current positon and the target is less than 300 units
        // and the distance fromthe camera's current position to the player is less than 30 units
        D_8011D83C[0].timerInit = camera->timer;
        ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D83C;
        ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D83C);
        if (camera->parentCamIdx != MAIN_CAM) {
            ONEPOINT_CS_INFO(camera)->keyFrameCnt--;
        } else {
            camera->timer += D_8011D83C[1].timerInit;
        }
    } else if (eyeTargetDist < 700.0f && ABS(sp4A) < 0x36B0) {
        // The distance between the camera's current position and the target is less than 700 units
        // and the angle between the camera's position and the player, and the player to the target
        // is less than ~76.9 degrees
        if (sp78.yaw >= 0x15 && sp78.yaw < 0x12C && sp78.pitch >= 0x29 && sp78.pitch < 0xC8 && eyePlayerGeo.r > 30.0f) {
            D_8011D88C[0].timerInit = camera->timer;
            ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D88C;
            ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D88C);
            if (camera->parentCamIdx != MAIN_CAM) {
                ONEPOINT_CS_INFO(camera)->keyFrameCnt--;
            } else {
                camera->timer += D_8011D88C[1].timerInit;
            }
        } else {
            D_8011D8DC[0].atTargetInit.z = eyeTargetDist * 0.6f;
            D_8011D8DC[0].eyeTargetInit.z = eyeTargetDist + 50.0f;
            D_8011D8DC[0].eyeTargetInit.x = Rand_ZeroOne() * 10.0f;
            if (BINANG_SUB(eyePlayerGeo.yaw, playerTargetGeo.yaw) > 0) {
                D_8011D8DC[0].atTargetInit.x = -D_8011D8DC[0].atTargetInit.x;
                D_8011D8DC[0].eyeTargetInit.x = -D_8011D8DC[0].eyeTargetInit.x;
                D_8011D8DC[0].rollTargetInit = -D_8011D8DC[0].rollTargetInit;
            }
            D_8011D8DC[0].timerInit = camera->timer;
            D_8011D8DC[1].timerInit = (s16)(eyeTargetDist * 0.005f) + 8;
            ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D8DC;
            ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D8DC);
            if (camera->parentCamIdx != MAIN_CAM) {
                ONEPOINT_CS_INFO(camera)->keyFrameCnt -= 2;
            } else {
                camera->timer += D_8011D8DC[1].timerInit + D_8011D8DC[2].timerInit;
            }
        }
    } else if (camera->target->category == ACTORCAT_DOOR) {
        // the target is a door.
        D_8011D954[0].timerInit = camera->timer - 5;
        sp4A = 0;
        if (!func_800C0D34(camera->play, camera->target, &sp4A)) {
            osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: attention demo: this door is dummy door!\n" VT_RST);
            if (ABS(playerTargetGeo.yaw - camera->target->shape.rot.y) >= 0x4000) {
                sp4A = camera->target->shape.rot.y;
            } else {
                sp4A = BINANG_ROT180(camera->target->shape.rot.y);
            }
        }

        D_8011D954[0].atTargetInit.y = D_8011D954[0].eyeTargetInit.y = D_8011D954[1].atTargetInit.y =
            camera->target->shape.rot.y == sp4A ? 180.0f : 0.0f;
        sp90 = (BINANG_SUB(playerTargetGeo.yaw, sp4A) < 0 ? 20.0f : -20.0f) * Rand_ZeroOne();
        D_8011D954[0].eyeTargetInit.y = D_8011D954->eyeTargetInit.y + sp90;
        temp_v0 = Rand_ZeroOne() * (sp90 * -0.2f);
        D_8011D954[1].rollTargetInit = temp_v0;
        D_8011D954[0].rollTargetInit = temp_v0;
        Actor_GetFocus(&targethead, camera->target);
        targethead.pos.x += 50.0f * Math_SinS(BINANG_ROT180(sp4A));
        targethead.pos.z += 50.0f * Math_CosS(BINANG_ROT180(sp4A));
        if (Camera_BGCheck(camera, &playerhead.pos, &targethead.pos)) {
            D_8011D954[1].actionFlags = 0xC1;
            D_8011D954[2].actionFlags = 0x8F;
        } else {
            D_8011D954[2].timerInit = (s16)(eyeTargetDist * 0.004f) + 6;
        }
        ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D954;
        ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D954);
        if (camera->parentCamIdx != MAIN_CAM) {
            ONEPOINT_CS_INFO(camera)->keyFrameCnt -= 2;
        } else {
            camera->timer += D_8011D954[2].timerInit + D_8011D954[3].timerInit;
        }
    } else {
        if (playerTargetGeo.r < 200.0f) {
            D_8011D9F4[0].eyeTargetInit.z = playerTargetGeo.r;
            D_8011D9F4[0].atTargetInit.z = playerTargetGeo.r * 0.25f;
        }
        if (playerTargetGeo.r < 400.0f) {
            D_8011D9F4[0].eyeTargetInit.x = Rand_ZeroOne() * 25.0f;
        }
        Player_GetHeight(camera->player);
        D_8011D9F4[0].timerInit = camera->timer;
        Actor_GetFocus(&targethead, camera->target);
        if (Camera_BGCheck(camera, &playerhead.pos, &targethead.pos)) {
            D_8011D9F4[1].timerInit = 4;
            D_8011D9F4[1].actionFlags = 0x8F;
        } else {
            t = eyeTargetDist * 0.005f;
            D_8011D9F4[1].timerInit = t + 8;
        }
        ONEPOINT_CS_INFO(camera)->keyFrames = D_8011D9F4;
        ONEPOINT_CS_INFO(camera)->keyFrameCnt = ARRAY_COUNT(D_8011D9F4);
        if (camera->parentCamIdx != MAIN_CAM) {
            if (camera->play->state.frames & 1) {
                D_8011D9F4[0].rollTargetInit = -D_8011D9F4[0].rollTargetInit;
                D_8011D9F4[1].rollTargetInit = -D_8011D9F4[1].rollTargetInit;
            }
            ONEPOINT_CS_INFO(camera)->keyFrameCnt -= 2;
        } else {
            camera->timer += D_8011D9F4[1].timerInit + D_8011D9F4[2].timerInit;
            D_8011D9F4[0].rollTargetInit = D_8011D9F4[1].rollTargetInit = 0;
        }
    }

    pad = sDemo5PrevSfxFrame - camera->play->state.frames;
    if ((pad >= 0x33) || (pad < -0x32)) {
        func_80078884(camera->data1);
    }

    sDemo5PrevSfxFrame = camera->play->state.frames;

    if (camera->player->stateFlags1 & 0x8000000 && (player->currentBoots != PLAYER_BOOTS_IRON)) {
        // swimming, and not iron boots
        player->stateFlags1 |= 0x20000000;
        // env frozen
        player->actor.freezeTimer = camera->timer;
    } else {
        sp4A = playerhead.rot.y - playerTargetGeo.yaw;
        if (camera->target->category == ACTORCAT_PLAYER) {
            pad = camera->play->state.frames - sDemo5PrevAction12Frame;
            if (player->stateFlags1 & 0x800) {
                // holding object over head.
                func_8002DF54(camera->play, camera->target, 8);
            } else if (ABS(pad) > 3000) {
                func_8002DF54(camera->play, camera->target, 12);
            } else {
                func_8002DF54(camera->play, camera->target, 69);
            }
        } else {
            func_8002DF54(camera->play, camera->target, 1);
        }
    }

    sDemo5PrevAction12Frame = camera->play->state.frames;
    Camera_ChangeSettingFlags(camera, CAM_SET_CS_C, (4 | 1));
    Camera_Unique9(camera);
    return true;
}

/**
 * Used in Forest Temple when poes are defeated, follows the flames to the torches.
 * Fixed position, rotates to follow the target
 */
s32 Camera_Demo6(Camera* camera) {
    Camera* mainCam;
    Demo6Anim* anim = (Demo6Anim*)&((Demo6*)camera->paramData)->anim;
    Vec3f* eyeNext = &camera->eyeNext;
    CameraModeValue* values;
    VecSph eyeOffset;
    Actor* camFocus;
    PosRot focusPosRot;
    s16 stateTimers[4];
    Vec3f* at = &camera->at;

    mainCam = Play_GetCamera(camera->play, MAIN_CAM);
    camFocus = camera->target;
    stateTimers[1] = 0x37;
    stateTimers[2] = 0x46;
    stateTimers[3] = 0x5A;

    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        ((Demo6*)camera->paramData)->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    switch (camera->animState) {
        case 0:
            // initalizes the camera state.
            anim->animTimer = 0;
            camera->fov = 60.0f;
            Actor_GetWorld(&focusPosRot, camFocus);
            camera->at.x = focusPosRot.pos.x;
            camera->at.y = focusPosRot.pos.y + 20.0f;
            camera->at.z = focusPosRot.pos.z;
            eyeOffset.r = 200.0f;
            // 0x7D0 ~10.99 degrees
            eyeOffset.yaw = Camera_XZAngle(&focusPosRot.pos, &mainCam->playerPosRot.pos) + 0x7D0;
            // -0x3E8 ~5.49 degrees
            eyeOffset.pitch = -0x3E8;
            Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeOffset);
            camera->eye = *eyeNext;
            camera->animState++;
        case 1:
            if (stateTimers[camera->animState] < anim->animTimer) {
                func_8002DF54(camera->play, &camera->player->actor, 8);
                Actor_GetWorld(&focusPosRot, camFocus);
                anim->atTarget.x = focusPosRot.pos.x;
                anim->atTarget.y = focusPosRot.pos.y - 20.0f;
                anim->atTarget.z = focusPosRot.pos.z;
                camera->animState++;
            } else {
                break;
            }
        case 2:
            Camera_LERPCeilVec3f(&anim->atTarget, at, 0.1f, 0.1f, 8.0f);
            if (stateTimers[camera->animState] < anim->animTimer) {
                camera->animState++;
            } else {
                break;
            }
        case 3:
            camera->fov = Camera_LERPCeilF(50.0f, camera->fov, 0.2f, 0.01f);
            if (stateTimers[camera->animState] < anim->animTimer) {
                camera->timer = 0;
                return true;
            }
            break;
    }

    anim->animTimer++;
    Actor_GetWorld(&focusPosRot, camFocus);

    return true;
}

s32 Camera_Demo7(Camera* camera) {
    if (camera->animState == 0) {
        camera->unk_14C &= ~4;
        camera->unk_14C |= 0x1000;
        camera->animState++;
    }
    //! @bug doesn't return
}

s32 Camera_Demo8(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Camera follows points specified by demo9.atPoints and demo9.eyePoints, allows finer control
 * over the final eye and at points than Camera_Demo1, by allowing the interpolated at and eye points
 * to be relative to the main camera's player, the current camera's player, or the main camera's target
 */
s32 Camera_Demo9(Camera* camera) {
    s32 pad;
    s32 finishAction;
    s16 onePointTimer;
    Demo9OnePointCs* demo9OnePoint = (Demo9OnePointCs*)camera->paramData;
    Vec3f csEyeUpdate;
    Vec3f csAtUpdate;
    Vec3f newEye;
    Vec3f newAt;
    f32 newRoll;
    CameraModeValue* values;
    Camera* mainCam;
    Vec3f* eye = &camera->eye;
    PosRot* mainCamPlayerPosRot;
    PosRot focusPosRot;
    s32 pad3;
    Vec3f* eyeNext = &camera->eyeNext;
    Demo9* demo9 = &demo9OnePoint->demo9;
    Vec3f* at = &camera->at;
    f32* camFOV = &camera->fov;
    Demo9Anim* anim = &demo9->anim;

    mainCam = Play_GetCamera(camera->play, MAIN_CAM);
    mainCamPlayerPosRot = &mainCam->playerPosRot;
    if (RELOAD_PARAMS) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        demo9->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = demo9->interfaceFlags;

    switch (camera->animState) {
        case 0:
            // initalize the camera state
            anim->keyframe = 0;
            anim->finishAction = 0;
            anim->curFrame = 0.0f;
            camera->animState++;
            anim->doLERPAt = false;
            finishAction = demo9OnePoint->onePointCs.actionParameters & 0xF000;
            if (finishAction != 0) {
                anim->finishAction = finishAction;

                // Clear finish parameters
                demo9OnePoint->onePointCs.actionParameters &= 0xFFF;
            }
            anim->animTimer = demo9OnePoint->onePointCs.initTimer;
        case 1:
            // Run the camera state
            if (anim->animTimer > 0) {
                // if the animation timer is still running, run the demo logic
                // if it is not, then the case will fallthrough to the finish logic.

                // Run the at and eye cs interpoloation functions, if either of them return 1 (that no more points
                // exist) change the animation state to 2 (standby)
                if (func_800BB2B4(&csEyeUpdate, &newRoll, camFOV, demo9OnePoint->onePointCs.eyePoints, &anim->keyframe,
                                  &anim->curFrame) != 0 ||
                    func_800BB2B4(&csAtUpdate, &newRoll, camFOV, demo9OnePoint->onePointCs.atPoints, &anim->keyframe,
                                  &anim->curFrame) != 0) {
                    camera->animState = 2;
                }

                if (demo9OnePoint->onePointCs.actionParameters == 1) {
                    // rotate around mainCam's player
                    Camera_RotateAroundPoint(mainCamPlayerPosRot, &csEyeUpdate, &newEye);
                    Camera_RotateAroundPoint(mainCamPlayerPosRot, &csAtUpdate, &newAt);
                } else if (demo9OnePoint->onePointCs.actionParameters == 4) {
                    // rotate around the current camera's player
                    Actor_GetWorld(&focusPosRot, &camera->player->actor);
                    Camera_RotateAroundPoint(&focusPosRot, &csEyeUpdate, &newEye);
                    Camera_RotateAroundPoint(&focusPosRot, &csAtUpdate, &newAt);
                } else if (demo9OnePoint->onePointCs.actionParameters == 8) {
                    // rotate around the current camera's target
                    if (camera->target != NULL && camera->target->update != NULL) {
                        Actor_GetWorld(&focusPosRot, camera->target);
                        Camera_RotateAroundPoint(&focusPosRot, &csEyeUpdate, &newEye);
                        Camera_RotateAroundPoint(&focusPosRot, &csAtUpdate, &newAt);
                    } else {
                        camera->target = NULL;
                        newEye = *eye;
                        newAt = *at;
                    }
                } else {
                    // simple copy
                    Camera_Vec3fCopy(&csEyeUpdate, &newEye);
                    Camera_Vec3fCopy(&csAtUpdate, &newAt);
                }

                *eyeNext = newEye;
                *eye = *eyeNext;
                if (anim->doLERPAt) {
                    Camera_LERPCeilVec3f(&newAt, at, 0.5f, 0.5f, 0.1f);
                } else {
                    *at = newAt;
                    anim->doLERPAt = true;
                }
                camera->roll = newRoll * 256.0f;
                anim->animTimer--;
                break;
            }
        case 3:
            // the cs is finished, decide the next action
            camera->timer = 0;
            if (anim->finishAction != 0) {
                if (anim->finishAction != 0x1000) {
                    if (anim->finishAction == 0x2000) {
                        // finish action = 0x2000, run OnePointCs 0x3FC (Dramatic Return to Link)
                        onePointTimer =
                            demo9OnePoint->onePointCs.initTimer < 50 ? 5 : demo9OnePoint->onePointCs.initTimer / 5;
                        OnePointCutscene_Init(camera->play, 1020, onePointTimer, NULL, camera->parentCamIdx);
                    }
                } else {
                    // finish action = 0x1000, copy the current camera's values to the
                    // default camera.
                    Camera_Copy(mainCam, camera);
                }
            }
            break;
        case 2:
            // standby while the timer finishes, change the animState to finish when
            // the timer runs out.
            anim->animTimer--;
            if (anim->animTimer < 0) {
                camera->animState++;
            }
            break;
        case 4:
            // do nothing.
            break;
    }

    return true;
}

s32 Camera_Demo0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special0(Camera* camera) {
    PosRot* playerPosRot = &camera->playerPosRot;
    Special0* spec0 = (Special0*)camera->paramData;

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        spec0->lerpAtScale = NEXTPCT;
        spec0->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    sCameraInterfaceFlags = spec0->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
    }

    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        if (camera->target == NULL) {
            osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: warning: circle: target is not valid, stop!\n" VT_RST);
        }
        camera->target = NULL;
        return true;
    }

    Actor_GetFocus(&camera->targetPosRot, camera->target);
    Camera_LERPCeilVec3f(&camera->targetPosRot.pos, &camera->at, spec0->lerpAtScale, spec0->lerpAtScale, 0.1f);

    camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->posOffset.z = camera->at.z - playerPosRot->pos.z;

    camera->dist = OLib_Vec3fDist(&camera->at, &camera->eye);
    camera->xzSpeed = 0.0f;
    if (camera->timer > 0) {
        camera->timer--;
    }
    return true;
}

s32 Camera_Special1(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special2(Camera* camera) {
    return Camera_Unique2(camera);
}

s32 Camera_Special3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special4(Camera* camera) {
    PosRot curTargetPosRot;
    s16 sp3A;
    s16* timer = &camera->timer;
    Special4* spec4 = (Special4*)camera->paramData;

    if (camera->animState == 0) {
        sCameraInterfaceFlags = 0x3200;
        camera->fov = 40.0f;
        camera->animState++;
        spec4->initalTimer = camera->timer;
    }

    camera->fov = Camera_LERPCeilF(80.0f, camera->fov, 1.0f / *timer, 0.1f);
    if ((spec4->initalTimer - *timer) < 0xF) {
        (*timer)--;
        return false;
    } else {
        camera->roll = -0x1F4;
        Actor_GetWorld(&curTargetPosRot, camera->target);

        camera->at = curTargetPosRot.pos;
        camera->at.y -= 150.0f;

        // 0x3E8 ~ 5.49 degrees
        sp3A = BINANG_ROT180(curTargetPosRot.rot.y) + 0x3E8;
        camera->eye.x = camera->eyeNext.x = (Math_SinS(sp3A) * 780.0f) + camera->at.x;
        camera->eyeNext.y = camera->at.y;
        camera->eye.z = camera->eyeNext.z = (Math_CosS(sp3A) * 780.0f) + camera->at.z;
        camera->eye.y = curTargetPosRot.pos.y;
        camera->eye.y = Camera_GetFloorY(camera, &camera->eye) + 20.0f;
        (*timer)--;
        return true;
    }
}

/**
 * Flying with hookshot
 */
s32 Camera_Special5(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot spA8;
    s16 pad;
    s16 spA4;
    CamColChk sp7C;
    VecSph sp74;
    VecSph sp6C;
    VecSph sp64;
    VecSph sp5C;
    PosRot* playerPosRot = &camera->playerPosRot;
    Special5* spec5 = (Special5*)camera->paramData;
    Special5Anim* anim = &spec5->anim;
    f32 temp_f0_2;
    f32 yOffset;

    yOffset = Player_GetHeight(camera->player);
    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / yOffset));
        spec5->yOffset = (NEXTPCT * yOffset) * yNormal;
        spec5->eyeDist = NEXTSETTING;
        spec5->minDistForRot = NEXTSETTING;
        spec5->timerInit = NEXTSETTING;
        spec5->pitch = DEGF_TO_BINANG(NEXTSETTING);
        spec5->fovTarget = NEXTSETTING;
        spec5->atMaxLERPScale = NEXTPCT;
        spec5->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&sp64, at, eye);
    OLib_Vec3fDiffToVecSphGeo(&sp5C, at, eyeNext);
    Actor_GetWorld(&spA8, camera->target);

    sCameraInterfaceFlags = spec5->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        anim->animTimer = spec5->timerInit;
    }

    if (anim->animTimer > 0) {
        anim->animTimer--;
    } else if (anim->animTimer == 0) {
        if (camera->target == NULL || camera->target->update == NULL) {
            camera->target = NULL;
            return true;
        }

        anim->animTimer--;
        if (spec5->minDistForRot < OLib_Vec3fDist(&spA8.pos, &playerPosRot->pos)) {
            sp6C.yaw = playerPosRot->rot.y;
            sp6C.pitch = -playerPosRot->rot.x;
            sp6C.r = 20.0f;
            Camera_Vec3fVecSphGeoAdd(&sp7C.pos, &spA8.pos, &sp6C);
            Camera_BGCheckInfo(camera, at, &sp7C);
            OLib_Vec3fToVecSphGeo(&sp6C, &sp7C.norm);
            spA4 = BINANG_SUB(playerPosRot->rot.y, sp6C.yaw);
            sp74.r = spec5->eyeDist;
            temp_f0_2 = Rand_ZeroOne();
            sp74.yaw =
                BINANG_ROT180(playerPosRot->rot.y) + (s16)(spA4 < 0 ? -(s16)(0x1553 + (s16)(temp_f0_2 * 2730.0f))
                                                                    : (s16)(0x1553 + (s16)(temp_f0_2 * 2730.0f)));
            sp74.pitch = spec5->pitch;
            Camera_Vec3fVecSphGeoAdd(eyeNext, &spA8.pos, &sp74);
            *eye = *eyeNext;
            Camera_BGCheck(camera, &spA8.pos, eye);
        }
    }

    Camera_CalcAtDefault(camera, &sp5C, spec5->yOffset, 0);
    camera->fov = Camera_LERPCeilF(spec5->fovTarget, camera->fov, camera->atLERPStepScale * PCT(OREG(4)), 1.0f);
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = Camera_ClampLERPScale(camera, spec5->atMaxLERPScale);
    return true;
}

/**
 * Camera's eye is fixed at points specified at D_8011DA6C / D_8011DA9C
 * depending on the player's position
 */
s32 Camera_Special7(Camera* camera) {
    Special7* spec7 = (Special7*)camera->paramData;
    PosRot* playerPosRot = &camera->playerPosRot;
    Vec3f atTarget;
    f32 yOffset;
    f32 temp_f0;

    yOffset = Player_GetHeight(camera->player);
    if (camera->animState == 0) {
        if (camera->play->sceneNum == SCENE_JYASINZOU) {
            // Spirit Temple
            spec7->idx = 3;
        } else if (playerPosRot->pos.x < 1500.0f) {
            spec7->idx = 2;
        } else if (playerPosRot->pos.y < 3000.0f) {
            spec7->idx = 0;
        } else {
            spec7->idx = 1;
        }
        camera->animState++;
        camera->roll = 0;
    }

    if (camera->at.y < D_8011DACC[spec7->idx]) {
        atTarget = playerPosRot->pos;
        atTarget.y -= 20.0f;
        Camera_LERPCeilVec3f(&atTarget, &camera->at, 0.4f, 0.4f, 0.10f);
        camera->eye = camera->eyeNext = D_8011DA6C[spec7->idx];
        temp_f0 = (playerPosRot->pos.y - D_8011DADC[spec7->idx]) / (D_8011DACC[spec7->idx] - D_8011DADC[spec7->idx]);
        camera->roll = D_8011DAEC[spec7->idx] * temp_f0;
        camera->fov = (20.0f * temp_f0) + 60.0f;
    } else {
        atTarget = playerPosRot->pos;
        atTarget.y += yOffset;
        Camera_LERPCeilVec3f(&atTarget, &camera->at, 0.4f, 0.4f, 0.1f);
        camera->roll = 0;
        camera->eye = camera->eyeNext = D_8011DA9C[spec7->idx];
        camera->fov = 70.0f;
    }

    camera->dist = OLib_Vec3fDist(&camera->at, &camera->eye);
    camera->atLERPStepScale = 0.0f;
    camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->posOffset.z = camera->at.z - playerPosRot->pos.z;
    return true;
}

/**
 * Courtyard.
 * Camera's eye is fixed on the z plane, slides on the xy plane with link
 * When the camera's scene data changes the animation to the next "screen"
 * happens for 12 frames.  The camera's eyeNext is the scene's camera data's position
 */
s32 Camera_Special6(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecSph atOffset;
    Vec3f sceneCamPos;
    Vec3f eyePosCalc;
    Vec3f eyeAnim;
    Vec3f atAnim;
    VecSph eyeAtOffset;
    PosRot* playerPosRot = &camera->playerPosRot;
    Vec3s* sceneCamData;
    Vec3s sceneCamRot;
    s16 fov;
    f32 sp54;
    f32 timerF;
    f32 timerDivisor;
    Special6* spec6 = (Special6*)camera->paramData;
    Special6Anim* anim = &spec6->anim;
    s32 pad;

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        spec6->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    OLib_Vec3fDiffToVecSphGeo(&eyeAtOffset, eye, at);

    sceneCamData = Camera_GetCamBGData(camera);
    Camera_Vec3sToVec3f(&sceneCamPos, &BGCAM_POS(sceneCamData));
    sceneCamRot = BGCAM_ROT(sceneCamData);
    fov = BGCAM_FOV(sceneCamData);
    if (fov == -1) {
        fov = 6000;
    }

    if (fov < 361) {
        fov *= 100;
    }

    sCameraInterfaceFlags = spec6->interfaceFlags;

    if (eyeNext->x != sceneCamPos.x || eyeNext->y != sceneCamPos.y || eyeNext->z != sceneCamPos.z ||
        camera->animState == 0) {
        // A change in the current scene's camera positon has been detected,
        // Change "screens"
        camera->player->actor.freezeTimer = 12;
        sCameraInterfaceFlags = (sCameraInterfaceFlags & 0xF0FF) | 0x300;
        anim->initalPlayerY = playerPosRot->pos.y;
        anim->animTimer = 12;
        *eyeNext = sceneCamPos;
        if (camera->animState == 0) {
            camera->animState++;
        }
    }

    if (anim->animTimer > 0) {
        // In transition between "screens"
        timerF = anim->animTimer;
        eyePosCalc = *eyeNext;
        eyePosCalc.x += (playerPosRot->pos.x - eyePosCalc.x) * 0.5f;
        eyePosCalc.y += (playerPosRot->pos.y - anim->initalPlayerY) * 0.2f;
        eyeAnim = eyePosCalc;
        eyeAnim.y = Camera_LERPCeilF(eyePosCalc.y, eye->y, 0.5f, 0.01f);

        // set the at point to be 100 units from the eye looking at the
        // direction specified in the scene's camera data.
        atOffset.r = 100.0f;
        atOffset.yaw = sceneCamRot.y;
        atOffset.pitch = -sceneCamRot.x;
        Camera_Vec3fVecSphGeoAdd(&atAnim, &eyeAnim, &atOffset);
        timerDivisor = 1.0f / timerF;
        eye->x += (eyeAnim.x - eye->x) * timerDivisor;
        eye->y += (eyeAnim.y - eye->y) * timerDivisor;
        eye->z += (eyeAnim.z - eye->z) * timerDivisor;
        at->x += (atAnim.x - at->x) * timerDivisor;
        at->y += (atAnim.y - at->y) * timerDivisor;
        at->z += (atAnim.z - at->z) * timerDivisor;
        camera->fov += (PCT(fov) - camera->fov) / anim->animTimer;
        anim->animTimer--;
    } else {
        // Camera following link on the x axis.
        sCameraInterfaceFlags &= 0xF0FF;
        eyePosCalc = *eyeNext;
        eyePosCalc.x += (playerPosRot->pos.x - eyePosCalc.x) * 0.5f;
        eyePosCalc.y += (playerPosRot->pos.y - anim->initalPlayerY) * 0.2f;
        *eye = eyePosCalc;
        eye->y = Camera_LERPCeilF(eyePosCalc.y, eye->y, 0.5f, 0.01f);

        // set the at point to be 100 units from the eye looking at the
        // direction specified in the scene's camera data.
        atOffset.r = 100.0f;
        atOffset.yaw = sceneCamRot.y;
        atOffset.pitch = -sceneCamRot.x;
        Camera_Vec3fVecSphGeoAdd(at, eye, &atOffset);
    }
    return true;
}

s32 Camera_Special8(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special9(Camera* camera) {
    s32 pad;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f spAC;
    VecSph eyeAdjustment;
    VecSph atEyeOffsetGeo;
    f32 playerYOffset;
    s32 pad3;
    PosRot* playerPosRot = &camera->playerPosRot;
    PosRot adjustedPlayerPosRot;
    f32 yNormal;
    Special9* spec9 = (Special9*)camera->paramData;
    Special9Params* params = &spec9->params;
    Special9Anim* anim = &params->anim;
    s32 pad4;
    Vec3s* camPosData;

    playerYOffset = Player_GetHeight(camera->player);
    camera->unk_14C &= ~0x10;
    yNormal = (1.0f + PCT(R_CAM_YOFFSET_NORM)) - (PCT(R_CAM_YOFFSET_NORM) * (68.0f / playerYOffset));

    if (RELOAD_PARAMS) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        params->yOffset = NEXTPCT * playerYOffset * yNormal;
        params->unk_04 = NEXTSETTING;
        params->interfaceFlags = NEXTSETTING;
    }

    if (R_RELOAD_CAM_PARAMS) {
        Camera_CopyPREGToModeValues(camera);
    }

    if (spec9->doorParams.doorActor != NULL) {
        Actor_GetWorldPosShapeRot(&adjustedPlayerPosRot, spec9->doorParams.doorActor);
    } else {
        adjustedPlayerPosRot = *playerPosRot;
        adjustedPlayerPosRot.pos.y += playerYOffset + params->yOffset;
        adjustedPlayerPosRot.rot.x = 0;
    }

    OLib_Vec3fDiffToVecSphGeo(&atEyeOffsetGeo, at, eye);

    sCameraInterfaceFlags = params->interfaceFlags;

    switch (camera->animState) {

        case 0:
            camera->unk_14C &= ~(0x4 | 0x2);
            camera->animState++;
            anim->targetYaw = ABS(playerPosRot->rot.y - adjustedPlayerPosRot.rot.y) >= 0x4000
                                  ? BINANG_ROT180(adjustedPlayerPosRot.rot.y)
                                  : adjustedPlayerPosRot.rot.y;
        case 1:
            spec9->doorParams.timer1--;
            if (spec9->doorParams.timer1 <= 0) {
                FrameInterpolation_DontInterpolateCamera();
                camera->animState++;
                if (params->interfaceFlags & 1) {
                    camPosData = Camera_GetCamBGData(camera);
                    Camera_Vec3sToVec3f(eyeNext, &BGCAM_POS(camPosData));
                    spAC = *eye = *eyeNext;
                } else {
                    s16 yaw;

                    // 0xE38 ~ 20 degrees
                    eyeAdjustment.pitch = 0xE38;
                    // 0xAAA ~ 15 degrees.
                    yaw = 0xAAA * ((camera->play->state.frames & 1) ? 1 : -1);
                    eyeAdjustment.yaw = anim->targetYaw + yaw;
                    eyeAdjustment.r = 200.0f * yNormal;
                    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeAdjustment);
                    spAC = *eye = *eyeNext;
                    if (Camera_CheckOOB(camera, &spAC, &playerPosRot->pos)) {
                        yaw = -yaw;
                        eyeAdjustment.yaw = anim->targetYaw + yaw;
                        Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeAdjustment);
                        *eye = *eyeNext;
                    }
                }
            } else {
                break;
            }
        case 2:
            spAC = playerPosRot->pos;
            spAC.y += playerYOffset + params->yOffset;

            Camera_LERPCeilVec3f(&spAC, at, 0.25f, 0.25f, 0.1f);
            spec9->doorParams.timer2--;
            if (spec9->doorParams.timer2 <= 0) {
                camera->animState++;
                anim->targetYaw = BINANG_ROT180(anim->targetYaw);
            } else {
                break;
            }
        case 3:
            spAC = playerPosRot->pos;
            spAC.y += (playerYOffset + params->yOffset);
            Camera_LERPCeilVec3f(&spAC, at, 0.5f, 0.5f, 0.1f);
            eyeAdjustment.pitch = Camera_LERPCeilS(0xAAA, atEyeOffsetGeo.pitch, 0.3f, 0xA);
            eyeAdjustment.yaw = Camera_LERPCeilS(anim->targetYaw, atEyeOffsetGeo.yaw, 0.3f, 0xA);
            eyeAdjustment.r = Camera_LERPCeilF(60.0f, atEyeOffsetGeo.r, 0.3f, 1.0f);
            Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeAdjustment);
            *eye = *eyeNext;
            spec9->doorParams.timer3--;
            if (spec9->doorParams.timer3 <= 0) {
                camera->animState++;
            } else {
                break;
            }
        case 4:
            camera->animState++;
        default:
            camera->unk_14C |= (0x400 | 0x10);
            sCameraInterfaceFlags = 0;

            if (camera->xzSpeed > 0.001f || CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_A) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_B) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CLEFT) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CDOWN) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CUP) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_CRIGHT) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_R) ||
                CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_Z) || params->interfaceFlags & 0x8) {

                Camera_ChangeSettingFlags(camera, camera->prevSetting, 2);
                camera->unk_14C |= (0x4 | 0x2);
            }
            break;
    }
    spAC = playerPosRot->pos;
    spAC.y += playerYOffset;
    camera->dist = OLib_Vec3fDist(&spAC, eye);
    camera->posOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->posOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->posOffset.z = camera->at.z - playerPosRot->pos.z;
    return true;
}

Camera* Camera_Create(View* view, CollisionContext* colCtx, PlayState* play) {
    Camera* newCamera = ZELDA_ARENA_MALLOC_DEBUG(sizeof(*newCamera));

    if (newCamera != NULL) {
        osSyncPrintf(VT_FGCOL(BLUE) "camera: create --- allocate %d byte" VT_RST "\n", sizeof(*newCamera) * 4);
        Camera_Init(newCamera, view, colCtx, play);
    } else {
        osSyncPrintf(VT_COL(RED, WHITE) "camera: create: not enough memory\n" VT_RST);
    }
    return newCamera;
}

void Camera_Destroy(Camera* camera) {
    if (camera != NULL) {
        osSyncPrintf(VT_FGCOL(BLUE) "camera: destroy ---" VT_RST "\n");
        ZELDA_ARENA_FREE_DEBUG(camera);
    } else {
        osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: destroy: already cleared\n" VT_RST);
    }
}

void Camera_Init(Camera* camera, View* view, CollisionContext* colCtx, PlayState* play) {
    Camera* camP;
    s32 i;
    s16 curUID;
    s16 j;

    memset(camera, 0, sizeof(*camera));
    if (sInitRegs) {
        for (i = 0; i < sOREGInitCnt; i++) {
            OREG(i) = sOREGInit[i];
        }

        for (i = 0; i < sCamDataRegsInitCount; i++) {
            R_CAM_DATA(i) = sCamDataRegsInit[i];
        }

        DbCamera_Reset(camera, &D_8015BD80);
        sInitRegs = false;
        PREG(88) = -1;
    }
    camera->play = D_8015BD7C = play;
    DbCamera_Init(&D_8015BD80, camera);
    curUID = sNextUID;
    sNextUID++;
    while (curUID != 0) {
        if (curUID == 0) {
            sNextUID++;
        }

        for (j = 0; j < NUM_CAMS; j++) {
            camP = camera->play->cameraPtrs[j];
            if (camP != NULL && curUID == camP->uid) {
                break;
            }
        }

        if (j == 4) {
            break;
        }

        curUID = sNextUID++;
    }

    // ~ 90 degrees
    camera->inputDir.y = 0x3FFF;
    camera->uid = curUID;
    camera->camDir = camera->inputDir;
    camera->rUpdateRateInv = 10.0f;
    camera->yawUpdateRateInv = 10.0f;
    camera->up.x = 0.0f;
    camera->up.y = 1.0f;
    camera->up.z = 0.0f;
    camera->fov = 60.0f;
    camera->pitchUpdateRateInv = R_CAM_DEFA_PHI_UPDRATE;
    camera->xzOffsetUpdateRate = PCT(OREG(2));
    camera->yOffsetUpdateRate = PCT(OREG(3));
    camera->fovUpdateRate = PCT(OREG(4));
    sCameraShrinkWindowVal = 0x20;
    sCameraInterfaceAlpha = 0;
    camera->unk_14C = 0;
    camera->setting = camera->prevSetting = CAM_SET_FREE0;
    camera->camDataIdx = camera->prevCamDataIdx = -1;
    camera->mode = 0;
    camera->bgCheckId = BGCHECK_SCENE;
    camera->csId = 0x7FFF;
    camera->timer = -1;
    camera->unk_14C |= 0x4000;

    camera->up.y = 1.0f;
    camera->up.z = camera->up.x = 0.0f;
    camera->skyboxOffset.x = camera->skyboxOffset.y = camera->skyboxOffset.z = 0;
    camera->atLERPStepScale = 1;
    sCameraInterfaceFlags = 0xFF00;
    sDbgModeIdx = -1;
    D_8011D3F0 = 3;
    osSyncPrintf(VT_FGCOL(BLUE) "camera: initialize --- " VT_RST " UID %d\n", camera->uid);
}

void func_80057FC4(Camera* camera) {
    if (camera != &camera->play->mainCamera) {
        camera->prevSetting = camera->setting = CAM_SET_FREE0;
        camera->unk_14C &= ~0x4;
    } else if (camera->play->roomCtx.curRoom.meshHeader->base.type != 1) {
        switch (camera->play->roomCtx.curRoom.behaviorType1) {
            case ROOM_BEHAVIOR_TYPE1_1:
                Camera_ChangeDoorCam(camera, NULL, -99, 0, 0, 18, 10);
                camera->prevSetting = camera->setting = CAM_SET_DUNGEON0;
                break;
            case ROOM_BEHAVIOR_TYPE1_0:
                osSyncPrintf("camera: room type: default set field\n");
                Camera_ChangeDoorCam(camera, NULL, -99, 0, 0, 18, 10);
                camera->prevSetting = camera->setting = CAM_SET_NORMAL0;
                break;
            default:
                osSyncPrintf("camera: room type: default set etc (%d)\n",
                             camera->play->roomCtx.curRoom.behaviorType1);
                Camera_ChangeDoorCam(camera, NULL, -99, 0, 0, 18, 10);
                camera->prevSetting = camera->setting = CAM_SET_NORMAL0;
                camera->unk_14C |= 4;
                break;
        }
    } else {
        osSyncPrintf("camera: room type: prerender\n");
        camera->prevSetting = camera->setting = CAM_SET_FREE0;
        camera->unk_14C &= ~0x4;
    }
}

void Camera_Stub80058140(Camera* camera) {
}

void Camera_InitPlayerSettings(Camera* camera, Player* player) {
    PosRot playerPosShape;
    VecSph eyeNextAtOffset;
    s32 bgId;
    Vec3f floorPos;
    s32 upXZ;
    f32 playerYOffset;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;

    Actor_GetWorldPosShapeRot(&playerPosShape, &player->actor);
    playerYOffset = Player_GetHeight(player);
    camera->player = player;
    camera->playerPosRot = playerPosShape;
    camera->dist = eyeNextAtOffset.r = 180.0f;
    camera->inputDir.y = playerPosShape.rot.y;
    eyeNextAtOffset.yaw = BINANG_ROT180(camera->inputDir.y);
    camera->inputDir.x = eyeNextAtOffset.pitch = 0x71C;
    camera->inputDir.z = 0;
    camera->camDir = camera->inputDir;
    camera->xzSpeed = 0.0f;
    camera->playerPosDelta.y = 0.0f;
    camera->at = playerPosShape.pos;
    camera->at.y += playerYOffset;

    camera->posOffset.x = 0;
    camera->posOffset.y = playerYOffset;
    camera->posOffset.z = 0;

    Camera_Vec3fVecSphGeoAdd(eyeNext, at, &eyeNextAtOffset);
    *eye = *eyeNext;
    camera->roll = 0;

    upXZ = 0;
    camera->up.z = upXZ;
    camera->up.y = 1.0f;
    camera->up.x = upXZ;

    if (Camera_GetFloorYNorm(camera, &floorPos, at, &bgId) != BGCHECK_Y_MIN) {
        camera->bgCheckId = bgId;
    }

    camera->waterPrevCamIdx = -1;
    camera->waterPrevCamSetting = -1;
    camera->unk_14C |= 4;

    if (camera == &camera->play->mainCamera) {
        sCameraInterfaceFlags = 0xB200;
    } else {
        sCameraInterfaceFlags = 0;
    }

    func_80057FC4(camera);
    camera->unk_14A = 0;
    camera->paramFlags = 0;
    camera->nextCamDataIdx = -1;
    camera->atLERPStepScale = 1.0f;
    Camera_CopyDataToRegs(camera, camera->mode);
    Camera_QRegInit();
    osSyncPrintf(VT_FGCOL(BLUE) "camera: personalize ---" VT_RST "\n");

    if (camera->thisIdx == MAIN_CAM) {
        Camera_UpdateWater(camera);
    }
}

s16 Camera_ChangeStatus(Camera* camera, s16 status) {
    CameraModeValue* values;
    CameraModeValue* valueP;
    s32 i;

    if (PREG(82)) {
        osSyncPrintf("camera: change camera status: cond %c%c\n", status == 7 ? 'o' : 'x',
                     camera->status != 7 ? 'o' : 'x');
    }

    if (PREG(82)) {
        osSyncPrintf("camera: res: stat (%d/%d/%d)\n", camera->thisIdx, camera->setting, camera->mode);
    }

    if (status == CAM_STAT_ACTIVE && camera->status != CAM_STAT_ACTIVE) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        for (i = 0; i < sCameraSettings[camera->setting].cameraModes[camera->mode].valueCnt; i++) {
            valueP = &values[i];
            R_CAM_DATA(valueP->dataType) = valueP->val;
            if (PREG(82)) {
                osSyncPrintf("camera: change camera status: PREG(%02d) = %d\n", valueP->dataType, valueP->val);
            }
        }
    }
    camera->status = status;
    return camera->status;
}

void Camera_PrintSettings(Camera* camera) {
    char sp58[8];
    char sp50[8];
    char sp48[8];
    s32 i;

    if ((OREG(0) & 1) && (camera->play->activeCamera == camera->thisIdx) && !gDbgCamEnabled) {
        for (i = 0; i < NUM_CAMS; i++) {
            if (camera->play->cameraPtrs[i] == NULL) {
                sp58[i] = '-';
                sp48[i] = ' ';
            } else {
                switch (camera->play->cameraPtrs[i]->status) {
                    case 0:
                        sp58[i] = 'c';
                        break;
                    case 1:
                        sp58[i] = 'w';
                        break;
                    case 3:
                        sp58[i] = 's';
                        break;
                    case 7:
                        sp58[i] = 'a';
                        break;
                    case 0x100:
                        sp58[i] = 'd';
                        break;
                    default:
                        sp58[i] = '*';
                        break;
                }
            }
            sp48[i] = ' ';
        }
        sp58[i] = '\0';
        sp48[i] = '\0';

        sp48[camera->play->activeCamera] = 'a';
        func_8006376C(3, 0x16, 5, sp58);
        func_8006376C(3, 0x16, 1, sp48);
        func_8006376C(3, 0x17, 5, "S:");
        func_8006376C(5, 0x17, 4, sCameraSettingNames[camera->setting]);
        func_8006376C(3, 0x18, 5, "M:");
        func_8006376C(5, 0x18, 4, sCameraModeNames[camera->mode]);
        func_8006376C(3, 0x19, 5, "F:");
        func_8006376C(5, 0x19, 4,
                      sCameraFunctionNames[sCameraSettings[camera->setting].cameraModes[camera->mode].funcIdx]);

        i = 0;
        if (camera->camDataIdx < 0) {
            sp50[i++] = '-';
        }

        //! @bug: this code was clearly meaning to print `abs(camera->camDataIdx)` as a
        //! one-or-two-digit number, instead of `i`.
        // "sp50[i++] = ..." matches here, but is undefined behavior due to conflicting
        // reads/writes between sequence points, triggering warnings. Work around by
        // putting i++ afterwards while on the same line.
        // clang-format off
        if (camera->camDataIdx / 10 != 0) {
            sp50[i] = i / 10 + '0'; i++;
        }
        sp50[i] = i % 10 + '0'; i++;
        // clang-format on

        sp50[i++] = ' ';
        sp50[i++] = ' ';
        sp50[i++] = ' ';
        sp50[i++] = ' ';
        sp50[i] = '\0';
        func_8006376C(3, 26, 5, "I:");
        func_8006376C(5, 26, 4, sp50);
    }
}

s32 Camera_UpdateWater(Camera* camera) {
    f32 waterY;
    s16 newQuakeId;
    s32 waterLightsIndex;
    s32* waterPrevCamSetting = &camera->waterPrevCamSetting;
    s16 waterCamIdx;
    s16* quakeId = (s16*)&camera->waterQuakeId;
    Player* player = camera->player;
    s16 prevBgId;

    if (!(camera->unk_14C & 2) || sCameraSettings[camera->setting].unk_00 & 0x40000000) {
        return 0;
    }

    if (camera->unk_14C & 0x200) {
        if (player->stateFlags2 & 0x800) {
            Camera_ChangeSettingFlags(camera, CAM_SET_PIVOT_WATER_SURFACE, 6);
            camera->unk_14C |= (s16)0x8000;
        } else if (camera->unk_14C & (s16)0x8000) {
            Camera_ChangeSettingFlags(camera, *waterPrevCamSetting, 6);
            camera->unk_14C &= ~((s16)0x8000);
        }
    }
    if (!(camera->unk_14C & (s16)0x8000)) {
        if (waterCamIdx = Camera_GetWaterBoxDataIdx(camera, &waterY), waterCamIdx == -2) {
            // No camera data idx
            if (!(camera->unk_14C & 0x200)) {
                camera->unk_14C |= 0x200;
                camera->waterYPos = waterY;
                camera->waterPrevCamIdx = camera->camDataIdx;
                *quakeId = -1;
            }
            if (camera->playerGroundY != camera->playerPosRot.pos.y) {
                prevBgId = camera->bgCheckId;
                camera->bgCheckId = BGCHECK_SCENE;
                Camera_ChangeSettingFlags(camera, CAM_SET_NORMAL3, 2);
                *waterPrevCamSetting = camera->setting;
                camera->bgCheckId = prevBgId;
                camera->camDataIdx = -2;
            }
        } else if (waterCamIdx != -1) {
            // player is in a water box
            if (!(camera->unk_14C & 0x200)) {
                camera->unk_14C |= 0x200;
                camera->waterYPos = waterY;
                camera->waterPrevCamIdx = camera->camDataIdx;
                *quakeId = -1;
            }
            if (camera->playerGroundY != camera->playerPosRot.pos.y) {
                prevBgId = camera->bgCheckId;
                camera->bgCheckId = BGCHECK_SCENE;
                Camera_ChangeDataIdx(camera, waterCamIdx);
                *waterPrevCamSetting = camera->setting;
                camera->bgCheckId = prevBgId;
            }
        } else if (camera->unk_14C & 0x200) {
            // player is out of a water box.
            osSyncPrintf("camera: water: off\n");
            camera->unk_14C &= ~0x200;
            prevBgId = camera->bgCheckId;
            camera->bgCheckId = BGCHECK_SCENE;
            if (camera->waterPrevCamIdx < 0) {
                func_80057FC4(camera);
                camera->camDataIdx = -1;
            } else {
                Camera_ChangeDataIdx(camera, camera->waterPrevCamIdx);
            }
            camera->bgCheckId = prevBgId;
        }
    }

    if (waterY = Camera_GetWaterSurface(camera, &camera->eye, &waterLightsIndex), waterY != BGCHECK_Y_MIN) {
        camera->waterYPos = waterY;
        if (!(camera->unk_14C & 0x100)) {
            camera->unk_14C |= 0x100;
            osSyncPrintf("kankyo changed water, sound on\n");
            Environment_EnableUnderwaterLights(camera->play, waterLightsIndex);
            camera->waterDistortionTimer = 80;
        }

        Audio_SetExtraFilter(0x20);

        if (PREG(81)) {
            Quake_RemoveFromIdx(*quakeId);
            *quakeId = -1;
            PREG(81) = 0;
        }

        if ((*quakeId == -1) || (Quake_GetCountdown(*quakeId) == 0xA)) {
            if (*quakeId = newQuakeId = Quake_Add(camera, 5U), newQuakeId != 0) {
                Quake_SetSpeed(*quakeId, 550);
                Quake_SetQuakeValues(*quakeId, 1, 1, 180, 0);
                Quake_SetCountdown(*quakeId, 1000);
            }
        }

        if (camera->waterDistortionTimer > 0) {
            camera->waterDistortionTimer--;
            camera->distortionFlags |= DISTORTION_UNDERWATER_STRONG;
        } else if (camera->play->sceneNum == SCENE_TURIBORI) {
            camera->distortionFlags |= DISTORTION_UNDERWATER_FISHING;
        } else {
            camera->distortionFlags |= DISTORTION_UNDERWATER_WEAK;
        }
    } else {
        if (camera->unk_14C & 0x100) {
            camera->unk_14C &= ~0x100;
            osSyncPrintf("kankyo changed water off, sound off\n");
            Environment_DisableUnderwaterLights(camera->play);
            if (*quakeId != 0) {
                Quake_RemoveFromIdx(*quakeId);
            }
            camera->waterDistortionTimer = 0;
            camera->distortionFlags = 0;
        }
        Audio_SetExtraFilter(0);
    }
    //! @bug: doesn't always return a value, but sometimes does.
}

s32 Camera_UpdateHotRoom(Camera* camera) {
    camera->distortionFlags &= ~DISTORTION_HOT_ROOM;
    if (camera->play->roomCtx.curRoom.behaviorType2 == ROOM_BEHAVIOR_TYPE2_3) {
        camera->distortionFlags |= DISTORTION_HOT_ROOM;
    }

    return 1;
}

s32 Camera_DbgChangeMode(Camera* camera) {
    s32 changeDir = 0;

    if (!gDbgCamEnabled && camera->play->activeCamera == MAIN_CAM) {
        if (CHECK_BTN_ALL(D_8015BD7C->state.input[2].press.button, BTN_CUP)) {
            osSyncPrintf("attention sound URGENCY\n");
            func_80078884(NA_SE_SY_ATTENTION_URGENCY);
        }
        if (CHECK_BTN_ALL(D_8015BD7C->state.input[2].press.button, BTN_CDOWN)) {
            osSyncPrintf("attention sound NORMAL\n");
            func_80078884(NA_SE_SY_ATTENTION_ON);
        }

        if (CHECK_BTN_ALL(D_8015BD7C->state.input[2].press.button, BTN_CRIGHT)) {
            changeDir = 1;
        }
        if (CHECK_BTN_ALL(D_8015BD7C->state.input[2].press.button, BTN_CLEFT)) {
            changeDir = -1;
        }
        if (changeDir != 0) {
            sDbgModeIdx = (sDbgModeIdx + changeDir) % 6;
            if (Camera_ChangeSetting(camera, D_8011DAFC[sDbgModeIdx]) > 0) {
                osSyncPrintf("camera: force change SET to %s!\n", sCameraSettingNames[D_8011DAFC[sDbgModeIdx]]);
            }
        }
    }
    return true;
}

s16 depthPhase = 0x3F0;
s16 screenPlanePhase = 0x156;
void Camera_UpdateDistortion(Camera* camera) {
    f32 scaleFactor;
    f32 speedFactor;
    f32 depthPhaseStep;
    f32 screenPlanePhaseStep;
    s32 pad[5];
    f32 xScale;
    f32 yScale;
    f32 zScale;
    f32 speed;

    if (camera->distortionFlags != 0) {
        if (camera->distortionFlags & DISTORTION_UNDERWATER_MEDIUM) {
            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 170.0f;

            xScale = -0.01f;
            yScale = 0.01f;
            zScale = 0.0f;

            speed = 0.6f;
            scaleFactor = camera->waterDistortionTimer / 60.0f;
            speedFactor = 1.0f;
        } else if (camera->distortionFlags & DISTORTION_UNDERWATER_STRONG) {
            depthPhaseStep = 248.0f;
            screenPlanePhaseStep = -90.0f;

            xScale = -0.3f;
            yScale = 0.3f;
            zScale = 0.2f;

            speed = 0.2f;
            scaleFactor = camera->waterDistortionTimer / 80.0f;
            speedFactor = 1.0f;
        } else if (camera->distortionFlags & DISTORTION_UNDERWATER_WEAK) {
            depthPhaseStep = 359.2f;
            screenPlanePhaseStep = -18.5f;

            xScale = 0.09f;
            yScale = 0.09f;
            zScale = 0.01f;

            speed = 0.08f;
            scaleFactor =
                (((camera->waterYPos - camera->eye.y) > 150.0f ? 1.0f : (camera->waterYPos - camera->eye.y) / 150.0f) *
                 0.45f) +
                (camera->speedRatio * 0.45f);
            speedFactor = scaleFactor;
        } else if (camera->distortionFlags & DISTORTION_HOT_ROOM) {
            // Gives the hot-room a small mirage-like appearance
            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 150.0f;

            xScale = -0.01f;
            yScale = 0.01f;
            zScale = 0.01f;

            speed = 0.6f;
            speedFactor = 1.0f;
            scaleFactor = 1.0f;
        } else {
            // DISTORTION_UNDERWATER_FISHING
            return;
        }

        depthPhase += DEGF_TO_BINANG(depthPhaseStep);
        screenPlanePhase += DEGF_TO_BINANG(screenPlanePhaseStep);

        View_SetDistortionOrientation(&camera->play->view, Math_CosS(depthPhase) * 0.0f, Math_SinS(depthPhase) * 0.0f,
                                 Math_SinS(screenPlanePhase) * 0.0f);
        View_SetDistortionScale(&camera->play->view, Math_SinS(screenPlanePhase) * (xScale * scaleFactor) + 1.0f,
                                Math_CosS(screenPlanePhase) * (yScale * scaleFactor) + 1.0f,
                                Math_CosS(depthPhase) * (zScale * scaleFactor) + 1.0f);
        View_SetDistortionSpeed(&camera->play->view, speed * speedFactor);

        camera->unk_14C |= 0x40;

    } else if (camera->unk_14C & 0x40) {
        View_ClearDistortion(&camera->play->view);
        camera->unk_14C &= ~0x40;
    }
}

s32 sOOBTimer = 0;
Vec3s Camera_Update(Camera* camera) {
    Vec3f viewAt;
    Vec3f viewEye;
    Vec3f viewUp;
    f32 viewFov;
    Vec3f spAC;
    s32 bgId;
    f32 playerGroundY;
    f32 playerXZSpeed;
    VecSph eyeAtAngle;
    s16 camDataIdx;
    PosRot curPlayerPosRot;
    QuakeCamCalc quake;
    Player* player;

    player = camera->play->cameraPtrs[MAIN_CAM]->player;

    if (R_DBG_CAM_UPDATE) {
        osSyncPrintf("camera: in %x\n", camera);
    }

    if (camera->status == CAM_STAT_CUT) {
        if (R_DBG_CAM_UPDATE) {
            osSyncPrintf("camera: cut out %x\n", camera);
        }
        return camera->inputDir;
    }

    sUpdateCameraDirection = false;

    if (camera->player != NULL) {
        Actor_GetWorldPosShapeRot(&curPlayerPosRot, &camera->player->actor);
        camera->xzSpeed = playerXZSpeed = OLib_Vec3fDistXZ(&curPlayerPosRot.pos, &camera->playerPosRot.pos);

        camera->speedRatio = OLib_ClampMaxDist(playerXZSpeed / (func_8002DCE4(camera->player) * PCT(OREG(8))), 1.0f);
        camera->playerPosDelta.x = curPlayerPosRot.pos.x - camera->playerPosRot.pos.x;
        camera->playerPosDelta.y = curPlayerPosRot.pos.y - camera->playerPosRot.pos.y;
        camera->playerPosDelta.z = curPlayerPosRot.pos.z - camera->playerPosRot.pos.z;
        spAC = curPlayerPosRot.pos;
        spAC.y += Player_GetHeight(camera->player);

        playerGroundY = BgCheck_EntityRaycastFloor5(camera->play, &camera->play->colCtx, &playerFloorPoly,
                                                    &bgId, &camera->player->actor, &spAC);
        if (playerGroundY != BGCHECK_Y_MIN) {
            // player is above ground.
            sOOBTimer = 0;
            camera->floorNorm.x = COLPOLY_GET_NORMAL(playerFloorPoly->normal.x);
            camera->floorNorm.y = COLPOLY_GET_NORMAL(playerFloorPoly->normal.y);
            camera->floorNorm.z = COLPOLY_GET_NORMAL(playerFloorPoly->normal.z);
            camera->bgCheckId = bgId;
            camera->playerGroundY = playerGroundY;
        } else {
            // player is not above ground.
            sOOBTimer++;
            camera->floorNorm.x = 0.0;
            camera->floorNorm.y = 1.0f;
            camera->floorNorm.z = 0.0;
        }

        camera->playerPosRot = curPlayerPosRot;

        if (sOOBTimer < 200) {
            if (camera->status == CAM_STAT_ACTIVE) {
                Camera_UpdateWater(camera);
                Camera_UpdateHotRoom(camera);
            }

            if (!(camera->unk_14C & 4)) {
                camera->nextCamDataIdx = -1;
            }

            if ((camera->unk_14C & 1) && (camera->unk_14C & 4) && (!(camera->unk_14C & 0x400)) &&
                (!(camera->unk_14C & 0x200) || (player->currentBoots == PLAYER_BOOTS_IRON)) &&
                (!(camera->unk_14C & (s16)0x8000)) && (playerGroundY != BGCHECK_Y_MIN)) {
                camDataIdx = Camera_GetDataIdxForPoly(camera, &bgId, playerFloorPoly);
                if (camDataIdx != -1) {
                    camera->nextBGCheckId = bgId;
                    if (bgId == BGCHECK_SCENE) {
                        camera->nextCamDataIdx = camDataIdx;
                    }
                }
            }

            if (camera->nextCamDataIdx != -1 && (fabsf(curPlayerPosRot.pos.y - playerGroundY) < 2.0f) &&
                (!(camera->unk_14C & 0x200) || (player->currentBoots == PLAYER_BOOTS_IRON))) {
                camera->bgCheckId = camera->nextBGCheckId;
                Camera_ChangeDataIdx(camera, camera->nextCamDataIdx);
                camera->nextCamDataIdx = -1;
            }
        }
    }
    Camera_PrintSettings(camera);
    Camera_DbgChangeMode(camera);

    if (camera->status == CAM_STAT_WAIT) {
        if (R_DBG_CAM_UPDATE) {
            osSyncPrintf("camera: wait out %x\n", camera);
        }
        return camera->inputDir;
    }

    camera->unk_14A = 0;
    camera->unk_14C &= ~(0x400 | 0x20);
    camera->unk_14C |= 0x10;

    if (R_DBG_CAM_UPDATE) {
        osSyncPrintf("camera: engine (%d %d %d) %04x \n", camera->setting, camera->mode,
                     sCameraSettings[camera->setting].cameraModes[camera->mode].funcIdx, camera->unk_14C);
    }

    if (sOOBTimer < 200) {
        sCameraFunctions[sCameraSettings[camera->setting].cameraModes[camera->mode].funcIdx](camera);
    } else if (camera->player != NULL) {
        OLib_Vec3fDiffToVecSphGeo(&eyeAtAngle, &camera->at, &camera->eye);
        Camera_CalcAtDefault(camera, &eyeAtAngle, 0.0f, 0);
    }

    if (camera->status == CAM_STAT_ACTIVE) {
        if ((gSaveContext.gameMode != 0) && (gSaveContext.gameMode != 3)) {
            sCameraInterfaceFlags = 0;
            Camera_UpdateInterface(sCameraInterfaceFlags);
        } else if ((D_8011D3F0 != 0) && (camera->thisIdx == MAIN_CAM)) {
            D_8011D3F0--;
            sCameraInterfaceFlags = 0x3200;
            Camera_UpdateInterface(sCameraInterfaceFlags);
        } else if (camera->play->transitionMode != 0) {
            sCameraInterfaceFlags = 0xF200;
            Camera_UpdateInterface(sCameraInterfaceFlags);
        } else if (camera->play->csCtx.state != CS_STATE_IDLE) {
            sCameraInterfaceFlags = 0x3200;
            Camera_UpdateInterface(sCameraInterfaceFlags);
        } else {
            Camera_UpdateInterface(sCameraInterfaceFlags);
        }
    }

    if (R_DBG_CAM_UPDATE) {
        osSyncPrintf("camera: shrink_and_bitem %x(%d)\n", sCameraInterfaceFlags, camera->play->transitionMode);
    }

    if (R_DBG_CAM_UPDATE) {
        osSyncPrintf("camera: engine (%s(%d) %s(%d) %s(%d)) ok!\n", &sCameraSettingNames[camera->setting],
                     camera->setting, &sCameraModeNames[camera->mode], camera->mode,
                     &sCameraFunctionNames[sCameraSettings[camera->setting].cameraModes[camera->mode].funcIdx],
                     sCameraSettings[camera->setting].cameraModes[camera->mode].funcIdx);
    }

    // enable/disable debug cam
    if (CVar_GetS32("gDebugCamera", 0) && CHECK_BTN_ALL(D_8015BD7C->state.input[2].press.button, BTN_START)) {
        gDbgCamEnabled ^= 1;
        if (gDbgCamEnabled) {
            DbgCamera_Enable(&D_8015BD80, camera);
        } else if (camera->play->csCtx.state != CS_STATE_IDLE) {
            func_80064534(camera->play, &camera->play->csCtx);
        }
    }

    // Debug cam update
    if (gDbgCamEnabled) {
        camera->play->view.fovy = D_8015BD80.fov;
        DbCamera_Update(&D_8015BD80, camera);
        func_800AA358(&camera->play->view, &D_8015BD80.eye, &D_8015BD80.at, &D_8015BD80.unk_1C);
        if (R_DBG_CAM_UPDATE) {
            osSyncPrintf("camera: debug out\n");
        }
        return D_8015BD80.sub.unk_104A;
    }

    OREG(0) &= ~8;

    if (camera->status == 3) {
        return camera->inputDir;
    }

    // setting bgCheckId to the ret of Quake_Calc, and checking that
    // is required, it doesn't make too much sense though.
    if ((bgId = Quake_Calc(camera, &quake), bgId != 0) && (camera->setting != CAM_SET_TURN_AROUND)) {
        viewAt.x = camera->at.x + quake.atOffset.x;
        viewAt.y = camera->at.y + quake.atOffset.y;
        viewAt.z = camera->at.z + quake.atOffset.z;
        viewEye.x = camera->eye.x + quake.eyeOffset.x;
        viewEye.y = camera->eye.y + quake.eyeOffset.y;
        viewEye.z = camera->eye.z + quake.eyeOffset.z;
        OLib_Vec3fDiffToVecSphGeo(&eyeAtAngle, &viewEye, &viewAt);
        Camera_CalcUpFromPitchYawRoll(&viewUp, eyeAtAngle.pitch + quake.rotZ, eyeAtAngle.yaw + quake.unk_1A,
                                      camera->roll);
        viewFov = camera->fov + BINANG_TO_DEGF(quake.zoom);
    } else {
        viewAt = camera->at;
        viewEye = camera->eye;
        OLib_Vec3fDiffToVecSphGeo(&eyeAtAngle, &viewEye, &viewAt);
        Camera_CalcUpFromPitchYawRoll(&viewUp, eyeAtAngle.pitch, eyeAtAngle.yaw, camera->roll);
        viewFov = camera->fov;
    }

    if (camera->paramFlags & 4) {
        camera->paramFlags &= ~4;
        viewUp = camera->up;
    } else {
        camera->up = viewUp;
    }

    camera->skyboxOffset = quake.eyeOffset;

    Camera_UpdateDistortion(camera);

    if ((camera->play->sceneNum == SCENE_SPOT00) && (camera->fov < 59.0f)) {
        View_SetScale(&camera->play->view, 0.79f);
    } else {
        View_SetScale(&camera->play->view, 1.0f);
    }
    camera->play->view.fovy = viewFov;
    func_800AA358(&camera->play->view, &viewEye, &viewAt, &viewUp);
    camera->camDir.x = eyeAtAngle.pitch;
    camera->camDir.y = eyeAtAngle.yaw;
    camera->camDir.z = 0;

    if (sUpdateCameraDirection == 0) {
        camera->inputDir.x = eyeAtAngle.pitch;
        camera->inputDir.y = eyeAtAngle.yaw;
        camera->inputDir.z = 0;
    }

    if (PREG(81)) {
        osSyncPrintf("dir  (%d) %d(%f) %d(%f) 0(0) \n", sUpdateCameraDirection, camera->inputDir.x,
                     BINANG_TO_DEGF(camera->inputDir.x), camera->inputDir.y, BINANG_TO_DEGF(camera->inputDir.y));
        osSyncPrintf("real (%d) %d(%f) %d(%f) 0(0) \n", sUpdateCameraDirection, camera->camDir.x,
                     BINANG_TO_DEGF(camera->camDir.x), camera->camDir.y, BINANG_TO_DEGF(camera->camDir.y));
    }

    if (camera->timer != -1 && CHECK_BTN_ALL(D_8015BD7C->state.input[0].press.button, BTN_DRIGHT) && CVar_GetS32("gDebugCamera", 0)) {
        camera->timer = 0;
    }

    if (R_DBG_CAM_UPDATE) {
        osSyncPrintf("camera: out (%f %f %f) (%f %f %f)\n", camera->at.x, camera->at.y, camera->at.z, camera->eye.x,
                     camera->eye.y, camera->eye.z);
        osSyncPrintf("camera: dir (%f %d(%f) %d(%f)) (%f)\n", eyeAtAngle.r, eyeAtAngle.pitch,
                     BINANG_TO_DEGF(eyeAtAngle.pitch), eyeAtAngle.yaw, BINANG_TO_DEGF(eyeAtAngle.yaw), camera->fov);
        if (camera->player != NULL) {
            osSyncPrintf("camera: foot(%f %f %f) dist (%f)\n", curPlayerPosRot.pos.x, curPlayerPosRot.pos.y,
                         curPlayerPosRot.pos.z, camera->dist);
        }
    }

    return camera->inputDir;
}

/**
 * When the camera's timer is 0, change the camera to its parent
 */
void Camera_Finish(Camera* camera) {
    Camera* mainCam = camera->play->cameraPtrs[MAIN_CAM];
    Player* player = GET_PLAYER(camera->play);

    if (camera->timer == 0) {
        Play_ChangeCameraStatus(camera->play, camera->parentCamIdx, CAM_STAT_ACTIVE);

        if ((camera->parentCamIdx == MAIN_CAM) && (camera->csId != 0)) {
            player->actor.freezeTimer = 0;
            player->stateFlags1 &= ~0x20000000;

            if (player->csMode != 0) {
                func_8002DF54(camera->play, &player->actor, 7);
                osSyncPrintf("camera: player demo end!!\n");
            }

            mainCam->unk_14C |= 8;
        }

        if (CHILD_CAM(camera)->parentCamIdx == camera->thisIdx) {
            CHILD_CAM(camera)->parentCamIdx = camera->parentCamIdx;
        }

        if (PARENT_CAM(camera)->childCamIdx == camera->thisIdx) {
            PARENT_CAM(camera)->childCamIdx = camera->childCamIdx;
        }

        if (PARENT_CAM(camera)->thisIdx == MAIN_CAM) {
            PARENT_CAM(camera)->animState = 0;
        }

        camera->childCamIdx = camera->parentCamIdx = SUBCAM_FREE;
        camera->timer = -1;
        camera->play->envCtx.fillScreen = false;

        Play_ClearCamera(camera->play, camera->thisIdx);
    }
}

s32 func_8005A02C(Camera* camera) {
    camera->unk_14C |= 0xC;
    camera->unk_14C &= ~(0x1000 | 0x8);
    return true;
}

s32 Camera_ChangeModeFlags(Camera* camera, s16 mode, u8 flags) {
    static s32 modeChangeFlags = 0;

    if (QREG(89)) {
        osSyncPrintf("+=+(%d)+=+ recive request -> %s\n", camera->play->state.frames, sCameraModeNames[mode]);
    }

    if (camera->unk_14C & 0x20 && flags == 0) {
        camera->unk_14A |= 0x20;
        return -1;
    }

    if (!((sCameraSettings[camera->setting].unk_00 & 0x3FFFFFFF) & (1 << mode))) {
        if (mode == CAM_MODE_FIRSTPERSON) {
            osSyncPrintf("camera: error sound\n");
            func_80078884(NA_SE_SY_ERROR);
        }

        if (camera->mode != CAM_MODE_NORMAL) {
            osSyncPrintf(VT_COL(YELLOW, BLACK) "camera: change camera mode: force NORMAL: %s %s refused\n" VT_RST,
                         sCameraSettingNames[camera->setting], sCameraModeNames[mode]);
            camera->mode = CAM_MODE_NORMAL;
            Camera_CopyDataToRegs(camera, camera->mode);
            func_8005A02C(camera);
            return 0xC0000000 | mode;
        } else {
            camera->unk_14A |= 0x20;
            camera->unk_14A |= 2;
            return 0;
        }
    } else {
        if (mode == camera->mode && flags == 0) {
            camera->unk_14A |= 0x20;
            camera->unk_14A |= 2;
            return -1;
        }
        camera->unk_14A |= 0x20;
        camera->unk_14A |= 2;
        Camera_CopyDataToRegs(camera, mode);
        modeChangeFlags = 0;
        switch (mode) {
            case CAM_MODE_FIRSTPERSON:
                modeChangeFlags = 0x20;
                break;
            case CAM_MODE_BATTLE:
                modeChangeFlags = 4;
                break;
            case CAM_MODE_FOLLOWTARGET:
                if (camera->target != NULL && camera->target->id != ACTOR_EN_BOOM) {
                    modeChangeFlags = 8;
                }
                break;
            case CAM_MODE_TARGET:
            case CAM_MODE_TALK:
            case CAM_MODE_BOWARROWZ:
            case CAM_MODE_HANGZ:
            case CAM_MODE_PUSHPULL:
                modeChangeFlags = 2;
                break;
        }

        switch (camera->mode) {
            case CAM_MODE_FIRSTPERSON:
                if (modeChangeFlags & 0x20) {
                    camera->animState = 0xA;
                }
                break;
            case CAM_MODE_TARGET:
                if (modeChangeFlags & 0x10) {
                    camera->animState = 0xA;
                }
                modeChangeFlags |= 1;
                break;
            case CAM_MODE_CHARGE:
                modeChangeFlags |= 1;
                break;
            case CAM_MODE_FOLLOWTARGET:
                if (modeChangeFlags & 8) {
                    camera->animState = 0xA;
                }
                modeChangeFlags |= 1;
                break;
            case CAM_MODE_BATTLE:
                if (modeChangeFlags & 4) {
                    camera->animState = 0xA;
                }
                modeChangeFlags |= 1;
                break;
            case CAM_MODE_BOWARROWZ:
            case CAM_MODE_HANGZ:
            case CAM_MODE_PUSHPULL:
                modeChangeFlags |= 1;
                break;
            case CAM_MODE_NORMAL:
                if (modeChangeFlags & 0x10) {
                    camera->animState = 0xA;
                }
                break;
        }
        modeChangeFlags &= ~0x10;
        if (camera->status == CAM_STAT_ACTIVE) {
            switch (modeChangeFlags) {
                case 1:
                    func_80078884(0);
                    break;
                case 2:
                    if (camera->play->roomCtx.curRoom.behaviorType1 == ROOM_BEHAVIOR_TYPE1_1) {
                        func_80078884(NA_SE_SY_ATTENTION_URGENCY);
                    } else {
                        func_80078884(NA_SE_SY_ATTENTION_ON);
                    }
                    break;
                case 4:
                    func_80078884(NA_SE_SY_ATTENTION_URGENCY);
                    break;
                case 8:
                    func_80078884(NA_SE_SY_ATTENTION_ON);
                    break;
            }
        }
        func_8005A02C(camera);
        camera->mode = mode;
        return 0x80000000 | mode;
    }
}

s32 Camera_ChangeMode(Camera* camera, s16 mode) {
    return Camera_ChangeModeFlags(camera, mode, 0);
}

s32 Camera_CheckValidMode(Camera* camera, s16 mode) {
    if (QREG(89) != 0) {
        osSyncPrintf("+=+=+=+ recive asking -> %s (%s)\n", sCameraModeNames[mode],
                     sCameraSettingNames[camera->setting]);
    }
    if (!(sCameraSettings[camera->setting].validModes & (1 << mode))) {
        return 0;
    } else if (mode == camera->mode) {
        return -1;
    } else {
        return mode | 0x80000000;
    }
}

s16 Camera_ChangeSettingFlags(Camera* camera, s16 setting, s16 flags) {
    if (camera->unk_14A & 1) {
        if (((u32)(sCameraSettings[camera->setting].unk_00 & 0xF000000) >> 0x18) >=
            ((u32)(sCameraSettings[setting].unk_00 & 0xF000000) >> 0x18)) {
            camera->unk_14A |= 0x10;
            return -2;
        }
    }
    if (((setting == CAM_SET_MEADOW_BIRDS_EYE) || (setting == CAM_SET_MEADOW_UNUSED)) && LINK_IS_ADULT &&
        (camera->play->sceneNum == SCENE_SPOT05)) {
        camera->unk_14A |= 0x10;
        return -5;
    }

    if (setting == CAM_SET_NONE || setting >= CAM_SET_MAX) {
        osSyncPrintf(VT_COL(RED, WHITE) "camera: error: illegal camera set (%d) !!!!\n" VT_RST, setting);
        return -99;
    }

    if ((setting == camera->setting) && (!(flags & 1))) {
        camera->unk_14A |= 0x10;
        if (!(flags & 2)) {
            camera->unk_14A |= 1;
        }
        return -1;
    }

    camera->unk_14A |= 0x10;
    if (!(flags & 2)) {
        camera->unk_14A |= 1;
    }

    camera->unk_14C |= 0xC;
    camera->unk_14C &= ~0x1008;

    if (!(sCameraSettings[camera->setting].unk_00 & 0x40000000)) {
        camera->prevSetting = camera->setting;
    }

    if (flags & 8) {
        camera->camDataIdx = camera->prevCamDataIdx;
        camera->prevCamDataIdx = -1;
    } else if (!(flags & 4)) {
        if (!(sCameraSettings[camera->setting].unk_00 & 0x40000000)) {
            camera->prevCamDataIdx = camera->camDataIdx;
        }
        camera->camDataIdx = -1;
    }

    camera->setting = setting;

    if (Camera_ChangeModeFlags(camera, camera->mode, 1) >= 0) {
        Camera_CopyDataToRegs(camera, camera->mode);
    }

    osSyncPrintf(VT_SGR("1") "%06u:" VT_RST " camera: change camera[%d] set %s\n", camera->play->state.frames,
                 camera->thisIdx, sCameraSettingNames[camera->setting]);

    return setting;
}

s32 Camera_ChangeSetting(Camera* camera, s16 setting) {
    return Camera_ChangeSettingFlags(camera, setting, 0);
}

s32 Camera_ChangeDataIdx(Camera* camera, s32 camDataIdx) {
    s16 newCameraSetting;
    s16 settingChangeSuccessful;

    if (camDataIdx == -1 || camDataIdx == camera->camDataIdx) {
        camera->unk_14A |= 0x40;
        return -1;
    }

    if (!(camera->unk_14A & 0x40)) {
        newCameraSetting = Camera_GetCamDataSetting(camera, camDataIdx);
        camera->unk_14A |= 0x40;
        settingChangeSuccessful = Camera_ChangeSettingFlags(camera, newCameraSetting, 5) >= 0;
        if (settingChangeSuccessful || sCameraSettings[camera->setting].unk_00 & 0x80000000) {
            camera->camDataIdx = camDataIdx;
            camera->unk_14A |= 4;
            Camera_CopyDataToRegs(camera, camera->mode);
        } else if (settingChangeSuccessful < -1) {
            //! @bug: This is likely checking the wrong value. The actual return of Camera_ChangeSettingFlags or
            // camDataIdx would make more sense.
            osSyncPrintf(VT_COL(RED, WHITE) "camera: error: illegal camera ID (%d) !! (%d|%d|%d)\n" VT_RST, camDataIdx,
                         camera->thisIdx, 0x32, newCameraSetting);
        }
        return 0x80000000 | camDataIdx;
    }
}

Vec3s* Camera_GetInputDir(Vec3s* dst, Camera* camera) {
    if (gDbgCamEnabled) {
        *dst = D_8015BD80.sub.unk_104A;
        return dst;
    } else {
        *dst = camera->inputDir;
        return dst;
    }
}

s16 Camera_GetInputDirPitch(Camera* camera) {
    Vec3s dir;

    Camera_GetInputDir(&dir, camera);
    return dir.x;
}

s16 Camera_GetInputDirYaw(Camera* camera) {
    Vec3s dir;

    Camera_GetInputDir(&dir, camera);
    return dir.y;
}

Vec3s* Camera_GetCamDir(Vec3s* dst, Camera* camera) {
    if (gDbgCamEnabled) {
        *dst = D_8015BD80.sub.unk_104A;
        return dst;
    } else {
        *dst = camera->camDir;
        return dst;
    }
}

s16 Camera_GetCamDirPitch(Camera* camera) {
    Vec3s camDir;

    Camera_GetCamDir(&camDir, camera);
    return camDir.x;
}

s16 Camera_GetCamDirYaw(Camera* camera) {
    Vec3s camDir;

    Camera_GetCamDir(&camDir, camera);
    return camDir.y;
}

s32 Camera_AddQuake(Camera* camera, s32 arg1, s16 y, s32 countdown) {
    s16 quakeIdx;

    quakeIdx = Quake_Add(camera, 3);
    if (quakeIdx == 0) {
        return 0;
    }
    Quake_SetSpeed(quakeIdx, 0x61A8);
    Quake_SetQuakeValues(quakeIdx, y, 0, 0, 0);
    Quake_SetCountdown(quakeIdx, countdown);
    return 1;
}

s32 Camera_SetParam(Camera* camera, s32 param, void* value) {
    s32 pad[3];

    if (value != NULL) {
        switch (param) {
            case 1:
                camera->paramFlags &= ~(0x10 | 0x8 | 0x1);
                camera->at = *(Vec3f*)value;
                break;
            case 16:
                camera->paramFlags &= ~(0x10 | 0x8 | 0x1);
                camera->targetPosRot.pos = *(Vec3f*)value;
                break;
            case 8:
                if (camera->setting == CAM_SET_CS_C || camera->setting == CAM_SET_CS_ATTENTION) {
                    break;
                }
                camera->target = (Actor*)value;
                camera->paramFlags &= ~(0x10 | 0x8 | 0x1);
                break;
            case 2:
                camera->eye = camera->eyeNext = *(Vec3f*)value;
                break;
            case 4:
                camera->up = *(Vec3f*)value;
                break;
            case 0x40:
                camera->roll = DEGF_TO_BINANG(*(f32*)value);
                break;
            case 0x20:
                camera->fov = *(f32*)value;
                break;
            default:
                return false;
        }
        camera->paramFlags |= param;
    } else {
        return false;
    }
    return true;
}

s32 Camera_UnsetParam(Camera* camera, s16 param) {
    camera->paramFlags &= ~param;
    return true;
}

s32 func_8005AC48(Camera* camera, s16 arg1) {
    camera->unk_14C = arg1;
    return true;
}

s32 Camera_ResetAnim(Camera* camera) {
    camera->animState = 0;
    return 1;
}

s32 Camera_SetCSParams(Camera* camera, CutsceneCameraPoint* atPoints, CutsceneCameraPoint* eyePoints, Player* player,
                       s16 relativeToPlayer) {
    PosRot playerPosRot;

    camera->data0 = atPoints;
    camera->data1 = eyePoints;
    camera->data2 = relativeToPlayer;

    if (camera->data2 != 0) {
        camera->player = player;
        Actor_GetWorldPosShapeRot(&playerPosRot, &player->actor);
        camera->playerPosRot = playerPosRot;

        camera->nextCamDataIdx = -1;
        camera->xzSpeed = 0.0f;
        camera->speedRatio = 0.0f;
    }

    FrameInterpolation_DontInterpolateCamera();

    return 1;
}

s16 func_8005ACFC(Camera* camera, s16 arg1) {
    camera->unk_14C |= arg1;
    return camera->unk_14C;
}

s16 func_8005AD1C(Camera* camera, s16 arg1) {
    camera->unk_14C &= ~arg1;
    return camera->unk_14C;
}

s32 Camera_ChangeDoorCam(Camera* camera, Actor* doorActor, s16 camDataIdx, f32 arg3, s16 timer1, s16 timer2,
                         s16 timer3) {
    DoorParams* doorParams = (DoorParams*)camera->paramData;

    if ((camera->setting == CAM_SET_CS_ATTENTION) || (camera->setting == CAM_SET_DOORC)) {
        return 0;
    }

    doorParams->doorActor = doorActor;
    doorParams->timer1 = timer1;
    doorParams->timer2 = timer2;
    doorParams->timer3 = timer3;
    doorParams->camDataIdx = camDataIdx;

    if (camDataIdx == -99) {
        Camera_CopyDataToRegs(camera, camera->mode);
        return -99;
    }

    if (camDataIdx == -1) {
        Camera_ChangeSetting(camera, CAM_SET_DOORC);
        osSyncPrintf(".... change default door camera (set %d)\n", CAM_SET_DOORC);
    } else {
        s32 setting = Camera_GetCamDataSetting(camera, camDataIdx);

        camera->unk_14A |= 0x40;

        if (Camera_ChangeSetting(camera, setting) >= 0) {
            camera->camDataIdx = camDataIdx;
            camera->unk_14A |= 4;
        }

        osSyncPrintf("....change door camera ID %d (set %d)\n", camera->camDataIdx, camera->setting);
    }

    Camera_CopyDataToRegs(camera, camera->mode);
    return -1;
}

s32 Camera_Copy(Camera* dstCamera, Camera* srcCamera) {
    s32 pad;

    dstCamera->posOffset.x = 0.0f;
    dstCamera->posOffset.y = 0.0f;
    dstCamera->posOffset.z = 0.0f;
    dstCamera->atLERPStepScale = 0.1f;
    dstCamera->at = srcCamera->at;

    dstCamera->eye = dstCamera->eyeNext = srcCamera->eye;

    dstCamera->dist = OLib_Vec3fDist(&dstCamera->at, &dstCamera->eye);
    dstCamera->fov = srcCamera->fov;
    dstCamera->roll = srcCamera->roll;
    func_80043B60(dstCamera);

    if (dstCamera->player != NULL) {
        Actor_GetWorld(&dstCamera->playerPosRot, &dstCamera->player->actor);
        dstCamera->posOffset.x = dstCamera->at.x - dstCamera->playerPosRot.pos.x;
        dstCamera->posOffset.y = dstCamera->at.y - dstCamera->playerPosRot.pos.y;
        dstCamera->posOffset.z = dstCamera->at.z - dstCamera->playerPosRot.pos.z;
        dstCamera->dist = OLib_Vec3fDist(&dstCamera->playerPosRot.pos, &dstCamera->eye);
        dstCamera->xzOffsetUpdateRate = 1.0f;
        dstCamera->yOffsetUpdateRate = 1.0f;
    }
    return true;
}

s32 Camera_GetDbgCamEnabled() {
    return gDbgCamEnabled;
}

Vec3f* Camera_GetSkyboxOffset(Vec3f* dst, Camera* camera) {
    *dst = camera->skyboxOffset;
    return dst;
}

void Camera_SetCameraData(Camera* camera, s16 setDataFlags, void* data0, void* data1, s16 data2, s16 data3,
                          UNK_TYPE arg6) {
    if (setDataFlags & 0x1) {
        camera->data0 = data0;
    }

    if (setDataFlags & 0x2) {
        camera->data1 = data1;
    }

    if (setDataFlags & 0x4) {
        camera->data2 = data2;
    }

    if (setDataFlags & 0x8) {
        camera->data3 = data3;
    }

    if (setDataFlags & 0x10) {
        osSyncPrintf(VT_COL(RED, WHITE) "camera: setCameraData: last argument not alive!\n" VT_RST);
    }
}

s32 Camera_QRegInit() {
    if (!R_RELOAD_CAM_PARAMS) {
        QREG(2) = 1;
        QREG(10) = -1;
        QREG(11) = 100;
        QREG(12) = 80;
        QREG(20) = 90;
        QREG(21) = 10;
        QREG(22) = 10;
        QREG(23) = 50;
        QREG(24) = 6000;
        QREG(25) = 240;
        QREG(26) = 40;
        QREG(27) = 85;
        QREG(28) = 55;
        QREG(29) = 87;
        QREG(30) = 23;
        QREG(31) = 20;
        QREG(32) = 4;
        QREG(33) = 5;
        QREG(50) = 1;
        QREG(51) = 20;
        QREG(52) = 200;
        QREG(53) = 1;
        QREG(54) = 15;
        QREG(55) = 60;
        QREG(56) = 15;
        QREG(57) = 30;
        QREG(58) = 0;
    }

    QREG(65) = 50;
    return true;
}

s32 func_8005B198() {
    return D_8011D3AC;
}

s16 func_8005B1A4(Camera* camera) {
    camera->unk_14C |= 0x8;

    if ((camera->thisIdx == MAIN_CAM) && (camera->play->activeCamera != MAIN_CAM)) {
        GET_ACTIVE_CAM(camera->play)->unk_14C |= 0x8;
        return camera->play->activeCamera;
    }

    return camera->thisIdx;
}

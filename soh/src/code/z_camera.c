#include <libultraship/libultra.h>
#include "global.h"
#include "vt.h"

#include <string.h>

#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

#include "soh/frame_interpolation.h"

#include "vr/vr_manager.h"

// Global VR manager instance
VRManager* gVRManager = NULL;

// Initialize VR if available
void Camera_InitVR() {
    if (VR_IsHmdPresent()) {
        gVRManager = (VRManager*)malloc(sizeof(VRManager));
        if (gVRManager != NULL) {
            if (!VRManager_InitVR(gVRManager)) {
                free(gVRManager);
                gVRManager = NULL;
                osSyncPrintf("VR initialization failed\n");
            }
        }
    }
}

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

    if (!(camera->player->stateFlags1 & PLAYER_STATE1_IN_WATER)) {
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
 * 0 if no collsion is found between at->eye
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
        camera->player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) {
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
                phi_f16 = 1.0f;
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
        camera->player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) {
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
    return Camera_LERPCeilF(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv,
                            CVarGetInteger(CVAR_ENHANCEMENT("FixCameraDrift"), 0) ? 0.0f : 0.2f);
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
    return Camera_LERPCeilF(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv,
                            CVarGetInteger(CVAR_ENHANCEMENT("FixCameraDrift"), 0) ? 0.0f : 0.2f);
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

//Follow player with collision
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

    camera->animState = 0;

    f32 newCamX = -D_8015BD7C->state.input[0].cur.right_stick_x * 10.0f * (CVarGetFloat(CVAR_SETTING("FreeLook.CameraSensitivity.X"), 1.0f));
    f32 newCamY = D_8015BD7C->state.input[0].cur.right_stick_y * 10.0f * (CVarGetFloat(CVAR_SETTING("FreeLook.CameraSensitivity.Y"), 1.0f));
    bool invertXAxis = (CVarGetInteger(CVAR_SETTING("FreeLook.InvertXAxis"), 0) && !CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0)) || (!CVarGetInteger(CVAR_SETTING("FreeLook.InvertXAxis"), 0) && CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0));

    camera->play->camX += newCamX * (invertXAxis ? -1 : 1);
    camera->play->camY += newCamY * (CVarGetInteger(CVAR_SETTING("FreeLook.InvertYAxis"), 1) ? 1 : -1);

    if (camera->play->camY > 0x32A4) {
        camera->play->camY = 0x32A4;
    }
    if (camera->play->camY < -0x228C) {
        camera->play->camY = -0x228C;
    }

    f32 distTarget = CVarGetInteger(CVAR_SETTING("FreeLook.MaxCameraDistance"), para1->distTarget);
    f32 speedScaler = CVarGetInteger(CVAR_SETTING("FreeLook.TransitionSpeed"), 25);
    f32 distDiff = ABS(distTarget - camera->dist);
    if (distDiff > 0)
        camera->dist = Camera_LERPCeilF(distTarget, camera->dist, speedScaler / (distDiff + speedScaler), 0.0f);
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
    if (CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0) && SetCameraManual(camera) == 1) {
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
        // idle camera re-center
        if (CVarGetInteger(CVAR_SETTING("A11yDisableIdleCam"), 0)) {
            return 1;
        }
        eyeAdjustment.pitch = atEyeNextGeo.pitch;
        eyeAdjustment.yaw =
            Camera_LERPCeilS(anim->swingYawTarget, atEyeNextGeo.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else if (anim->swing.unk_18 != 0) {
        // camera adjustments when obstructed/pushed by scene geometry
        eyeAdjustment.yaw =
            Camera_LERPCeilS(anim->swing.unk_16, atEyeNextGeo.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
        eyeAdjustment.pitch =
            Camera_LERPCeilS(anim->swing.unk_14, atEyeNextGeo.pitch, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else {
        // rotate yaw to follow player while moving around - to keep player on camera.
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
        if (!CVarGetInteger(CVAR_ENHANCEMENT("FixCameraSwing"), 0)) {
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
        } else {
            if (anim->startSwingTimer <= 0) {
                anim->swing.swingUpdateRate = camera->yawUpdateRateInv = norm1->unk_0C * 2.0f;
                anim->swing.unk_18 = 0;
            }
            func_80046E20(camera, &eyeAdjustment, norm1->distMin, norm1->unk_0C, &sp98, &anim->swing);
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
        if(!CVarGetInteger(CVAR_ENHANCEMENT("DisableCritWiggle"),0)) {
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
    if (CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0) && SetCameraManual(camera) == 1) {
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
    if (CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0) && SetCameraManual(camera) == 1) {
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
        camera->player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) {
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
    camera->roll = Camera_LERPCeilS(0, camera->roll, 0.5f, 0xA);
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
    if (CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0) && SetCameraManual(camera) == 1) {
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
    if (CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0) && SetCameraManual(camera) == 1) {
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
    if (CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0) && SetCameraManual(camera) == 1) {
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

    if (camera->player->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) {
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
        camera->player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) {
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
    camera->fov = Camera_LERPCeilF((player->meleeWeaponState != 0       ? 0.8f
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
    if (CVarGetInteger(CVAR_SETTING("FreeLook.Enabled"), 0) && SetCameraManual(camera) == 1) {
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
                camera->player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) {
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

s32 Camera_VR(Camera* camera) {
    if (gVRManager == NULL) {
        return Camera_Normal1(camera);
    }

    // Update VR tracking
    VRManager_UpdateHMDMatrixPose(gVRManager);

    // Get HMD rotation and apply to camera
    Vec3f hmdRot = VRManager_GetHMDRotation(gVRManager);
    camera->eye.x = hmdRot.x;
    camera->eye.y = hmdRot.y;
    camera->eye.z = hmdRot.z;

    // Handle controller input if needed
    if (VRManager_IsControllerActive(gVRManager, ETrackedControllerRole_RightHand)) {
        Vec3f controllerDir = VRManager_GetControllerDirection(gVRManager, ETrackedControllerRole_RightHand);
        // Use controller direction for additional camera control if needed
    }

    return 1;
}

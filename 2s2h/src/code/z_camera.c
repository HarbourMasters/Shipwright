/**
 * @file camera.c
 * Implements the camera system. Camera functionality depends on the current combination of
 * setting and mode.
 *
 * When working with the camera, you should be familiar with MM's coordinate system.
 * Relative to the camera, the coordinate system follows the right hand rule:
 *          +X points right.
 *          +Y points up.
 *          +Z points out of the screen.
 *
 * You should also be familiar with Euler angles: 'pitch', 'yaw', and 'roll'.
 *      pitch: rotation about the X-axis, measured from +Y.
 *          Unlike yaw and roll, pitch is bounded in +-0x4000 (90 degrees).
 *          Pitch is 0 when the camera points parallel to the xz-plane (+Y points straight up).
 *
 *      yaw: rotation about the Y-axis, measured from (absolute) +Z.
 *          Positive yaw rotates clockwise, towards +X.
 *
 *      roll: rotation about the Z-axis, measured from the camera's right direction.
 *          Unfortunately, it's weird: For some reason, roll is flipped. Positive roll makes the camera
 *          rotate counterclockwise, which means the WORLD rotates clockwise. Luckily roll is rarely
 *          used.
 *
 *      Remember the right hand rule: make a thumbs-up with your right hand, stick your thumb in the
 *      +direction (except for roll), and the angle follows the rotation of your curled fingers.
 *
 * Illustrations:
 * Following the right hand rule, each hidden axis's positive direction points out of the screen.
 *
 *       YZ-Plane (pitch)        XZ-Plane (yaw)          XY-Plane (roll -- Note flipped)
 *          +Y                      -Z                      +Y
 *           ^                       ^ (into the             ^
 *         --|--                     |   screen)             |<-
 * +pitch /  |  \ -pitch             |                       |  \ -roll
 *       v   |   v                   |                       |   |
 * +Z <------O------> -Z   -X <------O------> +X   -X <------O------> +X
 *           |                   ^   |   ^                   |   |
 *           |                    \  |  /                    |  / +roll
 *           |               -yaw  --|--  +yaw               |<-
 *           v                       v                       v
 *          -Y                      +Z                      -Y
 *
 */

#include "global.h"
#include "libc/string.h"
#include "z64malloc.h"
#include "z64quake.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

#include "2s2h/BenPort.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>

void func_800DDFE0(Camera* camera);
s32 Camera_ChangeMode(Camera* camera, s16 mode);
s16 Camera_ChangeSettingFlags(Camera* camera, s16 setting, s16 flags);
s16 Camera_UnsetStateFlag(Camera* camera, s16 flags);

#include "z_camera_data.inc"

PlayState* sCamPlayState;
SwingAnimation D_801EDC30[4];
Vec3f D_801EDDD0;
Vec3f D_801EDDE0;
Vec3f D_801EDDF0;

// Camera will reload its paramData. Usually that means setting the read-only data from what is stored in
// CameraModeValue arrays. Although sometimes some read-write data is reset as well
#define RELOAD_PARAMS(camera) ((camera->animState == 0) || (camera->animState == 10) || (camera->animState == 20))

/**
 * Camera data is stored in both read-only data and OREG as s16, and then converted to the appropriate type during
 * runtime. If a small f32 is being stored as an s16, it is common to store that value 100 times larger than the
 * original value. This is then scaled back down during runtime with the CAM_RODATA_UNSCALE macro.
 */
#define CAM_RODATA_SCALE(x) ((x)*100.0f)
#define CAM_RODATA_UNSCALE(x) ((x)*0.01f)

// Load the next value from camera read-only data stored in CameraModeValue
#define GET_NEXT_RO_DATA(values) ((values++)->val)
// Load the next value and scale down from camera read-only data stored in CameraModeValue
#define GET_NEXT_SCALED_RO_DATA(values) CAM_RODATA_UNSCALE(GET_NEXT_RO_DATA(values))

// Camera bg surface flags
#define FLG_ADJSLOPE (1 << 0)
#define FLG_OFFGROUND (1 << 7)

#define CAM_CHANGE_SETTING_0 (1 << 0)
#define CAM_CHANGE_SETTING_1 (1 << 1)
#define CAM_CHANGE_SETTING_2 (1 << 2)
#define CAM_CHANGE_SETTING_3 (1 << 3)

/*===============================================================*/
/*                   Camera Internal Functions                   */
/*===============================================================*/

/**
 * Returns the absolute value for floats
 */
f32 Camera_fabsf(f32 f) {
    return ABS(f);
}

/**
 * Returns the magnitude for 3D float vectors
 */
f32 Camera_Vec3fMagnitude(Vec3f* vec) {
    return sqrtf(SQ(vec->x) + SQ(vec->y) + SQ(vec->z));
}

/**
 * Interpolates along the curve shown below
 * returns value y ranging from 0.0f to 1.0f for -xMax <= x <= xMax
 * returns 1.0f otherwise
 *
 * y = 1.0f    ________                   _________
 *                     __               __
 *                       _             _
 * y axis                 _           _
 *                         ___     ___
 *                            _____
 * y = 0.0f           |         |         |
 *                 -xMax        0       xMax
 *
 *                           x axis
 */
f32 Camera_QuadraticAttenuation(f32 xRange, f32 x) {
    f32 y;
    f32 absX;
    f32 percent40 = 0.4f;
    f32 percent60;
    f32 xQuadratic;
    f32 xMaxQuadratic; // Normalizing constant

    absX = Camera_fabsf(x);

    if (absX > xRange) {
        // fixed value outside xMax range
        y = 1.0f;
    } else {
        // inside xMax range
        percent60 = 1.0f - percent40;

        if (absX < (xRange * percent60)) {
            // quadratic curve in the inner 60% of xMax range: +concavity (upward curve)
            xQuadratic = SQ(x) * (1.0f - percent40);
            xMaxQuadratic = SQ(xRange * percent60);

            y = xQuadratic / xMaxQuadratic;
        } else {
            // quadratic curve in the outer 40% of xMax range: -concavity (flattening curve)
            xQuadratic = SQ(xRange - absX) * percent40;
            xMaxQuadratic = SQ(0.4f * xRange);

            y = 1.0f - (xQuadratic / xMaxQuadratic);
        }
    }
    return y;
}

/**
 * @param[in] target target value
 * @param[in] cur current value
 * @param[in] stepScale fraction of (target - cur) to step towards target
 * @param[in] minDiff minimum value of (target - cur) perform a step. Otherwise, return `target`
 *
 * @return new current value
 */
f32 Camera_ScaledStepToCeilF(f32 target, f32 cur, f32 stepScale, f32 minDiff) {
    f32 diff = target - cur;
    f32 step = diff * stepScale;

    return (Camera_fabsf(diff) >= minDiff) ? cur + step : target;
}

/**
 * @param[in] target target value
 * @param[in] cur current value
 * @param[in] stepScale fraction of (target - cur) to step towards target
 * @param[in] minDiff minimum value of (target - cur) perform a step. Otherwise, return `target`
 *
 * @return new current value
 */
s16 Camera_ScaledStepToCeilS(s16 target, s16 cur, f32 stepScale, s16 minDiff) {
    s16 diff = target - cur;
    s16 step = diff * stepScale + 0.5f;

    return (ABS(diff) >= minDiff) ? cur + step : target;
}

/**
 * @param[in] target target value
 * @param[in] cur current value
 * @param[in] stepScale fraction of (target - cur) to step towards target
 * @param[in] minDiff minimum value of (target - cur) perform a step. Otherwise, return `current`
 *
 * @return new current value
 */
s16 Camera_ScaledStepToFloorS(s16 target, s16 cur, f32 stepScale, s16 minDiff) {
    s16 diff = target - cur;
    s16 step = diff * stepScale + 0.5f;

    return (ABS(diff) >= minDiff) ? cur + step : cur;
}

/**
 * @param[in] target target value
 * @param[in] cur current value
 * @param[in] stepScale fraction of (target - cur) to step towards target
 * @param[in] minDiff minimum value of (target - cur) perform a step. Otherwise, return `target`
 *
 * @return new current value
 */
void Camera_ScaledStepToCeilVec3f(Vec3f* target, Vec3f* cur, f32 xzStepScale, f32 yStepScale, f32 minDiff) {
    cur->x = Camera_ScaledStepToCeilF(target->x, cur->x, xzStepScale, minDiff);
    cur->y = Camera_ScaledStepToCeilF(target->y, cur->y, yStepScale, minDiff);
    cur->z = Camera_ScaledStepToCeilF(target->z, cur->z, xzStepScale, minDiff);
}

void Camera_SetUpdateRatesFastPitch(Camera* camera) {
    camera->yawUpdateRateInv = 100.0f;
    camera->pitchUpdateRateInv = 16.0f;
    camera->rUpdateRateInv = 20.0f;
    camera->yOffsetUpdateRate = 0.05f;
    camera->xzOffsetUpdateRate = 0.05f;
    camera->fovUpdateRate = 0.05f;
}

void Camera_SetUpdateRatesFastYaw(Camera* camera) {
    camera->rUpdateRateInv = 50.0f;
    camera->pitchUpdateRateInv = 100.0f;
    camera->yawUpdateRateInv = 50.0f;
    camera->yOffsetUpdateRate = 0.01f;
    camera->xzOffsetUpdateRate = 0.1f;
    camera->fovUpdateRate = 0.01f;
    if (camera->speedRatio > 1.0f) {
        camera->speedRatio = 1.0f;
    }
}

void Camera_SetUpdateRatesSlow(Camera* camera) {
    camera->rUpdateRateInv = 1800.0f;
    camera->yawUpdateRateInv = 1800.0f;
    camera->pitchUpdateRateInv = 1800.0f;
    camera->yOffsetUpdateRate = 0.01f;
    camera->xzOffsetUpdateRate = 0.01f;
    camera->fovUpdateRate = 0.01f;
}

Vec3f Camera_Vec3sToVec3f(Vec3s* src) {
    Vec3f dest;

    dest.x = src->x;
    dest.y = src->y;
    dest.z = src->z;

    return dest;
}

/**
 * Returns the difference between two angles and scales the difference up
 */
s16 Camera_AngleDiffAndScale(s16 angle1, s16 angle2, f32 scale) {
    return BINANG_SUB(angle1, angle2) * scale;
}

/**
 * Calculates the current offset between the camera's at-coordinates and the focal actors coordinates
 */
void Camera_SetFocalActorAtOffset(Camera* camera, Vec3f* focalActorPos) {
    camera->focalActorAtOffset.x = camera->at.x - focalActorPos->x;
    camera->focalActorAtOffset.y = camera->at.y - focalActorPos->y;
    camera->focalActorAtOffset.z = camera->at.z - focalActorPos->z;
}

f32 Camera_GetFocalActorHeight(Camera* camera) {
    PosRot focalActorFocus;
    Actor* focalActor = camera->focalActor;
    f32 focalActorHeight;

    if (focalActor == &GET_PLAYER(camera->play)->actor) {
        focalActorHeight = Player_GetHeight((Player*)focalActor);
    } else {
        focalActorFocus = Actor_GetFocus(focalActor);
        focalActorHeight = focalActorFocus.pos.y - camera->focalActorPosRot.pos.y;
        if (focalActorHeight == 0.0f) {
            focalActorHeight = 10.0f;
        }
    }
    return focalActorHeight;
}

f32 Camera_GetRunSpeedLimit(Camera* camera) {
    Actor* focalActor = camera->focalActor;
    f32 runSpeedLimit;

    if (focalActor == &GET_PLAYER(camera->play)->actor) {
        runSpeedLimit = Player_GetRunSpeedLimit((Player*)focalActor);
    } else {
        runSpeedLimit = 10.0f;
    }

    return runSpeedLimit;
}

s32 func_800CB7CC(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags3 & PLAYER_STATE3_10;
    } else {
        return 0;
    }
}

s32 Camera_IsMountedOnHorse(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags1 & PLAYER_STATE1_800000;
    } else {
        return 0;
    }
}

s32 Camera_IsDekuHovering(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags3 & PLAYER_STATE3_2000;
    } else {
        return 0;
    }
}

/**
 * When walking in a cutscene? Used during Postman's minigame.
 */
s32 func_800CB854(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags1 & PLAYER_STATE1_20;
    } else {
        return 0;
    }
}

s32 Camera_IsSwimming(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (focalActor == &GET_PLAYER(camera->play)->actor) {
        if (((Player*)focalActor)->stateFlags3 & PLAYER_STATE3_8000) {
            // Swimming as Zora
            return 999;
        } else {
            // Swimming as Human or Fierce Deity
            return ((Player*)focalActor)->stateFlags1 & PLAYER_STATE1_8000000;
        }
    } else {
        // Camera not focused on player
        return 0;
    }
}

s32 Camera_IsDiving(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags2 & PLAYER_STATE2_800;
    } else {
        return 0;
    }
}

s32 Camera_IsPlayerFormZora(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->transformation == PLAYER_FORM_ZORA;
    } else {
        return false;
    }
}

s32 func_800CB924(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags3 & PLAYER_STATE3_1000;
    } else {
        return 0;
    }
}

s32 func_800CB950(Camera* camera) {
    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Camera_fabsf(camera->focalActorPosRot.pos.y - camera->focalActorFloorHeight)) < 11.0f) ||
               (camera->focalActor->gravity > -0.1f) ||
               (((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_200000);
    } else {
        return true;
    }
}

s32 Camera_IsClimbingLedge(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags1 & PLAYER_STATE1_4;
    } else {
        return 0;
    }
}

s32 Camera_IsChargingSpinAttackOrDekuFlowerDive(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (focalActor == &GET_PLAYER(camera->play)->actor) {
        return (((Player*)focalActor)->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) ||
               (((Player*)focalActor)->stateFlags3 & PLAYER_STATE3_100);
    } else {
        return false;
    }
}

s32 func_800CBA7C(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags2 & PLAYER_STATE2_800000;
    } else {
        return 0;
    }
}

PlayerMeleeWeaponState func_800CBAAC(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->meleeWeaponState;
    } else {
        return PLAYER_MELEE_WEAPON_STATE_0;
    }
}

Vec3f Camera_GetFocalActorPos(Camera* camera) {
    PosRot focalPosRot;
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->bodyPartsPos[PLAYER_BODYPART_WAIST];
    } else {
        focalPosRot = Actor_GetWorldPosShapeRot(camera->focalActor);
        return focalPosRot.pos;
    }
}

s32 Camera_IsUnderwaterAsZora(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->currentBoots == PLAYER_BOOTS_ZORA_UNDERWATER;
    } else {
        return 0;
    }
}

/**
 * Evaluate if player is in one of two sword animations
 */
s32 func_800CBB88(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        if ((((Player*)focalActor)->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
            (((Player*)focalActor)->meleeWeaponAnimation == PLAYER_MWA_GORON_PUNCH_BUTT)) {
            return 3;
        }

        if ((((Player*)focalActor)->stateFlags2 & PLAYER_STATE2_20000) ||
            ((((Player*)focalActor)->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
             (((Player*)focalActor)->meleeWeaponAnimation == PLAYER_MWA_ZORA_PUNCH_KICK))) {
            return 1;
        }
    }

    return 0;
}

s32 func_800CBC00(Camera* camera) {
    Actor* focalActor = camera->focalActor;

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        return ((Player*)focalActor)->stateFlags1 & PLAYER_STATE1_200000;
    } else {
        return 0;
    }
}

s32 func_800CBC30(Camera* camera, f32 waterYMax, f32 waterYMin) {
    if ((camera->focalActorFloorHeight != camera->waterYPos) && (camera->waterYPos < waterYMax) &&
        (camera->waterYPos > waterYMin)) {
        return true;
    } else {
        return false;
    }
}

s32 func_800CBC84(Camera* camera, Vec3f* from, CameraCollision* to, s32 arg3) {
    CollisionContext* colCtx = &camera->play->colCtx;
    Vec3f toNewPos;
    Vec3f toPoint;
    Vec3f fromToNorm = OLib_Vec3fDistNormalize(from, &to->pos);
    f32 floorPolyY;
    s32 pad;
    s32 floorBgId;

    toPoint.x = to->pos.x + fromToNorm.x;
    toPoint.y = to->pos.y + fromToNorm.y;
    toPoint.z = to->pos.z + fromToNorm.z;

    if (!BgCheck_CameraLineTest1(colCtx, from, &toPoint, &toNewPos, &to->poly, (arg3 & 1) ? false : true, true,
                                 (arg3 & 2) ? false : true, -1, &floorBgId)) {
        toNewPos = to->pos;
        toNewPos.y += 5.0f;

        if ((arg3 != 0) && func_800CB7CC(camera)) {
            // 2S2H [Port] Remote hookshot hookslide in Great Bay Temple can lead to a crash when the player is over a
            // void due to the floor poly for the player being NULL.
            // We check for NULL and handle the crash appropriately, with safe fallback using a goto into the else path.
            if (camera->focalActor->floorPoly == NULL) {
                Ship_HandleConsoleCrashAsReset();
                goto SkipFocalActorFloorPoly;
            }
            to->poly = camera->focalActor->floorPoly;
            floorBgId = camera->focalActor->floorBgId;
            to->norm.x = COLPOLY_GET_NORMAL(to->poly->normal.x);
            to->norm.y = COLPOLY_GET_NORMAL(to->poly->normal.y);
            to->norm.z = COLPOLY_GET_NORMAL(to->poly->normal.z);

            if (!Math3D_LineSegVsPlane(to->norm.x, to->norm.y, to->norm.z, to->poly->dist, from, &toPoint, &toNewPos,
                                       1)) {
                floorPolyY = to->pos.y - 20.0f;
            } else {
                toNewPos.y += 5.0f;
                floorPolyY = to->pos.y;
            }
        } else {
        SkipFocalActorFloorPoly: // 2S2H [Port] Safe crash handling
            floorPolyY = BgCheck_CameraRaycastFloor2(colCtx, &to->poly, &floorBgId, &toNewPos);
        }

        if ((to->pos.y - floorPolyY) > 5.0f) {
            to->norm.x = -fromToNorm.x;
            to->norm.y = -fromToNorm.y;
            to->norm.z = -fromToNorm.z;

            return 0;
        }
    }

    to->bgId = floorBgId;
    to->norm.x = COLPOLY_GET_NORMAL(to->poly->normal.x);
    to->norm.y = COLPOLY_GET_NORMAL(to->poly->normal.y);
    to->norm.z = COLPOLY_GET_NORMAL(to->poly->normal.z);
    to->pos.x = to->norm.x + toNewPos.x;
    to->pos.y = to->norm.y + toNewPos.y;
    to->pos.z = to->norm.z + toNewPos.z;

    return floorBgId + 1;
}

void func_800CBFA4(Camera* camera, Vec3f* arg1, Vec3f* arg2, s32 arg3) {
    CameraCollision sp20;
    s32 pad;

    sp20.pos = *arg2;
    func_800CBC84(camera, arg1, &sp20, arg3);
    *arg2 = sp20.pos;
}

/**
 * Detects the collision poly between `from` and `to`, places collision info in `to`
 */
s32 Camera_BgCheckInfo(Camera* camera, Vec3f* from, CameraCollision* to) {
    CollisionPoly* floorPoly;
    Vec3f toNewPos;
    Vec3f fromToNorm;

    if (BgCheck_CameraLineTest1(&camera->play->colCtx, from, &to->pos, &toNewPos, &to->poly, true, true, true, -1,
                                &to->bgId)) {
        floorPoly = to->poly;
        to->norm.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        to->norm.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        to->norm.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
        to->pos = toNewPos;

        return to->bgId + 1;
    }

    fromToNorm = OLib_Vec3fDistNormalize(from, &to->pos);
    to->norm.x = -fromToNorm.x;
    to->norm.y = -fromToNorm.y;
    to->norm.z = -fromToNorm.z;

    return 0;
}

/**
 * Detects if there is collision between `from` and `to`
 */
s32 Camera_BgCheck(Camera* camera, Vec3f* from, Vec3f* to) {
    CollisionContext* colCtx = &camera->play->colCtx;
    Vec3f intersect;
    s32 bgId;
    CollisionPoly* poly = NULL;

    if (BgCheck_CameraLineTest1(colCtx, from, to, &intersect, &poly, 1, 1, 1, -1, &bgId)) {
        *to = intersect;
        return true;
    }

    return false;
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
    if (BgCheck_CameraLineTest1(colCtx, from, to, &intersect, &poly, true, true, true, 0, &bgId) &&
        (CollisionPoly_GetPointDistanceFromPlane(poly, from) < 0.0f)) {
        return true;
    }

    return false;
}

s16 func_800CC260(Camera* camera, Vec3f* arg1, Vec3f* arg2, VecGeo* arg3, Actor** exclusions, s16 numExclusions) {
    VecGeo sp90;
    s32 ret;
    s32 angleCount;
    f32 rand;
    PosRot playerFocus;
    Vec3f sp64;
    Player* player = GET_PLAYER(camera->play);
    s32 i;

    sp64 = *arg2;
    playerFocus = Actor_GetFocus(&player->actor); // playerFocus unused
    sp90 = *arg3;

    angleCount = ARRAY_COUNT(D_801B9E18);

    for (i = 0; i < angleCount; i++) {
        *arg1 = OLib_AddVecGeoToVec3f(arg2, &sp90);

        if (!Camera_CheckOOB(camera, arg1, &sp64) && !func_800CBC30(camera, arg2->y, arg1->y) &&
            !CollisionCheck_LineOCCheck(camera->play, &camera->play->colChkCtx, arg2, arg1, exclusions,
                                        numExclusions)) {
            break;
        }

        sp90.yaw = D_801B9E18[i] + arg3->yaw;
        rand = Rand_ZeroOne();
        sp90.pitch = D_801B9E34[i] + TRUNCF_BINANG(arg3->pitch * rand);

        if (sp90.pitch > 0x36B0) { // 76.9 degrees
            sp90.pitch -= 0x3E80;  // -87.9 degrees
        }
        if (sp90.pitch < -0x36B0) { // -76.9 degrees
            sp90.pitch += 0x3A98;   // 82.4 degrees
        }

        sp90.r *= 0.9f;
        sp64 = *arg2;
    }

    *arg3 = sp90;

    if (i == angleCount) {
        ret = -1;
    } else {
        ret = i;
    }

    return ret;
}

/**
 * Gets the floor position underneath `chkPos`, and returns the normal of the floor to `floorNorm`,
 * and bgId to `bgId`.  If no floor is found, then the normal is a flat surface pointing upwards.
 */
f32 Camera_GetFloorYNorm(Camera* camera, Vec3f* floorNorm, Vec3f* chkPos, s32* bgId) {
    CollisionContext* colCtx = &camera->play->colCtx;
    CollisionPoly* floorPoly;
    f32 floorY = BgCheck_EntityRaycastFloor3(colCtx, &floorPoly, bgId, chkPos);

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
 * it checks the next floor below that.  Returns the normal of the floor into `norm`
 */
f32 Camera_GetFloorYLayer(Camera* camera, Vec3f* norm, Vec3f* pos, s32* bgId) {
    CollisionPoly* floorPoly;
    CollisionContext* colCtx = &camera->play->colCtx;
    f32 floorY;

    if (camera->focalActor != NULL) {
        floorY = BgCheck_EntityRaycastFloor5_3(camera->play, &camera->play->colCtx, &floorPoly, bgId,
                                               camera->focalActor, pos);
    } else {
        floorY = BgCheck_CameraRaycastFloor2(colCtx, &floorPoly, bgId, pos);
    }

    if ((floorY == BGCHECK_Y_MIN) ||
        ((camera->focalActorFloorHeight < floorY) && !(COLPOLY_GET_NORMAL(floorPoly->normal.y) > 0.5f))) {
        // no floor, or player is below the floor and floor is not considered steep
        norm->x = 0.0f;
        norm->y = 1.0f;
        norm->z = 0.0f;
        floorY = BGCHECK_Y_MIN;
    } else {
        norm->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        norm->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        norm->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
    }

    return floorY;
}

#define CAM_DATA_IS_BG (1 << 12) // if not set, then cam data is for actor cutscenes

/**
 * Returns the CameraSettingType of the camera from either the bgCam or the actorCsCam at index `camDataId`
 */
s16 Camera_GetBgCamOrActorCsCamSetting(Camera* camera, u32 camDataId) {
    if (camDataId & CAM_DATA_IS_BG) {
        return BgCheck_GetBgCamSettingImpl(&camera->play->colCtx, camDataId & ~CAM_DATA_IS_BG, BGCHECK_SCENE);
    } else {
        return Play_GetActorCsCamSetting(camera->play, camDataId);
    }
}

/**
 * Returns either the bgCam data or the actorCsCam data at index `camDataId`
 */
Vec3s* Camera_GetBgCamOrActorCsCamFuncData(Camera* camera, u32 camDataId) {
    if (camDataId & CAM_DATA_IS_BG) {
        return BgCheck_GetBgCamFuncDataImpl(&camera->play->colCtx, camDataId & ~CAM_DATA_IS_BG, BGCHECK_SCENE);
    } else {
        return Play_GetActorCsCamFuncData(camera->play, camDataId);
    }
}

/**
 * Gets the bgCam index for the poly `poly`, returns -1 if
 * there is no camera data for that poly.
 */
s32 Camera_GetBgCamIndex(Camera* camera, s32* bgId, CollisionPoly* poly) {
    s32 bgCamIndex = SurfaceType_GetBgCamIndex(&camera->play->colCtx, poly, *bgId);
    s32 ret;

    if (BgCheck_GetBgCamSettingImpl(&camera->play->colCtx, bgCamIndex, *bgId) == CAM_SET_NONE) {
        ret = -1;
    } else {
        ret = bgCamIndex;
    }

    return ret;
}

/**
 * Gets the Camera setting for the water box the player is in.
 * Returns -1 if the player is not in a water box, or does not have a swimming state.
 * Returns -2 if there is no bgCam index for the water box.
 * Returns the camera setting otherwise.
 */
s32 Camera_GetWaterBoxBgCamSetting(Camera* camera, f32* waterY) {
    PosRot playerPosShape;
    WaterBox* waterBox;
    s32 camSetting;
    s32 bgId;

    playerPosShape = Actor_GetWorldPosShapeRot(camera->focalActor);
    *waterY = playerPosShape.pos.y;

    if (!WaterBox_GetSurfaceImpl(camera->play, &camera->play->colCtx, playerPosShape.pos.x, playerPosShape.pos.z,
                                 waterY, &waterBox, &bgId)) {
        // player's position is not in a waterbox
        *waterY = playerPosShape.pos.y;
        return -1;
    }

    if (!Camera_IsSwimming(camera)) {
        return -1;
    }

    camSetting = WaterBox_GetBgCamSetting(&camera->play->colCtx, waterBox, bgId);

    // -2: no bgCam index
    return (camSetting == CAM_SET_NONE) ? -2 : camSetting;
}

void func_800CC938(Camera* camera) {
    func_800DDFE0(camera);
}

/**
 * Calculates the angle between points `from` and `to`
 */
s16 Camera_CalcXZAngle(Vec3f* to, Vec3f* from) {
    return CAM_DEG_TO_BINANG(RAD_TO_DEG(Math_FAtan2F(from->x - to->x, from->z - to->z)));
}

// BSS
s16 D_801EDBF0;

/**
 * Calculates a pitch adjustment by sampling a position in front of the focal actor and testing the floor height.
 * Used to detect if the focal actor is near a slope, edge, or cliff.
 *
 * @param camera
 * @param viewYaw The yaw the camera is facing, gives a direction to sample a position
 * @param shouldInit
 *
 * @return pitchOffset resulting pitch adjustment
 */
s16 Camera_GetPitchAdjFromFloorHeightDiffs(Camera* camera, s16 viewYaw, s16 shouldInit) {
    static f32 sFloorYNear;
    static f32 sFloorYFar;
    static CameraCollision sFarColChk;
    Vec3f focalActorPos;
    Vec3f nearPos;
    Vec3f floorNorm;
    f32 checkOffsetY;
    s16 pitchNear;
    s16 pitchFar;
    f32 floorYDiffFar;
    f32 viewForwardsUnitX = Math_SinS(viewYaw);
    f32 viewForwardsUnitZ = Math_CosS(viewYaw);
    s32 bgId;
    f32 nearDist;
    f32 farDist;
    f32 floorYDiffNear;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);

    checkOffsetY = focalActorHeight * 1.2f;
    nearDist = focalActorHeight * 1.0f;
    farDist = focalActorHeight * 2.5f;

    focalActorPos.x = camera->focalActorPosRot.pos.x;
    focalActorPos.y = camera->focalActorFloorHeight + checkOffsetY;
    focalActorPos.z = camera->focalActorPosRot.pos.z;

    nearPos.x = focalActorPos.x + (nearDist * viewForwardsUnitX);
    nearPos.y = focalActorPos.y;
    nearPos.z = focalActorPos.z + (nearDist * viewForwardsUnitZ);

    if (shouldInit || ((camera->play->state.frames % 2) == 0)) {
        sFarColChk.pos.x = focalActorPos.x + (farDist * viewForwardsUnitX);
        sFarColChk.pos.y = focalActorPos.y;
        sFarColChk.pos.z = focalActorPos.z + (farDist * viewForwardsUnitZ);

        Camera_BgCheckInfo(camera, &focalActorPos, &sFarColChk);

        if (shouldInit) {
            sFloorYNear = sFloorYFar = camera->focalActorFloorHeight;
        }
    } else {
        farDist = OLib_Vec3fDistXZ(&focalActorPos, &sFarColChk.pos);

        sFarColChk.pos.x += sFarColChk.norm.x * 5.0f;
        sFarColChk.pos.y += sFarColChk.norm.y * 5.0f;
        sFarColChk.pos.z += sFarColChk.norm.z * 5.0f;

        if (nearDist > farDist) {
            nearDist = farDist;
            sFloorYNear = sFloorYFar = Camera_GetFloorYLayer(camera, &floorNorm, &sFarColChk.pos, &bgId);
        } else {
            sFloorYNear = Camera_GetFloorYLayer(camera, &floorNorm, &nearPos, &bgId);
            sFloorYFar = Camera_GetFloorYLayer(camera, &floorNorm, &sFarColChk.pos, &bgId);
        }

        if (sFloorYNear == BGCHECK_Y_MIN) {
            sFloorYNear = camera->focalActorFloorHeight;
        }

        if (sFloorYFar == BGCHECK_Y_MIN) {
            sFloorYFar = sFloorYNear;
        }
    }

    floorYDiffNear = (sFloorYNear - camera->focalActorFloorHeight) * 0.8f;
    floorYDiffFar = (sFloorYFar - camera->focalActorFloorHeight) * (20.0f * 0.01f);

    pitchNear = CAM_DEG_TO_BINANG(RAD_TO_DEG(Math_FAtan2F(floorYDiffNear, nearDist)));
    pitchFar = CAM_DEG_TO_BINANG(RAD_TO_DEG(Math_FAtan2F(floorYDiffFar, farDist)));

    return pitchNear + pitchFar;
}

f32 func_800CCCEC(Camera* camera, s16 reset) {
    static s32 D_801B9E5C = 0;
    static f32 D_801B9E60 = 0.0f;
    Vec3f offsetForwards;
    Vec3f angledOffsetForwards;
    PosRot focalActorPosRot;
    CameraCollision camCollision;
    f32 forwardsUnitZ;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    f32 forwardsUnitX;
    f32 distResult;
    s16 yawForwardsOffset;
    f32 distResultAdj;

    if (reset) {
        D_801B9E5C = 0;
        D_801B9E60 = 0.0f;
    }

    focalActorPosRot = camera->focalActorPosRot;

    forwardsUnitX = Math_SinS(focalActorPosRot.rot.y);
    forwardsUnitZ = Math_CosS(focalActorPosRot.rot.y);

    offsetForwards.x = focalActorPosRot.pos.x + (30.0f * forwardsUnitX);
    offsetForwards.y = focalActorPosRot.pos.y + focalActorHeight;
    offsetForwards.z = focalActorPosRot.pos.z + (30.0f * forwardsUnitZ);

    camCollision.pos.x = focalActorPosRot.pos.x + (12.0f * forwardsUnitX);
    camCollision.pos.y = offsetForwards.y;
    camCollision.pos.z = focalActorPosRot.pos.z + (12.0f * forwardsUnitZ);

    if ((camera->play->state.frames % 2) != 0) {
        // Turn and project 65.92 degrees left
        yawForwardsOffset = focalActorPosRot.rot.y + DEG_TO_BINANG(65.92f);

        angledOffsetForwards.x = (Math_SinS(yawForwardsOffset) * 50.0f) + offsetForwards.x;
        angledOffsetForwards.y = offsetForwards.y;
        angledOffsetForwards.z = (Math_CosS(yawForwardsOffset) * 50.0f) + offsetForwards.z;

        if ((Camera_BgCheckInfo(camera, &angledOffsetForwards, &camCollision) != 0) &&
            Camera_CheckOOB(camera, &offsetForwards, &angledOffsetForwards)) {
            // Always going to result in 28.0f?
            distResult = OLib_Vec3fDistXZ(&offsetForwards, &camCollision.pos);

            // (-D_801B9E60 < (50.0f - distResult))
            if (!((D_801B9E5C == 2) && (-D_801B9E60 >= (50.0f - distResult)))) {
                D_801B9E5C = 1;
                distResult = 50.0f - distResult; // 22.0f
                D_801B9E60 = distResult;

                // if (distResult == 0.0f)
                if (distResult == -distResult) {
                    distResultAdj = 0.0f;
                } else {
                    distResultAdj = distResult;
                }

                return distResultAdj;
            }
        }
    } else {
        // Turn and project 90 degrees right
        yawForwardsOffset = focalActorPosRot.rot.y - DEG_TO_BINANG(90);

        angledOffsetForwards.x = (Math_SinS(yawForwardsOffset) * 50.0f) + offsetForwards.x;
        angledOffsetForwards.y = offsetForwards.y;
        angledOffsetForwards.z = (Math_CosS(yawForwardsOffset) * 50.0f) + offsetForwards.z;

        if ((Camera_BgCheckInfo(camera, &angledOffsetForwards, &camCollision) != 0) &&
            Camera_CheckOOB(camera, &offsetForwards, &angledOffsetForwards)) {
            // Always going to result in 28.0f?
            distResult = OLib_Vec3fDistXZ(&offsetForwards, &camCollision.pos);

            if (!((D_801B9E5C == 1) && (D_801B9E60 >= -(distResult - 50.0f)))) {
                D_801B9E5C = 2;
                distResult = distResult - 50.0f;
                D_801B9E60 = distResult;
                if (distResult == -distResult) {
                    distResultAdj = 0.0f;
                } else {
                    distResultAdj = distResult;
                }
                return distResultAdj;
            }
        }
    }

    if (D_801B9E5C != 0) {
        distResult = D_801B9E60;
    } else {
        distResult = 0.0f;
    }

    D_801B9E5C = 0;
    D_801B9E60 = 0.0f;

    return distResult;
}

/**
 * Calculates a new Up vector from the pitch, yaw, roll
 */
Vec3f Camera_CalcUpVec(s16 pitch, s16 yaw, s16 roll) {
    f32 sinP = Math_SinS(pitch);
    f32 cosP = Math_CosS(pitch);
    f32 sinY = Math_SinS(yaw);
    f32 cosY = Math_CosS(yaw);
    f32 sinR = Math_SinS(-roll);
    f32 cosR = Math_CosS(-roll);
    Vec3f up;
    Vec3f baseUp;
    Vec3f u;
    Vec3f rollMtxRow1;
    Vec3f rollMtxRow2;
    Vec3f rollMtxRow3;
    s32 pad;

    // Axis to roll around
    u.x = cosP * sinY;
    u.y = sinP;
    u.z = cosP * cosY;

    // Matrix to apply the roll to the Up vector without roll
    rollMtxRow1.x = ((1.0f - SQ(u.x)) * cosR) + SQ(u.x);
    rollMtxRow1.y = ((u.x * u.y) * (1.0f - cosR)) - (u.z * sinR);
    rollMtxRow1.z = ((u.z * u.x) * (1.0f - cosR)) + (u.y * sinR);

    rollMtxRow2.x = ((u.x * u.y) * (1.0f - cosR)) + (u.z * sinR);
    rollMtxRow2.y = ((1.0f - SQ(u.y)) * cosR) + SQ(u.y);
    rollMtxRow2.z = ((u.y * u.z) * (1.0f - cosR)) - (u.x * sinR);

    rollMtxRow3.x = ((u.z * u.x) * (1.0f - cosR)) - (u.y * sinR);
    rollMtxRow3.y = ((u.y * u.z) * (1.0f - cosR)) + (u.x * sinR);
    rollMtxRow3.z = ((1.0f - SQ(u.z)) * cosR) + SQ(u.z);

    // Up without roll
    baseUp.x = -sinP * sinY;
    baseUp.y = cosP;
    baseUp.z = -sinP * cosY;

    // rollMtx * baseUp
    up.x = DOTXYZ(baseUp, rollMtxRow1);
    up.y = DOTXYZ(baseUp, rollMtxRow2);
    up.z = DOTXYZ(baseUp, rollMtxRow3);

    return up;
}

f32 Camera_ClampLerpScale(Camera* camera, f32 maxLerpScale) {
    f32 ret;

    if (camera->atLerpStepScale < 0.12f) {
        ret = 0.12f;
    } else if (camera->atLerpStepScale >= maxLerpScale) {
        ret = maxLerpScale;
    } else {
        ret = 1.1f * camera->atLerpStepScale;
    }

    return ret;
}

void Camera_ResetActionFuncState(Camera* camera, s32 mode) {
    camera->animState = 0;
}

void Camera_UpdateInterface(s32 interfaceFlags) {
    s32 hudVisibility;

    if ((interfaceFlags & CAM_LETTERBOX_MASK) != CAM_LETTERBOX_IGNORE) {
        switch (interfaceFlags & CAM_LETTERBOX_SIZE_MASK) {
            case CAM_LETTERBOX_SMALL:
                sCameraLetterboxSize = 26;
                break;

            case CAM_LETTERBOX_MEDIUM:
                sCameraLetterboxSize = 27;
                break;

            case CAM_LETTERBOX_LARGE:
                sCameraLetterboxSize = 32;
                break;

            default:
                sCameraLetterboxSize = 0;
                break;
        }

        if (interfaceFlags & CAM_LETTERBOX_INSTANT) {
            ShrinkWindow_Letterbox_SetSize(sCameraLetterboxSize);
        } else {
            ShrinkWindow_Letterbox_SetSizeTarget(sCameraLetterboxSize);
        }
    }

    if ((interfaceFlags & CAM_HUD_VISIBILITY_MASK) != CAM_HUD_VISIBILITY_IGNORE) {
        hudVisibility = (interfaceFlags & CAM_HUD_VISIBILITY_MASK) >> CAM_HUD_VISIBILITY_SHIFT;
        if (hudVisibility == (CAM_HUD_VISIBILITY_ALL >> CAM_HUD_VISIBILITY_SHIFT)) {
            hudVisibility = 50;
        }
        if (hudVisibility != sCameraHudVisibility) {
            sCameraHudVisibility = hudVisibility;
            Interface_SetHudVisibility(sCameraHudVisibility);
        }
    }
}

Vec3f Camera_BgCheckCorner(Vec3f* linePointA, Vec3f* linePointB, CameraCollision* pointAColChk,
                           CameraCollision* pointBColChk) {
    Vec3f closestPoint;

    func_800CAA14(pointAColChk->poly, pointBColChk->poly, linePointA, linePointB, &closestPoint);

    return closestPoint;
}

/**
 * Checks collision between at and eyeNext, if `checkEye` is set, if there is no collsion between
 * eyeNext->at, then eye->at is also checked.
 * Returns:
 * 0 if no collsion is found between at->eyeNext
 * 1 if the angle between the polys is between 60 degrees and 120 degrees
 * 3 ?
 * 6 if the angle between the polys is greater than 120 degrees
 */
s32 func_800CD44C(Camera* camera, VecGeo* diffGeo, CameraCollision* camEyeCollision, CameraCollision* camAtCollision,
                  s16 checkEye) {
    Vec3f* at = &camera->at;
    s32 pad[2];
    s32 atEyeBgId;
    s32 eyeAtBgId;
    s32 ret;
    f32 cosEyeAt;
    CameraCollision camCollision;

    camEyeCollision->pos = camera->eyeNext;

    ret = 0;

    atEyeBgId = func_800CBC84(camera, at, camEyeCollision, 0);
    if (atEyeBgId != 0) {
        // collision found between at->ey
        camAtCollision->pos = *at;

        camEyeCollision->geoNorm = OLib_Vec3fToVecGeo(&camEyeCollision->norm);

        if (camEyeCollision->geoNorm.pitch > 0x2EE0) { // 65.9 degrees
            camEyeCollision->geoNorm.yaw = diffGeo->yaw;
        }

        if (checkEye & 1) {
            memcpy(&camCollision, camAtCollision, sizeof(CameraCollision));
        }

        eyeAtBgId = Camera_BgCheckInfo(camera, &camera->eye, camAtCollision);

        if (eyeAtBgId == 0) {
            // no collision from eyeNext->at
            if (checkEye & 1) {
                memcpy(camAtCollision, &camCollision, sizeof(CameraCollision));
            } else {
                return 3;
            }
        }

        if (camEyeCollision->poly == camAtCollision->poly) {
            // at->eyeNext and eyeNext->at is the same poly
            return 3;
        }

        camAtCollision->geoNorm = OLib_Vec3fToVecGeo(&camAtCollision->norm);

        if (camAtCollision->geoNorm.pitch > 0x2EE0) { // 65.9 degrees
            camAtCollision->geoNorm.yaw = BINANG_ROT180(diffGeo->yaw);
        }

        if (atEyeBgId != eyeAtBgId) {
            // different bgIds for at->eye[Next] and eye[Next]->at
            ret = 3;
        } else {
            cosEyeAt = Math3D_Cos(&camEyeCollision->norm, &camAtCollision->norm);
            if (cosEyeAt < -0.5f) {
                ret = 6;
            } else if ((cosEyeAt > 0.5f) || (checkEye & 2)) {
                ret = 3;
            } else {
                ret = 1;
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
    VecGeo floorNormGeo;

    floorNormGeo = OLib_Vec3fToVecGeo(floorNorm);

    tmp = Math_CosS(floorNormGeo.pitch) * Math_CosS(playerYRot - floorNormGeo.yaw);

    return (Camera_fabsf(tmp) * adjAmt) * Math_CosS(playerYRot - eyeAtYaw);
}

/**
 * Gets the distance that the attention actor is in range.
 */
f32 Camera_GetAttentionActorRange(Actor* actor) {
    return sqrtf(gAttentionRanges[actor->attentionRangeType].attentionRangeSq /
                 gAttentionRanges[actor->attentionRangeType].lockOnLeashScale);
}

/**
 * Calculates new at vector for the camera pointing in `eyeAtDir`
 */
s32 Camera_CalcAtDefault(Camera* camera, VecGeo* eyeAtDir, f32 yOffset, s16 calcSlope) {
    Vec3f* at = &camera->at;
    Vec3f focalActorAtOffsetTarget;
    Vec3f atTarget;
    s32 pad;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);

    focalActorAtOffsetTarget.y = focalActorHeight + yOffset;
    focalActorAtOffsetTarget.x = 0.0f;
    focalActorAtOffsetTarget.z = 0.0f;

    if (calcSlope) {
        focalActorAtOffsetTarget.y -= OLib_ClampMaxDist(
            Camera_CalcSlopeYAdj(&camera->floorNorm, focalActorPosRot->rot.y, eyeAtDir->yaw, 25.0f), focalActorHeight);
    }

    Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, camera->xzOffsetUpdateRate,
                                 camera->yOffsetUpdateRate, 0.1f);

    atTarget.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
    atTarget.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
    atTarget.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;

    if (atTarget.y < (camera->focalActorFloorHeight + 10.0f)) {
        atTarget.y = camera->focalActorFloorHeight + 10.0f;
    }

    Camera_ScaledStepToCeilVec3f(&atTarget, at, camera->atLerpStepScale, camera->atLerpStepScale, 0.2f);

    return 1;
}

s32 Camera_CalcAtForScreen(Camera* camera, VecGeo* eyeAtDir, f32 yOffset, f32* focalActorPosY, f32 deltaYMax) {
    f32 deltaY;
    Vec3f focalActorAtOffsetTarget;
    Vec3f atTarget;
    s32 pad;
    f32 clampedDeltaY;
    f32 clampedAbsScreenY;
    s16 absScreenPosY;
    s16 screenPosY;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);

    focalActorAtOffsetTarget.y = focalActorHeight + yOffset;
    focalActorAtOffsetTarget.x = 0.0f;
    focalActorAtOffsetTarget.z = 0.0f;

    Actor_GetScreenPos(camera->play, camera->focalActor, &absScreenPosY, &screenPosY);
    screenPosY -= SCREEN_HEIGHT / 2;
    absScreenPosY = ABS(screenPosY);

    // result unused
    clampedAbsScreenY = OLib_ClampMaxDist(absScreenPosY / (f32)(SCREEN_HEIGHT / 2), 1.0f);

    deltaY = focalActorPosRot->pos.y - *focalActorPosY;
    clampedDeltaY = OLib_ClampMaxDist(deltaY, deltaYMax);

    if (absScreenPosY > (SCREEN_HEIGHT / 4)) {
        absScreenPosY = SCREEN_HEIGHT / 4;
    }

    clampedAbsScreenY = OLib_ClampMaxDist(absScreenPosY / (f32)(SCREEN_HEIGHT / 4), 1.0f);

    focalActorAtOffsetTarget.y -= clampedDeltaY * clampedAbsScreenY * clampedAbsScreenY;
    Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, camera->xzOffsetUpdateRate,
                                 camera->yOffsetUpdateRate, 0.1f);

    atTarget.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
    atTarget.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
    atTarget.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
    atTarget.y = CLAMP_MIN(atTarget.y, camera->focalActorFloorHeight + 10.0f);

    Camera_ScaledStepToCeilVec3f(&atTarget, &camera->at, camera->atLerpStepScale, camera->atLerpStepScale, 0.1f);

    return 1;
}

s32 Camera_CalcAtForNormal1(Camera* camera, VecGeo* arg1, f32 yOffset, f32 forwardDist) {
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Vec3f focalActorAtOffsetTarget;
    Vec3f atTarget;
    Vec3f collisionFromPos;
    s32 pad;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);

    focalActorAtOffsetTarget.x = Math_SinS(focalActorPosRot->rot.y) * forwardDist;
    focalActorAtOffsetTarget.z = Math_CosS(focalActorPosRot->rot.y) * forwardDist;
    focalActorAtOffsetTarget.y = focalActorHeight + yOffset;

    Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, camera->xzOffsetUpdateRate,
                                 camera->yOffsetUpdateRate, 0.1f);

    atTarget.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
    atTarget.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
    atTarget.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;

    collisionFromPos.x = focalActorPosRot->pos.x;
    collisionFromPos.y = atTarget.y;
    collisionFromPos.z = focalActorPosRot->pos.z;

    if (Camera_BgCheck(camera, &collisionFromPos, &atTarget)) {
        atTarget.x -= camera->focalActorAtOffset.x - (atTarget.x - collisionFromPos.x);
        atTarget.z -= camera->focalActorAtOffset.z - (atTarget.z - collisionFromPos.z);
    }

    Camera_ScaledStepToCeilVec3f(&atTarget, &camera->at, camera->atLerpStepScale, camera->atLerpStepScale, 0.2f);

    return 1;
}

/**
 * Adjusts the camera's at position for Camera_Parallel1
 */
s32 Camera_CalcAtForParallel(Camera* camera, VecGeo* arg1, f32 yOffset, f32 xzOffsetMax, f32* focalActorPosY,
                             s16 flags) {
    s32 pad;
    Vec3f focalActorAtOffsetTarget;
    Vec3f atTarget;
    f32 fovHeight;
    Vec3f* at = &camera->at;
    f32 deltaY;
    f32 eyeAtDistXZ;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    VecGeo focalActorAtOffsetTargetGeo;

    // Calculate the `focalActorAtOffsetTarget`
    // @TODO: Only uses `PARALLEL1_FLAG_` flags, but may need separate flags for `CalcAt` system
    if (flags & 0x40) {
        focalActorAtOffsetTargetGeo.r = func_800CCCEC(camera, flags & 0x10);
        focalActorAtOffsetTargetGeo.yaw = focalActorPosRot->rot.y + 0x4000;
        focalActorAtOffsetTargetGeo.pitch = 0;
        focalActorAtOffsetTarget = OLib_VecGeoToVec3f(&focalActorAtOffsetTargetGeo);
    } else {
        f32 xOffset = camera->focalActorAtOffset.x + camera->unk_0F0.x;
        f32 zOffset = camera->focalActorAtOffset.z + camera->unk_0F0.z;

        if (sqrtf(SQ(xOffset) + SQ(zOffset)) < xzOffsetMax) {
            focalActorAtOffsetTarget.x = xOffset;
            focalActorAtOffsetTarget.z = zOffset;
        } else {
            focalActorAtOffsetTarget.x = camera->focalActorAtOffset.x;
            focalActorAtOffsetTarget.z = camera->focalActorAtOffset.z;
        }
    }

    focalActorAtOffsetTarget.y = focalActorHeight + yOffset;

    // focalActorAtOffsetTarget.y based on slope
    if (PREG(76) && flags) {
        focalActorAtOffsetTarget.y -=
            Camera_CalcSlopeYAdj(&camera->floorNorm, focalActorPosRot->rot.y, arg1->yaw, 25.0f);
    }

    // Adjust posOffsetTarget.y based on something
    if (func_800CB950(camera)) {
        *focalActorPosY = Camera_ScaledStepToCeilF(focalActorPosRot->pos.y, *focalActorPosY, 0.4f, 0.1f);
        deltaY = focalActorPosRot->pos.y - *focalActorPosY;
        focalActorAtOffsetTarget.y -= deltaY;
        Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, camera->xzOffsetUpdateRate,
                                     camera->yOffsetUpdateRate, 0.1f);
    } else {
        deltaY = focalActorPosRot->pos.y - *focalActorPosY;
        eyeAtDistXZ = OLib_Vec3fDistXZ(at, &camera->eye);

        // Get the height based on 80% of the fov
        fovHeight = Math_FTanF(DEG_TO_RAD(camera->fov * (0.8f * 0.5f))) * eyeAtDistXZ;

        if (deltaY > fovHeight) {
            *focalActorPosY += deltaY - fovHeight;
            deltaY = fovHeight;
        } else if (deltaY < -fovHeight) {
            *focalActorPosY += deltaY + fovHeight;
            deltaY = -fovHeight;
        }

        focalActorAtOffsetTarget.y -= deltaY;
        Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, 0.3f, 0.2f, 0.1f);
        camera->xzOffsetUpdateRate = 0.3f;
        camera->yOffsetUpdateRate = 0.2f;
    }

    atTarget.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
    atTarget.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
    atTarget.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
    Camera_ScaledStepToCeilVec3f(&atTarget, at, camera->atLerpStepScale, camera->atLerpStepScale, 0.2f);

    return 1;
}

s32 Camera_CalcAtForFriendlyLockOn(Camera* camera, VecGeo* eyeAtDir, Vec3f* targetPos, f32 yOffset, f32 distance,
                                   f32* yPosOffset, VecGeo* outPlayerToTargetDir, s16 flags) {
    Vec3f* at = &camera->at;
    Vec3f focalActorAtOffsetTarget;
    Vec3f atTarget;
    Vec3f sp68;
    f32 temp_f0_6;
    VecGeo sp5C;
    f32 deltaY;
    f32 fovHeight;
    f32 phi_f16;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    f32 sp50;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;

    focalActorAtOffsetTarget.y = focalActorHeight + yOffset;
    focalActorAtOffsetTarget.x = 0.0f;
    focalActorAtOffsetTarget.z = 0.0f;

    if (PREG(76) && (flags & FLG_ADJSLOPE)) {
        focalActorAtOffsetTarget.y -=
            Camera_CalcSlopeYAdj(&camera->floorNorm, camera->focalActorPosRot.rot.y, eyeAtDir->yaw, 25.0f);
    }

    atTarget = focalActorPosRot->pos;
    atTarget.y += focalActorHeight;

    *outPlayerToTargetDir = OLib_Vec3fDiffToVecGeo(&atTarget, targetPos);
    sp5C = *outPlayerToTargetDir;

    sp5C.r = (distance < sp5C.r) ? (sp5C.r * 0.2f) : ((sp5C.r * 0.9f) - (sp5C.r * 0.7f * (sp5C.r / distance)));

    if (flags & FLG_OFFGROUND) {
        sp5C.r *= 0.2f;
        camera->yOffsetUpdateRate = camera->xzOffsetUpdateRate = 0.01f;
    }

    sp68 = OLib_VecGeoToVec3f(&sp5C);

    focalActorAtOffsetTarget.x += sp68.x;
    focalActorAtOffsetTarget.y += sp68.y;
    focalActorAtOffsetTarget.z += sp68.z;

    if (func_800CB950(camera)) {
        *yPosOffset = Camera_ScaledStepToCeilF(focalActorPosRot->pos.y, *yPosOffset, 0.4f, 0.1f);
        deltaY = focalActorPosRot->pos.y - *yPosOffset;
        focalActorAtOffsetTarget.y -= deltaY;
        Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, camera->xzOffsetUpdateRate,
                                     camera->yOffsetUpdateRate, 0.1f);
    } else {
        if (!(flags & 0x80)) {
            deltaY = focalActorPosRot->pos.y - *yPosOffset;
            sp50 = OLib_Vec3fDistXZ(at, &camera->eye);
            phi_f16 = sp50;
            Math_FAtan2F(deltaY, sp50);
            fovHeight = Math_FTanF(DEG_TO_RAD(camera->fov * 0.4f)) * phi_f16;

            if (fovHeight < deltaY) {
                *yPosOffset += deltaY - fovHeight;
                deltaY = fovHeight;
            } else if (deltaY < -fovHeight) {
                *yPosOffset += deltaY + fovHeight;
                deltaY = -fovHeight;
            }
            focalActorAtOffsetTarget.y -= deltaY;
        } else {
            deltaY = focalActorPosRot->pos.y - *yPosOffset;
            temp_f0_6 = Math_FAtan2F(deltaY, OLib_Vec3fDistXZ(at, &camera->eye));

            if (temp_f0_6 > 0.34906584f) { // (M_PI / 9)
                phi_f16 = 1.0f - Math_SinF(temp_f0_6 - 0.34906584f);
            } else if (temp_f0_6 < -0.17453292f) { // (M_PI / 18)
                phi_f16 = 1.0f - Math_SinF(-0.17453292f - temp_f0_6);
            } else {
                phi_f16 = 1.0f;
            }

            focalActorAtOffsetTarget.y -= deltaY * phi_f16;
        }

        Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, 0.5f, 0.5f, 0.1f);
        camera->xzOffsetUpdateRate = 0.5f;
        camera->yOffsetUpdateRate = 0.5f;
    }

    atTarget.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
    atTarget.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
    atTarget.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
    Camera_ScaledStepToCeilVec3f(&atTarget, at, camera->atLerpStepScale, camera->atLerpStepScale, 0.2f);

    return 1;
}

s32 Camera_CalcAtForEnemyLockOn(Camera* camera, VecGeo* arg1, Vec3f* arg2, f32 yOffset, f32 arg4, f32 arg5, f32* arg6,
                                VecGeo* arg7, s16 flags) {
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Vec3f focalActorAtOffsetTarget;
    Vec3f atTarget;
    Vec3f sp60;
    VecGeo sp58;
    f32 temp_f0_3;
    f32 deltaY;
    f32 temp;
    f32 fovHeight;
    f32 pad[2];

    focalActorAtOffsetTarget.y = Camera_GetFocalActorHeight(camera) + yOffset;
    focalActorAtOffsetTarget.x = 0.0f;
    focalActorAtOffsetTarget.z = 0.0f;

    sp58 = *arg7;
    sp58.r = arg7->r * (arg5 * arg4);

    if (flags & 0x80) {
        camera->yOffsetUpdateRate = 0.01f;
        camera->xzOffsetUpdateRate = 0.01f;
    }

    sp60 = OLib_VecGeoToVec3f(&sp58);

    focalActorAtOffsetTarget.x += sp60.x;
    focalActorAtOffsetTarget.y += sp60.y;
    focalActorAtOffsetTarget.z += sp60.z;

    if (func_800CB950(camera)) {
        *arg6 = Camera_ScaledStepToCeilF(focalActorPosRot->pos.y, *arg6, 0.4f, 0.1f);
        deltaY = focalActorPosRot->pos.y - *arg6;
        focalActorAtOffsetTarget.y -= deltaY;
        Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, camera->xzOffsetUpdateRate,
                                     camera->yOffsetUpdateRate, 0.1f);
    } else {
        deltaY = focalActorPosRot->pos.y - *arg6;

        temp = arg1->r;
        temp_f0_3 = Math_FAtan2F(deltaY, temp);

        if (!(flags & 0x80)) {
            fovHeight = Math_FTanF(DEG_TO_RAD(camera->fov * 0.4f)) * temp;

            if (fovHeight < deltaY) {
                *arg6 += deltaY - fovHeight;
                deltaY = fovHeight;
            } else if (deltaY < -fovHeight) {
                *arg6 += deltaY + fovHeight;
                deltaY = -fovHeight;
            }

            focalActorAtOffsetTarget.y -= deltaY;
        } else {
            if (temp_f0_3 > 0.34906584f) { // (M_PI / 9)
                temp = 1.0f - Math_SinF(temp_f0_3 - 0.34906584f);
            } else if (temp_f0_3 < -0.17453292f) { // (M_PI / 18)
                temp = 1.0f - Math_SinF(-0.17453292f - temp_f0_3);
            } else {
                temp = 1.0f;
            }

            focalActorAtOffsetTarget.y -= deltaY * temp;
        }
        Camera_ScaledStepToCeilVec3f(&focalActorAtOffsetTarget, &camera->focalActorAtOffset, 0.5f, 0.5f, 0.1f);
        camera->xzOffsetUpdateRate = 0.5f;
        camera->yOffsetUpdateRate = 0.5f;
    }

    atTarget.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
    atTarget.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
    atTarget.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
    Camera_ScaledStepToCeilVec3f(&atTarget, &camera->at, camera->atLerpStepScale, camera->atLerpStepScale, 0.2f);

    return true;
}

s32 Camera_CalcAtForHorse(Camera* camera, VecGeo* eyeAtDir, f32 yOffset, f32* yPosOffset, s16 calcSlope) {
    Vec3f* at = &camera->at;
    Vec3f posOffsetTarget;
    Vec3f atTarget;
    s32 pad[2];
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    Player* player = (Player*)camera->focalActor;
    PosRot horsePosRot;

    horsePosRot = Actor_GetWorld(player->rideActor);

    if (EN_HORSE_CHECK_JUMPING((EnHorse*)player->rideActor)) {
        horsePosRot.pos.y -= 49.0f;
        *yPosOffset = Camera_ScaledStepToCeilF(horsePosRot.pos.y, *yPosOffset, 0.1f, 0.1f);
        camera->atLerpStepScale = Camera_ScaledStepToCeilF(0.4f, camera->atLerpStepScale, 0.2f, 0.02f);
    } else {
        *yPosOffset = Camera_ScaledStepToCeilF(horsePosRot.pos.y, *yPosOffset, 0.5f, 0.1f);
    }

    posOffsetTarget.x = 0.0f;
    posOffsetTarget.y = focalActorHeight + yOffset;
    posOffsetTarget.z = 0.0f;

    if (calcSlope) {
        posOffsetTarget.y -=
            Camera_CalcSlopeYAdj(&camera->floorNorm, camera->focalActorPosRot.rot.y, eyeAtDir->yaw, 25.0f);
    }

    Camera_ScaledStepToCeilVec3f(&posOffsetTarget, &camera->focalActorAtOffset, camera->xzOffsetUpdateRate,
                                 camera->yOffsetUpdateRate, 0.1f);

    atTarget.x = camera->focalActorAtOffset.x + horsePosRot.pos.x;
    atTarget.y = camera->focalActorAtOffset.y + horsePosRot.pos.y;
    atTarget.z = camera->focalActorAtOffset.z + horsePosRot.pos.z;
    Camera_ScaledStepToCeilVec3f(&atTarget, at, camera->atLerpStepScale, camera->atLerpStepScale, 0.2f);

    return true;
}

f32 Camera_ClampDist1(Camera* camera, f32 dist, f32 minDist, f32 maxDist, s16 timer) {
    f32 distTarget;

    if ((dist / maxDist) > 1.2f) {
        distTarget = maxDist;

        camera->rUpdateRateInv = 20.0f / (dist / maxDist);
        if ((20.0f / (dist / maxDist)) < 10) {
            camera->rUpdateRateInv = 10;
        }
    } else if (dist < minDist) {
        distTarget = minDist;

        camera->rUpdateRateInv =
            Camera_ScaledStepToCeilF((timer != 0) ? 10.0f : 20.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
    } else if (maxDist < dist) {
        distTarget = maxDist;

        camera->rUpdateRateInv =
            Camera_ScaledStepToCeilF((timer != 0) ? 10.0f : 20.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
    } else {
        distTarget = dist;

        camera->rUpdateRateInv =
            Camera_ScaledStepToCeilF((timer != 0) ? 20.0f : 1.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
    }

    return Camera_ScaledStepToCeilF(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 0.1f);
}

f32 Camera_ClampDist2(Camera* camera, f32 dist, f32 minDist, f32 maxDist, s16 timer) {
    f32 distTarget;

    if (timer == 0) {
        distTarget = ((maxDist * 0.25f) > 80.0f) ? maxDist * 0.25f : 80.0f;

        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(1000.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
    } else if ((dist / maxDist) > 1.2f) {
        distTarget = maxDist;

        camera->rUpdateRateInv = 20.0f / (dist / maxDist);
        if ((20.0f / (dist / maxDist)) < 10.0f) {
            camera->rUpdateRateInv = 10.0f;
        }
    } else if (dist < minDist) {
        distTarget = minDist;

        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
    } else if (maxDist < dist) {
        distTarget = maxDist;

        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
    } else {
        distTarget = dist;

        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(1.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
    }

    return Camera_ScaledStepToCeilF(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 0.1f);
}

/**
 * Calculates the camera pitch to update to as player moves around
 * The pitch target starts with a default value on a flat surface, then standing on a sloped surface will create an
 * offset in default pitch Camera "AI" for pitch
 */
s16 Camera_CalcDefaultPitch(Camera* camera, s16 pitch, s16 flatSurfacePitchTarget, s16 slopePitchAdj) {
    f32 attenuation;
    f32 pitchStepScale;
    f32 t;
    s16 slopePitchAdjAttenuated;
    s16 pitchMag = ABS(pitch);
    s16 pitchTarget;

    // if slopePitchAdj is positive, then it is attenuated by a factor of Math_CosS(slopePitchAdj)
    slopePitchAdjAttenuated =
        (slopePitchAdj > 0) ? TRUNCF_BINANG(Math_CosS(slopePitchAdj) * slopePitchAdj) : slopePitchAdj;
    pitchTarget = flatSurfacePitchTarget - slopePitchAdjAttenuated;

    if (ABS(pitchTarget) < pitchMag) {
        // pitch is decreasing
        pitchStepScale = (1.0f / camera->pitchUpdateRateInv) * 3.0f;
    } else {
        // pitch is increasing
        t = pitchMag * (1.0f / DEG_TO_BINANG(79.655f));
        attenuation =
            Camera_QuadraticAttenuation(0.8f, 1.0f - t); // attenuation starts above pitch = 0xB54 (16 degrees)
        pitchStepScale = (1.0f / camera->pitchUpdateRateInv) * attenuation;
    }

    return Camera_ScaledStepToCeilS(pitchTarget, pitch, pitchStepScale, 5);
}

/**
 * Calculates the camera pitch to update to as player moves around
 * The yaw target starts with a default value, but will only update to that target yaw in proportion to players velocity
 * No velocity means yaw does not update
 * Camera "AI" for yaw
 */
s16 Camera_CalcDefaultYaw(Camera* camera, s16 yaw, s16 target, f32 attenuationYawDiffRange,
                          f32 attenuationYawDiffInterpParam) {
    f32 attenuationYawDiffAdj;
    f32 attenuationYawDiff;
    f32 attenuationYawDiffParam;
    f32 attenuationSpeedRatio;
    f32 yawUpdRate;
    s16 yawDiffToTarget;

    if (camera->xzSpeed > 0.001f) {
        yawDiffToTarget = target - BINANG_ROT180(yaw);
        attenuationYawDiffParam = BINANG_ROT180(yawDiffToTarget) / (f32)0x8000;
    } else {
        yawDiffToTarget = target - BINANG_ROT180(yaw);
        attenuationYawDiffParam = 0.3f;
    }

    // Attenuation 1 based on YawDiffToTarget.
    attenuationYawDiff = Camera_QuadraticAttenuation(attenuationYawDiffRange, attenuationYawDiffParam);
    attenuationYawDiffAdj = LERPIMP(attenuationYawDiff, 1.0f, attenuationYawDiffInterpParam);
    attenuationYawDiffAdj = CLAMP_MIN(attenuationYawDiffAdj, 0.0f);

    // attenuation 2 based on SpeedRatio
    attenuationSpeedRatio = Camera_QuadraticAttenuation(0.5f, camera->speedRatio);

    yawUpdRate = 1.0f / camera->yawUpdateRateInv;
    return yaw + TRUNCF_BINANG(yawDiffToTarget * attenuationYawDiffAdj * attenuationSpeedRatio * yawUpdRate);
}

void Camera_CalcDefaultSwing(Camera* camera, VecGeo* arg1, VecGeo* arg2, f32 arg3, f32 arg4, SwingAnimation* swing2,
                             s16* flags) {
    SwingAnimation* swing = swing2;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f peekAroundPoint;
    s32 sp8C = 0;
    f32 sp88;
    s32 checkEyeBit1;
    s32 checkEyeBit2;
    CameraCollision sp58;
    VecGeo sp50;
    Vec3f* eyeNext = &camera->eyeNext;

    if (swing->unk_64 == 1) {
        if (arg3 < (sp88 = OLib_Vec3fDist(at, &swing->collisionClosePoint))) {
            swing->unk_64 = 0;
        } else if ((sp88 = Math3D_PlaneF(swing->eyeAtColChk.norm.x, swing->eyeAtColChk.norm.y,
                                         swing->eyeAtColChk.norm.z, swing->eyeAtColChk.poly->dist, at)) > 0.0f) {
            swing->unk_64 = 0;
        } else if ((sp88 = OLib_Vec3fDist(eye, &swing->eyeAtColChk.pos)) < 10.0f) {
            swing->unk_64 = 0;
        } else if ((sp88 = Math3D_PlaneF(swing->atEyeColChk.norm.x, swing->atEyeColChk.norm.y,
                                         swing->atEyeColChk.norm.z, swing->atEyeColChk.poly->dist, eye)) > 0.0f) {
            swing->unk_64 = 0;
        } else if (swing->atEyeColChk.norm.y > 0.50f) {
            swing->unk_64 = 0;
        } else {
            Math3D_CosOut(&camera->unk_0F0, &swing->eyeAtColChk.norm, &sp88);
            if (sp88 > 0.0f) {
                swing->unk_64 = 0;
            }
        }

        if (swing->unk_64 == 1) {
            sp8C = 2;
        }
    } else if (swing->unk_64 == 2) {
        swing->unk_64 = 0;
    }

    if (sp8C == 0) {
        if (*flags & 2) {
            checkEyeBit1 = 1 << 1;
        } else {
            checkEyeBit1 = 0;
        }

        if (swing->unk_64 != 1) {
            checkEyeBit2 = 1 << 0;
        } else {
            checkEyeBit2 = 0;
        }

        sp8C =
            func_800CD44C(camera, arg1, &swing->atEyeColChk, &swing->eyeAtColChk, ((s16)checkEyeBit2 | checkEyeBit1));
    }

    switch (sp8C) {
        case 1:
            swing->collisionClosePoint =
                Camera_BgCheckCorner(&camera->at, &camera->eyeNext, &swing->atEyeColChk, &swing->eyeAtColChk);
            // fallthrough
        case 2:
            peekAroundPoint.x = swing->collisionClosePoint.x + (swing->atEyeColChk.norm.x + swing->eyeAtColChk.norm.x);
            peekAroundPoint.y = swing->collisionClosePoint.y + (swing->atEyeColChk.norm.y + swing->eyeAtColChk.norm.y);
            peekAroundPoint.z = swing->collisionClosePoint.z + (swing->atEyeColChk.norm.z + swing->eyeAtColChk.norm.z);

            sp50 = OLib_Vec3fDiffToVecGeo(at, &peekAroundPoint);
            sp50.r = arg1->r;
            swing->unk_64 = 1;
            swing->swingUpdateRate = 1.5f;
            sp58.pos = OLib_AddVecGeoToVec3f(at, &sp50);

            if (func_800CBC84(camera, &swing->eyeAtColChk.pos, &sp58, 0) == 0) {
                sp50.yaw =
                    Camera_AngleDiffAndScale(arg1->yaw, arg2->yaw, (camera->speedRatio * 0.5f) + 0.5f) + arg2->yaw;
                sp50.pitch = Camera_AngleDiffAndScale(arg1->pitch, arg2->pitch, (camera->speedRatio * 0.5f) + 0.5f) +
                             arg2->pitch;
                if (swing->atEyeColChk.geoNorm.pitch < 0x2AA8) { // 60 degrees
                    swing->yaw = sp50.yaw;
                    swing->pitch = sp50.pitch;
                } else {
                    swing->yaw = arg1->yaw;
                    swing->pitch = arg1->pitch;
                }
            }
            *eye = *eyeNext = sp58.pos;
            break;

        case 3:
        case 6:
            eyeNext = &swing->atEyeColChk.pos;
            sp88 = OLib_Vec3fDist(at, eyeNext);
            if (sp88 < 60.0f) {
                sp50.yaw = Camera_AngleDiffAndScale(arg1->yaw, arg2->yaw, camera->speedRatio) + arg2->yaw;
                if (*flags & 0x1000) {
                    sp50.pitch = arg2->pitch;
                } else {
                    sp50.pitch = Math_CosS(swing->atEyeColChk.geoNorm.pitch) * 0x3FFC;
                }
                sp50.r = 60.0f - sp88;
                sp58.pos = OLib_AddVecGeoToVec3f(eyeNext, &sp50);
            } else {
                sp50.yaw = Camera_AngleDiffAndScale(arg1->yaw, arg2->yaw, camera->speedRatio) + arg2->yaw;
                sp50.pitch = Camera_AngleDiffAndScale(arg1->pitch, arg2->pitch, camera->speedRatio) + arg2->pitch;
                sp50.r = arg1->r;
                sp58.pos = OLib_AddVecGeoToVec3f(at, &sp50);
            }
            func_800CBC84(camera, at, &sp58, 0);
            *eye = sp58.pos;
            break;

        default:
            eyeNext = &swing->atEyeColChk.pos;
            *flags &= ~0x1000;
            swing->swingUpdateRate = arg4;
            *eye = *eyeNext;
            break;
    }
}

/*===============================================================*/
/*     Camera Update Functions (Chosen by Settings & Modes)      */
/*===============================================================*/

s32 Camera_Noop(Camera* camera) {
    return 1;
}

s32 Camera_Normal1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f spD8;
    f32 spD4;
    f32 spD0;
    Vec3f* cameraDriftActorPos;
    f32 spC8;
    f32 spC4;
    f32 spC0;
    f32 phi_f0_4;
    VecGeo spB4;
    VecGeo spAC;
    VecGeo spA4;
    VecGeo sp9C;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Normal1ReadOnlyData* roData = &camera->paramData.norm1.roData;
    Normal1ReadWriteData* rwData = &camera->paramData.norm1.rwData;
    s16 phi_v1_2;
    s16 temp_a0_3;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
    f32 phi_f2;
    f32 rand;

    roData->unk_00 =
        GET_NEXT_RO_DATA(values) * (focalActorHeight * 0.01f * (0.8f - ((68.0f / focalActorHeight) * -0.2f)));
    roData->unk_04 =
        GET_NEXT_RO_DATA(values) * (focalActorHeight * 0.01f * (0.8f - ((68.0f / focalActorHeight) * -0.2f)));
    roData->unk_08 =
        GET_NEXT_RO_DATA(values) * (focalActorHeight * 0.01f * (0.8f - ((68.0f / focalActorHeight) * -0.2f)));
    roData->unk_04 = roData->unk_08 - (roData->unk_08 - roData->unk_04);

    if (RELOAD_PARAMS(camera)) {
        roData->unk_20 = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_0C = 40.0f - (40.0f - roData->unk_0C);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values) * 0.01f;
        roData->unk_14 = 1.0f - (1.0f - roData->unk_14);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_RO_DATA(values) * 0.01f;
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    spA4 = OLib_Vec3fDiffToVecGeo(at, eye);
    sp9C = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    switch (camera->animState) {
        case 20:
            Camera_SetUpdateRatesFastYaw(camera);
            // fallthrough
        case 0:
            rwData->unk_0C = 1;
            if (!(roData->interfaceFlags & NORMAL1_FLAG_3) && (camera->animState != 20)) {
                rwData->unk_0C |= 0x1000;
            }
            // fallthrough
        case 10:
            if (camera->animState == 10) {
                rwData->unk_0C = 0;
            }
            rwData->unk_08 = 0;
            D_801EDC30[camera->camId].yaw = D_801EDC30[camera->camId].pitch = D_801EDC30[camera->camId].unk_64 = 0;
            rwData->unk_0A = 0x514;
            D_801EDC30[camera->camId].swingUpdateRate = roData->unk_0C;
            rwData->unk_00 = focalActorPosRot->pos.y;
            rwData->unk_04 = camera->xzSpeed;
            D_801EDC30[camera->camId].timer = 0;
            sUpdateCameraDirection = false;
            rwData->unk_10 = 120.0f;
            break;

        default:
            break;
    }

    camera->animState = 1;
    sUpdateCameraDirection = true;

    if ((camera->speedRatio < 0.01f) || (rwData->unk_0A > 0x4B0)) {
        if (rwData->unk_0A > -0x4B0) {
            rwData->unk_0A--;
        }
    } else {
        rwData->unk_0A = 0x4B0;
    }

    if (func_800CB950(camera)) {
        rwData->unk_00 = focalActorPosRot->pos.y;
    }

    if (rwData->unk_0C & 0x1000) {
        spC8 = camera->speedRatio;
    } else {
        spC8 = ((camera->speedRatio * 3.0f) + 1.0f) * 0.25f;
    }

    spD8 = camera->focalActorAtOffset;
    spD8.y -= focalActorHeight + roData->unk_00;
    spC4 = Camera_Vec3fMagnitude(&spD8);

    if (spC4 > (roData->unk_04 + roData->unk_08)) {
        spC4 = 1.0f;
    } else {
        spC4 /= roData->unk_04 + roData->unk_08;
    }

    spD0 = 0.2f;

    phi_f0_4 = (camera->xzSpeed - rwData->unk_04) * (0.2f * 1.0f);
    if (phi_f0_4 < 0.0f) {
        phi_f0_4 = 0.0f;
    }

    spC0 = OLib_ClampMaxDist(SQ(phi_f0_4), 1.0f);

    camera->yOffsetUpdateRate =
        Camera_ScaledStepToCeilF(0.05f, camera->yOffsetUpdateRate, (0.5f * spC8) + (0.5f * spC4), 0.0001f);
    camera->xzOffsetUpdateRate =
        Camera_ScaledStepToCeilF(0.05f, camera->xzOffsetUpdateRate, (0.5f * spC8) + (0.5f * spC4), 0.0001f);
    camera->fovUpdateRate =
        Camera_ScaledStepToCeilF(0.05f, camera->fovUpdateRate, (0.5f * spC8) + (0.5f * spC4), 0.0001f);

    if (D_801EDC30[camera->camId].unk_64 == 1) {
        phi_f2 = 0.5f;
    } else {
        phi_f2 = (0.5f * spC8) + (0.5f * spC4);
    }

    rwData->unk_04 = camera->xzSpeed;

    if (D_801EDC30[camera->camId].timer != 0) {
        camera->yawUpdateRateInv =
            Camera_ScaledStepToCeilF(D_801EDC30[camera->camId].swingUpdateRate + (D_801EDC30[camera->camId].timer * 2),
                                     camera->yawUpdateRateInv, phi_f2, 0.1f);
        if (roData->interfaceFlags & NORMAL1_FLAG_3) {
            camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(100.0f, camera->pitchUpdateRateInv, 0.5f, 0.1f);
        } else {
            camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF((D_801EDC30[camera->camId].timer * 2) + 16.0f,
                                                                  camera->pitchUpdateRateInv, 0.2f, 0.1f);
        }
        D_801EDC30[camera->camId].timer--;
    } else {
        camera->yawUpdateRateInv = Camera_ScaledStepToCeilF(
            D_801EDC30[camera->camId].swingUpdateRate - (D_801EDC30[camera->camId].swingUpdateRate * 0.7f * spC0),
            camera->yawUpdateRateInv, phi_f2, 0.1f);
        if ((roData->interfaceFlags & NORMAL1_FLAG_3) && (camera->speedRatio > 0.01f)) {
            camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(100.0f, camera->pitchUpdateRateInv, 0.5f, 0.1f);
        } else if (gCameraDriftActor != NULL) {
            camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(32.0f, camera->pitchUpdateRateInv, 0.5f, 0.1f);
        } else {
            camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(16.0f, camera->pitchUpdateRateInv, 0.2f, 0.1f);
        }
    }

    if (roData->interfaceFlags & NORMAL1_FLAG_0) {
        //! FAKE:
        if (spC8) {}

        temp_a0_3 = Camera_GetPitchAdjFromFloorHeightDiffs(camera, spA4.yaw + 0x8000, rwData->unk_0C & 1);
        phi_f2 = (1.0f / roData->unk_10) * 0.7f;
        spD0 = (1.0f / roData->unk_10) * 0.3f * (1.0f - camera->speedRatio);
        rwData->unk_08 = Camera_ScaledStepToCeilS(temp_a0_3, rwData->unk_08, phi_f2 + spD0, 5);
    } else {
        rwData->unk_08 = 0;
    }

    if ((D_801EDC30[camera->camId].unk_64 == 1) && (roData->unk_00 > -40.0f)) {
        spD0 = Math_SinS(D_801EDC30[camera->camId].pitch);
        phi_f2 = (-40.0f * spD0) + roData->unk_00 * (1.0f - spD0);
        camera->yawUpdateRateInv = 80.0f;
        camera->pitchUpdateRateInv = 80.0f;
    } else {
        phi_f2 = roData->unk_00;
    }

    if (roData->interfaceFlags & (NORMAL1_FLAG_6 | NORMAL1_FLAG_5)) {
        if (camera->dist < roData->unk_04) {
            spD0 = 0.0f;
        } else if (roData->unk_08 < camera->dist) {
            spD0 = 1.0f;
        } else if (roData->unk_08 == roData->unk_04) {
            spD0 = 1.0f;
        } else {
            spD0 = (camera->dist - roData->unk_04) / (roData->unk_08 - roData->unk_04);
        }

        Camera_CalcAtForNormal1(camera, &sp9C, phi_f2, 25.0f * spD0 * camera->speedRatio);
        rwData->unk_10 = 120.0f;
    } else if ((roData->interfaceFlags & NORMAL1_FLAG_7) && (rwData->unk_0A < 0)) {
        phi_f0_4 = rwData->unk_0A / -1200.0f;
        Camera_CalcAtForNormal1(
            camera, &sp9C,
            phi_f2 - ((phi_f2 - ((0.8f - ((68.0f / focalActorHeight) * -0.2f)) * focalActorHeight * -0.45f)) *
                      phi_f0_4 * 0.75f),
            10.0f * phi_f0_4);
        rwData->unk_10 = 120.0f;
    } else if (roData->interfaceFlags & NORMAL1_FLAG_3) {
        Camera_CalcAtForScreen(camera, &sp9C, roData->unk_00, &rwData->unk_00, rwData->unk_10);
        if (rwData->unk_10 > 20.0f) {
            rwData->unk_10 -= 0.2f;
        }
    } else {
        Camera_CalcAtDefault(camera, &sp9C, phi_f2, roData->interfaceFlags & NORMAL1_FLAG_0);
        rwData->unk_10 = 120.0f;
    }

    spB4 = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    if ((roData->interfaceFlags & NORMAL1_FLAG_7) && (rwData->unk_0A < 0)) {
        if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
            switch (((Player*)camera->focalActor)->transformation) {
                case PLAYER_FORM_HUMAN:
                    spD0 = 66.0f;
                    break;

                case PLAYER_FORM_DEKU:
                    spD0 = 66.0f;
                    break;

                case PLAYER_FORM_GORON:
                    spD0 = 115.0f;
                    break;

                case PLAYER_FORM_ZORA:
                    spD0 = 115.0f;
                    break;

                case PLAYER_FORM_FIERCE_DEITY:
                    spD0 = roData->unk_04;
                    break;

                default:
                    spD0 = roData->unk_04;
                    break;
            }
        }
        phi_f0_4 = Camera_ClampDist2(camera, spB4.r, spD0, spD0, 0);
    } else if (roData->interfaceFlags & NORMAL1_FLAG_7) {
        phi_f0_4 = Camera_ClampDist2(camera, spB4.r, roData->unk_04, roData->unk_08, 1);
    } else {
        phi_f0_4 = Camera_ClampDist1(camera, spB4.r, roData->unk_04, roData->unk_08, rwData->unk_0A > 0);
    }

    camera->dist = spB4.r = phi_f0_4;

    if (D_801EDC30[camera->camId].unk_64 != 0) {
        spB4.pitch =
            Camera_ScaledStepToCeilS(D_801EDC30[camera->camId].pitch, sp9C.pitch, 1.0f / camera->yawUpdateRateInv, 5);
        spB4.yaw =
            Camera_ScaledStepToCeilS(D_801EDC30[camera->camId].yaw, sp9C.yaw, 1.0f / camera->yawUpdateRateInv, 5);
    } else if (roData->interfaceFlags & NORMAL1_FLAG_5) {
        spB4.yaw = sp9C.yaw;
        spB4.pitch = sp9C.pitch;
        camera->animState = 20;
    } else if (gCameraDriftActor != NULL) {
        VecGeo sp74;
        s16 sp72;
        f32 sp6C;

        //! FAKE:
        if (1) {}

        cameraDriftActorPos = &gCameraDriftActor->world.pos;
        sp74 = OLib_Vec3fDiffToVecGeo(&focalActorPosRot->pos, cameraDriftActorPos);
        sp72 = focalActorPosRot->rot.y - sp74.yaw;
        // Interface and shrink-window flags
        if ((roData->interfaceFlags & 0xFF00) == 0xFF00) {
            sp6C = 1.0f;
        } else {
            sp6C = 1.0f - (ABS(sp72) / 10922.0f);
        }

        if (ABS((s16)(sp9C.yaw - sp74.yaw)) < 0x4000) {
            sp74.yaw += 0x8000;
        }

        if (!(roData->interfaceFlags & NORMAL1_FLAG_3) || !func_800CB924(camera)) {
            spB4.yaw = Camera_CalcDefaultYaw(
                camera, sp9C.yaw, (s16)(focalActorPosRot->rot.y - TRUNCF_BINANG(sp72 * sp6C)), roData->unk_14, spC0);
        }

        if (!(roData->interfaceFlags & NORMAL1_FLAG_3) || (camera->speedRatio < 0.01f)) {
            spB4.pitch = Camera_CalcDefaultPitch(
                camera, sp9C.pitch, roData->unk_20 + TRUNCF_BINANG((roData->unk_20 - sp74.pitch) * sp6C * 0.75f),
                rwData->unk_08);
        }
    } else if (roData->interfaceFlags & NORMAL1_FLAG_1) {
        VecGeo sp64;

        if ((camera->speedRatio > 0.1f) || (rwData->unk_0A > 0x4B0)) {
            sp64 = OLib_Vec3fToVecGeo(&camera->unk_0F0);
            if (!(roData->interfaceFlags & NORMAL1_FLAG_3) || !func_800CB924(camera)) {
                spB4.yaw = Camera_CalcDefaultYaw(camera, sp9C.yaw, sp64.yaw, roData->unk_14, spC0);
            }
            if (!(roData->interfaceFlags & NORMAL1_FLAG_3)) {
                spB4.pitch = Camera_CalcDefaultPitch(camera, sp9C.pitch, roData->unk_20, rwData->unk_08);
            } else if ((camera->unk_0F0.y > 0.0f) && func_800CB924(camera)) {
                spB4.pitch = Camera_CalcDefaultPitch(camera, sp9C.pitch, roData->unk_20, rwData->unk_08);
            }
        } else {
            spB4.yaw = sp9C.yaw;
            spB4.pitch = sp9C.pitch;
        }
    } else {
        spB4.yaw = Camera_CalcDefaultYaw(camera, sp9C.yaw, focalActorPosRot->rot.y, roData->unk_14, spC0);
        if (!(roData->interfaceFlags & NORMAL1_FLAG_3) || (camera->speedRatio < 0.1f)) {
            spB4.pitch = Camera_CalcDefaultPitch(camera, sp9C.pitch, roData->unk_20, rwData->unk_08);
        }
    }

    // 76.9 degrees
    if (spB4.pitch > 0x36B0) {
        spB4.pitch = 0x36B0;
    }

    // -76.9 degrees
    if (spB4.pitch < -0x36B0) {
        spB4.pitch = -0x36B0;
    }

    *eyeNext = OLib_AddVecGeoToVec3f(at, &spB4);

    if ((camera->status == CAM_STATUS_ACTIVE) && !(roData->interfaceFlags & NORMAL1_FLAG_4) && (spC4 <= 0.9f)) {
        if (!func_800CBA7C(camera)) {
            CollisionPoly* sp60;
            s32 sp5C; // bgId
            f32 sp58;

            Camera_CalcDefaultSwing(camera, &spB4, &sp9C, roData->unk_04, roData->unk_0C, &D_801EDC30[camera->camId],
                                    &rwData->unk_0C);
            sp58 = BgCheck_CameraRaycastFloor2(&camera->play->colCtx, &sp60, &sp5C, eye);
            if ((roData->interfaceFlags & NORMAL1_FLAG_3) && func_800CB924(camera)) {
                spD0 = 25.0f;
            } else {
                spD0 = 5.0f;
            }

            phi_f2 = eye->y - sp58;
            if ((sp58 != BGCHECK_Y_MIN) && (phi_f2 < spD0)) {
                eye->y = sp58 + spD0;
            } else if ((camera->waterYPos != camera->focalActorFloorHeight) && ((eye->y - camera->waterYPos) < 5.0f) &&
                       ((eye->y - camera->waterYPos) > -5.0f)) {
                eye->y = camera->waterYPos + 5.0f;
            }
        }

        spAC = OLib_Vec3fDiffToVecGeo(eye, at);
        camera->inputDir.x = spAC.pitch;
        camera->inputDir.y = spAC.yaw;
        camera->inputDir.z = 0;

        // crit wiggle
        if (gSaveContext.save.saveInfo.playerData.health <= 0x10) {
            phi_v1_2 = ((s32)(camera->play->state.frames << 0x18) >> 0x15) & 0xFD68;
            camera->inputDir.y += phi_v1_2;
        }
    } else {
        D_801EDC30[camera->camId].swingUpdateRate = roData->unk_0C;
        D_801EDC30[camera->camId].unk_64 = 0;
        sUpdateCameraDirection = false;
        *eye = *eyeNext;
    }

    phi_f2 = (gSaveContext.save.saveInfo.playerData.health <= 0x10) ? 0.8f : 1.0f;
    camera->fov = Camera_ScaledStepToCeilF(roData->unk_18 * phi_f2, camera->fov, camera->fovUpdateRate, 0.1f);

    if (roData->interfaceFlags & NORMAL1_FLAG_2) {
        spD4 = Math_SinS((s16)(spA4.yaw - spB4.yaw));
        rand = Rand_ZeroOne() - 0.5f;
        camera->roll = Camera_ScaledStepToCeilS((rand * 500.0f * camera->speedRatio) + (spD4 * spD4 * spD4 * 10000.0f),
                                                camera->roll, 0.1f, 5);
    } else {
        if (gSaveContext.save.saveInfo.playerData.health <= 0x10) {
            rand = Rand_ZeroOne() - 0.5f;
            phi_v1_2 = rand * 100.0f * camera->speedRatio;
        } else {
            phi_v1_2 = 0.0f;
        }
        camera->roll = Camera_ScaledStepToCeilS(phi_v1_2, camera->roll, 0.2f, 5);
    }

    camera->atLerpStepScale = Camera_ClampLerpScale(camera, roData->unk_1C);
    rwData->unk_0C &= ~1;

    return true;
}

/**
 * Unused Camera RemoteBomb Setting
 */
s32 Camera_Normal2(Camera* camera) {
    return Camera_Noop(camera);
}

#define NORMAL3_RW_FLAG (1 << 0)

/**
 * Riding Epona and Zora
 */
s32 Camera_Normal3(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    f32 sp94;
    f32 sp90;
    f32 temp_f2;
    f32 sp88;
    VecGeo sp80;
    VecGeo sp78;
    VecGeo sp70;
    VecGeo sp68;
    f32 phi_f2;
    s16 sp62;
    s16 phi_v1_2;
    Player* player = (Player*)camera->focalActor;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Normal3ReadOnlyData* roData = &camera->paramData.norm3.roData;
    Normal3ReadWriteData* rwData = &camera->paramData.norm3.rwData;

    temp_f2 = Camera_GetFocalActorHeight(camera);

    if ((camera->setting == CAM_SET_HORSE) && (player->rideActor == NULL)) {
        Camera_ChangeSettingFlags(camera, camera->prevSetting, CAM_CHANGE_SETTING_1);
        return 1;
    }

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->yOffset = GET_NEXT_RO_DATA(values) * CAM_RODATA_UNSCALE(temp_f2);
        roData->distMin = GET_NEXT_RO_DATA(values) * CAM_RODATA_UNSCALE(temp_f2);
        roData->distMax = GET_NEXT_RO_DATA(values) * CAM_RODATA_UNSCALE(temp_f2);
        roData->pitchTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->yawUpdateRateInv = GET_NEXT_RO_DATA(values);
        roData->pitchUpdateRateInv = GET_NEXT_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->maxAtLERPScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sp70 = OLib_Vec3fDiffToVecGeo(at, eye);
    sp68 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    sUpdateCameraDirection = true;
    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
        case 10:
        case 20:
            rwData->isZero = 0;
            rwData->curPitch = 0;
            rwData->yPosOffset = camera->focalActorFloorHeight;

            D_801EDC30[camera->camId].yaw = D_801EDC30[camera->camId].pitch = D_801EDC30[camera->camId].unk_64 = 0;
            D_801EDC30[camera->camId].swingUpdateRate = roData->yawUpdateRateInv;
            rwData->yawUpdateRate = BINANG_SUB(BINANG_ROT180(camera->focalActorPosRot.rot.y), sp70.yaw) * (1.0f / 6.0f);
            rwData->distTimer = 0;
            rwData->is1200 = 1200;

            if (roData->interfaceFlags & NORMAL3_FLAG_1) {
                rwData->yawTimer = 6;
                Camera_SetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
            } else {
                rwData->yawTimer = 0;
            }

            camera->animState = 1;
            D_801EDC30[camera->camId].timer = 0;
            rwData->flag = NORMAL3_RW_FLAG;
            break;

        default:
            break;
    }

    if (rwData->distTimer != 0) {
        rwData->distTimer--;
    }

    sp90 = ((camera->speedRatio * 3.0f) + 1.0f) * 0.25f * 0.5f;
    sp94 = temp_f2 = camera->speedRatio * 0.2f;

    if (D_801EDC30[camera->camId].timer != 0) {
        camera->yawUpdateRateInv = Camera_ScaledStepToCeilF(
            (D_801EDC30[camera->camId].timer * 2) + roData->yawUpdateRateInv, camera->yawUpdateRateInv, sp90, 0.1f);
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF((D_801EDC30[camera->camId].timer * 2) + 16.0f,
                                                              camera->pitchUpdateRateInv, sp94, 0.1f);
        D_801EDC30[camera->camId].timer--;
    } else {
        camera->yawUpdateRateInv =
            Camera_ScaledStepToCeilF(roData->yawUpdateRateInv, camera->yawUpdateRateInv, sp90, 0.1f);
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(16.0f, camera->pitchUpdateRateInv, sp94, 0.1f);
    }

    camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->yOffsetUpdateRate, sp90, 0.001f);
    camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->xzOffsetUpdateRate, sp94, 0.0001f);
    camera->fovUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->fovUpdateRate, sp94, 0.0001f);

    phi_v1_2 = Camera_GetPitchAdjFromFloorHeightDiffs(camera, BINANG_ROT180(sp70.yaw), rwData->flag & NORMAL3_RW_FLAG);
    temp_f2 = ((1.0f / roData->pitchUpdateRateInv) * 0.5f) * (1.0f - camera->speedRatio);
    rwData->curPitch =
        Camera_ScaledStepToCeilS(phi_v1_2, rwData->curPitch, ((1.0f / roData->pitchUpdateRateInv) * 0.5f) + temp_f2, 5);

    if ((roData->interfaceFlags & NORMAL3_FLAG_6) || (player->rideActor == NULL)) {
        Camera_CalcAtDefault(camera, &sp68, roData->yOffset, 1);
    } else {
        Camera_CalcAtForHorse(camera, &sp68, roData->yOffset, &rwData->yPosOffset, 1);
    }

    sp88 = (roData->distMax + roData->distMin) * 0.5f;
    sp80 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    temp_f2 = Camera_ClampDist1(camera, sp80.r, roData->distMin, roData->distMax, rwData->distTimer);

    phi_f2 = sp88 - temp_f2;
    phi_f2 *= 0.002f;
    camera->dist = sp80.r = temp_f2 + phi_f2;

    if (roData->interfaceFlags & NORMAL3_FLAG_7) {
        sp80.pitch = Camera_ScaledStepToCeilS(camera->focalActor->focus.rot.x - rwData->curPitch, sp68.pitch, 0.25f, 5);
    } else {
        sp62 = roData->pitchTarget - rwData->curPitch;
        sp80.pitch = Camera_ScaledStepToCeilS(sp62, sp68.pitch, 1.0f / camera->pitchUpdateRateInv, 5);
    }

    sp80.pitch = CLAMP_MAX(sp80.pitch, DEG_TO_BINANG(79.655f));

    if (sp80.pitch < -DEG_TO_BINANG(29.995f)) {
        sp80.pitch = -DEG_TO_BINANG(29.995f);
    }

    if (roData->interfaceFlags & NORMAL3_FLAG_7) {
        sp62 = BINANG_SUB(camera->focalActor->focus.rot.y, BINANG_ROT180(sp68.yaw));
        temp_f2 = 1.0f;
    } else {
        sp62 = BINANG_SUB(focalActorPosRot->rot.y, BINANG_ROT180(sp68.yaw));
        sp78 = OLib_Vec3fToVecGeo(&camera->unk_0F0);
        phi_v1_2 = focalActorPosRot->rot.y - sp78.yaw;
        phi_v1_2 = ABS_ALT(phi_v1_2);

        if (phi_v1_2 < 0x555A) {
            temp_f2 = 1.0f;
        } else {
            temp_f2 = ((f32)0x8000 - phi_v1_2) / (f32)0x2AA6;
        }
    }

    sp90 = (sp62 * ((SQ(camera->speedRatio) * 0.8f) + 0.2f) * temp_f2) / camera->yawUpdateRateInv;
    if ((Camera_fabsf(sp90) > 150.0f) && (camera->speedRatio > 0.05f)) {
        sp80.yaw = sp68.yaw + sp90;
    }

    if (rwData->yawTimer > 0) {
        sp80.yaw += rwData->yawUpdateRate;
        rwData->yawTimer--;
        if (rwData->yawTimer == 0) {
            Camera_UnsetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
        }
    }

    *eyeNext = OLib_AddVecGeoToVec3f(at, &sp80);

    if (camera->status == CAM_STATUS_ACTIVE) {
        *eye = *eyeNext;
        func_800CBFA4(camera, at, eye, 0);
    } else {
        *eye = *eyeNext;
    }

    camera->fov = Camera_ScaledStepToCeilF(roData->fovTarget, camera->fov, camera->fovUpdateRate, 0.1f);

    if (roData->interfaceFlags & NORMAL3_FLAG_5) {
        camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.05f, 5);
    } else {
        camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.1f, 5);
    }

    camera->atLerpStepScale = Camera_ClampLerpScale(camera, roData->maxAtLERPScale);
    rwData->flag &= ~NORMAL3_RW_FLAG;

    return 1;
}

/**
 * Used for the unknown Naname setting.
 * Identical to Normal1 except reads camera scene data to apply a camera roll
 */
s32 Camera_Normal4(Camera* camera) {
    BgCamFuncData* bgCamFuncData;
    s16 roll;

    if (RELOAD_PARAMS(camera)) {
        bgCamFuncData = (BgCamFuncData*)Camera_GetBgCamOrActorCsCamFuncData(camera, camera->bgCamIndex);
        D_801EDBF0 = bgCamFuncData->rot.z;
    }

    roll = camera->roll;
    Camera_Normal1(camera);
    camera->roll = Camera_ScaledStepToCeilS(D_801EDBF0, roll, 0.05f, 5);

    return 1;
}

s32 Camera_Normal0(Camera* camera) {
    f32 phi_f0;
    f32 playerHeight = Player_GetHeight((Player*)camera->focalActor);
    s32 pad;
    f32 yNormal = 0.8f - ((68.0f / playerHeight) * -0.2f);
    f32 spA4;
    f32 spA0;
    VecGeo sp98;
    VecGeo sp90;
    VecGeo sp88;
    VecGeo sp80;
    VecGeo sp78;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    s16 temp_v1_2;
    s16 phi_a1;
    s16 phi_a0;
    BgCamFuncData* bgCamFuncData;
    Normal0ReadOnlyData* roData = &camera->paramData.norm0.roData;
    Normal0ReadWriteData* rwData = &camera->paramData.norm0.rwData;
    s32 pad2;

    if (!RELOAD_PARAMS(camera)) {
    } else {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_04 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_08 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_1C = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
        case 10:
        case 20:
            bgCamFuncData = (BgCamFuncData*)Camera_GetBgCamOrActorCsCamFuncData(camera, camera->bgCamIndex);
            rwData->unk_00 = Camera_Vec3sToVec3f(&bgCamFuncData->pos);
            rwData->unk_20 = bgCamFuncData->rot.x;
            rwData->unk_22 = bgCamFuncData->rot.y;
            rwData->unk_24 = focalActorPosRot->pos.y;
            if (bgCamFuncData->fov == -1) {
                rwData->unk_1C = roData->unk_14;
            } else {
                if (bgCamFuncData->fov > 360) {
                    phi_f0 = CAM_RODATA_UNSCALE(bgCamFuncData->fov);
                } else {
                    phi_f0 = bgCamFuncData->fov;
                }
                rwData->unk_1C = phi_f0;
            }

            if (bgCamFuncData->unk_0E == -1) {
                rwData->unk_2C = 0;
            } else {
                rwData->unk_2C = bgCamFuncData->unk_0E;
            }

            rwData->unk_18 = 0.0f;
            rwData->unk_28 = 120.0f;

            if (roData->interfaceFlags & NORMAL0_FLAG_2) {
                sp88.pitch = rwData->unk_20;
                sp88.yaw = rwData->unk_22;
                sp88.r = 100.0f;
                rwData->unk_0C = OLib_VecGeoToVec3f(&sp88);
            }
            camera->animState = 1;
            camera->yawUpdateRateInv = 50.0f;
            break;

        default:
            if (func_800CB950(camera)) {
                rwData->unk_24 = focalActorPosRot->pos.y;
            }
            break;
    }

    sp80 = OLib_Vec3fDiffToVecGeo(at, eye);
    sp78 = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    camera->speedRatio *= 0.50f;
    spA4 = camera->speedRatio * 0.5f;
    spA0 = camera->speedRatio * 0.2f;

    camera->yawUpdateRateInv =
        Camera_ScaledStepToCeilF(roData->unk_0C, camera->yawUpdateRateInv * camera->speedRatio, 0.5f, 0.1f);
    camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(16.0f, camera->pitchUpdateRateInv, spA0, 0.1f);
    camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->yOffsetUpdateRate, spA4, 0.0001f);
    camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->xzOffsetUpdateRate, spA0, 0.0001f);
    camera->fovUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.0001f);

    if (!(roData->interfaceFlags & NORMAL0_FLAG_7)) {
        Camera_CalcAtDefault(camera, &sp78, roData->unk_00, roData->interfaceFlags & NORMAL0_FLAG_0);
        rwData->unk_28 = 120.0f;
    } else {
        Camera_CalcAtForScreen(camera, &sp78, roData->unk_00, &rwData->unk_24, rwData->unk_28);
        if (rwData->unk_28 > 20.0f) {
            rwData->unk_28 -= 0.2f;
        }
    }

    if (roData->interfaceFlags & NORMAL0_FLAG_2) {
        rwData->unk_00.x = focalActorPosRot->pos.x + rwData->unk_0C.x;
        rwData->unk_00.z = focalActorPosRot->pos.z + rwData->unk_0C.z;
    }

    rwData->unk_00.y = focalActorPosRot->pos.y;
    sp88 = OLib_Vec3fDiffToVecGeo(&rwData->unk_00, at);
    sp90 = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    phi_a1 = (rwData->unk_2C & 2) ? rwData->unk_22 : roData->unk_1C;
    phi_a0 = sp90.yaw - sp88.yaw;
    if (((phi_a1 <= 0x4000) && (phi_a1 < ABS(phi_a0))) || ((phi_a1 > 0x4000) && (ABS(phi_a0) < phi_a1))) {
        phi_a0 = (phi_a0 < 0) ? -phi_a1 : phi_a1;
        phi_a0 += sp88.yaw;
        sp98.yaw =
            Camera_ScaledStepToCeilS(phi_a0, sp80.yaw, (1 / camera->yawUpdateRateInv) * 1 * camera->speedRatio, 5);
        if (rwData->unk_2C & 1) {
            sp98.pitch = Camera_CalcDefaultPitch(camera, sp78.pitch, rwData->unk_20, 0);
        } else {
            sp98.pitch = sp80.pitch;
        }
    } else {
        sp98 = sp90;
    }

    camera->dist = sp98.r = Camera_ClampDist1(camera, sp90.r, roData->unk_04, roData->unk_08, 0);
    if (!(rwData->unk_2C & 1)) {
        if (sp98.pitch > 0xE38) { // 20 degrees
            sp98.pitch += ((s16)(0xE38 - sp98.pitch) >> 2);
        }
        if (sp98.pitch < 0) {
            sp98.pitch += ((s16)(-0x38E - sp98.pitch) >> 2);
        }
    }
    *eyeNext = OLib_AddVecGeoToVec3f(at, &sp98);

    *eye = *eyeNext;

    if (camera->status == CAM_STATUS_ACTIVE) {
        if ((camera->play->envCtx.skyboxDisabled == 0) || (roData->interfaceFlags & NORMAL0_FLAG_4)) {
            Camera_BgCheck(camera, at, eye);
        } else {
            func_800CBFA4(camera, at, eye, 3);
            sp98 = OLib_Vec3fDiffToVecGeo(eye, at);
            camera->inputDir.x = sp98.pitch;
            camera->inputDir.y = sp98.yaw;
            camera->inputDir.z = 0;
        }
    }

    camera->fov = Camera_ScaledStepToCeilF(rwData->unk_1C, camera->fov, camera->fovUpdateRate, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);
    camera->atLerpStepScale = Camera_ClampLerpScale(camera, roData->unk_18);

    return 1;
}

/**
 * Used for targeting
 */
s32 Camera_Parallel1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f spB0;
    Vec3f spA4;
    f32 spA0;
    f32 sp9C;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    VecGeo sp90;
    VecGeo sp88;
    VecGeo sp80;
    VecGeo sp78;
    BgCamFuncData* bgCamFuncData;
    s16 sp72;
    s16 tangle;
    Parallel1ReadOnlyData* roData = &camera->paramData.para1.roData;
    Parallel1ReadWriteData* rwData = &camera->paramData.para1.rwData;
    s32 parallelFlagCond;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    s16 new_var2;
    s16 phi_a0;
    s32 phi_a0_2;

    // #region 2S2H [Enhancement] - FixTargettingCameraSnap
    static bool prevTargetingHeld = false;
    bool isChargingDekuFlowerDive = !!(((Player*)camera->focalActor)->stateFlags3 & PLAYER_STATE3_100);
    s16 timer4 = rwData->timer2 >> 5;
    rwData->timer2 &= 0x001F;
    // #endregion

    if (!RELOAD_PARAMS(camera)) {
    } else {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->unk_00 =
            GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_04 =
            GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_08 =
            GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_20 = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_22 = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_24 = GET_NEXT_RO_DATA(values);
        rwData->unk_00 = roData->unk_04;
    }

    sp80 = OLib_Vec3fDiffToVecGeo(at, eye);
    sp78 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    spA4 = Camera_GetFocalActorPos(camera);

    switch (camera->animState) {
        case 20:
            if ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) == 0) {
                Camera_SetUpdateRatesFastYaw(camera);
            }
            // fallthrough
        case 0:
        case 10:
            if ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
                (PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) {
                rwData->unk_10 = focalActorPosRot->pos;
            } else {
                camera->xzOffsetUpdateRate = 0.5f;
                camera->yOffsetUpdateRate = 0.5f;
            }

            if ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
                PARALLEL1_FLAG_3) {
                rwData->unk_10 = camera->focalActorPosRot.pos;
            }

            rwData->timer1 = 200.0f;

            if ((2.0f * roData->unk_04) < camera->dist) {
                camera->dist = 2.0f * roData->unk_04;
                sp78.r = camera->dist;
                sp80.r = sp78.r;
                *eye = OLib_AddVecGeoToVec3f(at, &sp80);
                *eyeNext = *eye;
            }

            rwData->unk_1C = 0;

            if (roData->interfaceFlags & PARALLEL1_FLAG_2) {
                rwData->timer2 = 20;
            } else {
                rwData->timer2 = 6;
                // #region 2S2H [Enhancement] - FixTargettingCameraSnap
                timer4 = 6;
                // #endregion
            }

            if ((camera->focalActor == &GET_PLAYER(camera->play)->actor) && (camera->mode == CAM_MODE_CHARGE)) {
                rwData->timer2 = 30;
                if (((Player*)camera->focalActor)->transformation == PLAYER_FORM_DEKU) {
                    roData->unk_24 = -1;
                }
            }

            if ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
                (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1)) {
                f32 yNormal;

                rwData->timer2 = 1;
                yNormal = 0.8f - ((68.0f / focalActorHeight) * -0.2f);

                bgCamFuncData = (BgCamFuncData*)Camera_GetBgCamOrActorCsCamFuncData(camera, camera->bgCamIndex);

                rwData->unk_20 = bgCamFuncData->rot.x;
                rwData->unk_1E = bgCamFuncData->rot.y;
                rwData->unk_08 = (bgCamFuncData->fov == -1)   ? roData->unk_14
                                 : (bgCamFuncData->fov > 360) ? CAM_RODATA_UNSCALE(bgCamFuncData->fov)
                                                              : bgCamFuncData->fov;
                rwData->unk_00 = (bgCamFuncData->unk_0E == -1)
                                     ? roData->unk_04
                                     : CAM_RODATA_UNSCALE(bgCamFuncData->unk_0E) * focalActorHeight * yNormal;
            } else {
                rwData->unk_08 = roData->unk_14;
                rwData->unk_00 = roData->unk_04;
            }

            rwData->timer3 = roData->unk_24;
            rwData->unk_04 = focalActorPosRot->pos.y - camera->unk_0F0.y;
            rwData->unk_26 = 1;
            camera->animState = 1;
            sCameraInterfaceFlags = roData->interfaceFlags;

            // #region 2S2H [Enhancement] - FixTargettingCameraSnap
            prevTargetingHeld = false;
            // #endregion

            break;

        default:
            break;
    }

    // #region 2S2H [Enhancement] - FixTargettingCameraSnap
    if (CVarGetInteger("gEnhancements.Camera.FixTargettingCameraSnap", 0)) {
        if ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) == PARALLEL1_FLAG_1) {
            Player* player = GET_PLAYER(camera->play);
            bool targetingHeld = Player_FriendlyLockOnOrParallel(player) || player->focusActor != NULL;

            // Fix camera rotating with player if z-target gets released too fast after transition.
            if ((targetingHeld) && (!prevTargetingHeld)) {
                // Reset timer2 to avoid immediate rotation, if player presses, releases and presses z-target in a very
                // short time-window.
                rwData->timer2 = 6;
                rwData->unk_1E = BINANG_ROT180(camera->focalActorPosRot.rot.y) + roData->unk_22;
            }

            // Maintain vanilla behavior for quitting z-target.
            if ((timer4 == 0) && (!targetingHeld)) {
                rwData->timer2 = 0;
            }

            // Decrease timer4 only in cases where timer2 would be decreased.
            if ((timer4 > 0) && (rwData->timer3 <= 0)) {
                timer4--;
            }

            prevTargetingHeld = targetingHeld;
        }
    }
    // #endregion

    if (rwData->timer2 != 0) {
        switch (roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) {
            case PARALLEL1_FLAG_1:
                // #region 2S2H [Enhancement] - FixTargettingCameraSnap
                if (CVarGetInteger("gEnhancements.Camera.FixTargettingCameraSnap", 0)) {
                    if (isChargingDekuFlowerDive) {
                        rwData->unk_1E = BINANG_ROT180(camera->focalActorPosRot.rot.y) + roData->unk_22;
                    }
                    rwData->unk_20 = roData->unk_20;
                    break;
                }
                // fallthrough
                // #endregion
            case (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1):
                rwData->unk_1E = BINANG_ROT180(camera->focalActorPosRot.rot.y) + roData->unk_22;
                rwData->unk_20 = roData->unk_20;
                break;

            case PARALLEL1_FLAG_2:
                rwData->unk_1E = roData->unk_22;
                rwData->unk_20 = roData->unk_20;
                break;

            case (PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1):
                if (rwData->timer3 == 1) {
                    sp88 = OLib_Vec3fDiffToVecGeo(&rwData->unk_10, &spA4);
                    rwData->unk_1E = ((ABS(BINANG_SUB(sp88.yaw, sp80.yaw)) < 0x3A98) || Camera_IsClimbingLedge(camera))
                                         ? sp80.yaw
                                         : sp80.yaw + (s16)((BINANG_SUB(sp88.yaw, sp80.yaw) >> 2) * 3);
                }
                rwData->unk_20 = roData->unk_20;
                break;

            case PARALLEL1_FLAG_3:
                rwData->unk_1E = sp80.yaw;
                rwData->unk_20 = roData->unk_20;
                break;

            case (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1):
                break;

            default:
                rwData->unk_1E = sp78.yaw + roData->unk_22;
                rwData->unk_20 = roData->unk_20;
                break;
        }
    } else if (roData->interfaceFlags & PARALLEL1_FLAG_5) {
        rwData->unk_1E = BINANG_ROT180(camera->focalActorPosRot.rot.y) + roData->unk_22;
    }

    if (camera->animState == 21) {
        camera->animState = 1;
    } else if (camera->animState == 11) {
        camera->animState = 1;
    }

    spA0 = camera->speedRatio * 0.5f;
    sp9C = camera->speedRatio * 0.2f;

    if (((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
         (PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ||
        ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) == PARALLEL1_FLAG_3) ||
        (roData->interfaceFlags & PARALLEL1_FLAG_5)) {
        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->rUpdateRateInv, 0.5f, 0.1f);
        camera->yawUpdateRateInv = Camera_ScaledStepToCeilF(roData->unk_0C, camera->yawUpdateRateInv, 0.5f, 0.1f);
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->pitchUpdateRateInv, 0.5f, 0.1f);
    } else {
        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->rUpdateRateInv, spA0, 0.1f);
        camera->yawUpdateRateInv = Camera_ScaledStepToCeilF(roData->unk_0C, camera->yawUpdateRateInv, spA0, 0.1f);
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(2.0f, camera->pitchUpdateRateInv, sp9C, 0.1f);
    }

    if ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
        (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) {
        camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.1f, camera->yOffsetUpdateRate, spA0, 0.0001f);
        camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.1f, camera->xzOffsetUpdateRate, sp9C, 0.0001f);
    } else if (roData->interfaceFlags & PARALLEL1_FLAG_7) {
        camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.5f, camera->yOffsetUpdateRate, spA0, 0.0001f);
        camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.5f, camera->xzOffsetUpdateRate, sp9C, 0.0001f);
    } else {
        camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->yOffsetUpdateRate, spA0, 0.0001f);
        camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->xzOffsetUpdateRate, sp9C, 0.0001f);
    }

    // TODO: Extra trailing 0 in 0.050f needed?
    camera->fovUpdateRate =
        Camera_ScaledStepToCeilF(0.050f, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.0001f);

    if (roData->interfaceFlags & PARALLEL1_FLAG_0) {
        tangle = Camera_GetPitchAdjFromFloorHeightDiffs(camera, BINANG_ROT180(sp80.yaw), rwData->unk_26 = 1);
        spA0 = ((1.0f / roData->unk_10));
        spA0 *= 0.6f;
        sp9C = ((1.0f / roData->unk_10) * 0.4f) * (1.0f - camera->speedRatio);
        rwData->unk_1C = Camera_ScaledStepToCeilS(tangle, rwData->unk_1C, spA0 + sp9C, 5);
    } else {
        rwData->unk_1C = 0;
    }

    if (func_800CB950(camera) || (((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) ||
        (((Player*)camera->focalActor)->stateFlags3 & PLAYER_STATE3_100)) {
        rwData->unk_04 = camera->focalActorPosRot.pos.y;
        sp72 = false;
    } else {
        sp72 = true;
    }

    if ((((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_4000) ||
        (((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_4) ||
        ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
         (PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1))) {
        spB0 = spA4;
        spB0.y += ((focalActorHeight * 0.6f) + roData->unk_00);
        Camera_ScaledStepToCeilVec3f(&spB0, at, camera->xzOffsetUpdateRate, camera->yOffsetUpdateRate, 0.0001f);
        Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    } else if ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
               (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) {
        spB0 = focalActorPosRot->pos;
        spB0.y += focalActorHeight + roData->unk_00;
        Camera_ScaledStepToCeilVec3f(&spB0, at, camera->xzOffsetUpdateRate, camera->yOffsetUpdateRate, 0.0001f);
        Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    } else if (rwData->timer2 != 0) {
        Camera_CalcAtDefault(camera, &sp78, roData->unk_00, 0);
        rwData->timer1 = 200.0f;
    } else if (!(roData->interfaceFlags & PARALLEL1_FLAG_7) && !sp72) {
        Camera_CalcAtForParallel(camera, &sp78, roData->unk_00, roData->unk_08, &rwData->unk_04,
                                 roData->interfaceFlags & (PARALLEL1_FLAG_6 | PARALLEL1_FLAG_0));
        rwData->timer1 = 200.0f;
    } else {
        Camera_CalcAtForScreen(camera, &sp78, roData->unk_00, &rwData->unk_04, rwData->timer1);
        if (rwData->timer1 > 10.0f) {
            rwData->timer1--;
        }
    }

    camera->dist = Camera_ScaledStepToCeilF(rwData->unk_00, camera->dist, 1.0f / camera->rUpdateRateInv, 0.1f);

    if (rwData->timer2 != 0) {
        if (rwData->timer3 <= 0) {
            if (rwData->timer3 == 0) {
                Camera_SetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
            }

            tangle = ((rwData->timer2 + 1) * rwData->timer2) >> 1;
            sp90.yaw = sp80.yaw + ((BINANG_SUB(rwData->unk_1E, sp80.yaw) / tangle) * rwData->timer2);
            phi_a0 = ((roData->interfaceFlags & PARALLEL1_FLAG_0) ? BINANG_SUB(rwData->unk_20, rwData->unk_1C)
                                                                  : rwData->unk_20);
            new_var2 = BINANG_SUB(phi_a0, sp80.pitch);
            sp90.pitch = sp80.pitch + ((new_var2 / tangle) * rwData->timer2);
            sp90.r = camera->dist;
            rwData->timer2--;
        } else {
            sp90 = sp80;
            sp90.r = camera->dist;
        }
    } else {
        sp90 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
        sp90.r = camera->dist;

        if (roData->interfaceFlags & PARALLEL1_FLAG_1) {
            sp90.yaw = Camera_ScaledStepToCeilS(rwData->unk_1E, sp78.yaw, 1.0f / camera->yawUpdateRateInv, 0xC8);
        }

        parallelFlagCond = (roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1));

        if (roData->interfaceFlags & PARALLEL1_FLAG_0) {
            phi_a0 = (rwData->unk_20 - rwData->unk_1C);
        } else {
            phi_a0 = rwData->unk_20;
        }

        if (parallelFlagCond == (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) {
            spA0 = CLAMP_MAX(camera->speedRatio, 1.0f);
            phi_a0 = (sp90.pitch * spA0) + (phi_a0 * (1.0f - spA0));
            sp90.pitch = Camera_ScaledStepToCeilS(phi_a0, sp78.pitch, 1.0f / camera->pitchUpdateRateInv, 5);
        } else if (parallelFlagCond != PARALLEL1_FLAG_3) {
            sp90.pitch = Camera_ScaledStepToCeilS(phi_a0, sp78.pitch, 1.0f / camera->pitchUpdateRateInv, 5);
        }

        if (sp90.pitch > DEG_TO_BINANG(79.655f)) {
            sp90.pitch = DEG_TO_BINANG(79.655f);
        }

        if (sp90.pitch < -DEG_TO_BINANG(29.995f)) {
            sp90.pitch = -DEG_TO_BINANG(29.995f);
        }
    }

    if (rwData->timer3 > 0) {
        rwData->timer3--;
    }

    *eyeNext = OLib_AddVecGeoToVec3f(at, &sp90);

    if (camera->status == CAM_STATUS_ACTIVE) {
        if ((camera->play->envCtx.skyboxDisabled == 0) || (roData->interfaceFlags & PARALLEL1_FLAG_4)) {
            spB0 = *at;
            if ((((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_4000) ||
                (((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_4) ||
                ((roData->interfaceFlags & (PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1)) ==
                 (PARALLEL1_FLAG_2 | PARALLEL1_FLAG_1))) {
                spB0.y += focalActorHeight;
            }
            *eye = *eyeNext;
            func_800CBFA4(camera, &spB0, eye, 0);
        } else {
            *eye = *eyeNext;
            func_800CBFA4(camera, at, eye, 3);
        }

        // #region 2S2H [Enhancement] - FixTargettingCameraSnap
        if (CVarGetInteger("gEnhancements.Camera.FixTargettingCameraSnap", 0)) {
            if ((isChargingDekuFlowerDive) && (rwData->timer2 != 0)) {
                sUpdateCameraDirection = true;
            } else {
                sUpdateCameraDirection = false;
            }
        } else {
            // #endregion
            if (rwData->timer2 != 0) {
                sUpdateCameraDirection = true;
            } else {
                sUpdateCameraDirection = false;
            }
        }
    }

    camera->fov = Camera_ScaledStepToCeilF(rwData->unk_08, camera->fov, camera->fovUpdateRate, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);
    camera->atLerpStepScale = Camera_ClampLerpScale(camera, sp72 ? roData->unk_1C : roData->unk_18);
    rwData->unk_26 &= ~1;

    // #region 2S2H [Enhancement] - FixTargettingCameraSnap
    rwData->timer2 |= timer4 << 5;
    // #endregion

    return 1;
}

/**
 * Unused Camera Maze Setting
 */
s32 Camera_Parallel2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Parallel3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Parallel4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Parallel0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Jump1(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Camera for climbing structures
 */
s32 Camera_Jump2(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f spC8;
    Vec3f spBC;
    VecGeo spB4;
    VecGeo spAC;
    VecGeo spA4;
    VecGeo sp9C;
    s16 temp_t2;
    s16 yawDiff;
    f32 tmp;
    f32 sp90;
    f32 sp8C;
    s32 sp88;
    CameraCollision sp60;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Jump2ReadOnlyData* roData = &camera->paramData.jump2.roData;
    Jump2ReadWriteData* rwData = &camera->paramData.jump2.rwData;
    f32 phi_f2;
    f32 yNormal; // used twice
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    f32 temp_f16;

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        yNormal = 0.8f - (-0.2f * (68.0f / focalActorHeight));

        if (camera->unk_0F0.y > 0.0f) {
            phi_f2 = -10.0f;
        } else {
            phi_f2 = 10.0f;
        }

        roData->unk_00 = CAM_RODATA_UNSCALE(phi_f2 + GET_NEXT_RO_DATA(values)) * focalActorHeight * yNormal;
        roData->unk_04 = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * yNormal;
        roData->unk_08 = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * yNormal;
        roData->unk_0C = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sp9C = OLib_Vec3fDiffToVecGeo(at, eye);
    spA4 = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (RELOAD_PARAMS(camera)) {
        spC8 = focalActorPosRot->pos;
        rwData->unk_00 = Camera_GetFloorY(camera, &spC8);
        rwData->unk_04 = spA4.yaw;
        rwData->unk_06 = 0;

        if (rwData->unk_00 == BGCHECK_Y_MIN) {
            rwData->unk_0A = -1;
            rwData->unk_00 = focalActorPosRot->pos.y - 1000.0f;
        } else if ((focalActorPosRot->pos.y - rwData->unk_00) < focalActorHeight) {
            rwData->unk_0A = 1;
        } else {
            rwData->unk_0A = -1;
        }

        yawDiff = BINANG_SUB(BINANG_ROT180(focalActorPosRot->rot.y), spA4.yaw);
        rwData->unk_06 = ((yawDiff / 6) / 4) * 3;

        if (roData->interfaceFlags & JUMP2_FLAG_1) {
            rwData->unk_08 = 10;
        } else {
            rwData->unk_08 = 10000;
        }

        focalActorPosRot->pos.x -= camera->unk_0F0.x;
        focalActorPosRot->pos.y -= camera->unk_0F0.y;
        focalActorPosRot->pos.z -= camera->unk_0F0.z;

        rwData->timer = 6;
        camera->animState++;
        camera->atLerpStepScale = roData->unk_1C;
    }

    sp90 = camera->speedRatio * 0.5f;
    sp8C = camera->speedRatio * 0.2f;

    camera->yawUpdateRateInv = Camera_ScaledStepToCeilF(roData->unk_10, camera->yawUpdateRateInv, sp90, 0.1f);
    camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(roData->unk_14, camera->yOffsetUpdateRate, sp90, 0.0001f);
    camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->xzOffsetUpdateRate, sp8C, 0.0001f);
    camera->fovUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->fovUpdateRate, camera->speedRatio * .05f, 0.0001f);
    camera->rUpdateRateInv = 1800.0f;

    Camera_CalcAtDefault(camera, &spA4, roData->unk_00, 0);
    spB4 = OLib_Vec3fDiffToVecGeo(at, eye);

    tmp = roData->unk_04;
    phi_f2 = roData->unk_08 + (roData->unk_08 * roData->unk_0C);
    temp_f16 = tmp - (roData->unk_04 * roData->unk_0C);

    if (spB4.r > phi_f2) {
        spB4.r = phi_f2;
    } else if (spB4.r < temp_f16) {
        spB4.r = temp_f16;
    }

    yawDiff = BINANG_SUB(BINANG_ROT180(focalActorPosRot->rot.y), spB4.yaw);
    if (rwData->timer != 0) {
        rwData->unk_04 = BINANG_ROT180(focalActorPosRot->rot.y);
        rwData->timer--;
        spB4.yaw = Camera_ScaledStepToCeilS(rwData->unk_04, spA4.yaw, 0.5f, 5);
    } else if (rwData->unk_08 < ABS(yawDiff)) {
        temp_t2 = BINANG_ROT180(focalActorPosRot->rot.y);
        spB4.yaw = Camera_ScaledStepToFloorS((yawDiff < 0) ? (temp_t2 + rwData->unk_08) : (temp_t2 - rwData->unk_08),
                                             spA4.yaw, 0.1f, 1);
    } else {
        spB4.yaw = Camera_ScaledStepToCeilS(spB4.yaw, spA4.yaw, 0.25f, 5);
    }

    spC8.x = focalActorPosRot->pos.x + (Math_SinS(focalActorPosRot->rot.y) * 25.0f);
    spC8.y = focalActorPosRot->pos.y + (focalActorHeight * 2.2f);
    spC8.z = focalActorPosRot->pos.z + (Math_CosS(focalActorPosRot->rot.y) * 25.0f);

    yNormal = Camera_GetFloorYNorm(camera, &spBC, &spC8, &sp88);
    if (camera->focalActor->bgCheckFlags & 0x10) {
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->pitchUpdateRateInv, 0.2f, 0.1f);
        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->rUpdateRateInv, 0.2f, 0.1f);
        spB4.pitch = Camera_ScaledStepToCeilS(-DEG_TO_BINANG(27.47f), spA4.pitch, 0.2f, 5);
    } else if ((yNormal != BGCHECK_Y_MIN) && (focalActorPosRot->pos.y < yNormal)) {
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->pitchUpdateRateInv, 0.2f, 0.1f);
        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->rUpdateRateInv, 0.2f, 0.1f);
        if (camera->unk_0F0.y > 1.0f) {
            spB4.pitch = Camera_ScaledStepToCeilS(0x1F4, spA4.pitch, 1.0f / camera->pitchUpdateRateInv, 5);
        }
    } else if ((focalActorPosRot->pos.y - rwData->unk_00) < focalActorHeight) {
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->pitchUpdateRateInv, 0.2f, 0.1f);
        camera->rUpdateRateInv = Camera_ScaledStepToCeilF(20.0f, camera->rUpdateRateInv, 0.2f, 0.1f);
        if (camera->unk_0F0.y > 1.0f) {
            spB4.pitch = Camera_ScaledStepToCeilS(0x1F4, spA4.pitch, 1.0f / camera->pitchUpdateRateInv, 5);
        }
    } else {
        camera->pitchUpdateRateInv = 100.0f;
        camera->rUpdateRateInv = 100.0f;
    }

    spB4.pitch = CLAMP_MAX(spB4.pitch, DEG_TO_BINANG(60.43f));
    spB4.pitch = CLAMP_MIN(spB4.pitch, -DEG_TO_BINANG(60.43f));

    *eyeNext = OLib_AddVecGeoToVec3f(at, &spB4);
    sp60.pos = *eyeNext;

    if (func_800CBC84(camera, at, &sp60, 0) != 0) {
        spC8 = sp60.pos;
        spAC.pitch = 0;
        spAC.r = spB4.r;
        spAC.yaw = spB4.yaw;
        sp60.pos = OLib_AddVecGeoToVec3f(at, &spAC);
        if (func_800CBC84(camera, at, &sp60, 0) != 0) {
            *eye = spC8;
        } else {
            spB4.pitch = Camera_ScaledStepToCeilS(0, spB4.pitch, 0.2f, 5);
            *eye = OLib_AddVecGeoToVec3f(at, &spB4);
            func_800CBFA4(camera, at, eye, 0);
        }
    } else {
        *eye = *eyeNext;
    }

    camera->dist = spB4.r;
    camera->fov = Camera_ScaledStepToCeilF(roData->unk_18, camera->fov, camera->fovUpdateRate, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);

    return true;
}

/**
 * Used for water-based camera settings
 * e.g. Gyorg, Pinnacle Rock, whirlpool, water
 */
s32 Camera_Jump3(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 spD0;
    f32 spCC;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    f32 phi_f0;
    f32 spC0;
    Vec3f spB4;
    VecGeo spAC;
    CameraModeValue* values;
    f32 phi_f14;
    VecGeo sp9C;
    VecGeo sp94;
    f32 phi_f2_2;
    f32 temp_f0;
    f32 temp1;
    s32 pad;
    Jump3ReadOnlyData* roData = &camera->paramData.jump3.roData;
    Jump3ReadWriteData* rwData = &camera->paramData.jump3.rwData;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    PosRot focalActorFocus = Actor_GetFocus(camera->focalActor);
    f32 sp60 = camera->waterYPos - eye->y;
    f32 sp5C;
    s32 sp58 = false;

    if (RELOAD_PARAMS(camera)) {
        rwData->unk_0A = camera->mode;
        rwData->timer2 = 0;
    }

    if (camera->mode == CAM_MODE_NORMAL) {
        if ((camera->focalActor->bgCheckFlags & 0x10) || (rwData->timer2 != 0)) {
            if (rwData->unk_0A != 0xF) {
                rwData->unk_0A = 0xF;
                sp58 = true;
                rwData->timer2 = 10;
            }
        } else if (sp60 < 50.0f) {
            if (rwData->unk_0A != 0) {
                rwData->unk_0A = 0;
                sp58 = true;
            }
        } else if (Camera_fabsf(camera->focalActorPosRot.pos.y - camera->focalActorFloorHeight) < 11.0f) {
            if (rwData->unk_0A != 5) {
                rwData->unk_0A = 5;
                sp58 = true;
            }
        } else if ((sp60 > 250.0f) && (rwData->unk_0A != 0x1A)) {
            rwData->unk_0A = 0x1A;
            sp58 = true;
        }
    }

    if (rwData->timer2 != 0) {
        rwData->timer2--;
    }

    sp9C = OLib_Vec3fDiffToVecGeo(at, eye);
    sp94 = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    if (!RELOAD_PARAMS(camera) && !sp58) {
    } else {
        values = sCameraSettings[camera->setting].cameraModes[rwData->unk_0A].values;

        sp5C = 0.8f - (-0.2f * (68.0f / focalActorHeight));
        spD0 = focalActorHeight * 0.01f * sp5C;

        roData->unk_00 = GET_NEXT_RO_DATA(values) * spD0;
        roData->unk_04 = GET_NEXT_RO_DATA(values) * spD0;
        roData->unk_08 = GET_NEXT_RO_DATA(values) * spD0;
        roData->unk_20 = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
            rwData->unk_10 = 0x1000;
            // fallthrough
        case 10:
        case 20:
            rwData->unk_00 = camera->focalActorFloorHeight;
            D_801EDC30[camera->camId].yaw = D_801EDC30[camera->camId].pitch = D_801EDC30[camera->camId].unk_64 = 0;
            rwData->timer1 = 10;
            D_801EDC30[camera->camId].swingUpdateRate = roData->unk_0C;
            camera->animState++;
            D_801EDC30[camera->camId].timer = 0;
            break;

        default:
            if (rwData->timer1 != 0) {
                rwData->timer1--;
            }
            break;
    }

    spC0 = focalActorFocus.pos.y - focalActorPosRot->pos.y;
    spB4 = *eye;

    spD0 = camera->speedRatio * 0.5f;
    spCC = camera->speedRatio * 0.2f;

    temp_f0 = (D_801EDC30[camera->camId].unk_64 == 1) ? 0.5f : spD0;

    if (D_801EDC30[camera->camId].timer != 0) {
        camera->yawUpdateRateInv =
            Camera_ScaledStepToCeilF((D_801EDC30[camera->camId].swingUpdateRate + D_801EDC30[camera->camId].timer * 2),
                                     camera->yawUpdateRateInv, spD0, 0.1f);
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF((40.0f + D_801EDC30[camera->camId].timer * 2),
                                                              camera->pitchUpdateRateInv, spCC, 0.1f);
        D_801EDC30[camera->camId].timer--;
    } else {
        camera->yawUpdateRateInv = Camera_ScaledStepToCeilF(D_801EDC30[camera->camId].swingUpdateRate,
                                                            camera->yawUpdateRateInv, temp_f0, 0.1f);
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(40.0f, camera->pitchUpdateRateInv, spCC, 0.1f);
    }

    if (roData->interfaceFlags & JUMP3_FLAG_7) {
        camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.01f, camera->yOffsetUpdateRate, spD0, 0.0001f);
        sp5C = sqrtf((camera->unk_0F0.x * camera->unk_0F0.x) + (camera->unk_0F0.z * camera->unk_0F0.z)) /
               Camera_GetRunSpeedLimit(camera);
        camera->speedRatio = OLib_ClampMaxDist(sp5C / Camera_GetRunSpeedLimit(camera), 1.8f);
        spCC = camera->speedRatio * 0.2f;
    } else {
        camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->yOffsetUpdateRate, spD0, 0.0001f);
    }

    camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.05f, camera->xzOffsetUpdateRate, spCC, 0.0001f);
    camera->fovUpdateRate =
        Camera_ScaledStepToCeilF(0.050f, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.0001f);

    if (sp60 < 50.0f) {
        sp5C = camera->waterYPos - spC0;

        Camera_CalcAtForScreen(camera, &sp94, roData->unk_00, &sp5C,
                               ((sp60 < 0.0f) ? 1.0f : 1.0f - (sp60 / 50.0f)) * 50.0f);
    } else {
        Camera_CalcAtDefault(camera, &sp94, roData->unk_00, roData->interfaceFlags);
    }

    spAC = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    spAC.r = Camera_ClampDist1(camera, spAC.r, roData->unk_04, roData->unk_08, rwData->timer1);
    camera->dist = spAC.r;

    if (!(Camera_fabsf(focalActorPosRot->pos.y - camera->focalActorFloorHeight) < 10.0f) &&
        !(Camera_fabsf(focalActorFocus.pos.y - camera->waterYPos) < 50.f)) {
        camera->pitchUpdateRateInv = 100.0f;
    }

    if (roData->interfaceFlags & JUMP3_FLAG_5) {
        spD0 = CLAMP_MAX(camera->speedRatio * 1.3f, 0.6f);

        //! FAKE: spCC =
        spAC.pitch = Camera_ScaledStepToCeilS(
            (spAC.pitch * spD0) + (roData->unk_20 * (1.0f - spD0)), sp94.pitch,
            1.0f / (spCC = ((camera->pitchUpdateRateInv + 1.0f) - (camera->pitchUpdateRateInv * spD0))), 5);
    } else if (D_801EDC30[camera->camId].unk_64 == 1) {
        spAC.yaw =
            Camera_ScaledStepToCeilS(D_801EDC30[camera->camId].yaw, sp94.yaw, 1.0f / camera->yawUpdateRateInv, 5);

        // Bug? Should be pitchUpdateRateInv
        spAC.pitch =
            Camera_ScaledStepToCeilS(D_801EDC30[camera->camId].pitch, sp94.pitch, 1.0f / camera->yawUpdateRateInv, 5);
    } else if (roData->interfaceFlags & (JUMP3_FLAG_7 | JUMP3_FLAG_3)) {
        spAC.yaw = Camera_CalcDefaultYaw(camera, sp94.yaw, focalActorPosRot->rot.y, roData->unk_14, 0.0f);

        spD0 = CLAMP_MAX(camera->speedRatio * 1.3f, 1.0f);

        //! FAKE: spCC =
        spAC.pitch = Camera_ScaledStepToCeilS(
            (spAC.pitch * spD0) + (roData->unk_20 * (1.0f - spD0)), sp94.pitch,
            1.0f / (spCC = (camera->pitchUpdateRateInv + 1.0f) - (camera->pitchUpdateRateInv * spD0)), 5);
    } else {
        spAC.yaw = Camera_CalcDefaultYaw(camera, sp94.yaw, focalActorPosRot->rot.y, roData->unk_14, 0.0f);
        spAC.pitch = Camera_CalcDefaultPitch(camera, sp94.pitch, roData->unk_20, 0);
    }

    if (spAC.pitch > DEG_TO_BINANG(79.655f)) {
        spAC.pitch = DEG_TO_BINANG(79.655f);
    }

    if (spAC.pitch < -DEG_TO_BINANG(29.995f)) {
        spAC.pitch = -DEG_TO_BINANG(29.995f);
    }

    *eyeNext = OLib_AddVecGeoToVec3f(at, &spAC);

    if ((camera->status == CAM_STATUS_ACTIVE) && !(roData->interfaceFlags & JUMP3_FLAG_6)) {
        if (func_800CBA7C(camera) == 0) {
            Camera_CalcDefaultSwing(camera, &spAC, &sp9C, roData->unk_04, roData->unk_0C, &D_801EDC30[camera->camId],
                                    &rwData->unk_10);
        }

        if (roData->interfaceFlags & JUMP3_FLAG_2) {
            camera->inputDir.x = -sp9C.pitch;
            camera->inputDir.y = sp9C.yaw + 0x8000;
            camera->inputDir.z = 0;
        } else {
            spAC = OLib_Vec3fDiffToVecGeo(eye, at);
            camera->inputDir.x = spAC.pitch;
            camera->inputDir.y = spAC.yaw;
            camera->inputDir.z = 0;
        }
    } else {
        D_801EDC30[camera->camId].swingUpdateRate = roData->unk_0C;
        D_801EDC30[camera->camId].unk_64 = 0;
        sUpdateCameraDirection = false;
        *eye = *eyeNext;
    }

    camera->fov = Camera_ScaledStepToCeilF(roData->unk_18, camera->fov, camera->fovUpdateRate, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, .5f, 5);
    camera->atLerpStepScale = Camera_ClampLerpScale(camera, roData->unk_1C);

    return 1;
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
    Vec3f sp120;
    Vec3f sp114;
    f32 temp_f14;
    f32 temp_f2_3;
    s32 pad1;
    f32 sp104;
    f32 var2;
    f32 spFC;
    f32 spF8;
    f32 spF4;
    s32 skipEyeAtCalc = false;
    f32 distRatio;
    CameraCollision spC4;
    VecGeo spBC;
    VecGeo spB4;
    VecGeo atToTargetDir;
    VecGeo spA4;
    VecGeo atToEyeDir;
    VecGeo atToEyeNextDir;
    s32 pad2;
    PosRot* focalActorFocus = &camera->focalActor->focus;
    s16 sp8A;
    s16 sp88;
    s16 sp86;
    s16 isOffGround;
    f32 swingPitchInitial;
    f32 swingPitchFinal;
    f32 fov;
    f32 temp_f12;
    Battle1ReadOnlyData* roData = &camera->paramData.batt1.roData;
    Battle1ReadWriteData* rwData = &camera->paramData.batt1.rwData;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    s32 pad3;

    if (!RELOAD_PARAMS(camera)) {
    } else {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->yOffset =
            GET_NEXT_RO_DATA(values) * 0.01f * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_0C = 80.0f - (80.0f - roData->unk_0C);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_10 = 0.0f - (0.0f - roData->unk_10);
        roData->swingPitchInitial = GET_NEXT_RO_DATA(values);
        roData->swingPitchInitial = 40.0f - (40.0f - roData->swingPitchInitial);
        roData->swingPitchFinal = GET_NEXT_RO_DATA(values);
        roData->swingPitchFinal = 20.0f - (20.0f - roData->swingPitchFinal);
        roData->swingPitchAdj = GET_NEXT_RO_DATA(values) * 0.01f;
        roData->swingPitchAdj = 1.0f - (1.0f - roData->swingPitchAdj);
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->unk_24 = GET_NEXT_RO_DATA(values) * 0.01f;
        roData->unk_24 = 1.0f - (1.0f - roData->unk_24);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        roData->unk_28 = GET_NEXT_RO_DATA(values) * 0.01f;
        roData->unk_28 = 0.2f - (0.2f - roData->unk_28);
        roData->unk_2C = GET_NEXT_RO_DATA(values) * 0.01f;
        if (!RELOAD_PARAMS(camera)) {
        } else {
            rwData->chargeTimer = 40;
            camera->pitchUpdateRateInv = 9.0f;
        }
    }

    swingPitchInitial = roData->swingPitchInitial;
    swingPitchFinal = roData->swingPitchFinal;
    fov = roData->fov;

    if (Camera_IsChargingSpinAttackOrDekuFlowerDive(camera)) {
        camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(18.0f, camera->pitchUpdateRateInv, 0.5f, 0.1f);
        camera->yOffsetUpdateRate = Camera_ScaledStepToCeilF(0.2f, camera->yOffsetUpdateRate, 0.5f, 0.0001f);
        camera->xzOffsetUpdateRate = Camera_ScaledStepToCeilF(0.2f, camera->xzOffsetUpdateRate, 0.5f, 0.0001f);
        if (rwData->chargeTimer > -20) {
            rwData->chargeTimer--;
        } else {
            swingPitchInitial = 50.0f;
            swingPitchFinal = 40.0f;
            fov = 60.0f;
        }
    } else {
        if (rwData->chargeTimer < 0) {
            swingPitchInitial = 50.0f;
            swingPitchFinal = 40.0f;
            fov = 60.0f;
            rwData->chargeTimer++;
        } else {
            rwData->chargeTimer = 40;
            camera->pitchUpdateRateInv = Camera_ScaledStepToCeilF(9.0f, camera->pitchUpdateRateInv, 0.5f, 0.1f);
            camera->yOffsetUpdateRate =
                Camera_ScaledStepToCeilF(0.6f, camera->yOffsetUpdateRate, camera->speedRatio * 0.5f, 0.0001f);
            camera->xzOffsetUpdateRate =
                Camera_ScaledStepToCeilF(0.6f, camera->xzOffsetUpdateRate, camera->speedRatio * 0.2f, 0.0001f);
        }
    }

    camera->fovUpdateRate =
        Camera_ScaledStepToCeilF(0.050f, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.0001f);

    focalActorHeight += roData->yOffset;

    atToEyeDir = OLib_Vec3fDiffToVecGeo(at, eye);
    atToEyeNextDir = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        camera->target = NULL;
        Camera_ChangeMode(camera, CAM_MODE_TARGET);
        return true;
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (RELOAD_PARAMS(camera)) {
        rwData->unk_10 = 0;
        rwData->unk_1A = 0;
        rwData->unk_08 = camera->target;
        camera->animState++;
        rwData->unk_16 = 7;
        rwData->unk_12 = atToEyeDir.yaw;
        rwData->unk_14 = atToEyeDir.pitch;
        rwData->unk_00 = atToEyeDir.r;
        rwData->unk_04 = camera->focalActorPosRot.pos.y - camera->unk_0F0.y;
        if (camera->dist > (2.0f * roData->unk_04)) {
            camera->dist = 2.0f * roData->unk_04;
            atToEyeNextDir.r = camera->dist;
            atToEyeDir.r = atToEyeNextDir.r;
            *eye = OLib_AddVecGeoToVec3f(at, &atToEyeDir);
            *eyeNext = *eye;
        }
    }

    if (camera->status == CAM_STATUS_ACTIVE) {
        sUpdateCameraDirection = true;
        camera->inputDir.x = -atToEyeDir.pitch;
        camera->inputDir.y = atToEyeDir.yaw + 0x8000;
        camera->inputDir.z = 0;
    }

    if (func_800CB950(camera)) {
        rwData->unk_04 = camera->focalActorPosRot.pos.y;
        isOffGround = false;
    } else {
        isOffGround = true;
    }

    if (rwData->unk_16 == 0) {
        camera->atLerpStepScale = Camera_ClampLerpScale(camera, isOffGround ? roData->unk_28 : roData->unk_24);
    }

    camera->targetPosRot = Actor_GetFocus(camera->target);

    if (rwData->unk_08 != camera->target) {
        camera->animState = 0;
        return true;
    }

    sp120 = camera->focalActorPosRot.pos;
    sp120.y += focalActorHeight;
    spA4 = OLib_Vec3fDiffToVecGeo(&sp120, &camera->targetPosRot.pos);

    sp104 = Camera_GetAttentionActorRange(camera->target);
    if (sp104 > (PREG(86) + 800.0f)) {
        sp104 = PREG(86) + 800.0f;
    }

    if ((spA4.r > sp104) || Camera_IsChargingSpinAttackOrDekuFlowerDive(camera)) {
        distRatio = 1.0f;
        spF8 = 10.0f;
    } else {
        distRatio = spA4.r / sp104;
        spF8 = 2.0f;
    }

    Camera_CalcAtForEnemyLockOn(camera, &atToEyeNextDir, &camera->targetPosRot.pos, roData->yOffset, roData->unk_2C,
                                1.0f - distRatio, &rwData->unk_04, &spA4,
                                (isOffGround ? (BATTLE1_FLAG_7 | BATTLE1_FLAG_0) : BATTLE1_FLAG_0) |
                                    roData->interfaceFlags);

    sp88 = spA4.yaw;
    spBC = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    spF4 = F32_LERPIMP(roData->unk_04, roData->unk_08, distRatio);
    camera->rUpdateRateInv = Camera_ScaledStepToCeilF(spF8, camera->rUpdateRateInv, 0.5f, 0.1f);
    spBC.r = camera->dist = Camera_ScaledStepToCeilF(spF4, camera->dist, 1.0f / camera->rUpdateRateInv, 0.1f);

    atToTargetDir = OLib_Vec3fDiffToVecGeo(at, &camera->targetPosRot.pos);

    atToTargetDir.r = spBC.r - (((spBC.r >= atToTargetDir.r) ? atToTargetDir.r : spBC.r) * 0.5f);

    if (rwData->unk_1A & 0x1000) {
        spFC = 15.0f;
    } else {
        spFC = roData->unk_0C + ((roData->unk_10 - roData->unk_0C) * (1.1f - distRatio));
    }

    spBC.yaw = atToEyeNextDir.yaw;
    sp8A = (s16)(atToTargetDir.yaw - (s16)(atToEyeNextDir.yaw + 0x8000));

    if (rwData->unk_16 != 0) {
        if (rwData->unk_16 > 0) {
            sp86 = rwData->unk_16 - 1;
            spB4 = OLib_Vec3fDiffToVecGeo(at, eye);
            spB4.yaw = sp88 + 0x8000;
            spF8 = (rwData->unk_00 - spB4.r) * (1.0f / 6.0f);
            sp8A = (s16)(rwData->unk_12 - spB4.yaw) * (1.0f / 6.0f);
            sp88 = (s16)(rwData->unk_14 - spB4.pitch) * (1.0f / 6.0f);
            spBC.r = Camera_ScaledStepToCeilF((sp86 * spF8) + spB4.r, atToEyeDir.r, 0.5f, 1.0f);
            spBC.yaw = Camera_ScaledStepToCeilS(spB4.yaw + (sp8A * sp86), atToEyeDir.yaw, 0.5f, 5);
            spBC.pitch = Camera_ScaledStepToCeilS(spB4.pitch + (sp88 * sp86), atToEyeDir.pitch, 0.5f, 5);
        } else {
            skipEyeAtCalc = true;
        }
        rwData->unk_16--;
    } else if ((ABS(sp8A) > CAM_DEG_TO_BINANG(spFC)) && !isOffGround) {
        sp104 = CAM_BINANG_TO_DEG(sp8A);
        temp_f2_3 = spFC + (((spFC + 10.0f) - spFC) * (OLib_ClampMaxDist(atToTargetDir.r, spBC.r) / spBC.r));
        temp_f12 = (SQ(temp_f2_3) - 2.0f) / (temp_f2_3 - 360.0f);
        var2 = ((temp_f12 * sp104) + (2.0f - (360.0f * temp_f12)));
        temp_f14 = SQ(sp104) / var2;

        if (sp8A >= 0) {
            sp88 = CAM_DEG_TO_BINANG(temp_f14);
        } else {
            sp88 = -CAM_DEG_TO_BINANG(temp_f14);
        }

        spBC.yaw = (s16)((s16)(atToEyeNextDir.yaw + 0x8000) + sp88) + 0x8000;
    } else {
        sp104 = (1.0f - camera->speedRatio) * 0.05f;
        sp88 = (sp8A >= 0) ? CAM_DEG_TO_BINANG(spFC) : -CAM_DEG_TO_BINANG(spFC);
        spBC.yaw = atToEyeNextDir.yaw - TRUNCF_BINANG((sp88 - sp8A) * sp104);
    }

    if (!skipEyeAtCalc) {
        spF8 = atToTargetDir.pitch * roData->swingPitchAdj;
        var2 = swingPitchInitial + ((swingPitchFinal - swingPitchInitial) * distRatio);
        sp8A = CAM_DEG_TO_BINANG(var2) - TRUNCF_BINANG((spA4.pitch * (0.5f + (distRatio * (1.0f - 0.5f)))) + 0.5f);
        sp8A += TRUNCF_BINANG(spF8);

        if (sp8A < -0x2AA8) {
            sp8A = -0x2AA8;
        } else if (sp8A > 0x2AA8) {
            sp8A = 0x2AA8;
        }

        spBC.pitch = Camera_ScaledStepToCeilS(sp8A, atToEyeNextDir.pitch, 1.0f / camera->pitchUpdateRateInv, 5);
        *eyeNext = OLib_AddVecGeoToVec3f(at, &spBC);
        spC4.pos = *eyeNext;

        if (camera->status == CAM_STATUS_ACTIVE) {
            if (!(roData->interfaceFlags & BATTLE1_FLAG_4)) {
                if (!camera->play->envCtx.skyboxDisabled || (roData->interfaceFlags & BATTLE1_FLAG_0)) {
                    if (func_800CBC84(camera, at, &spC4, 0) != 0) {
                        rwData->unk_1A |= 0x1000;
                    } else {
                        rwData->unk_1A &= ~0x1000;
                    }
                } else if (roData->interfaceFlags & BATTLE1_FLAG_1) {
                    func_800CBC84(camera, at, &spC4, 3);
                } else {
                    sp114 = OLib_Vec3fDistNormalize(at, &spC4.pos);
                    spC4.pos.x -= sp114.x;
                    spC4.pos.y -= sp114.y;
                    spC4.pos.z -= sp114.z;
                }
                *eye = spC4.pos;
                rwData->unk_1A &= ~0x10;
            } else if (!camera->play->envCtx.skyboxDisabled || (roData->interfaceFlags & BATTLE1_FLAG_0)) {
                if (func_800CBC84(camera, at, &spC4, 0) != 0) {
                    s16 screenPosX;
                    s16 screenPosY;

                    rwData->unk_1A |= 0x1000;
                    spF8 = OLib_Vec3fDist(at, &focalActorFocus->pos);

                    //! FAKE:
                    if (1) {}

                    spF4 = OLib_Vec3fDist(at, &spC4.pos);
                    spF8 += (rwData->unk_1A & 0x10) ? 40.0f : 0.0f;
                    Actor_GetScreenPos(camera->play, camera->focalActor, &screenPosX, &screenPosY);

                    if ((spF4 < spF8) || ((screenPosX >= 0) && (screenPosX <= SCREEN_WIDTH) && (screenPosY >= 0) &&
                                          (screenPosY <= SCREEN_HEIGHT))) {
                        rwData->unk_1A |= 0x10;
                        spB4.yaw = spA4.yaw + 0x8000;
                        spB4.pitch = -spA4.pitch;
                        spB4.r = 40.0f;
                        sp114 = focalActorFocus->pos;
                        sp114.y += 40.0f;
                        sp120 = OLib_AddVecGeoToVec3f(&sp114, &spB4);
                        Camera_ScaledStepToCeilVec3f(&sp120, eye, 0.15f, 0.15f, 0.2f);
                    } else if (rwData->unk_1A & 0x10) {
                        if (OLib_Vec3fDist(&spC4.pos, eye) < 20.0f) {
                            rwData->unk_1A &= ~0x10;
                            *eye = spC4.pos;
                        } else {
                            Camera_ScaledStepToCeilVec3f(&spC4.pos, eye, 0.15f, 0.15f, 0.2f);
                        }
                    } else {
                        rwData->unk_1A &= ~0x10;
                        *eye = spC4.pos;
                    }
                } else {
                    if (rwData->unk_1A & 0x10) {
                        if (OLib_Vec3fDist(&spC4.pos, eye) < 20.0f) {
                            rwData->unk_1A &= ~0x10;
                            *eye = spC4.pos;
                        } else {
                            Camera_ScaledStepToCeilVec3f(&spC4.pos, eye, 0.15f, 0.15f, 0.2f);
                        }
                    } else {
                        rwData->unk_1A &= ~0x10;
                        *eye = spC4.pos;
                    }
                    rwData->unk_1A &= ~0x1000;
                }
            } else if (roData->interfaceFlags & BATTLE1_FLAG_1) {
                rwData->unk_1A &= ~0x10;
                if (func_800CBC84(camera, at, &spC4, 3) != 0) {
                    *eye = spC4.pos;
                } else {
                    *eye = spC4.pos;
                }
            } else {
                rwData->unk_1A &= ~0x10;
                sp114 = OLib_Vec3fDistNormalize(at, &spC4.pos);
                spC4.pos.x -= sp114.x;
                spC4.pos.y -= sp114.y;
                spC4.pos.z -= sp114.z;
                *eye = spC4.pos;
            }
        } else {
            rwData->unk_1A &= ~0x10;
            *eye = *eyeNext;
        }
    }

    if (rwData->unk_16 != 0) {
        sp88 = 0;
    } else {
        sp88 = (s16)(atToEyeNextDir.yaw - spBC.yaw) * 0.50f;
    }
    camera->roll = Camera_ScaledStepToCeilS(sp88, camera->roll, 0.06f, 5);

    if (func_800CBAAC(camera) != PLAYER_MELEE_WEAPON_STATE_0) {
        temp_f12 = ((camera->play->state.frames & 8) != 0) ? roData->fov - (roData->fov * 0.5f) : roData->fov;
    } else {
        temp_f12 =
            ((gSaveContext.save.saveInfo.playerData.health <= 16) ? 0.8f : 1.0f) * (fov - (fov * 0.05f * distRatio));
    }
    camera->fov = Camera_ScaledStepToCeilF(temp_f12, camera->fov, camera->fovUpdateRate, 0.1f);

    return true;
}

s32 Camera_Battle2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Battle3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Battle4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Battle0(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Used for following a secondary target such as zora boomerangs or a z-target
 */
s32 Camera_KeepOn1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp130;
    Vec3f sp124;
    Vec3f sp118;
    f32 sp114;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    s32 pad1;
    f32 var2;
    f32 sp104;
    f32 temp_f2_3;
    f32 spFC;
    f32 spF8;
    f32 spF4;
    s16 spF2;
    s16 spF0;
    VecGeo spE8;
    VecGeo spE0;
    VecGeo spD8;
    VecGeo spD0;
    VecGeo atToEye;
    VecGeo atToEyeNext;
    s32 pad2;
    PosRot spA8;
    PosRot* focalActorFocus = &camera->focalActor->focus;
    CameraCollision sp7C;
    s32 skipEyeAtCalc = false;
    f32 sp74;
    s16 sp72;
    s16 sp70;
    KeepOn1ReadOnlyData* roData = &camera->paramData.keep1.roData;
    KeepOn1ReadWriteData* rwData = &camera->paramData.keep1.rwData;
    s32 pad3;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    s32 pad4;

    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        camera->target = NULL;
        Camera_ChangeMode(camera, CAM_MODE_TARGET);
        return 1;
    }

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->yOffset =
            GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = 40.0f - (40.0f - roData->unk_14);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = 20.0f - (20.0f - roData->unk_18);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_1C = 1.000f - (1.00f - roData->unk_1C);
        roData->unk_20 = GET_NEXT_RO_DATA(values);
        roData->unk_24 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_24 = 1.0f - (1.0f - roData->unk_24);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        roData->unk_28 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_28 = 0.2f - (0.2f - roData->unk_28);
    }

    focalActorHeight += roData->yOffset;

    atToEye = OLib_Vec3fDiffToVecGeo(at, eye);
    atToEyeNext = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (RELOAD_PARAMS(camera)) {
        camera->animState++;
        rwData->unk_18 = 0;
        rwData->unk_10 = 0;
        rwData->unk_04 = 0.0f;
        rwData->unk_0C = camera->target;
        rwData->unk_16 = 7;
        rwData->unk_12 = atToEye.yaw;
        rwData->unk_14 = atToEye.pitch;
        rwData->unk_00 = atToEye.r;
        rwData->unk_08 = focalActorPosRot->pos.y - camera->unk_0F0.y;
        if (camera->dist > (2.0f * roData->unk_04)) {
            camera->dist = 2.0f * roData->unk_04;
            atToEyeNext.r = camera->dist;
            atToEye.r = atToEyeNext.r;
            *eye = OLib_AddVecGeoToVec3f(at, &atToEye);
            *eyeNext = *eye;
        }
    }

    if (camera->status == CAM_STATUS_ACTIVE) {
        sUpdateCameraDirection = true;
        camera->inputDir.x = -atToEye.pitch;
        camera->inputDir.y = atToEye.yaw + 0x8000;
        camera->inputDir.z = 0;
    }

    if (func_800CB950(camera)) {
        rwData->unk_08 = focalActorPosRot->pos.y;
    }

    sp114 = roData->unk_04;

    if (camera->target->id != ACTOR_EN_BOOM) {
        camera->targetPosRot = Actor_GetWorldPosShapeRot(camera->target);
        spA8 = Actor_GetFocus(camera->target);
        camera->targetPosRot.pos.y = spA8.pos.y;
    } else {
        camera->targetPosRot = Actor_GetFocus(camera->target);
    }
    if (rwData->unk_0C != camera->target) {
        rwData->unk_0C = camera->target;
        camera->atLerpStepScale = 0.0f;
    }

    camera->yOffsetUpdateRate =
        Camera_ScaledStepToCeilF(1.0f, camera->yOffsetUpdateRate, camera->speedRatio * 0.5f, 0.0001f);
    camera->xzOffsetUpdateRate =
        Camera_ScaledStepToCeilF(1.0f, camera->xzOffsetUpdateRate, camera->speedRatio * 0.2f, 0.0001f);

    camera->fovUpdateRate = Camera_ScaledStepToCeilF(.05f, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.0001f);

    if (func_800CB950(camera)) {
        rwData->unk_08 = focalActorPosRot->pos.y;
        sp70 = false;
    } else {
        sp70 = true;
    }

    Camera_CalcAtForFriendlyLockOn(camera, &atToEyeNext, &camera->targetPosRot.pos, roData->yOffset, roData->unk_08,
                                   &rwData->unk_08, &spD0, roData->interfaceFlags | (sp70 ? KEEPON1_FLAG_7 : 0));

    sp124 = focalActorPosRot->pos;
    sp124.y += focalActorHeight;

    spD0 = OLib_Vec3fDiffToVecGeo(&sp124, &camera->targetPosRot.pos);

    if (spD0.r > sp114) {
        sp74 = 1.0f;
    } else {
        sp74 = spD0.r / sp114;
    }

    spE8 = OLib_Vec3fDiffToVecGeo(at, eyeNext);

    if (spE8.r < roData->unk_04) {
        sp114 = roData->unk_04;
        spF8 = 20.0f;
    } else if (spE8.r > roData->unk_08) {
        sp114 = roData->unk_08;
        spF8 = 20.0f;
    } else {
        sp114 = spE8.r;
        spF8 = 1.0f;
    }

    camera->rUpdateRateInv = Camera_ScaledStepToCeilF(spF8, camera->rUpdateRateInv, .5f, 0.1f);
    spE8.r = spF8 = camera->dist = Camera_ScaledStepToCeilF(sp114, camera->dist, 1.0f / camera->rUpdateRateInv, 0.1f);

    sp118 = camera->targetPosRot.pos;
    spD8 = OLib_Vec3fDiffToVecGeo(at, &sp118);

    spD8.r = spF8 - (((spD8.r <= spF8) ? spD8.r : spF8) * 0.5f);
    spE8.r = camera->dist = Camera_ScaledStepToCeilF(spF8, camera->dist, 0.06f, 0.1f);

    spFC = F32_LERPIMP(roData->unk_0C, roData->unk_10, 1.1f - sp74);

    spE8.yaw = atToEyeNext.yaw;
    spF2 = spD8.yaw - (s16)(atToEyeNext.yaw + 0x8000);

    if (rwData->unk_16 != 0) {
        if (rwData->unk_16 > 0) {
            sp72 = rwData->unk_16 - 1;
            spF2 = spD0.yaw;
            spD0 = OLib_Vec3fDiffToVecGeo(at, eye);

            spD0.yaw = spF2 + 0x8000;

            spF8 = (rwData->unk_00 - spD0.r) * (1.0f / 6.0f);
            spF2 = (s16)(rwData->unk_12 - spD0.yaw) * (1.0f / 6.0f);
            spF0 = (s16)(rwData->unk_14 - spD0.pitch) * (1.0f / 6.0f);

            spE8.r = Camera_ScaledStepToCeilF(spD0.r + (spF8 * sp72), atToEye.r, .5f, 0.1f);
            spE8.yaw = Camera_ScaledStepToCeilS(spD0.yaw + (spF2 * sp72), atToEye.yaw, .5f, 5);
            spE8.pitch = Camera_ScaledStepToCeilS(spD0.pitch + (spF0 * sp72), atToEye.pitch, .5f, 5);
        }
        skipEyeAtCalc = true;
        rwData->unk_16--;
    } else if (ABS(spF2) > CAM_DEG_TO_BINANG(spFC)) {
        sp104 = CAM_BINANG_TO_DEG(spF2);
        temp_f2_3 = ((OLib_ClampMaxDist(spD8.r, spE8.r) / spE8.r) * ((spFC + 10.0f) - spFC)) + spFC;
        temp_f2_3 = (SQ(temp_f2_3) - 2.0f) / (temp_f2_3 - 360.0f);
        var2 = (temp_f2_3 * sp104) + (2.0f - (360.0f * temp_f2_3));
        temp_f2_3 = SQ(sp104) / var2;

        if (spF2 >= 0) {
            spF0 = CAM_DEG_TO_BINANG(temp_f2_3);
        } else {
            spF0 = -CAM_DEG_TO_BINANG(temp_f2_3);
        }
        spE8.yaw = (s16)((s16)(atToEyeNext.yaw + 0x8000) + spF0) + 0x8000;
    } else {
        sp104 = (1.0f - camera->speedRatio) * 0.05f;
        spF0 = (spF2 >= 0) ? CAM_DEG_TO_BINANG(spFC) : -CAM_DEG_TO_BINANG(spFC);

        spE8.yaw = atToEyeNext.yaw - TRUNCF_BINANG((spF0 - spF2) * sp104);
    }

    if (!skipEyeAtCalc) {
        spF2 = CAM_DEG_TO_BINANG(F32_LERPIMP(roData->unk_14, roData->unk_18, sp74));
        spF2 -= TRUNCF_BINANG((spD0.pitch * (0.5f + (sp74 * 0.5f))) + 0.5f);

        spF8 = spD8.pitch * roData->unk_1C;
        spF2 += TRUNCF_BINANG(spF8);

        if (spF2 < -0x3200) {
            spF2 = -0x3200;
        } else if (spF2 > 0x3200) {
            spF2 = 0x3200;
        }

        spE8.pitch = Camera_ScaledStepToCeilS(spF2, atToEyeNext.pitch, 1.0f / 9.0f, 5);
        *eyeNext = OLib_AddVecGeoToVec3f(at, &spE8);
        sp7C.pos = *eyeNext;

        if (camera->status == CAM_STATUS_ACTIVE) {
            if (!(roData->interfaceFlags & KEEPON1_FLAG_4)) {
                if (!camera->play->envCtx.skyboxDisabled || (roData->interfaceFlags & KEEPON1_FLAG_0)) {
                    if (func_800CBC84(camera, at, &sp7C, 0) != 0) {
                        rwData->unk_18 |= 0x1000;
                    } else {
                        rwData->unk_18 &= ~0x1000;
                    }
                } else if (roData->interfaceFlags & KEEPON1_FLAG_1) {
                    func_800CBC84(camera, at, &sp7C, 3);
                } else {
                    sp130 = OLib_Vec3fDistNormalize(at, &sp7C.pos);
                    sp7C.pos.x -= sp130.x;
                    sp7C.pos.y -= sp130.y;
                    sp7C.pos.z -= sp130.z;
                }
                *eye = sp7C.pos;
                rwData->unk_18 &= ~0x10;
            } else if (!camera->play->envCtx.skyboxDisabled || (roData->interfaceFlags & KEEPON1_FLAG_0)) {
                if (func_800CBC84(camera, at, &sp7C, 0) != 0) {
                    s16 screenPosX;
                    s16 screenPosY;

                    rwData->unk_18 |= 0x1000;
                    spF8 = OLib_Vec3fDist(at, &focalActorFocus->pos);
                    spF4 = OLib_Vec3fDist(at, &sp7C.pos);
                    spF8 += (rwData->unk_18 & 0x10) ? 40 : 0.0f;

                    Actor_GetScreenPos(camera->play, camera->focalActor, &screenPosX, &screenPosY);

                    if ((spF4 < spF8) || ((screenPosX >= 0) && (screenPosX <= SCREEN_WIDTH) && (screenPosY >= 0) &&
                                          (screenPosY <= SCREEN_HEIGHT))) {
                        rwData->unk_18 |= 0x10;
                        spE0.yaw = (s16)(spD0.yaw + 0x8000);
                        spE0.pitch = -spD0.pitch;
                        spE0.r = 40.0f;
                        sp130 = focalActorFocus->pos;
                        sp130.y += 40.0f;
                        sp124 = OLib_AddVecGeoToVec3f(&sp130, &spE0);
                        Camera_ScaledStepToCeilVec3f(&sp124, eye, 0.15f, 0.15f, 0.2f);
                    } else if (rwData->unk_18 & 0x10) {
                        if (OLib_Vec3fDist(&sp7C.pos, eye) < 20.0f) {
                            rwData->unk_18 &= ~0x10;
                            *eye = sp7C.pos;
                        } else {
                            Camera_ScaledStepToCeilVec3f(&sp7C.pos, eye, 0.15f, 0.15f, 0.2f);
                        }
                    } else {
                        rwData->unk_18 &= ~0x10;
                        *eye = sp7C.pos;
                    }
                } else {
                    if (rwData->unk_18 & 0x10) {
                        if (OLib_Vec3fDist(&sp7C.pos, eye) < 20.0f) {
                            rwData->unk_18 &= ~0x10;
                            *eye = sp7C.pos;
                        } else {
                            Camera_ScaledStepToCeilVec3f(&sp7C.pos, eye, 0.15f, 0.15f, 0.2f);
                        }
                    } else {
                        rwData->unk_18 &= ~0x10;
                        *eye = sp7C.pos;
                    }
                    rwData->unk_18 &= ~0x1000;
                }
            } else if (roData->interfaceFlags & KEEPON1_FLAG_1) {
                rwData->unk_18 &= ~0x10;
                if (func_800CBC84(camera, at, &sp7C, 3) != 0) {
                    *eye = sp7C.pos;
                } else {
                    *eye = sp7C.pos;
                }
            } else {
                rwData->unk_18 &= ~0x10;
                sp130 = OLib_Vec3fDistNormalize(at, &sp7C.pos);
                sp7C.pos.x -= sp130.x;
                sp7C.pos.y -= sp130.y;
                sp7C.pos.z -= sp130.z;
                *eye = sp7C.pos;
            }
        } else {
            rwData->unk_18 &= ~0x10;
            *eye = *eyeNext;
        }
    }

    camera->fov = Camera_ScaledStepToCeilF(roData->unk_20, camera->fov, camera->fovUpdateRate, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.2f, 5);
    camera->atLerpStepScale = Camera_ClampLerpScale(camera, sp70 ? roData->unk_28 : roData->unk_24);

    return true;
}

s32 Camera_KeepOn2(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Talking to an NPC
 */
s32 Camera_KeepOn3(Camera* camera) {
    Vec3f* eye = &camera->eye;
    s32 temp_f0;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f spD8;
    Vec3f spCC;
    Vec3f spC0;
    f32 timer;
    f32 spB8;
    f32 spB4;
    f32 swingAngle;
    Actor* spA8[2];
    VecGeo spA0;
    VecGeo sp98;
    VecGeo sp90;
    VecGeo sp88;
    VecGeo sp80;
    Vec3f* at = &camera->at;
    s32 sp78;
    f32 phi_f14;
    PosRot* focalActorFocus = &camera->focalActor->focus;
    s16 sp6E;
    s16 sp6C;
    s16 sp6A = 0;
    s16 phi_a3;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    KeepOn3ReadOnlyData* roData = &camera->paramData.keep3.roData;
    KeepOn3ReadWriteData* rwData = &camera->paramData.keep3.rwData;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    s32 i;

    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        camera->target = NULL;
        Camera_ChangeMode(camera, CAM_MODE_TARGET);
        return 1;
    }

    if (RELOAD_PARAMS(camera)) {
        if (camera->play->view.unk164 == 0) {
            Camera_SetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
            camera->play->view.unk164 = camera->camId | 0x50;
            return 1;
        }
        Camera_UnsetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
    }

    Camera_UnsetStateFlag(camera, CAM_STATE_4);
    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->unk_00 =
            GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_20 = GET_NEXT_RO_DATA(values);
        roData->unk_24 = GET_NEXT_SCALED_RO_DATA(values);
        roData->timer = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        roData->unk_28 = GET_NEXT_SCALED_RO_DATA(values);
    }

    if (camera->animState == 10) {
        roData->timer >>= 1;
        if (roData->timer < 20) {
            roData->timer = 20;
        }
    }

    sp88 = OLib_Vec3fDiffToVecGeo(at, eye);
    sp80 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    camera->targetPosRot = Actor_GetFocus(camera->target);
    spD8 = focalActorPosRot->pos;
    spD8.y += focalActorHeight;
    spA0 = OLib_Vec3fDiffToVecGeo(&spD8, &camera->targetPosRot.pos);

    if (RELOAD_PARAMS(camera)) {
        camera->animState++;
        spA8[0] = camera->target;
        spA8[1] = camera->focalActor;
        rwData->unk_0C = camera->target;
        phi_f14 = (roData->unk_08 < spA0.r) ? 1.0f : (spA0.r / roData->unk_08);

        rwData->timer = roData->timer;
        spB8 = ((1.0f - phi_f14) * spA0.r) / rwData->timer;

        if (roData->interfaceFlags & KEEPON3_FLAG_6) {
            spA0.pitch = 0;
        }

        swingAngle = LERPIMP(roData->unk_14, roData->unk_18, phi_f14);
        sp98.pitch = CAM_DEG_TO_BINANG(swingAngle) + TRUNCF_BINANG(-(spA0.pitch * roData->unk_1C));
        swingAngle = LERPIMP(roData->unk_0C, roData->unk_10, phi_f14);

        phi_a3 = CAM_DEG_TO_BINANG(swingAngle);

        sp78 = 1;
        if ((camera->target->category == ACTORCAT_DOOR) || (camera->target->id == ACTOR_EN_TALK)) {
            if ((s16)(camera->targetPosRot.rot.y - sp80.yaw) > 0) {
                phi_a3 = -phi_a3;
            }

            if (ABS(spA0.pitch) < 0x157C) { // 30.2 degrees
                sp98.yaw = BINANG_ROT180(spA0.yaw) + phi_a3;
                sp6A = (s16)(BINANG_ROT180(spA0.yaw) - phi_a3);
            } else {
                sp78 = 0;
                sp90.r = 30.0f;
                sp90.yaw = BINANG_ROT180(spA0.yaw) + (phi_a3 >> 2);
                sp90.pitch = -spA0.pitch;
                spCC = focalActorFocus->pos;
                spCC.y += 30.0f;
                spC0 = OLib_AddVecGeoToVec3f(&spCC, &sp90);
                spCC.x = (camera->targetPosRot.pos.x + spD8.x) * 0.5f;
                spCC.y = (camera->targetPosRot.pos.y + spD8.y) * 0.5f;
                spCC.z = (camera->targetPosRot.pos.z + spD8.z) * 0.5f;
                sp98 = OLib_Vec3fDiffToVecGeo(&spCC, &spC0);
                if (sp98.pitch < -0x2328) { // 49.4 degrees
                    sp98.pitch = -0x2328;
                }
            }
        } else if (!(roData->interfaceFlags & KEEPON3_FLAG_5) && (ABS((s16)(spA0.yaw - sp80.yaw)) < 0x4000)) {
            if ((s16)(spA0.yaw - sp80.yaw) > 0) {
                phi_a3 = -phi_a3;
            }
            sp98.yaw = (s16)(spA0.yaw + phi_a3);
            sp6A = (s16)(spA0.yaw - phi_a3);
        } else {
            if ((s16)(BINANG_ROT180(spA0.yaw) - sp80.yaw) < 0) {
                phi_a3 = -phi_a3;
            }

            sp98.yaw = BINANG_ROT180(spA0.yaw) + phi_a3;
            sp6A = BINANG_ROT180(spA0.yaw) - phi_a3;
        }

        sp6C = sp98.yaw;
        sp6E = sp98.pitch;

        //! FAKE:
        if (roData->unk_28) {}

        spB4 = spA0.r;

        phi_f14 = roData->unk_28;
        spA0.r = spB8 * phi_f14 + (spB4 * (1.0f - phi_f14));
        spD8 = focalActorPosRot->pos;
        spD8.y += focalActorHeight;

        rwData->unk_10 = OLib_AddVecGeoToVec3f(&spD8, &spA0);
        rwData->unk_10.y += roData->unk_00;
        spA0.r = spB4;

        if (sp78 != 0) {
            sp98.r = ((roData->unk_04 + (spA0.r * 0.5f)) - sp80.r) + sp80.r;
            i = 0;
            sp78 = 14;
            spCC = OLib_AddVecGeoToVec3f(&rwData->unk_10, &sp98);
            if (!(roData->interfaceFlags & KEEPON3_FLAG_7)) {
                if (CollisionCheck_LineOCCheck(camera->play, &camera->play->colChkCtx, &rwData->unk_10, &spCC, spA8,
                                               2) ||
                    Camera_BgCheck(camera, &rwData->unk_10, &spCC)) {
                    sp98.yaw = sp6A;
                    spCC = OLib_AddVecGeoToVec3f(&rwData->unk_10, &sp98);
                }
                while (i < sp78) {
                    if (!CollisionCheck_LineOCCheck(camera->play, &camera->play->colChkCtx, &rwData->unk_10, &spCC,
                                                    spA8, 2) &&
                        !Camera_BgCheck(camera, &rwData->unk_10, &spCC)) {
                        break;
                    }
                    sp98.yaw = sp6C + D_801B9E18[i];
                    sp98.pitch = sp6E + D_801B9E34[i];
                    spCC = OLib_AddVecGeoToVec3f(&rwData->unk_10, &sp98);
                    i++;
                }
            }
        }

        Camera_UnsetStateFlag(camera, CAM_STATE_3 | CAM_STATE_2);
        temp_f0 = ((rwData->timer + 1) * rwData->timer) >> 1;
        rwData->unk_04 = (f32)(s16)(sp98.yaw - sp80.yaw) / temp_f0;
        rwData->unk_08 = (f32)(s16)(sp98.pitch - sp80.pitch) / temp_f0;
        rwData->unk_00 = (sp98.r - sp80.r) / temp_f0;
        sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_IGNORE, 0);

        return 1;
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (rwData->timer != 0) {
        timer = rwData->timer;

        at->x += (rwData->unk_10.x - at->x) / timer;
        at->y += (rwData->unk_10.y - at->y) / timer;
        at->z += (rwData->unk_10.z - at->z) / timer;

        sp98.r = (rwData->unk_00 * timer) + sp80.r + 1.0f;
        sp98.yaw = sp80.yaw + TRUNCF_BINANG(rwData->unk_04 * timer);
        sp98.pitch = sp80.pitch + TRUNCF_BINANG(rwData->unk_08 * timer);
        *eyeNext = OLib_AddVecGeoToVec3f(at, &sp98);
        *eye = *eyeNext;
        camera->fov = Camera_ScaledStepToCeilF(roData->unk_20, camera->fov, 0.5f, 0.1f);
        camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);
        camera->atLerpStepScale = Camera_ClampLerpScale(camera, roData->unk_24);
        func_800CBFA4(camera, at, eye, 3);
        rwData->timer--;
    } else {
        Camera_SetStateFlag(camera, CAM_STATE_10 | CAM_STATE_4);
    }

    Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    camera->dist = OLib_Vec3fDist(at, eye);
    if (camera->stateFlags & CAM_STATE_3) {
        sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
        Camera_SetUpdateRatesSlow(camera);
        camera->atLerpStepScale = 0.0f;

        if ((camera->xzSpeed > 0.001f) || CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_A) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_B) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CUP) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CDOWN) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CLEFT) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CRIGHT) ||
            // #region 2S2H [Dpad]
            CHECK_BTN_DPAD(CONTROLLER1(&camera->play->state)->press.button) ||
            // #endregion
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_Z) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_L) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_R)) {
            Camera_SetStateFlag(camera, CAM_STATE_2);
            Camera_UnsetStateFlag(camera, CAM_STATE_3);
        }
    }

    if (rwData->unk_0C != camera->target) {
        rwData->unk_0C = camera->target;
        camera->animState = 10;
    }

    return 1;
}

/**
 * Used for settings where camera turns around and faces player
 * eg. all item-based settings, elegy shell cutscene, death cutscene, ...
 */
s32 Camera_KeepOn4(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Actor* spCC[2];
    f32 temp_f0_2;
    f32 new_var;
    CollisionPoly* spC0;
    VecGeo spB8;
    VecGeo spB0;
    VecGeo spA8;
    s16* sp44 = &camera->data2;
    s16 spA2;
    s16 spA0;
    s16 sp9E;
    s16 sp9C;
    s16 pad1;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    KeepOn4ReadOnlyData* roData = &camera->paramData.keep4.roData;
    KeepOn4ReadWriteData* rwData = &camera->paramData.keep4.rwData;
    f32 focalActorHeight;
    s32 bgId;
    s16 camMode;
    Player* player;
    Vec3f* data;
    CameraModeValue* values;
    PosRot sp60;
    PosRot sp4C;

    if (RELOAD_PARAMS(camera)) {
        if (camera->play->view.unk164 == 0) {
            Camera_SetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
            Camera_UnsetStateFlag(camera, CAM_STATE_2 | CAM_STATE_CHECK_WATER);
            camera->play->view.unk164 = camera->camId | 0x50;
            return true;
        }
        rwData->unk_18 = *sp44;
        Camera_UnsetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
    }

    if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
        player = (Player*)camera->focalActor;

        switch (((Player*)camera->focalActor)->transformation) {
            case PLAYER_FORM_DEKU:
                camMode = CAM_MODE_DEKUSHOOT;
                break;

            case PLAYER_FORM_GORON:
                camMode = CAM_MODE_GORONDASH;
                break;

            case PLAYER_FORM_ZORA:
                camMode = CAM_MODE_DEKUFLY;
                break;

            case PLAYER_FORM_FIERCE_DEITY:
                camMode = CAM_MODE_JUMP;
                break;

            default:
                camMode = CAM_MODE_NORMAL;
                break;
        }

        focalActorHeight = Camera_GetFocalActorHeight(camera) - (player->unk_AB8 * camera->focalActor->scale.y);
    } else {
        camMode = CAM_MODE_NORMAL;
        focalActorHeight = Camera_GetFocalActorHeight(camera);
    }

    if (rwData->unk_18 != *sp44) {
        camera->animState = 20;
        Camera_SetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
        Camera_UnsetStateFlag(camera, CAM_STATE_2 | CAM_STATE_CHECK_WATER);
        camera->play->view.unk164 = camera->camId | 0x50;
        return true;
    }

    Camera_UnsetStateFlag(camera, CAM_STATE_4);
    if (RELOAD_PARAMS(camera)) {
        values = sCameraSettings[camera->setting].cameraModes[camMode].values;
        new_var = -0.5f;
        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight *
                         ((1.0f + new_var) - ((68.0f / focalActorHeight) * new_var));
        roData->unk_04 = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight *
                         ((1.0f + new_var) - ((68.0f / focalActorHeight) * new_var));
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_SCALED_RO_DATA(values);
        roData->timer = GET_NEXT_RO_DATA(values);
    }

    sUpdateCameraDirection = true;
    sCameraInterfaceFlags = roData->interfaceFlags;

    spB0 = OLib_Vec3fDiffToVecGeo(at, eye);
    data = &D_801EDDD0;
    spA8 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    D_801EDDD0 = focalActorPosRot->pos;
    D_801EDDD0.y = data->y + focalActorHeight;
    temp_f0_2 = BgCheck_CameraRaycastFloor2(&camera->play->colCtx, &spC0, &bgId, &D_801EDDD0);

    if ((roData->unk_00 + data->y) < temp_f0_2) {
        D_801EDDD0.y = temp_f0_2 + 10.0f;
    } else {
        D_801EDDD0.y = roData->unk_00 + data->y;
    }

    if (roData->unk_10 != 0.0f) {
        D_801EDDE0 = (*data);
        spB8.r = roData->unk_10;
        spB8.pitch = 0;
        spB8.yaw = focalActorPosRot->rot.y;
        D_801EDDD0 = OLib_AddVecGeoToVec3f(&D_801EDDD0, &spB8);
    }

    sp9C = 0;

    switch (camera->animState) {
        case 0:
        case 20:
            spCC[sp9C] = camera->focalActor;
            sp9C++;
            Camera_SetUpdateRatesFastPitch(camera);
            Camera_UnsetStateFlag(camera, CAM_STATE_2 | CAM_STATE_CHECK_WATER);
            rwData->timer = roData->timer;
            rwData->unk_08 = focalActorPosRot->pos.y - camera->unk_0F0.y;

            switch (roData->interfaceFlags & (KEEPON4_FLAG_3 | KEEPON4_FLAG_2 | KEEPON4_FLAG_1)) {
                case KEEPON4_FLAG_1:
                    spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                    spA0 = BINANG_SUB(BINANG_ROT180(focalActorPosRot->rot.y), spA8.yaw) < 0
                               ? BINANG_ROT180(focalActorPosRot->rot.y) + CAM_DEG_TO_BINANG(roData->unk_0C)
                               : BINANG_ROT180(focalActorPosRot->rot.y) - CAM_DEG_TO_BINANG(roData->unk_0C);
                    break;

                case (KEEPON4_FLAG_3 | KEEPON4_FLAG_2):
                    spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                    pad1 = BINANG_ROT180(focalActorPosRot->rot.y);
                    spA0 = pad1 + CAM_DEG_TO_BINANG(roData->unk_0C);
                    break;

                case KEEPON4_FLAG_2:
                    spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                    spA0 = CAM_DEG_TO_BINANG(roData->unk_0C);
                    break;

                case (KEEPON4_FLAG_2 | KEEPON4_FLAG_1):
                    spA2 = spA8.pitch + CAM_DEG_TO_BINANG(roData->unk_08);
                    spA0 = spA8.yaw + CAM_DEG_TO_BINANG(roData->unk_0C);
                    break;

                case KEEPON4_FLAG_3:
                    if (camera->target != NULL) {
                        sp60 = Actor_GetWorldPosShapeRot(camera->target);
                        spA2 = CAM_DEG_TO_BINANG(roData->unk_08) - sp60.rot.x;
                        spA0 = (BINANG_SUB(BINANG_ROT180(sp60.rot.y), spA8.yaw) > 0)
                                   ? BINANG_ROT180(sp60.rot.y) + CAM_DEG_TO_BINANG(roData->unk_0C)
                                   : BINANG_ROT180(sp60.rot.y) - CAM_DEG_TO_BINANG(roData->unk_0C);

                        spCC[1] = camera->target;
                        sp9C++;
                        break;
                    }
                    // fallthrough
                case (KEEPON4_FLAG_3 | KEEPON4_FLAG_1):
                    if (camera->target != 0) {
                        sp4C = Actor_GetWorld(camera->target);
                        spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                        sp9E = Camera_CalcXZAngle(&sp4C.pos, &focalActorPosRot->pos);
                        spA0 = (BINANG_SUB(sp9E, spA8.yaw) > 0) ? sp9E + CAM_DEG_TO_BINANG(roData->unk_0C)
                                                                : sp9E - CAM_DEG_TO_BINANG(roData->unk_0C);
                        spCC[1] = camera->target;
                        sp9C++;
                        break;
                    }
                    // fallthrough
                case (KEEPON4_FLAG_3 | KEEPON4_FLAG_2 | KEEPON4_FLAG_1):
                    spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                    spA0 = spA8.yaw;
                    break;

                default:
                    spA2 = spA8.pitch;
                    spA0 = spA8.yaw;
                    break;
            }

            spB8.pitch = spA2;
            spB8.yaw = spA0;
            spB8.r = roData->unk_04;
            // Odd that the return is set to bgId and remains unused
            bgId = func_800CC260(camera, &D_801EDDF0, &D_801EDDD0, &spB8, spCC, sp9C);
            rwData->unk_04 = (f32)(s16)(spB8.pitch - spA8.pitch) / rwData->timer;
            rwData->unk_00 = (f32)(s16)(spB8.yaw - spA8.yaw) / rwData->timer;
            rwData->unk_10 = spA8.yaw;
            rwData->unk_12 = spA8.pitch;
            camera->animState++;
            rwData->unk_16 = 1;
            break;

        case 10:
            rwData->unk_08 = focalActorPosRot->pos.y - camera->unk_0F0.y;
            break;
    }

    if (roData->interfaceFlags & KEEPON4_FLAG_6) {
        if (rwData->timer != 0) {
            at->x += ((data->x - at->x) / rwData->timer);
            at->y += ((data->y - at->y) / rwData->timer);
            at->z += ((data->z - at->z) / rwData->timer);
            Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
        }
        camera->yOffsetUpdateRate = 0.25f;
        camera->xzOffsetUpdateRate = 0.25f;
        camera->atLerpStepScale = 0.0f;
    } else {
        camera->yOffsetUpdateRate = 0.25f;
        camera->xzOffsetUpdateRate = 0.25f;
        camera->atLerpStepScale = 0.75f;
        Camera_ScaledStepToCeilVec3f(data, at, 0.2f, 0.2f, 0.2f);
        camera->atLerpStepScale = 0.0f;
        Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    }

    camera->dist = Camera_ScaledStepToCeilF(roData->unk_04, camera->dist, 0.25f, 1.0f);
    spB8.r = camera->dist;

    if (rwData->timer != 0) {
        Camera_SetStateFlag(camera, CAM_STATE_DISABLE_MODE_CHANGE);
        rwData->unk_10 += TRUNCF_BINANG(rwData->unk_00);
        rwData->unk_12 += TRUNCF_BINANG(rwData->unk_04);
        rwData->timer--;
    } else {
        Camera_SetStateFlag(camera, CAM_STATE_10 | CAM_STATE_4);
    }

    spB8.yaw = Camera_ScaledStepToCeilS(rwData->unk_10, spA8.yaw, roData->unk_14, 5);
    spB8.pitch = Camera_ScaledStepToCeilS(rwData->unk_12, spA8.pitch, roData->unk_14, 5);
    *eyeNext = OLib_AddVecGeoToVec3f(at, &spB8);
    *eye = *eyeNext;
    func_800CBFA4(camera, at, eye, 3);

    camera->fov = Camera_ScaledStepToCeilF(roData->unk_18, camera->fov, camera->fovUpdateRate, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);

    return 1;
}

s32 Camera_KeepOn0(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Used for many fixed-based camera settings i.e. camera is fixed in rotation, and often position (but not always)
 */
s32 Camera_Fixed1(Camera* camera) {
    s32 pad[2];
    s32 yawDiff;
    VecGeo eyeOffset;
    VecGeo eyeAtOffset;
    VecGeo sp7C;
    u32 negOne;
    Vec3f adjustedPos;
    BgCamFuncData* bgCamFuncData;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    CameraModeValue* values;
    PosRot* targetHome;
    PosRot* targetWorld;
    VecGeo sp44;
    Fixed1ReadOnlyData* roData = &camera->paramData.fixd1.roData;
    Fixed1ReadWriteData* rwData = &camera->paramData.fixd1.rwData;

    sp7C = OLib_Vec3fDiffToVecGeo(at, eye);

    if (!RELOAD_PARAMS(camera)) {
    } else {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        bgCamFuncData = (BgCamFuncData*)Camera_GetBgCamOrActorCsCamFuncData(camera, camera->bgCamIndex);
        rwData->eyePosRotTarget.pos = Camera_Vec3sToVec3f(&bgCamFuncData->pos);

        rwData->eyePosRotTarget.rot = bgCamFuncData->rot;
        rwData->fov = bgCamFuncData->fov;
        rwData->focalActor = camera->focalActor;

        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight;
        roData->unk_04 = GET_NEXT_SCALED_RO_DATA(values);
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);

        if (roData->interfaceFlags & FIXED1_FLAG_4) {
            if (camera->target == NULL) {
                return false;
            }

            targetHome = &camera->target->home;
            targetWorld = &camera->target->world;

            sp44 = OLib_Vec3fDiffToVecGeo(&targetHome->pos, &rwData->eyePosRotTarget.pos);
            sp44.yaw = targetWorld->rot.y + (s16)(sp44.yaw - targetHome->rot.y);
            rwData->eyePosRotTarget.pos = OLib_AddVecGeoToVec3f(&targetWorld->pos, &sp44);
            yawDiff = (s16)(rwData->eyePosRotTarget.rot.y - targetHome->rot.y);
            rwData->eyePosRotTarget.rot.y = targetWorld->rot.y + yawDiff;
        }
    }

    negOne = -1;

    if (rwData->focalActor != camera->focalActor) {
        camera->animState = 20;
    }

    if (rwData->fov == (s32)negOne) {
        rwData->fov = roData->fov * 100;
    } else if (rwData->fov <= 360) {
        rwData->fov *= 100;
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        Camera_SetUpdateRatesSlow(camera);
        if (rwData->fov != (s32)negOne) {
            roData->fov = CAM_RODATA_UNSCALE(rwData->fov);
        }

        if (bgCamFuncData->unk_0E != (s32)negOne) {
            roData->unk_04 = CAM_RODATA_UNSCALE(bgCamFuncData->unk_0E);
        }
    }

    eyeAtOffset = OLib_Vec3fDiffToVecGeo(eye, at);
    Camera_ScaledStepToCeilVec3f(&rwData->eyePosRotTarget.pos, eye, roData->unk_04, roData->unk_04, 0.2f);
    adjustedPos = focalActorPosRot->pos;
    adjustedPos.y += focalActorHeight;
    camera->dist = OLib_Vec3fDist(&adjustedPos, eye);
    eyeOffset.r = camera->dist;
    eyeOffset.pitch =
        Camera_ScaledStepToCeilS(rwData->eyePosRotTarget.rot.x * -1, eyeAtOffset.pitch, roData->unk_04, 5);
    eyeOffset.yaw = Camera_ScaledStepToCeilS(rwData->eyePosRotTarget.rot.y, eyeAtOffset.yaw, roData->unk_04, 5);
    *at = OLib_AddVecGeoToVec3f(eye, &eyeOffset);
    camera->eyeNext = *eye;
    Camera_BgCheck(camera, eye, at);

    camera->fov = Camera_ScaledStepToCeilF(roData->fov, camera->fov, roData->unk_04, 0.1f);
    camera->roll = 0;
    camera->atLerpStepScale = 0.0f;
    Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    camera->roll = Camera_ScaledStepToCeilS(rwData->eyePosRotTarget.rot.z, camera->roll, roData->unk_04, 5);

    return 1;
}

/**
 * Used for many pivot-based camera settings i.e. camera fixed in position but free to rotate
 */
s32 Camera_Fixed2(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f spB0;
    Vec3f spA4;
    Vec3f sp98;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    f32 temp_f0_3;
    s32 pad;
    BgCamFuncData* bgCamFuncData;
    VecGeo sp80;
    PosRot* sp7C;
    PosRot* sp78;
    VecGeo sp70;
    Vec3f* focalActorPos;
    Vec3f* targetActorPos;
    Actor* actor;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    Fixed2ReadOnlyData* roData = &camera->paramData.fixd2.roData;
    Fixed2ReadWriteData* rwData = &camera->paramData.fixd2.rwData;

    if (!RELOAD_PARAMS(camera)) {
    } else {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->unk_00 =
            GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_10 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        rwData->unk_1C = roData->unk_14 * 100.0f;
        bgCamFuncData = (BgCamFuncData*)Camera_GetBgCamOrActorCsCamFuncData(camera, camera->bgCamIndex);
        if (bgCamFuncData != NULL) {
            if (!(roData->interfaceFlags & FIXED2_FLAG_1)) {
                rwData->unk_00 = Camera_Vec3sToVec3f(&bgCamFuncData->pos);
            } else {
                if (camera->focalActor != &GET_PLAYER(camera->play)->actor) {
                    Player* player = GET_PLAYER(camera->play);

                    sp70 = OLib_Vec3fDiffToVecGeo(&player->actor.focus.pos, eye);
                    if (sp70.r < roData->unk_04) {
                        sp70.r = roData->unk_04;
                        if (sp70.pitch < 0xBB8) { // 16.5 degrees
                            sp70.pitch = 0xBB8;
                        }
                        rwData->unk_00 = OLib_AddVecGeoToVec3f(&player->actor.focus.pos, &sp70);
                    } else {
                        rwData->unk_00 = *eye;
                    }
                } else {
                    rwData->unk_00 = camera->eye;
                }
            }

            if (bgCamFuncData->fov != -1) {
                if (roData->interfaceFlags & FIXED2_FLAG_7) {
                    rwData->unk_1C = (bgCamFuncData->fov >> 1) + (bgCamFuncData->fov >> 2);
                    if (rwData->unk_1C < 0x1E) {
                        rwData->unk_1C = 0x1E;
                    }
                } else {
                    rwData->unk_1C = bgCamFuncData->fov;
                }
            }

            if (bgCamFuncData->unk_0E != -1) {
                rwData->unk_0C = bgCamFuncData->unk_0E;
            } else {
                rwData->unk_0C = roData->unk_08;
            }

            if (bgCamFuncData->unk_10 != -1) {
                if (roData->interfaceFlags & FIXED2_FLAG_2) {
                    rwData->unk_14 = bgCamFuncData->unk_10 * 0.01f;
                    rwData->unk_18 = roData->unk_0C;
                } else {
                    temp_f0_3 = bgCamFuncData->unk_10 * 0.01f;
                    rwData->unk_18 = temp_f0_3;
                    rwData->unk_14 = temp_f0_3;
                }
            } else {
                rwData->unk_14 = roData->unk_10;
                rwData->unk_18 = roData->unk_0C;
            }
            if (roData->interfaceFlags & FIXED2_FLAG_4) {
                if (camera->target == 0) {
                    return 0;
                }
                sp7C = &camera->target->home;
                sp78 = &camera->target->world;
                sp70 = OLib_Vec3fDiffToVecGeo(&sp7C->pos, &rwData->unk_00);
                sp70.yaw = sp78->rot.y + (s16)(sp70.yaw - sp7C->rot.y);
                rwData->unk_00 = OLib_AddVecGeoToVec3f(&sp78->pos, &sp70);
            }
        } else {
            rwData->unk_00 = camera->eye;
            rwData->unk_0C = roData->unk_08;
            rwData->unk_14 = roData->unk_10;
            rwData->unk_18 = roData->unk_0C;
        }
        if (rwData->unk_1C <= 360) {
            rwData->unk_1C *= 100;
        }
        if (camera->animState == 20) {
            rwData->unk_14 = 0.2f;
        }
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (roData->interfaceFlags & FIXED2_FLAG_3) {
        if (camera->target == NULL) {
            return false;
        }

        spB0.x = camera->target->focus.pos.x;
        spB0.y = camera->target->focus.pos.y;
        spB0.z = camera->target->focus.pos.z;

        camera->focalActorAtOffset.x = spB0.x - focalActorPosRot->pos.x;
        camera->focalActorAtOffset.y = spB0.y - focalActorPosRot->pos.y;
        camera->focalActorAtOffset.z = spB0.z - focalActorPosRot->pos.z;
    } else if (roData->interfaceFlags & FIXED2_FLAG_6) {
        sp98.x = 0.0f;
        sp98.y = roData->unk_00 + focalActorHeight;
        sp98.z = 0.0f;

        if (camera->target != NULL) {
            targetActorPos = &camera->target->focus.pos;
            focalActorPos = &camera->focalActor->focus.pos;
            sp98.x += (targetActorPos->x - focalActorPos->x) * 0.4f;
            sp98.y += (targetActorPos->y - focalActorPos->y) * 0.4f;
            sp98.z += (targetActorPos->z - focalActorPos->z) * 0.4f;
        }

        Camera_ScaledStepToCeilVec3f(&sp98, &camera->focalActorAtOffset, 0.25f, 0.25f, 0.1f);
        spB0.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
        spB0.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
        spB0.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
    } else if (roData->interfaceFlags & FIXED2_FLAG_7) {
        sp98.x = 0.0f;
        sp98.y = roData->unk_00 + focalActorHeight;
        sp98.z = 0.0f;

        if (camera->target != NULL) {
            targetActorPos = &camera->target->focus.pos;
            focalActorPos = &camera->focalActor->focus.pos;
            sp98.x += (targetActorPos->x - focalActorPos->x) * 0.7f;
            sp98.y += (targetActorPos->y - focalActorPos->y) * 0.7f;
            sp98.z += (targetActorPos->z - focalActorPos->z) * 0.7f;
        }

        Camera_ScaledStepToCeilVec3f(&sp98, &camera->focalActorAtOffset, 0.25f, 0.25f, 0.1f);
        spB0.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
        spB0.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
        spB0.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
    } else {
        sp98.x = 0.0f;
        sp98.z = 0.0f;

        if ((((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_4000) ||
            (((Player*)camera->focalActor)->stateFlags1 & PLAYER_STATE1_4)) {
            sp98.y = roData->unk_00;
        } else {
            sp98.y = roData->unk_00 + focalActorHeight;
        }

        Camera_ScaledStepToCeilVec3f(&sp98, &camera->focalActorAtOffset, rwData->unk_14, rwData->unk_14, 0.1f);
        spB0.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
        spB0.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
        spB0.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
    }

    if (camera->animState == 0) {
        Camera_SetUpdateRatesSlow(camera);
        if (!(roData->interfaceFlags & FIXED2_FLAG_0)) {
            Vec3f sp44;

            *at = spB0;
            sp80 = OLib_Vec3fDiffToVecGeo(at, &rwData->unk_00);
            if ((rwData->unk_0C < sp80.r) || (roData->interfaceFlags & FIXED2_FLAG_5)) {
                sp80.r = rwData->unk_0C;
                spA4 = OLib_AddVecGeoToVec3f(at, &sp80);
            } else {
                if (sp80.r < roData->unk_04) {
                    sp80.r = roData->unk_04;
                    spA4 = OLib_AddVecGeoToVec3f(at, &sp80);
                } else {
                    spA4 = rwData->unk_00;
                }
            }
            sp44 = spA4;
            camera->eyeNext = sp44;
            *eye = sp44;
            camera->fov = rwData->unk_1C * 0.01f;
        }
    }

    Camera_ScaledStepToCeilVec3f(&spB0, at, rwData->unk_14, rwData->unk_14, 1.0f);
    sp80 = OLib_Vec3fDiffToVecGeo(at, &rwData->unk_00);
    if ((rwData->unk_0C < sp80.r) || (roData->interfaceFlags & FIXED2_FLAG_5)) {
        sp80.r = rwData->unk_0C;
        spA4 = OLib_AddVecGeoToVec3f(at, &sp80);
    } else if (sp80.r < roData->unk_04) {
        sp80.r = roData->unk_04;
        spA4 = OLib_AddVecGeoToVec3f(at, &sp80);
    } else {
        spA4 = rwData->unk_00;
    }

    Camera_ScaledStepToCeilVec3f(&spA4, eyeNext, rwData->unk_18, rwData->unk_18, 0.1f);
    *eye = *eyeNext;
    camera->dist = OLib_Vec3fDist(at, eye);
    camera->roll = 0;
    camera->xzSpeed = 0;
    rwData->unk_10 = rwData->unk_1C * 0.01f;
    camera->fov = Camera_ScaledStepToCeilF(rwData->unk_10, camera->fov, rwData->unk_14, 0.1f);
    camera->atLerpStepScale = Camera_ClampLerpScale(camera, 1.0f);
    Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    camera->animState = 1;

    return true;
}

/**
 * Used by the unused PREREND_FIXED Camera Setting. Remnant of OoT
 */
s32 Camera_Fixed3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Fixed4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Fixed0(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * First person view
 */
s32 Camera_Subject1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp90;
    Vec3f sp84;
    VecGeo sp7C;
    VecGeo sp74;
    VecGeo tgeo;
    PosRot focalActorFocus = Actor_GetFocus(camera->focalActor);
    f32 sp54;
    f32 temp_f0_2;
    s16 sp4E;
    s16 sp4C;
    Subject1ReadOnlyData* roData = &camera->paramData.subj1.roData;
    Subject1ReadWriteData* rwData = &camera->paramData.subj1.rwData;
    CameraModeValue* values;
    f32 temp_f0;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);

    Camera_SetUpdateRatesFastPitch(camera);

    values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
    roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight;
    roData->unk_04 = GET_NEXT_RO_DATA(values);
    roData->unk_08 = GET_NEXT_RO_DATA(values);
    roData->unk_0C = GET_NEXT_RO_DATA(values);
    roData->unk_10 = GET_NEXT_RO_DATA(values) * 0.1f;
    roData->unk_14 = GET_NEXT_RO_DATA(values) * 0.1f;
    roData->unk_18 = GET_NEXT_RO_DATA(values) * 0.1f;
    roData->unk_1C = GET_NEXT_RO_DATA(values);
    roData->interfaceFlags = GET_NEXT_RO_DATA(values);

    sp7C.r = roData->unk_08;
    sp7C.yaw = BINANG_ROT180(focalActorFocus.rot.y);
    sp7C.pitch = focalActorFocus.rot.x;
    sp90 = focalActorFocus.pos;
    sp90.y += roData->unk_00;

    sp84 = OLib_AddVecGeoToVec3f(&sp90, &sp7C);
    sp74 = OLib_Vec3fDiffToVecGeo(&camera->at, eye);
    sCameraInterfaceFlags = roData->interfaceFlags;

    if (camera->play->view.unk164 == 0) {
        camera->play->view.unk164 = camera->camId | 0x50;
        return true;
    }

    if (RELOAD_PARAMS(camera)) {
        rwData->unk_00 = sp74.r;
        rwData->unk_04 = sp74.yaw;
        rwData->unk_06 = sp74.pitch;

        if (roData->interfaceFlags & SUBJECT1_FLAG_4) {
            camera->fov = roData->unk_1C;
            rwData->timer = 0;
        } else {
            rwData->timer = 6;
        }
        camera->dist = roData->unk_08;
        camera->rUpdateRateInv = 1.0f;
        camera->animState++;
        camera->dist = roData->unk_08;
    }

    tgeo.r = rwData->unk_00;
    tgeo.pitch = rwData->unk_04;
    tgeo.yaw = rwData->unk_06;

    if (rwData->timer != 0) {
        temp_f0 = (1.0f / rwData->timer);

        VEC3F_LERPIMPDST(at, at, &sp90, temp_f0);

        sp54 = ((tgeo.r - sp7C.r) * (1.0f / 6.0f));
        sp4E = BINANG_SUB(tgeo.pitch, sp7C.yaw) * (1.0f / 6.0f);
        sp4C = BINANG_SUB(tgeo.yaw, sp7C.pitch) * (1.0f / 6.0f);

        sp74.r = Camera_ScaledStepToCeilF((rwData->timer * sp54) + sp7C.r, sp74.r, 0.5f, 1.0f);
        sp74.yaw = Camera_ScaledStepToCeilS(sp7C.yaw + (sp4E * rwData->timer), sp74.yaw, 0.5f, 5);
        sp74.pitch = Camera_ScaledStepToCeilS(sp7C.pitch + (sp4C * rwData->timer), sp74.pitch, 0.5f, 5);

        *eyeNext = OLib_AddVecGeoToVec3f(at, &sp74);
        *eye = *eyeNext;
        rwData->timer--;
    } else {
        sp54 = Math_SinS(-focalActorFocus.rot.x);
        temp_f0_2 = Math_CosS(-focalActorFocus.rot.x);

        sp90.x = roData->unk_10;
        sp90.y = (roData->unk_14 * temp_f0_2) - (roData->unk_18 * sp54);
        sp90.z = (roData->unk_14 * sp54) + (roData->unk_18 * temp_f0_2);

        sp54 = Math_SinS(BINANG_ROT180(focalActorFocus.rot.y));
        temp_f0_2 = Math_CosS(BINANG_ROT180(focalActorFocus.rot.y));

        roData->unk_10 = (sp90.z * sp54) + (sp90.x * temp_f0_2);
        roData->unk_14 = sp90.y;
        roData->unk_18 = (sp90.z * temp_f0_2) - (sp90.x * sp54);

        at->x = roData->unk_10 + focalActorFocus.pos.x;
        at->y = roData->unk_14 + focalActorFocus.pos.y;
        at->z = roData->unk_18 + focalActorFocus.pos.z;

        sp74.r = roData->unk_08;
        sp74.yaw = BINANG_ROT180(focalActorFocus.rot.y);
        sp74.pitch = focalActorFocus.rot.x;

        camera->eyeNext = OLib_AddVecGeoToVec3f(at, &sp74);
        sp74.r = roData->unk_04;
        *eye = OLib_AddVecGeoToVec3f(at, &sp74);
    }

    // TODO: is skyboxDisabled accurate here?
    if (camera->play->envCtx.skyboxDisabled == 0) {
        Camera_BgCheck(camera, at, eye);
    } else {
        func_800CBFA4(camera, at, eye, 3);
    }

    camera->fov = Camera_ScaledStepToCeilF(roData->unk_1C, camera->fov, 0.25f, 0.1f);
    camera->roll = 0;
    camera->atLerpStepScale = 0.0f;

    return true;
}

s32 Camera_Subject2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Subject3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Subject4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Subject0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data1(Camera* camera) {
    return Camera_Normal1(camera);
}

s32 Camera_Data2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Data4(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Unique1(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Player diving from the surface of the water to underwater not as zora
 * Also used when using a door to leave a scene
 */
s32 Camera_Unique2(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    f32 phi_f16;
    Vec3f sp70;
    VecGeo sp68;
    VecGeo sp60;
    s32 pad[3];
    Unique2ReadOnlyData* roData = &camera->paramData.uniq2.roData;
    Unique2ReadWriteData* rwData = &camera->paramData.uniq2.rwData;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    Vec3f* eyeNext = &camera->eyeNext;

    sp60 = OLib_Vec3fDiffToVecGeo(at, eye);

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->unk_00 =
            GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * (0.8f - ((68.0f / focalActorHeight) * -0.2f));
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->unk_08 = 0.25f;
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    if ((camera->animState == 0) || (rwData->unk_04 != roData->interfaceFlags)) {
        rwData->unk_04 = roData->interfaceFlags;
    }

    if (camera->animState == 0) {
        camera->animState = 1;
        Camera_SetUpdateRatesSlow(camera);
        rwData->unk_00 = 200.0f;
    }

    if (roData->interfaceFlags & UNIQUE2_FLAG_4) {
        *eyeNext = *eye;
        Camera_UnsetStateFlag(camera, CAM_STATE_2);
    }

    sp70 = camera->focalActorPosRot.pos;

    if (roData->interfaceFlags & UNIQUE2_FLAG_0) {
        phi_f16 = 1.0f;
    } else {
        phi_f16 = camera->speedRatio;
    }

    at->x += (sp70.x - at->x) * phi_f16 * 0.3f;
    at->y += ((sp70.y + focalActorHeight + roData->unk_00) - at->y) * 0.2f;
    at->z += (sp70.z - at->z) * phi_f16 * 0.3f;

    rwData->unk_00 = F32_LERPIMP(rwData->unk_00, 2.0f, 0.05f);

    if (roData->interfaceFlags & UNIQUE2_FLAG_0) {
        sp68 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
        sp68.r = roData->unk_04;
        sp70 = OLib_AddVecGeoToVec3f(at, &sp68);
        Camera_ScaledStepToCeilVec3f(&sp70, eye, roData->unk_08, roData->unk_08, 0.2f);
    } else if (roData->interfaceFlags & UNIQUE2_FLAG_1) {
        if (OLib_Vec3fDistXZ(at, eyeNext) < roData->unk_04) {
            sp68 = OLib_Vec3fDiffToVecGeo(at, eyeNext);
            sp68.yaw = Camera_ScaledStepToCeilS(sp68.yaw, sp60.yaw, 0.1f, 5);
            sp68.r = roData->unk_04;
            sp68.pitch = 0;
            *eye = OLib_AddVecGeoToVec3f(at, &sp68);
            eye->y = eyeNext->y;
        } else {
            Camera_ScaledStepToCeilVec3f(eyeNext, eye, roData->unk_08, roData->unk_08, 0.2f);
        }
    }

    if (!(roData->interfaceFlags & UNIQUE2_FLAG_5)) {
        Camera_BgCheck(camera, at, eye);
    }

    camera->dist = OLib_Vec3fDist(at, eye);
    camera->roll = 0;
    camera->fov = Camera_ScaledStepToCeilF(roData->unk_0C, camera->fov, 0.2f, 0.1f);
    camera->atLerpStepScale = Camera_ClampLerpScale(camera, 1.0f);

    return true;
}

/**
 * Woodfall inside the Swamp, used for normal camModes and derivative modes
 */
s32 Camera_Unique3(Camera* camera) {
    s32 ret = Camera_Normal1(camera);

    if ((camera->play->sceneId == SCENE_21MITURINMAE) && (camera->eye.y < 5.0f)) {
        camera->eye.y = 5.0f;
    }
    return ret;
}

/**
 * Woodfall inside the Swamp, used for parallel camModes and derivative modes
 */
s32 Camera_Unique4(Camera* camera) {
    s32 ret = Camera_Parallel1(camera);

    if ((camera->play->sceneId == SCENE_21MITURINMAE) && (camera->eye.y < 5.0f)) {
        camera->eye.y = 5.0f;
    }
    return ret;
}

/**
 * Woodfall inside the Swamp, used for battle camMode
 */
s32 Camera_Unique5(Camera* camera) {
    s32 ret = Camera_Battle1(camera);

    if ((camera->play->sceneId == SCENE_21MITURINMAE) && (camera->eye.y < 5.0f)) {
        camera->eye.y = 5.0f;
    }
    return ret;
}

/**
 * Entering a room or scene (camera settings START0/START1/START2)
 */
s32 Camera_Unique0(Camera* camera) {
    f32 playerHeight = Player_GetHeight((Player*)camera->focalActor);
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    PosRot sp9C;
    Player* player = (Player*)camera->focalActor;
    Vec3f sp8C;
    VecGeo sp84;
    VecGeo sp7C;
    BgCamFuncData* bgCamFuncData;
    f32 sp74;
    s32 pad;
    s16 temp_v1;
    Unique0ReadOnlyData* roData = &camera->paramData.uniq0.roData;
    Unique0ReadWriteData* rwData = &camera->paramData.uniq0.rwData;

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->unk_00 = GET_NEXT_RO_DATA(values);
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sp7C = OLib_Vec3fDiffToVecGeo(&camera->at, &camera->eye);

    if (player->rideActor != NULL) {
        sp9C = Actor_GetWorld(player->rideActor);
        sp8C = sp9C.pos;
        sp8C.y += playerHeight + 20.0f;
    } else {
        sp8C = camera->focalActorPosRot.pos;
        sp8C.y += playerHeight;
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
            bgCamFuncData = (BgCamFuncData*)Camera_GetBgCamOrActorCsCamFuncData(camera, camera->bgCamIndex);
            rwData->unk_1C.point = Camera_Vec3sToVec3f(&bgCamFuncData->pos);
            camera->eye = camera->eyeNext = rwData->unk_1C.point;
            rwData->unk_34 = bgCamFuncData->rot;

            temp_v1 = bgCamFuncData->fov;
            if (temp_v1 != -1) {
                if (temp_v1 <= 360) {
                    camera->fov = temp_v1;
                } else {
                    camera->fov = temp_v1 * 0.01f;
                }
            }

            rwData->timer = bgCamFuncData->unk_0E;
            if (rwData->timer == -1) {
                rwData->timer = 60;
            }

            if (bgCamFuncData->unk_10 != -1) {
                rwData->unk_18 = bgCamFuncData->unk_10 * 0.01f;
            } else {
                rwData->unk_18 = roData->unk_04 * 0.01f;
            }

            rwData->unk_00 = focalActorPosRot->pos;
            if (roData->interfaceFlags & UNIQUE0_FLAG_4) {
                rwData->unk_0C.x = focalActorPosRot->pos.x;
                rwData->unk_0C.y = focalActorPosRot->pos.y + playerHeight + roData->unk_00;
                rwData->unk_0C.z = focalActorPosRot->pos.z;
            }
            rwData->unk_3A = camera->focalActor->world.rot.y;
            rwData->unk_3E = 0;
            camera->eye = camera->eyeNext = rwData->unk_1C.point;
            Camera_UnsetStateFlag(camera, CAM_STATE_2);
            camera->animState++;
            // fallthrough
        case 1:
            sp84.r = OLib_Vec3fDist(&sp8C, &camera->eye);
            sp84.yaw = rwData->unk_34.y;
            sp84.pitch = -rwData->unk_34.x;
            rwData->unk_1C.dir = OLib_VecGeoToVec3f(&sp84);
            Math3D_LineClosestToPoint(&rwData->unk_1C, &focalActorPosRot->pos, &rwData->unk_0C);
            camera->at = rwData->unk_0C;

            if (player->stateFlags1 & PLAYER_STATE1_20000000) {
                rwData->unk_00 = focalActorPosRot->pos;
            }

            if (roData->interfaceFlags & UNIQUE0_FLAG_4) {
                Vec3f sp54;

                sp54.x = focalActorPosRot->pos.x;
                sp54.y = focalActorPosRot->pos.y + playerHeight + roData->unk_00;
                sp54.z = focalActorPosRot->pos.z;
                Camera_ScaledStepToCeilVec3f(&sp54, &camera->at, camera->xzOffsetUpdateRate, camera->yOffsetUpdateRate,
                                             0.1f);
                camera->yOffsetUpdateRate =
                    Camera_ScaledStepToCeilF(rwData->unk_18, camera->yOffsetUpdateRate, 0.1f, 0.0001f);
                camera->xzOffsetUpdateRate =
                    Camera_ScaledStepToCeilF(rwData->unk_18, camera->xzOffsetUpdateRate, 0.1f, 0.0001f);
            }

            if ((camera->animState == 999) || (camera->animState == 666)) {
                rwData->unk_3E = 2;
            }

            if (roData->interfaceFlags & UNIQUE0_FLAG_0) {
                if (rwData->timer > 0) {
                    rwData->timer--;
                    rwData->unk_00 = focalActorPosRot->pos;
                } else if (!(player->stateFlags1 & PLAYER_STATE1_20000000) &&
                           ((OLib_Vec3fDistXZ(&focalActorPosRot->pos, &rwData->unk_00) >= 10.0f) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_A) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_B) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CUP) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CDOWN) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CLEFT) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CRIGHT) ||
                            // #region 2S2H [Dpad]
                            CHECK_BTN_DPAD(CONTROLLER1(&camera->play->state)->press.button) ||
                            // #endregion
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_Z) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_L) ||
                            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_R) ||
                            (roData->interfaceFlags & UNIQUE0_FLAG_1))) {
                    rwData->unk_3E = 1;
                }
            } else {
                if (rwData->timer > 0) {
                    rwData->timer--;
                    if (rwData->timer == 0) {
                        sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
                    }
                } else {
                    rwData->unk_00 = focalActorPosRot->pos;
                }

                if (!(player->stateFlags1 & PLAYER_STATE1_20000000)) { // TODO: Merge into 1 if-statement
                    if ((rwData->unk_3A != camera->focalActor->world.rot.y) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_A) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_B) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CUP) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CDOWN) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CLEFT) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CRIGHT) ||
                        // #region 2S2H [Dpad]
                        CHECK_BTN_DPAD(CONTROLLER1(&camera->play->state)->press.button) ||
                        // #endregion
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_Z) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_L) ||
                        CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_R) ||
                        (roData->interfaceFlags & UNIQUE0_FLAG_1)) {
                        rwData->unk_3E = 1;
                    }
                }
            }
            break;

        case 3:
            sp74 = 1.0f / rwData->timer;
            sp8C.y -= playerHeight * 0.2f;

            Camera_ScaledStepToCeilVec3f(&sp8C, &camera->at, sp74, sp74, 0.001f);

            sp84.r = OLib_Vec3fDist(&camera->at, &camera->eye);
            sp84.pitch = sp7C.pitch;
            sp84.yaw = sp7C.yaw;
            sp84.r = Camera_ScaledStepToCeilF(100.0f, sp84.r, sp74, 1.0f);

            sp8C = OLib_AddVecGeoToVec3f(&camera->at, &sp84);

            sp74 = (QREG(64) * 0.01f) + 0.2f;

            Camera_ScaledStepToCeilVec3f(&sp8C, &camera->eye, sp74, sp74, 0.001f);

            rwData->timer--;
            if (rwData->timer == 0) {
                rwData->unk_3E = 9;
            }
            break;

        default:
            break;
    }

    if ((rwData->unk_3E == 1) || (rwData->unk_3E == 2) || (rwData->unk_3E == 9)) {
        camera->dist = OLib_Vec3fDist(&camera->at, &camera->eye);
        camera->atLerpStepScale = Camera_ClampLerpScale(camera, 1.0f);

        Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);

        camera->atLerpStepScale = 0.0f;

        Camera_ChangeSettingFlags(camera, camera->prevSetting, CAM_CHANGE_SETTING_1);
        Camera_SetStateFlag(camera, CAM_STATE_2);

        camera->yawUpdateRateInv = 1200.0f;
        camera->pitchUpdateRateInv = 1200.0f;
        camera->yOffsetUpdateRate = 0.001f;
        camera->xzOffsetUpdateRate = 0.001f;
        camera->fovUpdateRate = 0.01f;
    }

    return true;
}

/**
 * Does the minimal amount of camera update.
 * Allows for manual control of camera parameters without interference from update
 */
s32 Camera_Unique6(Camera* camera) {
    f32 focalActorHeight;
    CameraModeValue* values;
    Vec3f playerPosDisp;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Unique6ReadOnlyData* roData = &camera->paramData.uniq6.roData;

    if (RELOAD_PARAMS(camera)) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        Camera_SetUpdateRatesFastPitch(camera);
    }

    if (camera->focalActor != NULL) {
        focalActorHeight = Camera_GetFocalActorHeight(camera);
        playerPosDisp = focalActorPosRot->pos;
        playerPosDisp.y += focalActorHeight;
        camera->dist = OLib_Vec3fDist(&playerPosDisp, &camera->eye);
        Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    } else {
        camera->dist = OLib_Vec3fDist(&camera->at, &camera->eye);
    }

    if (roData->interfaceFlags & UNIQUE6_FLAG_0) {
        if (camera->timer > 0) {
            camera->timer--;
        }
    }

    return true;
}

/**
 * Used by the unused PREREND_PIVOT Camera Setting. Remnant of OoT
 */
s32 Camera_Unique7(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Unique8(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Unique9(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Used by unknown settings DEMO0 and ATTENTION
 */
s32 Camera_Demo1(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* targetPosRot = &camera->targetPosRot;
    f32 temp_f0;
    Actor* sp98[1];
    s16 screenPosX;
    s16 screenPosY;
    s32 phi_v0;
    VecGeo sp88;
    PosRot sp74;
    PosRot targetHead;
    Demo1ReadOnlyData* roData = &camera->paramData.demo1.roData;
    Demo1ReadWriteData* rwData = &camera->paramData.demo1.rwData;
    s32 pad;

    if (camera->animState == 0) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    if (camera->animState == 0) {
        rwData->unk_1C = 0;
        rwData->unk_0C = OLib_Vec3fDiffToVecGeo(&camera->targetPosRot.pos, eye);
        rwData->unk_14 = OLib_Vec3fDiffToVecGeo(&camera->at, eye);
        targetHead = Actor_GetFocus(camera->target);
        sp74 = Actor_GetWorld(camera->target);
        camera->targetPosRot.pos.x = (sp74.pos.x + targetHead.pos.x) * 0.5f;
        camera->targetPosRot.pos.y = (sp74.pos.y + targetHead.pos.y) * 0.5f;
        camera->targetPosRot.pos.z = (sp74.pos.z + targetHead.pos.z) * 0.5f;
        camera->targetPosRot.rot = targetHead.rot;
        camera->animState++;
    }

    Actor_GetScreenPos(camera->play, camera->target, &screenPosX, &screenPosY);

    temp_f0 = rwData->unk_0C.r;
    if ((screenPosX > 20) && (screenPosX < (SCREEN_WIDTH - 20)) && (screenPosY > 40) &&
        (screenPosY < (SCREEN_HEIGHT - 40))) {
        if (temp_f0 < 700.0f) {
            phi_v0 = 0;
        } else {
            phi_v0 = 1;
        }
    } else if (temp_f0 < 50.0f) {
        phi_v0 = 2;
    } else if (temp_f0 < 300.0f) {
        phi_v0 = 3;
    } else if (temp_f0 < 700.0f) {
        phi_v0 = 4;
    } else {
        phi_v0 = 5;
    }

    if (camera->target->category == ACTORCAT_DOOR) {
        if ((phi_v0 > 0) && (phi_v0 < 5)) {
            phi_v0 = 6;
        } else if (phi_v0 >= 5) {
            phi_v0 = 7;
        }
    }

    if (camera->target->category == ACTORCAT_CHEST) {
        if ((phi_v0 > 1) && (phi_v0 < 5)) {
            phi_v0 = 8;
        }
    }

    switch (phi_v0) {
        case 1:
            Camera_ScaledStepToCeilVec3f(&camera->targetPosRot.pos, at, 0.1f, 0.1f, 0.1f);
            sp88 = OLib_Vec3fDiffToVecGeo(at, eye);
            sp88.r = rwData->unk_0C.r;
            *eyeNext = OLib_AddVecGeoToVec3f(at, &sp88);
            *eye = *eyeNext;
            Camera_BgCheck(camera, at, eye);
            break;

        case 2:
            Camera_ScaledStepToCeilVec3f(&camera->targetPosRot.pos, &camera->at, 0.1f, 0.1f, 0.1f);
            break;

        case 3:
            if (rwData->unk_1C == 0) {
                sp98[0] = camera->target;
                func_800CC260(camera, &rwData->unk_00, &targetPosRot->pos, &rwData->unk_14, sp98, 1);
            }
            Camera_ScaledStepToCeilVec3f(&targetPosRot->pos, at, 0.1f, 0.1f, 0.1f);
            Camera_ScaledStepToCeilVec3f(&rwData->unk_00, eyeNext, 0.1f, 0.1f, 0.1f);
            *eye = *eyeNext;
            Camera_BgCheck(camera, at, eye);
            break;

        case 4:
            if (rwData->unk_1C == 0) {
                sp98[0] = camera->target;
                rwData->unk_14.r = rwData->unk_0C.r;
                func_800CC260(camera, &rwData->unk_00, &targetPosRot->pos, &rwData->unk_14, sp98, 1);
            }
            Camera_ScaledStepToCeilVec3f(&targetPosRot->pos, at, 0.1f, 0.1f, 0.1f);
            Camera_ScaledStepToCeilVec3f(&rwData->unk_00, eyeNext, 0.1f, 0.1f, 0.1f);
            *eye = *eyeNext;
            Camera_BgCheck(camera, at, eye);
            break;

        case 5:
            eyeNext = &targetPosRot->pos;
            if (rwData->unk_1C == 0) {
                sp98[0] = camera->target;
                func_800CC260(camera, &rwData->unk_00, &targetPosRot->pos, &rwData->unk_0C, sp98, 1);
            }
            *at = *eyeNext;
            *eye = camera->eyeNext = rwData->unk_00;
            break;

        case 6:
            if (rwData->unk_1C == 0) {
                rwData->unk_0C.yaw = camera->target->shape.rot.y;
                sp98[0] = camera->target;
                rwData->unk_0C.r = rwData->unk_14.r;
                func_800CC260(camera, &rwData->unk_00, &targetPosRot->pos, &rwData->unk_0C, sp98, 1);
            }
            Camera_ScaledStepToCeilVec3f(&targetPosRot->pos, at, 0.1f, 0.1f, 0.1f);
            Camera_ScaledStepToCeilVec3f(&rwData->unk_00, eyeNext, 0.1f, 0.1f, 0.1f);
            *eye = *eyeNext;
            Camera_BgCheck(camera, at, eye);
            break;

        case 7:
            if (rwData->unk_1C == 0) {
                rwData->unk_0C.yaw = camera->target->shape.rot.y;
                sp98[0] = camera->target;
                func_800CC260(camera, &rwData->unk_00, &targetPosRot->pos, &rwData->unk_0C, sp98, 1);
            }
            *at = targetPosRot->pos;
            *eye = camera->eyeNext = rwData->unk_00;
            break;

        case 8:
            if (rwData->unk_1C == 0) {
                rwData->unk_0C.yaw = camera->target->shape.rot.y;
                sp98[0] = camera->target;
                func_800CC260(camera, &rwData->unk_00, &targetPosRot->pos, &rwData->unk_0C, sp98, 1);
            }
            Camera_ScaledStepToCeilVec3f(&targetPosRot->pos, at, 0.1f, 0.1f, 0.1f);
            Camera_ScaledStepToCeilVec3f(&rwData->unk_00, eyeNext, 0.1f, 0.1f, 0.1f);
            *eye = *eyeNext;
            Camera_BgCheck(camera, at, eye);
            break;

        default:
            break;
    }

    rwData->unk_1C++;

    return true;
}

// Data for opening chests (default)
VecGeo D_801B9E64[] = {
    { 50.0f, 0xEE3A, 0xD558 },
    { 75.0f, 0x0000, 0x8008 },
    { 80.0f, 0xEE3A, 0x8008 },
    { 15.0f, 0xEE3A, 0x8008 },
};
Vec3f D_801B9E84[] = {
    { 0.0f, 40.0f, 20.0f },
    { 0.0f, 40.0f, 0.0f },
    { 0.0f, 3.0f, -3.0f },
    { 0.0f, 3.0f, -3.0f },
};

// Data for opening chests (goron)
VecGeo D_801B9EB4[] = {
    { 60.0f, 0xEE3A, 0xD558 },
    { 95.0f, 0x0000, 0x8008 },
    { 90.0f, 0xEE3A, 0x8008 },
    { 20.0f, 0xEE3A, 0x8008 },
};
Vec3f D_801B9ED4[] = {
    { 0.0f, 40.0f, 20.0f },
    { 0.0f, 40.0f, 0.0f },
    { 0.0f, 3.0f, -3.0f },
    { 0.0f, 3.0f, -3.0f },
};

/**
 * Opening large chests.
 * The camera position will be at a fixed point, and rotate around at different intervals.
 * The direction, and initial position is dependent on when the camera was started.
 */
s32 Camera_Demo2(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    VecGeo atToEye;
    VecGeo eyeOffset;
    VecGeo atOffset;
    Vec3f sp7C;
    Vec3f sp70;
    Vec3f sp64;
    f32 sp60;
    s32 pad;
    u8 skipUpdateEye = false;
    f32 playerHeight = Player_GetHeight((Player*)camera->focalActor);
    s16 angle;
    VecGeo* sp4C = D_801B9E64;
    Vec3f* sp48 = D_801B9E84;
    Actor* focalActor = camera->focalActor;
    Demo2ReadOnlyData* roData = &camera->paramData.demo2.roData;
    Demo2ReadWriteData* rwData = &camera->paramData.demo2.rwData;

    if (((focalActor == &GET_PLAYER(camera->play)->actor) &&
         (((Player*)focalActor)->transformation == PLAYER_FORM_GORON))) {
        sp4C = D_801B9EB4;
        sp48 = D_801B9ED4;
    }

    Camera_UnsetStateFlag(camera, CAM_STATE_4);

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->fov = GET_NEXT_RO_DATA(values);
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    atToEye = OLib_Vec3fDiffToVecGeo(at, eye);

    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
            Camera_UnsetStateFlag(camera, CAM_STATE_3 | CAM_STATE_2);
            Camera_SetUpdateRatesSlow(camera);
            camera->fov = roData->fov;
            camera->roll = rwData->animFrame = 0;
            rwData->initialAt = focalActorPosRot->pos;
            if (camera->focalActorFloorHeight != BGCHECK_Y_MIN) {
                rwData->initialAt.y = camera->focalActorFloorHeight;
            }
            angle = focalActorPosRot->rot.y;
            sp70.x = rwData->initialAt.x + (Math_SinS(angle) * 40.0f);
            sp70.y = rwData->initialAt.y + 40.0f;
            sp70.z = rwData->initialAt.z + (Math_CosS(angle) * 40.0f);
            if (camera->play->state.frames & 2) {
                angle -= 0x4000;
                rwData->yawDir = 1;
            } else {
                angle += 0x4000;
                rwData->yawDir = -1;
            }

            sp7C.x = sp70.x + (sp4C[1].r * Math_SinS(angle));
            sp7C.y = rwData->initialAt.y + 5.0f;
            sp7C.z = sp70.z + (sp4C[1].r * Math_CosS(angle));
            if (Camera_BgCheck(camera, &sp70, &sp7C)) {
                rwData->yawDir = -rwData->yawDir;
            }

            atOffset = OLib_Vec3fToVecGeo(&sp48[0]);
            atOffset.yaw += focalActorPosRot->rot.y;
            *at = OLib_AddVecGeoToVec3f(&rwData->initialAt, &atOffset);

            eyeOffset.r = sp4C[0].r;
            eyeOffset.pitch = sp4C[0].pitch;
            eyeOffset.yaw = (sp4C[0].yaw * rwData->yawDir) + focalActorPosRot->rot.y;

            rwData->unk_0C = 1.0f;
            break;

        case 1:
            // This is the case taken for nearly the entire cutscene
            sp60 = (rwData->animFrame - 2) * (1.0f / 146.0f);

            VEC3F_LERPIMPDST(&sp64, &sp48[0], &sp48[1], sp60);

            atOffset = OLib_Vec3fToVecGeo(&sp64);
            atOffset.yaw = (atOffset.yaw * rwData->yawDir) + focalActorPosRot->rot.y;
            *at = OLib_AddVecGeoToVec3f(&rwData->initialAt, &atOffset);

            atOffset.r = LERPIMP(sp4C[0].r, sp4C[1].r, sp60);
            atOffset.pitch = BINANG_LERPIMP(sp4C[0].pitch, sp4C[1].pitch, sp60);
            atOffset.yaw = BINANG_LERPIMP(sp4C[0].yaw, sp4C[1].yaw, sp60);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * rwData->yawDir) + focalActorPosRot->rot.y;

            rwData->unk_0C -= (1.0f / 365.0f);
            break;

        case 2:
            sp60 = (rwData->animFrame - 148) * 0.1f;

            sp64.x = LERPIMP(sp48[1].x, sp48[2].x, sp60);
            sp64.y = LERPIMP((sp48[1].y - playerHeight), sp48[2].y, sp60);
            sp64.y += playerHeight;
            sp64.z = LERPIMP(sp48[1].z, sp48[2].z, sp60);

            atOffset = OLib_Vec3fToVecGeo(&sp64);
            atOffset.yaw = (atOffset.yaw * rwData->yawDir) + focalActorPosRot->rot.y;
            *at = OLib_AddVecGeoToVec3f(&rwData->initialAt, &atOffset);

            atOffset.r = LERPIMP(sp4C[1].r, sp4C[2].r, sp60);
            atOffset.pitch = BINANG_LERPIMP(sp4C[1].pitch, sp4C[2].pitch, sp60);
            atOffset.yaw = BINANG_LERPIMP(sp4C[1].yaw, sp4C[2].yaw, sp60);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * rwData->yawDir) + focalActorPosRot->rot.y;

            rwData->unk_0C -= 0.04f;
            break;

        case 3:
            sp60 = (rwData->animFrame - 159) * (1.0f / 9.0f);

            sp64.x = LERPIMP(sp48[2].x, sp48[3].x, sp60);
            sp64.y = LERPIMP(sp48[2].y, sp48[3].y, sp60);
            sp64.y += playerHeight;
            sp64.z = LERPIMP(sp48[2].z, sp48[3].z, sp60);

            atOffset = OLib_Vec3fToVecGeo(&sp64);
            atOffset.yaw = (atOffset.yaw * rwData->yawDir) + focalActorPosRot->rot.y;
            *at = OLib_AddVecGeoToVec3f(&rwData->initialAt, &atOffset);

            atOffset.r = LERPIMP(sp4C[2].r, sp4C[3].r, sp60);
            atOffset.pitch = BINANG_LERPIMP(sp4C[2].pitch, sp4C[3].pitch, sp60);
            atOffset.yaw = BINANG_LERPIMP(sp4C[2].yaw, sp4C[3].yaw, sp60);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * rwData->yawDir) + focalActorPosRot->rot.y;

            rwData->unk_0C += (4.0f / 45.0f);
            break;

        case 30:
            Camera_SetStateFlag(camera, CAM_STATE_10);
            if (camera->stateFlags & CAM_STATE_3) {
                camera->animState = 4;
            }
            // fallthrough
        case 10:
        case 20:
            // Taken on the 1st and 158th animation frame
            skipUpdateEye = true;
            break;
        case 4:
            eyeOffset.r = 80.0f;
            eyeOffset.pitch = 0;
            eyeOffset.yaw = atToEye.yaw;
            rwData->unk_0C = 0.1f;
            sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A, 0);

            if (!(((rwData->animFrame < 0) || (camera->xzSpeed > 0.001f) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_A) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_B) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CUP) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CDOWN) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CLEFT) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CRIGHT) ||
                   // #region 2S2H [Dpad]
                   CHECK_BTN_DPAD(CONTROLLER1(&camera->play->state)->press.button) ||
                   // #endregion
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_Z) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_L) ||
                   CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_R)) &&
                  (camera->stateFlags & CAM_STATE_3))) {
                goto skipeyeUpdate;
            }
            // fallthrough
        default:
            Camera_SetStateFlag(camera, CAM_STATE_4 | CAM_STATE_2);
            Camera_UnsetStateFlag(camera, CAM_STATE_3);
            func_800CC938(camera);
            sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
        skipeyeUpdate:
            skipUpdateEye = true;
            break;
    }

    rwData->animFrame++;

    /**
     * Opening a large chest only lasts 135 frames
     *
     * So for one frame, the animState is set to 10,
     * allowing the static data to be loaded into the subCamera mini-heap
     *
     * For the remainder of the cutscene, animState is set to 1
     */
    if (rwData->animFrame == 1) {
        camera->animState = 10;
    } else if (rwData->animFrame == 2) {
        camera->animState = 1;
    } else if (rwData->animFrame == 148) {
        camera->animState = 2;
    } else if (rwData->animFrame == 158) {
        camera->animState = 20;
    } else if (rwData->animFrame == 159) {
        camera->animState = 3;
    } else if (rwData->animFrame == 168) {
        camera->animState = 30;
    } else if (rwData->animFrame == 228) {
        camera->animState = 4;
    }

    if (!skipUpdateEye) {
        eyeOffset.r = Camera_ScaledStepToCeilF(eyeOffset.r, atToEye.r, rwData->unk_0C, 1.0f);
        eyeOffset.pitch = Camera_ScaledStepToCeilS(eyeOffset.pitch, atToEye.pitch, rwData->unk_0C, 5);
        eyeOffset.yaw = Camera_ScaledStepToCeilS(eyeOffset.yaw, atToEye.yaw, rwData->unk_0C, 5);
        *eyeNext = OLib_AddVecGeoToVec3f(at, &eyeOffset);
        *eye = *eyeNext;
    }

    camera->dist = OLib_Vec3fDist(at, eye);
    camera->atLerpStepScale = 0.1f;
    Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);

    return true;
}

/**
 * Taking the warp pad from the start of a dungeon to the boss-room
 */
s32 Camera_Demo3(Camera* camera) {
    s32 pad2;
    f32 temp;
    CameraModeValue* values;
    VecGeo atToEye;
    PosRot focalActorFocus;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Demo3ReadOnlyData* roData = &camera->paramData.demo3.roData;
    Demo3ReadWriteData* rwData = &camera->paramData.demo3.rwData;

    atToEye = OLib_Vec3fDiffToVecGeo(at, eye);
    focalActorFocus = Actor_GetFocus(camera->focalActor);
    focalActorFocus.pos.x = camera->focalActorPosRot.pos.x;
    focalActorFocus.pos.z = camera->focalActorPosRot.pos.z;
    focalActorFocus.pos.y -= (focalActorFocus.pos.y - camera->focalActorPosRot.pos.y) * 0.4f;
    Camera_ScaledStepToCeilVec3f(&focalActorFocus.pos, at, 0.1f, 0.1f, 0.1f);

    if (RELOAD_PARAMS(camera)) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        camera->animState = 0;
    }

    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
            // Init Data
            camera->animState++;
            rwData->timer = 125;
            Distortion_Request(DISTORTION_TYPE_ZORA_SWIMMING);
            Distortion_SetDuration(60);
            break;

        case 1:
            // Zoom into player, start to rise
            temp = 1.0f / rwData->timer;
            atToEye.r = Camera_ScaledStepToCeilF(140.0f, atToEye.r, temp, 0.1f);
            rwData->timer--;
            camera->fov = Camera_ScaledStepToCeilF(50.0f, camera->fov, 0.1f, 0.1f);
            if (rwData->timer <= 0) {
                rwData->timer = 20;
                camera->animState++;
                Distortion_Request(DISTORTION_TYPE_UNDERWATER_ENTRY);
                Distortion_SetDuration(80);
            }
            break;

        case 2:
            // continue rising
            rwData->timer--;
            if (rwData->timer <= 0) {
                rwData->unk_04 = 120;
                rwData->timer = 0;
                rwData->unk_00 = (175.0f - camera->fov) / rwData->unk_04;
                camera->animState++;
                Distortion_Request(DISTORTION_TYPE_BOSS_WARP);
                Distortion_SetDuration(15);
            }
            break;

        case 3:
            rwData->timer++;
            camera->fov += rwData->unk_00 * rwData->timer;
            if (rwData->timer >= 15) {
                Distortion_RemoveRequest(DISTORTION_TYPE_BOSS_WARP);
                Distortion_RemoveRequest(DISTORTION_TYPE_UNDERWATER_ENTRY);
                Distortion_RemoveRequest(DISTORTION_TYPE_ZORA_SWIMMING);
                camera->animState++;
            }
            break;

        default:
            break;
    }

    *eyeNext = OLib_AddVecGeoToVec3f(at, &atToEye);
    *eye = *eyeNext;
    return true;
}

/**
 * Mask Transformation Cutscene 0:
 * starting as a human and transforming into anything else
 */
s32 Camera_Demo4(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    VecGeo atToEye;
    CameraModeValue* values;
    PosRot focalActorFocus;
    f32 sp58;
    f32 sin;
    Demo4ReadOnlyData* roData = &camera->paramData.demo4.roData;
    Demo4ReadWriteData* rwData = &camera->paramData.demo4.rwData;
    s32 pad[2];

    atToEye = OLib_Vec3fDiffToVecGeo(at, eye);

    if (RELOAD_PARAMS(camera)) {
        values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        camera->animState = 0;
        rwData->unk_00 = *at;
        rwData->unk_18 = atToEye;
        rwData->unk_14 = camera->fov;
    }

    focalActorFocus = Actor_GetFocus(camera->focalActor);
    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        default:
            break;

        case 0:
            camera->animState++;
            rwData->timer = 0;

            if (!(atToEye.r < 40.0f)) {
                atToEye.r = 40.0f;
            }

            camera->fov = 80.0f;
            rwData->unk_10 = (Rand_ZeroOne() - 0.5f) * 40.0f;
            // fallthrough
        case 1:
            // Camera fixed on human player as the mask moves from the pocket to the face
            // Camera rolls left and right
            if (rwData->timer >= 12) {
                rwData->unk_0C = (rwData->timer - 12) * (135.0f / 13.0f);
                sin = Math_SinF(DEG_TO_RAD(rwData->unk_0C));
                rwData->unk_0C = ((rwData->unk_10 < 0.0f) ? -1.0f : 1.0f) * sin;
                if (rwData->timer == 12) {
                    Distortion_Request(DISTORTION_TYPE_MASK_TRANSFORM_1);
                    Distortion_SetDuration(26);
                }
            } else {
                rwData->unk_0C = 0.0f;
            }

            sp58 = rwData->timer * (6.0f / 19.0f);
            rwData->unk_20 = focalActorPosRot->rot.y + 0x4000;
            focalActorFocus.pos.x = (Math_SinS(rwData->unk_20) * sp58 * rwData->unk_0C) + focalActorPosRot->pos.x;
            focalActorFocus.pos.z = (Math_CosS(rwData->unk_20) * sp58 * rwData->unk_0C) + focalActorPosRot->pos.z;
            focalActorFocus.pos.y -= (focalActorFocus.pos.y - focalActorPosRot->pos.y) * 0.099999994f;
            Camera_ScaledStepToCeilVec3f(&focalActorFocus.pos, at, 0.2f, 0.2f, 0.1f);

            sp58 = (rwData->timer * (30.0f / 19.0f));
            camera->roll = CAM_DEG_TO_BINANG(sp58 * rwData->unk_0C);

            sp58 = 1.0f / (38 - rwData->timer);
            rwData->timer++;
            atToEye.r = Camera_ScaledStepToCeilF(30.0f, atToEye.r, sp58, 0.1f);
            atToEye.pitch = 0;

            if (rwData->timer >= 38) {
                rwData->timer = 24;
                camera->animState++;
                rwData->unk_0C = (32.0f - camera->fov) / 24.0f;
                Distortion_Request(DISTORTION_TYPE_MASK_TRANSFORM_2);
            }
            break;

        case 2:
            // Camera steadies as human player is fully croutched down and hiding face
            if (rwData->timer == 24) {
                at->x = (Math_SinS(focalActorPosRot->rot.y) * -7.0f) + focalActorPosRot->pos.x;
                at->y = focalActorFocus.pos.y - ((focalActorFocus.pos.y - focalActorPosRot->pos.y) * 0.1f);
                at->z = (Math_CosS(focalActorPosRot->rot.y) * -7.0f) + focalActorPosRot->pos.z;
            } else {
                focalActorFocus.pos.x = (Math_SinS(focalActorPosRot->rot.y) * -7.0f) + focalActorPosRot->pos.x;
                focalActorFocus.pos.y -= (focalActorFocus.pos.y - camera->focalActorPosRot.pos.y) * 0.1f;
                focalActorFocus.pos.z = (Math_CosS(focalActorPosRot->rot.y) * -7.0f) + focalActorPosRot->pos.z;
                Camera_ScaledStepToCeilVec3f(&focalActorFocus.pos, at, 0.25f, 0.25f, 0.1f);
            }

            if (rwData->timer > 0) {
                camera->fov += rwData->unk_0C;
            }

            rwData->timer--;

            atToEye.r = 35.0f;
            atToEye.pitch = 0x2000;
            camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.1f, 5);

            if (rwData->timer <= 0) {
                rwData->unk_20 = 630;
                rwData->timer = 0;
                rwData->unk_0C = (60.0f - camera->fov) / rwData->unk_20;
                camera->animState = 3;
            }
            break;

        case 3:
            // Camera zooms into human players face with bulging eyes
            focalActorFocus.pos.x = focalActorPosRot->pos.x;
            focalActorFocus.pos.y -= (focalActorFocus.pos.y - camera->focalActorPosRot.pos.y) * 0.1f;
            focalActorFocus.pos.z = focalActorPosRot->pos.z;

            Camera_ScaledStepToCeilVec3f(&focalActorFocus.pos, at, 0.25f, 0.25f, 0.1f);

            camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.1f, 5);
            rwData->timer++;
            camera->fov += rwData->unk_0C * rwData->timer;
            atToEye.pitch = 0x2000;
            atToEye.r = 35.0f;

            if (rwData->timer >= 35) {
                Distortion_RemoveRequest(DISTORTION_TYPE_MASK_TRANSFORM_1);
                Distortion_RemoveRequest(DISTORTION_TYPE_MASK_TRANSFORM_2);
                camera->animState = 4;
            }
            break;

        case 999:
            focalActorFocus = Actor_GetFocus(camera->focalActor);
            Distortion_RemoveRequest(DISTORTION_TYPE_MASK_TRANSFORM_1);
            Distortion_RemoveRequest(DISTORTION_TYPE_MASK_TRANSFORM_2);
            camera->animState = 4;
            break;

        case 4:
            // Camera backs up as player is now in a transformed state
            focalActorFocus.pos.y -= (focalActorFocus.pos.y - camera->focalActorPosRot.pos.y) * 0.1f;
            Camera_ScaledStepToCeilVec3f(&focalActorFocus.pos, at, 0.1f, 0.1f, 0.1f);
            atToEye = rwData->unk_18;
            camera->fov = rwData->unk_14;
            camera->roll = 0;
            break;
    }

    camera->eyeNext = OLib_AddVecGeoToVec3f(at, &atToEye);
    *eye = camera->eyeNext;

    return true;
}

/**
 * Mask Transformation Cutscene 1:
 * starting as non-human/fierce diety and transforming into anything else
 */
s32 Camera_Demo5(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecGeo atToEye;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    PosRot focalActorFocus;
    f32 new_var;
    f32 sp58;
    f32 sin;
    Demo5ReadOnlyData* roData = &camera->paramData.demo5.roData;
    Demo5ReadWriteData* rwData = &camera->paramData.demo5.rwData;

    atToEye = OLib_Vec3fDiffToVecGeo(at, eye);

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        camera->animState = 0;
        rwData->unk_00 = *at;
        rwData->unk_1C = atToEye;
        rwData->unk_18 = camera->fov;
    }

    focalActorFocus = Actor_GetFocus(camera->focalActor);

    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
            camera->animState++;
            atToEye.pitch = 0;
            rwData->timer = 0x12;
            rwData->unk_14 = 80.0f;
            atToEye.r = 30.0f;
            camera->fov = 80.0f;
            rwData->unk_10 = (Rand_ZeroOne() - 0.5f) * 40.0f;
            camera->roll = 0;
            focalActorFocus.pos.x = focalActorPosRot->pos.x;
            focalActorFocus.pos.z = focalActorPosRot->pos.z;
            *at = focalActorFocus.pos;
            // fallthrough
        case 1:
            // Camera remains still as player moves hands to face
            rwData->timer--;
            if (rwData->timer <= 0) {
                rwData->timer = 0;
                camera->animState = 2;
                rwData->unk_24 = camera->focalActorPosRot.rot.y + 0x4000;
                rwData->timer = 46;
                Distortion_Request(DISTORTION_TYPE_MASK_TRANSFORM_1);
                Distortion_SetDuration(46);
            }
            break;

        case 2:
            // Camera zooms out while rolling back and forth
            rwData->unk_0C = rwData->timer * (180.0f / 23.0f);
            sp58 = DEG_TO_RAD(rwData->unk_0C);
            sin = Math_SinF(sp58);
            rwData->unk_0C = ((rwData->unk_10 < 0.0f) ? -1.0f : 1.0f) * sin;
            new_var = (46 - rwData->timer) * (5.0f / 46.0f);
            focalActorFocus.pos.x = (Math_SinS(rwData->unk_24) * new_var * rwData->unk_0C) + focalActorPosRot->pos.x;
            focalActorFocus.pos.z = (Math_CosS(rwData->unk_24) * new_var * rwData->unk_0C) + focalActorPosRot->pos.z;
            focalActorFocus.pos.y -= (focalActorFocus.pos.y - camera->focalActorPosRot.pos.y) * 0.2f;
            Camera_ScaledStepToCeilVec3f(&focalActorFocus.pos, at, 0.1f, 0.1f, 0.1f);
            new_var = rwData->timer * (10.0f / 23.0f);
            camera->roll = CAM_DEG_TO_BINANG(rwData->unk_0C * new_var);
            new_var = 1.0f / rwData->timer;
            atToEye.r = Camera_ScaledStepToCeilF(rwData->unk_14, atToEye.r, new_var, 0.1f);
            rwData->timer--;
            atToEye.pitch = 0;
            if (rwData->timer <= 0) {
                camera->animState = 3;
                Distortion_RemoveRequest(DISTORTION_TYPE_MASK_TRANSFORM_1);
            }
            break;

        case 999:
            focalActorFocus = Actor_GetFocus(camera->focalActor);
            camera->animState = 3;
            Distortion_RemoveRequest(DISTORTION_TYPE_MASK_TRANSFORM_1);
            break;

        case 3:
            // Player is in new form
            focalActorFocus.pos.y -= (focalActorFocus.pos.y - camera->focalActorPosRot.pos.y) * 0.1f;
            Camera_ScaledStepToCeilVec3f(&focalActorFocus.pos, at, 0.1f, 0.1f, 0.1f);
            camera->roll = 0;
            atToEye = rwData->unk_1C;
            camera->fov = rwData->unk_18;
            break;

        default:
            break;
    }

    *eyeNext = OLib_AddVecGeoToVec3f(at, &atToEye);
    *eye = *eyeNext;

    return true;
}

s32 Camera_Demo6(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Demo7(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Demo8(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Demo9(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Smoothly and gradually return camera to Player after a cutscene
 * Used for global csId = 0x7E (Connect Camera Setting)
 */
s32 Camera_Demo0(Camera* camera) {
    s32 pad;
    f32 timer;
    s16 phi_v1;
    Camera* subCam = &camera->play->subCameras[2];
    Vec3f* eye = &camera->eye;
    Vec3f* subEye = &subCam->eye;
    Vec3f* at = &camera->at;
    Demo0ReadOnlyData* roData = &camera->paramData.demo0.roData;
    Demo0ReadWriteData* rwData = &camera->paramData.demo0.rwData;

    // Initialize
    if (camera->animState == 0) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    // Initialize
    if (camera->animState == 0) {
        rwData->subAtToEye = OLib_Vec3fDiffToVecGeo(&subCam->at, subEye);
        rwData->atToEye = OLib_Vec3fDiffToVecGeo(&camera->at, eye);
        if (rwData->subAtToEye.r < 50.0f) {
            rwData->subAtToEye.r = 50.0f;
            if (rwData->subAtToEye.pitch < 0x7D0) { // 11 degrees
                rwData->subAtToEye.pitch = 0x7D0;
            }
        }

        rwData->unk_00 = OLib_Vec3fDist(eye, subEye);
        if (rwData->unk_00 < 300.0f) {
            rwData->timer = 0;
        } else {
            rwData->timer = sqrtf(rwData->unk_00 - 200.0f);
        }

        rwData->unk_14 = subCam->inputDir.x - camera->inputDir.x;
        if (rwData->unk_14 >= 0) {
            phi_v1 = rwData->unk_14;
        } else {
            phi_v1 = -rwData->unk_14;
        }

        if (phi_v1 > 10000) {
            phi_v1 /= 1000;
            if (rwData->timer < phi_v1) {
                rwData->timer = phi_v1;
            }
        }

        rwData->unk_16 = subCam->inputDir.y - camera->inputDir.y;
        if (rwData->unk_16 >= 0) {
            phi_v1 = rwData->unk_16;
        } else {
            phi_v1 = -rwData->unk_16;
        }

        if (phi_v1 > 10000) {
            phi_v1 /= 1000;
            if (rwData->timer < phi_v1) {
                rwData->timer = phi_v1;
            }
        }

        rwData->unk_18 = subCam->inputDir.z - camera->inputDir.z;
        if (rwData->unk_18 >= 0) {
            phi_v1 = rwData->unk_18;
        } else {
            phi_v1 = -rwData->unk_18;
        }

        if (phi_v1 > 10000) {
            phi_v1 /= 1000;
            if (rwData->timer < phi_v1) {
                rwData->timer = phi_v1;
            }
        }

        if (rwData->timer != 0) {
            rwData->unk_00 = (rwData->subAtToEye.r - rwData->atToEye.r) / rwData->timer;
            rwData->unk_14 = (s16)(rwData->subAtToEye.pitch - rwData->atToEye.pitch) / rwData->timer;
            rwData->unk_16 = (s16)(rwData->subAtToEye.yaw - rwData->atToEye.yaw) / rwData->timer;
            rwData->unk_18 = (s16)(subCam->roll - camera->roll) / rwData->timer;
        }
        camera->animState++;
    }

    if (rwData->timer != 0) {
        timer = rwData->timer;

        // Update At (Inverse Interpolation)
        at->x += (subCam->at.x - camera->at.x) / timer;
        at->y += (subCam->at.y - camera->at.y) / timer;
        at->z += (subCam->at.z - camera->at.z) / timer;

        rwData->atToEye.r += rwData->unk_00;
        rwData->atToEye.pitch += rwData->unk_14;
        rwData->atToEye.yaw += rwData->unk_16;

        camera->roll += rwData->unk_18;

        // Update Eye
        *eye = OLib_AddVecGeoToVec3f(at, &rwData->atToEye);
        camera->eyeNext = *eye;

        rwData->timer--;
    }

    if (rwData->timer == 0) {
        CutsceneManager_Stop(0x7E);
    }

    return true;
}

s32 Camera_Special0(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special1(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special2(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special3(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special4(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Flying with hookshot
 */
s32 Camera_Special5(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot spA8;
    s16 pad1;
    s16 spA4;
    CameraCollision sp7C;
    VecGeo sp74;
    VecGeo sp6C;
    VecGeo atToEye;
    VecGeo atToEyeNext;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Special5ReadOnlyData* roData = &camera->paramData.spec5.roData;
    Special5ReadWriteData* rwData = &camera->paramData.spec5.rwData;
    f32 rand;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);

    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (0.8f - ((68.0f / focalActorHeight) * -0.2f));

        roData->yOffset = (GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight) * yNormal;
        roData->eyeDist = GET_NEXT_RO_DATA(values);
        roData->minDistForRot = GET_NEXT_RO_DATA(values);
        roData->pitch = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->atMaxLERPScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->timerInit = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    atToEye = OLib_Vec3fDiffToVecGeo(at, eye);
    atToEyeNext = OLib_Vec3fDiffToVecGeo(at, eyeNext);
    spA8 = Actor_GetWorld(camera->target);

    sCameraInterfaceFlags = roData->interfaceFlags;

    if (camera->animState == 0) {
        camera->animState++;
        rwData->animTimer = roData->timerInit;
    }

    if (rwData->animTimer > 0) {
        rwData->animTimer--;
    } else if (rwData->animTimer == 0) {
        if ((camera->target == NULL) || (camera->target->update == NULL)) {
            camera->target = NULL;
            return true;
        }

        rwData->animTimer--;
        if (roData->minDistForRot < OLib_Vec3fDist(&spA8.pos, &focalActorPosRot->pos)) {
            sp6C.yaw = focalActorPosRot->rot.y;
            sp6C.pitch = -focalActorPosRot->rot.x;
            sp6C.r = 20.0f;
            sp7C.pos = OLib_AddVecGeoToVec3f(&spA8.pos, &sp6C);
            func_800CBC84(camera, at, &sp7C, 0);
            sp6C = OLib_Vec3fToVecGeo(&sp7C.norm);
            spA4 = BINANG_SUB(focalActorPosRot->rot.y, sp6C.yaw);
            sp74.r = roData->eyeDist;
            rand = Rand_ZeroOne();
            sp74.yaw = BINANG_ROT180(focalActorPosRot->rot.y) +
                       (s16)((spA4 < 0) ? -(s16)(0x1553 + TRUNCF_BINANG(rand * 2730.0f))
                                        : (s16)(0x1553 + TRUNCF_BINANG(rand * 2730.0f)));
            sp74.pitch = roData->pitch;
            *eyeNext = OLib_AddVecGeoToVec3f(&spA8.pos, &sp74);
            *eye = *eyeNext;
            Camera_BgCheck(camera, &spA8.pos, eye);
        }
    }

    Camera_CalcAtDefault(camera, &atToEyeNext, roData->yOffset, 0);
    camera->fov = Camera_ScaledStepToCeilF(roData->fovTarget, camera->fov, camera->atLerpStepScale * 0.05f, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);
    camera->atLerpStepScale = Camera_ClampLerpScale(camera, roData->atMaxLERPScale);

    return true;
}

s32 Camera_Special6(Camera* camera) {
    return Camera_Noop(camera);
}

s32 Camera_Special7(Camera* camera) {
    return Camera_Noop(camera);
}

/**
 * Exiting a spiral staircase cutscene.
 * Camera remains fixed at given position, and tracks player for the duration of the cutscene
 */
s32 Camera_Special8(Camera* camera) {
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    Vec3f atTarget;
    Vec3f posOffsetTarget;
    f32 yNormal;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    DoorParams* doorParams = &camera->paramData.doorParams;
    Special8ReadOnlyData* roData = &camera->paramData.spec8.roData;
    Special8ReadWriteData* rwData = &camera->paramData.spec8.rwData;
    s32 pad[2];

    Camera_UnsetStateFlag(camera, CAM_STATE_4);
    yNormal = (0.8f - ((68.0f / focalActorHeight) * -0.2f));

    if (!RELOAD_PARAMS(camera)) {
    } else {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        // Initialize data
        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * yNormal;
        roData->eyeStepScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->posStepScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->spiralDoorCsLength = GET_NEXT_RO_DATA(values) * 5;
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        rwData->fov = roData->fov * 100.0f;
        rwData->spiralDoorCsFrame = 0;
        Camera_UnsetStateFlag(camera, CAM_STATE_2 | CAM_STATE_CHECK_WATER);
        rwData->eye.x = doorParams->eye.x;
        rwData->eye.y = doorParams->eye.y;
        rwData->eye.z = doorParams->eye.z;
    }

    // Check if cutscene is still playing
    if (rwData->spiralDoorCsFrame < roData->spiralDoorCsLength) {
        rwData->spiralDoorCsFrame++;
        sCameraInterfaceFlags = roData->interfaceFlags;
        posOffsetTarget.x = 0.0f;
        posOffsetTarget.y = roData->yOffset + focalActorHeight;
        posOffsetTarget.z = 0.0f;
        Camera_ScaledStepToCeilVec3f(&posOffsetTarget, &camera->focalActorAtOffset, roData->posStepScale,
                                     roData->posStepScale, 0.1f);

        // Camera follows player as they exit the stairwell
        atTarget.x = focalActorPosRot->pos.x + camera->focalActorAtOffset.x;
        atTarget.y = focalActorPosRot->pos.y + camera->focalActorAtOffset.y;
        atTarget.z = focalActorPosRot->pos.z + camera->focalActorAtOffset.z;
        if (camera->animState == 0) {
            camera->animState++;
            if (!(roData->interfaceFlags & SPECIAL8_FLAG_0)) {
                camera->eyeNext = rwData->eye;
                camera->at = atTarget;
            }
        }

        // Update at to look at player
        Camera_ScaledStepToCeilVec3f(&atTarget, at, roData->posStepScale, roData->posStepScale, 10.0f);

        // Move camera position &rwData->eye and remain there for the entire cutscen
        Camera_ScaledStepToCeilVec3f(&rwData->eye, eyeNext, roData->eyeStepScale, roData->eyeStepScale, 0.1f);
        camera->eye = *eyeNext;
        camera->dist = OLib_Vec3fDist(at, &camera->eye);
        camera->roll = 0;
        camera->xzSpeed = 0.0f;
        camera->fov = CAM_RODATA_UNSCALE(rwData->fov);
        camera->atLerpStepScale = Camera_ClampLerpScale(camera, 1.0f);
        Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);
    } else {
        // Cutscene is finished
        Camera_SetStateFlag(camera, CAM_STATE_10 | CAM_STATE_4);
        sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);

        // Wait for user input to move to the next camera update function
        if ((camera->xzSpeed > 0.001f) || CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_A) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_B) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CUP) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CDOWN) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CLEFT) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CRIGHT) ||
            // #region 2S2H [Dpad]
            CHECK_BTN_DPAD(CONTROLLER1(&camera->play->state)->press.button) ||
            // #endregion
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_Z) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_L) ||
            CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_R) ||
            (roData->interfaceFlags & SPECIAL8_FLAG_3)) {
            func_800CC938(camera);
            Camera_SetStateFlag(camera, CAM_STATE_2 | CAM_STATE_CHECK_WATER);
            Camera_UnsetStateFlag(camera, CAM_STATE_10);
        }
    }
    return true;
}

/**
 * Entering and exiting a door between rooms (eg. sliding doors)
 * Camera closely follows player on both the front and the back room
 */
s32 Camera_Special9(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    s32 rand1;
    Vec3f spB8;
    VecGeo spB0;
    VecGeo spA8;
    f32 focalActorHeight = Camera_GetFocalActorHeight(camera);
    s32 phi_v1_2;
    s16 csId = CutsceneManager_GetCurrentCsId();
    f32 yNormal;
    PosRot sp84;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* focalActorPosRot = &camera->focalActorPosRot;
    DoorParams* doorParams = &camera->paramData.doorParams;
    Special9ReadOnlyData* roData = &camera->paramData.spec9.roData;
    Special9ReadWriteData* rwData = &camera->paramData.spec9.rwData;
    s32 sp50[1];
    BgCamFuncData* bgCamFuncData;

    if ((csId != CS_ID_NONE) && (csId != CS_ID_GLOBAL_DOOR)) {
        func_800E0348(camera);
    }

    Camera_UnsetStateFlag(camera, CAM_STATE_4);
    yNormal = 0.8f - ((68.0f / focalActorHeight) * -0.2f);

    if (!RELOAD_PARAMS(camera)) {
    } else {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * focalActorHeight * yNormal;
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
    }

    if (doorParams->doorActor != NULL) {
        sp84 = Actor_GetWorldPosShapeRot(doorParams->doorActor);
    } else {
        sp84 = *focalActorPosRot;
        sp84.pos.y += focalActorHeight + roData->yOffset;
        sp84.rot.x = 0;
    }

    spA8 = OLib_Vec3fDiffToVecGeo(at, eye);

    sCameraInterfaceFlags = roData->interfaceFlags;

    switch (camera->animState) {
        case 0:
            // Init
            Camera_UnsetStateFlag(camera, CAM_STATE_2 | CAM_STATE_CHECK_WATER);
            camera->animState++;
            if (ABS((s16)(focalActorPosRot->rot.y - sp84.rot.y)) > 0x4000) {
                rwData->unk_00 = BINANG_ROT180(sp84.rot.y);
            } else {
                rwData->unk_00 = sp84.rot.y;
            }
            // fallthrough
        case 1:
            // Camera is fixed in front of the door
            doorParams->timer1--;
            if (doorParams->timer1 > 0) {
                break;
            }
            FrameInterpolation_DontInterpolateCamera();
            camera->animState++;

            // Setup for the camera moving behind the door
            if (roData->interfaceFlags & SPECIAL9_FLAG_0) {
                bgCamFuncData = (BgCamFuncData*)Camera_GetBgCamOrActorCsCamFuncData(camera, camera->bgCamIndex);
                *eyeNext = Camera_Vec3sToVec3f(&bgCamFuncData->pos);
                spB8 = *eye = *eyeNext;
            } else {
                s16 camEyeSide;
                s16 randFloat;

                spB0.pitch = TRUNCF_BINANG(Rand_ZeroOne() * 0x280) + 0xBB8;
                randFloat = TRUNCF_BINANG(Rand_ZeroOne() * 0x4CE) + 0x5DC;

                // The camera will either position itself either to the left or to the right
                // of the door when it jumps behind it. It's effectively 50/50 percent chance
                camEyeSide = (s16)(randFloat * ((camera->play->state.frames % 2) ? 1 : -1));

                spB0.yaw = rwData->unk_00 + camEyeSide;
                spB0.r = 200.0f * yNormal;
                *eyeNext = OLib_AddVecGeoToVec3f(at, &spB0);
                spB8 = *eye = *eyeNext;

                // If the side chosen moves the camera out-of-bounds, move to the other side
                if (Camera_CheckOOB(camera, &spB8, &focalActorPosRot->pos)) {
                    camEyeSide = (s16)-camEyeSide;
                    spB0.yaw = rwData->unk_00 + camEyeSide;
                    *eyeNext = OLib_AddVecGeoToVec3f(at, &spB0);
                    *eye = *eyeNext;
                }
            }
            // fallthrough
        case 2:
            // Camera is behind the door looking at player
            spB8 = focalActorPosRot->pos;
            spB8.y += focalActorHeight + roData->yOffset;
            // Update camera at to follow the player
            Camera_ScaledStepToCeilVec3f(&spB8, at, 0.25f, 0.25f, 0.1f);

            doorParams->timer2--;
            if (doorParams->timer2 > 0) {
                break;
            }

            // Setup for the camera turning around to look in front of player
            camera->animState++;
            rwData->unk_00 = BINANG_ROT180(rwData->unk_00);
            // fallthrough
        case 3:
            // Camera turns around to look in front of player
            spB8 = focalActorPosRot->pos;
            spB8.y += focalActorHeight + roData->yOffset;
            Camera_ScaledStepToCeilVec3f(&spB8, at, 0.5f, 0.5f, 0.1f);
            spB0.pitch = Camera_ScaledStepToCeilS(0xAAA, spA8.pitch, 0.3f, 5);
            spB0.yaw = Camera_ScaledStepToCeilS(rwData->unk_00, spA8.yaw, 0.3f, 5);
            spB0.r = Camera_ScaledStepToCeilF(60.0f, spA8.r, 0.3f, 1.0f);
            *eyeNext = OLib_AddVecGeoToVec3f(at, &spB0);

            *eye = *eyeNext;

            doorParams->timer3--;
            if (doorParams->timer3 > 0) {
                break;
            }

            camera->animState++;
            // fallthrough
        case 4:
            camera->animState++;
            // fallthrough
        case 999:
        default:
            // Door is closed and is waiting for user input to toggle to a new setting
            Camera_SetStateFlag(camera, CAM_STATE_10 | CAM_STATE_4);
            sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);

            if ((camera->xzSpeed > 0.001f) || CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_A) ||
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_B) ||
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CUP) ||
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CDOWN) ||
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CLEFT) ||
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_CRIGHT) ||
                // #region 2S2H [Dpad]
                CHECK_BTN_DPAD(CONTROLLER1(&camera->play->state)->press.button) ||
                // #endregion
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_Z) ||
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_L) ||
                CHECK_BTN_ALL(CONTROLLER1(&camera->play->state)->press.button, BTN_R) ||
                (roData->interfaceFlags & SPECIAL9_FLAG_3)) {

                func_800CC938(camera);
                Camera_SetStateFlag(camera, CAM_STATE_2 | CAM_STATE_CHECK_WATER);
                Camera_UnsetStateFlag(camera, CAM_STATE_10);
            }
            break;
    }

    spB8 = focalActorPosRot->pos;
    spB8.y += focalActorHeight;
    camera->dist = OLib_Vec3fDist(&spB8, eye);
    Camera_SetFocalActorAtOffset(camera, &focalActorPosRot->pos);

    return true;
}

/*===============================================================*/
/*                    Camera Public Functions                    */
/*===============================================================*/

Camera* Camera_Create(View* view, CollisionContext* colCtx, PlayState* play) {
    Camera* newCamera = ZeldaArena_Malloc(sizeof(Camera));

    if (newCamera != NULL) {
        Camera_Init(newCamera, view, colCtx, play);
    }
    return newCamera;
}

void Camera_Destroy(Camera* camera) {
    if (camera != NULL) {
        ZeldaArena_Free(camera);
    }
}

void Camera_Init(Camera* camera, View* view, CollisionContext* colCtx, PlayState* play) {
    Camera* cameraPtr;
    s32 i;
    s16 curUID;
    s16 j;

    __osMemset(camera, 0, sizeof(Camera));

    camera->play = sCamPlayState = play;
    curUID = sCameraNextUID;
    sCameraNextUID++;
    while (curUID != 0) {
        if (curUID == 0) {
            sCameraNextUID++;
        }

        for (j = 0; j < NUM_CAMS; j++) {
            cameraPtr = camera->play->cameraPtrs[j];
            if ((cameraPtr != NULL) && (curUID == cameraPtr->uid)) {
                break;
            }
        }

        if (j == NUM_CAMS) {
            break;
        }

        curUID = sCameraNextUID++;
    }

    camera->inputDir.y = 0x4000;
    camera->uid = curUID;
    camera->camDir = camera->inputDir;
    camera->nextCamSceneDataId = -1;
    camera->up.z = camera->up.x = 0.0f;
    camera->up.y = 1.0f;
    camera->fov = 60.0f;
    camera->yOffsetUpdateRate = 0.05f;
    camera->xzOffsetUpdateRate = 0.05f;
    camera->fovUpdateRate = 0.05f;
    camera->rUpdateRateInv = 10.0f;
    camera->yawUpdateRateInv = 10.0f;
    camera->pitchUpdateRateInv = 16.0f;

    sCameraLetterboxSize = 32;
    sCameraHudVisibility = 0;

    camera->setting = camera->prevSetting = CAM_SET_FREE0;
    camera->bgCamIndex = camera->prevBgCamDataId = -1;
    camera->stateFlags = 0;
    camera->mode = CAM_MODE_NORMAL;
    camera->bgId = BGCHECK_SCENE;
    camera->unk168 = 0xF;
    camera->timer = -1;
    camera->focalActor = NULL;
    camera->target = NULL;
    Camera_SetStateFlag(camera, CAM_STATE_INITIALIZED);
    camera->quakeOffset.z = camera->quakeOffset.y = camera->quakeOffset.x = 0;
    camera->up.z = camera->up.x = 0.0f;
    camera->atLerpStepScale = 1;
    camera->up.y = 1.0f;
    sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_IGNORE, 0);
    sCameraInitSceneTimer = 3;
}

void func_800DDFE0(Camera* camera) {
    if (camera != &camera->play->mainCamera) {
        camera->prevSetting = camera->setting = CAM_SET_FREE0;
        Camera_UnsetStateFlag(camera, CAM_STATE_2);
    } else {
        switch (camera->play->roomCtx.curRoom.type) {
            case ROOM_TYPE_DUNGEON:
                camera->prevSetting = CAM_SET_DUNGEON0;
                Camera_ChangeSettingFlags(camera, CAM_SET_DUNGEON0, CAM_CHANGE_SETTING_1);
                break;

            case ROOM_TYPE_NORMAL:
                camera->prevSetting = CAM_SET_NORMAL0;
                Camera_ChangeSettingFlags(camera, CAM_SET_NORMAL0, CAM_CHANGE_SETTING_1);
                break;

            case ROOM_TYPE_INDOORS:
                camera->prevSetting = CAM_SET_ROOM0;
                Camera_ChangeSettingFlags(camera, CAM_SET_ROOM0, CAM_CHANGE_SETTING_1);
                break;

            default:
                camera->prevSetting = CAM_SET_NORMAL0;
                Camera_ChangeSettingFlags(camera, CAM_SET_NORMAL0, CAM_CHANGE_SETTING_1);
                break;
        }
        Camera_SetStateFlag(camera, CAM_STATE_2);
    }
}

/**
 * Unused empty function
 */
void Camera_Stub800DE0E0(Camera* camera) {
}

void Camera_InitFocalActorSettings(Camera* camera, Actor* focalActor) {
    PosRot focalActorPosRot;
    VecGeo eyeNextAtOffset;
    s32 bgId;
    Vec3f floorPos;
    s32 upXZ;
    f32 focalActorHeight;
    Vec3f* eye = &camera->eye;

    focalActorPosRot = Actor_GetWorldPosShapeRot(focalActor);

    camera->focalActor = focalActor;
    focalActorHeight = Camera_GetFocalActorHeight(camera);
    camera->focalActorPosRot = focalActorPosRot;
    camera->dist = eyeNextAtOffset.r = 180.0f;
    camera->inputDir.y = focalActorPosRot.rot.y;
    eyeNextAtOffset.yaw = BINANG_ROT180(camera->inputDir.y);
    camera->inputDir.x = eyeNextAtOffset.pitch = 0x71C;
    camera->inputDir.z = 0;
    camera->camDir = camera->inputDir;
    camera->xzSpeed = 0.0f;
    camera->unk_0F0.y = 0.0f;
    camera->at = focalActorPosRot.pos;
    camera->at.y += focalActorHeight;

    camera->focalActorAtOffset.x = 0;
    camera->focalActorAtOffset.y = focalActorHeight;
    camera->focalActorAtOffset.z = 0;

    camera->eyeNext = OLib_AddVecGeoToVec3f(&camera->at, &eyeNextAtOffset);
    *eye = camera->eyeNext;
    camera->roll = 0;

    upXZ = 0;
    camera->up.z = upXZ;
    camera->up.y = 1.0f;
    camera->up.x = upXZ;

    {
        s32 requiredScopeTemp;

        if (Camera_GetFloorYNorm(camera, &floorPos, &camera->at, &bgId) != BGCHECK_Y_MIN) {
            camera->bgId = bgId;
        }
    }

    camera->waterPrevBgCamDataId = -1;
    camera->waterPrevCamSetting = -1;
    camera->waterQuakeId = -1;

    {
        s32 requiredScopeTemp;

        func_800DDFE0(camera);
    }

    Camera_SetStateFlag(camera, CAM_STATE_2);

    camera->viewFlags = 0;
    camera->nextCamSceneDataId = -1;
    camera->yOffsetUpdateRate = 0.01f;
    camera->xzOffsetUpdateRate = 0.01f;
    camera->fovUpdateRate = 0.01f;
    camera->atLerpStepScale = 1;
    Camera_ResetActionFuncState(camera, camera->mode);

    if (camera == &camera->play->mainCamera) {
        sCameraInterfaceFlags =
            CAM_INTERFACE_FLAGS(CAM_LETTERBOX_LARGE | CAM_LETTERBOX_INSTANT, CAM_HUD_VISIBILITY_NONE_ALT, 0);
        CutsceneManager_StoreCamera(camera);
    } else {
        sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NONE_ALT, 0);
    }
    Camera_UpdateWater(camera);
}

/**
 * Updates the camera status
 */
s32 Camera_ChangeStatus(Camera* camera, s16 status) {
    camera->status = status;
    return camera->status;
}

s32 Camera_UpdateWater(Camera* camera) {
    f32 waterY;
    s16 camSetting;
    s32 pad[2];
    s32* waterPrevCamSetting = &camera->waterPrevCamSetting;
    s16 prevBgId;

    if (!(camera->stateFlags & CAM_STATE_CHECK_WATER) || (sCameraSettings[camera->setting].flags & 0x40000000)) {
        return false;
    }

    if (camera->stateFlags & CAM_STATE_9) {
        if (Camera_IsDiving(camera)) {
            if (!Camera_IsPlayerFormZora(camera)) {
                Camera_ChangeSettingFlags(camera, CAM_SET_PIVOT_DIVING, CAM_CHANGE_SETTING_2 | CAM_CHANGE_SETTING_1);
            } else {
                Camera_ChangeSettingFlags(camera, CAM_SET_ZORA_DIVING, CAM_CHANGE_SETTING_2 | CAM_CHANGE_SETTING_1);
            }
            Camera_SetStateFlag(camera, CAM_STATE_15);
        } else if (camera->stateFlags & CAM_STATE_15) {
            Camera_ChangeSettingFlags(camera, *waterPrevCamSetting, CAM_CHANGE_SETTING_2 | CAM_CHANGE_SETTING_1);
            Camera_UnsetStateFlag(camera, CAM_STATE_15);
        }
    }

    if (!(camera->stateFlags & CAM_STATE_15)) {
        camSetting = Camera_GetWaterBoxBgCamSetting(camera, &waterY);
        if (camSetting == -2) {
            // CAM_SET_NONE
            if (!(camera->stateFlags & CAM_STATE_9)) {
                Camera_SetStateFlag(camera, CAM_STATE_9);
                camera->waterPrevBgCamDataId = camera->bgCamIndex;
                camera->waterQuakeId = -1;
            }

            if (!(Camera_fabsf(camera->focalActorPosRot.pos.y - camera->focalActorFloorHeight) < 11.0f) ||
                (Camera_IsSwimming(camera) && !Camera_IsUnderwaterAsZora(camera))) {
                prevBgId = camera->bgId;
                camera->bgId = BGCHECK_SCENE;
                waterPrevCamSetting = &camera->waterPrevCamSetting;
                Camera_ChangeSettingFlags(camera, CAM_SET_NORMAL3, CAM_CHANGE_SETTING_1);
                *waterPrevCamSetting = camera->setting;
                camera->bgId = prevBgId;
                camera->bgCamIndex = -2;
            }

        } else if (camSetting != -1) {
            // player is in a water box
            if (!(camera->stateFlags & CAM_STATE_9)) {
                Camera_SetStateFlag(camera, CAM_STATE_9);
                camera->waterPrevBgCamDataId = camera->bgCamIndex;
                camera->waterQuakeId = -1;
            }

            if (!(Camera_fabsf(camera->focalActorPosRot.pos.y - camera->focalActorFloorHeight) < 11.0f) ||
                (Camera_IsSwimming(camera) && !Camera_IsUnderwaterAsZora(camera))) {
                prevBgId = camera->bgId;
                camera->bgId = BGCHECK_SCENE;
                waterPrevCamSetting = &camera->waterPrevCamSetting;
                Camera_ChangeSettingFlags(camera, camSetting, CAM_CHANGE_SETTING_1);
                *waterPrevCamSetting = camera->setting;
                camera->bgId = prevBgId;
            }

        } else if (camera->stateFlags & CAM_STATE_9) {
            // player is out of a water box.
            Camera_UnsetStateFlag(camera, CAM_STATE_9);
            prevBgId = camera->bgId;
            camera->bgId = BGCHECK_SCENE;
            if (camera->waterPrevBgCamDataId < 0) {
                func_800DDFE0(camera);
                camera->bgCamIndex = -1;
            } else {
                Camera_ChangeActorCsCamIndex(camera, camera->waterPrevBgCamDataId);
            }
            camera->bgId = prevBgId;
        }
        camera->waterYPos = waterY;
    }
    return true;
}

void Camera_EarthquakeDay3(Camera* camera) {
    static s16 sEarthquakeTimer = 0;
    u16 time;
    s16 quakeIndex;
    s32 timeSpeedOffset;
    s16 sEarthquakeFreq[] = {
        0xFFC, // 1 Large Earthquake  between CLOCK_TIME(0, 00) to CLOCK_TIME(1, 30)
        0x7FC, // 2 Large Earthquakes between CLOCK_TIME(1, 30) to CLOCK_TIME(3, 00)
        0x3FC, // 4 Large Earthquakes between CLOCK_TIME(3, 00) to CLOCK_TIME(4, 30)
        0x1FC, // 8 Large Earthquakes between CLOCK_TIME(4, 30) to CLOCK_TIME(6, 00)
    };

    if (GameInteractor_Should(VB_EARTHQUAKE_ON_DAY_3,
                              (CURRENT_DAY == 3) && (CutsceneManager_GetCurrentCsId() == CS_ID_NONE))) {
        time = CURRENT_TIME;
        timeSpeedOffset = gSaveContext.save.timeSpeedOffset;

        // Large earthquake created
        // Times based on sEarthquakeFreq
        if ((time > CLOCK_TIME(0, 0)) && (time < CLOCK_TIME(6, 0)) && ((sEarthquakeFreq[time >> 12] & time) == 0) &&
            (Quake_GetNumActiveQuakes() < 2)) {
            quakeIndex = Quake_Request(camera, QUAKE_TYPE_4);
            if (quakeIndex != 0) {
                Quake_SetSpeed(quakeIndex, 30000);
                Quake_SetPerturbations(quakeIndex, (time >> 12) + 2, 1, 5, 60);
                sEarthquakeTimer = ((time >> 10) - timeSpeedOffset) + 80;
                Quake_SetDuration(quakeIndex, sEarthquakeTimer);
            }
        }

        // Small earthquake created
        // Around CLOCK_TIME(17, 33) || Around CLOCK_TIME(20, 33) || Every 1024 frames (around every 51s)
        if (((((time + 0x4D2) & 0xDFFC) == 0xC000) || ((camera->play->state.frames % 1024) == 0)) &&
            (Quake_GetNumActiveQuakes() < 2)) {
            quakeIndex = Quake_Request(camera, QUAKE_TYPE_3);
            if (quakeIndex != 0) {
                Quake_SetSpeed(quakeIndex, 16000);
                Quake_SetPerturbations(quakeIndex, 1, 0, 0, time & 0x3F); // %64
                sEarthquakeTimer = 120 - timeSpeedOffset;
                Quake_SetDuration(quakeIndex, sEarthquakeTimer);
            }
        }

        if (sEarthquakeTimer != 0) {
            sEarthquakeTimer--;
            Audio_PlaySfx_2(NA_SE_SY_EARTHQUAKE_OUTDOOR - SFX_FLAG);
        }
    }
}

/**
 * Sets the distortion to type 1 for a hot room
 * Remnant of OoT as no room in any MM scene is set to a hot-room
 */
s32 Camera_UpdateHotRoom(Camera* camera) {
    Distortion_RemoveRequest(DISTORTION_TYPE_HOT_ROOM);
    if (camera->play->roomCtx.curRoom.environmentType == ROOM_ENV_HOT) {
        Distortion_Request(DISTORTION_TYPE_HOT_ROOM);
    }
    return true;
}

s32 Camera_SetSwordDistortion(Camera* camera) {
    switch (func_800CBB88(camera)) {
        case 1:
            // non-magic spin attack
            if (Distortion_GetType() != DISTORTION_TYPE_ZORA_KICK) {
                Distortion_Request(DISTORTION_TYPE_ZORA_KICK);
                Distortion_SetDuration(12);
            }
            break;

        case 2:
            // Unused: case 2 is impossible to achieve
            if (Distortion_GetType() != DISTORTION_TYPE_UNK_ATTACK) {
                Distortion_Request(DISTORTION_TYPE_UNK_ATTACK);
                Distortion_SetDuration(5);
            }
            break;

        case 3:
            if (Distortion_GetType() != DISTORTION_TYPE_GORON_BUTT) {
                Distortion_Request(DISTORTION_TYPE_GORON_BUTT);
                Distortion_SetDuration(15);
            }
            break;

        default:
            break;
    }
    return true;
}

s32 Camera_RequestGiantsMaskSetting(Camera* camera) {
    Player* player = GET_PLAYER(camera->play);

    if ((camera->camId == CAM_ID_MAIN) && (camera->focalActor == &GET_PLAYER(camera->play)->actor) &&
        (player->currentMask == PLAYER_MASK_GIANT)) {
        Camera_ChangeSettingFlags(camera, CAM_SET_GIANT, CAM_CHANGE_SETTING_1);
        return true;
    } else {
        return false;
    }
}

Vec3s Camera_Update(Camera* camera) {
    Vec3f viewAt;
    Vec3f viewEye;
    Vec3f viewUp;
    Vec3f focalActorPos;
    s32 bgId;
    s32 sp98;
    s32 changeCamSceneDataType;
    CollisionPoly* sp90;
    CollisionPoly* sp8C;
    f32 runSpeedLimit;
    f32 speed;
    f32 viewFov;
    DynaPolyActor* meshActor;
    PosRot focalActorPosRot;
    ShakeInfo camShake;
    Actor* focalActor = camera->focalActor;
    VecGeo sp3C;
    s16 bgCamIndex;
    s16 numQuakesApplied;
    f32 focalActorFloorHeight;

    // Camera of status CUT only updates to this point
    if (camera->status == CAM_STATUS_CUT) {
        return camera->inputDir;
    }

    sUpdateCameraDirection = false;
    sIsFalse = false;

    if (camera->play->view.unk164 == 0) {
        if (camera->focalActor != NULL) {
            // Updates camera info on the actor it's tracking

            if (camera->focalActor == &GET_PLAYER(camera->play)->actor) {
                focalActorPosRot = Actor_GetWorldPosShapeRot(camera->focalActor);
            } else {
                focalActorPosRot = Actor_GetWorld(camera->focalActor);
            }
            camera->unk_0F0.x = focalActorPosRot.pos.x - camera->focalActorPosRot.pos.x;
            camera->unk_0F0.y = focalActorPosRot.pos.y - camera->focalActorPosRot.pos.y;
            camera->unk_0F0.z = focalActorPosRot.pos.z - camera->focalActorPosRot.pos.z;

            // bg related to tracked actor
            sp98 = 0;
            if (Camera_IsMountedOnHorse(camera)) {
                if (((Player*)focalActor)->rideActor->floorPoly != NULL) {
                    sp90 = ((Player*)focalActor)->rideActor->floorPoly;
                    camera->bgId = ((Player*)focalActor)->rideActor->floorBgId;
                    camera->focalActorFloorHeight = ((Player*)focalActor)->rideActor->floorHeight;
                    sp98 = 3;
                }
            } else if (func_800CB7CC(camera)) {
                if (camera->focalActor->floorPoly != NULL) {
                    sp90 = camera->focalActor->floorPoly;
                    camera->bgId = camera->focalActor->floorBgId;
                    camera->focalActorFloorHeight = camera->focalActor->floorHeight;
                    sp98 = 1;
                }
            } else {
                focalActorPos = focalActorPosRot.pos;
                focalActorPos.y += Camera_GetFocalActorHeight(camera);
                focalActorFloorHeight = BgCheck_EntityRaycastFloor5_3(camera->play, &camera->play->colCtx, &sp90, &bgId,
                                                                      camera->focalActor, &focalActorPos);
                if (focalActorFloorHeight != BGCHECK_Y_MIN) {
                    camera->bgId = bgId;
                    camera->focalActorFloorHeight = focalActorFloorHeight;
                    sp98 = 2;
                }
            }

            if ((sp98 != 0) && (Camera_fabsf(camera->focalActorPosRot.pos.y - camera->focalActorFloorHeight) < 11.0f)) {
                meshActor = DynaPoly_GetActor(&camera->play->colCtx, camera->bgId);
                if (meshActor != NULL) {
                    camera->floorNorm.x = COLPOLY_GET_NORMAL(sp90->normal.x);
                    camera->floorNorm.y = COLPOLY_GET_NORMAL(sp90->normal.y);
                    camera->floorNorm.z = COLPOLY_GET_NORMAL(sp90->normal.z);
                    camera->unk_0F0.x -= meshActor->actor.world.pos.x - camera->meshActorPos.x;
                    camera->unk_0F0.y -= meshActor->actor.world.pos.y - camera->meshActorPos.y;
                    camera->unk_0F0.z -= meshActor->actor.world.pos.z - camera->meshActorPos.z;
                    camera->meshActorPos = meshActor->actor.world.pos;
                }
            }

            // Set camera speed
            runSpeedLimit = Camera_GetRunSpeedLimit(camera) * 1.5f;
            speed = Camera_Vec3fMagnitude(&camera->unk_0F0);
            camera->xzSpeed = OLib_ClampMaxDist(speed, runSpeedLimit);
            camera->speedRatio = OLib_ClampMaxDist(speed / runSpeedLimit, 1.8f);
            camera->focalActorPosRot = focalActorPosRot;

            if (camera->camId == CAM_ID_MAIN) {
                Camera_UpdateWater(camera);
                Camera_UpdateHotRoom(camera);
                Camera_EarthquakeDay3(camera);
                Camera_SetSwordDistortion(camera);
            }

            /**
             * This section is about updating the camera setting based on the camera scene data
             *
             */

            // If doorTimer1 is active, set CAM_STATE_10 which suppresses bg camera scene data from being read
            if (camera->doorTimer1 != 0) {
                Camera_SetStateFlag(camera, CAM_STATE_10);
            } else if (!(camera->stateFlags & CAM_STATE_2)) {
                camera->nextCamSceneDataId = -1;
            }

            changeCamSceneDataType = 0; // default to no change in the cam scene data
            bgId = camera->bgId;

            // Sets the next cam scene data Index based on the bg surface
            if ((camera->stateFlags & CAM_STATE_0) && (camera->stateFlags & CAM_STATE_2) &&
                !(camera->stateFlags & CAM_STATE_10) &&
                (!(camera->stateFlags & CAM_STATE_9) || Camera_IsUnderwaterAsZora(camera)) &&
                !(camera->stateFlags & CAM_STATE_15) && !Camera_IsMountedOnHorse(camera) &&
                !Camera_RequestGiantsMaskSetting(camera) && !Camera_IsDekuHovering(camera) && (sp98 != 0)) {

                bgCamIndex = Camera_GetBgCamIndex(camera, &bgId, sp90);
                if ((bgCamIndex != -1) && (camera->bgId == BGCHECK_SCENE)) {
                    if (!func_800CBC00(camera)) {
                        camera->nextCamSceneDataId = bgCamIndex | CAM_DATA_IS_BG;
                    }
                }

                focalActorPos = focalActorPosRot.pos;
                focalActorPos.y += Camera_GetFocalActorHeight(camera);
                focalActorFloorHeight =
                    BgCheck_CameraRaycastFloor2(&camera->play->colCtx, &sp8C, &bgId, &focalActorPos);

                if ((focalActorFloorHeight != BGCHECK_Y_MIN) && (sp8C != sp90) && (bgId == BGCHECK_SCENE) &&
                    ((camera->focalActorFloorHeight - 2.0f) < focalActorFloorHeight)) {
                    bgCamIndex = Camera_GetBgCamIndex(camera, &bgId, sp8C);
                    if ((bgCamIndex != -1) && (bgId == BGCHECK_SCENE)) {
                        camera->nextCamSceneDataId = bgCamIndex | CAM_DATA_IS_BG;
                        changeCamSceneDataType = 1; // change cam scene data based on the bg cam data
                    }
                }
            }

            if (camera->doorTimer1 != 0) {
                camera->doorTimer1--;
                if (camera->doorTimer1 == 0) {
                    Camera_UnsetStateFlag(camera, CAM_STATE_10);
                    changeCamSceneDataType = 5; // change cam scene data based on the cutscene cam data
                }
            }

            if (((camera->camId == CAM_ID_MAIN) || (camera->stateFlags & CAM_STATE_6)) &&
                ((camera->bgId == BGCHECK_SCENE) || ((bgId == BGCHECK_SCENE) && (changeCamSceneDataType != 0))) &&
                (camera->nextCamSceneDataId != -1) && (camera->doorTimer1 == 0) &&
                ((Camera_fabsf(camera->focalActorPosRot.pos.y - camera->focalActorFloorHeight) < 11.0f) ||
                 (changeCamSceneDataType != 0)) &&
                (!(camera->stateFlags & CAM_STATE_9) || Camera_IsUnderwaterAsZora(camera))) {

                Camera_ChangeActorCsCamIndex(camera, camera->nextCamSceneDataId);
                camera->nextCamSceneDataId = -1;
                if (camera->doorTimer2 != 0) {
                    camera->doorTimer1 = camera->doorTimer2;
                    camera->doorTimer2 = 0;
                }
            }
        }

        // Camera of status WAIT only updates to this point
        if (camera->status == CAM_STATUS_WAIT) {
            return camera->inputDir;
        }

        camera->behaviorFlags = 0;
        Camera_UnsetStateFlag(camera, CAM_STATE_10 | CAM_STATE_DISABLE_MODE_CHANGE);
        Camera_SetStateFlag(camera, CAM_STATE_4);
    }

    // Call the camera update function
    if (GameInteractor_Should(VB_USE_CUSTOM_CAMERA, true, camera)) {
        sCameraUpdateHandlers[sCameraSettings[camera->setting].cameraModes[camera->mode].funcId](camera);
    }

    // Update the interface
    if (sCameraInitSceneTimer != 0) {
        sCameraInitSceneTimer--;
    }
    if (camera->status == CAM_STATUS_ACTIVE) {
        if (((sCameraInitSceneTimer != 0) || func_800CB854(camera)) && (camera->camId == CAM_ID_MAIN)) {
            // Surpresses the interface for the first few frames of a scene
            sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NONE_ALT, 0);
            Camera_UpdateInterface(sCameraInterfaceFlags);
        } else if ((camera->play->transitionMode != TRANS_MODE_OFF) && (camera->camId != CAM_ID_MAIN)) {
            sCameraInterfaceFlags = CAM_INTERFACE_FLAGS(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_IGNORE, 0);
            Camera_UpdateInterface(sCameraInterfaceFlags);
        } else {
            Camera_UpdateInterface(sCameraInterfaceFlags);
        }
    }

    // Camera of status UNK3 only updates to this point
    if (camera->status == CAM_STATUS_UNK3) {
        return camera->inputDir;
    }

    /**
     * This section is about updating view structs from the active camera,
     * which view uses to calculate the viewing/projection matrices
     */
    numQuakesApplied = Quake_Update(camera, &camShake);

    bgId = numQuakesApplied; // required to match

    if (numQuakesApplied != 0) {
        viewAt.x = camera->at.x + camShake.atOffset.x;
        viewAt.y = camera->at.y + camShake.atOffset.y;
        viewAt.z = camera->at.z + camShake.atOffset.z;
        viewEye.x = camera->eye.x + camShake.eyeOffset.x;
        viewEye.y = camera->eye.y + camShake.eyeOffset.y;
        viewEye.z = camera->eye.z + camShake.eyeOffset.z;
        sp3C = OLib_Vec3fDiffToVecGeo(&viewEye, &viewAt);
        viewUp = Camera_CalcUpVec(sp3C.pitch, sp3C.yaw, camera->roll + camShake.upRollOffset);
        viewFov = camera->fov + CAM_BINANG_TO_DEG(camShake.fovOffset);
    } else if (sIsFalse) {
        //! condition is impossible to achieve
        viewAt = camera->at;
        viewEye = camera->eye;
        sp3C = OLib_Vec3fDiffToVecGeo(&viewEye, &viewAt);
        viewUp = camera->up;
        viewFov = camera->fov;
    } else {
        viewAt = camera->at;
        viewEye = camera->eye;
        sp3C = OLib_Vec3fDiffToVecGeo(&viewEye, &viewAt);
        viewUp = Camera_CalcUpVec(sp3C.pitch, sp3C.yaw, camera->roll);
        viewFov = camera->fov;
    }

    // set view up
    if (camera->viewFlags & CAM_VIEW_UP) {
        camera->viewFlags &= ~CAM_VIEW_UP;
        viewUp = camera->up;
    } else {
        camera->up = viewUp;
    }

    camera->quakeOffset = camShake.eyeOffset;
    View_SetScale(&camera->play->view, (OREG(67) * 0.01f) + 1.0f);
    camera->play->view.fovy = viewFov;
    View_LookAt(&camera->play->view, &viewEye, &viewAt, &viewUp);
    camera->camDir.x = sp3C.pitch;
    camera->camDir.y = sp3C.yaw;
    camera->camDir.z = 0;

    if (!sUpdateCameraDirection) {
        camera->inputDir.x = sp3C.pitch;
        camera->inputDir.y = sp3C.yaw;
        camera->inputDir.z = 0;
    }

    GameInteractor_ExecuteAfterCameraUpdate(camera);

    return camera->inputDir;
}

s32 func_800DF498(Camera* camera) {
    Camera_SetStateFlag(camera, CAM_STATE_3 | CAM_STATE_2); // CAM_STATE_3 is set only immediately to be unset
    Camera_UnsetStateFlag(camera, CAM_STATE_12 | CAM_STATE_3);
    return true;
}

#define CAM_CHANGE_MODE_0 (1 << 0)
#define CAM_CHANGE_MODE_1 (1 << 1)
#define CAM_CHANGE_MODE_BATTLE (1 << 2)
#define CAM_CHANGE_MODE_FOLLOW_TARGET (1 << 3)
#define CAM_CHANGE_MODE_4 (1 << 4)
#define CAM_CHANGE_MODE_FIRST_PERSON (1 << 5)

s32 Camera_ChangeModeFlags(Camera* camera, s16 mode, u8 forceChange) {
    static s32 sModeChangeFlags = 0;

    if ((camera->setting == CAM_SET_TELESCOPE) && ((mode == CAM_MODE_FIRSTPERSON) || (mode == CAM_MODE_DEKUHIDE))) {
        forceChange = true;
    }

    // Mode change rejected by flag
    if ((camera->stateFlags & CAM_STATE_DISABLE_MODE_CHANGE) && !forceChange) {
        camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
        return -1;
    }

    // Mode change rejected by validModes
    if (!(sCameraSettings[camera->setting].validModes & (1 << mode))) {
        if (camera->mode != CAM_MODE_NORMAL) {
            camera->mode = CAM_MODE_NORMAL;
            Camera_ResetActionFuncState(camera, camera->mode);
            func_800DF498(camera);
            return mode | 0xC0000000;
        } else {
            camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
            camera->behaviorFlags |= CAM_BEHAVIOR_MODE_1;
            return 0;
        }
    }

    // Mode change rejected due to mode already being set. (otherwise, reset mode)
    if ((mode == camera->mode) && !forceChange) {
        camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
        return -1;
    }

    camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
    camera->behaviorFlags |= CAM_BEHAVIOR_MODE_1;

    Camera_ResetActionFuncState(camera, mode);

    sModeChangeFlags = 0;

    // Process Requested Camera Mode
    switch (mode) {
        case CAM_MODE_FIRSTPERSON:
            sModeChangeFlags = CAM_CHANGE_MODE_FIRST_PERSON;
            break;

        case CAM_MODE_BATTLE:
            sModeChangeFlags = CAM_CHANGE_MODE_BATTLE;
            break;

        case CAM_MODE_FOLLOWTARGET:
            if ((camera->target != NULL) && (camera->target->id != ACTOR_EN_BOOM)) {
                sModeChangeFlags = CAM_CHANGE_MODE_FOLLOW_TARGET;
            }
            break;

        case CAM_MODE_BOWARROWZ:
        case CAM_MODE_TARGET:
        case CAM_MODE_TALK:
        case CAM_MODE_HANGZ:
        case CAM_MODE_PUSHPULL:
            sModeChangeFlags = CAM_CHANGE_MODE_1;
            break;

        case CAM_MODE_NORMAL:
        case CAM_MODE_HANG:
            sModeChangeFlags = CAM_CHANGE_MODE_4;
            break;

        default:
            break;
    }

    // Process Current Camera Mode
    switch (camera->mode) {
        case CAM_MODE_FIRSTPERSON:
            if (sModeChangeFlags & CAM_CHANGE_MODE_FIRST_PERSON) {
                camera->animState = 10;
            }
            break;

        case CAM_MODE_JUMP:
        case CAM_MODE_HANG:
            if (sModeChangeFlags & CAM_CHANGE_MODE_4) {
                camera->animState = 20;
            }
            sModeChangeFlags |= CAM_CHANGE_MODE_0;
            break;

        case CAM_MODE_CHARGE:
            if (sModeChangeFlags & CAM_CHANGE_MODE_4) {
                camera->animState = 20;
            }
            sModeChangeFlags |= CAM_CHANGE_MODE_0;
            break;

        case CAM_MODE_FOLLOWTARGET:
            if (sModeChangeFlags & CAM_CHANGE_MODE_FOLLOW_TARGET) {
                camera->animState = 10;
            }
            sModeChangeFlags |= CAM_CHANGE_MODE_0;
            break;

        case CAM_MODE_BATTLE:
            if (sModeChangeFlags & CAM_CHANGE_MODE_BATTLE) {
                camera->animState = 10;
            }
            sModeChangeFlags |= 1;
            break;

        case CAM_MODE_BOWARROWZ:
        case CAM_MODE_HANGZ:
        case CAM_MODE_PUSHPULL:
            sModeChangeFlags |= CAM_CHANGE_MODE_0;
            break;

        case CAM_MODE_NORMAL:
            if (sModeChangeFlags & CAM_CHANGE_MODE_4) {
                camera->animState = 20;
            }
            break;

        default:
            break;
    }

    sModeChangeFlags &= ~CAM_CHANGE_MODE_4;

    // Z-Pressing Sfx
    if (camera->status == CAM_STATUS_ACTIVE) {
        switch (sModeChangeFlags) {
            case CAM_CHANGE_MODE_0:
                Audio_PlaySfx(0);
                break;

            case CAM_CHANGE_MODE_1:
                if (camera->play->roomCtx.curRoom.type == ROOM_TYPE_DUNGEON) {
                    Audio_PlaySfx(NA_SE_SY_ATTENTION_URGENCY);
                } else {

                    Audio_PlaySfx(NA_SE_SY_ATTENTION_ON);
                }
                break;

            case CAM_CHANGE_MODE_BATTLE:
                Audio_PlaySfx(NA_SE_SY_ATTENTION_URGENCY);
                break;

            case CAM_CHANGE_MODE_FOLLOW_TARGET:
                Audio_PlaySfx(NA_SE_SY_ATTENTION_ON);
                break;

            default:
                break;
        }
    }

    func_800DF498(camera);
    camera->mode = mode;
    GameInteractor_ExecuteOnCameraChangeModeFlags(camera);

    return mode | 0x80000000;
}

s32 Camera_ChangeMode(Camera* camera, s16 mode) {
    return Camera_ChangeModeFlags(camera, mode, false);
}

s32 Camera_CheckValidMode(Camera* camera, s16 mode) {
    if (camera->stateFlags & CAM_STATE_DISABLE_MODE_CHANGE) {
        return 0;
    } else if (!(sCameraSettings[camera->setting].validModes & (1 << mode))) {
        return 0;
    } else if (mode == camera->mode) {
        return -1;
    } else {
        return mode | 0x80000000;
    }
}

s16 Camera_ChangeSettingFlags(Camera* camera, s16 setting, s16 flags) {
    // Reject settings change based on priority
    if ((camera->behaviorFlags & CAM_BEHAVIOR_SETTING_USE_PRIORITY) &&
        ((sCameraSettings[camera->setting].flags & 0xF) >= (sCameraSettings[setting].flags & 0xF))) {
        camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_2;
        if (!(flags & CAM_CHANGE_SETTING_1)) {
            camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_USE_PRIORITY;
        }
        return -2;
    }

    // Reject settings change based on NONE setting
    if (setting == CAM_SET_NONE) {
        return 0;
    }

    // Reject settings change based on an invalid setting
    if (setting >= CAM_SET_MAX) {
        return -99;
    }

    // Reject settings change based on setting already set (and flags)
    if ((setting == camera->setting) && !(flags & CAM_CHANGE_SETTING_0)) {
        camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_2;
        if (!(flags & CAM_CHANGE_SETTING_1)) {
            camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_USE_PRIORITY;
        }
        return -1;
    }

    camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_2;

    if (!(flags & CAM_CHANGE_SETTING_1)) {
        camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_USE_PRIORITY;
    }

    func_800DF498(camera);

    if (!(sCameraSettings[camera->setting].flags & 0x40000000)) {
        camera->prevSetting = camera->setting;
    }

    if (flags & CAM_CHANGE_SETTING_3) {
        camera->bgCamIndex = camera->prevBgCamDataId;
        camera->prevBgCamDataId = -1;
    } else if (!(flags & CAM_CHANGE_SETTING_2)) {
        if (!(sCameraSettings[camera->setting].flags & 0x40000000)) {
            camera->prevBgCamDataId = camera->bgCamIndex;
        }
        camera->bgCamIndex = -1;
    }

    camera->setting = setting;

    GameInteractor_ExecuteOnCameraChangeSettingsFlags(camera);

    if (Camera_ChangeModeFlags(camera, camera->mode, true) >= 0) {
        Camera_ResetActionFuncState(camera, camera->mode);
    }

    return setting;
}

s32 Camera_ChangeSetting(Camera* camera, s16 setting) {
    s32 settingChangeSuccessful = Camera_ChangeSettingFlags(camera, setting, 0);

    if (settingChangeSuccessful >= 0) {
        camera->bgCamIndex = -1;
    }
    return settingChangeSuccessful;
}

s32 Camera_ChangeActorCsCamIndex(Camera* camera, s32 bgCamIndex) {
    s16 setting;

    if ((bgCamIndex == -1) || (bgCamIndex == camera->bgCamIndex)) {
        camera->behaviorFlags |= CAM_BEHAVIOR_BGCAM_2;
        return -1;
    }

    if (bgCamIndex < 0) {
        setting = sGlobalCamDataSettingsPtr[bgCamIndex];
    } else if (!(camera->behaviorFlags & CAM_BEHAVIOR_BGCAM_2)) {
        setting = Camera_GetBgCamOrActorCsCamSetting(camera, bgCamIndex);
    } else {
        return -1;
    }

    camera->behaviorFlags |= CAM_BEHAVIOR_BGCAM_2;

    // Sets camera setting based on bg/scene data
    if ((Camera_ChangeSettingFlags(camera, setting, CAM_CHANGE_SETTING_2 | CAM_CHANGE_SETTING_0) >= 0) ||
        (sCameraSettings[camera->setting].flags & 0x80000000)) {
        camera->bgCamIndex = bgCamIndex;
        camera->behaviorFlags |= CAM_BEHAVIOR_BGCAM_1;
        Camera_ResetActionFuncState(camera, camera->mode);
    }

    return bgCamIndex | 0x80000000;
}

Vec3s Camera_GetInputDir(Camera* camera) {
    return camera->inputDir;
}

s16 Camera_GetInputDirPitch(Camera* camera) {
    Vec3s dir = Camera_GetInputDir(camera);

    return dir.x;
}

s16 Camera_GetInputDirYaw(Camera* camera) {
    Vec3s dir = Camera_GetInputDir(camera);

    return dir.y;
}

Vec3s Camera_GetCamDir(Camera* camera) {
    return camera->camDir;
}

s16 Camera_GetCamDirPitch(Camera* camera) {
    Vec3s camDir = Camera_GetCamDir(camera);

    return camDir.x;
}

s16 Camera_GetCamDirYaw(Camera* camera) {
    Vec3s camDir = Camera_GetCamDir(camera);

    return camDir.y;
}

s32 Camera_AddQuake(Camera* camera, s32 arg1, s16 y, s32 countdown) {
    s16 quakeIndex = Quake_Request(camera, QUAKE_TYPE_3);

    if (quakeIndex == 0) {
        return false;
    }

    Quake_SetSpeed(quakeIndex, 25000);
    Quake_SetPerturbations(quakeIndex, y, 0, 0, 0);
    Quake_SetDuration(quakeIndex, countdown);
    return true;
}

s32 Camera_SetViewParam(Camera* camera, s32 viewFlag, void* param) {
    s32 pad[3];

    if (param != NULL) {
        switch (viewFlag) {
            case CAM_VIEW_AT:
                camera->viewFlags &= ~(CAM_VIEW_TARGET_POS | CAM_VIEW_TARGET | CAM_VIEW_AT);
                camera->at = *(Vec3f*)param;
                break;

            case CAM_VIEW_TARGET_POS:
                camera->viewFlags &= ~(CAM_VIEW_TARGET_POS | CAM_VIEW_TARGET | CAM_VIEW_AT);
                camera->targetPosRot.pos = *(Vec3f*)param;
                break;

            case CAM_VIEW_TARGET:
                camera->target = (Actor*)param;
                camera->viewFlags &= ~(CAM_VIEW_TARGET_POS | CAM_VIEW_TARGET | CAM_VIEW_AT);
                break;

            case CAM_VIEW_EYE:
                camera->eye = camera->eyeNext = *(Vec3f*)param;
                break;

            case CAM_VIEW_UP:
                camera->up = *(Vec3f*)param;
                break;

            case CAM_VIEW_ROLL:
                camera->roll = CAM_DEG_TO_BINANG(*(f32*)param);
                break;

            case CAM_VIEW_FOV:
                camera->fov = *(f32*)param;
                break;

            default:
                return false;
        }
        camera->viewFlags |= viewFlag;
    } else {
        return false;
    }
    return true;
}

s32 Camera_UnsetViewFlag(Camera* camera, s16 viewFlag) {
    camera->viewFlags &= ~viewFlag;
    return true;
}

s32 Camera_OverwriteStateFlags(Camera* camera, s16 stateFlags) {
    camera->stateFlags = stateFlags;
    return true;
}

s32 Camera_ResetActionFuncStateUnused(Camera* camera) {
    camera->animState = 0;
    return true;
}

/**
 * Unused Remnant of OoT. `CutsceneCameraPoint` struct no longer exists.
 */
s32 Camera_SetCsParams(Camera* camera, void* atPoints, void* eyePoints, Player* player, s16 relativeToPlayer) {
    return true;
}

s16 Camera_SetStateFlag(Camera* camera, s16 flags) {
    camera->stateFlags |= flags;
    return camera->stateFlags;
}

s16 Camera_UnsetStateFlag(Camera* camera, s16 flags) {
    camera->stateFlags &= ~flags;
    return camera->stateFlags;
}

s32 Camera_ChangeDoorCam(Camera* camera, Actor* doorActor, s16 bgCamIndex, f32 arg3, s16 timer1, s16 timer2,
                         s16 timer3) {
    DoorParams* doorParams = &camera->paramData.doorParams;

    if (camera->setting == CAM_SET_DOORC) {
        return 0;
    }

    doorParams->doorActor = doorActor;
    doorParams->timer1 = timer1;
    doorParams->timer2 = timer2;
    doorParams->timer3 = timer3;
    doorParams->bgCamIndex = bgCamIndex;

    if (bgCamIndex == -99) {
        Camera_ResetActionFuncState(camera, camera->mode);
        return -99;
    }

    if (bgCamIndex == -1) {
        Camera_ChangeSettingFlags(camera, CAM_SET_DOORC, 0);
    } else if (bgCamIndex == -2) {
        Camera_ChangeSettingFlags(camera, CAM_SET_SPIRAL_DOOR, 0);
    } else {
        camera->nextCamSceneDataId = bgCamIndex;
        camera->doorTimer1 = timer1;
        camera->doorTimer2 = timer2 + timer3;
    }

    Camera_ResetActionFuncState(camera, camera->mode);
    return -1;
}

s32 Camera_Copy(Camera* dstCam, Camera* srcCam) {
    s32 pad;

    dstCam->focalActorAtOffset.z = 0.0f;
    dstCam->focalActorAtOffset.y = 0.0f;
    dstCam->focalActorAtOffset.x = 0.0f;
    dstCam->atLerpStepScale = 0.1f;
    dstCam->at = srcCam->at;

    dstCam->eye = dstCam->eyeNext = srcCam->eye;

    dstCam->dist = OLib_Vec3fDist(&dstCam->at, &dstCam->eye);
    dstCam->fov = srcCam->fov;
    dstCam->roll = srcCam->roll;
    Camera_SetUpdateRatesSlow(dstCam);

    if (dstCam->focalActor != NULL) {
        if (dstCam->focalActor == &GET_PLAYER(dstCam->play)->actor) {
            dstCam->focalActorPosRot = Actor_GetWorldPosShapeRot(dstCam->focalActor);
        } else {
            dstCam->focalActorPosRot = Actor_GetWorld(dstCam->focalActor);
        }
        Camera_SetFocalActorAtOffset(dstCam, &dstCam->focalActorPosRot.pos);
    }
    return true;
}

/**
 * Unused Remnant of OoT/Debug
 */
s32 Camera_IsDbgCamEnabled(void) {
    return false;
}

Vec3f Camera_GetQuakeOffset(Camera* camera) {
    return camera->quakeOffset;
}

void Camera_SetCameraData(Camera* camera, s16 setDataFlags, void* data0, void* data1, s16 data2, s16 data3) {
    if (setDataFlags & 1) {
        camera->data0 = data0;
    }

    if (setDataFlags & 2) {
        camera->data1 = data1;
    }

    if (setDataFlags & 4) {
        camera->data2 = data2;
    }

    if (setDataFlags & 8) {
        camera->data3 = data3;
    }
}

s32 Camera_GetNegOne(void) {
    return sCameraNegOne;
}

s16 Camera_SetFinishedFlag(Camera* camera) {
    Camera_SetStateFlag(camera, CAM_STATE_3);
    if ((camera->camId == CAM_ID_MAIN) && (camera->play->activeCamId != CAM_ID_MAIN)) {
        Camera_SetStateFlag(GET_ACTIVE_CAM(camera->play), CAM_STATE_3);
        return camera->play->activeCamId;
    } else {
        return camera->camId;
    }
}

void Camera_SetFocalActor(Camera* camera, Actor* actor) {
    if (!GameInteractor_Should(VB_CAMERA_SET_FOCAL_ACTOR, true, actor)) {
        return;
    }

    camera->focalActor = actor;
    if (actor == &GET_PLAYER(camera->play)->actor) {
        camera->focalActorPosRot = Actor_GetWorldPosShapeRot(actor);
    } else {
        camera->focalActorPosRot = Actor_GetWorld(camera->focalActor);
    }

    camera->animState = 0;
}

void Camera_SetTargetActor(Camera* camera, Actor* actor) {
    camera->target = actor;
    camera->animState = 20;
}

f32 Camera_GetWaterYPos(Camera* camera) {
    if (camera->stateFlags & CAM_STATE_UNDERWATER) {
        return camera->waterYPos;
    } else {
        return BGCHECK_Y_MIN;
    }
}

void func_800E0348(Camera* camera) {
    if (!RELOAD_PARAMS(camera)) {
        camera->animState = 999;
        Camera_SetStateFlag(camera, CAM_STATE_10 | CAM_STATE_4 | CAM_STATE_2 | CAM_STATE_CHECK_WATER);
    } else {
        camera->animState = 666;
    }
}

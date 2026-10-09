#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "CameraUtils.h"

extern "C" {
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
extern PlayState* sCamPlayState;
extern f32 Camera_ScaledStepToCeilF(f32 target, f32 cur, f32 stepScale, f32 minDiff);
extern s16 Camera_ScaledStepToCeilS(s16 target, s16 cur, f32 stepScale, s16 minDiff);
extern Vec3f Camera_CalcUpVec(s16 pitch, s16 yaw, s16 roll);
}

#define CAMERA_DEBUG_DEFAULT_PORT 1

// Static Data Used For Free Camera
static bool sDebugCamRefreshParams = true;

Vec3f Camera_RotatePointAroundAxis(Vec3f* point, Vec3f* axis, s16 angle) {
    f32 q0 = Math_CosS(angle / 2);
    f32 q1 = Math_SinS(angle / 2) * axis->x;
    f32 q2 = Math_SinS(angle / 2) * axis->y;
    f32 q3 = Math_SinS(angle / 2) * axis->z;
    Vec3f endPoint;

    endPoint.x = (SQ(q0) + SQ(q1) - SQ(q2) - SQ(q3)) * point->x + 2 * (q1 * q2 - q0 * q3) * point->y +
                 2 * (q1 * q3 + q0 * q2) * point->z;
    endPoint.y = 2 * (q2 * q1 + q0 * q3) * point->x + (SQ(q0) - SQ(q1) + SQ(q2) - SQ(q3)) * point->y +
                 2 * (q2 * q3 - q0 * q1) * point->z;
    endPoint.z = 2 * (q3 * q1 - q0 * q2) * point->x + 2 * (q3 * q2 + q0 * q1) * point->y +
                 (SQ(q0) - SQ(q1) - SQ(q2) + SQ(q3)) * point->z;

    return endPoint;
}

void Camera_SetRollFromUp(Camera* camera) {
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* at = &camera->at;
    Vec3f* up = &camera->up;
    VecGeo diffGeo = OLib_Vec3fDiffToVecGeo(eyeNext, at);
    f32 pitch = diffGeo.pitch;
    f32 yaw = diffGeo.yaw;
    f32 sinP = Math_SinS(pitch);
    f32 cosP = Math_CosS(pitch);
    f32 sinY = Math_SinS(yaw);
    f32 cosY = Math_CosS(yaw);
    Vec3f preRollUp = { -sinP * sinY, cosP, -sinP * cosY };
    Vec3f normal;

    f32 dot = DOTXYZ(preRollUp, (*up));
    Math_Vec3f_Diff(eyeNext, at, &normal);
    Math3D_Normalize(&normal);
    // Setup Matrix With Normal
    //  ( preRollUp.x   up->x   normal.x)
    //  ( preRollUp.y   up->y   normal.y)
    //  ( preRollUp.z   up->z   normal.z)
    f32 det = preRollUp.x * up->y * normal.z + up->x * normal.y * preRollUp.z + normal.x * preRollUp.y * up->z -
              normal.x * up->y * preRollUp.z - normal.y * up->z * preRollUp.x - normal.z * up->x * preRollUp.y;

    camera->roll = RAD_TO_BINANG(atan2(-det, -dot) - M_PIf);
}

void Camera_RotateGeographic(Camera* camera, f32 pitchDiff, f32 yawDiff) {
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f camOut = OLib_Vec3fDistNormalize(at, eye);
    VecGeo transGeo = OLib_Vec3fToVecGeo(&camOut);
    transGeo.pitch += pitchDiff;
    transGeo.pitch = OLib_ClampMaxDist(transGeo.pitch, DEG_TO_BINANG(89.0f));
    transGeo.yaw += yawDiff;
    transGeo.r = camera->dist;
    *eyeNext = OLib_AddVecGeoToVec3f(at, &transGeo);
}

void Camera_Rotate6DOF(Camera* camera, f32 pitchDiff, f32 yawDiff) {
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* up = &camera->up;

    Vec3f camOut = OLib_Vec3fDistNormalize(at, eye);
    Math3D_Normalize(&camOut);
    Math3D_Normalize(up);
    Vec3f right;
    Math3D_Vec3f_Cross(up, &camOut, &right);
    Math3D_Normalize(&right);
    Vec3f force = { up->x * pitchDiff + right.x * yawDiff, up->y * pitchDiff + right.y * yawDiff,
                    up->z * pitchDiff + right.z * yawDiff };
    f32 angle = Math3D_Vec3fMagnitude(&force);

    Vec3f rotAxis;
    Math3D_Vec3f_Cross(&camOut, &force, &rotAxis);
    Math3D_Normalize(&rotAxis);

    Vec3f transPos = Camera_RotatePointAroundAxis(&camOut, &rotAxis, angle);
    VecGeo transGeo = OLib_Vec3fToVecGeo(&transPos);
    transGeo.r = camera->dist;
    *eyeNext = OLib_AddVecGeoToVec3f(at, &transGeo);
    *up = Camera_RotatePointAroundAxis(up, &rotAxis, angle);

    Camera_SetRollFromUp(camera);
}

void Camera_DebugCam(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecGeo eyeAdjustment;
    VecGeo originalEyeGeo = OLib_Vec3fDiffToVecGeo(at, eye);
    f32 distTarget;

    if (sDebugCamRefreshParams) {
        // Move camera focus to eye
        Vec3f unit = OLib_Vec3fDistNormalize(eye, at);
        Math_Vec3f_Sum(eye, &unit, at);
        sDebugCamRefreshParams = false;
    }

    s32 controllerPort = CVarGetInteger("gEnhancements.Camera.DebugCam.Port", CAMERA_DEBUG_DEFAULT_PORT) - 1;
    if (controllerPort > 3 || controllerPort < 0) {
        controllerPort = CAMERA_DEBUG_DEFAULT_PORT - 1;
        CVarSetInteger("gEnhancements.Camera.DebugCam.Port", CAMERA_DEBUG_DEFAULT_PORT);
    }

    /*
        Camera Speed
     */

    f32 camSpeed = CVarGetFloat("gEnhancements.Camera.DebugCam.CameraSpeed", 0.5f);
    if (CHECK_BTN_ANY(sCamPlayState->state.input[controllerPort].cur.button, BTN_A | BTN_B | BTN_L)) {
        camSpeed *= 3.0f;
    }

    /*
        Camera distance
     */

    // Transition speed set to be responsive
    f32 transitionSpeed = camSpeed * 200.0f;
    distTarget = camera->dist + (CHECK_BTN_ANY(sCamPlayState->state.input[controllerPort].cur.button, BTN_DDOWN) -
                                 CHECK_BTN_ANY(sCamPlayState->state.input[controllerPort].cur.button, BTN_DUP)) *
                                    1200.0f * camSpeed;
    distTarget = CLAMP_MIN(distTarget, 0.1f);
    // Smooth step camera away to max camera distance.
    camera->dist = Camera_ScaledStepToCeilF(distTarget, camera->dist,
                                            transitionSpeed / (ABS(distTarget - camera->dist) + transitionSpeed), 0.0f);

    /*
        Set Camera Pitch and Yaw
     */
    f32 yawDiff = -sCamPlayState->state.input[controllerPort].cur.right_stick_x * 10.0f *
                  (CVarGetFloat("gEnhancements.Camera.RightStick.CameraSensitivity.X", 1.0f));
    f32 pitchDiff = sCamPlayState->state.input[controllerPort].cur.right_stick_y * 10.0f *
                    (CVarGetFloat("gEnhancements.Camera.RightStick.CameraSensitivity.Y", 1.0f));

    yawDiff *= GameInteractor_InvertControl(GI_INVERT_CAMERA_RIGHT_STICK_X);
    pitchDiff *= -GameInteractor_InvertControl(GI_INVERT_CAMERA_RIGHT_STICK_Y);

    // Setup new camera angle based on the calculations from right stick inputs
    if (CVarGetInteger("gEnhancements.Camera.DebugCam.6DOF", 0)) {
        Camera_Rotate6DOF(camera, pitchDiff, yawDiff);
    } else {
        Camera_RotateGeographic(camera, pitchDiff, yawDiff);
    }

    /*
        Camera Roll
     */
    if (CVarGetInteger("gEnhancements.Camera.DebugCam.6DOF", 0)) {
        camera->roll += (CHECK_BTN_ANY(sCamPlayState->state.input[controllerPort].cur.button, BTN_DLEFT) -
                         CHECK_BTN_ANY(sCamPlayState->state.input[controllerPort].cur.button, BTN_DRIGHT)) *
                        1200.0f * camSpeed * GameInteractor_InvertControl(GI_INVERT_DEBUG_DPAD_X);
    } else {
        camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.1f, 5);
    }
    /*
        Camera Movement
     */

    // Movement differences from the point of view of the camera. Use max stick value for buttons scale
    Vec3f posDiff;
    posDiff.x = sCamPlayState->state.input[controllerPort].cur.stick_x * camSpeed;
    posDiff.y = (CHECK_BTN_ANY(sCamPlayState->state.input[controllerPort].cur.button, BTN_Z) -
                 CHECK_BTN_ANY(sCamPlayState->state.input[controllerPort].cur.button, BTN_R)) *
                120.0f * camSpeed;
    posDiff.z = -sCamPlayState->state.input[controllerPort].cur.stick_y * camSpeed;

    // Adjust movement to camera's current direction
    Vec3f camPosDiff = { 0.0f, 0.0f, 0.0f };

    VecGeo diffGeo = OLib_Vec3fDiffToVecGeo(eyeNext, at);
    Vec3f normX;
    Vec3f normY = camera->up = Camera_CalcUpVec(diffGeo.pitch, diffGeo.yaw, camera->roll);
    Vec3f normZ = OLib_Vec3fDistNormalize(at, eyeNext);

    // Cross Product
    Math3D_Vec3f_Cross(&normY, &normZ, &normX);

    Math_Vec3f_Scale(&normX, posDiff.x);
    Math_Vec3f_Scale(&normY, posDiff.y);
    Math_Vec3f_Scale(&normZ, posDiff.z);

    Math_Vec3f_Sum(&camPosDiff, &normX, &camPosDiff);
    Math_Vec3f_Sum(&camPosDiff, &normY, &camPosDiff);
    Math_Vec3f_Sum(&camPosDiff, &normZ, &camPosDiff);

    // "Move" camera eye around with controls
    Math_Vec3f_Sum(at, &camPosDiff, at);
    Math_Vec3f_Sum(eyeNext, &camPosDiff, eyeNext);

    *eye = *eyeNext;
}

void RegisterDebugCam() {
    sDebugCamRefreshParams = true;

    COND_VB_SHOULD(VB_USE_CUSTOM_CAMERA, CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0), {
        Camera* camera = va_arg(args, Camera*);
        Camera_DebugCam(camera);
        *should = false;
    });

    COND_HOOK(OnPassPlayerInputs, CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0), [](Input* input) {
        s32 controllerPort = CVarGetInteger("gEnhancements.Camera.DebugCam.Port", CAMERA_DEBUG_DEFAULT_PORT) - 1;
        if (controllerPort > 3 || controllerPort < 0) {
            controllerPort = CAMERA_DEBUG_DEFAULT_PORT - 1;
            CVarSetInteger("gEnhancements.Camera.DebugCam.Port", CAMERA_DEBUG_DEFAULT_PORT);
        }
        if (controllerPort == 0) {
            // Disable Link Inputs
            memset(input, 0, sizeof(Input));
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDebugCam, { "gEnhancements.Camera.DebugCam.Enable" });

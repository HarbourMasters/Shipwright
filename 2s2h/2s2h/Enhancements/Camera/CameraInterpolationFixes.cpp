#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "CameraUtils.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
extern CameraSetting sCameraSettings[];
}

// This function should check for outstanding conditions which the distance check would falsly flag
// as something we do not want to interpolate. This mostly applies to general play for now
bool Camera_ShouldOverrideInterpolationCheck(Camera* camera) {
    switch (sCameraSettings[camera->setting].cameraModes[camera->mode].funcId) {
        case CAM_FUNC_BATTLE0:
        case CAM_FUNC_BATTLE1:
        case CAM_FUNC_BATTLE2:
        case CAM_FUNC_BATTLE3:
        case CAM_FUNC_BATTLE4:
        case CAM_FUNC_PARALLEL0:
        case CAM_FUNC_PARALLEL1:
        case CAM_FUNC_PARALLEL2:
        case CAM_FUNC_PARALLEL3:
        case CAM_FUNC_PARALLEL4:
            return true;
    }
    return false;
}

// This function checks whether there is too large a distance or change in angle between the expected
// position camera and the actual position of the camera. If there is, then we should not interpolate
bool Camera_ShouldInterpolateDist(Camera* camera) {
    // Account for changes in position, pitch and yaw. Roll is rarely used so not currently handled
    // `Velocity` is measured as the change across the previous frame
    static f32 lastYaw = 0.0f;
    static f32 lastYawVelocity = 0.0f;
    static f32 lastPitch = 0.0f;
    static f32 lastPitchVelocity = 0.0f;
    static Vec3f lastEye = { 0.0f, 0.0f, 0.0f };
    static Vec3f lastEyeVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f eyeVelo;
    bool shouldInterpolate = true;
    VecGeo eyeGeo = OLib_Vec3fDiffToVecGeo(at, eye);
    Vec3f expectedEye;

    // Calculate Current
    f32 yaw = BINANG_TO_DEG(eyeGeo.yaw);
    if (yaw > 360.0f) {
        yaw -= 360.0f;
    } else if (yaw < 0.0f) {
        yaw += 360.0f;
    }
    f32 yawVelocity = yaw - lastYaw;
    f32 pitch = BINANG_TO_DEG(eyeGeo.pitch);
    f32 pitchVelocity = pitch - lastPitch;
    Math_Vec3f_Diff(eye, &lastEye, &eyeVelo);

    // Update static variables.
    lastYaw = yaw;
    lastYawVelocity = yawVelocity;
    lastPitch = pitch;
    lastPitchVelocity = pitchVelocity;
    lastEye = *eye;
    lastEyeVelocity = eyeVelo;

    if (Camera_ShouldOverrideInterpolationCheck(camera)) {
        return true;
    }

    // Calculate Expected
    f32 expectedYaw = lastYaw + lastYawVelocity;
    if (expectedYaw > 360.0f) {
        expectedYaw -= 360.0f;
    } else if (expectedYaw < 0.0f) {
        expectedYaw += 360.0f;
    }
    f32 expectedPitch = CLAMP(lastPitch + lastPitchVelocity, -180.0f, 180.0f);
    Math_Vec3f_Sum(&lastEye, &lastEyeVelocity, &expectedEye);

    // Check if changes are too great
    f32 diffYaw = fabsf(yaw - expectedYaw);
    f32 diffPitch = fabsf(pitch - expectedPitch);
    f32 diffDistEye = Math_Vec3f_DistXYZ(eye, &expectedEye);

    if ((diffYaw > 90.0f && diffYaw < 270.0f) || diffPitch > 60.0f || diffDistEye > 200.0f) {
        shouldInterpolate = false;
    }

    // If we aren't interpolating, then reset velocities as they are inaccurate to the camera's current movement
    if (!shouldInterpolate) {
        lastYawVelocity = 0.0f;
        lastPitchVelocity = 0.0f;
        lastEyeVelocity = { 0.0f, 0.0f, 0.0f };
    }

    return shouldInterpolate;
}

void RegisterCameraInterpolationFixes() {
    COND_HOOK(AfterCameraUpdate, true,
              [](Camera* camera) { FrameInterpolation_ShouldInterpolateFrame(Camera_ShouldInterpolateDist(camera)); });
}

static RegisterShipInitFunc initFunc(RegisterCameraInterpolationFixes, {});

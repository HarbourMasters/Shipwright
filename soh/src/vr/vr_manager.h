#pragma once

#include <stddef.h>     // for size_t
#include <stdint.h>     // for standard integer types
#include <stdlib.h>     // for standard library functions
#include <openvr.h>
#include "global.h"

class VRManager {
private:
    vr::IVRSystem* m_pHMD;
    vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    Matrix4 m_mat4ProjectionLeft;
    Matrix4 m_mat4ProjectionRight;
    Matrix4 m_mat4eyePosLeft;
    Matrix4 m_mat4eyePosRight;
    
public:
    bool InitVR();
    void ShutdownVR();
    void UpdateHMDMatrixPose();
    Matrix4 GetHMDMatrixProjectionEye(vr::Hmd_Eye eye);
    Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye eye);
    Vec3f GetHMDRotation();
    bool IsHMDPresent() { return vr::VR_IsHMDPresent(); }
    bool IsControllerActive(vr::ETrackedControllerRole role);
    Vec3f GetControllerDirection(vr::ETrackedControllerRole role);
}; 
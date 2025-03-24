#include "vr_manager.h"
#include "global.h"

// Global VR manager instance
VRManager* gVRManager = NULL;

bool VRManager_InitVR(VRManager* manager) {
    EVRInitError eError = EVRInitError_VRInitError_None;
    
    osSyncPrintf("Initializing OpenVR...\n");
    
    // Initialize OpenVR
    VR_InitInternal(&eError, EVRApplicationType_VRApplication_Scene);
    if (eError != EVRInitError_VRInitError_None) {
        osSyncPrintf("OpenVR initialization failed with error: %d\n", eError);
        return false;
    }
    
    osSyncPrintf("OpenVR initialized successfully\n");
    
    // Get system and compositor interfaces
    osSyncPrintf("Getting VR system interface...\n");
    manager->pHMD = VR_GetGenericInterface(IVRSystem_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || !manager->pHMD) {
        osSyncPrintf("Failed to get VR system interface, error: %d\n", eError);
        VR_ShutdownInternal();
        return false;
    }
    
    osSyncPrintf("Getting VR compositor interface...\n");
    manager->pCompositor = VR_GetGenericInterface(IVRCompositor_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || !manager->pCompositor) {
        osSyncPrintf("Failed to get VR compositor interface, error: %d\n", eError);
        VR_ShutdownInternal();
        return false;
    }
    
    osSyncPrintf("VR interfaces initialized successfully\n");
    
    // Store the global instance
    gVRManager = manager;
    osSyncPrintf("VR manager stored globally\n");
    return true;
}

void VRManager_ShutdownVR(VRManager* manager) {
    if (manager) {
        VR_ShutdownInternal();
        gVRManager = NULL;
    }
}

void VRManager_UpdateHMDMatrixPose(VRManager* manager) {
    osSyncPrintf("VRManager_UpdateHMDMatrixPose called\n");
    
    if (!manager) {
        osSyncPrintf("VR manager is NULL\n");
        return;
    }
    
    if (!manager->pCompositor) {
        osSyncPrintf("VR compositor is NULL\n");
        return;
    }

    if (!manager->pHMD) {
        osSyncPrintf("VR HMD interface is NULL\n");
        return;
    }
    
    osSyncPrintf("Checking if HMD is present...\n");
    if (!VR_IsHmdPresent()) {
        osSyncPrintf("No HMD detected\n");
        return;
    }
    osSyncPrintf("HMD is present\n");
    
    osSyncPrintf("Getting latest poses from compositor...\n");
    // Get latest poses
    EVRCompositorError compositorError = manager->pCompositor->GetLastPoses(manager->rTrackedDevicePose, k_unMaxTrackedDeviceCount, NULL, 0);
    if (compositorError != EVRCompositorError_VRCompositorError_None) {
        osSyncPrintf("GetLastPoses failed with error: %d\n", compositorError);
        return;
    }
    osSyncPrintf("Got poses from compositor\n");
    
    // Check if HMD pose is valid
    if (!manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
        osSyncPrintf("HMD pose is not valid\n");
    } else {
        osSyncPrintf("HMD pose is valid\n");
        // Log the pose matrix for debugging
        HmdMatrix34_t* pose = &manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
        osSyncPrintf("HMD Pose Matrix:\n");
        for (int i = 0; i < 3; i++) {
            osSyncPrintf("[%f %f %f %f]\n", 
                pose->m[i][0], pose->m[i][1], pose->m[i][2], pose->m[i][3]);
        }
    }
}

MtxF VRManager_GetHMDMatrixProjectionEye(VRManager* manager, EVREye eye) {
    MtxF result;
    
    if (!manager || !manager->pHMD) {
        // Return identity matrix if no HMD
        guMtxIdentF(&result);
        return result;
    }
    
    // Get projection matrix from OpenVR
    HmdMatrix44_t proj = manager->pHMD->GetProjectionMatrix(eye, 0.1f, 100.0f);
    
    // Convert to MtxF format
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.mf[i][j] = proj.m[i][j];
        }
    }
    
    return result;
}

MtxF VRManager_GetHMDMatrixPoseEye(VRManager* manager, EVREye eye) {
    MtxF result;
    
    if (!manager || !manager->pHMD) {
        // Return identity matrix if no HMD
        guMtxIdentF(&result);
        return result;
    }
    
    // Get eye-to-head transform from OpenVR
    HmdMatrix34_t eyeMatrix = manager->pHMD->GetEyeToHeadTransform(eye);
    
    // Convert to MtxF format (3x4 to 4x4)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            result.mf[i][j] = eyeMatrix.m[i][j];
        }
    }
    // Set bottom row to [0 0 0 1]
    result.mf[3][0] = 0.0f;
    result.mf[3][1] = 0.0f;
    result.mf[3][2] = 0.0f;
    result.mf[3][3] = 1.0f;
    
    return result;
}

Vec3f VRManager_GetHMDRotation(VRManager* manager) {
    Vec3f rotation = {0.0f, 0.0f, 0.0f};
    
    if (!manager || !manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
        return rotation;
    }
    
    // Get rotation from pose matrix
    HmdMatrix34_t* pose = &manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
    
    // Convert matrix to Euler angles (simplified)
    rotation.y = atan2f(pose->m[0][2], pose->m[2][2]); // Yaw
    rotation.x = -asinf(pose->m[1][2]); // Pitch
    rotation.z = atan2f(pose->m[1][0], pose->m[1][1]); // Roll
    
    return rotation;
}

bool VRManager_IsHMDPresent(void) {
    return VR_IsHmdPresent();
}

bool VRManager_IsControllerActive(VRManager* manager, ETrackedControllerRole role) {
    if (!manager || !manager->pHMD) return false;
    
    // Get the device index for the controller role
    TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    
    return deviceIndex != k_unTrackedDeviceIndexInvalid &&
           manager->rTrackedDevicePose[deviceIndex].bPoseIsValid;
}

Vec3f VRManager_GetControllerDirection(VRManager* manager, ETrackedControllerRole role) {
    Vec3f direction = {0.0f, 0.0f, -1.0f}; // Forward by default
    
    if (!manager || !manager->pHMD) return direction;
    
    // Get the device index for the controller role
    TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    
    if (deviceIndex != k_unTrackedDeviceIndexInvalid && 
        manager->rTrackedDevicePose[deviceIndex].bPoseIsValid) {
        // Get direction from pose matrix
        HmdMatrix34_t* pose = &manager->rTrackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking;
        
        // Forward vector is -Z axis of the transform matrix
        direction.x = -pose->m[0][2];
        direction.y = -pose->m[1][2];
        direction.z = -pose->m[2][2];
    }
    
    return direction;
} 
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
        exit(1);
    }
    
    osSyncPrintf("OpenVR initialized successfully\n");
    
    // Get system and compositor interfaces
    osSyncPrintf("Getting VR system interface...\n");
    osSyncPrintf("About to call VR_GetGenericInterface with version: %s\n", IVRSystem_Version);
    manager->pHMD = VR_GetGenericInterface(IVRSystem_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || !manager->pHMD) {
        osSyncPrintf("Failed to get VR system interface, OpenVR error: %d\n", eError);
        exit(1);
    }
    osSyncPrintf("Successfully got VR system interface\n");
    
    osSyncPrintf("Getting VR compositor interface...\n");
    manager->pCompositor = VR_GetGenericInterface(IVRCompositor_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || !manager->pCompositor) {
        osSyncPrintf("Failed to get VR compositor interface, error: %d\n", eError);
        exit(1);
    }
    
    osSyncPrintf("VR interfaces initialized successfully\n");
    
    // Initialize tracked device poses array
    memset(manager->rTrackedDevicePose, 0, sizeof(manager->rTrackedDevicePose));
    
    // Store the global instance
    gVRManager = manager;
    osSyncPrintf("VR manager stored globally\n");
    
    // Submit initial test frame
    VRManager_SubmitTestFrame(manager);
    
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
    
    // Set up a default pose for basic stereo display
    // This gives us a fixed forward-facing view
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking.m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].bPoseIsValid = true;
    
    /* Commented out pose tracking for now
    osSyncPrintf("Getting latest poses from compositor...\n");
    
    osSyncPrintf("About to call GetLastPoses...\n");
    EVRCompositorError compositorError = manager->pCompositor->GetLastPoses(manager->rTrackedDevicePose, k_unMaxTrackedDeviceCount, NULL, 0);
    osSyncPrintf("GetLastPoses returned with error: %d\n", compositorError);
    
    if (compositorError != EVRCompositorError_VRCompositorError_None) {
        osSyncPrintf("GetLastPoses failed with error: %d\n", compositorError);
        return;
    }
    
    osSyncPrintf("Got poses from compositor\n");
    */
    
    osSyncPrintf("Using fixed forward-facing pose for basic stereo display\n");
}

MtxF VRManager_GetHMDMatrixProjectionEye(VRManager* manager, EVREye eye) {
    MtxF result;
    
    osSyncPrintf("Getting projection matrix for eye %d\n", eye);
    
    // Default stereo projection values
    float nearZ = 0.1f;
    float farZ = 100.0f;
    float fov = 90.0f * (M_PI / 180.0f); // 90 degrees in radians
    float aspect = 1.0f;
    
    // Offset IPD for each eye
    float ipd = 0.064f; // Average human IPD in meters
    float eyeOffset = (eye == EVREye_Eye_Left) ? -ipd/2 : ipd/2;
    
    // Create perspective projection matrix
    float f = 1.0f / tanf(fov / 2.0f);
    
    // Zero out the matrix first
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.mf[i][j] = 0.0f;
        }
    }
    
    result.mf[0][0] = f / aspect;
    result.mf[1][1] = f;
    result.mf[2][2] = -(farZ + nearZ) / (farZ - nearZ);
    result.mf[2][3] = -1.0f;
    result.mf[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
    
    // Add eye offset
    result.mf[0][3] = eyeOffset;
    
    osSyncPrintf("Created default projection matrix for eye %d\n", eye);
    return result;
}

MtxF VRManager_GetHMDMatrixPoseEye(VRManager* manager, EVREye eye) {
    MtxF result;
    
    osSyncPrintf("Getting eye-to-head transform for eye %d\n", eye);
    
    // Create identity matrix
    guMtxIdentF(&result);
    
    // Add eye offset
    float ipd = 0.064f; // Average human IPD in meters
    float eyeOffset = (eye == EVREye_Eye_Left) ? -ipd/2 : ipd/2;
    result.mf[0][3] = eyeOffset;
    
    osSyncPrintf("Created default eye-to-head transform\n");
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

// New function to submit test frames to VR
void VRManager_SubmitTestFrame(VRManager* manager) {
    if (!manager || !manager->pCompositor) {
        return;
    }
    
    osSyncPrintf("Submitting test frame to VR...\n");
    
    // Create a simple colored texture for each eye
    uint32_t width = 1024;
    uint32_t height = 1024;
    uint32_t* leftEyeData = malloc(width * height * sizeof(uint32_t));
    uint32_t* rightEyeData = malloc(width * height * sizeof(uint32_t));
    
    if (!leftEyeData || !rightEyeData) {
        osSyncPrintf("Failed to allocate test frame buffers\n");
        return;
    }
    
    // Fill left eye with red, right eye with blue
    for (uint32_t i = 0; i < width * height; i++) {
        leftEyeData[i] = 0xFF0000FF;  // Red
        rightEyeData[i] = 0xFFFF0000;  // Blue
    }
    
    // Submit to compositor
    Texture_t leftEyeTexture = {leftEyeData, ETextureType_TextureType_OpenGL, EColorSpace_ColorSpace_Auto};
    Texture_t rightEyeTexture = {rightEyeData, ETextureType_TextureType_OpenGL, EColorSpace_ColorSpace_Auto};
    
    VRTextureBounds_t bounds = {0.0f, 0.0f, 1.0f, 1.0f};  // Full texture bounds
    
    EVRCompositorError error;
    error = manager->pCompositor->Submit(EVREye_Eye_Left, &leftEyeTexture, &bounds, EVRSubmitFlags_Submit_Default);
    if (error != EVRCompositorError_VRCompositorError_None) {
        osSyncPrintf("Failed to submit left eye texture: %d\n", error);
    }
    
    error = manager->pCompositor->Submit(EVREye_Eye_Right, &rightEyeTexture, &bounds, EVRSubmitFlags_Submit_Default);
    if (error != EVRCompositorError_VRCompositorError_None) {
        osSyncPrintf("Failed to submit right eye texture: %d\n", error);
    }
    
    // Cleanup
    free(leftEyeData);
    free(rightEyeData);
    
    osSyncPrintf("Test frame submitted\n");
} 
typedef enum {
    /* 0x00 */ CAM_MODE_NORMAL,
    /* 0x01 */ CAM_MODE_FOLLOWTARGET,
    /* 0x02 */ CAM_MODE_COMBAT,
    /* 0x03 */ CAM_MODE_BOWARROW,
    /* 0x04 */ CAM_MODE_BOWARROWZ,
    /* 0x05 */ CAM_MODE_HOOKSHOT,
    /* 0x06 */ CAM_MODE_BOOMERANG,
    /* 0x07 */ CAM_MODE_SLINGSHOT,
    /* 0x08 */ CAM_MODE_CLIMBZ,
    /* 0x09 */ CAM_MODE_JUMP,
    /* 0x0A */ CAM_MODE_HANG,
    /* 0x0B */ CAM_MODE_HANGZ,
    /* 0x0C */ CAM_MODE_FREEFALL,
    /* 0x0D */ CAM_MODE_CHARGE,
    /* 0x0E */ CAM_MODE_STILL,
    /* 0x0F */ CAM_MODE_PUSHPULL,
    /* 0x10 */ CAM_MODE_FOLLOWBOOMERANG,
    /* 0x11 */ CAM_MODE_VR,
    /* 0x12 */ CAM_MODE_MAX
} CameraMode;

// Function declarations
s32 Camera_Free(Camera* camera);
s32 Camera_VR(Camera* camera); 
#include "prevent_bss_reordering.h"
#include "global.h"
#include "z64quake.h"
#include "z64view.h"

#include "2s2h/GameInteractor/GameInteractor.h"

typedef struct {
    /* 0x00 */ s16 index;
    /* 0x02 */ s16 duration;
    /* 0x04 */ Camera* camera;
    /* 0x08 */ u32 type;
    /* 0x0C */ s16 y;
    /* 0x0E */ s16 x;
    /* 0x10 */ s16 fov;
    /* 0x12 */ s16 upRollOffset;
    /* 0x14 */ Vec3s orientation; // alters the orientation of the xy perturbation. Only x (pitch) and y (yaw) are used
    /* 0x1A */ s16 speed;
    /* 0x1C */ s16 isRelativeToScreen; // is the quake relative to the screen or on world coordinates
    /* 0x1E */ s16 timer;
    /* 0x20 */ s16 camId;
} QuakeRequest; // size = 0x24

typedef struct {
    /* 0x0 */ PlayState* play;
    /* 0x4 */ s32 type; // bitfield, highest set bit determines type
    /* 0x8 */ s16 timer;
    /* 0xA */ s16 state;
} DistortionRequest; // size = 0xC

typedef s16 (*QuakeCallbackFunc)(QuakeRequest*, ShakeInfo*);

typedef enum {
    /* 0 */ DISTORTION_INACTIVE,
    /* 1 */ DISTORTION_ACTIVE,
    /* 2 */ DISTORTION_SETUP
} DistortionState;

QuakeRequest sQuakeRequests[4];
DistortionRequest sDistortionRequest;

static s16 sQuakeUnused = 1;
static s16 sQuakeRequestCount = 0;

f32 Quake_Random(void) {
    return 2.0f * (Rand_ZeroOne() - 0.5f);
}

void Quake_UpdateShakeInfo(QuakeRequest* req, ShakeInfo* shake, f32 yOffset, f32 xOffset) {
    Vec3f* at = &req->camera->at;
    Vec3f* eye = &req->camera->eye;
    Vec3f atEyeOffset;
    VecSph geo;
    VecSph eyeToAtGeo;

    if (req->isRelativeToScreen) {
        atEyeOffset.x = 0;
        atEyeOffset.y = 0;
        atEyeOffset.z = 0;
        eyeToAtGeo = OLib_Vec3fDiffToVecGeo(eye, at);

        // y shake
        geo.r = req->y * yOffset;
        // point unit vector up, then add on `req->orientation`
        geo.pitch = eyeToAtGeo.pitch + req->orientation.x + 0x4000;
        geo.yaw = eyeToAtGeo.yaw + req->orientation.y;
        // apply y shake
        atEyeOffset = OLib_AddVecGeoToVec3f(&atEyeOffset, &geo);

        // x shake
        geo.r = req->x * xOffset;
        // point unit vector left, then add on `req->orientation`
        geo.pitch = eyeToAtGeo.pitch + req->orientation.x;
        geo.yaw = eyeToAtGeo.yaw + req->orientation.y + 0x4000;
        // apply x shake
        atEyeOffset = OLib_AddVecGeoToVec3f(&atEyeOffset, &geo);
    } else {
        atEyeOffset.x = 0;
        atEyeOffset.y = req->y * yOffset;
        atEyeOffset.z = 0;

        geo.r = req->x * xOffset;
        geo.pitch = req->orientation.x;
        geo.yaw = req->orientation.y;

        atEyeOffset = OLib_AddVecGeoToVec3f(&atEyeOffset, &geo);
    }

    shake->atOffset = shake->eyeOffset = atEyeOffset;
    shake->upRollOffset = req->upRollOffset * yOffset;
    shake->fovOffset = req->fov * yOffset;
}

s16 Quake_CallbackType1(QuakeRequest* req, ShakeInfo* shake) {
    if (req->timer > 0) {
        f32 xyOffset = Math_SinS(req->speed * req->timer);

        Quake_UpdateShakeInfo(req, shake, xyOffset, Rand_ZeroOne() * xyOffset);
        req->timer--;
    }
    return req->timer;
}

s16 Quake_CallbackType5(QuakeRequest* req, ShakeInfo* shake) {
    if (req->timer > 0) {
        f32 xyOffset = Math_SinS(req->speed * req->timer);

        Quake_UpdateShakeInfo(req, shake, xyOffset, xyOffset);
        req->timer--;
    }
    return req->timer;
}

s16 Quake_CallbackType6(QuakeRequest* req, ShakeInfo* shake) {
    f32 xyOffset;

    req->timer--;
    xyOffset = Math_SinS(req->speed * ((req->timer & 0xF) + 500));
    Quake_UpdateShakeInfo(req, shake, xyOffset, Rand_ZeroOne() * xyOffset);

    // Not returning the timer ensures quake type 6 continues indefinitely until manually removed
    return 1;
}

s16 Quake_CallbackType3(QuakeRequest* req, ShakeInfo* shake) {
    if (req->timer > 0) {
        f32 xyOffset = Math_SinS(req->speed * req->timer) * ((f32)req->timer / req->duration);

        Quake_UpdateShakeInfo(req, shake, xyOffset, xyOffset);
        req->timer--;
    }
    return req->timer;
}

s16 Quake_CallbackType2(QuakeRequest* req, ShakeInfo* shake) {
    if (req->timer > 0) {
        f32 xyOffset = Quake_Random();

        Quake_UpdateShakeInfo(req, shake, xyOffset, Rand_ZeroOne() * xyOffset);
        req->timer--;
    }
    return req->timer;
}

s16 Quake_CallbackType4(QuakeRequest* req, ShakeInfo* shake) {
    if (req->timer > 0) {
        f32 xyOffset = Quake_Random() * ((f32)req->timer / req->duration);

        Quake_UpdateShakeInfo(req, shake, xyOffset, Rand_ZeroOne() * xyOffset);
        req->timer--;
    }
    return req->timer;
}

s16 Quake_GetFreeIndex(void) {
    s32 i;
    s32 index = 0;
    s32 timerMin = UINT16_MAX + 1; // timer is a short, so start with a value beyond its range

    for (i = 0; i < ARRAY_COUNT(sQuakeRequests); i++) {
        if (sQuakeRequests[i].type == QUAKE_TYPE_NONE) {
            index = i;
            break;
        }

        if (timerMin > sQuakeRequests[i].timer) {
            timerMin = sQuakeRequests[i].timer;
            index = i;
        }
    }

    return index;
}

QuakeRequest* Quake_RequestImpl(Camera* camera, u32 type) {
    s16 index = Quake_GetFreeIndex();
    QuakeRequest* req = &sQuakeRequests[index];

    __osMemset(req, 0, sizeof(QuakeRequest));

    req->camera = camera;
    req->camId = camera->camId;
    req->type = type;
    req->isRelativeToScreen = true;

    // Add a unique random identifier to the upper bits of the index
    // The `~3` assumes there are only 4 requests
    req->index = index + (TRUNCF_BINANG(Rand_ZeroOne() * 0x10000) & ~3);

    sQuakeRequestCount++;

    return req;
}

void Quake_Remove(QuakeRequest* req) {
    req->type = QUAKE_TYPE_NONE;
    req->timer = -1;
    sQuakeRequestCount--;
}

QuakeRequest* Quake_GetRequest(s16 index) {
    QuakeRequest* req = &sQuakeRequests[index & 3];

    if (req->type == QUAKE_TYPE_NONE) {
        return NULL;
    }

    if (index != req->index) {
        return NULL;
    }

    return req;
}

// valueType for Quake_SetValue()
#define QUAKE_SPEED (1 << 0)
#define QUAKE_Y_OFFSET (1 << 1)
#define QUAKE_X_OFFSET (1 << 2)
#define QUAKE_FOV (1 << 3)
#define QUAKE_ROLL (1 << 4)
#define QUAKE_ORIENTATION_PITCH (1 << 5)
#define QUAKE_ORIENTATION_YAW (1 << 6)
#define QUAKE_ORIENTATION_ROLL (1 << 7)
#define QUAKE_DURATION (1 << 8)
#define QUAKE_IS_RELATIVE_TO_SCREEN (1 << 9)

u32 Quake_SetValue(s16 index, s16 valueType, s16 value) {
    QuakeRequest* req = Quake_GetRequest(index);

    if (req == NULL) {
        return false;
    }

    switch (valueType) {
        case QUAKE_SPEED:
            req->speed = value;
            break;

        case QUAKE_Y_OFFSET:
            req->y = value;
            break;

        case QUAKE_X_OFFSET:
            req->x = value;
            break;

        case QUAKE_FOV:
            req->fov = value;
            break;

        case QUAKE_ROLL:
            req->upRollOffset = value;
            break;

        case QUAKE_ORIENTATION_PITCH:
            req->orientation.x = value;
            break;

        case QUAKE_ORIENTATION_YAW:
            req->orientation.y = value;
            break;

        case QUAKE_ORIENTATION_ROLL:
            req->orientation.z = value;
            break;

        case QUAKE_DURATION:
            req->timer = value;
            req->duration = req->timer;
            break;

        case QUAKE_IS_RELATIVE_TO_SCREEN:
            req->isRelativeToScreen = value;
            break;

        default:
            break;
    }

    return true;
}

/**
 * @param index
 * @param speed for periodic types only, the angular frequency of the sine wave (binang / frame)
 * @return true if successfully applied, false if the request does not exist
 */
u32 Quake_SetSpeed(s16 index, s16 speed) {
    QuakeRequest* req = Quake_GetRequest(index);

    if (req != NULL) {
        req->speed = speed;
        return true;
    }
    return false;
}

/**
 * @param index quake request index to apply
 * @param duration number of frames to apply the quake
 * @return true if successfully applied, false if the request does not exist
 */
u32 Quake_SetDuration(s16 index, s16 duration) {
    QuakeRequest* req = Quake_GetRequest(index);

    if (req != NULL) {
        req->duration = req->timer = duration;
        return true;
    }
    return false;
}

/**
 * @param index quake request index to get
 * @return number of frames until the quake is finished
 */
s16 Quake_GetTimeLeft(s16 index) {
    QuakeRequest* req = Quake_GetRequest(index);

    if (req != NULL) {
        return req->timer;
    }
    return 0;
}

/**
 * @param index quake request index to apply
 * @param y apply up/down shake
 * @param x apply left/right shake
 * @param fov apply zooming in/out shake (binang)
 * @param roll apply rolling shake (binang)
 * @return true if successfully applied, false if the request does not exist
 */
u32 Quake_SetPerturbations(s16 index, s16 y, s16 x, s16 fov, s16 roll) {
    QuakeRequest* req = Quake_GetRequest(index);

    if (req != NULL) {
        req->y = y;
        req->x = x;
        req->fov = fov;
        req->upRollOffset = roll;
        return true;
    }
    return false;
}

/**
 * @param index quake request index to apply
 * @param isRelativeToScreen is the quake applied relative to the screen or in absolute world coordinates
 * @param orientation orient the x/y shake to a different direction
 * @return true if successfully applied, false if the request does not exist
 */
u32 Quake_SetOrientation(s16 index, s16 isRelativeToScreen, Vec3s orientation) {
    QuakeRequest* req = Quake_GetRequest(index);

    if (req != NULL) {
        req->isRelativeToScreen = isRelativeToScreen;
        req->orientation = orientation;
        return true;
    }
    return false;
}

void Quake_Init(void) {
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sQuakeRequests); i++) {
        sQuakeRequests[i].type = QUAKE_TYPE_NONE;
        sQuakeRequests[i].timer = 0;
    }
    sQuakeUnused = 1;
    sQuakeRequestCount = 0;
}

s16 Quake_Request(Camera* camera, u32 type) {
    return Quake_RequestImpl(camera, type)->index;
}

u32 Quake_RemoveRequest(s16 index) {
    QuakeRequest* req = Quake_GetRequest(index);

    if (req != NULL) {
        Quake_Remove(req);
        return true;
    }
    return false;
}

static QuakeCallbackFunc sQuakeCallbacks[] = {
    NULL,                // QUAKE_TYPE_NONE
    Quake_CallbackType1, // QUAKE_TYPE_1
    Quake_CallbackType2, // QUAKE_TYPE_2
    Quake_CallbackType3, // QUAKE_TYPE_3
    Quake_CallbackType4, // QUAKE_TYPE_4
    Quake_CallbackType5, // QUAKE_TYPE_5
    Quake_CallbackType6, // QUAKE_TYPE_6
};

s16 Quake_Update(Camera* camera, ShakeInfo* camShake) {
    f32 absSpeedDiv;
    ShakeInfo shake;
    QuakeRequest* req;
    f32 maxCurr;
    f32 maxNext;
    s32 index;
    s32 numQuakesApplied;
    u32 isDifferentCamId;
    Vec3f zeroVec;
    PlayState* play = camera->play;

    zeroVec.x = 0.0f;
    zeroVec.y = 0.0f;
    zeroVec.z = 0.0f;

    camShake->atOffset.x = 0.0f;
    camShake->atOffset.y = 0.0f;
    camShake->atOffset.z = 0.0f;

    camShake->eyeOffset.x = 0.0f;
    camShake->eyeOffset.y = 0.0f;
    camShake->eyeOffset.z = 0.0f;

    camShake->upRollOffset = 0;
    camShake->fovOffset = 0;
    camShake->maxOffset = 0.0f;

    if (sQuakeRequestCount == 0) {
        return 0;
    }

    numQuakesApplied = 0;
    for (index = 0; index < ARRAY_COUNT(sQuakeRequests); index++) {
        req = &sQuakeRequests[index];
        if (req->type == QUAKE_TYPE_NONE) {
            continue;
        }

        if (play->cameraPtrs[req->camId] == NULL) {
            Quake_Remove(req);
            continue;
        }

        isDifferentCamId = (camera->camId != req->camera->camId);
        absSpeedDiv = (f32)ABS(req->speed) / 0x8000;
        if (sQuakeCallbacks[req->type](req, &shake) == 0) {
            // Quake has reached the end of its timer.
            Quake_Remove(req);
            continue;
        }

        if (isDifferentCamId) {
            // Quake is attached to a different camId
            continue;
        }

        if (fabsf(camShake->atOffset.x) < fabsf(shake.atOffset.x)) {
            camShake->atOffset.x = shake.atOffset.x;
        }
        if (fabsf(camShake->atOffset.y) < fabsf(shake.atOffset.y)) {
            camShake->atOffset.y = shake.atOffset.y;
        }
        if (fabsf(camShake->atOffset.z) < fabsf(shake.atOffset.z)) {
            camShake->atOffset.z = shake.atOffset.z;
        }
        if (fabsf(camShake->eyeOffset.x) < fabsf(shake.eyeOffset.x)) {
            camShake->eyeOffset.x = shake.eyeOffset.x;
        }
        if (fabsf(camShake->eyeOffset.y) < fabsf(shake.eyeOffset.y)) {
            camShake->eyeOffset.y = shake.eyeOffset.y;
        }
        if (fabsf(camShake->eyeOffset.z) < fabsf(shake.eyeOffset.z)) {
            camShake->eyeOffset.z = shake.eyeOffset.z;
        }
        if (camShake->upRollOffset < shake.upRollOffset) {
            camShake->upRollOffset = shake.upRollOffset;
        }
        if (camShake->fovOffset < shake.fovOffset) {
            camShake->fovOffset = shake.fovOffset;
        }

        maxCurr = OLib_Vec3fDist(&shake.atOffset, &zeroVec) * absSpeedDiv;
        maxNext = OLib_Vec3fDist(&shake.eyeOffset, &zeroVec) * absSpeedDiv;
        maxCurr = CLAMP_MIN(maxCurr, maxNext);

        maxNext = (camShake->upRollOffset * (7.0f / 2500.0f)) * absSpeedDiv;
        maxCurr = CLAMP_MIN(maxCurr, maxNext);

        maxNext = (camShake->fovOffset * (7.0f / 2500.0f)) * absSpeedDiv;
        maxCurr = CLAMP_MIN(maxCurr, maxNext);

        if (camShake->maxOffset < maxCurr) {
            camShake->maxOffset = maxCurr;
        }

        numQuakesApplied++;
    }
    return numQuakesApplied;
}

void Distortion_Init(PlayState* play) {
    sDistortionRequest.play = play;
    View_ClearDistortion(&play->view);
    sDistortionRequest.type = 0;
    sDistortionRequest.timer = 0;
    sDistortionRequest.state = DISTORTION_INACTIVE;
}

void Distortion_SetDuration(s16 duration) {
    sDistortionRequest.timer = duration;
    sDistortionRequest.state = DISTORTION_SETUP;
}

s16 Distortion_GetTimeLeft(void) {
    return sDistortionRequest.timer;
}

s16 Distortion_GetType(void) {
    return sDistortionRequest.type;
}

void Distortion_Request(s32 type) {
    if (sDistortionRequest.type < type) {
        sDistortionRequest.type = type;
    }
}

void Distortion_RemoveRequest(s32 type) {
    if (sDistortionRequest.type == type) {
        sDistortionRequest.type = 0;
    }
}

/**
 * Checks that the bg surface is an underwater conveyor type and if so, returns the conveyor speed
 */
ConveyorSpeed Distortion_GetUnderwaterCurrentSpeed(Player* player) {
    if (!SurfaceType_IsFloorConveyor(&sDistortionRequest.play->colCtx, player->actor.floorPoly,
                                     player->actor.floorBgId)) {
        return SurfaceType_GetConveyorSpeed(&sDistortionRequest.play->colCtx, player->actor.floorPoly,
                                            player->actor.floorBgId);
    }
    return CONVEYOR_SPEED_DISABLED;
}

void Distortion_Update(void) {
    static s16 depthPhase = 0x3F0;
    static s16 screenPlanePhase = 0x156;
    static s16 duration = 1;
    f32 xyScaleFactor;
    f32 zScaleFactor;
    f32 speedScaleFactor;
    f32 countdownRatio;
    f32 waterYScaleFactor;
    f32 depthPhaseStep;
    f32 screenPlanePhaseStep;
    f32 rotX;
    f32 rotY;
    f32 rotZ;
    f32 xScale;
    f32 yScale;
    f32 zScale;
    f32 speed;
    Player* player;
    PlayState* play = sDistortionRequest.play;
    PosRot playerPosRot;
    Camera* camera = GET_ACTIVE_CAM(play);
    f32 speedRatio = CLAMP_MAX(camera->speedRatio, 1.0f);

    if (sDistortionRequest.type != 0) {
        if (sDistortionRequest.type & DISTORTION_TYPE_MASK_TRANSFORM_2) {
            sDistortionRequest.timer = 2;
            depthPhase = 0x3F0;
            screenPlanePhase = 0x156;

            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 170.0f;

            rotX = 0.0f;
            rotY = 0.0f;
            rotZ = 0.0f;

            xScale = -0.01f;
            yScale = 0.01f;
            zScale = 0.0f;

            speed = 0.6f;
            xyScaleFactor = zScaleFactor = sDistortionRequest.timer / 60.0f;
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_BOSS_WARP) {
            if (sDistortionRequest.state == DISTORTION_SETUP) {
                duration = sDistortionRequest.timer;
                depthPhase = 0x3F0;
                screenPlanePhase = 0x156;
            }

            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 50.0f / duration;

            rotX = 0.0f;
            rotY = 0.0f;
            rotZ = 0.0f;

            xScale = 0.01f;
            yScale = 100.0f;
            zScale = 0.0f;

            speed = 0.4f;
            xyScaleFactor = zScaleFactor = ((f32)duration - sDistortionRequest.timer) / (f32)duration;
            speedScaleFactor = 0.5f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_MASK_TRANSFORM_1) {
            if (sDistortionRequest.state == DISTORTION_SETUP) {
                duration = sDistortionRequest.timer;
                depthPhase = 0x1FC;
                screenPlanePhase = 0x156;
            }

            depthPhaseStep = -5.0f;
            screenPlanePhaseStep = 5.0f;

            rotX = 0.0f;
            rotY = 0.0f;
            rotZ = 2.0f;

            xScale = 0.3f;
            yScale = 0.3f;
            zScale = 0.0f;

            speed = 0.1f;
            xyScaleFactor = zScaleFactor = ((f32)duration - sDistortionRequest.timer) / (f32)duration;
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_GORON_BUTT) {
            if (sDistortionRequest.state == DISTORTION_SETUP) {
                duration = sDistortionRequest.timer;
                depthPhase = 0x2710;
                screenPlanePhase = 0x3E8;
            }

            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 150.0f;

            rotX = 0;
            rotY = 0;
            rotZ = 0.2f;

            xScale = 0.025f;
            yScale = 0.02f;
            zScale = 0.01f;

            speed = 1.5f;
            if (sDistortionRequest.timer < 5) {
                xyScaleFactor = zScaleFactor = ((f32)duration - sDistortionRequest.timer) / (f32)duration;
            } else {
                xyScaleFactor = zScaleFactor = 0.0f;
            }
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_UNK_ATTACK) {
            if (sDistortionRequest.state == DISTORTION_SETUP) {
                duration = sDistortionRequest.timer;
                depthPhase = 0x4B0;
                screenPlanePhase = 0x7D0;
            }

            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 150.0f;

            rotX = 0;
            rotY = 0;
            rotZ = 0;

            xScale = 0.03f;
            yScale = 0.02f;
            zScale = 0.01f;

            speed = 1.5f;
            xyScaleFactor = zScaleFactor = sDistortionRequest.timer / (f32)duration;
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_ZORA_KICK) {
            if (sDistortionRequest.state == DISTORTION_SETUP) {
                duration = sDistortionRequest.timer;
                depthPhase = 0x9C4;
                screenPlanePhase = 0xBB8;
            }

            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 150.0f;

            rotX = 0;
            rotY = 0;
            rotZ = 0;

            xScale = 0.03f;
            yScale = 0.03f;
            zScale = 0.01f;

            speed = 1.3f;
            if (sDistortionRequest.timer < 4) {
                xyScaleFactor = zScaleFactor = sDistortionRequest.timer / (f32)duration;
            } else {
                xyScaleFactor = zScaleFactor = 0.0f;
            }
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_SONG_OF_TIME) {
            sDistortionRequest.timer = 2;
            if (sDistortionRequest.state == DISTORTION_SETUP) {
                depthPhase = 0x9C4;
                screenPlanePhase = 0xBB8;
            }

            depthPhase += 0xB1;
            screenPlanePhase -= 0x2B;

            depthPhaseStep = -107.0f;
            screenPlanePhaseStep = 158.0f;

            rotX = 0.2f;
            rotY = 1.7f;
            rotZ = -2.9f;

            xScale = -0.6f;
            yScale = -0.7f;
            zScale = 0.6f;

            speed = 0.2f;
            zScaleFactor = 1.0f;
            xyScaleFactor = 1.0f;
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_UNDERWATER_ENTRY) {
            if (sDistortionRequest.state == DISTORTION_SETUP) {
                duration = sDistortionRequest.timer;
                depthPhase = 0x760;
                screenPlanePhase = 0x1BC;
            }

            depthPhaseStep = 248.0f;
            screenPlanePhaseStep = -90.0f;

            rotX = 0.0f;
            rotY = 0.0f;
            rotZ = 0.0f;

            xScale = -0.4f;
            yScale = 0.4f;
            zScale = 0.2f;

            speed = 0.25f;
            countdownRatio = sDistortionRequest.timer / (f32)duration;
            zScaleFactor = xyScaleFactor = countdownRatio;
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_ZORA_SWIMMING) {
            depthPhase = 0x3F0;
            screenPlanePhase = 0x156;

            sDistortionRequest.timer = 2;
            player = GET_PLAYER(play);

            if (player != NULL) {
                playerPosRot = Actor_GetWorldPosShapeRot(&player->actor);
            }

            depthPhaseStep = 359.2f;
            screenPlanePhaseStep = -18.5f;

            rotX = 0.0f;
            rotY = 0.0f;
            rotZ = 0.3f;

            switch (Distortion_GetUnderwaterCurrentSpeed(player)) {
                case CONVEYOR_SPEED_FAST:
                    xScale = -0.06f;
                    yScale = 0.1f;
                    zScale = 0.03f;
                    speed = 0.33f;
                    break;

                case CONVEYOR_SPEED_MEDIUM:
                    xScale = -0.06f;
                    yScale = 0.1f;
                    zScale = 0.03f;
                    speed = 0.33f;
                    break;

                case CONVEYOR_SPEED_SLOW:
                    xScale = -0.06f;
                    yScale = 0.1f;
                    zScale = 0.03f;
                    speed = 0.33f;
                    break;

                default:
                    xScale = -0.06f;
                    yScale = 0.1f;
                    zScale = 0.03f;
                    speed = 0.23f;
                    break;
            }

            if (player->unk_B86[1] < 0) {
                xyScaleFactor = (player->unk_B86[1] - (f32)0x4000) / (f32)0xC000;
            } else {
                xyScaleFactor = (player->unk_B86[1] + (f32)0x4000) / (f32)0xC000;
            }
            zScaleFactor = -xyScaleFactor;
            speedScaleFactor = 1.0f;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_NON_ZORA_SWIMMING) {
            depthPhase = 0x3F0;
            screenPlanePhase = 0x156;

            sDistortionRequest.timer = 2;
            player = GET_PLAYER(play);

            depthPhaseStep = 359.2f;
            screenPlanePhaseStep = -18.5f;

            rotX = 0.0f;
            rotY = 0.0f;
            rotZ = 0.0f;
            switch (Distortion_GetUnderwaterCurrentSpeed(player)) {
                case CONVEYOR_SPEED_FAST:
                    xScale = 0.12f;
                    yScale = 0.12f;
                    zScale = 0.08f;
                    speed = 0.18f;
                    break;

                case CONVEYOR_SPEED_MEDIUM:
                    xScale = 0.12f;
                    yScale = 0.12f;
                    zScale = 0.08f;
                    speed = 0.12f;
                    break;

                case CONVEYOR_SPEED_SLOW:
                    xScale = 0.12f;
                    yScale = 0.12f;
                    zScale = 0.08f;
                    speed = 0.08f;
                    break;

                default:
                    xScale = 0.12f;
                    yScale = 0.12f;
                    zScale = 0.08f;
                    speed = 0.05f;
                    break;
            }

            if ((camera->waterYPos - camera->eye.y) > 2000.0f) {
                waterYScaleFactor = 1.0f;
            } else {
                waterYScaleFactor = (camera->waterYPos - camera->eye.y) / 2000.0f;
            }

            xyScaleFactor = speedScaleFactor = (waterYScaleFactor * 0.15f) + 0.35f + (speedRatio * 0.4f);
            zScaleFactor = 0.9f - xyScaleFactor;
        } else if (sDistortionRequest.type & DISTORTION_TYPE_HOT_ROOM) {
            // Gives a small mirage-like appearance
            depthPhase = 0x3F0;
            screenPlanePhase = 0x156;

            sDistortionRequest.timer = 2;
            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 150.0f;

            rotX = 0.0f;
            rotY = 0.0f;
            rotZ = 0.0f;

            xScale = -0.01f;
            yScale = 0.01f;
            zScale = 0.01f;

            speed = 0.6f;
            speedScaleFactor = 1.0f;
            zScaleFactor = 1.0f;
            xyScaleFactor = 1.0f;
        } else {
            return;
        }

        depthPhase += CAM_DEG_TO_BINANG(depthPhaseStep);
        screenPlanePhase += CAM_DEG_TO_BINANG(screenPlanePhaseStep);

        if (GameInteractor_Should(VB_APPLY_SCREEN_DISTORTION, true, sDistortionRequest.type)) {
            View_SetDistortionOrientation(&sDistortionRequest.play->view,
                                          Math_CosS(depthPhase) * (DEG_TO_RAD(rotX) * xyScaleFactor),
                                          Math_SinS(depthPhase) * (DEG_TO_RAD(rotY) * xyScaleFactor),
                                          Math_SinS(screenPlanePhase) * (DEG_TO_RAD(rotZ) * zScaleFactor));
            View_SetDistortionScale(&sDistortionRequest.play->view,
                                    (Math_SinS(screenPlanePhase) * (xScale * xyScaleFactor)) + 1.0f,
                                    (Math_CosS(screenPlanePhase) * (yScale * xyScaleFactor)) + 1.0f,
                                    (Math_CosS(depthPhase) * (zScale * zScaleFactor)) + 1.0f);
            View_SetDistortionSpeed(&sDistortionRequest.play->view, speed * speedScaleFactor);
        }

        sDistortionRequest.state = DISTORTION_ACTIVE;

    } else if (sDistortionRequest.state != DISTORTION_INACTIVE) {
        View_ClearDistortion(&play->view);

        sDistortionRequest.state = DISTORTION_INACTIVE;
        sDistortionRequest.timer = 0;
    }

    if (sDistortionRequest.timer != 0) {
        sDistortionRequest.timer--;
        if (sDistortionRequest.timer == 0) {
            sDistortionRequest.type = 0;
        }
    }
}

s32 Quake_GetNumActiveQuakes(void) {
    s32 numActiveQuakes = 0;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sQuakeRequests); i++) {
        if (sQuakeRequests[i].type != QUAKE_TYPE_NONE) {
            numActiveQuakes++;
        }
    }

    return numActiveQuakes;
}

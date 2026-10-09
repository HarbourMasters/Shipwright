#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "CameraUtils.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"
extern PlayState* gPlayState;
extern PlayState* sCamPlayState;
extern f32 Camera_ScaledStepToCeilF(f32 target, f32 cur, f32 stepScale, f32 minDiff);
extern s16 Camera_ScaledStepToCeilS(s16 target, s16 cur, f32 stepScale, s16 minDiff);
extern s32 Camera_BgCheckInfo(Camera* camera, Vec3f* from, CameraCollision* to);
extern void Camera_ResetActionFuncState(Camera* camera, s32 mode);
extern void func_800CBFA4(Camera* camera, Vec3f* arg1, Vec3f* arg2, s32 arg3);
extern CameraSetting sCameraSettings[];
extern s32 sCameraInterfaceFlags;
}

// Check if bombchu remote control is active
extern bool IsBombchuFocused();

// Static Data Used For Free Camera
static bool sCanFreeLook = false;

void UpdateFreeLookState(Camera* camera) {
    switch (camera->mode) {
        case CAM_MODE_BOWARROWZ:
        case CAM_MODE_FIRSTPERSON:
        case CAM_MODE_FOLLOWBOOMERANG:
        case CAM_MODE_ZORAFIN:
        case CAM_MODE_FOLLOWTARGET:
        case CAM_MODE_TARGET:
        case CAM_MODE_TALK:
        case CAM_MODE_SLINGSHOT:
        case CAM_MODE_BOWARROW:
        case CAM_MODE_BATTLE:
        case CAM_MODE_DEKUHIDE:
        case CAM_MODE_CLIMBZ:
        case CAM_MODE_HOOKSHOT:
        case CAM_MODE_HANGZ:
        case CAM_MODE_DEKUFLYZ:
        case CAM_MODE_BOOMERANG:
        case CAM_MODE_CHARGEZ:
        case CAM_MODE_ZORAFINZ:
            sCanFreeLook = false;
    }
}

// Function based on several camera functions, including Camera_Parallel1
bool Camera_FreeLook(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Player* player = GET_PLAYER(gPlayState);
    VecGeo eyeAdjustment = OLib_Vec3fDiffToVecGeo(at, eye);
    Parallel1ReadOnlyData* roData = &camera->paramData.para1.roData;
    f32 playerHeight;
    f32 pitch = eyeAdjustment.pitch;
    f32 yaw = eyeAdjustment.yaw;

    // Smooth step camera 'at' towards player
    f32 playerYOffset = Player_GetHeight(player) / 1.2f;
    if (player->rideActor != NULL) {
        playerYOffset /= 2;
    }
    at->x = Camera_ScaledStepToCeilF(player->actor.world.pos.x, at->x, 0.5f, 1.0f);
    at->y = Camera_ScaledStepToCeilF(player->actor.world.pos.y + playerYOffset, at->y, 0.5f, 1.0f);
    at->z = Camera_ScaledStepToCeilF(player->actor.world.pos.z, at->z, 0.5f, 1.0f);

    // Equivalent to 'Camera_GetFocalActorHeight'
    playerHeight = Player_GetHeight(player);

    // Camera param reloading based on Camera_Parallel1, only updating roData as only that seems necessary
    if (RELOAD_PARAMS(camera)) {
        CameraModeValue* values = sCameraSettings[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (0.8f - ((68.0f / playerHeight) * -0.2f));

        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_04 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_08 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_20 = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_22 = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceFlags = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_24 = GET_NEXT_RO_DATA(values);
    }

    // 1 acts as a default value of sorts for sCameraInterfaceFlags
    sCameraInterfaceFlags = 1;

    Camera_ResetActionFuncState(camera, camera->mode);

    f32 yawDiff = -sCamPlayState->state.input[0].cur.right_stick_x * 10.0f *
                  (CVarGetFloat("gEnhancements.Camera.RightStick.CameraSensitivity.X", 1.0f));
    f32 pitchDiff = sCamPlayState->state.input[0].cur.right_stick_y * 10.0f *
                    (CVarGetFloat("gEnhancements.Camera.RightStick.CameraSensitivity.Y", 1.0f));

    yaw += yawDiff * GameInteractor_InvertControl(GI_INVERT_CAMERA_RIGHT_STICK_X);
    pitch += pitchDiff * -GameInteractor_InvertControl(GI_INVERT_CAMERA_RIGHT_STICK_Y);

    s16 maxPitch = DEG_TO_BINANG(CVarGetFloat("gEnhancements.Camera.FreeLook.MaxPitch", 72.0f));
    s16 minPitch = DEG_TO_BINANG(CVarGetFloat("gEnhancements.Camera.FreeLook.MinPitch", -49.0f));

    if (pitch > maxPitch) {
        pitch = maxPitch;
    }
    if (pitch < minPitch) {
        pitch = minPitch;
    }

    f32 distTarget = CVarGetInteger("gEnhancements.Camera.FreeLook.MaxCameraDistance", roData->unk_04);
    f32 transitionSpeed = CVarGetInteger("gEnhancements.Camera.FreeLook.TransitionSpeed", 25);
    // Smooth step camera away to max camera distance. Camera collision is calculated later
    camera->dist = Camera_ScaledStepToCeilF(distTarget, camera->dist,
                                            transitionSpeed / (ABS(distTarget - camera->dist) + transitionSpeed), 0.0f);

    // Setup new camera angle based on the calculations from stick inputs
    eyeAdjustment.r = camera->dist;
    eyeAdjustment.yaw = yaw;
    eyeAdjustment.pitch = pitch;

    *eyeNext = OLib_AddVecGeoToVec3f(at, &eyeAdjustment);
    // Apply new camera angle only when camera active
    if (camera->status == CAM_STATUS_ACTIVE) {
        CameraCollision camCollision = {};
        camCollision.pos = *eyeNext;

        if (Camera_BgCheckInfo(camera, at, &camCollision)) {
            f32 collDist = Math3D_Vec3f_DistXYZ(at, &camCollision.pos);
            eyeAdjustment.r = collDist - 3.0f;
            *eye = OLib_AddVecGeoToVec3f(at, &eyeAdjustment);
        } else {
            *eye = *eyeNext;
        }

        camera->dist = Math3D_Vec3f_DistXYZ(at, eye);
        camera->eyeNext = *eye;
    }

    // 65.0f based on value from SoH
    f32 fovTarget = 65.0f;
    camera->fov = Camera_ScaledStepToCeilF(fovTarget, camera->fov, camera->fovUpdateRate, 0.1f);
    camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);

    return true;
}

bool Camera_CanFreeLook(Camera* camera) {
    f32 camX = sCamPlayState->state.input[0].cur.right_stick_x * 10.0f;
    f32 camY = sCamPlayState->state.input[0].cur.right_stick_y * 10.0f;
    if (!sCanFreeLook && (fabsf(camX) >= 15.0f || fabsf(camY) >= 15.0f)) {
        sCanFreeLook = true;
    }
    // Pressing Z will "Reset" Camera
    if (CHECK_BTN_ALL(sCamPlayState->state.input[0].press.button, BTN_Z)) {
        sCanFreeLook = false;
    }
    // Reset camera during cutscenes
    if (gPlayState != nullptr && Player_InCsMode(gPlayState)) {
        // Unless kicking a chest open
        Actor* interactRangeActor = GET_PLAYER(gPlayState)->interactRangeActor;
        if (interactRangeActor != NULL && interactRangeActor->id == ACTOR_EN_BOX &&
            ((EnBox*)interactRangeActor)->unk_1EC == -1) {
            sCanFreeLook = true;
        } else {
            sCanFreeLook = false;
        }
    }
    // Disable freecam during bombchu control
    if (IsBombchuFocused()) {
        sCanFreeLook = false;
    }

    return sCanFreeLook;
}

void RegisterCameraFreeLook() {
    COND_VB_SHOULD(VB_USE_CUSTOM_CAMERA, CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0), {
        Camera* camera = va_arg(args, Camera*);
        switch (sCameraSettings[camera->setting].cameraModes[camera->mode].funcId) {
            case CAM_FUNC_NORMAL0:
            case CAM_FUNC_NORMAL1:
            case CAM_FUNC_NORMAL3:
            case CAM_FUNC_NORMAL4:
            case CAM_FUNC_JUMP2:
            case CAM_FUNC_JUMP3:
            case CAM_FUNC_BATTLE1:
            case CAM_FUNC_UNIQUE2:
            case CAM_FUNC_UNIQUE3:
                if (Camera_CanFreeLook(camera)) {
                    Camera_FreeLook(camera);
                    *should = false;
                }
                break;
            case CAM_FUNC_PARALLEL1:
                switch (camera->mode) {
                    case CAM_MODE_HANG:
                    case CAM_MODE_FREEFALL:
                    case CAM_MODE_ZORAFINZ:
                        if (Camera_CanFreeLook(camera)) {
                            Camera_FreeLook(camera);
                            *should = false;
                        }
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    });

    COND_HOOK(OnCameraChangeModeFlags, CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0),
              [](Camera* camera) { UpdateFreeLookState(camera); });
}

static RegisterShipInitFunc initFunc(RegisterCameraFreeLook, { "gEnhancements.Camera.FreeLook.Enable" });

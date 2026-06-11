#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"

void EnBomChu_Move(EnBomChu*, PlayState*);
void EnBomChu_Explode(EnBomChu*, PlayState*);
s32 Camera_BGCheck(Camera* camera, Vec3f* from, Vec3f* to);
}

#define CVAR_REMOTE_BOMBCHU_NAME CVAR_ENHANCEMENT("RemoteBombchu")
#define CVAR_REMOTE_BOMBCHU_DEFAULT 0
#define CVAR_REMOTE_BOMBCHU_VALUE CVarGetInteger(CVAR_REMOTE_BOMBCHU_NAME, CVAR_REMOTE_BOMBCHU_DEFAULT)

// Camera constants
#define CAM_DIST 200.0f
#define CAM_HEIGHT 50.0f
#define CAM_OFFSET_UP 15.0f

// Control constants
#define TURN_RATE 1200.0f
#define STICK_THRESHOLD 10.0f

typedef struct {
    EnBomChu* activeChu;
    s16 subCamId;
    bool isActive;
    Vec3f cameraEye;
    Vec3f cameraAt;
} RemoteBombchuState;

static RemoteBombchuState sState = { nullptr, SUBCAM_NONE, false, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

static void RotateVectorAroundAxis(Vec3f* vec, Vec3f* axis, f32 angle) {
    Matrix_RotateAxis(angle, axis, MTXMODE_NEW);
    Vec3f result;
    Matrix_MultVec3f(vec, &result);
    *vec = result;
}

static void StartControl(PlayState* play) {
    if (sState.isActive)
        return;

    sState.subCamId = Play_CreateSubCamera(play);
    if (sState.subCamId == SUBCAM_NONE)
        return;

    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, sState.subCamId, CAM_STAT_ACTIVE);

    // Initialize camera vectors from main camera for smooth transition
    Camera* mainCam = Play_GetCamera(play, MAIN_CAM);
    sState.cameraEye = mainCam->eye;
    sState.cameraAt = mainCam->at;

    Player* player = GET_PLAYER(play);
    player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;

    // Reset first-person/item camera state
    player->unk_6AD = 0;

    sState.isActive = true;
}

static void StopControl(PlayState* play) {
    if (!sState.isActive)
        return;

    if (sState.subCamId != SUBCAM_NONE) {
        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
        Play_ClearCamera(play, sState.subCamId);
        sState.subCamId = SUBCAM_NONE;
    }

    Player* player = GET_PLAYER(play);
    player->stateFlags1 &= ~PLAYER_STATE1_INPUT_DISABLED;

    sState.activeChu = nullptr;
    sState.isActive = false;
}

// UpdateCamera function
static void UpdateCamera(EnBomChu* chu, PlayState* play) {
    Vec3f targetEye, targetAt;

    // Calculate target "At" point (looking at the bombchu, slightly above)
    targetAt.x = chu->actor.world.pos.x + chu->axisUp.x * CAM_OFFSET_UP;
    targetAt.y = chu->actor.world.pos.y + chu->axisUp.y * CAM_OFFSET_UP;
    targetAt.z = chu->actor.world.pos.z + chu->axisUp.z * CAM_OFFSET_UP;

    // Calculate target "Eye" position (behind and above the bombchu)
    targetEye.x = targetAt.x - chu->axisForwards.x * CAM_DIST + chu->axisUp.x * CAM_HEIGHT;
    targetEye.y = targetAt.y - chu->axisForwards.y * CAM_DIST + chu->axisUp.y * CAM_HEIGHT;
    targetEye.z = targetAt.z - chu->axisForwards.z * CAM_DIST + chu->axisUp.z * CAM_HEIGHT;

    // Smoothly approach "At" position
    Math_ApproachF(&sState.cameraAt.x, targetAt.x, 0.2f, 30.0f);
    Math_ApproachF(&sState.cameraAt.y, targetAt.y, 0.2f, 30.0f);
    Math_ApproachF(&sState.cameraAt.z, targetAt.z, 0.2f, 30.0f);

    Camera* camera = Play_GetCamera(play, sState.subCamId);

    // Check if the ideal target eye position collides with walls
    Vec3f safeEye = targetEye;
    Camera_BGCheck(camera, &sState.cameraAt, &safeEye);

    // Smoothly approach the *safe* eye position
    Math_ApproachF(&sState.cameraEye.x, safeEye.x, 0.2f, 50.0f);
    Math_ApproachF(&sState.cameraEye.y, safeEye.y, 0.2f, 50.0f);
    Math_ApproachF(&sState.cameraEye.z, safeEye.z, 0.2f, 50.0f);

    Play_CameraSetAtEye(play, sState.subCamId, &sState.cameraAt, &sState.cameraEye);
}

static void HandleSteering(EnBomChu* chu, Input* input) {
    f32 stickX = input->cur.stick_x;
    if (fabsf(stickX) <= STICK_THRESHOLD)
        return;

    // Calculate turn angle based on stick input
    f32 turnAngle = BINANG_TO_RAD((s16)(TURN_RATE * (stickX / 85.0f)));

    // Rotate forward and left vectors around the up axis
    RotateVectorAroundAxis(&chu->axisForwards, &chu->axisUp, -turnAngle);
    RotateVectorAroundAxis(&chu->axisLeft, &chu->axisUp, -turnAngle);

    // Update actor rotation from the new orientation
    MtxF mf;
    mf.xx = chu->axisLeft.x;
    mf.yx = chu->axisLeft.y;
    mf.zx = chu->axisLeft.z;
    mf.xy = chu->axisUp.x;
    mf.yy = chu->axisUp.y;
    mf.zy = chu->axisUp.z;
    mf.xz = chu->axisForwards.x;
    mf.yz = chu->axisForwards.y;
    mf.zz = chu->axisForwards.z;

    Matrix_MtxFToYXZRotS(&mf, &chu->actor.world.rot, 0);
    chu->actor.world.rot.x = -chu->actor.world.rot.x;
    chu->actor.shape.rot = chu->actor.world.rot;
    chu->actor.shape.rot.x = -chu->actor.shape.rot.x;
}

static void HandleInput(EnBomChu* chu, Input* input, PlayState* play) {
    HandleSteering(chu, input);

    if (input->press.button & BTN_B) {
        EnBomChu_Explode(chu, play);
    }

    if (input->press.button & BTN_A) {
        StopControl(play);
    }
}

// Track the most recently spawned bombchu
static void OnActorInit(void* refActor) {
    sState.activeChu = (EnBomChu*)refActor;
}

// Clean up if active bombchu is destroyed
static void OnActorDestroy(void* refActor) {
    if (refActor == sState.activeChu) {
        if (sState.isActive) {
            StopControl(gPlayState);
        }
        sState.activeChu = nullptr;
    }
}

// Main update logic
static void OnActorUpdate(void* refActor) {
    if (refActor != sState.activeChu)
        return;

    if (sState.activeChu->actionFunc != EnBomChu_Move)
        return;

    // Start control when bombchu begins moving
    if (!sState.isActive) {
        StartControl(gPlayState);
    }

    // Exit early if control was not established or was stopped
    if (!sState.isActive)
        return;

    HandleInput(sState.activeChu, &gPlayState->state.input[0], gPlayState);

    // Check again after input handling (user might have pressed A to stop)
    if (!sState.isActive)
        return;

    UpdateCamera(sState.activeChu, gPlayState);
}

void RegisterRemoteBombchu() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_BOM_CHU, CVAR_REMOTE_BOMBCHU_VALUE, OnActorInit);
    COND_ID_HOOK(OnActorDestroy, ACTOR_EN_BOM_CHU, CVAR_REMOTE_BOMBCHU_VALUE, OnActorDestroy);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_BOM_CHU, CVAR_REMOTE_BOMBCHU_VALUE, OnActorUpdate);
}

static RegisterShipInitFunc initFunc(RegisterRemoteBombchu, { CVAR_REMOTE_BOMBCHU_NAME });

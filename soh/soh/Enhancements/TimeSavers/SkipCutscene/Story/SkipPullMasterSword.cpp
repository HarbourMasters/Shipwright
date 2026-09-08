#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "z64save.h"
#include "macros.h"
#include "functions.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Player_StartMode_Idle(PlayState* play, Player* player);
extern Vec3f* Camera_Vec3fVecSphGeoAdd(Vec3f* dest, Vec3f* a, VecSph* b);
}

// Where cutscene leaves Link after Sheik appears
static const Vec3f sMasterSwordEndPos = { 0.0f, 40.0f, 214.0f };
static const int16_t sMasterSwordEndYaw = 30037;

static bool sSkippedMasterSwordCs = false;
static HOOK_ID sMasterSwordCameraHook = 0;

// Both only live from skip until the spawn right after it.
static void SkipPullMasterSword_Reset() {
    sSkippedMasterSwordCs = false;
    if (sMasterSwordCameraHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneSpawnActors>(sMasterSwordCameraHook);
        sMasterSwordCameraHook = 0;
    }
}

// Play_Init aims camera before we move Link, redo here
static void SkipPullMasterSword_OnSceneSpawnActors() {
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneSpawnActors>(sMasterSwordCameraHook);
    sMasterSwordCameraHook = 0;

    Player* player = GET_PLAYER(gPlayState);
    Camera* camera = Play_GetCamera(gPlayState, CAM_ID_MAIN);

    // Drops the fixed shot spawn asks for, which frames pedestal we just left
    Camera_InitPlayerSettings(camera, player);
    Camera_RequestMode(camera, CAM_MODE_NORMAL);

    // That leaves camera behind Link. Put it in front of him instead,
    // the normal camera swings around behind him on its own.
    VecSph offset;
    offset.r = camera->dist;
    offset.pitch = camera->inputDir.x;
    offset.yaw = sMasterSwordEndYaw;
    Camera_Vec3fVecSphGeoAdd(&camera->eyeNext, &camera->at, &offset);
    camera->eye = camera->eyeNext;
    camera->inputDir.y = BINANG_ROT180(sMasterSwordEndYaw);
    camera->camDir = camera->inputDir;
}

void RegisterSkipPullMasterSword() {
    COND_HOOK(OnLoadGame, true, [](int32_t fileNum) { SkipPullMasterSword_Reset(); });

    COND_VB_SHOULD(VB_PLAY_PULL_MASTER_SWORD_CS,
                   CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO), {
                       if (!Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
                           // Normally, these would be done in the cutscene, but we're skipping it
                           Flags_SetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL);
                           Flags_SetEventChkInf(EVENTCHKINF_ENTERED_MASTER_SWORD_CHAMBER);
                           Flags_SetEventChkInf(EVENTCHKINF_SHEIK_SPAWNED_AT_MASTER_SWORD_PEDESTAL);
                           Flags_SetEventChkInf(EVENTCHKINF_TIME_TRAVELED_TO_ADULT);
                           if (!IS_RANDO) {
                               gSaveContext.dayTime = gSaveContext.skyboxTime = 0x8000;
                           }
                           if (GameInteractor_Should(VB_GIVE_ITEM_LIGHT_MEDALLION, true)) {
                               Item_Give(gPlayState, ITEM_MEDALLION_LIGHT);
                           }
                           sSkippedMasterSwordCs = true;
                       }
                       *should = false;
                   });

    // Skip drops us on time travel spawn, where Link would flourish the sword.
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_STARTMODE_FUNC,
                   !IS_RANDO && CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), 0), {
                       int32_t startMode = va_arg(args, int32_t);

                       // Only touch the spawn this skip lands on, leave any other time travel alone
                       if (sSkippedMasterSwordCs) {
                           sSkippedMasterSwordCs = false;
                           if (startMode == PLAYER_START_MODE_TIME_TRAVEL &&
                               gPlayState->sceneNum == SCENE_TEMPLE_OF_TIME) {
                               Player* player = GET_PLAYER(gPlayState);
                               player->actor.world.pos = sMasterSwordEndPos;
                               player->actor.prevPos = sMasterSwordEndPos;
                               player->actor.home.pos = sMasterSwordEndPos;
                               player->actor.world.rot.y = sMasterSwordEndYaw;
                               player->actor.shape.rot.y = sMasterSwordEndYaw;
                               player->yaw = sMasterSwordEndYaw;

                               // Stand still for a moment, then hand back control
                               Player_StartMode_Idle(gPlayState, player);
                               *should = false;

                               if (sMasterSwordCameraHook == 0) {
                                   sMasterSwordCameraHook =
                                       GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>(
                                           SkipPullMasterSword_OnSceneSpawnActors);
                               }
                           }
                       }
                   });
}

static RegisterShipInitFunc initFunc(RegisterSkipPullMasterSword,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), "IS_RANDO" });

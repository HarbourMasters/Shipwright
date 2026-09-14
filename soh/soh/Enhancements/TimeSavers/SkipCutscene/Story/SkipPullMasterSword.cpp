#include <libultraship/bridge/consolevariablebridge.h>

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

// Set flag before giving medallion. Setting flag queues the medallion as a get-item,
// and giving it takes it back out of the queue, so Link doesn't hold it up.
static void SkipPullMasterSword_GiveLightMedallion() {
    Flags_SetEventChkInf(EVENTCHKINF_TIME_TRAVELED_TO_ADULT);
    if (GameInteractor_Should(VB_GIVE_ITEM_LIGHT_MEDALLION, true)) {
        Item_Give(gPlayState, ITEM_MEDALLION_LIGHT);
    }
}

// Outside rando, the skip leaves EVENTCHKINF_TIME_TRAVELED_TO_ADULT unset.
// Like vanilla, it gets set once adult Link reaches the Temple of Time.
static bool SkipPullMasterSword_IsFirstAdultArrival() {
    return gPlayState->sceneNum == SCENE_TEMPLE_OF_TIME && LINK_IS_ADULT &&
           !Flags_GetEventChkInf(EVENTCHKINF_TIME_TRAVELED_TO_ADULT);
}

// Runs after the scene sets up its camera, which uses the time travel spawn's fixed camera
static void SkipPullMasterSword_OnSceneSpawnActors() {
    if (!SkipPullMasterSword_IsFirstAdultArrival()) {
        return;
    }
    SkipPullMasterSword_GiveLightMedallion();

    Player* player = GET_PLAYER(gPlayState);
    Camera* camera = Play_GetCamera(gPlayState, CAM_ID_MAIN);

    // Coming back as an adult uses a fixed camera aimed at the pedestal.
    // Switch to the normal camera that follows Link.
    Camera_InitPlayerSettings(camera, player);
    Camera_RequestMode(camera, CAM_MODE_NORMAL);

    // The normal camera starts behind Link. Start it in front of him,
    // facing him. It moves back behind him on its own.
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
    COND_VB_SHOULD(VB_PLAY_PULL_MASTER_SWORD_CS,
                   CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO), {
                       if (!Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
                           // Normally, these would be done in the cutscene, but we're skipping it
                           Flags_SetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL);
                           Flags_SetEventChkInf(EVENTCHKINF_ENTERED_MASTER_SWORD_CHAMBER);
                           Flags_SetEventChkInf(EVENTCHKINF_SHEIK_SPAWNED_AT_MASTER_SWORD_PEDESTAL);
                           if (IS_RANDO) {
                               SkipPullMasterSword_GiveLightMedallion();
                           } else {
                               gSaveContext.dayTime = gSaveContext.skyboxTime = 0x8000;
                           }
                       }
                       *should = false;
                   });

    // After the skip, Link appears at pedestal and raises sword. Move to where cutscene ends instead
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_STARTMODE_FUNC,
                   !IS_RANDO && CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), 0), {
                       int32_t startMode = va_arg(args, int32_t);

                       // Only change the first trip to adult, not later trips through time
                       if (startMode == PLAYER_START_MODE_TIME_TRAVEL && SkipPullMasterSword_IsFirstAdultArrival()) {
                           Player* player = GET_PLAYER(gPlayState);
                           player->actor.world.pos = sMasterSwordEndPos;
                           player->actor.prevPos = sMasterSwordEndPos;
                           player->actor.home.pos = sMasterSwordEndPos;
                           player->actor.world.rot.y = sMasterSwordEndYaw;
                           player->actor.shape.rot.y = sMasterSwordEndYaw;
                           player->yaw = sMasterSwordEndYaw;

                           // Just stand there instead of raising the sword
                           Player_StartMode_Idle(gPlayState, player);
                           *should = false;
                       }
                   });

    COND_HOOK(OnSceneSpawnActors, !IS_RANDO && CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), 0),
              SkipPullMasterSword_OnSceneSpawnActors);
}

static RegisterShipInitFunc initFunc(RegisterSkipPullMasterSword,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), "IS_RANDO" });

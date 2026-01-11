#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <set>
#include <unordered_map>

extern "C" {
#include "functions.h"
#include "variables.h"
#include "z64bgcheck.h"
}

#define CVAR_DISABLE_FIXED_CAMERA_NAME CVAR_ENHANCEMENT("DisableFixedCamera")
#define CVAR_DISABLE_FIXED_CAMERA_VALUE CVarGetInteger(CVAR_DISABLE_FIXED_CAMERA_NAME, 0)
static const std::set<SceneID> fixedCameraSceneList = {
    SCENE_MARKET_ENTRANCE_DAY,
    SCENE_MARKET_ENTRANCE_NIGHT,
    SCENE_MARKET_ENTRANCE_RUINS,
    SCENE_BACK_ALLEY_DAY,
    SCENE_BACK_ALLEY_NIGHT,
    SCENE_MARKET_DAY,
    SCENE_MARKET_NIGHT,
    SCENE_MARKET_RUINS,
    SCENE_CASTLE_COURTYARD_ZELDA,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS,
    SCENE_FOREST_TEMPLE,
    SCENE_KNOW_IT_ALL_BROS_HOUSE,
    SCENE_TWINS_HOUSE,
    SCENE_MIDOS_HOUSE,
    SCENE_SARIAS_HOUSE,
    SCENE_BACK_ALLEY_HOUSE,
    SCENE_POTION_SHOP_GRANNY,
    SCENE_SHOOTING_GALLERY,
    SCENE_LINKS_HOUSE,
    SCENE_DOG_LADY_HOUSE,
    SCENE_STABLE,
    SCENE_IMPAS_HOUSE,
    SCENE_KAKARIKO_CENTER_GUEST_HOUSE,
    SCENE_CARPENTERS_TENT,
    SCENE_GRAVEKEEPERS_HUT,
};

static int16_t sSetNormalCam = -1;
static bool sIsCamApplied = false;
static int sCheckItemCamState = -1;
static s16 sStoreLastCamType = -1;
static bool sWasEnabled = false;
static bool sWaitForSceneChange = false;

extern "C" void DisableFixedCamera_CheckCameraState(PlayState* play);

struct CamDataBackup {
    CamData* original = nullptr;
    CamData* copy = nullptr;
    size_t len = 0;
    bool active = false;
};

static std::unordered_map<CollisionHeader*, CamDataBackup> sCamDataBackups;

static void DisableFixedCamera_ResetState() {
    sSetNormalCam = -1;
    sIsCamApplied = false;
    sCheckItemCamState = -1;
    sStoreLastCamType = -1;
    sWasEnabled = false;
    sWaitForSceneChange = false;
}

static void DisableFixedCamera_RestoreAllCameraData() {
    for (auto& [colHeader, backup] : sCamDataBackups) {
        if (colHeader != nullptr && backup.active) {
            colHeader->cameraDataList = backup.original;
            backup.active = false;
        }
        delete[] backup.copy;
        backup.copy = nullptr;
        backup.original = nullptr;
        backup.len = 0;
    }
    sCamDataBackups.clear();
}

// Helper to check if a camera type is a fixed camera
static bool IsFixedCameraType(s16 type) {
    return type == CAM_SET_PREREND_FIXED || type == CAM_SET_PREREND_PIVOT || type == CAM_SET_PIVOT_FROM_SIDE;
}

static void RegisterDisableFixedCamera() {
    const bool disableFixedCamEnabled = CVAR_DISABLE_FIXED_CAMERA_VALUE != 0;

    COND_HOOK(OnCameraState, disableFixedCamEnabled,
              [](PlayState* play) { DisableFixedCamera_CheckCameraState(play); });

    if (!disableFixedCamEnabled) {
        DisableFixedCamera_RestoreAllCameraData();
        DisableFixedCamera_ResetState();
    }
}

static RegisterShipInitFunc initFunc(RegisterDisableFixedCamera, { CVAR_DISABLE_FIXED_CAMERA_NAME });

static void DisableFixedCamera_RestoreCameraData(CollisionHeader* colHeader) {
    if (colHeader == nullptr) {
        return;
    }

    auto it = sCamDataBackups.find(colHeader);
    if (it == sCamDataBackups.end() || !it->second.active) {
        return;
    }

    colHeader->cameraDataList = it->second.original;
    it->second.active = false;
}

extern "C" void DisableFixedCamera_SetNormalCamera(PlayState* play) {
    CollisionHeader* colHeader = BgCheck_GetCollisionHeader(&play->colCtx, BGCHECK_SCENE);
    if (colHeader != nullptr && colHeader->cameraDataList != nullptr && colHeader->cameraDataListLen > 0) {
        CamDataBackup& backup = sCamDataBackups[colHeader];
        if (backup.copy == nullptr) {
            backup.original = colHeader->cameraDataList;
            backup.len = colHeader->cameraDataListLen;
            backup.copy = new CamData[backup.len];
            memcpy(backup.copy, backup.original, sizeof(CamData) * backup.len);
        }
        if (colHeader->cameraDataList != backup.copy) {
            colHeader->cameraDataList = backup.copy;
        }
        backup.active = true;

        for (size_t i = 0; i < colHeader->cameraDataListLen; i++) {
            if (IsFixedCameraType(colHeader->cameraDataList[i].cameraSType)) {
                colHeader->cameraDataList[i].cameraSType = CAM_SET_NORMAL0;
            }
        }
    }
    play->unk_1242B = 0;
    if (IsFixedCameraType(play->mainCamera.setting)) {
        play->mainCamera.setting = CAM_SET_NORMAL0;
        play->mainCamera.prevSetting = CAM_SET_NORMAL0;
    }
    Camera_ChangeSetting(&play->mainCamera, CAM_SET_NORMAL0);
    Camera_ChangeMode(&play->mainCamera, CAM_MODE_NORMAL);
}

extern "C" void DisableFixedCamera_CheckCameraState(PlayState* play) {
    const bool disableFixedCamEnabled = CVarGetInteger(CVAR_DISABLE_FIXED_CAMERA_NAME, 0) != 0;
    if (!disableFixedCamEnabled) {
        DisableFixedCamera_RestoreAllCameraData();
        DisableFixedCamera_ResetState();
        return;
    }
    if (!sWasEnabled) {
        sWasEnabled = true;
        sWaitForSceneChange = true;
        sSetNormalCam = play->sceneNum;
        sIsCamApplied = true;
    }
    if (sWaitForSceneChange) {
        if (play->sceneNum == sSetNormalCam) {
            return;
        }
        sWaitForSceneChange = false;
        sIsCamApplied = false;
        sCheckItemCamState = -1;
        sStoreLastCamType = -1;
    }
    // prevents normal cam from taking effect during open cutscene to avoid crash
    if (play->sceneNum == SCENE_LINKS_HOUSE && gSaveContext.cutsceneIndex == 0xFFF1) {
        return;
    }
    // Only compute player state if we're in a relevant scene
    const bool isInFixedCameraScene = fixedCameraSceneList.contains(static_cast<SceneID>(play->sceneNum));
    if (!isInFixedCameraScene) {
        bool sceneChanged = play->sceneNum != sSetNormalCam;
        if (sceneChanged) {
            sSetNormalCam = play->sceneNum;
            sIsCamApplied = false;
            sStoreLastCamType = -1;
            // Restore camera data when leaving fixed camera scenes
            DisableFixedCamera_RestoreAllCameraData();
        }
        DisableFixedCamera_RestoreCameraData(BgCheck_GetCollisionHeader(&play->colCtx, BGCHECK_SCENE));
        return;
    }

    bool sceneChanged = play->sceneNum != sSetNormalCam;
    bool itemCamChanged = false;

    if (sceneChanged) {
        sSetNormalCam = play->sceneNum;
        sIsCamApplied = false;
        sStoreLastCamType = -1;
    }

    Player* player = (Player*)play->actorCtx.actorLists[ACTORCAT_PLAYER].head;

    bool ocarinaPulling = player && (player->stateFlags2 & PLAYER_STATE2_OCARINA_PLAYING);
    bool bottleUsing = false;
    if (player) {
        bool inItemCs = (player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS) != 0;
        bool isBottleAction =
            (player->itemAction >= PLAYER_IA_BOTTLE) && (player->itemAction <= PLAYER_IA_BOTTLE_FAIRY);
        bottleUsing = inItemCs && isBottleAction;
    }
    bool itemCamActive = ocarinaPulling || bottleUsing;

    if (sCheckItemCamState == -1) {
        sCheckItemCamState = itemCamActive;
    } else if (sCheckItemCamState != static_cast<int>(itemCamActive)) {
        sCheckItemCamState = itemCamActive;
        itemCamChanged = true;
    }

    if (!sceneChanged && !itemCamChanged) {
        return;
    }

    // sets cam when ocarina or bottle is used and sets it back to normal when done
    if (itemCamChanged && itemCamActive) {
        if (play->mainCamera.camDataIdx >= 0) {
            sStoreLastCamType = play->mainCamera.camDataIdx;
        }
        Camera_ChangeSetting(&play->mainCamera, CAM_SET_TURN_AROUND);
        Camera_ChangeMode(&play->mainCamera, CAM_MODE_NORMAL);
        if (sStoreLastCamType >= 0) {
            play->mainCamera.camDataIdx = sStoreLastCamType;
        }
        return;
    }
    if (itemCamChanged && !itemCamActive) {
        DisableFixedCamera_SetNormalCamera(play);
        if (sStoreLastCamType >= 0) {
            play->mainCamera.camDataIdx = sStoreLastCamType;
        }
        return;
    }

    if (!sIsCamApplied) {
        DisableFixedCamera_SetNormalCamera(play);
        sIsCamApplied = true;
    }
}

#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <set>
#include <unordered_map>
#include <cstring>

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

struct CamDataBackup {
    CamData* original = nullptr;
    CamData* copy = nullptr;
    size_t len = 0;
    bool active = false;
};

static std::unordered_map<const CollisionHeader*, CamDataBackup> sCamDataBackups;

static void DisableFixedCamera_StoreCamType(Camera* camera) {
    if (camera->camDataIdx >= 0) {
        sStoreLastCamType = camera->camDataIdx;
    }
}

static void DisableFixedCamera_RestoreCamType(Camera* camera) {
    if (sStoreLastCamType >= 0) {
        camera->camDataIdx = sStoreLastCamType;
    }
}

static void RegisterDisableFixedCamera() {
    COND_VB_SHOULD(VB_FORCE_NORMAL_CAM, true, {
        PlayState* play = va_arg(args, PlayState*);
        if (!fixedCameraSceneList.contains(static_cast<SceneID>(play->sceneNum))) {
            *should = false;
            return;
        }

        *should = CVAR_DISABLE_FIXED_CAMERA_VALUE != 0;
    });
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
            if (colHeader->cameraDataList[i].cameraSType == CAM_SET_PREREND_FIXED ||
                colHeader->cameraDataList[i].cameraSType == CAM_SET_PREREND_PIVOT ||
                colHeader->cameraDataList[i].cameraSType == CAM_SET_PIVOT_FROM_SIDE) {
                colHeader->cameraDataList[i].cameraSType = CAM_SET_NORMAL0;
            }
        }
    }
    play->unk_1242B = 0;
    if (play->mainCamera.setting == CAM_SET_PREREND_FIXED || play->mainCamera.setting == CAM_SET_PREREND_PIVOT ||
        play->mainCamera.setting == CAM_SET_PIVOT_FROM_SIDE) {
        play->mainCamera.setting = CAM_SET_NORMAL0;
        play->mainCamera.prevSetting = CAM_SET_NORMAL0;
    }
    Camera_ChangeSetting(&play->mainCamera, CAM_SET_NORMAL0);
    Camera_ChangeMode(&play->mainCamera, CAM_MODE_NORMAL);
}

extern "C" void DisableFixedCamera_CheckCameraState(PlayState* play) {
    bool sceneChanged = play->sceneNum != sSetNormalCam;
    bool itemCamChanged = false;
    Player* player = (Player*)play->actorCtx.actorLists[ACTORCAT_PLAYER].head;
    // prevents normal cam from taking effect during open cutscene to avoid crash
    if (play->sceneNum == SCENE_LINKS_HOUSE && player != nullptr && (player->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE)) {
        return;
    }
    bool ocarinaPulling = (player != nullptr) && (player->stateFlags2 & PLAYER_STATE2_OCARINA_PLAYING);
    bool bottleUsing = false;
    bool itemCamActive = ocarinaPulling;
    if (player != nullptr) {
        bool inItemCs = (player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS) != 0;
        bool isBottleAction =
            (player->itemAction >= PLAYER_IA_BOTTLE) && (player->itemAction <= PLAYER_IA_BOTTLE_FAIRY);
        bottleUsing = inItemCs && isBottleAction;
        itemCamActive = ocarinaPulling || bottleUsing;
    }
    if (sCheckItemCamState == -1) {
        sCheckItemCamState = itemCamActive;
    } else if (sCheckItemCamState != static_cast<int>(itemCamActive)) {
        sCheckItemCamState = itemCamActive;
        itemCamChanged = true;
    }
    if (sceneChanged) {
        sSetNormalCam = play->sceneNum;
        sIsCamApplied = false;
        sStoreLastCamType = -1;
    }

    if (!sceneChanged && !itemCamChanged) {
        return;
    }

    CollisionHeader* colHeader = BgCheck_GetCollisionHeader(&play->colCtx, BGCHECK_SCENE);
    if (CVAR_DISABLE_FIXED_CAMERA_VALUE == 0 || !fixedCameraSceneList.contains(static_cast<SceneID>(play->sceneNum))) {
        DisableFixedCamera_RestoreCameraData(colHeader);
        return;
    }

    // sets cam when ocarina or bottle is used and sets it back to normal when done
    if (itemCamChanged && itemCamActive) {
        DisableFixedCamera_StoreCamType(&play->mainCamera);
        Camera_ChangeSetting(&play->mainCamera, CAM_SET_TURN_AROUND);
        Camera_ChangeMode(&play->mainCamera, CAM_MODE_NORMAL);
        DisableFixedCamera_RestoreCamType(&play->mainCamera);
        return;
    }
    if (itemCamChanged && !itemCamActive) {
        DisableFixedCamera_SetNormalCamera(play);
        DisableFixedCamera_RestoreCamType(&play->mainCamera);
        return;
    }

    if (!sIsCamApplied && GameInteractor_Should(VB_FORCE_NORMAL_CAM, false, play)) {
        DisableFixedCamera_SetNormalCamera(play);
        sIsCamApplied = true;
    }
}

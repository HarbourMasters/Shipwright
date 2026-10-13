/**
 * @file z_eventmgr.c
 *
 * Manages all cutscenes except for manual
 */

#include "global.h"
#include "z64shrink_window.h"
#include "libc/string.h"
#include "2s2h/GameInteractor/GameInteractor.h"

ActorCutscene sGlobalCutsceneList[] = {
    // CS_ID_GLOBAL_78
    { -100, -1, CS_CAM_ID_NONE, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE_ALT, 255, CS_HUD_VISIBILITY_ALL_ALT, 255,
      255 },
    // CS_ID_GLOBAL_79
    { -100, -1, CS_CAM_ID_NONE, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE_ALT, 255, CS_HUD_VISIBILITY_ALL_ALT, 255,
      255 },
    // CS_ID_GLOBAL_7A
    { -100, -1, CS_CAM_ID_NONE, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE_ALT, 255, CS_HUD_VISIBILITY_ALL_ALT, 255,
      255 },
    // CS_ID_GLOBAL_ELEGY
    { 2, -1, CS_CAM_ID_GLOBAL_ELEGY, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE_ALT, 255, CS_HUD_VISIBILITY_NONE,
      CS_END_CAM_0, 32 },
    // CS_ID_GLOBAL_TALK
    { 32765, -1, CS_CAM_ID_NONE, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE_ALT, 255, CS_HUD_VISIBILITY_ALL_ALT,
      CS_END_CAM_0, 255 },
    // CS_ID_GLOBAL_DOOR
    { 32764, -1, CS_CAM_ID_NONE, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE_ALT, 255, CS_HUD_VISIBILITY_ALL_ALT,
      CS_END_CAM_0, 255 },
    // CS_ID_GLOBAL_RETURN_TO_CAM
    { 32766, -2, CS_CAM_ID_GLOBAL_CONNECT, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE, 255,
      CS_HUD_VISIBILITY_ALL_ALT, CS_END_CAM_0, 32 },
    // CS_ID_GLOBAL_END
    { 0, -1, CS_CAM_ID_NONE, CS_SCRIPT_ID_NONE, CS_ID_NONE, CS_END_SFX_NONE, 255, CS_HUD_VISIBILITY_ALL_ALT,
      CS_END_CAM_0, 32 },
};

typedef enum {
    /* 0 */ CS_START_0,
    /* 1 */ CS_START_1,
    /* 2 */ CS_START_2
} ActorCutsceneStartMethod;

typedef struct {
    /* 0x00 */ s16 csId;
    /* 0x02 */ s16 length;
    /* 0x04 */ s16 endCsId;
    /* 0x06 */ s16 subCamId;
    /* 0x08 */ Actor* targetActor;
    /* 0x0C */ s32 startMethod;
    /* 0x10 */ PlayState* play;
    /* 0x14 */ s16 retCamId;
    /* 0x16 */ s16 isCameraStored;
} CutsceneManager; // size = 0x18

CutsceneManager sCutsceneMgr = {
    CS_ID_NONE, 0, CS_ID_NONE, SUB_CAM_ID_DONE, NULL, CS_START_0, NULL, CAM_ID_MAIN, false,
};

ActorCutscene* sSceneCutsceneList;
s16 sSceneCutsceneCount;
u8 sWaitingCutsceneList[16];
static s32 sBssPad;
u8 sNextCutsceneList[16];
static s32 sBssPad2;

s16 CutsceneManager_SetHudVisibility(s16 csHudVisibility) {
    u16 hudVisibility;

    switch (csHudVisibility) {
        case CS_HUD_VISIBILITY_NONE:
            hudVisibility = HUD_VISIBILITY_NONE_ALT;
            break;

        case CS_HUD_VISIBILITY_ALL:
            hudVisibility = HUD_VISIBILITY_ALL;
            break;

        case CS_HUD_VISIBILITY_A_HEARTS_MAGIC:
            hudVisibility = HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE;
            break;

        case CS_HUD_VISIBILITY_C_HEARTS_MAGIC:
            hudVisibility = HUD_VISIBILITY_HEARTS_MAGIC_C;
            break;

        case CS_HUD_VISIBILITY_ALL_NO_MINIMAP:
            hudVisibility = HUD_VISIBILITY_ALL_NO_MINIMAP;
            break;

        case CS_HUD_VISIBILITY_A_B_C:
            hudVisibility = HUD_VISIBILITY_A_B_C;
            break;

        case CS_HUD_VISIBILITY_B_MINIMAP:
            hudVisibility = HUD_VISIBILITY_B_MINIMAP;
            break;

        case CS_HUD_VISIBILITY_A:
            hudVisibility = HUD_VISIBILITY_A;
            break;

        default:
            hudVisibility = HUD_VISIBILITY_ALL;
            break;
    }

    Interface_SetHudVisibility(hudVisibility);

    return hudVisibility;
}

ActorCutscene* CutsceneManager_GetCutsceneEntryImpl(s16 csId) {
    if (csId < CS_ID_GLOBAL_78) {
        return &sSceneCutsceneList[csId];
    } else {
        csId -= CS_ID_GLOBAL_78;
        return &sGlobalCutsceneList[csId];
    }
}

void CutsceneManager_Init(PlayState* play, ActorCutscene* cutsceneList, s16 numEntries) {
    s32 i;

    sSceneCutsceneList = cutsceneList;
    sSceneCutsceneCount = numEntries;

    for (i = 0; i < ARRAY_COUNT(sWaitingCutsceneList); i++) {
        sWaitingCutsceneList[i] = 0;
        sNextCutsceneList[i] = 0;
    }

    sCutsceneMgr.endCsId = CS_ID_NONE;
    sCutsceneMgr.play = play;
    sCutsceneMgr.length = -1;
    sCutsceneMgr.targetActor = NULL;
    sCutsceneMgr.subCamId = SUB_CAM_ID_DONE;
    sCutsceneMgr.isCameraStored = false;
    sCutsceneMgr.csId = sCutsceneMgr.endCsId;
}

/**
 * Store camera into subCam 2, and keep subCam 2 INACTIVE to preserve the struct.
 */
void CutsceneManager_StoreCamera(Camera* camera) {
    if (camera != NULL) {
        memcpy(&sCutsceneMgr.play->subCameras[2], camera, sizeof(Camera));
        sCutsceneMgr.play->subCameras[2].camId = camera->camId;
        Camera_ChangeStatus(&sCutsceneMgr.play->subCameras[2], CAM_STATUS_INACTIVE);
        sCutsceneMgr.isCameraStored = true;
    }
}

void CutsceneManager_ClearWaiting(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sWaitingCutsceneList); i++) {
        sWaitingCutsceneList[i] = 0;
    }
}

void CutsceneManager_ClearNextCutscenes(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sNextCutsceneList); i++) {
        sNextCutsceneList[i] = 0;
    }
}

s16 CutsceneManager_MarkNextCutscenes(void) {
    s16 bit;
    s32 i;
    s32 j;
    s32 count = 0;
    s16 csIdMax = CS_ID_NONE;
    s16 priorityMax = SHT_MAX; // lower number means higher priority
    s16 csId;
    s16 priority;

    for (i = 0; i < ARRAY_COUNT(sNextCutsceneList); i++) {
        for (bit = 1, j = 0; j < 8; j++) {
            if (sWaitingCutsceneList[i] & bit) {
                csId = (i << 3) | j;
                priority = CutsceneManager_GetCutsceneEntryImpl(csId)->priority;

                if ((priority ^ 0) == -1) {
                    sNextCutsceneList[i] |= bit;
                } else if ((priority < priorityMax) && (priority > 0)) {
                    csIdMax = csId;
                    priorityMax = priority;
                }
                count++;
            }
            bit <<= 1;
        }
    }
    if (csIdMax != CS_ID_NONE) {
        sNextCutsceneList[csIdMax >> 3] |= 1 << (csIdMax & 7);
    }
    return count;
}

#define RET_CAM sCutsceneMgr.play->cameraPtrs[sCutsceneMgr.retCamId]
#define CUR_CAM sCutsceneMgr.play->cameraPtrs[sCutsceneMgr.subCamId]

void CutsceneManager_End(void) {
    ActorCutscene* csEntry;
    s16 oldCamId;
    s16 oldStateFlags;

    switch (sCutsceneMgr.startMethod) {
        case CS_START_2:
            sCutsceneMgr.targetActor->flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
            // fallthrough
        case CS_START_1:
            Player_SetCsActionWithHaltedActors(sCutsceneMgr.play, NULL, PLAYER_CSACTION_END);
            sCutsceneMgr.startMethod = CS_START_0;
            break;

        default:
            break;
    }

    csEntry = CutsceneManager_GetCutsceneEntryImpl(sCutsceneMgr.csId);

    switch (csEntry->endSfx) {
        case CS_END_SFX_TRE_BOX_APPEAR:
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            break;

        case CS_END_SFX_CORRECT_CHIME:
            Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
            break;

        default: // CS_END_SFX_NONE
            break;
    }

    switch (csEntry->endCam) {
        case CS_END_CAM_SMOOTH:
            Play_CopyCamera(sCutsceneMgr.play, sCutsceneMgr.retCamId, sCutsceneMgr.subCamId);
            RET_CAM->stateFlags =
                (RET_CAM->stateFlags & ~CAM_STATE_UNDERWATER) | (CUR_CAM->stateFlags & CAM_STATE_UNDERWATER);
            CutsceneManager_Queue(CS_ID_GLOBAL_RETURN_TO_CAM);
            break;

        case CS_END_CAM_0:
        default:
            Play_CopyCamera(sCutsceneMgr.play, sCutsceneMgr.retCamId, sCutsceneMgr.subCamId);
            RET_CAM->stateFlags =
                (RET_CAM->stateFlags & ~CAM_STATE_UNDERWATER) | (CUR_CAM->stateFlags & CAM_STATE_UNDERWATER);
            break;

        case CS_END_CAM_1:
            oldCamId = RET_CAM->camId;
            oldStateFlags = RET_CAM->stateFlags;

            if (sCutsceneMgr.isCameraStored) {
                // Restore the camera that was stored in subCam 2
                memcpy(RET_CAM, &sCutsceneMgr.play->subCameras[2], sizeof(Camera));

                RET_CAM->stateFlags =
                    (RET_CAM->stateFlags & ~CAM_STATE_UNDERWATER) | (CUR_CAM->stateFlags & CAM_STATE_UNDERWATER);

                RET_CAM->stateFlags = (RET_CAM->stateFlags & ~CAM_STATE_2) | (oldStateFlags & CAM_STATE_2);
                sCutsceneMgr.isCameraStored = false;
            }
            RET_CAM->camId = oldCamId;
            break;
    }

    if (sCutsceneMgr.subCamId != SUB_CAM_ID_DONE) {
        Play_ClearCamera(sCutsceneMgr.play, sCutsceneMgr.subCamId);
        Play_ChangeCameraStatus(sCutsceneMgr.play, sCutsceneMgr.retCamId, CAM_STATUS_ACTIVE);
    }

    sCutsceneMgr.csId = CS_ID_NONE;
    sCutsceneMgr.endCsId = CS_ID_NONE;
    sCutsceneMgr.length = -1;
    sCutsceneMgr.targetActor = NULL;
    sCutsceneMgr.subCamId = SUB_CAM_ID_DONE;
}

s16 CutsceneManager_Update(void) {
    s16 sp1E = 0;

    if (CutsceneManager_IsNext(CS_ID_GLOBAL_RETURN_TO_CAM)) {
        CutsceneManager_StartWithPlayerCs(CS_ID_GLOBAL_RETURN_TO_CAM, &GET_PLAYER(sCutsceneMgr.play)->actor);
    }

    if (sCutsceneMgr.endCsId == CS_ID_NONE) {
        if (sCutsceneMgr.csId != CS_ID_NONE) {
            if (sCutsceneMgr.length > 0) {
                sCutsceneMgr.length--;
            }
            sp1E = 1;
            if (sCutsceneMgr.length == 0) {
                CutsceneManager_Stop(sCutsceneMgr.csId);
            }
        }
    }

    if (sCutsceneMgr.endCsId != CS_ID_NONE) {
        CutsceneManager_End();
        sp1E = 2;
    }

    CutsceneManager_ClearNextCutscenes();

    if (sCutsceneMgr.csId == CS_ID_NONE) {
        if ((CutsceneManager_MarkNextCutscenes() == 0) && (sp1E != 0)) {
            ShrinkWindow_Letterbox_SetSizeTarget(0);
        } else if (sp1E == 0) {
            CutsceneManager_StoreCamera(Play_GetCamera(sCutsceneMgr.play, sCutsceneMgr.retCamId));
        }
    }
    return sp1E;
}

void CutsceneManager_Queue(s16 csId) {
    if (!GameInteractor_Should(VB_QUEUE_CUTSCENE, true, &csId)) {
        return;
    }
    if (csId > CS_ID_NONE) {
        sWaitingCutsceneList[csId >> 3] |= 1 << (csId & 7);
    }
}

s16 CutsceneManager_IsNext(s16 csId) {
    if (csId == CS_ID_GLOBAL_END) {
        if (sCutsceneMgr.csId == CS_ID_NONE) {
            return 0x7F;
        } else {
            return 0;
        }
    }
    if (csId <= CS_ID_NONE) {
        return -1;
    }
    return (sNextCutsceneList[csId >> 3] & (1 << (csId & 7))) ? 1 : 0;
}

/**
 * Start an actor cutscene, activate Player Cutscene Action "Wait"
 */
s16 CutsceneManager_StartWithPlayerCs(s16 csId, Actor* actor) {
    s16 startCsId = CutsceneManager_Start(csId, actor);

    if (startCsId > CS_ID_NONE) {
        Player_SetCsActionWithHaltedActors(sCutsceneMgr.play, NULL, PLAYER_CSACTION_WAIT);
        if (sCutsceneMgr.length == 0) {
            CutsceneManager_Stop(sCutsceneMgr.csId);
        }
        sCutsceneMgr.startMethod = CS_START_1;
    }
    return startCsId;
}

/**
 * Start an actor cutscene, activate Player Cutscene Action "Wait", turn on ACTOR_FLAG_FREEZE_EXCEPTION
 */
s16 CutsceneManager_StartWithPlayerCsAndSetFlag(s16 csId, Actor* actor) {
    s16 startCsId = CutsceneManager_Start(csId, actor);

    if (startCsId > CS_ID_NONE) {
        Player_SetCsActionWithHaltedActors(sCutsceneMgr.play, NULL, PLAYER_CSACTION_WAIT);
        if (sCutsceneMgr.length == 0) {
            CutsceneManager_Stop(sCutsceneMgr.csId);
        }
        if (actor != NULL) {
            actor->flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
            sCutsceneMgr.startMethod = CS_START_2;
        } else {
            sCutsceneMgr.startMethod = CS_START_1;
        }
    }
    return startCsId;
}

s16 CutsceneManager_Start(s16 csId, Actor* actor) {
    if (!GameInteractor_Should(VB_START_CUTSCENE, true, &csId, actor)) {
        return CS_ID_NONE;
    }

    ActorCutscene* csEntry;
    Camera* subCam;
    Camera* retCam;
    s32 csType = 0;

    if ((csId <= CS_ID_NONE) || (sCutsceneMgr.csId != CS_ID_NONE)) {
        return csId;
    }

    sCutsceneMgr.startMethod = CS_START_0;
    csEntry = CutsceneManager_GetCutsceneEntryImpl(csId);

    ShrinkWindow_Letterbox_SetSizeTarget(csEntry->letterboxSize);
    CutsceneManager_SetHudVisibility(csEntry->hudVisibility);

    if (csId == CS_ID_GLOBAL_END) {
        csType = 1;
    } else if (csEntry->scriptIndex != CS_SCRIPT_ID_NONE) {
        // scripted cutscene
        csType = 1;
    } else if ((csId != CS_ID_GLOBAL_DOOR) && (csId != CS_ID_GLOBAL_TALK)) {
        csType = 2;
    }

    if (csType != 0) {
        sCutsceneMgr.retCamId = Play_GetActiveCamId(sCutsceneMgr.play);
        sCutsceneMgr.subCamId = Play_CreateSubCamera(sCutsceneMgr.play);

        subCam = Play_GetCamera(sCutsceneMgr.play, sCutsceneMgr.subCamId);
        retCam = Play_GetCamera(sCutsceneMgr.play, sCutsceneMgr.retCamId);

        if ((retCam->setting == CAM_SET_START0) || (retCam->setting == CAM_SET_START2) ||
            (retCam->setting == CAM_SET_START1)) {
            if (CutsceneManager_FindEntranceCsId() != csId) {
                func_800E0348(retCam);
            } else {
                Camera_UnsetStateFlag(retCam, CAM_STATE_2);
            }
        }

        memcpy(subCam, retCam, sizeof(Camera));
        subCam->camId = sCutsceneMgr.subCamId;
        Camera_UnsetStateFlag(subCam, CAM_STATE_6 | CAM_STATE_0);

        Play_ChangeCameraStatus(sCutsceneMgr.play, sCutsceneMgr.retCamId, CAM_STATUS_WAIT);
        Play_ChangeCameraStatus(sCutsceneMgr.play, sCutsceneMgr.subCamId, CAM_STATUS_ACTIVE);

        subCam->target = sCutsceneMgr.targetActor = actor;
        subCam->behaviorFlags = 0;

        if (csType == 1) {
            Camera_ChangeSetting(subCam, CAM_SET_FREE0);
            Cutscene_StartScripted(sCutsceneMgr.play, csEntry->scriptIndex);
            sCutsceneMgr.length = csEntry->length;
        } else {
            if (csEntry->csCamId != CS_CAM_ID_NONE) {
                Camera_ChangeActorCsCamIndex(subCam, csEntry->csCamId);
            } else {
                Camera_ChangeSetting(subCam, CAM_SET_FREE0);
            }
            sCutsceneMgr.length = csEntry->length;
        }
    }
    sCutsceneMgr.csId = csId;

    return csId;
}

s16 CutsceneManager_Stop(s16 csId) {
    ActorCutscene* csEntry;

    if (csId <= CS_ID_NONE) {
        return csId;
    }

    csEntry = CutsceneManager_GetCutsceneEntryImpl(sCutsceneMgr.csId);
    if ((sCutsceneMgr.length > 0) && (csEntry->scriptIndex == CS_SCRIPT_ID_NONE)) {
        return -2;
    }
    if ((csId != CS_ID_GLOBAL_END) && (csEntry->scriptIndex != CS_SCRIPT_ID_NONE)) {
        return -3;
    }

    if (csId == CS_ID_GLOBAL_END) {
        csId = sCutsceneMgr.csId;
    }
    if (csId == sCutsceneMgr.csId) {
        sCutsceneMgr.endCsId = sCutsceneMgr.csId;
        return sCutsceneMgr.endCsId;
    }
    return -1;
}

s16 CutsceneManager_GetCurrentCsId(void) {
    return sCutsceneMgr.csId;
}

ActorCutscene* CutsceneManager_GetCutsceneEntry(s16 csId) {
    return CutsceneManager_GetCutsceneEntryImpl(csId);
}

s16 CutsceneManager_GetAdditionalCsId(s16 csId) {
    if (csId <= CS_ID_NONE) {
        return CS_ID_NONE;
    }
    return CutsceneManager_GetCutsceneEntryImpl(csId)->additionalCsId;
}

s16 CutsceneManager_GetLength(s16 csId) {
    if (csId <= CS_ID_NONE) {
        return -1;
    }
    return CutsceneManager_GetCutsceneEntryImpl(csId)->length;
}

s16 CutsceneManager_GetCutsceneScriptIndex(s16 csId) {
    if (csId <= CS_ID_NONE) {
        return -1;
    }
    return CutsceneManager_GetCutsceneEntryImpl(csId)->scriptIndex;
}

s16 CutsceneManager_GetCutsceneCustomValue(s16 csId) {
    if (csId <= CS_ID_NONE) {
        return -1;
    }
    return CutsceneManager_GetCutsceneEntryImpl(csId)->customValue;
}

s16 CutsceneManager_GetCurrentSubCamId(s16 csId) {
    return sCutsceneMgr.subCamId;
}

s16 CutsceneManager_FindEntranceCsId(void) {
    PlayState* play;
    s32 csId;

    for (csId = 0; csId < sSceneCutsceneCount; csId++) {
        if (sSceneCutsceneList[csId].scriptIndex != CS_SCRIPT_ID_NONE) {
            PlayState* play = sCutsceneMgr.play;

            if ((sSceneCutsceneList[csId].scriptIndex < play->csCtx.scriptListCount) &&
                (sCutsceneMgr.play->curSpawn ==
                 sCutsceneMgr.play->csCtx.scriptList[sSceneCutsceneList[csId].scriptIndex].spawn)) {
                return csId;
            }
        }
    }

    for (csId = 0; csId < sSceneCutsceneCount; csId++) {
        if ((sSceneCutsceneList[csId].customValue >= 100) &&
            (sSceneCutsceneList[csId].customValue == (sCutsceneMgr.play->curSpawn + 100))) {
            return csId;
        }
    }

    return -1;
}

s32 func_800F22C4(s16 csId, Actor* actor) {
    f32 dist;
    s16 screenPosX;
    s16 screenPosY;

    if ((sCutsceneMgr.csId == CS_ID_NONE) || (csId == CS_ID_NONE)) {
        return 4;
    }

    Actor_GetScreenPos(sCutsceneMgr.play, actor, &screenPosX, &screenPosY);

    dist = OLib_Vec3fDist(&actor->focus.pos, &Play_GetCamera(sCutsceneMgr.play, sCutsceneMgr.subCamId)->eye);

    if ((screenPosX > 40) && (screenPosX < SCREEN_WIDTH - 40) && (screenPosY > 40) &&
        (screenPosY < SCREEN_HEIGHT - 40) && (dist < 700.0f)) {
        return 1;
    }
    if (sCutsceneMgr.length < 6) {
        return 2;
    }
    if (csId == sCutsceneMgr.csId) {
        return 0;
    }
    return 3;
}

void CutsceneManager_SetReturnCamera(s16 camId) {
    sCutsceneMgr.retCamId = camId;
}

#include "global.h"

static CutsceneCamera* sCurCsCamera;

typedef s16 (*CsCamInterpolateCallback)(Vec3f*, f32*, s16*, CsCmdCamPoint*, CsCmdCamMisc*, CutsceneCameraInterp*);

s16 CutsceneCamera_Interp_Off(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                              CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_None(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                               CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_MultiPointQuadratic(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                              CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_MultiPointCubic(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                          CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_Set(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                              CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_Linear(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                 CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_Scale(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_Geo(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                              CutsceneCameraInterp* interpState);
s16 CutsceneCamera_Interp_Unused(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                 CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState);

f32 func_80163660(Actor* actor);

s16 CutsceneCamera_Interp_Off(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                              CutsceneCameraInterp* interpState) {
    return 0;
}

/**
 * Initializes Cutscene Camera Info
 */
s32 CutsceneCamera_Init(Camera* camera, CutsceneCamera* csCamera) {
    csCamera->camera = camera;

    csCamera->nextSplineTimer = csCamera->updateSplineTimer = 0;
    csCamera->cmdIndex = 0;
    csCamera->splineIndex = -1;
    csCamera->splineNeedsInit = true;
    csCamera->state = CS_CAM_STATE_UPDATE_ALL;

    sCurCsCamera = csCamera;

    __osMemset(&csCamera->eyeInterp, 0, sizeof(CutsceneCameraInterp));
    __osMemset(&csCamera->atInterp, 0, sizeof(CutsceneCameraInterp));

    csCamera->eyeInterp.type = csCamera->atInterp.type = CS_CAM_INTERP_OFF;

    return 1;
}

CsCamInterpolateCallback CutsceneCamera_Interpolate(u8 interpType) {
    switch (interpType) {
        case CS_CAM_INTERP_OFF:
        default:
            return CutsceneCamera_Interp_Off;

        case CS_CAM_INTERP_NONE:
            return CutsceneCamera_Interp_None;

        case CS_CAM_INTERP_MP_QUAD:
            return CutsceneCamera_Interp_MultiPointQuadratic;

        case CS_CAM_INTERP_MP_CUBIC:
            return CutsceneCamera_Interp_MultiPointCubic;

        case CS_CAM_INTERP_SET:
            return CutsceneCamera_Interp_Set;

        case CS_CAM_INTERP_LINEAR:
            return CutsceneCamera_Interp_Linear;

        case CS_CAM_INTERP_SCALE:
            return CutsceneCamera_Interp_Scale;

        case CS_CAM_INTERP_GEO:
            return CutsceneCamera_Interp_Geo;
    }
}

u8 CutsceneCamera_ProcessSpline(CutsceneCamera* csCamera) {
    s32 sp5C;
    f32* camFov;
    s16* camRoll;
    CsCamInterpolateCallback interpHandler;
    Player* player;
    Actor* target;
    s16 numPoints;

    sp5C = true;
    if (csCamera->state == CS_CAM_STATE_DONE_SPLINE) {
        return false;
    }

    player = GET_PLAYER(csCamera->camera->play);
    target = csCamera->camera->target;

    if (csCamera->eyeCmd[csCamera->atInterp.curPoint].interpType <
        csCamera->atCmd[csCamera->eyeInterp.curPoint].interpType) {
        sp5C = false;
    }

    csCamera->eyeInterp.curPos = csCamera->camera->eye;
    csCamera->atInterp.curPos = csCamera->camera->at;

    if (sp5C) {
        camFov = NULL;
    } else {
        camFov = &csCamera->camera->fov;
    }

    if (sp5C) {
        camRoll = NULL;
    } else {
        camRoll = &csCamera->camera->roll;
    }

    interpHandler = CutsceneCamera_Interpolate(csCamera->atCmd[csCamera->eyeInterp.curPoint].interpType);

    switch (csCamera->atCmd[csCamera->eyeInterp.curPoint].relativeTo) {
        case CS_CAM_REL_2:
            OLib_Vec3fDiff(&player->actor.world, &csCamera->camera->at, &csCamera->camera->at, OLIB_DIFF);
            break;

        case CS_CAM_REL_3:
            OLib_Vec3fDiff(&player->actor.world, &csCamera->camera->at, &csCamera->camera->at, OLIB_DIFF_OFFSET);
            break;

        case CS_CAM_REL_1:
            OLib_Vec3fDiff(&player->actor.world, &csCamera->camera->at, &csCamera->camera->at, OLIB_DIFF_OFFSET);
            break;

        case CS_CAM_REL_4:
            OLib_Vec3fDiff(&target->world, &csCamera->camera->at, &csCamera->camera->at, OLIB_DIFF_OFFSET);
            break;

        case CS_CAM_REL_5:
            OLib_Vec3fDiff(&target->world, &csCamera->camera->at, &csCamera->camera->at, OLIB_DIFF);
            break;

        default: // CS_CAM_REL_0
            break;
    }

    numPoints = interpHandler(&csCamera->camera->at, camFov, camRoll, &csCamera->atCmd[csCamera->eyeInterp.curPoint],
                              &csCamera->miscCmd[csCamera->eyeInterp.curPoint], &csCamera->eyeInterp);

    switch (csCamera->atCmd[csCamera->eyeInterp.curPoint].relativeTo) {
        case CS_CAM_REL_2:
            OLib_Vec3fAdd(&player->actor.world, &csCamera->camera->at, &csCamera->camera->at, OLIB_ADD);
            break;

        case CS_CAM_REL_3:
            OLib_Vec3fAdd(&player->actor.world, &csCamera->camera->at, &csCamera->camera->at, OLIB_ADD_OFFSET);
            csCamera->camera->at.y += func_80163660(&player->actor);
            break;

        case CS_CAM_REL_1:
            OLib_Vec3fAdd(&player->actor.world, &csCamera->camera->at, &csCamera->camera->at, OLIB_ADD_OFFSET);
            break;

        case CS_CAM_REL_4:
            OLib_Vec3fAdd(&target->world, &csCamera->camera->at, &csCamera->camera->at, OLIB_ADD_OFFSET);
            break;

        case CS_CAM_REL_5:
            OLib_Vec3fAdd(&target->world, &csCamera->camera->at, &csCamera->camera->at, OLIB_ADD);
            break;

        default: // CS_CAM_REL_0
            break;
    }

    csCamera->eyeInterp.curPoint += numPoints;

    if (sp5C) {
        camFov = &csCamera->camera->fov;
    } else {
        camFov = NULL;
    }

    if (sp5C) {
        camRoll = &csCamera->camera->roll;
    } else {
        camRoll = NULL;
    }

    interpHandler = CutsceneCamera_Interpolate(csCamera->eyeCmd[csCamera->atInterp.curPoint].interpType);

    switch (csCamera->eyeCmd[csCamera->atInterp.curPoint].relativeTo) {
        case CS_CAM_REL_2:
            OLib_Vec3fDiff(&player->actor.world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_DIFF);
            break;

        case CS_CAM_REL_3:
            OLib_Vec3fDiff(&player->actor.world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_DIFF_OFFSET);
            break;

        case CS_CAM_REL_1:
            OLib_Vec3fDiff(&player->actor.world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_DIFF_OFFSET);
            break;

        case CS_CAM_REL_4:
            OLib_Vec3fDiff(&target->world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_DIFF_OFFSET);
            break;

        case CS_CAM_REL_5:
            OLib_Vec3fDiff(&target->world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_DIFF);
            break;

        default: // CS_CAM_REL_0
            break;
    }

    numPoints = interpHandler(&csCamera->camera->eye, camFov, camRoll, &csCamera->eyeCmd[csCamera->atInterp.curPoint],
                              &csCamera->miscCmd[csCamera->atInterp.curPoint], &csCamera->atInterp);

    switch (csCamera->eyeCmd[csCamera->atInterp.curPoint].relativeTo) {
        case CS_CAM_REL_2:
            OLib_Vec3fAdd(&player->actor.world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_ADD);
            break;

        case CS_CAM_REL_3:
            OLib_Vec3fAdd(&player->actor.world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_ADD_OFFSET);
            csCamera->camera->eye.y += func_80163660(&player->actor);
            break;

        case CS_CAM_REL_1:
            OLib_Vec3fAdd(&player->actor.world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_ADD_OFFSET);
            break;

        case CS_CAM_REL_4:
            OLib_Vec3fAdd(&target->world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_ADD_OFFSET);
            break;

        case CS_CAM_REL_5:
            OLib_Vec3fAdd(&target->world, &csCamera->camera->eye, &csCamera->camera->eye, OLIB_ADD);
            break;

        default: // CS_CAM_REL_0
            break;
    }

    csCamera->atInterp.curPoint += numPoints;

    if ((csCamera->eyeInterp.curPoint >= csCamera->eyeInterp.numEntries) ||
        (csCamera->atInterp.curPoint >= csCamera->atInterp.numEntries)) {
        return false;
    }

    return true;
}

/**
 * Processes camera cutscene commands
 */
s32 CutsceneCamera_UpdateSplines(u8* script, CutsceneCamera* csCamera) {
    CsCmdCamSpline* spline;

    switch (csCamera->state) {
        case CS_CAM_STATE_DONE:
            return 0;

        case CS_CAM_STATE_PAUSE:
            return csCamera->nextSplineTimer;

        case CS_CAM_STATE_UPDATE_SPLINE:
            if (csCamera->updateSplineTimer <= csCamera->duration) {
                csCamera->updateSplineTimer++;
                if (csCamera->updateSplineTimer <= csCamera->duration) {
                    // Process Spline
                    if (!CutsceneCamera_ProcessSpline(csCamera)) {
                        csCamera->state = CS_CAM_STATE_DONE_SPLINE;
                    }
                }
            }
            break;

        case CS_CAM_STATE_DONE_SPLINE:
            break;

        default: // CS_CAM_STATE_UPDATE_ALL
            if (csCamera->splineNeedsInit == true) {
                // Spline Header
                spline = (CsCmdCamSpline*)&script[csCamera->cmdIndex];
                // 2S2H [Port] This was reading OOB because `spline->numEntries` was -1. We can either set the fields to
                // 0 and continue, or goto an existing exit.
                if (spline->duration == -1) {
                    goto done;
                }
                csCamera->atInterp.numEntries = csCamera->eyeInterp.numEntries = spline->numEntries;
                csCamera->duration = spline->duration;
                csCamera->cmdIndex += sizeof(CsCmdCamSpline);

                // At Point
                csCamera->atCmd = (CsCmdCamPoint*)&script[csCamera->cmdIndex];
                csCamera->cmdIndex += (s16)(csCamera->eyeInterp.numEntries * sizeof(CsCmdCamPoint));

                // Eye Point
                csCamera->eyeCmd = (CsCmdCamPoint*)&script[csCamera->cmdIndex];
                csCamera->cmdIndex += (s16)(csCamera->eyeInterp.numEntries * sizeof(CsCmdCamPoint));

                // Misc
                csCamera->miscCmd = (CsCmdCamMisc*)&script[csCamera->cmdIndex];
                csCamera->cmdIndex += (s16)(csCamera->eyeInterp.numEntries * sizeof(CsCmdCamMisc));

                // Other Params
                csCamera->eyeInterp.curPoint = csCamera->atInterp.curPoint = 0;

                csCamera->splineNeedsInit = false;
                csCamera->splineIndex++;
                csCamera->state = CS_CAM_STATE_UPDATE_ALL;
                csCamera->nextSplineTimer = csCamera->updateSplineTimer = 0;
                csCamera->eyeInterp.type = csCamera->atInterp.type = CS_CAM_INTERP_OFF;
            }

            csCamera->nextSplineTimer++;

            if (csCamera->updateSplineTimer <= csCamera->duration) {
                csCamera->updateSplineTimer++;
                if (csCamera->updateSplineTimer <= csCamera->duration) {
                    // Process SubCommands
                    if (!CutsceneCamera_ProcessSpline(csCamera)) {
                        csCamera->state = CS_CAM_STATE_DONE_SPLINE;
                    }
                }
            }
            break;
    }

    if (csCamera->nextSplineTimer > csCamera->duration) {
        // Next Spline
        csCamera->splineNeedsInit = true;
        spline = (CsCmdCamSpline*)&script[csCamera->cmdIndex];
        if (spline->numEntries == -1) {
        done:
            csCamera->state = CS_CAM_STATE_DONE;
            return 0;
        }
    }

    return csCamera->nextSplineTimer;
}

// Unused
s16 func_80161BAC(void) {
    return (sCurCsCamera->state == CS_CAM_STATE_PAUSE) || (sCurCsCamera->state == CS_CAM_STATE_UPDATE_SPLINE);
}

void CutsceneCamera_SetState(s16 state) {
    if (sCurCsCamera->state == CS_CAM_STATE_UPDATE_ALL) {
        sCurCsCamera->state = state;
    }
}

void CutsceneCamera_Reset(void) {
    sCurCsCamera->state = CS_CAM_STATE_UPDATE_ALL;
}

// Linear interpolation from initial values to cmd values. Set weight to 100 to go all the way to the cmd value.
s16 CutsceneCamera_Interp_Linear(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                 CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState) {
    f32 lerp;

    if (interpState->type != CS_CAM_INTERP_LINEAR) {
        // Initialize
        interpState->type = CS_CAM_INTERP_LINEAR;
        interpState->waypoint = 0;
        interpState->curFrame = 0;
        interpState->duration = 1;
        if (camPos != NULL) {
            interpState->initPos.x = camPos->x;
            interpState->initPos.y = camPos->y;
            interpState->initPos.z = camPos->z;
        }

        if (camFov != NULL) {
            interpState->initFov = *camFov;
        }

        if (camRoll != NULL) {
            interpState->initRoll = *camRoll;
        }
    }

    interpState->curFrame++;

    lerp = ((f32)interpState->curFrame / pointCmd->duration) * (pointCmd->weight / 100.0f);

    if (camPos != NULL) {
        VEC3F_LERPIMPDST(camPos, &interpState->initPos, &pointCmd->pos, lerp);
    }

    if (camFov != NULL) {
        *camFov = LERPIMP(interpState->initFov, miscCmd->fov, lerp);
    }

    if (camRoll != NULL) {
        s16 targetRoll;
        s16 rollDiffToTarget;

        targetRoll = CAM_DEG_TO_BINANG(miscCmd->roll);

        rollDiffToTarget = (s16)(targetRoll - TRUNCF_BINANG(interpState->initRoll));

        *camRoll = TRUNCF_BINANG(interpState->initRoll) + TRUNCF_BINANG(rollDiffToTarget * lerp);
    }

    if (interpState->curFrame >= pointCmd->duration) {
        // Finished
        interpState->type = CS_CAM_INTERP_OFF;
        return 1;
    }

    return 0;
}

s16 CutsceneCamera_Interp_Scale(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState) {
    f32 lerp;
    f32 tmp1;
    f32 tmp2;

    if (interpState->type != CS_CAM_INTERP_SCALE) {
        // Initialize
        interpState->type = CS_CAM_INTERP_SCALE;
        interpState->waypoint = 0;
        interpState->curFrame = 0;
        interpState->duration = 1;
        if (camPos != NULL) {
            interpState->initPos.x = camPos->x;
            interpState->initPos.y = camPos->y;
            interpState->initPos.z = camPos->z;
        }
        if (camFov != NULL) {
            interpState->initFov = *camFov;
        }
        if (camRoll != NULL) {
            interpState->initRoll = *camRoll;
        }
    }

    tmp2 = (((pointCmd->weight + 100) * (pointCmd->duration / 2)) +
            (((pointCmd->weight + 100) / 2) * (pointCmd->duration & 1)));
    if (pointCmd->duration < 2) {
        lerp = 1.0f;
    } else {
        tmp1 = (f32)(pointCmd->weight - 100) / (pointCmd->duration - 1);
        lerp = ((interpState->curFrame * tmp1) + 100.0f) / tmp2;
    }

    interpState->curFrame++;

    if (camPos != NULL) {
        camPos->x += (pointCmd->pos.x - interpState->initPos.x) * lerp;
        camPos->y += (pointCmd->pos.y - interpState->initPos.y) * lerp;
        camPos->z += (pointCmd->pos.z - interpState->initPos.z) * lerp;
    }

    if (camFov != NULL) {
        *camFov += (miscCmd->fov - interpState->initFov) * lerp;
    }

    if (camRoll != NULL) {
        s16 targetRoll;
        s16 rollDiffToTarget;

        targetRoll = CAM_DEG_TO_BINANG(miscCmd->roll);

        rollDiffToTarget = (s16)(targetRoll - TRUNCF_BINANG(interpState->initRoll));

        *camRoll += TRUNCF_BINANG(rollDiffToTarget * lerp);
    }

    if (interpState->curFrame >= pointCmd->duration) {
        // Finished
        interpState->type = CS_CAM_INTERP_OFF;
        return 1;
    }

    return 0;
}

s16 CutsceneCamera_Interp_Geo(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                              CutsceneCameraInterp* interpState) {
    VecGeo sp40;
    f32 lerp;
    f32 tmp1;
    f32 tmp2;

    if (interpState->type != CS_CAM_INTERP_GEO) {
        // Initialize
        interpState->type = CS_CAM_INTERP_GEO;
        interpState->waypoint = 0;
        interpState->curFrame = 0;
        interpState->duration = 1;
        if (camPos != NULL) {
            interpState->unk_20 = OLib_Vec3fDist(&interpState->curPos, camPos) * Math_FTanF(DEG_TO_RAD(*camFov));
        }
        if (camFov != NULL) {
            interpState->initFov = *camFov;
        }
        if (camRoll != NULL) {
            interpState->initRoll = *camRoll;
        }
    }

    tmp2 = (((pointCmd->weight + 100) * (pointCmd->duration / 2)) +
            (((pointCmd->weight + 100) / 2) * (pointCmd->duration & 1)));
    if (pointCmd->duration < 2) {
        lerp = 1.0f;
    } else {
        tmp1 = (f32)(pointCmd->weight - 100) / (pointCmd->duration - 1);
        lerp = ((interpState->curFrame * tmp1) + 100.0f) / tmp2;
    }

    interpState->curFrame++;

    if (camPos != NULL) {
        sp40 = OLib_Vec3fDiffToVecGeo(&interpState->curPos, camPos);
        sp40.r = interpState->unk_20 / Math_FTanF(DEG_TO_RAD(*camFov));
        *camPos = OLib_AddVecGeoToVec3f(&interpState->curPos, &sp40);
    }

    if (camFov != NULL) {
        *camFov += (miscCmd->fov - interpState->initFov) * lerp;
    }

    if (camRoll != NULL) {
        s16 targetRoll;
        s16 rollDiffToTarget;

        targetRoll = CAM_DEG_TO_BINANG(miscCmd->roll);

        rollDiffToTarget = (s16)(targetRoll - TRUNCF_BINANG(interpState->initRoll));

        *camRoll += TRUNCF_BINANG(rollDiffToTarget * lerp);
    }

    if (interpState->curFrame >= pointCmd->duration) {
        // Finished
        interpState->type = CS_CAM_INTERP_OFF;
        return 1;
    }

    return 0;
}

// Updates the interpolation state but does not change the pos/fov/roll values
s16 CutsceneCamera_Interp_None(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                               CutsceneCameraInterp* interpState) {
    if (interpState->type != CS_CAM_INTERP_NONE) {
        // Initialize
        interpState->type = CS_CAM_INTERP_NONE;
        interpState->waypoint = 0;
        interpState->curFrame = 0;
        interpState->duration = 1;
    }

    interpState->curFrame++;

    if (interpState->curFrame >= pointCmd->duration) {
        // Finishes
        interpState->type = CS_CAM_INTERP_OFF;
        return 1;
    }

    return 0;
}

// Immediately sets the values
s16 CutsceneCamera_Interp_Set(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd, CsCmdCamMisc* miscCmd,
                              CutsceneCameraInterp* interpState) {
    s16 pad;

    if (interpState->type != CS_CAM_INTERP_SET) {
        // Initialize
        interpState->type = CS_CAM_INTERP_SET;
        interpState->waypoint = 0;
        interpState->curFrame = 0;
        interpState->duration = 1;
        if (camFov != NULL) {
            *camFov = miscCmd->fov;
        }
        if (camRoll != NULL) {
            *camRoll = CAM_DEG_TO_BINANG(miscCmd->roll);
        }
    }

    if (camPos != NULL) {
        camPos->x = pointCmd->pos.x;
        camPos->y = pointCmd->pos.y;
        camPos->z = pointCmd->pos.z;
    }

    interpState->curFrame++;

    if (interpState->curFrame >= pointCmd->duration) {
        // Finished
        interpState->type = CS_CAM_INTERP_OFF;
        return 1;
    }

    return 0;
}

void func_801624EC(f32 u, f32* coeff) {
    f32 u1 = 1.0f - u;

    coeff[0] = u1 * u1 * 0.5f;
    coeff[1] = u * u1 + 0.5f;
    coeff[2] = u * u * 0.5f;
}

s16 CutsceneCamera_Interp_MultiPointQuadratic(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                              CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState) {
    f32 new_var;
    f32 coeff[3];
    s32 waypoints[3];

    if (interpState->type != CS_CAM_INTERP_MP_QUAD) {
        // Initialize
        interpState->type = CS_CAM_INTERP_MP_QUAD;
        interpState->waypoint = 0;
        interpState->duration = pointCmd->duration;
        interpState->curFrame = 0;
    }

    new_var = (f32)interpState->curFrame / pointCmd[interpState->waypoint + 1].duration;

    if (interpState->waypoint < (interpState->duration - 1)) {
        waypoints[0] = interpState->waypoint;
    } else {
        waypoints[0] = interpState->duration - 1;
    }

    if ((interpState->waypoint + 1) < (interpState->duration - 1)) {
        waypoints[1] = interpState->waypoint + 1;
    } else {
        waypoints[1] = interpState->duration - 1;
    }

    if ((interpState->waypoint + 2) < (interpState->duration - 1)) {
        waypoints[2] = interpState->waypoint + 2;
    } else {
        waypoints[2] = interpState->duration - 1;
    }

    func_801624EC(new_var, coeff);

    if (camPos != NULL) {
        camPos->x = (coeff[0] * pointCmd[waypoints[0]].pos.x) + (coeff[1] * pointCmd[waypoints[1]].pos.x) +
                    (coeff[2] * pointCmd[waypoints[2]].pos.x);
        camPos->y = (coeff[0] * pointCmd[waypoints[0]].pos.y) + (coeff[1] * pointCmd[waypoints[1]].pos.y) +
                    (coeff[2] * pointCmd[waypoints[2]].pos.y);
        camPos->z = (coeff[0] * pointCmd[waypoints[0]].pos.z) + (coeff[1] * pointCmd[waypoints[1]].pos.z) +
                    (coeff[2] * pointCmd[waypoints[2]].pos.z);
    }

    if (camFov != NULL) {
        *camFov = (coeff[0] * miscCmd[waypoints[0]].fov) + (coeff[1] * miscCmd[waypoints[1]].fov) +
                  (coeff[2] * miscCmd[waypoints[2]].fov);
    }

    if (camRoll != NULL) {
        s16 targetRolls[3];
        s32 sp28[2];
        s32 rollDiffToTarget;

        targetRolls[0] = CAM_DEG_TO_BINANG(miscCmd[waypoints[0]].roll);
        targetRolls[1] = CAM_DEG_TO_BINANG(miscCmd[waypoints[1]].roll);
        targetRolls[2] = CAM_DEG_TO_BINANG(miscCmd[waypoints[2]].roll);

        sp28[0] = (s16)(targetRolls[1] - targetRolls[0]);
        sp28[1] = sp28[0] + (s16)(targetRolls[2] - targetRolls[1]);

        rollDiffToTarget = ((coeff[1] * sp28[0]) + (coeff[2] * sp28[1]));
        *camRoll = targetRolls[0] + rollDiffToTarget;
    }

    interpState->curFrame++;

    if (interpState->curFrame == pointCmd[interpState->waypoint + 1].duration) {
        interpState->waypoint++;
        interpState->curFrame = 0;
        if (interpState->waypoint >= (interpState->duration - 2)) {
            interpState->type = CS_CAM_INTERP_OFF;
            return interpState->duration;
        }
    }
    return 0;
}

/**
 * This code is very similar to the spline system in OoT's func_800BB0A0
 * in that it is based on the Super Mario 64 cutscene camera movement
 */
void func_801629BC(f32 u, f32* coeff) {
    coeff[0] = (1.0f - u) * (1.0f - u) * (1.0f - u) * (1.0f / 6.0f);
    coeff[1] = ((u * u * u * 0.5f) - u * u) + (2.0f / 3.0f);
    coeff[2] = (u * u * u * -0.5f) + (u * u * 0.5f) + (u * 0.5f) + (1.0f / 6.0f);
    coeff[3] = u * u * u * (1.0f / 6.0f);
}

s16 CutsceneCamera_Interp_MultiPointCubic(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                          CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState) {
    f32 new_var;
    f32 coeff[4];
    s32 waypoints[4];

    if (interpState->type != CS_CAM_INTERP_MP_CUBIC) {
        // Initialize
        interpState->type = CS_CAM_INTERP_MP_CUBIC;
        interpState->waypoint = 0;
        interpState->duration = pointCmd->duration;
        interpState->curFrame = 0;
    }

    new_var = (f32)interpState->curFrame / pointCmd[interpState->waypoint + 1].duration;

    if (interpState->waypoint < (interpState->duration - 1)) {
        waypoints[0] = interpState->waypoint;
    } else {
        waypoints[0] = interpState->duration - 1;
    }

    if ((interpState->waypoint + 1) < (interpState->duration - 1)) {
        waypoints[1] = interpState->waypoint + 1;
    } else {
        waypoints[1] = interpState->duration - 1;
    }

    if ((interpState->waypoint + 2) < (interpState->duration - 1)) {
        waypoints[2] = interpState->waypoint + 2;
    } else {
        waypoints[2] = interpState->duration - 1;
    }

    if ((interpState->waypoint + 3) < (interpState->duration - 1)) {
        waypoints[3] = interpState->waypoint + 3;
    } else {
        waypoints[3] = interpState->duration - 1;
    }

    func_801629BC(new_var, coeff);

    if (camPos != NULL) {
        camPos->x = (coeff[0] * pointCmd[waypoints[0]].pos.x) + (coeff[1] * pointCmd[waypoints[1]].pos.x) +
                    (coeff[2] * pointCmd[waypoints[2]].pos.x) + (coeff[3] * pointCmd[waypoints[3]].pos.x);
        camPos->y = (coeff[0] * pointCmd[waypoints[0]].pos.y) + (coeff[1] * pointCmd[waypoints[1]].pos.y) +
                    (coeff[2] * pointCmd[waypoints[2]].pos.y) + (coeff[3] * pointCmd[waypoints[3]].pos.y);
        camPos->z = (coeff[0] * pointCmd[waypoints[0]].pos.z) + (coeff[1] * pointCmd[waypoints[1]].pos.z) +
                    (coeff[2] * pointCmd[waypoints[2]].pos.z) + (coeff[3] * pointCmd[waypoints[3]].pos.z);
    }

    if (camFov != NULL) {
        *camFov = (coeff[0] * miscCmd[waypoints[0]].fov) + (coeff[1] * miscCmd[waypoints[1]].fov) +
                  (coeff[2] * miscCmd[waypoints[2]].fov) + (coeff[3] * miscCmd[waypoints[3]].fov);
    }

    if (camRoll != NULL) {
        s16 targetRolls[4];
        s32 sp2C[3];
        s32 rollDiffToTarget;

        targetRolls[0] = CAM_DEG_TO_BINANG(miscCmd[waypoints[0]].roll);
        targetRolls[1] = CAM_DEG_TO_BINANG(miscCmd[waypoints[1]].roll);
        targetRolls[2] = CAM_DEG_TO_BINANG(miscCmd[waypoints[2]].roll);
        targetRolls[3] = CAM_DEG_TO_BINANG(miscCmd[waypoints[3]].roll);

        sp2C[0] = (s16)(targetRolls[1] - targetRolls[0]);
        sp2C[1] = sp2C[0] + (s16)(targetRolls[2] - targetRolls[1]);
        sp2C[2] = sp2C[1] + (s16)(targetRolls[3] - targetRolls[2]);

        rollDiffToTarget = ((coeff[1] * sp2C[0]) + (coeff[2] * sp2C[1]) + (coeff[3] * sp2C[2]));
        *camRoll = targetRolls[0] + rollDiffToTarget;
    }

    interpState->curFrame++;

    if (interpState->curFrame == pointCmd[interpState->waypoint + 1].duration) {
        interpState->curFrame = 0;
        interpState->waypoint++;
        if (interpState->waypoint >= (interpState->duration - 3)) {
            interpState->type = CS_CAM_INTERP_OFF;
            return interpState->duration;
        }
    }
    return 0;
}

static f32 sKnots[38];

// Only used by unused CutsceneCamera_Interp_Unused
void func_80162FF8(s16 arg0) {
    f32 val = 0.0f;
    s32 i;

    sKnots[0] = 0.0f;
    sKnots[1] = 0.0f;
    sKnots[2] = 0.0f;

    for (i = 3; i < arg0; i++) {
        if (i == 3) {
            val += 0.9f;
        } else if ((i == 4) || (i == (arg0 - 1))) {
            val += 0.6f;
        } else {
            val += 0.3f;
        }

        sKnots[i] = val;
    }

    val += 0.9f;
    sKnots[i++] = val;
    sKnots[i++] = val;
    sKnots[i++] = val;
}

#define FUNC_801631DC_ORDER 3

// Only used by unused CutsceneCamera_Interp_Unused
void func_801631DC(f32 progress, s32 arg2, f32* coeff) {
    f32 coeffTemp[4][4];
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < FUNC_801631DC_ORDER + 1; i++) {
        for (j = 0; j < FUNC_801631DC_ORDER + 1; j++) {
            coeffTemp[i][j] = 0.0f;
        }
    }

    coeffTemp[0][FUNC_801631DC_ORDER - 1] = 1.0f;

    for (i = 1; i < FUNC_801631DC_ORDER; i++) {
        for (j = arg2 - i, k = (FUNC_801631DC_ORDER - 1) - i; j <= arg2; j++, k++) {
            if (sKnots[j + i] != sKnots[j]) {
                coeffTemp[i][k] = ((progress - sKnots[j]) / (sKnots[j + i] - sKnots[j])) * coeffTemp[i - 1][k];
            } else {
                coeffTemp[i][k] = 0.0f;
            }

            if (sKnots[j + i + 1] != sKnots[j + 1]) {
                coeffTemp[i][k] +=
                    ((sKnots[j + i + 1] - progress) / (sKnots[j + i + 1] - sKnots[j + 1])) * coeffTemp[i - 1][k + 1];
            }
        }
    }
    for (j = 0; j < FUNC_801631DC_ORDER; j++) {
        coeff[j] = coeffTemp[FUNC_801631DC_ORDER - 1][j];
    }
}

s16 CutsceneCamera_Interp_Unused(Vec3f* camPos, f32* camFov, s16* camRoll, CsCmdCamPoint* pointCmd,
                                 CsCmdCamMisc* miscCmd, CutsceneCameraInterp* interpState) {
    s32 index;
    f32 coeff[3];

    if (interpState->type != CS_CAM_INTERP_MP_CUBIC) {
        // Initialize
        interpState->type = CS_CAM_INTERP_MP_CUBIC;
        interpState->waypoint = 0;
        interpState->duration = pointCmd->duration;
        func_80162FF8(interpState->duration);
        interpState->curFrame = 0;
    }

    index = interpState->waypoint + 2;
    func_801631DC(F32_LERPIMP(sKnots[index], sKnots[index + 1],
                              (f32)interpState->curFrame / miscCmd[interpState->waypoint].unused0),
                  index, coeff);

    if (camPos != NULL) {
        camPos->x = (coeff[0] * pointCmd[interpState->waypoint].pos.x) +
                    (coeff[1] * pointCmd[interpState->waypoint + 1].pos.x) +
                    (coeff[2] * pointCmd[interpState->waypoint + 2].pos.x);
        camPos->y = (coeff[0] * pointCmd[interpState->waypoint].pos.y) +
                    (coeff[1] * pointCmd[interpState->waypoint + 1].pos.y) +
                    (coeff[2] * pointCmd[interpState->waypoint + 2].pos.y);
        camPos->z = (coeff[0] * pointCmd[interpState->waypoint].pos.z) +
                    (coeff[1] * pointCmd[interpState->waypoint + 1].pos.z) +
                    (coeff[2] * pointCmd[interpState->waypoint + 2].pos.z);
    }

    if (camFov != NULL) {
        *camFov = (coeff[0] * miscCmd[interpState->waypoint].fov) +
                  (coeff[1] * miscCmd[interpState->waypoint + 1].fov) +
                  (coeff[2] * miscCmd[interpState->waypoint + 2].fov);
    }

    if (camRoll != NULL) {
        *camRoll = CAM_DEG_TO_BINANG((coeff[0] * miscCmd[interpState->waypoint].roll) +
                                     (coeff[1] * miscCmd[interpState->waypoint + 1].roll) +
                                     (coeff[2] * miscCmd[interpState->waypoint + 2].roll));
    }

    interpState->curFrame++;

    if (interpState->curFrame >= miscCmd[interpState->waypoint].unused0) {
        interpState->waypoint++;
        interpState->curFrame = 0;
        if (interpState->waypoint >= (interpState->duration - 2)) {
            interpState->type = CS_CAM_INTERP_OFF;
            return interpState->duration;
        }
    }
    return 0;
}

f32 func_80163660(Actor* actor) {
    if (actor->category != ACTORCAT_PLAYER) {
        return 38.0f - (actor->focus.pos.y - actor->world.pos.y);
    }

    switch (((Player*)actor)->transformation) {
        case PLAYER_FORM_DEKU:
            return -8.0f;

        case PLAYER_FORM_GORON:
            return 23.0f;

        case PLAYER_FORM_ZORA:
            return 27.0f;

        case PLAYER_FORM_FIERCE_DEITY:
            return 17.0f;

        default:
            return 0.0f;
    }
}

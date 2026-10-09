/*
 * File: z_sub_s.c
 * Description: Various miscellaneous helpers
 */

#include "global.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "BenPort.h"

s16 sPathDayFlags[] = { 0x40, 0x20, 0x10, 8, 4, 2, 1, 0 };

#include "code/sub_s/sub_s.h"

Vec3f gOneVec3f = { 1.0f, 1.0f, 1.0f };

s32 D_801C5DBC[] = { 0, 1 }; // Unused

/**
 * Finds the first EnDoor instance of type `ENDOOR_TYPE_SCHEDULE` and the specified schType (a value from the
 * EnDoorScheduleType enum).
 */
EnDoor* SubS_FindScheduleDoor(PlayState* play, s32 schType) {
    Actor* actor = NULL;
    EnDoor* door;

    while (true) {
        actor = SubS_FindActor(play, actor, ACTORCAT_DOOR, ACTOR_EN_DOOR);
        door = (EnDoor*)actor;

        if (door == NULL) {
            break;
        }

        if ((door->doorType == ENDOOR_TYPE_SCHEDULE) && (door->typeVar.schType == (u8)schType)) {
            break;
        }

        if (actor->next == NULL) {
            door = NULL;
            break;
        }

        actor = actor->next;
    }

    return door;
}

Gfx* SubS_DrawTransformFlexLimb(PlayState* play, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                                OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                                TransformLimbDraw transformLimbDraw, Actor* actor, Mtx** mtx, Gfx* gfx) {
    StandardLimb* limb;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;

    Matrix_Push();
    limb = Lib_SegmentedToVirtual(skeleton[limbIndex]);
    limbIndex++;
    rot = jointTable[limbIndex];
    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;
    newDList = limbDList = limb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(play, limbIndex, &newDList, &pos, &rot, actor, &gfx)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        Matrix_Push();

        transformLimbDraw(play, limbIndex, actor, &gfx);

        if (newDList != NULL) {
            Matrix_ToMtx(*mtx);
            gSPMatrix(gfx++, *mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(gfx++, newDList);
            (*mtx)++;
        } else if (limbDList != NULL) {
            Matrix_ToMtx(*mtx);
            (*mtx)++;
        }
        Matrix_Pop();
    }
    if (postLimbDraw != NULL) {
        postLimbDraw(play, limbIndex, &limbDList, &rot, actor, &gfx);
    }
    if (limb->child != LIMB_DONE) {
        gfx = SubS_DrawTransformFlexLimb(play, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw,
                                         transformLimbDraw, actor, mtx, gfx);
    }
    Matrix_Pop();
    if (limb->sibling != LIMB_DONE) {
        gfx = SubS_DrawTransformFlexLimb(play, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw,
                                         transformLimbDraw, actor, mtx, gfx);
    }
    return gfx;
}

/**
 * Draw all limbs of type `StandardLimb` in a given flexible skeleton
 * Limbs in a flexible skeleton have meshes that can stretch to line up with other limbs.
 * An array of matrices is dynamically allocated so each limb can access any transform to ensure its meshes line up.
 *
 * Also makes use of a `TransformLimbDraw`, which transforms limbs based on world coordinates, as opposed to local limb
 * coordinates.
 * Note that the `TransformLimbDraw` does not have a NULL check, so must be provided even if empty.
 */
Gfx* SubS_DrawTransformFlex(PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount,
                            OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                            TransformLimbDraw transformLimbDraw, Actor* actor, Gfx* gfx) {
    StandardLimb* rootLimb;
    s32 pad;
    Gfx* newDlist;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;
    Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, dListCount * sizeof(Mtx));

    if (skeleton == NULL) {
        return NULL;
    }

    gSPSegment(gfx++, 0x0D, mtx);
    Matrix_Push();
    rootLimb = Lib_SegmentedToVirtual(skeleton[0]);
    pos.x = jointTable[LIMB_ROOT_POS].x;
    pos.y = jointTable[LIMB_ROOT_POS].y;
    pos.z = jointTable[LIMB_ROOT_POS].z;
    rot = jointTable[LIMB_ROOT_ROT];
    newDlist = rootLimb->dList;
    limbDList = rootLimb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(play, 1, &newDlist, &pos, &rot, actor, &gfx)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        Matrix_Push();

        transformLimbDraw(play, 1, actor, &gfx);

        if (newDlist != NULL) {
            Matrix_ToMtx(mtx);
            gSPMatrix(gfx++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(gfx++, newDlist);
            mtx++;
        } else if (limbDList != NULL) {
            Matrix_ToMtx(mtx);
            mtx++;
        }
        Matrix_Pop();
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(play, 1, &limbDList, &rot, actor, &gfx);
    }

    if (rootLimb->child != LIMB_DONE) {
        gfx = SubS_DrawTransformFlexLimb(play, rootLimb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw,
                                         transformLimbDraw, actor, &mtx, gfx);
    }
    Matrix_Pop();
    return gfx;
}

s32 SubS_InCsMode(PlayState* play) {
    s32 inCsMode = false;

    if (Play_InCsMode(play)) {
        inCsMode = true;
    }

    return inCsMode;
}

/**
 * Computes a limb's position and rotation for use in TransformLimbDraws
 *
 * @param[in] newRotZ value to override newRot's Z value if override is true
 * @param[in] newRotY value to override newRot's Y value if override is true
 * @param[out] pos limb's computed position
 * @param[out] rot limb's computed rotation
 * @param[in] stepRot boolean, step towards newRot instead of setting directly
 * @param[in] overrideRot boolean, override newRot with the specified input.
 *
 * @note if overrideRot is true, the rotation will automatically step instead of setting directly
 */
s32 SubS_UpdateLimb(s16 newRotZ, s16 newRotY, Vec3f* pos, Vec3s* rot, s32 stepRot, s32 overrideRot) {
    Vec3f newPos;
    Vec3f zeroVec = gZeroVec3f;
    Vec3s newRot;
    MtxF curState;

    Matrix_MultVec3f(&zeroVec, &newPos);
    Matrix_Get(&curState);
    Matrix_MtxFToYXZRot(&curState, &newRot, false);
    *pos = newPos;

    if (!stepRot && !overrideRot) {
        rot->x = newRot.x;
        rot->y = newRot.y;
        rot->z = newRot.z;
        return true;
    }

    if (overrideRot) {
        newRot.z = newRotZ;
        newRot.y = newRotY;
    }

    Math_SmoothStepToS(&rot->x, newRot.x, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&rot->y, newRot.y, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&rot->z, newRot.z, 3, 0x2AA8, 0xB6);
    return true;
}

void SubS_SetOfferMode(u16* flags, u16 offerMode, u16 mask) {
    *flags = (*flags & ~mask) | offerMode;
}

/**
 * Fills the knot array to be used with time paths
 *
 * The default knot array just pads with `order` duplicate knots of the first knot at the front and of the last knot
 * at the end.
 *
 * @param[out] knots an array of values that are used to compute the progress and the individual weights
 * @param[in] order the order of the interpolation i.e. the number of points in the interpolation
 * @param[in] numPoints the number of points to fill, generally the path count + order
 *
 * @note Same note as SubS_TimePathing_Update()
 */
void SubS_TimePathing_FillKnots(f32 knots[], s32 order, s32 numPoints) {
    s32 i;
    f32 val = 0.0f;

    for (i = 0; i < numPoints; i++) {
        if ((i >= order) && (i < (numPoints - order + 1))) {
            val += 1.0f;
        }
        knots[i] = val;
    }
}

typedef enum {
    /* 0 */ SUBS_TIME_PATHING_PROGRESS_STATUS_ERROR,
    /* 1 */ SUBS_TIME_PATHING_PROGRESS_STATUS_STILL_ON_PATH,
    /* 2 */ SUBS_TIME_PATHING_PROGRESS_STATUS_SHOULD_REACH_END
} SUBS_TIME_PATHING_PROGRESS_STATUS;

/**
 * Computes the progress to be used with time paths
 *
 * @param[out] progress the progress along the path, used to compute the weights
 * @param[in] elapsedTime how much time has passed
 * @param[in] waypointTime how much time per each waypoint
 * @param[in] totalTime how much time the path should take to travel
 * @param[in] pathCount the path count
 * @param[in] order the order of the interpolation i.e. the number of points in the interpolation
 * @param[in] knots see SubS_TimePathing_FillKnots()
 *
 * @return see SUBS_TIME_PATHING_PROGRESS_STATUS
 */
s32 SubS_TimePathing_ComputeProgress(f32* progress, s32 elapsedTime, s32 waypointTime, s32 totalTime, s32 pathCount,
                                     s32 order, f32 knots[]) {
    s32 i;
    s32 j;
    s32 k;
    f32 waypointTimeInv; // The fraction of a waypoint a single unit of time contains

    *progress = 0.0f;
    if ((waypointTime <= 0) || (elapsedTime < 0)) {
        return SUBS_TIME_PATHING_PROGRESS_STATUS_ERROR;
    }

    // When using the knots from SubS_TimePathing_FillKnots() these nested loops seem to simplify to
    // *progress = (f32)elapsedTime / (f32)waypointTime;
    waypointTimeInv = 1.0f / waypointTime;
    k = 0;
    for (i = order - 1; i < pathCount; i++) {
        for (j = 0; j < waypointTime; j++) {
            if (k == elapsedTime) {
                break;
            }
            *progress += (knots[i + 1] - knots[i]) * waypointTimeInv;
            k++;
        }
    }

    return (elapsedTime == totalTime) ? SUBS_TIME_PATHING_PROGRESS_STATUS_SHOULD_REACH_END
                                      : SUBS_TIME_PATHING_PROGRESS_STATUS_STILL_ON_PATH;
}

/**
 * Computes the interpolation weights to be used with time paths
 *
 * Seems to use some kind of B-Spline interpolation algorithm
 *
 * @param[in] order the order of the interpolation i.e. the number of points in the interpolation, max is 10
 * @param[in] progress see SubS_TimePathing_ComputeProgress()
 * @param[in] waypoint the current waypoint
 * @param[in] knots see SubS_TimePathing_FillKnots()
 * @param[out] weights how much to weight each point considered
 */
void SubS_TimePathing_ComputeWeights(s32 order, f32 progress, s32 waypoint, f32 knots[], f32 weights[]) {
    f32 weightsTemp[10][11];
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < order; i++) {
        for (j = 0; j < order + 1; j++) {
            weightsTemp[i][j] = 0.0f;
        }
    }

    weightsTemp[0][order - 1] = 1.0f;

    for (i = 1; i < order; i++) {
        for (j = waypoint - i, k = (order - 1) - i; j <= waypoint; j++, k++) {
            if (knots[j + i] != knots[j]) {
                weightsTemp[i][k] = ((progress - knots[j]) / (knots[j + i] - knots[j])) * weightsTemp[i - 1][k];
            } else {
                weightsTemp[i][k] = 0.0f;
            }

            if (knots[j + i + 1] != knots[j + 1]) {
                weightsTemp[i][k] +=
                    ((knots[j + i + 1] - progress) / (knots[j + i + 1] - knots[j + 1])) * weightsTemp[i - 1][k + 1];
            }
        }
    }
    for (j = 0; j < order; j++) {
        weights[j] = weightsTemp[order - 1][j];
    }
}

/**
 * Computes the X and Z component of the position to move to in time based paths
 *
 * @param[out] x computed x position
 * @param[out] z computed z position
 * @param[in] progress see SubS_TimePathing_ComputeProgress()
 * @param[in] order the order of the interpolation i.e. the number of points in the interpolation, max is 10
 * @param[in] waypoint the current waypoint
 * @param[in] points the path's points
 * @param[in] knots see SubS_TimePathing_FillKnots()
 */
void SubS_TimePathing_ComputeTargetPosXZ(f32* x, f32* z, f32 progress, s32 order, s32 waypoint, Vec3s points[],
                                         f32 knots[]) {
    f32 xPos;
    f32 zPos;
    f32 weights[11];
    f32 weightedX;
    f32 weightedZ;
    f32 weightedTotal;
    s32 i;

    SubS_TimePathing_ComputeWeights(order, progress, waypoint, knots, weights);
    weightedTotal = 0.0f;
    weightedZ = 0.0f;
    weightedX = 0.0f;

    for (i = 0; i < order; i++) {
        xPos = points[waypoint - order + i + 1].x;
        zPos = points[waypoint - order + i + 1].z;

        weightedX += weights[i] * xPos;
        weightedZ += weights[i] * zPos;
        weightedTotal += weights[i];
    }
    *x = weightedX / weightedTotal;
    *z = weightedZ / weightedTotal;
}

/**
 * Updates a time based path that an actor follows by:
 *  - Computing the X and Z components of the next point to move to
 *  - Updating the waypoint
 *  - Updating the time
 *
 * @param[in] path
 * @param[out] progress see SubS_TimePathing_ComputeProgress()
 * @param[in,out] elapsedTime how much time has passed
 * @param[in] waypointTime how much time per each waypoint
 * @param[in] totalTime how much time the path should take to travel
 * @param[in,out] waypoint the current waypoint, this and the previous two points will be used to compute the targetPos
 * @param[in] knots see SubS_TimePathing_FillKnots()
 * @param[out] targetPos the computed position to move to
 * @param[in] timeSpeed how fast time moves
 *
 * @return s32 returns true when the end has been reached.
 *
 * @note This system/function makes a couple of assumptions about the order used:
 *      1. the order is assumed to be 3, see SUBS_TIME_PATHING_ORDER
 *      2. even if SUBS_TIME_PATHING_ORDER is updated, the order can only be a max of 10
 */
s32 SubS_TimePathing_Update(Path* path, f32* progress, s32* elapsedTime, s32 waypointTime, s32 totalTime, s32* waypoint,
                            f32 knots[], Vec3f* targetPos, s32 timeSpeed) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 state;
    f32 endX;
    f32 endZ;
    s32 reachedEnd = false;

    if (*waypoint >= path->count) {
        state = SUBS_TIME_PATHING_PROGRESS_STATUS_SHOULD_REACH_END;
    } else {
        state = SubS_TimePathing_ComputeProgress(progress, *elapsedTime, waypointTime, totalTime, path->count,
                                                 SUBS_TIME_PATHING_ORDER, knots);
    }

    switch (state) {
        case SUBS_TIME_PATHING_PROGRESS_STATUS_STILL_ON_PATH:
            reachedEnd = false;
            SubS_TimePathing_ComputeTargetPosXZ(&targetPos->x, &targetPos->z, *progress, SUBS_TIME_PATHING_ORDER,
                                                *waypoint, points, knots);
            break;
        case SUBS_TIME_PATHING_PROGRESS_STATUS_SHOULD_REACH_END:
            endX = points[path->count - 1].x;
            endZ = points[path->count - 1].z;
            targetPos->x = endX * 1;
            targetPos->z = endZ * 1;
            reachedEnd = true;
            break;
    }

    *elapsedTime += timeSpeed;
    if (*elapsedTime >= totalTime) {
        *elapsedTime = totalTime;
    } else if (*elapsedTime < 0) {
        *elapsedTime = 0;
    }
    *waypoint = (*elapsedTime / waypointTime) + (SUBS_TIME_PATHING_ORDER - 1);

    return reachedEnd;
}

/**
 * Computes the initial Y component of a time based path
 *
 * @param[in] play
 * @param[in] path
 * @param[in] waypoint the current waypoint, this and the previous two points will be used to compute the target pos
 * @param[out] targetPos the computed position to move to, only the Y component has meaning
 *
 * @note Same note as SubS_TimePathing_Update()
 */
void SubS_TimePathing_ComputeInitialY(PlayState* play, Path* path, s32 waypoint, Vec3f* targetPos) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    Vec3f posA;
    Vec3f posB;
    Vec3f posResult;
    s32 i = waypoint - (SUBS_TIME_PATHING_ORDER - 1);
    s16 max;
    s16 min;
    s32 isSetup;
    CollisionPoly* outPoly = NULL;
    s32 bgId = 0;

    max = 0;
    min = 0;
    isSetup = false;
    for (; i <= waypoint; i++) {
        if (isSetup) {
            if (max < points[i].y) {
                max = points[i].y;
            }
            if (points[i].y < min) {
                min = points[i].y;
            }
        } else {
            max = min = points[i].y;
        }
        isSetup = true;
    }
    max += 30;
    min -= 30;
    posA = *targetPos;
    posB = *targetPos;
    posA.y = max;
    posB.y = min;
    if (BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &posResult, &outPoly, true, true, true, true, &bgId)) {
        targetPos->y = posResult.y;
    }
}

Path* SubS_GetAdditionalPath(PlayState* play, u8 pathIndex, s32 limit) {
    Path* path;
    s32 i = 0;

    do {
        path = &play->setupPathList[pathIndex];
        if (i >= limit) {
            break;
        }
        pathIndex = path->additionalPathIndex;
        i++;
    } while (pathIndex != ADDITIONAL_PATH_INDEX_NONE);

    return path;
}

/**
 * Finds the nearest actor instance of a specified Id and category to an actor.
 */
Actor* SubS_FindNearestActor(Actor* actor, PlayState* play, u8 actorCategory, s16 actorId) {
    Actor* actorIter = NULL;
    Actor* actorTmp;
    f32 dist;
    Actor* closestActor = NULL;
    f32 minDist = 99999.0f;
    s32 isSetup = false;

    do {
        actorIter = SubS_FindActor(play, actorIter, actorCategory, actorId);

        actorTmp = actorIter;
        if (actorTmp == NULL) {
            break;
        }
        actorIter = actorTmp;

        if (actorIter != actor) {
            dist = Actor_WorldDistXYZToActor(actor, actorIter);
            if (!isSetup || dist < minDist) {
                closestActor = actorIter;
                minDist = dist;
                isSetup = true;
            }
        }

        actorIter = actorIter->next;
    } while (actorIter != NULL);

    return closestActor;
}

s32 SubS_ChangeAnimationByInfoS(SkelAnime* skelAnime, AnimationInfoS* animationInfo, s32 animIndex) {
    s32 endFrame;
    s32 startFrame;

    animationInfo += animIndex;
    AnimationHeader* anim = animationInfo->animation;
    AnimationHeader* ogAnim = anim;

    if (ResourceMgr_OTRSigCheck(anim))
        anim = ResourceMgr_LoadAnimByName(anim);

    endFrame = animationInfo->frameCount;
    if (animationInfo->frameCount < 0) {
        endFrame = Animation_GetLastFrame(&anim->common);
    }
    startFrame = animationInfo->startFrame;
    if ((startFrame >= endFrame) || (startFrame < 0)) {
        return false;
    }
    if (animationInfo->playSpeed < 0.0f) {
        SWAP(s32, endFrame, startFrame);
    }
    Animation_Change(skelAnime, ogAnim, animationInfo->playSpeed, startFrame, endFrame, animationInfo->mode,
                     animationInfo->morphFrames);
    return true;
}

s32 SubS_HasReachedPoint(Actor* actor, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);
    if (((px * actor->world.pos.x) + (pz * actor->world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

Path* SubS_GetDayDependentPath(PlayState* play, u8 pathIndex, u8 pathIndexNone, s32* startPointIndex) {
    Path* path = NULL;
    s32 found = false;
    s16 time1 = TRUNCF_BINANG(TIME_TO_MINUTES_F(CURRENT_TIME));
    s16 time2 = TRUNCF_BINANG(TIME_TO_MINUTES_F(CURRENT_TIME));
    s32 time = ((time1 % 60) + (time2 / 60) * 60) / 30;
    s32 day = CURRENT_DAY;

    if (pathIndex == pathIndexNone) {
        return NULL;
    }

    while (pathIndex != ADDITIONAL_PATH_INDEX_NONE) {
        path = &play->setupPathList[pathIndex];
        if (path->customValue & sPathDayFlags[day]) {
            found = true;
            break;
        }
        pathIndex = path->additionalPathIndex;
    }

    if (found == true) {
        *startPointIndex = time;
        *startPointIndex = CLAMP(*startPointIndex, 0, path->count - 1);
    } else {
        *startPointIndex = 0;
    }

    return path;
}

/**
 * Computes the point to move toward using a weight based algorithm that considers 4 points along the path
 *
 * @param path
 * @param waypoint the current waypoint, this and the previous three points will be used to compute the point
 * @param point the point computed
 * @param progress the main weight value used to compute the weights for the points considered
 * @param direction the direciton along the path to move, 1 for forwards, anything else for backwards
 *
 * @note only computes X and Z components of the point
 */
s32 SubS_WeightPathing_ComputePoint(Path* path, s32 waypoint, Vec3f* point, f32 progress, s32 direction) {
    s32 i;
    f32 weight0;
    f32 weight1;
    f32 weight2;
    f32 weight3;
    s32 lastPoint;
    s32 secondLastPoint;
    s32 secondPoint;
    s32 firstPoint;
    f32 xPoints[4];
    f32 zPoints[4];
    f32 oneMinusProgress;
    f32 squared;
    f32 cubed;
    Vec3s* points;
    s32 count = path->count;
    s32 pointIndex;
    s32 tmp;

    if (path == NULL) {
        return false;
    }
    if (direction == 1) {
        if (waypoint <= 2) {
            pointIndex = 2;
        } else {
            pointIndex = (waypoint == 3) ? 3 : waypoint;
        }
        for (i = 0; i < 4; i++, pointIndex--) {
            if (pointIndex <= 0) {
                pointIndex = 0;
            }
            points = Lib_SegmentedToVirtual(path->points);
            points = &points[pointIndex];
            xPoints[i] = points->x;
            zPoints[i] = points->z;
        }
        lastPoint = count - 1;
        secondLastPoint = count - 2;
        secondPoint = 3;
        firstPoint = 2;
    } else {
        if (waypoint >= count - 3) {
            pointIndex = count - 3;
        } else {
            tmp = waypoint + 4;
            pointIndex = (count == tmp) ? count - 4 : waypoint;
        }
        for (i = 0; i < 4; i++, pointIndex++) {
            if (pointIndex >= path->count) {
                pointIndex = path->count - 1;
            }
            points = Lib_SegmentedToVirtual(path->points);
            points = &points[pointIndex];
            xPoints[i] = points->x;
            zPoints[i] = points->z;
        }
        lastPoint = 0;
        secondLastPoint = 1;
        secondPoint = count - 4;
        firstPoint = count - 3;
    }
    if (waypoint == lastPoint) {
        oneMinusProgress = 1.0f - progress;
        squared = progress * progress;
        cubed = progress * squared;
        weight0 = oneMinusProgress * oneMinusProgress * oneMinusProgress;
        weight1 = (1.75f * cubed) - (4.5f * squared) + (3.0f * progress);
        weight2 = ((-11.0f / 12.0f) * cubed) + (1.5f * squared);
        weight3 = (1.0f / 6.0f) * cubed;
    } else if (waypoint == secondLastPoint) {
        oneMinusProgress = 1.0f - progress;
        squared = progress * progress;
        cubed = progress * squared;
        weight0 = oneMinusProgress * oneMinusProgress * oneMinusProgress * ((void)0, 0.25f); //! FAKE:
        weight1 = ((7.0f / 12.0f) * cubed) - (1.25f * squared) + (0.25f * progress) + (7.0f / 12.0f);
        weight2 = (-0.5f * cubed) + (0.5f * squared) + (progress * 0.5f) + (1.0f / 6.0f);
        weight3 = cubed * (1.0f / 6.0f);
    } else if (waypoint == secondPoint) {
        oneMinusProgress = 1.0f - progress;
        squared = oneMinusProgress * oneMinusProgress;
        cubed = oneMinusProgress * squared;
        weight0 = (1.0f / 6.0f) * cubed;
        weight1 = (-0.5f * cubed) + (0.5f * squared) + (0.5f * oneMinusProgress) + (1.0f / 6.0f);
        weight2 = ((7.0f / 12.0f) * cubed) - (1.25f * squared) + (0.25f * oneMinusProgress) + (7.0f / 12.0f);
        weight3 = progress * progress * progress * 0.25f;
    } else if (((direction == 1) && (firstPoint >= waypoint)) || ((direction != 1) && (waypoint >= firstPoint))) {
        oneMinusProgress = 1.0f - progress;
        squared = oneMinusProgress * oneMinusProgress;
        cubed = oneMinusProgress * squared;
        weight0 = (1.0f / 6.0f) * cubed;
        weight1 = ((-11.0f / 12.0f) * cubed) + (1.5f * squared);
        weight2 = (1.75f * cubed) - (4.5f * squared) + (3.0f * oneMinusProgress);
        weight3 = progress * progress * progress;
    } else {
        oneMinusProgress = 1.0f - progress;
        squared = progress * progress;
        cubed = squared * progress;
        weight0 = oneMinusProgress * oneMinusProgress;
        weight0 = oneMinusProgress * weight0 / 6.0f;
        weight1 = (cubed * 0.5f) - squared + (2.0f / 3.0f);
        weight2 = (cubed / -2.0f) + (squared * 0.5f) + (progress * 0.5f) + (1.0f / 6.0f);
        weight3 = cubed / 6.0f;
    }
    point->x = (weight0 * xPoints[0]) + (weight1 * xPoints[1]) + (weight2 * xPoints[2]) + (weight3 * xPoints[3]);
    point->z = (weight0 * zPoints[0]) + (weight1 * zPoints[1]) + (weight2 * zPoints[2]) + (weight3 * zPoints[3]);

    return true;
}

// WeightPathing System is completely unused
/**
 * Moves an actor based on a weight based algorithm that takes into account 4 points along the path
 *
 * @param actor
 * @param path
 * @param waypoint the current waypoint, this and the previous three points will be used to move forward
 * @param progress the progress towards a given waypoint, used to compute the weights
 * @param direction the direction along the path to move, 1 for forwards, anything else for backwards
 * @param returnStart boolean, true if the actor should wrap back to start when reaching the end
 *
 * @return s32 true if actor reached the end of the path in this iteration, false otherwise
 */
s32 SubS_WeightPathing_Move(Actor* actor, Path* path, s32* waypoint, f32* progress, s32 direction, s32 returnStart) {
    Vec3f worldPos = actor->world.pos;
    Vec3f velocity = actor->velocity;
    Vec3f point;
    f32 dist;

    if (((direction != 1) && (*waypoint >= (path->count - 2))) || ((direction == 1) && (*waypoint < 2))) {
        return false;
    }
    while (true) {
        if (!SubS_WeightPathing_ComputePoint(path, *waypoint, &point, *progress, direction) ||
            ((s32)(actor->speed * 10000.0f) == 0)) {
            return false;
        }
        dist = Math_Vec3f_DistXZ(&actor->world.pos, &point);
        actor->world.rot.y = Math_Vec3f_Yaw(&actor->world.pos, &point);
        Actor_MoveWithGravity(actor);
        if (Math_Vec3f_DistXZ(&actor->world.pos, &point) < dist) {
            break;
        }
        *progress += 0.1f;
        if (*progress >= 1.1f) {
            if (direction != 1) {
                (*waypoint)++;
                if (*waypoint >= (path->count - 2)) {
                    if (returnStart) {
                        *waypoint = 0;
                    } else {
                        return true;
                    }
                }
            } else {
                (*waypoint)--;
                if (*waypoint < 2) {
                    if (returnStart) {
                        *waypoint = path->count - 2;
                    } else {
                        return true;
                    }
                }
            }
            *progress = 0.0f;
        }
        actor->world.pos = worldPos;
        actor->velocity = velocity;
    }
    return false;
}

s32 SubS_CopyPointFromPathCheckBounds(Path* path, s32 pointIndex, Vec3f* dst) {
    Vec3s* point;

    if ((path == NULL) || (pointIndex >= path->count) || (pointIndex < 0)) {
        return false;
    }

    point = Lib_SegmentedToVirtual(path->points);
    point = &point[pointIndex];
    dst->x = point->x;
    dst->y = point->y;
    dst->z = point->z;
    return true;
}

/**
 * Attempt to extend different offers to the player based on different checks
 * and on the provided mode (see SubSOfferMode).
 *
 * The offer types are either GetItem (see Actor_OfferGetItem) or TalkExchange (see Actor_OfferTalkExchange),
 * with more check variants provided for TalkExchange offers.
 *
 * @return `true` if offer was extended and the player can accept it
 */
s32 SubS_Offer(Actor* actor, PlayState* play, f32 xzRange, f32 yRange, s32 itemId, SubSOfferMode mode) {
    s32 canAccept = false;
    s16 screenPosX;
    s16 screenPosY;
    f32 xzDistToPlayerTemp;

    Actor_GetScreenPos(play, actor, &screenPosX, &screenPosY);

    switch (mode) {
        case SUBS_OFFER_MODE_GET_ITEM:
            yRange = fabsf(actor->playerHeightRel) + 1.0f;
            xzRange = actor->xzDistToPlayer + 1.0f;
            canAccept = Actor_OfferGetItem(actor, play, itemId, xzRange, yRange);
            break;

        case SUBS_OFFER_MODE_NEARBY:
            if ((fabsf(actor->playerHeightRel) <= yRange) && (actor->xzDistToPlayer <= xzRange)) {
                canAccept = Actor_OfferTalkExchange(actor, play, xzRange, yRange, itemId);
            }
            break;

        case SUBS_OFFER_MODE_ONSCREEN:
            //! @bug: Both x and y conditionals are always true, || should be an &&
            if (((screenPosX >= 0) || (screenPosX < SCREEN_WIDTH)) &&
                ((screenPosY >= 0) || (screenPosY < SCREEN_HEIGHT))) {
                canAccept = Actor_OfferTalkExchange(actor, play, xzRange, yRange, itemId);
            }
            break;

        case SUBS_OFFER_MODE_AUTO:
            yRange = fabsf(actor->playerHeightRel) + 1.0f;
            xzRange = actor->xzDistToPlayer + 1.0f;
            xzDistToPlayerTemp = actor->xzDistToPlayer;
            actor->xzDistToPlayer = 0.0f;
            actor->flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            canAccept = Actor_OfferTalkExchange(actor, play, xzRange, yRange, itemId);
            actor->xzDistToPlayer = xzDistToPlayerTemp;
            break;

        case SUBS_OFFER_MODE_AUTO_TARGETED:
            //! @bug: Both x and y conditionals are always true, || should be an &&
            if (((screenPosX >= 0) || (screenPosX < SCREEN_WIDTH)) &&
                ((screenPosY >= 0) || (screenPosY < SCREEN_HEIGHT)) && (fabsf(actor->playerHeightRel) <= yRange) &&
                (actor->xzDistToPlayer <= xzRange) && actor->isLockedOn) {
                actor->flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                canAccept = Actor_OfferTalkExchange(actor, play, xzRange, yRange, itemId);
            }
            break;

        case SUBS_OFFER_MODE_AUTO_NEARBY_ONSCREEN:
            //! @bug: Both x and y conditionals are always true, || should be an &&
            if (((screenPosX >= 0) || (screenPosX < SCREEN_WIDTH)) &&
                ((screenPosY >= 0) || (screenPosY < SCREEN_HEIGHT)) && (fabsf(actor->playerHeightRel) <= yRange) &&
                (actor->xzDistToPlayer <= xzRange)) {
                actor->flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                canAccept = Actor_OfferTalkExchange(actor, play, xzRange, yRange, itemId);
            }
            break;

        default:
            break;
    }

    return canAccept;
}

const u8 sShadowMaps[4][12][12] = {
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
    },
};

void SubS_FillShadowTex(s32 startCol, s32 startRow, u8* tex, s32 size) {
    s32 i;
    s32 j;
    s32 start;

    for (i = 0; i < 12; i++) {
        start = ((startRow + i) * 64) + startCol - 390;
        for (j = 0; j < 12; j++) {
            if (sShadowMaps[size][i][j] != 0) {
                if ((start + j >= 0) && (start + j < SUBS_SHADOW_TEX_SIZE)) {
                    tex[start + j] = 255;
                }
            }
        }
    }
}

void SubS_GenShadowTex(Vec3f bodyPartsPos[], Vec3f* worldPos, u8* tex, f32 weight, u8 bodyPartsNum, u8 sizes[],
                       s8 parentBodyParts[]) {
    Vec3f pos;
    Vec3f startVec;
    s32 i;
    s32 parentBodyPart;
    Vec3f* bodyPartPos;
    s32 startCol;
    s32 startRow;

    for (i = 0; i < bodyPartsNum; i++) {
        if (parentBodyParts[i] > BODYPART_NONE) {
            parentBodyPart = parentBodyParts[i];
            bodyPartPos = &bodyPartsPos[i];

            pos.x = (bodyPartsPos[parentBodyPart].x - bodyPartPos->x) * weight + (bodyPartPos->x - worldPos->x);
            pos.y = (bodyPartsPos[parentBodyPart].y - bodyPartPos->y) * weight + (bodyPartPos->y - worldPos->y);
            pos.z = (bodyPartsPos[parentBodyPart].z - bodyPartPos->z) * weight + (bodyPartPos->z - worldPos->z);
        } else {
            bodyPartPos = &bodyPartsPos[i];

            pos.x = bodyPartPos->x - worldPos->x;
            pos.y = bodyPartPos->y - worldPos->y;
            pos.z = bodyPartPos->z - worldPos->z;
        }

        Matrix_MultVec3f(&pos, &startVec);
        startCol = 64.0f + startVec.x;
        startRow = 64.0f - startVec.z;
        SubS_FillShadowTex(startCol >> 1, startRow >> 1, tex, sizes[i]);
    }
}

void SubS_DrawShadowTex(Actor* actor, GameState* gameState, u8* tex) {
    s32 pad;
    GraphicsContext* gfxCtx = gameState->gfxCtx;

    OPEN_DISPS(gfxCtx);

    gSPInvalidateTexCache(POLY_OPA_DISP++, tex);

    Gfx_SetupDL25_Opa(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 100);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    Matrix_Translate(actor->world.pos.x, 0.0f, actor->world.pos.z, MTXMODE_NEW);
    Matrix_Scale(0.6f, 1.0f, 0.6f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gShadowMaterialDL);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_I, G_IM_SIZ_8b, SUBS_SHADOW_TEX_WIDTH, SUBS_SHADOW_TEX_HEIGHT, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, gShadowModelDL);

    CLOSE_DISPS(gfxCtx);
}

/**
 * Computes the rotation based on the options and target rotation value
 *
 * @param[in,out] rot the computed rotation
 * @param[in] rotMax the max rotation in binary angles
 * @param[in] target the target rotation value
 * @param[in] slowness how slow to rotate, the larger the number the slower the rotation, cannot be 0
 * @param[in] stepMin the minimun step in degrees
 * @param[in] stepMax the maximum step in degrees
 */
s16 SubS_ComputeTrackPointRot(s16* rot, s16 rotMax, s16 target, f32 slowness, f32 stepMin, f32 stepMax) {
    s16 prevRot = *rot;
    f32 step;
    f32 prevRotStep;

    step = BINANG_TO_DEG(target - *rot);
    step *= gFramerateDivisorHalf;
    prevRotStep = step;
    if (step >= 0.0f) {
        step /= slowness;
        step = CLAMP(step, stepMin, stepMax);
        *rot += DEG_TO_BINANG_ALT2(step);
        if (prevRotStep < stepMin) {
            *rot = target;
        }
        if (rotMax != 0) {
            *rot = CLAMP(*rot, -rotMax, rotMax);
        }
    } else {
        step = (step / slowness) * -1.0f;
        step = CLAMP(step, stepMin, stepMax);
        *rot -= DEG_TO_BINANG_ALT2(step);
        if (-stepMin < prevRotStep) {
            *rot = target;
        }
        if (rotMax != 0) {
            *rot = CLAMP(*rot, -rotMax, rotMax);
        }
    }

    return prevRot - *rot;
}

/**
 * Computes the necessary HeadRot and TorsoRot to smoothly turn an actors's head and torso to a point
 *
 * @param[in] target the point to turn to
 * @param[in] focusPos the actor's focus postion
 * @param[in] shapeRot the actor's shape rotation
 * @param[in,out] trackTarget the intermediate target step that headRot and torsoRot step towards
 * @param[in,out] headRot the computed head rotation
 * @param[in,out] torsoRot the computed torso rotation
 * @param[in] options various options to adjust how the actor turns, see SubS_ComputeTrackPointRot()
 */
s32 SubS_TrackPoint(Vec3f* target, Vec3f* focusPos, Vec3s* shapeRot, Vec3s* trackTarget, Vec3s* headRot,
                    Vec3s* torsoRot, TrackOptionsSet* options) {
    s16 pitch;
    s16 yaw;
    s16 pad;
    s16 targetY;
    f32 diffX = target->x - focusPos->x;
    s16 targetX;
    f32 diffZ = target->z - focusPos->z;

    yaw = Math_Atan2S_XY(diffZ, diffX);
    pitch = Math_Atan2S_XY(sqrtf(SQ(diffX) + SQ(diffZ)), target->y - focusPos->y);
    Math_SmoothStepToS(&trackTarget->x, pitch, 4, 0x2710, 0);
    Math_SmoothStepToS(&trackTarget->y, yaw, 4, 0x2710, 0);

    targetX =
        SubS_ComputeTrackPointRot(&headRot->x, options->headRotX.rotMax, trackTarget->x, options->headRotX.slowness,
                                  options->headRotX.rotStepMin, options->headRotX.rotStepMax);
    //! @bug: torsoRotX uses headRotX slowness
    SubS_ComputeTrackPointRot(&torsoRot->x, options->torsoRotX.rotMax, targetX, options->headRotX.slowness,
                              options->torsoRotX.rotStepMin, options->torsoRotX.rotStepMax);

    targetY = trackTarget->y - shapeRot->y;
    SubS_ComputeTrackPointRot(&headRot->y, options->headRotY.rotMax, targetY - torsoRot->y, options->headRotY.slowness,
                              options->headRotY.rotStepMin, options->headRotY.rotStepMax);
    SubS_ComputeTrackPointRot(&torsoRot->y, options->torsoRotY.rotMax, targetY - headRot->y,
                              options->torsoRotY.slowness, options->torsoRotY.rotStepMin,
                              options->torsoRotY.rotStepMax);

    return true;
}

s32 SubS_AngleDiffLessEqual(s16 angleA, s16 threshold, s16 angleB) {
    return (ABS_ALT(BINANG_SUB(angleB, angleA)) <= threshold) ? true : false;
}

Path* SubS_GetPathByIndex(PlayState* play, s16 pathIndex, s16 pathIndexNone) {
    return (pathIndex != pathIndexNone) ? &play->setupPathList[pathIndex] : NULL;
}

s32 SubS_CopyPointFromPath(Path* path, s32 pointIndex, Vec3f* dst) {
    Vec3s* point;

    if (path == NULL) {
        return false;
    }

    point = Lib_SegmentedToVirtual(path->points);
    point = &point[pointIndex];
    dst->x = point->x;
    dst->y = point->y;
    dst->z = point->z;

    return true;
}

s16 SubS_GetDistSqAndOrientPoints(Vec3f* vecA, Vec3f* vecB, f32* distSq) {
    f32 diffX = vecA->x - vecB->x;
    f32 diffZ = vecA->z - vecB->z;

    *distSq = SQ(diffX) + SQ(diffZ);
    return Math_Atan2S(diffX, diffZ);
}

/**
 * Returns true when the actor has reached the inputed point
 */
s32 SubS_MoveActorToPoint(Actor* actor, Vec3f* point, s16 rotStep) {
    Vec3f offsetBefore;
    Vec3f offsetAfter;
    f32 distSqBefore;
    f32 distSqAfter;

    Actor_WorldToActorCoords(actor, &offsetBefore, point);
    Math_SmoothStepToS(&actor->world.rot.y, SubS_GetDistSqAndOrientPoints(point, &actor->world.pos, &distSqBefore), 4,
                       rotStep, 1);
    actor->shape.rot.y = actor->world.rot.y;
    Actor_MoveWithGravity(actor);
    Actor_WorldToActorCoords(actor, &offsetAfter, point);
    SubS_GetDistSqAndOrientPoints(point, &actor->world.pos, &distSqAfter);
    return ((offsetBefore.z > 0.0f) && (offsetAfter.z <= 0.0f)) ? true : false;
}

s16 SubS_GetDistSqAndOrientPath(Path* path, s32 pointIndex, Vec3f* pos, f32* distSq) {
    Vec3s* point;
    f32 diffX = 0.0f;
    f32 diffZ = 0.0f;

    if (path != NULL) {
        point = Lib_SegmentedToVirtual(path->points);
        point = &point[pointIndex];
        diffX = point->x - pos->x;
        diffZ = point->z - pos->z;
    }

    *distSq = SQ(diffX) + SQ(diffZ);
    return Math_Atan2S(diffX, diffZ);
}

s8 SubS_IsObjectLoaded(s8 objectSlot, PlayState* play) {
    return !Object_IsLoaded(&play->objectCtx, objectSlot) ? false : true;
}

s8 SubS_GetObjectSlot(s16 objectId, PlayState* play) {
    return Object_GetSlot(&play->objectCtx, objectId);
}

/**
 * Finds the first actor instance of a specified Id and category.
 */
Actor* SubS_FindActor(PlayState* play, Actor* actorListStart, u8 actorCategory, s16 actorId) {
    Actor* actor = actorListStart;

    if (actor == NULL) {
        actor = play->actorCtx.actorLists[actorCategory].first;
    }

    while ((actor != NULL) && (actorId != actor->id)) {
        actor = actor->next;
    }

    return actor;
}

/**
 * Fills two tables with rotation angles that can be used to simulate idle animations.
 *
 * The rotation angles are dependent on the current frame, so should be updated regularly, generally every frame.
 *
 * This is done for the desired limb by taking either the `sin` of the yTable value or the `cos` of the zTable value,
 * multiplying by some scale factor (generally 200), and adding that to the already existing rotation.
 *
 * Note: With the common scale factor of 200, this effect is practically unnoticeable if the current animation already
 * has motion involved.
 */
s32 SubS_UpdateFidgetTables(PlayState* play, s16* fidgetTableY, s16* fidgetTableZ, s32 tableLen) {
    u32 frames = play->gameplayFrames;
    s32 i;

    for (i = 0; i < tableLen; i++) {
        fidgetTableY[i] = (i * 50 + 0x814) * frames;
        fidgetTableZ[i] = (i * 50 + 0x940) * frames;
    }

    return true;
}

s32 SubS_IsFloorAbove(PlayState* play, Vec3f* pos, f32 distAbove) {
    CollisionPoly* outPoly;
    Vec3f posA;
    Vec3f posB;
    Vec3f posResult;
    s32 bgId;

    posA = posB = *pos;
    posB.y += distAbove;
    return BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, &posResult, &outPoly, false, true, false, true, &bgId);
}

s32 SubS_CopyPointFromPathList(Path* paths, s32 pathIndex, s32 pointIndex, Vec3f* dst) {
    Path* path = &paths[pathIndex];
    Vec3s* point = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[pointIndex];

    dst->x = point->x;
    dst->y = point->y;
    dst->z = point->z;

    return false;
}

u8 SubS_GetPathCountFromPathList(Path* paths, s32 pathIndex) {
    Path* path = &paths[pathIndex];

    return path->count;
}

void SubS_ActorPathing_Init(PlayState* play, Vec3f* worldPos, Actor* actor, ActorPathing* actorPath, Path* paths,
                            s32 pathIndex, s32 begPointIndex, s32 endPointIndex, s32 curPointIndex, u8 flags) {
    Path* path;

    actorPath->setupPathList = play->setupPathList;
    actorPath->pathIndex = pathIndex;
    path = &paths[pathIndex];
    actorPath->points = Lib_SegmentedToVirtual(path->points);
    actorPath->count = path->count;
    actorPath->begPointIndex = begPointIndex;
    if (endPointIndex == 0) {
        actorPath->endPointIndex = actorPath->count - 1;
    } else if (endPointIndex > 0) {
        actorPath->endPointIndex = endPointIndex;
    } else {
        //! @bug: endPointIndex is negative, subtraction causes result to be past the end
        actorPath->endPointIndex = (actorPath->count - endPointIndex) - 1;
    }
    actorPath->curPointIndex = curPointIndex;
    actorPath->curPoint.x = actorPath->points[0].x;
    actorPath->curPoint.y = actorPath->points[0].y;
    actorPath->curPoint.z = actorPath->points[0].z;
    Math_Vec3f_Copy(&actorPath->prevPoint, &actorPath->curPoint);
    actorPath->worldPos = worldPos;
    actorPath->actor = actor;
    actorPath->flags = flags;
    actorPath->prevFlags = flags;
}

s32 SubS_ActorPathing_Update(PlayState* play, ActorPathing* actorPath, ActorPathingComputeFunc computePointInfoFunc,
                             ActorPathingUpdateFunc updateActorInfoFunc, ActorPathingUpdateFunc moveFunc,
                             ActorPathingUpdateFunc setNextPointFunc) {
    s32 shouldSetNextPoint;
    s32 reupdate;

    actorPath->computePointInfoFunc = computePointInfoFunc;
    actorPath->updateActorInfoFunc = updateActorInfoFunc;
    actorPath->moveFunc = moveFunc;
    actorPath->setNextPointFunc = setNextPointFunc;
    actorPath->flags &= ~ACTOR_PATHING_REACHED_TEMPORARY;
    reupdate = false;
    if (actorPath->flags & ACTOR_PATHING_MOVE_BACKWARDS) {
        if (!(actorPath->prevFlags & ACTOR_PATHING_MOVE_BACKWARDS)) {
            actorPath->curPointIndex--;
        }
    } else if (actorPath->prevFlags & ACTOR_PATHING_MOVE_BACKWARDS) {
        actorPath->curPointIndex++;
    }
    do {
        shouldSetNextPoint = false;
        if (actorPath->computePointInfoFunc != NULL) {
            actorPath->computePointInfoFunc(play, actorPath);
        }
        if (actorPath->updateActorInfoFunc != NULL) {
            shouldSetNextPoint = actorPath->updateActorInfoFunc(play, actorPath);
        }
        if (shouldSetNextPoint) {
            if (actorPath->setNextPointFunc != NULL) {
                reupdate = actorPath->setNextPointFunc(play, actorPath);
            }
        } else if (actorPath->moveFunc != NULL) {
            reupdate = actorPath->moveFunc(play, actorPath);
        }
    } while (reupdate);
    actorPath->prevFlags = actorPath->flags;
    return false;
}

void SubS_ActorPathing_ComputePointInfo(PlayState* play, ActorPathing* actorPath) {
    Vec3f diff;

    actorPath->curPoint.x = actorPath->points[actorPath->curPointIndex].x + actorPath->pointOffset.x;
    actorPath->curPoint.y = actorPath->points[actorPath->curPointIndex].y + actorPath->pointOffset.y;
    actorPath->curPoint.z = actorPath->points[actorPath->curPointIndex].z + actorPath->pointOffset.z;
    diff.x = actorPath->curPoint.x - actorPath->worldPos->x;
    diff.y = actorPath->curPoint.y - actorPath->worldPos->y;
    diff.z = actorPath->curPoint.z - actorPath->worldPos->z;
    actorPath->distSqToCurPointXZ = Math3D_Dist1DSq(diff.x, diff.z);
    actorPath->distSqToCurPoint = Math3D_Vec3fMagnitudeSq(&diff);
    actorPath->rotToCurPoint.y = Math_Atan2S_XY(diff.z, diff.x);
    actorPath->rotToCurPoint.x = Math_Atan2S_XY(sqrtf(actorPath->distSqToCurPointXZ), -diff.y);
    actorPath->rotToCurPoint.z = 0;
}

s32 SubS_ActorPathing_MoveWithGravity(PlayState* play, ActorPathing* actorPath) {
    Actor_MoveWithGravity(actorPath->actor);
    return false;
}

s32 SubS_ActorPathing_MoveWithoutGravityReverse(PlayState* play, ActorPathing* actorPath) {
    Actor_MoveWithoutGravityReverse(actorPath->actor);
    return false;
}

s32 SubS_ActorPathing_SetNextPoint(PlayState* play, ActorPathing* actorPath) {
    s32 reupdate = true;

    Math_Vec3f_Copy(&actorPath->prevPoint, &actorPath->curPoint);
    if (!(actorPath->flags & ACTOR_PATHING_MOVE_BACKWARDS)) {
        if (actorPath->curPointIndex >= actorPath->endPointIndex) {
            if (actorPath->flags & ACTOR_PATHING_RETURN_TO_START) {
                actorPath->curPointIndex = actorPath->begPointIndex;
            } else if (actorPath->flags & ACTOR_PATHING_SWITCH_DIRECTION) {
                actorPath->flags |= ACTOR_PATHING_MOVE_BACKWARDS;
            } else {
                reupdate = false;
            }
            actorPath->flags |= ACTOR_PATHING_REACHED_END;
        } else {
            actorPath->curPointIndex++;
        }
        actorPath->flags |= ACTOR_PATHING_REACHED_POINT;
    } else {
        if (actorPath->begPointIndex >= actorPath->curPointIndex) {
            if (actorPath->flags & ACTOR_PATHING_RETURN_TO_START) {
                actorPath->curPointIndex = actorPath->endPointIndex;
            } else if (actorPath->flags & ACTOR_PATHING_SWITCH_DIRECTION) {
                actorPath->flags &= ~ACTOR_PATHING_MOVE_BACKWARDS;
            } else {
                reupdate = false;
            }
            actorPath->flags |= ACTOR_PATHING_REACHED_END;
        } else {
            actorPath->curPointIndex--;
        }
    }
    actorPath->flags |= ACTOR_PATHING_REACHED_POINT;
    return reupdate;
}

void SubS_ChangeAnimationBySpeedInfo(SkelAnime* skelAnime, AnimationSpeedInfo* animationInfo, s32 nextAnimIndex,
                                     s32* curAnimIndex) {
    AnimationSpeedInfo* animation = &animationInfo[nextAnimIndex];
    f32 startFrame = skelAnime->curFrame;
    f32 endFrame;
    f32 morphFrames;

    AnimationHeader* anim = animation->animation;
    AnimationHeader* ogAnim = anim;

    if (ResourceMgr_OTRSigCheck(anim))
        anim = ResourceMgr_LoadAnimByName(anim);

    if ((*curAnimIndex < 0) || (nextAnimIndex == *curAnimIndex)) {
        morphFrames = 0.0f;
        if (*curAnimIndex < 0) {
            startFrame = 0.0f;
        }
    } else {
        morphFrames = animation->morphFrames;
        if (nextAnimIndex != *curAnimIndex) {
            startFrame = 0.0f;
        }
    }
    if (animation->playSpeed >= 0.0f) {
        endFrame = Animation_GetLastFrame(&anim->common);
    } else {
        startFrame = Animation_GetLastFrame(&anim->common);
        endFrame = 0.0f;
    }
    Animation_Change(skelAnime, ogAnim, animation->playSpeed, startFrame, endFrame, animation->mode, morphFrames);
    *curAnimIndex = nextAnimIndex;
}

s32 SubS_StartCutscene(Actor* actor, s16 nextCsId, s16 curCsId, s32 type) {
    s32 isStarted = false;

    if ((curCsId != CS_ID_NONE) && (CutsceneManager_GetCurrentCsId() == curCsId)) {
        CutsceneManager_Stop(curCsId);
        CutsceneManager_Queue(nextCsId);
    } else if (CutsceneManager_IsNext(nextCsId)) {
        switch (type) {
            case SUBS_CUTSCENE_WITH_PLAYER:
                CutsceneManager_StartWithPlayerCs(nextCsId, actor);
                break;

            case SUBS_CUTSCENE_NORMAL:
                CutsceneManager_Start(nextCsId, actor);
                break;

            case SUBS_CUTSCENE_WITH_PLAYER_SET_FLAG:
                CutsceneManager_StartWithPlayerCsAndSetFlag(nextCsId, actor);
                break;

            default:
                break;
        }
        isStarted = true;
    } else {
        CutsceneManager_Queue(nextCsId);
    }

    return isStarted;
}

s32 SubS_FillCutscenesList(Actor* actor, s16 csIdList[], s16 numCutscenes) {
    s16 csId;
    s32 i;

    for (i = 0; i < numCutscenes; i++) {
        csIdList[i] = CS_ID_NONE;
    }

    csId = actor->csId;
    i = 0;

    while (csId != CS_ID_NONE) {
        // Note: Infinite loop if numCutscenes is less than possible additional csIdList
        if (i < numCutscenes) {
            csIdList[i] = csId;
            csId = CutsceneManager_GetAdditionalCsId(csId);
            i++;
        }
    }

    return i;
}

/**
 * Computes a plane based on a point on the plane, a unit vector and two angles
 *
 * @param[in] point a point on the plane
 * @param[in] unitVec the unit vector rotated that becomes the plane's normal
 * @param[in] rot the angles to rotate with, uses just the x and y components
 * @param[out] plane the computed plane
 *
 * @note the unit input vector is expected to already be normalized (only uses are with the z unit vector)
 */
void SubS_ConstructPlane(Vec3f* point, Vec3f* unitVec, Vec3s* rot, Plane* plane) {
    f32 sin;
    f32 cos;
    f32 temp;
    f32 unitVecZ;
    f32 normY;
    f32 unitVecYX;

    sin = Math_SinS(-rot->x);
    cos = Math_CosS(-rot->x);
    unitVecZ = unitVec->z;
    unitVecYX = unitVec->y;

    // Apply a rotation by -x about the X axis
    temp = (unitVecZ * cos) - (unitVecYX * sin);
    normY = (unitVecZ * sin) + (unitVecYX * cos);

    sin = Math_SinS(rot->y);
    cos = Math_CosS(rot->y);
    unitVecYX = unitVec->x;
    plane->normal.y = normY;

    // Apply a rotation by y about the Y axis
    plane->normal.z = (temp * cos) - (unitVecYX * sin);
    plane->normal.x = (temp * sin) + (unitVecYX * cos);

    plane->originDist = -((point->x * plane->normal.x) + (plane->normal.y * point->y) + (plane->normal.z * point->z));
}

s32 SubS_LineSegVsPlane(Vec3f* point, Vec3s* rot, Vec3f* unitVec, Vec3f* linePointA, Vec3f* linePointB,
                        Vec3f* intersect) {
    s32 lineSegVsPlane;
    Plane plane;

    SubS_ConstructPlane(point, unitVec, rot, &plane);
    lineSegVsPlane = Math3D_LineSegVsPlane(plane.normal.x, plane.normal.y, plane.normal.z, plane.originDist, linePointA,
                                           linePointB, intersect, false);

    return lineSegVsPlane ? true : false;
}

/**
 * Finds the first actor instance of a specified Id and category verified with a custom callback.
 * The callback should return `true` when the actor is successfully verified.
 */
Actor* SubS_FindActorCustom(PlayState* play, Actor* actor, Actor* actorListStart, u8 actorCategory, s16 actorId,
                            void* verifyData, VerifyFindActorFunc verifyActorFunc) {
    Actor* actorIter = actorListStart;

    if (actorListStart == NULL) {
        actorIter = play->actorCtx.actorLists[actorCategory].first;
    }

    while ((actorIter != NULL) &&
           ((actorId != actorIter->id) ||
            ((actorId == actorIter->id) &&
             ((verifyActorFunc == NULL) ||
              ((verifyActorFunc != NULL) && !verifyActorFunc(play, actor, actorIter, verifyData)))))) {
        actorIter = actorIter->next;
    }

    return actorIter;
}

/**
 * Will extend a TalkExchange offer to the player if the actor is verified with a custom callback.
 * The callback should return `true` when the actor is successfully verified.
 */
s32 SubS_OfferTalkExchangeCustom(Actor* actor, PlayState* play, f32 xzRange, f32 yRange, s32 exchangeItemAction,
                                 void* data, VerifyTalkExchangeActorFunc verifyActorFunc) {
    s32 canAccept = false;

    if ((verifyActorFunc == NULL) || ((verifyActorFunc != NULL) && verifyActorFunc(play, actor, data))) {
        canAccept = Actor_OfferTalkExchange(actor, play, xzRange, yRange, exchangeItemAction);
    }
    return canAccept;
}

s32 SubS_ArePlayerAndActorFacing(PlayState* play, Actor* actor, void* data) {
    Player* player = GET_PLAYER(play);
    Vec3s* yawRanges = (Vec3s*)data;
    s16 playerYaw = ABS(BINANG_SUB(Actor_WorldYawTowardActor(&player->actor, actor), player->actor.shape.rot.y));
    s16 actorYaw = ABS(BINANG_SUB(actor->yawTowardsPlayer, actor->shape.rot.y));
    s32 areFacing = false;
    s32 actorYawRange = ABS(yawRanges->y);
    s32 playerYawRange;

    if (actorYaw < (s16)actorYawRange) {
        playerYawRange = ABS(yawRanges->x);
        if (playerYaw < (s16)playerYawRange) {
            areFacing = true;
        }
    }

    return areFacing;
}

s32 SubS_OfferTalkExchangeFacing(Actor* actor, PlayState* play, f32 xzRange, f32 yRange, s32 exchangeItemAction,
                                 s16 playerYawRange, s16 actorYawRange) {
    Vec3s yawRanges;

    yawRanges.x = playerYawRange;
    yawRanges.y = actorYawRange;
    return SubS_OfferTalkExchangeCustom(actor, play, xzRange, yRange, exchangeItemAction, &yawRanges,
                                        SubS_ArePlayerAndActorFacing);
}

/**
 * Computes the necessary HeadRot and TorsoRot steps to be added to the normal rotation to smoothly turn an actors's
 * head and torso to a point
 *
 * @param[in] worldPos the actor's world position
 * @param[in] focusPos the actor's focus position
 * @param[in] shapeYRot the actor's shape's Y rotation
 * @param[in] yawTarget the target point to determine desired yaw
 * @param[in] pitchTarget the target point to determine desired pitch
 * @param[in,out] headZRotStep the computed actors' head's Z rotation step
 * @param[in,out] headXRotStep the computed actors' head's X rotation step
 * @param[in,out] torsoZRotStep the computed actors' torso's Z rotation step
 * @param[in,out] torsoXRotStep the computed actors' torso's X rotation step
 * @param[in] headZRotStepMax the max head's Z rotation step
 * @param[in] headXRotStepMax the max head's X rotation step
 * @param[in] torsoZRotStepMax the max torso's Z rotation step
 * @param[in] torsoXRotStepMax the max torso's X rotation step
 */
s32 SubS_TrackPointStep(Vec3f* worldPos, Vec3f* focusPos, s16 shapeYRot, Vec3f* yawTarget, Vec3f* pitchTarget,
                        s16* headZRotStep, s16* headXRotStep, s16* torsoZRotStep, s16* torsoXRotStep,
                        u16 headZRotStepMax, u16 headXRotStepMax, u16 torsoZRotStepMax, u16 torsoXRotStepMax) {
    s16 yaw = Math_Vec3f_Yaw(worldPos, yawTarget) - shapeYRot;
    s16 pad;
    s16 pad2;
    s16 pitch = Math_Vec3f_Pitch(focusPos, pitchTarget);

    if (BINANG_ADD(headXRotStepMax, torsoXRotStepMax) >= (s16)ABS(yaw)) {
        Math_ApproachS(headXRotStep, yaw - *torsoXRotStep, 4, 0x2AA8);
        *headXRotStep = CLAMP(*headXRotStep, -headXRotStepMax, headXRotStepMax);
        Math_ApproachS(torsoXRotStep, yaw - *headXRotStep, 4, 0x2AA8);
        *torsoXRotStep = CLAMP(*torsoXRotStep, -torsoXRotStepMax, torsoXRotStepMax);
    } else {
        Math_ApproachS(headXRotStep, 0, 4, 0x2AA8);
        Math_ApproachS(torsoXRotStep, 0, 4, 0x2AA8);
    }

    if (BINANG_ADD(headZRotStepMax, torsoZRotStepMax) >= (s16)ABS(pitch)) {
        Math_ApproachS(headZRotStep, pitch - *torsoZRotStep, 4, 0x2AA8);
        *headZRotStep = CLAMP(*headZRotStep, -headZRotStepMax, headZRotStepMax);
        Math_ApproachS(torsoZRotStep, pitch - *headZRotStep, 4, 0x2AA8);
        *torsoZRotStep = CLAMP(*torsoZRotStep, -torsoZRotStepMax, torsoZRotStepMax);
    } else {
        Math_ApproachS(headZRotStep, 0, 4, 0x2AA8);
        Math_ApproachS(torsoZRotStep, 0, 4, 0x2AA8);
    }

    return true;
}

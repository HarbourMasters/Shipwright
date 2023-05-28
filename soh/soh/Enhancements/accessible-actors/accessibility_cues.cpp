/**
* Accessibility audio cues, originally written by dcvz
*/
#include <functions.h>
#include "ActorAccessibility.h"
#include <macros.h>
#include <variables.h>
static Vec3f sLedgePosition;
static f32 sLedgeCuePitch;
static f32 sLedgeCueVolume;
static bool sLedgeCueIsActive;
static bool sLedgeCueIsPlaying;
static s32 sLedgeAlarmFrameOffset;

void accessible_va_ledge_cue(AccessibleActor* actor) {
    Vec3f vecA;
    Vec3f vecB;
    Vec3f vecResult;
    CollisionPoly* outPoly;
    f32 dist = FLT_MAX;
    PlayState* play = actor->play;
    Player* player = GET_PLAYER(play);
    bool isPlayerOnFloor = player->actor.bgCheckFlags & 1;
    bool isPlayerAiming = !!func_8002DD78(player);
    bool isPlayerPushingBlock = player->stateFlags2 & PLAYER_STATE2_0;
    bool isPlayerClimbing = player->stateFlags1 & PLAYER_STATE1_21;
    bool isPlayerHanging = player->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14);
    bool isPlayerRidingHorse = player->stateFlags1 & (PLAYER_STATE1_23);
    s16 prevYaw = player->actor.world.rot.y;

    sLedgeCueIsActive = false;

    if (isPlayerOnFloor && !isPlayerAiming && !isPlayerRidingHorse && !isPlayerHanging && !isPlayerClimbing &&
        !Player_InBlockingCsMode(play, player)) {
        const int numHeightSamples = 5;
        const f32 maxCheckAngle = 60.0f / 180.0f * M_PI;
        const f32 ledgeHeightAlarmThreshold = 25.0f;
        const f32 ledgeWarnAheadFrames = 20.0f;

        Vec3f vecStep;
        f32 linearVelocity;

        if (player->linearVelocity > 0.0f) {
            linearVelocity = player->linearVelocity;
        } else {
            const f32 hypotheticalCheckSpeed = 2.5f;
            linearVelocity = hypotheticalCheckSpeed;
        }
        vecStep.x = Math_SinS(player->currentYaw);
        vecStep.z = Math_CosS(player->currentYaw);
        vecStep.y = 0.0f;
        Math_Vec3f_Scale(&vecStep, linearVelocity * ledgeWarnAheadFrames / numHeightSamples);

        f32 stepLength = Math3D_Vec3fMagnitude(&vecStep);
        f32 checkHeight = stepLength * Math_FTanF(maxCheckAngle);

        vecResult = player->actor.world.pos;
        f32 prevHeight = vecResult.y;

        for (int i = 0; i < numHeightSamples; i++) {
            Math_Vec3f_Sum(&vecResult, &vecStep, &vecA);

            vecA.y += 20.0f;
            vecB.y += 20.0f;
            if (BgCheck_AnyLineTest2(&play->colCtx, &vecResult, &vecA, &vecB, &outPoly, true, false, false, 1)) {
                // don't check past walls
                break;
            }
            vecA.y -= 20.0f;
            vecB.y -= 20.0f;
            vecB = vecA;
            vecA.y += checkHeight;
            vecB.y -= checkHeight;

            if (!BgCheck_AnyLineTest2(&play->colCtx, &vecA, &vecB, &vecResult, &outPoly, true, true, false, 1) ||
                prevHeight - vecResult.y > ledgeHeightAlarmThreshold) {

                if (BgCheck_AnyLineTest2(&play->colCtx, &player->actor.world.pos, &vecA, &vecResult, &outPoly, true,
                                         false, false, 1)) {
                    // one last check to prevent some edge cases where walls aren't detected
                    break;
                }

                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &player->actor.world.pos, &vecA);
                vecB.y = player->actor.world.pos.y;
                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &vecB, &vecResult);
                Math_Vec3f_Diff(&vecResult, &vecA, &sLedgePosition);

                if (player->linearVelocity > 0.1f) {
                    sLedgeCuePitch = 2.2f;
                    Audio_PlaySoundGeneral(NA_SE_IT_ARROW_STICK_HRAD, &sLedgePosition, 4, &sLedgeCuePitch, &D_801333E0,
                                           &D_801333E8);
                } else {
                    sLedgeCuePitch = 0.7f;
                    sLedgeCueVolume = LERP(0.1f, 1.0f, Math_Vec3f_DistXZ(&vecResult, &vecA));
                    sLedgeCueIsActive = true;
                }

                break;
            }

            if (vecResult.y < prevHeight) {
                prevHeight = vecResult.y;
            }
        }
    }

    if (sLedgeCueIsActive && (!sLedgeCueIsPlaying || play->gameplayFrames % 20 == sLedgeAlarmFrameOffset)) {
        Audio_PlaySoundGeneral(NA_SE_PL_HOBBERBOOTS_LV, &sLedgePosition, 4, &sLedgeCuePitch, &D_801333E0, &D_801333E8);
        sLedgeCueIsPlaying = true;
        sLedgeAlarmFrameOffset = play->gameplayFrames % 20;
    } else if (sLedgeCueIsPlaying && !sLedgeCueIsActive) {
        Audio_StopSfxById(NA_SE_PL_HOBBERBOOTS_LV);
        sLedgeCueIsPlaying = false;
    }
}

static Vec3f sWallCuePosition;
static f32 sWallCuePitch;
static f32 sWallCueVolume;
static bool sWallCueIsPlaying;

void accessible_va_wall_cue(AccessibleActor* actor) {
    Vec3f vecA;
    Vec3f vecB;
    Vec3f vecResult;
    CollisionPoly* outPoly;
    f32 dist = FLT_MAX;
    PlayState* play = actor->play;
    Player* player = GET_PLAYER(actor->play);
    bool isPlayerOnFloor = player->actor.bgCheckFlags & 1;
    bool isPlayerAiming = !!func_8002DD78(player);
    bool isPlayerPushingBlock = player->stateFlags2 & PLAYER_STATE2_0;
    bool isPlayerClimbing = player->stateFlags1 & PLAYER_STATE1_21;
    bool isPlayerHanging = player->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14);
    bool isPlayerRidingHorse = player->stateFlags1 & (PLAYER_STATE1_23);
    s16 prevYaw = player->actor.world.rot.y;

    if (!isPlayerHanging && !isPlayerPushingBlock && !isPlayerAiming && !Player_InBlockingCsMode(play, player)) {
        if (isPlayerClimbing && player->actor.wallPoly != NULL) {
            const int radarSamples = 8;
            const f32 radarRange = 100.0f;

            MtxF rotationMtx;

            Vec3f vecUp;
            vecUp.x = 0.0f;
            vecUp.y = 1.0f;
            vecUp.z = 0.0f;

            Vec3f wallNormal;
            wallNormal.x = (f32)player->actor.wallPoly->normal.x / SHT_MAX;
            wallNormal.y = (f32)player->actor.wallPoly->normal.y / SHT_MAX;
            wallNormal.z = (f32)player->actor.wallPoly->normal.z / SHT_MAX;

            f32 minDist = FLT_MAX;
            Vec3f vecClosest;
            vecA = player->bodyPartsPos[PLAYER_BODYPART_HEAD];
            s16 angle = 0;
            for (int i = 0; i < radarSamples; i++) {
                SkinMatrix_SetRotateAxis(&rotationMtx, angle, wallNormal.x, wallNormal.y, wallNormal.z);
                SkinMatrix_Vec3fMtxFMultXYZ(&rotationMtx, &vecUp, &vecB);
                Math_Vec3f_Scale(&vecB, radarRange);
                Math_Vec3f_Sum(&vecA, &vecB, &vecB);

                if (BgCheck_AnyLineTest2(&play->colCtx, &vecA, &vecB, &vecResult, &outPoly, false, true, true, 1)) {
                    dist = Math_Vec3f_DistXYZ(&vecA, &vecResult);
                    if (dist < minDist) {
                        minDist = dist;
                        vecClosest = vecResult;
                    }
                }

                angle += SHT_MAX / radarSamples * 2;
            }

            if (minDist < radarRange) {
                Math_Vec3f_Diff(&vecClosest, &vecA, &vecB);
                f32 intoWall = Math3D_Vec3fMagnitude(&vecB);

                sWallCuePitch = LERP(0.1f, 0.5f, 0.5f); //(intoWall + 60.0f) / 120.0f);
                sWallCueVolume = LERP(0.1f, 1.2f, SQ((radarRange - minDist) / radarRange));

                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &vecA, &vecB);
                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &vecClosest, &vecA);
                Math_Vec3f_Diff(&vecA, &vecB, &sWallCuePosition);
                dist = minDist;
            } else {
                dist = 0.0f;
            }
        } else {
            const int radarSamples = 16;
            const f32 normalRadarRange = 200.0f;
            const f32 horseRadarRange = 600.0f;
            f32 radarRange = (isPlayerRidingHorse) ? horseRadarRange : normalRadarRange;

            f32 minDist = FLT_MAX;
            Vec3f vecClosest;
            CollisionPoly* polyClosest;
            vecA = player->bodyPartsPos[PLAYER_BODYPART_HEAD];
            for (int i = 0; i < radarSamples; i++) {
                f32 angle = 2.0f * M_PI * (float)i / radarSamples;
                vecB.x = Math_CosF(angle);
                vecB.y = 0.0f;
                vecB.z = Math_SinF(angle);
                Math_Vec3f_Scale(&vecB, radarRange);
                Math_Vec3f_Sum(&vecA, &vecB, &vecB);

                if (BgCheck_AnyLineTest2(&play->colCtx, &vecA, &vecB, &vecResult, &outPoly, true, false, false, 1)) {
                    dist = Math_Vec3f_DistXZ(&vecA, &vecResult);
                    if (dist < minDist) {
                        minDist = dist;
                        vecClosest = vecResult;
                        polyClosest = outPoly;
                    }
                }
            }

            if (minDist < radarRange) {
                Vec3f vecTestDir;
                if (false) {
                    f32 stickRadius = player->unk_A7C;
                    s16 stickAngle = player->unk_A80;
                    Camera* currentCamera = GET_ACTIVE_CAM(play);
                    stickAngle = player->currentYaw; // stickAngle + currentCamera->camDir.y;
                    vecTestDir.x = Math_SinS(stickAngle);
                    vecTestDir.y = 0.0f;
                    vecTestDir.z = Math_CosS(stickAngle);
                    Math_Vec3f_Scale(&vecTestDir, -stickRadius);
                } else {
                    if (isPlayerRidingHorse && player->rideActor != NULL) {
                        vecTestDir = player->rideActor->velocity;
                    } else {
                        vecTestDir = player->actor.velocity;
                    }
                    vecTestDir.y = 0.0f;
                    Math_Vec3f_Scale(&vecTestDir, -1.0);
                }

                Vec3f wallNormal;
                wallNormal.x = polyClosest->normal.x / (f32)SHT_MAX;
                wallNormal.y = 0.0f;
                wallNormal.z = polyClosest->normal.z / (f32)SHT_MAX;

                // Math_Vec3f_Scale(&intendedMove, 20.0f);
                f32 intoWall = DOTXYZ(wallNormal, vecTestDir); // Math3D_Vec3fMagnitude(&vecB);
                sWallCuePitch = LERP(0.05f, 0.8f, (intoWall + 60.0f) / 120.0f);

                sWallCueVolume = LERP(0.1f, 1.2f, SQ((radarRange - minDist) / radarRange));
                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &vecA, &vecB);
                SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &vecClosest, &vecA);
                Math_Vec3f_Diff(&vecA, &vecB, &sWallCuePosition);
                dist = minDist;
            } else {
                dist = 0.0f;
            }
        }
    } else {
        dist = 0.0f;
    }

    if (dist > 0.0f) {
        if (!sWallCueIsPlaying || play->gameplayFrames % 20 == 0) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_CHARGE, &sWallCuePosition, 4, &sWallCuePitch, &sWallCueVolume,
                                   &D_801333E8);
            sWallCueIsPlaying = true;
        }
    } else if (sWallCueIsPlaying) {
        Audio_StopSfxById(NA_SE_IT_SWORD_CHARGE);
        sWallCueIsPlaying = false;
    }
}

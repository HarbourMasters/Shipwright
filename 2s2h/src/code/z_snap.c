#include "global.h"
#include "z64snap.h"
#include "overlays/actors/ovl_En_Kakasi/z_en_kakasi.h"

#define PICTO_SEEN_IN_SCENE 1
#define PICTO_SEEN_ANYWHERE 2

/**
 * Test every loaded actor to see if it is a pictographable `PictoActor`, and if so, run its `validationFunc` to set
 * appropriate flags.
 *
 * @return s32 Number of pictograph actors validly captured.
 */
s32 Snap_RecordPictographedActors(PlayState* play) {
    PictoActor* pictoActor;
    Actor* actor;
    s32 category = 0;
    s32 seen;
    s32 validCount = 0;

    gSaveContext.save.saveInfo.pictoFlags0 = 0;
    gSaveContext.save.saveInfo.pictoFlags1 = 0;

    if (play->sceneId == SCENE_20SICHITAI) {
        Snap_SetFlag(PICTO_VALID_IN_SWAMP);
    }

    for (; category < ACTORCAT_MAX; category++) {
        for (actor = play->actorCtx.actorLists[category].first; actor != NULL; actor = actor->next) {
            seen = 0;

            // Actors which must be pictographed in a specific scene
            switch (play->sceneId) {
                case SCENE_20SICHITAI:
                    if ((actor->id == ACTOR_EN_MNK) || (actor->id == ACTOR_EN_BIGOKUTA)) {
                        seen = PICTO_SEEN_IN_SCENE;
                    }
                    break;

                default:
                    seen = 0;
                    break;
            }

            // Actors which may be pictographed anywhere
            switch (actor->id) {
                case ACTOR_EN_KAKASI:
                    if (KAKASI_GET_ABOVE_GROUND(actor) == 1) {
                        seen |= PICTO_SEEN_ANYWHERE;
                        break; //! @bug break is inside conditional, meaning it falls through if it is false
                    }
                    // FALLTHROUGH
                case ACTOR_EN_ZOV:
                    seen |= PICTO_SEEN_ANYWHERE;
                    break;

                case ACTOR_EN_BAL:
                    seen |= PICTO_SEEN_ANYWHERE;
                    break;

                case ACTOR_EN_DNQ:
                    seen |= PICTO_SEEN_ANYWHERE;
                    break;

                case ACTOR_EN_GE1:
                case ACTOR_EN_GE3:
                case ACTOR_EN_KAIZOKU:
                case ACTOR_EN_GE2:
                    seen |= PICTO_SEEN_ANYWHERE;
                    break;

                default:
                    break;
            }

            if (!seen) {
                continue;
            }

            // If actor is recordable, run its validity function and record if valid
            pictoActor = (PictoActor*)actor;
            if (pictoActor->validationFunc != NULL) {
                if (pictoActor->validationFunc(play, actor) == 0) {
                    validCount++;
                }
            }
        }
    }

    return validCount;
}

// Only used in this file
void Snap_SetFlag(s32 flag) {
    if (flag < 0x20) {
        gSaveContext.save.saveInfo.pictoFlags0 |= (1 << flag);
    } else {
        flag &= 0x1F;
        gSaveContext.save.saveInfo.pictoFlags1 |= (1 << flag);
    }
}

// Unused
void Snap_UnsetFlag(s32 flag) {
    if (flag < 0x20) {
        gSaveContext.save.saveInfo.pictoFlags0 &= ~(1 << flag);
    } else {
        flag &= 0x1F;
        gSaveContext.save.saveInfo.pictoFlags1 &= ~(1 << flag);
    }
}

u32 Snap_CheckFlag(s32 flag) {
    SaveContext* saveCtx = &gSaveContext;

    if (flag < 0x20) {
        return saveCtx->save.saveInfo.pictoFlags0 & (1 << flag);
    } else {
        flag &= 0x1F;
        return saveCtx->save.saveInfo.pictoFlags1 & (1 << flag);
    }
}

s16 Snap_AbsS(s16 val) {
    return ABS(val);
}

/**
 * Test if a pictograph is a valid pictograph of an actor
 *
 * @param play
 * @param actor Actor to test
 * @param flag Flag to set if checks successful
 * @param pos position of point to test (generally a particular point in the actor's model)
 * @param rot rotation of point to test (for facing camera)
 * @param distanceMin closest point may be
 * @param distanceMax farthest away point may be
 * @param angleRange Size of range that counts as facing the camera (-1 is used for any allowed)
 * @return s32 0 on success, `or`ed combination of the validity flag indices if not
 *
 * @note It is generally not possible to recover the actual failure mode(s) from the return value: oring `ret` with the
 * actual flag rather than its index would rectify this.
 */
s32 Snap_ValidatePictograph(PlayState* play, Actor* actor, s32 flag, Vec3f* pos, Vec3s* rot, f32 distanceMin,
                            f32 distanceMax, s16 angleRange) {
    Vec3f projectedPos;
    s16 x;
    s16 y;
    f32 distance;
    CollisionPoly* poly;
    Camera* camera = GET_ACTIVE_CAM(play);
    Actor* actors[2];
    s32 ret = 0;
    s32 bgId;

    // Check distance
    distance = OLib_Vec3fDist(pos, &camera->eye);
    if ((distance < distanceMin) || (distanceMax < distance)) {
        Snap_SetFlag(PICTO_VALID_BAD_DISTANCE);
        ret = PICTO_VALID_BAD_DISTANCE;
    }

    // Check rot is facing camera?
    x = Snap_AbsS(Camera_GetCamDirPitch(camera) + rot->x);
    y = Snap_AbsS(Camera_GetCamDirYaw(camera) - BINANG_SUB(rot->y, 0x7FFF));
    if ((0 < angleRange) && ((angleRange < x) || (angleRange < y))) {
        Snap_SetFlag(PICTO_VALID_BAD_ANGLE);
        ret |= PICTO_VALID_BAD_ANGLE;
    }

    // Check in capture region
    Actor_GetProjectedPos(play, pos, &projectedPos, &distance);
    // Convert to projected position to device coordinates, shift to be relative to the capture region's top-left corner
    x = TRUNCF_BINANG(PROJECTED_TO_SCREEN_X(projectedPos, distance)) - PICTO_VALID_TOPLEFT_X;
    y = TRUNCF_BINANG(PROJECTED_TO_SCREEN_Y(projectedPos, distance)) - PICTO_VALID_TOPLEFT_Y;

    // checks if the coordinates are within the capture region
    if ((x < 0) || (x > PICTO_VALID_WIDTH) || (y < 0) || (y > PICTO_VALID_HEIGHT)) {
        Snap_SetFlag(PICTO_VALID_NOT_IN_VIEW);
        ret |= PICTO_VALID_NOT_IN_VIEW;
    }

    // Check not obscured by bg collision
    if (BgCheck_ProjectileLineTest(&play->colCtx, pos, &camera->eye, &projectedPos, &poly, true, true, true, true,
                                   &bgId)) {
        Snap_SetFlag(PICTO_VALID_BEHIND_BG);
        ret |= PICTO_VALID_BEHIND_BG;
    }

    // Check not obscured by actor collision
    actors[0] = actor;
    actors[1] = &GET_PLAYER(play)->actor;
    if (CollisionCheck_LineOCCheck(play, &play->colChkCtx, pos, &camera->eye, actors, 2)) {
        Snap_SetFlag(PICTO_VALID_BEHIND_COLLISION);
        ret |= PICTO_VALID_BEHIND_COLLISION;
    }

    // If all of the above checks pass, set the flag
    if (ret == 0) {
        Snap_SetFlag(flag);
    }

    return ret;
}

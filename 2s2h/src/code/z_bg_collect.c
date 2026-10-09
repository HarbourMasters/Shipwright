#include "global.h"

/**
 * Update the `carriedActor`'s position based on the dynapoly actor identified by `bgId`.
 */
void DynaPolyActor_UpdateCarriedActorPos(CollisionContext* colCtx, s32 bgId, Actor* carriedActor) {
    MtxF prevTransform;
    MtxF prevTransformInv;
    MtxF curTransform;
    Vec3f pos;
    Vec3f tempPos;

    if (!DynaPoly_IsBgIdBgActor(bgId)) {
        return;
    }

    SkinMatrix_SetScaleRotateYRPTranslate(
        &prevTransform, colCtx->dyna.bgActors[bgId].prevTransform.scale.x,
        colCtx->dyna.bgActors[bgId].prevTransform.scale.y, colCtx->dyna.bgActors[bgId].prevTransform.scale.z,
        colCtx->dyna.bgActors[bgId].prevTransform.rot.x, colCtx->dyna.bgActors[bgId].prevTransform.rot.y,
        colCtx->dyna.bgActors[bgId].prevTransform.rot.z, colCtx->dyna.bgActors[bgId].prevTransform.pos.x,
        colCtx->dyna.bgActors[bgId].prevTransform.pos.y, colCtx->dyna.bgActors[bgId].prevTransform.pos.z);

    if (SkinMatrix_Invert(&prevTransform, &prevTransformInv) == 2) {
        return;
    }

    SkinMatrix_SetScaleRotateYRPTranslate(
        &curTransform, colCtx->dyna.bgActors[bgId].curTransform.scale.x,
        colCtx->dyna.bgActors[bgId].curTransform.scale.y, colCtx->dyna.bgActors[bgId].curTransform.scale.z,
        colCtx->dyna.bgActors[bgId].curTransform.rot.x, colCtx->dyna.bgActors[bgId].curTransform.rot.y,
        colCtx->dyna.bgActors[bgId].curTransform.rot.z, colCtx->dyna.bgActors[bgId].curTransform.pos.x,
        colCtx->dyna.bgActors[bgId].curTransform.pos.y, colCtx->dyna.bgActors[bgId].curTransform.pos.z);

    // Apply the movement of the dynapoly actor `bgId` over the last frame to the `carriedActor` position
    // pos = curTransform * prevTransformInv * pos
    // Note (curTransform * prevTransformInv) represents the transform relative to the previous frame
    SkinMatrix_Vec3fMtxFMultXYZ(&prevTransformInv, &carriedActor->world.pos, &tempPos);
    SkinMatrix_Vec3fMtxFMultXYZ(&curTransform, &tempPos, &pos);

    carriedActor->world.pos = pos;
}

/**
 * Update the `carriedActor`'s Y rotation based on the dynapoly actor identified by `bgId`.
 */
void DynaPolyActor_UpdateCarriedActorRotY(CollisionContext* colCtx, s32 bgId, Actor* carriedActor) {
    s16 rotY;

    if (!DynaPoly_IsBgIdBgActor(bgId)) {
        return;
    }

    rotY = colCtx->dyna.bgActors[bgId].curTransform.rot.y - colCtx->dyna.bgActors[bgId].prevTransform.rot.y;

    if (carriedActor->id == ACTOR_PLAYER) {
        ((Player*)carriedActor)->yaw += rotY;
    }

    carriedActor->shape.rot.y += rotY;
    carriedActor->world.rot.y += rotY;
}

void DynaPolyActor_AttachCarriedActor(CollisionContext* colCtx, Actor* carriedActor, s32 bgId) {
    DynaPolyActor* dynaActor;

    if (!DynaPoly_IsBgIdBgActor(bgId)) {
        return;
    }

    dynaActor = DynaPoly_GetActor(colCtx, bgId);
    if (dynaActor != NULL) {
        DynaPolyActor_SetActorOnTop(dynaActor);

        if (CHECK_FLAG_ALL(carriedActor->flags, ACTOR_FLAG_CAN_PRESS_SWITCHES)) {
            DynaPolyActor_SetSwitchPressed(dynaActor);
        }
        if (CHECK_FLAG_ALL(carriedActor->flags, ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES)) {
            DynaPolyActor_SetHeavySwitchPressed(dynaActor);
        }
    }
}

/**
 * Update the `carriedActor`'s position and Y rotation based on the dynapoly actor identified by `bgId`, according to
 * the dynapoly actor's move flags (see `DYNA_TRANSFORM_POS` and `DYNA_TRANSFORM_ROT_Y`).
 */
u32 DynaPolyActor_TransformCarriedActor(CollisionContext* colCtx, s32 bgId, Actor* carriedActor) {
    u32 wasUpdated = false;
    DynaPolyActor* dynaActor;

    if (!DynaPoly_IsBgIdBgActor(bgId)) {
        return false;
    }

    if ((colCtx->dyna.bgActorFlags[bgId] & BGACTOR_1) || !(colCtx->dyna.bgActorFlags[bgId] & BGACTOR_IN_USE)) {
        return false;
    }

    dynaActor = DynaPoly_GetActor(colCtx, bgId);

    if (dynaActor == NULL) {
        return false;
    }

    if (dynaActor->transformFlags & DYNA_TRANSFORM_POS) {
        DynaPolyActor_UpdateCarriedActorPos(colCtx, bgId, carriedActor);
        wasUpdated = true;
    }

    if (dynaActor->transformFlags & DYNA_TRANSFORM_ROT_Y) {
        DynaPolyActor_UpdateCarriedActorRotY(colCtx, bgId, carriedActor);
        wasUpdated = true;
    }

    return wasUpdated;
}

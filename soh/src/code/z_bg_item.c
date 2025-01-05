#include "global.h"

void DynaPolyActor_Init(DynaPolyActor* dynaActor, s32 flags) {
    dynaActor->bgId = -1;
    dynaActor->transformFlags = flags;
    dynaActor->interactFlags = 0;
    dynaActor->unk_150 = 0.0f;
    dynaActor->unk_154 = 0.0f;
}

void DynaPolyActor_UnsetAllInteractFlags(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags = 0;
}

void DynaPolyActor_SetActorOnTop(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_ACTOR_ON_TOP;
}

void DynaPolyActor_SetPlayerOnTop(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_PLAYER_ON_TOP;
}

void DynaPoly_SetPlayerOnTop(CollisionContext* colCtx, s32 floorBgId) {
    DynaPolyActor* dynaActor = DynaPoly_GetActor(colCtx, floorBgId);

    if (dynaActor != NULL) {
        DynaPolyActor_SetPlayerOnTop(dynaActor);
    }
}

void DynaPolyActor_SetPlayerAbove(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_PLAYER_ABOVE;
}

void DynaPoly_SetPlayerAbove(CollisionContext* colCtx, s32 floorBgId) {
    DynaPolyActor* dynaActor = DynaPoly_GetActor(colCtx, floorBgId);

    if (dynaActor != NULL) {
        DynaPolyActor_SetPlayerAbove(dynaActor);
    }
}

void DynaPolyActor_SetSwitchPressed(DynaPolyActor* dynaActor) {
    dynaActor->interactFlags |= DYNA_INTERACT_ACTOR_SWITCH_PRESSED;
}

s32 DynaPolyActor_IsActorOnTop(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_ACTOR_ON_TOP) {
        return true;
    } else {
        return false;
    }
}

s32 DynaPolyActor_IsPlayerOnTop(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_PLAYER_ON_TOP) {
        return true;
    } else {
        return false;
    }
}

s32 DynaPolyActor_IsPlayerAbove(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_PLAYER_ABOVE) {
        return true;
    } else {
        return false;
    }
}

s32 DynaPolyActor_IsSwitchPressed(DynaPolyActor* dynaActor) {
    if (dynaActor->interactFlags & DYNA_INTERACT_ACTOR_SWITCH_PRESSED) {
        return true;
    } else {
        return false;
    }
}

s32 func_800435D8(PlayState* play, DynaPolyActor* dynaActor, s16 arg2, s16 arg3, s16 arg4) {
    Vec3f posA;
    Vec3f posB;
    Vec3f posResult;
    f32 sin = Math_SinS(dynaActor->unk_158);
    f32 cos = Math_CosS(dynaActor->unk_158);
    s32 bgId;
    CollisionPoly* poly;
    f32 a2;
    f32 a3;
    f32 sign = (0.0f <= dynaActor->unk_150) ? 1.0f : -1.0f;

    a2 = (f32)arg2 - 0.1f;
    posA.x = dynaActor->actor.world.pos.x + (a2 * cos);
    posA.y = dynaActor->actor.world.pos.y + arg4;
    posA.z = dynaActor->actor.world.pos.z - (a2 * sin);

    a3 = (f32)arg3 - 0.1f;
    posB.x = sign * a3 * sin + posA.x;
    posB.y = posA.y;
    posB.z = sign * a3 * cos + posA.z;

    if (BgCheck_EntityLineTest3(&play->colCtx, &posA, &posB, &posResult, &poly, true, false, false, true, &bgId,
                                &dynaActor->actor, 0.0f)) {
        return false;
    }
    posA.x = (dynaActor->actor.world.pos.x * 2) - posA.x;
    posA.z = (dynaActor->actor.world.pos.z * 2) - posA.z;
    posB.x = sign * a3 * sin + posA.x;
    posB.z = sign * a3 * cos + posA.z;
    if (BgCheck_EntityLineTest3(&play->colCtx, &posA, &posB, &posResult, &poly, true, false, false, true, &bgId,
                                &dynaActor->actor, 0.0f)) {
        return false;
    }
    return true;
}

/**
 * cutscene_helper.c - Mini-cutscene system for item activations
 */

#include "cutscene_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include <math.h>

void CutsceneHelper_Start(Player* player, PlayState* play, CutsceneItemState* state) {
    if (state->active) return;

    state->active = 1;
    state->phase = 0;
    state->timer = 0;
    state->cameraOrbitRadius = 150.0f;
    state->cameraHeight = 80.0f;
    state->cameraOrbitSpeed = 0.05f;
    state->cameraOrbitAngle = player->actor.shape.rot.y * (M_PI / 32768.0f);

    player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
    player->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    player->actor.speedXZ = 0.0f;
    player->linearVelocity = 0.0f;
    player->actor.velocity.y = 0.0f;
    player->actor.gravity = 0.0f;

    CutsceneHelper_FreezeEnemies(play, &player->actor.world.pos, 500.0f);
}

u8 CutsceneHelper_Update(Player* player, PlayState* play, CutsceneItemState* state) {
    if (!state->active) return 1;

    state->timer++;
    player->actor.speedXZ = 0.0f;
    player->linearVelocity = 0.0f;

    state->cameraOrbitAngle += state->cameraOrbitSpeed;
    if (state->cameraOrbitAngle > M_PI * 2.0f) {
        state->cameraOrbitAngle -= M_PI * 2.0f;
    }

    Vec3f cameraPos;
    cameraPos.x = player->actor.world.pos.x + cosf(state->cameraOrbitAngle) * state->cameraOrbitRadius;
    cameraPos.y = player->actor.world.pos.y + state->cameraHeight;
    cameraPos.z = player->actor.world.pos.z + sinf(state->cameraOrbitAngle) * state->cameraOrbitRadius;

    Camera* camera = Play_GetCamera(play, 0);
    if (camera != NULL) {
        state->cameraOffset.x = cameraPos.x - player->actor.world.pos.x;
        state->cameraOffset.y = cameraPos.y - player->actor.world.pos.y;
        state->cameraOffset.z = cameraPos.z - player->actor.world.pos.z;
    }

    return 0;
}

void CutsceneHelper_End(Player* player, PlayState* play, CutsceneItemState* state) {
    if (!state->active) return;

    state->active = 0;
    state->phase = 0;
    state->timer = 0;

    player->stateFlags1 &= ~PLAYER_STATE1_INPUT_DISABLED;
    player->stateFlags2 &= ~PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    player->actor.gravity = -4.0f;

    CutsceneHelper_UnfreezeEnemies(play);
}

void CutsceneHelper_SetCameraOrbit(CutsceneItemState* state, f32 radius, f32 height, f32 speed) {
    state->cameraOrbitRadius = radius;
    state->cameraHeight = height;
    state->cameraOrbitSpeed = speed;
}

void CutsceneHelper_FreezeEnemies(PlayState* play, Vec3f* center, f32 radius) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ENEMY].head;
    while (actor != NULL) {
        if (actor->update != NULL) {
            f32 dist = Math_Vec3f_DistXYZ(center, &actor->world.pos);
            if (dist < radius) {
                actor->freezeTimer = 120;
                Actor_SetColorFilter(actor, 0, 120, 0x2000, 120);
            }
        }
        actor = actor->next;
    }
}

void CutsceneHelper_UnfreezeEnemies(PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ENEMY].head;
    while (actor != NULL) {
        if (actor->update != NULL) {
            actor->freezeTimer = 0;
        }
        actor = actor->next;
    }
}

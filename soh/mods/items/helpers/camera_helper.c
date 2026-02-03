/**
 * camera_helper.c - First-person aiming and camera utilities
 */

#include "camera_helper.h"
#include "functions.h"
#include "macros.h"
#include "objects/object_link_boy/object_link_boy.h"

extern int Player_IsZTargeting(Player* this);

void FirstPerson_Init(Player* player, PlayState* play) {
    player->unk_6AD = 2;  // weapon aiming mode
    player->stateFlags1 |= PLAYER_STATE1_FIRST_PERSON;
    player->stateFlags1 |= PLAYER_STATE1_ITEM_IN_HAND;
    player->stateFlags1 |= PLAYER_STATE1_READY_TO_FIRE;
    player->unk_834 = 14;
    Player_ZeroSpeedXZ(player);
}

void FirstPerson_Update(Player* player, PlayState* play) {
    player->unk_6AD = 2;

    if (player->unk_834 > 10) {
        player->unk_834--;
    } else if (player->unk_834 == 0) {
        player->unk_834 = 1;
    }

    player->stateFlags1 |= PLAYER_STATE1_FIRST_PERSON;
    player->stateFlags1 |= PLAYER_STATE1_READY_TO_FIRE;
}

void FirstPerson_Exit(Player* player, PlayState* play) {
    player->unk_6AD = 0;
    player->stateFlags1 &= ~PLAYER_STATE1_FIRST_PERSON;
    player->stateFlags1 &= ~PLAYER_STATE1_ITEM_IN_HAND;
    player->stateFlags1 &= ~PLAYER_STATE1_READY_TO_FIRE;
    player->unk_834 = 0;
}

s16 FirstPerson_GetAimYaw(Player* player) {
    return player->actor.focus.rot.y;
}

s16 FirstPerson_GetAimPitch(Player* player) {
    return player->actor.focus.rot.x;
}

void FirstPerson_DrawReticle(Player* player, PlayState* play, f32 range, u8 r, u8 g, u8 b) {
    if (!(player->stateFlags1 & PLAYER_STATE1_ITEM_IN_HAND) || player->unk_834 == 0) return;

    CollisionPoly* colPoly;
    s32 bgId;
    Vec3f rayStart, rayEnd, hitPos;
    Vec3f screenPos;
    f32 screenW;

    rayStart.x = player->actor.focus.pos.x;
    rayStart.y = player->actor.focus.pos.y;
    rayStart.z = player->actor.focus.pos.z;

    f32 cosY = Math_CosS(player->actor.focus.rot.y);
    f32 sinY = Math_SinS(player->actor.focus.rot.y);
    f32 cosX = Math_CosS(player->actor.focus.rot.x);
    f32 sinX = Math_SinS(player->actor.focus.rot.x);

    f32 maxRange = (range > 0) ? range : 10000.0f;

    rayEnd.x = rayStart.x + (sinY * cosX * maxRange);
    rayEnd.y = rayStart.y + (-sinX * maxRange);
    rayEnd.z = rayStart.z + (cosY * cosX * maxRange);

    if (BgCheck_AnyLineTest3(&play->colCtx, &rayStart, &rayEnd, &hitPos, &colPoly, 1, 1, 1, 1, &bgId)) {
        OPEN_DISPS(play->state.gfxCtx);

        OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, 0x07);

        SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &hitPos, &screenPos, &screenW);
        f32 scale = (screenW < 200.0f) ? 0.08f : (screenW / 200.0f) * 0.08f;

        Matrix_Translate(hitPos.x, hitPos.y, hitPos.z, MTXMODE_NEW);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

        gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(OVERLAY_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPTexture(OVERLAY_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPLoadTextureBlock(OVERLAY_DISP++, gLinkAdultHookshotReticleTex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, r, g, b, 255);

        gSPVertex(OVERLAY_DISP++, (uintptr_t)gLinkAdultHookshotReticleVtx, 3, 0);
        gSP1Triangle(OVERLAY_DISP++, 0, 1, 2, 0);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void ItemCamera_Init(ItemCameraState* state, Player* player, PlayState* play) {
    if (Player_IsZTargeting(player)) {
        state->mode = CAMERA_MODE_Z_TARGET;
        state->firstPersonActive = 0;
    } else {
        state->mode = CAMERA_MODE_FIRST_PERSON;
        FirstPerson_Init(player, play);
        state->firstPersonActive = 1;
    }
}

void ItemCamera_Update(ItemCameraState* state, Player* player, PlayState* play) {
    if (state->mode == CAMERA_MODE_FREE) {
        if (state->firstPersonActive) {
            FirstPerson_Exit(player, play);
            state->firstPersonActive = 0;
        }
        return;
    }

    u8 isZTargeting = Player_IsZTargeting(player);

    if (state->mode == CAMERA_MODE_FIRST_PERSON && isZTargeting) {
        FirstPerson_Exit(player, play);
        state->firstPersonActive = 0;
        state->mode = CAMERA_MODE_Z_TARGET;
    } else if (state->mode == CAMERA_MODE_Z_TARGET && !isZTargeting) {
        FirstPerson_Init(player, play);
        state->firstPersonActive = 1;
        state->mode = CAMERA_MODE_FIRST_PERSON;
    }

    if (state->firstPersonActive) {
        FirstPerson_Update(player, play);
    }
}

void ItemCamera_Exit(ItemCameraState* state, Player* player, PlayState* play) {
    if (state->firstPersonActive) {
        FirstPerson_Exit(player, play);
        state->firstPersonActive = 0;
    }
    state->mode = CAMERA_MODE_FIRST_PERSON;
}

void ItemCamera_ToggleFirstPerson(ItemCameraState* state, Player* player, PlayState* play) {
    if (state->firstPersonActive) {
        FirstPerson_Exit(player, play);
        state->firstPersonActive = 0;
        state->mode = Player_IsZTargeting(player) ? CAMERA_MODE_Z_TARGET : CAMERA_MODE_FREE;
    } else {
        FirstPerson_Init(player, play);
        state->firstPersonActive = 1;
        state->mode = CAMERA_MODE_FIRST_PERSON;
    }
}

s16 ItemCamera_GetAimYaw(ItemCameraState* state, Player* player, PlayState* play) {
    switch (state->mode) {
        case CAMERA_MODE_FIRST_PERSON:
            return state->firstPersonActive ? FirstPerson_GetAimYaw(player) : player->actor.shape.rot.y;

        case CAMERA_MODE_Z_TARGET:
            if (Player_IsZTargeting(player) && player->focusActor != NULL) {
                return Math_Vec3f_Yaw(&player->actor.world.pos, &player->focusActor->focus.pos);
            }
            return player->actor.shape.rot.y;

        case CAMERA_MODE_FREE:
        default:
            return player->actor.shape.rot.y;
    }
}

s16 ItemCamera_GetAimPitch(ItemCameraState* state, Player* player) {
    if (state->mode == CAMERA_MODE_FIRST_PERSON && state->firstPersonActive) {
        return FirstPerson_GetAimPitch(player);
    }
    return 0;
}

void ItemCamera_SetFreeMode(ItemCameraState* state, Player* player, PlayState* play) {
    if (state->firstPersonActive) {
        FirstPerson_Exit(player, play);
        state->firstPersonActive = 0;
    }
    state->mode = CAMERA_MODE_FREE;
}

s16 Camera_GetDirectionYaw(PlayState* play) {
    Camera* cam = play->cameraPtrs[play->activeCamera];
    return cam->camDir.y;
}

s16 Camera_GetDirectionPitch(PlayState* play) {
    Camera* cam = play->cameraPtrs[play->activeCamera];
    return -cam->camDir.x;
}

void Camera_InterpolateToDirection(s16* currentYaw, s16* currentPitch, PlayState* play, s16 yawSpeed, s16 pitchSpeed) {
    s16 targetYaw = Camera_GetDirectionYaw(play);
    s16 targetPitch = Camera_GetDirectionPitch(play);
    Math_ScaledStepToS(currentYaw, targetYaw, yawSpeed);
    Math_ScaledStepToS(currentPitch, targetPitch, pitchSpeed);
}

void Input_GetStickDirection(PlayState* play, s16* outYawDelta, s16* outPitchDelta, s16 sensitivity) {
    s8 stickX = play->state.input[0].cur.stick_x;
    s8 stickY = play->state.input[0].cur.stick_y;
    *outYawDelta = -stickX * sensitivity;
    *outPitchDelta = stickY * sensitivity;
}

void Projectile_UpdateDirectionFromStick(s16* yaw, s16* pitch, PlayState* play, s16 turnSpeed, s16 pitchMax) {
    s16 yawDelta, pitchDelta;
    Input_GetStickDirection(play, &yawDelta, &pitchDelta, turnSpeed);
    *yaw += yawDelta;
    *pitch += pitchDelta;
    if (*pitch > pitchMax) *pitch = pitchMax;
    if (*pitch < -pitchMax) *pitch = -pitchMax;
}

void Projectile_UpdateRotationFromStick(s16* yaw, s16* pitch, PlayState* play, s16 turnSpeed, s16 pitchMax) {
    Input* input = &play->state.input[0];
    f32 relX = input->rel.stick_x;
    f32 relY = input->rel.stick_y;
    f32 magnitude = sqrtf(SQ(relX) + SQ(relY));

    if (magnitude > 20.0f) {
        s16 stickAngle = Math_Atan2S(relY, -relX);
        f32 scaledSpeed = (magnitude / 60.0f) * turnSpeed;

        s16 targetYaw = *yaw + (s16)(Math_SinS(stickAngle) * scaledSpeed);
        s16 targetPitch = *pitch + (s16)(Math_CosS(stickAngle) * scaledSpeed);

        *yaw = targetYaw;
        *pitch = targetPitch;

        if (*pitch > pitchMax) *pitch = pitchMax;
        if (*pitch < -pitchMax) *pitch = -pitchMax;
    }
}

#include "global.h"

void Actor_ContinueText(PlayState* play, Actor* actor, u16 textId) {
    Message_ContinueTextbox(play, textId);
    actor->textId = textId;
}

/**
 * EventCheckInf does not exist, so this always returns false
 */
s32 Flags_GetEventChkInf(s32 flag) {
    return false;
}

/**
 * EventCheckInf does not exist, so this does nothing
 */
void Flags_SetEventChkInf(s32 flag) {
}

/**
 * InfTable does not exist, so this always returns false
 */
s32 Flags_GetInfTable(s32 flag) {
    return false;
}

/**
 * InfTable does not exist, so this does nothing
 */
void Flags_SetInfTable(s32 flag) {
}

s32 Actor_TrackNone(Vec3s* headRot, Vec3s* torsoRot) {
    Math_SmoothStepToS(&headRot->y, 0, 6, 0x1838, 0x64);
    Math_SmoothStepToS(&headRot->x, 0, 6, 0x1838, 0x64);
    Math_SmoothStepToS(&torsoRot->y, 0, 6, 0x1838, 0x64);
    Math_SmoothStepToS(&torsoRot->x, 0, 6, 0x1838, 0x64);
    return true;
}

s32 Actor_TrackPoint(Actor* actor, Vec3f* target, Vec3s* headRot, Vec3s* torsoRot) {
    s16 targetPitch;
    s16 targetYaw;
    s16 yawDiff;

    targetPitch = Math_Vec3f_Pitch(&actor->focus.pos, target);
    targetYaw = Math_Vec3f_Yaw(&actor->focus.pos, target) - actor->world.rot.y;

    Math_SmoothStepToS(&headRot->x, targetPitch, 6, 0x7D0, 1);
    headRot->x = CLAMP(headRot->x, -0x1770, 0x1770);

    yawDiff = Math_SmoothStepToS(&headRot->y, targetYaw, 6, 0x7D0, 1);
    headRot->y = CLAMP(headRot->y, -0x1F40, 0x1F40);

    if ((yawDiff != 0) && (ABS_ALT(headRot->y) < 0x1F40)) {
        return false;
    }

    Math_SmoothStepToS(&torsoRot->y, targetYaw - headRot->y, 4, 0x7D0, 1);
    torsoRot->y = CLAMP(torsoRot->y, -0x2EE0, 0x2EE0);

    return true;
}

/**
 * Same as Actor_TrackPlayer, except use the actor's world position as the focus point, with the height
 * specified.
 *
 * @param play
 * @param actor
 * @param headRot the computed actor's head's rotation step
 * @param torsoRot the computed actor's torso's rotation step
 * @param focusHeight the height of the focus point relative to their world position
 *
 * @return true if rotated towards player, false if rotations were stepped back to zero.
 *
 * @note same note as Actor_TrackPlayer
 */
s32 Actor_TrackPlayerSetFocusHeight(PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot, f32 focusHeight) {
    Player* player = GET_PLAYER(play);
    s16 yaw;
    Vec3f target;

    actor->focus.pos = actor->world.pos;
    actor->focus.pos.y += focusHeight;

    if (!((play->csCtx.state != CS_STATE_IDLE) || gDbgCamEnabled)) {
        yaw = ABS_ALT(BINANG_SUB(actor->yawTowardsPlayer, actor->shape.rot.y));
        if (yaw >= 0x4300) {
            Actor_TrackNone(headRot, torsoRot);
            return false;
        }
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || gDbgCamEnabled) {
        target = play->view.eye;
    } else {
        target = player->actor.focus.pos;
    }

    Actor_TrackPoint(actor, &target, headRot, torsoRot);

    return true;
}

/**
 * Computes the necessary HeadRot and TorsoRot steps to be added to the normal rotation to smoothly turn an actors's
 * head and torso towards the player if within a certain yaw, else smoothly returns the rotations back to zero.
 * Also sets the focus position with the specified point.
 *
 * @param play
 * @param actor
 * @param headRot the computed actor's head's rotation step
 * @param torsoRot the computed actor's torso's rotation step
 * @param focusPos the point to set as the actor's focus position
 *
 * @return true if rotated towards player, false if rotations were stepped back to zero.
 *
 * @note if in a cutscene or debug camera is enabled, the computed rotation will instead turn towards the view eye no
 * matter the yaw.
 */
s32 Actor_TrackPlayer(PlayState* play, Actor* actor, Vec3s* headRot, Vec3s* torsoRot, Vec3f focusPos) {
    Player* player = GET_PLAYER(play);
    s16 yaw;
    Vec3f target;

    actor->focus.pos = focusPos;

    if (!((play->csCtx.state != CS_STATE_IDLE) || gDbgCamEnabled)) {
        yaw = ABS_ALT(BINANG_SUB(actor->yawTowardsPlayer, actor->shape.rot.y));
        if (yaw >= 0x4300) {
            Actor_TrackNone(headRot, torsoRot);
            return false;
        }
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || gDbgCamEnabled) {
        target = play->view.eye;
    } else {
        target = player->actor.focus.pos;
    }

    Actor_TrackPoint(actor, &target, headRot, torsoRot);

    return true;
}

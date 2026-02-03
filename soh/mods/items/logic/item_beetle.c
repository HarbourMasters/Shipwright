/**
 * item_beetle.c - Beetle from Skyward Sword
 *
 * Controls:
 *   C Button:    Launch beetle in aimed direction
 *   Analog:      Steer beetle flight path
 *   C Button:    Recall beetle early
 *   B Button:    Boost speed temporarily
 *
 * Features:
 *   - Remote-controlled flying beetle with camera follow
 *   - Can grab and carry items back to Link
 *   - Damages enemies on impact
 *   - Limited flight time before returning
 */

#include "z64.h"
#include "item_beetle.h"
#include "../custom_items.h"
#include "../helpers/camera_helper.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

static u8 sBeetleColInitialized = 0;
static s8 sBeetlePrevInvinc = 0;

static void Beetle_DropGrabbedActor(Player* p);

static ColliderCylinderInit sBeetleColliderInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_ON | OC1_TYPE_ALL, OC2_TYPE_PLAYER, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2, { BEETLE_DMG_FLAGS, 0x00, 0x01 }, { 0xFFCFFFFF, 0x00, 0x00 },
      TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NORMAL, BUMP_NONE, OCELEM_ON },
    { (s16)BEETLE_DAMAGE_RADIUS, (s16)BEETLE_DAMAGE_HEIGHT, 0, { 0, 0, 0 } }
};

static void Beetle_InitCollider(PlayState* play, Player* p) {
    if (sBeetleColInitialized) return;
    Collider_InitCylinder(play, &beetleCollider);
    Collider_SetCylinder(play, &beetleCollider, &p->actor, &sBeetleColliderInit);
    sBeetleColInitialized = 1;
}

static void Beetle_UpdateCollider(PlayState* play, Vec3f* pos) {
    beetleCollider.dim.pos.x = (s16)pos->x;
    beetleCollider.dim.pos.y = (s16)pos->y;
    beetleCollider.dim.pos.z = (s16)pos->z;
    beetleCollider.base.atFlags |= AT_ON | AT_TYPE_PLAYER;
    CollisionCheck_SetAT(play, &play->colChkCtx, &beetleCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &beetleCollider.base);
}

static void Beetle_PlaySound(Vec3f* pos, u16 sfxId) {
    Audio_PlaySoundGeneral(sfxId, pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void Beetle_PlayLoopSound(Actor* actor, u16 sfxId) {
    func_8002F974(actor, sfxId - SFX_FLAG);
}

u8 Beetle_IsFlying(void) {
    return beetleActive && (beetleState == BEETLE_STATE_FLYING || beetleState == BEETLE_STATE_RETURNING);
}

static void Beetle_DestroySubCam(PlayState* play) {
    if (beetleSubCamId != SUBCAM_FREE) {
        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
        Play_ClearCamera(play, beetleSubCamId);
        beetleSubCamId = SUBCAM_FREE;
    }
}

static void Beetle_CreateSubCam(PlayState* play) {
    if (beetleSubCamId == SUBCAM_FREE) {
        beetleSubCamId = Play_CreateSubCamera(play);
        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
        Play_ChangeCameraStatus(play, beetleSubCamId, CAM_STAT_ACTIVE);
    }
}

static void Beetle_UpdateSubCam(PlayState* play) {
    if (beetleSubCamId == SUBCAM_FREE) return;

    f32 sinY = Math_SinS(beetleRot.y);
    f32 cosY = Math_CosS(beetleRot.y);
    f32 sinP = Math_SinS(beetleRot.x);
    f32 cosP = Math_CosS(beetleRot.x);

    Vec3f eye;
    eye.x = beetlePos.x - sinY * cosP * BEETLE_CAM_DISTANCE;
    eye.y = beetlePos.y - BEETLE_CAM_HEIGHT + sinP * BEETLE_CAM_DISTANCE;
    eye.z = beetlePos.z - cosY * cosP * BEETLE_CAM_DISTANCE;

    Vec3f at = beetlePos;

    Play_CameraSetAtEye(play, beetleSubCamId, &at, &eye);
}

static void Beetle_Stop(Player* p, PlayState* play) {
    if (beetleFirstPerson) {
        FirstPerson_Exit(p, play);
        beetleFirstPerson = 0;
    }
    Beetle_DestroySubCam(play);
    p->stateFlags1 &= ~PLAYER_STATE1_BOOMERANG_THROWN;
    p->stateFlags2 &= ~PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    beetleCollider.base.atFlags &= ~(AT_ON | AT_HIT);
    beetleActive = 0;
    beetleState = BEETLE_STATE_IDLE;
    beetleGrabbed = NULL;
    ItemEquip_PlayUnequipSFX(play, p);
}

static void Beetle_Start(Player* p, PlayState* play) {
    if (beetleActive) return;
    beetleActive = 1;
    beetleState = BEETLE_STATE_AIMING;
    beetleFirstPerson = 1;
    beetleGrabbed = NULL;
    beetleWingScale = BEETLE_WING_SCALE_MAX;
    beetleWingDir = -1;
    beetleTimer = BEETLE_MAX_TIME;

    LinkAnimation_PlayLoop(play, &p->upperSkelAnime, &gPlayerAnim_link_boom_throw_waitR);

    FirstPerson_Init(p, play);
    ItemEquip_PlayEquipSFX(play, p);
}

static void Beetle_Launch(Player* p, PlayState* play) {
    beetleState = BEETLE_STATE_FLYING;
    beetleTimer = BEETLE_MAX_TIME;
    beetleStartPos = p->actor.world.pos;

    s16 launchYaw = FirstPerson_GetAimYaw(p);
    s16 launchPitch = FirstPerson_GetAimPitch(p);

    beetlePos.x = p->actor.world.pos.x + Math_SinS(launchYaw) * BEETLE_LAUNCH_OFFSET_XZ;
    beetlePos.y = p->actor.world.pos.y + BEETLE_LAUNCH_OFFSET_Y;
    beetlePos.z = p->actor.world.pos.z + Math_CosS(launchYaw) * BEETLE_LAUNCH_OFFSET_XZ;

    beetleRot.x = launchPitch;
    beetleRot.y = launchYaw;
    beetleRot.z = 0;

    LinkAnimation_PlayOnce(play, &p->upperSkelAnime, &gPlayerAnim_link_boom_throwR);

    FirstPerson_Exit(p, play);
    beetleFirstPerson = 0;
    Beetle_CreateSubCam(play);

    Beetle_PlaySound(&p->actor.world.pos, BEETLE_SFX_LAUNCH);
    Audio_PlaySoundGeneral(
        LINK_IS_ADULT ? NA_SE_VO_LI_SWORD_N : NA_SE_VO_LI_SWORD_N_KID,
        &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void Beetle_StartReturn(Player* p, PlayState* play) {
    beetleState = BEETLE_STATE_RETURNING;
    Beetle_DestroySubCam(play);
    p->stateFlags1 &= ~PLAYER_STATE1_BOOMERANG_THROWN;
    p->stateFlags2 &= ~PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    Beetle_PlaySound(&beetlePos, BEETLE_SFX_RETURN);
}

static void Beetle_Catch(Player* p, PlayState* play) {
    LinkAnimation_PlayOnce(play, &p->upperSkelAnime, &gPlayerAnim_link_boom_catch);

    Beetle_PlaySound(&p->actor.world.pos, BEETLE_SFX_CATCH);
    Audio_PlaySoundGeneral(
        LINK_IS_ADULT ? NA_SE_VO_LI_SWORD_N : NA_SE_VO_LI_SWORD_N_KID,
        &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    Beetle_DropGrabbedActor(p);

    beetleActive = 0;
    beetleState = BEETLE_STATE_IDLE;
}

static void Beetle_Move(f32 speed) {
    f32 cosP = Math_CosS(beetleRot.x);
    f32 sinP = Math_SinS(beetleRot.x);
    f32 sinY = Math_SinS(beetleRot.y);
    f32 cosY = Math_CosS(beetleRot.y);

    beetlePos.x += sinY * cosP * speed;
    beetlePos.y -= sinP * speed;
    beetlePos.z += cosY * cosP * speed;
}

static u8 Beetle_CheckActorCollision(Player* p, PlayState* play) {
    if (!(beetleCollider.base.atFlags & AT_HIT)) return 0;

    Actor* hitActor = beetleCollider.base.at;
    u8 shouldReturn = 0;

    if (hitActor != NULL) {
        if (hitActor->id == ACTOR_EN_ITEM00 || hitActor->id == ACTOR_EN_SI) {
            beetleGrabbed = hitActor;
            if (hitActor->id == ACTOR_EN_SI) {
                hitActor->flags |= ACTOR_FLAG_HOOKSHOT_ATTACHED;
            }
        } else {
            Beetle_PlaySound(&beetlePos, BEETLE_SFX_HIT);
            shouldReturn = 1;
        }
    }
    beetleCollider.base.atFlags &= ~AT_HIT;
    return shouldReturn;
}

static u8 Beetle_CheckGeometryCollision(PlayState* play) {
    Vec3f hitPoint;
    CollisionPoly* hitPoly = NULL;
    s32 hitDynaId = 0;

    f32 cosP = Math_CosS(beetleRot.x);
    f32 sinP = Math_SinS(beetleRot.x);
    f32 sinY = Math_SinS(beetleRot.y);
    f32 cosY = Math_CosS(beetleRot.y);

    Vec3f prevPos = beetlePos;
    prevPos.x -= sinY * cosP * BEETLE_SPEED;
    prevPos.y += sinP * BEETLE_SPEED;
    prevPos.z -= cosY * cosP * BEETLE_SPEED;

    if (BgCheck_EntityLineTest1(&play->colCtx, &prevPos, &beetlePos, &hitPoint,
            &hitPoly, true, true, true, true, &hitDynaId)) {
        beetlePos = hitPoint;
        Beetle_PlaySound(&beetlePos, BEETLE_SFX_HIT);
        return 1;
    }
    return 0;
}

static void Beetle_UpdateGrabbedActor(void) {
    if (beetleGrabbed == NULL) return;
    if (beetleGrabbed->update == NULL) {
        beetleGrabbed = NULL;
        return;
    }
    Math_Vec3f_Copy(&beetleGrabbed->world.pos, &beetlePos);
}

static void Beetle_DropGrabbedActor(Player* p) {
    if (beetleGrabbed == NULL) return;

    Math_Vec3f_Copy(&beetleGrabbed->world.pos, &p->actor.world.pos);
    if (beetleGrabbed->id == ACTOR_EN_ITEM00) {
        beetleGrabbed->gravity = -0.9f;
        beetleGrabbed->bgCheckFlags &= ~0x03;
    } else if (beetleGrabbed->id == ACTOR_EN_SI) {
        beetleGrabbed->flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
    }
    beetleGrabbed = NULL;
}

static void Beetle_StateAiming(Player* p, PlayState* play, ItemInputState* in) {
    // Animation update is handled by Player_UpperAction_Beetle

    if (beetleFirstPerson) {
        FirstPerson_Update(p, play);
    }

    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
        if (beetleFirstPerson) {
            FirstPerson_Exit(p, play);
            beetleFirstPerson = 0;
        } else {
            FirstPerson_Init(p, play);
            beetleFirstPerson = 1;
        }
        ItemEquip_PlayEquipSFX(play, p);
        return;
    }

    u8 isZTargeting = Player_IsZTargeting(p);
    if (beetleFirstPerson && isZTargeting) {
        FirstPerson_Exit(p, play);
        beetleFirstPerson = 0;
    } else if (!beetleFirstPerson && !isZTargeting) {
        FirstPerson_Init(p, play);
        beetleFirstPerson = 1;
    }

    if (!in->isHeld && !in->isPressed) {
        Beetle_Launch(p, play);
    }
}

static void Beetle_StateFlying(Player* p, PlayState* play) {
    // Animation update is handled by Player_UpperAction_Beetle

    Player_ZeroSpeedXZ(p);
    p->stateFlags1 |= PLAYER_STATE1_BOOMERANG_THROWN;
    p->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;

    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
        Beetle_StartReturn(p, play);
        return;
    }

    u8 hitActor = Beetle_CheckActorCollision(p, play);

    Projectile_UpdateRotationFromStick(&beetleRot.y, &beetleRot.x, play, BEETLE_TURN_SPEED, BEETLE_PITCH_MAX);
    Beetle_Move(BEETLE_SPEED);
    Beetle_UpdateCollider(play, &beetlePos);
    Beetle_UpdateWingAnimation(&beetleWingScale, &beetleWingDir);
    Beetle_UpdateSubCam(play);
    Beetle_UpdateGrabbedActor();

    if (hitActor || Beetle_CheckGeometryCollision(play)) {
        Beetle_StartReturn(p, play);
        return;
    }

    f32 distFromStart = Math_Vec3f_DistXYZ(&beetlePos, &beetleStartPos);
    if (distFromStart > BEETLE_MAX_DISTANCE || DECR(beetleTimer) == 0) {
        Beetle_StartReturn(p, play);
        return;
    }

    Beetle_PlayLoopSound(&p->actor, BEETLE_SFX_FLY);
}

static void Beetle_StateReturning(Player* p, PlayState* play) {
    Vec3f targetPos = p->actor.world.pos;
    targetPos.y += BEETLE_LAUNCH_OFFSET_Y;

    f32 distToLink = Math_Vec3f_DistXYZ(&beetlePos, &targetPos);

    Beetle_UpdateWingAnimation(&beetleWingScale, &beetleWingDir);
    Beetle_UpdateGrabbedActor();

    if (distToLink > BEETLE_CATCH_DISTANCE) {
        f32 dx = targetPos.x - beetlePos.x;
        f32 dy = targetPos.y - beetlePos.y;
        f32 dz = targetPos.z - beetlePos.z;

        if (distToLink > 0.1f) {
            f32 invNorm = BEETLE_RETURN_SPEED / distToLink;
            beetlePos.x += dx * invNorm;
            beetlePos.y += dy * invNorm;
            beetlePos.z += dz * invNorm;
        }

        beetleRot.y = Math_Vec3f_Yaw(&beetlePos, &targetPos);
        beetleRot.x = Math_Vec3f_Pitch(&beetlePos, &targetPos);
        Beetle_PlayLoopSound(&p->actor, BEETLE_SFX_FLY);
    } else {
        Beetle_Catch(p, play);
    }
}

void Handle_Beetle(Player* p, PlayState* play) {
    if (!sBeetleColInitialized) Beetle_InitCollider(play, p);

    ItemInputState in;
    ItemInput_Update(&in, ITEM_BEETLE, p, play);

    if (!in.wasEquipped) {
        if (beetleActive) Beetle_Stop(p, play);
        return;
    }

    if (beetleState != BEETLE_STATE_FLYING && beetleState != BEETLE_STATE_RETURNING) {
        if (ItemInput_IsBlocked(p, play)) {
            if (beetleActive) Beetle_Stop(p, play);
            return;
        }
    }

    if (ItemInput_CheckDamage(p, &sBeetlePrevInvinc)) {
        if (beetleState == BEETLE_STATE_FLYING) {
            Beetle_StartReturn(p, play);
        } else if (beetleActive) {
            Beetle_Stop(p, play);
        }
        return;
    }

    if (!beetleActive) {
        if (in.isPressed) Beetle_Start(p, play);
        return;
    }

    if (beetleState == BEETLE_STATE_AIMING && CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
        Beetle_Stop(p, play);
        return;
    }

    switch (beetleState) {
        case BEETLE_STATE_AIMING:    Beetle_StateAiming(p, play, &in); break;
        case BEETLE_STATE_FLYING:    Beetle_StateFlying(p, play); break;
        case BEETLE_STATE_RETURNING: Beetle_StateReturning(p, play); break;
        default:
            beetleState = BEETLE_STATE_IDLE;
            beetleActive = 0;
            break;
    }
}

s32 Player_UpperAction_Beetle(Player* this, PlayState* play) {
    // Return busy if beetle is active - this makes the upper body use upperSkelAnime
    if (beetleActive) {
        LinkAnimation_Update(play, &this->upperSkelAnime);
        return 1;
    }
    return 0;
}

void Player_InitBeetleIA(PlayState* play, Player* this) {
    Beetle_InitCollider(play, this);
    beetleActive = 0;
    beetleState = BEETLE_STATE_IDLE;
    beetleFirstPerson = 0;
    beetleGrabbed = NULL;
    beetleWingScale = BEETLE_WING_SCALE_MAX;
    beetleWingDir = -1;
    beetleTimer = 0;
    beetleSubCamId = SUBCAM_FREE;
    this->stateFlags1 |= PLAYER_STATE1_ITEM_IN_HAND;
}

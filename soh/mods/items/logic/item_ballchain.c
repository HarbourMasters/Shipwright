/**
 * item_ballchain.c - Ball and Chain from Twilight Princess
 *
 * Controls:
 *   Hold C Button: Spin ball overhead (charging)
 *   Release C:     Throw ball in aimed direction
 *   During throw:  Ball returns automatically after hitting or max range
 *
 * Features:
 *   - Heavy damage to enemies and destructible objects
 *   - Breaks ice walls and armored enemies
 *   - Can activate heavy switches
 *   - Uses skeletal animation for swing poses
 */

#include "z64.h"
#include "item_ballchain.h"
#include "../custom_items.h"
#include "../helpers/camera_helper.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "../anim/ballchain/ballchain_anim_data.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.h"
#include "overlays/actors/ovl_Bg_Jya_Ironobj/z_bg_jya_ironobj.h"

// =============================================================================
// Static Data
// =============================================================================

static ColliderCylinderInit sBallChainColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER | AT_TYPE_OTHER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2, { DMG_JUMP_GIANT, 0x0F, BALLCHAIN_DAMAGE }, { 0, 0, 0 }, TOUCH_ON | TOUCH_SFX_NORMAL, BUMP_NONE, OCELEM_NONE },
    { BALLCHAIN_COL_RADIUS, BALLCHAIN_COL_HEIGHT, 0, { 0, 0, 0 } }
};

static u8 sBallChainColInitialized = 0;
static s8 sBallChainPrevInvinc = 0;
static u8 sBallChainThrownFirstFrame = 0;

// =============================================================================
// Pose Functions
// =============================================================================

static void BallChain_ResetPose(Player* p) {
    p->upperLimbRot.x = 0;
    p->upperLimbRot.y = 0;
    p->upperLimbRot.z = 0;
}

static void BallChain_SetEquipPose(Player* p) {
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].x = BC_EQUIP_L_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].y = BC_EQUIP_L_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].z = BC_EQUIP_L_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].x = BC_EQUIP_L_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].y = BC_EQUIP_L_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].z = BC_EQUIP_L_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].x = BC_EQUIP_L_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].y = BC_EQUIP_L_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].z = BC_EQUIP_L_HAND_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].x = BC_EQUIP_R_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].y = BC_EQUIP_R_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].z = BC_EQUIP_R_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].x = BC_EQUIP_R_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].y = BC_EQUIP_R_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].z = BC_EQUIP_R_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].x = BC_EQUIP_R_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].y = BC_EQUIP_R_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].z = BC_EQUIP_R_HAND_Z;
    p->upperLimbRot.x = 0;
    p->upperLimbRot.y = 0;
    p->upperLimbRot.z = 0;
}

static void BallChain_SetSpinPose(Player* p, f32 stickX, f32 stickY) {
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].x = BC_SPIN_L_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].y = BC_SPIN_L_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].z = BC_SPIN_L_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].x = BC_SPIN_L_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].y = BC_SPIN_L_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].z = BC_SPIN_L_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].x = BC_SPIN_L_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].y = BC_SPIN_L_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].z = BC_SPIN_L_HAND_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].x = BC_SPIN_R_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].y = BC_SPIN_R_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].z = BC_SPIN_R_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].x = BC_SPIN_R_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].y = BC_SPIN_R_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].z = BC_SPIN_R_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].x = BC_SPIN_R_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].y = BC_SPIN_R_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].z = BC_SPIN_R_HAND_Z;
    p->upperLimbRot.x = (s16)(-stickY * BALLCHAIN_LEAN_MULT);
    p->upperLimbRot.y = 0;
    p->upperLimbRot.z = (s16)(stickX * BALLCHAIN_LEAN_MULT);
}

// =============================================================================
// Collider Functions
// =============================================================================

static void BallChain_InitCollider(PlayState* play, Player* p) {
    if (sBallChainColInitialized) return;
    Collider_InitCylinder(play, &bcCollider);
    Collider_SetCylinder(play, &bcCollider, &p->actor, &sBallChainColInit);
    sBallChainColInitialized = 1;
}

static void BallChain_UpdateCollider(PlayState* play, Vec3f* pos) {
    bcCollider.dim.pos.x = (s16)pos->x;
    bcCollider.dim.pos.y = (s16)(pos->y - (BALLCHAIN_COL_HEIGHT / 2));
    bcCollider.dim.pos.z = (s16)pos->z;
    bcCollider.base.atFlags |= AT_ON | AT_TYPE_PLAYER | AT_TYPE_OTHER;
    CollisionCheck_SetAT(play, &play->colChkCtx, &bcCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &bcCollider.base);
}

// =============================================================================
// Hit Detection
// =============================================================================

static void BallChain_CheckHit(Vec3f* pos) {
    if (bcCollider.base.atFlags & AT_HIT) {
        Audio_PlaySoundGeneral(BALLCHAIN_SFX_HIT, pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        bcCollider.base.atFlags &= ~AT_HIT;
    }
}

static void BallChain_CheckDestructibles(PlayState* play, Vec3f* ballPos) {
    Actor* actor;
    Actor* next;
    f32 dist;
    f32 checkRadius = BALLCHAIN_COL_RADIUS + 40.0f;

    for (actor = play->actorCtx.actorLists[ACTORCAT_BG].head; actor != NULL; actor = next) {
        next = actor->next;

        if (actor->id == ACTOR_BG_ICE_SHELTER) {
            dist = Math_Vec3f_DistXYZ(ballPos, &actor->world.pos);
            if (dist < checkRadius) {
                BgIceShelter_BreakInstantly(actor, play);
            }
        }
    }

    // Iron objects are in ACTORCAT_PROP
    for (actor = play->actorCtx.actorLists[ACTORCAT_PROP].head; actor != NULL; actor = next) {
        next = actor->next;

        if (actor->id == ACTOR_BG_JYA_IRONOBJ) {
            dist = Math_Vec3f_DistXYZ(ballPos, &actor->world.pos);
            if (dist < checkRadius) {
                BgJyaIronobj_DestroyInstantly(actor, play);
            }
        }
    }
}

static void BallChain_ApplyDamageBonus(PlayState* play) {
    Actor* hit;

    if (!(bcCollider.base.atFlags & AT_HIT)) return;

    hit = bcCollider.base.at;
    if (hit == NULL || hit->update == NULL) return;

    if (hit->id == ACTOR_EN_ST || hit->id == ACTOR_EN_FZ) {
        if (hit->colChkInfo.health > 0) {
            hit->colChkInfo.health -= BALLCHAIN_DAMAGE;
            if (hit->colChkInfo.health < 0) {
                hit->colChkInfo.health = 0;
            }
        }
    }
}

// =============================================================================
// Core Functions
// =============================================================================

static void BallChain_ApplySpeedPenalty(Player* p) {
    p->actor.speedXZ *= BALLCHAIN_SPEED_MULT;
    p->linearVelocity *= BALLCHAIN_SPEED_MULT;
}

static void BallChain_Stop(Player* p, PlayState* play) {
    if (bcFirstPerson) {
        FirstPerson_Exit(p, play);
        bcFirstPerson = 0;
    }
    bcCollider.base.atFlags &= ~(AT_ON | AT_HIT);
    bcActive = 0;
    bcState = BALLCHAIN_STATE_INACTIVE;
    bcCharge = 0;
    bcSpinAngle = 0;
    sBallChainThrownFirstFrame = 0;
    BallChain_ResetPose(p);
    ItemEquip_PlayUnequipSFX(play, p);
}

static void BallChain_Start(Player* p, PlayState* play) {
    if (bcActive) return;
    bcActive = 1;
    bcCharge = 0;
    bcSpinAngle = 0;
    bcFirstPerson = 0;
    bcState = BALLCHAIN_STATE_EQUIP;
    ItemEquip_PlayEquipSFX(play, p);
}

// =============================================================================
// State: Equip
// =============================================================================

static void StateEquip(Player* p, PlayState* play, ItemInputState* in) {
    s16 yaw = p->actor.shape.rot.y;
    Vec3f* leftHand = &p->bodyPartsPos[PLAYER_BODYPART_L_HAND];
    Vec3f* rightHand = &p->bodyPartsPos[PLAYER_BODYPART_R_HAND];

    BallChain_ApplySpeedPenalty(p);
    p->skelAnime.playSpeed = 0.0f;
    BallChain_SetEquipPose(p);

    bcBallPos.x = (leftHand->x + rightHand->x) * 0.5f;
    bcBallPos.y = (leftHand->y + rightHand->y) * 0.5f + BALLCHAIN_EQUIP_Y_OFFSET;
    bcBallPos.z = (leftHand->z + rightHand->z) * 0.5f;

    if (in->isPressed) {
        bcState = BALLCHAIN_STATE_SPINNING;
        bcCharge = 0;
        bcThrowYaw = yaw;
        Audio_PlaySoundGeneral(BALLCHAIN_SFX_SWING, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

// =============================================================================
// State: Spinning
// =============================================================================

static void StateSpinning(Player* p, PlayState* play, ItemInputState* in) {
    s8 rawStickX = play->state.input[0].cur.stick_x;
    s8 rawStickY = play->state.input[0].cur.stick_y;
    u8 isZTarget = Player_IsZTargeting(p);
    f32 stickX = 0.0f;
    f32 stickY = 0.0f;
    f32 stickMag, chargeRatio, spinHeight;
    f32 orbitX, orbitZ, orbitY, heightMod, sideMod;
    s16 spinSpeed, yaw;

    if (isZTarget) {
        BallChain_ApplySpeedPenalty(p);
        p->skelAnime.playSpeed = 0.5f;
    } else {
        p->actor.speedXZ = 0.0f;
        p->linearVelocity = 0.0f;
        p->skelAnime.playSpeed = 0.0f;
    }

    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
        if (bcFirstPerson) {
            FirstPerson_Exit(p, play);
            bcFirstPerson = 0;
        } else {
            FirstPerson_Init(p, play);
            bcFirstPerson = 1;
        }
    }

    if (bcFirstPerson) {
        FirstPerson_Update(p, play);
    }

    if (isZTarget && p->focusActor != NULL) {
        bcThrowYaw = Math_Vec3f_Yaw(&p->actor.world.pos, &p->focusActor->focus.pos);
    }
    p->actor.shape.rot.y = bcThrowYaw;
    p->actor.world.rot.y = bcThrowYaw;
    p->yaw = bcThrowYaw;

    stickMag = sqrtf(SQ(rawStickX) + SQ(rawStickY));
    if (!isZTarget && stickMag > BALLCHAIN_STICK_DEADZONE) {
        stickX = (f32)rawStickX / 127.0f;
        stickY = (f32)rawStickY / 127.0f;
    }

    if (bcCharge < BALLCHAIN_CHARGE_MAX) {
        bcCharge++;
    }

    chargeRatio = (f32)bcCharge / (f32)BALLCHAIN_CHARGE_MAX;
    spinSpeed = (s16)(BALLCHAIN_SPIN_SPEED_MIN + (BALLCHAIN_SPIN_SPEED_MAX - BALLCHAIN_SPIN_SPEED_MIN) * chargeRatio);
    bcSpinAngle += spinSpeed;
    spinHeight = BALLCHAIN_SPIN_HEIGHT_MIN + (BALLCHAIN_SPIN_HEIGHT_MAX - BALLCHAIN_SPIN_HEIGHT_MIN) * chargeRatio;

    orbitX = Math_SinS(bcSpinAngle) * BALLCHAIN_SPIN_RADIUS;
    orbitZ = Math_CosS(bcSpinAngle) * BALLCHAIN_SPIN_RADIUS;
    orbitY = spinHeight;

    heightMod = -Math_CosS(bcSpinAngle) * stickY * BALLCHAIN_LEAN_TILT;
    sideMod = -Math_SinS(bcSpinAngle) * stickX * BALLCHAIN_LEAN_TILT;
    orbitY += heightMod + sideMod;

    yaw = p->actor.shape.rot.y;
    bcBallPos.x = p->actor.world.pos.x + (orbitX * Math_CosS(yaw) + orbitZ * Math_SinS(yaw));
    bcBallPos.y = p->actor.world.pos.y + orbitY;
    bcBallPos.z = p->actor.world.pos.z + (-orbitX * Math_SinS(yaw) + orbitZ * Math_CosS(yaw));

    BallChain_SetSpinPose(p, stickX, stickY);

    BallChain_UpdateCollider(play, &bcBallPos);
    BallChain_CheckDestructibles(play, &bcBallPos);
    BallChain_CheckHit(&bcBallPos);
    BallChain_ApplyDamageBonus(play);

    func_8002F974(&p->actor, BALLCHAIN_SFX_WHOOSH);

    if (!in->isHeld) {
        f32 throwDistMax;
        s16 throwYaw;

        bcState = BALLCHAIN_STATE_THROWN;
        throwDistMax = BALLCHAIN_THROW_DIST_MIN + (BALLCHAIN_THROW_DIST_MAX - BALLCHAIN_THROW_DIST_MIN) * chargeRatio;
        bcThrowDist = (s32)throwDistMax;

        if (bcFirstPerson) {
            throwYaw = FirstPerson_GetAimYaw(p);
            bcThrowPitch = FirstPerson_GetAimPitch(p);
            FirstPerson_Exit(p, play);
            bcFirstPerson = 0;
        } else if (isZTarget && p->focusActor != NULL) {
            throwYaw = Math_Vec3f_Yaw(&p->actor.world.pos, &p->focusActor->focus.pos);
            bcThrowPitch = 0;
        } else {
            throwYaw = p->actor.shape.rot.y + (s16)(stickX * BALLCHAIN_THROW_YAW_MAX);
            bcThrowPitch = (s16)(-stickY * BALLCHAIN_THROW_PITCH_MAX);
        }

        bcThrowYaw = throwYaw;
        sBallChainThrownFirstFrame = 1;

        Audio_PlaySoundGeneral(
            LINK_IS_ADULT ? BALLCHAIN_SFX_VOICE_ADULT : BALLCHAIN_SFX_VOICE_CHILD,
            &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        Audio_PlaySoundGeneral(BALLCHAIN_SFX_SWING, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

// =============================================================================
// State: Thrown
// =============================================================================

static void StateThrown(Player* p, PlayState* play) {
    Vec3f target;
    f32 dist, dx, dy, dz, norm;
    CollisionPoly* poly = NULL;
    Vec3f prevPos, resultPos;

    p->actor.speedXZ = 0.0f;
    p->linearVelocity = 0.0f;
    p->skelAnime.playSpeed = 0.0f;

    p->actor.shape.rot.y = bcThrowYaw;
    p->actor.world.rot.y = bcThrowYaw;
    p->yaw = bcThrowYaw;

    BallChain_SetSpinPose(p, 0.0f, 0.0f);
    p->upperLimbRot.x = BALLCHAIN_THROW_LEAN;

    if (sBallChainThrownFirstFrame) {
        bcBallPos = p->actor.world.pos;
        bcBallPos.y += 60.0f;
        sBallChainThrownFirstFrame = 0;
    }

    if (bcThrowDist > 0) {
        if (Player_IsZTargeting(p) && p->focusActor != NULL && p->focusActor->update != NULL) {
            target = p->focusActor->focus.pos;
        } else {
            f32 throwDist = (f32)bcThrowDist;
            f32 hDist = throwDist * Math_CosS(bcThrowPitch);
            target.x = p->actor.world.pos.x + Math_SinS(bcThrowYaw) * hDist;
            target.y = p->actor.world.pos.y + 60.0f - Math_SinS(bcThrowPitch) * throwDist;
            target.z = p->actor.world.pos.z + Math_CosS(bcThrowYaw) * hDist;
        }

        dist = Math_Vec3f_DistXYZ(&bcBallPos, &target);

        if (dist > 10.0f) {
            dx = target.x - bcBallPos.x;
            dy = target.y - bcBallPos.y;
            dz = target.z - bcBallPos.z;

            norm = BALLCHAIN_THROW_SPEED / dist;
            prevPos = bcBallPos;

            bcBallPos.x += dx * norm;
            bcBallPos.y += dy * norm;
            bcBallPos.z += dz * norm;

            resultPos = bcBallPos;
            if (BgCheck_EntitySphVsWall1(&play->colCtx, &resultPos, &bcBallPos, &prevPos,
                    BALLCHAIN_WALL_RADIUS, &poly, BALLCHAIN_WALL_HEIGHT)) {
                bcThrowDist = 0;
                Audio_PlaySoundGeneral(BALLCHAIN_SFX_WALL_BOUNCE, &bcBallPos, 4,
                    &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
        } else {
            bcThrowDist = 0;
        }
    } else {
        dist = Math_Vec3f_DistXYZ(&bcBallPos, &p->actor.world.pos);

        if (dist > BALLCHAIN_RETURN_DIST) {
            dx = p->actor.world.pos.x - bcBallPos.x;
            dy = (p->actor.world.pos.y + 60.0f) - bcBallPos.y;
            dz = p->actor.world.pos.z - bcBallPos.z;
            norm = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));

            if (norm > 0.1f) {
                norm = BALLCHAIN_RETRACT_SPEED / norm;
                bcBallPos.x += dx * norm;
                bcBallPos.y += dy * norm;
                bcBallPos.z += dz * norm;
            }
            func_8002F974(&p->actor, BALLCHAIN_SFX_RETRACT);
        } else {
            bcState = BALLCHAIN_STATE_EQUIP;
            return;
        }
    }

    BallChain_UpdateCollider(play, &bcBallPos);
    BallChain_CheckDestructibles(play, &bcBallPos);
    BallChain_CheckHit(&bcBallPos);
    BallChain_ApplyDamageBonus(play);
}

// =============================================================================
// Public API
// =============================================================================

void Handle_BallAndChain(Player* p, PlayState* play) {
    ItemInputState in;

    if (!sBallChainColInitialized) {
        BallChain_InitCollider(play, p);
    }

    ItemInput_Update(&in, ITEM_BALL_AND_CHAIN, p, play);

    if (!in.wasEquipped || ItemInput_IsBlocked(p, play) || ItemInput_CheckDamage(p, &sBallChainPrevInvinc)) {
        if (bcActive) BallChain_Stop(p, play);
        return;
    }
    if (in.otherButtonPressed) {
        BallChain_Stop(p, play);
        return;
    }

    if (!bcActive) {
        if (in.isPressed || in.isHeld) {
            BallChain_Start(p, play);
        }
        return;
    }

    switch (bcState) {
        case BALLCHAIN_STATE_EQUIP:    StateEquip(p, play, &in);    break;
        case BALLCHAIN_STATE_SPINNING: StateSpinning(p, play, &in); break;
        case BALLCHAIN_STATE_THROWN:   StateThrown(p, play);        break;
        default:                       bcState = BALLCHAIN_STATE_EQUIP; break;
    }
}

void Player_InitBallAndChainIA(PlayState* play, Player* p) {
    BallChain_InitCollider(play, p);
    bcActive = 0;
    bcCharge = 0;
    bcSpinAngle = 0;
    bcFirstPerson = 0;
    bcState = BALLCHAIN_STATE_INACTIVE;
    bcThrowDist = 0;
    sBallChainThrownFirstFrame = 0;
}

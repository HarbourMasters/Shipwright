#include "soh/Enhancements/mm/forms/FormsCommon.h"

#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "soh/Enhancements/mm/MMPlayerAnims.h"
}

// Goron form: roll, spike roll and ground pound, adapted from MM's Player_Action_96 and its
// helpers (2Ship2Harkinian mm/src/overlays/actors/ovl_player_actor/z_player.c:19748-20260).
//
// The state machine is ported onto OOT's action-function system: a vanilla roll
// (Player_Action_Roll) started while transformed is converted into GoronForm_RollAction
// before it runs its first frame. MM Player fields that don't exist on OOT's Player struct
// live in sRoll below; MM's speedXZ maps to OOT's linearVelocity. MM-only engine features
// were substituted: the spike drain uses repeated MAGIC_CONSUME_NOW requests instead of MM's
// MAGIC_CONSUME_GORON_ZORA drain state, damage is delivered through a dedicated AT cylinder
// flagged as Megaton Hammer damage (romhack precedent; also what makes Gossip Stones and
// hammer switches react), and Goron sound effects that have no OOT equivalent use the
// closest OOT sfx.

typedef struct GoronRollState {
    bool rolling;       // MM PLAYER_STATE3_1000: curled, ball drawn instead of the skeleton
    s16 spikeCharge;    // MM unk_B86[1]: nonzero while spike-rolling
    s16 spinAngle;      // MM unk_B86[0]: drift-induced pitch wobble
    s16 bounceTimer;    // MM unk_B8C: steering lock after a wall bounce
    s16 steerLockTimer; // MM unk_B8E
    s16 airTimer;       // MM unk_B8A: hover frames granted for the ground pound
    f32 rollSpeed;      // MM unk_B08: conserved momentum, decoupled from linearVelocity
    f32 lean;           // MM unk_B0C: turn-induced traction loss
    f32 squash;         // MM unk_ABC: ball squash/stretch factor
    f32 squashVel;      // MM unk_B48
    f32 chargeTint;     // ball tint toward red while charging/spiked
    s16 magicTimer;     // spike drain cadence
} GoronRollState;

// av1.actionVar1 during GoronForm_RollAction (MM meanings):
// 0 = curling up, 1 = airborne after a ball jump, 2 = ground pound landing,
// 3 = bonk pop-up (uncurls when falling), 4 = rolling, 5..0x36 = spike charge counter.
// av2.actionVar2 = ball spin rate (binang per frame), also the spike charge readiness gauge.

static GoronRollState sRoll;

static ColliderCylinder sAttackCollider;
static Actor* sAttackColliderActor = nullptr;

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

void GoronForm_RollAction(Player* player, PlayState* play);
void GoronForm_UncurlAction(Player* player, PlayState* play);

extern "C" bool GoronForm_IsRolling(void) {
    return sRoll.rolling;
}

extern "C" bool GoronForm_IsSpiked(void) {
    return sRoll.spikeCharge != 0;
}

extern "C" f32 GoronForm_GetChargeTint(void) {
    return sRoll.chargeTint;
}

extern "C" f32 GoronForm_GetSquash(void) {
    return sRoll.squash;
}

// Player-owned AT collider used for the roll body, spikes, ground pound and punches.
// (OOT's player only has AT quads for held weapons; MM used Player_SetCylinderForAttack.)
static void SetPlayerAttackCylinder(PlayState* play, Player* player, Vec3f* pos, s16 radius, s16 height, u32 dmgFlags,
                                    u8 damage) {
    static ColliderCylinderInit sCylinderInit = {
        {
            COLTYPE_NONE,
            AT_ON | AT_TYPE_PLAYER,
            AC_NONE,
            OC1_NONE,
            OC2_NONE,
            COLSHAPE_CYLINDER,
        },
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x01 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 25, 60, 0, { 0, 0, 0 } },
    };

    // The player actor is recreated on scene load, so rebind the collider when it changes
    if (sAttackColliderActor != &player->actor) {
        Collider_InitCylinder(play, &sAttackCollider);
        Collider_SetCylinder(play, &sAttackCollider, &player->actor, &sCylinderInit);
        sAttackColliderActor = &player->actor;
    }

    sAttackCollider.info.toucher.dmgFlags = dmgFlags;
    sAttackCollider.info.toucher.damage = damage;
    sAttackCollider.dim.radius = radius;
    sAttackCollider.dim.height = height;
    sAttackCollider.dim.pos.x = (s16)pos->x;
    sAttackCollider.dim.pos.y = (s16)pos->y;
    sAttackCollider.dim.pos.z = (s16)pos->z;
    CollisionCheck_SetAT(play, &play->colChkCtx, &sAttackCollider.base);
}

static bool IsGoronRollActionFunc(PlayerActionFunc func) {
    return func == GoronForm_RollAction || func == GoronForm_UncurlAction;
}

static void CleanupRoll(PlayState* play, Player* player) {
    if (sRoll.spikeCharge != 0) {
        Magic_Reset(play);
    }
    sRoll = {};
    player->actor.shape.rot.x = 0;
    player->actor.shape.rot.z = 0;
}

static void GoronForm_SetupRoll(PlayState* play, Player* player) {
    Player_SetupAction(play, player, GoronForm_RollAction, 0);
    sRoll = {};
    Player_AnimPlayOnceAdjusted(play, player, (LinkAnimationHeader*)gPlayerAnim_pg_maru_change);
    Player_PlaySfx(&player->actor, NA_SE_PL_ROLL); // MM NA_SE_PL_GORON_TO_BALL
}

// func_80857A44: rolling parameters take over once the curl-up animation finishes
static bool CurlFinished(PlayState* play, Player* player) {
    if (LinkAnimation_Update(play, &player->skelAnime)) {
        sRoll.rolling = true;
        player->av1.actionVar1 = 4;
        player->av2.actionVar2 = player->linearVelocity * 500.0f;
        player->actor.shape.shadowScale = 30.0f;
        sRoll.rollSpeed = player->linearVelocity;
        sRoll.lean = 0.0f;
        player->actor.home.rot.y = player->yaw;
        return true;
    }
    return false;
}

// func_80857950: uncurl when A is released (or after a bonk pop-up starts falling)
static bool TryUncurl(PlayState* play, Player* player) {
    Input* input = &play->state.input[0];

    if (((sRoll.spikeCharge == 0) && !CHECK_BTN_ALL(input->cur.button, BTN_A)) ||
        ((player->av1.actionVar1 == 3) && (player->actor.velocity.y < 0.0f))) {
        Player_SetupAction(play, player, GoronForm_UncurlAction, 1);
        Math_Vec3f_Copy(&player->actor.world.pos, &player->actor.prevPos);
        LinkAnimation_Change(play, &player->skelAnime, (LinkAnimationHeader*)gPlayerAnim_pg_maru_change,
                             -PLAYER_ANIM_ADJUSTED_SPEED, 7.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
        Player_PlaySfx(&player->actor, NA_SE_PL_ROLL); // MM NA_SE_PL_BALL_TO_GORON
        CleanupRoll(play, player);
        return true;
    }
    return false;
}

// func_808577E0: ball squash/stretch spring driven by the spin rate
static void UpdateSquash(Player* player) {
    f32 target = ABS(player->av2.actionVar2) * 0.00004f;

    sRoll.squashVel += (sRoll.squash < target) ? 0.08f : -0.07f;
    sRoll.squashVel = CLAMP(sRoll.squashVel, -0.2f, 0.14f);

    if (fabsf(sRoll.squashVel) < 0.12f) {
        sRoll.squash += sRoll.squashVel;
        if ((sRoll.squashVel >= 0.0f) ? (sRoll.squash >= target) : (sRoll.squash <= target)) {
            sRoll.squash = target;
            sRoll.squashVel = 0.0f;
        }
    } else {
        sRoll.squash += sRoll.squashVel;
        sRoll.squash = CLAMP(sRoll.squash, -0.7f, 0.3f);
    }
}

// func_80857AEC: landing while curled; a landing from a ball jump becomes the ground pound
static void HandleRollLanding(PlayState* play, Player* player) {
    if (player->actor.bgCheckFlags & 2) { // just touched the ground
        sRoll.lean += sRoll.rollSpeed * 0.05f;

        if (sRoll.spikeCharge == 0) {
            if (player->av1.actionVar1 == 1) {
                player->av1.actionVar1 = 2;
                Player_RequestQuake(play, 32967, 2, 30);
                Player_RequestRumble(player, 255, 20, 150, 0);
                Player_PlaySfx(&player->actor, NA_SE_IT_HAMMER_HIT); // MM NA_SE_PL_GORON_PUNCH
                EffectSsBlast_SpawnWhiteShockwave(play, &player->actor.world.pos, &sZeroVec, &sZeroVec);
                player->av2.actionVar2 = 0;
                sRoll.rollSpeed = 0.0f;
            } else {
                player->av1.actionVar1 = 4;
            }
        }
        Player_PlayLandingSfx(player);
    }
}

// func_8083A4A4 (MM): holding the stick against the roll direction decelerates instead of turning
static bool ReverseStickDecel(Player* player, f32* speedTarget, s16* yawTarget, f32 decelerationRate) {
    s16 yawDiff = player->yaw - *yawTarget;

    if (ABS(yawDiff) > 0x6000) {
        if (Math_StepToF(&player->linearVelocity, 0.0f, decelerationRate)) {
            *speedTarget = 0.0f;
            *yawTarget = player->yaw;
        } else {
            return true;
        }
    }
    return false;
}

// Core grounded momentum model from Player_Action_96: the ball's conserved momentum
// (rollSpeed along home.rot.y) is steered by the stick and pulled by the floor slope.
// MM's per-material traction variants and the ground-sampled z-tilt were dropped.
static void RollGroundPhysics(PlayState* play, Player* player, f32 speedTarget, s16 yawTarget, s32* spinTarget) {
    s16 sp90 = player->yaw;
    s16 turnDiff = player->yaw - player->actor.home.rot.y;
    f32 cosTurn = Math_CosS(turnDiff);
    f32 spBC;

    if (sRoll.spikeCharge == 0) {
        sRoll.lean = 0.0f;
        if (player->av1.actionVar1 >= 0x36) {
            // Spike roll engages: MM consumes 2 magic and drains continuously afterwards
            Magic_RequestChange(play, 2, MAGIC_CONSUME_NOW);
            sRoll.rollSpeed = 18.0f;
            sRoll.spikeCharge = 1;
            sRoll.magicTimer = 0;
            Player_PlaySfx(&player->actor, NA_SE_IT_SWORD_CHARGE); // MM NA_SE_PL_GORON_BALL_CHARGE_DASH
        }
    } else {
        sRoll.lean = CLAMP(sRoll.lean, 0.0f, 0.9f);
    }

    spBC = (1.0f - sRoll.lean) * sRoll.rollSpeed * cosTurn;
    if ((spBC < 0.0f) || ((speedTarget == 0.0f) && (ABS(turnDiff) > 0xFA0))) {
        spBC = 0.0f;
    }
    Math_StepToF(&sRoll.lean, 0.0f, fabsf(cosTurn) * 20.0f);

    s32 momentumSpin = CLAMP_MIN((s32)(spBC * 500.0f), 0);
    *spinTarget = CLAMP_MIN(*spinTarget, momentumSpin);

    f32 momentumX = spBC * Math_SinS(player->actor.home.rot.y);
    f32 momentumZ = spBC * Math_CosS(player->actor.home.rot.y);
    f32 velX = sRoll.rollSpeed * Math_SinS(player->yaw);
    f32 velZ = sRoll.rollSpeed * Math_CosS(player->yaw);
    f32 driftX = velX - momentumX;
    f32 driftZ = velZ - momentumZ;

    player->linearVelocity = spBC;
    player->yaw = player->actor.home.rot.y;

    f32 steerSpeed = speedTarget;
    s16 steerYaw = yawTarget;

    if (ReverseStickDecel(player, &steerSpeed, &steerYaw, (player->av1.actionVar1 >= 5) ? 0.0f : 1.0f)) {
        if (sRoll.spikeCharge == 0) {
            player->av1.actionVar1 = 4;
        }
        if (player->av1.actionVar1 == 4) {
            *spinTarget = -0xFA0;
        }
    } else {
        f32 accel = CLAMP_MIN(player->av2.actionVar2 * 0.0003f, 0.0f);
        f32 decel = CLAMP_MIN((Math_SinS(player->floorPitch) * 8.0f) + 0.6f, 0.0f);

        if (sRoll.spikeCharge == 0) {
            // Spike charge builds while spinning fast with magic available
            if ((gSaveContext.magicState == MAGIC_STATE_IDLE) && (gSaveContext.magic >= 2) &&
                (player->av2.actionVar2 >= 0x36B0)) {
                if (player->av1.actionVar1 < 0x36) {
                    player->av1.actionVar1++;
                }
                if (player->av1.actionVar1 == 5) {
                    Player_PlaySfx(&player->actor, NA_SE_IT_SWORD_CHARGE); // MM NA_SE_PL_GORON_BALL_CHARGE
                }
            } else {
                player->av1.actionVar1 = 4;
            }
        }

        if (speedTarget != steerSpeed) {
            player->yaw = steerYaw;
        }

        Math_AsymStepToF(&player->linearVelocity, speedTarget, accel, decel);

        s16 turnRate = (s16)(fabsf(player->actor.speedXZ) * 20.0f) + 300;
        turnRate = CLAMP_MIN(turnRate, 100);

        s16 steerLean = (s32)((s16)(yawTarget - player->yaw) * -0.5f);
        sRoll.lean += SQ((f32)steerLean) * 8e-9f;
        Math_ScaledStepToS(&player->yaw, yawTarget, turnRate);
        Math_ScaledStepToS(&player->actor.shape.rot.z, steerLean, turnRate);
    }

    // Recombine steering with the drift component and the slope pull
    spBC = player->linearVelocity;
    player->actor.home.rot.y = player->yaw;
    player->yaw = sp90;

    Vec3f slopeNormal = { 0.0f, 1.0f, 0.0f };
    if (player->actor.floorPoly != NULL) {
        slopeNormal.x = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.x);
        slopeNormal.y = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.y);
        slopeNormal.z = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.z);
    }

    f32 driftMag = sqrtf(SQ(driftX) + SQ(driftZ));
    if (sRoll.spikeCharge != 0) {
        if ((ABS(turnDiff) + ABS(player->floorPitch)) > 0x3A98) {
            // Spike lost on a sharp turn or steep slope
            sRoll.spikeCharge = 0;
            player->av1.actionVar1 = 4;
            sRoll.steerLockTimer = 20;
            player->av2.actionVar2 = 0;
            Magic_Reset(play);
        }
    } else {
        f32 pulledX = (0.6f * slopeNormal.x) + driftX;
        f32 pulledZ = (0.6f * slopeNormal.z) + driftZ;
        f32 pulledMag = sqrtf(SQ(pulledX) + SQ(pulledZ));

        if ((pulledMag < driftMag) || (pulledMag < 6.0f)) {
            driftX = pulledX;
            driftZ = pulledZ;
            driftMag = pulledMag;
        }
    }

    if (driftMag != 0.0f) {
        f32 reduced = CLAMP_MIN(driftMag - 0.3f, 0.0f);
        f32 scale = reduced / driftMag;

        driftX *= scale;
        driftZ *= scale;

        if ((reduced != 0.0f) && (player->av2.actionVar2 == 0)) {
            sRoll.spinAngle += (s16)(reduced * 800.0f);
        }
    }

    velX = (Math_SinS(player->actor.home.rot.y) * spBC) + driftX;
    velZ = (Math_CosS(player->actor.home.rot.y) * spBC) + driftZ;

    sRoll.rollSpeed = sqrtf(SQ(velX) + SQ(velZ));
    sRoll.rollSpeed = CLAMP_MAX(sRoll.rollSpeed, 18.0f);
    player->yaw = Math_Atan2S(velZ, velX);
}

void GoronForm_RollAction(Player* player, PlayState* play) {
    Input* input = &play->state.input[0];
    f32 speedTarget = 0.0f;
    s16 yawTarget = player->yaw;
    s32 spinTarget;

    // Without this, going airborne (rolling off a ledge, the ball jump, the bonk pop-up)
    // makes func_8083AA10 stomp the action into vanilla freefall, uncurling the ball
    player->stateFlags3 |= PLAYER_STATE3_MIDAIR;

    if ((player->av1.actionVar1 == 0) && !CurlFinished(play, player)) {
        return;
    }

    player->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    player->actor.shape.shadowScale = 30.0f;
    UpdateSquash(player);

    if (TryUncurl(play, player)) {
        return;
    }

    // --- wall interactions ---
    if ((player->actor.bgCheckFlags & 8) && (sRoll.rollSpeed >= 12.0f)) {
        s16 facingWall = player->yaw - (s16)(player->actor.wallYaw + 0x8000);

        if (ABS(facingWall) < 0x1C00) {
            // Head-on bonk: kill the momentum and pop up into the air
            player->linearVelocity *= 0.1f;
            sRoll.rollSpeed *= 0.1f;
            player->actor.velocity.y = 10.0f;
            player->actor.bgCheckFlags &= ~1;
            if (sRoll.spikeCharge != 0) {
                sRoll.spikeCharge = 0;
                player->av1.actionVar1 = 3;
                Magic_Reset(play);
            }
            Player_RequestQuake(play, 33267, 3, 12);
            Player_RequestRumble(player, 255, 20, 150, 0);
            Player_PlaySfx(&player->actor, NA_SE_PL_BODY_HIT);
        } else {
            // Shallow angle: reflect the roll direction off the wall (MM Action_96)
            s32 mag = ABS(facingWall);
            s16 quantized = ((facingWall >= 0) ? 1 : -1) * ((mag + 0x100) & ~0x1FF);

            sRoll.lean += sRoll.rollSpeed * 0.05f;
            player->yaw += (s16)(0x8000 - (s16)(quantized * 2));
            player->actor.home.rot.y = player->yaw;
            player->actor.shape.rot.y = player->yaw;
            sRoll.bounceTimer = 4;
            Player_PlaySfx(&player->actor, NA_SE_PL_BOUND); // MM NA_SE_IT_GORON_ROLLING_REFLECTION
        }
    }

    // --- steering input ---
    if (sRoll.steerLockTimer != 0) {
        sRoll.steerLockTimer--;
    } else {
        Player_GetMovementSpeedAndYaw(player, &speedTarget, &yawTarget, FORMS_SPEED_MODE_LINEAR, play);
        speedTarget *= 2.6f;
    }

    if (sRoll.bounceTimer != 0) {
        sRoll.bounceTimer--;
        yawTarget = player->yaw;
    }

    // --- spike roll upkeep ---
    if (sRoll.spikeCharge != 0) {
        speedTarget = 18.0f;
        if (player->av1.actionVar1 < 4) {
            player->av1.actionVar1++;
        }

        bool dropSpike = !CHECK_BTN_ALL(input->cur.button, BTN_A) || (gSaveContext.magic == 0) ||
                         ((player->av1.actionVar1 == 4) && (sRoll.rollSpeed < 12.0f));

        if (dropSpike) {
            sRoll.spikeCharge--;
            if (sRoll.spikeCharge <= 0) {
                sRoll.spikeCharge = 0;
                Magic_Reset(play);
                Player_PlaySfx(&player->actor, NA_SE_PL_BOUND); // MM NA_SE_PL_GORON_BALL_CHARGE_FAILED
            }
            player->av1.actionVar1 = 4;
        } else {
            if (sRoll.spikeCharge < 7) {
                sRoll.spikeCharge++;
            }
            // Continuous drain substitute for MM's MAGIC_CONSUME_GORON_ZORA state
            if (++sRoll.magicTimer >= 12) {
                sRoll.magicTimer = 0;
                Magic_RequestChange(play, 1, MAGIC_CONSUME_NOW);
            }
        }
        sRoll.chargeTint = 1.0f;
    } else {
        sRoll.chargeTint = (player->av1.actionVar1 >= 5) ? ((player->av1.actionVar1 - 4) / 50.0f) : 0.0f;
    }

    spinTarget = speedTarget * 900.0f;

    if (player->actor.bgCheckFlags & 1) { // grounded
        player->actor.gravity = -1.2f;
        HandleRollLanding(play, player);

        if (player->av1.actionVar1 == 2) {
            // Ground pound aftermath
            if (sRoll.airTimer == 0) {
                player->av1.actionVar1 = 4;
            } else {
                sRoll.airTimer--;
                sRoll.squash = 0.0f;
                sRoll.squashVel = 0.14f;
            }
        } else if ((sRoll.spikeCharge == 0) && CHECK_BTN_ALL(input->press.button, BTN_B)) {
            // Ball jump (func_80857640)
            player->actor.velocity.y = 14.0f;
            player->actor.bgCheckFlags &= ~1;
            player->linearVelocity = 0.0f;
            if (player->av2.actionVar2 < 0x1F40) {
                player->av2.actionVar2 = 0x1F40;
            }
            player->av1.actionVar1 = 1;
            sRoll.squashVel = 1.0f;
            Player_PlayVoiceSfx(player, NA_SE_VO_LI_SWORD_N);
            Player_PlaySfx(&player->actor, NA_SE_PL_SKIP); // MM NA_SE_PL_GORON_BALLJUMP
        } else {
            RollGroundPhysics(play, player, speedTarget, yawTarget, &spinTarget);
        }

        // Slip dust when the ball's spin disagrees with the actual velocity (func_808576BC)
        s32 slip = (s32)(((player->actor.velocity.z * Math_CosS(player->yaw)) +
                          (player->actor.velocity.x * Math_SinS(player->yaw))) *
                         800.0f) -
                   player->av2.actionVar2;
        if (ABS(slip) > 0x7D0) {
            func_8084269C(play, player);
        }

        if (player->actor.bgCheckFlags & 1) {
            player->linearVelocity = sRoll.rollSpeed * Math_CosS(player->floorPitch);
            player->actor.velocity.y = sRoll.rollSpeed * Math_SinS(player->floorPitch);
        }
    } else { // airborne
        Math_ScaledStepToS(&player->actor.shape.rot.z, 0, 0x190);
        sRoll.spinAngle = 0;

        if (sRoll.spikeCharge != 0) {
            f32 v;

            player->actor.gravity = -1.0f;
            Math_ScaledStepToS(&player->actor.home.rot.y, yawTarget, 0x190);
            v = sqrtf(SQ(player->linearVelocity) + SQ(player->actor.velocity.y)) *
                ((player->linearVelocity >= 0.0f) ? 1.0f : -1.0f);
            sRoll.rollSpeed = CLAMP_MAX(v, 18.0f);
        } else {
            sRoll.squashVel += player->actor.velocity.y * 0.005f;
            player->actor.gravity = -1.2f;

            if (player->av1.actionVar1 == 1) {
                if (player->actor.velocity.y > 0.0f) {
                    if ((player->actor.velocity.y + player->actor.gravity) < 0.0f) {
                        player->actor.velocity.y = -player->actor.gravity;
                    }
                } else {
                    sRoll.airTimer = 10;
                    if (player->actor.velocity.y > -1.0f) {
                        player->actor.gravity = -0.2f; // brief hover at the apex
                    } else {
                        player->actor.gravity = -10.0f; // slam down into the pound
                    }
                }
            }
            sRoll.rollSpeed = player->linearVelocity;
        }
    }

    Math_ScaledStepToS(&player->actor.shape.rot.y, player->actor.home.rot.y, 0x7D0);

    // Spin rate approaches the target asymmetrically (MM Math_AsymStepToS)
    {
        s16 step = (player->av2.actionVar2 < spinTarget) ? ((spinTarget >= 0) ? 0x7D0 : 0x4B0)
                                                         : ((spinTarget >= 0) ? 0x4B0 : 0x3E8);
        Math_ScaledStepToS(&player->av2.actionVar2, spinTarget, step);
    }

    if (player->av2.actionVar2 != 0) {
        s16 prevPitch = player->actor.shape.rot.x;

        player->actor.shape.rot.x += player->av2.actionVar2;
        Math_ScaledStepToS(&sRoll.spinAngle, 0, ABS(player->av2.actionVar2));

        // One roll sfx per ball revolution
        if ((player->actor.bgCheckFlags & 1) && (((s32)(player->av2.actionVar2 + prevPitch) * prevPitch) <= 0)) {
            Player_PlaySfx(&player->actor, NA_SE_PL_ROLL); // MM NA_SE_PL_GORON_ROLL
        }
    }

    // --- attack collider ---
    if (player->av1.actionVar1 == 2) {
        SetPlayerAttackCylinder(play, player, &player->actor.world.pos, 60, 60, DMG_HAMMER_JUMP, 4);
        player->invincibilityTimer = -5;
    } else if (sRoll.spikeCharge != 0) {
        SetPlayerAttackCylinder(play, player, &player->actor.world.pos, 25, 40, DMG_HAMMER_SWING, 2);
    } else if (sRoll.rollSpeed > 2.0f) {
        SetPlayerAttackCylinder(play, player, &player->actor.world.pos, 25, 40, DMG_HAMMER_SWING, 1);
    }
}

void GoronForm_UncurlAction(Player* player, PlayState* play) {
    player->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    // Let the uncurl anim finish even if airborne (A released mid-jump, bonk pop-up);
    // once it ends, func_80839F90's idle hands any remaining fall to vanilla freefall
    player->stateFlags3 |= PLAYER_STATE3_MIDAIR;
    Player_DecelerateToZero(player);

    if (LinkAnimation_Update(play, &player->skelAnime)) {
        func_80839F90(player, play);
    }
}

static void RegisterGoronForm() {
    // Convert a vanilla roll into the Goron roll before its first action frame runs,
    // and clean up if some external action change (damage, cutscene) left the roll
    GameInteractor::Instance->RegisterGameHook<GameInteractor::ShouldActorUpdate>([](void* actorRef, bool*) {
        Actor* actor = (Actor*)actorRef;
        PlayState* play = gPlayState;

        if (play == nullptr || actor != &GET_PLAYER(play)->actor) {
            return;
        }
        Player* player = GET_PLAYER(play);

        if (PlayerForm_GetApplied() != PLAYER_SHIPFORM_GORON) {
            if (sRoll.rolling) {
                CleanupRoll(play, player);
            }
            return;
        }

        if (sRoll.rolling && !IsGoronRollActionFunc(player->actionFunc)) {
            CleanupRoll(play, player);
        }

        if ((player->actionFunc == Player_Action_Roll) && (player->av2.bonked == 0)) {
            GoronForm_SetupRoll(play, player);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGoronForm);
